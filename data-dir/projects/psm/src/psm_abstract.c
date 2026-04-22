/***************************************************************************
*======= Copyright (c) [Year 2021] Continental AG and subsidiaries =========
****************************************************************************
* Copyrights should use the oldest year for the file and should not be updated each year.
*
* Title        : psm_abstract.c
*
* Description  : Abstraction layer source code of PSM module
*
* Environment  : TV2-C
*
* Responsible  : Yu Han Ng, VNI CE SW AS SGP SWP4
*
* Guidelines   : SMK 4.28
*
* Template name: SWMODxC2.C, Revision 1.9
*
*
****************************************************************************/

/***************************************************************************
* HEADER-FILES (Only those that are needed in this file)
****************************************************************************/
/* System headerfiles */
#include <Std_Types.h>

/* Foreign headerfiles */
#include "Dio.h"
#include "Gpt.h"
#include "Icu.h"
#include "Port.h"
#include "psm_internal.h"
/* Own headerfiles */
#include "psm_c1.h"

/***************************************************************************
* GLOBAL DEFINITIONS
*
* In this section define
* - all global ROM-constants of your module
* - all global variables of your module
* Avoid the usage of global variables as far as possible! See SMK Design Guideline DG 4.2
****************************************************************************/

/***************************************************************************
* FILE LOCAL DEFINITIONS
*
* In this section define
* - all file local macros
* - all file local define-constants
* - all file local ROM-constants (static)
* - all file local type definitions
* - all file local variables (static)
****************************************************************************/

/*** Abstraction Layer ***/

void Psm__vWriteDio(uint16 u16DioChannel, uint8 u8DioLevel)
{
    Dio_ChannelType enDioChannel = (Dio_ChannelType)u16DioChannel;
    Dio_LevelType enDioLevel = (Dio_LevelType)u8DioLevel;

    if((u8DioLevel == STD_HIGH) || (u8DioLevel == STD_LOW))
    {
        Dio_WriteChannel(enDioChannel, enDioLevel);
    }
    else if (u8DioLevel == HIGHZ_INBUFF_ON)
    {
        Port_SetPinDirection((Port_PinType)u16DioChannel, PORT_PIN_IN);
    }
    else if (u8DioLevel == HIGHZ_INBUFF_OFF)
    {
        Port_SetPinDirection((Port_PinType)u16DioChannel, PORT_PIN_IN_OUT_DISABLED);
    }
}

boolean Psm__boMatchDio(uint16 u16DioChannel, uint8 u8DioLevel)
{
    Dio_ChannelType enDioChannel = (Dio_ChannelType)u16DioChannel;
    Dio_LevelType enDioLevel = (Dio_LevelType)u8DioLevel;
    Dio_LevelType enDioReadLevel;
    boolean boMatch;

    enDioReadLevel = Dio_ReadChannel(enDioChannel);
    if (enDioReadLevel == enDioLevel)
    {
        boMatch = TRUE;
    }
    else
    {
        boMatch = FALSE;
    }

    return boMatch;
}

void Psm__vStopGptTimer(uint16 u16GptChannel)
{
    Gpt_ChannelType enGptChannel = (Gpt_ChannelType)u16GptChannel;

    Gpt_DisableNotification(enGptChannel);
    Gpt_StopTimer(enGptChannel);
}

void Psm__vStartGptTimerNonBlocking(uint16 u16GptChannel, uint32 u32TimeUs)
{
    Gpt_ChannelType enGptChannel = (Gpt_ChannelType)u16GptChannel;
    Gpt_ValueType xGptTimeValue = (Gpt_ValueType)u32TimeUs;

    /* Stop GPT and start it again to force restart the timer counter */
    Psm__vStopGptTimer(enGptChannel);
    Gpt_EnableNotification(enGptChannel);
    Gpt_StartTimer(enGptChannel, xGptTimeValue);
}

void Psm__vStartGptTimerBlocking(uint16 u16GptChannel, uint32 u32TimeUs)
{
    Gpt_ChannelType enGptChannel = (Gpt_ChannelType)u16GptChannel;
    Gpt_ValueType xGptTimeValue = (Gpt_ValueType)u32TimeUs;
    Gpt_ValueType xGptTimeRemaining;

    /* Stop GPT and start it again to force restart the timer counter */
    Psm__vStopGptTimer(enGptChannel);
    Gpt_StartTimer(enGptChannel, xGptTimeValue);

    /* Blocking delay, poll until GPT is done */

#ifndef GTEST
    do {
        xGptTimeRemaining = Gpt_GetTimeRemaining(enGptChannel);
    }
    while (xGptTimeRemaining > 0u);
#endif

#ifdef GTEST
        xGptTimeRemaining = Gpt_GetTimeRemaining(enGptChannel);
#endif
}

void Psm__vStopIcuDetection(uint16 u16IcuChannel)
{
    Icu_ChannelType enIcuChannel = (Icu_ChannelType)u16IcuChannel;

    Icu_DisableNotification(enIcuChannel);
    Icu_DisableEdgeDetection(enIcuChannel);
}

void Psm__vStartIcuDetection(uint16 u16IcuChannel)
{
    Icu_ChannelType enIcuChannel = (Icu_ChannelType)u16IcuChannel;

    Icu_EnableNotification(enIcuChannel);
    Icu_EnableEdgeDetection(enIcuChannel);
}




/***************************************************************************
* EOF: psm_abstract.c
****************************************************************************/
