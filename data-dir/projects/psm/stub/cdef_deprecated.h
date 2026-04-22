/***************************************************************************
*=====================      Copyright by Continental AG      ===============
****************************************************************************
* Title        : cdef_deprecated.h
*
*  Description: Deprecated symbols from cdef, should not be used, from
*                  both cdefs before CDS MR 8954/8955.
*                  Need to be defined/included after the endianess symbols
*                  because they are used.
*
* Environment  : Any
*               (but the CDEF component in total is
*                either little or big endian)
*
* Responsible  : Jan Rademacher, I ID RD SW OA O, aka OSP
*
* Guidelines   : SMK
*
* Template name: modified SWMODxCE.H, Revision 1.0
*
* CASE-Tool    : None
****************************************************************************/

/*********************************************************************************************
 * REQIRED PROJECT LEVEL MACRO DEFINITIONS (TARGET SPECIFIC): see CDEF_chips.txt
 *********************************************************************************************/



#ifndef   CDEF_DEPRECATED_H
#define   CDEF_DEPRECATED_H


/* In case stdint.h is included AND some of the deprecated symbols are defined by CDEF
 * the problem of "inconsistent redefinition of <symbol>" occurs.
 * Only in order to avoid this, CDEF can perform the deprecated include of std_int.h.
 * Anybody who wants to use symbols defined by stdint.h has to include it.
 * If that is done BEFORE cdef is included first time, the following deprecated
 * include inside CDEF is not needed.
 * That is why there is a seperate dedicated control switch for the deprecated include,
 * because needing the corresponding deprecated symbols (e.g. UINT8_MAX, INT16_MIN, ...)
 * does NOT always mean that the include is also needed.
 * Note: The definition of the corresponding deprecated symbols the stdint.h way tends
 *       to cause more QAC noise, even in the client code, when using the symbols.
 */
#if defined     CDEF_DEPRECATED_INCLUDESTDINT
#include <stdint.h>
/* For avoiding inconsitent redefinition.
 * NOT for extending the CDEF API.
 */
#endif /*       CDEF_DEPRECATED_INCLUDESTDINT      */


/* ___________________________________________________________________________________________
 * ___________________________________________________________________________________________
 *                      keep compatibility with existing sources
 *                      Do not use these symbols anymore. They are deprecated and might be
 *                      changed without notice.
 */

#if defined    CDEF_DEPRECATED_NONPREFIXUTYPES
typedef CDEF_tUint8  uint8;
typedef CDEF_tUint16 uint16;
typedef CDEF_tUint32 uint32;
# ifndef   _CPU_16BIT_
typedef CDEF_tUint64 uint64;
# endif /* _CPU_16BIT_ */
#endif /*      CDEF_DEPRECATED_NONPREFIXUTYPES    */

#if defined    CDEF_DEPRECATED_NONPREFIXSTYPES
typedef CDEF_tInt8  int8;
typedef CDEF_tInt16 int16;
typedef CDEF_tInt32 int32;
# ifndef   _CPU_16BIT_
typedef CDEF_tInt64 int64;
# endif /* _CPU_16BIT_ */
#endif /*      CDEF_DEPRECATED_NONPREFIXSTYPES    */

#if defined    CDEF_DEPRECATED_NONPREFIXBITTYPES
typedef CDEF_tu8BitField  tu8BitByte;
typedef CDEF_tu16BitField tu16BitWord;
typedef CDEF_tu32BitField tu32BitDWord;
# ifndef   _CPU_16BIT_
typedef CDEF_tu64BitField tu64BitLLWord;
# endif /* _CPU_16BIT_ */
#endif /*      CDEF_DEPRECATED_NONPREFIXBITTYPES  */

#if defined    CDEF_DEPRECATED_NONPREFIXBITFIELDS
typedef CDEF_tstBit8  bit8;
typedef CDEF_tstBit16 bit16;
typedef CDEF_tstBit32 bit32;
# ifndef   _CPU_16BIT_
typedef CDEF_tstBit64 bit64;
# endif /* _CPU_16BIT_ */
#endif /*      CDEF_DEPRECATED_NONPREFIXBITFIELDS */

#if defined    CDEF_DEPRECATED_NONPREFIXRECORDS
typedef CDEF_tunU16Rec tunU16Rec;
typedef CDEF_tunU32Rec tunU32Rec;
# ifndef   _CPU_16BIT_
typedef CDEF_tunU64Rec tunU64Rec;
# endif /* _CPU_16BIT_ */
#endif /*      CDEF_DEPRECATED_NONPREFIXRECORDS   */


#if defined    CDEF_DEPRECATED_NONPREFIXFP
typedef CDEF_tFuncPtr tFuncPtr;
#endif /*      CDEF_DEPRECATED_NONPREFIXFP        */

#if defined    CDEF_DEPRECATED_NONPREFIXYESNO
#define Yes CDEF_Yes
#define  No CDEF_No
#endif /*      CDEF_DEPRECATED_NONPREFIXYESNO     */

#if defined    CDEF_DEPRECATED_NONPREFIXONOFF
#define  On CDEF_On
#define Off CDEF_Off
#endif /*      CDEF_DEPRECATED_NONPREFIXONOFF     */

#if defined    CDEF_DEPRECATED_NONPREFIXOKNOK
#define    Ok CDEF_Ok
#define NotOk CDEF_NotOk
#endif /*      CDEF_DEPRECATED_NONPREFIXOKNOK     */

#if defined    CDEF_DEPRECATED_NONPREFIXTRUEFALSE
#define  True CDEF_True
#define False CDEF_False
#endif /*      CDEF_DEPRECATED_NONPREFIXTRUEFALSE */



#if defined    CDEF_DEPRECATED_NONPREFIXBOOLTYPE
#ifndef __cplusplus                        /* use Std ANSI C++ macro to detect C++ compilation  */
  typedef CDEF_tBool bool;
#endif
#endif /*      CDEF_DEPRECATED_NONPREFIXBOOLTYPE  */




#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_OFF 271, 280, 1251, 1252, 1253, 1254, 1255, 2860, 2940, 4131
/*	
QAC: 2860; 2017-02-15, Sabine Neumann-EXT: "Harmless", see comment in cdef_common.h
QAC: 2940; 2017-02-15, Sabine Neumann-EXT: "Harmless", see comment in cdef_common.h
271, 1251, 1252, 1253, 1254, 1255, 4131	:
				The following static assertions verify the correctness of value implementations 
				defined by cdef in an SMK-compliant way. They do so by comparing to values from 
				limits.h and similar files, which are not necessarily SMK-compliant. That means 
				using those values causes warnings which cannot be avoided, but using them cannot 
				be avoided here either. This kind of warnings is silenced here, while the same 
				warnings are AVOIDED in the clients of cdef.

280	:	
				The warning states that nobody should assume the possibility to represent 
				e.g. -128 in a signed 8bit int, even if it is possible in most compilers.
				That is because this assumption would imply the assumption that the compiler
				uses the 2-compliment representation of negative values, which is compiler 
				implementation specific. In this special case (and only here), the warning 
				is caused by exactly that line inside limits.h or similar, which explicitly 
				states THAT the compiler CAN represent this value in the signed int type in 
				question. Note that it is still not a good idea to actually use this value, 
				independent of how it is defined.
*/
#endif /* QAC_Analysis */


/* Define the deprecated min/max-values without CDEF_ prefix in one of two ways,
 * depending on which input symbols are available (from stdint.h or the like).
 * If [U]INT<nn>_(MIN|MAX) is available use it, otherwise use an alternative,
 * which in that case is assumed to be available.
 * The definitions in stdint.h (e.g. UINT8_MAX) and limits.h (UCHAR_MAX) differ
 * in the way they define the limit, but not in the value itself.
 * The way these symbols are defined is often not understandable for the
 * pre-processor, so in case both are present and in order to verify the amount
 * to the same, static assertions are used.
 */

#if defined    CDEF_DEPRECATED_NONPREFIXLIMITU8
# ifndef                UINT8_MAX
#  define               UINT8_MAX    CDEF_nu8Max       /* Maximum uint8  value  */
# endif /* ndef         UINT8_MAX */
  CDEF__STATIC_ASSERT(( UINT8_MAX == UCHAR_MAX ), stUINT8_MAX_CHECK);
#endif /*      CDEF_DEPRECATED_NONPREFIXLIMITU8   */

#if defined    CDEF_DEPRECATED_NONPREFIXLIMITU16
# ifndef                UINT16_MAX
#  define               UINT16_MAX   CDEF_nu16Max       /* Maximum uint16 value */
# endif /* ndef         UINT16_MAX */
  CDEF__STATIC_ASSERT(( UINT16_MAX == USHRT_MAX ), stUINT16_MAX_CHECK);
#if                                   USHRT_MAX != 0xFFFFU
#error __FILE__ : unsigned short range is not 0 .. 0xFFFF. Check your <limits.h>
#endif
#endif /*      CDEF_DEPRECATED_NONPREFIXLIMITU16  */

#if defined    CDEF_DEPRECATED_NONPREFIXLIMITU32
# ifndef                UINT32_MAX
#  define               UINT32_MAX   CDEF_nu32Max       /* Maximum uint32 value */
# endif /* ndef         UINT32_MAX */
  CDEF__STATIC_ASSERT(( UINT32_MAX == ULONG_MAX ), stUINT32_MAX_CHECK);
#if                                   ULONG_MAX != 0xFFFFFFFFUL
#error __FILE__ : unsigned long range is not 0 .. 0xFFFFFFFF. Check your <limits.h>
#endif
#endif /*      CDEF_DEPRECATED_NONPREFIXLIMITU32  */

#if !defined _CPU_16BIT_
# if defined   CDEF_DEPRECATED_NONPREFIXLIMITU64
#  ifndef                UINT64_MAX
#   define               UINT64_MAX   ULLONG_MAX      /* Maximum uint64 value */
#  endif /* ndef         UINT64_MAX */
   CDEF__STATIC_ASSERT(( UINT64_MAX == ULLONG_MAX), stUINT64_MAX_CHECK);
# endif /*     CDEF_DEPRECATED_NONPREFIXLIMITU64  */

# if defined   CDEF_DEPRECATED_INTERNALLIMITU64
#  if !defined           ULLONG_MAX
#   define               ULLONG_MAX          CDEF_nu64Max
#  endif /* ndef         ULLONG_MAX */
#  if                    ULLONG_MAX != 0xFFFFFFFFFFFFFFFFULL
#   error __FILE__ : unsigned long long range is not 0 .. 0xFFFFFFFFFFFFFFFF. Check your <limits.h>
#  endif /* error */
# endif /*     CDEF_DEPRECATED_INTERNALLIMITU64   */
#endif /*    _CPU_16BIT_ */

/* In case of complement-2 representation of negative integers,
 * SCHAR_MIN==((-SCHAR_MAX)-1) applies and is used below because of
 * reduced number and severity of QAC warnings.
 * The assumption might however not be valid, e.g. in complement-1
 * representation, in that case use the symbol from limits.h and
 * QAC can go and whistle.
 */

#if defined    CDEF_DEPRECATED_NONPREFIXLIMITSS8
# if !defined           INT8_MIN
#  define               INT8_MIN CDEF_ni8Min
# endif /* ndef         INT8_MIN */
/* <@QAC: 2860> <RelLineCount: 1> <SWQE: 2017-02-15, Sabine Neumann-EXT> <Comment: Harmless, see comment in cdef_common.h> */
  CDEF__STATIC_ASSERT(( INT8_MIN == SCHAR_MIN ), stINT8_MIN_CHECK);

# if !defined           INT8_MAX
#  define               INT8_MAX CDEF_ni8Max       /* Maximum int8  value   */
# endif /* ndef         INT8_MAX */
  CDEF__STATIC_ASSERT(( INT8_MAX == SCHAR_MAX ), stINT8_MAX_CHECK);
#endif /*      CDEF_DEPRECATED_NONPREFIXLIMITSS8  */
  
#if defined    CDEF_DEPRECATED_NONPREFIXLIMITSS16
# if !defined           INT16_MIN
#  define               INT16_MIN CDEF_ni16Min /* Minimum int16 value  */
# endif /* ndef         INT16_MIN */
/* <@QAC: 2860> <RelLineCount: 1> <SWQE: 2017-02-15, Sabine Neumann-EXT> <Comment: Harmless, see comment in cdef_common.h> */
  CDEF__STATIC_ASSERT(( INT16_MIN == SHRT_MIN  ), stINT16_MIN_CHECK);

# if !defined           INT16_MAX
#  define               INT16_MAX CDEF_ni16Max /* Maximum int16 value  */
# endif /* ndef         INT16_MAX */
  CDEF__STATIC_ASSERT(( INT16_MAX == SHRT_MAX  ), stINT16_MAX_CHECK);
#endif /*      CDEF_DEPRECATED_NONPREFIXLIMITSS16 */

#if defined    CDEF_DEPRECATED_NONPREFIXLIMITSS32
# if !defined           INT32_MIN
#  define               INT32_MIN    LONG_MIN        /* Minimum int32 value  */
# endif /* ndef         INT32_MIN */
/* <@QAC: 2940> <RelLineCount: 1> <SWQE: 2017-02-15, Sabine Neumann-EXT> <Comment: Harmless, see comment in cdef_common.h> */
  CDEF__STATIC_ASSERT(( INT32_MIN == LONG_MIN  ), stINT32_MIN_CHECK);

# if !defined           INT32_MAX
#  define               INT32_MAX    LONG_MAX        /* Maximum int32 value  */
# endif /* ndef         INT32_MAX */
  CDEF__STATIC_ASSERT(( INT32_MAX == LONG_MAX  ), stINT32_MAX_CHECK);
#endif /*      CDEF_DEPRECATED_NONPREFIXLIMITSS32 */

#if !defined _CPU_16BIT_
# if defined   CDEF_DEPRECATED_NONPREFIXLIMITSS64
#  if !defined           INT64_MIN
#   define               INT64_MIN    LLONG_MIN       /* Minimum int64 value  */
#  endif /* ndef         INT64_MIN */
/* <@QAC: 2940> <RelLineCount: 1> <SWQE: 2017-02-15, Sabine Neumann-EXT> <Comment: Harmless, see comment in cdef_common.h> */
   CDEF__STATIC_ASSERT(( INT64_MIN == LLONG_MIN ), stINT64_MIN_CHECK);

#  if !defined           INT64_MAX
#   define               INT64_MAX    LLONG_MAX       /* Maximum int64 value  */
#  endif /* ndef         INT64_MAX */
   CDEF__STATIC_ASSERT(( INT64_MAX == LLONG_MAX ), stINT64_MAX_CHECK);
# endif /*     CDEF_DEPRECATED_NONPREFIXLIMITSS64 */

# if defined   CDEF_DEPRECATED_INTERNALLIMITSS64
#  if !defined           LLONG_MIN
#   define               LLONG_MIN           CDEF_ni64Min
#  endif /* ndef         LLONG_MIN */

#  if !defined           LLONG_MAX
#   define               LLONG_MAX           CDEF_ni64Max
#  endif /* ndef         LLONG_MAX */
# endif /*     CDEF_DEPRECATED_INTERNALLIMITSS64  */
#endif /*    _CPU_16BIT_ */

#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_ON 271, 280, 1251, 1252, 1253, 1254, 1255, 2860, 2940, 4131
#endif /* QAC_Analysis */
  
#if defined    CDEF_DEPRECATED_TYPES
/** deprecated Unsigned 8-bit datatype : 0..255                */
typedef         CDEF_tUint8        byte;
typedef CDEF_tu8BitField       tbBitByte;  /* tbBitByte = (logical) byte within bitfield  */

/** deprecated Unsigned 16-bit datatype: 0..65535              */
typedef CDEF_tUint16             word;
typedef CDEF_tu16BitField twBitWord;     /* tbBitWord = (logical) word within bitfield  */
typedef CDEF_tunU16Rec       tunWordRec;  /* Word data type with single accessable bytes */

/** deprecated Unsigned 32-bit datatype: 0..4294967295         */   
typedef CDEF_tUint32                 dword;
typedef   CDEF_tu32BitField   tdwBitDWord;     /* tbBitWord = (logical) dword within bitfield */
typedef CDEF_tunU32Rec           tunDwordRec;  /* DWord data type with single accessable words & bytes */

/** Bitfield data type with  8 bits                 */   
typedef          CDEF_tstBit8        bitb;

/** Bitfield data type with 16 bits                 */
typedef         CDEF_tstBit16   tBitField;

#define    wW0      u16W0            /*  Low word. Range = 0 .. 65535 */
#define    wW1      u16W1            /* High word. Range = 0 .. 65535 */
#define    bB0       u8B0            /*  Low byte of low  word   = least significant byte */
#define    bB1       u8B1            /* High byte of low  word                            */
#define    bB2       u8B2            /*  Low byte of high word                            */
#define    bB3       u8B3            /* High  byte of high  word = most significant byte  */

#define BYTE_MAX        CDEF_nu8Max      /* Maximum Byte value  */
#define WORD_MAX       CDEF_nu16Max      /* Maximum Word value  */
#define DWORD_MAX      CDEF_nu32Max      /* Maximum Dword value */

#endif /*      CDEF_DEPRECATED_TYPES              */

#ifdef    CDEF_FLOAT_SYNONYMS
/* deprecated synonym adaptations for floating point types */
#define float32 CDEF_tFloat32
#define float64 CDEF_tFloat64
#endif /* CDEF_FLOAT_SYNONYMS */

#if defined    CDEF_DEPRECATED_MEDIUMCONST
# ifdef QAC_Analysis
# pragma PRQA_MESSAGES_OFF 3428
# endif
# define mediumconst const                /* resolved to special const __far    for FJ16     */
# ifdef QAC_Analysis
# pragma PRQA_MESSAGES_ON  3428
# endif
#endif /*      CDEF_DEPRECATED_MEDIUMCONST        */


/* ---------------------------------------------------------------------------------------- */
#if defined    CDEF_DEPRECATED_BITMASKS
/* ___________________________________________________________________________________________
 *                                       single bit constants
 *                                       literals from above (except 16bit) with cast
 *   These symbols (nu<n>Bit<m>) for constants are obsolete.
 *   Use the symbols CDEF_nu<n>Bit<m>_Suf instead, or CDEF_nu<n>Bit<m> 
 */ 
#define nu8Bit0    ((CDEF_tUint8) CDEF_nu8Bit0_Suf)         /* Bitmask for bit 0  */
#define nu8Bit1    ((CDEF_tUint8) CDEF_nu8Bit1_Suf)         /* Bitmask for bit 1  */
#define nu8Bit2    ((CDEF_tUint8) CDEF_nu8Bit2_Suf)         /* Bitmask for bit 2  */
#define nu8Bit3    ((CDEF_tUint8) CDEF_nu8Bit3_Suf)         /* Bitmask for bit 3  */
#define nu8Bit4    ((CDEF_tUint8) CDEF_nu8Bit4_Suf)         /* Bitmask for bit 4  */
#define nu8Bit5    ((CDEF_tUint8) CDEF_nu8Bit5_Suf)         /* Bitmask for bit 5  */
#define nu8Bit6    ((CDEF_tUint8) CDEF_nu8Bit6_Suf)         /* Bitmask for bit 6  */
#define nu8Bit7    ((CDEF_tUint8) CDEF_nu8Bit7_Suf)         /* Bitmask for bit 7  */

#define nu16Bit0   ((CDEF_tUint16)     0x1U)                /* Bitmask for bit 0  */
#define nu16Bit1   ((CDEF_tUint16)     0x2U)                /* Bitmask for bit 1  */
#define nu16Bit2   ((CDEF_tUint16)     0x4U)                /* Bitmask for bit 2  */
#define nu16Bit3   ((CDEF_tUint16)     0x8U)                /* Bitmask for bit 3  */
#define nu16Bit4   ((CDEF_tUint16)    0x10U)                /* Bitmask for bit 4  */
#define nu16Bit5   ((CDEF_tUint16)    0x20U)                /* Bitmask for bit 5  */
#define nu16Bit6   ((CDEF_tUint16)    0x40U)                /* Bitmask for bit 6  */
#define nu16Bit7   ((CDEF_tUint16)    0x80U)                /* Bitmask for bit 7  */
#define nu16Bit8   ((CDEF_tUint16)   0x100U)                /* Bitmask for bit 8  */
#define nu16Bit9   ((CDEF_tUint16)   0x200U)                /* Bitmask for bit 9  */
#define nu16Bit10  ((CDEF_tUint16)   0x400U)                /* Bitmask for bit 10 */
#define nu16Bit11  ((CDEF_tUint16)   0x800U)                /* Bitmask for bit 11 */
#define nu16Bit12  ((CDEF_tUint16)  0x1000U)                /* Bitmask for bit 12 */
#define nu16Bit13  ((CDEF_tUint16)  0x2000U)                /* Bitmask for bit 13 */
#define nu16Bit14  ((CDEF_tUint16)  0x4000U)                /* Bitmask for bit 14 */
#define nu16Bit15  ((CDEF_tUint16)  0x8000U)                /* Bitmask for bit 15 */

#define nu32Bit0   ((CDEF_tUint32) CDEF_nu32Bit00_Suf)      /* Bitmask for bit 0  */
#define nu32Bit1   ((CDEF_tUint32) CDEF_nu32Bit01_Suf)      /* Bitmask for bit 1  */
#define nu32Bit2   ((CDEF_tUint32) CDEF_nu32Bit02_Suf)      /* Bitmask for bit 2  */
#define nu32Bit3   ((CDEF_tUint32) CDEF_nu32Bit03_Suf)      /* Bitmask for bit 3  */
#define nu32Bit4   ((CDEF_tUint32) CDEF_nu32Bit04_Suf)      /* Bitmask for bit 4  */
#define nu32Bit5   ((CDEF_tUint32) CDEF_nu32Bit05_Suf)      /* Bitmask for bit 5  */
#define nu32Bit6   ((CDEF_tUint32) CDEF_nu32Bit06_Suf)      /* Bitmask for bit 6  */
#define nu32Bit7   ((CDEF_tUint32) CDEF_nu32Bit07_Suf)      /* Bitmask for bit 7  */
#define nu32Bit8   ((CDEF_tUint32) CDEF_nu32Bit08_Suf)      /* Bitmask for bit 8  */
#define nu32Bit9   ((CDEF_tUint32) CDEF_nu32Bit09_Suf)      /* Bitmask for bit 9  */
#define nu32Bit10  ((CDEF_tUint32) CDEF_nu32Bit10_Suf)      /* Bitmask for bit 10 */
#define nu32Bit11  ((CDEF_tUint32) CDEF_nu32Bit11_Suf)      /* Bitmask for bit 11 */
#define nu32Bit12  ((CDEF_tUint32) CDEF_nu32Bit12_Suf)      /* Bitmask for bit 12 */
#define nu32Bit13  ((CDEF_tUint32) CDEF_nu32Bit13_Suf)      /* Bitmask for bit 13 */
#define nu32Bit14  ((CDEF_tUint32) CDEF_nu32Bit14_Suf)      /* Bitmask for bit 14 */
#define nu32Bit15  ((CDEF_tUint32) CDEF_nu32Bit15_Suf)      /* Bitmask for bit 15 */
#define nu32Bit16  ((CDEF_tUint32) CDEF_nu32Bit16_Suf)      /* Bitmask for bit 16 */
#define nu32Bit17  ((CDEF_tUint32) CDEF_nu32Bit17_Suf)      /* Bitmask for bit 17 */
#define nu32Bit18  ((CDEF_tUint32) CDEF_nu32Bit18_Suf)      /* Bitmask for bit 18 */
#define nu32Bit19  ((CDEF_tUint32) CDEF_nu32Bit19_Suf)      /* Bitmask for bit 19 */
#define nu32Bit20  ((CDEF_tUint32) CDEF_nu32Bit20_Suf)      /* Bitmask for bit 20 */
#define nu32Bit21  ((CDEF_tUint32) CDEF_nu32Bit21_Suf)      /* Bitmask for bit 21 */
#define nu32Bit22  ((CDEF_tUint32) CDEF_nu32Bit22_Suf)      /* Bitmask for bit 22 */
#define nu32Bit23  ((CDEF_tUint32) CDEF_nu32Bit23_Suf)      /* Bitmask for bit 23 */
#define nu32Bit24  ((CDEF_tUint32) CDEF_nu32Bit24_Suf)      /* Bitmask for bit 24 */
#define nu32Bit25  ((CDEF_tUint32) CDEF_nu32Bit25_Suf)      /* Bitmask for bit 25 */
#define nu32Bit26  ((CDEF_tUint32) CDEF_nu32Bit26_Suf)      /* Bitmask for bit 26 */
#define nu32Bit27  ((CDEF_tUint32) CDEF_nu32Bit27_Suf)      /* Bitmask for bit 27 */
#define nu32Bit28  ((CDEF_tUint32) CDEF_nu32Bit28_Suf)      /* Bitmask for bit 28 */
#define nu32Bit29  ((CDEF_tUint32) CDEF_nu32Bit29_Suf)      /* Bitmask for bit 29 */
#define nu32Bit30  ((CDEF_tUint32) CDEF_nu32Bit30_Suf)      /* Bitmask for bit 30 */
#define nu32Bit31  ((CDEF_tUint32) CDEF_nu32Bit31_Suf)      /* Bitmask for bit 31 */

#ifndef   _CPU_16BIT_
#define nu64Bit0   ((CDEF_tUint64) CDEF_nu64Bit00_Suf)      /* Bitmask for bit 0  */
#define nu64Bit1   ((CDEF_tUint64) CDEF_nu64Bit01_Suf)      /* Bitmask for bit 1  */
#define nu64Bit2   ((CDEF_tUint64) CDEF_nu64Bit02_Suf)      /* Bitmask for bit 2  */
#define nu64Bit3   ((CDEF_tUint64) CDEF_nu64Bit03_Suf)      /* Bitmask for bit 3  */
#define nu64Bit4   ((CDEF_tUint64) CDEF_nu64Bit04_Suf)      /* Bitmask for bit 4  */
#define nu64Bit5   ((CDEF_tUint64) CDEF_nu64Bit05_Suf)      /* Bitmask for bit 5  */
#define nu64Bit6   ((CDEF_tUint64) CDEF_nu64Bit06_Suf)      /* Bitmask for bit 6  */
#define nu64Bit7   ((CDEF_tUint64) CDEF_nu64Bit07_Suf)      /* Bitmask for bit 7  */
#define nu64Bit8   ((CDEF_tUint64) CDEF_nu64Bit08_Suf)      /* Bitmask for bit 8  */
#define nu64Bit9   ((CDEF_tUint64) CDEF_nu64Bit09_Suf)      /* Bitmask for bit 9  */
#define nu64Bit10  ((CDEF_tUint64) CDEF_nu64Bit10_Suf)      /* Bitmask for bit 10 */
#define nu64Bit11  ((CDEF_tUint64) CDEF_nu64Bit11_Suf)      /* Bitmask for bit 11 */
#define nu64Bit12  ((CDEF_tUint64) CDEF_nu64Bit12_Suf)      /* Bitmask for bit 12 */
#define nu64Bit13  ((CDEF_tUint64) CDEF_nu64Bit13_Suf)      /* Bitmask for bit 13 */
#define nu64Bit14  ((CDEF_tUint64) CDEF_nu64Bit14_Suf)      /* Bitmask for bit 14 */
#define nu64Bit15  ((CDEF_tUint64) CDEF_nu64Bit15_Suf)      /* Bitmask for bit 15 */
#define nu64Bit16  ((CDEF_tUint64) CDEF_nu64Bit16_Suf)      /* Bitmask for bit 16 */
#define nu64Bit17  ((CDEF_tUint64) CDEF_nu64Bit17_Suf)      /* Bitmask for bit 17 */
#define nu64Bit18  ((CDEF_tUint64) CDEF_nu64Bit18_Suf)      /* Bitmask for bit 18 */
#define nu64Bit19  ((CDEF_tUint64) CDEF_nu64Bit19_Suf)      /* Bitmask for bit 19 */
#define nu64Bit20  ((CDEF_tUint64) CDEF_nu64Bit20_Suf)      /* Bitmask for bit 20 */
#define nu64Bit21  ((CDEF_tUint64) CDEF_nu64Bit21_Suf)      /* Bitmask for bit 21 */
#define nu64Bit22  ((CDEF_tUint64) CDEF_nu64Bit22_Suf)      /* Bitmask for bit 22 */
#define nu64Bit23  ((CDEF_tUint64) CDEF_nu64Bit23_Suf)      /* Bitmask for bit 23 */
#define nu64Bit24  ((CDEF_tUint64) CDEF_nu64Bit24_Suf)      /* Bitmask for bit 24 */
#define nu64Bit25  ((CDEF_tUint64) CDEF_nu64Bit25_Suf)      /* Bitmask for bit 25 */
#define nu64Bit26  ((CDEF_tUint64) CDEF_nu64Bit26_Suf)      /* Bitmask for bit 26 */
#define nu64Bit27  ((CDEF_tUint64) CDEF_nu64Bit27_Suf)      /* Bitmask for bit 27 */
#define nu64Bit28  ((CDEF_tUint64) CDEF_nu64Bit28_Suf)      /* Bitmask for bit 28 */
#define nu64Bit29  ((CDEF_tUint64) CDEF_nu64Bit29_Suf)      /* Bitmask for bit 29 */
#define nu64Bit30  ((CDEF_tUint64) CDEF_nu64Bit30_Suf)      /* Bitmask for bit 30 */
#define nu64Bit31  ((CDEF_tUint64) CDEF_nu64Bit31_Suf)      /* Bitmask for bit 31 */

#define nu64Bit32  ((CDEF_tUint64) CDEF_nu64Bit32_Suf)      /* Bitmask for bit 32 */
#define nu64Bit33  ((CDEF_tUint64) CDEF_nu64Bit33_Suf)      /* Bitmask for bit 33 */
#define nu64Bit34  ((CDEF_tUint64) CDEF_nu64Bit34_Suf)      /* Bitmask for bit 34 */
#define nu64Bit35  ((CDEF_tUint64) CDEF_nu64Bit35_Suf)      /* Bitmask for bit 35 */
#define nu64Bit36  ((CDEF_tUint64) CDEF_nu64Bit36_Suf)      /* Bitmask for bit 36 */
#define nu64Bit37  ((CDEF_tUint64) CDEF_nu64Bit37_Suf)      /* Bitmask for bit 37 */
#define nu64Bit38  ((CDEF_tUint64) CDEF_nu64Bit38_Suf)      /* Bitmask for bit 38 */
#define nu64Bit39  ((CDEF_tUint64) CDEF_nu64Bit39_Suf)      /* Bitmask for bit 39 */
#define nu64Bit40  ((CDEF_tUint64) CDEF_nu64Bit40_Suf)      /* Bitmask for bit 40 */
#define nu64Bit41  ((CDEF_tUint64) CDEF_nu64Bit41_Suf)      /* Bitmask for bit 41 */
#define nu64Bit42  ((CDEF_tUint64) CDEF_nu64Bit42_Suf)      /* Bitmask for bit 42 */
#define nu64Bit43  ((CDEF_tUint64) CDEF_nu64Bit43_Suf)      /* Bitmask for bit 43 */
#define nu64Bit44  ((CDEF_tUint64) CDEF_nu64Bit44_Suf)      /* Bitmask for bit 44 */
#define nu64Bit45  ((CDEF_tUint64) CDEF_nu64Bit45_Suf)      /* Bitmask for bit 45 */
#define nu64Bit46  ((CDEF_tUint64) CDEF_nu64Bit46_Suf)      /* Bitmask for bit 46 */
#define nu64Bit47  ((CDEF_tUint64) CDEF_nu64Bit47_Suf)      /* Bitmask for bit 47 */
#define nu64Bit48  ((CDEF_tUint64) CDEF_nu64Bit48_Suf)      /* Bitmask for bit 48 */
#define nu64Bit49  ((CDEF_tUint64) CDEF_nu64Bit49_Suf)      /* Bitmask for bit 49 */
#define nu64Bit50  ((CDEF_tUint64) CDEF_nu64Bit50_Suf)      /* Bitmask for bit 50 */
#define nu64Bit51  ((CDEF_tUint64) CDEF_nu64Bit51_Suf)      /* Bitmask for bit 51 */
#define nu64Bit52  ((CDEF_tUint64) CDEF_nu64Bit52_Suf)      /* Bitmask for bit 52 */
#define nu64Bit53  ((CDEF_tUint64) CDEF_nu64Bit53_Suf)      /* Bitmask for bit 53 */
#define nu64Bit54  ((CDEF_tUint64) CDEF_nu64Bit54_Suf)      /* Bitmask for bit 54 */
#define nu64Bit55  ((CDEF_tUint64) CDEF_nu64Bit55_Suf)      /* Bitmask for bit 55 */
#define nu64Bit56  ((CDEF_tUint64) CDEF_nu64Bit56_Suf)      /* Bitmask for bit 56 */
#define nu64Bit57  ((CDEF_tUint64) CDEF_nu64Bit57_Suf)      /* Bitmask for bit 57 */
#define nu64Bit58  ((CDEF_tUint64) CDEF_nu64Bit58_Suf)      /* Bitmask for bit 58 */
#define nu64Bit59  ((CDEF_tUint64) CDEF_nu64Bit59_Suf)      /* Bitmask for bit 59 */
#define nu64Bit60  ((CDEF_tUint64) CDEF_nu64Bit60_Suf)      /* Bitmask for bit 60 */
#define nu64Bit61  ((CDEF_tUint64) CDEF_nu64Bit61_Suf)      /* Bitmask for bit 61 */
#define nu64Bit62  ((CDEF_tUint64) CDEF_nu64Bit62_Suf)      /* Bitmask for bit 62 */
#define nu64Bit63  ((CDEF_tUint64) CDEF_nu64Bit63_Suf)      /* Bitmask for bit 63 */
#endif /* _CPU_16BIT_ */

/* synonyms for even older, more out-of-date spellings */
#define nbBit0    ((CDEF_tUint8) CDEF_nu8Bit0_Suf)         /* Bitmask for bit 0  */
#define nbBit1    ((CDEF_tUint8) CDEF_nu8Bit1_Suf)         /* Bitmask for bit 1  */
#define nbBit2    ((CDEF_tUint8) CDEF_nu8Bit2_Suf)         /* Bitmask for bit 2  */
#define nbBit3    ((CDEF_tUint8) CDEF_nu8Bit3_Suf)         /* Bitmask for bit 3  */
#define nbBit4    ((CDEF_tUint8) CDEF_nu8Bit4_Suf)         /* Bitmask for bit 4  */
#define nbBit5    ((CDEF_tUint8) CDEF_nu8Bit5_Suf)         /* Bitmask for bit 5  */
#define nbBit6    ((CDEF_tUint8) CDEF_nu8Bit6_Suf)         /* Bitmask for bit 6  */
#define nbBit7    ((CDEF_tUint8) CDEF_nu8Bit7_Suf)         /* Bitmask for bit 7  */

#define nwBit0   ((CDEF_tUint16)     0x1U)                /* Bitmask for bit 0  */
#define nwBit1   ((CDEF_tUint16)     0x2U)                /* Bitmask for bit 1  */
#define nwBit2   ((CDEF_tUint16)     0x4U)                /* Bitmask for bit 2  */
#define nwBit3   ((CDEF_tUint16)     0x8U)                /* Bitmask for bit 3  */
#define nwBit4   ((CDEF_tUint16)    0x10U)                /* Bitmask for bit 4  */
#define nwBit5   ((CDEF_tUint16)    0x20U)                /* Bitmask for bit 5  */
#define nwBit6   ((CDEF_tUint16)    0x40U)                /* Bitmask for bit 6  */
#define nwBit7   ((CDEF_tUint16)    0x80U)                /* Bitmask for bit 7  */
#define nwBit8   ((CDEF_tUint16)   0x100U)                /* Bitmask for bit 8  */
#define nwBit9   ((CDEF_tUint16)   0x200U)                /* Bitmask for bit 9  */
#define nwBit10  ((CDEF_tUint16)   0x400U)                /* Bitmask for bit 10 */
#define nwBit11  ((CDEF_tUint16)   0x800U)                /* Bitmask for bit 11 */
#define nwBit12  ((CDEF_tUint16)  0x1000U)                /* Bitmask for bit 12 */
#define nwBit13  ((CDEF_tUint16)  0x2000U)                /* Bitmask for bit 13 */
#define nwBit14  ((CDEF_tUint16)  0x4000U)                /* Bitmask for bit 14 */
#define nwBit15  ((CDEF_tUint16)  0x8000U)                /* Bitmask for bit 15 */

#define ndwBit0   ((CDEF_tUint32) CDEF_nu32Bit00_Suf)      /* Bitmask for bit 0  */
#define ndwBit1   ((CDEF_tUint32) CDEF_nu32Bit01_Suf)      /* Bitmask for bit 1  */
#define ndwBit2   ((CDEF_tUint32) CDEF_nu32Bit02_Suf)      /* Bitmask for bit 2  */
#define ndwBit3   ((CDEF_tUint32) CDEF_nu32Bit03_Suf)      /* Bitmask for bit 3  */
#define ndwBit4   ((CDEF_tUint32) CDEF_nu32Bit04_Suf)      /* Bitmask for bit 4  */
#define ndwBit5   ((CDEF_tUint32) CDEF_nu32Bit05_Suf)      /* Bitmask for bit 5  */
#define ndwBit6   ((CDEF_tUint32) CDEF_nu32Bit06_Suf)      /* Bitmask for bit 6  */
#define ndwBit7   ((CDEF_tUint32) CDEF_nu32Bit07_Suf)      /* Bitmask for bit 7  */
#define ndwBit8   ((CDEF_tUint32) CDEF_nu32Bit08_Suf)      /* Bitmask for bit 8  */
#define ndwBit9   ((CDEF_tUint32) CDEF_nu32Bit09_Suf)      /* Bitmask for bit 9  */
#define ndwBit10  ((CDEF_tUint32) CDEF_nu32Bit10_Suf)      /* Bitmask for bit 10 */
#define ndwBit11  ((CDEF_tUint32) CDEF_nu32Bit11_Suf)      /* Bitmask for bit 11 */
#define ndwBit12  ((CDEF_tUint32) CDEF_nu32Bit12_Suf)      /* Bitmask for bit 12 */
#define ndwBit13  ((CDEF_tUint32) CDEF_nu32Bit13_Suf)      /* Bitmask for bit 13 */
#define ndwBit14  ((CDEF_tUint32) CDEF_nu32Bit14_Suf)      /* Bitmask for bit 14 */
#define ndwBit15  ((CDEF_tUint32) CDEF_nu32Bit15_Suf)      /* Bitmask for bit 15 */
#define ndwBit16  ((CDEF_tUint32) CDEF_nu32Bit16_Suf)      /* Bitmask for bit 16 */
#define ndwBit17  ((CDEF_tUint32) CDEF_nu32Bit17_Suf)      /* Bitmask for bit 17 */
#define ndwBit18  ((CDEF_tUint32) CDEF_nu32Bit18_Suf)      /* Bitmask for bit 18 */
#define ndwBit19  ((CDEF_tUint32) CDEF_nu32Bit19_Suf)      /* Bitmask for bit 19 */
#define ndwBit20  ((CDEF_tUint32) CDEF_nu32Bit20_Suf)      /* Bitmask for bit 20 */
#define ndwBit21  ((CDEF_tUint32) CDEF_nu32Bit21_Suf)      /* Bitmask for bit 21 */
#define ndwBit22  ((CDEF_tUint32) CDEF_nu32Bit22_Suf)      /* Bitmask for bit 22 */
#define ndwBit23  ((CDEF_tUint32) CDEF_nu32Bit23_Suf)      /* Bitmask for bit 23 */
#define ndwBit24  ((CDEF_tUint32) CDEF_nu32Bit24_Suf)      /* Bitmask for bit 24 */
#define ndwBit25  ((CDEF_tUint32) CDEF_nu32Bit25_Suf)      /* Bitmask for bit 25 */
#define ndwBit26  ((CDEF_tUint32) CDEF_nu32Bit26_Suf)      /* Bitmask for bit 26 */
#define ndwBit27  ((CDEF_tUint32) CDEF_nu32Bit27_Suf)      /* Bitmask for bit 27 */
#define ndwBit28  ((CDEF_tUint32) CDEF_nu32Bit28_Suf)      /* Bitmask for bit 28 */
#define ndwBit29  ((CDEF_tUint32) CDEF_nu32Bit29_Suf)      /* Bitmask for bit 29 */
#define ndwBit30  ((CDEF_tUint32) CDEF_nu32Bit30_Suf)      /* Bitmask for bit 30 */
#define ndwBit31  ((CDEF_tUint32) CDEF_nu32Bit31_Suf)      /* Bitmask for bit 31 */


#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_OFF 3429, 3453
#endif
/* switch of unjustified QA C Warning here
   as discussed with SW QS on 08.06.2004,
   macro should be used here instead of function*/
#ifndef   _CPU_16BIT_
# define GetBitMaskB(n) (0x1ULL<<(n))   /* returns const unsigned int - only bit n set to 1 */
#else
# define GetBitMaskB(n) (0x1UL<<(n))   /* returns const unsigned int - only bit n set to 1 */
#endif /* _CPU_16BIT_ */

#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_ON 3429, 3453
#endif
#endif /*      CDEF_DEPRECATED_BITMASKS           */

#if defined    CDEF_DEPRECATED_BINARYCONVERTERS

/* ___________________________________________________________________________________________
 *                                       binary to integer converters 
 */
/*
 * Description       : Converts the binary Expression i into a numeric constant. 
 *                     Example:
 *                                nBinUInt8(11001001) -> (0xC9)
 * Parameter:
 *    i                 Binary coded number . Up to 8 binary digits (0 or 1) are allowed
 *                      Note: - Do NOT use a suffix like 1001UL or similar.
 *                            - Do NOT use hexadezimal syntax like 0x11011
 *                            - Using leading zeroes like 00101110 does not change the result
 *                              value, but might earn you QAC warnings.
 */
#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_OFF 3429, 3453
#endif
/* switch of unjustified QA C Warning here
   as discussed with SW QS on 08.06.2004,
   macro should be used here instead of function*/
#define nBinUInt8(u32Binary)  ((0x80UL&(0x##u32Binary##UL>>21))|(0x40UL&(0x##u32Binary##UL>>18)) \
                              |(0x20UL&(0x##u32Binary##UL>>15))|(0x10UL&(0x##u32Binary##UL>>12)) \
                              |(0x8UL &(0x##u32Binary##UL>> 9))|(0x4UL &(0x##u32Binary##UL>> 6)) \
                              |(0x2UL &(0x##u32Binary##UL>> 3))|(0x1UL &(0x##u32Binary##UL    )))
#define nBinByte(u32Binary)    (nBinUInt8(u32Binary))


#define bBinUInt8(ParBinB)                                                        \
/* returns CDEF_tUint8  */   ((CDEF_tUint8)nBinUInt8(ParBinB))
#define bBinByte(ParBinB)    ((CDEF_tUint8)nBinUInt8(ParBinB))         /* returns byte  */


#define nBinUInt16(ParBinMSB,ParBinLSB)                                           \
/* returns CDEF_tUint16 */ ( (((CDEF_tUint16)nBinUInt8(ParBinMSB))<<8U) |                     \
                             (((CDEF_tUint16)nBinUInt8(ParBinLSB))    )   )
/* Note: Cast upper 8 bits to CDEF_tUint16 BEFORE shifting,
              lower 8 bits for symmetry and ensured result type. 
         There is no literal suffix for 16bit.                   */
                                       
#define nBinWord(ParBinMSB,ParBinLSB)                                             \
/* returns CDEF_tUint16 */ ( (((CDEF_tUint16)nBinUInt8(ParBinMSB))<<8U) |                     \
                             (((CDEF_tUint16)nBinUInt8(ParBinLSB))    )   )
/* Note: Cast upper 8 bits to CDEF_tUint16 BEFORE shifting,
              lower 8 bits for symmetry and ensured result type.
        There is no literal suffix for 16bit.                     */
                                       
#define nBinUInt32(ParBinMSB,ParBinB3,ParBinB2,ParBinLSB)                         \
/* returns CDEF_tUint32 */ ( (((CDEF_tUint32)nBinUInt16(ParBinMSB,ParBinB3))<<16UL)|          \
                             (((CDEF_tUint32)nBinUInt16(ParBinB2,ParBinLSB))      )  )
/* Note: Cast upper 16 bits to CDEF_tUint32 BEFORE shifting,
              lower 16 bits for symmetry and ensured result type. */

#define nBinUInt64(ParBinMSB,ParBinB7,ParBinB6,ParBinB5,ParBinB4,ParBinB3,ParBinB2,ParBinLSB) \
/* returns CDEF_tUint64 */ ( (((CDEF_tUint64)nBinUInt32(ParBinMSB,ParBinB7,ParBinB6,ParBinB5))<<32ULL)|   \
                             (((CDEF_tUint64)nBinUInt32(ParBinB4,ParBinB3,ParBinB2,ParBinLSB))       )  )
/* Note: Cast upper 32 bits to CDEF_tUint64 BEFORE shifting,
              lower 32 bits for symmetry and ensured result type. */

#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_ON 3429, 3453
#endif

#endif /*      CDEF_DEPRECATED_BINARYCONVERTERS   */


#endif /* CDEF_DEPRECATED_H */

 /*********************************************************************************************
 *   Revision List:
 *   (Will be filled by MKS for CDS releases, 
 *    feel free to delete this for use in GIT or project MKS.)
 *         Archive:
 * $Log: cdef_deprecated.h  $
 * Revision 1.9 2017/05/04 14:52:21CEST Helk, Aribert (uidf2456) 
 * CDEF: copyright information missing
 * Revision 1.8 2017/02/16 10:51:23CET Rademacher, Jan (uidf3054) 
 * JIRA ticket CDS-1024: CDEF: QAC warnings to solve
 * - silence both warnings, via pragma and via syntax comment, with individual reasoning
 * _
 * Revision 1.7 2017/02/15 16:43:14CET Rademacher, Jan (uidf3054) 
 * - change according to SQE recommendation: remove changelog form start of file
 *    in order to avoid problems by mismatching line numbers due to keyword expansion
 *  - keep changelog at end of file, at least in the MKS-based CDS release,
 *    invite users to delete for use in GIT or customer department MKS
 * _
 * Revision 1.6 2014/07/08 11:57:39CEST Rademacher, Jan (uidf3054) 
 * - JIRA OSP 1749 CDEF cleaner name spaces
 *   - fix a backward compatibility issue "inconsistent redefinition of <symbol>"
 * - improve symmetrie of encapsulations with indentation
 *   and consistent idiom
 * _
 * Revision 1.5 2014/07/04 12:06:22CEST Rademacher, Jan (uidf3054) 
 * - JIRA OSP 1749 CDEF cleaner name spaces
 *   - fix a mistake in the backward compatibility synonyms
 * _
 * Revision 1.4 2014/06/30 11:48:44CEST Rademacher, Jan (uidf3054) 
 * - JIRA OSP 1749 CDEF cleaner name spaces
 *   - add prefix-less symbols (moved from API headers)
 *   - encapsulate analog to JIRA OSP 1732
 *   - use new symbols with prefix for deriving other symbols
 *     - avoid non-API symbols in plausibility checks
 *   - define symbols as SMK-friendly and QAC-friendly as possible
 *     - define symbols explicitly, only verify against available information
 *       (this allows to silence the QAC messages inside cdef,
 *         instead only in clients, i.e. less noisy QAC for clients)
 *     - only define symbols if not yet defined by headers
 *       which happen to be visible, or by C++
 *     - avoid internal "adaptation symbols" being visible
 * - improve symmetry of #if/#endif with comments
 * _
 * Revision 1.3 2014/06/18 11:41:09CEST Rademacher, Jan (uidf3054) 
 * - JIRA OSP1732 CDEF invert logic of support of deprecated symbols
 * Revision 1.2 2014/06/18 09:18:47CEST Rademacher, Jan (uidf3054) 
 * - add usually inactive, deprecated synonyms for float types
 * - alignment improvements for readability, functionally irrelevant
 * Revision 1.1 2010/07/20 11:00:13CEST Rademacher, Jan (uidf3054) 
 * Initial revision
 * Member added to project /id/_gen/sw/pkg/Basic/cdef/imp/core/project.pj
 *
 * Initial revision (above) created from below
 *
 * Revision 1.3 2010/07/20 08:26:36CEST Rademacher Jan (uidf3054) (uidf3054) 
 * - CDS MR 8766 CDEF: some definitions imported from GHS limits.h cause QAC warnings level 6
 *  - derive INT16_MIN as "((-SHRT_MAX)-1)" instead of as "SHRT_MIN",
 *    only in case "SHRT_MIN==((-SHRT_MAX)-1)"
 *  - silence QAC 271 and 4131 around the "#if" in cdef.h, which works in contrast to the
 *    former construct which triggered warnings in cdef-clients
 *  - same in an analog way for INT8_MIN
 *  - not needed for 32 and 64 bit, because they are defined differently,
 *    at least in an encapsulation which applies to GHS
 * - in the spirit of CDS MR 8766, do some additional QAC-related make-up and silencing
 * - fix a typo
 *         Archive: Revision 1.2 2010/07/14 17:46:11CEST Rademacher Jan (uidf3054) (uidf3054) 
 *         Archive: CDS MR 8810 CDEF: Support of 64 bit wide bitfields when using Softune
 *         Archive:    Unnconditionally use uint64 for 64bit bitfields.
 *         Archive:    This will fail with older Softune compilers.
 *         Archive:    This positively changes the allocation of bitfields with bitgroups
 *         Archive:    overlapping 32bit-borders.
 *         Archive: Revision 1.1 2009/11/27 13:51:34CET Rademacher Jan (uidf3054) (uidf3054) 
 *         Archive: Initial revision
 *         Archive: Member added to project /id/_gen/sw/pkg/Basic/cdef/imp/core/project.pj
 *********************************************************************************************/


/*====================================== END OF FILE =======================================*/

