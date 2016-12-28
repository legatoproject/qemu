/*
 *  Qualcomm chipset 9x15 emulation: msm9x15.c
 *
 *  Copyright (C) Sierra Wireless Inc.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu-common.h"
#include "hw/qdev.h"

#include "hw/sysbus.h"
#include "hw/arm/arm.h"
#include "hw/arm/primecell.h"
#include "hw/arm/msm.h"
#include "hw/devices.h"
#include "hw/block/flash.h"
#include "net/net.h"
#include "sysemu/block-backend.h"
#include "sysemu/device_tree.h"
#include "sysemu/sysemu.h"
#include "sysemu/kvm.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "exec/address-spaces.h"
#include "qemu/bitops.h"
#include "qemu/error-report.h"

//[    0.000000] MSM_SHARED_RAM_BASE v=fa300000 p=00040000
//[    0.000000] iotable_init: v FA002000 -> p 02000000 s 00001000
//[    0.000000] iotable_init: v FA003000 -> p 02002000 s 00001000
//[    0.000000] iotable_init: v FA00B000 -> p 02008000 s 00001000
//[    0.000000] iotable_init: v FA000000 -> p 0200A000 s 00001000
//[    0.000000] iotable_init: v FA017000 -> p 00800000 s 00100000
//[    0.000000] iotable_init: v FA008000 -> p 02009000 s 00001000
//[    0.000000] iotable_init: v FA006000 -> p 02011000 s 00001000
//[    0.000000] iotable_init: v FA004000 -> p 1A400000 s 00001000
//[    0.000000] iotable_init: v FA701000 -> p 02040000 s 00001000
//[    0.000000] iotable_init: v FA010000 -> p 00900000 s 00004000
//[    0.000000] iotable_init: v FA015000 -> p 28000000 s 00001000
//[    0.000000] iotable_init: v FA801000 -> p 00108000 s 00001000
//[    0.000000] iotable_init: v FA802000 -> p 00200000 s 00001000
//[    0.000000] iotable_init: v FA702000 -> p 02010000 s 00001000

// Nand Resource
#define MSM_NAND_PHYS		0x1B400000
#define MSM_NAND_BASE		0x1B400000
// Resource DMA : DMOV_NAND_CHAN=3   (mach-msm/include/mach/dma.h)
// DMOV_NAND_CRCI_CMD    15
// DMOV_NAND_CRCI_DATA   3
// ADM_0_SCSS_1_IRQ 171
// IOMEM .start=0x18320000, .end=0x18320000 + SZ_1M - 1

// Resource MEM : .start= MSM_NAND_PHYS, .end= MSM_NAND_PHYS+0x7FF

// 0-15 STI/STG | 16-31 PPI | 32+ SPI
// GIC PPI(private peripheral interrupts)
// GIC SPI (shared peripheral interrupts)
#define GIC_PPI_START 16
#define GIC_SPI_START 32

// 32K GPT and DGT Timers Registers offsets
#define TIMER_MATCH_VAL         0x0000
#define TIMER_COUNT_VAL         0x0004
#define TIMER_ENABLE            0x0008
#define TIMER_CLEAR             0x000C
#define DGT_CLK_CTL             0x0010	//0x34 -0x24 (DGT Base register is 0x24)
#define TIMER_STATUS            0x0088

/* Number of external interrupt lines to configure the GIC with */
#define NUM_IRQS 640

typedef struct MemMapEntry
{
    hwaddr base;
    uint32_t size;
} MemMapEntry;

typedef struct MSM_BoardInfo
{
    struct arm_boot_info bootinfo;
    const char *cpu_model;
    const MemMapEntry *memmap;
    const int *irqmap;
    int	smp_cpus;
    uint32_t clock_phandle;
    DeviceState *nand;
} MSM_BoardInfo;

qemu_irq pic[NUM_IRQS+GIC_SPI_START];

enum
{
    vMEM_NAND,
    vMEM_RAM,
    vGIC_DIST,
    vGIC_CPU,
    vSMD,
    vSMSM,
    vUART0,
    vUART1,
    vGP_TIMER,
    vDG_TIMER,
    vDMOV,
    vTLMM,
    vSMC91x
};

MemoryRegion tlmm;

static const MemMapEntry a8memmap[] =
{
    /* GIC distributor and CPU interfaces sit inside the CPU peripheral space */
    [vGIC_DIST] =   { 0x02000000, 0x00001000 },	// see msm_iomap-9615.h
    [vGIC_CPU] =    { 0x02002000, 0x00001000 },	// see msm_iomap-9615.h
    [vMEM_RAM] =    { 0x40800000, 0x0F800000 },	// See device-9615.c (256M bits)
    [vSMD] =        { 0x02011000, 0x00001000 },	// See device-9615.c (256M bits)
    [vSMSM] =       { 0x40000000, 0x00100000 },	// See device-9615.c (256M bits)
    [vUART0] = 	    { 0x16440000, 0x00001000 },	// see msm_iomap.h // GSBI5- HSL1
    [vUART1] = 	    { 0x16340000, 0x00001000 },	// see device-9615.c // GSBI4 - HSL0
    [vGP_TIMER] =   { 0x0200A004, 0x20       },
    [vDG_TIMER] =   { 0x0200A024, 0x20       },
    [vTLMM]     =   { 0x00800000, 0x00100000 }
};

static const int a8irqmap[] =
{
    [vUART0] 	= 154,	// see irqs-9615.h ... GSBI5_UARTDM_IRQ () - HSL1
    [vUART1] 	= 152,	// see irqs-9615.h ... GSBI4_UARTDM_IRQ () - HSL0
    [vGP_TIMER] = 18,	// see irqs-9615.h ... GSBI5_UARTDM_IRQ ()
    [vDG_TIMER] = 17,	// see irqs-9615.h ... GSBI5_UARTDM_IRQ ()
    [vSMD]      = 37,	// see irqs-9615.h ... GSBI5_UARTDM_IRQ ()
    [vSMSM]     = 38,	// see irqs-9615.h ... GSBI5_UARTDM_IRQ ()
    [vDMOV]     = 171,
    [vSMC91x]   = 155,
};

static MSM_BoardInfo machines[] =
{
    {
        .cpu_model = "cortex-a8",
        .memmap = a8memmap,
        .irqmap = a8irqmap,
    }
};

typedef struct msm_clock
{
    uint32_t 	   flags;
    unsigned long  rate;
    uint32_t       freq;
    uint32_t       shift;
    const char*    name;
}msm_clock;

enum
{
    MSM_CLOCK_GPT,
    MSM_CLOCK_DGT,
};

msm_clock msm9x15_clk_list[] =
{
    [MSM_CLOCK_GPT] =
          {
	     .name  = "msm9x15-GPT-timer",
	     .freq  = 32768,
	     .shift = 32,
             .rate  = 200,
	  },
    [MSM_CLOCK_DGT] =
          {
	     .name  = "msm9x15-DGT-timer",
	     .freq  = 6750000,
	     .shift = 32,
             .rate  = 300,
	  }
};

/* msm9x15 OS timers */
typedef struct msm9x15_timer_s {
    MemoryRegion iomem;
    qemu_irq irq;
    msm_clock* clk;
    uint32_t val;
    uint32_t match_val;
    int64_t time;
    QEMUTimer *timer;
    QEMUTimer *match;
    QEMUBH *tick;
    hwaddr addr;
    int64_t rate;
    uint32_t enable;
    uint32_t status;
    uint32_t dgt_clk_ctrl;
    uint32_t clear;
}msm9x15_timer_s;

/* 32-kHz timer */
struct msm9x15_32khz_timer_s {
    msm9x15_timer_s 	timer;
    MemoryRegion 	iomem;
};


/******************************************
* DG and GP timers init + Emulation.
******************************************/

static inline uint32_t msm9x15_timer_read(msm9x15_timer_s *timer)
{
uint64_t distance;

     distance = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) - timer->time;
     distance = muldiv64(distance,32768,NANOSECONDS_PER_SECOND);

        if (distance >= 0xffffffff - timer->val)
            return 0xffffffff;
        else
            return timer->val + distance;
}

static inline void msm9x15_timer_sync(msm9x15_timer_s *timer)
{
    timer->val = msm9x15_timer_read(timer);
    timer->time = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
}

static inline void msm9x15_timer_update(msm9x15_timer_s *timer)
{
int64_t expires;
int64_t matches;

    if (timer->enable) {
        expires = muldiv64((1ULL << 32)-timer->val,NANOSECONDS_PER_SECOND,32768);
        timer_mod(timer->timer, timer->time + expires);
        if (timer->match_val >= timer->val) {
            matches = muldiv64(timer->match_val - timer->val,NANOSECONDS_PER_SECOND,32768);
            timer_mod(timer->match, timer->time + matches);
        } else
            timer_del(timer->match);
    } else
    {
        timer_del(timer->timer);
        timer_del(timer->match);
    }
}

static void msm9x15_timer_hdl_write(void *opaque, hwaddr addr,uint64_t value, unsigned size)
{
struct msm9x15_timer_s *s = (msm9x15_timer_s*) opaque;

    switch (addr)
    {
    /*Clock MATCH Value register*/
    case TIMER_MATCH_VAL:
        msm9x15_timer_sync(s);
        s->match_val = value;
        msm9x15_timer_update(s);
        break;
    /*Clock Enable register*/
    case TIMER_ENABLE:
        msm9x15_timer_sync(s);
        s->enable = value & 0x1;
        msm9x15_timer_update(s);
        break;
    /*clock clear register*/
    case TIMER_CLEAR:
        s->clear = value & 0x1;
    	s->match_val = 0;
        break;
    /*clock status register*/
    case DGT_CLK_CTL:
        s->dgt_clk_ctrl = value & 0xf;
        break;
    default:
	fprintf(stderr,"Unexpected register address\n");
        break;
    }
}

static uint64_t msm9x15_timer_hdl_read(void *opaque, hwaddr addr, unsigned size)
{
struct msm9x15_timer_s *s = (msm9x15_timer_s*) opaque;

    switch (addr)
    {
    case TIMER_MATCH_VAL:
	return s->match_val;
    /* Timer Counter register*/
    case TIMER_COUNT_VAL:
        return msm9x15_timer_read(s);
    /* Enable register */
    case TIMER_ENABLE:
	return s->enable;
    }

    fprintf(stderr,"Unexpected register address\n");
    return 0;
}

static const MemoryRegionOps msm9x15_tim_ops = {
    .read = msm9x15_timer_hdl_read,
    .write = msm9x15_timer_hdl_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void msm9x15_timer_interrupt(void *opaque)
{
msm9x15_timer_s *timer = opaque;
    if (timer->enable) qemu_irq_pulse(timer->irq); /* Edge-triggered irq */
}

static void msm9x15_timer_tick(void *opaque)
{
msm9x15_timer_s *timer = (msm9x15_timer_s *) opaque;
    timer->time = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    msm9x15_timer_interrupt(timer);
    msm9x15_timer_update(timer);
}

static void msm9x15_timer_match(void *opaque)
{
msm9x15_timer_s *timer = (msm9x15_timer_s *) opaque;
    msm9x15_timer_interrupt(timer);
}

static void msm9x15_timer_reset(struct msm9x15_32khz_timer_s *s)
{
    timer_del(s->timer.timer);
    timer_del(s->timer.match);
    s->timer.enable = 0;
    s->timer.val = 0;
    s->timer.match_val = ~0;
    s->timer.time = 0;
}

static void msm9x15_dgtimer_init(const MSM_BoardInfo *b,MemoryRegion *memory,qemu_irq *pic)
{
hwaddr base = b->memmap[vDG_TIMER].base;
int irq = b->irqmap[vDG_TIMER]+NUM_IRQS;
const char* name = msm9x15_clk_list[MSM_CLOCK_DGT].name;
struct msm9x15_32khz_timer_s *s = (struct msm9x15_32khz_timer_s *)g_malloc0(sizeof(struct msm9x15_32khz_timer_s));

    s->timer.irq = pic[irq];
    s->timer.addr = base;
    s->timer.clk = &msm9x15_clk_list[MSM_CLOCK_DGT];
    s->timer.timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, msm9x15_timer_tick, &s->timer);
    s->timer.match = timer_new_ns(QEMU_CLOCK_VIRTUAL, msm9x15_timer_match, &s->timer);
    s->timer.tick = qemu_bh_new(msm9x15_timer_interrupt, s);
    msm9x15_timer_reset(s);

    memory_region_init_io(&s->iomem, NULL, &msm9x15_tim_ops, s, name, b->memmap[vDG_TIMER].size);
    memory_region_add_subregion(memory, base, &s->iomem);
}

static void msm9x15_gptimer_init(const MSM_BoardInfo *b,MemoryRegion *memory,qemu_irq *pic)
{
hwaddr base = b->memmap[vGP_TIMER].base;
int irq = b->irqmap[vGP_TIMER]+NUM_IRQS;
const char* name = msm9x15_clk_list[MSM_CLOCK_GPT].name;
struct msm9x15_32khz_timer_s *s = (struct msm9x15_32khz_timer_s *)g_malloc0(sizeof(struct msm9x15_32khz_timer_s));

    s->timer.irq = pic[irq];
    s->timer.addr = base;
    s->timer.clk = &msm9x15_clk_list[MSM_CLOCK_GPT];
    s->timer.timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, msm9x15_timer_tick, &s->timer);
    s->timer.match = timer_new_ns(QEMU_CLOCK_VIRTUAL, msm9x15_timer_match, &s->timer);
    s->timer.tick = qemu_bh_new(msm9x15_timer_interrupt, s);
    msm9x15_timer_reset(s);

    memory_region_init_io(&s->iomem, NULL, &msm9x15_tim_ops, s, name, b->memmap[vGP_TIMER].size);
    memory_region_add_subregion(memory, base, &s->iomem);
}

/******************************************
* SMD & SMSM memory init.
******************************************/

static void msm9x15_smd_smsm_init(const MSM_BoardInfo *b,MemoryRegion *memory,qemu_irq *pic)
{
hwaddr base;
int irq;
uint32_t size;

    base = b->memmap[vSMD].base;
    irq = b->irqmap[vSMD];
    size = b->memmap[vSMD].size;
    msm9x15_smd_init(memory, base, pic[irq], size);

    base = b->memmap[vSMSM].base;
    irq = b->irqmap[vSMSM];
    size = b->memmap[vSMSM].size;
    msm9x15_smsm_init(memory, base, pic[irq], size);
}

/******************************************
* Memory flash init. NOR Flash
******************************************/

static void msm9x15_flash_init(const MSM_BoardInfo *b)
{
DriveInfo *dinfo=NULL;

    dinfo = drive_get(IF_PFLASH, 0, 0);
    if (!dinfo)
    {
        fprintf(stderr, "Flash image must be given with the ""'pflash' parameter\n");
        exit(1);
    }

    if (!pflash_cfi01_register(MSM_NAND_BASE,
                               NULL, "msm.flash0", 256*1024*1024,
                               dinfo ? blk_by_legacy_dinfo(dinfo) : NULL,
                               0x40000, 256*1024*1024 / 0x40000,
                               4, 0, 0, 0, 0, 0))
    {
        fprintf(stderr, "qemu: Error registering flash memory.\n");
        exit(1);
    }
}

/******************************************
* GIC init.
******************************************/

static void msm9x15_gic_init(const MSM_BoardInfo *b, qemu_irq *pic)
{
int gpt_irq = b->irqmap[vGP_TIMER]+NUM_IRQS;
int dgt_irq = b->irqmap[vDG_TIMER]+NUM_IRQS;
//int dmov_irq = b->irqmap[vDMOV];
int smd_irq = b->irqmap[vSMD];
int smsm_irq = b->irqmap[vSMSM];
int smc91x_irq = b->irqmap[vSMC91x];

DeviceState *gicdev;
SysBusDevice *gicbusdev;
const char *gictype = "arm_gic";
int i;
DeviceState *cpudev = DEVICE(qemu_get_cpu(0));

    gicdev = qdev_create(NULL, gictype);
    qdev_prop_set_uint32(gicdev, "revision", 2);
    qdev_prop_set_uint32(gicdev, "num-cpu", smp_cpus);
    /*
     * Note that the num-irq property counts both internal and external
     * interrupts; there are always 32 of the former (mandated by GIC spec).
     */
    qdev_prop_set_uint32(gicdev, "num-irq", NUM_IRQS+GIC_SPI_START);
    qdev_init_nofail(gicdev);
    gicbusdev = SYS_BUS_DEVICE(gicdev);
    sysbus_mmio_map(gicbusdev, 0, b->memmap[vGIC_DIST].base);
    sysbus_mmio_map(gicbusdev, 1, b->memmap[vGIC_CPU].base);

    /*Wire Virtual Timers*/
    pic[gpt_irq] = qdev_get_gpio_in(gicdev,gpt_irq);
    pic[dgt_irq] = qdev_get_gpio_in(gicdev,dgt_irq);
    //pic[dmov_irq] = qdev_get_gpio_in(gicdev,dmov_irq);

    sysbus_connect_irq(gicbusdev, 0, qdev_get_gpio_in(cpudev, ARM_CPU_IRQ));

    for (i = 0; i < NUM_IRQS; i++) {
        pic[i] = qdev_get_gpio_in(gicdev, i);
    }

    pic[smd_irq] = qdev_get_gpio_in(gicdev,smd_irq);
    pic[smsm_irq] = qdev_get_gpio_in(gicdev,smsm_irq);
    pic[smc91x_irq] = qdev_get_gpio_in(gicdev,smc91x_irq);
}

/******************************************
* UART init.
******************************************/

static void msm9x15_uart_init(const MSM_BoardInfo *b,MemoryRegion *memory, qemu_irq *pic)
{
hwaddr base = b->memmap[vUART0].base;
int irq = b->irqmap[vUART0];

    msm_hsl_init(memory, base, pic[irq], 115200, serial_hds[0]);
}

/******************************************
* Board init.
* initialize MSM9x15 machine with cortex-a8 instead of cortex-a5, which is no supported yet on QEMU
******************************************/

static MSM_BoardInfo *find_machine_info(const char *cpu)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(machines); i++) {
        if (strcmp(cpu, machines[i].cpu_model) == 0) {
            return &machines[i];
        }
    }
    return NULL;
}

/******************************************
* TLMM init.
******************************************/

static uint64_t msm9x15_tlmm_read(void *opaque, hwaddr addr, unsigned size)
{
uint64_t ioval = 0;
//unsigned offset = (unsigned)addr;

    switch( size ) {
      case 4:    ioval |= (0 << 24);
      case 3:    ioval |= (0 << 16);
      case 2:    ioval |= (0 << 8);
      case 1:    ioval |= (0);
    }

return ioval;
}

static void msm9x15_tlmm_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
{
//unsigned offset = (unsigned)addr;

    fprintf( stderr, "TLMM %08X,%04X :", (unsigned)addr, size );
    if( addr == 0x820 && value == 0 && size == 4 ) {
      fprintf( stderr, "\nQEMU/MSM9X15 POWER OFF\n\n" );
      exit( 0 ); // TODO: get the event and send the same to ATLAS
    }
}

static const MemoryRegionOps msm9x15_tlmm_ops = {
    .read = msm9x15_tlmm_read,
    .write = msm9x15_tlmm_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void msm9x15_tlmm_init(const MSM_BoardInfo *b,MemoryRegion *memory)
{
hwaddr base = b->memmap[vTLMM].base;
    memory_region_init_io(&tlmm, NULL, &msm9x15_tlmm_ops, NULL, "tlmm", b->memmap[vTLMM].size);
    memory_region_add_subregion(memory, base, &tlmm);
}

/******************************************
* Network init.
******************************************/

static void msm9x15_net_init(const MSM_BoardInfo *b, qemu_irq *pic)
{
    smc91c111_init(&nd_table[0],0x14000000, pic[b->irqmap[vSMC91x]]);
}

/***********************
* Machine Initialization
***********************/

static void machmsm_init(MachineState *machine)
{
MemoryRegion *sysmem = get_system_memory();
int n;
MemoryRegion *ram = g_new(MemoryRegion, 1);
const char *cpu_model = machine->cpu_model;
MSM_BoardInfo *b=NULL;

    if (!cpu_model)
    {
        cpu_model = "cortex-a8";	// Let's use cortex-a5. It is compatible with cortex a8 and a9
    }

    // Machine identification
    if (!(b = find_machine_info(cpu_model)))
    {
        error_report("mach-msm: CPU %s not supported", cpu_model);
        exit(1);
    }

    // CPUs
    b->smp_cpus = smp_cpus;
    for (n = 0; n < smp_cpus; n++) {
        ObjectClass *oc = cpu_class_by_name(TYPE_ARM_CPU, cpu_model);
        Object *cpuobj;

        if (!oc) {
            fprintf(stderr, "Unable to find CPU definition\n");
            exit(1);
        }
        cpuobj = object_new(object_class_get_name(oc));

        /* Secondary CPUs start in PSCI powered-down state */
        if (n > 0) {
            object_property_set_bool(cpuobj, true, "start-powered-off", NULL);
        }

        object_property_set_bool(cpuobj, true, "realized", NULL);
    }

    // RAM Memory creation
    memory_region_init_ram(ram, NULL, "mach-msm.ram", machine->ram_size, &error_abort);
    vmstate_register_ram_global(ram);
    memory_region_add_subregion(sysmem, b->memmap[vMEM_RAM].base, ram);

    // Interrupt Controller creation - GIC
    msm9x15_gic_init(b,pic);

    // Uart creation
    msm9x15_uart_init(b,sysmem,pic);

    // GP and DG Timers clock creation
    msm9x15_gptimer_init(b,sysmem,pic);
    msm9x15_dgtimer_init(b,sysmem,pic);

    // Network card initialisation
    msm9x15_net_init(b,pic);

    // TLMM initialization
    msm9x15_tlmm_init(b,sysmem);

    // SMD & SMSM initialization
    msm9x15_smd_smsm_init(b,sysmem,pic);

    msm9x15_flash_init(b);

    // Boot...
    b->bootinfo.ram_size = machine->ram_size;
    b->bootinfo.kernel_filename = machine->kernel_filename;
    b->bootinfo.kernel_cmdline = machine->kernel_cmdline;
    b->bootinfo.initrd_filename = machine->initrd_filename;
    b->bootinfo.nb_cpus = smp_cpus;
    b->bootinfo.board_id = 3681;
    b->bootinfo.loader_start = b->memmap[vMEM_RAM].base + 0x08000000;

    arm_load_kernel(ARM_CPU(first_cpu), &b->bootinfo);
}

static void machmsm_machine_init(MachineClass *mc)
{
    mc->desc = "MSM9x15 Virtual Machine";
    mc->init = machmsm_init;
    mc->max_cpus = 1;
}

DEFINE_MACHINE("msm9x15", machmsm_machine_init)
