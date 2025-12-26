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

#ifndef MC_CAMERA_TRACKING_CONTROLLER_H
#define MC_CAMERA_TRACKING_CONTROLLER_H

#include <atomic>
#include <map>
#include "mechbody_controller_ipc_interface_code.h"
#include "mechbody_controller_log.h"
#include "mechbody_controller_types.h"
#include "mc_motion_manager.h"
#include "camera_manager.h"
#include "mech_session.h"
#include "istream_metadata.h"
#include "sensor_agent.h"
#include "sensor_agent_type.h"

namespace OHOS {
namespace MechBodyController {

enum class TrackingObjectType : uint8_t {
    MSG_OBJ_OTHER = 0,
    MSG_OBJ_HEAD = 1,
    MSG_OBJ_FACE = 2,
    MSG_OBJ_BODY = 3,
};

enum class CameraVideoStabilizationMode : int32_t {
    OHOS_CAMERA_VIDEO_STABILIZATION_OFF = 0,
    OHOS_CAMERA_VIDEO_STABILIZATION_LOW = 1,
    OHOS_CAMERA_VIDEO_STABILIZATION_MIDDLE = 2,
    OHOS_CAMERA_VIDEO_STABILIZATION_HIGH = 3,
    OHOS_CAMERA_VIDEO_STABILIZATION_AUTO = 4,
};

struct AppSetting {
    bool isTrackingEnabled = true;
};

struct CameraInfo {
    int32_t targetId;
    uint32_t tokenId = 0;
    float sensorWidth = 36.0f; // All devices have sensor width equivalent to 36mm
    float sensorHeight = 24.0f; // All devices have sensor height equivalent to 24mm
    uint8_t fovVBasic = 0;
    uint8_t fovHBasic = 0;
    uint8_t fovV = 0;
    uint8_t fovH = 0;
    int32_t width = 0;
    int32_t height = 0;
    float zoomFactor = 0;
    int32_t equivalentFocus = 24;
    bool isRecording = false;
    bool currentTrackingEnable = true;
    uint64_t trackingTargetNum = 0;
    bool searchingTarget = false;
    std::string searchTargetNapiCmdId = "";
    CameraTrackingLayout currentCameraTrackingLayout = CameraTrackingLayout::DEFAULT;
    CameraType cameraType = CameraType::BACK;
    int32_t focusMode = 1;
    int32_t sessionMode;
    int32_t videoStabilizationMode =
        static_cast<int32_t>(CameraVideoStabilizationMode::OHOS_CAMERA_VIDEO_STABILIZATION_OFF);
    bool isCameraOn = false;

    std::string ToString() const
    {
        return "CameraInfo { targetId=" + std::to_string(targetId) +
               ", sensorWidth=" + std::to_string(sensorWidth) +
               ", sensorHeight=" + std::to_string(sensorHeight) +
               ", fovV=" + std::to_string(fovV) +
               ", fovH=" + std::to_string(fovH) +
               ", fovVBasic=" + std::to_string(fovVBasic) +
               ", fovHBasic=" + std::to_string(fovHBasic) +
               ", width=" + std::to_string(width) +
               ", height=" + std::to_string(height) +
               ", zoomFactor=" + std::to_string(zoomFactor) +
               ", equivalentFocus=" + std::to_string(equivalentFocus) +
               ", isRecording=" + std::to_string(isRecording) +
               ", currentTrackingEnable=" + std::to_string(currentTrackingEnable) +
               ", trackingTargetNum=" + std::to_string(trackingTargetNum) +
               ", currentCameraTrackingLayout=" + std::to_string(static_cast<int>(currentCameraTrackingLayout)) +
               ", cameraType=" + std::to_string(static_cast<int>(cameraType)) +
               ", focusMode=" + std::to_string(focusMode) +
               ", sessionMode=" + std::to_string(sessionMode) +
               ", videoStabilizationMode=" + std::to_string(videoStabilizationMode) +
               ", isCameraOn=" + std::to_string(isCameraOn) + " }";
    }
};


class MechSessionCallbackImpl : public CameraStandard::MechSessionCallback {
public:
    ~MechSessionCallbackImpl() override = default;

    void OnFocusTrackingInfo(CameraStandard::FocusTrackingMetaInfo info) override;
    void OnCaptureSessionConfiged(CameraStandard::CaptureSessionInfo captureSessionInfo) override;
    void OnZoomInfoChange(int sessionid, CameraStandard::ZoomInfo zoomInfo) override;
    void OnSessionStatusChange(int sessionid, bool status) override;
};

class McCameraTrackingController {
public:
    static McCameraTrackingController& GetInstance();
private:
    McCameraTrackingController(const McCameraTrackingController&) = delete;
    McCameraTrackingController& operator= (const McCameraTrackingController&) = delete;
    McCameraTrackingController(McCameraTrackingController&&) = delete;
    McCameraTrackingController& operator= (McCameraTrackingController&&) = delete;

public:
    McCameraTrackingController();
    ~McCameraTrackingController();
    void Init();
    void UnInit();
    int32_t OnCaptureSessionConfiged(const CameraStandard::CaptureSessionInfo& captureSessionInfo);
    void UpdateCurrentCameraInfoByCaptureSessionInfo(
        const CameraStandard::CaptureSessionInfo& captureSessionInfo);
    int32_t OnZoomInfoChange(int32_t sessionid, const CameraStandard::ZoomInfo& zoomInfo);
    int32_t OnSessionStatusChange(int32_t sessionid, bool status);
    int32_t OnFocusTracking(CameraStandard::FocusTrackingMetaInfo &info);
    int32_t SetTrackingEnabled(const uint32_t &tokenId, bool isEnabled);
    int32_t GetTrackingEnabled(const uint32_t &tokenId, bool &isEnabled);
    int32_t RegisterTrackingEventCallback(const uint32_t &tokenId, sptr<IRemoteObject> callback);
    int32_t UnRegisterTrackingEventCallback(const uint32_t &tokenId);
    int32_t OnTrackingEvent(const int32_t &mechId, const TrackingEvent &event);
    int32_t SetTrackingLayout(CameraTrackingLayout &cameraTrackingLayout);

    int32_t SetTrackingLayout(const uint32_t &tokenId, CameraTrackingLayout &cameraTrackingLayout);
    int32_t SearchTarget(std::string &napiCmdId, uint32_t &tokenId,
        const std::shared_ptr<TargetInfo> &targetInfo, const std::shared_ptr<SearchParams> &searchParams);
    void SearchTargetRotateFinish(const std::string &cmdId);
    void SearchTargetStop();
    int32_t GetTrackingLayout(CameraTrackingLayout &cameraTrackingLayout);
    std::shared_ptr<CameraInfo> GetCurrentCameraInfo() const;
    void OnConnectChange();
    int32_t UpdateActionControl();
    int32_t SetStickOffset(const int16_t &stickX, const int16_t &stickY);
    uint32_t GetTokenIdOfCurrentCameraInfo();

private:
    int32_t ComputeFov();
    void SensorCutForVideoMode(float &sensorWidthCut, float &sensorHighCut);
    bool IsCurrentFocus();
    int32_t DoPrediction(std::shared_ptr<TrackingFrameParams> &trackingParams, int32_t &objectId);
    int32_t UpdateMotionManagers();
    std::shared_ptr<TrackingFrameParams> BuildTrackingParams(CameraStandard::FocusTrackingMetaInfo &info);
    int32_t GetTrackingTarget(CameraStandard::Rect &trackingRegion,
        std::vector<sptr<CameraStandard::MetadataObject>> &detectedObjects, int32_t trackingObjectId,
        sptr<CameraStandard::MetadataObject> &targetObject);
    int32_t UpdateMotionsWithTrackingData(
        const std::shared_ptr<TrackingFrameParams> &params, int32_t trackingObjectId);
    bool FilterDetectedObject(sptr<CameraStandard::MetadataObject> &detectedObject);
    void UpdateROI(std::shared_ptr<TrackingFrameParams> &trackingFrameParams, CameraStandard::Rect &rect);
    static void SensorCallback(SensorEvent* event);
    static MobileRotation CalculateSensorRotation(GravityData* gravityData);
    void ConvertObjectType(CameraStandard::MetadataObjectType &cameraObjectType, uint8_t &mechObjectType);
    void RegisterTrackingListener();
    void UnRegisterTrackingListener();
    void RegisterSensorListener();
    void UnRegisterSensorListener();
    float ParseLayout(CameraTrackingLayout &cameraTrackingLayout);
    float ParseReverseLayout(CameraTrackingLayout &cameraTrackingLayout);
    void ExecSearchTaskWithLimit(std::string &napiCmdId, uint32_t &tokenId,
        const bool &startFromNeg, const RotateDegreeLimit &limit);
    void ExecSearchTask(std::string &napiCmdId, uint32_t &tokenId, const bool &startFromNeg,
        int32_t &searchTimes);
    void RunSearchTarget(std::string &napiCmdId, uint32_t &tokenId, const std::shared_ptr<SearchParams> &searchParams,
        const RotateDegreeLimit &limit, const std::shared_ptr<EulerAngles> &currentPosition);
    void AdjustROI(ROI &roi, CameraStandard::Rect &rect, CameraType cameraType, MobileRotation sensorRotation);
    void AdjustOffset(std::shared_ptr<TrackingFrameParams> &trackingParams, CameraType cameraType);
    void AdjustYOffset(ROI &roi, CameraType cameraType, float &offsetX, float &offsetY, bool &isFace);
    void AdjustXOffset(ROI &roi, CameraType cameraType, float &offsetX, float &offsetY, bool &isFace);
    bool IsCurrentTrackingEnabled();

public:
    std::mutex trackingEventCallbackMutex_;
    std::map<uint32_t, sptr<IRemoteObject>> trackingEventCallback_;

private:
    std::mutex cameraTrackingControllerInitMutex_;
    std::mutex appSettingsMutex_;
    std::map<uint32_t, std::shared_ptr<AppSetting>> appSettings;

    SensorUser user = { "", nullptr, nullptr };
    MobileRotation sensorRotation_ = MobileRotation::UP;
    sptr<CameraStandard::MechSession> pMechSession;
    std::shared_ptr<CameraInfo> currentCameraInfo_ = std::make_shared<CameraInfo>();

    std::shared_ptr<TrackingFrameParams> lastTrackingFrame_ = std::make_shared<TrackingFrameParams>();

    std::shared_ptr<OHOS::AppExecFwk::EventHandler> eventHandler_ = nullptr;

    std::mutex searchTargetMutex_;
    std::condition_variable searchTargetCv_;

    float vertical_ = 0.0f;
    float horizontal_ = 0.0f;
};
} // namespace MechBodyController
} // namespace OHOS
#endif // MC_CAMERA_TRACKING_CONTROLLER_H