/***************************************************************************
*======= Copyright (c) [Year 2023] Continental AG and subsidiaries =========
****************************************************************************
* Copyrights should use the oldest year for the file and should not be updated each year.
*
* Title        : psm_api.c
*
* Description  : Source code of export interfaces of PSM module
*
* Environment  : TV2-C
*
* Responsible  : Yu Han Ng, VNI CE SW AS SGP SWP4
*
* Guidelines   : SMK 4.28
*
* Template name: SWMODxC1.C, Revision 1.9
*
*
***************************************************************************/

/***************************************************************************
* HEADER-FILES (Only those that are needed in this file)
***************************************************************************/
/* System-headerfiles */
#include "Std_Types.h"

/* Foreign headerfiles */

/* Own headerfiles */
#include "psm_internal.h"
#include "psm_api.h"
#include "psm_cfg.h"
#include "psm_externalinclude.h"
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

extern Psm__tstPowerSequence Psm__astPowerSequence[];

/*
* Description  : The configuration of the power sequences.
*/

/******************************************************************************
* FILE LOCAL FUNCTION PROTOTYPES
*
* In this section declare
* - all tasks
* - all file local function prototypes needed for your module (static)
******************************************************************************/

/******************************************************************************
* FUNCTION DEFINITIONS
*
* In this section define
* - all tasks
* - all functions
******************************************************************************/

Psm_tenPsState Psm_enGetState(Psm_tenPsId enPsId)
{
    Psm_tenPsState enPsState = Psm_nenIdle; /* Default return value */

    if (enPsId >= Psm_nenPsCount)
    {
        /* Wrong input parameter */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_GET_STATE,PSM_E_PARAM_INVALID_POWER_SEQUENCE);
    }
    else
    {
        /* Get public facing state */
        enPsState = Psm__astPowerSequence[enPsId].enPubState;

        /* Special handling for Simple PS type */
        if ((Psm__astPowerSequence[enPsId].pstConfig)->enType == Psm__nenSimplePs)
        {
            /* Categorize into Idle and Busy */
            if (enPsState != Psm_nenBusy)
            {
                enPsState = Psm_nenIdle;
            }
        }
    }

    return enPsState;
}

boolean Psm_boRunSequence(Psm_tenPsId enPsId)
{
    boolean boApiReturn = FALSE;
    Psm_tenSeqReturn enSeqReturn;

    if (enPsId >= Psm_nenPsCount)
    {
        /* Wrong input parameter */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_RUN_SEQUENCE,PSM_E_PARAM_INVALID_POWER_SEQUENCE);
    }
    else if ((Psm__astPowerSequence[enPsId].pstConfig)->enType != Psm__nenSimplePs)
    {
        /* Wrong input parameter */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_RUN_SEQUENCE,PSM_E_PARAM_INVALID_POWER_SEQUENCE_TYPE);
    }
    else if (Psm__astPowerSequence[enPsId].enPubState == Psm_nenBusy)
    {
        /* Power sequence is busy */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_RUN_SEQUENCE,PSM_E_INVALID_POWER_SEQUENCE_STATE);
    }
    else
    {
        Psm__vStartSeq(enPsId, Psm_nenStartup);

        enSeqReturn = Psm__enMainStateM(enPsId);

        if ((enSeqReturn == Psm_nenSeqComplete) || (enSeqReturn == Psm_nenSeqAsync))
        {
            boApiReturn = TRUE;   
        }
    }

    return boApiReturn;
}

boolean Psm_boStartup(Psm_tenPsId enPsId)
{
    boolean boApiReturn = FALSE;
    Psm_tenSeqReturn enSeqReturn;

    if (enPsId >= Psm_nenPsCount)
    {
        /* Wrong input parameter */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_STARTUP,PSM_E_PARAM_INVALID_POWER_SEQUENCE);
    }
    else if ((Psm__astPowerSequence[enPsId].pstConfig)->enType != Psm__nenPowerRailPs)
    {
        /* Wrong input parameter */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_STARTUP,PSM_E_PARAM_INVALID_POWER_SEQUENCE_TYPE);
    }
    else if (Psm__astPowerSequence[enPsId].enPubState == Psm_nenBusy)
    {
        /* Power sequence is busy */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_STARTUP,PSM_E_INVALID_POWER_SEQUENCE_STATE);        
    }
    else
    {
        Psm__vStartSeq(enPsId, Psm_nenStartup);

        enSeqReturn = Psm__enMainStateM(enPsId);

        if ((enSeqReturn == Psm_nenSeqComplete) || (enSeqReturn == Psm_nenSeqAsync))
        {
            boApiReturn = TRUE;   
        }
    }

    return boApiReturn;
}

boolean Psm_boShutdown(Psm_tenPsId enPsId)
{
    boolean boApiReturn = FALSE;
    Psm_tenSeqReturn enSeqReturn;

    if (enPsId >= Psm_nenPsCount)
    {
        /* Wrong input parameter */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_SHUTDOWN,PSM_E_PARAM_INVALID_POWER_SEQUENCE);
    }
    else if ((Psm__astPowerSequence[enPsId].pstConfig)->enType != Psm__nenPowerRailPs)
    {
        /* Wrong input parameter */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_SHUTDOWN,PSM_E_PARAM_INVALID_POWER_SEQUENCE_TYPE);
    }
    else if ((Psm__astPowerSequence[enPsId].enPubState != Psm_nenOn) && (Psm__astPowerSequence[enPsId].enPubState != Psm_nenFail))
    {
        /* Power sequence is in wrong state */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_SHUTDOWN,PSM_E_INVALID_POWER_SEQUENCE_STATE);
    }
    else
    {
        Psm__vStartSeq(enPsId, Psm_nenShutdown);

        enSeqReturn = Psm__enMainStateM(enPsId);

        if ((enSeqReturn == Psm_nenSeqComplete) || (enSeqReturn == Psm_nenSeqAsync))
        {
            boApiReturn = TRUE;   
        }
    }

    return boApiReturn;
}

boolean Psm_boResume(Psm_tenPsId enPsId)
{
    boolean boApiReturn = FALSE;
    Psm_tenSeqReturn enSeqReturn;

    if (enPsId >= Psm_nenPsCount)
    {
        /* Wrong input parameter */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_RESUME,PSM_E_PARAM_INVALID_POWER_SEQUENCE);
    }
    else if ((Psm__astPowerSequence[enPsId].pstConfig)->enType != Psm__nenPowerRailPs)
    {
        /* Wrong input parameter */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_RESUME,PSM_E_PARAM_INVALID_POWER_SEQUENCE_TYPE);
    }
    else if (Psm__astPowerSequence[enPsId].enPubState != Psm_nenFail)
    {
        /* Power sequence is in wrong state */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_RESUME,PSM_E_INVALID_POWER_SEQUENCE_STATE);
    }
    else
    {
        Psm__vResumeSeq(enPsId);

        enSeqReturn = Psm__enMainStateM(enPsId);

        if ((enSeqReturn == Psm_nenSeqComplete) || (enSeqReturn == Psm_nenSeqAsync))
        {
            boApiReturn = TRUE;   
        }
    }

    return boApiReturn;
}

void Psm_vStartMonitoring(Psm_tenPsId enPsId)
{
    if (enPsId >= Psm_nenPsCount)
    {
        /* Wrong input parameter */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_START_MONITORING,PSM_E_PARAM_INVALID_POWER_SEQUENCE);
    }
    else if (Psm__astPowerSequence[enPsId].enPubState != Psm_nenOn)
    {
        /* Power sequence is in wrong state */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_START_MONITORING,PSM_E_INVALID_POWER_SEQUENCE_STATE);
    }
    else
    {
        Psm__vStartMonitor(enPsId);
    }
}

void Psm_vStopMonitoring(Psm_tenPsId enPsId)
{
    if (enPsId >= Psm_nenPsCount)
    {
        /* Wrong input parameter */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_STOP_MONITORING,PSM_E_PARAM_INVALID_POWER_SEQUENCE);
    }
    else if (Psm__astPowerSequence[enPsId].enPubState != Psm_nenOn)
    {
        /* Power sequence is in wrong state */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_STOP_MONITORING,PSM_E_INVALID_POWER_SEQUENCE_STATE);
    }
    else
    {
        Psm__vStopMonitor(enPsId);
    }
}

void Psm_MainFunction_PowerSequence(void)
{
    uint16 u16DequeueRemainCtr = Psm_nenPsCount;
    Psm_tenPsId enPsmId;
    
    Psm__EnterCriticalSection();

    while ((FALSE != Psm_boAsyncEventDequeue(&enPsmId)) && (0 < u16DequeueRemainCtr))
    {
        Psm__ExitCriticalSection();
        (void)Psm__enMainStateM(enPsmId);
        --u16DequeueRemainCtr;
        Psm__EnterCriticalSection();
    }

    /* If we reach here with u16DequeueRemainCtr == 0 and the queue is still not empty, a new trigger event must have been sent */
    Psm__ExitCriticalSection();
}

/***************************************************************************
* EOF: psm_api.c
****************************************************************************/
