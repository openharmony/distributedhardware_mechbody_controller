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

#include "mechbodycontrollerservicesix_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include <string>

#define private public
#define protected public
#include "mechbody_controller_service.h"
#undef private
#undef protected
#include "ipc_skeleton.h"
#include "securec.h"
#include "accesstoken_kit.h"
#include "token_setproc.h"
#include "tokenid_kit.h"
#include "system_ability.h"

namespace OHOS {
using namespace OHOS::MechBodyController;
void SetUserOperationFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider fdp(data, size);
    int32_t num = fdp.ConsumeIntegral<int32_t>();
    std::shared_ptr<Operation> operation = std::make_shared<Operation>(
            static_cast<Operation>(num));
    std::string mac = fdp.ConsumeRandomLengthString();
    std::string param = fdp.ConsumeRandomLengthString();
    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.SetUserOperation(operation, mac, param);

    int32_t mechId = fdp.ConsumeIntegral<int32_t>();
    mechBodyControllerService.OnDeviceDisconnected(mechId);
}

void MoveBySpeedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    int32_t mechId = fdp.ConsumeIntegral<int32_t>();
    std::string cmdId = fdp.ConsumeRandomLengthString();
    uint16_t duration = fdp.ConsumeIntegral<uint16_t>();
    auto speedParams = std::make_shared<SpeedParams>();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.MoveBySpeed(mechId, cmdId, duration, speedParams);
}

void DoActionFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    int32_t mechId = fdp.ConsumeIntegralInRange<int32_t>(0, 3);
    std::string cmdId = fdp.ConsumeRandomLengthString();
    uint32_t enumIdx = fdp.ConsumeIntegral<uint32_t>() % 10;
    ActionType actionType = static_cast<ActionType>(enumIdx);

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.DoAction(mechId, cmdId, actionType);
}

void TurnBySpeedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    int32_t mechId = fdp.ConsumeIntegralInRange<int32_t>(0, 3);
    std::string cmdId = fdp.ConsumeRandomLengthString();
    float angleSpeed = fdp.ConsumeIntegral<uint32_t>();
    uint16_t duration = fdp.ConsumeIntegral<uint16_t>();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.TurnBySpeed(mechId, cmdId, angleSpeed, duration);
}

void SubscribeCallbackFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t enumIdx = fdp.ConsumeIntegral<uint32_t>() % 10;
    MechEventType mechEventType = static_cast<MechEventType>(enumIdx);
    sptr<IRemoteObject> callback;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.SubscribeCallback(callback, mechEventType);
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
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::SetUserOperationFuzzTest(data, size);
    OHOS::MoveBySpeedFuzzTest(data, size);
    OHOS::DoActionFuzzTest(data, size);
    OHOS::TurnBySpeedFuzzTest(data, size);
    OHOS::SubscribeCallbackFuzzTest(data, size);
    return 0;
}