#ifndef stub_C
#define stub_C

#ifdef PSMSTUB

/* System headerfiles */
#include "Std_Types.h"

/* Foreign headerfiles */
#include "Dio.h"
#include "Gpt.h"
#include "Icu.h"
#include "Uart_Buffer.h"

/* Own headerfiles */
#include "psm_cfg.h"
#include "psm_internal.h"
#include "psm_types.h"
#include "psm_api.h"
#include "stub.h"

/*
 ****************************
 *   {@Psm_cfg.c} START   *
 ****************************
*/

#define PSM_INVALID_LL_CHANNEL    0xFFFF  /*invalid lower layer channel */


/*--------------------- For  Generation PsmStartup DIO list------------------------*/

/*--------------------- For  Generation PsmStartup action list------------------------*/

/*--------------------- For PsmStartupSequenceList Generation------------------------*/

/********************** Power Sequence List*********************/

extern Psm__tstPowerSequence Psm__astPowerSequence[] ;
extern Psm__tstPsConfig Psm__astPsConfig[];

void Dummy_vfpForceShutdownFunc(void)
{
}

void Dummy_vfpNotifyFunc(Psm_tenSequence enseq, Psm_tenSeqReturn seqreturn, uint8 i)
{
}

Psm_tenFuncReturn Psm_vDelay_Ok(void)
{
    return Psm_nenFuncOk;
}

Psm_tenFuncReturn Psm_vDelay_Wait(void)
{
    return Psm_nenFuncWait;
}

Psm_tenFuncReturn Psm_vDelay_Error(void)
{
    return Psm_nenFuncError;
}

void Psm_voidFunc(void)
{
}

// Enqueue once and do nothing
void Psm_vCallbackPsmPowerSequence_0(Psm_tenPsId enPsId)
{
    if (enPsId < Psm_nenPsCount)
    {
        Psm__EnterCriticalSection();
        if(Psm_boAsyncEventEnqueue(enPsId)==TRUE)
        {
            Psm__ExitCriticalSection();
        }
        else
        {
            Psm__ExitCriticalSection();
            /* log error*/
        }
    }
}

// Execute state machine without enqueue
void Psm_vCallbackPsmPowerSequence_1(Psm_tenPsId enPsId)
{
    if (enPsId < Psm_nenPsCount)
    {
        SchM_ActMainFunction_Psm_IrPsmId_PowerSequence();
    }
}

/*
 ****************************
 *   {@Psm_cfg.c} ENDS   *
 ****************************
*/
#endif //#ifndef PSMSTUB

#endif //#ifndef stub_C
