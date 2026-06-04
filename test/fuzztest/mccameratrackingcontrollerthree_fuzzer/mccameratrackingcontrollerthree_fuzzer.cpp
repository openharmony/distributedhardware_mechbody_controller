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
#include "mccameratrackingcontrollerthree_fuzzer.h"
#include "mc_camera_tracking_controller.h"
#include "mechbody_controller_service.h"
#include "mechbody_controller_types.h"
#include "mechbody_controller_enums.h"
#include <memory>
#include <iremote_object.h>

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {
constexpr size_t MAX_CMD_ID_LENGTH = 64;
constexpr int32_t MIN_LAYOUT_TYPE = 0;
constexpr int32_t MAX_LAYOUT_TYPE = 2;

McCameraTrackingController* g_cameraTrackingController = nullptr;

void InitCameraTrackingController()
{
    if (g_cameraTrackingController == nullptr) {
        g_cameraTrackingController = &McCameraTrackingController::GetInstance();
    }
}

void FuzzSearchTarget(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    std::string napiCmdId = provider.ConsumeRandomLengthString(MAX_CMD_ID_LENGTH);
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    auto targetInfo = std::make_shared<TargetInfo>();
    auto searchParams = std::make_shared<SearchParams>();
    g_cameraTrackingController->SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);
}

void FuzzSearchTargetRotateFinish(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    std::string cmdId = provider.ConsumeRandomLengthString(MAX_CMD_ID_LENGTH);
    g_cameraTrackingController->SearchTargetRotateFinish(cmdId);
}

void FuzzRegisterTrackingEventCallback(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    sptr<IRemoteObject> callback = nullptr;
    g_cameraTrackingController->RegisterTrackingEventCallback(tokenId, callback);
}

void FuzzUnRegisterTrackingEventCallback(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    g_cameraTrackingController->UnRegisterTrackingEventCallback(tokenId);
}

void FuzzSetTrackingLayoutWithToken(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    CameraTrackingLayout layout = static_cast<CameraTrackingLayout>(
        provider.ConsumeIntegralInRange<int32_t>(MIN_LAYOUT_TYPE, MAX_LAYOUT_TYPE));
    g_cameraTrackingController->SetTrackingLayout(tokenId, layout);
}

void FuzzUpdateCurrentCameraInfo(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    CameraStandard::CaptureSessionInfo captureSessionInfo;
    captureSessionInfo.sessionId = provider.ConsumeIntegral<int32_t>();
    g_cameraTrackingController->UpdateCurrentCameraInfoByCaptureSessionInfo(captureSessionInfo);
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzSearchTarget(data, size);
    FuzzSearchTargetRotateFinish(data, size);
    FuzzRegisterTrackingEventCallback(data, size);
    FuzzUnRegisterTrackingEventCallback(data, size);
    FuzzSetTrackingLayoutWithToken(data, size);
    FuzzUpdateCurrentCameraInfo(data, size);
    return 0;
}