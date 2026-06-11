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
#include "mccameratrackingcontrollertwo_fuzzer.h"
#include "mc_camera_tracking_controller.h"
#include "mechbody_controller_service.h"
#include "mechbody_controller_types.h"
#include "mechbody_controller_enums.h"
#include <memory>
#include <iremote_object.h>

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {
constexpr size_t MIN_OBJECT_ID = 1;
constexpr size_t MAX_OBJECT_ID = 10;
constexpr int32_t MIN_TRACKING_MODE = 0;
constexpr int32_t MAX_TRACKING_MODE = 2;
constexpr int32_t MIN_CONFIDENCE = 0;
constexpr int32_t MAX_CONFIDENCE = 100;
constexpr int32_t MIN_TRACKING_EVENT = 0;
constexpr int32_t MAX_TRACKING_EVENT = 3;
constexpr uint32_t MIN_GET_COUNT = 1;
constexpr uint32_t MAX_GET_COUNT = 5;

McCameraTrackingController* g_cameraTrackingController = nullptr;

constexpr float MIN_RECT_COORD = 0.1f;
constexpr float MAX_RECT_COORD = 1.0f;

void InitCameraTrackingController()
{
    if (g_cameraTrackingController == nullptr) {
        g_cameraTrackingController = &McCameraTrackingController::GetInstance();
    }
}

CameraStandard::Rect GenerateRandomRect(FuzzedDataProvider &provider)
{
    CameraStandard::Rect rect;
    rect.topLeftX = provider.ConsumeFloatingPointInRange<float>(MIN_RECT_COORD, MAX_RECT_COORD);
    rect.topLeftY = provider.ConsumeFloatingPointInRange<float>(MIN_RECT_COORD, MAX_RECT_COORD);
    rect.width = provider.ConsumeFloatingPointInRange<float>(MIN_RECT_COORD, MAX_RECT_COORD);
    rect.height = provider.ConsumeFloatingPointInRange<float>(MIN_RECT_COORD, MAX_RECT_COORD);
    return rect;
}

void FuzzOnCaptureSessionConfiged(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    CameraStandard::CaptureSessionInfo captureSessionInfo;

    CameraStandard::OutputInfo videoOutputInfo;
    videoOutputInfo.type = CameraStandard::OutputType::VIDEO;
    videoOutputInfo.width = provider.ConsumeIntegral<int32_t>();
    videoOutputInfo.height = provider.ConsumeIntegral<int32_t>();
    captureSessionInfo.outputInfos.push_back(videoOutputInfo);

    CameraStandard::OutputInfo previewOutputInfo;
    previewOutputInfo.type = CameraStandard::OutputType::PREVIEW;
    previewOutputInfo.width = provider.ConsumeIntegral<int32_t>();
    previewOutputInfo.height = provider.ConsumeIntegral<int32_t>();

    captureSessionInfo.outputInfos.push_back(previewOutputInfo);
    int32_t dataRangeMin = 1;
    int32_t dataRangeMax = 6;
    captureSessionInfo.callerTokenId = provider.ConsumeIntegralInRange<int32_t>(dataRangeMin, dataRangeMax);
    captureSessionInfo.sessionMode = CameraStandard::SceneMode::CINEMATIC_VIDEO;
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

void FuzzOnFocusTrackingTypeHumanHead(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    CameraStandard::FocusTrackingMetaInfo info;
    int32_t trackingMode = provider.ConsumeIntegralInRange<int32_t>(MIN_TRACKING_MODE, MAX_TRACKING_MODE);
    info.SetTrackingMode(static_cast<CameraStandard::FocusTrackingMode>(trackingMode));
    info.SetTrackingObjectId(provider.ConsumeIntegralInRange<int32_t>(MIN_OBJECT_ID, MAX_OBJECT_ID));

    CameraStandard::Rect trackingRegion = GenerateRandomRect(provider);
    info.SetTrackingRegion(trackingRegion);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;
    CameraStandard::Rect bboxHumanHead = GenerateRandomRect(provider);
    CameraStandard::MetadataObjectType objectTypeHumanHead = CameraStandard::MetadataObjectType::HUMAN_HEAD;
    int64_t timestampHumanHead = provider.ConsumeIntegral<uint64_t>();
    int32_t objectIdHumanHead = provider.ConsumeIntegral<int32_t>();
    int32_t confidenceHumanHead = provider.ConsumeIntegralInRange<int32_t>(MIN_CONFIDENCE, MAX_CONFIDENCE);
    auto objHumanHead = new CameraStandard::MetadataObject(objectTypeHumanHead, timestampHumanHead, bboxHumanHead,
                                                           objectIdHumanHead, confidenceHumanHead);
    detectedObjects.push_back(objHumanHead);
    info.SetDetectedObjects(detectedObjects);
    g_cameraTrackingController->OnFocusTracking(info);
}

void FuzzOnFocusTrackingTypeHumanBody(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    CameraStandard::FocusTrackingMetaInfo info;
    int32_t trackingMode = provider.ConsumeIntegralInRange<int32_t>(MIN_TRACKING_MODE, MAX_TRACKING_MODE);
    info.SetTrackingMode(static_cast<CameraStandard::FocusTrackingMode>(trackingMode));
    info.SetTrackingObjectId(provider.ConsumeIntegralInRange<int32_t>(MIN_OBJECT_ID, MAX_OBJECT_ID));

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;
    CameraStandard::Rect trackingRegion = GenerateRandomRect(provider);
    info.SetTrackingRegion(trackingRegion);
    CameraStandard::Rect bboxHumanBody = GenerateRandomRect(provider);
    CameraStandard::MetadataObjectType objectTypeHumanBody = CameraStandard::MetadataObjectType::HUMAN_BODY;
    int64_t timestampHumanBody = provider.ConsumeIntegral<uint64_t>();
    int32_t objectIdHumanBody = provider.ConsumeIntegral<int32_t>();
    int32_t confidenceHumanBody = provider.ConsumeIntegralInRange<int32_t>(MIN_CONFIDENCE, MAX_CONFIDENCE);
    auto objHumanBody = new CameraStandard::MetadataObject(objectTypeHumanBody, timestampHumanBody, bboxHumanBody,
                                                           objectIdHumanBody, confidenceHumanBody);
    detectedObjects.push_back(objHumanBody);
    info.SetDetectedObjects(detectedObjects);
    g_cameraTrackingController->OnFocusTracking(info);
}

void FuzzOnFocusTrackingTypeFace(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    CameraStandard::FocusTrackingMetaInfo info;
    int32_t trackingMode = provider.ConsumeIntegralInRange<int32_t>(MIN_TRACKING_MODE, MAX_TRACKING_MODE);
    info.SetTrackingMode(static_cast<CameraStandard::FocusTrackingMode>(trackingMode));
    info.SetTrackingObjectId(provider.ConsumeIntegralInRange<int32_t>(MIN_OBJECT_ID, MAX_OBJECT_ID));

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;
    CameraStandard::Rect trackingRegion = GenerateRandomRect(provider);
    info.SetTrackingRegion(trackingRegion);
    CameraStandard::Rect bbox = GenerateRandomRect(provider);
    CameraStandard::MetadataObjectType objectType = CameraStandard::MetadataObjectType::FACE;
    int64_t timestamp = provider.ConsumeIntegral<uint64_t>();
    int32_t objectId = provider.ConsumeIntegral<int32_t>();
    int32_t confidence = provider.ConsumeIntegralInRange<int32_t>(MIN_CONFIDENCE, MAX_CONFIDENCE);
    auto obj = new CameraStandard::MetadataObject(objectType, timestamp, bbox, objectId, confidence);
    detectedObjects.push_back(obj);
    info.SetDetectedObjects(detectedObjects);
    g_cameraTrackingController->OnFocusTracking(info);
}

void FuzzOnSessionStatusChange(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    int32_t sessionid = provider.ConsumeIntegral<int32_t>();
    bool status = provider.ConsumeBool();
    g_cameraTrackingController->OnSessionStatusChange(sessionid, status);
}

class MockIRemoteObject : public IRemoteObject {
public:
    MockIRemoteObject() : IRemoteObject(u"mock_i_remote_object") {}
    virtual ~MockIRemoteObject() {}

    int32_t GetObjectRefCount() override { return 1; }
    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        return 0;
    }
    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override { return true; }
    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override { return true; }
    int Dump(int fd, const std::vector<std::u16string> &args) override { return 0; }
};

void FuzzOnTrackingEvent(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    int32_t mechId = provider.ConsumeIntegral<int32_t>();
    TrackingEvent event =
        static_cast<TrackingEvent>(provider.ConsumeIntegralInRange<int32_t>(MIN_TRACKING_EVENT, MAX_TRACKING_EVENT));
    g_cameraTrackingController->OnTrackingEvent(mechId, event);

    sptr<IRemoteObject> callback = new MockIRemoteObject();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    g_cameraTrackingController->RegisterTrackingEventCallback(tokenId, callback);
    g_cameraTrackingController->OnTrackingEvent(mechId, event);

    g_cameraTrackingController->UnRegisterTrackingEventCallback(tokenId);

    int32_t callbackCount = provider.ConsumeIntegralInRange<int32_t>(1, 3);
    std::vector<uint32_t> tokenIds;
    for (int32_t i = 0; i < callbackCount; i++) {
        sptr<IRemoteObject> callbackInFor = new MockIRemoteObject();
        uint32_t tokenIdInFor = provider.ConsumeIntegral<uint32_t>();
        tokenIds.push_back(tokenId);
        g_cameraTrackingController->RegisterTrackingEventCallback(tokenIdInFor, callbackInFor);
    }
    g_cameraTrackingController->OnTrackingEvent(mechId, event);

    for (auto tokenId : tokenIds) {
        g_cameraTrackingController->UnRegisterTrackingEventCallback(tokenId);
    }
}

void FuzzUpdateActionControl(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    uint32_t updateCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < updateCount; i++) {
        g_cameraTrackingController->UpdateActionControl();
    }
}

void FuzzGetCurrentCameraInfo(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        g_cameraTrackingController->GetCurrentCameraInfo();
    }
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzOnCaptureSessionConfiged(data, size);
    FuzzOnZoomInfoChange(data, size);
    FuzzOnFocusTrackingTypeHumanHead(data, size);
    FuzzOnFocusTrackingTypeHumanBody(data, size);
    FuzzOnFocusTrackingTypeFace(data, size);
    FuzzOnSessionStatusChange(data, size);
    FuzzOnTrackingEvent(data, size);
    FuzzUpdateActionControl(data, size);
    FuzzGetCurrentCameraInfo(data, size);
    return 0;
}