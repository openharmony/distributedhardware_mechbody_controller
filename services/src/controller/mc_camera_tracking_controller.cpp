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

#include <cmath>
#include <memory>
#include <valarray>
#include "load_mechbody_adapter.h"
#include "mc_camera_tracking_controller.h"
#include "mechbody_controller_service.h"
#include "mc_controller_ipc_death_listener.h"
#include "os_account_manager.h"
#include "capture_scene_const.h"
#include "mechbody_controller_utils.h"
#include "hisysevent_utils.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "McCameraTrackingController";

    constexpr float DOUBLE = 2.0f;
    constexpr int32_t NUM_1 = 1;
    constexpr int32_t NUM_2 = 2;

    constexpr int VALID_INCLINATION_ANGLE_THRESHOLD_COEFFICIENT = 3;
    constexpr int32_t POSTURE_INTERVAL = 100000000; // 100ms
    constexpr int32_t MOBILE_ROTATION_CHECK_UP_BEGIN = 330;
    constexpr int32_t MOBILE_ROTATION_CHECK_UP_END = 30;
    constexpr int32_t MOBILE_ROTATION_CHECK_RIGHT_BEGIN = 60;
    constexpr int32_t MOBILE_ROTATION_CHECK_RIGHT_END = 120;
    constexpr int32_t MOBILE_ROTATION_CHECK_DOWN_BEGIN = 150;
    constexpr int32_t MOBILE_ROTATION_CHECK_DOWN_END = 210;
    constexpr int32_t MOBILE_ROTATION_CHECK_LEFT_BEGIN = 240;
    constexpr int32_t MOBILE_ROTATION_CHECK_LEFT_END = 300;

    constexpr int32_t INVALID_USER_ID = -1;

    constexpr int32_t REAR_MOUNTED_LENS  = 1;

    constexpr float LAYOUT_MIDDLE = 0.5f;

    constexpr int32_t DEGREE_CONSTANT_90 = 90;
    constexpr int32_t DEGREE_CONSTANT_180 = 180;
    constexpr int32_t DEGREE_CONSTANT_360 = 360;
    
    constexpr int32_t TRACKING_STATE_TIME = 500;    // ms

    constexpr float TRACKING_LOST_CHECK = 0.0001f;
    constexpr float OFFSET_VALUE_BASE = 0.02f;
    constexpr float OFFSET_VALUE_BASE_1 = 0.01f;
    constexpr float OFFSET_VALUE_MAX = 0.3f;
    constexpr float OFFSET_VALUE_MIN = -0.3f;
    constexpr float MIDDLE_VALUE_HORIZONTAL = 0.5f;
    constexpr float MIDDLE_MAX = 0.55f;
    constexpr float MIDDLE_MIN = 0.45f;
    constexpr float FACE_OFFSET = 0.1f;
    constexpr int16_t STICK_BASE = 300;
    constexpr float UP_LIMIT = 1.2f;
    constexpr float DOWN_LIMIT = 0.2f;
    constexpr float UP_HORIZONTAL = 0.2f;
    constexpr float DOWN_HORIZONTAL = -0.2f;
    static const std::set<int32_t> FOCUS_MODE_WHITELIST = { 1 };
    static const std::set<int32_t> SESSION_MODE_WHITELIST = {
        static_cast<int32_t>(CameraStandard::SceneMode::NORMAL),    // 0
        static_cast<int32_t>(CameraStandard::SceneMode::CAPTURE),   // 1
        static_cast<int32_t>(CameraStandard::SceneMode::VIDEO),     // 2
        static_cast<int32_t>(CameraStandard::SceneMode::PORTRAIT),  // 3
    };

    const std::string SEND_CAMERA_INFO_TASK_NAME = "sendCameraInfoTaskName";
    constexpr int32_t SEND_CAMERA_INFO_TASK_DELAY = 500;

    constexpr float OHOS_CAMERA_VIDEO_STABILIZATION_HIGH_CUT = 0.6f;
    constexpr float OHOS_CAMERA_VIDEO_STABILIZATION_AUTO_CUT = 0.8f;
    static const std::set<int32_t> VIDEO_STABILIZATION_WHITELIST = {
        static_cast<int32_t>(CameraStandard::SceneMode::VIDEO),     // 2
    };

    const std::string SEARCH_TARGET_TASK_NAME = "searchTargetTaskName";
    constexpr float ROTATE_LIMITED_MAX = 3.14f;
    constexpr float ROTATE_LIMITED_MIN = -3.14f;
    constexpr float PI = 3.1415926f;
    constexpr int32_t ROTATE_TO_START_TIME_USED = 500; // ms
    constexpr int32_t SEARCH_TARGET_ROTATE_TO_START_WAIT_TME = 1; // s
    constexpr int32_t SEARCH_TARGET_TASK_DURATION = 500; // ms
    constexpr int32_t SEARCH_TARGET_TASK_INTERVAL = 200; // ms
    constexpr float SEARCH_TARGET_ROTATE_SPEED = 3.14 / 4; // rad/s
    constexpr float SEARCH_TARGET_ROTATE_TIMES =
            6.28 / (SEARCH_TARGET_ROTATE_SPEED * SEARCH_TARGET_TASK_DURATION / 1000 /
            SEARCH_TARGET_TASK_DURATION * SEARCH_TARGET_TASK_INTERVAL);

    const std::string UPDATE_ACTION_CONTROL_TASK_NAME = "update_action_control_task";
    constexpr int32_t UPDATE_ACTION_CONTROL_TASK_DELAY  = 500 * 3;
    constexpr float OFFSET_VALUE = 0.2f;
    constexpr int32_t ACTION_CONTROL_TIMEOUT = 2000;
    constexpr int32_t TRACKING_PARAM_LOST_DELAY = 200; // ms

    constexpr int32_t MAX_PAUSED_FRAME = 10;

    constexpr float IOU_THRESHOLD = 0.1f;
    constexpr float IOU_DEFAULT = 0.0f;
    constexpr int32_t META_DATA_SIZE = 2;
#ifdef MECHBODY_CONTROLLER_EXTENDED
    const std::string PREDICTION_TASK_NAME = "PredictionTaskName";
    constexpr int32_t PREDICTION_TASK_DELAY = 100; // ms
    int32_t g_predictionIgnoredFrame = 0;
    constexpr int32_t PREDICTION_IGNORED_FRAME_MAX = 8;
#endif
}

McCameraTrackingController& McCameraTrackingController::GetInstance()
{
    static auto instance = new McCameraTrackingController();
    return *instance;
}

McCameraTrackingController::McCameraTrackingController()
{
    auto runner = AppExecFwk::EventRunner::Create("McCameraTrackingController");
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
}

void McCameraTrackingController::Init()
{
    HILOGI("start");
    std::lock_guard<std::mutex> lock(cameraTrackingControllerInitMutex_);
    RegisterTrackingListener();
    RegisterSensorListener();
#ifdef MECHBODY_CONTROLLER_EXTENDED
    MechbodyAdapterUtils::InitTrackingCore();
#endif
    HILOGI("end");
}

void McCameraTrackingController::UnInit()
{
    HILOGI("start");
    std::lock_guard<std::mutex> lock(cameraTrackingControllerInitMutex_);
    UnRegisterTrackingListener();
    UnRegisterSensorListener();
    if (eventHandler_ != nullptr) {
        eventHandler_->RemoveTask(SEND_CAMERA_INFO_TASK_NAME);
        eventHandler_->RemoveTask(UPDATE_ACTION_CONTROL_TASK_NAME);
        #ifdef MECHBODY_CONTROLLER_EXTENDED
            eventHandler_->RemoveTask(PREDICTION_TASK_NAME);
        #endif
    }
#ifdef MECHBODY_CONTROLLER_EXTENDED
    MechbodyAdapterUtils::Clear();
#endif
    horizontal_ = 0.0f;
    vertical_ = 0.0f;
    HILOGI("end");
}

uint32_t McCameraTrackingController::GetTokenIdOfCurrentCameraInfo()
{
    if (currentCameraInfo_ == nullptr) {
        HILOGW("currentCameraInfo_ is nullptr; ");
        return 0;
    }
    return currentCameraInfo_->tokenId;
}

void McCameraTrackingController::UpdateCurrentCameraInfoByCaptureSessionInfo(
    const CameraStandard::CaptureSessionInfo& captureSessionInfo)
{
    if (currentCameraInfo_ == nullptr) {
        currentCameraInfo_ = std::make_shared<CameraInfo>();
    }
    currentCameraInfo_->tokenId = captureSessionInfo.callerTokenId;
    HILOGI("The app that is using the camera changes, app token id: %{public}s; ",
           GetAnonymUint32(currentCameraInfo_->tokenId).c_str());
    for (const auto &item: captureSessionInfo.outputInfos) {
        if (item.type == CameraStandard::OutputType::VIDEO ||
            item.type == CameraStandard::OutputType::PREVIEW) {
            currentCameraInfo_->width = item.width;
            currentCameraInfo_->height = item.height;
            break;
        }
    }
    if (currentCameraInfo_->cameraType != (captureSessionInfo.position == REAR_MOUNTED_LENS
        ? CameraType::BACK : CameraType::FRONT)) {
        HILOGI("Back or Rront change.");
#ifdef MECHBODY_CONTROLLER_EXTENDED
        MechbodyAdapterUtils::ResetTrackingCore();
#endif
        horizontal_ = 0.0f;
        vertical_ = 0.0f;
        targetVertical_ = 0.0f;
        targetHorizontal_ = 0.0f;
    }
    currentCameraInfo_->sessionMode = captureSessionInfo.sessionMode;
    currentCameraInfo_->isRecording =
        captureSessionInfo.sessionMode == static_cast<int32_t>(CameraStandard::SceneMode::VIDEO);
    currentCameraInfo_->cameraType =
        captureSessionInfo.position == REAR_MOUNTED_LENS ? CameraType::BACK : CameraType::FRONT;
    currentCameraInfo_->zoomFactor = captureSessionInfo.zoomInfo.zoomValue;
    currentCameraInfo_->equivalentFocus = captureSessionInfo.zoomInfo.equivalentFocus;
    currentCameraInfo_->videoStabilizationMode = captureSessionInfo.zoomInfo.videoStabilizationMode;
}

int32_t McCameraTrackingController::OnMetadataInfo(const std::shared_ptr<OHOS::Camera::CameraMetadata>& result)
{
    if (result == nullptr) {
        HILOGE("%{public}s result nullptr", __FUNCTION__);
        return METADATA_INFO_IS_EMPTY;
    }
    camera_metadata_item_t item;
    int ret = Camera::FindCameraMetadataItem(result->get(), OHOS_STATUS_FOV_INFOS, &item);
    if (ret == CAM_META_SUCCESS && item.count == META_DATA_SIZE) {
        currentCameraInfo_->fovH = static_cast<uint8_t>(item.data.f[0]);
        currentCameraInfo_->fovV = static_cast<uint8_t>(item.data.f[1]);
        HILOGI("currentCameraInfo Get FOV: [%{public}f, %{public}f]",
            static_cast<double>(currentCameraInfo_->fovH),
            static_cast<double>(currentCameraInfo_->fovV));
        return ERR_OK;
    } else {
        HILOGI("Find Tag OHOS_STATUS_FOV_INFOS failed, ret: %{public}d, count: %{public}d",
            ret, item.count);
        return GET_FOV_INFO_TAG_FAILED;
    }
}

int32_t McCameraTrackingController::OnCaptureSessionConfiged(
    const CameraStandard::CaptureSessionInfo& captureSessionInfo)
{
    HILOGI("start");
    UpdateCurrentCameraInfoByCaptureSessionInfo(captureSessionInfo);
    ComputeFov();
    if (currentCameraInfo_->tokenId != 0) {
        UpdateMotionManagers();
        bool isTrackingEnabledFlag = false;
        if (!appSettings.empty() && appSettings.find(currentCameraInfo_->tokenId) != appSettings.end()) {
            HILOGI("The application settings trackingEnable is not empty.");
            isTrackingEnabledFlag = appSettings[currentCameraInfo_->tokenId]->isTrackingEnabled;
            SetTrackingEnabled(currentCameraInfo_->tokenId,
                isTrackingEnabledFlag);
        } else {
            isTrackingEnabledFlag = true;
            SetTrackingEnabled(currentCameraInfo_->tokenId, isTrackingEnabledFlag);
        }
        UpdateActionControl();
        if (isTrackingEnabledFlag) {
            // only set app enable flag
            // need query device enable flag
            MechBodyControllerService::JudgeDeviceEnableSwitchAndReportFocustrackingStartEvent(
                currentCameraInfo_->tokenId);
        }
    }
    HILOGI("end");
    return ERR_OK;
}

int32_t McCameraTrackingController::OnZoomInfoChange(int32_t sessionid, const CameraStandard::ZoomInfo& zoomInfo)
{
    HILOGI("start");
    if (currentCameraInfo_ == nullptr) {
        HILOGW("currentCameraInfo_ is nullptr; ");
        return CAMERA_INFO_IS_EMPTY;
    }
    currentCameraInfo_->zoomFactor = zoomInfo.zoomValue;
    currentCameraInfo_->equivalentFocus = zoomInfo.equivalentFocus;
    currentCameraInfo_->videoStabilizationMode = zoomInfo.videoStabilizationMode;
    if (zoomInfo.focusMode >= 0) {
        currentCameraInfo_->focusMode = zoomInfo.focusMode;
    }
    if (currentCameraInfo_->tokenId != 0 && ComputeFov() == ERR_OK) {
        UpdateMotionManagers();
        UpdateActionControl();
    }
    HILOGI("end");
    return ERR_OK;
}

int32_t McCameraTrackingController::ComputeFov()
{
    if (currentCameraInfo_ == nullptr) {
        HILOGW("currentCameraInfo_ is nullptr; ");
        return CAMERA_INFO_IS_EMPTY;
    }
    float sensorWidthCut = 0.0f;
    float sensorHighCut = 0.0f;
    SensorCutForVideoMode(sensorWidthCut, sensorHighCut);
    auto focus = static_cast<float >(currentCameraInfo_->equivalentFocus);
    float fovA = DOUBLE *
        std::round(RadToDegree(std::atan(sensorWidthCut / (DOUBLE * focus * currentCameraInfo_->zoomFactor))));
    float fovABasic = DOUBLE * std::round(RadToDegree(std::atan(sensorWidthCut / (DOUBLE * focus))));
    float fovB = DOUBLE *
        std::round(RadToDegree(std::atan(sensorHighCut / (DOUBLE * focus * currentCameraInfo_->zoomFactor))));
    float fovBBasic = DOUBLE * std::round(RadToDegree(std::atan(sensorHighCut / (DOUBLE * focus))));
    float shortSide = (fovA < fovB) ? fovA : fovB;
    float shortSideBasic = (fovA < fovB) ? fovABasic : fovBBasic;
    float longSide = (fovA < fovB) ? fovB : fovA;
    float longSideBasic = (fovA < fovB) ? fovBBasic : fovABasic;
    if (currentCameraInfo_->fovH == 0 && currentCameraInfo_->fovV == 0) {
        if (sensorRotation_ == MobileRotation::UP || sensorRotation_ == MobileRotation::DOWN) {
            currentCameraInfo_->fovH = static_cast<uint8_t>(shortSide);
            currentCameraInfo_->fovV = static_cast<uint8_t>(longSide);
        } else {
            currentCameraInfo_->fovH = static_cast<uint8_t>(longSide);
            currentCameraInfo_->fovV = static_cast<uint8_t>(shortSide);
        }
    }
    currentCameraInfo_->fovHBasic = static_cast<uint8_t>(shortSideBasic);
    currentCameraInfo_->fovVBasic = static_cast<uint8_t>(longSideBasic);
    HILOGI("current camera info: %{public}s", currentCameraInfo_->ToString().c_str());
    return ERR_OK;
}

void McCameraTrackingController::SensorCutForVideoMode(float &sensorWidthCut, float &sensorHighCut)
{
    float sensorRatio = currentCameraInfo_->sensorHeight != 0 ?
        currentCameraInfo_->sensorWidth / currentCameraInfo_->sensorHeight : 0;
    float viewRatio = currentCameraInfo_->height > 0 ?
        static_cast<float>(currentCameraInfo_->width) / static_cast<float>(currentCameraInfo_->height) : 0;

    if (sensorRatio > viewRatio) {
        sensorHighCut = currentCameraInfo_->sensorHeight;
        sensorWidthCut = currentCameraInfo_->sensorHeight * viewRatio;
    } else {
        sensorWidthCut = currentCameraInfo_->sensorWidth;
        sensorHighCut = currentCameraInfo_->sensorWidth / (viewRatio > 0 ? viewRatio : 1);
    }
    if (VIDEO_STABILIZATION_WHITELIST.find(currentCameraInfo_->sessionMode) != VIDEO_STABILIZATION_WHITELIST.end()) {
        if (currentCameraInfo_->videoStabilizationMode ==
            static_cast<int32_t>(CameraVideoStabilizationMode::OHOS_CAMERA_VIDEO_STABILIZATION_AUTO)) {
            sensorWidthCut *= OHOS_CAMERA_VIDEO_STABILIZATION_AUTO_CUT;
            sensorHighCut *= OHOS_CAMERA_VIDEO_STABILIZATION_AUTO_CUT;
        }
        if (currentCameraInfo_->videoStabilizationMode ==
            static_cast<int32_t>(CameraVideoStabilizationMode::OHOS_CAMERA_VIDEO_STABILIZATION_HIGH)) {
            sensorWidthCut *= OHOS_CAMERA_VIDEO_STABILIZATION_HIGH_CUT;
            sensorHighCut *= OHOS_CAMERA_VIDEO_STABILIZATION_HIGH_CUT;
        }
    }
}

int32_t McCameraTrackingController::OnSessionStatusChange(int32_t sessionid, bool status)
{
    HILOGI("start, status: %{public}s", status ? "true" : "false");
    if (currentCameraInfo_ == nullptr) {
        HILOGW("currentCameraInfo_ is nullptr; ");
        return CAMERA_INFO_IS_EMPTY;
    }
    currentCameraInfo_->isCameraOn = status;
    if (!status) {
        currentCameraInfo_->trackingTargetNum = 0;
#ifdef MECHBODY_CONTROLLER_EXTENDED
        MechbodyAdapterUtils::ResetTrackingCore();
#endif
    }
    UpdateActionControl();
    HILOGI("end");
    return ERR_OK;
}

bool McCameraTrackingController::IsCurrentFocus()
{
    HILOGI("Current focus is : %{public}d.", currentCameraInfo_->focusMode);
    return FOCUS_MODE_WHITELIST.find(currentCameraInfo_->focusMode) == FOCUS_MODE_WHITELIST.end();
}

int32_t McCameraTrackingController::UpdateMotionManagers()
{
    HILOGI("start");
    if (eventHandler_ != nullptr) {
        eventHandler_->RemoveTask(SEND_CAMERA_INFO_TASK_NAME);
        eventHandler_->PostTask([]() {
                McCameraTrackingController::GetInstance().UpdateMotionManagers();
            }, SEND_CAMERA_INFO_TASK_NAME, SEND_CAMERA_INFO_TASK_DELAY);
    }
    const auto& motionManagers = MechBodyControllerService::GetInstance().motionManagers_;
    CameraInfoParams cameraInfoParams;
    cameraInfoParams.fovH = currentCameraInfo_->fovHBasic;
    cameraInfoParams.fovV = currentCameraInfo_->fovVBasic;
    cameraInfoParams.zoomFactor = currentCameraInfo_->zoomFactor;
    cameraInfoParams.isRecording = currentCameraInfo_->isRecording;
    cameraInfoParams.cameraType = currentCameraInfo_->cameraType;

    std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
    for (const auto &item : motionManagers) {
        int32_t mechId = item.first;
        std::shared_ptr<MotionManager> motionManager = item.second;
        if (!motionManager) {
            HILOGE("MotionManager not exits; mechId: %{public}d", mechId);
            continue;
        }

        int32_t result = motionManager->SetMechCameraInfo(cameraInfoParams);
        HILOGI("mech id: %{public}d result code: %{public}d", mechId, result);
        if (result != ERR_OK) {
            return result;
        }
    }
    HILOGI("end");
    return ERR_OK;
}

int32_t McCameraTrackingController::OnFocusTracking(CameraStandard::FocusTrackingMetaInfo &info)
{
    HILOGI("Tracking mode: %{public}d; Tracking region: [x: %{public}f, y: %{public}f, width: %{public}f,"
           " height: %{public}f];  Tracking object ID: %{public}d",
           info.GetTrackingMode(), info.GetTrackingRegion().topLeftX, info.GetTrackingRegion().topLeftY,
           info.GetTrackingRegion().width, info.GetTrackingRegion().height, info.GetTrackingObjectId());

    std::ostringstream oss;
    for (const auto& obj : info.GetDetectedObjects()) {
        oss << "Detected object ID: " << obj->GetObjectId() << ", Type: " << static_cast<int>(obj->GetType())
            << ", BBox: [" << obj->GetBoundingBox().topLeftX << ", " << obj->GetBoundingBox().topLeftY << ", "
            << obj->GetBoundingBox().width << ", " << obj->GetBoundingBox().height << "]" << ", Confidence: "
            << obj->GetConfidence();
    }
    HILOGI("tracking object info: %{public}s", oss.str().c_str());

    std::shared_ptr<TrackingFrameParams> trackingParams = BuildTrackingParams(info);
    if (!trackingParams) {
        HILOGE("Failed to build tracking params");
        return ERR_OK;
    }
    trackingParams->cameraType = currentCameraInfo_->cameraType;
    HILOGI("tracking param: %{public}s", trackingParams->ToString().c_str());
#ifdef MECHBODY_CONTROLLER_EXTENDED
    int32_t objectId = info.GetTrackingObjectId();
    if (trackingParams->roi.x > 0 || trackingParams->roi.y > 0) {
        g_predictionIgnoredFrame = 0;
        HILOGI("DoPrediction now with invalid data.");
        return DoPrediction(trackingParams, objectId);
    } else {
        if (eventHandler_ != nullptr) {
            eventHandler_->RemoveTask(PREDICTION_TASK_NAME);
            g_predictionIgnoredFrame ++;
            if (g_predictionIgnoredFrame > PREDICTION_IGNORED_FRAME_MAX) {
                HILOGI("DoPrediction now with invalid data.");
                return DoPrediction(trackingParams, objectId);
            }
            HILOGI("DoPrediction now with invalid data delay.");
            eventHandler_->PostTask([this, trackingParams, objectId] () mutable {
                    DoPrediction(trackingParams, objectId);
                }, PREDICTION_TASK_NAME, PREDICTION_TASK_DELAY);
        }
        return ERR_OK;
    }
#else
    if (trackingParams->roi.width <= TRACKING_LOST_CHECK || trackingParams->roi.height <= TRACKING_LOST_CHECK) {
        HILOGW("tracking rect lost, width or height is zero.");
        return ERR_OK;
    }
    AdjustOffset(trackingParams, currentCameraInfo_->cameraType);
    return UpdateMotionsWithTrackingData(trackingParams, info.GetTrackingObjectId());
#endif
}

int32_t McCameraTrackingController::DoPrediction(std::shared_ptr<TrackingFrameParams> &trackingParams,
    int32_t &objectId)
{
#ifdef MECHBODY_CONTROLLER_EXTENDED
    HILOGI("Start RunTrackingCore.");
    int32_t res = MechbodyAdapterUtils::RunTrackingCore(trackingParams->roi.x, trackingParams->roi.y,
        trackingParams->roi.width, trackingParams->roi.height,
        [this, trackingParams, objectId](float resultX, float resultY) mutable {
            trackingParams->roi.x = resultX;
            trackingParams->roi.y = resultY;
            HILOGI("tracking param after: %{public}s", trackingParams->ToString().c_str());
            if (trackingParams->roi.width > TRACKING_LOST_CHECK || trackingParams->roi.height > TRACKING_LOST_CHECK) {
                AdjustOffset(trackingParams, this->currentCameraInfo_->cameraType);
                HILOGI("tracking param offset: %{public}s", trackingParams->ToString().c_str());
            }
            UpdateMotionsWithTrackingData(trackingParams, objectId);
        });
    return res;
#else
    return ERR_OK;
#endif
}

std::shared_ptr<TrackingFrameParams> McCameraTrackingController::BuildTrackingParams(
    CameraStandard::FocusTrackingMetaInfo &info)
{
    CameraStandard::Rect trackingRegion = info.GetTrackingRegion();

    std::shared_ptr<TrackingFrameParams> trackingFrameParams = std::make_shared<TrackingFrameParams>();
    trackingFrameParams->confidence = ConfidenceLevel::HIGH;
    trackingFrameParams->targetId = info.GetTrackingObjectId();
    auto now = std::chrono::system_clock::now();
    trackingFrameParams->timeStamp = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count());
    trackingFrameParams->fovV = currentCameraInfo_->fovV;
    trackingFrameParams->fovH = currentCameraInfo_->fovH;
    trackingFrameParams->isRecording = currentCameraInfo_->isRecording;
    trackingFrameParams->timeDelay = 0;
    trackingFrameParams->objectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE);
    sptr<CameraStandard::MetadataObject> targetObject;
    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects = info.GetDetectedObjects();
    if (GetTrackingTarget(trackingRegion, detectedObjects, info.GetTrackingObjectId(), targetObject) ==
        ERR_OK && targetObject != nullptr) {
        trackingFrameParams->timeStamp = static_cast<uint64_t>(targetObject->GetTimestamp());
        trackingFrameParams->targetId = targetObject->GetObjectId();
        CameraStandard::MetadataObjectType objectType = targetObject->GetType();
        ConvertObjectType(objectType, trackingFrameParams->objectType);
        HILOGI("trackingFrameParams param: %{public}s", trackingFrameParams->ToString().c_str());
        CameraStandard::Rect targetRect = targetObject->GetBoundingBox();
        trackingRect_ = targetRect;
        UpdateROI(trackingFrameParams, targetRect);
    }
    uint64_t lastTrackingTargetNum = currentCameraInfo_->trackingTargetNum;
    currentCameraInfo_->trackingTargetNum = detectedObjects.size();
    if (currentCameraInfo_->searchingTarget && currentCameraInfo_->trackingTargetNum > 0) {
        HILOGE("Stop searching, camera info: %{public}s", currentCameraInfo_->ToString().c_str());
        SearchTargetRotateFinish(SEARCH_TARGET_TASK_NAME);
        eventHandler_->PostTask([this]() {
                SearchTargetStop();
            }, SEARCH_TARGET_TASK_NAME);
    }
    uint64_t currentTime = static_cast<uint64_t>(std::chrono::time_point_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now()).time_since_epoch().count());
    if ((lastTrackingTargetNum == 0 && currentCameraInfo_->trackingTargetNum > 0) ||
        (currentTime - lastTrackingFrame_->timeStamp >= TRACKING_PARAM_LOST_DELAY &&
         currentCameraInfo_->trackingTargetNum > 0)) {
        UpdateActionControl();
    }
    lastTrackingFrame_ = trackingFrameParams;
    lastTrackingFrame_->timeStamp = static_cast<uint64_t>
        (std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now())
            .time_since_epoch().count());
    lastTrackingRect_ = trackingRect_;
    return trackingFrameParams;
}

int32_t McCameraTrackingController::GetTrackingTarget(CameraStandard::Rect &trackingRegion,
    std::vector<sptr<CameraStandard::MetadataObject>> &detectedObjects, int32_t trackingObjectId,
    sptr<CameraStandard::MetadataObject> &targetObject)
{
    if (detectedObjects.empty()) {
        HILOGW("detectedObjects is empty!!!");
        return DETECTED_OBJECT_IS_EMPTY;
    }
    sptr<CameraStandard::MetadataObject> selectedObject = nullptr;
    if (trackingObjectId >= 0) {
        for (const auto &item : detectedObjects) {
            if (item->GetObjectId() == trackingObjectId) {
                HILOGI("got detected object for id: %{public}d, type: %{public}d",
                    trackingObjectId, static_cast<int>(item->GetType()));
                    selectedObject = item;
                break;
            }
        }
    }
    if (selectedObject != nullptr) {
        return ProcessTargetByType(selectedObject, detectedObjects, targetObject);
    }
    if (currentCameraInfo_ &&
        currentCameraInfo_->sessionMode == static_cast<int32_t>(CameraStandard::SceneMode::CINEMATIC_VIDEO) &&
        trackingObjectId >= 0) {
        HILOGW("CinematicVideo mode: trackingObjectId %{public}d not found in current frame", trackingObjectId);
    }
    return GetTrackingTargetFallback(trackingRegion, detectedObjects, targetObject);
}
int32_t McCameraTrackingController::ProcessTargetByType(
    const sptr<CameraStandard::MetadataObject> &selectedObject,
    const std::vector<sptr<CameraStandard::MetadataObject>> &detectedObjects,
    sptr<CameraStandard::MetadataObject> &targetObject)
{
    CameraStandard::MetadataObjectType selectedType = selectedObject->GetType();
        switch (selectedType) {
            case CameraStandard::MetadataObjectType::HUMAN_HEAD:
                targetObject = FindFaceForHead(*selectedObject, detectedObjects);
                if (targetObject != nullptr) {
                    HILOGI("Tracking matching face for head.ObjectId:%{public}d ", targetObject->GetObjectId());
                    return ERR_OK;
                }
                targetObject = selectedObject;
                HILOGI("Tracking head (no matching face found)");
                return ERR_OK;
            case CameraStandard::MetadataObjectType::HUMAN_BODY:
                targetObject = FindBestFaceOrHeadForBody(*selectedObject, detectedObjects);
                if (targetObject != nullptr) {
                    HILOGI("Tracking %{public}s for body, objectID:%{public}d",
                        targetObject->GetType() == (CameraStandard::MetadataObjectType::HUMAN_HEAD)
                            ? "head"
                            : "face", targetObject->GetObjectId());
                    return ERR_OK;
                }
                targetObject = selectedObject;
                HILOGI("No matching face/head found, continue tracking body");

                return ERR_OK;
            default:
                targetObject = selectedObject;
                HILOGW("Tracking face/unknown type: %{public}d", static_cast<int>(selectedType));
                return ERR_OK;
        }
}

sptr<CameraStandard::MetadataObject> McCameraTrackingController::FindFaceForHead(
    CameraStandard::MetadataObject& headObject,
    const std::vector<sptr<CameraStandard::MetadataObject>>& detectedObjects)
{
    CameraStandard::Rect headRect = headObject.GetBoundingBox();
    sptr<CameraStandard::MetadataObject> bestFace = nullptr;
    float bestIou = 0.0f;

    for (const auto& obj : detectedObjects) {
        if (obj->GetType() != CameraStandard::MetadataObjectType::FACE &&
            obj->GetType() != CameraStandard::MetadataObjectType::BASE_FACE_DETECTION) {
            continue;
        }
        float iou = CalculateIOU(headRect, obj->GetBoundingBox());
        if (iou > bestIou && iou > IOU_THRESHOLD) {
            bestIou = iou;
            bestFace = obj;
            HILOGI("FindFaceForHead objectId:%{public}d,  CalculateIOU %{public}f", obj->GetObjectId(),  iou);
        }
    }
    if (bestFace != nullptr) {
        HILOGI("Found matching face for head, IOU: %{public}f", bestIou);
    }
    return bestFace;
}

sptr<CameraStandard::MetadataObject> McCameraTrackingController::FindBestFaceOrHeadForBody(
    CameraStandard::MetadataObject &bodyObject,
    const std::vector<sptr<CameraStandard::MetadataObject>> &detectedObjects)
{
    CameraStandard::Rect bodyRect = bodyObject.GetBoundingBox();
    std::vector<sptr<CameraStandard::MetadataObject>> faces;
    std::vector<sptr<CameraStandard::MetadataObject>> heads;
    for (const auto &obj : detectedObjects) {
        CameraStandard::MetadataObjectType type = obj->GetType();
        if (type == CameraStandard::MetadataObjectType::FACE ||
            type == CameraStandard::MetadataObjectType::BASE_FACE_DETECTION) {
            faces.push_back(obj);
        } else if (type == CameraStandard::MetadataObjectType::HUMAN_HEAD) {
            heads.push_back(obj);
        }
    }
    sptr<CameraStandard::MetadataObject> bestMatch = FindBestMatchForBody(bodyRect, faces);
    if (bestMatch != nullptr) {
        HILOGI("Found matching face for body.Face ObjectId:%{public}d", bestMatch->GetObjectId());
        return bestMatch;
    }
    bestMatch = FindBestMatchForBody(bodyRect, heads);
    if (bestMatch != nullptr) {
        HILOGI("Found matching head for body.Head ObjectId:%{public}d", bestMatch->GetObjectId());
        return bestMatch;
    }
    return nullptr;
}

sptr<CameraStandard::MetadataObject> McCameraTrackingController::FindBestMatchForBody(
    const CameraStandard::Rect &bodyRect, std::vector<sptr<CameraStandard::MetadataObject>> &objectsForBody)
{
    sptr<CameraStandard::MetadataObject> bestMatch = nullptr;
    float bestIou = 0.0f;
    int matchCount = 0;
    for (const auto &object : objectsForBody) {
        float iou = CalculateIOU(bodyRect, object->GetBoundingBox());
        if (iou > IOU_THRESHOLD) {
            matchCount++;
            if (iou > bestIou) {
                bestIou = iou;
                bestMatch = object;
                HILOGI("Find MatchObject IOU For Body. objectId:%{public}d,  CalculateIOU %{public}f",
                    object->GetObjectId(),
                    iou);
            }
        }
    }
    if (matchCount > 1) {
        HILOGW("Found %{public}d matching object for body, keeping body tracking", matchCount);
        return nullptr;
    }
    return bestMatch;
}

int32_t McCameraTrackingController::GetTrackingTargetFallback(CameraStandard::Rect &trackingRegion,
    std::vector<sptr<CameraStandard::MetadataObject>> &detectedObjects,
    sptr<CameraStandard::MetadataObject> &targetObject)
{
    if (lastTrackingFrame_ != nullptr && lastTrackingFrame_->targetId >= 0) {
        for (const auto &item : detectedObjects) {
            if (item->GetObjectId() == lastTrackingFrame_->targetId) {
                HILOGI("got detected object for id: %{public}d", lastTrackingFrame_->targetId);
                targetObject = item;
                return ERR_OK;
            }
        }
    }

    for (const auto &item : detectedObjects) {
        CameraStandard::Rect itemRect = item->GetBoundingBox();
        if (std::abs(itemRect.topLeftX - trackingRegion.topLeftX) <= TRACKING_LOST_CHECK &&
            std::abs(itemRect.topLeftY - trackingRegion.topLeftY) <= TRACKING_LOST_CHECK) {
            HILOGI("got detected object which is same as trackingRegion");
            targetObject = item;
            return ERR_OK;
            }
    }

    HILOGW("use first object as target object.");
    targetObject = detectedObjects[0];
    return ERR_OK;
}

float McCameraTrackingController::CalculateIOU(const CameraStandard::Rect &rect1, const CameraStandard::Rect &rect2)
{
    if (rect1.width <= 0 || rect1.height <= 0 || rect2.width <= 0 || rect2.height <= 0) {
        return IOU_DEFAULT;
    }
    float left1 = rect1.topLeftX;
    float top1 = rect1.topLeftY;
    float right1 = rect1.width;
    float bottom1 = rect1.height;

    float left2 = rect2.topLeftX;
    float top2 = rect2.topLeftY;
    float right2 = rect2.width;
    float bottom2 = rect2.height;
    if (right1 <= left1 || bottom1 <= top1 || right2 <= left2 || bottom2 <= top2) {
        return IOU_DEFAULT;
    }
    float interLeft = std::max(left1, left2);
    float interTop = std::max(top1, top2);
    float interRight = std::min(right1, right2);
    float interBottom = std::min(bottom1, bottom2);
    if (interRight <= interLeft || interBottom <= interTop) {
        return IOU_DEFAULT;
    }
    float interArea = (interRight - interLeft) * (interBottom - interTop);
    float area1 = (right1 - left1) * (bottom1 - top1);
    float area2 = (right2 - left2) * (bottom2 - top2);
    float minArea = std::min(area1, area2);
    if (minArea <= TRACKING_LOST_CHECK) {
        return IOU_DEFAULT;
    }
    return interArea / minArea;
}

int32_t McCameraTrackingController::CinematicVideoModeTrackingTargetFilter(
    CameraStandard::FocusTrackingMetaInfo &info, std::shared_ptr<TrackingFrameParams> &trackingParams)
{
    if (currentCameraInfo_->sessionMode != static_cast<int32_t>(CameraStandard::SceneMode::CINEMATIC_VIDEO)) {
        return ERR_OK;
    }
    HILOGI("current is CINEMATIC_VIDEO mode; last target id: %{public}d; original target id: %{public}d;"
           "tracking target id:%{public}d",
        lastTrackingFrame_->targetId,
        info.GetTrackingObjectId(),
        trackingParams->targetId);
    if (info.GetTrackingMode() == CameraStandard::FocusTrackingMode::FOCUS_TRACKING_MODE_LOCKED &&
        std::abs(info.GetTrackingRegion().topLeftX - info.GetTrackingRegion().width) <= TRACKING_LOST_CHECK &&
        std::abs(info.GetTrackingRegion().topLeftY - info.GetTrackingRegion().height) <= TRACKING_LOST_CHECK) {
        HILOGE("FOCUS_TRACKING_MODE_LOCKED and tracking region is 0");
        return INVALID_TRACKING_TARGET;
    }

    if (lastTrackingFrame_->targetId != trackingParams->targetId &&
        currentCameraInfo_->pauseFrameCount < MAX_PAUSED_FRAME) {
        bool isAllowedConversion = false;
        HILOGI("lastTrackingFrame type: %{public}d; trackingParams type:%{public}d",
            lastTrackingFrame_->objectType,
            trackingParams->objectType);
        if (lastTrackingFrame_->objectType == static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_BODY) &&
            (trackingParams->objectType == static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE) ||
                trackingParams->objectType == static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_HEAD))) {
            isAllowedConversion = IsAllowedConversion(lastTrackingRect_, trackingRect_);
        } else if ((lastTrackingFrame_->objectType == static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE) &&
                       trackingParams->objectType == static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_HEAD)) ||
                   (lastTrackingFrame_->objectType == static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_HEAD) &&
                       trackingParams->objectType == static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE))) {
            isAllowedConversion = IsAllowedConversion(lastTrackingRect_, trackingRect_);
        }
        if (!isAllowedConversion) {
            currentCameraInfo_->pauseFrameCount++;
            HILOGE("Tracking target changed and not allowed, pause count: %{public}d",
                currentCameraInfo_->pauseFrameCount);
            return INVALID_TRACKING_TARGET;
        }
    }
    HILOGI("Tracking");
    currentCameraInfo_->pauseFrameCount = 0;
    return ERR_OK;
}

bool McCameraTrackingController::IsAllowedConversion(
    CameraStandard::Rect &lastTrackingRect, CameraStandard::Rect &trackingRect)
{
    float iou = CalculateIOU(lastTrackingRect, trackingRect);
    if (iou > IOU_THRESHOLD) {
        HILOGI("Conversion allowed, IOU: %{public}f", iou);
        return true;
    } else {
        HILOGW("Conversion rejected, IOU too low: %{public}f", iou);
        return false;
    }
}

bool McCameraTrackingController::FilterDetectedObject(sptr<CameraStandard::MetadataObject> &detectedObject)
{
    CameraStandard::MetadataObjectType cameraObjectType = detectedObject->GetType();
    switch (cameraObjectType) {
        case CameraStandard::MetadataObjectType::INVALID :
            return false;
        case CameraStandard::MetadataObjectType::FACE :
            return true;
        case CameraStandard::MetadataObjectType::HUMAN_BODY :
            return true;
        case CameraStandard::MetadataObjectType::CAT_FACE :
            return false;
        case CameraStandard::MetadataObjectType::CAT_BODY :
            return false;
        case CameraStandard::MetadataObjectType::DOG_FACE :
            return false;
        case CameraStandard::MetadataObjectType::DOG_BODY :
            return false;
        case CameraStandard::MetadataObjectType::SALIENT_DETECTION :
            return false;
        case CameraStandard::MetadataObjectType::BAR_CODE_DETECTION :
            return false;
        case CameraStandard::MetadataObjectType::BASE_FACE_DETECTION :
            return false;
        default:
            return false;
    }
}

void McCameraTrackingController::UpdateROI(std::shared_ptr<TrackingFrameParams> &trackingFrameParams,
    CameraStandard::Rect &rect)
{
    HILOGI("rect info, topx: %{public}f; topy: %{public}f;width: %{public}f; height: %{public}f", rect.topLeftX,
        rect.topLeftY, rect.width, rect.height);
    if (trackingFrameParams == nullptr) {
        HILOGW("trackingFrameParams is nullptr.");
        return;
    }
    if ((rect.topLeftX < TRACKING_LOST_CHECK && rect.topLeftY < TRACKING_LOST_CHECK) ||
            rect.width < TRACKING_LOST_CHECK || rect.height < TRACKING_LOST_CHECK) {
        trackingFrameParams->roi = {0.0f, 0.0f, 0.0f, 0.0f};
        return;
    }
    trackingFrameParams->roi.width = rect.width - rect.topLeftX;
    trackingFrameParams->roi.height = rect.height - rect.topLeftY;

    AdjustROI(trackingFrameParams->roi, rect, currentCameraInfo_->cameraType, sensorRotation_);
    HILOGI("roi info, roi.x: %{public}f; roi.y: %{public}f", trackingFrameParams->roi.x, trackingFrameParams->roi.y);
}

int32_t McCameraTrackingController::UpdateMotionsWithTrackingData(
    const std::shared_ptr<TrackingFrameParams> &params, int32_t trackingObjectId)
{
    if (params == nullptr) {
        HILOGW("Params is null for tracking ID: %{public}d", trackingObjectId);
        return ERR_OK;
    }
    bool isEnableTrackingMode =
            SESSION_MODE_WHITELIST.find(currentCameraInfo_->sessionMode) != SESSION_MODE_WHITELIST.end();
    if (!currentCameraInfo_->currentTrackingEnable || IsCurrentFocus() || !isEnableTrackingMode) {
        HILOGW("current tracking is not enabled");
        return ERR_OK;
    }
    HILOGD("tracking frame info: %{public}s", params->ToString().c_str());

    std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
    for (const auto &item : MechBodyControllerService::GetInstance().motionManagers_) {
        int32_t mechId = item.first;
        std::shared_ptr<MotionManager> motionManager = item.second;
        if (motionManager == nullptr) {
            HILOGE("MotionManager not exists; mechId: %{public}d", mechId);
            continue;
        }

        int32_t setResult = motionManager->SetMechCameraTrackingFrame(params);
        HILOGI("Set tracking info for mech id: %{public}d; result: %{public}s; code: %{public}d",
               mechId, setResult == ERR_OK ? "success" : "failed", setResult);
    }
    return ERR_OK;
}

int32_t McCameraTrackingController::SetTrackingEnabled(const uint32_t &tokenId, bool isEnabled)
{
    std::lock_guard<std::mutex> lock(appSettingsMutex_);
    if (appSettings.find(tokenId) == appSettings.end()) {
        std::shared_ptr<AppSetting> appSetting = std::make_shared<AppSetting>();
        appSettings[tokenId] = appSetting;
    }
    HILOGI("update tracking enable");
    appSettings[tokenId]->isTrackingEnabled = isEnabled;
    currentCameraInfo_->currentTrackingEnable = isEnabled;
    return ERR_OK;
}

int32_t McCameraTrackingController::GetTrackingEnabled(const uint32_t &tokenId, bool &isEnabled)
{
    std::lock_guard<std::mutex> lock(appSettingsMutex_);
    if (appSettings.find(tokenId) == appSettings.end()) {
        HILOGE("No configuration information is saved. app token id: %{public}s;", GetAnonymUint32(tokenId).c_str());
        isEnabled = true;
        return ERR_OK;
    }
    std::shared_ptr<AppSetting> setting = appSettings[tokenId];
    isEnabled = setting->isTrackingEnabled;
    return ERR_OK;
}

int32_t McCameraTrackingController::RegisterTrackingEventCallback(const uint32_t &tokenId,
    sptr <IRemoteObject> callback)
{
    HILOGI("start, tokenid: %{public}s;", GetAnonymUint32(tokenId).c_str());
    sptr <MechControllerIpcDeathListener> diedListener = new MechControllerIpcDeathListener();
    diedListener->tokenId_ = tokenId;
    diedListener->objectType_ = RemoteObjectType::TRACKING_EVENT_CALLBACK;
    CHECK_POINTER_RETURN_VALUE(callback, INVALID_PARAMETERS_ERR, "callback");
    callback->AddDeathRecipient(diedListener);
    {
        std::lock_guard<std::mutex> lock(trackingEventCallbackMutex_);
        trackingEventCallback_[tokenId] = callback;
    }
    HILOGI("callback add success, tokenid: %{public}s;", GetAnonymUint32(tokenId).c_str());
    return ERR_OK;
}

int32_t McCameraTrackingController::UnRegisterTrackingEventCallback(const uint32_t &tokenId)
{
    HILOGI("start, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    std::lock_guard<std::mutex> lock(trackingEventCallbackMutex_);
    trackingEventCallback_.erase(tokenId);
    HILOGI("callback remove success, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    return ERR_OK;
}

int32_t McCameraTrackingController::OnTrackingEvent(const int32_t &mechId, const TrackingEvent &event)
{
    HILOGI("start notify tracking event, mechId: %{public}d; event: %{public}d", mechId,
           static_cast<int32_t>(event));
    std::lock_guard<std::mutex> lock(trackingEventCallbackMutex_);
    for (const auto &item: trackingEventCallback_) {
        uint32_t tokenId = item.first;
        int32_t isTrackingEnableNum = static_cast<int32_t>(event);
        if (event == TrackingEvent::CAMERA_TRACKING_USER_ENABLED &&
            appSettings.find(tokenId) != appSettings.end() && !appSettings[tokenId]->isTrackingEnabled) {
            HILOGE("App tracking enabled setting is false");
            isTrackingEnableNum = static_cast<int32_t>(TrackingEvent::CAMERA_TRACKING_USER_DISABLED);
        }
        HILOGI("notify tracking event to tokenId: %{public}s; isTrackingEnableNum: %{public}d",
            GetAnonymUint32(tokenId).c_str(), isTrackingEnableNum);
        sptr <IRemoteObject> callback = item.second;
        MessageParcel data;
        if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
            HILOGE("Write interface token failed.");
            continue;
        }
        if (!data.WriteInt32(mechId)) {
            HILOGE("Write mechId failed.");
            continue;
        }

        if (!data.WriteInt32(isTrackingEnableNum)) {
            HILOGE("Write event failed.");
            continue;
        }
        MessageParcel reply;
        MessageOption option;
        CHECK_POINTER_RETURN_VALUE(callback, INVALID_PARAMETERS_ERR, "callback");
        int32_t error = callback->SendRequest(
            static_cast<uint32_t>(IMechBodyControllerCode::TRACKING_EVENT_CALLBACK), data, reply, option);
        HILOGI("notify tracking event to tokenId: %{public}u; result: %{public}s", tokenId,
            error == ERR_NONE ? "success" : "failed");
    }
    return ERR_OK;
}

bool McCameraTrackingController::IsCurrentTrackingEnabled()
{
    bool deviceIsEnable = false;
    {
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
        if (MechBodyControllerService::GetInstance().motionManagers_.empty()) {
            return currentCameraInfo_->currentTrackingEnable;
        }
        for (auto it : MechBodyControllerService::GetInstance().motionManagers_) {
            int32_t mechId = it.first;
            std::shared_ptr<MotionManager> motionManager = it.second;
            if (motionManager == nullptr) {
                continue;
            }
            bool isEnabledTmp;
            motionManager->GetMechCameraTrackingEnabled(isEnabledTmp);
            HILOGI("got device tracking state, mech id: %{public}d; isEnable: %{public}s",
                   mechId, isEnabledTmp ? "true" : "false");
            deviceIsEnable |= isEnabledTmp;
        }
    }
    return currentCameraInfo_->currentTrackingEnable && deviceIsEnable;
}

int32_t McCameraTrackingController::UpdateActionControl()
{
    if (eventHandler_ != nullptr) {
        eventHandler_->RemoveTask(UPDATE_ACTION_CONTROL_TASK_NAME);
        eventHandler_->PostTask(
            [this]() {
                UpdateActionControl();
            },
            UPDATE_ACTION_CONTROL_TASK_NAME, UPDATE_ACTION_CONTROL_TASK_DELAY);
    }
    ActionControlParams actionControlParam;
    actionControlParam.controlReq = 0;
    actionControlParam.timeOut = ACTION_CONTROL_TIMEOUT;
    actionControlParam.yawControl = 0;
    actionControlParam.pitchControl = 0;
    actionControlParam.rollControl = 0;
    uint64_t currentTime = static_cast<uint64_t>
        (std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now())
            .time_since_epoch().count());
    if (currentCameraInfo_ != nullptr && currentCameraInfo_->trackingTargetNum > 0 &&
        currentTime - lastTrackingFrame_->timeStamp < TRACKING_PARAM_LOST_DELAY && IsCurrentTrackingEnabled()) {
        actionControlParam.controlReq = 1;
        actionControlParam.yawControl = 1;
        actionControlParam.pitchControl = 1;
        actionControlParam.rollControl = 1;
    }
    HILOGI("actionControlParam: %{public}s", actionControlParam.ToString().c_str());
    {
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
        for (const auto &item : MechBodyControllerService::GetInstance().motionManagers_) {
            int32_t mechId = item.first;
            std::shared_ptr motionManager = item.second;
            if (motionManager == nullptr) {
                HILOGE("MotionManager not exists; mechId: %{public}d", mechId);
                continue;
            }
            int32_t controlResult = motionManager->ActionGimbalFeatureControl(actionControlParam);
            HILOGI("action gimbal control Result: %{public}s; code: %{public}d",
                   controlResult == ERR_OK ? "success" : "failed", controlResult);
        }
    }
    return ERR_OK;
}

int32_t McCameraTrackingController::SetStickOffset(const int16_t &stickX, const int16_t &stickY)
{
    HILOGI("before.horizontal_: %{public}f vertical_: %{public}f stickX: %{public}d stickY: %{public}d.",
        targetHorizontal_, targetVertical_, stickX, stickY);
    if (stickX > STICK_BASE) {
        targetHorizontal_ += OFFSET_VALUE_BASE;
    }
    if (stickX < -STICK_BASE) {
        targetHorizontal_ -= OFFSET_VALUE_BASE;
    }
    if (stickY > STICK_BASE) {
        targetVertical_ += OFFSET_VALUE_BASE;
    }
    if (stickY < -STICK_BASE) {
        targetVertical_ -= OFFSET_VALUE_BASE;
    }
    targetHorizontal_ = targetHorizontal_ > OFFSET_VALUE_MAX ? OFFSET_VALUE_MAX : targetHorizontal_;
    targetHorizontal_ = targetHorizontal_ < OFFSET_VALUE_MIN ? OFFSET_VALUE_MIN : targetHorizontal_;
    targetVertical_ = targetVertical_ > OFFSET_VALUE_MAX ? OFFSET_VALUE_MAX : targetVertical_;
    targetVertical_ = targetVertical_ < OFFSET_VALUE_MIN ? OFFSET_VALUE_MIN : targetVertical_;
    isStick_ = false;
    HILOGI("after.horizontal_: %{public}f vertical_: %{public}f.", targetHorizontal_, targetVertical_);
    return ERR_OK;
}

int32_t McCameraTrackingController::SetTrackingLayout(CameraTrackingLayout &cameraTrackingLayout)
{
    std::shared_ptr<LayoutParams> layoutParam = std::make_shared<LayoutParams>();
    layoutParam->isDefault = false;
    layoutParam->offsetX = LAYOUT_MIDDLE;
    layoutParam->offsetY = LAYOUT_MIDDLE;
    bool hasSuccess = false;
    {
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
        std::map<int32_t, std::shared_ptr<MotionManager>> motionManagers =
            MechBodyControllerService::GetInstance().motionManagers_;
        for (const auto &item: motionManagers) {
            int32_t mechId = item.first;
            std::shared_ptr<MotionManager> motionManager = item.second;
            if (motionManager == nullptr) {
                HILOGE("motionManager not exist. mechId: %{public}d;", mechId);
                continue;
            }
            int32_t result = motionManager->SetMechCameraTrackingLayout(layoutParam);
            hasSuccess = hasSuccess || (result == ERR_OK);
            HILOGI("mech id: %{public}d result code: %{public}d, layout: %{public}s",
                   mechId, result, layoutParam->ToString().c_str());
        }
    }
    if (hasSuccess) {
        currentCameraInfo_->currentCameraTrackingLayout = cameraTrackingLayout;
    }
    return hasSuccess ? ERR_OK : INVALID_TRACKING_LAYOUT;
}

int32_t McCameraTrackingController::SetTrackingLayout(const uint32_t &tokenId,
    CameraTrackingLayout &cameraTrackingLayout)
{
    HILOGI("tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    currentCameraInfo_->currentCameraTrackingLayout = cameraTrackingLayout;
    horizontal_ = cameraTrackingLayout == CameraTrackingLayout::RIGHT ? OFFSET_VALUE
        : (cameraTrackingLayout == CameraTrackingLayout::LEFT ? -OFFSET_VALUE : 0.0f);
    targetHorizontal_ = horizontal_;
    targetVertical_ = 0.0f;
    vertical_ = 0.0f;
    OnTrackingEvent(0, TrackingEvent::CAMERA_TRACKING_LAYOUT_CHANGED);
    HILOGI("SetTrackingLayout horizontal_: %{public}f vertical_: %{public}f.", horizontal_, vertical_);
    return ERR_OK;
}

int32_t McCameraTrackingController::GetTrackingLayout(CameraTrackingLayout &cameraTrackingLayout)
{
    cameraTrackingLayout = horizontal_ + MIDDLE_VALUE_HORIZONTAL > MIDDLE_MAX ? CameraTrackingLayout::RIGHT
        : (horizontal_ + MIDDLE_VALUE_HORIZONTAL < MIDDLE_MIN ? CameraTrackingLayout::LEFT
        : CameraTrackingLayout::MIDDLE);
    
    currentCameraInfo_->currentCameraTrackingLayout = cameraTrackingLayout;
    return ERR_OK;
}

int32_t McCameraTrackingController::SearchTarget(std::string &napiCmdId, uint32_t &tokenId,
    const std::shared_ptr<TargetInfo> &targetInfo, const std::shared_ptr<SearchParams> &searchParams)
{
    if (targetInfo == nullptr || searchParams == nullptr) {
        HILOGE("SEARCH_TARGET search param is nullptr");
        return INVALID_PARAMETERS_ERR;
    }
    HILOGI("SEARCH_TARGET start, cmdId: %{public}s; target info: %{public}s, search param: %{public}s",
           napiCmdId.c_str(), targetInfo->ToString().c_str(), searchParams->ToString().c_str());
    std::shared_ptr<MotionManager> motionManager;
    {
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
        std::map<int32_t, std::shared_ptr<MotionManager>> motionManagers =
                MechBodyControllerService::GetInstance().motionManagers_;
        if (motionManagers.empty()) {
            HILOGE("motion managers is empty. no useful mechanic.");
            return NO_DEVICE_CONNECTED;
        }
        for (const auto &item: motionManagers) {
            motionManager = item.second;
            break;
        }
    }
    if (motionManager == nullptr) {
        HILOGE("SEARCH_TARGET motion managers is empty, can not search target.");
        return NO_DEVICE_CONNECTED;
    }
    if (currentCameraInfo_ != nullptr && currentCameraInfo_->trackingTargetNum > 0) {
        MechBodyControllerService::GetInstance().SearchTargetEnd(
            tokenId, napiCmdId, currentCameraInfo_->trackingTargetNum);
        return ERR_OK;
    }
    currentCameraInfo_->currentTrackingEnable = false;
    RotateDegreeLimit limit;
    if (motionManager->GetRotationLimit(limit) != ERR_OK) {
        HILOGE("SEARCH_TARGET get device limited info failed.");
        return GET_LIMIT_INFO_FAILED;
    }

    std::shared_ptr<EulerAngles> currentPosition;
    if (motionManager->GetCurrentPosition(currentPosition) != ERR_OK) {
        HILOGE("SEARCH_TARGET get device limited info failed.");
        return GET_CURRENT_POSITION_FAILED;
    }
    RunSearchTarget(napiCmdId, tokenId, searchParams, limit, currentPosition);
    HILOGE("SEARCH_TARGET end.");
    return ERR_OK;
}

void McCameraTrackingController::RunSearchTarget(std::string &napiCmdId, uint32_t &tokenId,
    const std::shared_ptr<SearchParams> &searchParams, const RotateDegreeLimit &limit,
    const std::shared_ptr<EulerAngles> &currentPosition)
{
    float toLeft = std::min(std::abs(limit.negMax.yaw - (currentPosition->yaw - PI - PI)),
        std::abs(limit.negMax.yaw - currentPosition->yaw));
    float toRight = std::min(std::abs(limit.posMax.yaw - (currentPosition->yaw + PI + PI)),
        std::abs(limit.posMax.yaw - currentPosition->yaw));
    bool startFromNeg = (searchParams->direction == SearchDirection::DEFAULT && toLeft < toRight) ||
        searchParams->direction == SearchDirection::RIGHTWARD;
    HILOGI("SEARCH_TARGET current position: %{public}s; limit: %{public}s; start search from: %{public}s",
        currentPosition->ToString().c_str(), limit.ToString().c_str(), startFromNeg ? "NEG" : "POS");
    if (limit.negMax.yaw > ROTATE_LIMITED_MIN || limit.posMax.yaw < ROTATE_LIMITED_MAX) {
        HILOGI("SEARCH_TARGET device has limited position, start from: %{public}s", startFromNeg ? "NEG" : "POS");
        ExecSearchTaskWithLimit(napiCmdId, tokenId, startFromNeg, limit);
    } else {
        int32_t searchTimes = 0;
        ExecSearchTask(napiCmdId, tokenId, startFromNeg, searchTimes);
    }
}

void McCameraTrackingController::SearchTargetRotateFinish(const std::string &cmdId)
{
    HILOGI("SEARCH_TARGET start");
    if (cmdId != SEARCH_TARGET_TASK_NAME) {
        return;
    }
    std::unique_lock<std::mutex> lock(searchTargetMutex_);
    searchTargetCv_.notify_all();
    HILOGI("SEARCH_TARGET end");
}

void McCameraTrackingController::SearchTargetStop()
{
    HILOGI("SEARCH_TARGET start");
    if (!currentCameraInfo_->searchingTarget) {
        HILOGI("SEARCH_TARGET search target has stoped.");
        return;
    }
    currentCameraInfo_->searchingTarget = false;
    if (eventHandler_ != nullptr) {
        eventHandler_->RemoveTask(SEARCH_TARGET_TASK_NAME);
    }
    uint32_t tokenId = currentCameraInfo_->tokenId;
    HILOGI("SEARCH_TARGET camera info: %{public}s", currentCameraInfo_->ToString().c_str());
    if (appSettings.find(tokenId) == appSettings.end()) {
        SetTrackingEnabled(tokenId, true);
    } else {
        SetTrackingEnabled(tokenId, appSettings[tokenId]->isTrackingEnabled);
    }
    MechBodyControllerService::GetInstance().SearchTargetEnd(
        tokenId, currentCameraInfo_->searchTargetNapiCmdId, currentCameraInfo_->trackingTargetNum);
    std::string taskId = SEARCH_TARGET_TASK_NAME;
    std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
    std::map<int32_t, std::shared_ptr<MotionManager>> motionManagers =
            MechBodyControllerService::GetInstance().motionManagers_;
    for (const auto &item: motionManagers) {
        int32_t mechId = item.first;
        std::shared_ptr<MotionManager> motionManager = item.second;
        if (motionManager == nullptr) {
            HILOGE("motionManager not exist. mechId: %{public}d;", mechId);
            continue;
        }
        int32_t result = motionManager->StopRotate(currentCameraInfo_->tokenId, taskId);
        HILOGI("mech id: %{public}d result code: %{public}d", mechId, result);
    }
    HILOGI("SEARCH_TARGET end");
}

std::shared_ptr<CameraInfo> McCameraTrackingController::GetCurrentCameraInfo() const
{
    return currentCameraInfo_;
}

void McCameraTrackingController::SensorCallback(SensorEvent* event)
{
    if (event == nullptr) {
        return;
    }
    GravityData* gravityData = reinterpret_cast<GravityData*>(event->data);
    MobileRotation sensorRotation = CalculateSensorRotation(gravityData);
    if (sensorRotation != MobileRotation::INVALID &&
        McCameraTrackingController::GetInstance().sensorRotation_ != sensorRotation) {
        HILOGI("update device position to: %{public}d;", static_cast<int32_t>(sensorRotation));
        McCameraTrackingController::GetInstance().horizontal_ = 0.0f;
        McCameraTrackingController::GetInstance().vertical_ = 0.0f;
        McCameraTrackingController::GetInstance().targetHorizontal_ = 0.0f;
        McCameraTrackingController::GetInstance().targetVertical_ = 0.0f;
        McCameraTrackingController::GetInstance().sensorRotation_ = sensorRotation;
        std::shared_ptr<CameraInfo> currentCameraInfo =
            McCameraTrackingController::GetInstance().GetCurrentCameraInfo();
        if (currentCameraInfo == nullptr) {
            HILOGE("currentCameraInfo is nullptr;");
            return;
        }
        if (currentCameraInfo->tokenId != 0 &&
            McCameraTrackingController::GetInstance().ComputeFov() == ERR_OK) {
            McCameraTrackingController::GetInstance().UpdateMotionManagers();
        }
    }
}

MobileRotation McCameraTrackingController::CalculateSensorRotation(GravityData* gravityData)
{
    if (gravityData == nullptr) {
        return MobileRotation::UP;
    }
    float x = gravityData->x;
    float y = gravityData->y;
    float z = gravityData->z;

    if ((x * x + y * y) * VALID_INCLINATION_ANGLE_THRESHOLD_COEFFICIENT < z * z) {
        return MobileRotation::INVALID;
    }

    int degree = DEGREE_CONSTANT_90 - static_cast<int>(round(std::atan2(y, -x) / M_PI * DEGREE_CONSTANT_180));
    int32_t sensorDegree = degree >= 0 ?
        degree % DEGREE_CONSTANT_360 : degree % DEGREE_CONSTANT_360 + DEGREE_CONSTANT_360;

    if (sensorDegree >= 0 &&
        (sensorDegree <= MOBILE_ROTATION_CHECK_UP_END || sensorDegree >= MOBILE_ROTATION_CHECK_UP_BEGIN)) {
        return MobileRotation::UP;
    } else if (sensorDegree >= MOBILE_ROTATION_CHECK_RIGHT_BEGIN && sensorDegree <= MOBILE_ROTATION_CHECK_RIGHT_END) {
        return MobileRotation::RIGHT;
    } else if (sensorDegree >= MOBILE_ROTATION_CHECK_DOWN_BEGIN && sensorDegree <= MOBILE_ROTATION_CHECK_DOWN_END) {
        return MobileRotation::DOWN;
    } else if (sensorDegree >= MOBILE_ROTATION_CHECK_LEFT_BEGIN && sensorDegree <= MOBILE_ROTATION_CHECK_LEFT_END) {
        return MobileRotation::LEFT;
    } else {
        return MobileRotation::UP;
    }
}

void McCameraTrackingController::ConvertObjectType(CameraStandard::MetadataObjectType &cameraObjectType,
    uint8_t &mechObjectType)
{
    switch (cameraObjectType) {
        case CameraStandard::MetadataObjectType::FACE :
            mechObjectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE);
            break;
        case CameraStandard::MetadataObjectType::BASE_FACE_DETECTION :
            mechObjectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE);
            break;
        case CameraStandard::MetadataObjectType::HUMAN_BODY :
            mechObjectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_BODY);
            break;
        default:
            mechObjectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER);
    }
}

void McCameraTrackingController::RegisterTrackingListener()
{
    HILOGI("start");
    if (pMechSession != nullptr) {
        HILOGE("MechSession has registered.");
        return;
    }
    int userId = INVALID_USER_ID;
    ErrCode err = AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(userId);
    if (err != ERR_OK) {
        HILOGE("GetLocalAccountId passing param invalid or return error!, err : %{public}d", err);
    }
    HILOGI("Create MechSession. user id: %{public}d", userId);
    sptr<CameraStandard::CameraManager> cameraManager = CameraStandard::CameraManager::GetInstance();
    if (cameraManager == nullptr) {
        HILOGE("CameraManager is nullptr.");
        return;
    }
    int createSessionResult = CameraStandard::CameraManager::GetInstance()->CreateMechSession(userId, &pMechSession);
    HILOGI("Camera session create result: %{public}s; code: %{public}d",
           createSessionResult == CameraStandard::CameraErrorCode::SUCCESS ? "success" : "failed",
           createSessionResult);
    if (createSessionResult == CameraStandard::CameraErrorCode::SUCCESS && pMechSession != nullptr) {
        HILOGI("Set Session Callback.");
        pMechSession->EnableMechDelivery(true);
        std::shared_ptr<MechSessionCallbackImpl> mechSessionCallback = std::make_shared<MechSessionCallbackImpl>();
        pMechSession->SetCallback(mechSessionCallback);
    }
    HILOGI("end");
}

void McCameraTrackingController::UnRegisterTrackingListener()
{
    HILOGI("start");
    if (pMechSession == nullptr) {
        return;
    }
    pMechSession->EnableMechDelivery(false);
    pMechSession->Release();
    pMechSession = nullptr;
    HILOGI("end");
}

void McCameraTrackingController::RegisterSensorListener()
{
    HILOGI("start");
    if (user.callback != nullptr) {
        return;
    }
    user.callback = McCameraTrackingController::SensorCallback;
    int32_t subscribeRet = SubscribeSensor(SENSOR_TYPE_ID_GRAVITY, &user);
    HILOGI("RegisterSensorCallback, subscribeRet: %{public}d", subscribeRet);
    int32_t setBatchRet = SetBatch(SENSOR_TYPE_ID_GRAVITY, &user, POSTURE_INTERVAL, 0);
    HILOGI("RegisterSensorCallback, setBatchRet: %{public}d", setBatchRet);
    int32_t activateRet = ActivateSensor(SENSOR_TYPE_ID_GRAVITY, &user);
    HILOGI("RegisterSensorCallback, activateRet: %{public}d", activateRet);
    HILOGI("end");
}

void McCameraTrackingController::UnRegisterSensorListener()
{
    HILOGI("start");
    if (user.callback == nullptr) {
        return;
    }
    int32_t activateRet = DeactivateSensor(SENSOR_TYPE_ID_GRAVITY, &user);
    HILOGI("UnRegisterSensorCallback, activateRet: %{public}d", activateRet);
    int32_t subscribeRet = UnsubscribeSensor(SENSOR_TYPE_ID_GRAVITY, &user);
    HILOGI("UnRegisterSensorCallback, subscribeRet: %{public}d", subscribeRet);
    user.callback = nullptr;
    HILOGI("end");
}

void McCameraTrackingController::AdjustROI(ROI &roi, CameraStandard::Rect &rect, CameraType cameraType,
    MobileRotation sensorRotation)
{
    if (cameraType == CameraType::FRONT) {
        if (sensorRotation == MobileRotation::UP || sensorRotation == MobileRotation::LEFT) {
            roi.x = NUM_1 - rect.topLeftX - roi.width / NUM_2;
            roi.y = NUM_1 - rect.topLeftY - roi.height / NUM_2;
        } else if (sensorRotation == MobileRotation::RIGHT || sensorRotation == MobileRotation::DOWN) {
            roi.x = rect.topLeftX + roi.width / NUM_2;
            roi.y = rect.topLeftY + roi.height / NUM_2;
        }
    } else if (cameraType == CameraType::BACK) {
        if (sensorRotation == MobileRotation::UP || sensorRotation == MobileRotation::LEFT) {
            roi.x = rect.topLeftX + roi.width / NUM_2;
            roi.y = rect.topLeftY + roi.height / NUM_2;
        } else if (sensorRotation == MobileRotation::RIGHT || sensorRotation == MobileRotation::DOWN) {
            roi.x = NUM_1 - rect.topLeftX - roi.width / NUM_2;
            roi.y = NUM_1 - rect.topLeftY - roi.height / NUM_2;
        }
    }
    HILOGI("Before the offset, roi.x: %{public}f; roi.y: %{public}f", roi.x, roi.y);
}

void McCameraTrackingController::AdjustOffset(std::shared_ptr<TrackingFrameParams> &trackingParams,
    CameraType cameraType)
{
    HILOGI("AdjustOffset begin.");
    if (std::abs(targetVertical_ - vertical_) > TRACKING_LOST_CHECK) {
        if (targetVertical_ > vertical_) {
            vertical_ += OFFSET_VALUE_BASE_1;
        } else if (targetVertical_ < vertical_) {
            vertical_ -= OFFSET_VALUE_BASE_1;
        }
    }

    if (std::abs(targetHorizontal_ - horizontal_) > TRACKING_LOST_CHECK) {
        if (targetHorizontal_ > horizontal_) {
            horizontal_ += OFFSET_VALUE_BASE_1;
        } else if (targetHorizontal_ < horizontal_) {
            horizontal_ -= OFFSET_VALUE_BASE_1;
        }
    }

    if (!isStick_) {
        auto currentTime = std::chrono::steady_clock::now();
        auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastStickTime_).count();
        if (timeDiff > TRACKING_STATE_TIME) {
            isStick_ = true;
        }
    }

    bool isFace = trackingParams->objectType == static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE);
    if (isStick_) {
        if (sensorRotation_ == MobileRotation::UP || sensorRotation_ == MobileRotation::DOWN) {
            AddYOffset(trackingParams->roi, cameraType, vertical_, horizontal_, isFace);
        } else if (sensorRotation_ == MobileRotation::LEFT || sensorRotation_ == MobileRotation::RIGHT) {
            AddXOffset(trackingParams->roi, cameraType, vertical_, horizontal_, isFace);
        }
    } else {
        if (sensorRotation_ == MobileRotation::UP || sensorRotation_ == MobileRotation::DOWN) {
            AdjustYOffset(trackingParams->roi, cameraType, vertical_, horizontal_, isFace);
        } else if (sensorRotation_ == MobileRotation::LEFT || sensorRotation_ == MobileRotation::RIGHT) {
            AdjustXOffset(trackingParams->roi, cameraType, vertical_, horizontal_, isFace);
        }
    }

    setTrackingLimit(trackingParams->roi);

    trackingParams->objectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER);
}

void McCameraTrackingController::setTrackingLimit(ROI &roi)
{
    roi.x = roi.x > 1.0f ? 1.0f : (roi.x < 0.0f ? 0.0f : roi.x);
    roi.y = roi.y > 1.0f ? 1.0f : (roi.y < 0.0f ? 0.0f : roi.y);
}
void McCameraTrackingController::AddYOffset(ROI &roi, CameraType cameraType,
    float &offsetX, float &offsetY, bool &isFace)
{
    HILOGI("start offset before ROI: %{public}s", roi.ToString().c_str());
    float yOffset = offsetY;
    float xOffset = -offsetX;

    if (cameraType == CameraType::FRONT) {
        yOffset = -yOffset;
    }
    yOffset = yOffset > UP_HORIZONTAL ? UP_HORIZONTAL : (yOffset < DOWN_HORIZONTAL ? DOWN_HORIZONTAL : yOffset);
    roi.y = roi.y + yOffset;

    if (isFace) {
        xOffset += FACE_OFFSET;
    }
    xOffset = xOffset > UP_HORIZONTAL ? UP_HORIZONTAL : (xOffset < DOWN_HORIZONTAL ? DOWN_HORIZONTAL : xOffset);
    roi.x = roi.x + xOffset;
    HILOGI("start offset after ROI: %{public}s", roi.ToString().c_str());
}

void McCameraTrackingController::AddXOffset(ROI &roi, CameraType cameraType,
    float &offsetX, float &offsetY, bool &isFace)
{
    HILOGI("start offset before ROI: %{public}s", roi.ToString().c_str());
    float yOffset = offsetX;
    float xOffset = -offsetY;

    if (isFace) {
        if (cameraType == CameraType::FRONT) {
            yOffset -= FACE_OFFSET;
        } else if (cameraType == CameraType::BACK) {
            yOffset += FACE_OFFSET;
        }
    }
    yOffset = yOffset > UP_HORIZONTAL ? UP_HORIZONTAL : (yOffset < DOWN_HORIZONTAL ? DOWN_HORIZONTAL : yOffset);
    roi.y = roi.y + yOffset;

    xOffset = xOffset > UP_HORIZONTAL ? UP_HORIZONTAL : (xOffset < DOWN_HORIZONTAL ? DOWN_HORIZONTAL : xOffset);
    roi.x = roi.x + xOffset;
    HILOGI("start offset after ROI: %{public}s", roi.ToString().c_str());
}

void McCameraTrackingController::AdjustYOffset(ROI &roi, CameraType cameraType,
    float &offsetX, float &offsetY, bool &isFace)
{
    HILOGI("start offset before ROI: %{public}s", roi.ToString().c_str());
    float yOffset = offsetY;
    float xOffset = -offsetX;

    float virtualCenterY = 0.0f;
    if (cameraType == CameraType::FRONT) {
        virtualCenterY = MIDDLE_VALUE_HORIZONTAL - yOffset;
    } else if (cameraType == CameraType::BACK) {
        virtualCenterY = MIDDLE_VALUE_HORIZONTAL + yOffset;
    }
    if (roi.y > virtualCenterY) {
        yOffset = (roi.y - virtualCenterY) / (UP_LIMIT - virtualCenterY) * (1 - MIDDLE_VALUE_HORIZONTAL);
    } else if (roi.y < virtualCenterY) {
        yOffset = (roi.y - virtualCenterY) / (virtualCenterY - DOWN_LIMIT) * (1 - MIDDLE_VALUE_HORIZONTAL);
    }
    yOffset = yOffset > UP_HORIZONTAL ? UP_HORIZONTAL : (yOffset < DOWN_HORIZONTAL ? DOWN_HORIZONTAL : yOffset);
    roi.y = MIDDLE_VALUE_HORIZONTAL + yOffset;

    if (isFace) {
        xOffset += FACE_OFFSET;
    }
    xOffset = xOffset > UP_HORIZONTAL ? UP_HORIZONTAL : (xOffset < DOWN_HORIZONTAL ? DOWN_HORIZONTAL : xOffset);
    roi.x = roi.x + xOffset;
    HILOGI("start offset after ROI: %{public}s", roi.ToString().c_str());
}

void McCameraTrackingController::AdjustXOffset(ROI &roi, CameraType cameraType,
    float &offsetX, float &offsetY, bool &isFace)
{
    HILOGI("start offset before ROI: %{public}s", roi.ToString().c_str());
    float yOffset = offsetX;
    float xOffset = -offsetY;

    if (isFace) {
        if (cameraType == CameraType::FRONT) {
            yOffset -= FACE_OFFSET;
        } else if (cameraType == CameraType::BACK) {
            yOffset += FACE_OFFSET;
        }
    }
    yOffset = yOffset > UP_HORIZONTAL ? UP_HORIZONTAL : (yOffset < DOWN_HORIZONTAL ? DOWN_HORIZONTAL : yOffset);
    roi.y = roi.y + yOffset;
    
    float virtualCenterX = MIDDLE_VALUE_HORIZONTAL + xOffset;
    if (roi.x > virtualCenterX) {
        xOffset = (roi.x - virtualCenterX) / (UP_LIMIT - virtualCenterX) * (1 - MIDDLE_VALUE_HORIZONTAL);
    } else if (roi.x < virtualCenterX) {
        xOffset = (roi.x - virtualCenterX) / (virtualCenterX - DOWN_LIMIT) * (1 - MIDDLE_VALUE_HORIZONTAL);
    }
    xOffset = xOffset > UP_HORIZONTAL ? UP_HORIZONTAL : (xOffset < DOWN_HORIZONTAL ? DOWN_HORIZONTAL : xOffset);
    roi.x = MIDDLE_VALUE_HORIZONTAL + xOffset;
    HILOGI("start offset after ROI: %{public}s", roi.ToString().c_str());
}

void McCameraTrackingController::OnConnectChange()
{
    HILOGI("start.");
    if (currentCameraInfo_ != nullptr) {
        currentCameraInfo_->currentCameraTrackingLayout = CameraTrackingLayout::DEFAULT;
        HILOGI("currentCameraTrackingLayout: %{public}d", currentCameraInfo_->currentCameraTrackingLayout);
    }
    HILOGI("end.");
}

void McCameraTrackingController::ExecSearchTaskWithLimit(std::string &napiCmdId, uint32_t &tokenId,
    const bool &startFromNeg, const RotateDegreeLimit &limit)
{
    HILOGI("SEARCH_TARGET start. ");
    auto fun = [this, startFromNeg, limit, tokenId, napiCmdId]() mutable {
        currentCameraInfo_->searchingTarget = true;
        currentCameraInfo_->tokenId = tokenId;
        currentCameraInfo_->searchTargetNapiCmdId = napiCmdId;
        std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
        rotateParam->isRelative = false;
        rotateParam->duration = ROTATE_TO_START_TIME_USED;
        rotateParam->degree.pitch = 0;
        rotateParam->degree.roll = 0;
        rotateParam->degree.yaw = startFromNeg ? limit.negMax.yaw : limit.posMax.yaw;

        std::string taskId = SEARCH_TARGET_TASK_NAME;
        {
            std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
            std::map<int32_t, std::shared_ptr<MotionManager>> motionManagers =
                    MechBodyControllerService::GetInstance().motionManagers_;
            for (const auto &item: motionManagers) {
                int32_t mechId = item.first;
                std::shared_ptr<MotionManager> motionManager = item.second;
                if (motionManager == nullptr) {
                    HILOGE("motionManager not exist. mechId: %{public}d;", mechId);
                    continue;
                }
                int32_t result = motionManager->Rotate(rotateParam, tokenId, taskId);
                HILOGI("mech id: %{public}d result code: %{public}d", mechId, result);
            }
        }
        std::unique_lock<std::mutex> lock(searchTargetMutex_);
        searchTargetCv_.wait_for(lock, std::chrono::seconds(SEARCH_TARGET_ROTATE_TO_START_WAIT_TME),
                                 [this]() mutable {
                                     return currentCameraInfo_ != nullptr && currentCameraInfo_->trackingTargetNum > 0;
                                 });

        eventHandler_->PostTask([this, napiCmdId, taskId, tokenId, startFromNeg]() mutable {
            int32_t searchTimes = 0;
            ExecSearchTask(napiCmdId, tokenId, startFromNeg, searchTimes);
            }, SEARCH_TARGET_TASK_NAME);
    };

    if (eventHandler_ != nullptr) {
        HILOGE("SEARCH_TARGET Post ExecSearchTaskWithLimit");
        eventHandler_->RemoveTask(SEARCH_TARGET_TASK_NAME);
        eventHandler_->PostTask(fun, SEARCH_TARGET_TASK_NAME);
    }
    HILOGE("SEARCH_TARGET end.");
}

void McCameraTrackingController::ExecSearchTask(std::string &napiCmdId, uint32_t &tokenId, const bool &startFromNeg,
    int32_t &searchTimes)
{
    HILOGI("SEARCH_TARGET start.");
    if (eventHandler_ != nullptr) {
        eventHandler_->RemoveTask(SEARCH_TARGET_TASK_NAME);
    }
    std::string taskId = SEARCH_TARGET_TASK_NAME;
    HILOGI("SEARCH_TARGET current camera info: %{public}s", currentCameraInfo_->ToString().c_str());
    if ((currentCameraInfo_ == nullptr || currentCameraInfo_->trackingTargetNum == 0) &&
        searchTimes < SEARCH_TARGET_ROTATE_TIMES) {
        currentCameraInfo_->searchingTarget = true;
        currentCameraInfo_->tokenId = tokenId;
        currentCameraInfo_->searchTargetNapiCmdId = napiCmdId;
        std::shared_ptr<RotateBySpeedParam> rotateSpeedParam = std::make_shared<RotateBySpeedParam>();
        rotateSpeedParam->duration = SEARCH_TARGET_TASK_DURATION;
        rotateSpeedParam->speed.yawSpeed = startFromNeg ? SEARCH_TARGET_ROTATE_SPEED : -SEARCH_TARGET_ROTATE_SPEED;
        rotateSpeedParam->speed.rollSpeed = 0;
        rotateSpeedParam->speed.pitchSpeed = 0;
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
        std::map<int32_t, std::shared_ptr<MotionManager>> motionManagers =
                MechBodyControllerService::GetInstance().motionManagers_;
        for (const auto &item: motionManagers) {
            int32_t mechId = item.first;
            std::shared_ptr<MotionManager> motionManager = item.second;
            if (motionManager == nullptr) {
                HILOGE("motionManager not exist. mechId: %{public}d;", mechId);
                continue;
            }
            int32_t result = motionManager->RotateBySpeed(rotateSpeedParam, tokenId, taskId);
            HILOGI("mech id: %{public}d result code: %{public}d", mechId, result);
        }
        searchTimes++;
        eventHandler_->PostTask([this, napiCmdId, tokenId, startFromNeg, searchTimes]() mutable {
            ExecSearchTask(napiCmdId, tokenId, startFromNeg, searchTimes);
            }, SEARCH_TARGET_TASK_NAME, SEARCH_TARGET_TASK_INTERVAL);
        return;
    }
    SearchTargetStop();
    HILOGE("SEARCH_TARGET end.");
}

void MechSessionCallbackImpl::OnFocusTrackingInfo(CameraStandard::FocusTrackingMetaInfo info)
{
    McCameraTrackingController::GetInstance().OnFocusTracking(info);
}

void MechSessionCallbackImpl::OnCaptureSessionConfiged(CameraStandard::CaptureSessionInfo captureSessionInfo)
{
    McCameraTrackingController::GetInstance().OnCaptureSessionConfiged(captureSessionInfo);
}

void MechSessionCallbackImpl::OnZoomInfoChange(int sessionid, CameraStandard::ZoomInfo zoomInfo)
{
    McCameraTrackingController::GetInstance().OnZoomInfoChange(sessionid, zoomInfo);
}

void MechSessionCallbackImpl::OnSessionStatusChange(int32_t sessionid, bool status)
{
    McCameraTrackingController::GetInstance().OnSessionStatusChange(sessionid, status);
}

void MechSessionCallbackImpl::OnMetadataInfo(const std::shared_ptr<OHOS::Camera::CameraMetadata>& result)
{
    McCameraTrackingController::GetInstance().OnMetadataInfo(result);
}

} // namespace MechBodyController
} // namespace OHOS
