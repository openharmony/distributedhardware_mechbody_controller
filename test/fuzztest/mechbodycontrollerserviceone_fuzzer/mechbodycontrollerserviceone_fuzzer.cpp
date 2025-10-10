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

#include "mechbodycontrollerserviceone_fuzzer.h"

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

void UnRegisterAttachStateChangeCallbackFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t tokenId = fdp.ConsumeIntegral<uint32_t>();
    const sptr<OHOS::IRemoteObject> callback;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.listener_ = std::shared_ptr<IMechConnectListener>();
    mechBodyControllerService.deviceAttachCallback_[tokenId] = callback;
    mechBodyControllerService.UnRegisterAttachStateChangeCallback();
}

void OnAttachStateChangeFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    std::string mechName = fdp.ConsumeRandomLengthString();
    uint32_t tokenId = fdp.ConsumeIntegral<uint32_t>();
    uint32_t enumIdx = fdp.ConsumeIntegral<uint32_t>() % 2;
    AttachmentState attachmentState = static_cast<AttachmentState>(enumIdx);
    AttachmentState state = static_cast<AttachmentState>(enumIdx);
    const sptr<OHOS::IRemoteObject> callback;
    MechInfo mechInfo;
    mechInfo.mechName = mechName;
    mechInfo.state = state;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.deviceAttachCallback_[tokenId] = callback;
    mechBodyControllerService.OnAttachStateChange(attachmentState, mechInfo);
}

void OnDeviceConnectedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    int32_t mechId = fdp.ConsumeIntegral<int32_t>();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.OnDeviceConnected(mechId);
}

void GetAttachedDevicesFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    std::set<MechInfo> mechInfos;
    MechInfo mechInfo;
    FuzzedDataProvider fdp(data, size);
    std::string macHash = fdp.ConsumeRandomLengthString();
    std::string mechName = fdp.ConsumeRandomLengthString();
    mechInfo.mechName = mechName;
    mechInfos.insert(mechInfo);

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetAttachedDevices(mechInfos);
}

void SetTrackingEnabledFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    bool isEnabled = fdp.ConsumeIntegral<bool>();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.SetTrackingEnabled(isEnabled);
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::UnRegisterAttachStateChangeCallbackFuzzTest(data, size);
    OHOS::OnAttachStateChangeFuzzTest(data, size);
    OHOS::OnDeviceConnectedFuzzTest(data, size);
    OHOS::GetAttachedDevicesFuzzTest(data, size);
    OHOS::SetTrackingEnabledFuzzTest(data, size);
    return 0;
}