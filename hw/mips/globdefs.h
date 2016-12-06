/*****************************************************************************\
*                                                                             *
* Copyright (C) CEVA Inc. All rights reserved.                                *
*                                                                             *
* Project: Debugger                                                           *
*                                                                             *
* File Name: GLOBDEFS.H                                                       *
*                                                                             *
* Written by: Menachem Stern                                                  *
* Date:       17/4/2001                                                       *
*                                                                             *
* Purpose: Defines global definitions of the Debugger project(s).             *
*                                                                             *
\*****************************************************************************/

#ifndef GLOBAL_DEFINITIONS_H_
#define GLOBAL_DEFINITIONS_H_


//====================
// Symbol Definitions:
//====================


// TRUE/FALSE Definition:
//-----------------------------------------------------------------------------
#undef  TRUE
#undef  FALSE
#define TRUE     1
#define FALSE    0

// number of words in one K
#define Global_K_SizeOfOneK   0x400


// NULL Definition:
//-----------------------------------------------------------------------------
#undef  NULL
#define NULL     0


// FAILURE/SUCCESS Definition:
//-----------------------------------------------------------------------------
#undef  SUCCESS
#undef  FAILURE
#define SUCCESS  TRUE
#define FAILURE  FALSE


// OFF/ON Definition:
//-----------------------------------------------------------------------------
#undef  ON
#undef  OFF
#define ON       TRUE
#define OFF      FALSE


// MATCH Definition:
//-----------------------------------------------------------------------------
#undef MATCH
#define MATCH    0  // for strcmp


// Maximal string length definition:
//-----------------------------------------------------------------------------
#define Global_K_MaxStrLength 384


//==================
// Type Definitions:
//==================


// T_Status - Return Code (FAILURE/SUCCESS) Definition:
//-----------------------------------------------------------------------------
typedef int              T_Status;


// Standard Types Definition:
//-----------------------------------------------------------------------------
typedef unsigned short   WORD;
typedef unsigned long    DWORD;
typedef int              BOOL;
typedef unsigned char    BYTE;
typedef signed long      LONG;
typedef signed short     SHORT;

#ifdef INT64
#undef INT64
#endif
#ifdef UINT64
#undef UINT64
#endif

typedef signed short        INT16;
typedef signed int          INT32;

#if defined (NON_UNIX) || defined (MAINWIN) // = PC
#ifdef LINUX_NATIVE
typedef signed long long    INT64;
typedef unsigned long long  UINT64;
#else
typedef signed __int64      INT64;
typedef unsigned __int64    UINT64;
#endif
#else // UNIX
typedef signed long long    INT64;
typedef unsigned long long  UINT64;
#endif // WIN32


// Global Macros:
//-----------------------------------------------------------------------------
// lbu #if _MSC_VER != 1400
//#undef MAKELONG
//#define MAKELONG(a, b) ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
//#endif


// For global variables declaration.
// The owner module should define this symbol.
#ifdef LOCAL_DEFINITION_
#define EXTERN_
#else
#define EXTERN_ extern
#endif


// AEC debug mode
//---------------------------------------------------------------------------
/* _VCP_DEBUG defined if fw compiled with it */
#define _VCP_DEBUG 1

//---------------------------------------------------------------------------
/* _TEMP_DEBUG_TRACE defined if fw compiled with it */
#define _TEMP_DEBUG_TRACE 1

//---------------------------------------------------------------------------
/* GEN_CIPHER_REF defined in order to generate refrence file from 3gpp ref */
//#define GEN_CIPHER_REF


#endif /* GLOBAL_DEFINITIONS_H_ */
/*================================= E N D ===================================*/
