/*
 * QEMU altair with MIPS 5150
 *
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/hw.h"
#include "hw/i386/pc.h"
#include "hw/char/serial.h"
#include "hw/block/fdc.h"
#include "net/net.h"
#include "hw/boards.h"
#include "hw/i2c/smbus.h"
#include "sysemu/block-backend.h"
#include "hw/block/flash.h"
#include "hw/mips/mips.h"
#include "hw/mips/cpudevs.h"
#include "hw/pci/pci.h"
#include "sysemu/char.h"
#include "sysemu/sysemu.h"
#include "audio/audio.h"
#include "qemu/log.h"
#include "hw/loader.h"
#include "hw/mips/bios.h"
#include "hw/ide.h"
#include "elf.h"
#include "sysemu/blockdev.h"
#include "exec/address-spaces.h"
#include "sysemu/qtest.h"
#include "qemu/error-report.h"

#include "stdcomp.h"
#include "globdefs.h"
#include "guidefs.h"
#include "ulc_map.h"
#include "userio.h"

#define ENVP_ADDR       0x80202000l
#define ENVP_NB_ENTRIES	 	16
#define ENVP_ENTRY_SIZE	 	256


#define MEM_BASE_ADDRESS        0x1000000
#define SHARED_REG_BASE_ADDRESS (MEM_BASE_ADDRESS + SHARED_REG_BASE)
#define DPRAM_BASE_ADDRESS      (MEM_BASE_ADDRESS + DPRAM_BASE)
#define TXPORT_BASE_ADDRESS     (MEM_BASE_ADDRESS + TX_PORT_BASE)
#define TXPORT_IRQ              TX_PROCESS_INT
#define ADD_DPRAM (short int*)DPRAM_BASE
#define SHARED_REGISTERS (short int*)SHARED_REG_BASE
#include "ULC_Dpram.h"
#include "ULC_Aliases.h"
CPUMIPSState *env;

#define Param_RACH_Type 8

#define RAM_HOST_ADD(X) (ramHost + X)
/* Host address of the DPRAM memory */
#define DPRAM_HOST_ADD(X) ((unsigned char*)ramHost + (unsigned int)(intptr_t)(X) + MEM_BASE_ADDRESS)
/* Host address of the shared registers */
#define SHARED_REG_HOST_ADD(X) ((unsigned char*)ramHost + (unsigned int)(intptr_t)(X) + (unsigned int)MEM_BASE_ADDRESS)

#define UART_BASE_ADDRESS   0x01100000

unsigned char * ramHost, *dpramHost, *sharedRegHost;
bool endTransmit = 0;
FILE *pInputDataFilePointer = NULL;
FILE *pOutputDataFilePointer = NULL;
int testLoopCnt = -1;

typedef struct dpram_s {
    MemoryRegion iomem;
    qemu_irq irq;
    uint8_t *ramptr;
    FILE    *fd;
} altair_txport;


static struct _loaderparams {
    int ram_size;
    const char *kernel_filename;
    const char *kernel_cmdline;
    const char *initrd_filename;
} loaderparams;

void readRACHData(FILE *pInputDataFilePointer);
void initBuffers(void);
void testRach(int loopCnt);
void testRachEnd(void);

static int64_t load_kernel (CPUMIPSState *env)
{
    int64_t kernel_entry, kernel_low, kernel_high;
    long initrd_size;
    ram_addr_t initrd_offset;

    if (load_elf(loaderparams.kernel_filename, cpu_mips_kseg0_to_phys, NULL,
                 (uint64_t *)&kernel_entry, (uint64_t *)&kernel_low,
                 (uint64_t *)&kernel_high, 0, EM_MIPS, 1, 0) < 0) {
        fprintf(stderr, "qemu: could not load kernel '%s'\n",
                loaderparams.kernel_filename);
        exit(1);
    }

    /* load initrd */
    initrd_size = 0;
    initrd_offset = 0;
    if (loaderparams.initrd_filename) {
        initrd_size = get_image_size (loaderparams.initrd_filename);
        if (initrd_size > 0) {
            initrd_offset = (kernel_high + ~INITRD_PAGE_MASK) & INITRD_PAGE_MASK;
            if (initrd_offset + initrd_size > ram_size) {
                fprintf(stderr,
                        "qemu: memory too small for initial ram disk '%s'\n",
                        loaderparams.initrd_filename);
                exit(1);
            }
            initrd_size = load_image_targphys(loaderparams.initrd_filename,
                                     initrd_offset, ram_size - initrd_offset);
        }
        if (initrd_size == (target_ulong) -1) {
            fprintf(stderr, "qemu: could not load initial ram disk '%s'\n",
                    loaderparams.initrd_filename);
            exit(1);
        }
    }

    return kernel_entry;
}

static void write_bootloader (CPUMIPSState *env, uint8_t *base, int64_t kernel_addr)
{
    uint32_t *p;

    /* Small bootloader */
    p = (uint32_t *) base;

    stl_p(p++, 0x0bf00010);                                      /* j 0x1fc00040 */
    stl_p(p++, 0x00000000);                                      /* nop */

    /* Second part of the bootloader */
    p = (uint32_t *) (base + 0x040);

#if 0
    /* For big endian code */
    stl_p(p++, 0x3c040000);                                      /* lui a0, 0 */
    stl_p(p++, 0x34840002);                                      /* ori a0, a0, 2 */
    stl_p(p++, 0x3c050000 | ((ENVP_ADDR >> 16) & 0xffff));       /* lui a1, high(ENVP_ADDR) */
    stl_p(p++, 0x34a50000 | (ENVP_ADDR & 0xffff));               /* ori a1, a0, low(ENVP_ADDR) */
    stl_p(p++, 0x3c060000 | (((ENVP_ADDR + 8) >> 16) & 0xffff)); /* lui a2, high(ENVP_ADDR + 8) */
    stl_p(p++, 0x34c60000 | ((ENVP_ADDR + 8) & 0xffff));         /* ori a2, a2, low(ENVP_ADDR + 8) */
    stl_p(p++, 0x3c070000 | (loaderparams.ram_size >> 16));      /* lui a3, high(env->ram_size) */
    stl_p(p++, 0x34e70000 | (loaderparams.ram_size & 0xffff));   /* ori a3, a3, low(env->ram_size) */
    stl_p(p++, 0x3c1f0000 | ((kernel_addr >> 16) & 0xffff));     /* lui ra, high(kernel_addr) */;
    stl_p(p++, 0x37ff0000 | (kernel_addr & 0xffff));             /* ori ra, ra, low(kernel_addr) */
    stl_p(p++, 0x03e00008);                                      /* jr ra */
    stl_p(p++, 0x00000000);                                      /* nop */
#endif
    /* For little endian code */
    stl_p(p++, 0x0000043c);                                      /* lui a0, 0 */
    stl_p(p++, 0x02008434);                                      /* ori a0, a0, 2 */
    stl_p(p++, 0x0000053c | ((ENVP_ADDR >> 16) & 0xffff));       /* lui a1, high(ENVP_ADDR) */
    stl_p(p++, 0x0000a534 | (ENVP_ADDR & 0xffff));               /* ori a1, a0, low(ENVP_ADDR) */
    stl_p(p++, 0x0000063c | (((ENVP_ADDR + 8) >> 16) & 0xffff)); /* lui a2, high(ENVP_ADDR + 8) */
    stl_p(p++, 0x0000c634 | ((ENVP_ADDR + 8) & 0xffff));         /* ori a2, a2, low(ENVP_ADDR + 8) */
    stl_p(p++, 0x0000073c | (loaderparams.ram_size >> 16));      /* lui a3, high(env->ram_size) */
    stl_p(p++, 0x0000e734 | (loaderparams.ram_size & 0xffff));   /* ori a3, a3, low(env->ram_size) */
    stl_p(p++, 0x00001f3c | ((kernel_addr >> 16) & 0xffff));     /* lui ra, high(kernel_addr) */;
    stl_p(p++, 0x0000ff37 | (kernel_addr & 0xffff));             /* ori ra, ra, low(kernel_addr) */
    stl_p(p++, 0x0800e003);                                      /* jr ra */
    stl_p(p++, 0x00000000);                                      /* nop */

}


static void main_cpu_reset(void *opaque)
{
    MIPSCPU *cpu = opaque;
    CPUMIPSState *env = &cpu->env;

    cpu_reset(CPU(cpu));
    /* TODO: 2E reset stuff */
    if (loaderparams.kernel_filename) {
        env->CP0_Status &= ~((1 << CP0St_BEV) | (1 << CP0St_ERL));
    }
}


static uint64_t altair_txport_read(void *opaque, hwaddr addr, unsigned size)
{
    fprintf( stderr, "altair_txport_read at %d size %d\n", (int)addr, size );
    return 0;
}

static void altair_txport_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
{
    /*
     * Fake TXPORT registers:
     *
     * 0-3 : Tx register. Data to transmit. Data are written in a file.
     *
     * 4-7 : bit 0. if write 0, Acknowledge of the TX port interrupt
     *              if write 1, TX data end. Raise Tx IRQ to restart a new RACCH loop
     *
     */
    altair_txport *s = opaque;

    //fprintf( stderr, "altair_txport_addr 0x%x write %d\n", (int) addr, (int)value );
    if ((int)addr == 4){
        endTransmit = 1;
        if (value == 0)
        {
            fprintf( stderr, "IRQ acknowledge \n" );
            qemu_irq_lower(s->irq);
        }
        else if (value == 1) {
            if (testLoopCnt > 0)
            {
                testLoopCnt--;
                fflush( stdout ); fflush( stderr );
                fprintf( stderr, "RACCH loopcount %d\n", testLoopCnt );
                /* Reinitialize the test for a new loop */
                uint32_t *pSrBuffCtrl = (uint32_t *)SHARED_REG_HOST_ADD(SR_BUFFER_CONTROL);
                *pSrBuffCtrl = 1;
                initBuffers();
                readRACHData(pInputDataFilePointer);
                fprintf( stderr, "end of RACCH. Raise IRQ \n" );
                qemu_irq_raise(s->irq);
            }
            else {
                /* End of the test */
                testRachEnd();
            }
        }
    }
    else if ((int)addr == 0){
        //fprintf( stderr, "Write to file \n" );
        fprintf(pOutputDataFilePointer,"0x%.4x\n",(unsigned int)value);
    }
}

static const MemoryRegionOps altair_txport_ops = {
    .read = altair_txport_read,
    .write = altair_txport_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void altair_inittxport(MemoryRegion *memory,hwaddr base,qemu_irq irq,uint32_t size)
{
    altair_txport *s = (altair_txport *)g_malloc0( sizeof( altair_txport ) );

    memory_region_init_io(&s->iomem, NULL, &altair_txport_ops, s, "dpram", size);
    memory_region_add_subregion(memory, base, &s->iomem);
    s->irq = irq;
}

static void mips_altair_init(MachineState *machine)
{
    ram_addr_t ram_size = machine->ram_size;
    const char *cpu_model = machine->cpu_model;
    const char *kernel_filename = machine->kernel_filename;
    const char *kernel_cmdline = machine->kernel_cmdline;
    const char *initrd_filename = machine->initrd_filename;
    //char *filename;
    MemoryRegion *address_space_mem = get_system_memory();
    MemoryRegion *ram = g_new(MemoryRegion, 1);
    MemoryRegion *bios = g_new(MemoryRegion, 1);
#if 0
    MemoryRegion *dpram = g_new(MemoryRegion, 1);
    MemoryRegion *shared_reg = g_new(MemoryRegion, 1);
#endif
    long bios_size;
    int64_t kernel_entry;
    MIPSCPU *cpu;
    //CPUMIPSState *env;

    /* init CPUs */
    if (cpu_model == NULL) {
        cpu_model = "P5600";
    }
    cpu = cpu_mips_init(cpu_model);
    if (cpu == NULL) {
        fprintf(stderr, "Unable to find CPU definition\n");
        exit(1);
    }
    env = &cpu->env;

    qemu_register_reset(main_cpu_reset, cpu);

    /* Set bios size to 10k */
    bios_size = 10 * 1024;

    /* allocate RAM */
    memory_region_allocate_system_memory(ram, NULL, "altair.ram", ram_size);
    memory_region_init_ram(bios, NULL, "altair.bios", bios_size,
                           &error_fatal);
    vmstate_register_ram_global(bios);
    //memory_region_set_readonly(bios, true);

    fprintf(stderr, "RAM init add 0 Machine ram size 0x%x\n", (int)machine->ram_size);
    memory_region_add_subregion(address_space_mem, 0, ram);
    ramHost =  memory_region_get_ram_ptr(ram);
#if 0
    dpramHost =  memory_region_get_ram_ptr(dpram);
    sharedRegHost =  memory_region_get_ram_ptr(shared_reg);
#endif

    fprintf(stderr, "Bios init add 0x%x size 0x%x\n", (int)0x1fc00000, (int)bios_size);
    memory_region_add_subregion(address_space_mem, 0x1fc00000LL, bios);

#if 0
    //short int * dpramHost = memory_region_get_ram_ptr();
    //short int * sharedRegHost = memory_region_get_ram_ptr();
    fprintf(stderr, "DPRAM init add 0x%x size 0x%x\n", DPRAM_BASE_ADDRESS, DPRAM_SIZE);
    //memory_region_allocate_system_memory(ram, NULL, "dpram.ram", DPRAM_SIZE);
    memory_region_init_ram(dpram, NULL, "dpram.ram", DPRAM_SIZE,
                           &error_fatal);
    memory_region_add_subregion(address_space_mem, DPRAM_BASE_ADDRESS, dpram);
    vmstate_register_ram_global(dpram);

    fprintf(stderr, "SHARED REG init add 0x%x size 0x%x\n", SHARED_REG_BASE_ADDRESS, SHARED_REG_SIZE);
    //memory_region_allocate_system_memory(ram, NULL, "shared_reg.ram", SHARED_REG_SIZE);
    memory_region_init_ram(shared_reg, NULL, "shared_reg.ram", SHARED_REG_SIZE,
                           &error_fatal);
    memory_region_add_subregion(address_space_mem, SHARED_REG_BASE_ADDRESS, shared_reg);
    vmstate_register_ram_global(shared_reg);
#endif

    if (kernel_filename) {
        loaderparams.ram_size = ram_size;
        loaderparams.kernel_filename = kernel_filename;
        loaderparams.kernel_cmdline = kernel_cmdline;
        loaderparams.initrd_filename = initrd_filename;
        kernel_entry = load_kernel (env);
        write_bootloader(env, memory_region_get_ram_ptr(bios), kernel_entry);
        fprintf(stderr, "kernel and bootloader loaded\n");
    }
    else {
        fprintf(stderr, "ERROR: kernel filename missing!!!!\n");
        return;
    }

    /* Init internal devices */
    cpu_mips_irq_init_cpu(env);
    cpu_mips_clock_init(env);

    fprintf(stderr, "TX port init add 0x%x irq 0x%x size 0x%x\n", TXPORT_BASE_ADDRESS, TXPORT_IRQ, TX_PORT_SIZE);
    altair_inittxport(address_space_mem, TXPORT_BASE_ADDRESS, env->irq[TXPORT_IRQ], TX_PORT_SIZE);

    /* 16450 uart */
    serial_init(UART_BASE_ADDRESS, env->irq[4], 115200, serial_hds[0],
                    get_system_io());
    testRach(18);
}


/**************************************************/

void readRACHData(FILE *pInputDataFilePointer)
{
    uint16_t *pDpTxAbInfo = (uint16_t *)DPRAM_HOST_ADD(DP_TX_AB_INFO);
    char ReadBuffer[16];
    uint16_t val = 0,i;
    *pDpTxAbInfo = 0;
    for (i=0;i<Param_RACH_Type;i++)
    {
        s32 ret;
        fprintf(stderr, "Read data!!!\n");
        ret = fscanf(pInputDataFilePointer,"%s",ReadBuffer);
        if (ret == 0)
            fprintf(stderr, "No more data!!!\n");
        val = (short)atoi(ReadBuffer);
        val <<=i;
        *pDpTxAbInfo |= val;
    }
}

void initBuffers(void)
{
    uint16_t *pSrBuffCtrl = (uint16_t *)SHARED_REG_HOST_ADD(SR_BUFFER_CONTROL);
    uint16_t *pSrTxFmtBurst0 = (uint16_t*)SHARED_REG_HOST_ADD(SR_TX_FMT_BURST0);
    uint16_t *pDpTxFmtRampSymb = (uint16_t*)DPRAM_HOST_ADD(DP_TX_FMT_RAMP_SYMB);
    uint16_t *pDpTxAbTsc =(uint16_t *)DPRAM_HOST_ADD(DP_TX_AB_TSC);
    uint16_t *pDpTxAbBsic = (uint16_t *)DPRAM_HOST_ADD(DP_TX_AB_BSIC);
//    *(int*)(ramHost + 0x100073) = 0xABCDABCD;
//    *(char*)(ramHost + 0x10a473) = 0x2;
//    *(int*)(ramHost) = 0xABCDABCD;
//    //*(int*)(dpramHost) = 0xABCDABCD;
//    uint16_t test = (uint16_t)( intptr_t)pSrBuffCtrl;
//    uint16_t test2 = (uint16_t)( intptr_t)(SR_BUFFER_CONTROL);
//    //uint16_t test3 = (uint16_t)( intptr_t)(sharedRegHost);
//    uint16_t test3 = (uint16_t)( intptr_t)(ramHost);
//    uint16_t test4 = (uint16_t)( intptr_t)(SHARED_REG_BASE);
//    fprintf(stderr, "SR_BUFFER_CONTROL 0x%x test2 0x%x test3 0x%x test4 0x%x\n", test, test2, test3, test4);
//    test = (uint16_t)( intptr_t)pDpTxAbTsc;
//    test2 = (uint16_t)( intptr_t)(DP_TX_AB_TSC);
//    //uint16_t test3 = (uint16_t)( intptr_t)(sharedRegHost);
//    test3 = (uint16_t)( intptr_t)(ramHost);
//    test4 = (uint16_t)( intptr_t)(DPRAM_BASE);
//    fprintf(stderr, "DP_TX_AB_TSC 0x%x test2 0x%x test3 0x%x test4 0x%x\n", test, test2, test3, test4);
    *pSrBuffCtrl = 1;
    *pDpTxAbTsc = 0x23;
    *pDpTxAbBsic = 0x23;
    *pSrTxFmtBurst0 = 0x0808;
    *pDpTxFmtRampSymb = 0x0f02;
    fprintf(stderr, "*pDpTxAbTsc 0x%x\n", *pDpTxAbTsc);
}

void testRach(int loopCnt)
{
    testLoopCnt = loopCnt;
    pInputDataFilePointer = fopen("input.txt","r");
	if (pInputDataFilePointer == NULL)
    {
		fprintf(stderr, "ERROR: no input file!!!!\n");
        exit(1);
    }
    pOutputDataFilePointer = fopen("output.txt","w");
    initBuffers();
}

void testRachEnd(void)
{
    fprintf(stderr, "End of the test. Close qemu!");
    fflush( stdout ); fflush( stderr );
    fclose(pInputDataFilePointer);
    fclose(pOutputDataFilePointer);
    exit(0);
}

static void mips_altair_machine_init(MachineClass *mc)
{
    mc->desc = "Altair machine";
    mc->init = mips_altair_init;
}

DEFINE_MACHINE("mipsaltair", mips_altair_machine_init)
