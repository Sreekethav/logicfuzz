/*---------------------------------------------------------
Template file for Doxygen based Module Specification

CoDeQ.2018 ADG - Automated Document Generation
---------------------------------------------------------*/
 /**
@page chapter5 Chapter 5: Export Interface
@short Description of the Export Interface

@tableofcontents

@section export1 5.1 Static View
@startuml
hide empty description

class Psm_Api {
    +Psm_enGetState(Psm_tenPsId) : Psm_tenPsState
    +Psm_boRunSequence(Psm_tenPsId) : boolean
    +Psm_boStartup(Psm_tenPsId) : boolean
    +Psm_boShutdown(Psm_tenPsId) : boolean
    +Psm_boResume(Psm_tenPsId) : boolean
    +Psm_vStartMonitoring(Psm_tenPsId) : void
    +Psm_vStopMonitoring(Psm_tenPsId) : void
}

enum Psm_tenPsState {
    +Psm_nenIdle
    +Psm_nenOff
    +Psm_nenOn
    +Psm_nenFail
    +Psm_nenBusy
}

Psm_Api ..> Psm_tenPsState

enum Psm_tenPsId {
    +{field}Psm_nen<PsId(0..*)>
}

Psm_Api ..> Psm_tenPsId
@enduml

C = CLASS
<br>
E = ENUM

@subsection export11 5.1.1 Functions

| Function Description  |                                                                                               |
|-----------------------|-----------------------------------------------------------------------------------------------|
| **Prototype**         | **boolean Psm_boRunSequence(Psm_tenPsId enPsId)**                                             |
| Documentation         | Run the power sequence specified by enPsId.<br>Only applicable to Simple Sequence type in idle state. |
| Parameters            | enPsId: Power sequence ID                                                                                          |
| Return Values         | True - No error, the sequence has been completed or is executing an asynchronous action.<br>False - Pre-condition not fulfilled or error occurred during the power sequence. |
|                       |                                                                                               |
| **Prototype**         | **boolean Psm_boStartup(Psm_tenPsId enPsId)**                                                 |
| Documentation         | Run the startup power sequence specified by enPsId.<br>If enabled, the monitoring will be activated at the end of sequence.<br>Only applicable to Power Rail Sequence type in off state. |
| Parameters            | enPsId: Power sequence ID                                                                                          |
| Return Values         | True - No error, the sequence has been completed or is executing an asynchronous action.<br>False - Pre-condition not fulfilled or error occurred during the power sequence. |
|                       |                                                                                               |
| **Prototype**         | **boolean Psm_boShutdown(Psm_tenPsId enPsId)**                                                |
| Documentation         | Run the shutdown power sequence (PS) specified by enPsId.<br>Only applicable to Power Rail Sequence type in ON or FAIL state.<br>If the current state of the PS is on state, normal shutdown sequence will be executed.<br>If the current state of the PS is fail state, forced shutdown sequence will be executed.|
| Parameters            | enPsId: Power sequence ID                                                                                          |
| Return Values         | True - No error, the sequence has been completed or is executing an asynchronous action<br>False - Pre-condition not fulfilled or error occurred during the power sequence|
|                       |                                                                                               |
| **Prototype**         | **Psm_tenPsState Psm_enGetState(Psm_tenPsId enPsId)**                                         |
| Documentation         | Return the state of the power sequence (PS) specified by enPsId.                              |
| Parameters            | enPsId: Power sequence ID                                                                                          |
| Return Values         | Psm_nenIdle - Only for simple PS type. The PS is idling or has encountered an error.<br>Psm_nenOff - Only for power rail PS type. The PS is in OFF state (shutdown PS has been completed or the PS has not been called before).<br>Psm_nenOn - Only for power rail PS type. The PS is in ON state (startup PS has been completed).<br>Psm_nenFail - Only for power rail PS type. The PS has encountered an error.<br>Psm_nenBusy - The PS is being executed. |
|                       |                                                                                               |
| **Prototype**         | **boolean Psm_boResume(Psm_tenPsId enPsId)**               |
| Documentation         | Continue running the power sequence specified by enPsId.<br>Only applicable to Power Rail Sequence type in FAIL state.<br>The failed action will be retried and then the subsequent actions will be executed accordingly.                                                     |
| Parameters            | enPSId: Power sequence ID                                                                       |
| Return Values         | True - No error, the sequence has been completed or is executing an asynchronous action<br>False - Pre-condition not fulfilled or error occurred during the power sequence                                                                                |
|                       |                                                                                               |
| **Prototype**         | **void Psm_vStartMonitoring(Psm_tenPsId enPsId)**                                             |
| Documentation         | Start the monitoring specified by enPsId.<br>Do nothing if the monitoring is already started.<br>Only applicable to Power Rail Sequence type in ON state.                                                   |
| Parameters            | enPsId: Power sequence ID                                                                     |
| Return Values         | void                                                                                          |
|                       |                                                                                               |
| **Prototype**         | **void Psm_vStopMonitoring(Psm_tenPsId enPsId)**                                              |
| Documentation         | Stop the monitoring specified by enPsId.<br>Do nothing if the monitoring is already stopped.<br>Only applicable to Power Rail Sequence type in ON state.                                                   |
| Parameters            | enPsId: Power sequence ID                                                                     |
| Return Values         | void                                                                                          |

@subsection export12 5.1.2 Variables, Types, Constants, Macros

|<span style="display: inline-block; width:500px">Variable name</span> | <span style="display: inline-block; width:700px">Description</span>|
|--------------------------|------------------------------------------------------------------------------|
| Psm__astPowerSequence | Represents the power sequences configurations. |

|<span style="display: inline-block; width:500px">Constant name</span> | <span style="display: inline-block; width:700px">Description</span>  |
|--------------------------|------------------------------------------------------------------------------|
| None | |

|<span style="display: inline-block; width:500px">Type name</span> | <span style="display: inline-block; width:700px">Description</span> |
|--------------------------|------------------------------------------------------------------------------|
| Psm_tenPsState        | -  Psm_nenIdle: State is Idle<br>-  Psm_nenOff: State is OFF<br>-  Psm_nenOn: State is ON<br>-  Psm_nenBusy: State is busy<br>-  Psm_nenFail: State is Fail  |

|<span style="display: inline-block; width:500px">Macro name</span>| <span style="display: inline-block; width:700px">Description</span> |
|--------------------------|------------------------------------------------------------------------------|
| None | |

@section export2 5.2 Dynamic View

@subsection export21 5.2.1 Simple Sequence API
The following sequence diagram shows the routine of simple sequence API
@startuml
hide footbox
autoactivate on
participant App
participant Psm_Api
participant Psm_Internal
participant Psm_Cfg
participant Gpt

activate App
App -> Psm_Api : Psm_boRunSequence(PS_ID)
    alt (enType == SIMPLE) && (enPubState != BUSY)
        Psm_Api -> Psm_Internal : Psm__vStartSeq(PS_ID, STARTUP)
            note right
                Refer to Psm__vStartSeq() sequence diagram
                Internally, Simple sequence type shares similar handling as Startup sequence
            end note
        return
        Psm_Api -> Psm_Internal : Psm__enMainStateM(PS_ID)
            note right
                Refer to Psm__enMainStateM() sequence diagram
            end note
        return enSeqReturn
    end alt
    alt (enSeqReturn == SEQUENCE_COMPLETE) || (enSeqReturn == SEQUENCE_ASYNC)
        Psm_Api -> Psm_Api : boApiReturn = TRUE
        deactivate
    else
        Psm_Api -> Psm_Api : boApiReturn = FALSE
        deactivate
    end alt
return boApiReturn
destroy App

activate Gpt
... Gpt timer expires ...

Gpt -> Psm_Cfg : Psm_vCallback(PS_ID)
    Psm_Cfg -> Psm_Internal : Psm__enMainStateM(PS_ID)
        |||
        note left
            Refer to Psm__enMainStateM() sequence diagram
        end note
    return enSeqReturn
return
destroy Gpt
@enduml

@subsection export22 5.2.2 Power Rail Startup API
The following sequence diagram shows the routine of Power Rail Startup API
@startuml
hide footbox
autoactivate on
participant App
participant Psm_Api
participant Psm_Internal
participant Psm_Cfg
participant Gpt
participant Icu

activate App
App -> Psm_Api : Psm_boStartup(PS_ID)
    alt (enType == RAIL) && (enPubState == OFF)
        Psm_Api -> Psm_Internal : Psm__vStartSeq(PS_ID, STARTUP)
            note right
                Refer to Psm__vStartSeq() sequence diagram
            end note
        return
        Psm_Api -> Psm_Internal : Psm__enMainStateM(PS_ID)
            note right
                Refer to Psm__enMainStateM() sequence diagram
            end note
        return enSeqReturn
    end alt
    alt (enSeqReturn == SEQUENCE_COMPLETE) || (enSeqReturn == SEQUENCE_ASYNC)
        Psm_Api -> Psm_Api : boApiReturn = TRUE
        deactivate
    else
        Psm_Api -> Psm_Api : boApiReturn = FALSE
        deactivate
    end alt
return boApiReturn
destroy App

activate Gpt
... Gpt timer expires ...

Gpt -> Psm_Cfg : Psm_vCallback(PS_ID)
    Psm_Cfg -> Psm_Internal : Psm__enMainStateM(PS_ID)
        |||
        note left
            Refer to Psm__enMainStateM() sequence diagram
        end note
    return enSeqReturn
return
destroy Gpt

activate Icu
... Icu triggers (only valid for interrupt-based monitoring) ...

Icu -> Psm_Cfg : Psm_vCallback(PS_ID)
    Psm_Cfg -> Psm_Internal : Psm__enMainStateM(PS_ID)
        |||
        note left
            Refer to Psm__enMainStateM() sequence diagram
        end note
    return enSeqReturn
return
destroy Icu
@enduml

@subsection export23 5.2.3 Power Rail Shutdown API
The following sequence diagram shows the routine of Power Rail Shutdown API
@startuml
hide footbox
autoactivate on
participant App
participant Psm_Api
participant Psm_Internal
participant Psm_Cfg
participant Gpt

activate App
App -> Psm_Api : Psm_boShutdown(PS_ID)
    alt (enType == RAIL) && ((enPubState == ON) || (enPubState == FAIL))
        Psm_Api -> Psm_Internal : Psm__vStartSeq(PS_ID, SHUTDOWN)
            note right
                Refer to Psm__vStartSeq() sequence diagram
            end note
        return
        Psm_Api -> Psm_Internal : Psm__enMainStateM(PS_ID)
            note right
                Refer to Psm__enMainStateM() sequence diagram
            end note
        return enSeqReturn
    end alt
    alt (enSeqReturn == SEQUENCE_COMPLETE) || (enSeqReturn == SEQUENCE_ASYNC)
        Psm_Api -> Psm_Api : boApiReturn = TRUE
        deactivate
    else
        Psm_Api -> Psm_Api : boApiReturn = FALSE
        deactivate
    end alt
return boApiReturn
destroy App

activate Gpt
... Gpt timer expires ...

Gpt -> Psm_Cfg : Psm_vCallback(PS_ID)
    Psm_Cfg -> Psm_Internal : Psm__enMainStateM(PS_ID)
        |||
        note left
            Refer to Psm__enMainStateM() sequence diagram
        end note
    return enSeqReturn
return
destroy Gpt
@enduml

@subsection export24 5.2.4 Power Rail Resume API
The following sequence diagram shows the routine of Power Rail Resume API
@startuml
hide footbox
autoactivate on
participant App
participant Psm_Api
participant Psm_Internal
participant Psm_Cfg
participant Gpt
participant Icu

activate App
App -> Psm_Api : Psm_boResume(PS_ID)
    alt (enType == RAIL) && (enPubState == FAIL)
        Psm_Api -> Psm_Internal : Psm__enMainStateM(PS_ID)
            note right
                Refer to Psm__enMainStateM() sequence diagram
            end note
        return enSeqReturn
    end alt
    alt (enSeqReturn == SEQUENCE_COMPLETE) || (enSeqReturn == SEQUENCE_ASYNC)
        Psm_Api -> Psm_Api : boApiReturn = TRUE
        deactivate
    else
        Psm_Api -> Psm_Api : boApiReturn = FALSE
        deactivate
    end alt
return boApiReturn
destroy App

activate Gpt
... Gpt timer expires ...

Gpt -> Psm_Cfg : Psm_vCallback(PS_ID)
    Psm_Cfg -> Psm_Internal : Psm__enMainStateM(PS_ID)
        |||
        note left
            Refer to Psm__enMainStateM() sequence diagram
        end note
    return enSeqReturn
return
destroy Gpt

activate Icu
... Icu triggers (only valid for interrupt-based monitoring) ...

Icu -> Psm_Cfg : Psm_vCallback(PS_ID)
    Psm_Cfg -> Psm_Internal : Psm__enMainStateM(PS_ID)
        |||
        note left
            Refer to Psm__enMainStateM() sequence diagram
        end note
    return enSeqReturn
return
destroy Icu
@enduml

@subsection export25 5.2.5 Get State API
The following sequence diagram shows the routine of Power Get State API
@startuml
hide footbox
autoactivate on
participant App
participant Psm_Api
participant Psm_Internal
participant Psm_Cfg

activate App
App -> Psm_Api : Psm_enGetState(PS_ID)
    Psm_Api -> Psm_Cfg : Get enPubState of PS_ID
    return
    alt enType == SIMPLE
        alt enPubState != BUSY
            Psm_Api -> Psm_Api : enApiReturn = IDLE
            deactivate
        else
            Psm_Api -> Psm_Api : enApiReturn = BUSY
            deactivate
        end alt
    else
        Psm_Api -> Psm_Api : enApiReturn = enPubState
        deactivate
    end alt
return enApiReturn
@enduml
*/
