/*---------------------------------------------------------
Template file for Doxygen based Module Specification

CoDeQ.2018 ADG - Automated Document Generation
---------------------------------------------------------*/
/**
@page chapter6 Chapter 6: Integration
@short Description of Configuration

@tableofcontents

@section integration1 6.1 Runtime Contexts

| Function name           | Task Context                                                           |
|-------------------------|------------------------------------------------------------------------|
| None | |

@section integration2 6.2 Configuration Switches
@subsection integration21 6.2.1 Configuration Switches for external information
| Configuration Switch          | Value                                        | Description                                             |
|-------------------------------|----------------------------------------------|---------------------------------------------------------|
| None | | |

@subsection integration22 6.2.2 Configuration Switches for internal information
| Configuration Switch          | Value                                                       | Description                                    |
|-------------------------------|-------------------------------------------------------------|------------------------------------------------|
| None | | |

@section integration3 6.3 Integration
@subsection integration31 6.3.1 Integration steps for PSM

| <span style="display: inline-block; width:1500px">Steps</span> |
|---------------------------------------------------------------|
|1) Open tresos, EcuM and configuration tab. |
|2) Select the EcuM configuration to edit.<br>@image{inline} html img/6_3_1_1.png "" |
|3) Select init list to edit. |
|4) Add item PSM_init to the module initialisation list.<br>@image{inline} html img/6_3_1_2.png ""|
|5) Link the item to the module header file, specify the module service and module config string (depends on project configuration).<br>@image{inline} html img/6_3_1_3.png ""|
|6) Open PSM and configure it based on the PCB requirement. An example is shown below.<br>@image{inline} html img/6_3_2_1.png ""<br>@image{inline} html img/6_3_2_2.png ""<br>@image{inline} html img/6_3_2_3.png ""<br>@image{inline} html img/6_3_2_4.png ""|

@subsection integration32 6.3.2 DIO Configuration

| <span style="display: inline-block; width:1500px">Steps</span> |
|---------------------------------------------------------------|
|1) Open Dio and go to General.<br>@image{inline} html img/6_3_3_1.png "" |
|2) Go to Dio configuration and open to edit.<br>@image{inline} html img/6_3_3_2.png "" |
|3) Open Dio Pin 6.<br>@image{inline} html img/6_3_3_3.png "" |
|4) Select the configurations for it.<br>@image{inline} html img/6_3_3_4.png "" |

@subsection integration33 6.3.3 GPT Configuration

| <span style="display: inline-block; width:1500px">Steps</span> |
|---------------------------------------------------------------|
|1) Open GPT and go to GptChannelConfigSet. |
|2) Select the Gpt channel config set to edit.<br>@image{inline} html img/6_3_4_1.png "" |
|3) Open GptChannelConfiguration and add Gpt channels for each power supply instance.<br>@image{inline} html img/6_3_4_2.png "" |
|4) Add the Gpt channel as follows.<br>@image{inline} html img/6_3_4_3.png "" |

@subsection integration34 6.3.4 ICU Configuration

| <span style="display: inline-block; width:1500px">Steps</span> |
|---------------------------------------------------------------|
|1) Open Icu and go to IcuConfigSet.<br>@image{inline} html img/6_3_5_1.png ""  |
|2) Select the Icu configuration set to edit.<br>@image{inline} html img/6_3_5_2.png "" |
|3) Open IcuChannel and add Icu Channels for each power good signal to monitor.|
|4) Add the Icu channel as follows. IcuResource should be set to the power good signal pin.<br>@image{inline} html img/6_3_5_3.png "" |

@subsection integration35 6.3.5 Error Callout Handling

Whenever any error occurs within the module, this  particular error callout will be invoked with the function prototype.
PSM_ErrorCalloutHandler(ModuleID,InstanceID,APICode,ErrorCode)
For more details, refer chapter 7.1.2 Macros section.


*/
