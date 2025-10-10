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

#include "mechbodycontrollerservicefour_fuzzer.h"

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
bool Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(uint64_t tokenId)
{
    return true;
}

void RotateToEulerAnglesFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t duration = fdp.ConsumeIntegral<uint32_t>();
    float yaw = fdp.ConsumeIntegral<uint32_t>();
    float roll = fdp.ConsumeIntegral<uint32_t>();
    float pitch = fdp.ConsumeIntegral<uint32_t>();
    RotateToEulerAnglesParam rotateParam;
    rotateParam.duration = static_cast<int32_t>(duration);
    rotateParam.angles.yaw = yaw;
    rotateParam.angles.roll = roll;
    rotateParam.angles.pitch = pitch;
    auto rotateToEulerAnglesParam = std::make_shared<RotateToEulerAnglesParam>(rotateParam);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    std::string cmdId = fdp.ConsumeRandomLengthString();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.RotateToEulerAngles(mechId, cmdId, rotateToEulerAnglesParam);
}

void GetMaxRotationTimeFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    auto timeLimit = std::shared_ptr<TimeLimit>();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetMaxRotationTime(mechId, timeLimit);
}

void GetMaxRotationSpeedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    float yawMin = fdp.ConsumeIntegral<uint32_t>();
    float rollMin = fdp.ConsumeIntegral<uint32_t>();
    float pitchMin = fdp.ConsumeIntegral<uint32_t>();
    float yawMax = fdp.ConsumeIntegral<uint32_t>();
    float rollMax = fdp.ConsumeIntegral<uint32_t>();
    float pitchMax = fdp.ConsumeIntegral<uint32_t>();
    RotateSpeedLimit rotateSpeedLimit;
    rotateSpeedLimit.speedMax.yawSpeed = yawMax;
    rotateSpeedLimit.speedMax.rollSpeed = rollMax;
    rotateSpeedLimit.speedMax.pitchSpeed = pitchMax;
    rotateSpeedLimit.speedMin.yawSpeed = yawMin;
    rotateSpeedLimit.speedMin.rollSpeed = rollMin;
    rotateSpeedLimit.speedMin.pitchSpeed = pitchMin;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetMaxRotationSpeed(mechId, rotateSpeedLimit);
}

void RotateBySpeedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t duration = fdp.ConsumeIntegral<uint32_t>();
    float yaw = fdp.ConsumeIntegral<uint32_t>();
    float roll = fdp.ConsumeIntegral<uint32_t>();
    float pitch = fdp.ConsumeIntegral<uint32_t>();
    RotateBySpeedParam rotateParam;
    rotateParam.duration = static_cast<float>(duration);
    rotateParam.speed.yawSpeed = yaw;
    rotateParam.speed.rollSpeed = roll;
    rotateParam.speed.pitchSpeed = pitch;
    auto rotateBySpeedParam = std::make_shared<RotateBySpeedParam>(rotateParam);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    std::string cmdId = fdp.ConsumeRandomLengthString();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.RotateBySpeed(mechId, cmdId, rotateBySpeedParam);
}

void StopMovingFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    std::string cmdId = fdp.ConsumeRandomLengthString();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.StopMoving(mechId, cmdId);
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::RotateToEulerAnglesFuzzTest(data, size);
    OHOS::GetMaxRotationTimeFuzzTest(data, size);
    OHOS::GetMaxRotationSpeedFuzzTest(data, size);
    OHOS::RotateBySpeedFuzzTest(data, size);
    OHOS::StopMovingFuzzTest(data, size);
    return 0;
}