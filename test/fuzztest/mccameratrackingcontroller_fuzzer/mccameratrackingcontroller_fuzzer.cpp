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
#include "mccameratrackingcontroller_fuzzer.h"
#include "mc_camera_tracking_controller.h"
#include "mechbody_controller_service.h"
#include "mechbody_controller_types.h"
#include "mechbody_controller_enums.h"
#include <memory>
#include <iremote_object.h>

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

enum class TestFunctionId {
    FUZZ_INIT = 0,
    FUZZ_UNINIT = 1,
    FUZZ_SET_TRACKING_ENABLED = 2,
    FUZZ_GET_TRACKING_ENABLED = 3,
    FUZZ_SET_TRACKING_LAYOUT = 4,
    FUZZ_GET_TRACKING_LAYOUT = 5,
    FUZZ_SET_STICK_OFFSET = 6,
    FUZZ_ON_CONNECT_CHANGE = 7,
    FUZZ_USER_ID_CHANGE_CALLBACK = 8,
    FUZZ_GET_TOKEN_ID_OF_CURRENT_CAMERA_INFO = 9,
    FUZZ_SEARCH_TARGET_ROTATE_FINISH = 10,
    FUZZ_SEARCH_TARGET_STOP = 11,
    FUZZ_REGISTER_TRACKING_EVENT_CALLBACK = 12,
    FUZZ_UNREGISTER_TRACKING_EVENT_CALLBACK = 13,
    FUZZ_SET_TRACKING_LAYOUT_WITH_TOKEN = 14,
    FUZZ_SEARCH_TARGET = 15,
    FUZZ_ON_SESSION_STATUS_CHANGE = 16,
    FUZZ_ON_TRACKING_EVENT = 17,
    FUZZ_UPDATE_ACTION_CONTROL = 18,
    FUZZ_ON_ZOOM_INFO_CHANGE = 19,
    FUZZ_GET_CURRENT_CAMERA_INFO = 20,
    FUZZ_ON_CAPTURE_SESSION_CONFIGED = 21,
    FUZZ_ON_METADATA_INFO = 22,
    FUZZ_UPDATE_CURRENT_CAMERA_INFO = 23
};

McCameraTrackingController* g_cameraTrackingController = nullptr;

void InitCameraTrackingController()
{
    if (g_cameraTrackingController == nullptr) {
        g_cameraTrackingController = &McCameraTrackingController::GetInstance();
    }
}

void FuzzInit(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    g_cameraTrackingController->Init();
}

void FuzzUnInit(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    g_cameraTrackingController->UnInit();
}

void FuzzSetTrackingEnabled(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    bool isEnabled = provider.ConsumeBool();
    g_cameraTrackingController->SetTrackingEnabled(tokenId, isEnabled);
}

void FuzzGetTrackingEnabled(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    bool isEnabled = false;
    g_cameraTrackingController->GetTrackingEnabled(tokenId, isEnabled);
}

void FuzzSetTrackingLayout(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    CameraTrackingLayout layout = static_cast<CameraTrackingLayout>(
        provider.ConsumeIntegralInRange<int32_t>(0, 2));
    g_cameraTrackingController->SetTrackingLayout(layout);
}

void FuzzGetTrackingLayout(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    CameraTrackingLayout layout;
    g_cameraTrackingController->GetTrackingLayout(layout);
}

void FuzzSetStickOffset(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    int16_t stickX = provider.ConsumeIntegral<int16_t>();
    int16_t stickY = provider.ConsumeIntegral<int16_t>();
    g_cameraTrackingController->SetStickOffset(stickX, stickY);
}

void FuzzOnConnectChange(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    g_cameraTrackingController->OnConnectChange();
}

void FuzzUserIdChangeCallback(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    g_cameraTrackingController->UserIdChangeCallback();
}

void FuzzGetTokenIdOfCurrentCameraInfo(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    g_cameraTrackingController->GetTokenIdOfCurrentCameraInfo();
}

void FuzzSearchTargetRotateFinish(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    std::string cmdId = provider.ConsumeRandomLengthString(64);
    g_cameraTrackingController->SearchTargetRotateFinish(cmdId);
}

void FuzzSearchTargetStop(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    g_cameraTrackingController->SearchTargetStop();
}

void FuzzRegisterTrackingEventCallback(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    sptr<IRemoteObject> callback = nullptr;
    g_cameraTrackingController->RegisterTrackingEventCallback(tokenId, callback);
}

void FuzzUnRegisterTrackingEventCallback(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    g_cameraTrackingController->UnRegisterTrackingEventCallback(tokenId);
}

void FuzzSetTrackingLayoutWithToken(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    CameraTrackingLayout layout = static_cast<CameraTrackingLayout>(
        provider.ConsumeIntegralInRange<int32_t>(0, 2));
    g_cameraTrackingController->SetTrackingLayout(tokenId, layout);
}

void FuzzSearchTarget(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    std::string napiCmdId = provider.ConsumeRandomLengthString(64);
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    auto targetInfo = std::make_shared<TargetInfo>();
    auto searchParams = std::make_shared<SearchParams>();
    g_cameraTrackingController->SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);
}

void FuzzOnSessionStatusChange(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    int32_t sessionid = provider.ConsumeIntegral<int32_t>();
    bool status = provider.ConsumeBool();
    g_cameraTrackingController->OnSessionStatusChange(sessionid, status);
}

void FuzzOnTrackingEvent(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    int32_t mechId = provider.ConsumeIntegral<int32_t>();
    TrackingEvent event = static_cast<TrackingEvent>(
        provider.ConsumeIntegralInRange<int32_t>(0, 3));
    g_cameraTrackingController->OnTrackingEvent(mechId, event);
}

void FuzzUpdateActionControl(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    g_cameraTrackingController->UpdateActionControl();
}

void FuzzOnZoomInfoChange(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    int32_t sessionid = provider.ConsumeIntegral<int32_t>();
    CameraStandard::ZoomInfo zoomInfo;
    zoomInfo.zoomValue = provider.ConsumeFloatingPoint<float>();
    zoomInfo.equivalentFocus = provider.ConsumeIntegral<int32_t>();
    zoomInfo.videoStabilizationMode = provider.ConsumeIntegral<int32_t>();
    zoomInfo.focusMode = provider.ConsumeIntegral<int32_t>();
    g_cameraTrackingController->OnZoomInfoChange(sessionid, zoomInfo);
}

void FuzzGetCurrentCameraInfo(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    g_cameraTrackingController->GetCurrentCameraInfo();
}

void FuzzOnCaptureSessionConfiged(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    CameraStandard::CaptureSessionInfo captureSessionInfo;
    g_cameraTrackingController->OnCaptureSessionConfiged(captureSessionInfo);
}

void FuzzOnMetadataInfo(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    g_cameraTrackingController->OnMetadataInfo(nullptr);
}

void FuzzUpdateCurrentCameraInfo(FuzzedDataProvider &provider)
{
    InitCameraTrackingController();
    CameraStandard::CaptureSessionInfo captureSessionInfo;
    g_cameraTrackingController->UpdateCurrentCameraInfoByCaptureSessionInfo(captureSessionInfo);
}

void RunFuzzTestGroup1(FuzzedDataProvider &provider, int32_t testFunctionId)
{
    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_INIT:
            FuzzInit(provider);
            break;
        case TestFunctionId::FUZZ_UNINIT:
            FuzzUnInit(provider);
            break;
        case TestFunctionId::FUZZ_SET_TRACKING_ENABLED:
            FuzzSetTrackingEnabled(provider);
            break;
        case TestFunctionId::FUZZ_GET_TRACKING_ENABLED:
            FuzzGetTrackingEnabled(provider);
            break;
        case TestFunctionId::FUZZ_SET_TRACKING_LAYOUT:
            FuzzSetTrackingLayout(provider);
            break;
        case TestFunctionId::FUZZ_GET_TRACKING_LAYOUT:
            FuzzGetTrackingLayout(provider);
            break;
        case TestFunctionId::FUZZ_SET_STICK_OFFSET:
            FuzzSetStickOffset(provider);
            break;
        case TestFunctionId::FUZZ_ON_CONNECT_CHANGE:
            FuzzOnConnectChange(provider);
            break;
        default:
            break;
    }
}

void RunFuzzTestGroup2(FuzzedDataProvider &provider, int32_t testFunctionId)
{
    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_USER_ID_CHANGE_CALLBACK:
            FuzzUserIdChangeCallback(provider);
            break;
        case TestFunctionId::FUZZ_GET_TOKEN_ID_OF_CURRENT_CAMERA_INFO:
            FuzzGetTokenIdOfCurrentCameraInfo(provider);
            break;
        case TestFunctionId::FUZZ_SEARCH_TARGET_ROTATE_FINISH:
            FuzzSearchTargetRotateFinish(provider);
            break;
        case TestFunctionId::FUZZ_SEARCH_TARGET_STOP:
            FuzzSearchTargetStop(provider);
            break;
        case TestFunctionId::FUZZ_REGISTER_TRACKING_EVENT_CALLBACK:
            FuzzRegisterTrackingEventCallback(provider);
            break;
        case TestFunctionId::FUZZ_UNREGISTER_TRACKING_EVENT_CALLBACK:
            FuzzUnRegisterTrackingEventCallback(provider);
            break;
        case TestFunctionId::FUZZ_SET_TRACKING_LAYOUT_WITH_TOKEN:
            FuzzSetTrackingLayoutWithToken(provider);
            break;
        case TestFunctionId::FUZZ_SEARCH_TARGET:
            FuzzSearchTarget(provider);
            break;
        default:
            break;
    }
}

void RunFuzzTestGroup3(FuzzedDataProvider &provider, int32_t testFunctionId)
{
    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_ON_SESSION_STATUS_CHANGE:
            FuzzOnSessionStatusChange(provider);
            break;
        case TestFunctionId::FUZZ_ON_TRACKING_EVENT:
            FuzzOnTrackingEvent(provider);
            break;
        case TestFunctionId::FUZZ_UPDATE_ACTION_CONTROL:
            FuzzUpdateActionControl(provider);
            break;
        case TestFunctionId::FUZZ_ON_ZOOM_INFO_CHANGE:
            FuzzOnZoomInfoChange(provider);
            break;
        case TestFunctionId::FUZZ_GET_CURRENT_CAMERA_INFO:
            FuzzGetCurrentCameraInfo(provider);
            break;
        case TestFunctionId::FUZZ_ON_CAPTURE_SESSION_CONFIGED:
            FuzzOnCaptureSessionConfiged(provider);
            break;
        case TestFunctionId::FUZZ_ON_METADATA_INFO:
            FuzzOnMetadataInfo(provider);
            break;
        case TestFunctionId::FUZZ_UPDATE_CURRENT_CAMERA_INFO:
            FuzzUpdateCurrentCameraInfo(provider);
            break;
        default:
            break;
    }
}

void RunFuzzTest(FuzzedDataProvider &provider)
{
    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0, 23);

    if (testFunctionId < 8) {
        RunFuzzTestGroup1(provider, testFunctionId);
    } else if (testFunctionId < 16) {
        RunFuzzTestGroup2(provider, testFunctionId);
    } else {
        RunFuzzTestGroup3(provider, testFunctionId);
    }
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    RunFuzzTest(provider);
    return 0;
}