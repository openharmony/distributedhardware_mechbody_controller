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
#include "mc_camera_tracking_controller.h"
#include "mechbody_controller_service.h"
#include "mc_controller_ipc_death_listener.h"
#include "os_account_manager.h"
#include "mechbody_controller_utils.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "McCameraTrackingController";

    constexpr int32_t DOUBLE = 2;
    constexpr int32_t NUM_1 = 1;
    constexpr int32_t NUM_2 = 2;

    constexpr int32_t CONFIDENCE_LEVEL_LOST_END = 10;
    constexpr int32_t CONFIDENCE_LEVEL_LOW_END = 40;
    constexpr int32_t CONFIDENCE_LEVEL_MIDDEL_END = 70;

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

    constexpr float LAYOUT_LEFT = 0.3f;
    constexpr float LAYOUT_MIDDLE = 0.5f;
    constexpr float LAYOUT_RIGHT = 0.7f;

    constexpr int32_t DEGREE_CONSTANT_90 = 90;
    constexpr int32_t DEGREE_CONSTANT_180 = 180;
    constexpr int32_t DEGREE_CONSTANT_360 = 360;

    const size_t ROI_WINDOW_MAX_SIZE = 5;
    constexpr float TOUCH_CHAECK = 0.0001f;

    constexpr float TRACKING_LOST_CHECK = 0.0001f;
}

McCameraTrackingController& McCameraTrackingController::GetInstance()
{
    static auto instance = new McCameraTrackingController();
    return *instance;
}

McCameraTrackingController::McCameraTrackingController()
{
}

void McCameraTrackingController::Init()
{
    HILOGI("start");
    std::lock_guard<std::mutex> lock(cameraTrackingControllerInitMutex_);
    RegisterTrackingListener();
    RegisterSensorListener();
    HILOGI("end");
}

void McCameraTrackingController::UnInit()
{
    HILOGI("start");
    std::lock_guard<std::mutex> lock(cameraTrackingControllerInitMutex_);
    UnRegisterTrackingListener();
    UnRegisterSensorListener();
    HILOGI("end");
}

int32_t McCameraTrackingController::OnUsingAppChange(const CameraStandard::CameraAppInfo &cameraAppInfo)
{

    cameraAppInfo_ = cameraAppInfo;
    uint32_t tokenId = cameraAppInfo.tokenId;
    HILOGI("The app that is using the camera changes., app token id: %{public}s; ", GetAnonymUint32(tokenId).c_str());

    auto cameraInfo = CalculateCameraInfo(cameraAppInfo);
    if (!cameraInfo) {
        return INVALID_PARAMETERS_ERR;
    }
    currentCameraInfo_ = std::move(cameraInfo);
    HILOGI("camera info: %{public}s; ", currentCameraInfo_->ToString().c_str());
    std::shared_ptr<AppSetting> setting = std::make_shared<AppSetting>();
    if (appSettings.find(tokenId) == appSettings.end()) {
        HILOGW("No configuration information is saved. app token id: %{public}s;", GetAnonymUint32(tokenId).c_str());
    } else {
        setting = appSettings[tokenId];
    }

    int32_t recoverTrackingStatus = SetTrackingEnabled(tokenId, setting->isTrackingEnabled);
    if (recoverTrackingStatus != ERR_OK) {
        HILOGE("Failed to recover tracking status, app token id: %{public}s", GetAnonymUint32(tokenId).c_str());
        return recoverTrackingStatus;
    }

    int32_t recoverLayout = SetTrackingLayout(tokenId, setting->cameraTrackingLayout);
    if (recoverLayout != ERR_OK) {
        HILOGE("Failed to recover tracking layout, app token id: %{public}s", GetAnonymUint32(tokenId).c_str());
        return recoverLayout;
    }

    int32_t updateResult = UpdateMotionManagers(tokenId);
    return updateResult;
}

std::shared_ptr<CameraInfo> McCameraTrackingController::CalculateCameraInfo(
    const CameraStandard::CameraAppInfo &cameraAppInfo)
{
    CameraInfo cameraInfo;
    cameraInfo.width = cameraAppInfo.width;
    cameraInfo.height = cameraAppInfo.height;
    cameraInfo.zoomFactor = cameraAppInfo.zoomValue;
    cameraInfo.cameraType = cameraAppInfo.position == 1 ? CameraType::BACK : CameraType::FRONT;
    cameraInfo.isRecording = cameraAppInfo.videoStatus;

    float sensorWidthCut = 0.0f;
    float sensorHighCut = 0.0f;
    float sensorRatio = cameraInfo.sensorHeight != 0 ? cameraInfo.sensorWidth / cameraInfo.sensorHeight : 0;
    float viewRatio = cameraInfo.height > 0 ?
        static_cast<float>(cameraInfo.width) / static_cast<float>(cameraInfo.height) : 0;

    if (sensorRatio > viewRatio) {
        sensorHighCut = cameraInfo.sensorHeight;
        sensorWidthCut = cameraInfo.sensorHeight * viewRatio;
    } else {
        sensorWidthCut = cameraInfo.sensorWidth;
        sensorHighCut = cameraInfo.sensorWidth / (viewRatio > 0 ? viewRatio : 1);
    }

    auto focus = cameraAppInfo.equivalentFocus;
    uint8_t fovA = cameraInfo.fovH = DOUBLE * static_cast<uint8_t>(
        std::round(RadToDegree(std::atan(sensorWidthCut / (DOUBLE * focus * cameraInfo.zoomFactor)))));
    uint8_t fovB = cameraInfo.fovV = DOUBLE * static_cast<uint8_t>(
        std::round(RadToDegree(std::atan(sensorHighCut / (DOUBLE * focus * cameraInfo.zoomFactor)))));
    if (sensorRotation_ == MobileRotation::UP || sensorRotation_ == MobileRotation::DOWN) {
        HILOGI("device vertical");
        cameraInfo.fovH = (fovA < fovB) ? fovA : fovB;
        cameraInfo.fovV = (fovA < fovB) ? fovB : fovA;
    } else {
        HILOGI("device horizontal");
        cameraInfo.fovH = (fovA < fovB) ? fovB : fovA;
        cameraInfo.fovV = (fovA < fovB) ? fovA : fovB;
    }

    cameraInfo.currentCameraTrackingLayout = currentCameraInfo_->currentCameraTrackingLayout;
    cameraInfo.currentTrackingEnable = currentCameraInfo_->currentTrackingEnable;
    HILOGI("fovH %{public}d, fovV %{public}d", cameraInfo.fovH, cameraInfo.fovV);

    return std::make_shared<CameraInfo>(cameraInfo);
}

int32_t McCameraTrackingController::UpdateMotionManagers(uint32_t tokenId)
{
    const auto& motionManagers = MechBodyControllerService::GetInstance().motionManagers_;
    CameraInfoParams cameraInfoParams;
    cameraInfoParams.fovH = currentCameraInfo_->fovH;
    cameraInfoParams.fovV = currentCameraInfo_->fovV;
    cameraInfoParams.zoomFactor = currentCameraInfo_->zoomFactor;
    cameraInfoParams.isRecording = currentCameraInfo_->isRecording;
    cameraInfoParams.cameraType = currentCameraInfo_->cameraType;

    std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
    for (const auto &item : motionManagers) {
        int32_t mechId = item.first;
        std::shared_ptr<MotionManager> motionManager = item.second;
        if (!motionManager) {
            HILOGE("MotionManager not exits, tokenId: %{public}s; mechId: %{public}d",
                   GetAnonymUint32(tokenId).c_str(), mechId);
            continue;
        }

        int32_t result = motionManager->SetMechCameraInfo(cameraInfoParams);
        HILOGI("tokenId: %{public}s; mech id: %{public}d result code: %{public}d",
            GetAnonymUint32(tokenId).c_str(), mechId, result);
        if (result != ERR_OK) {
            return result;
        }
    }
    return ERR_OK;
}

int32_t McCameraTrackingController::OnFocusTracking(CameraStandard::FocusTrackingMetaInfo &info)
{
    HILOGI("OnFocusTracking.");
    if (!currentCameraInfo_->currentTrackingEnable) {
        HILOGW("current tracking is not enabled");
        return ERR_OK;
    }

    HILOGI("Tracking mode: %{public}d; "
           "Tracking region: [x: %{public}f, y: %{public}f, width: %{public}f, height: %{public}f]; "
           "Tracking object ID: %{public}d",
           info.GetTrackingMode(),
           info.GetTrackingRegion().topLeftX, info.GetTrackingRegion().topLeftY,
           info.GetTrackingRegion().width, info.GetTrackingRegion().height,
           info.GetTrackingObjectId());

    std::ostringstream oss;
    for (const auto& obj : info.GetDetectedObjects()) {
        oss << "Detected object ID: " << obj->GetObjectId()
            << ", Type: " << static_cast<int>(obj->GetType())
            << ", BBox: ["
            << obj->GetBoundingBox().topLeftX << ", "
            << obj->GetBoundingBox().topLeftY << ", "
            << obj->GetBoundingBox().width << ", "
            << obj->GetBoundingBox().height << "]"
            << ", Confidence: " << obj->GetConfidence();
    }
    HILOGI("tracking object info: %{public}s", oss.str().c_str());

    if (info.GetDetectedObjects().empty()) {
        HILOGW("No target tracking object! Id is: %{public}d", info.GetTrackingObjectId());
        return ERR_OK;
    }

    std::shared_ptr<TrackingFrameParams> trackingParams = BuildTrackingParams(info);
    if (!trackingParams) {
        HILOGE("Failed to build tracking params");
        return ERR_OK;
    }
    HILOGI("tracking param: %{public}s", trackingParams->ToString().c_str());
    if (trackingParams->roi.width <= TRACKING_LOST_CHECK ||
        trackingParams->roi.height <= TRACKING_LOST_CHECK) {
        HILOGW("tracking rect lost, width or height is zero.");
        return ERR_OK;
    }
    return UpdateMotionsWithTrackingData(trackingParams, info.GetTrackingObjectId());
}

std::shared_ptr<TrackingFrameParams> McCameraTrackingController::BuildTrackingParams(
    CameraStandard::FocusTrackingMetaInfo &info)
{
    CameraStandard::Rect trackingRegion = info.GetTrackingRegion();
    AddROIToWindow(trackingRegion);

    std::shared_ptr<TrackingFrameParams> trackingFrameParams = std::make_shared<TrackingFrameParams>();

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects = info.GetDetectedObjects();
    sptr<CameraStandard::MetadataObject> trackingTarget;
    int32_t selectResult = SelectTrackingTarget(detectedObjects, trackingTarget, trackingRegion);
    HILOGI("selectResult: %{public}d", selectResult);
    if (trackingTarget == nullptr || selectResult != ERR_OK) {
        return nullptr;
    }
    trackingFrameParams->timeStamp = static_cast<uint64_t>(trackingTarget->GetTimestamp());
    trackingFrameParams->targetId = trackingTarget->GetObjectId();
    int32_t confidence = trackingTarget->GetConfidence();
    if (confidence > CONFIDENCE_LEVEL_MIDDEL_END) {
        trackingFrameParams->confidence = ConfidenceLevel::HIGH;
    } else if (confidence > CONFIDENCE_LEVEL_LOW_END && confidence <= CONFIDENCE_LEVEL_MIDDEL_END) {
        trackingFrameParams->confidence = ConfidenceLevel::MEDIUM;
    } else if (confidence > CONFIDENCE_LEVEL_LOST_END && confidence <= CONFIDENCE_LEVEL_LOW_END) {
        trackingFrameParams->confidence = ConfidenceLevel::LOW;
    } else {
        trackingFrameParams->confidence = ConfidenceLevel::LOST;
    }
    CameraStandard::MetadataObjectType objectType = trackingTarget->GetType();
    ConvertObjectType(objectType, trackingFrameParams->objectType);

    CameraStandard::Rect trackingTargetRect = trackingTarget->GetBoundingBox();
    UpdateROI(trackingFrameParams, trackingTargetRect);
    trackingFrameParams->fovV = currentCameraInfo_->fovV;
    trackingFrameParams->fovH = currentCameraInfo_->fovH;
    trackingFrameParams->isRecording = currentCameraInfo_->isRecording;
    auto now = std::chrono::system_clock::now();
    int64_t now_millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    int64_t delta = now_millis - static_cast<int64_t>(trackingFrameParams->timeStamp);
    HILOGI("current time: %{public}zu", now_millis);
    HILOGI("target timestamp: %{public}zu", trackingFrameParams->timeStamp);
    if (delta < 0) {
        trackingFrameParams->timeDelay = 0;
    } else {
        if (delta > std::numeric_limits<uint32_t>::max()) {
            trackingFrameParams->timeDelay = std::numeric_limits<uint32_t>::max();
        } else {
            trackingFrameParams->timeDelay = static_cast<uint32_t>(delta);
        }
    }
    lastTrackingFrame_ = trackingFrameParams;
    return trackingFrameParams;
}

int32_t McCameraTrackingController::ParseRectToROI(CameraStandard::Rect &rect, ROI &roi)
{
    roi.width = rect.width - rect.topLeftX;
    roi.height = rect.height - rect.topLeftY;
    if (roi.width == 0 || roi.height == 0) {
        HILOGW("failed, tracking rect lost, width or height is zero.");
        return INVALID_PARAMETERS_ERR;
    }
    if(currentCameraInfo_->cameraType == CameraType::FRONT){
        if(sensorRotation_ == MobileRotation::UP || sensorRotation_ == MobileRotation::LEFT){
            roi.x = NUM_1 - rect.topLeftX - roi.width / NUM_2;
            roi.y = NUM_1 - rect.topLeftY - roi.height / NUM_2;
        }else if(sensorRotation_ == MobileRotation::RIGHT || sensorRotation_ == MobileRotation::DOWN){
            roi.x = rect.topLeftX + roi.width / NUM_2;
            roi.y = rect.topLeftY + rect.height / NUM_2;
        }
    }else if(currentCameraInfo_->cameraType == CameraType::BACK){
        if(sensorRotation_ == MobileRotation::UP || sensorRotation_ == MobileRotation::LEFT){
            roi.x = rect.topLeftX + roi.width / NUM_2;
            roi.y = rect.topLeftY + roi.height / NUM_2;
        }else if(sensorRotation_ == MobileRotation::RIGHT || sensorRotation_ == MobileRotation::DOWN){
            roi.x = NUM_1 - rect.topLeftX - roi.width / NUM_2;
            roi.y = NUM_1 - rect.topLeftY - roi.height / NUM_2;
        }
    }
    return ERR_OK;
}

void McCameraTrackingController::AddROIToWindow(CameraStandard::Rect &rect)
{
    ROI roi;
    if(ParseRectToROI(rect, roi) != ERR_OK){
        return;
    }
    std::lock_guard<std::mutex> lock(roiWindowMutex_);
    if (roiWindow_.size() >= ROI_WINDOW_MAX_SIZE) {
        roiWindow_.pop_front();
    }
    roiWindow_.push_back(roi);
}

bool McCameraTrackingController::IsSelectTrackingObject()
{
    float lastX = 0.0f;
    float lastY = 0.0f;
    float lastWidth = 0.0f;
    float lastHeight = 0.0f;
    std::lock_guard<std::mutex> lock(roiWindowMutex_);
    if(roiWindow_.size() < 2){
        return false;
    }
    for (const auto &item: roiWindow_) {
        if(lastX > 0 && std::abs(item.x - lastX) > TOUCH_CHAECK){
            return false;
        }
        if(lastX > 0 && std::abs(item.y - lastY) > TOUCH_CHAECK){
            return false;
        }
        if(lastX > 0 && std::abs(item.width - lastWidth) > TOUCH_CHAECK){
            return false;
        }
        if(lastX > 0 && std::abs(item.height - lastHeight) > TOUCH_CHAECK){
            return false;
        }
        lastX = item.x;
        lastY = item.y;
        lastWidth = item.width;
        lastHeight = item.height;
    }
    return true;
}

int32_t McCameraTrackingController::SelectTrackingTarget(std::vector<sptr<CameraStandard::MetadataObject>> &targets,
    sptr<CameraStandard::MetadataObject> &finalTarget, CameraStandard::Rect trackingRegion)
{
    if (IsSelectTrackingObject()) {
        HILOGI("is select tracking object: true");
        double registration = 0.0f;
        for (auto &item: targets) {
            if (!FilterDetectedObject(item)) {
                continue;
            }
            double iou = ComputeRegistration(item, trackingRegion);
            if (iou > registration) {
                registration = iou;
                finalTarget = item;
            }
        }
        if (finalTarget != nullptr) {
            return ERR_OK;
        }
    }
    ROI lastRoi = {0.0f, 0.0f, 0.0f, 0.0f};
    if (lastTrackingFrame_ != nullptr) {
        lastRoi = lastTrackingFrame_->roi;
    } else {
        ParseRectToROI(trackingRegion, lastRoi);
    }
    HILOGI("last roi: x = %{public}f; y = %{public}f;", lastRoi.x, lastRoi.y);
    float distance = 2.0f;
    ROI checkRect;
    for (auto &item: targets) {
        CameraStandard::Rect rect = item->GetBoundingBox();
        if (!FilterDetectedObject(item)) {
            continue;
        }
        HILOGI("find target: check rect [x: %{public}f, y: %{public}f];", rect.topLeftX, rect.topLeftY);
        if (ParseRectToROI(rect, checkRect) != ERR_OK) {
            continue;
        }
        float checkDistance = std::abs(lastRoi.x - checkRect.x) * std::abs(lastRoi.x - checkRect.x) +
                              std::abs(lastRoi.y - checkRect.y) * std::abs(lastRoi.y - checkRect.y);
        if (checkDistance < distance) {
            distance = checkDistance;
            finalTarget = item;
        }
    }
    return ERR_OK;
}

double McCameraTrackingController::ComputeRegistration(
    sptr<CameraStandard::MetadataObject> &target, CameraStandard::Rect &trackingRegion)
{
    CameraStandard::Rect rect = target->GetBoundingBox();
    double xmin = std::max(trackingRegion.topLeftX, rect.topLeftX);
    double ymin = std::max(trackingRegion.topLeftY, rect.topLeftY);
    double xmax = std::min(trackingRegion.width, rect.width);
    double ymax = std::min(trackingRegion.height, rect.height);

    double width = xmax - xmin;
    double height = ymax - ymin;
    double intersectionArea = (width > 0 && height > 0) ? width * height : 0.0;

    double trackingRegionArea = (trackingRegion.width - trackingRegion.topLeftX) *
                                (trackingRegion.height - trackingRegion.topLeftY);
    double rectArea = (rect.width - rect.topLeftX) * (rect.height - rect.topLeftY);

    double trackingRegionIou = (trackingRegionArea > 0) ? (intersectionArea / trackingRegionArea) : 0.0;
    double rectIou = (rectArea > 0) ? (intersectionArea / rectArea) : 0.0;
    double iou = std::max(trackingRegionIou, rectIou);
    return iou;
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
    if (currentCameraInfo_->cameraType == CameraType::FRONT) {
        if (sensorRotation_ == MobileRotation::UP || sensorRotation_ == MobileRotation::LEFT) {
            trackingFrameParams->roi.x = NUM_1 - rect.topLeftX - trackingFrameParams->roi.width / NUM_2;
            trackingFrameParams->roi.y = NUM_1 - rect.topLeftY - trackingFrameParams->roi.height / NUM_2;
        } else if (sensorRotation_ == MobileRotation::RIGHT || sensorRotation_ == MobileRotation::DOWN) {
            trackingFrameParams->roi.x = rect.topLeftX + trackingFrameParams->roi.width / NUM_2;
            trackingFrameParams->roi.y = rect.topLeftY + trackingFrameParams->roi.height / NUM_2;
        }
    } else if (currentCameraInfo_->cameraType == CameraType::BACK) {
        if (sensorRotation_ == MobileRotation::UP || sensorRotation_ == MobileRotation::LEFT) {
            trackingFrameParams->roi.x = rect.topLeftX + trackingFrameParams->roi.width / NUM_2;
            trackingFrameParams->roi.y = rect.topLeftY + trackingFrameParams->roi.height / NUM_2;
        } else if (sensorRotation_ == MobileRotation::RIGHT || sensorRotation_ == MobileRotation::DOWN) {
            trackingFrameParams->roi.x = NUM_1 - rect.topLeftX - trackingFrameParams->roi.width / NUM_2;
            trackingFrameParams->roi.y = NUM_1 - rect.topLeftY - trackingFrameParams->roi.height / NUM_2;
        }
    }
}

int32_t McCameraTrackingController::UpdateMotionsWithTrackingData(
const std::shared_ptr<TrackingFrameParams> &params, int32_t trackingObjectId)
{
    if (params == nullptr) {
        HILOGW("Params is null for tracking ID: %{public}d", trackingObjectId);
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

int32_t McCameraTrackingController::SetTrackingEnabled(const uint32_t &tokenId, bool &isEnabled)
{
    bool hasSuccess = false;
    {
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
        std::map<int32_t, std::shared_ptr<MotionManager>> motionManagers =
            MechBodyControllerService::GetInstance().motionManagers_;
        for (const auto &item: motionManagers){
            int32_t mechId = item.first;
            std::shared_ptr<MotionManager> motionManager = item.second;
            if (motionManager == nullptr) {
                HILOGE("MotionManager not exits, tokenId: %{public}s; mechId: %{public}d",
                    GetAnonymUint32(tokenId).c_str(), mechId);
                continue;
            }
            int32_t result = motionManager->SetMechCameraTrackingEnabled(isEnabled);
            hasSuccess = hasSuccess || (result == ERR_OK);
            HILOGI("tokenId: %{public}s; mech id: %{public}d result code: %{public}d",
                GetAnonymUint32(tokenId).c_str(), mechId,  result);
        }
    }
    {
        std::lock_guard<std::mutex> lock(appSettingsMutex_);
        if (appSettings.find(tokenId) == appSettings.end()) {
            std::shared_ptr<AppSetting> appSetting = std::make_shared<AppSetting>();
            appSettings[tokenId] = appSetting;
        }
        if (hasSuccess) {
            HILOGI("update tracking enable");
            appSettings[tokenId]->isTrackingEnabled = isEnabled;
            currentCameraInfo_->currentTrackingEnable = isEnabled;
        }
    }
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
        HILOGI("notify tracking event to tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
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

        if (!data.WriteInt32(static_cast<int32_t>(event))) {
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

int32_t McCameraTrackingController::SetTrackingLayout(CameraTrackingLayout &cameraTrackingLayout)
{
    std::shared_ptr<LayoutParams> layoutParam = std::make_shared<LayoutParams>();
    layoutParam->isDefault = cameraTrackingLayout == CameraTrackingLayout::DEFAULT;
    layoutParam->offsetX = LAYOUT_MIDDLE;
    layoutParam->offsetY = LAYOUT_MIDDLE;
    if (sensorRotation_ == MobileRotation::UP) {
        layoutParam->offsetY = ParseLayout(cameraTrackingLayout);
    }
    if (sensorRotation_ == MobileRotation::DOWN) {
        layoutParam->offsetY = ParseReverseLayout(cameraTrackingLayout);
    }
    if (sensorRotation_ == MobileRotation::LEFT) {
        layoutParam->offsetX = ParseReverseLayout(cameraTrackingLayout);
    }
    if (sensorRotation_ == MobileRotation::RIGHT) {
        layoutParam->offsetX = ParseLayout(cameraTrackingLayout);
    }
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
    int32_t result = SetTrackingLayout(cameraTrackingLayout);
    {
        std::lock_guard<std::mutex> lock(appSettingsMutex_);
        if (appSettings.find(tokenId) == appSettings.end()) {
            std::shared_ptr<AppSetting> appSetting = std::make_shared<AppSetting>();
            appSettings[tokenId] = appSetting;
        }
        if (result == ERR_OK) {
            appSettings[tokenId]->cameraTrackingLayout = cameraTrackingLayout;
        }
    }
    return ERR_OK;
}

int32_t McCameraTrackingController::GetTrackingLayout(const uint32_t &tokenId,
    CameraTrackingLayout &cameraTrackingLayout)
{
    HILOGI("tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    if (appSettings.find(tokenId) == appSettings.end()) {
        HILOGE("No configuration information is saved. app token id: %{public}s;", GetAnonymUint32(tokenId).c_str());
        cameraTrackingLayout = CameraTrackingLayout::DEFAULT;
        return ERR_OK;
    }

    std::shared_ptr<AppSetting> setting = appSettings[tokenId];
    if (setting == nullptr) {
        HILOGE("setting is nullptr. app token id: %{public}s;", GetAnonymUint32(tokenId).c_str());
        cameraTrackingLayout = CameraTrackingLayout::DEFAULT;
        return ERR_OK;
    }
    cameraTrackingLayout = setting->cameraTrackingLayout;
    return ERR_OK;
}

std::shared_ptr<CameraInfo> McCameraTrackingController::GetCurrentCameraInfo() const{
    return currentCameraInfo_;
}

void McCameraTrackingController::SensorCallback(SensorEvent* event){
    if (event == nullptr) {
        return;
    }
    GravityData* gravityData = reinterpret_cast<GravityData*>(event->data);
    MobileRotation sensorRotation = CalculateSensorRotation(gravityData);
    if (sensorRotation != MobileRotation::INVALID &&
        McCameraTrackingController::GetInstance().sensorRotation_ != sensorRotation) {
        HILOGI("update device position to: %{public}d;", static_cast<int32_t>(sensorRotation));
        McCameraTrackingController::GetInstance().sensorRotation_ = sensorRotation;
        std::shared_ptr<CameraInfo> currentCameraInfo =
            McCameraTrackingController::GetInstance().GetCurrentCameraInfo();
        if (currentCameraInfo == nullptr) {
            HILOGE("currentCameraInfo is nullptr;");
            return;
        }
        McCameraTrackingController::GetInstance().SetTrackingLayout(currentCameraInfo->currentCameraTrackingLayout);
        McCameraTrackingController::GetInstance().OnUsingAppChange(
            McCameraTrackingController::GetInstance().cameraAppInfo_);
    }
}

MobileRotation McCameraTrackingController::CalculateSensorRotation(GravityData* gravityData) {
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
        case CameraStandard::MetadataObjectType::INVALID :
            mechObjectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER);
            break;
        case CameraStandard::MetadataObjectType::FACE :
            mechObjectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE);
            break;
        case CameraStandard::MetadataObjectType::HUMAN_BODY :
            mechObjectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_HUMAN_BODY);
            break;
        case CameraStandard::MetadataObjectType::CAT_FACE :
            mechObjectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_ANIMAL_HEAD);
            break;
        case CameraStandard::MetadataObjectType::CAT_BODY :
            mechObjectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_ANIMAL_BODY);
            break;
        case CameraStandard::MetadataObjectType::DOG_FACE :
            mechObjectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_ANIMAL_HEAD);
            break;
        case CameraStandard::MetadataObjectType::DOG_BODY :
            mechObjectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_ANIMAL_BODY);
            break;
        case CameraStandard::MetadataObjectType::SALIENT_DETECTION :
            mechObjectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER);
            break;
        case CameraStandard::MetadataObjectType::BAR_CODE_DETECTION :
            mechObjectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER);
            break;
        case CameraStandard::MetadataObjectType::BASE_FACE_DETECTION :
            mechObjectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER);
            break;
        default:
            mechObjectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER);
    }
}

void McCameraTrackingController::RegisterTrackingListener()
{
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
        return;
    }
}

void McCameraTrackingController::UnRegisterTrackingListener()
{
    if(pMechSession == nullptr){
        return;
    }
    pMechSession->EnableMechDelivery(false);
    pMechSession->Release();
    pMechSession = nullptr;
}

void McCameraTrackingController::RegisterSensorListener()
{
    user.callback = McCameraTrackingController::SensorCallback;
    int32_t subscribeRet = SubscribeSensor(SENSOR_TYPE_ID_GRAVITY, &user);
    HILOGI("RegisterSensorCallback, subscribeRet: %{public}d", subscribeRet);
    int32_t setBatchRet = SetBatch(SENSOR_TYPE_ID_GRAVITY, &user, POSTURE_INTERVAL, 0);
    HILOGI("RegisterSensorCallback, setBatchRet: %{public}d", setBatchRet);
    int32_t activateRet = ActivateSensor(SENSOR_TYPE_ID_GRAVITY, &user);
    HILOGI("RegisterSensorCallback, activateRet: %{public}d", activateRet);
    HILOGI("success");
}

void McCameraTrackingController::UnRegisterSensorListener() {
    if (user.callback == nullptr) {
        return;
    }
    int32_t activateRet = DeactivateSensor(SENSOR_TYPE_ID_GRAVITY, &user);
    HILOGI("UnRegisterSensorCallback, activateRet: %{public}d", activateRet);
    int32_t subscribeRet = SubscribeSensor(SENSOR_TYPE_ID_GRAVITY, &user);
    HILOGI("UnRegisterSensorCallback, subscribeRet: %{public}d", subscribeRet);
    user.callback = nullptr;
    HILOGI("success");
}

float McCameraTrackingController::ParseLayout(CameraTrackingLayout &cameraTrackingLayout)
{
    if(cameraTrackingLayout == CameraTrackingLayout::LEFT){
        return LAYOUT_LEFT;
    }
    if(cameraTrackingLayout == CameraTrackingLayout::MIDDLE){
        return LAYOUT_MIDDLE;
    }
    if(cameraTrackingLayout == CameraTrackingLayout::RIGHT){
        return LAYOUT_RIGHT;
    }
    return LAYOUT_MIDDLE;
}

float McCameraTrackingController::ParseReverseLayout(CameraTrackingLayout &cameraTrackingLayout)
{
    if(cameraTrackingLayout == CameraTrackingLayout::LEFT){
        return LAYOUT_RIGHT;
    }
    if(cameraTrackingLayout == CameraTrackingLayout::MIDDLE){
        return LAYOUT_MIDDLE;
    }
    if(cameraTrackingLayout == CameraTrackingLayout::RIGHT){
        return LAYOUT_LEFT;
    }
    return LAYOUT_MIDDLE;
}

void MechSessionCallbackImpl::OnFocusTrackingInfo(CameraStandard::FocusTrackingMetaInfo info)
{
    McCameraTrackingController::GetInstance().OnFocusTracking(info);
}

void MechSessionCallbackImpl::OnCameraAppInfo(const std::vector<CameraStandard::CameraAppInfo> &cameraAppInfos)
{
    // There is currently only one application information.
    HILOGI("app info size: %{public}zu;", cameraAppInfos.size());
    if(!cameraAppInfos.empty()){
        HILOGI("app width: %{public}d; height: %{public}d", cameraAppInfos[0].width, cameraAppInfos[0].height);
        McCameraTrackingController::GetInstance().OnUsingAppChange(cameraAppInfos[0]);
    }
}

} // namespace MechBodyController
} // namespace OHOS
