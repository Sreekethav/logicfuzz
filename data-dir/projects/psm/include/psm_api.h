/***************************************************************************
*======= Copyright (c) [Year 2023] Continental AG and subsidiaries =========
****************************************************************************
* Copyrights should use the oldest year for the file and should not be updated each year.
*
* Title        : psm_api.h
*
* Description  : Header file of export interfaces of PSM module
*
* Environment  : TV2-C
*
* Responsible  : Yu Han Ng, VNI CE SW AS SGP SWP4
*
* Guidelines   : SMK 4.28
*
* Template name: SWMODxC1.H, Revision 1.9
*
*
***************************************************************************/

/**
* Check if information is already included
*/
#ifndef PSM_API_H
#define PSM_API_H


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

/***************************************************************************
* Interface Description: Executes the specified power sequence (PS). This function
*                        will return when the PS is completed or when it reaches
*                        an asynchronous action or when an error is encountered.
*                        NOTE: This function only works on Simple Type PS that is in
*                        idle state.
*
* Implementation       : Runs the state machine.
*
* Return Value         : TRUE  - The sequence has been completed or is executing
*                                an asynchronous action
*                        FALSE - Pre-condition not fulfilled or error occurred
*                                during the power sequence
* 
* Author               : Yu Han Ng
*
****************************************************************************/
FUNC(boolean , PSM_CODE) Psm_boRunSequence(
    CONST(Psm_tenPsId, AUTOMATIC) enPsId /*
    *
    *  Description: The ID of the power sequence to be turned on.
    *
    *  Direction  : in
    *
    *  Values     : 0 to (Psm_nenPsCount - 1)
    */
);

/***************************************************************************
* Interface Description: Executes the startup sequence of the specified power
*                        sequence (PS) to turn it on. This function will return
*                        when the startup sequence is completed or when it reaches
*                        an asynchronous action or when an error is encountered.
*                        If monitoring is enabled for this PS, it will be
*                        activated at the end of sequence.
*                        NOTE: This function only works on Power Rail Type PS
*                        that is in off state.
*
* Implementation       : Runs the state machine.
*
* Return Value         : TRUE  - The sequence has been completed or is executing
*                                an asynchronous action
*                        FALSE - Pre-condition not fulfilled or error occurred
*                                during the power sequence
* 
* Author               : Yu Han Ng
*
****************************************************************************/
FUNC(boolean , PSM_CODE) Psm_boStartup(
    CONST(Psm_tenPsId, AUTOMATIC) enPsId /*
    *
    *  Description: The ID of the power sequence to be turned on.
    *
    *  Direction  : in
    *
    *  Values     : 0 to (Psm_nenPsCount - 1)
    */
);

/***************************************************************************
* Interface Description: Executes the shutdown sequence of the specified power
*                        sequence (PS) to turn it off. This function will return
*                        when the shutdown sequence is completed or when it reaches
*                        an asynchronous action or when an error is encountered.
*                        NOTE: This function only works on Power Rail Type PS
*                        that is in on or fail state.
*                        If the PS is in on state, normal shutdown sequence will be
*                        executed. If the PS is in fail state, forced shutdown
*                        sequence will be executed.
*
* Implementation       : Runs the state machine.
*
* Return Value         : TRUE  - The sequence has been completed or is executing
*                                an asynchronous action
*                        FALSE - Pre-condition not fulfilled or error occurred
*                                during the power sequence
* 
* Author               : Yu Han Ng
*
****************************************************************************/
FUNC(boolean, PSM_CODE) Psm_boShutdown(
    CONST(Psm_tenPsId, AUTOMATIC) enPsId /*
    *
    *  Description: The ID of the power sequence to be turned off.
    *
    *  Direction  : in
    *
    *  Values     : 0 to (Psm_nenPsCount - 1)
    */
);

/***************************************************************************
* Interface Description: Continue executing the specified power sequence (PS)
*                        which encountered error. This function will return
*                        when the shutdown sequence is completed or when it reaches
*                        an asynchronous action or when an error is encountered.
*                        NOTE: This function only works on Power Rail Type PS
*                        that is in fail state.
*
* Implementation       : Runs the state machine.
*
* Return Value         : TRUE  - The sequence has been completed or is executing
*                                an asynchronous action
*                        FALSE - Pre-condition not fulfilled or error occurred
*                                during the power sequence
* 
* Author               : Yu Han Ng
*
****************************************************************************/
FUNC(boolean , PSM_CODE) Psm_boResume(
    CONST(Psm_tenPsId, AUTOMATIC) enPsId /*
    *
    *  Description: The ID of the power sequence to be resumed executing.
    *
    *  Direction  : in
    *
    *  Values     : 0 to (Psm_nenPsCount - 1)
    */
);

/***************************************************************************
* Interface Description: Returns the state of the power sequence (PS) specified.
*
* Implementation       : Reads from state machine's variable.
*
* Return Value         : One of Psm_tenPsState enum.
* 
* Author               : Yu Han Ng
*
****************************************************************************/
FUNC(Psm_tenPsState, PSM_CODE) Psm_enGetState(
    CONST(Psm_tenPsId, AUTOMATIC) enPsId /*
    *
    *  Description: The ID of the power sequence to be checked.
    *
    *  Direction  : in
    *
    *  Values     : 0 to (Psm_nenPsCount - 1)
    */
);

/***************************************************************************
* Interface Description: Starts the monitoring of the specified power sequence.
*
* Implementation       : Starts the corresponding mechanism to start monitoring
*                        the configured DIO pin.
*                        NOTE: This function only works on Power Rail Type PS
*                        that is in on state.
*
* Return Value         : void
* 
* Author               : Yu Han Ng
*
****************************************************************************/
FUNC(void, PSM_CODE) Psm_vStartMonitoring(
    CONST(Psm_tenPsId, AUTOMATIC) enPsId /*
    *
    *  Description: The ID of the power sequence to be started monitoring.
    *
    *  Direction  : in
    *
    *  Values     : 0 to (Psm_nenPsCount - 1)
    */
);

/***************************************************************************
* Interface Description: Stops the monitoring of the specified power sequence.
*
* Implementation       : Stops the corresponding mechanism to stop monitoring
*                        the configured DIO pin.
*                        NOTE: This function only works on Power Rail Type PS
*                        that is in on state.
*
* Return Value         : void
* 
* Author               : Yu Han Ng
*
****************************************************************************/
FUNC(void, PSM_CODE) Psm_vStopMonitoring(
    CONST(Psm_tenPsId, AUTOMATIC) enPsId /*
    *
    *  Description: The ID of the power sequence to be stopped monitoring.
    *
    *  Direction  : in
    *
    *  Values     : 0 to (Psm_nenPsCount - 1)
    */
);

void Psm_MainFunction_PowerSequence(void);

/*
* End of Check if information is already included
*/
#endif                                  /* ifndef PSM_API_H */

/***************************************************************************
* EOF: psm_api.h
****************************************************************************/
