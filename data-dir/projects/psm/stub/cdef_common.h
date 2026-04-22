/***************************************************************************
*=====================      Copyright by Continental AG      ===============
****************************************************************************
* Title        : cdef_common.h
*
*  Description: Collect endianess independent definitions from
 *                  both cdefs before CDS MR 8954/8955.
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

#ifndef CDEF_COMMON_H
#define CDEF_COMMON_H

/*  CDS MR 7034 CDEF: inline handling - softune problems
 *    CCB 26.02.2009 "___inline raus"
 *    understood as  Remove inline-handling completely from cdef.
 *
 *  Ladislav Cezner (2009-04-23):
 *  Softune now understands "inline".
 *  GreenHills always understood "inline" AND "__inline".
 */


/* CDS MR 9940 CDEF: add a compiler switch to remove the keywords for IAR compiler
 * CCB: Unconditionally remove __short, __far, __near .
 */

/* ___________________________________________________________________________________________
 *                              check general assumptions 
 * 
 * Only "exotic" hardware (e.g. 4bit CPUs) should produce an error here 
 */    
#if CHAR_BIT != 8
#error __FILE__ : not 8 bits per byte. Check <limits.h>
#endif


/*********************************************************************************************
 **************************************       ************************************************
 **************************************  API  ************************************************
 **************************************       ************************************************
 *********************************************************************************************/

/* ___________________________________________________________________________________________
 *                                 Basic data types 
 */ 
#ifndef __cplusplus                        /* use Std ANSI C++ macro to detect C++ compilation  */
/** Boolean datatype  (keyword in C++)                                  */
  typedef unsigned char       CDEF_tBool;
#else
  typedef bool                CDEF_tBool;
#endif

/** Unsigned 8-bit datatype : 0..255                                    */  
  typedef unsigned char        CDEF_tUint8;
  
/** Unsigned 16-bit datatype: 0..65535                                  */
  typedef unsigned short int   CDEF_tUint16;
  
/** Unsigned 32-bit datatype: 0..4294967295                             */  
  // typedef unsigned long int    CDEF_tUint32;
  typedef unsigned int    CDEF_tUint32; // modified due to fuzz test build is done on 64bit cpus

#ifndef   _CPU_16BIT_
/** Unsigned 64-bit datatype: 0..18446744073709551615                   */
  typedef unsigned long long int CDEF_tUint64;
#endif /* _CPU_16BIT_ */

/** Signed 8-bit datatype   : -128...+127                               */
  typedef signed char            CDEF_tInt8;

/** Signed 16-bit datatype  : -32768..+32767                            */
  typedef signed short int       CDEF_tInt16;
  
/** Signed 32-bit datatype  : -2147483648..2147483647                   */  
  typedef signed long int        CDEF_tInt32;

#ifndef   _CPU_16BIT_
/** Signed 64-bit datatype  : -9223372036854775808..9223372036854775807 */
  typedef signed long long int   CDEF_tInt64;
#endif /* _CPU_16BIT_ */

#ifdef CDEF_FLOAT_SUPPORT
/* Only define float types, if environment switches
  (i.e. probably from "_build_options") signal that the
  compiler, hardware and/or project decisions allow
  the use of floating point data.
 */

/** 32-bit floating point datatype */
  typedef  float CDEF_tFloat32;
  
#ifndef   _CPU_16BIT_
/** 64-bit floating point datatype */
  typedef double CDEF_tFloat64;
#endif /* _CPU_16BIT_ */
#endif /* CDEF_FLOAT_SUPPORT */


  /** tu8BitByte = (logical)   8bit within bitfield     */
  typedef CDEF_tUint8              CDEF_tu8BitField;

  /** tu16BitWord = (logical) 16bit within bitfield     */
  typedef CDEF_tUint16            CDEF_tu16BitField;

  /** tu32BitDWord = (logical) 32bit within bitfield    */
  typedef CDEF_tUint32            CDEF_tu32BitField;
  
#ifndef   _CPU_16BIT_
/* CDS MR 8810 CDEF: Support of 64 bit wide bitfields when using Softune
   Unnconditionally use uint64 for 64bit bitfields.
   This will fail with older Softune compilers.
   This positively changes the allocation of bitfields with bitgroups
   overlapping 32bit-borders. */
   
/** tu64BitLLWord = (logical) 64bit within bitfield  */
  typedef CDEF_tUint64          CDEF_tu64BitField;
#endif /* _CPU_16BIT_ */


/** ptr to function expecting void + returning void,
   to be used in containers and other references to functions */
  typedef void (*CDEF_tFuncPtr) (void);

/* ___________________________________________________________________________________________
 *                               numeric min/max constants 
 *
 * VDO prefers own names for compiler specific, differently named but semantically identical
 *  macros from limits.h.
 */

/* Define the min/max-values literally in a way which keeps QAC happy.
 * (Note: There is no compiler-independent way to make a 8bit-wide unsigned literal
 *        or a a 16bit-wide unsigned literal. In both cases, the probably wider, but
 *        at least definitly unsigned literal-syntax "0xn...nU" is used.
 *        There is also no compiler-independent way to make 8/16bit signed literals,
 *        so use the totally bare "0xn...n", i.e. no suffix whatsoever.)
 * Then (further down) look for a source of information from compiler supplier
 * to verify the value, in one of three ways, depending on available input symbols
 * (from stdint.h, limits.h or the like).
 * Cdef cannot know, whether stdint.h has been included in the current .c file and
 * cannot include it itself, because not all compilers/platforms offer that header.
 * If [U]INT<nn>_(MIN|MAX) is available, then use it;
 * otherwise use one of two alternatives, depending on compiler suppliers preferences.
 * One is "usual", if it is not present the other is checked for presence,
 * if not then error.
 * The definitions in stdint.h (e.g. UINT8_MAX) and limits.h (UCHAR_MAX) differ
 * in the way they define the limit, but not in the value itself.
 */


#define CDEF_nu8Max  (0xffU)
#define CDEF_ni8Max  (0x7f)
#define CDEF_nu16Max (0xffffU)
#define CDEF_ni16Max (0x7fff)
#define CDEF_nu32Max (0xffffffffUL)
#define CDEF_ni32Max (0x7fffffffL )
#ifndef   _CPU_16BIT_
#define CDEF_nu64Max (0xffffffffffffffffULL)
#define CDEF_ni64Max (0x7fffffffffffffffLL )
#endif /* _CPU_16BIT_ */


#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_OFF 271, 2860, 4131
/* QAC: 2860; 2017-02-15, Sabine Neumann-EXT: "Harmless"
 * Because the two implementation specific constructs (SCHAR_MIN and SCHAR_MAX)
 * together make an implementation independent bigger construct, based on their
 * semantic, which is guaranteed, because they both come from a compiler-header
 * which is aware of whatever implementation specific assumptions are made by the
 * compiler.
 * The bigger construct simply says
 * "if negative highest possible signed character is same as lowest possible
 * signed char, then it is complement 1 representation of negative numbers."
 * This is almost the definition of "complement 1 representation".
 * Similarly, complement 2 representation is assumed if "negative highest
 * possible signed character, additionally minus 1, is same as lowest possible,
 * then complement 2 representation" is assumed. This, too is practically the
 * the definition of complement 2 representation. 
 * Note that any unexpected representation will be caught be the else branch
 * and that the attribute verified by the preprocessor "if" lines is the only
 * assumption used in the subsequent implementation line.
 * This explanation also applies directly to SHRT_MAX,SHRT_MIN.
 * Older silencing:
 * QAC: 271 
 * QAC: 4131 
 * These warnings can be silenced here, sparing the cdef-clients,
 * according to CDS MR 8766.
 */
#endif

/* <@QAC: 2860> <RelLineCount: 1> <SWQE: 2017-02-15, Sabine Neumann-EXT> <Comment: Harmless, see comment above> */
#if   (((-SCHAR_MAX)-1)==SCHAR_MIN) /* complement ? representation */
#define CDEF_ni8Min  (-0x7f-0x1)    /* complement-2 representation */
/* <@QAC: 2860> <RelLineCount: 1> <SWQE: 2017-02-15, Sabine Neumann-EXT> <Comment: Harmless, see comment above> */
#elif (( -SCHAR_MAX   )==SCHAR_MIN)                            
#define CDEF_ni8Min  (-0x7f    )    /* complement-1 representation */
#else
#error Unknown representation of negative numbers.
#endif                              /* complement ? representation */

/* <@QAC: 2860> <RelLineCount: 1> <SWQE: 2017-02-15, Sabine Neumann-EXT> <Comment: Harmless, see comment above> */
#if   (((-SHRT_MAX)-1)==SHRT_MIN)   /* complement ? representation */
#define CDEF_ni16Min (-0x7fff-1)    /* complement-2 representation */

/* There are no known symbols to test the complement ? representation of signed 32bit and 64bit.
   Hence reuse the detection of 16bit representation and assume that the same is used for 32/64. */
#define CDEF_ni32Min (-0x7fffffffL-1L)

#ifndef   _CPU_16BIT_
#define CDEF_ni64Min (-0x7fffffffffffffffLL-0x1LL)
#endif /* _CPU_16BIT_ */


/* <@QAC: 2860> <RelLineCount: 1> <SWQE: 2017-02-15, Sabine Neumann-EXT> <Comment: Harmless, see comment above> */
#elif (( -SHRT_MAX   )==SHRT_MIN)                             
#define CDEF_ni16Min (-0x7fff  )    /* complement-1 representation */

#define CDEF_ni32Min (-0x7fffffffL   )

#ifndef   _CPU_16BIT_
#define CDEF_ni64Min (-0x7fffffffffffffffLL      )
#endif /* _CPU_16BIT_ */

#else
#error Unknown representation of negative numbers.
#endif                              /* complement ? representation */


#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_ON 271, 2860, 4131
#endif





/* ___________________________________________________________________________________________
 *                                    bool constants 
 */
#define CDEF_True    ((CDEF_tBool) ( 1 == 1 ))    /* Boolean value for True       */
#define CDEF_False   ((CDEF_tBool) ( 1 == 0 ))    /* Boolean value for False      */

#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_OFF 3628
#endif
/* The octal sequences serve as 8bit literal values with the advantages described below. */

/* ___________________________________________________________________________________________
 *                                integer-type constants 
 */
/* NOTE:
    '\001' is an 8bit wide literal 1.
    These constants are meant to be used by the pre-processor only.
    When using them in compiler context, use an appropriate cast.
    QAC currently does not complain about assigning e.g. On to any uint<n> variable,
     because the 8bit definition is narrower or equally wide.
    sizeof(1U), sizeof(1UL), sizeof(On) is always the preferred width (e.g. 4). Only
     sizeof literals wider than preferred width (e.g. 1ULL) is different.
 */
#define CDEF_Yes       '\001'                  /* 8bit Integer-type value for Yes   */
#define CDEF_No        '\000'                  /* 8bit Integer-type value for No    */
#define CDEF_On        '\001'                  /* 8bit Integer-type value for On    */
#define CDEF_Off       '\000'                  /* 8bit Integer-type value for Off   */
#define CDEF_Ok        '\001'                  /* 8bit Integer-type value for Ok    */
#define CDEF_NotOk     '\000'                  /* 8bit Integer-type value for NotOk */
#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_ON 3628
#endif



/* ___________________________________________________________________________________________
 *                                       parameterised bit masks 
 */
#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_OFF 3429, 3453
#endif
/* switch of unjustified QA C Warning here
   as discussed with SW QS on 08.06.2004,
   macro should be used here instead of function*/

#define  CDEF_nu8GetBitMask_Suf(u8BitPosition) ('\x1'<<(u8BitPosition))
/* returns const char (8bits)           - only one bit set at the indicated BitPosition */

/* There is no way of defining CDEF_u16GetBitMask(), if you want to use it, you can
   probably work around with (CDEF_tUint16) CDEF_nu32GetBitMask_Suf() */

#define CDEF_nu32GetBitMask_Suf(u8BitPosition) (0x1UL<<(u8BitPosition))
/* returns const unsigned long int      - only one bit set at the indicated BitPosition */

#ifndef   _CPU_16BIT_
#define CDEF_nu64GetBitMask_Suf(u8BitPosition) (0x1ULL<<(u8BitPosition))
/* returns const unsigned long long int - only one bit set at the indicated BitPosition */
#endif /* _CPU_16BIT_ */

#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_ON 3429, 3453
#endif

/* ___________________________________________________________________________________________
 *                                       single bit constants
 *                                       literals without cast
 *    These constants should be readable by the pre-processor, e.g. plausibility checks are
 *   possible. But because of the syntax used for defining them, the compiler and QAC also
 *   associate a certain unsigned integer type. Correctly used, this allows to avoid QAC
 *   warnings. 
 */
#define CDEF_nu8Bit0_Suf ( '\x1')                          /* Bitmask for bit 0  */
#define CDEF_nu8Bit1_Suf ( '\x2')                          /* Bitmask for bit 1  */
#define CDEF_nu8Bit2_Suf ( '\x4')                          /* Bitmask for bit 2  */
#define CDEF_nu8Bit3_Suf ( '\x8')                          /* Bitmask for bit 3  */
#define CDEF_nu8Bit4_Suf ('\x10')                          /* Bitmask for bit 4  */
#define CDEF_nu8Bit5_Suf ('\x20')                          /* Bitmask for bit 5  */
#define CDEF_nu8Bit6_Suf ('\x40')                          /* Bitmask for bit 6  */
#define CDEF_nu8Bit7_Suf ('\x80')                          /* Bitmask for bit 7  */

/* there is no way of defining a 16bit literal with an implicit type,
   use CDEF_nu16Bit<m>, which uses a type cast */

#define CDEF_nu32Bit00_Suf        (0x1UL)                  /* Bitmask for bit  0 */
#define CDEF_nu32Bit01_Suf        (0x2UL)                  /* Bitmask for bit  1 */
#define CDEF_nu32Bit02_Suf        (0x4UL)                  /* Bitmask for bit  2 */
#define CDEF_nu32Bit03_Suf        (0x8UL)                  /* Bitmask for bit  3 */
#define CDEF_nu32Bit04_Suf       (0x10UL)                  /* Bitmask for bit  4 */
#define CDEF_nu32Bit05_Suf       (0x20UL)                  /* Bitmask for bit  5 */
#define CDEF_nu32Bit06_Suf       (0x40UL)                  /* Bitmask for bit  6 */
#define CDEF_nu32Bit07_Suf       (0x80UL)                  /* Bitmask for bit  7 */
#define CDEF_nu32Bit08_Suf      (0x100UL)                  /* Bitmask for bit  8 */
#define CDEF_nu32Bit09_Suf      (0x200UL)                  /* Bitmask for bit  9 */
#define CDEF_nu32Bit10_Suf      (0x400UL)                  /* Bitmask for bit 10 */
#define CDEF_nu32Bit11_Suf      (0x800UL)                  /* Bitmask for bit 11 */
#define CDEF_nu32Bit12_Suf     (0x1000UL)                  /* Bitmask for bit 12 */
#define CDEF_nu32Bit13_Suf     (0x2000UL)                  /* Bitmask for bit 13 */
#define CDEF_nu32Bit14_Suf     (0x4000UL)                  /* Bitmask for bit 14 */
#define CDEF_nu32Bit15_Suf     (0x8000UL)                  /* Bitmask for bit 15 */
#define CDEF_nu32Bit16_Suf    (0x10000UL)                  /* Bitmask for bit 16 */
#define CDEF_nu32Bit17_Suf    (0x20000UL)                  /* Bitmask for bit 17 */
#define CDEF_nu32Bit18_Suf    (0x40000UL)                  /* Bitmask for bit 18 */
#define CDEF_nu32Bit19_Suf    (0x80000UL)                  /* Bitmask for bit 19 */
#define CDEF_nu32Bit20_Suf   (0x100000UL)                  /* Bitmask for bit 20 */
#define CDEF_nu32Bit21_Suf   (0x200000UL)                  /* Bitmask for bit 21 */
#define CDEF_nu32Bit22_Suf   (0x400000UL)                  /* Bitmask for bit 22 */
#define CDEF_nu32Bit23_Suf   (0x800000UL)                  /* Bitmask for bit 23 */
#define CDEF_nu32Bit24_Suf  (0x1000000UL)                  /* Bitmask for bit 24 */
#define CDEF_nu32Bit25_Suf  (0x2000000UL)                  /* Bitmask for bit 25 */
#define CDEF_nu32Bit26_Suf  (0x4000000UL)                  /* Bitmask for bit 26 */
#define CDEF_nu32Bit27_Suf  (0x8000000UL)                  /* Bitmask for bit 27 */
#define CDEF_nu32Bit28_Suf (0x10000000UL)                  /* Bitmask for bit 28 */
#define CDEF_nu32Bit29_Suf (0x20000000UL)                  /* Bitmask for bit 29 */
#define CDEF_nu32Bit30_Suf (0x40000000UL)                  /* Bitmask for bit 30 */
#define CDEF_nu32Bit31_Suf (0x80000000UL)                  /* Bitmask for bit 31 */

#ifndef   _CPU_16BIT_
#define CDEF_nu64Bit00_Suf                (0x1ULL)         /* Bitmask for bit  0 */
#define CDEF_nu64Bit01_Suf                (0x2ULL)         /* Bitmask for bit  1 */
#define CDEF_nu64Bit02_Suf                (0x4ULL)         /* Bitmask for bit  2 */
#define CDEF_nu64Bit03_Suf                (0x8ULL)         /* Bitmask for bit  3 */
#define CDEF_nu64Bit04_Suf               (0x10ULL)         /* Bitmask for bit  4 */
#define CDEF_nu64Bit05_Suf               (0x20ULL)         /* Bitmask for bit  5 */
#define CDEF_nu64Bit06_Suf               (0x40ULL)         /* Bitmask for bit  6 */
#define CDEF_nu64Bit07_Suf               (0x80ULL)         /* Bitmask for bit  7 */
#define CDEF_nu64Bit08_Suf              (0x100ULL)         /* Bitmask for bit  8 */
#define CDEF_nu64Bit09_Suf              (0x200ULL)         /* Bitmask for bit  9 */
#define CDEF_nu64Bit10_Suf              (0x400ULL)         /* Bitmask for bit 10 */
#define CDEF_nu64Bit11_Suf              (0x800ULL)         /* Bitmask for bit 11 */
#define CDEF_nu64Bit12_Suf             (0x1000ULL)         /* Bitmask for bit 12 */
#define CDEF_nu64Bit13_Suf             (0x2000ULL)         /* Bitmask for bit 13 */
#define CDEF_nu64Bit14_Suf             (0x4000ULL)         /* Bitmask for bit 14 */
#define CDEF_nu64Bit15_Suf             (0x8000ULL)         /* Bitmask for bit 15 */
#define CDEF_nu64Bit16_Suf            (0x10000ULL)         /* Bitmask for bit 16 */
#define CDEF_nu64Bit17_Suf            (0x20000ULL)         /* Bitmask for bit 17 */
#define CDEF_nu64Bit18_Suf            (0x40000ULL)         /* Bitmask for bit 18 */
#define CDEF_nu64Bit19_Suf            (0x80000ULL)         /* Bitmask for bit 19 */
#define CDEF_nu64Bit20_Suf           (0x100000ULL)         /* Bitmask for bit 20 */
#define CDEF_nu64Bit21_Suf           (0x200000ULL)         /* Bitmask for bit 21 */
#define CDEF_nu64Bit22_Suf           (0x400000ULL)         /* Bitmask for bit 22 */
#define CDEF_nu64Bit23_Suf           (0x800000ULL)         /* Bitmask for bit 23 */
#define CDEF_nu64Bit24_Suf          (0x1000000ULL)         /* Bitmask for bit 24 */
#define CDEF_nu64Bit25_Suf          (0x2000000ULL)         /* Bitmask for bit 25 */
#define CDEF_nu64Bit26_Suf          (0x4000000ULL)         /* Bitmask for bit 26 */
#define CDEF_nu64Bit27_Suf          (0x8000000ULL)         /* Bitmask for bit 27 */
#define CDEF_nu64Bit28_Suf         (0x10000000ULL)         /* Bitmask for bit 28 */
#define CDEF_nu64Bit29_Suf         (0x20000000ULL)         /* Bitmask for bit 29 */
#define CDEF_nu64Bit30_Suf         (0x40000000ULL)         /* Bitmask for bit 30 */
#define CDEF_nu64Bit31_Suf         (0x80000000ULL)         /* Bitmask for bit 31 */
#define CDEF_nu64Bit32_Suf        (0x100000000ULL)         /* Bitmask for bit 32 */
#define CDEF_nu64Bit33_Suf        (0x200000000ULL)         /* Bitmask for bit 33 */
#define CDEF_nu64Bit34_Suf        (0x400000000ULL)         /* Bitmask for bit 34 */
#define CDEF_nu64Bit35_Suf        (0x800000000ULL)         /* Bitmask for bit 35 */
#define CDEF_nu64Bit36_Suf       (0x1000000000ULL)         /* Bitmask for bit 36 */
#define CDEF_nu64Bit37_Suf       (0x2000000000ULL)         /* Bitmask for bit 37 */
#define CDEF_nu64Bit38_Suf       (0x4000000000ULL)         /* Bitmask for bit 38 */
#define CDEF_nu64Bit39_Suf       (0x8000000000ULL)         /* Bitmask for bit 39 */
#define CDEF_nu64Bit40_Suf      (0x10000000000ULL)         /* Bitmask for bit 40 */
#define CDEF_nu64Bit41_Suf      (0x20000000000ULL)         /* Bitmask for bit 41 */
#define CDEF_nu64Bit42_Suf      (0x40000000000ULL)         /* Bitmask for bit 42 */
#define CDEF_nu64Bit43_Suf      (0x80000000000ULL)         /* Bitmask for bit 43 */
#define CDEF_nu64Bit44_Suf     (0x100000000000ULL)         /* Bitmask for bit 44 */
#define CDEF_nu64Bit45_Suf     (0x200000000000ULL)         /* Bitmask for bit 45 */
#define CDEF_nu64Bit46_Suf     (0x400000000000ULL)         /* Bitmask for bit 46 */
#define CDEF_nu64Bit47_Suf     (0x800000000000ULL)         /* Bitmask for bit 47 */
#define CDEF_nu64Bit48_Suf    (0x1000000000000ULL)         /* Bitmask for bit 48 */
#define CDEF_nu64Bit49_Suf    (0x2000000000000ULL)         /* Bitmask for bit 49 */
#define CDEF_nu64Bit50_Suf    (0x4000000000000ULL)         /* Bitmask for bit 50 */
#define CDEF_nu64Bit51_Suf    (0x8000000000000ULL)         /* Bitmask for bit 51 */
#define CDEF_nu64Bit52_Suf   (0x10000000000000ULL)         /* Bitmask for bit 52 */
#define CDEF_nu64Bit53_Suf   (0x20000000000000ULL)         /* Bitmask for bit 53 */
#define CDEF_nu64Bit54_Suf   (0x40000000000000ULL)         /* Bitmask for bit 54 */
#define CDEF_nu64Bit55_Suf   (0x80000000000000ULL)         /* Bitmask for bit 55 */
#define CDEF_nu64Bit56_Suf  (0x100000000000000ULL)         /* Bitmask for bit 56 */
#define CDEF_nu64Bit57_Suf  (0x200000000000000ULL)         /* Bitmask for bit 57 */
#define CDEF_nu64Bit58_Suf  (0x400000000000000ULL)         /* Bitmask for bit 58 */
#define CDEF_nu64Bit59_Suf  (0x800000000000000ULL)         /* Bitmask for bit 59 */
#define CDEF_nu64Bit60_Suf (0x1000000000000000ULL)         /* Bitmask for bit 60 */
#define CDEF_nu64Bit61_Suf (0x2000000000000000ULL)         /* Bitmask for bit 61 */
#define CDEF_nu64Bit62_Suf (0x4000000000000000ULL)         /* Bitmask for bit 62 */
#define CDEF_nu64Bit63_Suf (0x8000000000000000ULL)         /* Bitmask for bit 63 */
#endif /* _CPU_16BIT_ */

/* ___________________________________________________________________________________________
 *                                       single bit constants
 *                                       literals from above (except 16bit) with cast 
 *   These constant cannot be interpreted by the pre-processor, because they use a type cast
 *  to force the type corresponding to their nu<n> prefix.
 */ 
#define CDEF_nu8Bit0    ((CDEF_tUint8) CDEF_nu8Bit0_Suf)         /* Bitmask for bit  0 */
#define CDEF_nu8Bit1    ((CDEF_tUint8) CDEF_nu8Bit1_Suf)         /* Bitmask for bit  1 */
#define CDEF_nu8Bit2    ((CDEF_tUint8) CDEF_nu8Bit2_Suf)         /* Bitmask for bit  2 */
#define CDEF_nu8Bit3    ((CDEF_tUint8) CDEF_nu8Bit3_Suf)         /* Bitmask for bit  3 */
#define CDEF_nu8Bit4    ((CDEF_tUint8) CDEF_nu8Bit4_Suf)         /* Bitmask for bit  4 */
#define CDEF_nu8Bit5    ((CDEF_tUint8) CDEF_nu8Bit5_Suf)         /* Bitmask for bit  5 */
#define CDEF_nu8Bit6    ((CDEF_tUint8) CDEF_nu8Bit6_Suf)         /* Bitmask for bit  6 */
#define CDEF_nu8Bit7    ((CDEF_tUint8) CDEF_nu8Bit7_Suf)         /* Bitmask for bit  7 */

#define CDEF_nu16Bit0   ((CDEF_tUint16)     0x1U)             /* Bitmask for bit  0 */
#define CDEF_nu16Bit1   ((CDEF_tUint16)     0x2U)             /* Bitmask for bit  1 */
#define CDEF_nu16Bit2   ((CDEF_tUint16)     0x4U)             /* Bitmask for bit  2 */
#define CDEF_nu16Bit3   ((CDEF_tUint16)     0x8U)             /* Bitmask for bit  3 */
#define CDEF_nu16Bit4   ((CDEF_tUint16)    0x10U)             /* Bitmask for bit  4 */
#define CDEF_nu16Bit5   ((CDEF_tUint16)    0x20U)             /* Bitmask for bit  5 */
#define CDEF_nu16Bit6   ((CDEF_tUint16)    0x40U)             /* Bitmask for bit  6 */
#define CDEF_nu16Bit7   ((CDEF_tUint16)    0x80U)             /* Bitmask for bit  7 */
#define CDEF_nu16Bit8   ((CDEF_tUint16)   0x100U)             /* Bitmask for bit  8 */
#define CDEF_nu16Bit9   ((CDEF_tUint16)   0x200U)             /* Bitmask for bit  9 */
#define CDEF_nu16Bit10  ((CDEF_tUint16)   0x400U)             /* Bitmask for bit 10 */
#define CDEF_nu16Bit11  ((CDEF_tUint16)   0x800U)             /* Bitmask for bit 11 */
#define CDEF_nu16Bit12  ((CDEF_tUint16)  0x1000U)             /* Bitmask for bit 12 */
#define CDEF_nu16Bit13  ((CDEF_tUint16)  0x2000U)             /* Bitmask for bit 13 */
#define CDEF_nu16Bit14  ((CDEF_tUint16)  0x4000U)             /* Bitmask for bit 14 */
#define CDEF_nu16Bit15  ((CDEF_tUint16)  0x8000U)             /* Bitmask for bit 15 */

#define CDEF_nu32Bit0   ((CDEF_tUint32) CDEF_nu32Bit00_Suf)      /* Bitmask for bit  0 */
#define CDEF_nu32Bit1   ((CDEF_tUint32) CDEF_nu32Bit01_Suf)      /* Bitmask for bit  1 */
#define CDEF_nu32Bit2   ((CDEF_tUint32) CDEF_nu32Bit02_Suf)      /* Bitmask for bit  2 */
#define CDEF_nu32Bit3   ((CDEF_tUint32) CDEF_nu32Bit03_Suf)      /* Bitmask for bit  3 */
#define CDEF_nu32Bit4   ((CDEF_tUint32) CDEF_nu32Bit04_Suf)      /* Bitmask for bit  4 */
#define CDEF_nu32Bit5   ((CDEF_tUint32) CDEF_nu32Bit05_Suf)      /* Bitmask for bit  5 */
#define CDEF_nu32Bit6   ((CDEF_tUint32) CDEF_nu32Bit06_Suf)      /* Bitmask for bit  6 */
#define CDEF_nu32Bit7   ((CDEF_tUint32) CDEF_nu32Bit07_Suf)      /* Bitmask for bit  7 */
#define CDEF_nu32Bit8   ((CDEF_tUint32) CDEF_nu32Bit08_Suf)      /* Bitmask for bit  8 */
#define CDEF_nu32Bit9   ((CDEF_tUint32) CDEF_nu32Bit09_Suf)      /* Bitmask for bit  9 */
#define CDEF_nu32Bit10  ((CDEF_tUint32) CDEF_nu32Bit10_Suf)      /* Bitmask for bit 10 */
#define CDEF_nu32Bit11  ((CDEF_tUint32) CDEF_nu32Bit11_Suf)      /* Bitmask for bit 11 */
#define CDEF_nu32Bit12  ((CDEF_tUint32) CDEF_nu32Bit12_Suf)      /* Bitmask for bit 12 */
#define CDEF_nu32Bit13  ((CDEF_tUint32) CDEF_nu32Bit13_Suf)      /* Bitmask for bit 13 */
#define CDEF_nu32Bit14  ((CDEF_tUint32) CDEF_nu32Bit14_Suf)      /* Bitmask for bit 14 */
#define CDEF_nu32Bit15  ((CDEF_tUint32) CDEF_nu32Bit15_Suf)      /* Bitmask for bit 15 */
#define CDEF_nu32Bit16  ((CDEF_tUint32) CDEF_nu32Bit16_Suf)      /* Bitmask for bit 16 */
#define CDEF_nu32Bit17  ((CDEF_tUint32) CDEF_nu32Bit17_Suf)      /* Bitmask for bit 17 */
#define CDEF_nu32Bit18  ((CDEF_tUint32) CDEF_nu32Bit18_Suf)      /* Bitmask for bit 18 */
#define CDEF_nu32Bit19  ((CDEF_tUint32) CDEF_nu32Bit19_Suf)      /* Bitmask for bit 19 */
#define CDEF_nu32Bit20  ((CDEF_tUint32) CDEF_nu32Bit20_Suf)      /* Bitmask for bit 20 */
#define CDEF_nu32Bit21  ((CDEF_tUint32) CDEF_nu32Bit21_Suf)      /* Bitmask for bit 21 */
#define CDEF_nu32Bit22  ((CDEF_tUint32) CDEF_nu32Bit22_Suf)      /* Bitmask for bit 22 */
#define CDEF_nu32Bit23  ((CDEF_tUint32) CDEF_nu32Bit23_Suf)      /* Bitmask for bit 23 */
#define CDEF_nu32Bit24  ((CDEF_tUint32) CDEF_nu32Bit24_Suf)      /* Bitmask for bit 24 */
#define CDEF_nu32Bit25  ((CDEF_tUint32) CDEF_nu32Bit25_Suf)      /* Bitmask for bit 25 */
#define CDEF_nu32Bit26  ((CDEF_tUint32) CDEF_nu32Bit26_Suf)      /* Bitmask for bit 26 */
#define CDEF_nu32Bit27  ((CDEF_tUint32) CDEF_nu32Bit27_Suf)      /* Bitmask for bit 27 */
#define CDEF_nu32Bit28  ((CDEF_tUint32) CDEF_nu32Bit28_Suf)      /* Bitmask for bit 28 */
#define CDEF_nu32Bit29  ((CDEF_tUint32) CDEF_nu32Bit29_Suf)      /* Bitmask for bit 29 */
#define CDEF_nu32Bit30  ((CDEF_tUint32) CDEF_nu32Bit30_Suf)      /* Bitmask for bit 30 */
#define CDEF_nu32Bit31  ((CDEF_tUint32) CDEF_nu32Bit31_Suf)      /* Bitmask for bit 31 */

#ifndef   _CPU_16BIT_
#define CDEF_nu64Bit0   ((CDEF_tUint64) CDEF_nu64Bit00_Suf)      /* Bitmask for bit  0 */
#define CDEF_nu64Bit1   ((CDEF_tUint64) CDEF_nu64Bit01_Suf)      /* Bitmask for bit  1 */
#define CDEF_nu64Bit2   ((CDEF_tUint64) CDEF_nu64Bit02_Suf)      /* Bitmask for bit  2 */
#define CDEF_nu64Bit3   ((CDEF_tUint64) CDEF_nu64Bit03_Suf)      /* Bitmask for bit  3 */
#define CDEF_nu64Bit4   ((CDEF_tUint64) CDEF_nu64Bit04_Suf)      /* Bitmask for bit  4 */
#define CDEF_nu64Bit5   ((CDEF_tUint64) CDEF_nu64Bit05_Suf)      /* Bitmask for bit  5 */
#define CDEF_nu64Bit6   ((CDEF_tUint64) CDEF_nu64Bit06_Suf)      /* Bitmask for bit  6 */
#define CDEF_nu64Bit7   ((CDEF_tUint64) CDEF_nu64Bit07_Suf)      /* Bitmask for bit  7 */
#define CDEF_nu64Bit8   ((CDEF_tUint64) CDEF_nu64Bit08_Suf)      /* Bitmask for bit  8 */
#define CDEF_nu64Bit9   ((CDEF_tUint64) CDEF_nu64Bit09_Suf)      /* Bitmask for bit  9 */
#define CDEF_nu64Bit10  ((CDEF_tUint64) CDEF_nu64Bit10_Suf)      /* Bitmask for bit 10 */
#define CDEF_nu64Bit11  ((CDEF_tUint64) CDEF_nu64Bit11_Suf)      /* Bitmask for bit 11 */
#define CDEF_nu64Bit12  ((CDEF_tUint64) CDEF_nu64Bit12_Suf)      /* Bitmask for bit 12 */
#define CDEF_nu64Bit13  ((CDEF_tUint64) CDEF_nu64Bit13_Suf)      /* Bitmask for bit 13 */
#define CDEF_nu64Bit14  ((CDEF_tUint64) CDEF_nu64Bit14_Suf)      /* Bitmask for bit 14 */
#define CDEF_nu64Bit15  ((CDEF_tUint64) CDEF_nu64Bit15_Suf)      /* Bitmask for bit 15 */
#define CDEF_nu64Bit16  ((CDEF_tUint64) CDEF_nu64Bit16_Suf)      /* Bitmask for bit 16 */
#define CDEF_nu64Bit17  ((CDEF_tUint64) CDEF_nu64Bit17_Suf)      /* Bitmask for bit 17 */
#define CDEF_nu64Bit18  ((CDEF_tUint64) CDEF_nu64Bit18_Suf)      /* Bitmask for bit 18 */
#define CDEF_nu64Bit19  ((CDEF_tUint64) CDEF_nu64Bit19_Suf)      /* Bitmask for bit 19 */
#define CDEF_nu64Bit20  ((CDEF_tUint64) CDEF_nu64Bit20_Suf)      /* Bitmask for bit 20 */
#define CDEF_nu64Bit21  ((CDEF_tUint64) CDEF_nu64Bit21_Suf)      /* Bitmask for bit 21 */
#define CDEF_nu64Bit22  ((CDEF_tUint64) CDEF_nu64Bit22_Suf)      /* Bitmask for bit 22 */
#define CDEF_nu64Bit23  ((CDEF_tUint64) CDEF_nu64Bit23_Suf)      /* Bitmask for bit 23 */
#define CDEF_nu64Bit24  ((CDEF_tUint64) CDEF_nu64Bit24_Suf)      /* Bitmask for bit 24 */
#define CDEF_nu64Bit25  ((CDEF_tUint64) CDEF_nu64Bit25_Suf)      /* Bitmask for bit 25 */
#define CDEF_nu64Bit26  ((CDEF_tUint64) CDEF_nu64Bit26_Suf)      /* Bitmask for bit 26 */
#define CDEF_nu64Bit27  ((CDEF_tUint64) CDEF_nu64Bit27_Suf)      /* Bitmask for bit 27 */
#define CDEF_nu64Bit28  ((CDEF_tUint64) CDEF_nu64Bit28_Suf)      /* Bitmask for bit 28 */
#define CDEF_nu64Bit29  ((CDEF_tUint64) CDEF_nu64Bit29_Suf)      /* Bitmask for bit 29 */
#define CDEF_nu64Bit30  ((CDEF_tUint64) CDEF_nu64Bit30_Suf)      /* Bitmask for bit 30 */
#define CDEF_nu64Bit31  ((CDEF_tUint64) CDEF_nu64Bit31_Suf)      /* Bitmask for bit 31 */

#define CDEF_nu64Bit32  ((CDEF_tUint64) CDEF_nu64Bit32_Suf)      /* Bitmask for bit 32 */
#define CDEF_nu64Bit33  ((CDEF_tUint64) CDEF_nu64Bit33_Suf)      /* Bitmask for bit 33 */
#define CDEF_nu64Bit34  ((CDEF_tUint64) CDEF_nu64Bit34_Suf)      /* Bitmask for bit 34 */
#define CDEF_nu64Bit35  ((CDEF_tUint64) CDEF_nu64Bit35_Suf)      /* Bitmask for bit 35 */
#define CDEF_nu64Bit36  ((CDEF_tUint64) CDEF_nu64Bit36_Suf)      /* Bitmask for bit 36 */
#define CDEF_nu64Bit37  ((CDEF_tUint64) CDEF_nu64Bit37_Suf)      /* Bitmask for bit 37 */
#define CDEF_nu64Bit38  ((CDEF_tUint64) CDEF_nu64Bit38_Suf)      /* Bitmask for bit 38 */
#define CDEF_nu64Bit39  ((CDEF_tUint64) CDEF_nu64Bit39_Suf)      /* Bitmask for bit 39 */
#define CDEF_nu64Bit40  ((CDEF_tUint64) CDEF_nu64Bit40_Suf)      /* Bitmask for bit 40 */
#define CDEF_nu64Bit41  ((CDEF_tUint64) CDEF_nu64Bit41_Suf)      /* Bitmask for bit 41 */
#define CDEF_nu64Bit42  ((CDEF_tUint64) CDEF_nu64Bit42_Suf)      /* Bitmask for bit 42 */
#define CDEF_nu64Bit43  ((CDEF_tUint64) CDEF_nu64Bit43_Suf)      /* Bitmask for bit 43 */
#define CDEF_nu64Bit44  ((CDEF_tUint64) CDEF_nu64Bit44_Suf)      /* Bitmask for bit 44 */
#define CDEF_nu64Bit45  ((CDEF_tUint64) CDEF_nu64Bit45_Suf)      /* Bitmask for bit 45 */
#define CDEF_nu64Bit46  ((CDEF_tUint64) CDEF_nu64Bit46_Suf)      /* Bitmask for bit 46 */
#define CDEF_nu64Bit47  ((CDEF_tUint64) CDEF_nu64Bit47_Suf)      /* Bitmask for bit 47 */
#define CDEF_nu64Bit48  ((CDEF_tUint64) CDEF_nu64Bit48_Suf)      /* Bitmask for bit 48 */
#define CDEF_nu64Bit49  ((CDEF_tUint64) CDEF_nu64Bit49_Suf)      /* Bitmask for bit 49 */
#define CDEF_nu64Bit50  ((CDEF_tUint64) CDEF_nu64Bit50_Suf)      /* Bitmask for bit 50 */
#define CDEF_nu64Bit51  ((CDEF_tUint64) CDEF_nu64Bit51_Suf)      /* Bitmask for bit 51 */
#define CDEF_nu64Bit52  ((CDEF_tUint64) CDEF_nu64Bit52_Suf)      /* Bitmask for bit 52 */
#define CDEF_nu64Bit53  ((CDEF_tUint64) CDEF_nu64Bit53_Suf)      /* Bitmask for bit 53 */
#define CDEF_nu64Bit54  ((CDEF_tUint64) CDEF_nu64Bit54_Suf)      /* Bitmask for bit 54 */
#define CDEF_nu64Bit55  ((CDEF_tUint64) CDEF_nu64Bit55_Suf)      /* Bitmask for bit 55 */
#define CDEF_nu64Bit56  ((CDEF_tUint64) CDEF_nu64Bit56_Suf)      /* Bitmask for bit 56 */
#define CDEF_nu64Bit57  ((CDEF_tUint64) CDEF_nu64Bit57_Suf)      /* Bitmask for bit 57 */
#define CDEF_nu64Bit58  ((CDEF_tUint64) CDEF_nu64Bit58_Suf)      /* Bitmask for bit 58 */
#define CDEF_nu64Bit59  ((CDEF_tUint64) CDEF_nu64Bit59_Suf)      /* Bitmask for bit 59 */
#define CDEF_nu64Bit60  ((CDEF_tUint64) CDEF_nu64Bit60_Suf)      /* Bitmask for bit 60 */
#define CDEF_nu64Bit61  ((CDEF_tUint64) CDEF_nu64Bit61_Suf)      /* Bitmask for bit 61 */
#define CDEF_nu64Bit62  ((CDEF_tUint64) CDEF_nu64Bit62_Suf)      /* Bitmask for bit 62 */
#define CDEF_nu64Bit63  ((CDEF_tUint64) CDEF_nu64Bit63_Suf)      /* Bitmask for bit 63 */
#endif /* _CPU_16BIT_ */

#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_OFF 3429, 3453
/* switch of unjustified QA C Warning here
   as discussed with SW QS on 08.06.2004,
   macro should be used here instead of function*/
#endif

/* These are the background macros for the binary to literal converter API below.
   They are NOT part of the API, because its result is a 32/64bit literal which only uses
   the lower 8 bits. */
#define CDEF__nu32BinTo8_Suf(u32Binary) ( (0x80UL &(0x##u32Binary##UL >>21UL )) \
                                         |(0x40UL &(0x##u32Binary##UL >>18UL )) \
                                         |(0x20UL &(0x##u32Binary##UL >>15UL )) \
                                         |(0x10UL &(0x##u32Binary##UL >>12UL )) \
                                         |(0x8UL  &(0x##u32Binary##UL >> 9UL )) \
                                         |(0x4UL  &(0x##u32Binary##UL >> 6UL )) \
                                         |(0x2UL  &(0x##u32Binary##UL >> 3UL )) \
                                         |(0x1UL  &(0x##u32Binary##UL        ))  )

#ifndef   _CPU_16BIT_
#define CDEF__nu64BinTo8_Suf(u32Binary) ( (0x80ULL&(0x##u32Binary##ULL>>21ULL)) \
                                         |(0x40ULL&(0x##u32Binary##ULL>>18ULL)) \
                                         |(0x20ULL&(0x##u32Binary##ULL>>15ULL)) \
                                         |(0x10ULL&(0x##u32Binary##ULL>>12ULL)) \
                                         |(0x8ULL &(0x##u32Binary##ULL>> 9ULL)) \
                                         |(0x4ULL &(0x##u32Binary##ULL>> 6ULL)) \
                                         |(0x2ULL &(0x##u32Binary##ULL>> 3ULL)) \
                                         |(0x1ULL &(0x##u32Binary##ULL       ))  )
#endif /* _CPU_16BIT_ */


/* ___________________________________________________________________________________________
 *                                       binary to literal converters 
 */
/*
 * Description       : Converts the binary Expression ParBin into an unsigned integer literal
 *
 *                     Example:
 *                             CDEF_nu32Bin_Suf(11001001, 11101010, 11111111,00000001)
 *                             -> (0xC9DAFF01UL)
 *                     
 * Parameter:
 *    ParBinMSB         Binary coded number . Up to 8 binary digits (0 or 1) are allowed
 *                      Note: - Do NOT use a suffix like 1001UL or similar.
 *                            - Do NOT use hexadezimal syntax like 0x11011
 *                            - Using leading zeroes like 00101110 does not change the result
 *                              value, but might earn you QAC warnings.
 *    ParBinB<n>        see above, for the nth byte, counted from LSB to MSB
 *    ParBinLSB         see above, for the least significant byte
 *
 * Result   :           The result can be used as an unsigned integer literal, it is
 *                      understood by the pre-processor, which allows plausibility checks.
 */

/*  Only literals of at least 32bit width can be the result without a cast,
   because eight digits (interpreted as hex) need 29bit to begin with. Octal
   interpretation would reduce that to 22bit, but that's still a 32bit.      */
   
#define CDEF_nu32Bin_Suf(ParBinMSB,ParBinB3,ParBinB2,ParBinLSB)       \
/* returns UL-literal */ ((CDEF__nu32BinTo8_Suf(ParBinMSB)<<24UL)  |  \
                          (CDEF__nu32BinTo8_Suf(ParBinB3 )<<16UL)  |  \
                          (CDEF__nu32BinTo8_Suf(ParBinB2 )<< 8UL)  |  \
                          (CDEF__nu32BinTo8_Suf(ParBinLSB)      )   )

#ifndef   _CPU_16BIT_
#define CDEF_nu64Bin_Suf(ParBinMSB,ParBinB7,ParBinB6,ParBinB5,ParBinB4,ParBinB3,ParBinB2,ParBinLSB)\
/* returns ULL-literal */((CDEF__nu64BinTo8_Suf(ParBinMSB)<<56ULL) |  \
                          (CDEF__nu64BinTo8_Suf(ParBinB7) <<48ULL) |  \
                          (CDEF__nu64BinTo8_Suf(ParBinB6) <<40ULL) |  \
                          (CDEF__nu64BinTo8_Suf(ParBinB5) <<32ULL) |  \
                          (CDEF__nu64BinTo8_Suf(ParBinB4) <<24ULL) |  \
                          (CDEF__nu64BinTo8_Suf(ParBinB3) <<16ULL) |  \
                          (CDEF__nu64BinTo8_Suf(ParBinB2) << 8ULL) |  \
                          (CDEF__nu64BinTo8_Suf(ParBinLSB)       )  )
#endif /* _CPU_16BIT_ */


/* ___________________________________________________________________________________________
 *                                       binary to integer converters 
 */
/*
 * Description       : Converts the binary Expression ParBinB into a cast uint constant. 
 *                     Example:
 *                                CDEF_nu8Bin(11001001) -> ((CDEF_tUint8) 0xC9)
 * Parameter:
 *    ParBinB           Binary coded number . Up to 8 binary digits (0 or 1) are allowed
 *                      Note: - Do NOT use a suffix like 1001UL or similar.
 *                            - Do NOT use hexadezimal syntax like 0x11011
 *                            - Using leading zeroes like 00101110 does not change the result
 *                              value, but might earn you QAC warnings.
 * 
 * Result   :           A type cast is performed on the result to ensure the uint<n> type.
 */
#define CDEF_nu8Bin(ParBinB)                                                     \
/* returns uint8  */         ((CDEF_tUint8)CDEF__nu32BinTo8_Suf(ParBinB))

#define CDEF_nu16Bin(ParBinMSB,ParBinLSB)                                         \
/* returns uint16 */ ( (((CDEF_tUint16)CDEF__nu32BinTo8_Suf(ParBinMSB))<<8UL)|    \
                       (((CDEF_tUint16)CDEF__nu32BinTo8_Suf(ParBinLSB))     ) )

#define CDEF_nu32Bin(ParBinMSB,ParBinB3,ParBinB2,ParBinLSB)                       \
/* returns uint32 */         ((CDEF_tUint32)CDEF_nu32Bin_Suf(ParBinMSB,ParBinB3,  \
                                                             ParBinB2 ,ParBinLSB)) 

#ifndef   _CPU_16BIT_
#define CDEF_nu64Bin(ParBinMSB,ParBinB7,ParBinB6,ParBinB5,ParBinB4,ParBinB3,ParBinB2,ParBinLSB) \
/* returns uint64 */ ((CDEF_tUint64)CDEF_nu64Bin_Suf(ParBinMSB,ParBinB7,                        \
                                                     ParBinB6, ParBinB5,                        \
                                                     ParBinB4, ParBinB3,                        \
                                                     ParBinB2, ParBinLSB)) 
#endif /* _CPU_16BIT_ */

#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_ON 3429, 3453
#endif

#endif        /* end of  --->   #ifndef CDEF_COMMON_H                                       */

 /*********************************************************************************************
 *   Revision List:
 *   (Will be filled by MKS for CDS releases, 
 *    feel free to delete this for use in GIT or project MKS.)
 *         Archive:
 * $Log: cdef_common.h  $
 * Revision 1.11 2017/05/04 14:52:32CEST Helk, Aribert (uidf2456) 
 * CDEF: copyright information missing
 * Revision 1.10 2017/02/16 10:58:22CET Rademacher, Jan (uidf3054) 
 * JIRA ticket OSP-3587: CDEF: introduce code file
 * - move "once-only" constructs to newly introduced code file
 * _
 * Revision 1.9 2017/02/15 16:38:18CET Rademacher, Jan (uidf3054) 
 * - change according to SQE recommendation: remove changelog form start of file
 *   in order to avoid problems by mismatching line numbers due to keyword expansion
 * - keep changelog at end of file, at least in the MKS-based CDS release,
 *   invite users to delete for use in GIT or customer department MKS
 * _
 * Revision 1.8 2017/02/15 16:12:37CET Rademacher, Jan (uidf3054) 
 * JIRA ticket CDS-1024: CDEF: QAC warnings to solve
 * - silence both warnings, via pragma and via syntax comment, with individual reasoning
 * - also complete the duality of the two representation for similar symbols 32bit and 64bit
 * _
 * Revision 1.7 2016/08/05 10:35:34CEST Rademacher, Jan (uidf3054) 
 * OSP-3524 CDEF: signed CDEF_nu8Max, CDEF_nu16Max; to be unsigned
 * Add "U" twice and add a comment why more cannot be done.
 * Revision 1.6 2014/06/30 13:56:52CEST Rademacher, Jan (uidf3054) 
 * - JIRA OSP 1749 CDEF cleaner name spaces
 *   - introduce symbols with prefix
 *     - use them for deriving other symbols
 *     - avoid non-API symbols in plausibility checks
 *   - move prefix-less symbols to deprecated,
 *     including internal non-API symbols
 *   - define new symbols as SMK-frinedly and QAC-friendly as possible
 *     - define symbols explicitly, only verify against available information
 *       (this allows to silence the QAC messages inside cdef,
 *         instead only in clients, i.e. less noisy QAC for clients)
 *     - complain if no information for verification is available
 *     - only define symbols if not yet defined by headers
 *       which happen to be visible, or by C++
 *     - do not include now unneeded "useful" headers
 *       (but tolerate if they happen to be visible)
 *     - avoid internal "adaptation symbols" being visible
 * - fix an asymmetric QAC OFF-> symmetric QAC ON
 * - improve symmetry of #if/#endif with comments
 * _
 * Revision 1.5 2014/05/13 14:58:46CEST Rademacher, Jan (uidf3054) 
 * - CDS MR 11103 CDEF: Add typedef for 32bit float type to cdef
 *   and JIRA issue OSP-1472 CDEF: Add typedef for 32bit float type to cdef
 *   - add a conditional typedef for 32bit float and double-conditional for 64bit double
 *   - condition on FPU presence and on 64bit presence
 *   - add a comment on this
 * Revision 1.4 2013/08/22 15:24:25CEST Meganathan, Dinesh Kumar (uidt8287) 
 * Turned Off QAC warings connected to CDEF__STATIC_ASSERT for
 * ID RD SW MR 14057: CDEF avoid/silence QAC warnings on static assertions
 * Revision 1.3 2013/06/26 10:47:45CEST Meganathan, Dinesh Kumar (uidt8287) 
 * Incorporation of ORM_0022_JGP2014_CDEF_Code
 *   - changed #if for include of stdint.h
 *   - improved comments
 *   - added static-assertions for size of the types and their limits
 *   - limits are not redefined if already present, but checked
 *   added
 *   #include <stdint.h>
 *   for iMX6
 * Revision 1.2 2011/07/11 15:02:04CEST Rademacher, Jan (uidf3054) 
 * CDS MR 9940 CDEF: add a compiler switch to remove the keywords for IAR compiler
 * - according to CCB: Unconditionally remove __short, __far, __near.
 * _
 * Revision 1.1 2010/07/20 11:00:22CEST Rademacher Jan (uidf3054) (uidf3054) 
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
 ********************************************************************************************/

/*====================================== END OF FILE =======================================*/

