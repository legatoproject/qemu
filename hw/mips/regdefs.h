/*****************************************************************************\
*                                                                             *
* Copyright (C) CEVA Inc. All rights reserved.                                *
*                                                                             *
* Project: Debugger                                                           *
*                                                                             *
* File Name: REGDEFS.H                                                        *
*                                                                             *
* Written by: Menachem Stern                                                  *
* Date:       2/4/2001                                                        *
*                                                                             *
* Purpose: Defines common registers related definitions.                      *
*                                                                             *
\*****************************************************************************/

#ifndef REGISTERS_DEFINITIONS_H_
#define REGISTERS_DEFINITIONS_H_

#include "globdefs.h"

// Maor [4/2/2004]:
// enum for the bkrep registers:
//==============================
typedef enum T_BkrepRegId_tag {
    BkrepRegId_E_LC,
    BkrepRegId_E_SA,
    BkrepRegId_E_EA,
    BkrepRegId_E_Size
} T_BkrepRegId;

//----------------------------------------------
#if (DSP_TYPE == OAK)
//----------------------------------------------

// Registers types:
//=================
typedef WORD T_Reg;
typedef WORD T_ExtReg;


// Number of external registers:
//==============================
#define kNumberOfExtRegs 4


// Interrupts vector addresses:
//-----------------------------
#define Core_K_InterruptVectorAddress_Reset   0x0000
#define Core_K_InterruptVectorAddress_TrapBpi 0x0002
#define Core_K_InterruptVectorAddress_Nmi     0x0004
#define Core_K_InterruptVectorAddress_Int0    0x0006
#define Core_K_InterruptVectorAddress_Int1    0x000E
#define Core_K_InterruptVectorAddress_Int2    0x0016


// Short registers enumeration:
//=============================
typedef enum ShortRegId_T_ID_tag {
#ifdef MWBIG_ENDIAN
    // Half Regs must be first in order to overlay properly
    /* 0  */ ShortRegId_E_A0H,
    /* 1  */ ShortRegId_E_A0L,
    /* 2  */ ShortRegId_E_A1H,
    /* 3  */ ShortRegId_E_A1L,
    /* 4  */ ShortRegId_E_B0H,
    /* 5  */ ShortRegId_E_B0L,
    /* 6  */ ShortRegId_E_B1H,
    /* 7  */ ShortRegId_E_B1L,
    /* 8  */ ShortRegId_E_PH,
    /* 9  */ ShortRegId_E_PL,
#else
    // Half Regs must be first in order to overlay properly
    /* 0  */ ShortRegId_E_A0L,
    /* 1  */ ShortRegId_E_A0H,
    /* 2  */ ShortRegId_E_A1L,
    /* 3  */ ShortRegId_E_A1H,
    /* 4  */ ShortRegId_E_B0L,
    /* 5  */ ShortRegId_E_B0H,
    /* 6  */ ShortRegId_E_B1L,
    /* 7  */ ShortRegId_E_B1H,
    /* 8  */ ShortRegId_E_PL,
    /* 9  */ ShortRegId_E_PH,
#endif
    /* 10 */ ShortRegId_E_R0,
    /* 11 */ ShortRegId_E_R1,
    /* 12 */ ShortRegId_E_R2,
    /* 13 */ ShortRegId_E_R3,
    /* 14 */ ShortRegId_E_R4,
    /* 15 */ ShortRegId_E_R5,
    /* 16 */ ShortRegId_E_RB,
    /* 17 */ ShortRegId_E_Y,
    /* 18 */ ShortRegId_E_X,
    /* 19 */ ShortRegId_E_LC,
    /* 20 */ ShortRegId_E_PC,
    /* 21 */ ShortRegId_E_ST0,
    /* 22 */ ShortRegId_E_ST1,
    /* 23 */ ShortRegId_E_ST2,
    /* 24 */ ShortRegId_E_SP,
    /* 25 */ ShortRegId_E_CFGI,
    /* 26 */ ShortRegId_E_CFGJ,
    /* 27 */ ShortRegId_E_EXT0,
    /* 28 */ ShortRegId_E_EXT1,
    /* 29 */ ShortRegId_E_EXT2,
    /* 30 */ ShortRegId_E_EXT3,
    /* 31 */ ShortRegId_E_SV,
    /* 32 */ ShortRegId_E_DVM,
    /* 33 */ ShortRegId_E_MIXP,
    /* 34 */ ShortRegId_E_ICR,
    /* 35 */ ShortRegId_E_B0E,
    /* 36 */ ShortRegId_E_B1E,
    /* 37 */ ShortRegId_E_R0B,
    /* 38 */ ShortRegId_E_R1B,
    /* 39 */ ShortRegId_E_R4B,
    /* 40 */ ShortRegId_E_CFGIB,
    /* 41 */ ShortRegId_E_REPC,
    /* 42 */ ShortRegId_E_ST0S,
    /* 43 */ ShortRegId_E_ST1S,
    /* 44 */ ShortRegId_E_ST2S,
    /* 45 */ ShortRegId_E_PgSw,

#ifdef DSP_TKL2
// Guy Avnet: [1/17/2005] tkl2 task
    /* 46 */ ShortRegId_E_PRPAGE,
    /* 47 */ ShortRegId_E_XPAGE,
    /* 48 */ ShortRegId_E_YPAGE,
    /* 49 */ ShortRegId_E_PDLPAGE,
#endif

    /* 46 */ ShortRegId_E_Size,

    // this enumeration is used only for breakpoints:
    ShortRegId_E_A0E = ShortRegId_E_Size,
    ShortRegId_E_A1E
} T_ShortRegID;

typedef T_ShortRegID T_RegPartId;
typedef T_ShortRegID T_RegId;
#define M_RegId(reg) ShortRegId_E_##reg

// Long registers enumeration:
//============================
typedef enum LongRegId_T_ID_tag {
    /* 0  */ LongRegId_E_A0,
    /* 1  */ LongRegId_E_A1,
    /* 2  */ LongRegId_E_B0,
    /* 3  */ LongRegId_E_B1,
    /* 4  */ LongRegId_E_P,

    /* 5  */ LongRegId_E_Size
} T_LongRegID;


// Flags/Fields/Control bits enumeration:
//=======================================
typedef enum T_FlagID_tag {
    /* 0  */ FlagId_E_A0E,
    /* 1  */ FlagId_E_A1E,
    /* 2  */ FlagId_E_Z,
    /* 3  */ FlagId_E_M,
    /* 4  */ FlagId_E_N,
    /* 5  */ FlagId_E_V,
    /* 6  */ FlagId_E_C,
    /* 7  */ FlagId_E_E,
    /* 8  */ FlagId_E_L,
    /* 9  */ FlagId_E_R,
    /* 10 */ FlagId_E_IM0,
    /* 11 */ FlagId_E_IM1,
    /* 12 */ FlagId_E_IM2,
    /* 13 */ FlagId_E_IE,
    /* 14 */ FlagId_E_SAT,
    /* 15 */ FlagId_E_PS,
    /* 16 */ FlagId_E_PAGE,
    /* 17 */ FlagId_E_IP0,
    /* 18 */ FlagId_E_IP1,
    /* 19 */ FlagId_E_IP2,
    /* 20 */ FlagId_E_IU0,
    /* 21 */ FlagId_E_IU1,
    /* 22 */ FlagId_E_OU0,
    /* 23 */ FlagId_E_OU1,
    /* 24 */ FlagId_E_S,
    /* 25 */ FlagId_E_M0,
    /* 26 */ FlagId_E_M1,
    /* 27 */ FlagId_E_M2,
    /* 28 */ FlagId_E_M3,
    /* 29 */ FlagId_E_M4,
    /* 30 */ FlagId_E_M5,
    /* 31 */ FlagId_E_IRP,
    /* 32 */ FlagId_E_BC,
    /* 33 */ FlagId_E_LP,
    /* 34 */ FlagId_E_IC0,
    /* 35 */ FlagId_E_IC1,
    /* 36 */ FlagId_E_IC2,
    /* 37 */ FlagId_E_NMIC,
    /* 38 */ FlagId_E_MODI,
    /* 39 */ FlagId_E_MODJ,
    /* 40 */ FlagId_E_STEPI,
    /* 41 */ FlagId_E_STEPJ,

    /* 42 */ FlagId_E_Size
} T_FlagID;

//----------------------------------------------
#elif (DSP_TYPE == TEAK)
//----------------------------------------------

// Registers types:
//=================
typedef DWORD T_Reg;
typedef WORD  T_ExtReg;


// Number of external registers:
//==============================
#define kNumberOfExtRegs 4


// Interrupts vector addresses:
//-----------------------------
#define Core_K_InterruptVectorAddress_Reset     0x0000
#define Core_K_InterruptVectorAddress_TrapBpi   0x0002
#define Core_K_InterruptVectorAddress_Nmi       0x0004
#define Core_K_InterruptVectorAddress_Int0      0x0006
#define Core_K_InterruptVectorAddress_Int1      0x000E
#define Core_K_InterruptVectorAddress_Int2      0x0016


// Short registers enumeration:
//=============================
typedef enum ShortRegId_T_ID_tag {
    /* 0  */ ShortRegId_E_A0H,
    /* 1  */ ShortRegId_E_A1H,
    /* 2  */ ShortRegId_E_B0H,
    /* 3  */ ShortRegId_E_B1H,
    /* 4  */ ShortRegId_E_A0L,
    /* 5  */ ShortRegId_E_A1L,
    /* 6  */ ShortRegId_E_B0L,
    /* 7  */ ShortRegId_E_B1L,
    /* 8  */ ShortRegId_E_A0E,
    /* 9  */ ShortRegId_E_A1E,
    /* 10 */ ShortRegId_E_B0E,
    /* 11 */ ShortRegId_E_B1E,
    /* 12 */ ShortRegId_E_AR0,
    /* 13 */ ShortRegId_E_AR1,
    /* 14 */ ShortRegId_E_ARP0,
    /* 15 */ ShortRegId_E_ARP1,
    /* 16 */ ShortRegId_E_ARP2,
    /* 17 */ ShortRegId_E_ARP3,
    /* 18 */ ShortRegId_E_CFGI,
    /* 19 */ ShortRegId_E_CFGJ,
    /* 20 */ ShortRegId_E_DVM,
    /* 21 */ ShortRegId_E_EXT0,
    /* 22 */ ShortRegId_E_EXT1,
    /* 23 */ ShortRegId_E_EXT2,
    /* 24 */ ShortRegId_E_EXT3,
    /* 25 */ ShortRegId_E_ICR,
    /* 26 */ ShortRegId_E_LC,
    /* 27 */ ShortRegId_E_MIXP,
    /* 28 */ ShortRegId_E_MOD0,
    /* 29 */ ShortRegId_E_MOD1,
    /* 30 */ ShortRegId_E_MOD2,
    /* 31 */ ShortRegId_E_MOD3,
    /* 32 */ ShortRegId_E_P0L,
    /* 33 */ ShortRegId_E_P1L,
    /* 34 */ ShortRegId_E_P0H,
    /* 35 */ ShortRegId_E_P1H,
    /* 36 */ ShortRegId_E_PC,
    /* 37 */ ShortRegId_E_PRPAGE,
    /* 38 */ ShortRegId_E_R0,
    /* 39 */ ShortRegId_E_R1,
    /* 40 */ ShortRegId_E_R2,
    /* 41 */ ShortRegId_E_R3,
    /* 42 */ ShortRegId_E_R4,
    /* 43 */ ShortRegId_E_R5,
    /* 44 */ ShortRegId_E_R6,
    /* 45 */ ShortRegId_E_R7,
    /* 46 */ ShortRegId_E_REPC,
    /* 47 */ ShortRegId_E_SP,
    /* 48 */ ShortRegId_E_ST0,
    /* 49 */ ShortRegId_E_ST1,
    /* 50 */ ShortRegId_E_ST2,
    /* 51 */ ShortRegId_E_STT0,
    /* 52 */ ShortRegId_E_STT1,
    /* 53 */ ShortRegId_E_STT2,
    /* 54 */ ShortRegId_E_STT3,
    /* 55 */ ShortRegId_E_SV,
    /* 56 */ ShortRegId_E_VTR0,
    /* 57 */ ShortRegId_E_VTR1,
    /* 58 */ ShortRegId_E_X0,
    /* 59 */ ShortRegId_E_X1,
    /* 60 */ ShortRegId_E_Y0,
    /* 61 */ ShortRegId_E_Y1,
    /* 62 */ ShortRegId_E_A1LShadow,
    /* 63 */ ShortRegId_E_A1HShadow,
    /* 64 */ ShortRegId_E_A1EShadow,
    /* 65 */ ShortRegId_E_B1LShadow,
    /* 66 */ ShortRegId_E_B1HShadow,
    /* 67 */ ShortRegId_E_B1EShadow,
    /* 68 */ ShortRegId_E_CFGIBank,
    /* 69 */ ShortRegId_E_CFGJBank,
    /* 70 */ ShortRegId_E_STT0Shadow,
    /* 71 */ ShortRegId_E_STT1Shadow,
    /* 72 */ ShortRegId_E_STT2Swap,
    /* 73 */ ShortRegId_E_MOD0Swap,
    /* 74 */ ShortRegId_E_MOD1Swap,
    /* 75 */ ShortRegId_E_MOD2Swap,
    /* 76 */ ShortRegId_E_MOD3Shadow,
    /* 77 */ ShortRegId_E_R0Bank,
    /* 78 */ ShortRegId_E_R1Bank,
    /* 79 */ ShortRegId_E_R4Bank,
    /* 80 */ ShortRegId_E_R7Bank,
    /* 81 */ ShortRegId_E_REPCShadow,
    /* 82 */ ShortRegId_E_P0HS,
    /* 83 */ ShortRegId_E_AR0Swap,
    /* 84 */ ShortRegId_E_AR1Swap,
    /* 85 */ ShortRegId_E_ARP0Swap,
    /* 86 */ ShortRegId_E_ARP1Swap,
    /* 87 */ ShortRegId_E_ARP2Swap,
    /* 88 */ ShortRegId_E_ARP3Swap,
    /* 89 */ ShortRegId_E_STEPI0,        // 19/6/2000 - Teak ACP
    /* 90 */ ShortRegId_E_STEPJ0,
    /* 91 */ ShortRegId_E_STEPI0Bank,
    /* 92 */ ShortRegId_E_STEPJ0Bank,

    /* 93 */ ShortRegId_E_Size,

    // this enumeration is used only for breakpoints:
    ShortRegId_E_P0E = ShortRegId_E_Size,
    ShortRegId_E_P1E
} T_ShortRegID;

typedef T_ShortRegID T_RegPartId;
typedef T_ShortRegID T_RegId;
#define M_RegId(reg) ShortRegId_E_##reg

// Long registers enumeration:
//============================
typedef enum LongRegId_T_ID_tag {
    /* 0  */ LongRegId_E_A0,
    /* 1  */ LongRegId_E_A1,
    /* 2  */ LongRegId_E_B0,
    /* 3  */ LongRegId_E_B1,
    /* 4  */ LongRegId_E_P0,
    /* 5  */ LongRegId_E_P1,
    /* 6  */ LongRegId_E_A1S,
    /* 7  */ LongRegId_E_B1S,

    /* 8  */ LongRegId_E_Size
} T_LongRegID;


// Flags/Fields/Control bits enumeration:
//=======================================
typedef enum T_FlagID_tag {
    /* 0  */ FlagId_E_Z,
    /* 1  */ FlagId_E_M,
    /* 2  */ FlagId_E_N,
    /* 3  */ FlagId_E_V,
    /* 4  */ FlagId_E_C,
    /* 5  */ FlagId_E_C1,
    /* 6  */ FlagId_E_E,
    /* 7  */ FlagId_E_VL,
    /* 8  */ FlagId_E_LM,
    /* 9  */ FlagId_E_R,
    /* 10 */ FlagId_E_P0E,      // 33 bit of p0
    /* 11 */ FlagId_E_P1E,      // 33 bit of p0
    /* 12 */ FlagId_E_IU0,      // Input user - Read Only
    /* 13 */ FlagId_E_IU1,      // Input user - Read Only
    /* 14 */ FlagId_E_OU0,      // Output user pin 0
    /* 15 */ FlagId_E_OU1,      // Output user pin 1
    /* 16 */ FlagId_E_LP,       // in loop bit
    /* 17 */ FlagId_E_BCx,      // block repeat counter
    /* 18 */ FlagId_E_PCMhi,    // PC register MSB's for movp and movd
    /* 19 */ FlagId_E_PChi,     // PC register 4 MSB's
    /* 20 */ FlagId_E_IE,       // Interrupt Eanble
    /* 21 */ FlagId_E_IP0,      // Interrupt Pending
    /* 22 */ FlagId_E_IP1,
    /* 23 */ FlagId_E_IP2,
    /* 24 */ FlagId_E_IPV,
    /* 25 */ FlagId_E_IM0,      // interrupt masking
    /* 26 */ FlagId_E_IM1,
    /* 27 */ FlagId_E_IM2,
    /* 28 */ FlagId_E_IMV,
    /* 29 */ FlagId_E_NMIC,     // non maskable interrupt
    /* 30 */ FlagId_E_IC0,      // interrupt context switching flags
    /* 31 */ FlagId_E_IC1,
    /* 32 */ FlagId_E_IC2,
    /* 33 */ FlagId_E_BRV0,
    /* 34 */ FlagId_E_BRV1,
    /* 35 */ FlagId_E_BRV2,
    /* 36 */ FlagId_E_BRV3,
    /* 37 */ FlagId_E_BRV4,
    /* 38 */ FlagId_E_BRV5,
    /* 39 */ FlagId_E_BRV6,
    /* 40 */ FlagId_E_BRV7,
    /* 41 */ FlagId_E_MD0,
    /* 42 */ FlagId_E_MD1,
    /* 43 */ FlagId_E_MD2,
    /* 44 */ FlagId_E_MD3,
    /* 45 */ FlagId_E_MD4,
    /* 46 */ FlagId_E_MD5,
    /* 47 */ FlagId_E_MD6,
    /* 48 */ FlagId_E_MD7,
    /* 49 */ FlagId_E_InRep,    // in Rep
    /* 50 */ FlagId_E_PS0,      // post shift for pX
    /* 51 */ FlagId_E_PS1,
    /* 52 */ FlagId_E_S,        // shift mode - clear: arithmetic, set: logical
    /* 53 */ FlagId_E_HWM,      // byte word multiplication - for multiply options
    /* 54 */ FlagId_E_SAT,      // Saturation on move mode - enable sat when clear
    /* 55 */ FlagId_E_SATA,     // Saturation on accumulator mode  - enable when clear
    /* 56 */ FlagId_E_STP16,    // Size of step resgister (7/16 Bit)
    /* 57 */ FlagId_E_CMD,      // compatibility bit for Modulo
    /* 58 */ FlagId_E_EPI,      // external pointer I
    /* 59 */ FlagId_E_EPJ,      // external pointer J
    /* 60 */ FlagId_E_PAGE,     // page for directPage addressing mode
    /* 61 */ FlagId_E_CREP,     // compatible repeat bit
    /* 62 */ FlagId_E_CPC,      // compatible PC
    /* 63 */ FlagId_E_CCNTA,    // compatible contex switch to aX.
    /* 64 */ FlagId_E_RN0,      // Adressing register bits
    /* 65 */ FlagId_E_RN1,
    /* 66 */ FlagId_E_RN2,
    /* 67 */ FlagId_E_RN3,
    /* 68 */ FlagId_E_RI0,
    /* 69 */ FlagId_E_RI1,
    /* 70 */ FlagId_E_RI2,
    /* 71 */ FlagId_E_RI3,
    /* 72 */ FlagId_E_RJ0,
    /* 73 */ FlagId_E_RJ1,
    /* 74 */ FlagId_E_RJ2,
    /* 75 */ FlagId_E_RJ3,
    /* 76 */ FlagId_E_PM0,      // Adressing register post-modification bits
    /* 77 */ FlagId_E_PM1,
    /* 78 */ FlagId_E_PM2,
    /* 79 */ FlagId_E_PM3,
    /* 80 */ FlagId_E_PMI0,     // ARP register post-modification bits
    /* 81 */ FlagId_E_PMI1,
    /* 82 */ FlagId_E_PMI2,
    /* 83 */ FlagId_E_PMI3,
    /* 84 */ FlagId_E_PMJ0,
    /* 85 */ FlagId_E_PMJ1,
    /* 86 */ FlagId_E_PMJ2,
    /* 87 */ FlagId_E_PMJ3,
    /* 88 */ FlagId_E_CONSEC0,  // Adressing register consecutive bits
    /* 89 */ FlagId_E_CONSEC1,
    /* 90 */ FlagId_E_CONSEC2,
    /* 91 */ FlagId_E_CONSEC3,
    /* 92 */ FlagId_E_CONSECI0, // ARP register consecutive bits
    /* 93 */ FlagId_E_CONSECI1,
    /* 94 */ FlagId_E_CONSECI2,
    /* 95 */ FlagId_E_CONSECI3,
    /* 96 */ FlagId_E_CONSECJ0,
    /* 97 */ FlagId_E_CONSECJ1,
    /* 98 */ FlagId_E_CONSECJ2,
    /* 99 */ FlagId_E_CONSECJ3,
    /* 100*/ FlagId_E_MODI,
    /* 101*/ FlagId_E_MODJ,
    /* 102*/ FlagId_E_STEPI,
    /* 103*/ FlagId_E_STEPJ,
    /* 104*/ FlagId_E_OU2,      // Output user pin 2
    /* 105*/ FlagId_E_OU3,      // Output user pin 3
    /* 106*/ FlagId_E_OU4,      // Output user pin 4
    /* 107*/ FlagId_E_Size
} T_FlagID;


//----------------------------------------------
#elif (DSP_TYPE == PALM)
//----------------------------------------------

// Registers types:
//=================
typedef DWORD T_Reg;
typedef DWORD T_ExtReg;


// Number of external registers:
//==============================
#define kNumberOfExtRegs 16


// Interrupts vector addresses:
//-----------------------------
#define Core_K_InterruptVectorAddress_Reset     0x0000
#define Core_K_InterruptVectorAddress_TrapeBpi  0x0002
#define Core_K_InterruptVectorAddress_Trap      0x0004
#define Core_K_InterruptVectorAddress_Nmi       0x0006
#define Core_K_InterruptVectorAddress_Int0      0x0016
#define Core_K_InterruptVectorAddress_Int1      0x0026
#define Core_K_InterruptVectorAddress_Int2      0x0036


// Short registers enumeration:
//=============================
typedef enum T_ShortRegID_tag {
    /* 0  */ ShortRegId_E_A0H,
    /* 1  */ ShortRegId_E_A1H,
    /* 2  */ ShortRegId_E_A2H,
    /* 3  */ ShortRegId_E_A3H,
    /* 4  */ ShortRegId_E_A0L,
    /* 5  */ ShortRegId_E_A1L,
    /* 6  */ ShortRegId_E_A2L,
    /* 7  */ ShortRegId_E_A3L,
    /* 8  */ ShortRegId_E_A0E,
    /* 9  */ ShortRegId_E_A1E,
    /* 10 */ ShortRegId_E_A2E,
    /* 11 */ ShortRegId_E_A3E,
    /* 12 */ ShortRegId_E_CICR,
    /* 13 */ ShortRegId_E_CST0,
    /* 14 */ ShortRegId_E_CST1,
    /* 15 */ ShortRegId_E_CST2,
    /* 16 */ ShortRegId_E_DVM,
    /* 17 */ ShortRegId_E_EXT0,
    /* 18 */ ShortRegId_E_EXT1,
    /* 19 */ ShortRegId_E_EXT2,
    /* 20 */ ShortRegId_E_EXT3,
    /* 21 */ ShortRegId_E_EXT4,
    /* 22 */ ShortRegId_E_EXT5,
    /* 23 */ ShortRegId_E_EXT6,
    /* 24 */ ShortRegId_E_EXT7,
    /* 25 */ ShortRegId_E_EXT8,
    /* 26 */ ShortRegId_E_EXT9,
    /* 27 */ ShortRegId_E_EXT10,
    /* 28 */ ShortRegId_E_EXT11,
    /* 29 */ ShortRegId_E_EXT12,
    /* 30 */ ShortRegId_E_EXT13,
    /* 31 */ ShortRegId_E_EXT14,
    /* 32 */ ShortRegId_E_EXT15,
    /* 33 */ ShortRegId_E_LC,
    /* 34 */ ShortRegId_E_MIXP,
    /* 35 */ ShortRegId_E_MIXP2,
    /* 36 */ ShortRegId_E_MOD0,
    /* 37 */ ShortRegId_E_MOD1,
    /* 38 */ ShortRegId_E_MOD2,
    /* 39 */ ShortRegId_E_MOD3,
    /* 40 */ ShortRegId_E_MODUI,
    /* 41 */ ShortRegId_E_MODUJ,
    /* 42 */ ShortRegId_E_CMODUI,
    /* 43 */ ShortRegId_E_CMODUJ,
    /* 44 */ ShortRegId_E_P0L,
    /* 45 */ ShortRegId_E_P1L,
    /* 46 */ ShortRegId_E_P0H,
    /* 47 */ ShortRegId_E_P1H,
    /* 48 */ ShortRegId_E_P0E,
    /* 49 */ ShortRegId_E_P1E,
    /* 50 */ ShortRegId_E_PC,
    /* 51 */ ShortRegId_E_PRPAGE,
    /* 52 */ ShortRegId_E_R0,
    /* 53 */ ShortRegId_E_R1,
    /* 54 */ ShortRegId_E_R2,
    /* 55 */ ShortRegId_E_R3,
    /* 56 */ ShortRegId_E_R4,
    /* 57 */ ShortRegId_E_R5,
    /* 58 */ ShortRegId_E_R6,
    /* 59 */ ShortRegId_E_R7,
    /* 60 */ ShortRegId_E_REPC,
    /* 61 */ ShortRegId_E_REPI,
    /* 62 */ ShortRegId_E_SP,
    /* 63 */ ShortRegId_E_ST0,
    /* 64 */ ShortRegId_E_ST1,
    /* 65 */ ShortRegId_E_ST2,
    /* 66 */ ShortRegId_E_ST3,
    /* 67 */ ShortRegId_E_STEPI0,
    /* 68 */ ShortRegId_E_STEPI1,
    /* 69 */ ShortRegId_E_STEPI2,
    /* 70 */ ShortRegId_E_STEPJ0,
    /* 71 */ ShortRegId_E_STEPJ1,
    /* 72 */ ShortRegId_E_STEPJ2,
    /* 73 */ ShortRegId_E_SV,
    /* 74 */ ShortRegId_E_VTR0,
    /* 75 */ ShortRegId_E_VTR1,
    /* 76 */ ShortRegId_E_WI0,
    /* 77 */ ShortRegId_E_WI1,
    /* 78 */ ShortRegId_E_WR0,
    /* 79 */ ShortRegId_E_WR1,
    /* 80 */ ShortRegId_E_X0,
    /* 81 */ ShortRegId_E_X1,
    /* 82 */ ShortRegId_E_Y0,
    /* 83 */ ShortRegId_E_Y1,
    /* 84 */ ShortRegId_E_A1LShadow,
    /* 85 */ ShortRegId_E_A1HShadow,
    /* 86 */ ShortRegId_E_A1EShadow,
    /* 87 */ ShortRegId_E_A3LShadow,
    /* 88 */ ShortRegId_E_A3HShadow,
    /* 89 */ ShortRegId_E_A3EShadow,
    /* 90 */ ShortRegId_E_ST0Shadow,
    /* 91 */ ShortRegId_E_ST1Shadow,
    /* 92 */ ShortRegId_E_ST2Swap,
    /* 93 */ ShortRegId_E_MOD0Swap,
    /* 94 */ ShortRegId_E_MOD1Swap,
    /* 95 */ ShortRegId_E_MOD2Swap,
    /* 96 */ ShortRegId_E_MOD3Shadow,
    /* 97 */ ShortRegId_E_MODUIBank,
    /* 98 */ ShortRegId_E_MODUJBank,
    /* 99 */ ShortRegId_E_STEPI0Bank,
    /* 100*/ ShortRegId_E_STEPJ0Bank,
    /* 101*/ ShortRegId_E_R0Bank,
    /* 102*/ ShortRegId_E_R1Bank,
    /* 103*/ ShortRegId_E_R4Bank,
    /* 104*/ ShortRegId_E_R7Bank,
    /* 105*/ ShortRegId_E_REPIShadow,
    /* 106*/ ShortRegId_E_REPCShadow,
    /* 107*/ ShortRegId_E_A0LP,
    /* 108*/ ShortRegId_E_A0HP,
    /* 109*/ ShortRegId_E_A2HP,
    /* 110*/ ShortRegId_E_P0HS,
    /* 111*/ ShortRegId_E_FAP0,
    /* 112*/ ShortRegId_E_FAP1,
    /* 113*/ ShortRegId_E_FAP2,
    /* 114*/ ShortRegId_E_FBP0,
    /* 115*/ ShortRegId_E_FBP1,
    /* 116*/ ShortRegId_E_FBP2,
    /* 117*/ ShortRegId_E_ZI0,
    /* 118*/ ShortRegId_E_ZI1,
    /* 119*/ ShortRegId_E_ZR0,
    /* 120*/ ShortRegId_E_ZR1,

    /* 121*/ ShortRegId_E_Size
} T_ShortRegID;

typedef T_ShortRegID T_RegPartId;
typedef T_ShortRegID T_RegId;
#define M_RegId(reg) ShortRegId_E_##reg

// Long registers enumeration:
//============================
typedef enum LongRegId_T_ID_tag {
    /* 0  */ LongRegId_E_A0,
    /* 1  */ LongRegId_E_A1,
    /* 2  */ LongRegId_E_A2,
    /* 3  */ LongRegId_E_A3,
    /* 4  */ LongRegId_E_P0,
    /* 5  */ LongRegId_E_P1,
    /* 6  */ LongRegId_E_A1S,
    /* 7  */ LongRegId_E_A3S,

    /* 8  */ LongRegId_E_Size
} T_LongRegID;


// Flags/Fields/Control bits enumeration:
//=======================================
typedef enum T_FlagID_tag {
    /* 0  */ FlagId_E_Z0,
    /* 1  */ FlagId_E_Z1,
    /* 2  */ FlagId_E_M0,
    /* 3  */ FlagId_E_M1,
    /* 4  */ FlagId_E_N0,
    /* 5  */ FlagId_E_N1,
    /* 6  */ FlagId_E_V0,
    /* 7  */ FlagId_E_V1,
    /* 8  */ FlagId_E_C0,
    /* 9  */ FlagId_E_C1,
    /* 10 */ FlagId_E_E0,
    /* 11 */ FlagId_E_E1,
    /* 12 */ FlagId_E_VL0,
    /* 13 */ FlagId_E_VL1,
    /* 14 */ FlagId_E_LM0,
    /* 15 */ FlagId_E_LM1,
    /* 16 */ FlagId_E_G,
    /* 17 */ FlagId_E_DZ,
    /* 18 */ FlagId_E_DM,
    /* 19 */ FlagId_E_DV,
    /* 20 */ FlagId_E_DC,
    /* 21 */ FlagId_E_BRV0,
    /* 22 */ FlagId_E_BRV1,
    /* 23 */ FlagId_E_BRV2,
    /* 24 */ FlagId_E_BRV3,
    /* 25 */ FlagId_E_BRV4,
    /* 26 */ FlagId_E_BRV5,
    /* 27 */ FlagId_E_BRV6,
    /* 28 */ FlagId_E_BRV7,
    /* 29 */ FlagId_E_MD0,
    /* 30 */ FlagId_E_MD1,
    /* 31 */ FlagId_E_MD2,
    /* 32 */ FlagId_E_MD3,
    /* 33 */ FlagId_E_MD4,
    /* 34 */ FlagId_E_MD5,
    /* 35 */ FlagId_E_MD6,
    /* 36 */ FlagId_E_MD7,
    /* 37 */ FlagId_E_SAT,   // Saturation on move mode - enable sat when clear
    /* 38 */ FlagId_E_SATA,  // Saturation on accumulator mode  - enable when clear
    /* 39 */ FlagId_E_SATP,  // register p Saturation mode - enable sat when clear
    /* 40 */ FlagId_E_PA0,
    /* 41 */ FlagId_E_PA1,
    /* 42 */ FlagId_E_CMD,   // Compatibility modulu bit
    /* 43 */ FlagId_E_EPJ,   // external pointer J
    /* 44 */ FlagId_E_EPI,   // external pointer I
    /* 45 */ FlagId_E_PS0,   // post shift for pX
    /* 46 */ FlagId_E_PS1,
    /* 47 */ FlagId_E_BWM,   // byte word multiplication - for multiply options
    /* 48 */ FlagId_E_S,     // shift mode - clear: arithmetic, set: logical
    /* 49 */ FlagId_E_IU0,   // Input user - Read Only
    /* 50 */ FlagId_E_IU1,   // Input user - Read Only
    /* 51 */ FlagId_E_OU0,   // Output user pin 0
    /* 52 */ FlagId_E_OU1,   // Output user pin 1
    /* 53 */ FlagId_E_IE,    // Interrupt Eanble
    /* 54 */ FlagId_E_IP0,   // Interrupt Pending
    /* 55 */ FlagId_E_IP1,
    /* 56 */ FlagId_E_IP2,
    /* 57 */ FlagId_E_IPV,
    /* 58 */ FlagId_E_IM0,   // interrupt masking
    /* 59 */ FlagId_E_IM1,
    /* 60 */ FlagId_E_IM2,
    /* 61 */ FlagId_E_IMV,
    /* 62 */ FlagId_E_NMIC,  // non maskable interrupt
    /* 63 */ FlagId_E_IC0,   // interrupt context switching flags
    /* 64 */ FlagId_E_IC1,
    /* 65 */ FlagId_E_IC2,
    /* 66 */ FlagId_E_LP,    // in loop bit
    /* 67 */ FlagId_E_BCx,   // block repeat counter
    /* 68 */ FlagId_E_PAGE,  // page for directPage addressing mode
    /* 69 */ FlagId_E_PCMhi, // PC register MSB's for movp and movd
    /* 70 */ FlagId_E_PChi,  // PC register 4 MSB's
    /* 71 */ FlagId_E_InRep, // in Rep
    /* 72 */ FlagId_E_EL,    // end loop indication for interrupts
    /* 73 */ FlagId_E_OU2,   // Output user pin 2
    /* 74 */ FlagId_E_OU3,   // Output user pin 3
    /* 75 */ FlagId_E_OU4,   // Output user pin 4

    /* 73 */ FlagId_E_Size
} T_FlagID;

//----------------------------------------------
#elif (DSP_TYPE == CEVAX)
//----------------------------------------------


// Interrupts vector addresses (in bytes):
//--------------------------------------------------------
#define Core_K_InterruptVectorAddress_Reset     0x00000000
#define Core_K_InterruptVectorAddress_TrapeBpi  0x00000020
#define Core_K_InterruptVectorAddress_Trap      0x00000040
#define Core_K_InterruptVectorAddress_Nmi       0x00000060
#define Core_K_InterruptVectorAddress_Int0      0x00000080
#define Core_K_InterruptVectorAddress_Int1      0x000000C0
#define Core_K_InterruptVectorAddress_Int2      0x00000100
#define Core_K_InterruptVectorAddress_Int3      0x00000140
#define Core_K_InterruptVectorAddress_Int4      0x00000180
#define Core_K_InterruptVectorAddress_Crcall    0x000001c0

//----------------------------------------------
//	Registers
//----------------------------------------------
// We use the Internal Register list in "reglist.h" in order to create both the
// Registers' Enumerator and strings array (for debug purposes).

// Creating the Register Enumerator:
// The Enumerator format is: InternalReg_E_<RegName>
#define M_RegList(RegId)  RegId_E_##RegId,

typedef enum T_RegId_tag
{	
	#include "reglist.h"		
	
}T_RegId;

#undef M_RegList

// Creating the Register string array:
// The string format is: "InternalReg_<RegName>"
#define M_RegList(RegId) "Reg_"#RegId,

static char *RegStringArr[RegId_E_Size + 1] = 
{
	#include "reglist.h"
};

#undef M_RegList
////////////////////////////////////////////////////

typedef enum T_RegPartId_tag {
RegPartId_E_R0H, //0
    RegPartId_E_R1H, //1
    RegPartId_E_R2H, //2
    RegPartId_E_R3H, //3
    RegPartId_E_R4H, //4
    RegPartId_E_R5H, //5
    RegPartId_E_R6H, //6
    RegPartId_E_R7H, //7
    RegPartId_E_G0H, //8
    RegPartId_E_G1H, //9
    RegPartId_E_G2H, //10
    RegPartId_E_G3H, //11
    RegPartId_E_G4H, //12
    RegPartId_E_G5H, //13
    RegPartId_E_G6H, //14
    RegPartId_E_G7H, //15
    RegPartId_E_MODU0H, //16
    RegPartId_E_MODU1H, //17
    RegPartId_E_MODU2H, //18
    RegPartId_E_MODU3H, //19
    RegPartId_E_S0H, //20
    RegPartId_E_S1H, //21
    RegPartId_E_S2H, //22
    RegPartId_E_S3H, //23
    RegPartId_E_MOD0H, //24
    RegPartId_E_MOD1H, //25
    RegPartId_E_MOD2H, //26
    RegPartId_E_R0L, //27
    RegPartId_E_R1L, //28
    RegPartId_E_R2L, //29
    RegPartId_E_R3L, //30
    RegPartId_E_R4L, //31
    RegPartId_E_R5L, //32
    RegPartId_E_R6L, //33
    RegPartId_E_R7L, //34
    RegPartId_E_G0L, //35
    RegPartId_E_G1L, //36
    RegPartId_E_G2L, //37
    RegPartId_E_G3L, //38
    RegPartId_E_G4L, //39
    RegPartId_E_G5L, //40
    RegPartId_E_G6L, //41
    RegPartId_E_G7L, //42
    RegPartId_E_MODU0L, //43
    RegPartId_E_MODU1L, //44
    RegPartId_E_MODU2L, //45
    RegPartId_E_MODU3L, //46
    RegPartId_E_S0L, //47
    RegPartId_E_S1L, //48
    RegPartId_E_S2L, //49
    RegPartId_E_S3L, //50
    RegPartId_E_MOD0L, //51
    RegPartId_E_MOD1L, //52
    RegPartId_E_MOD2L, //53
    RegPartId_E_SPL, //54
    RegPartId_E_SPH, //55
    RegPartId_E_A0H, //56
    RegPartId_E_B0H, //57
    RegPartId_E_C0H, //58
    RegPartId_E_D0H, //59
    RegPartId_E_A1H, //60
    RegPartId_E_B1H, //61
    RegPartId_E_C1H, //62
    RegPartId_E_D1H, //63
    RegPartId_E_A2H, //64
    RegPartId_E_B2H, //65
    RegPartId_E_C2H, //66
    RegPartId_E_D2H, //67
    RegPartId_E_A3H, //68
    RegPartId_E_B3H, //69
    RegPartId_E_C3H, //70
    RegPartId_E_D3H, //71
    RegPartId_E_A4H, //72
    RegPartId_E_B4H, //73
    RegPartId_E_C4H, //74
    RegPartId_E_D4H, //75
    RegPartId_E_A5H, //76
    RegPartId_E_B5H, //77
    RegPartId_E_C5H, //78
    RegPartId_E_D5H, //79
    RegPartId_E_A6H, //80
    RegPartId_E_B6H, //81
    RegPartId_E_C6H, //82
    RegPartId_E_D6H, //83
    RegPartId_E_A7H, //84
    RegPartId_E_B7H, //85
    RegPartId_E_C7H, //86
    RegPartId_E_D7H, //87
    RegPartId_E_A8H, //88
    RegPartId_E_B8H, //89
    RegPartId_E_C8H, //90
    RegPartId_E_D8H, //91
    RegPartId_E_A9H, //92
    RegPartId_E_B9H, //93
    RegPartId_E_C9H, //94
    RegPartId_E_D9H, //95
    RegPartId_E_A10H, //96
    RegPartId_E_B10H, //97
    RegPartId_E_C10H, //98
    RegPartId_E_D10H, //99
    RegPartId_E_A11H, //100
    RegPartId_E_B11H, //101
    RegPartId_E_C11H, //102
    RegPartId_E_D11H, //103
    RegPartId_E_A12H, //104
    RegPartId_E_B12H, //105
    RegPartId_E_C12H, //106
    RegPartId_E_D12H, //107
    RegPartId_E_A13H, //108
    RegPartId_E_B13H, //109
    RegPartId_E_C13H, //110
    RegPartId_E_D13H, //111
    RegPartId_E_A14H, //112
    RegPartId_E_B14H, //113
    RegPartId_E_C14H, //114
    RegPartId_E_D14H, //115
    RegPartId_E_A15H, //116
    RegPartId_E_B15H, //117
    RegPartId_E_C15H, //118
    RegPartId_E_D15H, //119

    // Dima Bayer [01/11/2006] - cevax 1641 task:
    RegPartId_E_A16H, //120
    RegPartId_E_B16H, //121
    RegPartId_E_C16H, //122
    RegPartId_E_D16H, //123
    RegPartId_E_A17H, //124
    RegPartId_E_B17H, //125
    RegPartId_E_C17H, //126
    RegPartId_E_D17H, //127
    RegPartId_E_A18H, //128
    RegPartId_E_B18H, //129
    RegPartId_E_C18H, //130
    RegPartId_E_D18H, //131
    RegPartId_E_A19H, //132
    RegPartId_E_B19H, //133
    RegPartId_E_C19H, //134
    RegPartId_E_D19H, //135
    RegPartId_E_A20H, //136
    RegPartId_E_B20H, //137
    RegPartId_E_C20H, //138
    RegPartId_E_D20H, //139
    RegPartId_E_A21H, //140
    RegPartId_E_B21H, //141
    RegPartId_E_C21H, //142
    RegPartId_E_D21H, //143
    RegPartId_E_A22H, //144
    RegPartId_E_B22H, //145
    RegPartId_E_C22H, //146
    RegPartId_E_D22H, //147
    RegPartId_E_A23H, //148
    RegPartId_E_B23H, //149
    RegPartId_E_C23H, //150
    RegPartId_E_D23H, //151


    RegPartId_E_A0L, //152
    RegPartId_E_B0L, //153
    RegPartId_E_C0L, //154
    RegPartId_E_D0L, //155
    RegPartId_E_A1L, //156
    RegPartId_E_B1L, //157
    RegPartId_E_C1L, //158
    RegPartId_E_D1L, //159
    RegPartId_E_A2L, //160
    RegPartId_E_B2L, //161
    RegPartId_E_C2L, //162
    RegPartId_E_D2L, //163
    RegPartId_E_A3L, //164
    RegPartId_E_B3L, //165
    RegPartId_E_C3L, //166
    RegPartId_E_D3L, //167
    RegPartId_E_A4L, //168
    RegPartId_E_B4L, //169
    RegPartId_E_C4L, //170
    RegPartId_E_D4L, //171
    RegPartId_E_A5L, //172
    RegPartId_E_B5L, //173
    RegPartId_E_C5L, //174
    RegPartId_E_D5L, //175
    RegPartId_E_A6L, //176
    RegPartId_E_B6L, //177
    RegPartId_E_C6L, //178
    RegPartId_E_D6L, //179
    RegPartId_E_A7L, //180
    RegPartId_E_B7L, //181
    RegPartId_E_C7L, //182
    RegPartId_E_D7L, //183
    RegPartId_E_A8L, //184
    RegPartId_E_B8L, //185
    RegPartId_E_C8L, //186
    RegPartId_E_D8L, //187
    RegPartId_E_A9L, //188
    RegPartId_E_B9L, //189
    RegPartId_E_C9L, //190
    RegPartId_E_D9L, //191
    RegPartId_E_A10L, //192
    RegPartId_E_B10L, //193
    RegPartId_E_C10L, //194
    RegPartId_E_D10L, //195
    RegPartId_E_A11L, //196
    RegPartId_E_B11L, //197
    RegPartId_E_C11L, //198
    RegPartId_E_D11L, //199
    RegPartId_E_A12L, //200
    RegPartId_E_B12L, //201
    RegPartId_E_C12L, //202
    RegPartId_E_D12L, //203
    RegPartId_E_A13L, //204
    RegPartId_E_B13L, //205
    RegPartId_E_C13L, //206
    RegPartId_E_D13L, //207
    RegPartId_E_A14L, //208
    RegPartId_E_B14L, //209
    RegPartId_E_C14L, //210
    RegPartId_E_D14L, //211
    RegPartId_E_A15L, //212
    RegPartId_E_B15L, //213
    RegPartId_E_C15L, //214
    RegPartId_E_D15L, //215

    // Dima Bayer [01/11/2006] - cevax 1641 task:
    RegPartId_E_A16L, //216
    RegPartId_E_B16L, //217
    RegPartId_E_C16L, //218
    RegPartId_E_D16L, //219
    RegPartId_E_A17L, //220
    RegPartId_E_B17L, //221
    RegPartId_E_C17L, //222
    RegPartId_E_D17L, //223
    RegPartId_E_A18L, //224
    RegPartId_E_B18L, //225
    RegPartId_E_C18L, //226
    RegPartId_E_D18L, //227
    RegPartId_E_A19L, //228
    RegPartId_E_B19L, //229
    RegPartId_E_C19L, //230
    RegPartId_E_D19L, //231
    RegPartId_E_A20L, //232
    RegPartId_E_B20L, //233
    RegPartId_E_C20L, //234
    RegPartId_E_D20L, //235
    RegPartId_E_A21L, //236
    RegPartId_E_B21L, //237
    RegPartId_E_C21L, //238
    RegPartId_E_D21L, //239
    RegPartId_E_A22L, //240
    RegPartId_E_B22L, //241
    RegPartId_E_C22L, //242
    RegPartId_E_D22L, //243
    RegPartId_E_A23L, //244
    RegPartId_E_B23L, //245
    RegPartId_E_C23L, //246
    RegPartId_E_D23L, //247

    RegPartId_E_LCI0L, //248
    RegPartId_E_LCI1L, //249
    RegPartId_E_LCI2L, //250
    RegPartId_E_LCI3L, //251
    RegPartId_E_LCSTEP0L, //252
    RegPartId_E_LCSTEP1L, //253
    RegPartId_E_BKNEST0L, //254
    RegPartId_E_BKNEST1L, //255
    RegPartId_E_MODPL, //260
    RegPartId_E_LCL, //261
    RegPartId_E_RETREGL, //262
    RegPartId_E_RETREGIL, //263
    RegPartId_E_RETREGNL, //264
    RegPartId_E_RETREGBL, //265
    RegPartId_E_MODQL, //266
	RegPartId_E_VERSIONL, //267
    RegPartId_E_CRPCL, //268
    RegPartId_E_Size
} T_RegPartId;

// Registers types:
//=================
typedef DWORD T_Reg;

// in Cevax normal regs are 32 bits:
typedef T_RegId T_ShortRegID;
typedef T_RegId T_LongRegID;
#define M_RegId(reg)   RegId_E_##reg

// Flags/Fields/Control bits enumeration:
//=======================================
typedef enum T_FlagId_tag {

    // modp flags:
    /* 0  */ FlagId_E_ICNTX, // Interrupt context switching includes NMI, INT0-3
    /* 1  */ FlagId_E_NMIC,  // Interrupt context for NMI
    /* 2  */ FlagId_E_IC0,   // Interrupt context for INT0
    /* 3  */ FlagId_E_IC1,   // Interrupt context for INT1
    /* 4  */ FlagId_E_IC2,   // Interrupt context for INT2
    /* 5  */ FlagId_E_IC3,   // Interrupt context for INT3
    /* 6  */ FlagId_E_IC4,   // Interrupt context for INT4
    /* 7  */ FlagId_E_IE,    // Interrupt Enable

    /* 8  */ FlagId_E_IMASK, // Interrupt Mask - includes INT0-3 + VINT
    /* 9  */ FlagId_E_IM0,   // Interrupt Mask for INT0
    /* 10 */ FlagId_E_IM1,   // Interrupt Mask for INT1
    /* 11 */ FlagId_E_IM2,   // Interrupt Mask for INT2
    /* 12 */ FlagId_E_IM3,   // Interrupt Mask for INT3
    /* 13 */ FlagId_E_IM4,   // Interrupt Mask for INT4
    /* 14 */ FlagId_E_IMV,   // Interrupt Mask for VINT

    /* 15 */ FlagId_E_IPEND, // Interrupt Pending
    /* 16 */ FlagId_E_IP0,   // Interrupt Pending for INT0
    /* 17 */ FlagId_E_IP1,   // Interrupt Pending for INT1
    /* 18 */ FlagId_E_IP2,   // Interrupt Pending for INT2
    /* 19 */ FlagId_E_IP3,   // Interrupt Pending for INT3
    /* 20 */ FlagId_E_IP4,   // Interrupt Pending for INT4
    /* 21 */ FlagId_E_IPV,   // Interrupt Pending for VINT

    /* 22 */ FlagId_E_UO,    // User Output bits - includes UO0-UO11
    /* 23 */ FlagId_E_UO0,   // User Output bit #0
    /* 24 */ FlagId_E_UO1,   // User Output bit #1
    /* 25 */ FlagId_E_UO2,   // User Output bit #2
    /* 26 */ FlagId_E_UO3,   // User Output bit #3
    /* 27 */ FlagId_E_UO4,   // User Output bit #4
    /* 28 */ FlagId_E_UO5,   // User Output bit #5

    /* 29 */ FlagId_E_BCx,   // Block Repeat nesting level
    /* 30 */ FlagId_E_LP,    // Loop indication

    // moda, modb, modc, modd flags :
    // DON'T CHANGE, Must appear in this order to allow
    // updaing using flagid_cluster_a + cluster_index for all clusters
    /* 31 */ FlagId_E_SATMOD_MODA, // Saturation mode cluster A - includes all sat bits
    /* 32 */ FlagId_E_SATMOD_MODB, // Saturation mode cluster B - includes all sat bits
    /* 33 */ FlagId_E_SATMOD_MODC, // Saturation mode cluster C - includes all sat bits
    /* 34 */ FlagId_E_SATMOD_MODD, // Saturation mode cluster D - includes all sat bits
    /* 35 */ FlagId_E_SAT_MODA,  // SAT cluster A
    /* 36 */ FlagId_E_SAT_MODB,  // SAT mode cluster B
    /* 37 */ FlagId_E_SAT_MODC,  // SAT mode cluster C
    /* 38 */ FlagId_E_SAT_MODD,  // SAT mode cluster D
    /* 39 */ FlagId_E_SATA_MODA, // SATA cluster A
    /* 40 */ FlagId_E_SATA_MODB, // SATA cluster B
    /* 41 */ FlagId_E_SATA_MODC, // SATA cluster C
    /* 42 */ FlagId_E_SATA_MODD, // SATA cluster D
    /* 43 */ FlagId_E_SATP_MODA, // SATP cluster A
    /* 44 */ FlagId_E_SATP_MODB, // SATP cluster B
    /* 45 */ FlagId_E_SATP_MODC, // SATP cluster C
    /* 46 */ FlagId_E_SATP_MODD, // SATP cluster D
    /* 47 */ FlagId_E_SATW_MODA, // SATW cluster A
    /* 48 */ FlagId_E_SATW_MODB, // SATW cluster B
    /* 49 */ FlagId_E_SATW_MODC, // SATW cluster C
    /* 50 */ FlagId_E_SATW_MODD, // SATW cluster D
    /* 51 */ FlagId_E_SATB_MODA, // SATB cluster A
    /* 52 */ FlagId_E_SATB_MODB, // SATB cluster B
    /* 53 */ FlagId_E_SATB_MODC, // SATB cluster C
    /* 54 */ FlagId_E_SATB_MODD, // SATB cluster D
    /* 55 */ FlagId_E_UO_MODA,     // User output cluster A - includes U0A0, UOA1
    /* 56 */ FlagId_E_UO_MODB,     // User output cluster B - includes U0B0, UOB1
    /* 57 */ FlagId_E_UO_MODC,     // User output cluster C - includes U0C0, UOC1
    /* 58 */ FlagId_E_UO_MODD,     // User output cluster D - includes U0D0, UOD1
    /* 59 */ FlagId_E_UO0_MODA,    // User outputs 0 cluster A
    /* 60 */ FlagId_E_UO0_MODB,    // User outputs 0 cluster B
    /* 61 */ FlagId_E_UO0_MODC,    // User outputs 0 cluster C
    /* 62 */ FlagId_E_UO0_MODD,    // User outputs 0 cluster D
    /* 63 */ FlagId_E_UO1_MODA,    // User outputs 1 cluster A
    /* 64 */ FlagId_E_UO1_MODB,    // User outputs 1 cluster B
    /* 65 */ FlagId_E_UO1_MODC,    // User outputs 1 cluster C
    /* 66 */ FlagId_E_UO1_MODD,    // User outputs 1 cluster D
    /* 67 */ FlagId_E_PS_MODA,     // Product shift cluster A
    /* 68 */ FlagId_E_PS_MODB,     // Product shift cluster B
    /* 69 */ FlagId_E_PS_MODC,     // Product shift cluster C
    /* 70 */ FlagId_E_PS_MODD,     // Product shift cluster D
    /* 71 */ FlagId_E_FLAGS_MODA,  // All CBU Flags of cluster A
    /* 72 */ FlagId_E_FLAGS_MODB,  // All CBU Flags of cluster B
    /* 73 */ FlagId_E_FLAGS_MODC,  // All CBU Flags of cluster C
    /* 74 */ FlagId_E_FLAGS_MODD,  // All CBU Flags of cluster D
    /* 75 */ FlagId_E_VS_MODA,     // Overflow flag of S unit cluster A
    /* 76 */ FlagId_E_VS_MODB,     // Overflow flag of S unit cluster B
    /* 77 */ FlagId_E_VS_MODC,     // Overflow flag of S unit cluster C
    /* 78 */ FlagId_E_VS_MODD,     // Overflow flag of S unit cluster D
    /* 79 */ FlagId_E_CS_MODA,     // Carry flag of S unit cluster A
    /* 80 */ FlagId_E_CS_MODB,     // Carry flag of S unit cluster B
    /* 81 */ FlagId_E_CS_MODC,     // Carry flag of S unit cluster C
    /* 82 */ FlagId_E_CS_MODD,     // Carry flag of S unit cluster D
    /* 83 */ FlagId_E_VL_MODA,     // Overflow flag of L unit cluster A
    /* 84 */ FlagId_E_VL_MODB,     // Overflow flag of L unit cluster B
    /* 85 */ FlagId_E_VL_MODC,     // Overflow flag of L unit cluster C
    /* 86 */ FlagId_E_VL_MODD,     // Overflow flag of L unit cluster D
    /* 87 */ FlagId_E_CL_MODA,     // Carry flag of L unit cluster A
    /* 88 */ FlagId_E_CL_MODB,     // Carry flag of L unit cluster B
    /* 89 */ FlagId_E_CL_MODC,     // Carry flag of L unit cluster C
    /* 90 */ FlagId_E_CL_MODD,     // Carry flag of L unit cluster D
    /* 91 */ FlagId_E_VM1_MODA,    // Overflow flag of M1 unit cluster A
    /* 92 */ FlagId_E_VM1_MODB,    // Overflow flag of M1 unit cluster B
    /* 93 */ FlagId_E_VM1_MODC,    // Overflow flag of M1 unit cluster C
    /* 94 */ FlagId_E_VM1_MODD,    // Overflow flag of M1 unit cluster D
    /* 95 */ FlagId_E_CM1_MODA,    // Carry flag of M1 unit cluster A
    /* 96 */ FlagId_E_CM1_MODB,    // Carry flag of M1 unit cluster B
    /* 97 */ FlagId_E_CM1_MODC,    // Carry flag of M1 unit cluster C
    /* 98 */ FlagId_E_CM1_MODD,    // Carry flag of M1 unit cluster D
    /* 99*/  FlagId_E_VM0_MODA,    // Overflow flag of M0 unit cluster A
    /* 100*/ FlagId_E_VM0_MODB,    // Overflow flag of M0 unit cluster B
    /* 101*/ FlagId_E_VM0_MODC,    // Overflow flag of M0 unit cluster C
    /* 102*/ FlagId_E_VM0_MODD,    // Overflow flag of M0 unit cluster D
    /* 103*/ FlagId_E_CM0_MODA,    // Carry flag of M0 unit cluster A
    /* 104*/ FlagId_E_CM0_MODB,    // Carry flag of M0 unit cluster B
    /* 105*/ FlagId_E_CM0_MODC,    // Carry flag of M0 unit cluster C
    /* 106*/ FlagId_E_CM0_MODD,    // Carry flag of M0 unit cluster D
    /* 107*/ FlagId_E_CVLCLM_MODA, // Cluster Overflow-Limit Flag Cluster A
    /* 108*/ FlagId_E_CVLCLM_MODB, // Cluster Overflow-Limit Flag Cluster B
    /* 109*/ FlagId_E_CVLCLM_MODC, // Cluster Overflow-Limit Flag Cluster C
    /* 110*/ FlagId_E_CVLCLM_MODD, // Cluster Overflow-Limit Flag Cluster D
    /* 111*/ FlagId_E_CLM_MODA,    // Cluster Limit Flag cluster A
    /* 112*/ FlagId_E_CLM_MODB,    // Cluster Limit Flag cluster B
    /* 113*/ FlagId_E_CLM_MODC,    // Cluster Limit Flag cluster C
    /* 114*/ FlagId_E_CLM_MODD,    // Cluster Limit Flag cluster D
    /* 115*/ FlagId_E_CVL_MODA,    // Cluster Latched Overflow cluster A
    /* 116*/ FlagId_E_CVL_MODB,    // Cluster Latched Overflow cluster B
    /* 117*/ FlagId_E_CVL_MODC,    // Cluster Latched Overflow cluster C
    /* 118*/ FlagId_E_CVL_MODD,    // Cluster Latched Overflow cluster D
    /* 119*/ FlagId_E_UI_MODA,     // User inputs cluster A - includes UIA0, UIA1
    /* 120*/ FlagId_E_UI_MODB,     // User inputs cluster B - includes UIB0, UIB1
    /* 121*/ FlagId_E_UI_MODC,     // User inputs cluster C - includes UIC0, UIC1
    /* 122*/ FlagId_E_UI_MODD,     // User inputs cluster D - includes UID0, UID1
    /* 123*/ FlagId_E_UI0_MODA,    // User inputs 0 cluster A
    /* 124*/ FlagId_E_UI0_MODB,    // User inputs 0 cluster B
    /* 125*/ FlagId_E_UI0_MODC,    // User inputs 0 cluster C
    /* 126*/ FlagId_E_UI0_MODD,    // User inputs 0 cluster D
    /* 127*/ FlagId_E_UI1_MODA,    // User inputs 1 cluster A
    /* 128*/ FlagId_E_UI1_MODB,    // User inputs 1 cluster B
    /* 129*/ FlagId_E_UI1_MODC,    // User inputs 1 cluster C
    /* 130*/ FlagId_E_UI1_MODD,    // User inputs 1 cluster D
    /* 131*/ FlagId_E_PRA0123,     // Predicate registers cluster A
    /* 132*/ FlagId_E_PRB0123,     // Predicate registers cluster B
    /* 133*/ FlagId_E_PRC0123,     // Predicate registers cluster C
    /* 134*/ FlagId_E_PRD0123,     // Predicate registers cluster D
    /* 135*/ FlagId_E_PRA0,        // Predicate 0 cluster A
    /* 136*/ FlagId_E_PRB0,        // Predicate 0 cluster B
    /* 137*/ FlagId_E_PRC0,        // Predicate 0 cluster C
    /* 138*/ FlagId_E_PRD0,        // Predicate 0 cluster D
    /* 139*/ FlagId_E_PRA1,        // Predicate 1 cluster A
    /* 140*/ FlagId_E_PRB1,        // Predicate 1 cluster B
    /* 141*/ FlagId_E_PRC1,        // Predicate 1 cluster C
    /* 142*/ FlagId_E_PRD1,        // Predicate 1 cluster D
    /* 143*/ FlagId_E_PRA2,        // Predicate 2 cluster A
    /* 144*/ FlagId_E_PRB2,        // Predicate 2 cluster B
    /* 145*/ FlagId_E_PRC2,        // Predicate 2 cluster C
    /* 146*/ FlagId_E_PRD2,        // Predicate 2 cluster D
    /* 147*/ FlagId_E_PRA3,        // Predicate 3 cluster A
    /* 148*/ FlagId_E_PRB3,        // Predicate 3 cluster B
    /* 149*/ FlagId_E_PRC3,        // Predicate 3 cluster C
    /* 150*/ FlagId_E_PRD3,        // Predicate 3 cluster D
    /* 151*/ FlagId_E_PRA01,       // Predicate 01 cluster A
    /* 152*/ FlagId_E_PRB01,       // Predicate 01 cluster B
    /* 153*/ FlagId_E_PRC01,       // Predicate 01 cluster C
    /* 154*/ FlagId_E_PRD01,       // Predicate 01 cluster D
    /* 155*/ FlagId_E_PRA23,       // Predicate 23 cluster A
    /* 156*/ FlagId_E_PRB23,       // Predicate 23 cluster B
    /* 157*/ FlagId_E_PRC23,       // Predicate 23 cluster C
    /* 158*/ FlagId_E_PRD23,       // Predicate 23 cluster D

    // mod0 flags:
    /* 159*/FlagId_E_MD,           // Modulo enable
    /* 160*/FlagId_E_MD0,          // Modulo enable r0
    /* 161*/FlagId_E_MD1,          // Modulo enable r1
    /* 162*/FlagId_E_MD2,          // Modulo enable r2
    /* 163*/ FlagId_E_MD3,         // Modulo enable r3
    /* 164*/ FlagId_E_MD4,         // Modulo enable r4
    /* 165*/ FlagId_E_MD5,         // Modulo enable r5
    /* 166*/ FlagId_E_MD6,         // Modulo enable r6
    /* 167*/ FlagId_E_MD7,         // Modulo enable r7
    /* 168*/ FlagId_E_BRV,         // Bit Reverse Enable
    /* 169*/ FlagId_E_BRV0,        // Bit Reverse Enable r0
    /* 170*/ FlagId_E_BRV1,        // Bit Reverse Enable r1
    /* 171*/ FlagId_E_BRV2,        // Bit Reverse Enable r2
    /* 172*/ FlagId_E_BRV3,        // Bit Reverse Enable r3
    /* 173*/ FlagId_E_BRV4,        // Bit Reverse Enable r4
    /* 174*/ FlagId_E_BRV5,        // Bit Reverse Enable r5
    /* 175*/ FlagId_E_BRV6,        // Bit Reverse Enable r6
    /* 176*/ FlagId_E_BRV7,        // Bit Reverse Enable r7
    /* 177*/ FlagId_E_PRG,         // Global Predicate Registers
    /* 178*/ FlagId_E_PRG0,        // Global Predicate 0
    /* 179*/ FlagId_E_PRG1,        // Global Predicate 1
    /* 180*/ FlagId_E_PRG2,        // Global Predicate 2
    /* 181*/ FlagId_E_PRG01,       // Global Predicate 01
    /* 182*/ FlagId_E_PRG12,       // Global Predicate 12
    /* 183*/ FlagId_E_PRS,         // DAAU Predicate Registers
    /* 184*/ FlagId_E_PRS0,        // DAAU Predicate 0
    /* 185*/ FlagId_E_PRS1,        // DAAU Predicate 1
    /* 186*/ FlagId_E_PRS2,        // DAAU Predicate 2
    /* 187*/ FlagId_E_PRS3,        // DAAU Predicate 3
    /* 188*/ FlagId_E_PRS01,       // DAAU Predicate 01
    /* 189*/ FlagId_E_PRS23,       // DAAU Predicate 23

     // mod1 flags:
    /* 190*/ FlagId_E_PAGE,        // Data Memory Page (short direct mode)

    // mod2 flags:
    /* 191*/ FlagId_E_IOPAGE,      // I/O Space Page (short direct mode)

    // modq flags
    /* 192*/ FlagId_E_MS,          // monitor status
    /* 193*/ FlagId_E_OM,          // operation mode
    /* 194*/ FlagId_E_POM,         // previous operation mode
    /* 195*/ FlagId_E_PI,          // permission interrupt output pin
    /* 196*/ FlagId_E_PV,          // permission vioulation
    /* 197*/ FlagId_E_VOM,         // violation operation mode

    // virtual flags
    /* 198*/ FlagId_E_ACLM,        // Fake flag for or of all CLM flags
    /* 199*/ FlagId_E_ACVL,        // Fake flag for or of all CVL flags
    /* 200*/ FlagId_E_PRED_TRUE,   // Fake flag for predication true case
    /* 201*/ FlagId_E_PRA02,       // Fake flag for predicates 0 & 2 combination
    /* 202*/ FlagId_E_PRB02,       // Fake flag for predicates 0 & 2 combination
    /* 203*/ FlagId_E_PRC02,       // Fake flag for predicates 0 & 2 combination
    /* 204*/ FlagId_E_PRD02,       // Fake flag for predicates 0 & 2 combination
    /* 205*/ FlagId_E_PRA03,       // Fake flag for predicates 0 & 3 combination
    /* 206*/ FlagId_E_PRB03,       // Fake flag for predicates 0 & 3 combination
    /* 207*/ FlagId_E_PRC03,       // Fake flag for predicates 0 & 3 combination
    /* 208*/ FlagId_E_PRD03,       // Fake flag for predicates 0 & 3 combination
    /* 209*/ FlagId_E_PRA12,       // Fake flag for predicates 1 & 2 combination
    /* 210*/ FlagId_E_PRB12,       // Fake flag for predicates 1 & 2 combination
    /* 211*/ FlagId_E_PRC12,       // Fake flag for predicates 1 & 2 combination
    /* 212*/ FlagId_E_PRD12,       // Fake flag for predicates 1 & 2 combination
    /* 213*/ FlagId_E_PRA13,       // Fake flag for predicates 1 & 3 combination
    /* 214*/ FlagId_E_PRB13,       // Fake flag for predicates 1 & 3 combination
    /* 215*/ FlagId_E_PRC13,       // Fake flag for predicates 1 & 3 combination
    /* 216*/ FlagId_E_PRD13,       // Fake flag for predicates 1 & 3 combination
    /* 217*/ FlagId_E_PRS02,       // Fake flag for predicates 0 & 2 combination
    /* 218*/ FlagId_E_PRS03,       // Fake flag for predicates 0 & 3 combination
    /* 219*/ FlagId_E_PRS12,       // Fake flag for predicates 1 & 2 combination
    /* 220*/ FlagId_E_PRS13,       // Fake flag for predicates 1 & 3 combination
    /* 221*/ FlagId_E_PRG02,       // Fake flag for predicates 0 & 2 combination
    /* 222*/ FlagId_E_PRG20,       // Fake flag for predicates 2 & 0 combination
    /* 223*/ FlagId_E_PRA0213,     // Fake flag for predicates 02 & 13 combination
    /* 224*/ FlagId_E_PRB0213,     // Fake flag for predicates 02 & 13 combination
    /* 225*/ FlagId_E_PRC0213,     // Fake flag for predicates 02 & 13 combination
    /* 226*/ FlagId_E_PRD0213,     // Fake flag for predicates 02 & 13 combination

    // IMPORTANT - this enum is saved in a char type variable for space efficiency
    // therfore any FlagId that can appear in an instruction must have an index which
    // is smaller than 255 (shadows are not coded)

    // Shadow flags
    /* 227*/ FlagId_E_SATMOD_MODA_SHADOW,
    /* 228*/ FlagId_E_SATMOD_MODB_SHADOW,
    /* 229*/ FlagId_E_SATMOD_MODC_SHADOW,
    /* 230*/ FlagId_E_SATMOD_MODD_SHADOW,
    /* 231*/ FlagId_E_PS_MODA_SHADOW,
    /* 232*/ FlagId_E_PS_MODB_SHADOW,
    /* 233*/ FlagId_E_PS_MODC_SHADOW,
    /* 234*/ FlagId_E_PS_MODD_SHADOW,
    /* 235*/ FlagId_E_PRA0123_SHADOW,
    /* 236*/ FlagId_E_PRB0123_SHADOW,
    /* 237*/ FlagId_E_PRC0123_SHADOW,
    /* 238*/ FlagId_E_PRD0123_SHADOW,
    /* 239*/ FlagId_E_FLAGS_MODA_SHADOW,
    /* 240*/ FlagId_E_FLAGS_MODB_SHADOW,
    /* 241*/ FlagId_E_FLAGS_MODC_SHADOW,
    /* 242*/ FlagId_E_FLAGS_MODD_SHADOW,
    /* 243*/ FlagId_E_CLM_MODA_SHADOW,
    /* 244*/ FlagId_E_CLM_MODB_SHADOW,
    /* 245*/ FlagId_E_CLM_MODC_SHADOW,
    /* 246*/ FlagId_E_CLM_MODD_SHADOW,
    /* 247*/ FlagId_E_CVL_MODA_SHADOW,
    /* 248*/ FlagId_E_CVL_MODB_SHADOW,
    /* 249*/ FlagId_E_CVL_MODC_SHADOW,
    /* 250*/ FlagId_E_CVL_MODD_SHADOW,
    /* 251*/ FlagId_E_PRS_SHADOW,
    /* 252*/ FlagId_E_PRG_SHADOW,
    /* 253*/ FlagId_E_BRV_SHADOW,
    /* 254*/ FlagId_E_MD_SHADOW,
    /* 255*/ FlagId_E_PAGE_SHADOW,
    /* 256*/ FlagId_E_IOPAGE_SHADOW,
    /* 257*/ FlagId_E_IMASK_SHADOW,

    /* 258*/ FlagId_E_Size
} T_FlagId;


// IO Pins Enumeration
//=======================================
typedef enum T_IOPinId_tag {
    //INPUTS
    //General and logic inputs
    IOPinId_E_boot,          //general Boot request
    IOPinId_E_ceva_clk,      //general Core clock
    IOPinId_E_external_wait, //logic External wait request
    IOPinId_E_orst_n,        //logic Asynchronous reset
    IOPinId_E_tck,           //logic JTAG protocol tck
    IOPinId_E_tms,           //logic JTAG protocol tms
    IOPinId_E_tdi,           //logic JTAG protocol tdi
    IOPinId_E_testmodep,     //general External test mode indication
    IOPinId_E_scanen,        //general External scan enable indication

    //Interrupt control unit inputs
    IOPinId_E_int0,          //cedar Maskable interrupt 0
    IOPinId_E_int1,          //cedar Maskable interrupt 1
    IOPinId_E_int2,          //cedar Maskable interrupt 2
    IOPinId_E_nmi,           //cedar Non-maskable interrupt
    IOPinId_E_vint,          //cedar Vectored interrupt
    IOPinId_E_vector,        //cedar Interrupt vector
    IOPinId_E_vintc,         //cedar vint context switch enable

    //Semaphore interface
    IOPinId_E_mon_stat,      //cedar Monitor status bit

    //Other inputs
    IOPinId_E_uia0,         //cedar	User input a0 to CBU
    IOPinId_E_uia1,         //cedar	User input a1 to CBU
    IOPinId_E_uib0,         //cedar	User input b0 to CBU
    IOPinId_E_uib1,         //cedar	User input b1 to CBU
    IOPinId_E_uic0,         //cedar	User input c0 to CBU
    IOPinId_E_uic1,         //cedar	User input c1 to CBU
    IOPinId_E_uid0,         //cedar	User input d0 to CBU
    IOPinId_E_uid1,         //cedar	User input d1 to CBU
    IOPinId_E_ext_pv,       //cedar	External permission violation indication
    IOPinId_E_ext_vom,      //cedar	External permission violation operation mode

    //PMSS inputs - External Memory
    IOPinId_E_mp_pdin,      //pmss Data in from external memory
    IOPinId_E_mp_dready,    //pmss Ready indication from external memory

    //IO Port inputs
    IOPinId_E_iop_din,      //ioport External data in
    IOPinId_E_iop_ready,    //ioport External ready indication

    //DMSS inputs - External Memory
    IOPinId_E_md_ep_dready,  //dmss	External port data ready indication
    IOPinId_E_md_ep_pdin,    //dmss	External port datain bus

    //DMSS inputs - External Transfer Port
    IOPinId_E_md_etp_padd,   //dmss	External transfer port address bus
    IOPinId_E_md_etp_pdin,   //dmss	External transfer port datain bus
    IOPinId_E_md_etp_pmode,  //dmss	External transfer port mode
    IOPinId_E_md_etp_pbsize, //dmss	External transfer port burst size
    IOPinId_E_md_etp_pwidth, //dmss	External transfer port width
    IOPinId_E_md_etp_prw,    //dmss	External transfer port read/write indication
    IOPinId_E_md_etp_pslct,  //dmss	External transfer port select indication
    IOPinId_E_md_etp_pprio,  //dmss	External transfer port priority indication over the DMA

    //DMSS inputs - DMA debug
    IOPinId_E_md_dma_dbg_ext_ack,    //dmss	External acknoledge for the DMA/ETP debug match

    //OCEM inputs
    IOPinId_E_usr_scan1_tdo, //ocem	User scan chain #1 Test Data Out signal
    IOPinId_E_usr_scan2_tdo, //ocem	User scan chain #2 Test Data Out signal
    IOPinId_E_usr_scan3_tdo, //ocem	User scan chain #3 Test Data Out signal
    IOPinId_E_usr_scan4_tdo, //ocem	User scan chain #4 Test Data Out signal
    IOPinId_E_ext_bp1_req,   //ocem	External breakpoint request #1
    IOPinId_E_ext_bp2_req,   //ocem	External breakpoint request #2
    IOPinId_E_ext_pmem_wr,   //ocem	External memory write indication
    IOPinId_E_bs_reg_tdo,    //ocem	Boundary scan register Test Data Out signal

    //TCB inputs
    IOPinId_E_tcb_hold,      //tcb TCB hold indication
    IOPinId_E_tcb_tdi,       //tcb TCB data input
    IOPinId_E_tcb_tck,       //tcb TCB clock
    IOPinId_E_tcb_enable,    //tcb TCB enable indication
    IOPinId_E_tcb_update,    //tcb TCB update indication
    IOPinId_E_tcb_tc,        //tcb TCB control


    //OUTPUTS
    //User outputs
    IOPinId_E_ceva_cuoa0,    //cedar CBU User output a0
    IOPinId_E_ceva_cuoa1,    //cedar CBU User output a1
    IOPinId_E_ceva_cuob0,    //cedar CBU User output b0
    IOPinId_E_ceva_cuob1,    //cedar CBU User output b1
    IOPinId_E_ceva_cuoc0,    //cedar CBU User output c0
    IOPinId_E_ceva_cuoc1,    //cedar CBU User output c1
    IOPinId_E_ceva_cuod0,    //cedar CBU User output d0
    IOPinId_E_ceva_cuod1,    //cedar CBU User output d1
    IOPinId_E_ceva_puo0,     //cedar PCU User output 0
    IOPinId_E_ceva_puo1,     //cedar PCU User output 1
    IOPinId_E_ceva_puo2,     //cedar PCU User output 2
    IOPinId_E_ceva_puo3,     //cedar PCU User output 3
    IOPinId_E_ceva_puo4,     //cedar PCU User output 4
    IOPinId_E_ceva_puo5,     //cedar PCU User output 5

    //Verification indications
    IOPinId_E_ceva_cverbit,  //cedar Verification bit
    IOPinId_E_ceva_peotbit,  //cedar End Of Test bit

    //Interrupt indications
    IOPinId_E_ceva_piack0n,  //cedar	int0 acknowledge
    IOPinId_E_ceva_piack1n,  //cedar	int1 acknowledge
    IOPinId_E_ceva_piack2n,  //cedar	int2 acknowledge
    IOPinId_E_ceva_piackbn,  //cedar	trape/breakpoint acknowledge
    IOPinId_E_ceva_piacknn,  //cedar	nmi acknowledge
    IOPinId_E_ceva_piackvn,  //cedar	vint acknowledge

    // Maor Avni [12/1/2005]: New Ack Pins fort RTL 1.2
    IOPinId_E_ceva_piack0n_4c,  //cedar	int0 stretched acknowledge
    IOPinId_E_ceva_piack1n_4c,  //cedar	int1 stretched acknowledge
    IOPinId_E_ceva_piack2n_4c,  //cedar	int2 stretched acknowledge
    IOPinId_E_ceva_piackbn_4c,  //cedar	trape/breakpoint streched acknowledge
    IOPinId_E_ceva_piacknn_4c,  //cedar	nmi stretched acknowledge
    IOPinId_E_ceva_piackvn_4c,  //cedar	vint stretched acknowledge\

    //Semaphore interface
    IOPinId_E_ceva_pmon_off_d3,  //cedar monitor{off} instruction indication
    IOPinId_E_ceva_pmon_on_d3,   //cedar monitor{on} instruction indication

    //Other inputs
    IOPinId_E_ceva_ptrp_srv,     //cedar An active service routine of a trap instruction

    //Operation Mode Support
    IOPinId_E_ceva_p_om,         //cedar Operation mode
    IOPinId_E_ceva_p_pi_out,     //cedar Permission interrupt output

    //PMSS outputs - External Memory
    IOPinId_E_ceva_mp_padd,      //pmss	External port address bus
    IOPinId_E_ceva_mp_pbsize,    //pmss	External port burst size bus
    IOPinId_E_ceva_mp_pdout,     //pmss	External port data bus
    IOPinId_E_ceva_mp_pmode,     //pmss	External port mode bus
    IOPinId_E_ceva_mp_prw,       //pmss	External port read/write strobe
    IOPinId_E_ceva_mp_pwidth,    //pmss	External port width bus

    //IO Port outputs
    IOPinId_E_ceva_iop_width,    //ioport IO port MMIO width bus
    IOPinId_E_ceva_iop_wr,       //ioport IO port MMIO write strobe
    IOPinId_E_ceva_iop_rd,       //ioport IO port MMIO read strobe
    IOPinId_E_ceva_iop_add,      //ioport IO port MMIO address bus
    IOPinId_E_ceva_iop_dout,     //ioport IO port MMIO data bus

    //DMSS outputs - External Memory
    IOPinId_E_ceva_md_ep_pdout,  //dmss	External port dataout bus
    IOPinId_E_ceva_md_ep_padd,   //dmss	External port address bus
    IOPinId_E_ceva_md_ep_pmode,  //dmss	External port mode
    IOPinId_E_ceva_md_ep_pbsize, //dmss	External port burst size
    IOPinId_E_ceva_md_ep_pwidth, //dmss	External port width
    IOPinId_E_ceva_md_ep_prw,    //dmss	External port read/write indication

    //DMSS outputs - External Transfer Port
    IOPinId_E_ceva_md_etp_pdout, //dmss	External Transfer Port dataout bus
    IOPinId_E_ceva_md_etp_dready,//dmss	External Transfer Port data ready indication

    //DMSS outputs - DMA debug
    IOPinId_E_ceva_md_dma_dbg_match, //dmss	DMA debug match indication

    //OCEM outputs
    IOPinId_E_ceva_tdo,              //ocem JTAG TDO signal
    IOPinId_E_ceva_tdo_oen,          //ocem	JTAG TDO output enable signal
    IOPinId_E_ceva_rtck,             //logic Return Test Clock
    IOPinId_E_ceva_jtag_op,          //ocem	JTAG standard instructions opcode
    IOPinId_E_ceva_jtag_state,       //ocem	JTAG state
    IOPinId_E_ceva_gp_out,           //ocem	OCEM general purpose outputs
    IOPinId_E_ceva_ousr_scan1_sel,   //ocem	User scan chain #1 select signal
    IOPinId_E_ceva_ousr_scan2_sel,   //ocem	User scan chain #2 select signal
    IOPinId_E_ceva_ousr_scan3_sel,   //ocem	User scan chain #3 select signal
    IOPinId_E_ceva_ousr_scan4_sel,   //ocem	User scan chain #4 select signal
    IOPinId_E_ceva_oext1_ack,        //ocem	Acknowledge signal for the external breakpoint request #1
    IOPinId_E_ceva_oext2_ack,        //ocem	Acknowledge signal for the external breakpoint request #2

    //TCB outputs
    IOPinId_E_ceva_tcb_tdo,          //tcb TCB dataout
    IOPinId_E_NumOfInputpins

} T_IOPinId;

//----------------------------------------------
#elif (DSP_TYPE == TKL3)
//----------------------------------------------

// Interrupts vector addresses:
//--------------------------------------------------------
#define Core_K_InterruptVectorAddress_Reset     0x00000000
#define Core_K_InterruptVectorAddress_TrapeBpi  0x00000002
#define Core_K_InterruptVectorAddress_Nmi       0x00000004
#define Core_K_InterruptVectorAddress_Int0      0x00000006
#define Core_K_InterruptVectorAddress_Int1      0x0000000E
#define Core_K_InterruptVectorAddress_Int2      0x00000016
#define Core_K_InterruptVectorAddress_Int3      0x00000020
#define Core_K_InterruptVectorAddress_Crcall    0x00000040


//----------------------------------------------
//	Registers
//----------------------------------------------
// We use the Internal Register list in "reglist.h" in order to create both the
// Registers' Enumerator and strings array (for debug purposes).

// Creating the Register Enumerator: will contain all resource (register) names in TKL3
// The Enumerator format is: InternalReg_E_<RegName>
#define M_RegList(RegId)  RegId_E_##RegId,

typedef enum T_RegId_tag
{	
	#include "reglist.h"		
	
}T_RegId;

#undef M_RegList

// Creating the Register string array:
// The string format is: "InternalReg_<RegName>"
#define M_RegList(RegId) "Reg_"#RegId,

static char *RegStringArr[RegId_E_Size + 1] = 
{
	#include "reglist.h"
};

#undef M_RegList


typedef enum T_RegPartId_tag
{
    RegPartId_E_R0L,
    RegPartId_E_R1L,
    RegPartId_E_R2L,
    RegPartId_E_R3L,
    RegPartId_E_R4L,
    RegPartId_E_R5L,
    RegPartId_E_R6L,
    RegPartId_E_R7L,
	RegPartId_E_R0H,
	RegPartId_E_R1H,
	RegPartId_E_R2H,
	RegPartId_E_R3H,
	RegPartId_E_R4H,
	RegPartId_E_R5H,
	RegPartId_E_R6H,
    RegPartId_E_R7H,

    // rb is a ghost of r6
    RegPartId_E_RBL,
    RegPartId_E_RBH,

    RegPartId_E_SPL,
    RegPartId_E_SPH,

    // cfgi is a ghost of mcfgi
    RegPartId_E_CFGI,
    RegPartId_E_STEPI7BITS,
    RegPartId_E_MODI9BITS,
    RegPartId_E_MCFGI,
    RegPartId_E_STEPI,
    RegPartId_E_MODUI,

    // cfgj is a ghost of mcfgj
    RegPartId_E_CFGJ,
    RegPartId_E_STEPJ7BITS,
    RegPartId_E_MODJ9BITS,
    RegPartId_E_MCFGJ,
    RegPartId_E_STEPJ,
    RegPartId_E_MODUJ,

    RegPartId_E_STEPK,
    RegPartId_E_DSEG,
    RegPartId_E_DVM,

    RegPartId_E_MIXPL,
    RegPartId_E_MIXPH,

    //general registers
    RegPartId_E_EXT0,
    RegPartId_E_EXT1,
    RegPartId_E_EXT2,
    RegPartId_E_EXT3,

    // sts0 and all its flags
    RegPartId_E_STS0,
    RegPartId_E_FLAGS,
    RegPartId_E_R,
    RegPartId_E_L,
    RegPartId_E_E,
    RegPartId_E_C,
    RegPartId_E_V,
    RegPartId_E_N,
    RegPartId_E_M,
    RegPartId_E_Z,
    RegPartId_E_BCX,
    RegPartId_E_LP,
    RegPartId_E_S,
    RegPartId_E_PE0,
    RegPartId_E_PE1,
    RegPartId_E_IP0,
    RegPartId_E_IP1,
    RegPartId_E_IP2,
    RegPartId_E_IP3,
    RegPartId_E_IPV,

    // mod0 and its members
    RegPartId_E_MOD0,
    RegPartId_E_DATASEG,
    RegPartId_E_PAGE,

    // mod1 and its members
    RegPartId_E_MOD1,
	RegPartId_E_IMASK,
	RegPartId_E_IM0,
    RegPartId_E_IM1,
    RegPartId_E_IM2,
    RegPartId_E_IM3,
    RegPartId_E_IMV,
    RegPartId_E_IC0,
    RegPartId_E_IC1,
    RegPartId_E_IC2,
    RegPartId_E_IC3,
    RegPartId_E_NMIC,
    RegPartId_E_UI0,
    RegPartId_E_UI1,
    RegPartId_E_UO0,
    RegPartId_E_UO1,
    RegPartId_E_IE,
    RegPartId_E_MODUEN,
    RegPartId_E_MODUEN0,
    RegPartId_E_MODUEN1,
    RegPartId_E_MODUEN2,
    RegPartId_E_MODUEN3,
    RegPartId_E_MODUEN4,
    RegPartId_E_MODUEN5,
    RegPartId_E_MODUEN6,
    RegPartId_E_MODUEN7,
    RegPartId_E_BITREV,
    RegPartId_E_BITREV0,
    RegPartId_E_BITREV1,
    RegPartId_E_BITREV2,
    RegPartId_E_BITREV3,
    RegPartId_E_BITREV4,
    RegPartId_E_BITREV5,
    RegPartId_E_BITREV6,
    RegPartId_E_BITREV7,

    // mod2 and its members
    RegPartId_E_MOD2,
    RegPartId_E_ETSI,
	RegPartId_E_SATMOD,
    RegPartId_E_SAT,
    RegPartId_E_SATA,
    //  Maor Avni [22/1/2007]: Programming model change:
    //RegPartId_E_satw,
    RegPartId_E_SATP,
	RegPartId_E_PS32,
    RegPartId_E_PS,
    RegPartId_E_PR0,
    RegPartId_E_PR1,
    RegPartId_E_PR2,

    // mod3 and its members
    RegPartId_E_MOD3,
    RegPartId_E_CPC,
	RegPartId_E_TKB,

    // The following are ghosts
    RegPartId_E_ST0,
    RegPartId_E_ST1,
    RegPartId_E_ST2,
    RegPartId_E_MODUEN6BITS,
    RegPartId_E_ICR,

    // pcu registers
    RegPartId_E_PC,
    RegPartId_E_PCL,
    RegPartId_E_PCH,
    RegPartId_E_REPC,
    
    RegPartId_E_LC1,
    RegPartId_E_LC2,
    RegPartId_E_LC3,
    RegPartId_E_LC4,
    
    RegPartId_E_LC,    //!< This is a dummy register: it points to one of the 4 lc, 
                      //!< pointed to by bcx
    RegPartId_E_RETREG,

    // cbu registers
    RegPartId_E_X0,
    RegPartId_E_X1,
    RegPartId_E_Y0,
    RegPartId_E_Y1,
    RegPartId_E_P0L,
	RegPartId_E_P0H,
    RegPartId_E_A0L,
	RegPartId_E_A0H,
	RegPartId_E_A0E,
	RegPartId_E_A1L,
	RegPartId_E_A1H,
	RegPartId_E_A1E,
	RegPartId_E_B0L,
	RegPartId_E_B0H,
	RegPartId_E_B0E,
	RegPartId_E_B1L,
	RegPartId_E_B1H,
	RegPartId_E_B1E,
	RegPartId_E_Size = 146
}T_RegPartId;

typedef T_RegId T_ShortRegID;

typedef T_RegId T_LongRegID;

typedef DWORD T_ExtReg;

typedef enum T_FlagID_tag
{
	FlagId_E_DSEG,	
    FlagId_E_PAGE,
    FlagId_E_IPEND,
    FlagId_E_IU,
    FlagId_E_OU,
    FlagId_E_ICNTX,
    FlagId_E_IMASK,
    FlagId_E_IE,
    FlagId_E_PRX,
    FlagId_E_PS,
    FlagId_E_PS32,
    FlagId_E_PE,
	FlagId_E_ESTI,
    FlagId_E_SATMOD,
    FlagId_E_TLB,
    FlagId_E_MODUEN,
	FlagId_E_LP,
    FlagId_E_BCX,
    FlagId_E_FLAGS,
    FlagId_E_A0E,
    FlagId_E_Z,
    FlagId_E_M,
    FlagId_E_N,
    FlagId_E_V,
    FlagId_E_C,
    FlagId_E_E,
    FlagId_E_L,
    FlagId_E_R,
    FlagId_E_IM1,
    FlagId_E_IM0,
    FlagId_E_SAT,
    FlagId_E_A1E,
    FlagId_E_IP1,
    FlagId_E_IP0,
    FlagId_E_IP2,
    FlagId_E_IU1,
    FlagId_E_IU0,
    FlagId_E_OU1,
    FlagId_E_OU0,
    FlagId_E_S,
    FlagId_E_IM2,
    FlagId_E_MODUEN5,
    FlagId_E_BC2,
    FlagId_E_BC1,
    FlagId_E_BC0,
    FlagId_E_IC2,
    FlagId_E_IC1,
    FlagId_E_IC0,
    FlagId_E_NMIC,
    FlagId_E_MODI,
    FlagId_E_STEPI,
    FlagId_E_MODJ,
    FlagId_E_STEPJ,
	FlagId_E_Size = 100
} T_FlagId;

// Registers types:
//=================
typedef DWORD T_Reg;

//! enum for the various decode modes available in the simulator:
enum E_DecodeMode {
    DecodeMode_E_OneShot, //!< The simulator will decode the entire code memory, and every
                          //!< A code change is detected.
    DecodeMode_E_EachInstruction //!< The simulator will decode only the next opcode each cycle.
};

enum E_BubbleCacheMode {
    BubbleCacheMode_E_On, //!< The bubble cache will be turned on.
    BubbleCacheMode_E_Off, //!< The bubble cache will be turned off. Every cycle the simulator will consult
                      //!< The restriction checker to check for bubbles.
    BubbleCacheMode_E_Test //!< Each cycle both modes will be checked and compared for testing purposes.

};

#define M_RegId(reg)   RegId_E_##reg

//----------------------------------------------
#else
//----------------------------------------------

#error Unknown DSPCore type

//----------------------------------------------
#endif
//----------------------------------------------


#endif /* REGISTERS_DEFINITIONS_H_ */
/*================================== E N D ==================================*/
