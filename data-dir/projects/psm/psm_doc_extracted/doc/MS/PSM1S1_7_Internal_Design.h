/*---------------------------------------------------------
Template file for Doxygen based Module Specification

CoDeQ.2018 ADG - Automated Document Generation
---------------------------------------------------------*/
 /**
@page chapter7 Chapter 7: Internal Design
@short Description of the Internal Design

@tableofcontents

@section internal1 7.1 Static View
@startuml
scale 1500x1500
hide empty description

class Psm_Internal {
    +Psm__enMainStateM(Psm_tenPsId) : Psm_tenSeqReturn
    +Psm__enSeqStateM(Psm_tenPsId) : Psm_tenSeqReturn
    +Psm__vStartSeq(Psm_tenPsId, Psm_tenSequence) : void
    +Psm__boMonitor(Psm_tenPsId) : boolean
    +Psm__vStartMonitor(Psm_tenPsId) : void
    +Psm__vStopMonitor(Psm_tenPsId) : void
    +Psm__vForceShutdown(Psm_tenPsId) : void
    +Psm__enDriveIoStateM(Psm_tenPsId) : Psm__tenActReturn
    +Psm__enPollIoStateM(Psm_tenPsId) : Psm__tenActReturn
    +Psm__enSyncDelayStateM(Psm_tenPsId) : Psm__tenActReturn
    +Psm__enAsyncDelayStateM(Psm_tenPsId) : Psm__tenActReturn
    +Psm__enCallFuncStateM(Psm_tenPsId) : Psm__tenActReturn
    +Psm__enCallVoidFuncStateM(Psm_tenPsId) : Psm__tenActReturn
    +Psm__enTriggerSeqStateM(Psm_tenPsId) : Psm__tenActReturn
    +Psm__enWaitSeqStateM(Psm_tenPsId) : Psm__tenActReturn
}

class Psm__tstPowerSequence << (S,orchid) >> {
    +pstConfig : Psm__tstPsConfig*
    +enState : Psm_tenPsState
    +enPubState : Psm_tenPsState
    +enSequence : Psm_tenSequence
    +u32DelayCycle : uint32
    +boDoNotify : boolean
    +boDoForceShutdown : boolean
    +u8SeqStepId : uint8
    +u8SeqSubStepId : uint8
}

class Psm__tstPsConfig << (S,orchid) >> {
    +pastStartupAction : Psm__tstAction*
    +pastShutdownAction : Psm__tstAction*
    +pastForceShutdownDio : Psm__tstDioState*
    +{field}vfpForceShutdownFunc : void(*)(void)
    +{field}vfpNotifyFunc : void(*)(Psm_tenSequence, Psm_tenSeqReturn, uint8)
    +stMonitorDioState : Psm__tstDioState
    +u32MonitorIntervalUs : uint32
    +u16GptChannel : uint16
    +u16IcuChannel : uint16
    +enType : Psm__tenPsType
    +enMonitorType : Psm__tenMonitorType
    +u8NumOfForceShutdownDio : uint8
    +u8NumOfStartupAction : uint8
    +u8NumOfShutdownAction : uint8
}

class Psm__tstAction << (S,orchid) >> {
    +enAction : Psm__tenAction
    +pastDioState : Psm__tstDioState*
    +enTargetPsId : Psm_tenPsId
    +enTriggerPsSequence : Psm_tenSequence
    +enWaitForPsState : Psm_tenPsState
    +{field}vfpUserFunc : Psm_tenFuncReturn(*)(void)
    +{field}vfpVoidUserFunc : void(*)(void)
    +u32DelayInterval : uint32
    +u32DelayCycleLimit : uint32
    +u8NumOfDio : uint8
}

class Psm_Cfg {
    +astPowerSequence : Psm__tstPowerSequence[]
    +{field}astStartupList<PsId(0..*)> : Psm__tstAction[]
    +{field}astShutdownList<PsId(0..*)> : Psm__tstAction[]
    +{field}astSequenceList<PsId(0..*)> : Psm__tstAction[]
}

class Psm_Ca {
    +Psm__vWriteDio(uint16, uint8) : void
    +Psm__boMatchDio(uint16, uint8) : boolean
    +Psm__vStopGptTimer(uint16) : void
    +Psm__vStartGptTimerNonBlocking(uint16, uint32) : void
    +Psm__vStartGptTimerBlocking(uint16, uint32) : void
    +Psm__vStopIcuDetection(uint16) : void
    +Psm__vStartIcuDetection(uint16) : void
}


Psm_Internal -- Psm_Cfg
Psm_Internal -- Psm_Ca
Psm__tstPowerSequence -- Psm_Cfg
Psm__tstAction -- Psm_Cfg
Psm__tstPowerSequence *-- Psm__tstPsConfig
Psm__tstPsConfig "1" *-- "0..*" Psm__tstAction

enum Psm_tenPsId {
    +{field}Psm_nen<PsId(0..*)>
}

Psm_Internal ..> Psm_tenPsId
Psm__tstAction ..> Psm_tenPsId

enum Psm_tenPsState {
    +Psm_nenIdle
    +Psm_nenOff
    +Psm_nenOn
    +Psm_nenFail
    +Psm_nenBusy
}
Psm__tstPowerSequence ..> Psm_tenPsState

enum Psm_tenSequence {
    +Psm_nenNoSequence
    +Psm_nenStartup
    +Psm_nenShutdown
}

Psm_Internal ..> Psm_tenSequence
Psm__tstPowerSequence ..> Psm_tenSequence
Psm__tstPsConfig ..> Psm_tenSequence
Psm__tstAction ..> Psm_tenSequence

enum Psm__tenPsType {
    +Psm__nenSimplePs
    +Psm__nenPowerRailPs
}

Psm__tstPsConfig ..> Psm__tenPsType

enum Psm__tenMonitorType {
    +Psm__nenNoMonitor
    +Psm__nenPolling
    +Psm__nenInterrupt
}

Psm__tstPsConfig ..> Psm__tenMonitorType

class Psm__tstDioState << (S,orchid) >>  {
    +u16DioChannel : uint16
    +u8DioLevel : uint8
}

Psm__tstPsConfig ..> Psm__tstDioState
Psm__tstAction ..> Psm__tstDioState

enum Psm__tenAction {
    +Psm__nenInvalidAction
    +Psm__nenDriveIo
    +Psm__nenPollIo
    +Psm__nenSyncDelay
    +Psm__nenAsyncDelay
    +Psm__nenCallFunc
    +Psm__nenCallVoidFunc
    +Psm__nenTriggerSeq
    +Psm__nenWaitSeq
}

Psm__tstAction ..> Psm__tenAction

enum Psm_tenSeqReturn {
    +Psm_nenSeqComplete
    +Psm_nenSeqAsync
    +Psm_nenSeqAbort
    +Psm_nenSeqMonitorFail
}

Psm_Internal ..> Psm_tenSeqReturn
Psm__tstPsConfig ..> Psm_tenSeqReturn

enum Psm__tenActReturn {
    +Psm__nenActComplete
    +Psm__nenActBusy
    +Psm__nenActAsync
    +Psm__nenActAbort
}

Psm_Internal ..> Psm__tenActReturn

enum Psm_tenFuncReturn {
    +Psm_nenFuncOk
    +Psm_nenFuncError
    +Psm_nenFuncWait
}

Psm__tstAction ..> Psm_tenFuncReturn
@enduml

C = CLASS
<br>
S = STRUCT
<br>
E = ENUM

@subsection internal11 7.1.1 Description of internal functions


| Function Description | PSM_INTERNAL                                                                               |
|----------------------|--------------------------------------------------------------------------------|
| **Prototype**        | **Psm_tenSeqReturn Psm__enMainStateM(Psm_tenPsId enPsId)**                     |
| Documentation        | Executes the init/deinit sequences defined. Returns when init/deinit is completed, an asynchronous step is reached or and error occured.  |
| Parameters           | enPsId: Power sequence ID                                                      |
| Return Values        | enSeqReturn: Sequence ID is returned                                           |
|                      |                                                                                |
| **Prototype**        | **Psm_tenSeqReturn Psm__enSeqStateM(Psm_tenPsId enPsId)**                      |
| Documentation        | Start the init/deinit sequences from first step based on action type           |
| Parameters           | enPsId: Power sequence ID                                                      |
| Return Values        | enSeqReturn: Sequence ID is returned                                           |
|                      |                                                                                |
| **Prototype**        | **void Psm__vStartSeq(Psm_tenPsId enPsId, Psm_tenSequence enSequence)**        |
| Documentation        | Start the init/deinit sequences from first step.                               |
| Parameters           | enPsId: Power sequence ID                                                      |
| Return Values        | void                                                                           |
|                      |                                                                                |
| **Prototype**        | **boolean Psm__boMonitor(Psm_tenPsId enPsId)**                                 |
| Documentation        | Function to enable & disbale monitoring for the configured ICU channel                                       |
| Parameters           | enPsId: Power sequence ID                                                      |
| Return Values        | True - Monitor is started .<br>False - Monitor is stopped                      |
|                      |                                                                                |
| **Prototype**        | **void Psm__vStartMonitor(Psm_tenPsId enPsId)**                                |
| Documentation        | Function to start monitoring for the configured ICU channel                                                   |
| Parameters           | enPsId: Power sequence ID                                                      |
| Return Values        | void                                                                           |
|                      |                                                                                |
| **Prototype**        | **void Psm__vStopMonitor(Psm_tenPsId enPsId)**                                 |
| Documentation        | Function to stop monitoring for the configured ICU channel                                                    |
| Parameters           | enPsId: Power sequence ID                                                      |
| Return Values        | void                                                                           |
|                      |                                                                                |
| **Prototype**        | **void Psm__vForceShutdown(Psm_tenPsId enPsId)**                               |
| Documentation        | Bypass the main state machine and skip directly to user notification           |
| Parameters           | enPsId: Power sequence ID                                                      |
| Return Values        | void                                                                           |
|                      |                                                                                |
| **Prototype**        | **Psm__tenActReturn Psm__enDriveIoStateM(Psm_tenPsId enPsId, const Psm__tstActionDriveIo *pstActionDriveIo)**  |
| Documentation        | Set the specified DIO channel(s) to the specified state.                       |
| Parameters           | enPsId: Power sequence ID<br>pstActionDriveIo: No of Drive IOs                 |
| Return Values        | enActReturn: Action ID is returned                                                |
|                      |                                                                                |
| **Prototype**        | **Psm__tenActReturn Psm__enPollIoStateM(Psm_tenPsId enPsId, const Psm__tstActionPollIo *pstActionPollIo)**  |
| Documentation        | Poll and wait for the specified DIO channel(s) to reach the specified state    |
| Parameters           | enPsId: Power sequence ID<br>pstActionPollIo: No of Poll IOs                   |
| Return Values        | enActReturn: Action ID is returned                                             |
|                      |                                                                                |
| **Prototype**        | **Psm__tenActReturn Psm__enSyncDelayStateM(Psm_tenPsId enPsId, const Psm__tstActionDelay *pstActionDelay)**  |
| Documentation        | Blocking wait until the specified delay duration is expired (not recommended for long delay)  |
| Parameters           | enPsId: Power sequence ID<br>pstActionDelay: No of delay intervals             |
| Return Values        | enActReturn: Action ID is returned                                             |
|                      |                                                                                |
| **Prototype**        | **Psm__tenActReturn Psm__enAsyncDelayStateM(Psm_tenPsId enPsId, const Psm__tstActionDelay *pstActionDelay)**  |
| Documentation        | Non-blocking wait until the specified delay duration is expired                |
| Parameters           | enPsId: Power sequence ID<br>pstActionDelay: No of delay intervals             |
| Return Values        | enActReturn: Action ID is returned                                             |
|                      |                                                                                |
| **Prototype**        | **Psm__tenActReturn Psm__enCallFuncStateM(Psm_tenPsId enPsId, const Psm__tstActionFunc *pstActionFunc)**  |
| Documentation        | Call the specified user function (to perform custom actions) (wait state is supported)  |
| Parameters           | enPsId: Power sequence ID<br>pstActionFunc: User defined function              |
| Return Values        | enActReturn: Action ID is returned                                             |
|                      |                                                                                |
| **Prototype**        | **Psm__tenActReturn Psm__enCallVoidFuncStateM(Psm_tenPsId enPsId, const Psm__tstActionVoidFunc *pstActionVoidFunc)**  |
| Documentation        | Call the specified user function (to perform custom actions) (wait state is not supported)  |
| Parameters           | enPsId: Power sequence ID<br>pstActionVoidFunc: Void function                  |
| Return Values        | enActReturn: Action ID is returned                                             |
|                      |                                                                                |
| **Prototype**        | **Psm__tenActReturn Psm__enTriggerSeqStateM(Psm_tenPsId enPsId, const Psm__tstActionTriggerPs *pstActionTriggerPs)**  |
| Documentation        | Trigger the specified power sequence (PS) to start the specified sequence.     |
| Parameters           | enPsId: Power sequence ID<br>pstActionTriggerPs: Pointing to target power sequence ID  |
| Return Values        | enActReturn: Action ID is returned                                             |
|                      |                                                                                |
| **Prototype**        | **Psm__tenActReturn Psm__enWaitSeqStateM(Psm_tenPsId enPsId, const Psm__tstActionWaitPs *pstActionWaitPs)**  |
| Documentation        | Poll and wait for the specified power sequence (PS) to reach the specified state   |
| Parameters           | enPsId: Power sequence ID<br>pstActionWaitPs: Pointing to target power sequence ID  |
| Return Values        | enActReturn: Action ID is returned                                             |

| Function Description | PSM_ABSTRACT                                                                   |
|----------------------|--------------------------------------------------------------------------------|
| **Prototype**        | **void Psm__vWriteDio(uint16 u16DioChannel, uint8 u8DioLevel)**                |
| Documentation        | Drive the DIO channel based on DIO level                                       |
| Parameters           | u16DioChannel: Configure GPIO channel<br>u8DioLevel: STD_HIGH/STD_LOW          |
| Return Values        | void                                                                           |
|                      |                                                                                |
| **Prototype**        | **boolean Psm__boMatchDio(uint16 u16DioChannel, uint8 u8DioLevel)**            |
| Documentation        | Compare DIO channel state and DIO level                                        |
| Parameters           | u16DioChannel: Configure GPIO channel <br>u8DioLevel: STD_HIGH/STD_LOW         |
| Return Values        | TRUE: DIO read level is equal to set DIO level<br>FALSE: DIO read level is not equal to set DIO level.  |
|                      |                                                                                |
| **Prototype**        | **void Psm__vStopGptTimer(uint16 u16GptChannel)**                              |
| Documentation        | Stops the GPT timer                                                            |
| Parameters           | u16GptChannel: Configure GPT channel                                           |
| Return Values        | void                                                                           |
|                      |                                                                                |
| **Prototype**        | **void Psm__vStartGptTimerNonBlocking(uint16 u16GptChannel, uint32 u32TimeUs)**|
| Documentation        | It is called during asynchronous action call                                   |
| Parameters           | u16GptChannel: Configure GPT channel<br>u32TimeUs: Configure GPT time delay    |
| Return Values        | void                                                                           |
|                      |                                                                                |
| **Prototype**        | **void Psm__vStartGptTimerBlocking(uint16 u16GptChannel, uint32 u32TimeUs)**   |
| Documentation        | It is called during synchronous action call                                    |
| Parameters           | u16GptChannel: Configure GPT channel<br>u32TimeUs: Configure GPT time delay    |
| Return Values        | void                                                                           |
|                      |                                                                                |
| **Prototype**        | **void Psm__vStopIcuDetection(uint16 u16IcuChannel)**                          |
| Documentation        | Stops the ICU detection                                                        |
| Parameters           | u16IcuChannel: Configure ICU channel                                           |
| Return Values        | void                                                                           |
|                      |                                                                                |
| **Prototype**        | **void Psm__vStartIcuDetection(uint16 u16IcuChannel)**                         |
| Documentation        | Starts the ICU detection                                                       |
| Parameters           | u16IcuChannel: Configure ICU channel                                           |
| Return Values        | void                                                                           |

@subsection internal12 7.1.2 Variables, Types, Constants, Macros

|<span style="display: inline-block; width:500px">Variable name</span> | <span style="display: inline-block; width:700px">Description</span>|
|--------------------------|------------------------------------------------------------------------------|
| Psm__astPowerSequence | Represents the power sequences configurations. |

|<span style="display: inline-block; width:500px">Constant name</span> | <span style="display: inline-block; width:700px">Description</span>  |
|--------------------------|------------------------------------------------------------------------------|
| None | |

|<span style="display: inline-block; width:500px">Type name</span> | <span style="display: inline-block; width:700px">Description</span> |
|--------------------------|------------------------------------------------------------------------------|
| Psm_tenSeqReturn |- Psm_nenSeqComplete = Sequence is completed<br>- Psm_nenSeqAsync = Sequence is asynchronous<br>- Psm_nenSeqAbort = Sequence is aborted<br>- Psm_nenSeqMonitorFail = Monitored signal is failed|
| Psm_tenSequence |- Psm_nenNoSequence = No sequence is passed<br>- Psm_nenStartup = Sequence is Startup<br>- Psm_nenShutdown = Sequence is Shutdown|
| Psm__tenActReturn |- Psm__nenActComplete = Action return is Complete<br>- Psm__nenActBusy = Action return is Busy<br>- Psm__nenActAsync = Action return is Asynchronous<br>- Psm__nenActAbort = Action return is Abort |

|<span style="display: inline-block; width:500px">API Codes Macro name</span>| <span style="display: inline-block; width:700px">Description</span> |
|--------------------------|------------------------------------------------------------------------------|
|PSM_API_GET_STATE               |Get the current state of a power sequence.<br> Function name: Psm_enGetState  |
|PSM_API_RUN_SEQUENCE            |Run a specified power sequence.<br> Function name: Psm_boRunSequence |
|PSM_API_STARTUP                 |Start the power sequence manager.<br> Function name: Psm_boStartup |
|PSM_API_SHUTDOWN                |Shutdown the power sequence manager.<br> Function name:Psm_boShutdown|
|PSM_API_RESUME                  |Resume a paused or interrupted power sequence.<br> Function name:Psm_boResume |
|PSM_API_START_MONITORING        |Start monitoring the power sequence manager.<br> Function name:Psm_vStartMonitoring |
|PSM_API_STOP_MONITORING         |Stop monitoring the power sequence manager.<br> Function name:Psm_vStopMonitoring|
|PSM_API_TRIGGER_SEQSTATE        |Trigger a specific state in the power sequence manager.<br> Function name: Psm__enTriggerSeqStateM|
|PSM_API_CALL_VOIDFUNCTIONSTATE  |Call a void function in the power sequence manager.<br> Function name: Psm__enCallVoidFuncStateM|
|PSM_API_CALL_FUNCTIONSTATE      |Call a user defined function in the power sequence manager.<br> Function name: Psm__enCallFuncStateM |
|PSM_API_POLL_IOSTATE            |Poll the state of the I/O channel.<br> Function name: Psm__enPollIoStateM|
|PSM_API_DRIVE_IOSTATE           |Drive the state of the I/O channel.<br> Function name: Psm__enDriveIoStateM |
|PSM_API_FORCE_SHUTDOWN          |Force shutdown the power sequence manager.<br> Function name: Psm__vForceShutdown |
|PSM_API_SEQ_STATE               |Get the current state of the power sequence manager.<br> Function name: Psm__enSeqStateM |
|PSM_API_DEQUEUE                 |Dequeue a power sequence from the manager.<br> Function name: Not in use |
|PSM_API_ENQUEUE                 |Enqueue a power sequence into the manager.<br> Function name: Psm__boPsIdEnequeue |
|PSM_API_ASYNC_DELAYSTATE        |Handle asynchronous delay state.<br>  Function name: Psm__enAsyncDelayStateM|
|PSM_API_SYNC_DELAYSTATE         |Handle synchronous delay state.<br> Function name: Psm__enSyncDelayStateM |

|<span style="display: inline-block; width:500px">Error Codes Macro name</span>| <span style="display: inline-block; width:700px">Description</span> |
|--------------------------|------------------------------------------------------------------------------|
|PSM_E_NULL_POINTER                       | Null pointer error. |
|PSM_E_PARAM_INVALID_POWER_SEQUENCE       | Invalid power sequence parameter. |
|PSM_E_PARAM_INVALID_POWER_SEQUENCE_TYPE  | Invalid power sequence type parameter.
|PSM_E_INVALID_POWER_SEQUENCE_STATE       | Invalid or unexpected power sequence state. |
|PSM_E_INVALID_ACTION                     | Invalid action specified in the sequence. |
|PSM_E_INVALID_CONFIG                     | Invalid configuration detected. |
|PSM_E_NO_SEQUENCE                        | No valid power sequence found. |
|PSM_E_QUEUE_EMPTY                        | Queue is empty when a dequeue is attempted. |
|PSM_E_QUEUE_FULL                         |Queue is full when an enqueue is attempted |

@section internal2 7.2 Dynamic View

@subsection internal22 7.2.1 Psm__vStartSeq
The following sequence diagram describes the start sequence of PSM
@startuml
hide footbox
autoactivate on
participant Psm_Api
participant Psm_Internal
participant Psm_Cfg
participant Dio
participant Gpt
participant Icu

activate Psm_Api
Psm_Api -> Psm_Internal : Psm__vStartSeq(PS_ID, STARTUP/SHUTDOWN)
    Psm_Internal -> Psm_Internal : Set public state
    deactivate
    note right
        enPubState = BUSY
    end note
    alt STARTUP requested
        Psm_Internal -> Psm_Internal : Set internal states
        deactivate
        note right
            enSequence = STARTUP
            boDoForceShutdown = FALSE
        end note
    else SHUTDOWN requested
        alt enState == FAIL
            Psm_Internal -> Psm_Internal : Set internal states
            deactivate
            note right
                enSequence = SHUTDOWN
                boDoForceShutdown = TRUE
            end note
        else
            Psm_Internal -> Psm_Internal : Set internal states
            deactivate
            note right
                enSequence = SHUTDOWN
                boDoForceShutdown = FALSE
            end note
        end alt
    end alt
    Psm_Internal -> Psm_Internal : Set internal states
    deactivate
    note right
        enState = BUSY
        u8SeqStepId = 0
        u8SeqSubStepId = 0
    end note
    alt enSequence == SHUTDOWN
        Psm_Internal -> Psm_Internal : Psm__vStopMonitor(PS_ID)
            alt Polling-based monitoring
                Psm_Internal -> Gpt : Stop Gpt timer
                deactivate
            else Interrupt-based monitoring
                Psm_Internal -> Icu : Disable Icu interrupt
                deactivate
            end alt
        deactivate
    end alt
    alt boDoForceShutdown == TRUE
        Psm_Internal -> Psm_Internal : Psm__vForceShutdown(PS_ID)
            Psm_Internal -> Dio : Set the specified DIOs to their initial states
            deactivate
            note right
                When forced shutdown is used, the normal shutdown sequence will not be called.
            end note
            Psm_Internal -> Psm_Internal : Set internal states
            deactivate
            note right
                enState = OFF
                boDoNotify = TRUE
            end note
        deactivate
    end alt
return
@enduml

@subsection internal21 7.2.2 Psm__enMainStateM
The following sequence diagram shows the Main State function
@startuml
hide footbox
autoactivate on
participant App
participant Psm_Api
participant Psm_Internal
participant Psm_Cfg
participant Dio
participant Gpt
participant Icu

activate Psm_Api
Psm_Api -> Psm_Internal : Psm__enMainStateM(PS_ID)
    alt enState == BUSY
        Psm_Internal -> Psm_Internal : Psm__enSeqStateM(PS_ID)
            loop (enState == BUSY) && (boBreakLoop == FALSE)
                alt enSequence == STARTUP
                    Psm_Internal -> Psm_Cfg : Get Startup action list
                    return
                else
                    Psm_Internal -> Psm_Cfg : Get Shutdown action list
                    return
                end alt
                alt ActionList[u8SeqStepId].enAction == DRIVE_IO
                    Psm_Internal -> Psm_Internal : Psm__enDriveIoStateM()
                        Psm_Internal -> Dio : Set DIO state
                        deactivate
                    return enActionReturn = ACTION_COMPLETE
                    |||
                else ActionList[u8SeqStepId].enAction == SYNC_DELAY
                    |||
                    Psm_Internal -> Psm_Internal : Psm__enSyncDelayStateM()
                        Psm_Internal -> Gpt : Start Gpt timer
                        deactivate
                        loop Until Gpt timer is done
                            Psm_Internal -> Gpt : Get Gpt timer status
                            return
                        end loop
                    return enActionReturn = ACTION_COMPLETE
                    |||
                else ActionList[u8SeqStepId].enAction == ASYNC_DELAY
                    |||
                    Psm_Internal -> Psm_Internal : Psm__enAsyncDelayStateM()
                        alt u8SeqSubStepId == 0
                            Psm_Internal -> Gpt : Start Gpt timer
                            deactivate
                            Psm_Internal -> Psm_Internal : Set internal states
                            deactivate
                            note right
                                enActionReturn = ACTION_ASYNC
                                u8SeqSubStepId++
                            end note
                        else u8SeqSubStepId == 1
                            Psm_Internal -> Gpt : Get Gpt timer status
                            return
                            alt Gpt timer is done
                                Psm_Internal -> Psm_Internal : Set internal states
                                deactivate
                                note right
                                    enActionReturn = ACTION_COMPLETE
                                    u8SeqSubStepId = 0
                                end note
                            else
                                Psm_Internal -> Psm_Internal : enActionReturn = ACTION_ASYNC
                                deactivate
                            end alt
                        end alt
                    return enActionReturn
                    |||
                else ... other actions ...
                    |||
                else ActionList[u8SeqStepId].enAction == INVALID_ACTION
                    |||
                    Psm_Internal -> Psm_Internal : End of action list
                    deactivate
                    note right
                        enActionReturn = ACTION_COMPLETE
                        boDoNotify = TRUE
                        enSeqReturn = SEQUENCE_COMPLETE
                    end note
                    alt enSequence == STARTUP
                        Psm_Internal -> Psm_Internal : enState = ON
                        deactivate
                    else
                        Psm_Internal -> Psm_Internal : enState = OFF
                        deactivate
                    end alt
                end alt
                alt enState != BUSY
                    Psm_Internal -> Psm_Internal : Do nothing
                    deactivate
                else enActionReturn == ACTION_COMPLETE
                    Psm_Internal -> Psm_Internal : Go to next action
                    deactivate
                    note right
                        u8SeqStepId++
                        u8SeqSubStepId = 0
                    end note
                else enActionReturn == ACTION_ASYNC
                    Psm_Internal -> Psm_Internal : Stay in current action and break from loop
                    deactivate
                    note right
                        boBreakLoop = TRUE
                        enSeqReturn = SEQUENCE_ASYNC
                    end note
                else enActionReturn == ACTION_ABORT
                    Psm_Internal -> Psm_Internal : Failure occurred
                    deactivate
                    note right
                        enState = FAIL
                        boDoNotify = TRUE
                        enSeqReturn = SEQUENCE_ABORT
                    end note
                end alt
            end loop
        return enSeqReturn
    end alt
    alt (enType == RAIL) && (enState == ON)
        Psm_Internal -> Psm_Internal : Psm__vMonitor(PS_ID)
            Psm_Internal -> Dio : Read the monitoring DIO state
            deactivate
            alt DIO state is OK
                alt Polling-based monitoring
                    Psm_Internal -> Gpt : Start Gpt timer for the next polling interval
                    deactivate
                else Interrupt-based monitoring
                    Psm_Internal -> Icu : Enable Icu interrupt on the monitoring DIO
                    deactivate
                end alt
            else DIO state is NOT OK
                alt Interrupt-based monitoring
                    Psm_Internal -> Icu : Disable Icu interrupt on the monitoring DIO
                    deactivate
                end alt
                Psm_Internal -> Psm_Internal : Set internal states
                deactivate
                note right
                    enState = FAIL
                    boDoNotify = TRUE
                    enSeqReturn = SEQUENCE_MONITOR_FAIL
                end note
            end alt
        deactivate
    end alt
    alt boDoNotify == TRUE
        Psm_Internal -> Psm_Internal : Psm__vNotify(PS_ID, enSequence, enSeqReturn)
            alt vfpNotifyFunc != NULL_PTR
                Psm_Internal -> App : vfpNotifyFunc(enSequence, enSeqReturn, u8SeqStepId)
                deactivate
                note right
                    Call user notification function
                end note
            end alt
        deactivate
        Psm_Internal -> Psm_Internal : boDoNotify = FALSE
        deactivate
    end alt
    alt (enState == ON) || (enState == OFF)
        Psm_Internal -> Psm_Internal : Set internal states
        deactivate
        note right
            enSequence = NO_SEQUENCE
            enSeqReturn = SEQUENCE_COMPLETE
            Do not clear sequence if the state is still busy or fail
            Force return to sequence complete (useful for forced shutdown sequence)
        end note
    end alt
    Psm_Internal -> Psm_Internal : Set public state
    deactivate
    note right
        enPubState = enState
    end note
return enSeqReturn
@enduml

@section internal3 7.3 Exception and Error Handling
Exception and error handling in the PSM module is managed through centralized error reporting and early exit strategies. 
All error conditions, such as invalid sequences, null pointers, and action failures, are detected within the relevant state machine functions. 
When an error is identified, the PSM_ERROR_CALLOUT_FUNCTION macro is used to log the error with details including module, instance, API, and error type. 
Functions return immediately after reporting a critical error, ensuring only one error code is issued per failure and preventing further processing. 

User notification function(Psm_vCallbackPsName) will be called at the end of power sequence and when failure occurs.
@section internal4 7.4 Further Implementation Aspects
None.

@section internal5 7.5 Verification Criteria
No project specific tests. All tests are available as per Module Test Specification.

*/
