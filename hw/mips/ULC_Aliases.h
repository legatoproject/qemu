#ifndef __ULC_ALIASES_H__
#define __ULC_ALIASES_H__

//#include "ram_map.h"

// --------------------------------------------------------------------------------------------
// NAME          : What5_h.asm
// AUTHOR        : FSH
// DESCRIPTION   : Common header file for all asm files
// --------------------------------------------------------------------------------------------
// Reference :   Registers Description

// Modfication History :
//   Date   | Author | Modification
// ---------+--------+-------------------------------------------------------------------------
// 20/04/05 |  fsh   | Creation
// 29/04/05 |  bma   | Renamed definitions for RESET interrupt in CPU2DSP_INT shared reg.
// 10/05/05 |  jch   | added DSMA_WAIT_STATE  register corresponding to sw_cfg asic side
// 19/04/06 |  fva   | added melody shared regs according to DpramSReg_ES4_03.xls
// 30/03/11 |  kbc   | Added DSMA transfer requests in SR_CPUTODSP_INT and removed PAGE requests
//          |        | Added DSMA hardware registers from 0xA020 to 0xA036
//          |        | + bitfields definitions.
// 22/07/11 |  fbo   | Added CIPHER block interface definitions
// 17/04/12 |  kbc   | Added new register @ 0x0076 ACCESS_TURN for semaphore
// --------------------------------------------------------------------------------------------

// Defined Registers:
//       - Shared registers
//       - Frame Counters
//       - interrupt controller registers
//       - DSMA config register
//       - PCM port registers
//       - Radio Port registers
//       - Viterbi Copro registers
//       - Metric Copro registers
//       - Cifire Copro registers
//       - FFT Copro registers
//       - LMS Copro registers
//       - DSP Control registers
//       - Cipher Copro registers
// --------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------
// Teak Status / Mod Registers definitions
// --------------------------------------------------------------------------------------------
// mod0
#define    TEAK_SATURATION         0x0003  // set -> disable, rst -> enable
#define    TEAK_SATA               0x0002  // Accumulator saturation : 0 : enabled, 1 disabled (def = 1)
#define    TEAK_SAT                0x0001  // Transfer saturation control : 0 : enabled, 1 : disabled (def = 0)

// mod1
#define    TEAK_CMD                0x2000  // Compatible modulo
#define    TEAK_EPJ                0x8000  // External pointer modi J
#define    TEAK_EPI                0x4000  // External pointer modi I
#define    TEAK_STEP16             0x1000  // Step 16 bit
#define    TEAK_DATA_PAGE_MASK     0x00ff  // Page mask

// mod3
#define    MEMORY_PAGE_1           0x0000  // Data Memory Page 1 => Modem
#define    MEMORY_PAGE_2           0x0010  // Data Memory Page 2 => Modem
#define    MEMORY_PAGE_3           0x0020  // Data Memory Page 3 => Applications
#define    MEMORY_PAGE_4           0x0030  // Data Memory Page 4 => Applications
#define    MEMORY_PAGE_5           0x0040  // Data Memory Page 5 => Applications (AMR-WB)
#define    MEMORY_PAGE_MASK        0xFF8F  // Clear Bits[6:4]
#define    MEMORY_PAGE_FIELD       0x0070  // Field corresponding to OU2 OU3 OU4
#define    TEAK_INT_ENABLE         0x0080  // set -> enable, rst -> disable
#define    MASK_TEAK_INT0          0x0100  // set -> enable, rst -> disable
#define    MASK_TEAK_INT1          0x0200  // set -> enable, rst -> disable
#define    MASK_TEAK_INT2          0x0400  // set -> enable, rst -> disable
#define    MASK_TEAK_VINT          0x0800  // set -> enable, rst -> disable
#define    MASK_TEAK_CPC           0x4000  // Compatible Program Counter Mask
#define    TEAK_CREP               0x8000  // Compatible Repeat
#define    TEAK_CCNTA              0x2000  // Compatible Swap accumulator

// --------------------------------------------------------------------------------------------
// SHARED REGISTERS
// --------------------------------------------------------------------------------------------
#define    SHARED_REGS_BASE       SHARED_REGISTERS
#define    SHARED_REGS_SIZE           192

#define    SR_CPUTODSP_INT            SHARED_REGS_BASE               //size 1 word
#define    SR_DSP_EVENT_SOURCE        SHARED_REGS_BASE + 0x0002      //size 3 word
#define    SR_GPEB                    SHARED_REGS_BASE + 0x000A      //size 2 word
//#define    WORD_NUMBER             SHARED_REGS_BASE + 0x000C      ;size 1 word  ; FBO: no longer used
#define    SR_TX_WIN_MODE_0           SHARED_REGS_BASE + 0x000E      //size 1 word
#define    SR_TX_WIN_MODE_1           SHARED_REGS_BASE + 0x000F      //size 1 word

#define    SR_SWDER0                  SHARED_REGS_BASE + 0x0010      //size 1 word
#define    SR_ALGO_CONTROL            SHARED_REGS_BASE + 0x0010      //size 1 word

#define    SR_SWDER1                  SHARED_REGS_BASE + 0x0011      //size 1 word
#define    SR_GPRS_CONTROL            SHARED_REGS_BASE + 0x0011

#define    SR_SWDER2                  SHARED_REGS_BASE + 0x0012      //size 1 word
#define    SR_MS_RX_BITMAPS           SHARED_REGS_BASE + 0x0012

#define    SR_SWDER3                  SHARED_REGS_BASE + 0x0014      //size 1 word
#define    SR_MS_TX_BITMAPS           SHARED_REGS_BASE + 0x0014

#define    SR_SWDER4                  SHARED_REGS_BASE + 0x0015      //size 1 word
#define    SR_SOFT_COUNTERS           SHARED_REGS_BASE + 0x0015
//#define    SR_NBCCH_RX_COUNTER        SHARED_REGS_BASE + 0x0015      ; no monger used: NBCCH Rx Burst Counter modulo 4
#define    SR_NBCCH_TX_COUNTER        SHARED_REGS_BASE + 0x0015      // NBCCH Tx Burst Counter modulo 4

#define    SR_SWDER5                  SHARED_REGS_BASE + 0x0016      //size 1 word
#define    SR_TX_FMT_BURST0           SHARED_REGS_BASE + 0x0016      // for burst #0

#define    SR_SWDER6                  SHARED_REGS_BASE + 0x0017      //size 1 word
#define    SR_TX_FMT_BURST1           SHARED_REGS_BASE + 0x0017      // for burst #1

#define    SR_SWDER7                  SHARED_REGS_BASE + 0x0018      //size 1 word
#define    SR_TX_FMT_BURST2           SHARED_REGS_BASE + 0x0018      // for burst #2

#define    SR_SWDER8                  SHARED_REGS_BASE + 0x0019      //size 1 word
#define    SR_TX_FMT_BURST3           SHARED_REGS_BASE + 0x0019      // for burst #3

#define    SR_SWDER9                  SHARED_REGS_BASE + 0x001A      //size 1 word
#define    SR_SWDER10                 SHARED_REGS_BASE + 0x001B      //size 1 word
#define    SR_SOFT_TRACE_0            SHARED_REGS_BASE + 0x001C      //size 1 word
#define    SR_SOFT_TRACE_1            SHARED_REGS_BASE + 0x001D      //size 1 word
#define    SR_PATCH_CTRL              SHARED_REGS_BASE + 0x001E      //size 1 word
#define    SR_PATCH1_BREAK_ADD        SHARED_REGS_BASE + 0x0020      //size 2 word
#define    SR_PATCH2_BREAK_ADD        SHARED_REGS_BASE + 0x0022      //size 2 word
#define    SR_PATCH3_BREAK_ADD        SHARED_REGS_BASE + 0x0024      //size 2 word
#define    SR_PATCH4_BREAK_ADD        SHARED_REGS_BASE + 0x0026      //size 2 word
#define    SR_PATCH5_BREAK_ADD        SHARED_REGS_BASE + 0x0028      //size 2 word
#define    SR_PATCH6_BREAK_ADD        SHARED_REGS_BASE + 0x002A      //size 2 word
#define    SR_PATCH7_BREAK_ADD        SHARED_REGS_BASE + 0x002C      //size 2 word
#define    SR_PATCH8_BREAK_ADD        SHARED_REGS_BASE + 0x002E      //size 2 word
#define    SR_PATCH9_BREAK_ADD        SHARED_REGS_BASE + 0x0030      //size 2 word
#define    SR_PATCH10_BREAK_ADD       SHARED_REGS_BASE + 0x0032      //size 2 word
#define    SR_PATCH11_BREAK_ADD       SHARED_REGS_BASE + 0x0034      //size 2 word
#define    SR_PATCH12_BREAK_ADD       SHARED_REGS_BASE + 0x0036      //size 2 word
#define    SR_PATCH13_BREAK_ADD       SHARED_REGS_BASE + 0x0038      //size 2 word
#define    SR_PATCH14_BREAK_ADD       SHARED_REGS_BASE + 0x003A      //size 2 word
#define    SR_PATCH15_BREAK_ADD       SHARED_REGS_BASE + 0x003C      //size 2 word
#define    SR_PATCH16_BREAK_ADD       SHARED_REGS_BASE + 0x003E      //size 2 word
#define    SR_PATCH1_INST1            SHARED_REGS_BASE + 0x0040      //size 1 word
#define    SR_PATCH1_INST2            SHARED_REGS_BASE + 0x0041      //size 1 word
#define    SR_PATCH2_INST1            SHARED_REGS_BASE + 0x0042      //size 1 word
#define    SR_PATCH2_INST2            SHARED_REGS_BASE + 0x0043      //size 1 word
#define    SR_PATCH3_INST1            SHARED_REGS_BASE + 0x0044      //size 1 word
#define    SR_PATCH3_INST2            SHARED_REGS_BASE + 0x0045      //size 1 word
#define    SR_PATCH4_INST1            SHARED_REGS_BASE + 0x0046      //size 1 word
#define    SR_PATCH4_INST2            SHARED_REGS_BASE + 0x0047      //size 1 word
#define    SR_PATCH5_INST1            SHARED_REGS_BASE + 0x0048      //size 1 word
#define    SR_PATCH5_INST2            SHARED_REGS_BASE + 0x0049      //size 1 word
#define    SR_PATCH6_INST1            SHARED_REGS_BASE + 0x004A      //size 1 word
#define    SR_PATCH6_INST2            SHARED_REGS_BASE + 0x004B      //size 1 word
#define    SR_PATCH7_INST1            SHARED_REGS_BASE + 0x004C      //size 1 word
#define    SR_PATCH7_INST2            SHARED_REGS_BASE + 0x004D      //size 1 word
#define    SR_PATCH8_INST1            SHARED_REGS_BASE + 0x004E      //size 1 word
#define    SR_PATCH8_INST2            SHARED_REGS_BASE + 0x004F      //size 1 word
#define    SR_PATCH9_INST1            SHARED_REGS_BASE + 0x0050      //size 1 word
#define    SR_PATCH9_INST2            SHARED_REGS_BASE + 0x0051      //size 1 word
#define    SR_PATCH10_INST1           SHARED_REGS_BASE + 0x0052      //size 1 word
#define    SR_PATCH10_INST2           SHARED_REGS_BASE + 0x0053      //size 1 word
#define    SR_PATCH11_INST1           SHARED_REGS_BASE + 0x0054      //size 1 word
#define    SR_PATCH11_INST2           SHARED_REGS_BASE + 0x0055      //size 1 word
#define    SR_PATCH12_INST1           SHARED_REGS_BASE + 0x0056      //size 1 word
#define    SR_PATCH12_INST2           SHARED_REGS_BASE + 0x0057      //size 1 word
#define    SR_PATCH13_INST1           SHARED_REGS_BASE + 0x0058      //size 1 word
#define    SR_PATCH13_INST2           SHARED_REGS_BASE + 0x0059      //size 1 word
#define    SR_PATCH14_INST1           SHARED_REGS_BASE + 0x005A      //size 1 word
#define    SR_PATCH14_INST2           SHARED_REGS_BASE + 0x005B      //size 1 word
#define    SR_PATCH15_INST1           SHARED_REGS_BASE + 0x005C      //size 1 word
#define    SR_PATCH15_INST2           SHARED_REGS_BASE + 0x005D      //size 1 word
#define    SR_PATCH16_INST1           SHARED_REGS_BASE + 0x005E      //size 1 word
#define    SR_PATCH16_INST2           SHARED_REGS_BASE + 0x005F      //size 1 word

//#define   PRIORITY_CTRL           SHARED_REGS_BASE + 0x0060      ;size 1 word ; FBO: handled by CPU

#define    SR_SHARED0                 SHARED_REGS_BASE + 0x0070      //size 1 word
#define    SR_SOFT_TASK_CONTROL_0     SHARED_REGS_BASE + 0x0070      //size 1 word

#define    SR_SHARED1                 SHARED_REGS_BASE + 0x0071      //size 1 word
#define    SR_SOFT_TASK_CONTROL_1     SHARED_REGS_BASE + 0x0071      //size 1 word

#define    SR_SHARED2                 SHARED_REGS_BASE + 0x0072      //size 1 word
#define    SR_BURST_STATUS            SHARED_REGS_BASE + 0x0072      //size 1 word

#define    SR_SHARED3                 SHARED_REGS_BASE + 0x0073      //size 1 word
#define    SR_BUFFER_CONTROL          SHARED_REGS_BASE + 0x0073      //size 1 word

#define    SR_SHARED4                 SHARED_REGS_BASE + 0x0074      //size 1 word
#define    SR_BLOCK_STATUS            SHARED_REGS_BASE + 0x0074      //size 1 word

#define    SR_SHARED5                 SHARED_REGS_BASE + 0x0075      //size 1 word
#define    SR_BFI_STATUS              SHARED_REGS_BASE + 0x0075      //size 1 word

#define    SR_SHARED6                 SHARED_REGS_BASE + 0x0076      //size 1 word
#define    SR_ACCESS_TURN             SHARED_REGS_BASE + 0x0076      //size 1 word
// KBC: for SR_BUFFER_CONTROL & SR_BURST_STATUS & SR_BLOCK_STATUS semaphore
#define    SR_SHARED7                 SHARED_REGS_BASE + 0x0077      //size 1 word
//#define    AUDIO_INIT_CONTROL      SHARED7                                             ; FBO: no longer used

#define    SR_SHARED8                 SHARED_REGS_BASE + 0x0078      //size 1 word
#define    SR_SHARED9                 SHARED_REGS_BASE + 0x0079      //size 1 word
#define    SR_SHARED10                SHARED_REGS_BASE + 0x007A      //size 1 word
#define    SR_SHARED11                SHARED_REGS_BASE + 0x007B      //size 1 word
#define    SR_SHARED12                SHARED_REGS_BASE + 0x007C      //size 1 word
#define    SR_SHARED13                SHARED_REGS_BASE + 0x007D      //size 1 word
#define    SR_SHARED14                SHARED_REGS_BASE + 0x007E      //size 1 word
#define    SR_SHARED15                SHARED_REGS_BASE + 0x007F      //size 1 word


// --------------------------------------------------------------------------------------------
// SR_CPUTODSP_INT                      cmp
// --------------------------------------------------------------------------------------------
#define    REQ_RST_STOP            0
#define    REQ_RST_BOOT            1
#define    REQ_RST_IDLE            2
#define    DOWN_REQUEST_DPRAM      3
#define    DL_REQUEST_DSMA_1       4
#define    DL_REQUEST_DSMA_2       5
#define    UL_REQUEST_DSMA_1       6
#define    UL_REQUEST_DSMA_2       7
#define    REQ_AUDIO_INIT_FIFO     8


// --------------------------------------------------------------------------------------------
// SR_CPUTODSP_INT              RESET MODE PART
// --------------------------------------------------------------------------------------------

// Bit number definitions
#define    RESET_BIT_MODE  0                               // Reset Mode bit number

// Bit field definitions
#define    RESET_MSK_MODE  (0x3<<RESET_BIT_MODE)           // Reset Mode bit field

// Reset Mode bit number definitions
#define    RESET_MODE_BIT_BOOT     (RESET_BIT_MODE+0)      // Boot Mode bit number
#define    RESET_MODE_BIT_IDLE     (RESET_BIT_MODE+1)      // Idle Mode bit number

// Reset Mode bit fields definitions
#define    RESET_MODE_MSK_STOP     (0<<RESET_BIT_MODE)     // Stop Mode value
#define    RESET_MODE_MSK_BOOT     (1<<RESET_BIT_MODE)     // Boot Mode value
#define    RESET_MODE_MSK_IDLE     (2<<RESET_BIT_MODE)     // Idle Mode value



// --------------------------------------------------------------------------------------------
// SR_DSP_EVENT_SOURCE DEFINITIONS  tstb
// --------------------------------------------------------------------------------------------
// Never used
#define    RESET_INT_BIT                   0
#define    EVT_SRC_EXTENSION_INT_BIT       0
#define    CPU_TO_DSP_INT_BIT              1
#define    TX_SOURCE_INT_BIT               2
#define    RX_SOURCE_INT_BIT               3
#define    FB_DETECTION_BIT                4
#define    NB_RX_0_BIT                     5
#define    NB_RX_1_BIT                     6
#define    NB_RX_2_BIT                     7
#define    RX_BLOCK_PROCESS_BIT            8
#define    TX_BLOCK_PROCESS_BIT            9
#define    QUICK_USF_ESTIM_BIT             10
#define    PWR_MEASURE_BIT                 11
#define    ERROR_DETECTED_BIT              12
#define    QUICK_RATSCCH_DEC_IT_BIT        13
#define    NBCCH_FAST_DEC_OK_IT_BIT        14

// --------------------------------------------------------------------------------------------
// SR_DSP_EVENT_SOURCE DEFINITIONS  set/rst
// --------------------------------------------------------------------------------------------
#define    RESET_INT_COMPLETE      0x0001 // constant kept here to remember that reset IT was the
#define    EVT_SRC_EXTENDED_INT    0x0001 // initial definition of this bit
#define    CPU_INT_COMPLETE        0x0002
#define    TX_SOURCE_INT_COMPLETE  0x0004
#define    RX_SOURCE_INT_COMPLETE  0x0008
#define    FB_DETECTION            0x0010
#define    NB_RX_0                 0x0020
#define    NB_RX_1                 0x0040
#define    NB_RX_2                 0x0080
#define    RX_BLOCK_PROCESS        0x0100
#define    TX_BLOCK_PROCESS        0x0200
#define    QUICK_USF_ESTIM         0x0400
#define    PWR_MEASURE_COMPLETE    0x0800
#define    ERROR_DETECTED_IT       0x1000
#define    QUICK_RATSCCH_DEC_IT    0x2000 // not used by HWL/L1
#define    NBCCH_FAST_DEC_OK_IT    0x4000

/* .MACRO _GEN_DSP_TO_CPU_INT msk, r
    mov    ##SR_DSP_EVENT_SOURCE, r
    set    ##msk, (r)
.ENDM */ //_GEN_DSP_TO_CPU_INT

/* .MACRO _GEN_DSP_TO_CPU_INT_EXTENDED msk, r
    mov    ##DP_DSP_EVT_SRC_EXTENDED, r
    set    ##msk, (r)
    mov    ##SR_DSP_EVENT_SOURCE, r
    set    ##EVT_SRC_EXTENDED_INT, (r)    // works for reset complete (RESET_INT_COMPLETE) as well
.ENDM */ //_GEN_DSP_TO_CPU_INT_EXTENDED

// --------------------------------------------------------------------------------------------
// TX_WIN_MODE_x DEFINITIONS     and
// --------------------------------------------------------------------------------------------
#define    TX_IQ_SWAP              0x0001
#define    TX_PROCESS              0x000E
#define    TX_TSC                  0x0070
#define    TX_WIN_BITMAP_MASK      0x0F00
// --------------------------------------------------------------------------------------------
// TX_WIN_MODE_x DEFINITIONS         cmp
// --------------------------------------------------------------------------------------------
#define    TX_NO_PROCESS           0x0000
#define    TX_AB_PROCESS           0x0002
#define    TX_NB_PROCESS           0x0004
#define    TX_PDCH_PROCESS    0x0006  // GPRS PDCH
#define    TX_EPDCH_PROCESS        0x0008  // TX_EDGE_PROCESSING
// --------------------------------------------------------------------------------------------
// TX_WIN_MODE_x DEFINITIONS     shfi
// --------------------------------------------------------------------------------------------
#define    TX_TSC_SHIFT            -4
#define    TX_WIN_BITMAP_SHIFT     -8

// --------------------------------------------------------------------------------------------
// SR_ALGO_CONTROL BITFIELDS                and
// --------------------------------------------------------------------------------------------
#define    CIPHER_ALGO             (7<<0)
#define    VOCODEC_ALGO            (7<<3)
#define    CHANNEL_MODE            (3<<6)
// --------------------------------------------------------------------------------------------
// SR_ALGO_CONTROL DEFINITIONS              cmp
// --------------------------------------------------------------------------------------------
#define    CIPHER_A51              (0<<0)
#define    CIPHER_A52              (1<<0)
#define    CIPHER_A53_A54          (1<<1)
#define    VOCODEC_FR              (0<<3)
#define    VOCODEC_EFR             (1<<3)
#define    VOCODEC_HR              (2<<3)
#define    VOCODEC_AMR             (4<<3)
#define    TCH_DEDICATED           (1<<6)
#define    IDLE_MODE               (2<<6)
// --------------------------------------------------------------------------------------------
// SR_ALGO_CONTROL BITS                 tstb
// --------------------------------------------------------------------------------------------
#define    VOCODER_EFR_BIT                 3
#define    VOCODER_HR_BIT                  4
#define    VOCODER_AMR_BIT                 5
#define    GSM_BIT                         6
#define    GPRS_BIT                        7
#define    NBCCH_FAST_DEC_BIT              8
#define    PDCH_FAST_ERROR_COUNT_BIT       9
#define    PDCH_TX_SWAP                    10
#define    CIR_CLEAN_BIT                   11

// --------------------------------------------------------------------------------------------
// SR_GPRS_CONTROL                              and
// --------------------------------------------------------------------------------------------
#define    ALLOC_MODE              (3<<0)
#define    RX_BLOCK_INT_BITMAP     (15<<2)
#define    TX_EXTRA_BIT_POS        (3<<6)
#define    TIME_ADVANCE            (63<<8)
// --------------------------------------------------------------------------------------------
// SR_GPRS_CONTROL                          cmp
// --------------------------------------------------------------------------------------------
#define    ALLOC_FIXED             (0<<0)
#define    ALLOC_DYN               (1<<0)
#define    ALLOC_EXT_DYN           (2<<0)
#define    RX_BLOCK_0_INT          (1<<2)
#define    RX_BLOCK_1_INT          (2<<2)
#define    RX_BLOCK_2_INT          (4<<2)
#define    RX_BLOCK_3_INT          (8<<2)
#define    TX_EXTRA_BIT_0          (0<<6)
#define    TX_EXTRA_BIT_1          (1<<6)
#define    TX_EXTRA_BIT_2          (2<<6)
#define    TX_EXTRA_BIT_3          (3<<6)
// --------------------------------------------------------------------------------------------
// GPRS CONTROL                          tstb
// --------------------------------------------------------------------------------------------
#define    RX_BLOCK_0_INT_BIT      2
#define    RX_BLOCK_1_INT_BIT      3
#define    RX_BLOCK_2_INT_BIT      4
#define    RX_BLOCK_3_INT_BIT      5
// --------------------------------------------------------------------------------------------
// GPRS CONTROL                          shfi
// --------------------------------------------------------------------------------------------
#define    RX_BLOCK_INT_SHIFT      -2
#define    TX_EXTRA_BIT_SHIFT      -6
#define    TIME_ADVANCE_SHIFT      -8

// --------------------------------------------------------------------------------------------
// SR_MS_RX_BITMAPS                         and
// --------------------------------------------------------------------------------------------
#define    RX_BITMAP               (0xFF<<0)
#define    RX_NB_BITMAP            (0xF<<0)
#define    RX_DECOD_BITMAP         (0xF<<4)
#define    RX_QUICKUSF             (0xF<<8)
#define    RX_ADJFILTER_2          (0xF<<12)
#define    RX_BITMAP_GPRS          0x0F0F
// --------------------------------------------------------------------------------------------
// SR_MS_RX_BITMAPS                         tstb
// --------------------------------------------------------------------------------------------
#define    PDCH_RX_NB_0            0
#define    PDCH_RX_NB_1            1
#define    PDCH_RX_NB_2            2
#define    PDCH_RX_NB_3            3
#define    QUICK_USF_0             8
#define    QUICK_USF_1             9
#define    QUICK_USF_2             10
#define    QUICK_USF_3             11
// --------------------------------------------------------------------------------------------
// SR_MS_RX_BITMAPS                             set/rst
// --------------------------------------------------------------------------------------------
#define    PDCH_RX_NB_0_FLAG       0x0001
#define    PDCH_RX_NB_1_FLAG       0x0002
#define    PDCH_RX_NB_2_FLAG       0x0004
#define    PDCH_RX_NB_3_FLAG       0x0008
#define    QUICK_USF_0_FLAG        0x0100
#define    QUICK_USF_1_FLAG        0x0200
#define    QUICK_USF_2_FLAG        0x0400
#define    QUICK_USF_3_FLAG        0x0800
// --------------------------------------------------------------------------------------------
// SR_MS_RX_BITMAPS                             shfi
// --------------------------------------------------------------------------------------------
#define    RX_QUICKUSF_SHIFT       -8
#define    RX_ADJFILTER_2_SHIFT    -12

// --------------------------------------------------------------------------------------------
// EDGE SR_MS_RX_BITMAPS    Structure
// --------------------------------------------------------------------------------------------
#define    SR_RX_GPRS_BITMAPS      SR_MS_RX_BITMAPS
#define    RX_DECOD_BITMAP_MASK    0x00f0
#define    RX_DECOD_BITMAP_SHFT    -4
#define    RX_BURST_BITMAP_MASK    0x000f
#define    RX_BURST_BITMAP_SHFT    0
#define    RX_QUICK_USF_MASK       0x0f00
#define    RX_QUICK_USF_SHFT       -8

// --------------------------------------------------------------------------------------------
// SR_MS_TX_BITMAPS                         shfi
// --------------------------------------------------------------------------------------------
#define    TX_NB_BITMAP_SHIFT      -0
#define    TX_AB_BITMAP_SHIFT      -4
#define    PDCH_TX_MAP_0           8
#define    PDCH_TX_MAP_1           10
#define    PDCH_TX_MAP_2           12
#define    PDCH_TX_MAP_3           14
// --------------------------------------------------------------------------------------------
// SR_MS_TX_BITMAPS                         and
// --------------------------------------------------------------------------------------------
#define    TX_BITMAP               0x00FF
#define    TX_NB_BITMAP            (0xF<<0)
#define    TX_AB_BITMAP            (0xF<<4)
#define    PDCH_TX_BLOCK_0         (3<<PDCH_TX_MAP_0)
#define    PDCH_TX_BLOCK_1         (3<<PDCH_TX_MAP_1)
#define    PDCH_TX_BLOCK_2         (3<<PDCH_TX_MAP_2)
#define    PDCH_TX_BLOCK_3         (3<<PDCH_TX_MAP_3)
// --------------------------------------------------------------------------------------------
// SR_MS_TX_BITMAPS                         tstb
// --------------------------------------------------------------------------------------------
#define    PDCH_TX_NB_0            0
#define    PDCH_TX_NB_1            1
#define    PDCH_TX_NB_2            2
#define    PDCH_TX_NB_3            3
#define    PDCH_TX_AB_0            4
#define    PDCH_TX_AB_1            5
#define    PDCH_TX_AB_2            6
#define    PDCH_TX_AB_3            7
// --------------------------------------------------------------------------------------------
// SR_MS_TX_BITMAPS                         set/rst
// --------------------------------------------------------------------------------------------
#define    PDCH_TX_NB_0_FLAG       0x0001
#define    PDCH_TX_NB_1_FLAG       0x0002
#define    PDCH_TX_NB_2_FLAG       0x0004
#define    PDCH_TX_NB_3_FLAG       0x0008
#define    PDCH_TX_AB_0_FLAG       0x0010
#define    PDCH_TX_AB_1_FLAG       0x0020
#define    PDCH_TX_AB_2_FLAG       0x0040
#define    PDCH_TX_AB_3_FLAG       0x0080

// --------------------------------------------------------------------------------------------
// EDGE SR_MS_TX_BITMAPS    Structure
// --------------------------------------------------------------------------------------------
#define    SR_TX_GPRS_BITMAPS      SR_MS_TX_BITMAPS
#define    TX_NB_BITMAP_MASK       0x000f
#define    TX_AB_BITMAP_MASK       0x00f0

// --------------------------------------------------------------------------------------------
// SR_SOFT_COUNTERS                         and
// --------------------------------------------------------------------------------------------
#define    NBCCH_RX_COUNTER_MASK   0x0007
#define    NBCCH_TX_COUNTER_MASK   0x0070
// --------------------------------------------------------------------------------------------
// SR_SOFT_COUNTERS                         shfi
// --------------------------------------------------------------------------------------------
#define    NBCCH_RX_COUNTER_SHIFT  0
#define    NBCCH_TX_COUNTER_SHIFT  -4

// --------------------------------------------------------------------------------------------
// SR_TX_FMT_BURSTx                         shfi
// --------------------------------------------------------------------------------------------
#define    TX_FMT_BIT_PRE           0
#define    TX_FMT_BIT_POST          8
// --------------------------------------------------------------------------------------------
// SR_TX_FMT_BURSTx                         and
// --------------------------------------------------------------------------------------------
#define    TX_FMT_MASK_PRE          (0xFF<<TX_FMT_BIT_PRE)
#define    TX_FMT_MASK_POST         (0xFF<<TX_FMT_BIT_POST)

// --------------------------------------------------------------------------------------------
// SR_SOFT_TRACE_0 DEFINITIONS              set/rst
// --------------------------------------------------------------------------------------------
#define    STOP_MODE_0             0x0001
#define    STOP_MODE_1             0x0002
#define    IDLE_TASK               0x0004
#define    INIT_TASK               0x0008
#define    AUDIO_TASK              0x0010  // int0
#define    RX_PROC_TASK            0x0020  // int1
#define    TX_PROC_TASK            0x0040  // int1
#define    SPEECH_ENCODE_TASK      0x0080  // int2
#define    SPEECH_DECODE_TASK      0x0100  // int2
#define    CPU_TASK                0x0200
#define    DSMA_TASK               0x0400
#define    RAM_TEST_TASK           0x0800
#define    ROM_TEST_TASK           0x1000
#define    ERROR_DETECTED          0x2000
#define    BOOT_TASK               0x4000
#define    DEBUG_TASK              0x8000  // vint (not used at this time)

// --------------------------------------------------------------------------------------------
// SR_SOFT_TRACE_1 DEFINITIONS              set/rst
// --------------------------------------------------------------------------------------------
#define    EDGE_TX_PROC_TASK       0x0001
#define    EDGE_GMSK_RX_PROC_TASK  0x0002
#define    EDGE_8PSK_RX_PROC_TASK  0x0004
#define    EDGE_RX_BLOCK_DEC_TASK  0x0008
#define    DTMF_DEC_TASK           0x0080
#define    VCP_TASK                0x0100

// --------------------------------------------------------------------------------------------
// SR_SOFT_TRACE_1 DEFINITIONS              shfi
// --------------------------------------------------------------------------------------------
#define    PAGE_SELECT_SHIFT       -4
// --------------------------------------------------------------------------------------------
// SR_SOFT_TRACE_1 DEFINITIONS              mask
// --------------------------------------------------------------------------------------------
#define    PAGE_SELCECT_MASK       0xFF8F
// --------------------------------------------------------------------------------------------
// SR_SOFT_TASK_CONTROL_0 BITFIELDS         and
// --------------------------------------------------------------------------------------------
#define    ENCRYPTION              (3<<0)
// --------------------------------------------------------------------------------------------
// SR_SOFT_TASK_CONTROL_0 DEFINITIONS       set/rst
// --------------------------------------------------------------------------------------------
#define    ENCRYPTION_ON           (1<<0)
#define    DECRYPTION_ON           (1<<1)
#define    CRC_FIRE_ON             (1<<2)
#define    VOCODING_ON             (1<<3)
#define    VODECODING_ON           (1<<4)
#define    TX_PROC_RST             (1<<5)
#define    RX_PROC_RST             (1<<6)
#define    VOCODING_RST            (1<<7)
#define    VODECODING_RST          (1<<8)
#define    ERROR_CONCEAL           (1<<9)
#define    STOP_MODE_0_ON          (1<<10)
#define    STOP_MODE_1_ON          (1<<11)
#define    TX_BUFFER_FILL          (1<<12)
// --------------------------------------------------------------------------------------------
// SR_SOFT_TASK_CONTROL_0 BITS              tstb
// --------------------------------------------------------------------------------------------
#define    ENCRYPTION_BIT          0
#define    DECRYPTION_BIT          1
#define    CRC_FIRE_BIT            2
#define    VOCODING_BIT            3
#define    VODECODING_BIT          4
#define    TX_PROC_RST_BIT         5
#define    RX_PROC_RST_BIT         6
#define    VOCODING_RST_BIT        7
#define    VODECODING_RST_BIT      8
#define    ERROR_CONCEAL_BIT       9
#define    STOP_MODE_0_BIT         10
#define    STOP_MODE_1_BIT         11
#define    TX_BUFFER_FILL_BIT      12
#define    TX_DUMMY_BURST          12

// --------------------------------------------------------------------------------------------
// SR_SOFT_TASK_CONTROL_1                   tstb
// --------------------------------------------------------------------------------------------
#define    IR_RESET                0
//#define    EDGE_RESET              1
#define    UP_RESET_BIT            2
#define    DWN_RESET_BIT           3
// --------------------------------------------------------------------------------------------
// SR_SOFT_TASK_CONTROL_1                   and
// --------------------------------------------------------------------------------------------
#define    IR_RESET_MASK           0x0001
#define    UP_RESET_MASK           0x01<<UP_RESET_BIT
#define    DWN_RESET_MASK          0x01<<DWN_RESET_BIT

// --------------------------------------------------------------------------------------------
// SR_BURST_STATUS                          set/rst
// --------------------------------------------------------------------------------------------
#define    BURST_0_OK              (1<<0)
#define    BURST_1_OK              (1<<1)
#define    BURST_2_OK              (1<<2)
#define    BURST_3_OK              (1<<3)
#define    PWR_WIN0_OK             (1<<4)
#define    PWR_WIN1_OK             (1<<5)
#define    PWR_WIN2_OK             (1<<6)
#define    PWR_WIN3_OK             (1<<7)
#define    PWR_WIN4_OK             (1<<8)
#define    PWR_WIN5_OK             (1<<9)
#define    PWR_WIN6_OK             (1<<10)
#define    PWR_WIN7_OK             (1<<11)
#define    FB_OK                   (1<<12)
#define    SB_OK                   (1<<13)

// --------------------------------------------------------------------------------------------
// SR_BUFFER_CONTROL                        tstb
// --------------------------------------------------------------------------------------------
#define    TX_RACH_FULL_BIT        0
#define    TX_EXTRACH_FULL_BIT     1
#define    TX_NBCCH_FULL_BIT       2
#define    TX_FACCH_FULL_BIT       3
#define    TX_TCH0_FULL_BIT        4
#define    TX_TCH1_FULL_BIT        5
#define    TX_TCH2_FULL_BIT        6
#define    TX_TCH3_FULL_BIT        7
#define    TX_RATSCCH_FULL_BIT     8
#define    RX_TCH0_FULL_BIT        9
#define    RX_TCH1_FULL_BIT        10
#define    RX_TCH2_FULL_BIT        11
#define    RX_TCH3_FULL_BIT        12
#define    RX_RATSCCH_FULL_BIT     13

#define    TX_PDCH0_FULL_BIT       TX_TCH0_FULL_BIT
#define    TX_PDCH1_FULL_BIT       TX_TCH1_FULL_BIT
#define    TX_PDCH2_FULL_BIT       TX_TCH2_FULL_BIT
#define    TX_PDCH3_FULL_BIT       TX_TCH3_FULL_BIT
#define    RX_PDCH0_FULL_BIT       RX_TCH0_FULL_BIT
#define    RX_PDCH1_FULL_BIT       RX_TCH1_FULL_BIT
#define    RX_PDCH2_FULL_BIT       RX_TCH2_FULL_BIT
#define    RX_PDCH3_FULL_BIT       RX_TCH3_FULL_BIT
// --------------------------------------------------------------------------------------------
// SR_BUFFER_CONTROL                        set/rst
// --------------------------------------------------------------------------------------------
#define    TX_RACH_FULL            (1<<TX_RACH_FULL_BIT)
#define    TX_EXTRACH_FULL         (1<<TX_EXTRACH_FULL_BIT)
#define    TX_NBCCH_FULL           (1<<TX_NBCCH_FULL_BIT)
#define    TX_FACCH_FULL           (1<<TX_FACCH_FULL_BIT)
#define    TX_TCH0_FULL            (1<<TX_TCH0_FULL_BIT)
#define    TX_TCH1_FULL            (1<<TX_TCH1_FULL_BIT)
#define    TX_TCH2_FULL            (1<<TX_TCH2_FULL_BIT)
#define    TX_TCH3_FULL            (1<<TX_TCH3_FULL_BIT)
#define    TX_RATSCCH_FULL         (1<<TX_RATSCCH_FULL_BIT)
#define    RX_TCH0_FULL            (1<<RX_TCH0_FULL_BIT)
#define    RX_TCH1_FULL            (1<<RX_TCH1_FULL_BIT)
#define    RX_TCH2_FULL            (1<<RX_TCH2_FULL_BIT)
#define    RX_TCH3_FULL            (1<<RX_TCH3_FULL_BIT)
#define    RX_RATSCCH_FULL         (1<<RX_RATSCCH_FULL_BIT)

#define    TX_PDCH0_FULL           TX_TCH0_FULL
#define    TX_PDCH1_FULL           TX_TCH1_FULL
#define    TX_PDCH2_FULL           TX_TCH2_FULL
#define    TX_PDCH3_FULL           TX_TCH3_FULL
#define    RX_PDCH0_FULL           RX_TCH0_FULL
#define    RX_PDCH1_FULL           RX_TCH1_FULL
#define    RX_PDCH2_FULL           RX_TCH2_FULL
#define    RX_PDCH3_FULL           RX_TCH3_FULL

// --------------------------------------------------------------------------------------------
// SR_BLOCK_STATUS                  tstb
// --------------------------------------------------------------------------------------------
#define    RX_TCH0_OK_BIT          0
#define    RX_TCH1_OK_BIT          1
#define    RX_TCH2_OK_BIT          2
#define    RX_TCH3_OK_BIT          3
#define    RX_NBCCH_OK_BIT         4
#define    RX_FACCH_OK_BIT         5
#define    RX_PDCH0_OK_BIT         RX_TCH0_OK_BIT
#define    RX_PDCH1_OK_BIT         RX_TCH1_OK_BIT
#define    RX_PDCH2_OK_BIT         RX_TCH2_OK_BIT
#define    RX_PDCH3_OK_BIT         RX_TCH3_OK_BIT
#define    RX_PTCCH_OK_BIT         RX_NBCCH_OK_BIT
// --------------------------------------------------------------------------------------------
// SR_BLOCK_STATUS                          set/rst
// --------------------------------------------------------------------------------------------
#define    RX_TCH0_OK              (1<<0)
#define    RX_TCH1_OK              (1<<1)
#define    RX_TCH2_OK              (1<<2)
#define    RX_TCH3_OK              (1<<3)
#define    RX_NBCCH_OK             (1<<4)
#define    RX_FACCH_OK             (1<<5)
#define    RX_PDCH0_OK             RX_TCH0_OK
#define    RX_PDCH1_OK             RX_TCH1_OK
#define    RX_PDCH2_OK             RX_TCH2_OK
#define    RX_PDCH3_OK             RX_TCH3_OK
#define    RX_PTCCH_OK             RX_FACCH_OK

// --------------------------------------------------------------------------------------------
// SR_BFI_STATUS                            shfi
// --------------------------------------------------------------------------------------------
#define    SCH_BFI                 (1<<0)
#define    NBCCH_BFI               (1<<1)
#define    FACCH_BFI               (1<<2)
#define    NB0_BFI                 (1<<3)
#define    NB1_BFI                 (1<<4)
#define    NB2_BFI                 (1<<5)
#define    NB3_BFI                 (1<<6)

// --------------------------------------------------------------------------------------------
// SR_ACCESS_TURN                           definitions
// --------------------------------------------------------------------------------------------
#define    DSP_TURN                0
#define    CPU_TURN                1


// --------------------------------------------------------------------------------------------
// DSP INTERRUPT CONTROLLER REGISTERS
// --------------------------------------------------------------------------------------------
#define    DSP_INT_CONTROLLER_BASE_ADD     CONFIG_REGISTERS                //@ 0xA000

#define    DSP_SOURCE_CLEAR1_ADD       DSP_INT_CONTROLLER_BASE_ADD             //1word / @ 0xA000
#define    DSP_SOURCE_CLEAR2_ADD       DSP_INT_CONTROLLER_BASE_ADD + 2         //1word / @ 0xA002
#define    DSP_SOURCE_STATUS1_ADD      DSP_INT_CONTROLLER_BASE_ADD             //1word / @ 0xA000
#define    DSP_SOURCE_STATUS2_ADD      DSP_INT_CONTROLLER_BASE_ADD + 2         //1word / @ 0xA002
#define    DSP_INT_CLEAR_ADD           DSP_INT_CONTROLLER_BASE_ADD + 4         //1word / @ 0xA004
#define    DSP_PROGINT_CTRL_ADD        DSP_INT_CONTROLLER_BASE_ADD + 6         //1word / @ 0xA006

// --------------------------------------------------------------------------------------------
// DSP SOURCE STATUS DEFINTIONS          tstb
// --------------------------------------------------------------------------------------------
#define    AUDIO_TX_FIFO_INT_BIT   0       // INT0
#define    AUDIO_RX_FIFO_INT_BIT   1       // INT0
#define    SPEECH_CODE_INT_BIT     2       // VINT
#define    SPEECH_DECODE_INT_BIT   3       // VINT
#define    TX_PROCESS_INT_BIT      4       // INT1
#define    RX_PROCESS_INT_BIT      5       // INT1
#define    CPU_INT_BIT             6       // VINT
#define    USER_INT_BIT            7       // ? (not used)
#define    EDGE_DETECTOR_INT_BIT   8       // INT0
#define    EDGE_VITERBI_INT_BIT    9       // INT0
#define    EDGE_PREFILTER_INT_BIT  10      // INT0
#define    DSMA_INT_BIT            11      // VINT
#define    SW_PROG_INT_BIT         12      // INT2

// --------------------------------------------------------------------------------------------
// DSP SOURCE CLEAR DEFINITIONS          set/rst
// --------------------------------------------------------------------------------------------
#define    CLEAR_AUDIO_TX_FIFO_INT         0x0001
#define    CLEAR_AUDIO_RX_FIFO_INT         0x0002
#define    CLEAR_SPEECH_CODE_INT           0x0004
#define    CLEAR_SPEECH_DECODE_INT         0x0008
#define    CLEAR_TX_PROCESS_INT            0x0010
#define    CLEAR_RX_PROCESS_INT            0x0020
#define    CLEAR_CPU_INT                   0x0040
#define    CLEAR_USER_INT                  0x0080
#define    CLEAR_EDGE_DETECTOR_INT         0x0100
#define    CLEAR_EDGE_VITERBI_INT          0x0200
#define    CLEAR_EDGE_PREFILTER_INT        0x0400
#define    CLEAR_DSMA_INT                  0x0800
#define    CLEAR_SW_PROG_INT               0x1000
#define    CLEAR_ALL_INT                   0x1FFF

// --------------------------------------------------------------------------------------------
// DSP INT CLEAR DEFINITIONS             set/rst
// --------------------------------------------------------------------------------------------
#define    CLEAR_TEAK_NMI          0x0001
#define    CLEAR_TEAK_INT0         0x0002
#define    CLEAR_TEAK_INT1         0x0004
#define    CLEAR_TEAK_INT2         0x0008
#define    CLEAR_TEAK_VINT         0x0010
#define    CLEAR_ALL_TEAK_INT      0x001F

// --------------------------------------------------------------------------------------------
// DSMA REGISTERS
// --------------------------------------------------------------------------------------------
#define    DSMA_CTRL_BASE_ADD      CONFIG_REGISTERS + 0x20         //@ 0xA020

#define    DSMA_IT_CPU_MASK           DSMA_CTRL_BASE_ADD
#define    DSMA_IT_DSP_MASK           DSMA_CTRL_BASE_ADD + 0x2
#define    DSMA_IT_RAW_STATUS         DSMA_CTRL_BASE_ADD + 0x4
#define    DSMA_IT_STATUS             DSMA_CTRL_BASE_ADD + 0x6
#define    DSMA_CPUTODSP1_FIFO_LEVEL  DSMA_CTRL_BASE_ADD + 0x8
#define    DSMA_CPUTODSP2_FIFO_LEVEL  DSMA_CTRL_BASE_ADD + 0xA
#define    DSMA_DSPTOCPU1_FIFO_LEVEL  DSMA_CTRL_BASE_ADD + 0xC
#define    DSMA_DSPTOCPU2_FIFO_LEVEL  DSMA_CTRL_BASE_ADD + 0xE
#define    DSMA_CPUTODSP1_FIFO_DATA   DSMA_CTRL_BASE_ADD + 0x10
#define    DSMA_CPUTODSP2_FIFO_DATA   DSMA_CTRL_BASE_ADD + 0x12
#define    DSMA_DSPTOCPU1_FIFO_DATA   DSMA_CTRL_BASE_ADD + 0x14
#define    DSMA_DSPTOCPU2_FIFO_DATA   DSMA_CTRL_BASE_ADD + 0x16

// --------------------------------------------------------------------------------------------
// DSMA_IT_DSP_MASK & DSMA_IT_RAW_STATUS      set/rst
// --------------------------------------------------------------------------------------------
#define    CPUTODSP1_FE_BIT           0
#define    CPUTODSP1_THRESH_LOW_BIT   1
#define    CPUTODSP1_THRESH_HIGH_BIT  2
#define    CPUTODSP1_FF_BIT           3
#define    CPUTODSP2_FE_BIT           4
#define    CPUTODSP2_THRESH_LOW_BIT   5
#define    CPUTODSP2_THRESH_HIGH_BIT  6
#define    CPUTODSP2_FF_BIT           7
#define    DSPTOCPU1_FE_BIT           8
#define    DSPTOCPU1_THRESH_LOW_BIT   9
#define    DSPTOCPU1_THRESH_HIGH_BIT  10
#define    DSPTOCPU1_FF_BIT           11
#define    DSPTOCPU2_FE_BIT           12
#define    DSPTOCPU2_THRESH_LOW_BIT   13
#define    DSPTOCPU2_THRESH_HIGH_BIT  14
#define    DSPTOCPU2_FF_BIT           15

// --------------------------------------------------------------------------------------------
// DSMA_IT_DSP_MASK & DSMA_IT_STATUS          and
// --------------------------------------------------------------------------------------------
#define    CPUTODSP1_FE              (1 << CPUTODSP1_FE_BIT         )
#define    CPUTODSP1_THRESH_LOW      (1 << CPUTODSP1_THRESH_LOW_BIT )
#define    CPUTODSP1_THRESH_HIGH     (1 << CPUTODSP1_THRESH_HIGH_BIT)
#define    CPUTODSP1_FF              (1 << CPUTODSP1_FF_BIT         )
#define    CPUTODSP2_FE              (1 << CPUTODSP2_FE_BIT         )
#define    CPUTODSP2_THRESH_LOW      (1 << CPUTODSP2_THRESH_LOW_BIT )
#define    CPUTODSP2_THRESH_HIGH     (1 << CPUTODSP2_THRESH_HIGH_BIT)
#define    CPUTODSP2_FF              (1 << CPUTODSP2_FF_BIT         )
#define    DSPTOCPU1_FE              (1 << DSPTOCPU1_FE_BIT         )
#define    DSPTOCPU1_THRESH_LOW      (1 << DSPTOCPU1_THRESH_LOW_BIT )
#define    DSPTOCPU1_THRESH_HIGH     (1 << DSPTOCPU1_THRESH_HIGH_BIT)
#define    DSPTOCPU1_FF              (1 << DSPTOCPU1_FF_BIT         )
#define    DSPTOCPU2_FE              (1 << DSPTOCPU2_FE_BIT         )
#define    DSPTOCPU2_THRESH_LOW      (1 << DSPTOCPU2_THRESH_LOW_BIT )
#define    DSPTOCPU2_THRESH_HIGH     (1 << DSPTOCPU2_THRESH_HIGH_BIT)
#define    DSPTOCPU2_FF              (1 << DSPTOCPU2_FF_BIT         )

// --------------------------------------------------------------------------------------------
// DSMA_IT_DSP_MASK              set/rst
// --------------------------------------------------------------------------------------------
#define    CLEAR_ALL_DSMA_IT_DSP  0xFFFF

#define    DSMA_FIFO_SIZE         32         // in 32-bit words

// --------------------------------------------------------------------------------------------
// HARDWARE FRAME COUNTERS
// --------------------------------------------------------------------------------------------
#define    FRAME_COUNTERS_BASE_ADD         CONFIG_REGISTERS + 0x40         //@ 0xA040

#define    FRAME_COUNTER1  FRAME_COUNTERS_BASE_ADD         // counter%51, +1 each TDMA
#define    FRAME_COUNTER2  FRAME_COUNTERS_BASE_ADD + 0x2   // counter%256, +1 each FC3 = 26*51*8
#define    FRAME_COUNTER3  FRAME_COUNTERS_BASE_ADD + 0x4   // counter%26*51*8, +1 each TDMA
#define    FRAME_COUNTER4  FRAME_COUNTERS_BASE_ADD + 0x6   // counter%104, +1 each TDMA
#define    TBU_COUNTER     FRAME_COUNTERS_BASE_ADD + 0x8   // TBU QB counter (in quarter bits),
                                                        // width = 14 bits

// --------------------------------------------------------------------------------------------
// HARDWARE FRAME COUNTERS CONSTANTS
// --------------------------------------------------------------------------------------------
#define    MODULO_2048             1326            // to divide FC3, before multiplying by FC2

// --------------------------------------------------------------------------------------------
// RADIO PORT REGISTERS / RX_PORT
// --------------------------------------------------------------------------------------------
#define    RX_PORT_CONTROL_BASE_ADD        CONFIG_REGISTERS + 0x60 //@ 0xA060

#define    RX_CONTROL_ADD              RX_PORT_CONTROL_BASE_ADD        //1word / @ 0xA060
#define    RX_PROCESSING_POSITION_ADD  RX_PORT_CONTROL_BASE_ADD + 0x2  //1word / @ 0xA062
#define    RX_SAMPLE_COUNTER_ADD       RX_PORT_CONTROL_BASE_ADD + 0x4  //1word / @ 0xA064
#define    RX_STATUS_ADD               RX_PORT_CONTROL_BASE_ADD + 0x6  //1word / @ 0xA066
#define    RX_WIN_MODE_ADD             RX_PORT_CONTROL_BASE_ADD + 0x8  //1word / @ 0xA068

#define    RX_BUFFER               RX_PORT_BUFFER                  //max 2048 words / @ 0xE000

// --------------------------------------------------------------------------------------------
// RX CONTROL DEFINITIONS        set/rst
// --------------------------------------------------------------------------------------------
#define    RX_RESET                0x0001  // Rx Port Reset
#define    RX_SAMPLE_INT_ENA       0x0002  // Rx Sample Interrupt enable
#define    RX_END_OF_WINDOW_ENA      0x0004  // Rx End of Win Interrupt enable

// --------------------------------------------------------------------------------------------
// RX STATUS DEFINITIONS         and
// --------------------------------------------------------------------------------------------
//#define    RX_STATUS_ADD_MASK          0x000F
#define    RX_IT_SAMPLE             0x0008
//#define    RX_WIN_NB               0x0007

// --------------------------------------------------------------------------------------------
// RX STATUS DEFINITIONS         tstb
// --------------------------------------------------------------------------------------------
#define    RX_IT_SAMPLE_BIT         3

// --------------------------------------------------------------------------------------------
// RX WIN MODE DEFINITIONS       and
// --------------------------------------------------------------------------------------------
#define    RX_WIN_MODE_ADD_MASK        0x7FFF
#define    RX_IQ_SWAP              0x0001  // hardware swap
#define    RX_PROCESS              0x000E
#define    RX_DCOFFSET             0x0010
#define    RX_DC0                  0x00E0
#define    RX_SOFT_SWAP            0x0100  // software swap
#define    RX_WIN_NB               0x0E00
#define    RX_NBCCH_COUNTER_MASK   0x7000

// --------------------------------------------------------------------------------------------
// RX WIN MODE DEFINITIONS       cmp
// --------------------------------------------------------------------------------------------
#define    RX_PROCESSING_PWR          0x0000
#define    RX_PROCESSING_FB           0x0002
#define    RX_PROCESSING_SB           0x0004
#define    RX_PROCESSING_NB           0x0006
#define    RX_PROCESSING_ENB          0x0008
// --------------------------------------------------------------------------------------------
// RX WIN MODE DEFINITIONS       tstb
// --------------------------------------------------------------------------------------------
#define    RX_IQ_SWAP_BIT          0       // hardware swap
#define    RX_DC_OFFSET_BIT        4
#define    RX_SOFT_SWAP_BIT        8       // software swap
// --------------------------------------------------------------------------------------------
// RX WIN MODE DEFINITIONS       shfi
// --------------------------------------------------------------------------------------------
#define    RX_DC0_SHIFT            -5
#define    RX_WIN_NB_SHIFT         -9
#define    RX_NBCCH_COUNTER_SHIFT  -12

// --------------------------------------------------------------------------------------------
// RX PORT CONSTANTS
// --------------------------------------------------------------------------------------------
#define    RX_IT_END_WIN           0       //not used

#define    RX_WIN_IS_LONG          0x0001  //not used
#define    RX_RESET_SAMPLE         155
#define    RX_BUFFER_LENGTH        2048    // (RX_BUFFER_LENGTH 10-bit words)
#define    NB_COUPLES_READ         154     // NB stands on 154 couples
#define    NB_COUPLES_WIN          156



// --------------------------------------------------------------------------------------------
// AUDIO I/F REGISTERS
// --------------------------------------------------------------------------------------------
#define    AUDIO_IF_BASE_ADD       CONFIG_REGISTERS + 0x80 //@ 0xA080

#define    AUDIO_FIFO_IN           AUDIO_IF_BASE_ADD               // TX size 2words
#define    AUDIO_FIFO_OUT          AUDIO_IF_BASE_ADD + 0x2         // RX size 2words
#define    AUDIO_DSP_MISC          AUDIO_IF_BASE_ADD + 0x4         // size 2words
#define    AUDIO_DSP_FIFO_NWORDS   AUDIO_IF_BASE_ADD + 0x6         // size 2words
#define    AUDIO_DSP_FIFO_LEVEL    AUDIO_IF_BASE_ADD + 0x8         // size 2words

// --------------------------------------------------------------------------------------------
// DSP AUDIO TIMER SIGNALS (mapped onto DSP register st2)        tstb
// --------------------------------------------------------------------------------------------
#define    PCM_RX_COPY_BIT         10      // compatibility register st2 bit IU0 (not used)
#define    PCM_TX_COPY_BIT         11      // compatibility register st2 bit IU1

// --------------------------------------------------------------------------------------------
// AUDIO_DSP_MISC DEFINITIONS            and
// --------------------------------------------------------------------------------------------
#define    AUDIO_DSP_MISC_MASK     0x03FF   // not used
//#define    REFRESH_INPUTFIFO_MASK  0x0040   ; useless for Full Reticle
//#define    REFRESH_OUTPUTFIFO_MASK 0x0080   ; useless for Full Reticle


// --------------------------------------------------------------------------------------------
// AUDIO_DSP_MISC DEFINITIONS            tstb
// --------------------------------------------------------------------------------------------
#define    PCMOUT_RDY              0       //PCM data are available for downlink
#define    OUT_FIFO_EMPTY          1       //The DSP output FIFO is Empty
#define    IN_FIFO_EVENT           2       //The DSP Input FIFO event is true
#define    INPUT_FIFO_1WORD        3       //Input FIFO event occurs (not used)
#define    STARTDSPTIMER           4       //Start signal
#define    DSPMS                   5       //Type of transmission : mono or stereo
//#define    REFRESH_INPUTFIFO       6       ;Refresh Tx FIFO after reading N words
//#define    REFRESH_OUTPUTFIFO      7       ;Refresh Rx FIFO after writing N words
#define    OUT_FIFO_ERROR          8       //Output FIFO overflow/underflow error
#define    IN_FIFO_ERROR           9       //Input FIFO overflow/underflow error

// --------------------------------------------------------------------------------------------
// AUDIO_DSP_FIFO_NWORDS DEFINITIONS      and
// --------------------------------------------------------------------------------------------
#define    INPUT_FIFO_NWORDS_MASK   0x003F  // 6 LSB of lower word of AUDIO_DSP_FIFO_NWORDS
#define    OUTPUT_FIFO_NWORDS_MASK  0x003F  // 6 LSB of upper word of AUDIO_DSP_FIFO_NWORDS

// --------------------------------------------------------------------------------------------
// AUDIO_DSP_FIFO_LEVEL DEFINITIONS       and/shfi
// --------------------------------------------------------------------------------------------
#define    INPUT_FIFO_LEVEL_MASK   0x007F  // first word of AUDIO_DSP_FIFO_LEVEL (bits[6:0])
#define    INPUT_FIFO_LEVEL_SHIFT  0
#define    OUTPUT_FIFO_LEVEL_MASK  0x7F00  // first word of AUDIO_DSP_FIFO_LEVEL (bits[14:8])
#define    OUTPUT_FIFO_LEVEL_SHIFT -8


// --------------------------------------------------------------------------------------------
// VITERBI REGISTERS
// --------------------------------------------------------------------------------------------
#define    VITERBI_BASE_ADD        CONFIG_REGISTERS + 0xA0 //@ 0xA0A0

#define    VITERBI_CONFIG          VITERBI_BASE_ADD
#define    VITERBI_THRESH          VITERBI_BASE_ADD + 0x2
#define    VITERBI_SCALE         VITERBI_BASE_ADD + 0x4
#define    VITERBI_BESTNODE        VITERBI_BASE_ADD + 0x6
#define    VITERBI_BESTPATH        VITERBI_BASE_ADD + 0x8
#define    VITERBI_BESTMETRIC      VITERBI_BASE_ADD + 0xA

// --------------------------------------------------------------------------------------------
// VITERBI DEFINITIONS                   mov
// --------------------------------------------------------------------------------------------


// Channel Decoding: signed branch metrics, rescale on, best node search off, 1 bm, k = 5
#define    VITERBI_CHDEC5_STOP             0x02AC  // stop mode
#define    VITERBI_CHDEC5_WRITE            0x22AE  // write node metrics ram mode
#define    VITERBI_CHDEC5_RUN              0x02AF  // run mode

// Channel Decoding: signed branch metrics, rescale on, best node search on, k = 5
#define    VITERBI_ECHDEC5_STOP            0x02BC  // stop mode
#define    VITERBI_ECHDEC5_WRITE           0x22BE  // write node metrics ram mode
#define    VITERBI_ECHDEC5_RUN1            0x02BF  // run mode, 1 bm
#define    VITERBI_ECHDEC5_RUN4            0x02FF  // run mode, 4 bm

// Channel Decoding: signed branch metrics, rescale on, best node search on, k = 7
#define    VITERBI_ECHDEC7_STOP            0x03BC  // stop mode
#define    VITERBI_ECHDEC7_WRITE           0x23BE  // write node metrics ram mode
#define    VITERBI_ECHDEC7_RUN1            0x03BF  // run mode, 1 bm
#define    VITERBI_ECHDEC7_RUN4            0x03FF  // run mode, 4 bm

// Equalization: unsigned branch metrics, rescale off, best node search on, 4 bm, k = 5
#define    VITERBI_EQUAL_STOP              0x0AF0  // stop mode
#define    VITERBI_EQUAL_WRITE             0x2AF2  // write node metrics ram mode
#define    VITERBI_EQUAL_RUN               0x0AF3  // run mode
#define    VITERBI_EQUAL_READ_PATHS        0x1AF2  // read path ram mode
#define    VITERBI_EQUAL_READ_METRICS      0x0AF2  // read node metrics ram mode

// --------------------------------------------------------------------------------------------
// BRANCH METRIC REGISTERS
// --------------------------------------------------------------------------------------------
#define    BMC_BASE_ADD            CONFIG_REGISTERS + 0xC0 //@ 0xA0C0

#define    BMC_CONTROL          BMC_BASE_ADD
#define    BMC_TRACE            BMC_BASE_ADD    + 0x2
#define    BMC_Z0I              BMC_BASE_ADD    + 0x4
#define    BMC_Z0Q              BMC_BASE_ADD    + 0x6
#define    BMC_Z1I              BMC_BASE_ADD    + 0x8
#define    BMC_Z1Q              BMC_BASE_ADD    + 0xA
#define    BMC_METRIC           BMC_BASE_ADD    + 0xC

// --------------------------------------------------------------------------------------------
// BRANCH METRIC DEFINTIONS      mov
// --------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------
// CIFIRE REGISTERS
// --------------------------------------------------------------------------------------------
#define    CIFIRE_BASE_ADD         CONFIG_REGISTERS + 0xE0 //@ 0xA0E0

#define    CIFIRE_CONTROL          CIFIRE_BASE_ADD
#define    CIFIRE_STATUS           CIFIRE_BASE_ADD + 0x2

// --------------------------------------------------------------------------------------------
// CIFIRE_STATUS DEFINITIONS     tstb
// --------------------------------------------------------------------------------------------
#define    CIFIRE_ERROR_BIT        0
#define    CIFIRE_PATTERN_BIT      1
// --------------------------------------------------------------------------------------------
// CIFIRE_STATUS DEFINITIONS     cmp
// --------------------------------------------------------------------------------------------
#define    CIFIRE_ERROR            0x0000
#define    CIFIRE_NO_ERROR         0x0001
#define    CIFIRE_PATTERN          0x0002
// --------------------------------------------------------------------------------------------
// CIFIRE_CONTROL DEFINITIONS
// --------------------------------------------------------------------------------------------

#define    CIFIRE_SOFT_SET         0x0002
#define    CIFIRE_REG_INV          0x0004
#define    CIFIRE_FIRE_DECODE      0x0000
#define    CIFIRE_FIRE_DECODE_INV  0x0004
#define    CIFIRE_FIRE_CODE        0x0008
#define    CIFIRE_FIRE_CODE_INV    0x000C
#define    CIFIRE_RUN_A51          0x0010
#define    CIFIRE_RUN_A51_INV      0x0014
#define    CIFIRE_RUN_A52          0x0018
#define    CIFIRE_RUN_A52_INV      0x001C

#define    CIFIRE_INFO_LENGTH      184
#define    CIFIRE_REMAINDER_LENGTH 40
#define    CIFIRE_CORRECT_LENGTH   12
#define    CIFIRE_CRYPT_SEQ_LENGTH 114
#define    CIFIRE_RUN_UP_CYCLES    101             // 100 what1, 101 what2

// --------------------------------------------------------------------------------------------
// SINE GEN REGISTERS
// --------------------------------------------------------------------------------------------
#define    SINE_GEN_BASE_ADD       CONFIG_REGISTERS + 0x100        //@ 0xA100

#define    SINE_GEN_PHASE1_INIT    SINE_GEN_BASE_ADD
#define    SINE_GEN_PHASE1_INC     SINE_GEN_BASE_ADD + 0x2
#define    SINE_GEN_AMP1_INIT      SINE_GEN_BASE_ADD + 0x4
#define    SINE_GEN_AMP1_INC       SINE_GEN_BASE_ADD + 0x6
#define    SINE_GEN_PHASE2_INIT    SINE_GEN_BASE_ADD + 0x8
#define    SINE_GEN_PHASE2_INC     SINE_GEN_BASE_ADD + 0xA
#define    SINE_GEN_AMP2_INIT      SINE_GEN_BASE_ADD + 0xC
#define    SINE_GEN_AMP2_INC       SINE_GEN_BASE_ADD + 0xE
#define    SINE_GEN_CFG            SINE_GEN_BASE_ADD + 0x10
#define    SINE_GEN_NOISE_POLY     SINE_GEN_BASE_ADD + 0x12
#define    SINE_GEN_NOISE_RES      SINE_GEN_BASE_ADD + 0x14
#define    SYS_TIMER_SPARE         SINE_GEN_BASE_ADD + 0x16

// --------------------------------------------------------------------------------------------
// SINE GEN  DEFINITIONS         and
// --------------------------------------------------------------------------------------------
#define     SINE_GEN_NBSAMPLE    0x00FF
#define     SINE_GEN_START       0x0100
#define     SINE_GEN_DATA_READ   0x1000
#define     SINE_GEN_MODE        0xC000
// --------------------------------------------------------------------------------------------
// SINE GEN  DEFINITIONS         cmp
// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------
// SINE GEN DEFINITIONS          tstb
// --------------------------------------------------------------------------------------------
#define     SINE_GEN_START_BIT       8
#define     SINE_GEN_DATA_READ_BIT   12
#define     SINE_GEN_MODE_BIT        14
// --------------------------------------------------------------------------------------------
// SINE GEN DEFINITIONS          shfi
// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------
// RADIO PORT REGISTERS / TX port
// --------------------------------------------------------------------------------------------
#define    TX_PORT_BASE_ADD        CONFIG_REGISTERS + 0x120        //@ 0xA140

//#define    TX_BIT_NBR_ADD              TX_PORT_BASE_ADD
#define    TX_BUF_SIZE_1           TX_PORT_BASE_ADD
#define    TX_BUF_SIZE_2           TX_PORT_BASE_ADD + 0x2
#define    TX_BUF_SIZE_3           TX_PORT_BASE_ADD + 0x4
#define    TX_BUF_SIZE_4           TX_PORT_BASE_ADD + 0x6
#define    TX_CONTROL_ADD              TX_PORT_BASE_ADD + 0x8
#define    TX_SPARE                TX_PORT_BASE_ADD + 0xA

#define    TX_BUFFER               TX_PORT_BUFFER          //max 384 words @ 0xC800

// --------------------------------------------------------------------------------------------
// TX CONTROL DEFINITIONS        set/rst
// --------------------------------------------------------------------------------------------
#define    TX_DTX_ON               0x0001  // Tx Port DTX mask (bit 0)
#define    TX_RESET_MODE           0x0002  // Tx Port Reset Mode mask (bit 1):
                                        //   0 (default) -> buffer pointer is reset by FINT
                                        //   1 -> buffer pointer is reset at the end of burst
                                        //        (backward compatibility with MLR)

// --------------------------------------------------------------------------------------------
// TX PORT CONSTANTS
// --------------------------------------------------------------------------------------------
#define    TX_RAMP_UP_DELAY        8       // number of bits for power up ramping delay
#define    TX_RAMP_DOWN_DELAY      8       // number of bits for power down ramping delay
#define    TX_RAMP_DELAY_BIT       1       // value of the bits for delay
#define    TX_SLOT_LENGTH          156
#define    TX_AB_LENGTH            TX_RAMP_UP_DELAY + TX_RAMP_DOWN_DELAY + 88
#define    TX_NB_LENGTH            TX_RAMP_UP_DELAY + TX_RAMP_DOWN_DELAY + 148


// --------------------------------------------------------------------------------------------
// DSP SYSTEM CONTROL REGISTERS
// --------------------------------------------------------------------------------------------
#define    SYS_DSP_REG_BASE_ADD    CONFIG_REGISTERS + 0x260  //@0xA260

#define    DSP_WAKEUP_CTRL         SYS_DSP_REG_BASE_ADD         //@0xA260
#define    DSP_COPRO_POWER_CTRL    SYS_DSP_REG_BASE_ADD + 0x02  //@0xA262
#define    DSP_COPRO_RESET_CTRL    SYS_DSP_REG_BASE_ADD + 0x04  //@0xA264
#define    DSP_USR_INT_CTRL        SYS_DSP_REG_BASE_ADD + 0x06  //@0xA266
#define    DSP_SPARE               SYS_DSP_REG_BASE_ADD + 0x08  //@0xA268

// --------------------------------------------------------------------------------------------
// DSP_WAKEUP_CTRL DEFINITIONS       tstb
// --------------------------------------------------------------------------------------------
#define    STOP0_EN_BIT    0
#define    STOP1_EN_BIT    1  //not used
#define    INT0_EN_BIT     2
#define    INT1_EN_BIT     3
#define    INT2_EN_BIT     4
#define    NMI_EN_BIT      5
#define    VINT_EN_BIT     6
#define    TRAP_EN_BIT     7
// --------------------------------------------------------------------------------------------
// DSP_WAKEUP_CTRL DEFINITIONS       set/rst
// --------------------------------------------------------------------------------------------
#define    STOP0_EN        1<<0
#define    STOP1_EN        1<<1  //not used
#define    INT0_EN         1<<2
#define    INT1_EN         1<<3
#define    INT2_EN         1<<4
#define    NMI_EN          1<<5
#define    VINT_EN         1<<6
#define    TRAP_EN         1<<7

// --------------------------------------------------------------------------------------------
// DSP_COPRO_POWER_CTRL DEFINITIONS       set/rst
// --------------------------------------------------------------------------------------------
#define    CIPHER_POWERDOWN              0x0001
#define    PATCH_UNIT_POWERDOWN          0x0002
#define    SINE_GEN_POWERDOWN            0x0004
#define    GSM_VITERBI_POWERDOWN         0x0008
#define    BMC_POWERDOWN                 0x0010
#define    CIFIRE_POWERDOWN              0x0020
#define    EDGE_PREF_POWERDOWN           0x0040
#define    EDGE_DETECT_POWERDOWN         0x0080
#define    EDGE_VITERBI_POWERDOWN        0x0100
#define    GSM_GPRS_EDGE_DSP_COPRO_POWERDOWN       0x01FD  // exclude Patch Unit

// --------------------------------------------------------------------------------------------
// DSP_COPRO_RESET_CTRL DEFINITIONS       set/rst
// --------------------------------------------------------------------------------------------
#define    CIPHER_RESET        0x0001
#define    SINE_GEN_RESET      0x0004
#define    COPRO_CIFIRE_RESET  0x0020      //??????pb car déjà defini dans CIFIRE_CONTROL
#define    EDGE_PREF_RESET     0x0040
#define    EDGE_DETECT_RESET   0x0080
#define    EDGE_VITERBI_RESET  0x0100

// --------------------------------------------------------------------------------------------
// CIPHER REGISTERS
// --------------------------------------------------------------------------------------------
#define    CIPHER_BASE_ADD         CONFIG_REGISTERS + 0x2A0 //@ 0xA2A0

#define    CIPHER_CTRL             CIPHER_BASE_ADD
#define    CIPHER_CK0              CIPHER_BASE_ADD + 0x02   // ciphering key (bits 31..0)
#define    CIPHER_CK1              CIPHER_BASE_ADD + 0x04   // ciphering key (bits 63..32)
#define    CIPHER_CK2              CIPHER_BASE_ADD + 0x06   // ciphering key (bits 95..64)
#define    CIPHER_CK3              CIPHER_BASE_ADD + 0x08   // ciphering key (bits 127..96)
#define    CIPHER_COUNT            CIPHER_BASE_ADD + 0x0A   // 22-bit count
#define    CIPHER_KSB1_0           CIPHER_BASE_ADD + 0x0C   // RX/TX cipher stream (bits 32..63):   decryption bits [32..63]
#define    CIPHER_KSB1_1           CIPHER_BASE_ADD + 0x0E   // RX/TX cipher stream (bits 0..31):    decryption bits [0..31]
#define    CIPHER_KSB2_0           CIPHER_BASE_ADD + 0x10   // RX/TX cipher stream (bits 96..127):  decryption bits [96..113] + encryption bits [0..13]
#define    CIPHER_KSB2_1           CIPHER_BASE_ADD + 0x12   // RX/TX cipher stream (bits 64..95):   decryption bits [64..95]
#define    CIPHER_KSB3_0           CIPHER_BASE_ADD + 0x14   // RX/TX cipher stream (bits 160..191): encryption bits [46..77]
#define    CIPHER_KSB3_1           CIPHER_BASE_ADD + 0x16   // RX/TX cipher stream (bits 128..159): encryption bits [14..45]
#define    CIPHER_KSB4_0           CIPHER_BASE_ADD + 0x18   // RX/TX cipher stream (bits 224..255): encryption bits [110..113] + 28 padding bits
#define    CIPHER_KSB4_1           CIPHER_BASE_ADD + 0x1A   // RX/TX cipher stream (bits 192..223): encryption bits [78..109]
#define    CIPHER_SPARE            CIPHER_BASE_ADD + 0x1C

// --------------------------------------------------------------------------------------------
// CIPHER DEFINITIONS            set/rst
// --------------------------------------------------------------------------------------------
#define    CIPHER_START   0x0001
#define    CIPHER_DONE    0x0000

#define    CIPHER_RUN_UP_CYCLES    252

// --------------------------------------------------------------------------------------------
// Vars not defined in asm
// --------------------------------------------------------------------------------------------
#define TX_TRAINSEQ_TCS0    0<<4  // Tx Training sequence  0
#define TX_TRAINSEQ_TCS1    1<<4  // Tx Training sequence  1
#define TX_TRAINSEQ_TCS2    2<<4  // Tx Training sequence  2
#define TX_TRAINSEQ_TCS3    3<<4  // Tx Training sequence  3
#define TX_TRAINSEQ_TCS4    4<<4  // Tx Training sequence  4
#define TX_TRAINSEQ_TCS5    5<<4  // Tx Training sequence  5
#define TX_TRAINSEQ_TCS6    6<<4  // Tx Training sequence  6
#define TX_TRAINSEQ_TCS7    7<<4  // Tx Training sequence  7

#define RX_TSC0  0x0000
#define RX_TSC1  0x0020
#define RX_TSC2  0x0040
#define RX_TSC3  0x0060
#define RX_TSC4  0x0080
#define RX_TSC5  0x00A0
#define RX_TSC6  0x00C0
#define RX_TSC7  0x00E0

#define     RESET_HARD             0
#define     RESET_SOFTWARE_CPU     1
#define     RESET_DEFAULT          2
#define     DOWN_REQUEST_DPRAM     3
#define     DL_REQUEST_DSMA_1      4
#define     DL_REQUEST_DSMA_2      5
#define     UL_REQUEST_DSMA_1      6
#define     UL_REQUEST_DSMA_2      7
#define     AUDIO_INIT_REQUEST     8
#define     AUDIO_OUT_CFG_REQUEST  9

#define RX_WIN0  0 // 0x0001
#define RX_WIN1  1 // 0x0002
#define RX_WIN2  2 // 0x0003

#endif /* __ULC_ALIASES_H__ */
