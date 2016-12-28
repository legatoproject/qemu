/*
 *  Qualcomm chipset 9x15 SMD/SMEM emulation: msm9x15_smd.c
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

static FILE *smdfd = NULL;

typedef struct msm9x15_smd_s {
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

typedef struct msm9x15_smsm_s {
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

struct msm9x15_smsm_s *smsm = NULL;

// Channel 26 -> DATA4 -> /dev/smd8
// QMI0 seems to be DATA5
// QMI1 seems to be DATA6
// QMI2 seems to be DATA7

static char bufat[2000];
static int bufatoff = 0;

/**/
/* msm9x15_write_channel_available : Check how much fifo free space available*/
/**/
static int msm9x15_write_channel_available(void* opaque, int ch)
{
	/* msm_smd *s = (msm_smd *)opaque; */

    struct smd_half_channel *half_ch = &(smsm->half_ch[ch][1]);
    return smsm->fifo_size[ch] - (half_ch->head - half_ch->tail);
}

/**/
/* msm9x15_write_channel_l2 : Write into channel fifo : level2*/
/**/
static int msm9x15_write_channel_l2(void *opaque, int ch, uint8_t *buffer, int len)
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
/* msm9x15_write_channel_l1 : Write into channel fifo : level1*/
/**/
static void msm9x15_write_channel_l1(void *opaque, int ch, uint8_t *buffer, int len )
{
msm_smd *s = (msm_smd *)opaque;
struct smd_half_channel *half_ch = &(smsm->half_ch[ch][1]);
int r;
uint8_t* p;
int l;
int size = msm9x15_write_channel_available(opaque,ch);
uint8_t header[20]={0};

	if(size < (20+len)) return; // Error ENOMEM/buffuring--flow control to be managed

	// Send Header
	*(unsigned *)(&header[0]) = len;
	l = 20;
	p = (uint8_t*)&header[0];
	while((r = msm9x15_write_channel_l2(opaque,ch,p,l)) != 0)
	{
		l -= r;
		p += r;
		if(!l) break;
	}

	// Send Payload
	l = len;
	p = buffer;
	while((r = msm9x15_write_channel_l2(opaque,ch,p,l)) != 0)
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
/* msm9x15_smd_read : SMD Read API*/
/**/
static uint64_t msm9x15_smd_read(void *opaque, hwaddr addr, unsigned size)
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

    //fprintf( stderr, "msm9x15_smd_read: ioaddr %08X, ioval %08llX", ioaddr, ioval );
    return ioval;
}

/**/
/* msm9x15_smd_write : SMD Write API*/
/**/
static void msm9x15_smd_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
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
      case 0x08:   if( (1 << 3) & ioval ) {
                     for( i = 0; i < 64; i++ ) {
                       if( smsm->half_ch[i] && smsm->half_ch[i]->state != smsm->state[i] ) {
                         struct smd_half_channel *half_ch = &(smsm->half_ch[i][1]);
			 // Channel Openning
                         if( smsm->half_ch[i]->state == 1 ) {
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
                           if( i == 26 ) {
                             bufatoff = 0;
                             memset( bufat, 0, sizeof(bufat) );
                           }
                           smsm->state[i] = 1;
                         }
			 // Channel Opened
                         else if( smsm->half_ch[i]->state == 2 ) {
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
                           smsm->half_ch[i]->tail = 0;
                           if( i == 26 ) {
                             bufatoff = 0;
                             memset( bufat, 0, sizeof(bufat) );
                           }
                           smsm->state[i] = 2;

                           /* CONFIG_ATLAS */
                           qemu_smd_backend_open(i,msm9x15_write_channel_l1,s);
                           /* CONFIG_ATLAS */

                         }
			 // Channel Closed
                         else if( smsm->half_ch[i]->state == 0 ) {
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
                           if( i == 26 ) {
                             bufatoff = 0;
                             memset( bufat, 0, sizeof(bufat) );
                           }
                           smsm->state[i] = 0;

                           /* CONFIG_ATLAS */
                           qemu_smd_backend_close(i);
                           /* CONFIG_ATLAS */

                         }
                       }
                       else if( smsm->half_ch[i] && (smsm->half_ch[i]->tail != smsm->half_ch[i]->head) ) {
                         if( smsm->half_ch[i]->state == 2 && (smsm->half_ch[i]->fTAIL || smsm->half_ch[i]->fHEAD) ) {
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
                           if( i == 26 ) {
                             if( smsm->off[i] > 20 ) {
                               memcpy( &bufat[bufatoff], &smsm->ptr[i][20], smsm->off[i] - 20 );
                               bufatoff += smsm->off[i] - 20;
                               bufat[bufatoff] = 0;
                               smsm->off[i] = 0;
                               if( bufat[bufatoff - 1] == '\n' ) {
				 // ATLAS: Call backend
				 bufat[bufatoff-1] = '\r';
				 qemu_smd_backend_send(26,(uint8_t*)bufat,bufatoff);
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

static const MemoryRegionOps msm9x15_smd_ops = {
    .read = msm9x15_smd_read,
    .write = msm9x15_smd_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

/**/
/* msm9x15_smd_init : SMD Initialization API*/
/**/
void msm9x15_smd_init(MemoryRegion *memory,hwaddr base,qemu_irq irq,uint32_t size)
{
msm_smd *s = (msm_smd *)g_malloc0( sizeof( msm_smd ) );

//    smdfd = fopen( "/tmp/smsm.txt", "w" );

    memory_region_init_io(&s->iomem, NULL, &msm9x15_smd_ops, s, "smd", size);
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
/* msm9x15_smsm_read : SMSM read from RAM*/
/**/
static uint64_t msm9x15_smsm_read(void *opaque, hwaddr addr, unsigned size)
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
/* msm9x15_smsm_write : SMSM Write to RAM*/
/**/
static void msm9x15_smsm_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
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

static const MemoryRegionOps msm9x15_smsm_ops = {
    .read = msm9x15_smsm_read,
    .write = msm9x15_smsm_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static QemuMutex msm9x15_smd_mutex;

/**/
/* msm9x15_smsm_init : SMSM Initialization API. get content from an SMD mirror file and build channels resources according to the map*/
/**/
void msm9x15_smsm_init(MemoryRegion *memory,hwaddr base,qemu_irq irq,uint32_t size)
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

    qemu_mutex_init(&msm9x15_smd_mutex);

    memory_region_init_io(&s->iomem, NULL, &msm9x15_smsm_ops, s, "smsm", size);
    memory_region_add_subregion(memory, base, &s->iomem);

    s->ramptr = (uint8_t*)g_malloc0( size );
    memset( &s->ramptr[0x0], 0xDA, 0x00100000 );

    if( 0 <= (fd = open( smd_filename, O_RDONLY )) ) {
        fprintf(stderr,"Reading sierra_smem ... " );
        int nb = read( fd, &s->ramptr[0x0], 0x00100000 );
        fprintf(stderr, "%02X %02X %02X %02X\n",
                s->ramptr[0xf5000], s->ramptr[0xf5001], s->ramptr[0xf5002], s->ramptr[0xf5003] );
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
    for( i = 0; i < 512; i++ ) {
      if( s->heapentry[i].allocated ) {
        fprintf(stderr, "  Entry %3d: o:%08X s:%u\n", i, s->heapentry[i].offset, s->heapentry[i].size );
      }
    }

    struct smd_alloc_elm *ch = (struct smd_alloc_elm *)&(s->ramptr[s->heapentry[13].offset]);
	int loffset=0;
    for( i = 0; i < 64; i++ ) {
      if( ch[i].ref_count && 0 == (ch[i].type & 0xFF) ) {

		s->heapentry[ch[i].cid+SMEM_SMD_BASE_ID].offset += loffset;
		s->heapentry[ch[i].cid+SMEM_SMD_FIFO_BASE_ID].offset += loffset;

		if(ch[i].cid == 31)
		{
			s->heapentry[ch[i].cid+SMEM_SMD_FIFO_BASE_ID].size = 4096*2;	//Increase FIFO size for SMD 31. Seems that Embedded SMD/QMI badly manage Frame more than FIFO size !!
			//start updating the offset
			loffset = 4096*2;
		}

        fprintf(stderr, "%-20s cid:%3u type:%08X ref_count:%8X\n", ch[i].name, ch[i].cid, ch[i].type, ch[i].ref_count );
        fprintf(stderr, "Entry %u: offset:%08X size=%u\n",
                ch[i].cid,s->heapentry[ch[i].cid+SMEM_SMD_BASE_ID].offset,s->heapentry[ch[i].cid+SMEM_SMD_BASE_ID].size);
        fprintf(stderr, "Entry %u: offset:%08X size=%u\n",
                ch[i].cid,s->heapentry[ch[i].cid+SMEM_SMD_FIFO_BASE_ID].offset,s->heapentry[ch[i].cid+SMEM_SMD_FIFO_BASE_ID].size);

        s->half_ch[ch[i].cid] = (struct smd_half_channel *)&(s->ramptr[s->heapentry[ch[i].cid+SMEM_SMD_BASE_ID].offset]);
        s->half_ch[ch[i].cid]->state = 0;
        s->half_ch[ch[i].cid][1].state = 0;
        s->fifo_ch[ch[i].cid] = (uint8_t *)&(s->ramptr[s->heapentry[ch[i].cid+SMEM_SMD_FIFO_BASE_ID].offset]);
        s->fifo_size[ch[i].cid] = (s->heapentry[ch[i].cid+SMEM_SMD_FIFO_BASE_ID].size) / 2;
      }
    }

    // Export the AT channel value to ATLAS backend.
    qemu_smd_backend_set_atchannel(26);
}
