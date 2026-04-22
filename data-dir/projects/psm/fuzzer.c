#include "cdef.h"
#include "gtest/gtest.h"

#include "mock.hpp"
#include "stub.h"

#include "psm_api.h"
#include "psm_internal.h"

#include <cstring>
    
using ::testing::NiceMock;

template<typename T>
T read_bytes(const uint8_t*& data, size_t& size) {
    T val = 0;
    if (size >= sizeof(T)) {
        std::memcpy(&val, data, sizeof(T));
        data += sizeof(T);
        size -= sizeof(T);
    } else if (size > 0) {
        // Partial data: fill low bytes, pad the rest with zeros
        std::memcpy(&val, data, size);
        data += size;
        size = 0;
    }
    return val;
}

boolean read_bool(const uint8_t*& data, size_t& size) {
    uint8_t byte = read_bytes<uint8_t>(data, size);
    return byte & 1;
}

Psm_tenPsId read_enPsId(const uint8_t*& data, size_t& size) {
    uint8_t val = read_bytes<uint8_t>(data, size);
    return static_cast<Psm_tenPsId>(val);
}

Psm_tenPsState read_enPsState(const uint8_t*& data, size_t& size) {
    uint8_t val = read_bytes<uint8_t>(data, size);
    return static_cast<Psm_tenPsState>(val);
}

Psm_tenSequence read_enSequence(const uint8_t*& data, size_t& size) {
    uint8_t val = read_bytes<uint8_t>(data, size);
    return static_cast<Psm_tenSequence>(val);
}

Psm__tenPsType read_enPsType(const uint8_t*& data, size_t& size) {
    uint8_t val = read_bytes<uint8_t>(data, size);
    return static_cast<Psm__tenPsType>(val);
}

Psm__tenMonitorType read_enMonitorType(const uint8_t*& data, size_t& size) {
    uint8_t val = read_bytes<uint8_t>(data, size);
    return static_cast<Psm__tenMonitorType>(val);
}

Psm__tenAction read_enAction(const uint8_t*& data, size_t& size) {
    uint8_t val = read_bytes<uint8_t>(data, size);
    return static_cast<Psm__tenAction>(val);
}

uint8_t read_DioLevel(const uint8_t*& data, size_t& size) {
    uint8_t val = read_bytes<uint8_t>(data, size);
    return val;
}

Psm__tstPowerSequence Psm__astPowerSequence[16];

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){

    NiceMock<MockSchM> mock_schm;
    NiceMock<MockGpt> mock_gpt;
    NiceMock<MockPsm> mock_psm;
    NiceMock<MockDio> mock_dio;
    NiceMock<MockIcu> mock_icu;

    uint8 Psm_u8PsCount = (read_bytes<uint8>(Data, Size) % 16) + 1; // Limit to max 16 PS for memory constraints
    for (int i = 0; i < Psm_u8PsCount; i++)
    {
        Psm__astPowerSequence[i].pstConfig = new Psm__tstPsConfig;
        Psm__astPowerSequence[i].enState = read_enPsState(Data, Size);
        Psm__astPowerSequence[i].enPubState = read_enPsState(Data, Size);
        Psm__astPowerSequence[i].enSequence = read_enSequence(Data, Size);
        Psm__astPowerSequence[i].u32DelayCycle = read_bytes<uint32>(Data, Size);
        Psm__astPowerSequence[i].boDoNotify = read_bool(Data, Size);
        Psm__astPowerSequence[i].boDoForceShutdown = read_bool(Data, Size);
        Psm__astPowerSequence[i].u8SeqStepId = read_bytes<uint8>(Data, Size);
        Psm__astPowerSequence[i].u8SeqSubStepId = read_bytes<uint8>(Data, Size);

        uint8 numStartupActions = read_bytes<uint8>(Data, Size) % 32;
        if (numStartupActions == 0)
        {
            Psm__astPowerSequence[i].pstConfig->pastStartupAction = NULL_PTR;
        }
        else
        {
            Psm__astPowerSequence[i].pstConfig->pastStartupAction = new Psm__tstAction[numStartupActions];
            for (int j = 0; j < numStartupActions; j++)
            {
                Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].enAction = read_enAction(Data, Size);
                switch (Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].enAction)
                {
                    case Psm__nenInvalidAction:
                    {
                        break;
                    }
                    case Psm__nenDriveDio:
                    {
                        Psm__tstActionDriveIo * fuzz__stActionDriveIo = new Psm__tstActionDriveIo;
                        uint8 u8numPastDioState = read_bytes<uint8>(Data, Size) % 32;
                        if (u8numPastDioState > 0 && read_bool(Data, Size) == TRUE)
                        {
                            fuzz__stActionDriveIo->pastDioState = new Psm__tstDioState[u8numPastDioState];
                            for (uint8 k = 0; k < u8numPastDioState; k++)
                            {
                                fuzz__stActionDriveIo->pastDioState[k].u16DioChannel = read_bytes<uint16>(Data, Size);
                                fuzz__stActionDriveIo->pastDioState[k].u8DioLevel = read_DioLevel(Data, Size);
                            }
                        }
                        else
                        {
                            fuzz__stActionDriveIo->pastDioState = NULL_PTR;
                        }
                        fuzz__stActionDriveIo->u8NumOfDio = u8numPastDioState;
                        Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].pvActionData = fuzz__stActionDriveIo;
                        break;
                    }
                    case Psm__nenPollDio:
                    {
                        Psm__tstActionPollIo * fuzz__stActionPollIo = new Psm__tstActionPollIo;
                        uint8 u8numPastDioState = read_bytes<uint8>(Data, Size) % 32;
                        if (u8numPastDioState > 0 && read_bool(Data, Size) == TRUE)
                        {
                            fuzz__stActionPollIo->pastDioState = new Psm__tstDioState[u8numPastDioState];
                            for (uint8 k = 0; k < u8numPastDioState; k++)
                            {
                                fuzz__stActionPollIo->pastDioState[k].u16DioChannel = read_bytes<uint16>(Data, Size);
                                fuzz__stActionPollIo->pastDioState[k].u8DioLevel = read_DioLevel(Data, Size);
                            }
                        }
                        else
                        {
                            fuzz__stActionPollIo->pastDioState = NULL_PTR;
                        }
                        fuzz__stActionPollIo->u32WaitIntervalUs = read_bytes<uint32>(Data, Size);
                        fuzz__stActionPollIo->u32WaitCycleLimit = read_bytes<uint32>(Data, Size) % 128;
                        fuzz__stActionPollIo->u8NumOfDio = u8numPastDioState;
                        fuzz__stActionPollIo->boBlockingWait = read_bool(Data, Size);
                        Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].pvActionData = fuzz__stActionPollIo;
                        break;
                    }
                    case Psm__nenSynchronousDelay:
                    case Psm__nenAsynchronousDelay:
                    {
                        Psm__tstActionDelay * fuzz__stActionDelay = new Psm__tstActionDelay;
                        fuzz__stActionDelay->u32DelayIntervalUs = read_bytes<uint32>(Data, Size);
                        fuzz__stActionDelay->u32DelayCycleLimit = read_bytes<uint32>(Data, Size) % 128;
                        Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].pvActionData = fuzz__stActionDelay;
                        break;
                    }
                    case Psm__nenCallUserFunction:
                    {
                        if (read_bool(Data, Size) == TRUE) Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].pvActionData = NULL_PTR;
                        else 
                        {
                            Psm__tstActionFunc * fuzz__stActionFunc = new Psm__tstActionFunc;
                            switch (read_bytes<uint8>(Data, Size))
                            {
                                case 0:
                                    fuzz__stActionFunc->enfpUserFunc = Psm_vDelay_Ok;
                                    break;
                                case 1:
                                    fuzz__stActionFunc->enfpUserFunc = Psm_vDelay_Wait;
                                    break;
                                case 2:
                                    fuzz__stActionFunc->enfpUserFunc = Psm_vDelay_Error;
                                    break;
                                default:
                                    fuzz__stActionFunc->enfpUserFunc = NULL_PTR;
                                    break;
                            }
                            fuzz__stActionFunc->u32WaitIntervalUs = read_bytes<uint32>(Data, Size);
                            fuzz__stActionFunc->u32WaitCycleLimit = read_bytes<uint32>(Data, Size) % 128;
                            fuzz__stActionFunc->boBlockingWait = read_bool(Data, Size);
                            Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].pvActionData = fuzz__stActionFunc;
                        }
                        break;
                    }
                    case Psm__nenCallUserFunction_VoidReturn:
                    {                        
                        if (read_bool(Data, Size) == TRUE) Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].pvActionData =NULL_PTR;
                        else
                        {
                            Psm__tstActionVoidFunc * fuzz__stActionVoidFunc = new Psm__tstActionVoidFunc;
                            fuzz__stActionVoidFunc->vfpVoidUserFunc = (read_bool(Data, Size)) ? Psm_voidFunc : NULL_PTR;
                            Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].pvActionData = fuzz__stActionVoidFunc;
                        }
                        break;
                    }
                    case Psm__nenTriggerPowerSequence:
                    {
                        Psm__tstActionTriggerPs * fuzz__stActionTriggerPs = new Psm__tstActionTriggerPs;
                        Psm_tenPsId fuzz__targetPsId = read_enPsId(Data, Size);
                        fuzz__stActionTriggerPs->enTargetPsId = (fuzz__targetPsId < Psm_u8PsCount) ? fuzz__targetPsId : Psm_nenPsCount;
                        fuzz__stActionTriggerPs->enTriggerPsSequence = read_enSequence(Data, Size);
                        Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].pvActionData = fuzz__stActionTriggerPs;
                        break;
                    }
                    case Psm__nenWaitForPowerSequence:
                    {
                        Psm__tstActionWaitPs * fuzz__stActionWaitPs = new Psm__tstActionWaitPs;
                        Psm_tenPsId fuzz__targetPsId = read_enPsId(Data, Size);
                        fuzz__stActionWaitPs->enTargetPsId = (fuzz__targetPsId < Psm_u8PsCount) ? fuzz__targetPsId : Psm_nenPsCount;
                        fuzz__stActionWaitPs->enWaitForPsState = read_enPsState(Data, Size);
                        fuzz__stActionWaitPs->u32WaitIntervalUs = read_bytes<uint32>(Data, Size);
                        fuzz__stActionWaitPs->u32WaitCycleLimit = read_bytes<uint32>(Data, Size) % 128;
                        fuzz__stActionWaitPs->boBlockingWait = read_bool(Data, Size);
                        Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].pvActionData = fuzz__stActionWaitPs;
                        break;
                    }
                    default:
                    {    
                        Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].pvActionData = NULL_PTR;
                        break;
                    }
                }
            }
        }
        uint8 numShutdownActions = read_bytes<uint8>(Data, Size) % 32;
        if (numShutdownActions == 0)
        {
            Psm__astPowerSequence[i].pstConfig->pastShutdownAction = NULL_PTR;
        }
        else
        {
            Psm__astPowerSequence[i].pstConfig->pastShutdownAction = new Psm__tstAction[numShutdownActions];
            for (int j = 0; j < numShutdownActions; j++)
            {
                Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].enAction = read_enAction(Data, Size);
                switch (Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].enAction)
                {
                    case Psm__nenInvalidAction:
                    {
                        break;
                    }
                    case Psm__nenDriveDio:
                    {
                        Psm__tstActionDriveIo * fuzz__stActionDriveIo = new Psm__tstActionDriveIo;
                        uint8 u8numPastDioState = read_bytes<uint8>(Data, Size) % 32;
                        if (u8numPastDioState > 0 && read_bool(Data, Size) == TRUE)
                        {
                            fuzz__stActionDriveIo->pastDioState = new Psm__tstDioState[u8numPastDioState];
                            for (uint8 k = 0; k < u8numPastDioState; k++)
                            {
                                fuzz__stActionDriveIo->pastDioState[k].u16DioChannel = read_bytes<uint16>(Data, Size);
                                fuzz__stActionDriveIo->pastDioState[k].u8DioLevel = read_DioLevel(Data, Size);
                            }
                        }
                        else
                        {
                            fuzz__stActionDriveIo->pastDioState = NULL_PTR;
                        }
                        fuzz__stActionDriveIo->u8NumOfDio = u8numPastDioState;
                        Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].pvActionData = fuzz__stActionDriveIo;
                        break;
                    }
                    case Psm__nenPollDio:
                    {
                        Psm__tstActionPollIo * fuzz__stActionPollIo = new Psm__tstActionPollIo;
                        uint8 u8numPastDioState = read_bytes<uint8>(Data, Size) % 32;
                        if (u8numPastDioState > 0 && read_bool(Data, Size) == TRUE)
                        {
                            fuzz__stActionPollIo->pastDioState = new Psm__tstDioState[u8numPastDioState];
                            for (uint8 k = 0; k < u8numPastDioState; k++)
                            {
                                fuzz__stActionPollIo->pastDioState[k].u16DioChannel = read_bytes<uint16>(Data, Size);
                                fuzz__stActionPollIo->pastDioState[k].u8DioLevel = read_DioLevel(Data, Size);
                            }
                        }
                        else
                        {
                            fuzz__stActionPollIo->pastDioState = NULL_PTR;
                        }
                        fuzz__stActionPollIo->u32WaitIntervalUs = read_bytes<uint32>(Data, Size);
                        fuzz__stActionPollIo->u32WaitCycleLimit = read_bytes<uint32>(Data, Size) % 128;
                        fuzz__stActionPollIo->u8NumOfDio = u8numPastDioState;
                        fuzz__stActionPollIo->boBlockingWait = read_bool(Data, Size);
                        Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].pvActionData = fuzz__stActionPollIo;
                        break;
                    }
                    case Psm__nenSynchronousDelay:
                    case Psm__nenAsynchronousDelay:
                    {
                        Psm__tstActionDelay * fuzz__stActionDelay = new Psm__tstActionDelay;
                        fuzz__stActionDelay->u32DelayIntervalUs = read_bytes<uint32>(Data, Size);
                        fuzz__stActionDelay->u32DelayCycleLimit = read_bytes<uint32>(Data, Size) % 128;
                        Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].pvActionData = fuzz__stActionDelay;
                        break;
                    }
                    case Psm__nenCallUserFunction:
                    {
                        if (read_bool(Data, Size) == TRUE) Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].pvActionData = NULL_PTR;
                        else 
                        {
                            Psm__tstActionFunc * fuzz__stActionFunc = new Psm__tstActionFunc;
                            switch (read_bytes<uint8>(Data, Size))
                            {
                                case 0:
                                    fuzz__stActionFunc->enfpUserFunc = Psm_vDelay_Ok;
                                    break;
                                case 1:
                                    fuzz__stActionFunc->enfpUserFunc = Psm_vDelay_Wait;
                                    break;
                                case 2:
                                    fuzz__stActionFunc->enfpUserFunc = Psm_vDelay_Error;
                                    break;
                                default:
                                    fuzz__stActionFunc->enfpUserFunc = NULL_PTR;
                                    break;
                            }
                            fuzz__stActionFunc->u32WaitIntervalUs = read_bytes<uint32>(Data, Size);
                            fuzz__stActionFunc->u32WaitCycleLimit = read_bytes<uint32>(Data, Size) % 128;
                            fuzz__stActionFunc->boBlockingWait = read_bool(Data, Size);
                            Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].pvActionData = fuzz__stActionFunc;
                        }
                        break;
                    }
                    case Psm__nenCallUserFunction_VoidReturn:
                    {                        
                        if (read_bool(Data, Size) == TRUE) Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].pvActionData =NULL_PTR;
                        else
                        {
                            Psm__tstActionVoidFunc * fuzz__stActionVoidFunc = new Psm__tstActionVoidFunc;
                            fuzz__stActionVoidFunc->vfpVoidUserFunc = (read_bool(Data, Size)) ? Psm_voidFunc : NULL_PTR;
                            Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].pvActionData = fuzz__stActionVoidFunc;
                        }
                        break;
                    }
                    case Psm__nenTriggerPowerSequence:
                    {
                        Psm__tstActionTriggerPs * fuzz__stActionTriggerPs = new Psm__tstActionTriggerPs;
                        Psm_tenPsId fuzz__targetPsId = read_enPsId(Data, Size);
                        fuzz__stActionTriggerPs->enTargetPsId = (fuzz__targetPsId < Psm_u8PsCount) ? fuzz__targetPsId : Psm_nenPsCount;
                        fuzz__stActionTriggerPs->enTriggerPsSequence = read_enSequence(Data, Size);
                        Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].pvActionData = fuzz__stActionTriggerPs;
                        break;
                    }
                    case Psm__nenWaitForPowerSequence:
                    {
                        Psm__tstActionWaitPs * fuzz__stActionWaitPs = new Psm__tstActionWaitPs;
                        Psm_tenPsId fuzz__targetPsId = read_enPsId(Data, Size);
                        fuzz__stActionWaitPs->enTargetPsId = (fuzz__targetPsId < Psm_u8PsCount) ? fuzz__targetPsId : Psm_nenPsCount;
                        fuzz__stActionWaitPs->enWaitForPsState = read_enPsState(Data, Size);
                        fuzz__stActionWaitPs->u32WaitIntervalUs = read_bytes<uint32>(Data, Size);
                        fuzz__stActionWaitPs->u32WaitCycleLimit = read_bytes<uint32>(Data, Size) % 128;
                        fuzz__stActionWaitPs->boBlockingWait = read_bool(Data, Size);
                        Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].pvActionData = fuzz__stActionWaitPs;
                        break;
                    } 
                    default:
                    {    
                        Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].pvActionData = NULL_PTR;
                        break;
                    }
                }
            }
        }
        uint8 numForceShutdownDio = read_bytes<uint8>(Data, Size) % 32;
        if (numForceShutdownDio > 0 && read_bool(Data, Size) == TRUE)
        {
            Psm__astPowerSequence[i].pstConfig->pastForceShutdownDio = new Psm__tstDioState[numForceShutdownDio];
            for (int j = 0; j < numForceShutdownDio; j++)
            {
                Psm__astPowerSequence[i].pstConfig->pastForceShutdownDio[j].u16DioChannel = read_bytes<uint16>(Data, Size);
                Psm__astPowerSequence[i].pstConfig->pastForceShutdownDio[j].u8DioLevel = read_DioLevel(Data, Size);
            }
        }
        else
        {
            Psm__astPowerSequence[i].pstConfig->pastForceShutdownDio = NULL_PTR;
        }

        if (read_bool(Data, Size) == TRUE) Psm__astPowerSequence[i].pstConfig->vfpForceShutdownFunc = &Dummy_vfpForceShutdownFunc;
        else Psm__astPowerSequence[i].pstConfig->vfpForceShutdownFunc = NULL_PTR;

        if (read_bool(Data, Size) == TRUE) Psm__astPowerSequence[i].pstConfig->vfpNotifyFunc = &Dummy_vfpNotifyFunc;
        else Psm__astPowerSequence[i].pstConfig->vfpNotifyFunc = NULL_PTR;

        Psm__astPowerSequence[i].pstConfig->stMonitorDioState.u16DioChannel = read_bytes<uint16>(Data, Size);
        Psm__astPowerSequence[i].pstConfig->stMonitorDioState.u8DioLevel = read_DioLevel(Data, Size);

        Psm__astPowerSequence[i].pstConfig->u32MonitorIntervalUs = read_bytes<uint32>(Data, Size);
        Psm__astPowerSequence[i].pstConfig->u16GptChannel = read_bytes<uint16>(Data, Size);
        Psm__astPowerSequence[i].pstConfig->u16IcuChannel = read_bytes<uint16>(Data, Size);
        Psm__astPowerSequence[i].pstConfig->enType = read_enPsType(Data, Size);
        Psm__astPowerSequence[i].pstConfig->enMonitorType = read_enMonitorType(Data, Size);
        Psm__astPowerSequence[i].pstConfig->u8NumOfForceShutdownDio = numForceShutdownDio;
        Psm__astPowerSequence[i].pstConfig->u8NumOfStartupAction = numStartupActions;
        Psm__astPowerSequence[i].pstConfig->u8NumOfShutdownAction = numShutdownActions;
    }

    extern Psm__tstPsIdQueue Psm__stAsyncEventQueue;
    uint8 u8MaxNumOfPsIdQueue = read_bytes<uint8>(Data, Size) % 32;
    if (read_bool(Data, Size) == TRUE) 
    {
        Psm__stAsyncEventQueue.pau8PsidQueue = NULL_PTR;
    } 
    else 
    {
        if (u8MaxNumOfPsIdQueue == 0)
        {
            Psm__stAsyncEventQueue.pau8PsidQueue = new uint8[1];
        }
        else
        {
            uint8 u8NumPsIdQueue = read_bytes<uint8>(Data, Size) % (u8MaxNumOfPsIdQueue + 1);
            Psm__stAsyncEventQueue.pau8PsidQueue = new uint8[u8MaxNumOfPsIdQueue];
            for (int i = 0; i < u8NumPsIdQueue; i++) 
            {
                Psm_tenPsId fuzz__enPsId = read_enPsId(Data, Size);
                if (fuzz__enPsId >= Psm_u8PsCount) fuzz__enPsId = Psm_nenPsCount;
                Psm__stAsyncEventQueue.pau8PsidQueue[i] = fuzz__enPsId;
            }
        }
    }
    Psm__stAsyncEventQueue.i8HeadIdx = read_bytes<sint8>(Data, Size);
    Psm__stAsyncEventQueue.i8TailIdx = read_bytes<sint8>(Data, Size);
    Psm__stAsyncEventQueue.u8MaxQueuesize = u8MaxNumOfPsIdQueue;

    uint8 u8NumOfOperations = read_bytes<uint8>(Data, Size);
    for (int i = 0; i < u8NumOfOperations; i++)
    {
        Psm_tenPsId fuzz__enPsId = read_enPsId(Data, Size);
        if (fuzz__enPsId >= Psm_u8PsCount) fuzz__enPsId = Psm_nenPsCount;
        switch (read_bytes<uint8>(Data, Size) % 9)
        {
            case 0:
                Psm_boRunSequence(fuzz__enPsId);
                break;
            case 1:
                Psm_boStartup(fuzz__enPsId);
                break;
            case 2:
                Psm_boShutdown(fuzz__enPsId);
                break;
            case 3:
                Psm_boResume(fuzz__enPsId);
                break;
            case 4:
                Psm_enGetState(fuzz__enPsId);
                break;
            case 5:
                Psm_vStartMonitoring(fuzz__enPsId);
                break;
            case 6:
                Psm_vStopMonitoring(fuzz__enPsId);
                break;
            case 7:
                Psm_vCallbackPsmPowerSequence_0(fuzz__enPsId);
                break;
            case 8:
                Psm_vCallbackPsmPowerSequence_1(fuzz__enPsId);
                break;
        }
    }
    
    // free allocated memory
    for (int i = 0; i < Psm_u8PsCount; ++i) {
        if (Psm__astPowerSequence[i].pstConfig) {
            // Free startup actions
            if (Psm__astPowerSequence[i].pstConfig->pastStartupAction) {
                for (int j = 0; j < Psm__astPowerSequence[i].pstConfig->u8NumOfStartupAction; ++j) {
                    if (Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].enAction == Psm__nenDriveDio ||
                        Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].enAction == Psm__nenPollDio) {
                        Psm__tstActionDriveIo* actionData = static_cast<Psm__tstActionDriveIo*>(Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].pvActionData);
                        delete[] actionData->pastDioState;
                    }
                    if (Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].enAction != Psm__nenInvalidAction) delete Psm__astPowerSequence[i].pstConfig->pastStartupAction[j].pvActionData;
                }
                delete[] Psm__astPowerSequence[i].pstConfig->pastStartupAction;
            }

            // Free shutdown actions
            if (Psm__astPowerSequence[i].pstConfig->pastShutdownAction) {
                for (int j = 0; j < Psm__astPowerSequence[i].pstConfig->u8NumOfShutdownAction; ++j) {
                    if (Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].enAction == Psm__nenDriveDio ||
                        Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].enAction == Psm__nenPollDio) {
                        Psm__tstActionDriveIo* actionData = static_cast<Psm__tstActionDriveIo*>(Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].pvActionData);
                        delete[] actionData->pastDioState;
                    }
                    if (Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].enAction != Psm__nenInvalidAction) delete Psm__astPowerSequence[i].pstConfig->pastShutdownAction[j].pvActionData;
                }
                delete[] Psm__astPowerSequence[i].pstConfig->pastShutdownAction;
            }

            // Free force shutdown DIO array
            if (Psm__astPowerSequence[i].pstConfig->pastForceShutdownDio) {
                delete[] Psm__astPowerSequence[i].pstConfig->pastForceShutdownDio;
            }

            delete Psm__astPowerSequence[i].pstConfig;
        }
    }

    if (Psm__stAsyncEventQueue.pau8PsidQueue != NULL_PTR) {
        delete[] Psm__stAsyncEventQueue.pau8PsidQueue;
    }

    return 0;
}