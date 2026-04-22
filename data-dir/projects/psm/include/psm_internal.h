/***************************************************************************
*======= Copyright (c) [Year 2023] Continental AG and subsidiaries =========
****************************************************************************
* Copyrights should use the oldest year for the file and should not be updated each year.
*
* Title        : psm_internal.h
*
* Description  : Header file of internal interfaces of PSM module
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
***************************************************************************/

/**
* Check if information is already included
*/
#ifndef PSM_INTERNAL_H
#define PSM_INTERNAL_H


/***************************************************************************
* HEADER-FILES (Only those that are needed in this file)
****************************************************************************/
/* System-headerfiles */
#include "Std_Types.h"

/* Foreign headerfiles */

/* Own headerfiles */
#include "psm_types.h"
#include <SchM_Psm.h>
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
#define Psm__EnterCriticalSection() SchM_Enter_Psm_INTERRUPT_CONTROL_PROTECTION_AREA()
#define Psm__ExitCriticalSection()  SchM_Exit_Psm_INTERRUPT_CONTROL_PROTECTION_AREA()

#define Psm_nQueueInvalidIdxVal (-1)

#ifndef PSM_STATIC
#define PSM_STATIC STATIC
#endif

#define HIGHZ_INBUFF_ON 2U
#define HIGHZ_INBUFF_OFF 3U

#define PSM_MODULE_ID    81U
#define PSM_INSTANCE_ID  0U

/*Defines for PSM API Codes*/
#define PSM_API_GET_STATE                               0U
#define PSM_API_RUN_SEQUENCE                            1U
#define PSM_API_STARTUP                                 2U
#define PSM_API_SHUTDOWN                                3U
#define PSM_API_RESUME                                  4U
#define PSM_API_START_MONITORING                        5U
#define PSM_API_STOP_MONITORING                         6U
#define PSM_API_TRIGGER_SEQSTATE                        7U
#define PSM_API_CALL_VOIDFUNCTIONSTATE                  8U
#define PSM_API_CALL_FUNCTIONSTATE                      9U
#define PSM_API_POLL_IOSTATE                            10U
#define PSM_API_DRIVE_IOSTATE                           11U
#define PSM_API_FORCE_SHUTDOWN                          12U
#define PSM_API_SEQ_STATE                               13U
#define PSM_API_DEQUEUE                                 15U
#define PSM_API_ENQUEUE                                 16U
#define PSM_API_ASYNC_DELAYSTATE                      17U
#define PSM_API_SYNC_DELAYSTATE                       18U
#define PSM_API_START_SEQ                            19U

/*Define for PSM Error Codes*/
#define PSM_E_NULL_POINTER                              0U
#define PSM_E_PARAM_INVALID_POWER_SEQUENCE              1U
#define PSM_E_PARAM_INVALID_POWER_SEQUENCE_TYPE         2U
#define PSM_E_INVALID_POWER_SEQUENCE_STATE              3U
#define PSM_E_INVALID_ACTION                            4U
#define PSM_E_INVALID_CONFIG                            5U
#define PSM_E_NO_SEQUENCE                               6U
#define PSM_E_QUEUE_EMPTY                               7U
#define PSM_E_QUEUE_FULL                               8U


enum Psm__enPsType
{
    Psm__nenSimplePs,
    Psm__nenPowerRailPs,
    Psm__nenPsTypeCount
};
typedef enum Psm__enPsType Psm__tenPsType;

enum Psm__enMonitorType
{
    Psm__nenNoMonitor,
    Psm__nenPolling,
    Psm__nenInterrupt
};
typedef enum Psm__enMonitorType Psm__tenMonitorType;

struct Psm__stDioState
{
    uint16 u16DioChannel;
    uint8 u8DioLevel;
};
typedef struct Psm__stDioState Psm__tstDioState;

enum Psm__enActReturn {
    Psm__nenActComplete,
    Psm__nenActBusy,
    Psm__nenActAsync,
    Psm__nenActAbort
};
typedef enum Psm__enActReturn Psm__tenActReturn;

enum Psm__enAction
{
    Psm__nenInvalidAction,
	Psm__nenDriveDio,
    Psm__nenPollDio,
    Psm__nenSynchronousDelay,
	Psm__nenAsynchronousDelay,
    Psm__nenCallUserFunction,
    Psm__nenCallUserFunction_VoidReturn,
    Psm__nenTriggerPowerSequence,
    Psm__nenWaitForPowerSequence,
    Psm__nenActionCount
};
typedef enum Psm__enAction Psm__tenAction;

struct Psm__stActionDriveIo {
    Psm__tstDioState *pastDioState;
    uint8 u8NumOfDio;
};
typedef struct Psm__stActionDriveIo Psm__tstActionDriveIo;

struct Psm__stActionPollIo {
    Psm__tstDioState *pastDioState;
    uint32 u32WaitIntervalUs;
    uint32 u32WaitCycleLimit;
    uint8 u8NumOfDio;
    boolean boBlockingWait;
};
typedef struct Psm__stActionPollIo Psm__tstActionPollIo;

struct Psm__stActionDelay {
    uint32 u32DelayIntervalUs;
    uint32 u32DelayCycleLimit;
};
typedef struct Psm__stActionDelay Psm__tstActionDelay;

struct Psm__stActionFunc {
    Psm_tenfpUserFunc enfpUserFunc;
    uint32 u32WaitIntervalUs;
    uint32 u32WaitCycleLimit;
    boolean boBlockingWait;
};
typedef struct Psm__stActionFunc Psm__tstActionFunc;

struct Psm__stActionVoidFunc {
    Psm_tvfpVoidUserFunc vfpVoidUserFunc;
};
typedef struct Psm__stActionVoidFunc Psm__tstActionVoidFunc;

struct Psm__stActionTriggerPs {
    Psm_tenPsId enTargetPsId;
    Psm_tenSequence enTriggerPsSequence;
};
typedef struct Psm__stActionTriggerPs Psm__tstActionTriggerPs;

struct Psm__stActionWaitPs {
    Psm_tenPsId enTargetPsId;
    Psm_tenPsState enWaitForPsState;
    uint32 u32WaitIntervalUs;
    uint32 u32WaitCycleLimit;
    boolean boBlockingWait;
};
typedef struct Psm__stActionWaitPs Psm__tstActionWaitPs;

struct Psm__stAction {
    Psm__tenAction enAction;
    void *pvActionData;
};
typedef struct Psm__stAction Psm__tstAction;

struct Psm__stPsConfig
{
    Psm__tstAction *pastStartupAction;
    Psm__tstAction *pastShutdownAction;
    Psm__tstDioState *pastForceShutdownDio;
    Psm_tvfpVoidUserFunc vfpForceShutdownFunc;
    Psm_tvfpNotifyFunc vfpNotifyFunc;
    Psm__tstDioState stMonitorDioState;
    uint32 u32MonitorIntervalUs;
    uint16 u16GptChannel;
    uint16 u16IcuChannel;
    Psm__tenPsType enType;
    Psm__tenMonitorType enMonitorType;
    uint8 u8NumOfForceShutdownDio;
    uint8 u8NumOfStartupAction;
    uint8 u8NumOfShutdownAction;
};
typedef struct Psm__stPsConfig Psm__tstPsConfig;

#ifndef GTEST
struct Psm__stPowerSequence
{
    const Psm__tstPsConfig *const pstConfig;
    Psm_tenPsState enState;
    Psm_tenPsState enPubState;
    Psm_tenSequence enSequence;
    uint32 u32DelayCycle;
    boolean boDoNotify;
    boolean boDoForceShutdown;
    uint8 u8SeqStepId;
    uint8 u8SeqSubStepId;
};
#endif

#ifdef GTEST
struct Psm__stPowerSequence
{
    Psm__tstPsConfig *pstConfig;
    Psm_tenPsState enState;
    Psm_tenPsState enPubState;
    Psm_tenSequence enSequence;
    uint32 u32DelayCycle;
    boolean boDoNotify;
    boolean boDoForceShutdown;
    uint8 u8SeqStepId;
    uint8 u8SeqSubStepId;
};
#endif
typedef struct Psm__stPowerSequence Psm__tstPowerSequence;

struct Psm__stPsidQueue
{
	uint8 *pau8PsidQueue;
	sint8 i8HeadIdx;
	sint8 i8TailIdx;
	uint8 u8MaxQueuesize;
};
typedef struct Psm__stPsidQueue Psm__tstPsIdQueue;
/***************************************************************************
* MODULE GLOBAL FUNCTION PROTOTYPES
*
* In this section declare
* - all module global function prototypes of your module
****************************************************************************/

void Psm__vStartMonitor(Psm_tenPsId enPsId);
void Psm__vStopMonitor(Psm_tenPsId enPsId);
boolean Psm__boMonitor(Psm_tenPsId enPsId);
void Psm__vForceShutdown(Psm_tenPsId enPsId);
void Psm__vStartSeq(Psm_tenPsId enPsId, Psm_tenSequence enSequence);
void Psm__vResumeSeq(Psm_tenPsId enPsId);
Psm_tenSeqReturn Psm__enSeqStateM(Psm_tenPsId enPsId);
Psm_tenSeqReturn Psm__enMainStateM(Psm_tenPsId enPsId);
boolean Psm_boAsyncEventEnqueue(Psm_tenPsId enPsId);
boolean Psm_boAsyncEventDequeue(Psm_tenPsId* penPsId);
/*
* End of Check if information is already included
*/
#endif                                  /* ifndef PSM_INTERNAL_H */

/***************************************************************************
* EOF: psm_internal.h
****************************************************************************/
