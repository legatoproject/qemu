/*
**  Important : This software is protected by Copyright and the information
**              contained herein is confidential. Distribution, reproduction
**              as well as exploitation and transmission of its contents is
**              not allowed except if expressly permitted. Infringments
**              result in damage claims.
**              Copyright Wavecom S.A. (c) 1998, 1999, 2000, 2001
*/
/****************************************************************************/
/*  File    : comdefs.hc                                                    */
/*--------------------------------------------------------------------------*/
/*  Scope   : GSM project common define flags file                          */
/*                                                                          */
/*  Contents:                                                               */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*  Historical :                                                            */
/*--------------------------------------------------------------------------*/
/*   Date   | Author | Modification                                         */
/*----------+--------+------------------------------------------------------*/
/* 17.04.98 |   PGU  | - Creation                                           */
/*----------+--------+------------------------------------------------------*/
/* 27.05.98 |   PGU  | - LCD_EMULATION and not __LCD_EMULATION__            */
/*----------+--------+------------------------------------------------------*/
/* 04.08.98 |   PGU  | - Add __NEW_SYNTHE... and __USE_HW_WAKE...           */
/*----------+--------+------------------------------------------------------*/
/* 15.04.99 |   ADA  | - Radio 2c management.                               */
/*----------+--------+------------------------------------------------------*/
/* 15.04.99 |   EDA  | - common flags                                       */
/****************************************************************************/

#ifndef __COMDEFS_HC__
#define __COMDEFS_HC__

/* ----------------- common flags ------------------ */
#define SIMU_SC
#define SIMU_SP
#define SIMU_RR
#define SIMU_DL
#define __HWL_DEBUG__

#ifdef __VISUALC__
#undef __DEBUG__
#endif
#define __DEBUG__

#undef  DL_ON
#undef  __TARGET__

#define __PHASE2__
#define __MULTIBAND__
#define __WISMO_2C__
#define __AUTOBAUD__
#define __DEBUG__
//#define __SLOW_CLOCK__
//#define __SLEEP_MODE__
#ifndef __TARGET__
#define _PENTIUM_
#define SIMU_PC
#endif

#if defined(__HWL_DEBUG__)
#define __DBG1_TIMER__
#define __DBG2_TIMER__
#define __DBG3_TIMER__
#define __DBG4_TIMER__
#define __DBG5_TIMER__
#define __DBG6_TIMER__
#define __DBG7_TIMER__
#define __DBG8_TIMER__
#define __DBG9_TIMER__
#define __DBG1_SYNTHE__
#define __DBG1_ALC__
#define __DBG1_DL__
#undef  __DOWNLOAD_TI__
#endif

/* ----------------- radio dependent flags ------------------ */
#ifdef __WISMO_2C__
#define __RADIO_2C__
#endif

/* ----------------- WISMO dependent flags ------------------ */

#define __KERNEL32__
#define __KERNEL32A__
#define __KERNEL4__
#define __KERNEL5__
#define __ONE_C__

#define __E2P_NM24C16__
#define __ALL_PATCHES__
#define __LARGER_RXEN2__
#define __UFO__

/* ----------------- miscellaneous flags ------------------ */

#if defined(__DEBUG__)
#define __ZERO_MEMORY__
#define __DBG_POOL__
#define __DBG_SCR__
#define __WATCH__
#define __DIAGNOSE__
#define __RTK_DIAGNOSE__
#define __REMOTE_RTK__
#define __CHECK_ACTIVITY__
#define __L1C_WATCH__
#define __HWL_WATCH__
#define __BATT_WATCH__
#define __L1C_CNF__
#define __SPV_DEBUG__
#define LCD_EMULATION
#endif

#if !defined(__NO_POWER_DOWN__)
#define __HWL_PWDN__
#define __HWL_VAR_PWDN__
#define __AFC_PWDN__
#define __SLEEP_MODE__
#define __SLEEP_ARM__
#endif

/* ----------- mandatory flag (to be removed from code) ------------ */

#define __CBCH__
#define __TEST_TRANSMITTER__
#define __FAST_SCAN__
#define __L1C_USE_DATA_DTX__

#endif /* __COMDEFS_HC__ */


