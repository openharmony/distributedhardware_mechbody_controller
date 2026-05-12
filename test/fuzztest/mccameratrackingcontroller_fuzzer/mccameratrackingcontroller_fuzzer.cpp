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

constexpr int32_t TEST_FUNCTION_GROUP1_END = static_cast<int32_t>(TestFunctionId::FUZZ_ON_CONNECT_CHANGE);
constexpr int32_t TEST_FUNCTION_GROUP2_END = static_cast<int32_t>(TestFunctionId::FUZZ_SEARCH_TARGET);
constexpr int32_t TEST_FUNCTION_MAX_ID = static_cast<int32_t>(TestFunctionId::FUZZ_UPDATE_CURRENT_CAMERA_INFO);

// Fuzz test constants
constexpr uint32_t MIN_INIT_COUNT = 1;
constexpr uint32_t MAX_INIT_COUNT = 3;
constexpr uint32_t MIN_GET_COUNT = 1;
constexpr uint32_t MAX_GET_COUNT = 5;
constexpr size_t MIN_ITEM_CAPACITY = 1;
constexpr size_t MAX_ITEM_CAPACITY = 100;
constexpr size_t MIN_DATA_CAPACITY = 1;
constexpr size_t MAX_DATA_CAPACITY = 1000;
constexpr int32_t MIN_LAYOUT_TYPE = 0;
constexpr int32_t MAX_LAYOUT_TYPE = 2;
constexpr int32_t MIN_TRACKING_EVENT = 0;
constexpr int32_t MAX_TRACKING_EVENT = 3;

McCameraTrackingController* g_cameraTrackingController = nullptr;

void InitCameraTrackingController(FuzzedDataProvider &provider)
{
    if (provider.ConsumeBool() && g_cameraTrackingController == nullptr) {
        g_cameraTrackingController = &McCameraTrackingController::GetInstance();
    }
}

void FuzzInit(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    uint32_t initCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_INIT_COUNT, MAX_INIT_COUNT);
    for (uint32_t i = 0; i < initCount; i++) {
        if (provider.ConsumeBool()) {
            g_cameraTrackingController->Init();
        }
    }
}

void FuzzUnInit(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    uint32_t uninitCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_INIT_COUNT, MAX_INIT_COUNT);
    for (uint32_t i = 0; i < uninitCount; i++) {
        if (provider.ConsumeBool()) {
            g_cameraTrackingController->UnInit();
        }
    }
}

void FuzzSetTrackingEnabled(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    bool isEnabled = provider.ConsumeBool();
    g_cameraTrackingController->SetTrackingEnabled(tokenId, isEnabled);
}

void FuzzGetTrackingEnabled(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    bool isEnabled = false;
    g_cameraTrackingController->GetTrackingEnabled(tokenId, isEnabled);
}

void FuzzSetTrackingLayout(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    CameraTrackingLayout layout = static_cast<CameraTrackingLayout>(
        provider.ConsumeIntegralInRange<int32_t>(MIN_LAYOUT_TYPE, MAX_LAYOUT_TYPE));
    g_cameraTrackingController->SetTrackingLayout(layout);
}

void FuzzGetTrackingLayout(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    CameraTrackingLayout layout;
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        if (provider.ConsumeBool()) {
            g_cameraTrackingController->GetTrackingLayout(layout);
        }
    }
}

void FuzzSetStickOffset(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    int16_t stickX = provider.ConsumeIntegral<int16_t>();
    int16_t stickY = provider.ConsumeIntegral<int16_t>();
    g_cameraTrackingController->SetStickOffset(stickX, stickY);
}

void FuzzOnConnectChange(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    uint32_t changeCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < changeCount; i++) {
        if (provider.ConsumeBool()) {
            g_cameraTrackingController->OnConnectChange();
        }
    }
}

void FuzzUserIdChangeCallback(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    uint32_t callbackCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < callbackCount; i++) {
        if (provider.ConsumeBool()) {
            g_cameraTrackingController->UserIdChangeCallback();
        }
    }
}

void FuzzGetTokenIdOfCurrentCameraInfo(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        if (provider.ConsumeBool()) {
            g_cameraTrackingController->GetTokenIdOfCurrentCameraInfo();
        }
    }
}

void FuzzSearchTargetRotateFinish(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    std::string cmdId = provider.ConsumeRandomLengthString(64);
    g_cameraTrackingController->SearchTargetRotateFinish(cmdId);
}

void FuzzSearchTargetStop(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    uint32_t stopCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < stopCount; i++) {
        if (provider.ConsumeBool()) {
            g_cameraTrackingController->SearchTargetStop();
        }
    }
}

void FuzzRegisterTrackingEventCallback(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    sptr<IRemoteObject> callback = nullptr;
    g_cameraTrackingController->RegisterTrackingEventCallback(tokenId, callback);
}

void FuzzUnRegisterTrackingEventCallback(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    g_cameraTrackingController->UnRegisterTrackingEventCallback(tokenId);
}

void FuzzSetTrackingLayoutWithToken(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    CameraTrackingLayout layout = static_cast<CameraTrackingLayout>(
        provider.ConsumeIntegralInRange<int32_t>(MIN_LAYOUT_TYPE, MAX_LAYOUT_TYPE));
    g_cameraTrackingController->SetTrackingLayout(tokenId, layout);
}

void FuzzSearchTarget(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    std::string napiCmdId = provider.ConsumeRandomLengthString(64);
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    auto targetInfo = std::make_shared<TargetInfo>();
    auto searchParams = std::make_shared<SearchParams>();
    g_cameraTrackingController->SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);
}

void FuzzOnSessionStatusChange(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    int32_t sessionid = provider.ConsumeIntegral<int32_t>();
    bool status = provider.ConsumeBool();
    g_cameraTrackingController->OnSessionStatusChange(sessionid, status);
}

void FuzzOnTrackingEvent(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    int32_t mechId = provider.ConsumeIntegral<int32_t>();
    TrackingEvent event = static_cast<TrackingEvent>(
        provider.ConsumeIntegralInRange<int32_t>(MIN_TRACKING_EVENT, MAX_TRACKING_EVENT));
    g_cameraTrackingController->OnTrackingEvent(mechId, event);
}

void FuzzUpdateActionControl(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    uint32_t updateCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < updateCount; i++) {
        if (provider.ConsumeBool()) {
            g_cameraTrackingController->UpdateActionControl();
        }
    }
}

void FuzzOnZoomInfoChange(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
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
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        if (provider.ConsumeBool()) {
            g_cameraTrackingController->GetCurrentCameraInfo();
        }
    }
}

void FuzzOnCaptureSessionConfiged(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    CameraStandard::CaptureSessionInfo captureSessionInfo;
    captureSessionInfo.sessionId = provider.ConsumeIntegral<int32_t>();
    g_cameraTrackingController->OnCaptureSessionConfiged(captureSessionInfo);
}

void FuzzOnMetadataInfo(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    // Use fuzz data to decide whether to pass nullptr
    bool passNull = provider.ConsumeBool();
    if (passNull) {
        g_cameraTrackingController->OnMetadataInfo(nullptr);
    } else {
        // Create a mock CameraMetadata object with required parameters
        size_t itemCapacity = provider.ConsumeIntegralInRange<size_t>(MIN_ITEM_CAPACITY, MAX_ITEM_CAPACITY);
        size_t dataCapacity = provider.ConsumeIntegralInRange<size_t>(MIN_DATA_CAPACITY, MAX_DATA_CAPACITY);
        auto metadata = std::make_shared<OHOS::Camera::CameraMetadata>(itemCapacity, dataCapacity);
        g_cameraTrackingController->OnMetadataInfo(metadata);
    }
}

void FuzzUpdateCurrentCameraInfo(FuzzedDataProvider &provider)
{
    InitCameraTrackingController(provider);
    if (!provider.ConsumeBool()) {
        return;
    }
    CameraStandard::CaptureSessionInfo captureSessionInfo;
    captureSessionInfo.sessionId = provider.ConsumeIntegral<int32_t>();
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
    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0, TEST_FUNCTION_MAX_ID);

    if (testFunctionId <= TEST_FUNCTION_GROUP1_END) {
        RunFuzzTestGroup1(provider, testFunctionId);
    } else if (testFunctionId <= TEST_FUNCTION_GROUP2_END) {
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