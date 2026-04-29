/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fuzzer/FuzzedDataProvider.h>
#include "mechbodycontrollerservicesix_fuzzer.h"
#include "mechbody_controller_service.h"
#include "mechbody_controller_ipc_interface_code.h"
#include "ipc_skeleton.h"
#include "mc_motion_manager.h"
#include "mc_camera_tracking_controller.h"

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

enum class TestFunctionId {
    FUZZ_REGISTER_ATTACH_STATE_CHANGE_CALLBACK = 0,
    FUZZ_CLEAN_MOTION_MANAGERS = 1,
    FUZZ_GET_TRACKING_ENABLED_DEVICE = 2,
    FUZZ_NOTIFY_MECH_EVENT = 3,
    FUZZ_REGISTER_TRACKING_EVENT_CALLBACK = 4,
    FUZZ_UNREGISTER_TRACKING_EVENT_CALLBACK = 5,
    FUZZ_REGISTER_CMD_CHANNEL = 6,
    FUZZ_REGISTER_ROTATION_AXES_STATUS_CHANGE_CALLBACK = 7,
    FUZZ_UNREGISTER_ROTATION_AXES_STATUS_CHANGE_CALLBACK = 8,
    FUZZ_SEARCH_TARGET = 9,
    FUZZ_SEARCH_TARGET_END = 10,
    FUZZ_MOVE = 11,
    FUZZ_MOVE_BY_SPEED = 12,
    FUZZ_TURN_BY_SPEED = 13,
    FUZZ_IS_SUPPORT_ACTION = 14,
    FUZZ_DO_ACTION = 15,
    FUZZ_SUBSCRIBE_CALLBACK = 16,
    FUZZ_UNSUBSCRIBE_CALLBACK = 17,
    FUZZ_FULL_WORKFLOW = 18
};

void FuzzRegisterAttachStateChangeCallback(FuzzedDataProvider &provider)
{
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    bool shouldUseNull = provider.ConsumeBool();
    if (shouldUseNull) {
        service.RegisterAttachStateChangeCallback(nullptr);
    } else {
        service.RegisterAttachStateChangeCallback(static_cast<sptr<IRemoteObject>>(nullptr));
    }
}

void FuzzCleanMotionManagers(FuzzedDataProvider &provider)
{
    (void)provider;
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
}

void FuzzGetTrackingEnabledDevice(FuzzedDataProvider &provider)
{
    (void)provider;
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    bool isEnabled = false;
    service.GetTrackingEnabledDevice(isEnabled);
}

void FuzzNotifyMechEvent(FuzzedDataProvider &provider)
{
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    int32_t mechId = provider.ConsumeIntegral<int32_t>();
    int32_t eventTypeId = provider.ConsumeIntegral<int32_t>();
    MechEventType mechEventType = static_cast<MechEventType>(eventTypeId);
    service.NotifyMechEvent(mechId, mechEventType);
}

void FuzzRegisterTrackingEventCallback(FuzzedDataProvider &provider)
{
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    bool shouldUseNull = provider.ConsumeBool();
    if (shouldUseNull) {
        service.RegisterTrackingEventCallback(nullptr);
    } else {
        service.RegisterTrackingEventCallback(static_cast<sptr<IRemoteObject>>(nullptr));
    }
}

void FuzzUnRegisterTrackingEventCallback(FuzzedDataProvider &provider)
{
    (void)provider;
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    service.UnRegisterTrackingEventCallback();
}

void FuzzRegisterCmdChannel(FuzzedDataProvider &provider)
{
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    bool shouldUseNull = provider.ConsumeBool();
    if (shouldUseNull) {
        service.RegisterCmdChannel(nullptr);
    } else {
        service.RegisterCmdChannel(static_cast<sptr<IRemoteObject>>(nullptr));
    }
}

void FuzzRegisterRotationAxesStatusChangeCallback(FuzzedDataProvider &provider)
{
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    bool shouldUseNull = provider.ConsumeBool();
    if (shouldUseNull) {
        service.RegisterRotationAxesStatusChangeCallback(nullptr);
    } else {
        service.RegisterRotationAxesStatusChangeCallback(static_cast<sptr<IRemoteObject>>(nullptr));
    }
}

void FuzzUnRegisterRotationAxesStatusChangeCallback(FuzzedDataProvider &provider)
{
    (void)provider;
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    service.UnRegisterRotationAxesStatusChangeCallback();
}

void FuzzSearchTarget(FuzzedDataProvider &provider)
{
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    std::string napiCmdId = provider.ConsumeRandomLengthString(64);
    
    auto targetInfo = std::make_shared<TargetInfo>();
    if (targetInfo != nullptr) {
        targetInfo->targetType = static_cast<TargetType>(provider.ConsumeIntegral<uint32_t>());
    }
    
    auto searchParams = std::make_shared<SearchParams>();
    if (searchParams != nullptr) {
        searchParams->direction = static_cast<SearchDirection>(provider.ConsumeIntegral<uint32_t>());
    }
    
    service.SearchTarget(napiCmdId, targetInfo, searchParams);
}

void FuzzSearchTargetEnd(FuzzedDataProvider &provider)
{
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString(64);
    int32_t targetNum = provider.ConsumeIntegral<int32_t>();
    service.SearchTargetEnd(tokenId, napiCmdId, targetNum);
}

void FuzzMove(FuzzedDataProvider &provider)
{
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    int32_t mechId = provider.ConsumeIntegral<int32_t>();
    std::string cmdId = provider.ConsumeRandomLengthString(64);
    
    auto moveParams = std::make_shared<MoveParams>();
    if (moveParams != nullptr) {
        moveParams->distance = provider.ConsumeIntegral<int32_t>();
        moveParams->angle = provider.ConsumeFloatingPoint<float>();
        moveParams->speedGear = static_cast<SpeedGear>(provider.ConsumeIntegral<int32_t>());
        moveParams->mode = static_cast<MarchingMode>(provider.ConsumeIntegral<int32_t>());
    }
    
    service.Move(mechId, cmdId, moveParams);
}

void FuzzMoveBySpeed(FuzzedDataProvider &provider)
{
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    int32_t mechId = provider.ConsumeIntegral<int32_t>();
    std::string cmdId = provider.ConsumeRandomLengthString(64);
    uint16_t duration = provider.ConsumeIntegral<uint16_t>();
    
    auto speedParams = std::make_shared<SpeedParams>();
    if (speedParams != nullptr) {
        speedParams->speed = provider.ConsumeIntegral<int16_t>();
        speedParams->angle = provider.ConsumeFloatingPoint<float>();
        speedParams->mode = static_cast<MarchingMode>(provider.ConsumeIntegral<int32_t>());
    }
    
    service.MoveBySpeed(mechId, cmdId, duration, speedParams);
}

void FuzzTurnBySpeed(FuzzedDataProvider &provider)
{
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    int32_t mechId = provider.ConsumeIntegral<int32_t>();
    std::string cmdId = provider.ConsumeRandomLengthString(64);
    float angleSpeed = provider.ConsumeFloatingPoint<float>();
    uint16_t duration = provider.ConsumeIntegral<uint16_t>();
    service.TurnBySpeed(mechId, cmdId, angleSpeed, duration);
}

void FuzzIsSupportAction(FuzzedDataProvider &provider)
{
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    int32_t mechId = provider.ConsumeIntegral<int32_t>();
    int32_t actionTypeId = provider.ConsumeIntegral<int32_t>();
    ActionType actionType = static_cast<ActionType>(actionTypeId);
    bool isSupport = false;
    service.IsSupportAction(mechId, actionType, isSupport);
}

void FuzzDoAction(FuzzedDataProvider &provider)
{
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    int32_t mechId = provider.ConsumeIntegral<int32_t>();
    std::string cmdId = provider.ConsumeRandomLengthString(64);
    int32_t actionTypeId = provider.ConsumeIntegral<int32_t>();
    ActionType actionType = static_cast<ActionType>(actionTypeId);
    service.DoAction(mechId, cmdId, actionType);
}

void FuzzSubscribeCallback(FuzzedDataProvider &provider)
{
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    bool shouldUseNull = provider.ConsumeBool();
    int32_t eventTypeId = provider.ConsumeIntegral<int32_t>();
    MechEventType mechEventType = static_cast<MechEventType>(eventTypeId);
    
    if (shouldUseNull) {
        sptr<IRemoteObject> callback = nullptr;
        service.SubscribeCallback(callback, mechEventType);
    } else {
        sptr<IRemoteObject> callback = nullptr;
        service.SubscribeCallback(callback, mechEventType);
    }
}

void FuzzUnSubscribeCallback(FuzzedDataProvider &provider)
{
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    int32_t eventTypeId = provider.ConsumeIntegral<int32_t>();
    MechEventType mechEventType = static_cast<MechEventType>(eventTypeId);
    service.UnSubscribeCallback(mechEventType);
}

void FuzzFullWorkflow(FuzzedDataProvider &provider)
{
    MechBodyControllerService &service = MechBodyControllerService::GetInstance();
    
    sptr<IRemoteObject> callback = nullptr;
    service.RegisterAttachStateChangeCallback(callback);
    service.RegisterTrackingEventCallback(callback);
    service.RegisterCmdChannel(callback);
    service.RegisterRotationAxesStatusChangeCallback(callback);
    
    bool isEnabled = false;
    service.GetTrackingEnabledDevice(isEnabled);
    
    int32_t mechId = provider.ConsumeIntegral<int32_t>();
    int32_t eventTypeId = provider.ConsumeIntegral<int32_t>();
    MechEventType mechEventType = static_cast<MechEventType>(eventTypeId);
    service.NotifyMechEvent(mechId, mechEventType);
    
    std::string napiCmdId = provider.ConsumeRandomLengthString(64);
    auto targetInfo = std::make_shared<TargetInfo>();
    auto searchParams = std::make_shared<SearchParams>();
    service.SearchTarget(napiCmdId, targetInfo, searchParams);
    
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    int32_t targetNum = provider.ConsumeIntegral<int32_t>();
    service.SearchTargetEnd(tokenId, napiCmdId, targetNum);
    
    std::string cmdId = provider.ConsumeRandomLengthString(64);
    auto moveParams = std::make_shared<MoveParams>();
    if (moveParams != nullptr) {
        moveParams->distance = provider.ConsumeIntegral<int32_t>();
        moveParams->angle = provider.ConsumeFloatingPoint<float>();
        moveParams->speedGear = static_cast<SpeedGear>(provider.ConsumeIntegral<int32_t>());
        moveParams->mode = static_cast<MarchingMode>(provider.ConsumeIntegral<int32_t>());
    }
    service.Move(mechId, cmdId, moveParams);
    
    uint16_t duration = provider.ConsumeIntegral<uint16_t>();
    auto speedParams = std::make_shared<SpeedParams>();
    if (speedParams != nullptr) {
        speedParams->speed = provider.ConsumeIntegral<int16_t>();
        speedParams->angle = provider.ConsumeFloatingPoint<float>();
        speedParams->mode = static_cast<MarchingMode>(provider.ConsumeIntegral<int32_t>());
    }
    service.MoveBySpeed(mechId, cmdId, duration, speedParams);
    
    float angleSpeed = provider.ConsumeFloatingPoint<float>();
    service.TurnBySpeed(mechId, cmdId, angleSpeed, duration);
    
    ActionType actionType = static_cast<ActionType>(provider.ConsumeIntegral<int32_t>());
    bool isSupport = false;
    service.IsSupportAction(mechId, actionType, isSupport);
    service.DoAction(mechId, cmdId, actionType);
    
    service.SubscribeCallback(callback, mechEventType);
    service.UnSubscribeCallback(mechEventType);
    
    service.UnRegisterTrackingEventCallback();
    service.UnRegisterRotationAxesStatusChangeCallback();
    
    service.CleanMotionManagers();
}

void RunFuzzTest(FuzzedDataProvider &provider)
{
    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0, 18);
    
    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_REGISTER_ATTACH_STATE_CHANGE_CALLBACK:
            FuzzRegisterAttachStateChangeCallback(provider);
            break;
        case TestFunctionId::FUZZ_CLEAN_MOTION_MANAGERS:
            FuzzCleanMotionManagers(provider);
            break;
        case TestFunctionId::FUZZ_GET_TRACKING_ENABLED_DEVICE:
            FuzzGetTrackingEnabledDevice(provider);
            break;
        case TestFunctionId::FUZZ_NOTIFY_MECH_EVENT:
            FuzzNotifyMechEvent(provider);
            break;
        case TestFunctionId::FUZZ_REGISTER_TRACKING_EVENT_CALLBACK:
            FuzzRegisterTrackingEventCallback(provider);
            break;
        case TestFunctionId::FUZZ_UNREGISTER_TRACKING_EVENT_CALLBACK:
            FuzzUnRegisterTrackingEventCallback(provider);
            break;
        case TestFunctionId::FUZZ_REGISTER_CMD_CHANNEL:
            FuzzRegisterCmdChannel(provider);
            break;
        case TestFunctionId::FUZZ_REGISTER_ROTATION_AXES_STATUS_CHANGE_CALLBACK:
            FuzzRegisterRotationAxesStatusChangeCallback(provider);
            break;
        case TestFunctionId::FUZZ_UNREGISTER_ROTATION_AXES_STATUS_CHANGE_CALLBACK:
            FuzzUnRegisterRotationAxesStatusChangeCallback(provider);
            break;
        case TestFunctionId::FUZZ_SEARCH_TARGET:
            FuzzSearchTarget(provider);
            break;
        case TestFunctionId::FUZZ_SEARCH_TARGET_END:
            FuzzSearchTargetEnd(provider);
            break;
        case TestFunctionId::FUZZ_MOVE:
            FuzzMove(provider);
            break;
        case TestFunctionId::FUZZ_MOVE_BY_SPEED:
            FuzzMoveBySpeed(provider);
            break;
        case TestFunctionId::FUZZ_TURN_BY_SPEED:
            FuzzTurnBySpeed(provider);
            break;
        case TestFunctionId::FUZZ_IS_SUPPORT_ACTION:
            FuzzIsSupportAction(provider);
            break;
        case TestFunctionId::FUZZ_DO_ACTION:
            FuzzDoAction(provider);
            break;
        case TestFunctionId::FUZZ_SUBSCRIBE_CALLBACK:
            FuzzSubscribeCallback(provider);
            break;
        case TestFunctionId::FUZZ_UNSUBSCRIBE_CALLBACK:
            FuzzUnSubscribeCallback(provider);
            break;
        case TestFunctionId::FUZZ_FULL_WORKFLOW:
            FuzzFullWorkflow(provider);
            break;
        default:
            break;
    }
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    RunFuzzTest(provider);
    return 0;
}