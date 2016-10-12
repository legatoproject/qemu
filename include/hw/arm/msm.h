#ifndef MSM_H
#define MSM_H

#include <qemu/typedefs.h>
#include <exec/hwaddr.h>
#include <hw/irq.h>

void msm9x15_smd_init(MemoryRegion *memory,hwaddr base,qemu_irq irq,uint32_t size);
void msm9x15_smsm_init(MemoryRegion *memory,hwaddr base,qemu_irq irq,uint32_t size);

void msm9x40_smd_init(MemoryRegion *memory,hwaddr base,qemu_irq irq,uint32_t size);
void msm9x40_smsm_init(MemoryRegion *memory,hwaddr base,qemu_irq irq,uint32_t size);
void msm9x40_smem_targ_info_init(MemoryRegion *memory,hwaddr base,qemu_irq irq,uint32_t size);
void msm9x40_smem_aux1_init(MemoryRegion *memory,hwaddr base,qemu_irq irq,uint32_t size);

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
    CharDriverState *chr;
    qemu_irq irq;
    const unsigned char *id;
}MsmHslCtx;

MsmHslCtx* msm_hsl_init(MemoryRegion *address_space,hwaddr base,qemu_irq irq, int baudbase,CharDriverState *chr);
MsmHslCtx* msm_hs_init(MemoryRegion *address_space,hwaddr base,qemu_irq irq, int baudbase,CharDriverState *chr);

#endif /* MSM_H */
