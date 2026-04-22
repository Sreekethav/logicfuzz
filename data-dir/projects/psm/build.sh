#!/bin/bash -eu

cd $SRC/psm-project

# Common flags: enable the PSMSTUB macro (via global_overrides.h) and set include paths
COMMON_FLAGS="-DPSMSTUB -include stub/global_overrides.h -Istub -Iinclude"

# ---- Compile the PSM library source files ----
$CC $CFLAGS $COMMON_FLAGS -c src/psm_api.c -o psm_api.o
$CC $CFLAGS $COMMON_FLAGS -c src/psm_internal.c -o psm_internal.o
$CC $CFLAGS $COMMON_FLAGS -c src/psm_abstract.c -o psm_abstract.o

# ---- Compile the existing stub.c (provides dummy callbacks, config helpers) ----
$CC $CFLAGS $COMMON_FLAGS -c stub/stub.c -o stub_funcs.o

# ---- Create HAL stub implementations + test configuration ----
cat > $SRC/hal_stubs.c << 'HALEOF'
#include "Std_Types.h"
#include "Dio.h"
#include "Gpt.h"
#include "Icu.h"
#include "Port.h"
#include "SchM_Psm.h"
#include "psm_internal.h"
#include "psm_api.h"

/* ---- DIO stubs ---- */
static Dio_LevelType dio_levels[64];

Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId) {
    if (ChannelId < 64) return dio_levels[ChannelId];
    return STD_LOW;
}

void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level) {
    if (ChannelId < 64) dio_levels[ChannelId] = Level;
}

/* ---- GPT stubs ---- */
Gpt_ValueType Gpt_GetTimeElapsed(Gpt_ChannelType Channel) { return 0; }
Gpt_ValueType Gpt_GetTimeRemaining(Gpt_ChannelType Channel) { return 0; }
void Gpt_StartTimer(Gpt_ChannelType Channel, Gpt_ValueType Value) { (void)Channel; (void)Value; }
void Gpt_StopTimer(Gpt_ChannelType Channel) { (void)Channel; }
void Gpt_EnableNotification(Gpt_ChannelType Channel) { (void)Channel; }
void Gpt_DisableNotification(Gpt_ChannelType Channel) { (void)Channel; }

/* ---- ICU stubs ---- */
void Icu_DisableEdgeDetection(Icu_ChannelType Channel) { (void)Channel; }
void Icu_EnableEdgeDetection(Icu_ChannelType Channel) { (void)Channel; }
void Icu_DisableNotification(Icu_ChannelType Channel) { (void)Channel; }
void Icu_EnableNotification(Icu_ChannelType Channel) { (void)Channel; }

/* ---- Port stubs ---- */
void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction) {
    (void)Pin; (void)Direction;
}

/* ---- SchM stubs ---- */
void SchM_Enter_Psm_INTERRUPT_CONTROL_PROTECTION_AREA(void) { }
void SchM_Exit_Psm_INTERRUPT_CONTROL_PROTECTION_AREA(void) { }
void SchM_ActMainFunction_Psm_IrPsmId_PowerSequence(void) {
    Psm_MainFunction_PowerSequence();
}

/* ---- Error callout ---- */
void PSM_ErrorCalloutHandler(uint8 ModuleID, uint8 InstanceID, uint8 APICode, uint8 ErrorCode) {
    (void)ModuleID; (void)InstanceID; (void)APICode; (void)ErrorCode;
}

/* ---- UART stub ---- */
void UART_Printf_direct(void *fmt, ...) { (void)fmt; }

/* ---- Minimal power sequence configuration ---- */
static Psm__tstDioState startup_dio_0[] = { { 0, STD_HIGH } };
static Psm__tstDioState shutdown_dio_0[] = { { 0, STD_LOW } };
static Psm__tstDioState force_shutdown_dio_0[] = { { 0, STD_LOW } };

static Psm__tstActionDriveIo drive_startup_0 = { startup_dio_0, 1 };
static Psm__tstActionDriveIo drive_shutdown_0 = { shutdown_dio_0, 1 };

static Psm__tstAction startup_actions_0[] = {
    { Psm__nenDriveDio, &drive_startup_0 }
};
static Psm__tstAction shutdown_actions_0[] = {
    { Psm__nenDriveDio, &drive_shutdown_0 }
};

static Psm__tstActionDriveIo drive_simple = { startup_dio_0, 1 };
static Psm__tstAction simple_actions[] = {
    { Psm__nenDriveDio, &drive_simple }
};

static Psm__tstPsConfig ps_config[] = {
    /* [0] Simple PS */
    {
        .pastStartupAction = simple_actions,
        .pastShutdownAction = NULL,
        .pastForceShutdownDio = NULL,
        .vfpForceShutdownFunc = NULL,
        .vfpNotifyFunc = Dummy_vfpNotifyFunc,
        .stMonitorDioState = { 0, STD_HIGH },
        .u32MonitorIntervalUs = 0,
        .u16GptChannel = 0,
        .u16IcuChannel = 0,
        .enType = Psm__nenSimplePs,
        .enMonitorType = Psm__nenNoMonitor,
        .u8NumOfForceShutdownDio = 0,
        .u8NumOfStartupAction = 1,
        .u8NumOfShutdownAction = 0
    },
    /* [1] Power Rail PS */
    {
        .pastStartupAction = startup_actions_0,
        .pastShutdownAction = shutdown_actions_0,
        .pastForceShutdownDio = force_shutdown_dio_0,
        .vfpForceShutdownFunc = Dummy_vfpForceShutdownFunc,
        .vfpNotifyFunc = Dummy_vfpNotifyFunc,
        .stMonitorDioState = { 0, STD_HIGH },
        .u32MonitorIntervalUs = 1000,
        .u16GptChannel = 0,
        .u16IcuChannel = 0,
        .enType = Psm__nenPowerRailPs,
        .enMonitorType = Psm__nenPolling,
        .u8NumOfForceShutdownDio = 1,
        .u8NumOfStartupAction = 1,
        .u8NumOfShutdownAction = 1
    }
};

Psm__tstPowerSequence Psm__astPowerSequence[Psm_nenPsCount] = {
    { .pstConfig = &ps_config[0], .enState = Psm_nenIdle, .enPubState = Psm_nenIdle, .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
    { .pstConfig = &ps_config[1], .enState = Psm_nenOff,  .enPubState = Psm_nenOff,  .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
    { .pstConfig = &ps_config[0], .enState = Psm_nenIdle, .enPubState = Psm_nenIdle, .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
    { .pstConfig = &ps_config[0], .enState = Psm_nenIdle, .enPubState = Psm_nenIdle, .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
    { .pstConfig = &ps_config[0], .enState = Psm_nenIdle, .enPubState = Psm_nenIdle, .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
    { .pstConfig = &ps_config[0], .enState = Psm_nenIdle, .enPubState = Psm_nenIdle, .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
    { .pstConfig = &ps_config[0], .enState = Psm_nenIdle, .enPubState = Psm_nenIdle, .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
    { .pstConfig = &ps_config[0], .enState = Psm_nenIdle, .enPubState = Psm_nenIdle, .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
    { .pstConfig = &ps_config[0], .enState = Psm_nenIdle, .enPubState = Psm_nenIdle, .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
    { .pstConfig = &ps_config[0], .enState = Psm_nenIdle, .enPubState = Psm_nenIdle, .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
    { .pstConfig = &ps_config[0], .enState = Psm_nenIdle, .enPubState = Psm_nenIdle, .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
    { .pstConfig = &ps_config[0], .enState = Psm_nenIdle, .enPubState = Psm_nenIdle, .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
    { .pstConfig = &ps_config[0], .enState = Psm_nenIdle, .enPubState = Psm_nenIdle, .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
    { .pstConfig = &ps_config[0], .enState = Psm_nenIdle, .enPubState = Psm_nenIdle, .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
    { .pstConfig = &ps_config[0], .enState = Psm_nenIdle, .enPubState = Psm_nenIdle, .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
    { .pstConfig = &ps_config[0], .enState = Psm_nenIdle, .enPubState = Psm_nenIdle, .enSequence = Psm_nenNoSequence, .u32DelayCycle = 0, .boDoNotify = FALSE, .boDoForceShutdown = FALSE, .u8SeqStepId = 0, .u8SeqSubStepId = 0 },
};
HALEOF

$CC $CFLAGS $COMMON_FLAGS -c $SRC/hal_stubs.c -o hal_stubs.o

# ---- Build the static library ----
ar rcs libpsm.a psm_api.o psm_internal.o psm_abstract.o stub_funcs.o hal_stubs.o

# ---- Compile and link the fuzzer ----
# The LLM-generated fuzz target is placed at fuzzer.c by the LogicFuzz pipeline.
# LLMs almost always emit C++ (FuzzedDataProvider, extern "C", etc.) even when
# the project is C, so we:
#   1. Strip markdown code fences the LLM may wrap the code in.
#   2. Rewrite ConsumeIntegral<EnumType>() to cast via int (C++ enum != integral).
#   3. Rename to .cpp so the compiler picks the right language mode.
#   4. Compile with $CXX/$CXXFLAGS and our PSM include paths + psm_cpp_compat.h
#      which wraps all C headers in extern "C".
sed -i '/^```/d' $SRC/psm-project/fuzzer.c
# FuzzedDataProvider::ConsumeIntegral requires std::is_integral so enum types fail.
# Rewrite  obj.ConsumeIntegral<Psm_tenPsId>()  ->  static_cast<Psm_tenPsId>(obj.ConsumeIntegral<int>())
sed -i -E 's/([a-zA-Z_][a-zA-Z0-9_]*(\.|->))ConsumeIntegral<(Psm_[a-zA-Z_]+)>\s*\(\)/static_cast<\3>(\1ConsumeIntegral<int>())/g' $SRC/psm-project/fuzzer.c
cp $SRC/psm-project/fuzzer.c $WORK/fuzzer.cpp

$CXX $CXXFLAGS $COMMON_FLAGS -include stub/psm_cpp_compat.h \
    -c $WORK/fuzzer.cpp -o $WORK/fuzzer.o
$CXX $CXXFLAGS $LIB_FUZZING_ENGINE $WORK/fuzzer.o \
    $SRC/psm-project/libpsm.a -o $OUT/psm_project_fuzzer

# ---- Copy source files for coverage and FI source-code retrieval ----
mkdir -p $OUT/src $OUT/include $OUT/stub
cp $SRC/psm-project/src/*.c $OUT/src/
cp $SRC/psm-project/include/*.h $OUT/include/
cp $SRC/psm-project/stub/*.h $OUT/stub/
cp $SRC/psm-project/stub/*.c $OUT/stub/
