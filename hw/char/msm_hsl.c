/*
 * MSM HSL UART emulation - msmx9x15
 *
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu-common.h"
#include "qemu/timer.h"

#include "hw/char/serial.h"
#include "sysemu/char.h"
#include "qemu/timer.h"
#include "exec/address-spaces.h"
#include "qemu/error-report.h"

# define TX_FIFO_SIZE	256
# define RX_FIFO_SIZE	256

typedef struct MsmHslCtx {
    MemoryRegion iomem;
    int vid;
    uint32_t misr;
    uint32_t dmrx;
    uint32_t mr1;
    uint32_t mr2;
    uint32_t imr;
    uint32_t sr;
    uint32_t cr;
    uint32_t csr;
    uint32_t ipr;
    uint32_t isr;
    uint32_t rx_total_snap;
    uint32_t rfwr;
    uint32_t tfwr;
    uint32_t rf;
    uint32_t tf;
    uint32_t txfs;
    uint32_t rxfs;
    uint32_t ncf_tx;
    uint32_t rfr_level;
    uint32_t speed;
    uint32_t int_enabled;
    uint32_t int_level;
    uint8_t rx_fifo[RX_FIFO_SIZE];
    uint8_t tx_fifo[TX_FIFO_SIZE];
    uint32_t tx_count;
    uint32_t rx_count;
    uint32_t rx_wpos;
    uint64_t tx_timeout;
    QEMUTimer *fifo_trigger_handle;
    CharDriverState *chr;
    qemu_irq irq;
    const unsigned char *id;
}MsmHslCtx;
#define RESET_RX		0x10
#define RESET_TX		0x20
#define START_BREAK		0x50
#define STOP_BREAK		0x60

#define	UARTDM_MR1_REG	0x0
#define	UARTDM_MR2_REG  0x4
#define	UARTDM_IMR_REG  0x14	// WO
#define	UARTDM_SR_REG   0x08	// RO
#define	UARTDM_CR_REG   0x10
#define	UARTDM_CSR_REG  0x08
#define	UARTDM_IPR_REG  0x18
#define	UARTDM_ISR_REG  0x14	// RO
#define	UARTDM_RX_TOTAL_SNAP_REG 0x38
#define	UARTDM_RFWR_REG 0x20
#define	UARTDM_TFWR_REG 0x1c
#define	UARTDM_RF_REG   0x70
#define	UARTDM_TF_REG   0x70
#define	UARTDM_MISR_REG 0x10	// RO
#define	UARTDM_DMRX_REG 0x34
#define	UARTDM_NCF_TX_REG 0x40
#define	UARTDM_DMEN_REG 0x3c
#define	UARTDM_TXFS_REG 0x4c
#define	UARTDM_RXFS_REG 0x50

#define	UARTDM_HS 0xB4

#define UARTDM_VERSION_11_13	0
#define UARTDM_VERSION_14	1

#define TYPE_MSM_HSL "msm_hsl"
#define MSM_HSL(obj) OBJECT_CHECK(MsmHslCtx, (obj), TYPE_MSM_HSL)

#define UARTDM_CSR_115200 0xFF
#define UARTDM_CSR_57600  0xEE
#define UARTDM_CSR_38400  0xDD
#define UARTDM_CSR_28800  0xCC
#define UARTDM_CSR_19200  0xBB
#define UARTDM_CSR_14400  0xAA
#define UARTDM_CSR_9600   0x99
#define UARTDM_CSR_7200   0x88
#define UARTDM_CSR_4800   0x77
#define UARTDM_CSR_3600   0x66
#define UARTDM_CSR_2400   0x55
#define UARTDM_CSR_1200   0x44
#define UARTDM_CSR_600    0x33
#define UARTDM_CSR_300    0x22
#define UARTDM_CSR_150    0x11
#define UARTDM_CSR_75     0x00

#define UARTDM_SR_RX_BREAK_BMSK	        1<<6
#define UARTDM_SR_PAR_FRAME_BMSK	1<<5
#define UARTDM_SR_OVERRUN_BMSK		1<<4
#define UARTDM_SR_TXEMT_BMSK		1<<3
#define UARTDM_SR_TXRDY_BMSK		1<<2
#define UARTDM_SR_RXRDY_BMSK		1<<0

#define UARTDM_ISR_TX_READY_BMSK	BIT(7)
#define UARTDM_ISR_CURRENT_CTS_BMSK	BIT(6)
#define UARTDM_ISR_DELTA_CTS_BMSK	BIT(5)
#define UARTDM_ISR_RXLEV_BMSK		BIT(4)
#define UARTDM_ISR_RXSTALE_BMSK		BIT(3)
#define UARTDM_ISR_RXBREAK_BMSK		BIT(2)
#define UARTDM_ISR_RXHUNT_BMSK		BIT(1)
#define UARTDM_ISR_TXLEV_BMSK		BIT(0)

void msm_hsl_set_baudrate(MsmHslCtx* s);
void msm_hsl_update_termio_params(MsmHslCtx* s);
void msm_hsl_fifo_tx(MsmHslCtx* s,int value);
void msm_hsl_realize_core(MsmHslCtx *s, Error **errp);
MsmHslCtx* msm_hsl_init(MemoryRegion *,hwaddr,qemu_irq, int,CharDriverState *);

void msm_hsl_set_baudrate(MsmHslCtx* s)
{
uint32_t baud_code = s->csr;

    switch(baud_code)
    {
    case UARTDM_CSR_75:
	s->speed = 300;
	break;
    case UARTDM_CSR_150:
	s->speed = 600;
	break;
    case UARTDM_CSR_300:
	s->speed = 1200;
	break;
    case UARTDM_CSR_600:
	s->speed = 2400;
	break;
    case UARTDM_CSR_1200:
	s->speed = 4800;
	break;
    case UARTDM_CSR_2400:
	s->speed = 9600;
	break;
    case UARTDM_CSR_3600:
	s->speed = 14400;
	break;
    case UARTDM_CSR_4800:
	s->speed = 19200;
	break;
    case UARTDM_CSR_7200:
	s->speed = 28800;
	break;
    case UARTDM_CSR_9600:
	s->speed = 38400;
	break;
    case UARTDM_CSR_14400:
	s->speed = 57600;
	break;
    case UARTDM_CSR_28800:
	s->speed = 115200;
	break;
    case UARTDM_CSR_57600:
	s->speed = 230400;
	break;
    case UARTDM_CSR_115200:
	s->speed = 460800;
	break;
    default:
	s->speed = 921600; /* ... 4000000*/
	break;
    }
};

void msm_hsl_update_termio_params(MsmHslCtx* s)
{
int parity, data_bits, stop_bits, frame_size;
QEMUSerialSetParams ssp;
uint32_t mr2 = s->mr2;

    frame_size = 1;

    // Parity
    if((mr2&0x3) == 0)
	parity = 'N';
    else if ((mr2&0x3) == 1)
    {
	parity = 'O';
   	frame_size++;
    }
    else if ((mr2&0x3) == 2)
    {
 	parity = 'E';
   	frame_size++;
    }
    else if ((mr2&0x3) == 3)
	parity = 'S';

    // Stop Bit
    if((((mr2&0xc) >> 2)&0x3) == 3)
	stop_bits = 2;
    else
	stop_bits = 1;

    // Bits per Char
    data_bits = ((mr2&0x30)>>4) + 5;
    frame_size += data_bits + stop_bits;

    // Get Baud Rate

    ssp.speed = s->speed;
    ssp.parity = parity;
    ssp.data_bits = data_bits;
    ssp.stop_bits = stop_bits;

    s->tx_timeout = (NANOSECONDS_PER_SECOND / s->speed) * frame_size;

    qemu_chr_fe_ioctl(s->chr, CHR_IOCTL_SERIAL_SET_PARAMS, &ssp);
    //fprintf(stderr,"speed=%d parity=%c data=%d stop=%d\n",s->speed, parity, data_bits, stop_bits);
}

static void msm_hsl_uart_update_t_status(MsmHslCtx *s)
{
uint32_t flags;

    if(!s->tx_count)
    {
	s->sr  |= UARTDM_SR_TXRDY_BMSK;
    	s->isr |= UARTDM_ISR_TX_READY_BMSK;
    }
    else
    {
	s->sr &= ~UARTDM_SR_TXEMT_BMSK;
    	s->isr &= ~UARTDM_ISR_TX_READY_BMSK;
    }

    flags = s->imr & s->isr;

    qemu_set_irq(s->irq,(flags != 0));
}

static void msm_hsl_uart_update_r_status(MsmHslCtx *s)
{
uint32_t flags;

    flags = s->imr & s->isr;

    qemu_set_irq(s->irq,(flags != 0));
}

static void msm_hsl_read_rx_fifo(MsmHslCtx *s, uint32_t *c)
{
int count = (s->rx_count>4)?4:s->rx_count;
int i;

    for(i=0;i<count;i++)
    {
        uint32_t rx_rpos = (RX_FIFO_SIZE + s->rx_wpos - s->rx_count) % RX_FIFO_SIZE;
        *c |= (s->rx_fifo[rx_rpos] << (i*8));
        s->rx_count--;
    	s->sr  |= UARTDM_SR_RXRDY_BMSK;
    	s->isr |= UARTDM_ISR_RXSTALE_BMSK;
    	s->misr |= UARTDM_ISR_RXSTALE_BMSK;
    }

    s->rx_total_snap -= count;

    if (s->chr)
    {
	qemu_chr_accept_input(s->chr);
    }

    if(!s->rx_count)
    {
    	s->isr &= ~UARTDM_ISR_RXSTALE_BMSK;
    	s->misr &= ~UARTDM_ISR_RXSTALE_BMSK;
    	s->sr  &= ~UARTDM_SR_RXRDY_BMSK;
    }

    msm_hsl_uart_update_r_status(s);
}

static void fifo_trigger_update(void *opaque)
{
MsmHslCtx* s = (MsmHslCtx *)opaque;

    if(s->rx_count)
    {
    	s->sr  |= UARTDM_SR_RXRDY_BMSK;
    	s->misr |= UARTDM_ISR_RXLEV_BMSK;
    	s->isr |= UARTDM_ISR_RXLEV_BMSK;

    	msm_hsl_uart_update_r_status(s);
    }
}

static void msm_hsl_write_rx_fifo(void *opaque, const uint8_t *buf, int size)
{
MsmHslCtx *s = (MsmHslCtx *)opaque;
    //uint64_t new_rx_time = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    int i;

    if (!(s->cr & (1<<0)))
    	return;

    if (s->rx_count == RX_FIFO_SIZE)
    	s->sr  |= UARTDM_SR_OVERRUN_BMSK;
    else
    {
        for (i = 0; i < size; i++)
	{
            s->rx_fifo[s->rx_wpos] = buf[i];
            s->rx_wpos = (s->rx_wpos + 1) % RX_FIFO_SIZE;
            s->rx_count++;
        }
//        timer_mod(s->fifo_trigger_handle, new_rx_time + (s->tx_timeout * 4));

  	s->sr  |= UARTDM_SR_RXRDY_BMSK;
    }

    s->rx_total_snap += size;

    s->isr |= UARTDM_ISR_RXSTALE_BMSK;
    s->misr |= UARTDM_ISR_RXSTALE_BMSK;

    msm_hsl_uart_update_r_status(s);
}


static gboolean msm_hsl_uart_xmit(GIOChannel *chan, GIOCondition cond,void *opaque)
{
MsmHslCtx *s = opaque;
int ret;

    /* instant drain the fifo when there's no back-end */
    if (!s->chr) {
        s->tx_count = 0;
        return FALSE;
    }

    if (!s->tx_count) {
        return FALSE;
    }

    ret = qemu_chr_fe_write(s->chr, s->tx_fifo, s->tx_count);
    s->tx_count -= ret;

    memmove(s->tx_fifo, s->tx_fifo + ret, s->tx_count);

    if (s->tx_count)
    {
        int r = qemu_chr_fe_add_watch(s->chr, G_IO_OUT|G_IO_HUP, msm_hsl_uart_xmit, s);
        assert(r);
    }

    msm_hsl_uart_update_t_status(s);

    return FALSE;
}

void msm_hsl_fifo_tx(MsmHslCtx* s,int value)
{
int i;
int tx_count = (s->ncf_tx>4)?4:s->ncf_tx;
int offset = 0;
char* pchar = (char*)&value;

    for(i=0;i<tx_count;i++)
    {
    	s->tx_fifo[s->tx_count++] = *pchar;
    	pchar++;
    	offset++;
    }

    //fprintf(stderr,"%s",(char*)&value);

    s->ncf_tx -= offset;

    msm_hsl_uart_xmit(NULL, G_IO_OUT, s);
}

static void msm_hsl_reset_rx(MsmHslCtx *s)
{
    s->rx_wpos = 0;
    s->rx_count = 0;
    if (s->chr)
        qemu_chr_accept_input(s->chr);
}

static void msm_hsl_reset_tx(MsmHslCtx *s)
{
    s->tx_count = 0;
}

static void msm_hsl_start_break(MsmHslCtx *s)
{
int break_enabled = 1;

    if (s->chr)
        qemu_chr_fe_ioctl(s->chr, CHR_IOCTL_SERIAL_SET_BREAK, &break_enabled);
}

static void msm_hsl_update_ctrl(MsmHslCtx *s)
{
    if (s->cr & RESET_TX)
    {
        msm_hsl_reset_tx(s);
	s->cr &= ~RESET_TX;
    }
    if (s->cr & RESET_RX)
    {
        msm_hsl_reset_rx(s);
	s->cr &= ~RESET_RX;
    }

    if ((s->cr & START_BREAK) && !(s->cr & STOP_BREAK))
    {
        msm_hsl_start_break(s);
    }
}

// HW flow control UARTDM_MR1_CTS_CTL_BMSK | UARTDM_MR1_RX_RDY_CTL_BMSK
// RTS Set UARTDM_MR1_RX_RDY_CTL_BMSK
// MR2:: Loopback RFR_N of RX channel and CTS_N of TX channel (s->mr2 & 0x80)
// IMR = UARTDM_ISR_RXSTALE_BMSK | UARTDM_ISR_DELTA_CTS_BMSK | UARTDM_ISR_RXLEV_BMSK | UARTDM_ISR_RXBREAK_BMSK
// UARTDM_ISR_TXLEV_BMSK UARTDM_ISR_TX_READY_BMSK
// SR: UARTDM_SR_TXEMT_BMSK
// ISR UARTDM_ISR_TX_READY_BMSK

static void msm_hsl_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)
{
MsmHslCtx* s = (MsmHslCtx *)opaque;

    switch((uint32_t)offset)
    {
    case UARTDM_MR1_REG:
	s->mr1 = value;
	/* Get RFR level0 : UARTDM_MR1_AUTO_RFR_LEVEL0_BMSK*/
	if(value & 0x3f)
	{
		s->rfr_level = value&0x3f;
	}
	if(!s->imr) {
		s->ncf_tx = 1;
		msm_hsl_fifo_tx(s,value);
    		qemu_set_irq(s->irq,1);
	}
	break;
    case UARTDM_MR2_REG:
	s->mr2 = value;
	s->isr &= ~(UARTDM_ISR_CURRENT_CTS_BMSK | UARTDM_ISR_DELTA_CTS_BMSK);
	msm_hsl_update_termio_params(s);
	break;
    case UARTDM_IMR_REG:
	s->imr = value;
	if(s->imr & UARTDM_ISR_TXLEV_BMSK)
	{
		s->misr |= UARTDM_ISR_TXLEV_BMSK;
    		s->isr |= UARTDM_ISR_TXLEV_BMSK;
	}
	else
	{
		s->misr &= ~UARTDM_ISR_TXLEV_BMSK;
    		s->isr &= ~UARTDM_ISR_TXLEV_BMSK;
	}
	msm_hsl_uart_update_t_status(s);
	break;
    case UARTDM_CR_REG:
	s->cr |= value;
	msm_hsl_update_ctrl(s);
	break;
    case UARTDM_CSR_REG:
	s->csr = value;
	msm_hsl_set_baudrate(s);
	break;
    case UARTDM_IPR_REG:
	s->ipr = value;
	break;
    case UARTDM_TFWR_REG:
	s->tfwr = value; /* Tx fifo threshold */
	break;
    case UARTDM_RFWR_REG:
	s->rfwr = value; /* Rx fifo threshold */
	break;
    case UARTDM_TF_REG:
	/*Check if transmitter is enable in CR and ISR TX enabled in IMR */
	if ((s->cr & (1<<2)))
	{
		// Normal Mode
		msm_hsl_fifo_tx(s,value);
	}
        // Update ******
	break;
    case UARTDM_DMRX_REG:
	s->dmrx = value;
	break;
    case UARTDM_NCF_TX_REG:
	//fprintf(stderr,"ncf_tx=%d\n",value);
	s->ncf_tx = value;
	break;
    default:
 	qemu_log_mask(LOG_GUEST_ERROR,"msm_hsl_write: Bad offset %x\n",(int)offset);
	break;
    }
}

static uint64_t msm_hsl_read(void *opaque, hwaddr offset, unsigned size)
{
MsmHslCtx *s = (MsmHslCtx *)opaque;
uint32_t ret = 0;

    switch((uint32_t)offset)
    {
    case UARTDM_MR1_REG:
	ret = s->mr1;
	break;
    case UARTDM_MR2_REG:
	ret = s->mr2;
	break;
    case UARTDM_SR_REG:
	ret = s->sr;
	break;
    case UARTDM_ISR_REG:
	ret = s->isr;
	break;
    case UARTDM_RX_TOTAL_SNAP_REG:
	ret = s->rx_total_snap;
	break;
    case UARTDM_RF_REG:
	s->rf = 0;
	if ((s->cr & (1<<0)))
	{
		msm_hsl_read_rx_fifo(s,&(s->rf));
	}
	ret = s->rf;
	break;
    case UARTDM_MISR_REG:
	ret = s->misr;
	break;
    case UARTDM_NCF_TX_REG:
	ret = s->ncf_tx;
	break;
    case UARTDM_RFWR_REG:
	ret = s->rfwr;
	break;
    case UARTDM_HS:
        ret = 0x80;
        break;
    default:
 	qemu_log_mask(LOG_GUEST_ERROR,"msm_hsl_read: Bad offset %x\n",(int)offset);
	break;
    }

    msm_hsl_uart_update_r_status(s);

return ret;
}

static int msm_hsl_can_receive(void *opaque)
{
MsmHslCtx *s = (MsmHslCtx *)opaque;
int ret;
	ret = s->rfwr*4 - s->rx_count;

return ret;
}

static void msm_hsl_receive(void *opaque, const uint8_t *buf, int size)
{
    //fprintf(stderr,"MSM HSL UART :: Data received\n");
    msm_hsl_write_rx_fifo(opaque, buf, size);
}

static void msm_hsl_event(void *opaque, int event)
{
    //if (event == CHR_EVENT_BREAK)
	//fprintf(stderr,"MSM HSL UART :: Event received\n");
}

static const MemoryRegionOps pl011_ops = {
    .read = msm_hsl_read,
    .write = msm_hsl_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static const VMStateDescription vmstate_msm_hsl = {
    .name = "msm_hsl",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(misr, MsmHslCtx),
        VMSTATE_UINT32(dmrx, MsmHslCtx),
        VMSTATE_UINT32(mr1, MsmHslCtx),
        VMSTATE_UINT32(mr2, MsmHslCtx),
        VMSTATE_UINT32(imr, MsmHslCtx),
        VMSTATE_UINT32(sr, MsmHslCtx),
        VMSTATE_UINT32(cr, MsmHslCtx),
        VMSTATE_UINT32(csr, MsmHslCtx),
        VMSTATE_UINT32(ipr, MsmHslCtx),
        VMSTATE_UINT32(isr, MsmHslCtx),
        VMSTATE_UINT32(rx_total_snap, MsmHslCtx),
        VMSTATE_UINT32(rfwr, MsmHslCtx),
        VMSTATE_UINT32(tfwr, MsmHslCtx),
        VMSTATE_UINT32(rf, MsmHslCtx),
        VMSTATE_UINT32(tf, MsmHslCtx),
        VMSTATE_UINT32(ncf_tx, MsmHslCtx),
        VMSTATE_UINT32(rfr_level, MsmHslCtx),
        VMSTATE_UINT32(speed, MsmHslCtx),
        VMSTATE_UINT32(int_enabled, MsmHslCtx),
        VMSTATE_UINT32(int_level, MsmHslCtx),
        VMSTATE_UINT8_ARRAY(tx_fifo, MsmHslCtx, 256),
        VMSTATE_UINT8_ARRAY(rx_fifo, MsmHslCtx, 256),
        VMSTATE_UINT32(rx_count, MsmHslCtx),
        VMSTATE_UINT32(tx_count, MsmHslCtx),
        VMSTATE_END_OF_LIST()
    }
};

static const MemoryRegionOps msm_hsl_mm_ops = {
   .read = msm_hsl_read,
   .write = msm_hsl_write,
   .endianness = DEVICE_LITTLE_ENDIAN,
};

/* There's data in recv_fifo and s->rbr has not been read for 4 char transmit times */
#if 0
static void fifo_timeout_int (void *opaque) {
/*    SerialState *s = opaque;
    if (s->recv_fifo.num) {
        s->timeout_ipending = 1;
        serial_update_irq(s);
    }*/
}
#endif

static void msm_hsl_reset(void *opaque)
{
MsmHslCtx *s = (MsmHslCtx *)opaque;

    s->vid 	= UARTDM_VERSION_11_13;

    s->mr1 	= 0;
    s->mr2 	= 0;
    s->sr 	= UARTDM_SR_TXEMT_BMSK;
    s->isr 	= UARTDM_ISR_CURRENT_CTS_BMSK | UARTDM_ISR_DELTA_CTS_BMSK;
    s->rx_total_snap = 0;
    s->ncf_tx = 0;
    s->txfs 	= 0;
    s->rxfs 	= 0;
    s->rf 	= 0;
    s->rfwr 	= 0;

    msm_hsl_reset_rx(s);
    msm_hsl_reset_tx(s);

    msm_hsl_uart_update_t_status(s);
}

void msm_hsl_realize_core(MsmHslCtx *s, Error **errp)
{
    if (!s->chr) {
        error_setg(errp, "Can't create MSM HSL device, empty char device");
        return;
    }

    s->fifo_trigger_handle = timer_new_ns(QEMU_CLOCK_VIRTUAL,(QEMUTimerCB *)fifo_trigger_update,s);
    s->tx_timeout = (NANOSECONDS_PER_SECOND/9600)*10;

    qemu_register_reset(msm_hsl_reset, s);
    qemu_chr_add_handlers(s->chr, msm_hsl_can_receive, msm_hsl_receive, msm_hsl_event, s);
    msm_hsl_reset(s);
}

MsmHslCtx* msm_hsl_init(MemoryRegion *address_space,hwaddr base,qemu_irq irq, int baudbase,CharDriverState *chr)
{
MsmHslCtx *s;
Error *err = NULL;

    s = g_malloc0(sizeof(MsmHslCtx));

    s->irq = irq;
    s->speed = baudbase;
    s->chr = chr;

    msm_hsl_realize_core(s, &err);
    if (err != NULL) {
        error_report("%s", error_get_pretty(err));
        error_free(err);
        exit(1);
    }
    vmstate_register(NULL, base, &vmstate_msm_hsl, s);

    memory_region_init_io(&s->iomem, NULL, &msm_hsl_mm_ops, s, "msm_hsl", 0x1000);
    memory_region_add_subregion(address_space, base, &s->iomem);

return s;
}

