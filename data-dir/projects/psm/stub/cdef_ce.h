/***************************************************************************
*=====================      Copyright by Continental AG      ===============
****************************************************************************
* Title        : cdef1ce.h
*
* Description  : Externally visible configuration for CDEF.
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

/*
* Start Reinclusion Guard
*/
#ifndef CDEF_CE_H
#define CDEF_CE_H

/***************************************************************************
* HEADER-FILES (Only those that are needed in this file)
****************************************************************************/
/* System-headerfiles */
/* obviously, cdef should NOT be included here, it includes this file. */

/***************************************************************************
* EXPORT CONFIGURATION CONSTANTS
*
* In this section define
* - constants and macros to configure your export interface. These constants
*   are public for other modules
****************************************************************************/

/*
 * Description: 
 * This is the list of the so called "CDEF backward compatibility switches".
 * They allow to configure CDEF to tolerate outdated software, which insists
 * on using risky typenames like "byte" or "uint8".
 * Using these typenames is risky, because they are too "obvious";
 * i.e. they tend to be used by third party software. If that happens, code
 * which sees both, CDEF and also the third party software header, will see
 * identical or even conflicting definitions for these typenames. This would 
 * cause compiler warnings or errors or bugs which are hard to find.
 * It also becomes extremely hard to find out which definition is actually
 * used. It is not always the same definition for one system. It depends on
 * include order. It can be different for two source code files (*.c). It can
 * change for a source file, even if the source file has not been changed.
 * Because the include order can change in included OTHER headers.
 * The whole problem also applies to other symbols defined by CDEF,
 * like "Yes", "No", "On", "Off".
 * It is therefore recommended NOT to use these deprecated typenames and
 * symbols, which means NOT to define the backward compatibility switches.
 * Most of these symbols are deprecated because they have been abolished for
 * the reasons above and should not be used anymore. But a few
 * (e.g. "float32") are quite recent. They are deprecated because they should
 * never have been introduced. They have been used for a few years in spite
 * of recommendations not to use them. In order to give some additional years
 * for cleanup, CDEF offers optional backward compatibility. CDEF however
 * assumes clean client code and is not tested for compatibility to deprecated
 * symbols being used.
 * The switches are expected to already be defined or not defined when cdef is
 * included. The idea is that they are optionally defined in some globally
 * effective central file (e.g. "build_options" or "CMakeLists.txt") or are
 * defined just before including cdef.
 * Do not define the switches in visible headers, i.e. do not define them
 * in your *c1.h, not in your *ce.h, not in your *ct.h. Defining them in a
 * correctly used ci.h (i.e. invisible from outside) is mostly OK.
 * In order to find out which of the switch establishes what kind of
 * backward compatibility, please find the place inside cdef_deprecated.h,
 * which has the corresponding "#if defined CDEF_DEPRECATED*".
 * Define CDEF_DEPRECATED_H before including cdef, in order to override all
 * backward compatibility for one code file. That allows convenient cleanup
 * of components in your responsibility, without affecting the rest of the
 * system.
 *
 * Values     : 
 * CDEF_DEPRECATED_H:     defined: no backward compatibility at all
 *                    not defined:    backward compatibility according to
 *                                    the other switches
 * CDEF_DEPRECATED_*:     defined: tolerate unclean client code for "*"
 *                    not defined:  require   clean client code for "*"
 */

//#define CDEF_DEPRECATED_H
#define CDEF_DEPRECATED_INCLUDESTDINT
#define CDEF_DEPRECATED_NONPREFIXUTYPES
#define CDEF_DEPRECATED_NONPREFIXSTYPES
#define CDEF_DEPRECATED_NONPREFIXBITTYPES
#define CDEF_DEPRECATED_NONPREFIXBITFIELDS
#define CDEF_DEPRECATED_NONPREFIXRECORDS
#define CDEF_DEPRECATED_NONPREFIXFP
#define CDEF_DEPRECATED_NONPREFIXYESNO
#define CDEF_DEPRECATED_NONPREFIXONOFF
#define CDEF_DEPRECATED_NONPREFIXOKNOK
#define CDEF_DEPRECATED_NONPREFIXTRUEFALSE
#define CDEF_DEPRECATED_NONPREFIXBOOLTYPE
#define CDEF_DEPRECATED_NONPREFIXLIMITU8
#define CDEF_DEPRECATED_NONPREFIXLIMITU16
// #define CDEF_DEPRECATED_NONPREFIXLIMITU32
#define CDEF_DEPRECATED_NONPREFIXLIMITU64
#define CDEF_DEPRECATED_INTERNALLIMITU64
#define CDEF_DEPRECATED_NONPREFIXLIMITSS8
#define CDEF_DEPRECATED_NONPREFIXLIMITSS16
// #define CDEF_DEPRECATED_NONPREFIXLIMITSS32
#define CDEF_DEPRECATED_NONPREFIXLIMITSS64
#define CDEF_DEPRECATED_INTERNALLIMITSS64
#define CDEF_DEPRECATED_TYPES
#define CDEF_FLOAT_SYNONYMS
#define CDEF_DEPRECATED_MEDIUMCONST
#define CDEF_DEPRECATED_BITMASKS
#define CDEF_DEPRECATED_BINARYCONVERTERS
 

/*
* Close Reinclusion Guard
*/
#endif                                  /* ifndef CDEF_CE_H */

/***************************************************************************
* Revision List: (Will be filled by PVCS)
* Archive: $Log: cdef1ce.h  $
* Archive: Revision 1.2 2016/07/01 15:52:06CEST Rademacher, Jan (uidf3054) 
* Archive: OSP-2590 CDEF backward compatibility switches into ce.h
* Archive:  with improved comments and
* Archive:  added description of similarily using CDEF_DEPRECATED_H
* Archive: Revision 1.1 2016/06/30 15:46:03CEST Rademacher, Jan (uidf3054) 
* Archive: Initial revision
* Archive: Member added to project /id/_gen/sw/pkg/Basic/cdef/~_gen.sw.pkg.cdef_COMMON_LilEnd_Var1/adapt/project.pj
*
****************************************************************************/

/***************************************************************************
* EOF: cdef1ce.h
****************************************************************************/
