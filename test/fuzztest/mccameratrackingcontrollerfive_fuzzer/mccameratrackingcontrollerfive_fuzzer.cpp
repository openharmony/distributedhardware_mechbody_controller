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
#include "mccameratrackingcontrollerfive_fuzzer.h"
#include "mc_camera_tracking_controller.h"
#include "mechbody_controller_service.h"
#include "mechbody_controller_types.h"
#include "mechbody_controller_enums.h"
#include <memory>
#include <iremote_object.h>
#include "event_handler.h"
#include "common_event.h"

using namespace OHOS;
using namespace OHOS::MechBodyController;
using namespace EventFwk;

namespace {
constexpr int32_t MIN_TRACKING_MODE = 0;
constexpr int32_t MAX_TRACKING_MODE = 2;
constexpr size_t MIN_ITEM_CAPACITY = 1;
constexpr size_t MAX_ITEM_CAPACITY = 100;
constexpr size_t MIN_DATA_CAPACITY = 1;
constexpr size_t MAX_DATA_CAPACITY = 1000;
constexpr size_t MAX_ACTION_STRING_LENGTH = 64;

McCameraTrackingController* g_cameraTrackingController = nullptr;

void InitCameraTrackingController()
{
    if (g_cameraTrackingController == nullptr) {
        g_cameraTrackingController = &McCameraTrackingController::GetInstance();
    }
}

CameraStandard::Rect GenerateRandomRect(FuzzedDataProvider &provider)
{
    CameraStandard::Rect rect;
    rect.topLeftX = provider.ConsumeFloatingPointInRange<float>(0.0f, 1.0f);
    rect.topLeftY = provider.ConsumeFloatingPointInRange<float>(0.0f, 1.0f);
    rect.width = provider.ConsumeFloatingPointInRange<float>(0.0f, 1.0f);
    rect.height = provider.ConsumeFloatingPointInRange<float>(0.0f, 1.0f);
    return rect;
}

void FuzzOnFocusTrackingInfo(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    CameraStandard::FocusTrackingMetaInfo info;
    info.SetTrackingObjectId(provider.ConsumeIntegral<int32_t>());

    CameraStandard::Rect trackingRegion = GenerateRandomRect(provider);
    info.SetTrackingRegion(trackingRegion);

    int32_t trackingMode = provider.ConsumeIntegralInRange<int32_t>(MIN_TRACKING_MODE, MAX_TRACKING_MODE);
    info.SetTrackingMode(static_cast<CameraStandard::FocusTrackingMode>(trackingMode));
    g_cameraTrackingController->OnFocusTracking(info);
}

void FuzzOnCaptureSessionConfiged(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    CameraStandard::CaptureSessionInfo captureSessionInfo;
    captureSessionInfo.sessionId = provider.ConsumeIntegral<int32_t>();
    g_cameraTrackingController->OnCaptureSessionConfiged(captureSessionInfo);
}

void FuzzOnZoomInfoChange(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    int32_t sessionid = provider.ConsumeIntegral<int32_t>();
    CameraStandard::ZoomInfo zoomInfo;
    zoomInfo.zoomValue = provider.ConsumeFloatingPoint<float>();
    zoomInfo.equivalentFocus = provider.ConsumeIntegral<int32_t>();
    zoomInfo.videoStabilizationMode = provider.ConsumeIntegral<int32_t>();
    zoomInfo.focusMode = provider.ConsumeIntegral<int32_t>();
    g_cameraTrackingController->OnZoomInfoChange(sessionid, zoomInfo);
}

void FuzzOnSessionStatusChange(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    int32_t sessionid = provider.ConsumeIntegral<int32_t>();
    bool status = provider.ConsumeBool();
    g_cameraTrackingController->OnSessionStatusChange(sessionid, status);
}

void FuzzOnMetadataInfo(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    bool passNull = provider.ConsumeBool();
    if (passNull) {
        g_cameraTrackingController->OnMetadataInfo(nullptr);
    } else {
        size_t itemCapacity = provider.ConsumeIntegralInRange<size_t>(MIN_ITEM_CAPACITY, MAX_ITEM_CAPACITY);
        size_t dataCapacity = provider.ConsumeIntegralInRange<size_t>(MIN_DATA_CAPACITY, MAX_DATA_CAPACITY);
        auto metadata = std::make_shared<OHOS::Camera::CameraMetadata>(itemCapacity, dataCapacity);
        g_cameraTrackingController->OnMetadataInfo(metadata);
    }
}

void FuzzOnReceiveEvent(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    std::string action = provider.ConsumeRandomLengthString(MAX_ACTION_STRING_LENGTH);
    CommonEventData eventData;
    eventData.SetCode(provider.ConsumeIntegral<int32_t>());
    MechBodyEventBaseService::GetInstance().OnReceiveEvent(action, eventData);
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzOnFocusTrackingInfo(data, size);
    FuzzOnCaptureSessionConfiged(data, size);
    FuzzOnZoomInfoChange(data, size);
    FuzzOnSessionStatusChange(data, size);
    FuzzOnMetadataInfo(data, size);
    FuzzOnReceiveEvent(data, size);
    return 0;
}