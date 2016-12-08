/*
 * ARM mach-msm 9x40 emulation
 *
 */

//#include "linux/kernel.h"
#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu-common.h"
#include "qemu/timer.h"
#include "cpu.h"

#include "hw/sysbus.h"
#include "hw/arm/arm.h"
#include "hw/arm/primecell.h"
#include "hw/devices.h"
#include "net/net.h"
//#include "sysemu/block-backend.h"
#include "sysemu/device_tree.h"
#include "sysemu/sysemu.h"
#include "sysemu/kvm.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "hw/block/flash.h"
#include "exec/address-spaces.h"
#include "qemu/bitops.h"
#include "qemu/error-report.h"
#include "hw/char/serial.h"
#include "hw/arm/msm.h"


// Nand Resource
#define MSM_NAND_BASE       0xA0000000

// Resource MEM : .start= MSM_NAND_PHYS, .end= MSM_NAND_PHYS+0x7FF

// 0-15 STI/STG | 16-31 PPI | 32+ SPI
// GIC PPI(private peripheral interrupts)
// GIC SPI (shared peripheral interrupts)
#define GIC_PPI_START 16
#define GIC_SPI_START 32

/* Number of external interrupt lines to configure the GIC with */
#define NUM_IRQS 640

typedef struct MemMapEntry
{
    hwaddr base;
    hwaddr size;
} MemMapEntry;

typedef struct MSM_BoardInfo
{
    struct arm_boot_info bootinfo;
    const char *cpu_model;
    const MemMapEntry *memmap;
    const int *irqmap;
    int smp_cpus;
    uint32_t clock_phandle;
    DeviceState *nand;
} MSM_BoardInfo;

qemu_irq pic_9x40[NUM_IRQS];

enum
{
    vMEM_NAND,
    vMEM_RAM1,
    vMEM_RAM2,
    vMEM_RAM3,
    vMEM_RAM4,
    vGIC_DIST,
    vGIC_CPU,
    vSMD,
    vSMSM,
    vSMEM_SIERRA,
    vSMEM_TARG_INFO,
    vSMEM_AUX1,
    vUART0,
    vUART1,
    vTIMER,
    vTLMM,
    vRPM,
    vSPINLOCK,
    vTEST,
    vSMC91x,
};

static const MemMapEntry a15memmap[] =
{
    /* GIC distributor and CPU interfaces sit inside the CPU peripheral space */
    [vGIC_DIST] =   { 0x0b000000, 0x00001000 }, // see msm_iomap-9640.h
    [vGIC_CPU] =    { 0x0b002000, 0x00001000 }, // see msm_iomap-9640.h
    [vMEM_RAM1] =   { 0x80000000, 0x10000000 }, // See device tree swi-mdm9640.dtsi
    [vMEM_NAND] =   { 0x90000000, 0x0c000000 }, // See device tree swi-mdm9640.dtsi
    [vMEM_RAM2] =   { 0x87c00000, 0x00400000 }, // See device tree swi-mdm9640.dtsi
    [vMEM_RAM3] =   { 0x88000000, 0x06c00000 }, // See device tree swi-mdm9640.dtsi
    [vMEM_RAM4] =   { 0x8ee00000, 0x01100000 }, // See device tree swi-mdm9640.dtsi
    [vSMD] =        { 0x0b011000, 0x00000100 }, // See device tree swi-mdm9640.dtsi
    [vSMSM] =       { 0x87e80000, 0x000C0000 }, // See device tree swi-mdm9640.dtsi
    [vSMEM_SIERRA] ={ 0x8fC00000, 0x00400000 }, // See device tree swi-mdm9640.dtsi
    [vSMEM_TARG_INFO] = { 0x0193D000, 0x00000008 }, // See device tree swi-mdm9640.dtsi
    [vSMEM_AUX1] =  { 0x00060000, 0x00008000 }, // See device tree swi-mdm9640.dtsi
    [vUART0] =      { 0x078AF000, 0x00001000 }, // See device tree swi-mdm9640.dtsi
    [vUART1] =      { 0x078B0000, 0x00001000 }, // See device tree swi-mdm9640.dtsi
    [vTIMER] =      { 0x0b020000, 0x3000     },
    [vTLMM]     =   { 0x00800000, 0x00100000 },
    [vRPM]      =   { 0xFFFFFFFF, 0x00001000 },
    [vSPINLOCK] =   { 0x01905000, 0x00008000 },
    [vTEST] =       { 0x8a800000, 0x00100000 }, // See device tree swi-mdm9640.dtsi
};

static const int a15irqmap[] =
{
    [vUART0]    = 107, // See device tree (swi-mdm9640.dtsi)
    [vUART1]    = 108, // swi-mdm9640.dtsi
    [vTIMER] = 6,
    [vSMD]      = 25,
    [vSMSM]     = 26,
    [vRPM]      = 168,
    [vSMC91x]   = 150,
};

static MSM_BoardInfo machines[] =
{
    {
        .cpu_model = "cortex-a15",
        .memmap = a15memmap,
        .irqmap = a15irqmap,
    }
};


/******************************************
* arch arm timer
******************************************/

// Qemu virtal timer 1GHz / MDM9x timer 19.2 MHz
#define TIMER_SCALE (1000000000/19200000)
//#define TIMER_SCALE 100


#define ARCH_TIMER_CTRL_ENABLE          (1 << 0)
#define ARCH_TIMER_CTRL_IT_MASK         (1 << 1)
#define ARCH_TIMER_CTRL_IT_STAT         (1 << 2)

#define ARCH_TIMER_REG_CTRL             0
#define ARCH_TIMER_REG_TVAL             1

#define ARCH_TIMER_PHYS_ACCESS          0
#define ARCH_TIMER_VIRT_ACCESS          1
#define ARCH_TIMER_MEM_PHYS_ACCESS      2
#define ARCH_TIMER_MEM_VIRT_ACCESS      3

#define ARCH_TIMER_USR_PCT_ACCESS_EN    (1 << 0) /* physical counter */
#define ARCH_TIMER_USR_VCT_ACCESS_EN    (1 << 1) /* virtual counter */
#define ARCH_TIMER_VIRT_EVT_EN          (1 << 2)
#define ARCH_TIMER_EVT_TRIGGER_SHIFT    (4)
#define ARCH_TIMER_EVT_TRIGGER_MASK     (0xF << ARCH_TIMER_EVT_TRIGGER_SHIFT)
#define ARCH_TIMER_USR_VT_ACCESS_EN     (1 << 8) /* virtual timer registers */
#define ARCH_TIMER_USR_PT_ACCESS_EN     (1 << 9) /* physical timer registers */

#define ARCH_TIMER_EVT_STREAM_FREQ      10000   /* 100us */

#define TIMER_CNTTIDR           0x8
#define TIMER_PCNT_HI           0x1004
#define TIMER_PCNT_LO           0x1000
#define TIMER_VCNT_HI           0x100C
#define TIMER_VCNT_LO           0x1008
//#define TIMER_PCNT_TVAL         0x1028
//#define TIMER_PCNT_CTL          0x102C
#define TIMER_VCNT_TVAL         0x1038
#define TIMER_VCNT_CTL          0x103C

// For register TIMER_CNTTIDR
#define TIMER_VIRTUAL           0x1
#define TIMER_FRAME_IMPLEMENTED 0x2

typedef struct msm_clock
{
    uint32_t       flags;
    unsigned long  rate;
    uint32_t       freq;
    uint32_t       shift;
    const char*    name;
}msm_clock;

enum
{
    MSM_CLOCK_DGT,
};

msm_clock msm9x40_clk_list[] =
{
    [0] =
          {
         .name  = "msm9x40-timer",
         .freq  = 19200000,
         .shift = 32,
             .rate  = 300,
      }
};

/* msm9x40 OS timers */
typedef struct msm9x40_timer_s {
    MemoryRegion iomem;
    qemu_irq irq;
    msm_clock* clk;
    uint64_t val;
    uint64_t tval; // virtual downcounter timer
    uint32_t reset_val;
    int64_t time;
    QEMUTimer *timer;
    QEMUBH *tick;
    hwaddr addr;
    int64_t rate;
    uint32_t ctl;
    uint32_t status;
    uint32_t dgt_clk_ctrl;
    uint32_t clear;
}msm9x40_timer_s;

/* 32-kHz timer */
struct msm9x40_32khz_timer_s {
    msm9x40_timer_s     timer;
    MemoryRegion    iomem;
};

/******************************************
 * ARM generic timer emulation.
******************************************/

static inline void msm9x40_timer_sync(msm9x40_timer_s *timer)
{
    timer->time = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL)/TIMER_SCALE;
}

static inline uint64_t msm9x40_timer_read(msm9x40_timer_s *timer)
{
    msm9x40_timer_sync(timer);
    //fprintf(stderr, "Timer read %lld\n", (long long int)timer->time);
    return timer->time;
}

static inline void msm9x40_timer_update(msm9x40_timer_s *timer)
{
int64_t expires;

    //fprintf( stderr, "ctl=%lld\n", (long long int)timer->ctl);
    if (timer->ctl & ARCH_TIMER_CTRL_ENABLE){

        // if tval !=0, it means that a timer must be started
        if (timer->tval != 0)
        {
            msm9x40_timer_sync(timer);
            //fprintf( stderr, "Tval=%lld\n", (long long int)timer->tval);
            expires = timer->time + timer->tval;
            //fprintf(stderr,"Current time %lld, qemu time %lld\n",(long long int)timer->time, (long long int)timer->time*TIMER_SCALE);
            //fprintf(stderr,"Expected Timer %lld, qemu time %lld\n",(long long int)expires, (long long int)expires*TIMER_SCALE);
            timer_mod(timer->timer, expires*TIMER_SCALE);
        }
    } else
    {
        //fprintf(stderr, "Nothing to update\n");
        timer_del(timer->timer);
        timer->ctl &= ~ARCH_TIMER_CTRL_IT_MASK;
        qemu_irq_lower(timer->irq);
    }
}


static void msm9x40_timer_hdl_write(void *opaque, hwaddr addr,uint64_t value, unsigned size)
{
struct msm9x40_timer_s *timer = (msm9x40_timer_s*) opaque;


//fprintf(stderr,"****** msm9x40_timer_hdl_write:register address (1) %08X, %llu %u\n", (unsigned)addr,(unsigned long long)value, size);
    switch (addr)
    {
#if 0
    /*Clock MATCH Value register*/
    case TIMER_MATCH_VAL:
        msm9x40_timer_sync(s);
        timer->reset_val = value-timer->val;
        msm9x40_timer_update(s);
        break;
#endif
    /*Clock ctl register*/
    case TIMER_VCNT_CTL:
        //fprintf(stderr, "TIMER_VCNT_CTL %lld\n", (long long int)value);
        //msm9x40_timer_sync(s);
        timer->ctl = value;
        if ((value & ARCH_TIMER_CTRL_ENABLE) == 0)
        {
            //fprintf(stderr, "Init IRQ\n");
            // If enable is changed, no more IRQ: change IT status and IRQ level
            timer->ctl &= ~ARCH_TIMER_CTRL_IT_STAT;
            timer->ctl &= ~ARCH_TIMER_CTRL_IT_MASK;
            qemu_irq_lower(timer->irq);
        }
        msm9x40_timer_update(timer);
        break;
    case TIMER_VCNT_TVAL:
        //msm9x40_timer_sync(s);
        timer->tval = value;
        //fprintf(stderr, "TIMER_VCNT_TVAL %d\n", (int)value);
        msm9x40_timer_update(timer);
        //fprintf(stderr, "qemu_irq_lower\n");
        // If Tval is changed, no more IRQ: change IT status and IRQ level
        qemu_irq_lower(timer->irq); /* Edge-triggered irq */
        timer->ctl &= ~ARCH_TIMER_CTRL_IT_STAT;
        timer->ctl &= ~ARCH_TIMER_CTRL_IT_MASK;
        break;
    default:
        //fprintf(stderr,"Unexpected register address (1) %08X, %llu %u\n", (uint32_t)addr, (long long unsigned int)value, size);
        break;
    }
}

static uint64_t msm9x40_timer_hdl_read(void *opaque, hwaddr addr, unsigned size)
{
struct msm9x40_timer_s *s = (msm9x40_timer_s*) opaque;

//fprintf(stderr, "msm9x40_timer_hdl_read:%08X %u \n", (unsigned int)addr, size );
    switch (addr)
    {
    /* Timer Counter register*/
    case TIMER_CNTTIDR:
        //fprintf(stderr, "TIMER_CNTTIDR\n");
        return (TIMER_VIRTUAL | TIMER_FRAME_IMPLEMENTED);
    case TIMER_PCNT_HI:
    case TIMER_VCNT_HI:
        //fprintf(stderr, "read: %d\n", (unsigned int)((msm9x40_timer_read(s) >> 32) & 0xFFFFFFFFUL ));
        return (msm9x40_timer_read(s) >> 32) & 0xFFFFFFFFUL;
    case TIMER_PCNT_LO:
    case TIMER_VCNT_LO:
        //fprintf(stderr, "read: %d\n", (unsigned int)((msm9x40_timer_read(s)) & 0xFFFFFFFFUL ));
        return msm9x40_timer_read(s) & 0xFFFFFFFFUL;
    case TIMER_VCNT_CTL:
        //fprintf(stderr, "TIMER_VCNT_CTL\n");
        return( s->ctl );
    default:
        //fprintf(stderr,"TIMER : Unexpected register address (%X)\n", (unsigned)addr);
        break;
    }
    return 0;
}

static const MemoryRegionOps msm9x40_tim_ops = {
    .read = msm9x40_timer_hdl_read,
    .write = msm9x40_timer_hdl_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void msm9x40_timer_interrupt(void *opaque)
{
msm9x40_timer_s *timer = opaque;

//fprintf(stderr, "**** msm9x40_timer_interrupt\n");
    if ((timer->ctl & ARCH_TIMER_CTRL_ENABLE)
            && (!(timer->ctl & ARCH_TIMER_CTRL_IT_MASK)))
    {
       timer->ctl |= ARCH_TIMER_CTRL_IT_STAT;
       //timer->val += timer->tval;
       timer->tval = 0;
       //qemu_irq_pulse(timer->irq); /* Edge-triggered irq */
       //fprintf(stderr, "Raise interrupt\n");
       qemu_irq_raise(timer->irq); /* Edge-triggered irq */
    }
    else
    {
        //fprintf(stderr, "Irq not raise\n");
    }
}

static void msm9x40_timer_tick(void *opaque)
{
msm9x40_timer_s *timer = (msm9x40_timer_s *) opaque;

    timer->time = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    timer->val += 300;

    timer->reset_val=0;
    msm9x40_timer_interrupt(timer);

    msm9x40_timer_update(timer);
}

static void msm9x40_timer_reset(struct msm9x40_32khz_timer_s *s)
{
    timer_del(s->timer.timer);
    s->timer.ctl = 0;
    s->timer.val = 0;
    s->timer.tval = 0;
    s->timer.reset_val = ~0;
    s->timer.time = 0;
}

static void msm9x40_timer_init(const MSM_BoardInfo *b,MemoryRegion *memory,qemu_irq *pic_9x40)
{
hwaddr base = b->memmap[vTIMER].base;
int irq = b->irqmap[vTIMER];
const char* name = msm9x40_clk_list[0].name;
struct msm9x40_32khz_timer_s *s = (struct msm9x40_32khz_timer_s *)g_malloc0(sizeof(struct msm9x40_32khz_timer_s));

    s->timer.irq = pic_9x40[irq];
    s->timer.addr = base;
    s->timer.clk = &msm9x40_clk_list[0];
    s->timer.timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, msm9x40_timer_tick, &s->timer);
    s->timer.tick = qemu_bh_new(msm9x40_timer_interrupt, s);
    msm9x40_timer_reset(s);

    memory_region_init_io(&s->iomem, NULL, &msm9x40_tim_ops, s, name, b->memmap[vTIMER].size);
    memory_region_add_subregion(memory, base, &s->iomem);
}

/******************************************
* SMD & SMSM memory init.
******************************************/

static void msm9x40_smd_smsm_init(const MSM_BoardInfo *b,MemoryRegion *memory,qemu_irq *pic)
{
hwaddr base;
int irq;
uint32_t size;

    base = b->memmap[vSMD].base;
    irq = b->irqmap[vSMD];
    size = b->memmap[vSMD].size;
    msm9x40_smd_init(memory, base, pic[irq], size);

    base = b->memmap[vSMSM].base;
    irq = b->irqmap[vSMSM];
    size = b->memmap[vSMSM].size;
    msm9x40_smsm_init(memory, base, pic[irq], size);

#if 0
    base = b->memmap[vSMEM_SIERRA].base;
    irq = b->irqmap[vSMEM_SIERRA];
    size = b->memmap[vSMEM_SIERRA].size;
    msm9x40_smem_sierra_init(memory, base, pic[irq], size);
#endif

    base = b->memmap[vSMEM_TARG_INFO].base;
    irq = b->irqmap[vSMEM_TARG_INFO];
    size = b->memmap[vSMEM_TARG_INFO].size;
    msm9x40_smem_targ_info_init(memory, base, pic[irq], size);

#if 0
    base = b->memmap[vSMEM_AUX1].base;
    irq = b->irqmap[vSMEM_AUX1];
    size = b->memmap[vSMEM_AUX1].size;
    msm9x40_smem_aux1_init(memory, base, pic[irq], size);
#endif
 }

/******************************************
* Memory flash init. NOR Flash
******************************************/

static void msm9x40_flash_init(const MSM_BoardInfo *b)
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

static void msm9x40_gic_init(const MSM_BoardInfo *b, qemu_irq *pic_9x40)
{

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
    qdev_prop_set_uint32(gicdev, "num-irq", NUM_IRQS);
    qdev_init_nofail(gicdev);
    gicbusdev = SYS_BUS_DEVICE(gicdev);
    sysbus_mmio_map(gicbusdev, 0, b->memmap[vGIC_DIST].base);
    sysbus_mmio_map(gicbusdev, 1, b->memmap[vGIC_CPU].base);

    sysbus_connect_irq(gicbusdev, 0, qdev_get_gpio_in(cpudev, ARM_CPU_IRQ));

    for (i = 0; i < NUM_IRQS; i++) {
        pic_9x40[i] = qdev_get_gpio_in(gicdev, i);
    }
}

/******************************************
* UART init.
******************************************/

static void msm9x40_uart_init(const MSM_BoardInfo *b,MemoryRegion *memory, qemu_irq *pic_9x40)
{
	// console UART is a HS uart
	hwaddr base = b->memmap[vUART1].base;
    int irq = b->irqmap[vUART1];

    msm_hs_init(memory, base, pic_9x40[irq], 115200, serial_hds[0]);

	// Early debug use UART0 (HSL lite driver).
	//base = b->memmap[vUART0].base;
    //irq = b->irqmap[vUART0];

    //msm_hsl_init(memory, base, pic_9x40[irq], 115200, serial_hds[0]);
}

/******************************************
* Board init.
* initialize MSM9x40 machine with cortex-a15 instead of cortex-a5, which is no supported yet on QEMU
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
* Network init.
******************************************/

static void msm9x40_net_init(const MSM_BoardInfo *b, qemu_irq *pic)
{
	smc91c111_init(&nd_table[0], 0x8f00000, pic[b->irqmap[vSMC91x]]);
}

/***********************
* Machine Initialization
***********************/

static void machmsm_init(MachineState *machine)
{
MemoryRegion *sysmem = get_system_memory();
int n;
MemoryRegion *nand = g_new(MemoryRegion, 1);
MemoryRegion *ram1 = g_new(MemoryRegion, 1);
MemoryRegion *spinlock = g_new(MemoryRegion, 1);
const char *cpu_model = machine->cpu_model;
MSM_BoardInfo *b=NULL;

    if (!cpu_model)
    {
        cpu_model = "cortex-a15";   // Let's use cortex-a7. It is compatible with cortex a15 and a9
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
    memory_region_init_ram(nand, NULL, "mach-msm.nand", b->memmap[vMEM_NAND].size, &error_abort);
    vmstate_register_ram_global(nand);
    memory_region_add_subregion(sysmem, b->memmap[vMEM_NAND].base, nand);
    memory_region_init_ram(ram1, NULL, "mach-msm.ram1", machine->ram_size, &error_abort);
    vmstate_register_ram_global(ram1);
    memory_region_add_subregion(sysmem, b->memmap[vMEM_RAM1].base, ram1);
    // Spinlocks
    memory_region_init_ram(spinlock, NULL, "mach-msm.spinlock", b->memmap[vSPINLOCK].size, &error_abort);
    vmstate_register_ram_global(spinlock);
    memory_region_add_subregion(sysmem, b->memmap[vSPINLOCK].base, spinlock);

    // Interrupt Controller creation - GIC
    msm9x40_gic_init(b,pic_9x40);

    // Uart creation
    msm9x40_uart_init(b,sysmem,pic_9x40);

    // arm_arch_timer
    msm9x40_timer_init(b,sysmem,pic_9x40);

    // Network card initialisation
    msm9x40_net_init(b,pic_9x40);

    // SMD & SMSM initialization
    msm9x40_smd_smsm_init(b,sysmem,pic_9x40);

    msm9x40_flash_init(b);

    // Boot...
    b->bootinfo.ram_size = machine->ram_size;
    b->bootinfo.kernel_filename = machine->kernel_filename;
    b->bootinfo.kernel_cmdline = machine->kernel_cmdline;
    b->bootinfo.initrd_filename = machine->initrd_filename;
    b->bootinfo.nb_cpus = smp_cpus;
    b->bootinfo.board_id = -1; // Because we use a device tree dtb
    b->bootinfo.loader_start = b->memmap[vMEM_NAND].base;

    arm_load_kernel(ARM_CPU(first_cpu), &b->bootinfo);
}

static void machmsm_machine_init(MachineClass *mc)
{
    mc->desc = "MSM9x40";
    mc->init = machmsm_init;
}

DEFINE_MACHINE("msm9x40", machmsm_machine_init)

