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

#include "mechbodycontrollerservicetwo_fuzzer.h"

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

void GetTrackingEnabledFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    bool isEnabled = fdp.ConsumeIntegral<bool>();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetTrackingEnabled(isEnabled);
}

void SetTrackingLayoutFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t enumIdx = fdp.ConsumeIntegral<uint32_t>() % 4;
    CameraTrackingLayout cameraTrackingLayout = static_cast<CameraTrackingLayout>(enumIdx);

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.SetTrackingLayout(cameraTrackingLayout);
}

void GetTrackingLayoutFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t enumIdx = fdp.ConsumeIntegral<uint32_t>() % 4;
    CameraTrackingLayout cameraTrackingLayout = static_cast<CameraTrackingLayout>(enumIdx);

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetTrackingLayout(cameraTrackingLayout);
}

void RotateByDegreeFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t duration = fdp.ConsumeIntegral<uint32_t>();
    float yaw = fdp.ConsumeIntegral<uint32_t>();
    float roll = fdp.ConsumeIntegral<uint32_t>();
    float pitch = fdp.ConsumeIntegral<uint32_t>();
    RotateByDegreeParam rotateParam;
    rotateParam.duration = static_cast<int32_t>(duration);
    rotateParam.degree.yaw = yaw;
    rotateParam.degree.roll = roll;
    rotateParam.degree.pitch = pitch;
    auto rotateByDegreeParam = std::make_shared<RotateByDegreeParam>(rotateParam);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    std::string cmdId = fdp.ConsumeRandomLengthString();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.RotateByDegree(mechId, cmdId, rotateByDegreeParam);
}

void NotifyOperationResultFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t enumIdx = fdp.ConsumeIntegral<uint32_t>() % 4;
    ExecResult result = static_cast<ExecResult>(enumIdx);
    uint32_t tokenId = fdp.ConsumeIntegral<uint32_t>();
    std::string cmdId = fdp.ConsumeRandomLengthString();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.NotifyOperationResult(tokenId, cmdId, result);
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::GetTrackingEnabledFuzzTest(data, size);
    OHOS::SetTrackingLayoutFuzzTest(data, size);
    OHOS::GetTrackingLayoutFuzzTest(data, size);
    OHOS::RotateByDegreeFuzzTest(data, size);
    OHOS::NotifyOperationResultFuzzTest(data, size);
    return 0;
}