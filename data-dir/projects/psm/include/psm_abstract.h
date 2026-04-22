/***************************************************************************
*======= Copyright (c) [Year 2021] Continental AG and subsidiaries =========
****************************************************************************
* Copyrights should use the oldest year for the file and should not be updated each year.
*
* Title        : psm_ca.h
*
* Description  : Generated header file of project specific adapatation of PSM module
*
* Environment  : TV2-C
*
* Responsible  : Yu Han Ng, VNI CE SW AS SGP SWP4
*
* Guidelines   : SMK 4.28
*
* Template name: SWMODxC2.H, Revision 1.9
*
*
****************************************************************************/

/*
* Check if information is already included
*/
#ifndef PSM_ABSTRACT_H
#define PSM_ABSTRACT_H


/***************************************************************************
* HEADER-FILES (Only those that are needed in this file)
****************************************************************************/
/* System-headerfiles */
#include "Std_Types.h"

/* Foreign headerfiles */

/* Own headerfiles */
#include "psm_types.h"

/***************************************************************************
* MODULE GLOBAL DEFINITIONS AND DECLARATIONS
*
* In this section
* - define helpful macros for easy data access and for a comfortable function
*   use if necessary
* - define module global define-constants
* - declare module global interface ROM-constants
* - define module global type definitions
* - declare module global interface variables
****************************************************************************/

/***************************************************************************
* MODULE GLOBAL FUNCTION PROTOTYPES
*
* In this section declare
* - all module global function prototypes of your module
****************************************************************************/

#ifndef GTEST
inline void Psm__vEnterCriticalSection(void)
{
    ;
}

inline void Psm__vExitCriticalSection(void)
{
    ;
}
#endif

void Psm__vWriteDio(uint16 u16DioChannel, uint8 u8DioLevel);

boolean Psm__boMatchDio(uint16 u16DioChannel, uint8 u8DioLevel);

void Psm__vStopGptTimer(uint16 u16GptChannel);

void Psm__vStartGptTimerNonBlocking(uint16 u16GptChannel, uint32 u32TimeUs);

void Psm__vStartGptTimerBlocking(uint16 u16GptChannel, uint32 u32TimeUs);

void Psm__vStopIcuDetection(uint16 u16IcuChannel);

void Psm__vStartIcuDetection(uint16 u16IcuChannel);

#endif                                  /* ifndef PSM_ABSTRACT_H */

/***************************************************************************
* EOF: psm_ca.h
****************************************************************************/
