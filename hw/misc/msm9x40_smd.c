#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu-common.h"
#include "hw/qdev.h"

#include "hw/sysbus.h"
#include "hw/arm/arm.h"
#include "hw/arm/primecell.h"
#include "hw/devices.h"
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

#include "hw/arm/msm.h"

#include "atlas/qemu_smd.h"

#define ALIGN_KERNEL(x, a)             __ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask)  (((x) + (mask)) & ~(mask))

#define SMEM_NUM_SMD_STREAM_CHANNELS        64
#define SMEM_TARG_INFO_IDENTIFIER_ADD       0x000BF0B0
#define SMEM_TARG_INFO_IDENTIFIER           0x49494953
#define SMEM_RAM_PHYS_SIZE                  0x000BF0B4
#define SMEM_RAM_PHYS_ADD                   0x000BF0B8
#define SMEM_ALLOCATION_CANARY              0xa5a5
#define SMEM_TOC_MAX_EXCLUSIONS             4

static FILE *smdfd = NULL;

typedef struct msm9x40_smd_s {
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

struct smem_proc_comm {
    unsigned command;
    unsigned status;
    unsigned data1;
    unsigned data2;
};

struct smem_shared {
    struct smem_proc_comm proc_comm[4];
    unsigned version[32];
    struct smem_heap_info heap_info;
#define SMD_HEAP_SIZE 512
    struct smem_heap_entry heap_toc[SMD_HEAP_SIZE];
};

struct smem_partition_header {
    /* Identifier is a constant, set to SMEM_PART_HDR_IDENTIFIER. */
    uint32_t identifier;
    uint16_t host0;
    uint16_t host1;
    uint32_t size;
    uint32_t offset_free_uncached;
    uint32_t offset_free_cached;
    uint32_t reserved[3];
};

struct smem_partition_allocation_header {
    /* Canary is a constant, set to SMEM_ALLOCATION_CANARY */
    uint16_t canary;
    uint16_t smem_type;
    uint32_t size; /* includes padding bytes */
    uint16_t padding_data;
    uint16_t padding_hdr;
    uint32_t reserved[1];
};

struct smem_toc_entry {
    uint32_t offset;
    uint32_t size;
    uint32_t flags;
    uint16_t host0;
    uint16_t host1;
    uint32_t size_cacheline;
    uint32_t reserved[3];
    uint32_t exclusion_sizes[SMEM_TOC_MAX_EXCLUSIONS];
};

struct smem_toc {
    /* Identifier is a constant, set to SMEM_TOC_IDENTIFIER. */
    uint32_t identifier;
    uint32_t version;
    uint32_t num_entries;
    uint32_t reserved[5];
    struct smem_toc_entry entry[];
};

enum {
    SMEM_APPS,
    SMEM_MODEM,
    SMEM_Q6,
    SMEM_DSPS,
    SMEM_WCNSS,
    SMEM_MODEM_Q6_FW,
    SMEM_RPM,
    NUM_SMEM_SUBSYSTEMS,
};

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
    SMEM_SMD_BASE_ID_2,
    SMEM_SMD_FIFO_BASE_ID_2 = SMEM_SMD_BASE_ID_2 +
                        SMEM_NUM_SMD_STREAM_CHANNELS,
    SMEM_CHANNEL_ALLOC_TBL_2 = SMEM_SMD_FIFO_BASE_ID_2 +
                        SMEM_NUM_SMD_STREAM_CHANNELS,
    SMEM_I2C_MUTEX = SMEM_CHANNEL_ALLOC_TBL_2 +
                        SMEM_NUM_SMD_STREAM_CHANNELS,
    SMEM_SCLK_CONVERSION,
    SMEM_SMD_SMSM_INTR_MUX,
    SMEM_SMSM_CPU_INTR_MASK,
    SMEM_APPS_DEM_SLAVE_DATA,
    SMEM_QDSP6_DEM_SLAVE_DATA,
    SMEM_VSENSE_DATA,
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
    SMEM_SMP2P_APPS_BASE = 427,
    SMEM_SMP2P_MODEM_BASE = SMEM_SMP2P_APPS_BASE + 8,    /* 435 */
    SMEM_SMP2P_AUDIO_BASE = SMEM_SMP2P_MODEM_BASE + 8,   /* 443 */
    SMEM_SMP2P_WIRLESS_BASE = SMEM_SMP2P_AUDIO_BASE + 8, /* 451 */
    SMEM_SMP2P_POWER_BASE = SMEM_SMP2P_WIRLESS_BASE + 8, /* 459 */
    SMEM_FLASH_DEVICE_INFO = SMEM_SMP2P_POWER_BASE + 8,  /* 467 */
    SMEM_BAM_PIPE_MEMORY,     /* 468 */
    SMEM_IMAGE_VERSION_TABLE, /* 469 */
    SMEM_LC_DEBUGGER, /* 470 */
    SMEM_FLASH_NAND_DEV_INFO, /* 471 */
    SMEM_A2_BAM_DESCRIPTOR_FIFO, /* 472 */
    SMEM_CPR_CONFIG, /* 473 */
    SMEM_CLOCK_INFO, /* 474 */
    SMEM_IPC_FIFO, /* 475 */
    SMEM_RF_EEPROM_DATA, /* 476 */
    SMEM_COEX_MDM_WCN, /* 477 */
    SMEM_GLINK_NATIVE_XPRT_DESCRIPTOR, /* 478 */
    SMEM_GLINK_NATIVE_XPRT_FIFO_0, /* 479 */
    SMEM_GLINK_NATIVE_XPRT_FIFO_1, /* 480 */
    SMEM_SMP2P_SENSOR_BASE, /* 481 */
    SMEM_NUM_ITEMS = SMEM_SMP2P_SENSOR_BASE + 8, /* 489 */
};


#define SMD_BUF_SIZE             8192
#define SMD_CHANNELS             64
#define SMD_HEADER_SIZE          20

#define SMD_SS_CLOSED            0x00000000
#define SMD_SS_OPENING           0x00000001
#define SMD_SS_OPENED            0x00000002
#define SMD_SS_FLUSHING          0x00000003
#define SMD_SS_CLOSING           0x00000004
#define SMD_SS_RESET             0x00000005
#define SMD_SS_RESET_OPENING     0x00000006

// From swi-mdm9640.dtsi
#define SMD_MODEM_IRQ_BITMASK   0x10
#define SMSM_IRQ_BITMASK        0x20
#define SMD_RPM_IRQ_BITMASK     0x1

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

typedef struct msm9x40_smsm_s {
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

static struct msm9x40_smsm_s *smsm = NULL;

MemoryRegion smem_targ_info_9x40;
MemoryRegion smem_aux1_9x40;

// Channel 14 -> DATA4 -> /dev/smd8
#define SMD_CHANNEL_DATA4 14
static char bufat[2000];
static int bufatoff = 0;

/**/
/* msm9x40_write_channel_available : Check how much fifo free space available*/
/**/
static int msm9x40_write_channel_available(void* opaque, int ch)
{
	/* msm_smd *s = (msm_smd *)opaque; */

    struct smd_half_channel *half_ch = &(smsm->half_ch[ch][1]);
    return smsm->fifo_size[ch] - (half_ch->head - half_ch->tail);
}

/**/
/* msm9x40_write_channel_l2 : Write into channel fifo : level2*/
/**/
static int msm9x40_write_channel_l2(void *opaque, int ch, uint8_t *buffer, int len)
{
/*msm_smd *s = (msm_smd *)opaque;*/
struct smd_half_channel *half_ch = &(smsm->half_ch[ch][1]);
uint8_t* ptr = buffer;
int l_len = len;
int flen=0;

	if(half_ch->head >= half_ch->tail)
		flen = smsm->fifo_size[ch] - half_ch->head;
	else
		flen = half_ch->tail - half_ch->head;

	if(flen >= l_len)
	{
     	memset( &(smsm->fifo_ch[ch][smsm->fifo_size[ch] + half_ch->head]),0,l_len);
		memcpy( &(smsm->fifo_ch[ch][smsm->fifo_size[ch] + half_ch->head]),ptr,l_len);
		half_ch->head += l_len;
		half_ch->head %= smsm->fifo_size[ch];
		l_len = 0;
	}
	else
	{
        memset( &(smsm->fifo_ch[ch][smsm->fifo_size[ch] + half_ch->head]),0,flen);
		memcpy( &(smsm->fifo_ch[ch][smsm->fifo_size[ch] + half_ch->head]),ptr,flen);
		half_ch->head += flen;
		half_ch->head %= smsm->fifo_size[ch];
		l_len -= flen;
	}
return (len-l_len);
}

/**/
/* msm9x40_write_channel_l1 : Write into channel fifo : level1*/
/**/
static void msm9x40_write_channel_l1(void *opaque, int ch, uint8_t *buffer, int len )
{
msm_smd *s = (msm_smd *)opaque;
struct smd_half_channel *half_ch = &(smsm->half_ch[ch][1]);
int r;
uint8_t* p;
int l;
int size = msm9x40_write_channel_available(opaque,ch);
uint8_t header[20]={0};

	if(size < (20+len)) return; // Error ENOMEM/buffuring--flow control to be managed

	// Send Header
	*(unsigned *)(&header[0]) = len;
	l = 20;
	p = (uint8_t*)&header[0];
	while((r = msm9x40_write_channel_l2(opaque,ch,p,l)) != 0)
	{
		l -= r;
		p += r;
		if(!l) break;
	}

	// Send Payload
	l = len;
	p = buffer;
	while((r = msm9x40_write_channel_l2(opaque,ch,p,l)) != 0)
	{
		l -= r;
		p += r;
		if(!l) break;
	}

	smsm->half_ch[ch]->fTAIL = smsm->half_ch[ch]->fHEAD = 0;
   	half_ch->fHEAD = 1;

    	qemu_irq_pulse(s->irq);
}

/**/
/* msm9x40_smd_read : SMD Read API*/
/**/
static uint64_t msm9x40_smd_read(void *opaque, hwaddr addr, unsigned size)
{
// root@swi-mdm9x40:~# echo 0xb011000:100 > /proc/msm/core/physaccess
// [  390.605370] YKEM CORE PHYSACCESS (3) address = 0B011000, virtual = D14FE000
// [  390.612138]  +000 : 00000000 FFFFFFFF 00000000 00000000
// [  390.616606]  +010 : 0000004F 00008100 00000708 00000018
// [  390.622529]  +020 : 00000000 00000000 00000000 00000000
// [  390.627016]  +030 : 00000000 00000000 00000000 00000000
// [  390.632224]  +040 : 00000000 00000000 00000000 00000000
// [  390.637433]  +050 : 00000000 00000000 00000000 00000000
// [  390.642642]  +060 : 00000000 00000000 00000000 00000000
// [  390.647850]  +070 : 00000000 00000000 00000000 00000000
// [  390.653057]  +080 : 00000000 00000000 00000000 00000000
// [  390.658265]  +090 : 00000000 00000000 00000000 00000000
// [  390.663474]  +0A0 : 00000000 00000000 00000000 00000000
// [  390.668682]  +0B0 : 00000000 00000000 00000000 00000000
// [  390.673891]  +0C0 : 00000000 00000000 00000000 00000000
// [  390.679097]  +0D0 : 00000000 00000000 00000000 00000000

    hwaddr ioaddr = addr & 0x3C;
    uint64_t ioval;

    switch( ioaddr ) {
      case 0x00:   ioval = 0x00000000; break;
      case 0x04:   ioval = 0xFFFFFFFF; break;
      case 0x08:   ioval = 0x00000000; break;
      case 0x0C:   ioval = 0x00000000; break;
      case 0x10:   ioval = 0x0000004F; break;
      case 0x14:   ioval = 0x00008100; break;
      case 0x18:   ioval = 0x00000708; break;
      case 0x1C:   ioval = 0x00000018; break;
      default:
                   //fprintf( stderr, "msm9x40_smd_read error ioaddr not supported 0x%x\n", (unsigned int)ioaddr);
                   ioval = 0x00000000; break;
    }

    //fprintf( stderr, "msm9x40_smd_read: ioaddr %08X, ioval %08llX", ioaddr, ioval );
    return ioval;
}

/**/
/* msm9x40_smd_write : SMD Write API*/
/**/
static void msm9x40_smd_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
{
    msm_smd *s = (msm_smd *)opaque;
    unsigned ioaddr = (unsigned)addr & 0x3C;
    unsigned ioval = 0;
    int i;
    if(s->fd) fprintf( s->fd, "SMD %08X,%04X :", (unsigned)addr, size );
    switch( size ) {
      case 4:    ioval |= ((value >> 24) & 0xFF );
      case 3:    ioval |= ((value >> 16) & 0xFF );
      case 2:    ioval |= ((value >> 8) & 0xFF );
      case 1:    ioval |= ((value) & 0xFF );
    }
    if(s->fd) {
    	fprintf( s->fd, " = %08X\n", ioval );
    	fflush( s->fd );
    }
    switch( ioaddr ) {
      case 0x00:   break;
      case 0x04:   break;
      case 0x08:   if( SMD_MODEM_IRQ_BITMASK & ioval ) {
                     for( i = 0; i < SMD_CHANNELS; i++ ) {
                       if( smsm->half_ch[i] && smsm->half_ch[i]->state != smsm->state[i] ) {
                         struct smd_half_channel *half_ch = &(smsm->half_ch[i][1]);
			 // Channel Openning
                         if( smsm->half_ch[i]->state == SMD_SS_OPENING ) {
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
                           smsm->half_ch[i]->tail = 0;
                           if( i == SMD_CHANNEL_DATA4 ) {
                             bufatoff = 0;
                             memset( bufat, 0, sizeof(bufat) );
                           }
                           smsm->state[i] = SMD_SS_OPENING;
                         }
			 // Channel Opened
                         else if( smsm->half_ch[i]->state == SMD_SS_OPENED ) {
                           half_ch->state = SMD_SS_OPENED;
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
                           smsm->half_ch[i]->tail = 0;
                           if( i == SMD_CHANNEL_DATA4 ) {
                             bufatoff = 0;
                             memset( bufat, 0, sizeof(bufat) );
                           }
                           smsm->state[i] = SMD_SS_OPENED;

                           /* CONFIG_ATLAS */
                           qemu_smd_backend_open(i,msm9x40_write_channel_l1,s);
                           /* CONFIG_ATLAS */

                         }
			 // Channel Closed
                         else if( smsm->half_ch[i]->state == SMD_SS_CLOSED ) {
                           struct smd_half_channel *half_ch = &(smsm->half_ch[i][1]);
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
                           smsm->half_ch[i]->tail = 0;
                           if( i == SMD_CHANNEL_DATA4 ) {
                             bufatoff = 0;
                             memset( bufat, 0, sizeof(bufat) );
                           }
                           smsm->state[i] = SMD_SS_CLOSED;

                           /* CONFIG_ATLAS */
                           qemu_smd_backend_close(i);
                           /* CONFIG_ATLAS */

                         }
                       }
                       else if( smsm->half_ch[i] && (smsm->half_ch[i]->tail != smsm->half_ch[i]->head) ) {
                         if( smsm->half_ch[i]->state == 2 && (smsm->half_ch[i]->fTAIL || smsm->half_ch[i]->fHEAD) ) {
                           /*struct smd_half_channel *half_ch = &(smsm->half_ch[i][1]);*/
                           if( smsm->half_ch[i]->tail <= smsm->half_ch[i]->head ) {
                             memcpy(&smsm->ptr[i][smsm->off[i]],
                                    &(smsm->fifo_ch[i][smsm->half_ch[i]->tail]),
                                    smsm->half_ch[i]->head-smsm->half_ch[i]->tail);
                             smsm->off[i] += smsm->half_ch[i]->head-smsm->half_ch[i]->tail;
                           }
                           else {
                             memcpy(&smsm->ptr[i][smsm->off[i]],
                                    &(smsm->fifo_ch[i][smsm->half_ch[i]->tail]),
                                    (smsm->fifo_size[i])-smsm->half_ch[i]->tail);
                             smsm->off[i] += smsm->fifo_size[i]-smsm->half_ch[i]->tail;
                             memcpy(&smsm->ptr[i][smsm->off[i]],
                                    &(smsm->fifo_ch[i][0]),
                                    smsm->half_ch[i]->head);
                             smsm->off[i] += smsm->half_ch[i]->head;
                           }
                           smsm->half_ch[i]->tail = smsm->half_ch[i]->head;
                           if( i == SMD_CHANNEL_DATA4 ) {
                             if( smsm->off[i] > 20 ) {
                               memcpy( &bufat[bufatoff], &smsm->ptr[i][20], smsm->off[i] - 20 );
                               bufatoff += smsm->off[i] - 20;
                               bufat[bufatoff] = 0;
                               smsm->off[i] = 0;
                               if( bufat[bufatoff - 1] == '\n' ) {
				 // ATLAS: Call backend
				 bufat[bufatoff-1] = '\r';
				 qemu_smd_backend_send(SMD_CHANNEL_DATA4,(uint8_t*)bufat,bufatoff);
				 // ATLAS End
                                 bufatoff = smsm->off[i] = 0;
                               }
                             }
                           }
			   else {
			     if( smsm->off[i] > 20 ) {

                 qemu_smd_backend_send(i,&smsm->ptr[i][20],smsm->off[i]-20);

				 smsm->off[i] = 0;
			     }
			   }
			 //  else smsm->off[i] = 0;
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
    if(s->fd) fflush( s->fd );
}

static const MemoryRegionOps msm9x40_smd_ops = {
    .read = msm9x40_smd_read,
    .write = msm9x40_smd_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

/**/
/* msm9x40_smd_init : SMD Initialization API*/
/**/
void msm9x40_smd_init(MemoryRegion *memory,hwaddr base,qemu_irq irq,uint32_t size)
{
msm_smd *s = (msm_smd *)g_malloc0( sizeof( msm_smd ) );

    memory_region_init_io(&s->iomem, NULL, &msm9x40_smd_ops, s, "smd", size);
    memory_region_add_subregion(memory, base, &s->iomem);
    s->fd = smdfd;
    s->irq = irq;
}

// DATA4
// [  190.983488] set_state: FA38CA70 = 00000001
// [  190.983549] notify_modem_smd(2): 0
// [  190.984037] SMD Int Modem->Apps
// [  190.984098] get_state: FA38CA84 = 00000001

/**/
/* msm9x40_smsm_read : SMSM read from RAM*/
/**/
static uint64_t msm9x40_smsm_read(void *opaque, hwaddr addr, unsigned size)
{
    msm_smsm *s = (msm_smsm *)opaque;
    uint64_t ioval = 0;
    unsigned offset = (unsigned)addr;

    switch( size ) {
      case 4:    ioval |= ((s->ramptr[offset + 3]) << 24);
      case 3:    ioval |= ((s->ramptr[offset + 2]) << 16);
      case 2:    ioval |= ((s->ramptr[offset + 1]) << 8);
      case 1:    ioval |= (s->ramptr[offset]);
    }
    return ioval;
}

/**/
/* msm9x40_smsm_write : SMSM Write to RAM*/
/**/
static void msm9x40_smsm_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
{
msm_smsm *s = (msm_smsm *)opaque;
unsigned offset = (unsigned)addr;

    if(s->fd) fprintf( s->fd, "SMSM %08X,%04X :", (unsigned)addr, size );
    switch( size ) {
      case 4:    s->ramptr[offset + 3] = ((value >> 24) & 0xFF );
                 if(s->fd) fprintf( s->fd, " %02hhX", s->ramptr[offset + 3] );
      case 3:    s->ramptr[offset + 2] = ((value >> 16) & 0xFF );
                 if(s->fd) fprintf( s->fd, " %02hhX", s->ramptr[offset + 2] );
      case 2:    s->ramptr[offset + 1] = ((value >> 8) & 0xFF );
                 if(s->fd) fprintf( s->fd, " %02hhX", s->ramptr[offset + 1] );
      case 1:    s->ramptr[offset] = ((value) & 0xFF );
                 if(s->fd) fprintf( s->fd, " %02hhX", s->ramptr[offset] );
    }

    if(s->fd) {
	fputc( '\n', s->fd );
    	fflush( s->fd );
    }
}

static const MemoryRegionOps msm9x40_smsm_ops = {
    .read = msm9x40_smsm_read,
    .write = msm9x40_smsm_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

/**/
/* msm9x40_smem_targ_info_read : Read from the target info area */
/**/
static uint64_t msm9x40_smem_targ_info_read(void *opaque, hwaddr addr, unsigned size)
{
//[56925.276745] YKEM CORE PHYSACCESS (3) address = 0193D000, virtual = D14FE000
//[56925.282823]  +000 : 87F3F0B0 00000000 00000000 00000000
//[56925.287981]  +010 : 00000000 00000000 00000000 00000000
//[56925.293978]  +020 : 00000000 00000000 00000000 00000000
//[56925.298395]  +030 : 00000000 00000000 00000000 00000000

    uint64_t ioval = 0;
    uint32_t val = 0;

    switch( (unsigned)(addr) ) {
      case 0: val = 0x87F3F0B0; break;
      default: val = 0;
    }
    switch( size ) {
      case 4:    ioval = val; break;
      case 3:    ioval = val & 0xFFFFFF; break;
      case 2:    ioval = val & 0xFFFF; break;
      case 1:    ioval = val & 0xFF; break;
    }
    //fprintf( stderr, "SMEM target read %08X,%04X  %08llX\n", (unsigned)addr, size, (long long unsigned)ioval );
    return ioval;
}

/**/
/* msm9x40_smem_targ_info_write : write the target info area */
/**/
static void msm9x40_smem_targ_info_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
{
    //fprintf( stderr, "SMEM targ info write %08X,%04X = %08llX :", (unsigned)addr, size, (long long unsigned)value );
}

static const MemoryRegionOps msm9x40_smem_targ_info_ops = {
    .read = msm9x40_smem_targ_info_read,
    .write = msm9x40_smem_targ_info_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

/**/
/* msm9x40_smem_targ_info_init : target info initialization API */
/**/
void msm9x40_smem_targ_info_init(MemoryRegion *memory,hwaddr base,qemu_irq irq,uint32_t size)
{
    memory_region_init_io(&smem_targ_info_9x40, NULL, &msm9x40_smem_targ_info_ops, NULL, "smem_targ_info_9x40", size);
    memory_region_add_subregion(memory, base, &smem_targ_info_9x40);
}

/**/
/* msm9x40_smem_aux1_read : SMEM AUX1 (SMEM no security, used by RPM) read */
/**/
static uint64_t msm9x40_smem_aux1_read(void *opaque, hwaddr addr, unsigned size)
{
//[24268.408421] YKEM CORE PHYSACCESS (3) address = 00060000, virtual = D080A000
//[24268.414452]  +000 : 00000002 00000001 00000001 00000001
//[24268.419655]  +010 : 00000000 00000000 00000001 00000000
//[24268.425718]  +020 : 00000001 000000B0 000000B0 00000002
//[24268.430923]  +030 : 00000001 00000001 00000001 00000000
//[24268.436135]  +040 : 00000000 00000000 00000000 00000001
//[24268.441342]  +050 : 00000348 00000348 E6BFA417 A23F6771
//[24268.446550]  +060 : 9E79CAD2 AF166AB3 90499B36 3953E504
//[24268.451759]  +070 : 8E5B96AD 63981678 BD988EC9 567FB523

    msm_smsm *s = (msm_smsm *)opaque;
    uint64_t ioval = 0;
    unsigned offset = (unsigned)addr;

    switch( size ) {
      case 4:    ioval |= ((s->ramptr[offset + 3]) << 24);
      case 3:    ioval |= ((s->ramptr[offset + 2]) << 16);
      case 2:    ioval |= ((s->ramptr[offset + 1]) << 8);
      case 1:    ioval |= (s->ramptr[offset]);
    }
    //fprintf( stderr, "SMEM aux1 read %08X,%04X  %08llX\n", (unsigned)addr, size, (long long unsigned)ioval );
    return ioval;
}

/**/
/* msm9x40_smem_aux1_write : SMEM AUX1 (SMEM no security, used by RPM) write */
/**/
static void msm9x40_smem_aux1_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
{
    msm_smsm *s = (msm_smsm *)opaque;
    unsigned offset = (unsigned)addr;
    //fprintf( stderr, "SMEM aux1 write %08X,%04X = %08llX \n", (unsigned)addr, size, (long long unsigned)value );
    switch( size ) {
      case 4:    s->ramptr[offset + 3] = ((value >> 24) & 0xFF );
      case 3:    s->ramptr[offset + 2] = ((value >> 16) & 0xFF );
      case 2:    s->ramptr[offset + 1] = ((value >> 8) & 0xFF );
      case 1:    s->ramptr[offset] = ((value) & 0xFF );
    }
}

static const MemoryRegionOps msm9x40_smem_aux1_ops = {
    .read = msm9x40_smem_aux1_read,
    .write = msm9x40_smem_aux1_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

/**/
/* msm9x40_smem_aux1_init : SMEM AUX1 (SMEM no security, used by RPM) initialization */
/**/
void msm9x40_smem_aux1_init(MemoryRegion *memory,hwaddr base,qemu_irq irq,uint32_t size)
{
    msm_smsm *s = (msm_smsm *)g_malloc0( sizeof( msm_smsm ) );
    int fd;
    s->ramptr = (uint8_t*)g_malloc0( size );
    memset( &s->ramptr[0x0], 0xDA, size );
    if( 0 <= (fd = open( "smem_aux.bin", O_RDONLY )) ) {
        int n;
        printf( "Reading sierra_smem ... " );
        n = read( fd, &s->ramptr[0x0], size );
        close( fd );
        if( n < 0 )
          return;
    }

    memory_region_init_io(&smem_aux1_9x40, NULL, &msm9x40_smem_aux1_ops, s, "smem_aux1_9x40", size);
    memory_region_add_subregion(memory, base, &smem_aux1_9x40);
}

/**/
/* smem_get_entry: Get an entry in the SMEM */
/**/
static void * smem_get_entry(unsigned id, unsigned * size, struct smem_partition_header *hdr)
{
    struct smem_partition_allocation_header *alloc_hdr;
    void *item = NULL;
    for (alloc_hdr = (void *)(hdr) + sizeof(*hdr);
            (void *)(alloc_hdr) < (void *)(hdr) +
            hdr->offset_free_uncached;
            alloc_hdr = (void *)(alloc_hdr) +
            sizeof(*alloc_hdr) +
            alloc_hdr->padding_hdr +
            alloc_hdr->size) {
        if (alloc_hdr->canary != SMEM_ALLOCATION_CANARY) {
            fprintf(stderr,
                    "%s: SMEM corruption detected.  Partition %d to %d at %p\n",
                    __func__,
                    1,
                    1,
                    alloc_hdr);

        }
        //fprintf(stderr, "Id %d, alloc_hdr->smem_type %d" id, alloc_hdr->smem_type);
        if (alloc_hdr->smem_type == id) {
            /* 8 byte alignment to match legacy */
            *size = ALIGN_KERNEL(alloc_hdr->size -
                    alloc_hdr->padding_data, 8);
            item = (void *)(alloc_hdr) +
                sizeof(*alloc_hdr) +
                alloc_hdr->padding_hdr;
            fprintf( stderr, "Found for id %d, size %lld, add 0x%llx ", id, (unsigned long long) size, (unsigned long long)item );
        }
    }
    return item;
}

/**/
/* msm9x40_smsm_init : SMSM Initialization API. get content from an SMD mirror file and build channels resources according to the map*/
/**/
void msm9x40_smsm_init(MemoryRegion *memory,hwaddr base,qemu_irq irq,uint32_t size)
{
msm_smsm *s = (msm_smsm *)g_malloc0( sizeof( msm_smsm ) );
int fd;
int i;


	const char * smd_filename = option_rom[0].name;
    if(!smd_filename)
    {
        fprintf(stderr,"You did not specified '-option-rom <path>' option." );
        exit(0);
    }

    memory_region_init_io(&s->iomem, NULL, &msm9x40_smsm_ops, s, "smsm", size);
    memory_region_add_subregion(memory, base, &s->iomem);

    s->ramptr = (uint8_t*)g_malloc0( size );
    memset( &s->ramptr[0x0], 0xDA, size );


    if( 0 <= (fd = open( smd_filename, O_RDONLY )) ) {

        fprintf(stderr,"Reading sierra_smem ... " );
        int nb = read( fd, &s->ramptr[0x0], size );
        fprintf(stderr,"%d read \n",nb );
        close( fd );
    }

    s->fd = NULL;
    s->irq = irq;
    smsm = s;
    memset( s->half_ch, 0, sizeof( s->half_ch ) );
    memset( s->fifo_ch, 0, sizeof( s->fifo_ch ) );
    memset( s->fifo_size, 0, sizeof( s->fifo_size ) );
    memset( s->off, 0, sizeof( s->off ) );
    memset( s->ptr, 0, sizeof( s->ptr ) );
    memset( s->state, 0, sizeof( s->state ) );
    s->shared = (struct smem_shared *)s->ramptr;
    s->heapinfo = (struct smem_heap_info *)&(s->shared->heap_info);
    s->heapentry = (struct smem_heap_entry *)&(s->shared->heap_toc);
    fprintf(stderr, "sramptr %08"PRIXPTR", heapinfo %08"PRIXPTR", heapentry %08"PRIXPTR"\n",
            (uintptr_t)s->shared,(uintptr_t)s->heapinfo,(uintptr_t)s->heapentry );
    for( i = 0; i < SMD_HEAP_SIZE; i++ ) {
      if( s->heapentry[i].allocated ) {
        fprintf(stderr, "  Entry %3d: o:%08X s:%u\n", i, s->heapentry[i].offset, s->heapentry[i].size );
      }
    }

    void *item = NULL;
    uint32_t id, tmpSize;
    struct smem_toc * toc = (struct smem_toc *)&(s->ramptr[size - 4*1024]);
    uint32_t partition1_offset = 0;
    for (i = 0; i < toc->num_entries; ++i) {
        if (toc->entry[i].host0 == SMEM_APPS
            || toc->entry[i].host1 == SMEM_APPS)
        {
            partition1_offset = toc->entry[i].offset;
        }
    }
    struct smem_partition_header *hdr  = (struct smem_partition_header *)&(s->ramptr[partition1_offset]);
    for( id = SMEM_SMD_BASE_ID; id < SMEM_SMD_BASE_ID + SMEM_NUM_SMD_STREAM_CHANNELS; id++ ) {
        item = smem_get_entry(id, &tmpSize, hdr);
        if (item != NULL)
        {
            fprintf( stderr, "Found for id %d, size %d, add 0x%llx ", id, tmpSize, (unsigned long long)item );
            int cid = id - SMEM_SMD_BASE_ID;
            s->half_ch[cid] = item;
            s->half_ch[cid]->state = 0;
            s->half_ch[cid][1].state = 0;
        }
    }
    for( id = SMEM_SMD_FIFO_BASE_ID; id < SMEM_SMD_FIFO_BASE_ID + SMEM_NUM_SMD_STREAM_CHANNELS; id++ ) {
        item = smem_get_entry(id, &tmpSize, hdr);
        if (item != NULL)
        {
            fprintf( stderr, "Found for id %d, size %d, add 0x%llx ", id, tmpSize, (unsigned long long)item );
            int cid = id - SMEM_SMD_FIFO_BASE_ID;
            s->fifo_ch[cid] = item;
            s->fifo_size[cid] = tmpSize / 2;
        }
    }

    // Export the AT channel value to ATLAS backend.
    qemu_smd_backend_set_atchannel(SMD_CHANNEL_DATA4);
}
