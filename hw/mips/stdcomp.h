/*
**  Important : This software is protected by Copyright and the information
**              contained herein is confidential. Distribution, reproduction
**              as well as exploitation and transmission of its contents is
**              not allowed except if expressly permitted. Infringments
**              result in damage claims.
**              Copyright VMTS S.A.S (c) 2005
*/
/****************************************************************************/
/*  File    : stdcomp.h                                                     */
/*--------------------------------------------------------------------------*/
/*  Scope   : GSM project global header file                                */
/*                                                                          */
/*  Contents:                                                               */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*  Historical :                                                            */
/*--------------------------------------------------------------------------*/
/*   Date   | Author | Err  |  Modification                                 */
/*----------+--------+------+-----------------------------------------------*/
/* 07.11.94 |   YFE  |      |  Creation                                     */
/*----------+--------+------+-----------------------------------------------*/
/* 10.09.96 |   RDO  |      | macro CAT was not correct for arm, fix it.    */
/*----------+--------+------+-----------------------------------------------*/
/* 24.11.96 |   PGU  |      | GLOB_DATA for Borland C compiler              */
/*----------+--------+------+-----------------------------------------------*/
/* 14.05.97 |   PGU  |      | huge definition for arm compiler              */
/*----------+--------+------+-----------------------------------------------*/
/* 17.04.98 |   PGU  |      | comdefs.hc is used for defines                */
/****************************************************************************/

#ifndef __STDCOMP_H__
#define __STDCOMP_H__

#include "comdefs.hc"

/*
** =========================================================================
**  Portable types
** =========================================================================
*/

#if defined (__BORLANDC__) || defined (__VISUALC__)

#ifdef __PRODUCT_EXE__
#include "bcdefs.h"
#endif

#ifndef __cplusplus
typedef unsigned char  bool;
#endif

typedef unsigned char  u8;
#ifdef __VISUALC__
typedef unsigned short   u16;	// Bug in return of RTK2_GET_DS (32 bits instead of 16 bits)
#else
typedef unsigned int   u16;
#endif
typedef unsigned long int Primitive;
typedef unsigned long  u32;
typedef signed   char  ascii;
typedef signed   char  s8;
#ifdef __VISUALC__
typedef signed   short   s16;
#else
typedef signed   int   s16;
#endif
typedef signed   long  s32;
#define huge

#elif (defined unix)

#ifndef __cplusplus
// lbu typedef unsigned char  bool;
#endif

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    Primitive;
typedef unsigned int    u32;
typedef          char   ascii;
typedef          char   s8;
typedef          short  s16;
typedef          int    s32;

#elif (defined _MCC68K)

typedef unsigned int    Primitive;
typedef unsigned char   bool;
typedef unsigned char   u8;
typedef signed   char   s8;
typedef signed   char   ascii;
typedef unsigned short  u16;
typedef          short  s16;
typedef unsigned int    u32;
typedef          int    s32;
typedef          double u64;

#elif (defined __arm) || defined(__GNUC__)

typedef unsigned int    Primitive;
typedef unsigned char   bool;
typedef unsigned char   u8;
typedef signed   char   s8;
typedef          char   ascii;
typedef unsigned short  u16;
typedef          short  s16;
typedef unsigned int    u32;
typedef          int    s32;
typedef          double u64;

#else

/* ERROR */

#endif

/*
** =========================================================================
**  Memory alignment tools
** =========================================================================
*/

typedef struct __alg { char c; void * p; } __alg_t;
#define ALIGNMENT               (offsetof(__alg_t, p) - 1)
#define ALIGN(P)                ( (u32) ((char *)(P) + ALIGNMENT) & ~ALIGNMENT)

/*
** =========================================================================
**  ARM Specific Tools
** =========================================================================
*/

#if !defined(__STDC__) && !defined(__BORLANDC__) && !defined (__VISUALC__)
#define GLOB_DATA_TOP(_Mod_)   typedef struct _Mod_/**/_/**/_Data {
#define GLOB_DATA(_Typ_,_Nam_) _Typ_  _/**/_Nam_
#ifdef __arm
#ifdef __GLOBAL_REG__
#define ACCESS_TO(_Mod_,_Nam_)   _Mod_/**/_r6->_/**/_Nam_
#define GLOB_DATA_END(_Mod_)   } _Mod_/**/_Global_t; \
			       EXT _Mod_/**/_Global_t _Mod_/**/_Global;\
			       _Mod_/**/_Global_t __global_reg(3) *_Mod_/**/_r6
#else
#define ACCESS_TO(_Mod_,_Nam_) _Mod_/**/_Global._/**/_Nam_
#define GLOB_DATA_END(_Mod_)   } _Mod_/**/_Global_t; \
			       EXT _Mod_/**/_Global_t _Mod_/**/_Global;\
			       EXT _Mod_/**/_Global_t * _Mod_/**/_r6
#endif
#else
#define ACCESS_TO(_Mod_,_Nam_) _Mod_/**/_Global._/**/_Nam_
#define GLOB_DATA_END(_Mod_)   } _Mod_/**/_Global_t; \
			       EXT _Mod_/**/_Global_t _Mod_/**/_Global;\
			       EXT _Mod_/**/_Global_t * _Mod_/**/_r6
#endif

#else    /* __STDC__ || __BORLANDC__ || __VISUALC__*/

#define GLOB_DATA_TOP(_Mod_)   typedef struct _Mod_##_##_Data {
#define GLOB_DATA(_Typ_,_Nam_) _Typ_  _##_Nam_
#ifdef __arm
#ifdef __GLOBAL_REG__
#define ACCESS_TO(_Mod_,_Nam_) ((_Mod_##_Global_t *) _Mod_##_r6)->_##_Nam_
#define GLOB_DATA_END(_Mod_)   } _Mod_##_Global_t; \
			       EXT _Mod_##_Global_t _Mod_##_Global;\
			       _Mod_##_Global_t global_reg(3) *_Mod_##_r6
#else
#define ACCESS_TO(_Mod_,_Nam_) _Mod_##_r6._##_Nam_
#define GLOB_DATA_END(_Mod_)   } _Mod_##_Global_t; \
			       EXT _Mod_##_Global_t _Mod_##_Global;\
			       EXT _Mod_##_Global_t * _Mod_##_r6
#endif
#else
//#define ACCESS_TO(_Mod_,_Nam_) _Mod_##_r6->_##_Nam_
#define ACCESS_TO(_Mod_,_Nam_) _Mod_##_Global._##_Nam_
#define GLOB_DATA_END(_Mod_)   } _Mod_##_Global_t; \
			       EXT _Mod_##_Global_t _Mod_##_Global;\
			       EXT _Mod_##_Global_t  * _Mod_##_r6
#endif

#endif


/*
** =========================================================================
**  Basic goodies
** =========================================================================
*/

//lbu #define FALSE 0
//#define TRUE  1

/*
** =========================================================================
**  ANSI / NON ANSI compatibility
** =========================================================================
*/

#if defined __arm  || defined(__GNUC__)
#define CAT(X,Y)                X/**/Y
#else
#define CAT(X,Y)                X##Y
#endif

#if defined(__STDC__) || defined(__BORLANDC__) || defined(__arm) || defined (__VISUALC__)  || defined(__GNUC__)

#ifdef __VISUALC__
#undef offsetof
#endif
// lbu #define offsetof(S,F)           (unsigned) (&(((S *) 0)->F))
#ifndef NULL
#define NULL                    ((void *) 0)
#endif
#define USE_PROTOTYPES
#define VOLATILE                volatile
#define CONST                   const
#define VOID                    void
#define _PROTO(X)               X
#define TOKEN                   CAT(CAT(y,f),__LINE__)
#ifdef INLINING
#define MEMCPY(Dst,Src,Len)     ( *((struct TOKEN {char xx[Len];} *) Dst) =\
				  *((struct TOKEN *) Src), Dst )
#endif

#else /* not ANSI */

#ifndef NULL
#define NULL                    ((char *) 0)
#endif
#define signed
#define VOLATILE
#define volatile
#define CONST
#define const
#define VOID
#define _PROTO(X)               ( )
#define offsetof(S,F)           (unsigned) (&(((S *) 0)->F))
#endif

/*
** =========================================================================
**  IBM-PC extended keywords (with Microsoft C compiler V4.0 or above)
** =========================================================================
*/
//
//#ifdef __MSDOS__
//
//#define FAR  _far
//#define NEAR _near
//#define HUGE huge
//
//#else
//
//#ifdef __VISUALC__
//#undef FAR
//#undef NEAR
//#endif
//#define FAR
//#define far
//#define NEAR
//#define near
//#define HUGE
//
//
//#endif /*__MSDOS__*/
//

/*
** =========================================================================
**  Various defines
** =========================================================================
*/

#define PNUL     ((void *)0)            /* NULL pointer                     */
#define PFNUL    ((void far *)0)        /* NULL far pointer                 */
#define PNIL     ((void *)-1)           /* Negative pointer                 */
#define PFNIL    ((void far *)-1)       /* Negative far pointer             */
#define PFCNUL   ((void (*)())0)        /* NULL function pointer            */
#define PFCNIL   ((void (*)())-1)       /* Negative function pointer        */
#define PFCFNUL  ((void (far *)())0)    /* NULL far function pointer        */
#define PFCFNIL  ((void (far *)())-1)   /* Negative far function pointer    */

/*
** =========================================================================
**  RTK unix only
** =========================================================================
*/

#ifdef unix

#define REVERSE_BIT_FIELDS
#define GO     static int _ini_=0;\
	       if (!_ini_) _ini_++; else goto NEXT
#define END    if (_ini_) return

#else

#define GO
#define END

#endif  /* unix */


#endif /* __STDCOMP_H__ */


