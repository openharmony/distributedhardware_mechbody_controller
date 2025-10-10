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

#include "mechbodycontrollerservicefive_fuzzer.h"

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

void GetRotationAnglesFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    float yaw = fdp.ConsumeIntegral<uint32_t>();
    float roll = fdp.ConsumeIntegral<uint32_t>();
    float pitch = fdp.ConsumeIntegral<uint32_t>();
    EulerAngles eulerAngles;
    eulerAngles.yaw = yaw;
    eulerAngles.roll = roll;
    eulerAngles.pitch = pitch;
    auto eulerAnglesPtr = std::make_shared<EulerAngles>(eulerAngles);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetRotationAngles(mechId, eulerAnglesPtr);
}

void GetRotationDegreeLimitsFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    float yawNeg = fdp.ConsumeIntegral<uint32_t>();
    float rollNeg = fdp.ConsumeIntegral<uint32_t>();
    float pitchNeg = fdp.ConsumeIntegral<uint32_t>();
    float yawPos = fdp.ConsumeIntegral<uint32_t>();
    float rollPos = fdp.ConsumeIntegral<uint32_t>();
    float pitchPos = fdp.ConsumeIntegral<uint32_t>();
    RotateDegreeLimit rotationLimit;
    rotationLimit.negMax.yaw = yawNeg;
    rotationLimit.negMax.roll = rollNeg;
    rotationLimit.negMax.pitch = pitchNeg;
    rotationLimit.posMax.yaw = yawPos;
    rotationLimit.posMax.roll = rollPos;
    rotationLimit.posMax.pitch = pitchPos;
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetRotationDegreeLimits(mechId, rotationLimit);
}

void GetRotationAxesStatusFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    bool boolIdx = fdp.ConsumeIntegral<uint32_t>() % 2;
    uint32_t enumIdx = fdp.ConsumeIntegral<uint32_t>() % 3;
    RotationAxisLimited limited = static_cast<RotationAxisLimited>(enumIdx);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    RotationAxesStatus axesStatus;
    axesStatus.pitchEnabled = boolIdx;
    axesStatus.rollEnabled = boolIdx;
    axesStatus.yawEnabled = boolIdx;
    axesStatus.pitchLimited = limited;
    axesStatus.rollLimited = limited;
    axesStatus.yawLimited = limited;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetRotationAxesStatus(mechId, axesStatus);
}

void OnRotationAxesStatusChangeFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    bool boolIdx = fdp.ConsumeIntegral<uint32_t>() % 2;
    uint32_t enumIdx = fdp.ConsumeIntegral<uint32_t>() % 3;
    RotationAxisLimited limited = static_cast<RotationAxisLimited>(enumIdx);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    RotationAxesStatus axesStatus;
    axesStatus.pitchEnabled = boolIdx;
    axesStatus.rollEnabled = boolIdx;
    axesStatus.yawEnabled = boolIdx;
    axesStatus.pitchLimited = limited;
    axesStatus.rollLimited = limited;
    axesStatus.yawLimited = limited;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.OnRotationAxesStatusChange(mechId, axesStatus);
}

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
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::GetRotationAnglesFuzzTest(data, size);
    OHOS::GetRotationDegreeLimitsFuzzTest(data, size);
    OHOS::GetRotationAxesStatusFuzzTest(data, size);
    OHOS::OnRotationAxesStatusChangeFuzzTest(data, size);
    OHOS::SetUserOperationFuzzTest(data, size);
    return 0;
}