/***************************************************************************
*=====================      Copyright by Continental AG      ===============
****************************************************************************
* Title        : CDEF.H
*
*   Description: Separate little-endian-specific definitions
*                (the endionaess independent definitions are inside
*                 cdef_common.h)
*      Platform: LITTLE ENDIAN platforms up to 64-bit bus bit width
*                  see CDEF_chips.txt
*
* Environment  : little endian
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



#ifndef INC_CDEF_H
#define INC_CDEF_H

#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_OFF 841,3410,3413,3429
/* switch of warnings which are likely to be triggered by compiler headers */
#endif

/* System-headerfiles */
#include <limits.h>            /* type range definitions                                  */
#include <stddef.h>            /* e.g. NULL, size_t, ptrdiff_t , ...                      */

#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_ON  841,3410,3413,3429
#endif

#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_OFF 3412, 3429
/* QAC message 3429:  switch off unjustified QAC Warning as discussed
                      with SW QS on 08.06.2004, macro should be used here
                                          instead of function.
   QAC message 3412:  CDEF_STATIC_ASSERT contains key words and special
                      characters in order to implement a special construct
                      for static assertions.
               
*/
#endif /* QAC_Analysis */
/* This static assertion is taken from ASRL as we cannot include ASRL inside CDEF.
 * If you plan to use static assertions, please use the one provided by ASRL.
 */
#define CDEF__STATIC_ASSERT(ParExpr, ParName)   struct ParName {struct ParName* CDEF__ST_ASS_MEMBER[(ParExpr)?1:-1];}

#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_ON 3412, 3429
#endif /* QAC_Analysis */

/* Own headers files */
#include "cdef_ce.h"           /* externally visible configurations                       */
#include "cdef_common.h"       /* anything endianess-independent                          */

/* ___________________________________________________________________________________________
 *                                    check endianess 
 */    
#ifdef _BIG_ENDIAN_
     #error BIG ENDIAN is not supported. Possible reasons: Target CPU does not support selected endianess, wrong variant of the Cdef.h included in project or wrong project build settings.
#endif

#ifndef _LITTLE_ENDIAN_
#define _LITTLE_ENDIAN_  
#endif

#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_OFF 750, 3621, 1314
#endif
/* The union type specifier is the only way to provide both, an endianess-abstracting view 
 * and a total view on the endianess affected parts of the type.
 */
/* Bit structures are the only way to provide endianess abstraction for bits. */
/* Structure definitions inside structures are the ancient cdef-way.
 * Cdef could be rewritten to define outside and use inside, but that would be
 * too much risk and effort. */

#ifdef WIN32
/* For windows-simulation, switch off warning 4214, reason: see QAC-comment above
  warning C4214: nonstandard extension used : bit field types other than int */
#pragma warning ( disable : 4214 )
#endif
/* ___________________________________________________________________________________________
 *                             bit and byte access types
 */
typedef 
/** -------------------------------------------------------------------------------------------
*  Description      : Bitfield data type with 8 bits. The following bitfield depends on
*                     the compiler and machine! The defined structure has to be redefined
*                     for other compilers, so that the following conditions are correct:
*                     bit8 xBit8
*                     xBit8.u8 == ( xBit8.F._7 <<7 ) + 
*                                 ( xBit8.F._6 <<6 ) + ...
*                                 ( xBit8.F._0 <<0 ) 
* @alias bit8
* @shortDesc Bitfield data type with 8 bits.
* @order 10
*/
union CDEF_bit8_tag
   {
   /** u8 provides 8bit wide access
     *   (B is a deprecated synonym)
     */
   CDEF_tUint8 u8;                      

#ifdef CDEF_DEPRECATED_TYPES
   /*# u8 provides 8bit wide access
     *   (B is a deprecated synonym)
     */
   CDEF_tUint8 B;                      
#endif

   /** F is a bitfield for single bit access to each bit
    * @alias F
    */
   struct CDEF_F8_tag
      {
      /** least significant bit. range = 0..1 */
      CDEF_tu8BitField _0 : 1;
      CDEF_tu8BitField _1 : 1;
      CDEF_tu8BitField _2 : 1;
      CDEF_tu8BitField _3 : 1;
                                                                               /* B2-sync 00 */
      CDEF_tu8BitField _4 : 1;
      CDEF_tu8BitField _5 : 1;
      CDEF_tu8BitField _6 : 1;
      /** most significant bit. range = 0..1  */    
      CDEF_tu8BitField _7 : 1;
      } F;                       /* Flag access identifier              */
   } CDEF_tstBit8;

typedef 
/** -------------------------------------------------------------------------------------------
 * Description         : Bitfield data type with 16 bits
 *                       The following bitfield depends on the compiler
 *                       and machine! The defined structure has to be
 *                       redefined for other compilers, so that the following
 *                       conditions are correct:
 *                       bit16 xBit16
 *                       xBit16.u16    == ( xBit16.u8View._1  <<8  ) +
 *                                        ( xBit16.u8View._0  <<0  )
 *                       xBit16.u16    == ( xBit16.F._15 <<15 ) +
 *                                        ( xBit16.F._14 <<14 ) + ...
 *                                        ( xBit16.F._0  <<0  )
 * @alias bit16
 * @shortDesc Bitfield data type with 16 bits.
 * @order 20
 */
union CDEF_bit16_tag
   {
   /** u16 provides 16bit wide access
          (W is a deprecated synonym) */    
   CDEF_tUint16 u16;
   
#ifdef CDEF_DEPRECATED_TYPES
   /*# u16 provides 16bit wide access
          (W is a deprecated synonym) */    
   CDEF_tUint16 W;
#endif

   /** u8View provides 8bit wide access, twice within 16bit
     *     (stB is a deprecated synonym)
     * @alias u8View
     */
   struct CDEF_u8ViewIn16_tag
      {
                                                                               /* B2-sync 01 */
      /**  least significant 8bit (Bits 0 to  7) . Range  0..255     */    
      CDEF_tUint8 _0;
                                                                               /* B2-sync 02 */
      /**   most significant 8bit (Bits 8 to 15) . Range  0..255     */    
      CDEF_tUint8 _1;
                                                                               /* B2-sync 03 */
      }
#ifdef CDEF_DEPRECATED_TYPES
   B,
#endif
	   u8View;
   
   /** F is a bitfield for single bit access to each bit
    * @alias F
    */
   struct CDEF_F16_tag
      {
      /** least significant bit. Range = 0..1 */    
      CDEF_tu16BitField _0 : 1;
      CDEF_tu16BitField _1 : 1;
      CDEF_tu16BitField _2 : 1;
      CDEF_tu16BitField _3 : 1;
      CDEF_tu16BitField _4 : 1;
      CDEF_tu16BitField _5 : 1;
      CDEF_tu16BitField _6 : 1;
      CDEF_tu16BitField _7 : 1; 
                                                                               /* B2-sync 04 */      
      CDEF_tu16BitField _8 : 1; 
      CDEF_tu16BitField _9 : 1;
      CDEF_tu16BitField _10: 1;
      CDEF_tu16BitField _11: 1;
      CDEF_tu16BitField _12: 1;
      CDEF_tu16BitField _13: 1;
      CDEF_tu16BitField _14: 1;
      /**  most significant bit. Range = 0..1 */    
      CDEF_tu16BitField _15: 1;
      } F;                      /* Flag access identifier                       */
   } CDEF_tstBit16;

typedef 
/** -------------------------------------------------------------------------------------------
 * Description         : Bitfield data type with 32 bits
 *                       The following bitfield depends on the compiler
 *                       and machine! The defined structure has to be
 *                       redefined for other compilers, so that the following
 *                       conditions are correct:
 *                       bit32 xBit32
 *                       xBit32.u32    == ( xBit32.u16View._1 <<16 ) +
 *                                        ( xBit32.u16View._0 <<0  )
 *                       xBit32.u32    == ( xBit32.u8View._3  <<24 ) +
 *                                        ( xBit32.u8View._2  <<16 ) +
 *                                        ( xBit32.u8View._1  <<8  ) +
 *                                        ( xBit32.u8View._0  <<0  )
 *                       xBit32.u32    == ( xBit32.F._31      <<31 ) +
 *                                        ( xBit32.F._30      <<30 ) + ...
 *                                        ( xBit32.F._0       <<0  )
 * @alias bit32
 * @shortDesc Bitfield data type with 32 bits.
 * @order 30
 */
union CDEF_bit32_tag
   {
   /** u32 provides 32bit wide access
          (DW is a deprecated synonym) */
   CDEF_tUint32 u32;
       
#ifdef CDEF_DEPRECATED_TYPES
   /*# u32 provides 32bit wide access
          (DW is a deprecated synonym) */
   CDEF_tUint32 DW;
#endif

   /** u16View provides 16bit wide access, twice within 32bit
     *     (W is a deprecated synonym)
     * @alias u16View
     */
   struct CDEF_u16ViewIn32_tag
      {
      /**  least significant 16bit (Bits  0 to 15) . Range  0..65535  */    
      CDEF_tUint16 _0;
                                                                               /* B2-sync 05 */
      /**   most significant 16bit (Bits 16 to 31) . Range  0..65535  */          
      CDEF_tUint16 _1;
      }
#ifdef CDEF_DEPRECATED_TYPES
   W,
#endif
	   u16View;

   /** u8View provides 8bit wide access, four times within 32bit
     *     (stB is a deprecated synonym)
     * @alias u8View
     */
   struct CDEF_u8ViewIn32_tag
      {
      /** least significant 8bit (Bits  0 to  7) . Range  0..255       */    
      CDEF_tUint8 _0;
      CDEF_tUint8 _1; 
                                                                               /* B2-sync 06 */
      CDEF_tUint8 _2;

      /**  most significant 8bit (Bits 24 to 31) . Range  0..255       */    
      CDEF_tUint8 _3;                    
      }
#ifdef CDEF_DEPRECATED_TYPES
   B,
#endif
	   u8View;


   /** F is a bitfield for single bit access to each bit
    * @alias F
    */
   struct CDEF_F32_tag
      {
      /** least significant bit. Range = 0..1 */    
      CDEF_tu32BitField _0 : 1;
      CDEF_tu32BitField _1 : 1;
      CDEF_tu32BitField _2 : 1;
      CDEF_tu32BitField _3 : 1;
      CDEF_tu32BitField _4 : 1;
      CDEF_tu32BitField _5 : 1;
      CDEF_tu32BitField _6 : 1;
      CDEF_tu32BitField _7 : 1;
                                                                               /* B2-sync 07 */
      CDEF_tu32BitField _8 : 1;
      CDEF_tu32BitField _9 : 1;
      CDEF_tu32BitField _10: 1;
      CDEF_tu32BitField _11: 1;
      CDEF_tu32BitField _12: 1;
      CDEF_tu32BitField _13: 1;
      CDEF_tu32BitField _14: 1;
      CDEF_tu32BitField _15: 1;
                                                                               /* B2-sync 08 */
      CDEF_tu32BitField _16: 1;
      CDEF_tu32BitField _17: 1;
      CDEF_tu32BitField _18: 1;
      CDEF_tu32BitField _19: 1;
      CDEF_tu32BitField _20: 1;
      CDEF_tu32BitField _21: 1;
      CDEF_tu32BitField _22: 1;
      CDEF_tu32BitField _23: 1;
                                                                               /* B2-sync 09 */
      CDEF_tu32BitField _24: 1;
      CDEF_tu32BitField _25: 1;
      CDEF_tu32BitField _26: 1;
      CDEF_tu32BitField _27: 1;
      CDEF_tu32BitField _28: 1;
      CDEF_tu32BitField _29: 1;
      CDEF_tu32BitField _30: 1;
      /** most significant bit. Range = 0..1 */
      CDEF_tu32BitField _31: 1;
      } F;                      /* Flag access identifier                       */
   } CDEF_tstBit32;

#ifndef _CPU_16BIT_
typedef 
/** -------------------------------------------------------------------------------------------
 * Description         : Bitfield data type with 64 bits
 *                       The following bitfield depends on the compiler
 *                       and machine! The defined structure has to be
 *                       redefined for other compilers, so that the following
 *                       conditions are correct:
 *                       bit64 xBit64
 *                       xBit64.LLW   == ( xBit64.u32View._1 <<32 ) +
 *                                       ( xBit64.u32View._0 <<0  )
 *                       xBit64.LLW   == ( xBit64.u16View._3 <<48 ) +
 *                                       ( xBit64.u16View._2 <<32 )
 *                                       ( xBit64.u16View._1 <<16 )
 *                                       ( xBit64.u16View._0 <<0  )
 *                       xBit64.LLW   == ( xBit64.u8View._8  <<56 ) +
 *                                       ( xBit64.u8View._7  <<48 ) + ...
 *                                       ( xBit64.u8View._1  <<8  ) +
 *                                       ( xBit64.u8View._0  <<0  )
 *                       xBit64.LLW   == ( xBit64.F._63      <<63 ) +
 *                                       ( xBit64.F._62      <<62 ) + ...
 *                                       ( xBit64.F._0       <<0  )
 * @alias bit64
 * @shortDesc Bitfield data type with 64 bits.
 * @order 40
 */
union CDEF_bit64_tag
   {
   /** u64 provides 64bit wide access
          (LLW is a deprecated synonym) */
    CDEF_tUint64 u64;
       
#ifdef CDEF_DEPRECATED_TYPES
   /*# u64 provides 64bit wide access
          (LLW is a deprecated synonym) */
    CDEF_tUint64 LLW;
#endif

   /** u32View provides 32bit wide access, twice within 64bit
     *     (DW is a deprecated synonym)
     * @alias u32View
     */
   struct CDEF_u32ViewIn64_tag
      {
      /**  least significant 32bit (Bits  0 to 31) . Range  0..4294697295 */    
      CDEF_tUint32 _0;
                                                                               /* B2-sync 10 */
      /**   most significant 32bit (Bits 32 to 63) . Range  0..4294697295 */    
      CDEF_tUint32 _1;
      }
#ifdef CDEF_DEPRECATED_TYPES
   DW,
#endif
	   u32View;
      
   /** u16View provides 16bit wide access, four times within 64bit
     *     (W is a deprecated synonym)
     * @alias u16View
     */
   struct CDEF_u16ViewIn64_tag
      {
      /** least significant 16bit (Bits  0 to 15) . Range  0..65535         */
      CDEF_tUint16 _0;
      CDEF_tUint16 _1;
                                                                               /* B2-sync 11 */
      CDEF_tUint16 _2;
      /**  most significant 16bit (Bits 48 to 63) . Range  0..65535         */       
      CDEF_tUint16 _3;
      }
#ifdef CDEF_DEPRECATED_TYPES
   W,
#endif
	   u16View;

   /** u8View provides 8bit wide access, eight times within 64bit
     *     (stB is a deprecated synonym)
     * @alias u8View
     */
   struct CDEF_u8ViewIn64_tag
      {
      /** least significant 8bit (Bits  0 to  7) . Range  0..255            */    
      CDEF_tUint8 _0;
      CDEF_tUint8 _1;
      CDEF_tUint8 _2;
      CDEF_tUint8 _3;
                                                                               /* B2-sync 12 */
      CDEF_tUint8 _4;
      CDEF_tUint8 _5;
      CDEF_tUint8 _6;
      /**  most significant 8bit (Bits 56 to 63) . Range  0..255            */    
      CDEF_tUint8 _7;
      }
#ifdef CDEF_DEPRECATED_TYPES
   B,
#endif
	   u8View;
      
   /** F is a bitfield for single bit access to each bit
    * @alias F
    */
   struct CDEF_F64_tag
      {
      /** least significant bit. Range = 0..1      */    
      CDEF_tu64BitField _0: 1;
      CDEF_tu64BitField _1: 1;
      CDEF_tu64BitField _2: 1;
      CDEF_tu64BitField _3: 1;
      CDEF_tu64BitField _4: 1;
      CDEF_tu64BitField _5: 1;
      CDEF_tu64BitField _6: 1;
      CDEF_tu64BitField _7: 1;
                                                                               /* B2-sync 13 */
      CDEF_tu64BitField _8: 1;
      CDEF_tu64BitField _9: 1;
      CDEF_tu64BitField _10: 1;
      CDEF_tu64BitField _11: 1;
      CDEF_tu64BitField _12: 1;
      CDEF_tu64BitField _13: 1;
      CDEF_tu64BitField _14: 1;
      CDEF_tu64BitField _15: 1;
                                                                               /* B2-sync 14 */
      CDEF_tu64BitField _16: 1;
      CDEF_tu64BitField _17: 1;
      CDEF_tu64BitField _18: 1;
      CDEF_tu64BitField _19: 1;
      CDEF_tu64BitField _20: 1;
      CDEF_tu64BitField _21: 1;
      CDEF_tu64BitField _22: 1;
      CDEF_tu64BitField _23: 1;
                                                                               /* B2-sync 15 */
      CDEF_tu64BitField _24: 1;
      CDEF_tu64BitField _25: 1;
      CDEF_tu64BitField _26: 1;
      CDEF_tu64BitField _27: 1;
      CDEF_tu64BitField _28: 1;
      CDEF_tu64BitField _29: 1;
      CDEF_tu64BitField _30: 1;
      CDEF_tu64BitField _31: 1;
                                                                               /* B2-sync 16 */
      CDEF_tu64BitField _32: 1;
      CDEF_tu64BitField _33: 1;
      CDEF_tu64BitField _34: 1;
      CDEF_tu64BitField _35: 1;
      CDEF_tu64BitField _36: 1;
      CDEF_tu64BitField _37: 1;
      CDEF_tu64BitField _38: 1;
      CDEF_tu64BitField _39: 1;
                                                                               /* B2-sync 17 */
      CDEF_tu64BitField _40: 1;
      CDEF_tu64BitField _41: 1;
      CDEF_tu64BitField _42: 1;
      CDEF_tu64BitField _43: 1;
      CDEF_tu64BitField _44: 1;
      CDEF_tu64BitField _45: 1;
      CDEF_tu64BitField _46: 1;
      CDEF_tu64BitField _47: 1;
                                                                               /* B2-sync 18 */
      CDEF_tu64BitField _48: 1;
      CDEF_tu64BitField _49: 1;
      CDEF_tu64BitField _50: 1;
      CDEF_tu64BitField _51: 1;
      CDEF_tu64BitField _52: 1;
      CDEF_tu64BitField _53: 1;
      CDEF_tu64BitField _54: 1;
      CDEF_tu64BitField _55: 1;
                                                                               /* B2-sync 19 */
      CDEF_tu64BitField _56: 1;
      CDEF_tu64BitField _57: 1;
      CDEF_tu64BitField _58: 1;
      CDEF_tu64BitField _59: 1;
      CDEF_tu64BitField _60: 1;
      CDEF_tu64BitField _61: 1;
      CDEF_tu64BitField _62: 1;
      /**  most significant bit. Range = 0..1      */
      CDEF_tu64BitField _63: 1;
      } F;                      /* Flag access identifier                   */
   } CDEF_tstBit64;
#endif

#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_ON 3621
#endif /* QAC_Analysis */

#ifdef WIN32
/* For windows-simulation, enable the warning again if disabled before. */
#pragma warning ( default : 4214 )
#endif


typedef 
/** -------------------------------------------------------------------------------------------
 * Description         : 16bit data type with single accessable 8bit members
 *                       The following bitfield depends on the compiler
 *                       and machine! The defined structure has to be
 *                       redefined for other compilers, so that the following
 *                       conditions are correct:
 *                       tunU16Rec xRec16
 *                       xRec16.u16 == ( xRec16.u8View.u8B1 <<8 ) +
 *                                     ( xRec16.u8View.u8B0 <<0 )
 * @alias tunU16Rec
 * @shortDesc 16bit data type with single accessable 8bit members
 * @order 50
 */
union CDEF_tunU16Rec_tag
   {
   /** u16 provides 16bit wide access
          (u16W is a deprecated synonym) */
   CDEF_tUint16 u16;

#ifdef CDEF_DEPRECATED_TYPES
   /*# u16 provides 16bit wide access
          (u16W is a deprecated synonym) */
   CDEF_tUint16 u16W, wW;
#endif

    /** u8View provides 8bit wide access, twice within 16bit
     *     (stB is a deprecated synonym)
     * @alias u8View
     */
   struct CDEF_u8ViewIn16Rec_tag
      {
      /* least significant 8bit (Bits 0 to  7) . Range  0..255 */    
      CDEF_tUint8   u8B0;
                                                                               /* B2-sync 20 */
      /*  most significant 8bit (Bits 8 to 15) . Range  0..255 */    
      CDEF_tUint8   u8B1;
      }
#ifdef CDEF_DEPRECATED_TYPES
   stB,
#endif
	   u8View;
   } CDEF_tunU16Rec;

typedef 
/** -------------------------------------------------------------------------------------------
 * Description         : 32bit data type with single accessable 16bit and 8bit members
 *                       The following bitfield depends on the compiler
 *                       and machine! The defined structure has to be
 *                       redefined for other compilers, so that the following
 *                       conditions are correct:
 *                       tunU32Rec xRec32
 *                       xRec32.stW.u16W0 == ( xRec32.u8View.u8B1   <<8  ) +
 *                                           ( xRec32.u8View.u8B0   <<0  )
 *                       xRec32.stW.u16W1 == ( xRec32.u8View.u8B3   <<8  ) +
 *                                           ( xRec32.u8View.u8B2   <<0  )
 *                       xRec32.u32       == ( xRec32.u16View.u16W1 <<16 ) +
 *                                           ( xRec32.u16View.u16W0 <<0  )
 * @alias tunU32Rec
 * @shortDesc 32bit data type with single accessable 16bit and 8bit members
 * @order 60
 */
union CDEF_tunU32Rec_tag
   {
   /** u32 provides 32bit wide access
          (u32DW is a deprecated synonym) */
   CDEF_tUint32 u32;

#ifdef CDEF_DEPRECATED_TYPES
   /*# u32 provides 32bit wide access
          (u32DW is a deprecated synonym) */
   CDEF_tUint32 u32DW, dwD;
#endif

   /** u16View provides 16bit wide access, twice within 32bit
     *     (stW is a deprecated synonym)
     * @alias u16View
     */
   struct CDEF_u16ViewIn32Rec_tag
      {
      /** least significant 16bit. Range = 0 .. 65535                     */    
      CDEF_tUint16   u16W0;
                                                                               /* B2-sync 21 */
      /**  most significant 16bit  Range = 0 .. 65535                     */    
      CDEF_tUint16   u16W1;
      }
#ifdef CDEF_DEPRECATED_TYPES
   stW,
#endif
	   u16View;

   /** u8View provides 8bit wide access, four times within 32bit
     *     (stB is a deprecated synonym)
     * @alias u8View
     */
   struct CDEF_u8ViewIn32Rec_tag
      {
      /** least significant 8bit of least significant 16bit
          = least significant 8bit of all                 */    
      CDEF_tUint8   u8B0;
                                                                               /* B2-sync 22 */
      /**  most significant 8bit of least significant 16bit */    
      CDEF_tUint8   u8B1;
                                                                               /* B2-sync 23 */
      /** least significant 8bit of  most significant 16bit */    
      CDEF_tUint8   u8B2;
                                                                               /* B2-sync 24 */
      /**  most significant 8bit of  most significant 16bit
          = most significant 8bit                         */    
      CDEF_tUint8   u8B3;
      }
#ifdef CDEF_DEPRECATED_TYPES
   stB,
#endif
	   u8View;
   } CDEF_tunU32Rec;
   
#ifndef _CPU_16BIT_
typedef
/** -------------------------------------------------------------------------------------------
 * Description         : 64bit data type
 *                       with single accessable 32bit,16bit and 8bit members
 *                       The following bitfield depends on the compiler
 *                       and machine! The defined structure has to be
 *                       redefined for other compilers, so that the following
 *                       conditions are correct:
 *                       tunU64Rec xRec64
 *                       xRec64.u16View.u16W0  == ( xRec64.stB.u8B1  <<8  ) +
 *                                                ( xRec64.stB.u8B0  <<0  )
 *                       xRec64.u16View.u16W1  == ( xRec64.stB.u8B3  <<8  ) +
 *                                                ( xRec64.stB.u8B2  <<0  )
 *                       xRec64.u16View.u16W2  == ( xRec64.stB.u8B5  <<8  ) +
 *                                                ( xRec64.stB.u8B4  <<0  )
 *                       xRec64.u16View.u16W3  == ( xRec64.stB.u8B7  <<8  ) +
 *                                                ( xRec64.stB.u8B6  <<0  )
 *                       xRec64.u32View.u32DW0 == ( unLLW.stW.u16HW1 <<16 ) +
 *                                                ( unLLW.stW.u16HW0 <<0  )
 *                       xRec64.u32View.u32DW1 == ( unLLW.stW.u16HW3 <<16 ) +
 *                                                ( unLLW.stW.u16HW2 <<0  )
 *                       xRec64.u64            == ( unLLW.stDW.u32W1 <<32 ) +
 *                                                ( unLLW.stDW.u32W0 <<0  )
 * @shortDesc 64bit data type with single accessable 32bit,16bit and 8bit members
 * @alias tunU64Rec
 * @order 70
 */

 union CDEF_tunU64Rec_tag
   {
   /** u64 provides 64bit wide access
          (u64LLW is a deprecated synonym) */
   CDEF_tUint64 u64;

#ifdef CDEF_DEPRECATED_TYPES
   /*# u64 provides 64bit wide access
          (u64LLW is a deprecated synonym) */
   CDEF_tUint64 u64LLW;
#endif

   /** u32View provides 32bit wide access, twice within 64bit
     *     (stDW is a deprecated synonym)
     * @alias u32View
     */
   struct CDEF_u32ViewIn64Rec_tag
      {
      /** least significant 32bit. Range = 0 .. 4294697295          */    
      CDEF_tUint32   u32DW0;
                                                                               /* B2-sync 25 */
      /**  most significant 32bit. Range = 0 .. 4294697295          */    
      CDEF_tUint32   u32DW1;
      }
#ifdef CDEF_DEPRECATED_TYPES
   stDW,
#endif
	   u32View;

   /** u16View provides 16bit wide access, four times within 64bit
     *     (stW is a deprecated synonym)
     * @alias u16View
     */
   struct CDEF_u16ViewIn64Rec_tag
      {
      /** least significant 16bit of the least significant 32bit    */    
      CDEF_tUint16   u16W0;
                                                                               /* B2-sync 26 */
      /**  most significant 16bit of the least significant 32bit    */    
      CDEF_tUint16   u16W1;
                                                                               /* B2-sync 27 */
      /** least significant 16bit of the  most significant 32bit    */    
      CDEF_tUint16   u16W2;
                                                                               /* B2-sync 28 */
      /**  most significant 16bit of the  most significant 32bit    */    
      CDEF_tUint16   u16W3;
      }
#ifdef CDEF_DEPRECATED_TYPES
   stW,
#endif
	   u16View;

   /** u8View provides 8bit wide access, eight times within 64bit
     *     (stB is a deprecated synonym)
     * @alias u8View
     */
   struct CDEF_u8ViewIn64Rec_tag
      {
      /** least significant 8bit of least significant 16bit
          = least significant 8bit of all                      */    
      CDEF_tUint8   u8B0;
                                                                               /* B2-sync 29 */
      /**  most significant 8bit of least significant 16bit    */    
      CDEF_tUint8   u8B1;
      CDEF_tUint8   u8B2;
      CDEF_tUint8   u8B3;
                                                                               /* B2-sync 30 */
      CDEF_tUint8   u8B4;
      CDEF_tUint8   u8B5;
                                                                               /* B2-sync 31 */
      /**  least significant 8bit of  most significant  16bit  */    
      CDEF_tUint8   u8B6;
                                                                               /* B2-sync 32 */
      /**  most significant  8bit of  most significant  16bit
          = most significant 8bit of all                       */          
      CDEF_tUint8   u8B7;
      }
#ifdef CDEF_DEPRECATED_TYPES
   stB,
#endif
	   u8View;
   } CDEF_tunU64Rec;
#endif
#ifdef QAC_Analysis
#pragma PRQA_MESSAGES_ON 750, 1314
#endif

/* Including this after definitions of endianess-depending definitions
 * is necessary, because the content depends on the endianess symbols. */
#include "cdef_deprecated.h"

#endif        /* end of  --->   #ifndef INC_CDEF_H                                          */


 /*********************************************************************************************
 *   Revision List:
 *   (Will be filled by MKS for CDS releases, 
 *    feel free to delete this for use in GIT or project MKS.)
 *         Archive:
 * $Log: cdef.h  $
 * Revision 1.14 2017/06/28 11:36:30CEST Rademacher, Jan (uidf3054) 
 * OSP-6659 CDEF: copyright information missing
 * - update file header
 * _
 * Revision 1.13 2017/05/04 14:52:59CEST Helk, Aribert (uidf2456) 
 * CDEF: copyright information missing
 * Revision 1.12 2017/02/15 16:50:28CET Rademacher, Jan (uidf3054) 
 * - change according to SQE recommendation: remove changelog form start of file
 *    in order to avoid problems by mismatching line numbers due to keyword expansion
 *  - keep changelog at end of file, at least in the MKS-based CDS release,
 *    invite users to delete for use in GIT or customer department MKS
 *  _
 * Revision 1.11 2016/07/01 13:35:27CEST Rademacher, Jan (uidf3054) 
 * OSP-2590 CDEF backward compatibility switches into ce.h
 * Include cdef1ce.h from cdef.h.
 * Revision 1.10 2016/06/30 16:08:06CEST Rademacher, Jan (uidf3054) 
 * Change on the side of OSP-2590 CDEF backward compatibility switches into ce.h
 * remove name and date from otherwise useful comments.
 * Revision 1.9 2014/06/30 10:54:31CEST Rademacher, Jan (uidf3054) 
 * - JIRA OSP 1749 CDEF cleaner name spaces
 *   - introduce symbols with prefix
 *     - use them for deriving other symbols
 *     - name new symbols without reference to deprecated
 *   - move prefix-less symbols to deprecated
 *     (exception: encapsulate union member synonyms)
 *   - define new symbols as QAC-friendly as possible
 *     - define symbols explicitly, only verify against available information
 *       (this allows to silence the QAC messages inside cdef,
 *         instead only in clients, i.e. less noisy QAC for clients)
 *     - only define symbols if not yet defined by headers
 *       which happen to be visible
 *     - do not include now unneeded "useful" headers
 *       (but tolerate if they happen to be visible)
 * _
 * Revision 1.8 2014/05/09 14:35:09CEST Rademacher, Jan (uidf3054) 
 * - improve comments,
 *   for readability,for being similar between endianesses and to fix typoes
 * _
 * Revision 1.7 2013/08/22 15:22:37CEST Meganathan, Dinesh Kumar (uidt8287) 
 * Turned Off QAC warnings connected to CDEF__STATIC_ASSERT
 * Revision 1.6 2013/06/26 10:46:00CEST Meganathan, Dinesh Kumar (uidt8287) 
 * Incorporation of ORM_0022_JGP2014_CDEF_Code
 *  - changed #if for include of stdint.h
 *  - improved comments
 *  - added static-assertion-macro direcly (normally present in ASRL) to prevent cyclic inclusions
 *  - include stdint.h to ensure having the same limit-values (not available on Win32)
 *  - switch off warning 4214 (nonstandard extension used) for bit-accesses on Win32 only
 *  add:
 *  #pragma warning (disable: 4214)
 *  for iMX6 usage
 * Revision 1.5 2010/07/20 11:02:38CEST Kramer, Johannes02 (uidf2687) 
 * No changes, just ensure correct revision number
 * Revision 1.4 2010/07/20 11:02:38CEST Rademacher Jan (uidf3054) (uidf3054) 
 * - CDS MR 8954/8955 CDEF both: Introduce common header
 *   - move endianess-independent definitions into "new cdef_common.h"
 *   - except for deprecated symbols 
 *     (defined after endianess-depending defintions)
 *     they are moved into new "cdef_deprecated.h",
 *     which gets included from cdef suitably late
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
* 
*    Rev 1.16   Nov 27 2009 13:57:06   Jan.Rademacher
* - reduce QAC warnings by
*   - change to capital suffixes,
*     which are preferred by QAC,
*     in code and in comments
*   - use some additional suffixes
*   - add more brace pairs () for QAC
* - add some brace pairs ()
*   for symmetry/readability
* - add comments on macro constructs
* - make some alignments/indentations
*   prettier
* 
*    Rev 1.15   Apr 23 2009 10:48:10   Jan.Rademacher
* - CDS MR 7034 CDEF: inline handling - softune problems
*   - remove inline handling completely
* 
*    Rev 1.14   Apr 22 2009 16:29:00   Jan.Rademacher
* - CDS MR 6162 CDEF both, define problematics from limits.h conditionally
*   - define three 64bit-related limit symbols only if they are not yet defined
*   - ignore the hitherto used encapsulation switch
*   - complain if the base for the definition is not defined
*   - add a comment explaining the relation of 16bit-CPU and 64bit related symbols
* 
*    Rev 1.13   Feb 19 2009 17:17:08   Jan.Rademacher
* - remove the use of i and j as formal parameters, to implement
*   CDS MR 7190 CDEF: formal parameters wrong
* 
*    Rev 1.12   Jul 16 2007 16:29:54   Jan.Rademacher
* - fix missing renaming to u8View
* - add lots of backward synonyms (even farther backward)
* - fix nu64Bit0-2 to access 64bit constants
* 
*    Rev 1.11   Jul 16 2007 15:26:42   Jan.Rademacher
* - use agreed-upon naming convention for suffix-literal constants
* - remove redundant F from some CDEF prefixes
* - correct example result in a comment
* - use least/most significant bit<n> instead of byte, word, etc.
*   and instead of signifying
* - correct the result description for CDEF_nu64Bin_Suf
* - mention the cast in description of CDEF_nu<n>Bin
* - remove use of obsolete type prefixes from comments
* - SMK-correct uint union member names, now u<n>View
* - correct an unsigned value range
* 
*    Rev 1.10   Jul 13 2007 16:51:06   Jan.Rademacher
* - CDS MR 5634 CDEF be considerate of Together
*   - move lots of description comments before the definition
*     and use slash-asterisk-asterisk to start the comment
*   - add tags to structs and unions
*   - add alias entries to the expected names
*   - use slash-asterisk-hash for comments on deprecated types
*     this makes them invisible in together diagrams
*   - add shortDesc entries, just to be nice
*   - add order entries to force the order in the file
*   - move the 16bit encapsulation, so that together does not
*     see a comment without the commented object
* - change comments to stop implying the deprecated old
*   terms for different width variables
* - add synchronsiation support lines for BeyondCompare,
*   this makes it much simpler to maintain BIG and LITTLE
*   consistence
* 
*    Rev 1.9   Jul 09 2007 10:28:24   Jan.Rademacher
* - CDS MR 5631 CDEF nBinUInt<16> cast consistently
*   - introduce non-API background macros for 32 and 64 bit
*   - introduce API macros CDEF_nBinTo<n>BitLiteral for 32/64
*   - based on above re-implement existing macros as
*     CDEF_nu<n>BinToInt for 8,16,32,64
*   - only provide 64 on CPU > 16
*   - move now obsolete macros into section which is
*     encapsualted by ifndef CDEF_SMK_BINARYCONVERTERS
* - fix a QAC encapsulation (ON instead of OFF needed)
* 
*    Rev 1.8   Jul 05 2007 12:54:14   Jan.Rademacher
* - CDS MR 5633 CDEF binary to uint<n> conversion should be based on unsigned literals
*   - change the used suffix from 'l' to 'ul'
* - extend the explanation comment on binary to uint conversion
* - rename the macros parameter name from i to u32Binary
* 
*    Rev 1.7   Jul 04 2007 17:57:12   Jan.Rademacher
* - CDS MR 5629 CDEF Introduce CDEF_u<n>GetBitMask(u8BitPosition)
*   - introduce CDEF_u8GetBitMask
*   - introduce CDEF_u32GetBitMask
*   - introduce CDEF_u64GetBitMask
*   - comment that there cannot be a CDEF_u16GetBitMask
*   - none of implementations depend on the 
*     _CPU_16BIT_ switch
*     (only whether the 64 wide is defined or not)
*   - move the old definition of GetBitMaskB(n) into the section
*     for obsolete API which is encapsulated by
*     CDEF_SMK_BITMASKS
* 
*    Rev 1.6   Jul 03 2007 16:33:30   Jan.Rademacher
* - CDS MR 5630 CDEF base nu<n>Bit<m> constant definition on suffixes
*   - introduce CDEF_nBit<m>of<n>, typed constants without type cast
*   - comment on the 16bit exception for above
*   - introduce CDEF_nu<n>Bit<m>, constants which type cast above,
*     eventually to replace not so strictly named nu<n>Bit<m> symbols
*   - move definition of nu<n>Bit<m> symbols into the obsolete section,
*     where they can be disabled by a environment definition
*   - change definition of nu<n>Bit<m>
*     - to remove leading zeroes (which annoy QAC)
*     - to use proper suffixes (at least xxu for 16bit, unsigned)
* - change to use of literals to avoid QAC warnings on leading zeroes
* - introduce two individual alternative environment definition switches
*   to evetually replace TYPEDEFS_SMK_STRICT
* 
*    Rev 1.5   Jun 25 2007 16:03:40   Jan.Rademacher
* - CDS MR 5628 CDEF: On and Off definition should use suffixes
* 
*    Rev 1.4   Jun 22 2007 14:08:44   Jan.Rademacher
* - incorporation of non-MR comments from QRM 956
* 
*    Rev 1.3   Jun 19 2007 16:26:36   Jan.Rademacher
* - CDS MR 5250 cdef.h compilation under MSVC crashes because of "octal" QA C pragma
* 
*    Rev 1.2   Jun 06 2007 15:21:36   Jan.Rademacher
* - CDS MR 5627 CDEF definition of True and False should be in on enclosing brace-pair
* 
*    Rev 1.1   Sep 15 2006 12:22:32   ladislav.cezner
* Volatile basic datatypes removed due to conflict with CANbedded
* 
*    Rev 1.0   Sep 04 2006 14:58:18   ladislav.cezner
* Initial revision.
 ********************************************************************************************/

/*====================================== END OF FILE =======================================*/

