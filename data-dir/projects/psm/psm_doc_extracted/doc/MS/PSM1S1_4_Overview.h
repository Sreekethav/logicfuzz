/*---------------------------------------------------------
Template file for Doxygen based Module Specification

CoDeQ.2018 ADG - Automated Document Generation
---------------------------------------------------------*/
/**
@page chapter4 Chapter 4: Overview
@short Introduction into the package



@tableofcontents

@section overview1 4.1 Module Description
PSM components supports a collection of one or more power sequences.

Each power sequence (can be of simple type or power rail type) consists of one or more collection of actions.
Simple type supports only one action collection (or action list).
Power rail type supports two action collections (or actions lists), one for the startup sequence, and another for the shutdown sequence.
See more details under chapter "Types of Sequence Supported" below.

In each action collection (or action list), there can be one or more actions.
The supported action types are:

- Drive IO
- Set IO to Hi-Z mode
- Poll IO
- Synchronous Delay
- Asynchronous Delay
- Function Call (void return type)
- Function Call (custom return type): supports error handling and wait handling
- Trigger Another Power Sequence
- Wait For Another Power Sequence

@startuml
hide empty description

state "Collection of Power Sequence" as Collection #palegreen {
    state "Power Sequence 1 (Simple)" as Sequence1 #lightcyan {
        state "Collection of Action" as ActionCollection1 #palegreen {
            state "Action 1" as Action1 #lightpink
            state "Action 2" as Action2 #lightpink
            state "Action N" as Action3 #lightpink
        }
    }

    state "Power Sequence 2 (Power Rail)" as Sequence2 #lightcyan {
        state "Collection of Startup Action" as ActionCollection2 #palegreen {
            state "Action 1" as Action4 #lightpink
            state "Action 2" as Action5 #lightpink
            state "Action N" as Action6 #lightpink
        }
        state "Collection of Shutdown Action" as ActionCollection3 #palegreen {
            state "Action 1" as Action7 #lightpink
            state "Action 2" as Action8 #lightpink
            state "Action N" as Action9 #lightpink
        }
    }
}
@enduml

@startuml
hide empty description

object Psm
object "Power Rail / Power Supply" as PowerRail {
    a.k.a. "Power Rail Sequence"
}
object Sequence {
    a.k.a. "Simple Sequence"
}
object State
object Action

Psm -right-> PowerRail : manages
Psm -right-> Sequence : executes

PowerRail -down-> Sequence : contains 2 : "startup" & "shutdown"
PowerRail -right-> State : has

Sequence -right-> Action : contains 1 or more
@enduml

@subsection overview12 4.1.1 Input-Output Diagram
@startuml
[enPsId]..>[PSM]
[PSM] ..> [Psm_tenPsState]
[PSM] ..> [Psm_tenSeqReturn]
@enduml

<table class="center" style="margin-right: auto;margin-left: auto;border: 1px solid black; border-collapse: collapse;">
  <tr>
    <th style="width: 100px;border: 1px solid black;background-color:black;color:white;">Object</th>
    <th style="width: 900px;border: 1px solid black;background-color:black;color:white;">Description</th>
  </tr>
  <tr>
    <td style="border: 1px solid black;">enPsId</td>
    <td style="border: 1px solid black;">Power supply Id used in power supply specific functions such as PSM_boStartup()</td>
  </tr>
  <tr>
    <td style="border: 1px solid black;">Psm_tenPsState</td>
    <td style="border: 1px solid black;">
      - Psm_nenIdle: State is Idle
      - Psm_nenOff: State is OFF
      - Psm_nenOn: State is ON
      - Psm_nenBusy: State is Busy
      - Psm_nenFail: State is Fail
    </td>
  </tr>
  <tr>
    <td style="border: 1px solid black;">Psm_tenSeqReturn</td>
    <td style="border: 1px solid black;">
      - Psm_nenSeqComplete = Sequence is completed
      - Psm_nenSeqAsync = Sequence is asynchronous
      - Psm_nenSeqAbort = Sequence is aborted
      - Psm_nenSeqMonitorFail = Monitored signal is failed
    </td>
  </tr>
</table>

@subsection overview13 4.1.2 Module Interdependencies
@startuml
'Component diagram
[PSM] ..> [GPT]
[PSM] ..> [ICU]
[PSM] ..> [DIO]
[PSM] ..> [PORT]
@enduml
<table class="center" style="margin-right: auto;margin-left: auto;border: 1px solid black; border-collapse: collapse;">
   <tr>
    <th style="width: 100px;border: 1px solid black;background-color:black;color:white;">Node</th>
    <th style="width: 900px;border: 1px solid black;background-color:black;color:white;">Description</th>
  </tr>
  <tr>
    <td style="border: 1px solid black;">GPT</td>
    <td style="border: 1px solid black;">Implement synchronous and asynchronous delays and interrupts</td>
  </tr>
  <tr>
    <td style="border: 1px solid black;">ICU</td>
    <td style="border: 1px solid black;">Detect power failure and trigger interrupt</td>
  </tr>
    <tr>
    <td style="border: 1px solid black;">DIO</td>
    <td style="border: 1px solid black;">Drive the GPIOs</td>
  </tr>
  </tr>
    <tr>
    <td style="border: 1px solid black;">PORT</td>
    <td style="border: 1px solid black;">Driver module that provides the service for initializing the whole port structure of the microcontroller.</td>
  </tr>
</table>

@section overview4 4.2 Use Cases
@startuml
'Use case diagram
left to right direction
skinparam packageStyle rectangle
actor GPT
actor ICU
actor DIO
actor PORT
actor Application
rectangle PSM {
  Application -- (Get status of power supplies)
  (Start/Shutdown power supplies) ..> (Read, write and configure GPIOs) : include
  (Read, write and configure GPIOs) -- PORT
  (Read, write and configure GPIOs) -- DIO
  (Start/Shutdown power supplies) ..> (Synchronous and Asynchronous delays) : include
  (Synchronous and Asynchronous delays) -- GPT
  (Start/Shutdown power supplies) ..> (Call user function) : include
  Application -- (Start/Shutdown power supplies)
  Application -- (Start/Stop monitoring power supplies)
  (Start/Stop monitoring power supplies) -- ICU
  (Start/Stop monitoring power supplies) -- GPT
}
@enduml

@section overview15 4.3 Resource consumption

| Configuration | Code [bytes] | Data(.bss + .rodata) [bytes] | Max Stack [bytes]     | EEPROM [bytes] |
|---------------|--------------|------------------------------|-----------------------|----------------|
| -             | -            | -                            | -                     | -              |


| Function (measured without Scheduler calls)   | runtime (usec)] | Frequency [calls/sec]             |
|-----------------------------------------------|-----------------|-----------------------------------|
| Psm_enGetState                                | tbd             | Once per startup                  |
| Psm_boRunSequence                             | tbd             | Once per shutdown                 |
| Psm_boStartup                                 | tbd             | Once per PS instance per startup  |
| Psm_boShutdown                                | tbd             | Once per PS instance per shutdown |
| Psm_boResume                                  | tbd             | On demand                         |
| Psm_vStartMonitoring                          | tbd             | On demand                         |
| Psm_vStopMonitoring                           | tbd             | On demand                         |

Note: Runtimes are currently "tbd" as there are no modules to help analyse runtime yet.

*/
