#ifndef __ULC_DPRAM_H__
#define __ULC_DPRAM_H__

// -----------------------------------------------------------------------------
// NAME          : dpram_h.inc
// AUTHOR        : BMA
// DESCRIPTION   : DPRAM Mapping for asm files
// -----------------------------------------------------------------------------
// Reference :   CPU / DSP Interface - Shared DPRAM

// Modfication History :
//   Date   | Author | Modification
// ---------+--------+----------------------------------------------------------
// 28/09/01 |  bma   | Creation
// 27/06/02 |  bma   | Frame counters relocated in DPRAM (patch purpose)
// 09/07/02 |  bma   | Power Measure bits added for each Rx win in Burst Status
// 04/01/03 |  bma   | Quick USF Results added
// 22/01/03 |  bma   | Mapping changed to fit future What 3.0 mapping
// 20/04/05 |  fsh   | Added DP_AUDIO_FIFO_CTRL and DP_AUDIO_RUN_CTRL + definition of fields
//          |        | for these parameters
// 30/04/05 |  gha   | Mapping updated to fit with DpramSReg_22.xls  file, suppression of
//          |        | unused words and flags, words and flags for Digital Gain,MEMO VOCAL
//          |        | and MONITOR_ADDR added
// 04/05/05 |  gha   | Changed AMR_LINK CONTROL into DP_AMR_DOWNLINK_CONTROL and DP_AMR_UPLINK_CONTROL
// 19/04/06 |  fva   | Add melody dpram regs & fiels according to DpramSReg_ES4_03.xls
// 25/11/10 |  fbo   | Modified DP_AUDIO_FIFO_CTRL for ULC new audio interface and
//          |        | remapped some audio and radio DPRAM parameters such as to
//          |        | separate both as well as to prepare for the addition of
//          |        | new features
// 17/01/11 |  fbo   | Extended DP_CIPHERING_KEY to 8 words and DP_PWR_RESULT to 8
//          |        | words resulting in an address offset shift of 9 of all
//          |        | other fields up to DP_DSP_EVT_SRC_EXTENDED
// 30/03/11 |  kbc   | Added new DSMA transfer complete bits in DP_DSP_EVT_SRC_EXTENDED
//          |        | Added DSMA configuration registers at offset 0x033D
// 12/04/11 |  fbo   | Added inband modem interface (DP_IB_MODEM_CTRL)
// 18/04/11 |  fbo   | Changed the audio gain interface in DP_DIGITAL_AUDIO_GAIN
//          |        | (DL gain added + range extension). Removed the
//          |        | predistortion filter interface. Reworked and cleaned
//          |        | flags in DP_AUDIO_RUN_CTRL
// 21/07/11 |  fbo   | Added VCP interface (AEC/NS)
// 21/07/11 |  fbo   | Redefined DSP version number to include a patch version
//          |        | number
// 17/04/12 |  kbc   | Added 2 new registers @0x0344 - 0x0345 for semaphore
//          |        | DP_DSP_WANTS_ACCESS and DP_CPU_WANTS_ACCESS
// 23/05/16 |  fbo   | Removed VCP interface (AEC/NS): out of scope for porting
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// EXTERNAL DECLARATIONS
// -----------------------------------------------------------------------------


// DSP ROM Version
#define    DSP_ROM_VERSION_NUMBER         001


// -----------------------------------------------------------------------------
// DPRAM REGISTERS
// -----------------------------------------------------------------------------
#define    DPRAM_BASE_ADD  ADD_DPRAM           // DPRAM Base Address
//#define    DPRAM_SIZE                 2048
#define    DPRAM_MAX_SIZE_TRANSFER    2048-5          // -5: because of download protocol

// DSP Version ID
#define    DP_DSP_VERSION_ID          DPRAM_BASE_ADD + 0x0000

// DSP Error Label
#define    DP_DSP_ERROR_LABEL1        DPRAM_BASE_ADD + 0x0001
#define    DP_DSP_ERROR_LABEL2        DPRAM_BASE_ADD + 0x0002
#define    DP_DSP_ERROR_LABEL3        DPRAM_BASE_ADD + 0x0003
#define    DP_DSP_ERROR_LABEL4        DPRAM_BASE_ADD + 0x0004

// TCH Channel Attributes
#define    DP_TCH_CHANNEL_ATTRIBUTE   DPRAM_BASE_ADD + 0x0005

// SACCH Start Frame Number
#define    DP_SACCH_START_FRAME       DPRAM_BASE_ADD + 0x0006

// AMR Link Control
#define    DP_AMR_UPLINK_CONTROL      DPRAM_BASE_ADD + 0x0007
#define    DP_AMR_DOWNLINK_CONTROL    DPRAM_BASE_ADD + 0x0008

// AMR Thresholds and Hysteresis
#define    DP_AMR_THR1_HYST1          DPRAM_BASE_ADD + 0x0009 // AMR THRESH1 & HYST1
#define    DP_AMR_THR2_HYST2          DPRAM_BASE_ADD + 0x000A // AMR THRESH2 & HYST2
#define    DP_AMR_THR3_HYST3          DPRAM_BASE_ADD + 0x000B // AMR THRESH3 & HYST3

// AMR Rx Absolute Codec Mode
#define    DP_RX_ABS_CODEC_MODE       DPRAM_BASE_ADD + 0x000C

// AMR Rx Frame type
#define    DP_RX_FRAME_TYPE           DPRAM_BASE_ADD + 0x000D

// AMR filtered C/I
#define    DP_AMR_CI_FILTERED         DPRAM_BASE_ADD + 0x000E

// AMR Tx Absolute Codec Mode
#define    DP_TX_ABS_CODEC_MODE       DPRAM_BASE_ADD + 0x000F

// AMR Tx Frame type
#define    DP_TX_FRAME_TYPE           DPRAM_BASE_ADD + 0x0010

// TCH miscellanous flag
#define    DP_TCH_FLAG                DPRAM_BASE_ADD + 0x0011

// Thresholds
#define    DP_FCH_ENERGY_THRESH       DPRAM_BASE_ADD + 0x0012 // FCH Energy Threshold
#define    DP_FCH_DETECT_THRESH       DPRAM_BASE_ADD + 0x0013 // FCH Detection Weight (4 LSBs)
#define    DP_FACCH_FR_DETECT_THRESH  DPRAM_BASE_ADD + 0x0013 // FACCH/F Detection Threshold (bits [11:8])
#define    DP_FACCH_HR_DETECT_THRESH  DPRAM_BASE_ADD + 0x0013 // FACCH/H Detection Threshold (bits [14:12])
#define    DP_TCH_ERROR_THRESH        DPRAM_BASE_ADD + 0x0014 // TCH Error Count Threshold (8 LSBs)
#define    DP_UFI_HR_THRESH           DPRAM_BASE_ADD + 0x0014 // UFI Detection Threshold
#define    DP_GPRS_FOI_THRESH         DPRAM_BASE_ADD + 0x0015 // Foi computation disable thresh

// USF estimation results
#define    DP_QUICK_USF_RES_0         DPRAM_BASE_ADD + 0x0016 // Quick USF Results on NB0(Max1 + Max2 Correlations)
#define    DP_QUICK_USF_RES_1         DPRAM_BASE_ADD + 0x0018 // Quick USF Results on NB1
#define    DP_QUICK_USF_RES_2         DPRAM_BASE_ADD + 0x001A // Quick USF Results on NB2
#define    DP_QUICK_USF_RES_3         DPRAM_BASE_ADD + 0x001C // Quick USF Results on NB3

// Incremental redundancy control
#define    DP_IRCTRL              DPRAM_BASE_ADD + 0x001E // Incremental Redundency control
#define    DP_WDWSIZE             DPRAM_BASE_ADD + 0x001E // RLC window length
#define    DP_TFI                 DPRAM_BASE_ADD + 0x001E // DP_TFI
#define    DP_IRONOFF             DPRAM_BASE_ADD + 0x001E // IR switch On:1 / Off:0
#define    DP_IRMAXMEM            DPRAM_BASE_ADD + 0x001F // Max Nunber of block allows in IR Memory
#define    DP_IRUSEDMEM           DPRAM_BASE_ADD + 0x0020 // Max Nunber of block in IR Memory
#define    DP_VQDSP               DPRAM_BASE_ADD + 0x0021 // IR DSP counter
#define    DP_VRDSP               DPRAM_BASE_ADD + 0x0022
#define    DP_VQCPU               DPRAM_BASE_ADD + 0x0023 // IR CPU counter
#define    DP_VRCPU               DPRAM_BASE_ADD + 0x0024
#define    DP_EDGEALGOCTRL        DPRAM_BASE_ADD + 0x0025 // EDGE Algorithms control

// Interferer detection
#define    DP_RX0ADJCOUNT         DPRAM_BASE_ADD + 0x0026     // Counter of detection for slot 0
#define    DP_RX1ADJCOUNT         DPRAM_BASE_ADD + 0x0027     // Counter of detection for slot 1
#define    DP_RX2ADJCOUNT         DPRAM_BASE_ADD + 0x0028     // Counter of detection for slot 2
#define    DP_RX3ADJCOUNT         DPRAM_BASE_ADD + 0x0029     // Counter of detection for slot 3

// RX EDGE Algo Autocalibration control
#define    DP_RXAUTOCALCTRLSET    DPRAM_BASE_ADD + 0x002A     // Autocalibration Control, Setting part
#define    DP_RXADJACN            DP_RXAUTOCALCTRLSET         // Number of bursts for the Adjacent detection auocal
#define    DP_RXADJACACTIVATE     DP_RXAUTOCALCTRLSET         // Bit to activate Adjacent Detection autocal

#define    DP_RXAUTOCALCTRLOK     DPRAM_BASE_ADD + 0x002B     // Autocalibration Control, indicating end of processing
#define    DP_RXADJACCOMPLETE     DP_RXAUTOCALCTRLOK          // bit set to 1 when autocal is over

// Rx Blind detection
#define    DP_RX0BDCOUNT          DPRAM_BASE_ADD + 0x002C     // 8PSK detections counter for slot 0
#define    DP_RX1BDCOUNT          DPRAM_BASE_ADD + 0x002D     // 8PSK detections counter for slot 1
#define    DP_RX2BDCOUNT          DPRAM_BASE_ADD + 0x002E     // 8PSK detections counter for slot 2
#define    DP_RX3BDCOUNT          DPRAM_BASE_ADD + 0x002F

// Channel Tracking
#define    DP_RXCHTRACKMJU        DPRAM_BASE_ADD + 0x0030     // µ parameter for the channel tracking
#define    DP_RXCHTRACKSTEP       DPRAM_BASE_ADD + 0x0031     // Step parameter for channel tracking
#define    DP_RXCHTRACKNBUPDATE   DPRAM_BASE_ADD + 0x0031     // NbUpdate parameter (int(60/Step))

// TCH Loop Back Modes
#define    DP_LOOP_BACK_MODE      DPRAM_BASE_ADD + 0x0032 // Loop Back Mode Control

// Ciphering Key
#define    DP_CIPHERING_KEY       DPRAM_BASE_ADD + 0x0033 //(8 words)

// Power Measure Results
#define    DP_PWR_RESULT          DPRAM_BASE_ADD + 0x003B //(8 words)

// Equalization Result for NB processing
#define    DP_NB_RESULT_0         DPRAM_BASE_ADD + 0x0043 // GSM Burst / GPRS Burst 0 Equalization Results
#define    DP_NB_RESULT_1         DPRAM_BASE_ADD + 0x004B // GPRS Burst 1 Equalization Results
#define    DP_NB_RESULT_2         DPRAM_BASE_ADD + 0x0053 // GPRS Burst 2 Equalization Results
#define    DP_NB_RESULT_3         DPRAM_BASE_ADD + 0x005B // GPRS Burst 3 Equalization Results

// Result for FCH research
#define    DP_FB_RESULT           DPRAM_BASE_ADD + 0x0063 // FCCH Research Results

// SCH processing results
#define    DP_SB_RESULT           DPRAM_BASE_ADD + 0x006B // SCH Equalization Results
#define    DP_SB_BUFFER           DPRAM_BASE_ADD + 0x0073
#define    DP_SB_INFO             DPRAM_BASE_ADD + 0x0074 // SB Info Buffer

//RX NBCCH Processing results
#define    DP_RX_NBCCH_BUFFER     DPRAM_BASE_ADD + 0x0076 // Rx NBCCH Block Buffer
#define    DP_RX_NBCCH_INFO       DPRAM_BASE_ADD + 0x0077

// RX Buffers TCH / GPRS /EGPRS
#define    DP_RX_BUFFERS          DPRAM_BASE_ADD + 0x0083 // (332 words)

//RX FACCH Block
#define    DP_RX_FACCH_BUFFER     DPRAM_BASE_ADD + 0x01CF // Rx FACCH Block Buffer
#define    DP_RX_FACCH_INFO       DPRAM_BASE_ADD + 0x01D0 // (12 words)

//Tx AB Block Buffer
#define    DP_TX_AB_BUFFER        DPRAM_BASE_ADD + 0x01DC
#define    DP_TX_AB_INFO          DPRAM_BASE_ADD + 0x01DC // Information Bits (8 or 11 bits)
#define    DP_TX_AB_BSIC          DPRAM_BASE_ADD + 0x01DD // BSIC (6 bits)
#define    DP_TX_AB_TSC           DPRAM_BASE_ADD + 0x01DD // Training Sequence Code (2 bits)

//Tx NBCCH Block Buffer
#define    DP_TX_NBCCH_BUFFER     DPRAM_BASE_ADD + 0x01DE // Tx NBCCH Block Buffer

// TX Buffers TCH / GPRS /EGPRS
#define    DP_TX_BUFFERS          DPRAM_BASE_ADD + 0x01EA

// Tx FACCH Block Buffer
#define    DP_TX_FACCH_BUFFER     DPRAM_BASE_ADD + 0x032A

// EDGE Energy shift before final metrics calculation
#define    DP_EDGE_METRICS_SHIFT      DPRAM_BASE_ADD + 0x0336

// GSM/GPRS I/Q shift value
#define    DP_GSM_GPRS_IQ_SHIFT       DPRAM_BASE_ADD + 0x0337
// GSM/GPRS I/Q saturation threshold
#define    DP_GSM_GPRS_IQ_SAT_THRESH  DPRAM_BASE_ADD + 0x0338

// Empty space
// 0x0339 --> 0x0339: 1 word

// TX window ramp format
#define    DP_TX_FMT_RAMP_SYMB    DPRAM_BASE_ADD + 0x033A // ramp symbol

// RFIC Rx Parameters
#define    DP_RFIC_RX_PARAM       DPRAM_BASE_ADD + 0x033B

// Extended DSP to CPU IT event source
#define    DP_DSP_EVT_SRC_EXTENDED  DPRAM_BASE_ADD + 0x033C

// DSMA transfer parameters
#define    DP_DSMA_DEST_SRC_ADD     DPRAM_BASE_ADD + 0x033D
#define    DP_DSMA_DEST_SRC_PAGE    DPRAM_BASE_ADD + 0x033E
#define    DP_DSMA_TRANSFER_LENGTH  DPRAM_BASE_ADD + 0x033F
#define    DP_DSMA_PADDING_LENGTH   DPRAM_BASE_ADD + 0x0340

// Smart Alignment control
#define    DP_SMART_ALIGNMENT_CTRL  DPRAM_BASE_ADD + 0x0341

// SAIC algorithm control
#define    DP_SAIC_CTRL             DPRAM_BASE_ADD + 0x0342

// AMR burst raw C/I
#define    DP_AMR_CI_BURST_RAW      DPRAM_BASE_ADD + 0x0343

// DSP Access to SR_BUFFER_CONTROL & SR_BURST_STATUS & SR_BLOCK_STATUS
#define    DP_DSP_WANTS_ACCESS      DPRAM_BASE_ADD + 0x0344
// CPU Access to SR_BUFFER_CONTROL & SR_BURST_STATUS & SR_BLOCK_STATUS
#define    DP_CPU_WANTS_ACCESS      DPRAM_BASE_ADD + 0x0345

// CPU AMR BFI thresholds
#define    DP_DBG_CPU_THRESH_BFI_AXS      DPRAM_BASE_ADD + 0x0346

// Adjacent decision related registers
#define    DP_DBG_REG_ADJ_BD_RESULT       DPRAM_BASE_ADD + 0x034D  // adjacent decision and blind detection result bitmap
#define    DP_DBG_REG_ADJ_DEC_USED_SCORES DPRAM_BASE_ADD + 0x034E  // scores used for adjacent decision
#define    DP_DBG_REG_ADJ_RIGHT_MPY_FACT  DPRAM_BASE_ADD + 0x034F  // right adjacent score multiply factor after division by 128
#define    DP_DBG_REG_ADJ_LEFT_MPY_FACT   DPRAM_BASE_ADD + 0x0350  // left adjacent score multiply factor after division by 128

// GSM equalizer soft output control
#define    DP_GSM_EQUAL_OUT_CTRL          DPRAM_BASE_ADD + 0x0351 // (3 words)

// Empty space
// 0x0354 --> 0x035C: 9 words

// Voice memo parameters
#define    DP_MEMO_CTRL             DPRAM_BASE_ADD + 0x035D
#define    DP_AMR_REC_CODEC_MODE    DPRAM_BASE_ADD + 0x035E

// Audio FIFO levels (if FIFO error)
#define    DP_AUDIO_FIFO_LEVEL      DPRAM_BASE_ADD + 0x035F

// Audio control parameters
#define    DP_AUDIO_FIFO_CTRL       DPRAM_BASE_ADD + 0x0360 // audio link control
#define    DP_AUDIO_FIFO_WORDS      DPRAM_BASE_ADD + 0x0361 // number of words to read/write in TX/RX FIFO (internal definition)
#define    DP_AUDIO_RUN_CTRL        DPRAM_BASE_ADD + 0x0362 // DSP SW audio control

// Mute Control
#define    DP_MUTE_CONTROL          DPRAM_BASE_ADD + 0x0363 // Mute Control

// UL/DL audio gains
#define    DP_DIGITAL_AUDIO_GAIN    DPRAM_BASE_ADD + 0x0364

// DTMF decoder
// DTMF decoder control
#define    DP_DTMF_DEC_CONTROL               DPRAM_BASE_ADD + 0x0367
// DTMF decoder parameters
#define    DP_DTMF_DEC_PARAMETERS            DPRAM_BASE_ADD + 0x0368
// DTMF decoder results and status
#define    DP_DTMF_DEC_RESULTS               DPRAM_BASE_ADD + 0x037D
// DTMF decoder calculation information
#define    DP_DTMF_DEC_CALC_INFO             DPRAM_BASE_ADD + 0x0382
// DTMF decoder debug input sample buffer
#define    DP_DTMF_DEC_DBG_IN_SAMPLE_BUFFER  DPRAM_BASE_ADD + 0x0399 // Beware: not contiguous with other DTMF decoder fields
                                                               // at DP_SPEECH_OUT_BUFFER
// Source decoding output buffer
#define    DP_SPEECH_OUT_BUFFER              DPRAM_BASE_ADD + 0x0399

// Source encoding input buffer
#define    DP_SPEECH_IN_BUFFER               DPRAM_BASE_ADD + 0x0439

// PCM playback interface
#define    DP_PCM_PLAYBACK_CTRL              DPRAM_BASE_ADD + 0x0398 // (replaces IB_MODEM_CTRL)
#define    DP_PCM_PLAYBACK_RX_BUFFER         DPRAM_BASE_ADD + 0x0399 // at DP_SPEECH_OUT_BUFFER (replaces DP_PCM_PLAYBACK_BUFFER and DP_RX_INBAND_DATA_BUFFER)
#define    DP_PCM_PLAYBACK_TX_BUFFER         DPRAM_BASE_ADD + 0x0439 // at DP_SPEECH_IN_BUFFER (replaces TX_INBAND_DATA_BUFFER)

// Voice memo buffers
#define    DP_MEMO_RECORDING_DEST_BUFFER     DPRAM_BASE_ADD + 0x04D9
#define    DP_MEMO_PLAYBACK_SRC_BUFFER       DPRAM_BASE_ADD + 0x04EA

// Empty space
// 0x04FB --> 0x076C: 626 words

// RX NB burst buffer (for Smart Alignment - TCH / NBCCH blocks)
#define    DP_RX_NB_BURST_INFO      DPRAM_BASE_ADD + 0x076D

// Debug Registers
#define    DP_DSP_DEBUG_REGS          DPRAM_BASE_ADD + 0x07E0
#define    DP_DEBUG_REG_0             DP_DSP_DEBUG_REGS + 0
#define    DP_DEBUG_REG_1             DP_DSP_DEBUG_REGS + 1
#define    DP_DEBUG_REG_2             DP_DSP_DEBUG_REGS + 2
#define    DP_DEBUG_REG_3             DP_DSP_DEBUG_REGS + 3
#define    DP_DEBUG_REG_4             DP_DSP_DEBUG_REGS + 4
#define    DP_DEBUG_REG_5             DP_DSP_DEBUG_REGS + 5
#define    DP_DEBUG_REG_6             DP_DSP_DEBUG_REGS + 6
#define    DP_DEBUG_REG_7             DP_DSP_DEBUG_REGS + 7
#define    DP_DEBUG_REG_8             DP_DSP_DEBUG_REGS + 8
#define    DP_DEBUG_REG_9             DP_DSP_DEBUG_REGS + 9
#define    DP_DEBUG_REG_10            DP_DSP_DEBUG_REGS + 10
#define    DP_DEBUG_REG_11            DP_DSP_DEBUG_REGS + 11
#define    DP_DEBUG_REG_12            DP_DSP_DEBUG_REGS + 12
#define    DP_DEBUG_REG_13            DP_DSP_DEBUG_REGS + 13
#define    DP_DEBUG_REG_14            DP_DSP_DEBUG_REGS + 14
#define    DP_DEBUG_REG_15            DP_DSP_DEBUG_REGS + 15
#define    DP_DEBUG_REG_16            DP_DSP_DEBUG_REGS + 16
#define    DP_DEBUG_REG_17            DP_DSP_DEBUG_REGS + 17
#define    DP_DEBUG_REG_18            DP_DSP_DEBUG_REGS + 18
#define    DP_DEBUG_REG_19            DP_DSP_DEBUG_REGS + 19
#define    DP_DEBUG_REG_20            DP_DSP_DEBUG_REGS + 20
#define    DP_DEBUG_REG_21            DP_DSP_DEBUG_REGS + 21
#define    DP_DEBUG_REG_22            DP_DSP_DEBUG_REGS + 22
#define    DP_DEBUG_REG_23            DP_DSP_DEBUG_REGS + 23
#define    DP_DEBUG_REG_24            DP_DSP_DEBUG_REGS + 24
#define    DP_DEBUG_REG_25            DP_DSP_DEBUG_REGS + 25
#define    DP_DEBUG_REG_26            DP_DSP_DEBUG_REGS + 26
#define    DP_DEBUG_REG_27            DP_DSP_DEBUG_REGS + 27
#define    DP_DEBUG_REG_28            DP_DSP_DEBUG_REGS + 28
#define    DP_DEBUG_REG_29            DP_DSP_DEBUG_REGS + 29
#define    DP_DEBUG_REG_30            DP_DSP_DEBUG_REGS + 30
#define    DP_DEBUG_REG_31            DP_DSP_DEBUG_REGS + 31


// -----------------------------------------------------------------------------
// RX Buffers TCH / GPRS /EGPRS
// -----------------------------------------------------------------------------

// RX NB Block Processing
// GSM/TCH Block
#define    DP_RX_TCH_BUFFER_0         DP_RX_BUFFERS              // Rx GSM TCH Block 0 Buffer
#define    DP_RX_TCH_INFO_0           DP_RX_TCH_BUFFER_0 + 4     // (79 words)

// RX EDGE Blocks
#define    DP_RX_EPDCH_BUFFER_0       DP_RX_BUFFERS              // Base address of the RX buffer 0
#define    DP_RX_EPDCH_BUFFER_1       DP_RX_EPDCH_BUFFER_0+83    // Base address of the RX buffer 1
#define    DP_RX_EPDCH_BUFFER_2       DP_RX_EPDCH_BUFFER_1+83    // Base address of the RX buffer 2
#define    DP_RX_EPDCH_BUFFER_3       DP_RX_EPDCH_BUFFER_2+83    // Base address of the RX buffer 3

// GPRS/EDGE Block 0
#define    DP_RX_PDCH_BUFFER_0        DP_RX_BUFFERS              // Rx GPRS PDCH Block 0 Buffer
#define    DP_RX_PDCH_INFO_0          DP_RX_PDCH_BUFFER_0 + 4
// GPRS/EDGE Block 1
#define    DP_RX_PDCH_BUFFER_1        DP_RX_PDCH_BUFFER_0 + 83   // Rx GPRS PDCH Block 1 Buffer
#define    DP_RX_PDCH_INFO_1          DP_RX_PDCH_BUFFER_1 + 4
// GPRS/EDGE Block 2
#define    DP_RX_PDCH_BUFFER_2        DP_RX_PDCH_BUFFER_1 + 83   // Rx GPRS PDCH Block 2 Buffer
#define    DP_RX_PDCH_INFO_2          DP_RX_PDCH_BUFFER_2 + 4
// GPRS/EDGE Block 3
#define    DP_RX_PDCH_BUFFER_3        DP_RX_PDCH_BUFFER_2 + 83   // Rx GPRS PDCH Block 3 Buffer
#define    DP_RX_PDCH_INFO_3          DP_RX_PDCH_BUFFER_3 + 4

// -----------------------------------------------------------------------------
// ; TX Buffers TCH / GPRS /EGPRS
// -----------------------------------------------------------------------------

// TX GSM/TCH Block
#define    DP_TX_TCH_BUFFER_0         DP_TX_BUFFERS              // Tx GSM TCH Block 0 Buffer
#define    DP_TX_TCH_INFO_0           DP_TX_TCH_BUFFER_0+ 1

// TX GPRS Blocks
#define    DP_TX_PDCH_BUFFER_0        DP_TX_BUFFERS              // Tx GPRS PDCH Block 0 Buffer
#define    DP_TX_PDCH_BUFFER_1        DP_TX_PDCH_BUFFER_0 + 80   // Tx GPRS PDCH Block 1 Buffer
#define    DP_TX_PDCH_BUFFER_2        DP_TX_PDCH_BUFFER_1 + 80   // Tx GPRS PDCH Block 2 Buffer
#define    DP_TX_PDCH_BUFFER_3        DP_TX_PDCH_BUFFER_2 + 80   // Tx GPRS PDCH Block 3 Buffer

#define    DP_TX_PDCH_INFO_0          DP_TX_PDCH_BUFFER_0 + 1
#define    DP_TX_PDCH_INFO_1          DP_TX_PDCH_BUFFER_1 + 1
#define    DP_TX_PDCH_INFO_2          DP_TX_PDCH_BUFFER_2 + 1
#define    DP_TX_PDCH_INFO_3          DP_TX_PDCH_BUFFER_3 + 1

// TX EGPRS Blocks
#define    DP_TX_EPDCH_BUFFER_0   DP_TX_BUFFERS              // Base address of the TX buffer 0
#define    DP_TX_EPDCH_BUFFER_1   DP_TX_EPDCH_BUFFER_0+80    // Base address of the TX buffer 1
#define    DP_TX_EPDCH_BUFFER_2   DP_TX_EPDCH_BUFFER_1+80    // Base address of the TX buffer 2
#define    DP_TX_EPDCH_BUFFER_3   DP_TX_EPDCH_BUFFER_2+80    // Base address of the TX buffer 3

// -----------------------------------------------------------------------------
// DP_DSP_VERSION_ID                        fields
// -----------------------------------------------------------------------------
#define    DSP_ROM_VERSION_MASK        0x00FF  // not used
#define    DSP_ROM_VERSION_SHIFT       0       // not used
#define    DSP_PATCH_VERSION_MASK      0xFF00  // not used
#define    DSP_PATCH_VERSION_SHIFT     8

// -----------------------------------------------------------------------------
// DP_DSP_ERROR_LABEL1 definitions          set/rst
// -----------------------------------------------------------------------------
#define    VOCODEC_ERR             0x0001  // Vocodec unknown
#define    CIPHER_ALGO_ERR         0x0002  // Ciphering Algorithm unknown
#define    CHANNEL_MODE_ERR        0x0004  // Channel Mode unknown
#define    TCH_TYPE_ERR            0x0008  // TCH Type unknown

#define    RX_SAMPLE_NB_NEG        0x0010  // negative number of samples to read in RxPort
#define    RX_SAMPLE_NB_SUP        0x0020  // too many samples to read in RxPort
#define    RX_PROC_ERR             0x0040  // Rx Process Type unknown
#define    INT_NMI_ERR             0x0080  // Unsollicited NMI

#define    TX_PROCESS_ERR          0x0100  // Tx Process Type unknown
#define    TX_NBCCH_FULL_ERR       0x0200  // NBCCH Buffer full flag clear
#define    TX_TCH_FULL_ERR         0x0400  // TCH Buffer full flag clear
#define    TX_RACH_FULL_ERR        0x0800  // PDCH Tx Process: No RACH to encode (to remove)

#define    TX_PDCH_BITMAP_ERR      0x1000  // PDCH Tx Process: bitmap error
#define    TX_PDCH_CS_ERR          0x2000  // PDCH Tx Process: coding scheme unknown (dpram)
#define    TX_PDCH_AB_ERR          0x4000  // PDCH AB Process: coding scheme unknown (dpram)
#define    TX_PDCH_NB_ERR          0x8000  // conflict between PDCH Tx Process and AB Process

// -----------------------------------------------------------------------------
// DP_DSP_ERROR_LABEL2 definitions          set/rst
// -----------------------------------------------------------------------------
#define    RX_AUDIO_BUF_PTR_ERR    0x0001  // DSP RX audio buffer pointers out of range
#define    TX_AUDIO_BUF_PTR_ERR    0x0002  // DSP TX audio buffer pointers out of range
#define    CPU_REQUEST_ERR         0x0008
#define    TX_EPDCH_NOINIT_ERR     0x0010  // TX EPDCH process Error : Attempting to perform BurstMapping
                                        // but no init done (~bad SR_NBCCH_TX_COUNTER value)
#define    MEMO_VOCODEC_ERR        0x0020  // invalid speech codec for memo
#define    CIPHER_COPRO_TIME_OUT   0x0200  // error when no CIPHER copro response
#define    MEMO_AMR_FT_ERR         0x0400  // invalid memo AMR FT field (FT>7)
// FBO: MEMO error on bit 10
#ifdef _AUDIO_FIFO_DEBUG
  #define    RX_AUDIO_FIFO_FLOW_ERR  0x0800  // error of overflow/underflow in audio FIFO OUT
  #define    TX_AUDIO_FIFO_FLOW_ERR  0x1000  // error of overflow/underflow in audio FIFO IN
#endif


// -----------------------------------------------------------------------------
// DP_TCH_CHANNEL_ATTRIBUTE BITFIELDS       and
// -----------------------------------------------------------------------------
#define    TCH_CHANNEL_ATTRIBUTE_MASK      0x03FF
#define    TCH_TYPE                        0x007F
#define    DTX_UPLINK                      (1<<7)
#define    DTX_DOWNLINK                    (1<<8)
#define    HR_SUBCHANNEL                   (1<<9)
// -----------------------------------------------------------------------------
// DP_TCH_CHANNEL_ATTRIBUTE DEFINITIONS     cmp
// -----------------------------------------------------------------------------
#define    TCH_FULLSPEECH              0x0002
#define    TCH_HALFSPEECH              0x0003
#define    TCH_AMR_FULLSPEECH          0x0006
#define    TCH_AMR_HALFSPEECH          0x0007
#define    TCH_DATA_F144               0x0008
#define    TCH_DATA_F96                0x000A
#define    TCH_DATA_F48                0x000C
#define    TCH_DATA_F24                0x000E
#define    TCH_DATA_H48                0x000B
#define    TCH_DATA_H24                0x000D
#define    DTX_UP_OK                   (1<<7)
#define    DTX_DOWN_OK                 (1<<8)
#define    HR_SUBCHANNEL_0             (0<<9)
#define    HR_SUBCHANNEL_1             (1<<9)
// -----------------------------------------------------------------------------
// DP_TCH_CHANNEL_ATTRIBUTE DEFINITIONS     tstb
// -----------------------------------------------------------------------------
#define    TCH_HALFRATE_BIT        0
#define    TCH_DATA_BIT            3
#define    DTX_UPLINK_BIT          7
#define    DTX_DOWNLINK_BIT        8
#define    HR_SUBCHANNEL_BIT       9

// -----------------------------------------------------------------------------
// DP_SACCH_START_FRAME DEFINITIONS         and
// -----------------------------------------------------------------------------
#define    SACCH_START         0x00ff

// -----------------------------------------------------------------------------
// DP_AMR_UPLINK_CONTROL                        tstb
// -----------------------------------------------------------------------------
#define    UP_ACS_BIT          0
#define    UP_ICM_BIT          8
#define    UP_ACK_BIT          10
// -----------------------------------------------------------------------------
// DP_AMR_UPLINK_CONTROL                        rst
// -----------------------------------------------------------------------------
#define    UP_ACS_MASK         0xFF<<UP_ACS_BIT
#define    UP_ICM_MASK         0x03<<UP_ICM_BIT
#define    UP_ACK_MASK         0x03<<UP_ACK_BIT
// -----------------------------------------------------------------------------
// DP_AMR_DOWNLINK_CONTROL                      tstb
// -----------------------------------------------------------------------------
#define    DWN_ACS_BIT                   0
#define    DWN_ICM_BIT                   8
#define    DWN_PHASE_BIT                 10
#define    INVALID_THRESH_SET_BIT        11
#define    DBG_CPU_THRESH_BFI_AXS_EN_BIT 15
// -----------------------------------------------------------------------------
// DP_AMR_DOWNLINK_CONTROL                      rst
// -----------------------------------------------------------------------------
#define    DWN_ACS_MASK               0xFF<<DWN_ACS_BIT
#define    DWN_ICM_MASK               0x03<<DWN_ICM_BIT
#define    DWN_PHASE_MASK             0x01<<DWN_PHASE_BIT
#define    INVALID_THRESH_SET_MASK    0x01<<INVALID_THRESH_SET_BIT

// -----------------------------------------------------------------------------
// AMR THRESHOLDS and HYSTERESIS                tstb
// -----------------------------------------------------------------------------
#define    AMR_THR_BIT            0
#define    AMR_HYST_BIT           6
// -----------------------------------------------------------------------------
// AMR THRESHOLDS and HYSTERESIS                and
// -----------------------------------------------------------------------------
#define    AMR_THR_MASK           0x3F<<AMR_THR_BIT
#define    AMR_HYST_MASK          0x0F<<AMR_HYST_BIT

// -----------------------------------------------------------------------------
// DP_TCH_FLAG                                  tstb
// -----------------------------------------------------------------------------
#define    DRX_FLAG_BIT            0
#define    TX_DTX_UP_BIT           1
#define    TX_SPEECH_BIT           2
#define    CMR_BIT                 4
#define    CMI_UL_BIT              6
#define    CMC_BIT                 8
#define    CMI_DL_BIT              10
#define    UL_IB_INDICATION_BIT    12
#define    DL_IB_INDICATION_BIT    13
// -----------------------------------------------------------------------------
// DP_TCH_FLAG                                  set/rst
// -----------------------------------------------------------------------------
#define    DRX_FLAG_MASK           1<<DRX_FLAG_BIT
#define    TX_DTX_USED             0x0002              // DTX Up-Link Indicator
#define    TX_SPEECH_DETECTED      0x0004              // Speech Detection Indicator (from vocoder)
#define    CMR_MASK                3<<CMR_BIT
#define    CMI_UL_MASK             3<<CMI_UL_BIT
#define    CMC_MASK                3<<CMC_BIT
#define    CMI_DL_MASK             3<<CMI_DL_BIT
#define    UL_IB_INDICATION_MASK   1<<UL_IB_INDICATION_BIT
#define    DL_IB_INDICATION_MASK   1<<DL_IB_INDICATION_BIT

// -----------------------------------------------------------------------------
// THRESHOLDS                                and
// -----------------------------------------------------------------------------
#define    FCH_DETECT               0x000F
#define    FACCH_FR_DETECT_MASK     0x0F00
#define    FACCH_HR_DETECT_MASK     0xF000
#define    TCH_ERROR                0x00FF
#define    UFI_THRES_MASK           0xFF00
#define    GPRS_FOI_THRESH_MASK     0xFFFF
// -----------------------------------------------------------------------------
// THRESHOLDS                                shfi
// -----------------------------------------------------------------------------
#define    FACCH_FR_DETECT_SHIFT    -8
#define    FACCH_HR_DETECT_SHIFT    -12
#define    UFI_THRES_SHIFT          -8

// -----------------------------------------------------------------------------
// DP_IRCTRL                                    and
// -----------------------------------------------------------------------------
#define    WDWSIZE_MASK         0x03ff
#define    TFI_MASK             0x7c00
#define    IRONOFF_MASK         0b1000000000000000
#define    IRUSEDMEM_MASK       0x7f
#define    IRMAXMEM_MASK        0x3f80

// -----------------------------------------------------------------------------
// DP_IRCTRL                                    shfi
// -----------------------------------------------------------------------------
#define    WDWSIZE_SHIFT       0
#define    TFI_SHIFT           -10
#define    IRONOFF_SHIFT       -15
#define    IRUSEDMEM_SHIFT     0
#define    IRMAXMEM_SHIFT      -7

// -----------------------------------------------------------------------------
// DP_EDGEALGOCTRL
// -----------------------------------------------------------------------------
#define    IRTEST_MASK         0x0001
#define    IRTEST_SHFT         0

#define    MSWMF_MASK          0x0002
#define    MSWMF_SHFT          -1

#define    CSTSNOLOAD_MASK     0x0004
#define    CSTSNOLOAD_SHFT     -2

#define    IRBUFFSPLIT_MASK    0x0008
#define    IRBUFFSPLIT_SHFT    -3

// -----------------------------------------------------------------------------
// DP_RXAUTOCALCTRLSET                  and
// -----------------------------------------------------------------------------
#define    RXADJACN_MASK            0x000f
#define    RXADJACACTIVATE_MASK     0x0010
// -----------------------------------------------------------------------------
// DP_RXAUTOCALCTRLOK                   and
// -----------------------------------------------------------------------------
#define    RXADJACCOMPLETE_MASK     0x0001

// -----------------------------------------------------------------------------
// DP_RXAUTOCALCTRLSET                  shfi
// -----------------------------------------------------------------------------
#define    RXADJACN_SHFT            0
#define    RXADJACACTIVATE_SHFT     -4

// -----------------------------------------------------------------------------
// DP_RXAUTOCALCTRLOK                   shfi
// -----------------------------------------------------------------------------
#define    RXADJACCOMPLETE_SHFT     0

// -----------------------------------------------------------------------------
// RXCHTRACKING
// DP_RXCHTRACKSTEP, DP_RXCHTRACKNBUPDATE                  and
// -----------------------------------------------------------------------------
#define    RXCHTRACKSTEP_MASK       0x00ff
#define    RXCHTRACKNBUPDATE_MASK   0xff00
// -----------------------------------------------------------------------------
// RXCHTRACKING
// DP_RXCHTRACKSTEP, DP_RXCHTRACKNBUPDATE                  shfi
// -----------------------------------------------------------------------------
#define    RXCHTRACKSTEP_SHFI       0
#define    RXCHTRACKNBUPDATE_SHFI   -8


// -----------------------------------------------------------------------------
// DP_LOOP_BACK_MODE                        tstb
// -----------------------------------------------------------------------------
#define    A_LOOP_BIT            0
#define    B_LOOP_BIT            1
#define    C_LOOP_BIT            2
#define    D_LOOP_BIT            3
#define    E_LOOP_BIT            4
#define    F_LOOP_BIT            5
#define    I_LOOP_BIT            6
#define    VOCO_LOOP_BIT         8
#define    EDGE_LOOP_BIT         9

// -----------------------------------------------------------------------------
// DP_LOOP_BACK_MODE                        and
// -----------------------------------------------------------------------------
#define    A_LOOP                (1<<A_LOOP_BIT)
#define    B_LOOP                (1<<B_LOOP_BIT)
#define    C_LOOP                (1<<C_LOOP_BIT)
#define    D_LOOP                (1<<D_LOOP_BIT)
#define    E_LOOP                (1<<E_LOOP_BIT)
#define    F_LOOP                (1<<F_LOOP_BIT)
#define    I_LOOP                (1<<I_LOOP_BIT)
#define    VOCO_LOOP             (1<<VOCO_LOOP_BIT)
#define    EDGE_LOOP_BACK_MASK   (1<<EDGE_LOOP_BIT)
// -----------------------------------------------------------------------------
// DP_LOOP_BACK_MODE                        shfi
// -----------------------------------------------------------------------------
#define    EDGE_LOOP_BACK_SHFT   -9

// -----------------------------------------------------------------------------
// DP_SB_BUFFER                                     tstb
// -----------------------------------------------------------------------------
#define    SB_BFI_BIT            0
#define    SB_ERROR_COUNT_BIT    8
// -----------------------------------------------------------------------------
// DP_SB_BUFFER                                     set/rst
// -----------------------------------------------------------------------------
#define    SB_BFI_MASK           (1<<SB_BFI_BIT)
#define    SB_ERROR_COUNT_MASK   (0xFF<<SB_ERROR_COUNT_BIT)
// -----------------------------------------------------------------------------
// DP_SB_BUFFER                                     shfi
// -----------------------------------------------------------------------------
#define    SB_ERROR_COUNT_SHIFT   8 // error count on 8 MSBs

// -----------------------------------------------------------------------------
// DP_RX_NBCCH_BUFFER                               tstb
// -----------------------------------------------------------------------------
#define    NBCCH_BFI_BIT            0
#define    NBCCH_ERROR_COUNT_BIT    8
// -----------------------------------------------------------------------------
// DP_RX_NBCCH_BUFFER                               set/rst
// -----------------------------------------------------------------------------
#define    NBCCH_BFI_MASK           (1<<NBCCH_BFI_BIT)      // Bad Frame Indicator
#define    NBCCH_ERROR_COUNT_MASK   (0xFF<<NBCCH_ERROR_COUNT_BIT)
// -----------------------------------------------------------------------------
// DP_RX_NBCCH_BUFFER                               shfi
// -----------------------------------------------------------------------------
#define    NBCCH_ERROR_COUNT_SHIFT  8               // error count on 8 MSBs

// -----------------------------------------------------------------------------
// RX_TCH_BUFFER_x or RX_PDCH_BUFFER_x   Offset
// -----------------------------------------------------------------------------
#define    ERROR_COUNT_OFFSET       1
#define    RX_INFO_OFFSET           4
// -----------------------------------------------------------------------------
// RX_TCH_BUFFER_x or RX_PDCH_BUFFER_x   tstb
// -----------------------------------------------------------------------------
#define    RX_CODING_SCHEME_BIT     0
#define    RX_ERROR_COUNT_BIT       0
#define    RX_BFI_BIT               4
#define    RX_TAF_BIT               7
#define    RX_SID_BIT               8
#define    RX_UFI_BIT               10
// -----------------------------------------------------------------------------
// RX_TCH_BUFFER_x or RX_PDCH_BUFFER_x   set/rst
// -----------------------------------------------------------------------------
#define    RX_BFI_MASK              (1<<RX_BFI_BIT)             // Bad Frame Indicator
#define    RX_TAF_MASK              (1<<RX_TAF_BIT)             // Time Alignment Flag
#define    RX_SID_MASK              (3<<RX_SID_BIT)             // Silence Descriptor
#define    RX_UFI_MASK              (1<<RX_UFI_BIT)             // Unreliable Frame Indicator
#define    RX_ERROR_COUNT_MASK      (0xFF<<RX_ERROR_COUNT_BIT)
// -----------------------------------------------------------------------------
// RX_TCH_BUFFER_x or RX_PDCH_BUFFER_x   shfi
// -----------------------------------------------------------------------------
#define    RX_BFI_SHIFT             4
#define    RX_TAF_SHIFT             7
#define    RX_SID_SHIFT             8
#define    RX_UFI_SHIFT             9
#define    RX_PDCH_CS_SHIFT         0
#define    RX_CODING_SCHEME_SHIFT   0
#define    RX_ERROR_COUNT_SHIFT     0
// -----------------------------------------------------------------------------
// RX_TCH_BUFFER_x or RX_PDCH_BUFFER_x   and
// -----------------------------------------------------------------------------
#define    RX_PDCH_CS_MASK          (0xF<<RX_PDCH_CS_SHIFT)
#define    RX_PDCH_CS1              (0<<RX_PDCH_CS_SHIFT)
#define    RX_PDCH_CS2              (1<<RX_PDCH_CS_SHIFT)
#define    RX_PDCH_CS3              (2<<RX_PDCH_CS_SHIFT)
#define    RX_PDCH_CS4              (3<<RX_PDCH_CS_SHIFT)



// -----------------------------------------------------------------------------
// Rx EPDCH buffers structure
// -----------------------------------------------------------------------------

#define    RX_EDGE_MCS_OFFSET              0         // Offset of the MCS indication
#define    RX_EDGE_MCS_MASK                0x000f    // Mask of the MCS indication in DPRAM
#define    RX_EDGE_MCS_SHFT                0

#define    RX_EDGE_HEADER_BFI_OFFSET       0         // Offset of the Header BFI bit
#define    RX_EDGE_HEADER_BFI_MASK         0x0010    // Mask of the Header BFI bit
#define    RX_EDGE_HEADER_BFI_SHFT         4         // shift of the Header BFI bit

#define    RX_EDGE_BFI1_OFFSET             0         // Offset of the BFI bit block 1 from the base address
#define    RX_EDGE_BFI1_SET                0b100000  // Mask to set this bit

#define    RX_EDGE_BFI2_OFFSET             0         // Offset of the BFI bit block 2 from the base address
#define    RX_EDGE_BFI2_SET                0b1000000 // Mask to set this bit

#define    RX_EDGE_MEAN_BEP_BLOCK_OFFSET   2
#define    RX_EDGE_MEAN_BEP_BLOCK_SHIFT    0
#define    RX_EDGE_MEAN_BEP_BLOCK_MASK     0x7fff

#define    RX_EDGE_CV_BEP_BLOCK_OFFSET     3
#define    RX_EDGE_CV_BEP_BLOCK_SHIFT      0
#define    RX_EDGE_CV_BEP_BLOCK_MASK       0x7fff

#define    RX_EDGE_HEADER_OFFSET           4         // Offset of the header buffer

#define    RX_EDGE_EPDCH_BLOCK1_OFFSET     7         // Offset of the block 1 buffer
#define    RX_EDGE_EPDCH_BLOCK2_OFFSET     45        // Offset of the block 2 buffer

#define    RX_EDGE_HEADER_BUFFER_SIZE      3
#define    RX_EDGE_DATA_BUFFER_SIZE        (79-3)

// -----------------------------------------------------------------------------
// MCS Aliases
// -----------------------------------------------------------------------------
#define    MCS1                     4
#define    MCS2                     5
#define    MCS3                     6
#define    MCS4                     7
#define    MCS5                     8
#define    MCS6                     9
#define    MCS7                     10
#define    MCS8                     11
#define    MCS9                     12
#define    MCS_ERROR                0xf

// -----------------------------------------------------------------------------
// RX_EPDCH_BUFFER_x                         and
// -----------------------------------------------------------------------------
#define    RX_EPDCH_MCS1            (MCS1<<RX_EDGE_MCS_SHFT)
#define    RX_EPDCH_MCS2            (MCS2<<RX_EDGE_MCS_SHFT)
#define    RX_EPDCH_MCS3            (MCS3<<RX_EDGE_MCS_SHFT)
#define    RX_EPDCH_MCS4            (MCS4<<RX_EDGE_MCS_SHFT)
#define    RX_EPDCH_MCS5            (MCS5<<RX_EDGE_MCS_SHFT)
#define    RX_EPDCH_MCS6            (MCS6<<RX_EDGE_MCS_SHFT)
#define    RX_EPDCH_MCS7            (MCS7<<RX_EDGE_MCS_SHFT)
#define    RX_EPDCH_MCS8            (MCS8<<RX_EDGE_MCS_SHFT)
#define    RX_EPDCH_MCS9            (MCS9<<RX_EDGE_MCS_SHFT)
#define    RX_EPDCH_MCS_ERROR       (MCS_ERROR<<RX_EDGE_MCS_SHFT)

// -----------------------------------------------------------------------------
// DP_RX_FACCH_BUFFER                           tstb
// -----------------------------------------------------------------------------
#define    FACCH_BFI_BIT            0
#define    FACCH_ERROR_COUNT_BIT    8
// -----------------------------------------------------------------------------
// DP_RX_FACCH_BUFFER                       set/rst
// -----------------------------------------------------------------------------
#define    FACCH_BFI_MASK           (1<<FACCH_BFI_BIT)      // Bad Frame Indicator
#define    FACCH_ERROR_COUNT_MASK   (0xFF<<FACCH_ERROR_COUNT_BIT)
// -----------------------------------------------------------------------------
// DP_RX_FACCH_BUFFER                           shfi
// -----------------------------------------------------------------------------
#define    FACCH_ERROR_COUNT_SHIFT  8    // error count on 8 MSBs

// -----------------------------------------------------------------------------
// DP_TX_AB_TSC
// -----------------------------------------------------------------------------
#define    TX_AB_TSC_BIT            8
#define    TX_AB_TSC_MSK            (0x3<<TX_AB_TSC_BIT)

#define    TX_AB_TSC_TS0            0    // GSM/GPRS
#define    TX_AB_TSC_TS1            1    // E-GPRS TS1
#define    TX_AB_TSC_TS2            2    // E-GPRS TS2

// -----------------------------------------------------------------------------
// DP_TX_TCH_BUFFER_x or TX_PDCH_BUFFER_x   and
// -----------------------------------------------------------------------------
#define    TX_PDCH_CS               0x000F
#define    TX_PDCH_CS_AB            0x060F
#define    TX_PDCH_AB_MASK          0x0600
// -----------------------------------------------------------------------------
// DP_TX_TCH_BUFFER_x or TX_PDCH_BUFFER_x   shfi
// -----------------------------------------------------------------------------
#define    TX_PDCH_CS_SHIFT         0
#define    TX_PDCH_AB_SHIFT         9
// -----------------------------------------------------------------------------
// DP_TX_TCH_BUFFER_x or TX_PDCH_BUFFER_x   cmp
// -----------------------------------------------------------------------------
#define    TX_PDCH_CS1              (0<<TX_PDCH_CS_SHIFT)
#define    TX_PDCH_CS2              (1<<TX_PDCH_CS_SHIFT)
#define    TX_PDCH_CS3              (2<<TX_PDCH_CS_SHIFT)
#define    TX_PDCH_CS4              (3<<TX_PDCH_CS_SHIFT)
#define    TX_PDCH_PRACH            (1<<TX_PDCH_AB_SHIFT)
#define    TX_PDCH_CPRACH           (2<<TX_PDCH_AB_SHIFT)

// -----------------------------------------------------------------------------
// Tx EPDCH buffers structure
// -----------------------------------------------------------------------------
#define    TX_EDGE_MCS_OFFSET       0             // Offset of the MCS indication
#define    TX_EDGE_MCS_MASK         0x000f        // Mask for this indication
#define    TX_EDGE_MCS_SHIFT        0             // Shift of the MCS indication

#define    TX_EDGE_PADD_OFFSET      0             // Offset for the padding flag
#define    TX_EDGE_PADD_MASK        0x10          // mask for the padding flag
#define    TX_EDGE_PADD_SHIFT       -4            // shift to extract the padding flag

#define    TX_EDGE_PS1_OFFSET       0             // Offset of the PS1 indication
#define    TX_EDGE_PS1_MASK         0x0060        // Mask for this indication
#define    TX_EDGE_PS1_SHIFT        -5            // Shift of the PS1 indication

#define    TX_EDGE_PS2_OFFSET       0             // Offset of the PS2 indication
#define    TX_EDGE_PS2_MASK         0x0180        // Mask  for this indication
#define    TX_EDGE_PS2_SHIFT        -7            // Shift of the PS2 indication

#define    TX_AB_TYPE_OFFSET        0             // Access Burst type 0 : AB8 1 : AB11
#define    TX_AB_TYPE_SHIFT         -9            // Shift for the access burst type
#define    TX_AB_TYPE_MASK          0b11000000000 // Mask for the access burst type


#define    TX_AB_TSC_OFFSET         0           // Access Burst training sequence 0 : TS1  1 : TSC 2
#define    TX_AB_TSC_SHIFT          -11         // Shift for the access burst tsc
#define    TX_AB_TSC_MASK           0x0800      // Mask for the access burst tsc



#define    TX_EDGE_HEADER_OFFSET       1        // Offset of the header TX buffer
#define    TX_EDGE_EPDCH_BLOCK1_OFFSET 4        // Offset of the block 1 buffer
#define    TX_EDGE_EPDCH_BLOCK2_OFFSET 42       // Offset of the block 2 buffer

// -----------------------------------------------------------------------------
// DP_TX_EPDCH_BUFFER_x                         and
// -----------------------------------------------------------------------------
#define    TX_EPDCH_MCS1            (MCS1<<TX_EDGE_MCS_SHIFT)
#define    TX_EPDCH_MCS2            (MCS2<<TX_EDGE_MCS_SHIFT)
#define    TX_EPDCH_MCS3            (MCS3<<TX_EDGE_MCS_SHIFT)
#define    TX_EPDCH_MCS4            (MCS4<<TX_EDGE_MCS_SHIFT)
#define    TX_EPDCH_MCS5            (MCS5<<TX_EDGE_MCS_SHIFT)
#define    TX_EPDCH_MCS6            (MCS6<<TX_EDGE_MCS_SHIFT)
#define    TX_EPDCH_MCS7            (MCS7<<TX_EDGE_MCS_SHIFT)
#define    TX_EPDCH_MCS8            (MCS8<<TX_EDGE_MCS_SHIFT)
#define    TX_EPDCH_MCS9            (MCS9<<TX_EDGE_MCS_SHIFT)
#define    TX_EPDCH_MCS_ERROR       (MCS_ERROR<<TX_EDGE_MCS_SHIFT)


// -----------------------------------------------------------------------------
// DP_EDGE_METRICS_SHIFT                            fields
// -----------------------------------------------------------------------------
#define    EDGE_METRICS_SHIFT_MASK          0x000F

// -----------------------------------------------------------------------------
// DP_TX_FMT_RAMP_SYMB
// -----------------------------------------------------------------------------
#define    TX_FMT_RAMP_SYMB_BIT_GMSK     0
#define    TX_FMT_RAMP_SYMB_BIT_8PSK     8
#define    TX_FMT_RAMP_SYMB_MASK_GMSK    (0x3<<TX_FMT_RAMP_SYMB_BIT_GMSK)
#define    TX_FMT_RAMP_SYMB_MASK_8PSK    (0xF<<TX_FMT_RAMP_SYMB_BIT_8PSK)

// -----------------------------------------------------------------------------
// DP_RFIC_RX_PARAM
// -----------------------------------------------------------------------------
#define    RFIC_RX_WIDTH            DP_RFIC_RX_PARAM
#define    RFIC_RX_WIDTH_MSK        (0x1F)   // RFIC_RX_WIDTH_MAX = 16 = 0x1F (min = 10)

// -----------------------------------------------------------------------------
// DP_SMART_ALIGNMENT_CTRL                          tstb
// -----------------------------------------------------------------------------
#define    SA_ON_BIT         0

// -----------------------------------------------------------------------------
// DP_SAIC_CTRL                                     tstb
// -----------------------------------------------------------------------------
#define    SAIC_ON_BIT       0

// -----------------------------------------------------------------------------
// DP_DSP_EVT_SRC_EXTENDED
// -----------------------------------------------------------------------------
#define    RESET_COMPLETE             (1<<0)
#define    DEBUG_TRACE_COMPLETE       (1<<1)
#define    DSMA_CPUTODSP1_DL_COMPLETE (1<<2)
#define    DSMA_CPUTODSP2_DL_COMPLETE (1<<3)
#define    DSMA_DSPTOCPU1_UL_COMPLETE (1<<4)
#define    DSMA_DSPTOCPU2_UL_COMPLETE (1<<5)

// -----------------------------------------------------------------------------
// DP_DSP_WANTS_ACCESS                              set/rst
// -----------------------------------------------------------------------------
#define    DSP_ASK_ACCESS          1
// -----------------------------------------------------------------------------
// DP_CPU_WANTS_ACCESS                              set/rst
// -----------------------------------------------------------------------------
#define    CPU_ASK_ACCESS          1

// -----------------------------------------------------------------------------
// DP_DBG_REG_ADJ_BD_RESULT                         set/rst/and
// -----------------------------------------------------------------------------
#define    ADJ_DETECTED_BURST_0_BIT         0
#define    ADJ_DETECTED_BURST_1_BIT         1
#define    ADJ_DETECTED_BURST_2_BIT         2
#define    ADJ_DETECTED_BURST_3_BIT         3
#define    BLIND_DETECTION_BURST_0_BIT      4
#define    BLIND_DETECTION_BURST_1_BIT      5
#define    BLIND_DETECTION_BURST_2_BIT      6
#define    BLIND_DETECTION_BURST_3_BIT      7

// -----------------------------------------------------------------------------
// DP_DBG_REG_ADJ_BD_RESULT                         fields
// -----------------------------------------------------------------------------
#define    ADJ_DETECTED_BURST_0             (1<<ADJ_DETECTED_BURST_0_BIT)
#define    ADJ_DETECTED_BURST_1             (1<<ADJ_DETECTED_BURST_1_BIT)
#define    ADJ_DETECTED_BURST_2             (1<<ADJ_DETECTED_BURST_2_BIT)
#define    ADJ_DETECTED_BURST_3             (1<<ADJ_DETECTED_BURST_3_BIT)
#define    BLIND_DETECTION_BURST_0          (1<<BLIND_DETECTION_BURST_0_BIT)
#define    BLIND_DETECTION_BURST_1          (1<<BLIND_DETECTION_BURST_1_BIT)
#define    BLIND_DETECTION_BURST_2          (1<<BLIND_DETECTION_BURST_2_BIT)
#define    BLIND_DETECTION_BURST_3          (1<<BLIND_DETECTION_BURST_3_BIT)

// -----------------------------------------------------------------------------
// DP_DBG_REG_ADJ_DEC_USED_SCORES                   tstb
// -----------------------------------------------------------------------------
#define    ADJ_LEFT_USE_FAD_L_BIT           0
#define    ADJ_LEFT_USE_COCH_L_BIT          1
#define    ADJ_LEFT_USE_FAD_R_BIT           2
#define    ADJ_LEFT_USE_COCH_R_BIT          3
#define    ADJ_RIGHT_USE_FAD_L_BIT          4
#define    ADJ_RIGHT_USE_COCH_L_BIT         5
#define    ADJ_RIGHT_USE_FAD_R_BIT          6
#define    ADJ_RIGHT_USE_COCH_R_BIT         7

// -----------------------------------------------------------------------------
// DP_GSM#defineAL_OUT_CTRL                            fields
// -----------------------------------------------------------------------------
#define    SO_DOWNSCALE_SHIFT_1_SHIFT      0
#define    FINAL_METRIC_THRESH_1_SHIFT     -8
#define    SO_DOWNSCALE_SHIFT_2_SHIFT      0
#define    FINAL_METRIC_THRESH_2_SHIFT     -8
#define    SO_DOWNSCALE_SHIFT_3_SHIFT      0
#define    TOI_THRESH_SHIFT                -8
#define    SO_DOWNSCALE_SHIFT_1_MASK       0x00FF
#define    FINAL_METRIC_THRESH_1_MASK      0xFF00
#define    SO_DOWNSCALE_SHIFT_2_MASK       0x00FF
#define    FINAL_METRIC_THRESH_2_MASK      0xFF00
#define    SO_DOWNSCALE_SHIFT_3_MASK       0x00FF
#define    TOI_THRESH_MASK                 0xFF00

// Audio part
// ----------

// -----------------------------------------------------------------------------
// DP_MEMO_CTRL                                     tstb
// -----------------------------------------------------------------------------
#define    MEMO_REC_RESET_BIT         0
#define    MEMO_PLAY_RESET_BIT        1
#define    MEMO_REC_ON_BIT            2
#define    MEMO_PLAY_ON_BIT           3
#define    MEMO_REC_DL_PATH_BIT       4
#define    MEMO_REC_UL_PATH_BIT       5
#define    MEMO_PLAY_DL_PATH_BIT      6
#define    MEMO_PLAY_UL_PATH_BIT      7
#define    MEMO_REC_MODE              8
#define    MEMO_PLAY_MODE            10
#define    MEMO_REC_BUFFER_FULL_BIT  12
#define    MEMO_PLAY_BUFFER_FULL_BIT 13

// -----------------------------------------------------------------------------
// DP_MEMO_CTRL                                     set/rst/and
// -----------------------------------------------------------------------------
#define    MEMO_REC_RESET             1<<MEMO_REC_RESET_BIT
#define    MEMO_PLAY_RESET            1<<MEMO_PLAY_RESET_BIT
#define    MEMO_REC_ON                1<<MEMO_REC_ON_BIT            // not used
#define    MEMO_PLAY_ON               1<<MEMO_PLAY_ON_BIT           // not used
#define    MEMO_REC_DL_PATH           1<<MEMO_REC_DL_PATH_BIT       // not used
#define    MEMO_REC_UL_PATH           1<<MEMO_REC_UL_PATH_BIT       // not used
#define    MEMO_PLAY_DL_PATH          1<<MEMO_PLAY_DL_PATH_BIT      // not used
#define    MEMO_PLAY_UL_PATH          1<<MEMO_PLAY_UL_PATH_BIT      // not used
#define    MEMO_REC_MODE_MASK         3<<MEMO_REC_MODE
#define    MEMO_PLAY_MODE_MASK        3<<MEMO_PLAY_MODE
#define    MEMO_REC_BUFFER_FULL_MASK  1<<MEMO_REC_BUFFER_FULL_BIT   // set by the memo recorder
#define    MEMO_PLAY_BUFFER_FULL_MASK 1<<MEMO_PLAY_BUFFER_FULL_BIT  // read by the memo player

// MEMO_REC_MODE / MEMO_PLAY_MODE definitions
// Speech vocoder and frame format
//  0b00 -> FR vocoder (default format)
//  0b10 -> AMR vocoder (storage format)
#define    MEMO_REC_MODE_SHIFT      -8
#define    MEMO_PLAY_MODE_SHIFT     -10
#define    FR_MEMO                       0
#define    AMR_MEMO                      2
#define    AMR_MEMO_STORAGE_FORMAT       2

// -----------------------------------------------------------------------------
// DP_AMR_REC_CODEC_MODE                            set/rst/and
// -----------------------------------------------------------------------------
#define    AMR_REC_CODEC_MODE_MASK    0x0007

#ifdef _AUDIO_FIFO_DEBUG
  // -----------------------------------------------------------------------------
  // DP_AUDIO_FIFO_LEVEL                              fields
  // -----------------------------------------------------------------------------
  #define    TX_FIFO_LEVEL_MASK      0x007F
  #define    RX_FIFO_LEVEL_MASK      0x7F00
#endif

// -----------------------------------------------------------------------------
// DP_AUDIO_FIFO_CTRL                               tstb
// -----------------------------------------------------------------------------
#define    LINK_SELECTION_BIT       0
#define    AUDIO_RESET_BIT          1
#define    I2S_TX_MODE_BIT          2
#define    I2S_RX_MODE_BIT          4
#define    SAMPLE_RATE_BIT          5

// -----------------------------------------------------------------------------
// DP_AUDIO_FIFO_CTRL                               set/rst/and
// -----------------------------------------------------------------------------
#define    AUDIO_RESET              1<<AUDIO_RESET_BIT
#define    I2S_TX_MODE              3<<I2S_TX_MODE_BIT
#define    I2S_TX_MODE_SHIFT        -2
#define    I2S_RX_MODE              1<<I2S_RX_MODE_BIT
#define    SAMPLE_RATE              1<<SAMPLE_RATE_BIT

// -----------------------------------------------------------------------------
// DP_AUDIO_FIFO_WORDS                              fields
// -----------------------------------------------------------------------------
#define    NB_WORDS_TX_FIFO         0x007F
#define    NB_WORDS_TX_FIFO_SHIFT   0
#define    NB_WORDS_RX_FIFO         0x7F00
#define  NB_WORDS_RX_FIFO_SHIFT  8

// -----------------------------------------------------------------------------
// DP_AUDIO_RUN_CTRL                                tstb
// -----------------------------------------------------------------------------
#define    DAI_LOOP_FLAG_BIT                   8
#define    UL_AUDIO_GAIN_INIT_FLAG_BIT         9
#define    UL_AUDIO_GAIN_ENABLE_FLAG_BIT       10
#define    DL_AUDIO_GAIN_INIT_FLAG_BIT         11
#define    DL_AUDIO_GAIN_ENABLE_FLAG_BIT       12

// -----------------------------------------------------------------------------
// DP_AUDIO_RUN_CTRL                                set/rst
// -----------------------------------------------------------------------------
#define    UL_AUDIO_GAIN_INIT_FLAG_BIT_MASK     1<<UL_AUDIO_GAIN_INIT_FLAG_BIT
#define    DL_AUDIO_GAIN_INIT_FLAG_BIT_MASK     1<<DL_AUDIO_GAIN_INIT_FLAG_BIT

// -----------------------------------------------------------------------------
// DP_MUTE_CONTROL                                 and
// -----------------------------------------------------------------------------
#define    MUTE_TX               0x0001
#define    MUTE_RX               0x0002
// -----------------------------------------------------------------------------
// MUTE CONTROL                                    tstb
// -----------------------------------------------------------------------------
#define    MUTE_TX_BIT           0
#define    MUTE_RX_BIT           1
#define    MUTE_TX_SPENC_IN_BIT  2
#define    MUTE_RX_SPDEC_OUT_BIT 3

// -----------------------------------------------------------------------------
// DP_DIGITAL_AUDIO_GAIN                           fields
// -----------------------------------------------------------------------------
#define    UL_AUDIO_GAIN_MASK              0x007F
#define    DL_AUDIO_GAIN_MASK              0x7F00

// -----------------------------------------------------------------------------
// DP_DTMF_DEC_CONTROL                             tstb
// -----------------------------------------------------------------------------
#define    DTMF_DEC_ON_BIT             0
#define    DTMF_DEC_PARAM_UPDATE_BIT   1
#define    DTMF_DEC_MODE_BIT           2

// -----------------------------------------------------------------------------
// DP_DTMF_DEC_CONTROL                             and/set/rst
// -----------------------------------------------------------------------------
#define    DTMF_DEC_PARAM_UPDATE_MASK  1<<DTMF_DEC_PARAM_UPDATE_BIT
#define    DTMF_DEC_MODE_MASK          3<<DTMF_DEC_MODE_BIT
#define    DTMF_DEC_MODE_SHIFT         -2

// -----------------------------------------------------------------------------
// DP_DTMF_DEC_PARAMETERS Structure (21 words) - offset values
// -----------------------------------------------------------------------------
#define    DTMF_Post_Proc_Ctrl                     0
#define    DTMF_Mini_Threshold                     DTMF_Post_Proc_Ctrl                +  1
#define    DTMF_Maxi_Threshold                     DTMF_Mini_Threshold                +  1
#define    DTMF_Forward_Twist                      DTMF_Maxi_Threshold                +  1
#define    DTMF_Reverse_Twist                      DTMF_Forward_Twist                 +  1
#define    DTMF_SecondTo_Fund_RatioRows            DTMF_Reverse_Twist                 +  1
#define    DTMF_SecondTo_Fund_RatioCols            DTMF_SecondTo_Fund_RatioRows       +  1
#define    DTMF_Relative_Peak_RatioRows            DTMF_SecondTo_Fund_RatioCols       +  1
#define    DTMF_Relative_Peak_RatioCols            DTMF_Relative_Peak_RatioRows       +  1
#define    DTMF_Dec_R1_Lev                         DTMF_Relative_Peak_RatioCols       +  1
#define    DTMF_Dec_R2_Lev                         DTMF_Dec_R1_Lev                    +  1
#define    DTMF_Param_Debug1                       DTMF_Dec_R2_Lev                    +  1
#define    DTMF_Param_Debug2                       DTMF_Param_Debug1                  +  1
#define    DTMF_Param_Debug3                       DTMF_Param_Debug2                  +  1
#define    DTMF_Param_Debug4                       DTMF_Param_Debug3                  +  1
#define    DTMF_Param_Debug5                       DTMF_Param_Debug4                  +  1
#define    DTMF_Purity                             DTMF_Param_Debug5                  +  1
#define    DTMF_Relative_Peak_RatioRows_AMR        DTMF_Purity                        +  1
#define    DTMF_Relative_Peak_RatioCols_AMR        DTMF_Relative_Peak_RatioRows_AMR   +  1
#define    DTMF_Forward_Twist_AMR                  DTMF_Relative_Peak_RatioCols_AMR   +  1
#define    DTMF_Reverse_Twist_AMR                  DTMF_Forward_Twist_AMR             +  1

// -----------------------------------------------------------------------------
// DP_DTMF_DEC_RESULTS Structure (5 words) - offset values
// -----------------------------------------------------------------------------
#define    DTMF_Decoding_Result_Updated            0
#define    DTMF_Decoding_Status                    DTMF_Decoding_Result_Updated       +  1
#define    DTMF_Decoded_Digit                      DTMF_Decoding_Status               +  1
#define    DTMF_Dec_Digit_Win_End_Pos              DTMF_Decoded_Digit                 +  1
#define    DTMF_Decoding_Block_Size                DTMF_Dec_Digit_Win_End_Pos         +  1

// -----------------------------------------------------------------------------
// DP_DTMF_DEC_CALC_INFO Structure (22 words) - offset values
// -----------------------------------------------------------------------------
#define    DTMF_Mini_Energy_Rows                   0
#define    DTMF_Mini_Energy_Cols                   DTMF_Mini_Energy_Rows              +  1
#define    DTMF_Maxi_Energy_Rows                   DTMF_Mini_Energy_Cols              +  1
#define    DTMF_Maxi_Energy_Cols                   DTMF_Maxi_Energy_Rows              +  1
#define    DTMF_Rel_Peak_Ratio1_Rows               DTMF_Maxi_Energy_Cols              +  1
#define    DTMF_Rel_Peak_Ratio1_Cols               DTMF_Rel_Peak_Ratio1_Rows          +  1
#define    DTMF_Rel_Peak_Ratio2_Rows               DTMF_Rel_Peak_Ratio1_Cols          +  1
#define    DTMF_Rel_Peak_Ratio2_Cols               DTMF_Rel_Peak_Ratio2_Rows          +  1
#define    DTMF_2nd_To_Fund_Ratio_Rows             DTMF_Rel_Peak_Ratio2_Cols          +  1
#define    DTMF_2nd_To_Fund_Ratio_Cols             DTMF_2nd_To_Fund_Ratio_Rows        +  1
#define    DTMF_Max_Rows_Over_Max_Cols_Ratio       DTMF_2nd_To_Fund_Ratio_Cols        +  1
#define    DTMF_Dec_R1                             DTMF_Max_Rows_Over_Max_Cols_Ratio  +  1
#define    DTMF_Dec_R2                             DTMF_Dec_R1                        +  1
#define    DTMF_Dec_R3                             DTMF_Dec_R2                        +  1
#define    DTMF_Dec_R4                             DTMF_Dec_R3                        +  1
#define    DTMF_Dec_C1                             DTMF_Dec_R4                        +  1
#define    DTMF_Dec_C2                             DTMF_Dec_C1                        +  1
#define    DTMF_Dec_C3                             DTMF_Dec_C2                        +  1
#define    DTMF_Dec_C4                             DTMF_Dec_C3                        +  1
#define    DTMF_Dec_AGC_Gain                       DTMF_Dec_C4                        +  1
#define    DTMF_Dec_Energy_M                       DTMF_Dec_AGC_Gain                  +  1
#define    DTMF_Dec_Energy_L                       DTMF_Dec_Energy_M                  +  1

// -----------------------------------------------------------------------------
// DTMF_Post_Proc_Ctrl                          tstb
// -----------------------------------------------------------------------------
#define    DTMF_AGC_EN_BIT                        0
#define    DTMF_SIG_LEV_THRESH_EN_BIT             1
#define    DTMF_TWIST_CHECK_EN_BIT                2
#define    DTMF_REL_PEAK_CHECK_EN_BIT             3
#define    DTMF_2ND_TOFUND_CHECK_EN_BIT           4
#define    DTMF_PWR_FLOOR_CHECK_EN_BIT            5
#define    DTMF_KEY_PAUSE_DETECT_EN_BIT           6 // not implemented
#define    DTMF_PURITY_CHECK_EN_BIT               7
#define    DTMF_AMR_SPECIFIC_DETECT_EN_BIT        8

// -----------------------------------------------------------------------------
// DTMF_Decoding_Status                         set
// -----------------------------------------------------------------------------
#define    DTMF_BELOW_THRESH_ERR                  1<<0
#define    DTMF_ABOVE_THRESH_ERR                  1<<1
#define    DTMF_ABOVE_FWD_TW_ERR                  1<<2
#define    DTMF_ABOVE_REVERSE_TW_ERR              1<<3
#define    DTMF_REL_P_THRESH1_ROW_ERR             1<<4
#define    DTMF_REL_P_THRESH1_COL_ERR             1<<5
#define    DTMF_REL_P_THRESH2_ROW_ERR             1<<6
#define    DTMF_REL_P_THRESH2_COL_ERR             1<<7
#define    DTMF_2ND_TO_FUND_ROW_ERR               1<<8
#define    DTMF_2ND_TO_FUND_COL_ERR               1<<9
#define    DTMF_PWR_LEVEL_ERR                     1<<10
#define    DTMF_KEY_PAUSE1_ERR                    1<<11 // not implemented
#define    DTMF_KEY_PAUSE2_ERR                    1<<12 // not implemented
#define    DTMF_KEY_PAUSE3_ERR                    1<<13 // not implemented
#define    DTMF_NO_INDEX_ERR                      1<<14
#define    DTMF_PURITY_ERR                        1<<15

// -----------------------------------------------------------------------------
// DP_PCM_PLAYBACK_CTRL                            tstb
// -----------------------------------------------------------------------------
#define    PCM_PLAYBACK_TX_ENABLE_FLAG            0
#define    PCM_PLAYBACK_RX_ENABLE_FLAG            1


// -----------------------------------------------------------------------------
// DP_DSP_DEBUG_REGS allocations/definitions
// -----------------------------------------------------------------------------
#define    DP_DBG_REG_TRACE_ON                DP_DEBUG_REG_0
#ifdef _TEMP_DEBUG_TRACE
  #define  DP_DBG_REG_PREDEFINED_DATA_TRACE   DP_DEBUG_REG_1
#endif

// Definitions below only apply if predefined traces are not selected
#define    DP_DBG_REG_ACK_NACK                DP_DEBUG_REG_2
#define    DP_DBG_REG_IT_SOURCE_TO_TRACE      DP_DEBUG_REG_3
#define    DP_DBG_REG_IT_SOURCE_STATUS        DP_DEBUG_REG_4
#define    DP_DBG_REG_NB_DSP_MEM_AREAS        DP_DEBUG_REG_5
#define    DP_DBG_REG_WR_DIRECTION            DP_DEBUG_REG_6
#define    DP_DBG_REG_DPRAM_TRACE_OFFSET      DP_DEBUG_REG_7

#define    DP_DBG_REG_DSP_1_MEM_ADDR          DP_DEBUG_REG_8
#define    DP_DBG_REG_DSP_1_MEM_PAGE          DP_DEBUG_REG_9
#define    DP_DBG_REG_DSP_1_MEM_SIZE          DP_DEBUG_REG_10
// End

#define    DP_DBG_REG_ADJ_RIGHT_SCORE         DP_DEBUG_REG_25
#define    DP_DBG_REG_COCH_RIGHT_SCORE        DP_DEBUG_REG_26
#define    DP_DBG_REG_FAD_RIGHT_SCORE         DP_DEBUG_REG_27
#define    DP_DBG_REG_ADJ_LEFT_SCORE          DP_DEBUG_REG_28
#define    DP_DBG_REG_COCH_LEFT_SCORE         DP_DEBUG_REG_29
#define    DP_DBG_REG_FAD_LEFT_SCORE          DP_DEBUG_REG_30

// Definitions below only apply if predefined traces are selected
#ifdef _TEMP_DEBUG_TRACE
//  #define    DP_DBG_REG_PREDEF_TRACE_EDGE_IQ_NB_SAMPLES  DP_DEBUG_REG_31
  // ---------------------------------------------------------------------------
  // DP_DBG_REG_PREDEF_TRACE_EDGE_IQ
  //   0x04FB --> 0x07DF : 741 words
  // ---------------------------------------------------------------------------
  #define    DP_DBG_REG_PREDEF_TRACE_EDGE_IQ             DPRAM_BASE_ADD + 0x04FB
  #define    TRACE_EDGE_IQ_BUF_SIZE                      741  // to be defined in
                                                           // appropriate file
                                                           // when depatching
#endif
// End

// -----------------------------------------------------------------------------
// DP_DBG_REG_TRACE_ON definition                  tstb
// -----------------------------------------------------------------------------
#define    TRACE_ON_BIT            0

// -----------------------------------------------------------------------------
// DP_DBG_REG_PREDEFINED_DATA_TRACE definitions    tstb
// -----------------------------------------------------------------------------
#ifdef _TEMP_DEBUG_TRACE
  #define    RX_IQ_SAMPLES_BIT             0
  #define    AUDIO_FIFO_RX_SAMPLES_BIT     1
  #define    AUDIO_FIFO_TX_SAMPLES_BIT     2
  #define    EDGE_IQ_SAMPLES_BIT           3
  #define    GSM_IQ_SAMPLES_INJECT_BIT     4
#endif

// -----------------------------------------------------------------------------
// DP_DBG_REG_ACK_NACK                             set
// -----------------------------------------------------------------------------
#define    DBG_OK                          0x0001 // bit 0
#define    DBG_ERR_NO_IT_SELECTED          0x0002 // bit 1
#define    DBG_ERR_NACK_FROM_L1            0x0004 // bit 2
#define    DBG_ERR_WRONG_DSP_ADDR          0x0008 // bit 3
#define    DBG_ERR_WRONG_PAGE              0x0010 // bit 4
#define    DBG_ERR_WRONG_NB_MEM_AREAS      0x0020 // bit 5
#define    DBG_ERR_WRONG_DPRAM_OFFSET      0x0040 // bit 6
//#define    DBG_ERR_DPRAM_OFFSET_EXCEED     0x0080 ; bit 7 FBO: not used
#define    DBG_INFO_TRACE_OFF              0x0100 // bit 8

// -----------------------------------------------------------------------------
// DP_DBG_REG_IT_SOURCE_TO_TRACE                   tstb
// -----------------------------------------------------------------------------
#define    DBG_TRACE_INT0_AUDIO            0 // trace requested for the end of INT0 audio processing
#define    DBG_TRACE_INT0_EDGE             1 // trace requested for the end of INT0 EDGE processing
#define    DBG_TRACE_INT1_RX               2 // trace requested for the end of INT1 RX processing
#define    DBG_TRACE_INT1_TX               3 // trace requested for the end of INT1 TX processing
#define    DBG_TRACE_INT2_AUDIO            4 // trace requested for the end of INT2 audio processing
#define    DBG_TRACE_INT2_EDGE             5 // trace requested for the end of INT2 EDGE processing
#define    DBG_TRACE_VINT_SPEECH_COD       6 // trace requested for the end of VINT speech coding processing
#define    DBG_TRACE_VINT_SPEECH_DECOD     7 // trace requested for the end of VINT speech decoding processing


// ----------------------------------------------------------------------------
///* .MACROS DEFINITION
// ----------------------------------------------------------------------------

// DPRAM latency macro for safe access
// This macro must be inserted:
//  - between one WR access immediatly followed by one RD access to the DPRAM
/* .MACRO _DPRAM_LATENCY
    nop
    nop
.ENDM */ //_DPRAM_LATENCY

// Macro for ROM version number upload to DPRAM
/* .MACRO _DSP_INIT_ROM_VERSION acc
     mov    ##DSP_ROM_VERSION_NUMBER, acc
     mov    acc'l, [##DP_DSP_VERSION_ID]
.ENDM */ //_DSP_INIT_ROM_VERSION

// Macro for Patch version number upload to DPRAM
/* .MACRO _DSP_INIT_PATCH_VERSION
     mov    ##LOWADDR(DSP_PATCH_VERSION_NUMBER), a0
     load   #HIGHADDR(DSP_PATCH_VERSION_NUMBER), movpd
     movp   (a0l), r0
     mov    r0, a0l
     shfi   a0, a1, #DSP_PATCH_VERSION_SHIFT
     mov    [##DP_DSP_VERSION_ID], a0
     or     a0, a1
     mov    a1l, [##DP_DSP_VERSION_ID]
.ENDM */ //_DSP_INIT_PATCH_VERSION


// --------------------------------------------------------------------------------------------
// DPRAM Patch data 
// --------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------
// Vars not defined in v020
// --------------------------------------------------------------------------------------------
#define  AUDIO_LINK_PCM     (1<<LINK_SELECTION_BIT)
#define  RX_EDGE_BFI1_MASK  0x20
#define  RX_EDGE_BFI1_SHIFT 5
#define  RX_EDGE_BFI2_MASK  0x40
#define  RX_EDGE_BFI2_SHIFT 6




#endif /* __ULC_DPRAM_H__ */
