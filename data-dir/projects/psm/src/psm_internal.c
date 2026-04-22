/***************************************************************************
*======= Copyright (c) [Year 2023] Continental AG and subsidiaries =========
****************************************************************************
* Copyrights should use the oldest year for the file and should not be updated each year.
*
* Title        : psm_internal.c
*
* Description  : Source code of internal interfaces of PSM module
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
***************************************************************************/

/***************************************************************************
* HEADER-FILES (Only those that are needed in this file)
***************************************************************************/
/* System-headerfiles */
#include "Std_Types.h"

/* Foreign headerfiles */

/* Own headerfiles */
#include "psm_cfg.h"
#include "psm_internal.h"
#include "psm_api.h"
#include "psm_abstract.h"
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
uint8 Psm_au8PsidQueue[PSM_MaxNoOfPowerSequences];
Psm__tstPsIdQueue Psm__stAsyncEventQueue =
{
    &Psm_au8PsidQueue[0],
    -1,
    -1,
    PSM_MaxNoOfPowerSequences
};
extern Psm__tstPowerSequence Psm__astPowerSequence[]; /*
*
* Description  : The configuration of the power sequences.
*/

/******************************************************************************
* FILE LOCAL FUNCTION PROTOTYPES
*
* In this section declare
* - all tasks
* - all file local function prototypes needed for your module (static)
******************************************************************************/

PSM_STATIC Psm__tenActReturn Psm__enPerformDelay(Psm_tenPsId enPsId, boolean boBlockingDelay, uint32 u32DelayIntervalUs, uint32 u32DelayCycleLimit);
PSM_STATIC Psm__tenActReturn Psm__enDriveIoStateM(Psm_tenPsId enPsId, const Psm__tstActionDriveIo *pstActionDriveIo);
PSM_STATIC Psm__tenActReturn Psm__enPollIoStateM(Psm_tenPsId enPsId, const Psm__tstActionPollIo *pstActionPollIo);
PSM_STATIC Psm__tenActReturn Psm__enSyncDelayStateM(Psm_tenPsId enPsId, const Psm__tstActionDelay *pstActionDelay);
PSM_STATIC Psm__tenActReturn Psm__enAsyncDelayStateM(Psm_tenPsId enPsId, const Psm__tstActionDelay *pstActionDelay);
PSM_STATIC Psm__tenActReturn Psm__enCallFuncStateM(Psm_tenPsId enPsId, const Psm__tstActionFunc *pstActionFunc);
PSM_STATIC Psm__tenActReturn Psm__enCallVoidFuncStateM(Psm_tenPsId enPsId, const Psm__tstActionVoidFunc *pstActionVoidFunc);
PSM_STATIC Psm__tenActReturn Psm__enTriggerSeqStateM(Psm_tenPsId enPsId, const Psm__tstActionTriggerPs *pstActionTriggerPs);
PSM_STATIC Psm__tenActReturn Psm__enWaitSeqStateM(Psm_tenPsId enPsId, const Psm__tstActionWaitPs *pstActionWaitPs);
PSM_STATIC boolean Psm__boPsIdEnequeue(Psm_tenPsId enPsId, Psm__tstPsIdQueue* pstPsmPsIdQueue);
PSM_STATIC boolean Psm__boPsIdDequeue(Psm_tenPsId *penDequeuePsid, Psm__tstPsIdQueue* pstPsmPsIdQueue);
/******************************************************************************
* FUNCTION DEFINITIONS
*
* In this section define
* - all tasks
* - all functions
******************************************************************************/

void Psm__vStartMonitor(Psm_tenPsId enPsId)
{
    const Psm__tstPsConfig * const pstPsConfig = Psm__astPowerSequence[enPsId].pstConfig;

    if (pstPsConfig->enMonitorType == Psm__nenPolling)
    {
        /* Start GPT timer */
        Psm__vStartGptTimerNonBlocking(pstPsConfig->u16GptChannel, pstPsConfig->u32MonitorIntervalUs);
    }
    else if (pstPsConfig->enMonitorType == Psm__nenInterrupt)
    {
        /* Start ICU monitoring */
        Psm__vStartIcuDetection(pstPsConfig->u16IcuChannel);
    }
}

void Psm__vStopMonitor(Psm_tenPsId enPsId)
{
    const Psm__tstPsConfig * const pstPsConfig = Psm__astPowerSequence[enPsId].pstConfig;

    if (pstPsConfig->enMonitorType == Psm__nenPolling)
    {
        /* Stop GPT timer */
        Psm__vStopGptTimer(pstPsConfig->u16GptChannel);
    }
    else if (pstPsConfig->enMonitorType == Psm__nenInterrupt)
    {
        /* Stop ICU monitoring */
        Psm__vStopIcuDetection(pstPsConfig->u16IcuChannel);
    }
}

boolean Psm__boMonitor(Psm_tenPsId enPsId)
{
    const Psm__tstPsConfig * const pstPsConfig = Psm__astPowerSequence[enPsId].pstConfig;
    boolean boDioOk;

    /* Read monitoring IO state */
    boDioOk = Psm__boMatchDio(pstPsConfig->stMonitorDioState.u16DioChannel, pstPsConfig->stMonitorDioState.u8DioLevel);

    if (boDioOk == TRUE)
    {
        /* Start monitoring */
        Psm__vStartMonitor(enPsId);
    }
    else
    {
        /* Stop monitoring */
        Psm__vStopMonitor(enPsId);
    }

    return boDioOk;
}

void Psm__vForceShutdown(Psm_tenPsId enPsId)
{
    const Psm__tstPsConfig * const pstPsConfig = Psm__astPowerSequence[enPsId].pstConfig;
    uint8 u8Count = 0;
    uint8 u8TotalCount = pstPsConfig->u8NumOfForceShutdownDio;

    if (u8TotalCount > 0)
    {
        if (pstPsConfig->pastForceShutdownDio == NULL_PTR)
        {
            /* Error, null pointer detected */
            PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_FORCE_SHUTDOWN,PSM_E_NULL_POINTER);
        }
        else
        {
            /* Call DIO to put IOs in their shutdown state */
            while (u8Count < u8TotalCount)
            {
                Psm__vWriteDio(pstPsConfig->pastForceShutdownDio[u8Count].u16DioChannel, pstPsConfig->pastForceShutdownDio[u8Count].u8DioLevel);
                u8Count++;
            }
        }
    }

    /* Call user function */
    if (pstPsConfig->vfpForceShutdownFunc != NULL_PTR)
    {
        pstPsConfig->vfpForceShutdownFunc();
    }
}

void Psm__vStartSeq(Psm_tenPsId enPsId, Psm_tenSequence enSequence)
{
    Psm__tstPowerSequence * const pstPs = &Psm__astPowerSequence[enPsId];

    Psm_tenPsState enPreviousState = pstPs->enPubState;
    /* Update public facing state, will be updated again at the end of sequence */
    pstPs->enPubState = Psm_nenBusy;
    
    if (enSequence == Psm_nenStartup)
    {
        /* Startup requested */
        pstPs->enSequence = Psm_nenStartup;
        pstPs->boDoForceShutdown = FALSE;
    }
    else if (enSequence == Psm_nenShutdown)
    {
        /* Shutdown requested */
        pstPs->enSequence = Psm_nenShutdown;
        if (pstPs->enState == Psm_nenFail)
        {
            /* Perform forced shutdown if the current state is fail */
            pstPs->boDoForceShutdown = TRUE;
        }
        else
        {
            pstPs->boDoForceShutdown = FALSE;
        }
    }
    else
    {
        /*when sequence is neither startup or shutdown*/
        /*Invalid sequence requested, should not happen*/
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID, PSM_INSTANCE_ID, PSM_API_START_SEQ, PSM_E_NO_SEQUENCE);
        pstPs->enPubState = enPreviousState; 
    }

    if((enSequence == Psm_nenShutdown) || (enSequence == Psm_nenStartup))     
    {
        pstPs->enState = Psm_nenBusy;
        
        /* Reset other variables */
        pstPs->u32DelayCycle = 0u;
        pstPs->boDoNotify = FALSE;
        pstPs->u8SeqStepId = 0u;
        pstPs->u8SeqSubStepId = 0u;
        
        /* Clean up of ongoing activity */
        if (enSequence == Psm_nenShutdown)
        {
            Psm__vStopMonitor(enPsId);
        }
        
        /* Perform forced shutdown if required */
        if (pstPs->boDoForceShutdown == TRUE)
        {
            Psm__vForceShutdown(enPsId);
            /* Change to off state to bypass the main state machine and skip directly to user notification */
            pstPs->enState = Psm_nenOff;
            pstPs->boDoNotify = TRUE;
        }
    }
    else
    {
        // do nothing
    }  
}

void Psm__vResumeSeq(Psm_tenPsId enPsId)
{
    Psm__tstPowerSequence * const pstPs = &Psm__astPowerSequence[enPsId];

    /* Update public facing state, will be updated again at the end of sequence */
    pstPs->enPubState = Psm_nenBusy;

    /* Check if it has reached steady state (on) before */
    if (pstPs->enSequence == Psm_nenNoSequence)
    {
        /* Set internal state to ON */
        pstPs->enState = Psm_nenOn;
    }
    else
    {
        /* Set internal state to Busy */
        pstPs->enState = Psm_nenBusy;
    }
    
    /* Reset other variables */
    pstPs->u32DelayCycle = 0u;
    pstPs->boDoNotify = FALSE;
    pstPs->boDoForceShutdown = FALSE;
}

Psm_tenSeqReturn Psm__enSeqStateM(Psm_tenPsId enPsId)
{
    Psm__tstPowerSequence * const pstPs = &Psm__astPowerSequence[enPsId];
    boolean boBreakLoop = FALSE;
    const Psm__tstAction *pastActionList = NULL_PTR;
    uint8 u8TotalStep = 0;
    Psm_tenSeqReturn enSeqReturn = Psm_nenSeqComplete;
    Psm__tenActReturn enActReturn;
    boolean boIsValidSequence;

      /* Get action list */
    if (pstPs->enSequence == Psm_nenStartup)
    {
        pastActionList = (pstPs->pstConfig)->pastStartupAction;
        u8TotalStep = (pstPs->pstConfig)->u8NumOfStartupAction;
        boIsValidSequence = TRUE;
    }
    else if (pstPs->enSequence == Psm_nenShutdown)
    {
        pastActionList = (pstPs->pstConfig)->pastShutdownAction;
        u8TotalStep = (pstPs->pstConfig)->u8NumOfShutdownAction;
        boIsValidSequence = TRUE;
    }
    else
    {
        /* Invalid sequence or no sequence */
        boIsValidSequence = FALSE;
    }

    /* Error check */
    if ((pastActionList == NULL_PTR) && (boIsValidSequence == TRUE))
    {
        pstPs->enState = Psm_nenFail;
        pstPs->boDoNotify = TRUE;
        enSeqReturn = Psm_nenSeqAbort;
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_SEQ_STATE,PSM_E_NULL_POINTER);
    }
    else if(boIsValidSequence == FALSE)
    {
        pstPs->enState = Psm_nenFail;
        pstPs->boDoNotify = TRUE;
        enSeqReturn = Psm_nenSeqAbort;
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID, PSM_INSTANCE_ID, PSM_API_SEQ_STATE, PSM_E_NO_SEQUENCE);
    }
    else
    {
        /* do nothing */
    }


    while((pstPs->enState == Psm_nenBusy) && (boBreakLoop == FALSE))
    {
        if (pstPs->u8SeqStepId >= u8TotalStep)
        {
            /* End of action */
            if (pstPs->enSequence == Psm_nenStartup)
            {
                pstPs->enState = Psm_nenOn;
            }
            else
            {
                pstPs->enState = Psm_nenOff;
            }
            pstPs->boDoNotify = TRUE;
            enSeqReturn = Psm_nenSeqComplete;
        }
        else
        {
            if (pastActionList[pstPs->u8SeqStepId].pvActionData == NULL_PTR)
            {
                /* Error, null pointer detected */
                enActReturn = Psm__nenActAbort;
                PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_SEQ_STATE,PSM_E_NULL_POINTER);
            }
            else
            {
                /* Perform action */
                switch (pastActionList[pstPs->u8SeqStepId].enAction)
                {
                    case Psm__nenDriveDio:
                        enActReturn = Psm__enDriveIoStateM(enPsId, (const Psm__tstActionDriveIo *)pastActionList[pstPs->u8SeqStepId].pvActionData);
                        break;

                    case Psm__nenPollDio:
                        enActReturn = Psm__enPollIoStateM(enPsId, (const Psm__tstActionPollIo *)pastActionList[pstPs->u8SeqStepId].pvActionData);
                        break;

                    case Psm__nenSynchronousDelay:
                        enActReturn = Psm__enSyncDelayStateM(enPsId, (const Psm__tstActionDelay *)pastActionList[pstPs->u8SeqStepId].pvActionData);
                        break;

                    case Psm__nenAsynchronousDelay:
                        enActReturn = Psm__enAsyncDelayStateM(enPsId, (const Psm__tstActionDelay *)pastActionList[pstPs->u8SeqStepId].pvActionData);
                        break;

                    case Psm__nenCallUserFunction:
                        enActReturn = Psm__enCallFuncStateM(enPsId, (const Psm__tstActionFunc *)pastActionList[pstPs->u8SeqStepId].pvActionData);
                        break;

                    case Psm__nenCallUserFunction_VoidReturn:
                        enActReturn = Psm__enCallVoidFuncStateM(enPsId, (const Psm__tstActionVoidFunc *)pastActionList[pstPs->u8SeqStepId].pvActionData);
                        break;

                    case Psm__nenTriggerPowerSequence:
                        enActReturn = Psm__enTriggerSeqStateM(enPsId, (const Psm__tstActionTriggerPs *)pastActionList[pstPs->u8SeqStepId].pvActionData);
                        break;

                    case Psm__nenWaitForPowerSequence:
                        enActReturn = Psm__enWaitSeqStateM(enPsId, (const Psm__tstActionWaitPs *)pastActionList[pstPs->u8SeqStepId].pvActionData);
                        break;
                        
                    case Psm__nenInvalidAction:
                    default:
                        /* Error, invalid action detected */
                        enActReturn = Psm__nenActAbort;
                        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_SEQ_STATE,PSM_E_INVALID_ACTION);
                        break;
                }
            }

            /* Post action processing */
            if (enActReturn == Psm__nenActComplete)
            {
                /* Go to next action */
                pstPs->u8SeqStepId++;
                pstPs->u8SeqSubStepId = 0;
                pstPs->u32DelayCycle = 0;
            }
            else if (enActReturn == Psm__nenActBusy)
            {
                /* Stay in current action */
            }
            else if (enActReturn == Psm__nenActAsync)
            {
                /* Stay in current action and break from loop */
                boBreakLoop = TRUE;
                enSeqReturn = Psm_nenSeqAsync;
            }
            else if (enActReturn == Psm__nenActAbort)
            {
                /* Failure occurred */
                pstPs->enState = Psm_nenFail;
                pstPs->boDoNotify = TRUE;
                enSeqReturn = Psm_nenSeqAbort;
            }
        }
    }

    return enSeqReturn;
}

Psm_tenSeqReturn Psm__enMainStateM(Psm_tenPsId enPsId)
{
    Psm__tstPowerSequence * const pstPs = &Psm__astPowerSequence[enPsId];
    Psm_tenSeqReturn enSeqReturn;
    boolean boMonitorReturn;

    if (pstPs->enState == Psm_nenBusy)
    {
        /* Execute state machine */
        enSeqReturn = Psm__enSeqStateM(enPsId);
    }
    else
    {
        enSeqReturn = Psm_nenSeqComplete;
    }

    /* Perform IO monitoring when on state is reached */
    if (((pstPs->pstConfig)->enType == Psm__nenPowerRailPs)
        && ((pstPs->pstConfig)->enMonitorType != Psm__nenNoMonitor)
        && (pstPs->enState == Psm_nenOn))
    {
        boMonitorReturn = Psm__boMonitor(enPsId);
        if (boMonitorReturn == FALSE)
        {
            pstPs->enState = Psm_nenFail;
            pstPs->boDoNotify = TRUE;
            enSeqReturn = Psm_nenSeqMonitorFail;
        }
    }

    /* Call user notification */
    if (pstPs->boDoNotify == TRUE)
    {
        if ((pstPs->pstConfig)->vfpNotifyFunc != NULL_PTR)
        {
            (pstPs->pstConfig)->vfpNotifyFunc(pstPs->enSequence, enSeqReturn, pstPs->u8SeqStepId);
        }
        pstPs->boDoNotify = FALSE;
    }

    /* Clear sequence variable when it reache steady state (on or off), keep the sequence when it fails */
    if ((pstPs->enState == Psm_nenOn) || (pstPs->enState == Psm_nenOff))
    {
        pstPs->enSequence = Psm_nenNoSequence;
    }

    /* Update public facing state at the end of sequence */
    pstPs->enPubState = pstPs->enState;

    return enSeqReturn;
}

PSM_STATIC Psm__tenActReturn Psm__enPerformDelay(Psm_tenPsId enPsId, boolean boBlockingDelay, uint32 u32DelayIntervalUs, uint32 u32DelayCycleLimit)
{
    Psm__tstPowerSequence * const pstPs = &Psm__astPowerSequence[enPsId];
    Psm__tenActReturn enActReturn;
    
    /* Repeat delay for u32DelayCycleLimit times */
    if (pstPs->u32DelayCycle < u32DelayCycleLimit)
    {
        pstPs->u32DelayCycle++;

        if (boBlockingDelay == FALSE)
        {
            Psm__vStartGptTimerNonBlocking((pstPs->pstConfig)->u16GptChannel, u32DelayIntervalUs);
            /* Asynchronous delay started, GPT will call the state machine once it is elapsed */
            enActReturn = Psm__nenActAsync;
        }
        else
        {
            Psm__vStartGptTimerBlocking((pstPs->pstConfig)->u16GptChannel, u32DelayIntervalUs);
            /* Synchronous delay done, request state machine to immediately trigger again */
            enActReturn = Psm__nenActBusy;
        }
    }
    else
    {
        /* Total delay fulfilled */
        enActReturn = Psm__nenActComplete;
    }
    /* Total delay = u32DelayIntervalUs x u32DelayCycleLimit */

    return enActReturn;
}

PSM_STATIC Psm__tenActReturn Psm__enDriveIoStateM(Psm_tenPsId enPsId, const Psm__tstActionDriveIo *pstActionDriveIo)
{
    uint8 u8Count = 0;
    uint8 u8TotalCount = pstActionDriveIo->u8NumOfDio;
    Psm__tenActReturn enActReturn = Psm__nenActComplete;

    if (u8TotalCount > 0)
    {
        if (pstActionDriveIo->pastDioState == NULL_PTR)
        {
            /* Error, null pointer detected */
            PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_DRIVE_IOSTATE,PSM_E_NULL_POINTER);
            enActReturn = Psm__nenActAbort;
        }
        else
        {
            /* Call DIO to configure IOs */
            while (u8Count < u8TotalCount)
            {
                Psm__vWriteDio(pstActionDriveIo->pastDioState[u8Count].u16DioChannel, pstActionDriveIo->pastDioState[u8Count].u8DioLevel);
                u8Count++;
            }
        }
    }
    else
    {
        /* Error, Invalid config detected */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID, PSM_INSTANCE_ID, PSM_API_DRIVE_IOSTATE, PSM_E_INVALID_CONFIG);
        enActReturn = Psm__nenActAbort;  
    }
    
    return enActReturn;
}

PSM_STATIC Psm__tenActReturn Psm__enPollIoStateM(Psm_tenPsId enPsId, const Psm__tstActionPollIo *pstActionPollIo)
{
    uint8 u8Count = 0;
    uint8 u8TotalCount = pstActionPollIo->u8NumOfDio;
    boolean boMatch = TRUE;
    Psm__tenActReturn enActReturn = Psm__nenActAbort;

    if (u8TotalCount > 0)
    {
        if (pstActionPollIo->pastDioState == NULL_PTR)
        {
            /* Error, null pointer detected */
            PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID, PSM_INSTANCE_ID, PSM_API_POLL_IOSTATE, PSM_E_NULL_POINTER);
            enActReturn = Psm__nenActAbort;
        }
        else
        {
            /* Call DIO to configure IOs */
            while ((u8Count < u8TotalCount) && (boMatch == TRUE))
            {
                boMatch = Psm__boMatchDio(pstActionPollIo->pastDioState[u8Count].u16DioChannel, pstActionPollIo->pastDioState[u8Count].u8DioLevel);
                u8Count++;
            }
            if (boMatch == TRUE)
            {
                /* Done */
                enActReturn = Psm__nenActComplete;
            }
            else
            {
                /* Call generic delay function for wait state */
                enActReturn = Psm__enPerformDelay(enPsId, pstActionPollIo->boBlockingWait, pstActionPollIo->u32WaitIntervalUs,pstActionPollIo->u32WaitCycleLimit);
                /* Translate "delay complete" into failure */
                if (enActReturn == Psm__nenActComplete)
                {
                    /* Retry limit reached */
                    enActReturn = Psm__nenActAbort;
                }
            }
        }
    }
    else
    {
        /* Error, Invalid config detected */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID, PSM_INSTANCE_ID, PSM_API_POLL_IOSTATE, PSM_E_INVALID_CONFIG);
        enActReturn = Psm__nenActAbort;  
    }
    return enActReturn;
}

PSM_STATIC Psm__tenActReturn Psm__enSyncDelayStateM(Psm_tenPsId enPsId, const Psm__tstActionDelay *pstActionDelay)
{
    Psm__tenActReturn enActReturn;

    if ((pstActionDelay->u32DelayIntervalUs == 0) || (pstActionDelay->u32DelayCycleLimit == 0))
    {
        /* Error, invalid config detected */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID, PSM_INSTANCE_ID, PSM_API_SYNC_DELAYSTATE, PSM_E_INVALID_CONFIG);
        enActReturn = Psm__nenActAbort;
    }
    else
    {
        /* Call generic delay function for sync delay */
        enActReturn = Psm__enPerformDelay(enPsId, TRUE, pstActionDelay->u32DelayIntervalUs, pstActionDelay->u32DelayCycleLimit);
    }

    return enActReturn;
}

PSM_STATIC Psm__tenActReturn Psm__enAsyncDelayStateM(Psm_tenPsId enPsId, const Psm__tstActionDelay *pstActionDelay)
{
    Psm__tenActReturn enActReturn;

    if ((pstActionDelay->u32DelayIntervalUs == 0) || (pstActionDelay->u32DelayCycleLimit == 0))
    {
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID, PSM_INSTANCE_ID, PSM_API_ASYNC_DELAYSTATE, PSM_E_INVALID_CONFIG);
        enActReturn = Psm__nenActAbort;
    }
    else
    {
        /* Call generic delay function for async delay */
        enActReturn = Psm__enPerformDelay(enPsId, FALSE, pstActionDelay->u32DelayIntervalUs,
                                          pstActionDelay->u32DelayCycleLimit);
    }

    return enActReturn;
}

PSM_STATIC Psm__tenActReturn Psm__enCallFuncStateM(Psm_tenPsId enPsId, const Psm__tstActionFunc *pstActionFunc)
{
    Psm_tenFuncReturn enFuncReturn;
    Psm__tenActReturn enActReturn;

    if (pstActionFunc->enfpUserFunc == NULL_PTR)
    {
        /* Error, null pointer detected */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_CALL_FUNCTIONSTATE,PSM_E_NULL_POINTER);
        enActReturn = Psm__nenActAbort;
    }
    else
    {
        /* Call user function */
        enFuncReturn = pstActionFunc->enfpUserFunc();

        /* Process return value from user function */
        if (enFuncReturn == Psm_nenFuncOk)
        {
            enActReturn = Psm__nenActComplete;
        }
        else if (enFuncReturn == Psm_nenFuncWait)
        {
            /* Call generic delay function for wait state */
            enActReturn = Psm__enPerformDelay(enPsId, pstActionFunc->boBlockingWait, pstActionFunc->u32WaitIntervalUs,
                                              pstActionFunc->u32WaitCycleLimit);
            
            /* Translate "delay complete" into failure */
            if (enActReturn == Psm__nenActComplete)
            {
                /* Retry limit reached */
                enActReturn = Psm__nenActAbort;
            }
        }
        else /* (enFuncReturn == Psm_nenFuncError) */
        {
            /* Error returned from user function */
            enActReturn = Psm__nenActAbort;
        }
    }

    return enActReturn;
}

PSM_STATIC Psm__tenActReturn Psm__enCallVoidFuncStateM(Psm_tenPsId enPsId, const Psm__tstActionVoidFunc *pstActionVoidFunc)
{
    Psm__tenActReturn enActReturn;

    if (pstActionVoidFunc->vfpVoidUserFunc == NULL_PTR)
    {
        /* Error, null pointer detected */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_CALL_VOIDFUNCTIONSTATE,PSM_E_NULL_POINTER);
        enActReturn = Psm__nenActAbort;
    }
    else
    {
        /* Call user function */
        pstActionVoidFunc->vfpVoidUserFunc();

        enActReturn = Psm__nenActComplete;
    }

    return enActReturn;
}

PSM_STATIC Psm__tenActReturn Psm__enTriggerSeqStateM(Psm_tenPsId enPsId, const Psm__tstActionTriggerPs *pstActionTriggerPs)
{
    boolean boSuccess;
    Psm__tenActReturn enActReturn;

    Psm_tenPsId enTargetPsId = pstActionTriggerPs->enTargetPsId;
    
    if (enTargetPsId >= Psm_nenPsCount)
    {
        /* Error, invalid config detected */
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_TRIGGER_SEQSTATE,PSM_E_INVALID_CONFIG);
        boSuccess = FALSE;
    }
    else
    {
        const Psm__tstPsConfig * const pstTargetPsConfig = Psm__astPowerSequence[enTargetPsId].pstConfig;

        /* Determine which API to call */
        if (pstTargetPsConfig->enType == Psm__nenSimplePs)
        {
            boSuccess = Psm_boRunSequence(enTargetPsId);     
        }
        else if (pstTargetPsConfig->enType == Psm__nenPowerRailPs)
        {
            if (pstActionTriggerPs->enTriggerPsSequence == Psm_nenStartup)
            {
                boSuccess = Psm_boStartup(enTargetPsId);
            }
            else if (pstActionTriggerPs->enTriggerPsSequence == Psm_nenShutdown)
            {
                boSuccess = Psm_boShutdown(enTargetPsId);
            }
            else
            {
                /* Error, invalid config detected */
                PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_TRIGGER_SEQSTATE,PSM_E_INVALID_CONFIG);
                boSuccess = FALSE;
            }      
        }
        else
        {
            /* Error, invalid config detected */
            PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_TRIGGER_SEQSTATE,PSM_E_INVALID_CONFIG);
            boSuccess = FALSE;
        }
    }

    if (boSuccess == TRUE)
    {
        enActReturn = Psm__nenActComplete;
    }
    else
    {
        enActReturn = Psm__nenActAbort;
    }   

    return enActReturn;
}

PSM_STATIC Psm__tenActReturn Psm__enWaitSeqStateM(Psm_tenPsId enPsId, const Psm__tstActionWaitPs *pstActionWaitPs)
{
    Psm__tenActReturn enActReturn;

    /* Use public API */
    if (Psm_enGetState(pstActionWaitPs->enTargetPsId) == pstActionWaitPs->enWaitForPsState)
    {
        /* Done */
        enActReturn = Psm__nenActComplete;
    }
    else
    {
        /* Call generic delay function for wait state */
        enActReturn = Psm__enPerformDelay(enPsId, pstActionWaitPs->boBlockingWait, pstActionWaitPs->u32WaitIntervalUs,
                                          pstActionWaitPs->u32WaitCycleLimit);
        
        /* Translate "delay complete" into failure */
        if (enActReturn == Psm__nenActComplete)
        {
            /* Retry limit reached */
            enActReturn = Psm__nenActAbort;
        }
    }
    
    return enActReturn;
}

boolean Psm_boAsyncEventEnqueue(Psm_tenPsId enPsId)
{
    return Psm__boPsIdEnequeue(enPsId,&Psm__stAsyncEventQueue);
}

boolean Psm_boAsyncEventDequeue(Psm_tenPsId* penPsId)
{
    return (Psm__boPsIdDequeue(penPsId,&Psm__stAsyncEventQueue));
}


PSM_STATIC boolean Psm__boPsIdEnequeue(Psm_tenPsId enPsId, Psm__tstPsIdQueue* pstPsmPsIdQueue)
{
    boolean boEnequeueOk = FALSE;
    if((pstPsmPsIdQueue->i8TailIdx==Psm_nQueueInvalidIdxVal)||(pstPsmPsIdQueue->i8HeadIdx==Psm_nQueueInvalidIdxVal))
    {
        pstPsmPsIdQueue->i8HeadIdx=0;
        pstPsmPsIdQueue->i8TailIdx=0;
        pstPsmPsIdQueue->pau8PsidQueue[pstPsmPsIdQueue->i8TailIdx]=enPsId;
        boEnequeueOk = TRUE;
    }
    else if(((pstPsmPsIdQueue->i8TailIdx+1)%pstPsmPsIdQueue->u8MaxQueuesize)==pstPsmPsIdQueue->i8HeadIdx)
    {
        /*queue is full*/
        PSM_ERROR_CALLOUT_FUNCTION(PSM_MODULE_ID,PSM_INSTANCE_ID,PSM_API_ENQUEUE,PSM_E_QUEUE_FULL);
        boEnequeueOk = FALSE;
    }
    else if(((pstPsmPsIdQueue->i8HeadIdx) >=(pstPsmPsIdQueue->u8MaxQueuesize))||
                  ((pstPsmPsIdQueue->i8TailIdx) >=(pstPsmPsIdQueue->u8MaxQueuesize)))
    {
        /*index error resetting the index*/
        pstPsmPsIdQueue->i8HeadIdx=Psm_nQueueInvalidIdxVal;
        pstPsmPsIdQueue->i8TailIdx=Psm_nQueueInvalidIdxVal;
        boEnequeueOk = FALSE;
    }
    else
    {
        pstPsmPsIdQueue->i8TailIdx=((pstPsmPsIdQueue->i8TailIdx+1)%pstPsmPsIdQueue->u8MaxQueuesize);
        pstPsmPsIdQueue->pau8PsidQueue[pstPsmPsIdQueue->i8TailIdx]=enPsId;
        boEnequeueOk = TRUE;
    }
    return boEnequeueOk;
}

PSM_STATIC boolean Psm__boPsIdDequeue(Psm_tenPsId *penDequeuePsid, Psm__tstPsIdQueue* pstPsmPsIdQueue)
{
    boolean boDeequeueOk = FALSE;
    if((pstPsmPsIdQueue->i8TailIdx==(Psm_nQueueInvalidIdxVal))||(pstPsmPsIdQueue->i8HeadIdx==(Psm_nQueueInvalidIdxVal)))
    {
        //The API Psm_MainFunction_PowerSequence() will be intentionally calling it until empty is reached so we cannot call the Error handling function PSM_E_QUEUE_EMPTY when queue is empty.
        boDeequeueOk = FALSE;
    }
    else if(pstPsmPsIdQueue->i8TailIdx==pstPsmPsIdQueue->i8HeadIdx)
    {
        *penDequeuePsid =(Psm_tenPsId)pstPsmPsIdQueue->pau8PsidQueue[pstPsmPsIdQueue->i8HeadIdx];
        pstPsmPsIdQueue->i8TailIdx=Psm_nQueueInvalidIdxVal;
        pstPsmPsIdQueue->i8HeadIdx=Psm_nQueueInvalidIdxVal;
        boDeequeueOk = TRUE;
    }
    else if(((pstPsmPsIdQueue->i8HeadIdx) >=(pstPsmPsIdQueue->u8MaxQueuesize))||
                  ((pstPsmPsIdQueue->i8TailIdx) >=(pstPsmPsIdQueue->u8MaxQueuesize)))
    {
        /*index error resetting the index*/
        pstPsmPsIdQueue->i8HeadIdx=Psm_nQueueInvalidIdxVal;
        pstPsmPsIdQueue->i8TailIdx=Psm_nQueueInvalidIdxVal;
        boDeequeueOk = FALSE;
    }
    else
    {
        *penDequeuePsid =(Psm_tenPsId)pstPsmPsIdQueue->pau8PsidQueue[pstPsmPsIdQueue->i8HeadIdx];
        pstPsmPsIdQueue->i8HeadIdx=((pstPsmPsIdQueue->i8HeadIdx+1)%pstPsmPsIdQueue->u8MaxQueuesize);
        boDeequeueOk = TRUE;
    }
    return boDeequeueOk;
}
/***************************************************************************
* EOF: psm_internal.c
****************************************************************************/
