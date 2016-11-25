/*
 * ARM mach-msm 9x28 emulation
 *
 */

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

//extern void msm_hsl_init(MemoryRegion *address_space,hwaddr base,qemu_irq irq, int baudbase,CharDriverState *chr);
extern void msm_hs_init(MemoryRegion *address_space,hwaddr base,qemu_irq irq, int baudbase,CharDriverState *chr);
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
#define MSM_NAND_BASE       0x1B400000
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

/* Number of external interrupt lines to configure the GIC with */
#define NUM_IRQS 512

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

qemu_irq pic_9x28[NUM_IRQS];

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
    vUART0,
    vUART1,
    vTIMER,
    vTLMM,
    vRPM,
    vDMOV,
};

static const MemMapEntry a15memmap[] =
{
    /* GIC distributor and CPU interfaces sit inside the CPU peripheral space */
    [vGIC_DIST] =   { 0x0b000000, 0x00001000 }, // see msm_iomap-9640.h
    [vGIC_CPU] =    { 0x0b002000, 0x00001000 }, // see msm_iomap-9640.h
    [vMEM_RAM1] =   { 0x80000000, 0x10000000 }, // See device-9640.c (256M bits)
    [vMEM_NAND] =   { 0x90000000, 0x0c000000 }, // See device-9640.c (256M bits)
    //[vMEM_RAM1] =   { 0x80000000, 0x07c00000 },   // See device-9640.c (256M bits)
    [vMEM_RAM2] =   { 0x87c00000, 0x00400000 }, // See device-9640.c (256M bits)
    [vMEM_RAM3] =   { 0x88000000, 0x06c00000 }, // See device-9640.c (256M bits)
    [vMEM_RAM4] =   { 0x8ee00000, 0x01100000 }, // See device-9640.c (256M bits)
    [vSMD] =        { 0x02011000, 0x00001000 }, // See device-9640.c (256M bits)
    [vSMSM] =       { 0x40000000, 0x00100000 }, // See device-9640.c (256M bits)
    //[vUART0] =      { 0x078b0000, 0x00001000 }, // see msm_iomap.h // GSBI5- HSL1
    [vUART0] =      { 0x078af000, 0x00001000 }, // see msm_iomap.h // GSBI5- HSL1
    [vUART1] =      { 0x16430000, 0x00001000 }, // see device-9640.c // GSBI4 - HSL0
    [vTIMER] =      { 0x0b020000, 0x3000     },
    [vTLMM]     =   { 0x00800000, 0x00100000 },
    [vRPM]      =   { 0x00108000, 0x00001000 },
};

static const int a15irqmap[] =
{
    [vUART0]    = 107,  // see irqs-9640.h ... GSBI5_UARTDM_IRQ () - HSL1
    [vUART1]    = 152,  // see irqs-9640.h ... GSBI4_UARTDM_IRQ () - HSL0
    [vTIMER]    = 6,  // see irqs-9640.h ... GSBI5_UARTDM_IRQ ()
    [vSMD]      = 57,   // see irqs-9640.h ... GSBI5_UARTDM_IRQ ()
    [vSMSM]     = 58,   // see irqs-9640.h ... GSBI5_UARTDM_IRQ ()
    [vRPM]      = 200,
    [vDMOV]     = 171,
};

static MSM_BoardInfo machines[] =
{
    {
        .cpu_model = "cortex-a15",
        .memmap = a15memmap,
        .irqmap = a15irqmap,
    }
};

MemoryRegion tlmm_9x28;
MemoryRegion rpm_9x28;

FILE *smdfd_9x28 = NULL;

typedef struct msm9x28_smd_s {
    MemoryRegion iomem;
    qemu_irq irq;
    uint8_t *ramptr;
    FILE    *fd;
} msm_smd;

struct smd_half_channel {
    // From linux/arch/arm/mach-msm/smd_private.h
    unsigned state;
    unsigned char fDSR;
    unsigned char fCTS;
    unsigned char fCD;
    unsigned char fRI;
    unsigned char fHEAD;
    unsigned char fTAIL;
    unsigned char fSTATE;
    unsigned char fBLOCKREADINTR;
    unsigned tail;
    unsigned head;
};

struct smem_heap_info {
    unsigned initialized;
    unsigned free_offset;
    unsigned heap_remaining;
    unsigned reserved;
};

struct smem_heap_entry {
    unsigned allocated;
    unsigned offset;
    unsigned size;
    unsigned reserved; /* bits 1:0 reserved, bits 31:2 aux smem base addr */
};

struct smem_shared {
    unsigned proc_comm[16];
    unsigned version[32];
    struct smem_heap_info heap_info;
#define SMD_HEAP_SIZE 512
    struct smem_heap_entry heap_toc[SMD_HEAP_SIZE];
};

#define SMEM_NUM_SMD_STREAM_CHANNELS        64
#define SMEM_NUM_SMD_BLOCK_CHANNELS         64

enum {
        /* fixed items */
        SMEM_PROC_COMM = 0,
        SMEM_HEAP_INFO,
        SMEM_ALLOCATION_TABLE,
        SMEM_VERSION_INFO,
        SMEM_HW_RESET_DETECT,
        SMEM_AARM_WARM_BOOT,
        SMEM_DIAG_ERR_MESSAGE,
        SMEM_SPINLOCK_ARRAY,
        SMEM_MEMORY_BARRIER_LOCATION,
        SMEM_FIXED_ITEM_LAST = SMEM_MEMORY_BARRIER_LOCATION,

        /* dynamic items */
        SMEM_AARM_PARTITION_TABLE,
        SMEM_AARM_BAD_BLOCK_TABLE,
        SMEM_RESERVE_BAD_BLOCKS,
        SMEM_WM_UUID,
        SMEM_CHANNEL_ALLOC_TBL,
        SMEM_SMD_BASE_ID,
        SMEM_SMEM_LOG_IDX = SMEM_SMD_BASE_ID + SMEM_NUM_SMD_STREAM_CHANNELS,
        SMEM_SMEM_LOG_EVENTS,
        SMEM_SMEM_STATIC_LOG_IDX,
        SMEM_SMEM_STATIC_LOG_EVENTS,
        SMEM_SMEM_SLOW_CLOCK_SYNC,
        SMEM_SMEM_SLOW_CLOCK_VALUE,
        SMEM_BIO_LED_BUF,
        SMEM_SMSM_SHARED_STATE,
        SMEM_SMSM_INT_INFO,
        SMEM_SMSM_SLEEP_DELAY,
        SMEM_SMSM_LIMIT_SLEEP,
        SMEM_SLEEP_POWER_COLLAPSE_DISABLED,
        SMEM_KEYPAD_KEYS_PRESSED,
        SMEM_KEYPAD_STATE_UPDATED,
        SMEM_KEYPAD_STATE_IDX,
        SMEM_GPIO_INT,
        SMEM_MDDI_LCD_IDX,
        SMEM_MDDI_HOST_DRIVER_STATE,
        SMEM_MDDI_LCD_DISP_STATE,
        SMEM_LCD_CUR_PANEL,
        SMEM_MARM_BOOT_SEGMENT_INFO,
        SMEM_AARM_BOOT_SEGMENT_INFO,
        SMEM_SLEEP_STATIC,
        SMEM_SCORPION_FREQUENCY,
        SMEM_SMD_PROFILES,
        SMEM_TSSC_BUSY,
        SMEM_HS_SUSPEND_FILTER_INFO,
        SMEM_BATT_INFO,
        SMEM_APPS_BOOT_MODE,
        SMEM_VERSION_FIRST,
        SMEM_VERSION_SMD = SMEM_VERSION_FIRST,
        SMEM_VERSION_LAST = SMEM_VERSION_FIRST + 24,
        SMEM_OSS_RRCASN1_BUF1,
        SMEM_OSS_RRCASN1_BUF2,
        SMEM_ID_VENDOR0,
        SMEM_ID_VENDOR1,
        SMEM_ID_VENDOR2,
        SMEM_HW_SW_BUILD_ID,
        SMEM_SMD_BLOCK_PORT_BASE_ID,
        SMEM_SMD_BLOCK_PORT_PROC0_HEAP = SMEM_SMD_BLOCK_PORT_BASE_ID +
                                                SMEM_NUM_SMD_BLOCK_CHANNELS,
        SMEM_SMD_BLOCK_PORT_PROC1_HEAP = SMEM_SMD_BLOCK_PORT_PROC0_HEAP +
                                                SMEM_NUM_SMD_BLOCK_CHANNELS,
        SMEM_I2C_MUTEX = SMEM_SMD_BLOCK_PORT_PROC1_HEAP +
                                                SMEM_NUM_SMD_BLOCK_CHANNELS,
        SMEM_SCLK_CONVERSION,
        SMEM_SMD_SMSM_INTR_MUX,
        SMEM_SMSM_CPU_INTR_MASK,
        SMEM_APPS_DEM_SLAVE_DATA,
        SMEM_QDSP6_DEM_SLAVE_DATA,
        SMEM_CLKREGIM_BSP,
        SMEM_CLKREGIM_SOURCES,
        SMEM_SMD_FIFO_BASE_ID,
        SMEM_USABLE_RAM_PARTITION_TABLE = SMEM_SMD_FIFO_BASE_ID +
                                                SMEM_NUM_SMD_STREAM_CHANNELS,
        SMEM_POWER_ON_STATUS_INFO,
        SMEM_DAL_AREA,
        SMEM_SMEM_LOG_POWER_IDX,
        SMEM_SMEM_LOG_POWER_WRAP,
        SMEM_SMEM_LOG_POWER_EVENTS,
        SMEM_ERR_CRASH_LOG,
        SMEM_ERR_F3_TRACE_LOG,
        SMEM_SMD_BRIDGE_ALLOC_TABLE,
        SMEM_SMDLITE_TABLE,
        SMEM_SD_IMG_UPGRADE_STATUS,
        SMEM_SEFS_INFO,
        SMEM_RESET_LOG,
        SMEM_RESET_LOG_SYMBOLS,
        SMEM_MODEM_SW_BUILD_ID,
        SMEM_SMEM_LOG_MPROC_WRAP,
        SMEM_BOOT_INFO_FOR_APPS,
        SMEM_SMSM_SIZE_INFO,
        SMEM_SMD_LOOPBACK_REGISTER,
        SMEM_SSR_REASON_MSS0,
        SMEM_SSR_REASON_WCNSS0,
        SMEM_SSR_REASON_LPASS0,
        SMEM_SSR_REASON_DSPS0,
        SMEM_SSR_REASON_VCODEC0,
        SMEM_MEM_LAST = SMEM_SSR_REASON_VCODEC0,
        SMEM_NUM_ITEMS,
};


#define SMD_BUF_SIZE             8192
#define SMD_CHANNELS             64
#define SMD_HEADER_SIZE          20

/* 'type' field of smd_alloc_elm structure
 * has the following breakup
 * bits 0-7   -> channel type
 * bits 8-11  -> xfer type
 * bits 12-31 -> reserved
 */
struct smd_alloc_elm {
    char name[20];
    uint32_t cid;
    uint32_t type;
    uint32_t ref_count;
};

struct smd_channel {
    char name[20];
    uint32_t cid;
    uint32_t offset;
    uint32_t size;
};

typedef struct msm9x28_smsm_s {
    MemoryRegion iomem;
    qemu_irq irq;
    uint8_t *ramptr;
    FILE    *fd;
    struct smem_shared *shared;
    struct smem_heap_info *heapinfo;
    struct smem_heap_entry *heapentry;
    struct smd_half_channel *half_ch[64];
    uint8_t *fifo_ch[64];
    unsigned fifo_size[64];
    unsigned off[64];
    uint8_t ptr[64][8192];
    uint8_t state[64];
} msm_smsm;

struct msm9x28_smsm_s *smsm_9x28 = NULL;

// Channel 26 -> DATA4 -> /dev/smd8
static char bufat[26];
static int bufatoff = 0;

#if 0
static uint8_t at[63] = {
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'A',
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'T',
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0D,
};
static uint8_t ok[26] = {
  6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0D, 0x0A, 'O', 'K', 0x0D, 0x0A,
};
#endif

/******************************************
* SMD init.
******************************************/

static void msm9x28_write_channel( int ch, const char *buffer, int len )
{
    struct smd_half_channel *half_ch = &(smsm_9x28->half_ch[ch][1]);
    int flen, wrlen;
    wrlen = 20;
    do {
      if( half_ch->head >= half_ch->tail ) {
        if( (half_ch->head + wrlen) <= smsm_9x28->fifo_size[ch] ) {
          if( len > 0 ) {
            memset( &(smsm_9x28->fifo_ch[ch][smsm_9x28->fifo_size[ch] + half_ch->head]),0,wrlen);
            *(unsigned *)(&smsm_9x28->fifo_ch[ch][smsm_9x28->fifo_size[ch] + half_ch->head]) = len;
          }
          else {
            memcpy( &(smsm_9x28->fifo_ch[ch][smsm_9x28->fifo_size[ch] + half_ch->head]),buffer,wrlen );
          }
          half_ch->head += wrlen;
          half_ch->head %= smsm_9x28->fifo_size[ch];
        }
        else {
          flen = wrlen - (smsm_9x28->fifo_size[ch] - half_ch->head);
          if( len > 0 ) {
            memset( &(smsm_9x28->fifo_ch[ch][smsm_9x28->fifo_size[ch] + half_ch->head]),0,flen);
            smsm_9x28->fifo_ch[ch][smsm_9x28->fifo_size[ch] + half_ch->head] = len;
            flen = wrlen - flen;
            memset( &(smsm_9x28->fifo_ch[ch][smsm_9x28->fifo_size[ch]]),0,flen);
          }
          else {
            memcpy( &(smsm_9x28->fifo_ch[ch][smsm_9x28->fifo_size[ch] + half_ch->head]),buffer,flen );
            flen = wrlen - flen;
            memcpy( &(smsm_9x28->fifo_ch[ch][smsm_9x28->fifo_size[ch]]),buffer,flen );
          }
          half_ch->head = flen;
        }
      }
      else {
        flen = half_ch->tail - (half_ch->head + wrlen);
        if( flen < 0 )
          return;
        if( len > 0 ) {
          memset( &(smsm_9x28->fifo_ch[ch][half_ch->head]),0,wrlen);
          smsm_9x28->fifo_ch[ch][smsm_9x28->fifo_size[ch] + half_ch->head] = len;
        }
        else {
          memcpy( &(smsm_9x28->fifo_ch[ch][smsm_9x28->fifo_size[ch] + half_ch->head]),buffer,wrlen );
        }
        half_ch->head += wrlen;
      }
      wrlen = len;
      len = -1;
    }
    while( wrlen != -1 );
    smsm_9x28->half_ch[ch]->fTAIL = smsm_9x28->half_ch[ch]->fHEAD = 0;
    half_ch->fHEAD = 1;

}

static uint64_t msm9x28_smd_read(void *opaque, hwaddr addr, unsigned size)
{
//[ 2923.405279]  +000 : 00000007 00000000 00000000 00000000
//[ 2923.410437]  +010 : 0000004F 00008100 00000708 00000000
//[ 2923.416175]  +020 : 00000000 00000000 00000000 00000000
//[ 2923.420875]  +030 : 00000000 00000000 00000000 00000000

    hwaddr ioaddr = addr & 0x3C;
    uint64_t ioval;

    switch( ioaddr ) {
      case 0x00:   ioval = 0x00000007; break;
      case 0x04:   ioval = 0x00000000; break;
      case 0x08:   ioval = 0x00000000; break;
      case 0x0C:   ioval = 0x00000000; break;
      case 0x10:   ioval = 0x0000004F; break;
      case 0x14:   ioval = 0x00008100; break;
      case 0x18:   ioval = 0x00000708; break;
      case 0x1C:   ioval = 0x00000000; break;
      default:    ioval = 0x00000000; break;
    }

    //fprintf( stderr, "msm9x28_smd_read: ioaddr %08X, ioval %08llX", ioaddr, ioval );
    return ioval;
}
static void msm9x28_smd_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
{
    msm_smd *s = (msm_smd *)opaque;
    unsigned ioaddr = (unsigned)addr & 0x3C;
    unsigned ioval = 0;
    int i;
    fprintf( s->fd, "SMD %08X,%04X :", (unsigned)addr, size );
    switch( size ) {
      case 4:    ioval |= ((value >> 24) & 0xFF );
      case 3:    ioval |= ((value >> 16) & 0xFF );
      case 2:    ioval |= ((value >> 8) & 0xFF );
      case 1:    ioval |= ((value) & 0xFF );
    }
    fprintf( s->fd, " = %08X\n", ioval );
    fflush( s->fd );
    switch( ioaddr ) {
      case 0x00:   break;
      case 0x04:   break;
      case 0x08:   if( (1 << 3) & ioval ) {
                     for( i = 0; i < 64; i++ ) {
                       if( smsm_9x28->half_ch[i] && smsm_9x28->half_ch[i]->state != smsm_9x28->state[i] ) {
                         struct smd_half_channel *half_ch = &(smsm_9x28->half_ch[i][1]);
                         if( smsm_9x28->half_ch[i]->state == 1 ) {
                           half_ch->state = 2;
                           half_ch->fDSR = 1;
                           half_ch->fCTS = 1;
                           half_ch->fCD = 1;
                           half_ch->fRI = 0;
                           half_ch->fHEAD = 0;
                           half_ch->fTAIL = 0;
                           half_ch->fSTATE = 1;
                           half_ch->fBLOCKREADINTR = 0;
                           half_ch->tail = 0;
                           half_ch->head = 0;
                           smsm_9x28->half_ch[i]->tail = 0;
                           if( i == 26 ) {
                             bufatoff = 0;
                             memset( bufat, 0, sizeof(bufat) );
                           }
                           smsm_9x28->state[i] = 1;
                         }
                         else if( smsm_9x28->half_ch[i]->state == 2 ) {
                           half_ch->state = 2;
                           half_ch->fDSR = 0;
                           half_ch->fCTS = 0;
                           half_ch->fCD = 0;
                           half_ch->fRI = 0;
                           half_ch->fHEAD = 0;
                           half_ch->fTAIL = 0;
                           half_ch->fSTATE = 1;
                           half_ch->fBLOCKREADINTR = 0;
                           half_ch->tail = 0;
                           half_ch->head = 0;
                           smsm_9x28->half_ch[i]->tail = 0;
                           if( i == 26 ) {
                             bufatoff = 0;
                             memset( bufat, 0, sizeof(bufat) );
                           }
                           smsm_9x28->state[i] = 2;
                         }
                         else if( smsm_9x28->half_ch[i]->state == 0 ) {
                           struct smd_half_channel *half_ch = &(smsm_9x28->half_ch[i][1]);
                           half_ch->state = 0;
                           half_ch->fDSR = 0;
                           half_ch->fCTS = 0;
                           half_ch->fCD = 0;
                           half_ch->fRI = 0;
                           half_ch->fHEAD = 0;
                           half_ch->fTAIL = 0;
                           half_ch->fSTATE = 1;
                           half_ch->fBLOCKREADINTR = 0;
                           half_ch->tail = 0;
                           half_ch->head = 0;
                           smsm_9x28->half_ch[i]->tail = 0;
                           if( i == 26 ) {
                             bufatoff = 0;
                             memset( bufat, 0, sizeof(bufat) );
                           }
                           smsm_9x28->state[i] = 0;
                         }
                       }
                       else if( smsm_9x28->half_ch[i] && (smsm_9x28->half_ch[i]->tail != smsm_9x28->half_ch[i]->head) ) {
                         if( smsm_9x28->half_ch[i]->state == 2 && (smsm_9x28->half_ch[i]->fTAIL || smsm_9x28->half_ch[i]->fHEAD) ) {
                           //struct smd_half_channel *half_ch = &(smsm_9x28->half_ch[i][1]);
                           if( smsm_9x28->half_ch[i]->tail <= smsm_9x28->half_ch[i]->head ) {
                             memcpy(&smsm_9x28->ptr[i][smsm_9x28->off[i]],
                                    &(smsm_9x28->fifo_ch[i][smsm_9x28->half_ch[i]->tail]),
                                    smsm_9x28->half_ch[i]->head-smsm_9x28->half_ch[i]->tail);
                             smsm_9x28->off[i] += smsm_9x28->half_ch[i]->head-smsm_9x28->half_ch[i]->tail;
                           }
                           else {
                             memcpy(&smsm_9x28->ptr[i][smsm_9x28->off[i]],
                                    &(smsm_9x28->fifo_ch[i][smsm_9x28->half_ch[i]->tail]),
                                    (smsm_9x28->fifo_size[i])-smsm_9x28->half_ch[i]->tail);
                             smsm_9x28->off[i] += smsm_9x28->fifo_size[i]-smsm_9x28->half_ch[i]->tail;
                             memcpy(&smsm_9x28->ptr[i][smsm_9x28->off[i]],
                                    &(smsm_9x28->fifo_ch[i][0]),
                                    smsm_9x28->half_ch[i]->head);
                             smsm_9x28->off[i] += smsm_9x28->half_ch[i]->head;
                           }
                           smsm_9x28->half_ch[i]->tail = smsm_9x28->half_ch[i]->head;
                           if( i == 26 ) {
                             if( smsm_9x28->off[i] > 20 ) {
                               memcpy( &bufat[bufatoff], &smsm_9x28->ptr[i][20], smsm_9x28->off[i] - 20 );
                               bufatoff += smsm_9x28->off[i] - 20;
                               bufat[bufatoff] = 0;
                               smsm_9x28->off[i] = 0;
                               if( bufat[bufatoff - 1] == '\r' ) {
                                 if( strcasecmp( bufat, "AT\r" ) == 0 ) {
                                   msm9x28_write_channel( i, "\n\rOK\n\r", 6 );
                                 }
                                 else if( strcasecmp( bufat, "ATI9\r" ) == 0 ) {
                                   int ati;
                                   const char *ati9[] = {
                                     "\n\r",
                                     "Manufacturer: Sierra Wireless, Incorporated\n\r",
                                     "Model: AR8652\n\r",
                                     "Revision: SWI9X15A_06.03.50.00 r27935 CNSHZ-AR-BUILD 2015/04/24 08:33:52\n\r",
                                     "IMEI: 358464060000377\n\r",
                                     "IMEI SV: 40\n\r",
                                     "FSN: L5451201080203\n\r",
                                     "+GCAP: +CGSM,+FCLASS,+DS\n\r",
                                     "\n\r",
                                     "OK\n\r",
                                   };
                                   for( ati = 0; ati < 10; ati++ ) {
                                     msm9x28_write_channel( i, ati9[ati], strlen(ati9[ati]) );
                                   }
                                 }
                                 else {
                                   msm9x28_write_channel( i, "\n\rERROR\n\r", 9 );
                                 }
                                 bufatoff = smsm_9x28->off[i] = 0;
                               }
                             }
                           }
                         }
                       }
                     }
                     qemu_irq_pulse(s->irq);
                   }
                   break;
      case 0x0C:   break;
      case 0x10:   break;
      case 0x14:   break;
      case 0x18:   break;
      case 0x1C:   break;
      default:     break;
    }
    fflush( s->fd );
}

static const MemoryRegionOps msm9x28_smd_ops = {
    .read = msm9x28_smd_read,
    .write = msm9x28_smd_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void msm9x28_smd_init(const MSM_BoardInfo *b,MemoryRegion *memory,qemu_irq *pic_9x28)
{
hwaddr base = b->memmap[vSMD].base;
int irq = b->irqmap[vSMD];
msm_smd *s = (msm_smd *)g_malloc0( sizeof( msm_smd ) );

    memory_region_init_io(&s->iomem, NULL, &msm9x28_smd_ops, s, "smd", b->memmap[vSMD].size);
    memory_region_add_subregion(memory, base, &s->iomem);
    s->fd = smdfd_9x28;
    s->irq = pic_9x28[irq];
}

// DATA4
// [  190.983488] set_state: FA38CA70 = 00000001
// [  190.983549] notify_modem_smd(2): 0
// [  190.984037] SMD Int Modem->Apps
// [  190.984098] get_state: FA38CA84 = 00000001

static uint64_t msm9x28_smsm_read(void *opaque, hwaddr addr, unsigned size)
{
    msm_smsm *s = (msm_smsm *)opaque;
    uint64_t ioval = 0;
    //unsigned offset = (unsigned)addr - 0x40000000;
    unsigned offset = (unsigned)addr;

//fprintf( stderr, "msm9x28_smsm_read: %08X %u\n", (unsigned) addr, size );
    switch( size ) {
      case 4:    ioval |= ((s->ramptr[offset + 3]) << 24);
      case 3:    ioval |= ((s->ramptr[offset + 2]) << 16);
      case 2:    ioval |= ((s->ramptr[offset + 1]) << 8);
      case 1:    ioval |= (s->ramptr[offset]);
    }
    return ioval;
}

static void msm9x28_smsm_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
{
    msm_smsm *s = (msm_smsm *)opaque;
    unsigned offset = (unsigned)addr;
    fprintf( s->fd, "SMSM %08X,%04X :", (unsigned)addr, size );
    switch( size ) {
      case 4:    s->ramptr[offset + 3] = ((value >> 24) & 0xFF );
                 fprintf( s->fd, " %02hhX", s->ramptr[offset + 3] );
      case 3:    s->ramptr[offset + 2] = ((value >> 16) & 0xFF );
                 fprintf( s->fd, " %02hhX", s->ramptr[offset + 2] );
      case 2:    s->ramptr[offset + 1] = ((value >> 8) & 0xFF );
                 fprintf( s->fd, " %02hhX", s->ramptr[offset + 1] );
      case 1:    s->ramptr[offset] = ((value) & 0xFF );
                 fprintf( s->fd, " %02hhX", s->ramptr[offset] );
    }
    fputc( '\n', s->fd );
    fflush( s->fd );
}

static const MemoryRegionOps msm9x28_smsm_ops = {
    .read = msm9x28_smsm_read,
    .write = msm9x28_smsm_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void msm9x28_smsm_init(const MSM_BoardInfo *b,MemoryRegion *memory,qemu_irq *pic_9x28)
{
hwaddr base = b->memmap[vSMSM].base;
int irq = b->irqmap[vSMSM];
msm_smsm *s = (msm_smsm *)g_malloc0( sizeof( msm_smsm ) );
int fd;
int i;

    memory_region_init_io(&s->iomem, NULL, &msm9x28_smsm_ops, s, "smsm", b->memmap[vSMSM].size);
    memory_region_add_subregion(memory, base, &s->iomem);

    s->ramptr = (uint8_t*)g_malloc0( b->memmap[vSMSM].size );
    memset( &s->ramptr[0x0], 0xDA, 0x00100000 );
    if( 0 <= (fd = open( "sierra_SMD", O_RDONLY )) ) {
        int n;
        printf( "Reading sierra_smem ... " );
        n = read( fd, &s->ramptr[0x0], 0x00100000 );
        printf( "%02X %02X %02X %02X\n",
                s->ramptr[0xf5000], s->ramptr[0xf5001], s->ramptr[0xf5002], s->ramptr[0xf5003] );
        close( fd );
        if( n < 0 )
          return;
    }
    //s->fd = fopen( "smsm.txt", "w" );
    s->fd = smdfd_9x28;
    s->irq = pic_9x28[irq];
    smsm_9x28 = s;
    memset( s->half_ch, 0, sizeof( s->half_ch ) );
    memset( s->fifo_ch, 0, sizeof( s->fifo_ch ) );
    memset( s->fifo_size, 0, sizeof( s->fifo_size ) );
    memset( s->off, 0, sizeof( s->off ) );
    memset( s->ptr, 0, sizeof( s->ptr ) );
    memset( s->state, 0, sizeof( s->state ) );
    s->shared = (struct smem_shared *)s->ramptr;
    s->heapinfo = (struct smem_heap_info *)&(s->shared->heap_info);
    s->heapentry = (struct smem_heap_entry *)&(s->shared->heap_toc);
    //printf( "sramptr %08X, heapinfo %08X, heapentry %08X\n",
            //(unsigned)s->shared,(unsigned)s->heapinfo,(unsigned)s->heapentry );
    for( i = 0; i < 512; i++ ) {
      if( s->heapentry[i].allocated ) {
        printf( "  Entry %3d: o:%08X s:%u\n", i, s->heapentry[i].offset, s->heapentry[i].size );
      }
    }

    struct smd_alloc_elm *ch = (struct smd_alloc_elm *)&(s->ramptr[s->heapentry[13].offset]);
    for( i = 0; i < 64; i++ ) {
      if( ch[i].ref_count && 0 == (ch[i].type & 0xFF) ) {
        printf( "%-20s cid:%3u type:%08X ref_count:%8X\n", ch[i].name, ch[i].cid, ch[i].type, ch[i].ref_count );
        printf( "Entry %u: offset:%08X size=%u\n",
                ch[i].cid,s->heapentry[ch[i].cid+SMEM_SMD_BASE_ID].offset,s->heapentry[ch[i].cid+SMEM_SMD_BASE_ID].size);
        printf( "Entry %u: offset:%08X size=%u\n",
                ch[i].cid,s->heapentry[ch[i].cid+SMEM_SMD_FIFO_BASE_ID].offset,s->heapentry[ch[i].cid+SMEM_SMD_FIFO_BASE_ID].size);
        s->half_ch[ch[i].cid] = (struct smd_half_channel *)&(s->ramptr[s->heapentry[ch[i].cid+SMEM_SMD_BASE_ID].offset]);
        s->half_ch[ch[i].cid]->state = 0;
        s->half_ch[ch[i].cid][1].state = 0;
        s->fifo_ch[ch[i].cid] = (uint8_t *)&(s->ramptr[s->heapentry[ch[i].cid+SMEM_SMD_FIFO_BASE_ID].offset]);
        s->fifo_size[ch[i].cid] = (s->heapentry[ch[i].cid+SMEM_SMD_FIFO_BASE_ID].size) / 2;
      }
    }
}

//TLMM

static uint64_t msm9x28_tlmm_9x28_read(void *opaque, hwaddr addr, unsigned size)
{
    //msm_smsm *s = (msm_smsm *)opaque;
    uint64_t ioval = 0;
    //unsigned offset = (unsigned)addr;
    (void)addr; (void)opaque;
    switch( size ) {
      case 4:    ioval |= (0 << 24);
      case 3:    ioval |= (0 << 16);
      case 2:    ioval |= (0 << 8);
      case 1:    ioval |= (0);
    }
    return ioval;
}

static void msm9x28_tlmm_9x28_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
{
    //unsigned offset = (unsigned)addr;
    //fprintf( stderr, "TLMM %08X,%04X = %08X :", (unsigned)addr, size, value );
    if( addr == 0x820 && value == 0 && size == 4 ) {
      fprintf( stderr, "\nQEMU/MSM9X15 POWER OFF\n\n" );
      exit( 0 );
    }
    else
      fprintf( stderr, "\n" );
}

static const MemoryRegionOps msm9x28_tlmm_9x28_ops = {
    .read = msm9x28_tlmm_9x28_read,
    .write = msm9x28_tlmm_9x28_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void msm9x28_tlmm_9x28_init(const MSM_BoardInfo *b,MemoryRegion *memory)
{
    hwaddr base = b->memmap[vTLMM].base;

    memory_region_init_io(&tlmm_9x28, NULL, &msm9x28_tlmm_9x28_ops, NULL, "tlmm_9x28", b->memmap[vTLMM].size);
    memory_region_add_subregion(memory, base, &tlmm_9x28);
}

//RPM

static uint64_t msm9x28_rpm_9x28_read(void *opaque, hwaddr addr, unsigned size)
{
    //msm_smsm *s = (msm_smsm *)opaque;
    uint64_t ioval = 0;
    uint32_t val = 0;
    //unsigned offset = (unsigned)addr;
    (void)opaque;(void)addr;

    switch( (unsigned)(addr) ) {
      // RPM firmware 3.0.16842958
      case 0: val = 3; break;
      case 4: val = 0; break;
      case 8: val = 16842958; break;
    }
    switch( size ) {
      case 4:    ioval = val; break;
      case 3:    ioval = val & 0xFFFFFF; break;
      case 2:    ioval = val & 0xFFFF; break;
      case 1:    ioval = val & 0xFF; break;
    }
    //fprintf( stderr, "RPM read %08X,%04X %-> %08llX\n", (unsigned)addr, size, ioval );
    return ioval;
}

static void msm9x28_rpm_9x28_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
{
    //unsigned offset = (unsigned)addr;
    //fprintf( stderr, "RPM write %08X,%04X = %08X :", (unsigned)addr, size, value );
    if( addr == 0x820 && value == 0 && size == 4 ) {
      fprintf( stderr, "\nQEMU/MSM9X15 POWER OFF\n\n" );
    }
    else
      fprintf( stderr, "\n" );
}

static const MemoryRegionOps msm9x28_rpm_9x28_ops = {
    .read = msm9x28_rpm_9x28_read,
    .write = msm9x28_rpm_9x28_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void msm9x28_rpm_9x28_init(const MSM_BoardInfo *b,MemoryRegion *memory)
{
    hwaddr base = b->memmap[vRPM].base;

    memory_region_init_io(&rpm_9x28, NULL, &msm9x28_rpm_9x28_ops, NULL, "rpm_9x28", b->memmap[vRPM].size);
    memory_region_add_subregion(memory, base, &rpm_9x28);
}

/******************************************
* arch arm timer
******************************************/
// Qemu virtal timer 1GHz / MDM9x timer 19.2 MHz
//#define TIMER_SCALE (1000000000/19200000)
#define TIMER_SCALE 100


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

msm_clock msm9x28_clk_list[] =
{
    [0] =
          {
         .name  = "msm9x28-timer",
         .freq  = 19200000,
         .freq  = 32768,
         .shift = 32,
         .rate  = 300,
      }
};

/* msm9x28 OS timers */
typedef struct msm9x28_timer_s {
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
}msm9x28_timer_s;

/* 32-kHz timer */
struct msm9x28_32khz_timer_s {
    msm9x28_timer_s     timer;
    MemoryRegion    iomem;
};

/******************************************
 * ARM generic timer emulation.
******************************************/

static inline void msm9x28_timer_sync(msm9x28_timer_s *timer)
{
    timer->time = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL)/TIMER_SCALE;
}

static inline uint64_t msm9x28_timer_read(msm9x28_timer_s *timer)
{
    msm9x28_timer_sync(timer);
    fprintf(stderr, "Timer read %lld\n", (long long int)timer->time);
    return timer->time;
}

static inline void msm9x28_timer_update(msm9x28_timer_s *timer)
{
int64_t expires;

    fprintf( stderr, "ctl=%lld\n", (long long int)timer->ctl);
    if (timer->ctl & ARCH_TIMER_CTRL_ENABLE){

        // if tval !=0, it means that a timer must be started
        if (timer->tval != 0)
        {
            msm9x28_timer_sync(timer);
            fprintf( stderr, "Tval=%lld\n", (long long int)timer->tval);
            expires = timer->time + timer->tval;
            fprintf(stderr,"Current time %lld, qemu time %lld\n",(long long int)timer->time, (long long int)timer->time*TIMER_SCALE);
            fprintf(stderr,"Expected Timer %lld, qemu time %lld\n",(long long int)expires, (long long int)expires*TIMER_SCALE);
            timer_mod(timer->timer, expires*TIMER_SCALE);
        }
    } else
    {
        fprintf(stderr, "Nothing to update\n");
        timer_del(timer->timer);
        timer->ctl &= ~ARCH_TIMER_CTRL_IT_MASK;
        qemu_irq_lower(timer->irq);
    }
}


static void msm9x28_timer_hdl_write(void *opaque, hwaddr addr,uint64_t value, unsigned size)
{
struct msm9x28_timer_s *timer = (msm9x28_timer_s*) opaque;


fprintf(stderr,"****** msm9x28_timer_hdl_write:register address (1) %08X, %llu %u\n", (unsigned)addr,(unsigned long long)value, size);
    switch (addr)
    {
#if 0
    /*Clock MATCH Value register*/
    case TIMER_MATCH_VAL:
        msm9x28_timer_sync(s);
        timer->reset_val = value-timer->val;
        msm9x28_timer_update(s);
        break;
#endif
    /*Clock ctl register*/
    case TIMER_VCNT_CTL:
        fprintf(stderr, "TIMER_VCNT_CTL %lld\n", (long long int)value);
        //msm9x28_timer_sync(s);
        timer->ctl = value;
        if ((value & ARCH_TIMER_CTRL_ENABLE) == 0)
        {
            fprintf(stderr, "Init IRQ\n");
            // If enable is changed, no more IRQ: change IT status and IRQ level
            timer->ctl &= ~ARCH_TIMER_CTRL_IT_STAT;
            timer->ctl &= ~ARCH_TIMER_CTRL_IT_MASK;
            qemu_irq_lower(timer->irq);
        }
        msm9x28_timer_update(timer);
        break;
    case TIMER_VCNT_TVAL:
        //msm9x28_timer_sync(s);
        timer->tval = value;
        fprintf(stderr, "TIMER_VCNT_TVAL %d\n", (int)value);
        msm9x28_timer_update(timer);
        fprintf(stderr, "qemu_irq_lower\n");
        // If Tval is changed, no more IRQ: change IT status and IRQ level
        qemu_irq_lower(timer->irq); /* Edge-triggered irq */
        timer->ctl &= ~ARCH_TIMER_CTRL_IT_STAT;
        timer->ctl &= ~ARCH_TIMER_CTRL_IT_MASK;
        break;
    default:
        fprintf(stderr,"Unexpected register address (1) %08X, %llu %u\n", (uint32_t)addr, (long long unsigned int)value, size);
        break;
    }
}

static uint64_t msm9x28_timer_hdl_read(void *opaque, hwaddr addr, unsigned size)
{
struct msm9x28_timer_s *s = (msm9x28_timer_s*) opaque;

fprintf(stderr, "msm9x28_timer_hdl_read:%08X %u \n", (unsigned int)addr, size );
    switch (addr)
    {
    /* Timer Counter register*/
    case TIMER_CNTTIDR:
        fprintf(stderr, "TIMER_CNTTIDR\n");
        return (TIMER_VIRTUAL | TIMER_FRAME_IMPLEMENTED);
    case TIMER_PCNT_HI:
    case TIMER_VCNT_HI:
        fprintf(stderr, "read: %d\n", (unsigned int)((msm9x28_timer_read(s) >> 32) & 0xFFFFFFFFUL ));
        return (msm9x28_timer_read(s) >> 32) & 0xFFFFFFFFUL;
    case TIMER_PCNT_LO:
    case TIMER_VCNT_LO:
        fprintf(stderr, "read: %d\n", (unsigned int)((msm9x28_timer_read(s)) & 0xFFFFFFFFUL ));
        return msm9x28_timer_read(s) & 0xFFFFFFFFUL;
    case TIMER_VCNT_CTL:
        fprintf(stderr, "TIMER_VCNT_CTL\n");
        return( s->ctl );
    default:
        fprintf(stderr,"TIMER : Unexpected register address (%X)\n", (unsigned)addr);
    }
    return 0;
}

static const MemoryRegionOps msm9x28_tim_ops = {
    .read = msm9x28_timer_hdl_read,
    .write = msm9x28_timer_hdl_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void msm9x28_timer_interrupt(void *opaque)
{
msm9x28_timer_s *timer = opaque;

fprintf(stderr, "**** msm9x28_timer_interrupt\n");
    if ((timer->ctl & ARCH_TIMER_CTRL_ENABLE)
            && (!(timer->ctl & ARCH_TIMER_CTRL_IT_MASK)))
    {
       timer->ctl |= ARCH_TIMER_CTRL_IT_STAT;
       //timer->val += timer->tval;
       timer->tval = 0;
       //qemu_irq_pulse(timer->irq); /* Edge-triggered irq */
       fprintf(stderr, "Raise interrupt\n");
       qemu_irq_raise(timer->irq); /* Edge-triggered irq */
    }
    else
    {
        fprintf(stderr, "Irq not raise\n");
    }
}

static void msm9x28_timer_tick(void *opaque)
{
msm9x28_timer_s *timer = (msm9x28_timer_s *) opaque;

    fprintf(stderr, "msm9x28_timer_tick\n");
    //timer->time = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    //timer->val += 300;

    timer->reset_val=0;
    msm9x28_timer_interrupt(timer);

    msm9x28_timer_update(timer);
}

static void msm9x28_timer_reset(struct msm9x28_32khz_timer_s *s)
{
    timer_del(s->timer.timer);
    s->timer.ctl = 0;
    s->timer.val = 0;
    s->timer.tval = 0;
    s->timer.reset_val = ~0;
    s->timer.time = 0;
}

static void msm9x28_timer_init(const MSM_BoardInfo *b,MemoryRegion *memory,qemu_irq *pic_9x28)
{
hwaddr base = b->memmap[vTIMER].base;
int irq = b->irqmap[vTIMER];
const char* name = msm9x28_clk_list[0].name;
struct msm9x28_32khz_timer_s *s = (struct msm9x28_32khz_timer_s *)g_malloc0(sizeof(struct msm9x28_32khz_timer_s));

    s->timer.irq = pic_9x28[irq];
    s->timer.addr = base;
    s->timer.clk = &msm9x28_clk_list[0];
    s->timer.timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, msm9x28_timer_tick, &s->timer);
    s->timer.tick = qemu_bh_new(msm9x28_timer_interrupt, s);
    msm9x28_timer_reset(s);

    memory_region_init_io(&s->iomem, NULL, &msm9x28_tim_ops, s, name, b->memmap[vTIMER].size);
    memory_region_add_subregion(memory, base, &s->iomem);
}

/******************************************
* GIC init.
******************************************/

static void msm9x28_gic_init(const MSM_BoardInfo *b, qemu_irq *pic_9x28)
{
//int tmr_irq = b->irqmap[vTIMER];
//int dmov_irq = b->irqmap[vDMOV];
//int smd_irq = b->irqmap[vSMD];
//int smsm_irq = b->irqmap[vSMSM];

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
    qdev_prop_set_uint32(gicdev, "num-irq", NUM_IRQS + 32);
    qdev_init_nofail(gicdev);
    gicbusdev = SYS_BUS_DEVICE(gicdev);
    sysbus_mmio_map(gicbusdev, 0, b->memmap[vGIC_DIST].base);
    sysbus_mmio_map(gicbusdev, 1, b->memmap[vGIC_CPU].base);


    sysbus_connect_irq(gicbusdev, 0, qdev_get_gpio_in(cpudev, ARM_CPU_IRQ));
    /*Wire Virtual Timers*/
    //pic_9x28[tmr_irq] = qdev_get_gpio_in(gicdev,tmr_irq);
    //pic_9x28[dmov_irq] = qdev_get_gpio_in(gicdev,dmov_irq);

    for (i = 0; i < NUM_IRQS; i++) {
        pic_9x28[i] = qdev_get_gpio_in(gicdev, i);
    }
    //pic_9x28[tmr_irq] = qdev_get_gpio_in(gicdev,tmr_irq);
    //pic_9x28[smd_irq] = qdev_get_gpio_in(gicdev,smd_irq);
    //pic_9x28[smsm_irq] = qdev_get_gpio_in(gicdev,smsm_irq);
    //pic_9x28[155] = qdev_get_gpio_in(gicdev,155);
}

/******************************************
* UART init.
******************************************/

static void msm9x28_uart_init(const MSM_BoardInfo *b,MemoryRegion *memory, qemu_irq *pic_9x28)
{
#if 1
hwaddr base = b->memmap[vUART0].base;
int irq = b->irqmap[vUART0];

msm_hs_init(memory, base, pic_9x28[irq], 115200, serial_hds[0]);
#endif
}

/******************************************
* Board init.
* initialize MSM9x15 machine with cortex-a15 instead of cortex-a5, which is no supported yet on QEMU
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

static void machmsm_init(MachineState *machine)
{
MemoryRegion *sysmem = get_system_memory();
int n;
MemoryRegion *nand = g_new(MemoryRegion, 1);
MemoryRegion *ram1 = g_new(MemoryRegion, 1);
//MemoryRegion *ram2 = g_new(MemoryRegion, 1);
//MemoryRegion *ram3 = g_new(MemoryRegion, 1);
//MemoryRegion *ram4= g_new(MemoryRegion, 1);
//MemoryRegion *sram = g_new(MemoryRegion, 1);
const char *cpu_model = machine->cpu_model;
MSM_BoardInfo *b=NULL;
DriveInfo *dinfo;
//int fd;
//uint8_t *sramptr;


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
    //memory_region_init_ram(ram1, NULL, "mach-msm.ram1", b->memmap[vMEM_RAM1].size, &error_abort);
    memory_region_init_ram(ram1, NULL, "mach-msm.ram1", machine->ram_size, &error_abort);
    vmstate_register_ram_global(ram1);
    memory_region_add_subregion(sysmem, b->memmap[vMEM_RAM1].base, ram1);
#if 0
    memory_region_init_ram(ram2, NULL, "mach-msm.ram2", b->memmap[vMEM_RAM2].size, &error_abort);
    vmstate_register_ram_global(ram2);
    memory_region_add_subregion(sysmem, b->memmap[vMEM_RAM2].base, ram2);
    memory_region_init_ram(ram3, NULL, "mach-msm.ram3", b->memmap[vMEM_RAM3].size, &error_abort);
    vmstate_register_ram_global(ram3);
    memory_region_add_subregion(sysmem, b->memmap[vMEM_RAM3].base, ram3);
    memory_region_init_ram(ram4, NULL, "mach-msm.ram4", b->memmap[vMEM_RAM4].size, &error_abort);
    vmstate_register_ram_global(ram4);
    memory_region_add_subregion(sysmem, b->memmap[vMEM_RAM4].base, ram4);
#endif

    // Interrupt Controller creation - GIC
    msm9x28_gic_init(b,pic_9x28);

    // Uart creation
    msm9x28_uart_init(b,sysmem,pic_9x28);

    // arm_arch_timer
    msm9x28_timer_init(b,sysmem,pic_9x28);

#if 0
    smdfd_9x28 = fopen( "smsm.txt", "w" );

    msm9x28_smsm_init(b,sysmem,pic_9x28);

    msm9x28_smd_init(b,sysmem,pic_9x28);
#endif
    (void)msm9x28_smsm_init;
    (void)msm9x28_smd_init;

    msm9x28_tlmm_9x28_init(b,sysmem);
    msm9x28_rpm_9x28_init(b,sysmem);

    /* 0x4e000000 LAN9118 Ethernet */
    printf( "Init LAN9118\n" );
    //lan9118_init(&nd_table[0], 0x14000000, pic_9x28[155]);
    smc91c111_init(&nd_table[0], 0x14000000, pic_9x28[155]);

    // Boot...
    b->bootinfo.ram_size = machine->ram_size;
    b->bootinfo.kernel_filename = machine->kernel_filename;
    b->bootinfo.kernel_cmdline = machine->kernel_cmdline;
    b->bootinfo.initrd_filename = machine->initrd_filename;
    b->bootinfo.nb_cpus = smp_cpus;
    b->bootinfo.board_id = -1; // Because we use a device tree dtb
    b->bootinfo.loader_start = b->memmap[vMEM_NAND].base;

    dinfo = NULL;
    dinfo = drive_get(IF_PFLASH, 0, 0);
    if (!dinfo) {
        fprintf(stderr, "Flash image must be given with the "
                "'pflash' parameter\n");
        exit(1);
    }

#if 0
    if (!pflash_cfi01_register(MSM_NAND_BASE,
                               NULL, "msm.flash0", 256*1024*1024,
                               dinfo ? blk_by_legacy_dinfo(dinfo) : NULL,
                               0x40000, 256*1024*1024 / 0x40000,
                               4, 0, 0, 0, 0, 0)) {
        fprintf(stderr, "qemu: Error registering flash memory.\n");
        exit(1);
    }
#endif


    arm_load_kernel(ARM_CPU(first_cpu), &b->bootinfo);
}

#if 0
static MachineClass machmsm_a15_machine =
//static QEMUMachine machmsm_a15_machine =
{
    .name = "msm9x28",
    .desc = "MSM ARM Virtual Machine for AR759x",
    .init = machmsm_init,
    .max_cpus = 1,
};

static void machmsm_machine_init(void)
{
    qemu_register_machine(&machmsm_a15_machine);
}

machine_init(machmsm_machine_init);
#endif

#if 1
static void machmsm_machine_init(MachineClass *mc)
{
    mc->desc = "MSM9x28";
    mc->init = machmsm_init;
}

//machine_init(machmsm_machine_init);
DEFINE_MACHINE("msm9x28", machmsm_machine_init)

#endif

