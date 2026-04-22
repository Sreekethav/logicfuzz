#ifndef stub_H
#define stub_H

#ifdef PSMSTUB
#include "cdef.h"

/*
 ****************************
 * {@Platform_Types.h} START*
 ****************************
*/
#define boolean           bool
#define TRUE              1
#define FALSE             0

/*
 ****************************
 *   {@Prj_Patch.h} START   *
 ****************************
*/
#define PSM_STATIC        

#define Psm_nenPsId_0     1
#define Psm_nenPsId_1     2
#define Psm_nenPsId_2     3

/*
 ****************************
 * {@Compiler_Cfg.h} START  *
 ****************************
*/
#define PSM_CODE
#define REGSPACE
#define DIO_CODE
#define RTE_CODE

/*
 ****************************
 *   {@Std_Types.h} START   *
 ****************************
*/
#define STD_LOW           0U
#define STD_HIGH          1U

/*
 ****************************
 *   {@Compiler.h} START    *
 ****************************
*/
#define AUTOMATIC
#define NULL_PTR          NULL
#define GTEST


#define FUNC(rettype, memclass) memclass rettype
#define FUNC_P2CONST(rettype, ptrclass, memclass) const ptrclass rettype * memclass
#define FUNC_P2VAR(rettype, ptrclass, memclass) ptrclass rettype * memclass
#define P2VAR(ptrtype, memclass, ptrclass) ptrclass ptrtype * memclass
#define P2CONST(ptrtype, memclass, ptrclass) const ptrtype ptrclass * memclass
#define CONSTP2VAR(ptrtype, memclass, ptrclass) ptrclass ptrtype * const memclass
#define CONSTP2CONST(ptrtype, memclass, ptrclass) const ptrclass ptrtype * const memclass
#define CONSTP2FUNC(rettype, ptrclass, fctname) rettype ptrclass (* const fctname)
#define P2FUNC(rettype, ptrclass, fctname) rettype ptrclass (* fctname)
#define CONST(consttype, memclass) memclass const consttype
#define VAR(vartype, memclass) memclass vartype

#define PSM_INVALID_LL_CHANNEL      0xFFFF
#define sint8                       int8
#define uint8                       uint8
#define PSM_MaxNoOfPowerSequences   Psm_nenPsCount
#define PSM_GPT_CHANNEL             8u
#define PSM_ICU_CHANNEL             3u
#define PSM_DIO_CHANNEL             8u

#define PSM_ERROR_CALLOUT_FUNCTION  PSM_ErrorCalloutHandler
/*
 ****************************
 *  {@Dio_types.h} START   *
 ****************************
*/
typedef uint32 Dio_ChannelType;
typedef uint8 Dio_LevelType;

#define Dio_Channel_1 0
#define Dio_Channel_2 1

/*
 ****************************
 *  {@Gpt_Public.h} START   *
 ****************************
*/
typedef uint16 Gpt_ChannelType;
typedef uint32 Gpt_ValueType;

/*
 ****************************
 *   {@Icu_Types.h} START   *
 ****************************
*/
typedef uint16 Icu_ChannelType;

/*
 ****************************
 *   {@Psm_types.h} START   *
 ****************************
*/
enum Psm_enPsId
{
    Psm_nenPsIdPsmPowerSequence_0,
    Psm_nenPsIdPsmPowerSequence_1,
    Psm_nenPsIdPsmPowerSequence_2,
    Psm_nenPsIdPsmPowerSequence_3,
    Psm_nenPsIdPsmPowerSequence_4,
    Psm_nenPsIdPsmPowerSequence_5,
    Psm_nenPsIdPsmPowerSequence_6,
    Psm_nenPsIdPsmPowerSequence_7,
    Psm_nenPsIdPsmPowerSequence_8,
    Psm_nenPsIdPsmPowerSequence_9,
    Psm_nenPsIdPsmPowerSequence_10,
    Psm_nenPsIdPsmPowerSequence_11,
    Psm_nenPsIdPsmPowerSequence_12,
    Psm_nenPsIdPsmPowerSequence_13,
    Psm_nenPsIdPsmPowerSequence_14,
    Psm_nenPsIdPsmPowerSequence_15,
    Psm_nenPsCount
};
typedef enum Psm_enPsId Psm_tenPsId;

enum Psm_enPsState
{
    Psm_nenIdle,
    Psm_nenOff,
    Psm_nenOn,
    Psm_nenBusy,
    Psm_nenFail
};
typedef enum Psm_enPsState Psm_tenPsState;

enum Psm_enSequence
{
    Psm_nenNoSequence,
    Psm_nenStartup,
    Psm_nenShutdown
};
typedef enum Psm_enSequence Psm_tenSequence;

enum Psm_enSeqReturn
{
    Psm_nenSeqComplete,
    Psm_nenSeqAsync,
    Psm_nenSeqAbort,
    Psm_nenSeqMonitorFail
};
typedef enum Psm_enSeqReturn Psm_tenSeqReturn;

enum Psm_enFuncReturn
{
    Psm_nenFuncOk,
    Psm_nenFuncError,
    Psm_nenFuncWait
};

typedef enum
{
    PORT_PIN_IN  = 0,             /* Sets port pin as input.  */
    PORT_PIN_OUT = 1,             /* Sets port pin as output. */
    PORT_PIN_IN_OUT_DISABLED = 2  /* Sets port pin as input/output disabled. */
} Port_PinDirectionType;

typedef enum Psm_enFuncReturn Psm_tenFuncReturn;

typedef void(*Psm_tvfpNotifyFunc)(Psm_tenSequence, Psm_tenSeqReturn, uint8);

typedef Psm_tenFuncReturn(*Psm_tenfpUserFunc)(void);

typedef void(*Psm_tvfpVoidUserFunc)(void);

void Dummy_vfpForceShutdownFunc(void);
void Dummy_vfpNotifyFunc(Psm_tenSequence enseq, Psm_tenSeqReturn seqreturn, uint8 i);

void Psm_vCallbackPsmPowerSequence_0(Psm_tenPsId enPsId);
void Psm_vCallbackPsmPowerSequence_1(Psm_tenPsId enPsId);

void SchM_ActMainFunction_Psm_IrPsmId_PowerSequence(void);

Psm_tenFuncReturn Psm_vDelay_Ok(void);

Psm_tenFuncReturn Psm_vDelay_Wait(void);

Psm_tenFuncReturn Psm_vDelay_Error(void);

void Psm_voidFunc(void);

void PSM_ErrorCalloutHandler(uint8 ModuleID,uint8 InstanceID,uint8 APICode,uint8 ErrorCode);

extern FUNC(void, RTE_CODE) SchM_Enter_Psm_INTERRUPT_CONTROL_PROTECTION_AREA( void);
extern FUNC(void, RTE_CODE) SchM_Exit_Psm_INTERRUPT_CONTROL_PROTECTION_AREA(void);

#define HIGHZ_INBUFF_ON 2U
#define HIGHZ_INBUFF_OFF 3U
typedef uint16 Port_PinType;

void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction);
void Psm_MainFunction_PowerSequence(void);

#endif //#ifndef PSMSTUB
#endif //#ifndef stub_H
