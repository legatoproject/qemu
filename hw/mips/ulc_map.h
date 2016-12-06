/*
**  Important : This software is protected by Copyright and the information
**              contained herein is confidential. Distribution, reproduction
**              as well as exploitation and transmission of its contents is
**              not allowed except if expressly permitted. Infringments
**              result in damage claims.
**              Copyright Wavecom S.A. (c) 2005
*/

/****************************************************************************/
/*  File    : ulc_map.h                Revision:0.0                         */
/*--------------------------------------------------------------------------*/
/*  Scope   : ULC ES1                                                       */
/*                                                                          */
/*  Contents: All mapping definitions used in the simulator                 */
/*                                                                          */
/*  Note:                                                                   */
/*                                                                          */
/*  Changes:                                                                */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/*  Version      | Date           | Author |   Modification                 */
/*--------------------------------------------------------------------------*/
/*   v0     	 |  07.12.2010    |  KBC   |    Creation                    */
/****************************************************************************/
#ifndef __ULC_MAP_H__
#define __ULC_MAP_H__

/********************** Common **********************************************/
#define Z_MEM_BASE             0xA000
#define Z_MEM_SIZE             2*1024

// lbu
#define SHARED_REG_BASE        0xA400
#define SHARED_REG_SIZE        2*1024

#define DPRAM_BASE             0xA800
#define DPRAM_SIZE             2*1024

#define RAM_STACK_BASE         0xB000
#define RAM_STACK_SIZE         4*1024


/********************** Page 1 & 2 ******************************************/
#define PAGE1_2_RAMX_1_BASE    0x0000
#define PAGE1_2_RAMX_1_SIZE    2*10*1024

#define PAGE1_2_ROMX_BASE      0x5000
#define PAGE1_2_ROMX_SIZE      2*6*1024

#define PAGE1_2_RAMX_2_BASE    0x8000
#define PAGE1_2_RAMX_2_SIZE    2*1*1024

#define PAGE1_RAMY_BASE        0xC000
#define PAGE1_RAMY_SIZE        2*6*1024

#define PAGE1_ROMY_BASE        0xF000
#define PAGE1_ROMY_SIZE        2*2*1024

#define PAGE2_RAMY_BASE        0xD000
#define PAGE2_RAMY_SIZE        2*2*1024

#define PAGE2_ROMY_BASE        0xE000
#define PAGE2_ROMY_SIZE        2*3*1024

#define RX_PORT_BASE           0xC000
#define RX_PORT_SIZE           2048

#define TX_PORT_BASE           0xC800
#define TX_PORT_SIZE           384

/********************** Page 3 & 4 ******************************************/
#define PAGE3_ROMX_BASE        0x0000
#define PAGE3_ROMX_SIZE        2*8*1024

#define PAGE4_ROMX_BASE        0x0000
#define PAGE4_ROMX_SIZE        2*7*1024

#define SINE_GEN_BASE          0x3800
#define SINE_GEN_SIZE          128

#define PAGE3_4_RAMX_BASE      0x4000
#define PAGE3_4_RAMX_SIZE      2*9*1024

#define PAGE3_4_ROMX_BASE      0x8800
#define PAGE3_4_ROMX_SIZE      2*3*1024

#define PAGE3_4_RAMY_BASE      0xC000
#define PAGE3_4_RAMY_SIZE      2*8*1024

/********************** Page 5 **********************************************/
#define PAGE5_ROMX_BASE        0x0000
#define PAGE5_ROMX_SIZE        2*10*1024

#define PAGE5_RAMX_BASE        0x6000
#define PAGE5_RAMX_SIZE        2*5*1024

#define PAGE5_ROMY_BASE        0xC000
#define PAGE5_ROMY_SIZE        2*4*1024

#define PAGE5_RAMY_BASE        0xE000
#define PAGE5_RAMY_SIZE        2*3*1024

/********************** GSM & GPRS ******************************************/
#define GSM_GPRS_IT1_DRAM		0x0000
#define GSM_GPRS_IT1_DADD		0x1700
#define GSM_GPRS_IT1_SRAM		0x1800
#define GSM_GPRS_IT1_YRAM		0xC000

/********************** EDGE ************************************************/
#define    EDGE_IT1_DRAM        0x0000
#define    EDGE_IT1_DADD        0x3A00
#define    EDGE_IT1_SRAM        0x31B0
#define    EDGE_IT1_YRAM        0xC000

#define    EDGE_IT2_DRAM        0x0A80
#define    EDGE_IT2_SRAM_PART1  0x1C08
#define    EDGE_IT2_SRAM_PART2  0xCF00
#define    EDGE_IT2_SRAM_PART3  0x3B50
//#define    EDGE_IT2_DADD        0x3A90
#define    EDGE_IT2_YRAM        0xC72C
#define    EDGE_IT1_YSRAM       0xE700

// Direct addressing sections
#define    EDGE_IT1_SUB_DA_PG   0x3A1C
#define    BLIND_DET_DA_PG      0x3A1C
#define    ADJ_DET_DA_PG        0x3A21
#define    DFE_DA_PG            0x3A37
#define    PFE_DA_PG            0x3A46
#define    PREFLT_DA_PG         0x3A5A
#define    BURST_MEAS_DA_PG     0x3A60
#define    EQUALIZER_DA_PG      0x3A6D
#define    BLOCK_ENC_DA_PG      0x3A8F
#define    BLOCK_DEC_DA_PG      0x3AB0
#define    CS1DEC_P2_DA_PG      0x3ACF


/********************** Audio ***********************************************/
//#define PCM_BUFFERS	          0x4200
//#define PCM_Buf_Admin         0x4800
//
//
//#define TX_Buffer1			  0x4200
//#define TX_Buffer2			  0x4340
//#define RX_Buffer1			  0x4480
//#define RX_Buffer2			  0x45C0
//#define AudioInputBuffer	  0x4700
//#define AudioOutputBuffer	  0x4780
//
//
//#define BufferX_TX			  0x4800
//#define BufferY_TX			  0x4801
//
//#define BufferX_RX			  0x4802
//#define BufferY_RX			  0x4803
//
//#define PCMIN_Pt			  0x4804
//#define PCMOUT_Pt			  0x4805
//#define PCMIN_Current_Pt	  0x4806
//#define PCMOUT_Current_Pt	  0x4807
//
//#define SPEECH_IN			  0x4808
//#define SPEECH_OUT			  0x4809
//
//#define AudioInputSpeechPtr			0x480A
//#define CurrentAudioInputSpeechPtr	0x480B
//#define AudioInputAECPtr			0x480C
//#define AudioOutputSpeechPtr		0x480D
//#define CurrentAudioOutputSpeechPtr 0x480E
//#define AudioOutputAECPtr			0x480F
//#define It0_AudioCounter			0Ax4810
//#define save_st2_for_aec			0x4811

#endif  //__ULC_MAP_H__
