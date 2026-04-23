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

#include "mc_motion_manager.h"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <iostream>

#include "mechbody_controller_log.h"
#include "mc_subscription_center.h"
#include "mechbody_controller_service.h"
#include "mc_camera_tracking_controller.h"
#include "mc_connect_manager.h"
#include "mechbody_controller_utils.h"
#include "hisysevent_utils.h"
#include <cmath>

namespace OHOS {
namespace MechBodyController {
namespace {
const std::string TAG = "MotionManager";
const std::string EVENT_THREAD_NAME = "MotionManager_EventHandler";
const std::string AISERVICE_BUNDLE = "com.example.hmos.aibase";
constexpr float DEGREE_CIRCLED_MAX = 6.28;
constexpr float DEGREE_CIRCLED_MIN = -6.28;
constexpr float DEFAULT_DURATION = 500;
static constexpr int32_t TRANS_TIME = 1000;
constexpr int32_t WHEEL_FILTER_COUNT = 2;
constexpr int32_t WHEEL_CORRECT_COUNT = 3;
constexpr float WHEEL_ZOOM_THRESHOLD = 10.0f;
constexpr int32_t WHEEL_ZOOM_01X_EVENT_NUM = 1;
constexpr int32_t WHEEL_ZOOM_1X_EVENT_NUM = 10;
constexpr float YAW_DISCREPANCY = 0.1f;
constexpr float MILLISECONDS_TO_SECONDS = 1000.0f;
constexpr float NO_LIMIT_MAX = 3.1415927f;
constexpr float NO_LIMIT_MIN = -3.1415926f;
constexpr int TASK_COMPLETED = 2;
constexpr int32_t RESPONSE_TIMEOUT = 300000;
constexpr float YAW_OFFSET = 0.2f;
constexpr int32_t CMD_SEND_INTERVAL = 100;
constexpr int32_t TRACKING_CHECKER_INTERVAL = 100;
constexpr int32_t MS_CONVERSION = 1000;
constexpr int32_t CM_CONVERSION = 10;

const std::map<CameraKeyEvent, int32_t> MAP_KEY_EVENT_VALUE = {
    {CameraKeyEvent::START_FILMING, MMI::KeyEvent::KEYCODE_VOLUME_UP},
    {CameraKeyEvent::SWITCH_CAMERA, MMI::KeyEvent::KEYCODE_VIDEO_NEXT},
    {CameraKeyEvent::ZOOM_IN, MMI::KeyEvent::KEYCODE_ZOOMIN},
    {CameraKeyEvent::ZOOM_OUT, MMI::KeyEvent::KEYCODE_ZOOMOUT},
    {CameraKeyEvent::SWITCH_PHOTO_FILM, MMI::KeyEvent::KEYCODE_VCR2}
};

std::map<SpeedGear, int16_t> DISTANCE_SPEED_MAP = {
    {SpeedGear::LOW_SPEED, 20},
    {SpeedGear::MIDDLE_SPEED, 50},
    {SpeedGear::HIGH_SPEED, 80},
};
 
std::map<SpeedGear, float> ANGLE_SPEED_MAP = {
    {SpeedGear::LOW_SPEED, 20.0},
    {SpeedGear::MIDDLE_SPEED, 50.0},
    {SpeedGear::HIGH_SPEED, 80.0},
};
 
constexpr float ANGLE_SPEED_DEFAULT = 50.0;

const std::vector<RotateParam> NOD_ACTIONS = {
    {{0.0f, 0.0f, -0.02f}, 100, false},
    {{0.0f, 0.0f, 0.2f}, 600, false},
    {{0.0f, 0.0f, 0.0f}, 500, false}
};
}

template<typename T>
T GetFinalSpeed(bool isFuShu, T speed)
{
    if (isFuShu) {
        return -speed;
    } else {
        return speed;
    }
}

static uint16_t SafeMsToUint16(uint64_t ms)
{
    return static_cast<uint16_t>(std::min<uint64_t>(ms, UINT16_MAX));
}

static uint64_t DistanceToMs(int32_t distanceCm, int16_t speedCmPerSec)
{
    if (speedCmPerSec <= 0) {
        return 0;
    }
    return static_cast<uint64_t>(std::abs(distanceCm)) * CM_CONVERSION * MS_CONVERSION / speedCmPerSec;
}

bool CheckRotatePointParam(EulerAngles rotateDegree)
{
    if (rotateDegree.yaw > DEGREE_CIRCLED_MAX || rotateDegree.yaw < DEGREE_CIRCLED_MIN) {
        HILOGE("The 'yaw' value is out of the specified range. %{public}f.", rotateDegree.yaw);
        return false;
    } else if (rotateDegree.roll > DEGREE_CIRCLED_MAX || rotateDegree.roll < DEGREE_CIRCLED_MIN) {
        HILOGE("The 'roll' value is out of the specified range. %{public}f.", rotateDegree.roll);
        return false;
    } else if (rotateDegree.pitch > DEGREE_CIRCLED_MAX || rotateDegree.pitch < DEGREE_CIRCLED_MIN) {
        HILOGE("The 'pitch' value is out of the specified range. %{public}f.", rotateDegree.pitch);
        return false;
    }
    return true;
}

void MotionManager::MechAttitudeNotify(const std::shared_ptr<CommonRegisterMechPositionInfoCmd> &cmd)
{
    HILOGD("Received gimbal attitude change event.");
    CHECK_POINTER_RETURN(cmd, "cmd");
    EulerAngles position = cmd->GetPosition();
    RotationAxesStatus status = status_;
    {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        if (deviceStatus_->eulerAngles != position) {
            HILOGI("New position: %{public}s", position.ToString().c_str());
            deviceStatus_->eulerAngles = position;
        }
    }
    HILOGD("notify end");
}

void MotionManager::MMIKeyEvent(CameraKeyEvent eventType)
{
    HILOGI("event: %{public}d.", eventType);
    auto it = MAP_KEY_EVENT_VALUE.find(eventType);
    if (it == MAP_KEY_EVENT_VALUE.end()) {
        HILOGW("The camera key event not found.");
        return;
    }
    int32_t keyCode = it->second;
    auto keyDown = CreateKeyEvent(keyCode, MMI::KeyEvent::KEY_ACTION_DOWN);
    auto keyUp = CreateKeyEvent(keyCode, MMI::KeyEvent::KEY_ACTION_UP);
    if (keyDown == nullptr || keyUp == nullptr) {
        HILOGW("Failed to create back key event");
        return;
    }
    MMI::InputManager::GetInstance()->SimulateInputEvent(keyDown);
    MMI::InputManager::GetInstance()->SimulateInputEvent(keyUp);
}

std::shared_ptr<MMI::KeyEvent> MotionManager::CreateKeyEvent(int32_t keyCode, int32_t keyAction)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    if (keyEvent == nullptr) {
        HILOGE("Failed to create key event.");
        return nullptr;
    }
    MMI::KeyEvent::KeyItem keyItem;
    keyItem.SetKeyCode(keyCode);
    keyItem.SetPressed(keyAction == MMI::KeyEvent::KEY_ACTION_DOWN);
    keyItem.SetDownTime(GetSysClockTime());
    keyEvent->SetKeyCode(keyCode);
    keyEvent->SetKeyAction(keyAction);
    keyEvent->AddPressedKeyItems(keyItem);
    return keyEvent;
}

int64_t MotionManager::GetSysClockTime()
{
    struct timespec ts = {0, 0};
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }
    return (ts.tv_sec * TRANS_TIME * TRANS_TIME) + (ts.tv_nsec / TRANS_TIME);
}

void MotionManager::MechButtonEventNotify(const std::shared_ptr<CommonRegisterMechKeyEventCmd> &cmd)
{
    CHECK_POINTER_RETURN(cmd, "cmd");
    CameraKeyEvent eventType = cmd->GetEvent();
    HILOGI("Received gimbal key press event. eventNo: %{public}d.", eventType);
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    CHECK_POINTER_RETURN(cmd, "KeyEvent");
    switch (eventType) {
        case CameraKeyEvent::START_FILMING :
            HILOGI("ButtonEvent START_FILMING.");
            MMIKeyEvent(CameraKeyEvent::START_FILMING);
            break;
        case CameraKeyEvent::SWITCH_CAMERA :
            HILOGI("ButtonEvent SWITCH_CAMERA.");
            MMIKeyEvent(CameraKeyEvent::SWITCH_CAMERA);
            break;
        case CameraKeyEvent::ZOOM_IN :
            HILOGI("ButtonEvent ZOOM_IN.");
            MMIKeyEvent(CameraKeyEvent::ZOOM_IN);
            break;
        case CameraKeyEvent::ZOOM_OUT :
            HILOGI("ButtonEvent ZOOM_OUT.");
            MMIKeyEvent(CameraKeyEvent::ZOOM_OUT);
            break;
        case CameraKeyEvent::SWITCH_PHOTO_FILM :
            HILOGI("ButtonEvent SWITCH_PHOTO_FILM.");
            MMIKeyEvent(CameraKeyEvent::SWITCH_PHOTO_FILM);
            break;
        default:
            HILOGW("ButtonEvent undefined action");
            break;
    }
    McCameraTrackingController::GetInstance().SetStickOffset(cmd->GetStickX(), cmd->GetStickY());
}

void MotionManager::MechParamNotify(const std::shared_ptr<CommonRegisterMechStateInfoCmd> &cmd)
{
    HILOGD("Received gimbal param change event.");
    CHECK_POINTER_RETURN(cmd, "cmd");
    MechStateInfo info = cmd->GetInfo();
    RotationAxesStatus status{};
    {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        if (deviceStatus_->stateInfo == info) {
            return;
        }
        HILOGI("mech info change, isPhoneOn %{public}d", info.isPhoneOn);
        deviceStatus_->stateInfo = info;
        if (MechConnectManager::GetInstance().GetMechState(mechId_) !=
            static_cast<AttachmentStateMap>(info.isPhoneOn)) {
            HandleMechPlacementChange(info.isPhoneOn);
        }
        // roll is not enable now, it will be available in the future
        switch (info.mechMode) {
            case MechMode::FOLLOW:
                status.yawEnabled = true;
                status.rollEnabled = false;
                status.pitchEnabled = true;
                break;
            case MechMode::PITCH_FREE:
                status.yawEnabled = true;
                status.rollEnabled = false;
                status.pitchEnabled = false;
                break;
            case MechMode::FPV:
            case MechMode::ROLL:
                status.yawEnabled = true;
                status.rollEnabled = false;
                status.pitchEnabled = true;
                break;
            case MechMode::FREE:
                status.yawEnabled = false;
                status.rollEnabled = false;
                status.pitchEnabled = false;
                break;
            default:
                break;
        }
        if (deviceStatus_->rotationAxesStatus == status) {
            return;
        }
        deviceStatus_->rotationAxesStatus = status;
    }
    HILOGI("notify");
    MechBodyControllerService::GetInstance().OnRotationAxesStatusChange(mechId_, status);
}


void MotionManager::MechGenericEventNotify(const std::shared_ptr<NormalRegisterMechGenericEventCmd>& cmd)
{
    HILOGD("Received generic param change event.");
    CHECK_POINTER_RETURN(cmd, "cmd");
    if (cmd->GetLowPower() == BIT_1) {
        MechBodyControllerService::GetInstance().NotifyMechEvent(mechId_, MechEventType::LOW_POWER);
        HILOGI("notify low battery event");
    }
    DeviceStateInfo info = cmd->GetParams();
    RotationAxesStatus status{};
    {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        if (info.attached != static_cast<uint8_t>(-1)) {
            HILOGI("mech info change, isPhoneOn %{public}d", info.attached);
            deviceStatus_->stateInfo.isPhoneOn = info.attached;
            if (MechConnectManager::GetInstance().GetMechState(mechId_) !=
                static_cast<AttachmentStateMap>(info.attached)) {
                HandleMechPlacementChange(info.attached);
            }
        }
        if (info.yawDisable == static_cast<uint8_t>(-1)) {
            return;
        }
        status.yawEnabled = !bool(info.yawDisable);
        status.rollEnabled = !bool(info.rollDisable);
        status.pitchEnabled = !bool(info.pitchDisable);
        if (deviceStatus_->rotationAxesStatus == status) {
            return;
        }
        deviceStatus_->rotationAxesStatus = status;
    }
    HILOGI("notify");
    MechBodyControllerService::GetInstance().OnRotationAxesStatusChange(mechId_, status);
}

void MotionManager::MechCliffInfoNotify(const std::shared_ptr<RegisterMechCliffInfoCmd>& cmd)
{
    HILOGI("notify");
    MechBodyControllerService::GetInstance().NotifyMechEvent(mechId_, MechEventType::REACH_CLIFF);
    HILOGI("notify end");
}

void MotionManager::MechObstacleInfoNotify(const std::shared_ptr<RegisterMechObstacleInfoCmd>& cmd)
{
    HILOGI("notify");
    MechBodyControllerService::GetInstance().NotifyMechEvent(mechId_, MechEventType::REACH_OBSTACLE);
    HILOGI("notify end");
}

void MotionManager::HandleMechPlacementChange(bool isPhoneOn)
{
    HILOGI("start, isPhoneOn = %{public}d.", isPhoneOn);
    if (!isPhoneOn) {
        HandlePhoneOff(isPhoneOn);
    } else {
        HandlePhoneOn(isPhoneOn);
    }
    HILOGI("end.");
}

void MotionManager::HandlePhoneOff(bool isPhoneOn)
{
    auto hidCmd = factory.CreateSetMechHidPreemptiveCmd(true);
    CHECK_POINTER_RETURN(hidCmd, "hidCmd is empty.");

    auto callback = [weakThis = std::weak_ptr<MotionManager>(shared_from_this()), hidCmd, isPhoneOn]() {
        auto sharedThis = weakThis.lock();
        if (!sharedThis) {
            return;
        }
        uint8_t result = hidCmd->GetResult();
        HILOGI("SetMechHidPreemptiveCmd result: %{public}u.", result);
        MechConnectManager::GetInstance().NotifyMechState(sharedThis->mechId_, isPhoneOn);
    };
    hidCmd->SetResponseCallback(callback);
    CHECK_POINTER_RETURN(sendAdapter_, "sendAdapter_ is null.");
    sendAdapter_->SendCommand(hidCmd);
    MechBodyControllerService::GetInstance().NotifyMechEvent(mechId_, MechEventType::DEVICE_UNADSORBED);
    MechConnectManager::GetInstance().NotifyMechState(mechId_, isPhoneOn);
}

void MotionManager::HandlePhoneOn(bool isPhoneOn)
{
    if (deviceBaseInfo_.devType == static_cast<uint8_t>(MechType::WHEEL_BASE)) {
        ConnectAbility();
    }
    MechBodyControllerService::GetInstance().NotifyMechEvent(mechId_, MechEventType::DEVICE_ADSORBED);
    MechConnectManager::GetInstance().NotifyMechState(mechId_, isPhoneOn);
}

void MotionManager::ProcessPhoneOffForegroundCheck()
{
    auto appManager = GetAppManagerInstance();
    if (appManager == nullptr) {
        HILOGE("appManager is null, cannot check foreground app");
        return;
    }

    std::vector<AppExecFwk::AppStateData> foregroundApps;
    int32_t ret = appManager->GetForegroundApplications(foregroundApps);
    if (ret != 0) {
        HILOGE("Failed to get foreground applications, ret: %{public}d", ret);
        return;
    }
}

void MotionManager::ProcessPhoneOnForegroundCheck()
{
    auto appManager = GetAppManagerInstance();
    if (appManager == nullptr) {
        HILOGE("appManager is null, cannot check desktop scene");
        return;
    }

    std::vector<AppExecFwk::AppStateData> foregroundApps;
    int32_t ret = appManager->GetForegroundApplications(foregroundApps);
    if (ret != 0) {
        HILOGE("Failed to get foreground applications, ret: %{public}d", ret);
        return;
    }

    if (IsDesktopScene(foregroundApps)) {
        HILOGI("Desktop scene detected, start AI dispatch service directly");
        ConnectAbility();
    } else {
        HILOGI("Non-desktop scene, send notification to start AI dispatch service");
    }
}

sptr<AppExecFwk::IAppMgr> MotionManager::GetAppManagerInstance()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        HILOGE("Failed to get samgr");
        return nullptr;
    }
    sptr<IRemoteObject> object = samgr->GetSystemAbility(APP_MGR_SERVICE_ID);
    return iface_cast<AppExecFwk::IAppMgr>(object);
}

bool MotionManager::IsAiDispatchServiceInForeground(const std::vector<AppExecFwk::AppStateData> &list)
{
    for (const AppExecFwk::AppStateData &appStateData : list) {
        if (AppExecFwk::AppStateData::IsUIExtension(appStateData.extensionType)) {
            continue;
        }
        if (appStateData.bundleName == AISERVICE_BUNDLE && appStateData.isFocused) {
            HILOGI("AI dispatch service found in foreground and focused, bundleName: %{public}s,"
                "state: %{public}d, pid: %{public}d",
                appStateData.bundleName.c_str(), appStateData.state, appStateData.pid);
            return true;
        }
    }
    return false;
}

bool MotionManager::IsDesktopScene(const std::vector<AppExecFwk::AppStateData> &list)
{
    HILOGI("start");
    const std::string SCENEBOARD_BUNDLE_NAME = "com.ohos.sceneboard";
    bool hasNonUIExtensionFocus = false;
    bool hasSceneboardFocus = false;

    for (const AppExecFwk::AppStateData &appStateData : list) {
        if (appStateData.isFocused) {
            if (appStateData.bundleName == SCENEBOARD_BUNDLE_NAME) {
                hasSceneboardFocus = true;
            } else if (!AppExecFwk::AppStateData::IsUIExtension(appStateData.extensionType)) {
                hasNonUIExtensionFocus = true;
                HILOGI("Not desktop scene, focused on non-UIExtension app: %{public}s",
                    appStateData.bundleName.c_str());
            }
        }
    }
    if (hasNonUIExtensionFocus) {
        return false;
    }
    if (hasSceneboardFocus) {
        HILOGI("Is desktop scene, desktop is focused");
        return true;
    }
    for (const AppExecFwk::AppStateData &appStateData : list) {
        if (!AppExecFwk::AppStateData::IsUIExtension(appStateData.extensionType) &&
            appStateData.bundleName != SCENEBOARD_BUNDLE_NAME) {
            HILOGI("Not desktop scene, found non-UIExtension app: %{public}s", appStateData.bundleName.c_str());
            return false;
        }
    }

    HILOGI("Is desktop scene (only UIExtensions or sceneboard in foreground)");
    return true;
}

void MotionManager::ConnectServiceExtension(
    AAFwk::WantParams wantParams)
{
    HILOGI("connect service extension start");
    AAFwk::Want want;
    AppExecFwk::ElementName element("", "com.example.hmos.aibase", "WakeUpExtAbility", "");
    HILOGI("connect service extension bundleName:com.example.hmos.aibase, abilityName:example");
    want.SetElement(element);
    want.SetParams(wantParams);
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    int32_t ret = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    HILOGI("connect service extension startAbility ret: %{public}d", ret);
    IPCSkeleton::SetCallingIdentity(identity);
}

int32_t MotionManager::ConnectAbility()
{
    HILOGI("connect ability start");
    AAFwk::WantParams wantParams;
    wantParams.SetParam("launch_type", AAFwk::String::Box("robot_wakeup"));
    wantParams.SetParam("mechDeviceType", AAFwk::String::Box(""));
    wantParams.SetParam("mechId", AAFwk::Integer::Box(0));
    wantParams.SetParam("mechName", AAFwk::String::Box(""));
    ConnectServiceExtension(wantParams);
    return ERR_OK;
}

void MotionManager::MechExecutionResultNotify(const std::shared_ptr<RegisterMechControlResultCmd> &cmd)
{
    HILOGD("start.");
    if (cmd == nullptr) {
        HILOGW("Result cmd is empty.");
        return;
    }
    uint16_t result = cmd->GetControlResult();
    uint16_t taskId = cmd->GetTaskId();
    HILOGD("Notify callback id: %{public}d", taskId);
    {
        std::unique_lock <std::mutex> lock(seqCallbackMutex_);
        if (seqCallbacks_.empty()) {
            HILOGI("The callback is empty.");
            return;
        }
        auto it = seqCallbacks_.find(taskId);
        if (it == seqCallbacks_.end()) {
            HILOGI("The callback is not found.");
            return;
        }
        MechNapiCommandCallbackInfo callbackInfo = it->second;
        HILOGI("callback info: %{public}s", callbackInfo.ToString().c_str());
        if (result != TASK_COMPLETED) {
            HILOGW("Task is running.");
            return;
        }
        std::string taskName = EVENT_THREAD_NAME + std::to_string(taskId);
        if (eventHandler_ != nullptr) {
            eventHandler_->RemoveTask(taskName);
        }
        if (callbackInfo.willLimitChange) {
            MechBodyControllerService::GetInstance().OnRotationAxesStatusChange(mechId_, status_);
        }
        if (IsLimited()) {
            HILOGI("Task is limit.");
            MechBodyControllerService::GetInstance()
                .NotifyOperationResult(callbackInfo.tokenId, callbackInfo.napiCmdId, ExecResult::LIMITED);
        } else {
            HILOGD("Task completed.");
            MechBodyControllerService::GetInstance().NotifyOperationResult(callbackInfo.tokenId,
                callbackInfo.napiCmdId, ExecResult::COMPLETED);
        }
    }
}

void MotionManager::MechWheelZoomNotify(const std::shared_ptr<RegisterMechWheelDataCmd> &cmd)
{
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on.");
        return;
    }

    CHECK_POINTER_RETURN(cmd, "cmd");
    WheelData data = cmd->GetWheelData();
    auto func = [this, data]() {
        HILOGI("MechWheelZoomNotify wheel data: degree %{public}d, speed %{public}d, speed_ratio %{public}u",
            data.degree, data.speed, data.speed_ratio);
        if (data.degree == 0) {
            HILOGI("MechWheelZoomNotify degree is zero");
            return;
        }
        if (wheelFilterCnt_ < WHEEL_FILTER_COUNT) {
            HILOGI("MechWheelZoomNotify wheelFilterCnt_ is %{public}d.", wheelFilterCnt_);
            wheelFilterCnt_++;
            return;
        }
        wheelFilterCnt_ = 0;

        std::shared_ptr<CameraInfo> cameraInfo = McCameraTrackingController::GetInstance().GetCurrentCameraInfo();
        if (cameraInfo == nullptr) {
            HILOGE("MechWheelZoomNotify current camera info is nullptr");
            return;
        }
        float currentZoom = cameraInfo->zoomFactor;
        int32_t keyEventNum = WHEEL_ZOOM_01X_EVENT_NUM;
        if (currentZoom > WHEEL_ZOOM_THRESHOLD + FLOAT_EPSILON) {
            int32_t left = static_cast<int32_t>(currentZoom * DECIMAL_SCALE) % DECIMAL_SCALE;
            HILOGI("MechWheelZoomNotify left is %{public}d, wheelCorretCnt_ %{public}d.", left, wheelCorretCnt_);
            if (left == 0) {
                keyEventNum = WHEEL_ZOOM_1X_EVENT_NUM;
                wheelCorretCnt_ = 0;
            } else if (wheelCorretCnt_ < WHEEL_CORRECT_COUNT) {
                keyEventNum = WHEEL_ZOOM_1X_EVENT_NUM;
                wheelCorretCnt_++;
            } else {
                keyEventNum = WHEEL_ZOOM_1X_EVENT_NUM - left;
                wheelCorretCnt_ = 0;
            }
        }

        for (int32_t i = 0; i < keyEventNum; i++) {
            data.speed > 0 ? MMIKeyEvent(CameraKeyEvent::ZOOM_OUT) : MMIKeyEvent(CameraKeyEvent::ZOOM_IN);
        }
    };
    CHECK_POINTER_RETURN(eventHandler_, "eventHandler_");
    eventHandler_->PostTask(func);
    return;
}

void ReportFocustrackingStartEvent()
{
    uint32_t tokenId = McCameraTrackingController::GetInstance().GetTokenIdOfCurrentCameraInfo();
    if (tokenId == 0) {
        // No token: The yuntai is connected and there are no camera or other applications open.
        // Simply click on the tracking switch on the yuntai and come here without a token ID
        HILOGI("tokenId not exist, report.");
        HisyseventUtils::DotReportFocustrackingStartEvent("");
    } else {
        MechBodyControllerService::JudgeAppEnableSwitchAndReportFocustrackingStartEvent(tokenId);
    }
}

void MotionManager::MechTrackingStatusNotify(const std::shared_ptr<CommonRegisterMechTrackingEnableCmd> &cmd)
{
    CHECK_POINTER_RETURN(cmd, "cmd");
    bool isEnabled = cmd->GetIsEnabled();
    HILOGI("Received mech tracking status notify, isEnabled: %{public}d.", isEnabled);
    TrackingEvent trackingEvent;
    std::shared_ptr<CommonSetMechCameraTrackingEnableCmd> tkCmd = nullptr;
    {
        std::unique_lock<std::mutex> lock(deviceStatusMutex_);
        CHECK_POINTER_RETURN(deviceStatus_, "deviceStatus_ is empty.");
        if (!isEnabled) {
            trackingEvent = TrackingEvent::CAMERA_TRACKING_USER_DISABLED;
            deviceStatus_->isEnabled = false;
            deviceStatus_->trackingStatus = MechTrackingStatus::MECH_TK_DISABLE;
            tkCmd = factory.CreateSetMechCameraTrackingEnableCmd(deviceStatus_->trackingStatus);
            HILOGI("tkCmd is:MECH_TK_DISABLE.");
        } else {
            trackingEvent = TrackingEvent::CAMERA_TRACKING_USER_ENABLED;
            deviceStatus_->isEnabled = true;
            deviceStatus_->trackingStatus = MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET;
            tkCmd = factory.CreateSetMechCameraTrackingEnableCmd(deviceStatus_->trackingStatus);
            HILOGI("tkCmd is:MECH_TK_ENABLE_NO_TARGET.");
            ReportFocustrackingStartEvent();
        }
    }
    CHECK_POINTER_RETURN(sendAdapter_, "sendAdapter_ is empty.");
    sendAdapter_->SendCommand(tkCmd);
    McControllerManager::GetInstance().OnTrackingEvent(mechId_, trackingEvent);
}

uint8_t MotionManager::CreateResponseTaskId()
{
    HILOGI("called");
    std::unique_lock<std::shared_mutex> responseReadLock(taskIdMutex_);
    if (lastTaskId_ >= UINT8_MAX) {
        HILOGE("the taskId is full, reorder the taskId.");
        lastTaskId_ = 0;
        return 0;
    }
    uint8_t taskId = ++lastTaskId_;
    HILOGI("end, new taskId is %{public}d", taskId);
    return taskId;
}

uint16_t MotionManager::CreateRotateTaskId()
{
    HILOGI("called");
    std::unique_lock<std::shared_mutex> responseReadLock(taskIdMutex_);
    if (taskId_ >= UINT16_MAX) {
        HILOGE("the taskId is full, reorder the taskId.");
        taskId_ = 1;
        return 1;
    }
    uint16_t taskId = ++taskId_;
    HILOGI("end, new taskId is %{public}d", taskId);
    return taskId;
}

bool MotionManager::IsLimited()
{
    HILOGI("isLimited.");
    {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        // The roll axis data is inaccurate and should not be used as the current judgment criterion.
        if (deviceStatus_->rotationAxesStatus.pitchLimited == RotationAxisLimited::NOT_LIMITED &&
            deviceStatus_->rotationAxesStatus.yawLimited == RotationAxisLimited::NOT_LIMITED) {
            return false;
        }
    }
    return true;
}

bool MotionManager::IsLimited(const float &negMax, const float &posMax, const float &position)
{
    if (negMax < 0 && posMax < 0 && position < negMax && position > posMax) {
        return true;
    }
    if (negMax > 0 && posMax > 0 && position < negMax && position > posMax) {
        return true;
    }
    if (negMax < 0 && posMax > 0) {
        if (position > 0 && position > posMax) {
            return true;
        }
        if (position < 0 && position < negMax) {
            return true;
        }
    }
    return false;
}

void SetTimeout()
{
    HILOGI("MotionManager start.");
}

MotionManager::MotionManager(const std::shared_ptr<TransportSendAdapter> sendAdapter, int32_t mechId,
    bool isFirstConnect, const uint32_t &deviceIdentifier)
    : sendAdapter_(sendAdapter), mechId_(mechId), isFirstConnect_(isFirstConnect), deviceIdentifier_(deviceIdentifier)
{
    HILOGI("MotionManager start.");

    deviceStatus_ = std::make_shared<DeviceStatus>();
    deviceStatus_->rotateSpeedLimit.speedMin.yawSpeed = DEGREE_CIRCLED_MIN;
    deviceStatus_->rotateSpeedLimit.speedMin.rollSpeed = DEGREE_CIRCLED_MIN;
    deviceStatus_->rotateSpeedLimit.speedMin.pitchSpeed = DEGREE_CIRCLED_MIN;
    deviceStatus_->rotateSpeedLimit.speedMax.yawSpeed = DEGREE_CIRCLED_MAX;
    deviceStatus_->rotateSpeedLimit.speedMax.rollSpeed = DEGREE_CIRCLED_MAX;
    deviceStatus_->rotateSpeedLimit.speedMax.pitchSpeed = DEGREE_CIRCLED_MAX;

    HILOGI("MotionManager end.");
}

int32_t MotionManager::Init()
{
    HILOGI("MotionManager Init start");

    int32_t result = GetProtocolVer();
    if (result == MECH_CONNECT_FAILED) {
        return MECH_CONNECT_FAILED;
    }

    factory.SetFactoryProtocolVer(protocolVer_);
    if (protocolVer_ == 0x01) {
        GetMechRealName();
    } else if (protocolVer_ >= 0x02) {
        SetProtocolVer();
        int32_t ret = GetDeviceBaseInfo();
        if (ret != ERR_OK) {
            HILOGE("GetDeviceBaseInfo failed");
            return ret;
        }
        GetDeviceCapabilityInfo();
        if (deviceBaseInfo_.devType == static_cast<uint8_t>(MechType::WHEEL_BASE)) {
            GetWheelCapabilityInfo();
            if (isFirstConnect_) {
                SetDevicePairingInfo(deviceIdentifier_);
            }
        }
        GetDeviceStateInfo();
    } else {
        HILOGW("Unknown protocol version: %{public}d", protocolVer_);
    }
    factory.SetFactoryDevType(deviceBaseInfo_.devType);
    GetMechLimitInfo();
    if (protocolVer_ == 0x01) {
        auto hidCmd = factory.CreateSetMechHidPreemptiveCmd(false);
        CHECK_POINTER_RETURN_VALUE(hidCmd, MECH_CONNECT_FAILED, "hidCmd is empty.");
        sendAdapter_->SendCommand(hidCmd, CMD_SEND_INTERVAL);
    }

    SetMechTkEnableNoTarget();

    PerformPresetAction(PresetAction::NOD, CMD_SEND_INTERVAL * BIT_OFFSET_3);
    eventThread_ = std::thread(&MotionManager::StartEvent, this);
    std::unique_lock<std::mutex> lock(eventMutex_);
    eventCon_.wait(lock, [this] {
        return eventHandler_ != nullptr;
    });

    startTrackingChecker_ = true;
    trackingCheckerThread_ = std::make_unique<std::thread>(&MotionManager::TrackingChecker, this);
    HILOGI("MotionManager Init end.");
    return ERR_OK;
}

void MotionManager::SetMechTkEnableNoTarget()
{
    HILOGI("start");

    auto tkCmd = factory.CreateSetMechCameraTrackingEnableCmd(MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET);
    auto tkCallback = [this, tkCmd]() {
        deviceStatus_->trackingStatus = MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET;
    };
    tkCmd->SetResponseCallback(tkCallback);

    CHECK_POINTER_RETURN(tkCmd, "tkCmd is empty.");
    sendAdapter_->SendCommand(tkCmd, CMD_SEND_INTERVAL * BIT_OFFSET_2);
}

void MotionManager::TrackingChecker()
{
    HILOGI("TrackingChecker thread started");
    while (startTrackingChecker_.load()) {
        bool shouldProcessTracking = false;
        {
            std::unique_lock<std::mutex> lock(deviceStatusMutex_);
            shouldProcessTracking = (deviceStatus_->trackingStatus == MechTrackingStatus::MECH_TK_ENABLE_WITH_TARGET);
        }
        
        if (shouldProcessTracking) {
            ProcessTrackingStatus();
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void MotionManager::ProcessTrackingStatus()
{
    auto currentTime = std::chrono::steady_clock::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTrackingFrameTime_).count();
    if (timeDiff <= TRACKING_CHECKER_INTERVAL) {
        return;
    }
    std::shared_ptr<CommonSetMechCameraTrackingEnableCmd> tkCmd = nullptr;
    {
        std::unique_lock<std::mutex> lock(deviceStatusMutex_);
        deviceStatus_->trackingStatus = MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET;
        tkCmd = factory.CreateSetMechCameraTrackingEnableCmd(deviceStatus_->trackingStatus);
    }
    
    HILOGI("Tracking status set to MECH_TK_ENABLE_NO_TARGET due to inactivity.");
    if (sendAdapter_ != nullptr) {
        sendAdapter_->SendCommand(tkCmd);
    }
}

int32_t MotionManager::GetProtocolVer()
{
    HILOGI("start");
    std::shared_ptr<GetMechProtocolVerCmd> protocolVerCmd = factory.CreateGetMechProtocolVerCmd();
    CHECK_POINTER_RETURN_VALUE(protocolVerCmd, MECH_CONNECT_FAILED, "protocolVerCmd is empty.");
    auto protocolVerCallback = [this, protocolVerCmd]() {
        std::lock_guard<std::mutex> lock(protocolVerMutex_);
        protocolVerCon_.notify_all();
        protocolVer_ = protocolVerCmd->GetParams();
        HILOGI("device callback protocol version: %{public}d", protocolVer_);
        };

    protocolVerCmd->SetResponseCallback(protocolVerCallback);
    protocolVerCmd->SetTimeoutCallback(SetTimeout);
    CHECK_POINTER_RETURN_VALUE(sendAdapter_, MECH_CONNECT_FAILED, "sendAdapter_");
    sendAdapter_->SendCommand(protocolVerCmd);
    {
        std::unique_lock<std::mutex> lock(protocolVerMutex_);

        bool protocolVerReady = protocolVerCon_.wait_for(lock, std::chrono::seconds(3), [this] {
            return protocolVer_ >= static_cast<uint8_t>(ProtocolVersion::V01);
        });
        if (!protocolVerReady) {
            MechConnectManager::GetInstance().NotifyMechState(mechId_, false);
            return MECH_CONNECT_FAILED;
        }
    }
    HILOGI("end");
    return ERR_OK;
}

void MotionManager::SetProtocolVer()
{
    HILOGI("start");
    std::shared_ptr<NormalSetMechProtocolVerCmd> protocolVerCmd = factory.CreateNormalSetMechProtocolVerCmd(0x02);
    CHECK_POINTER_RETURN(protocolVerCmd, "protocolVerCmd is empty.");

    protocolVerCmd->SetTimeoutCallback(SetTimeout);
    CHECK_POINTER_RETURN(sendAdapter_, "sendAdapter_");
    sendAdapter_->SendCommand(protocolVerCmd);
    HILOGI("end");
}

void MotionManager::GetMechRealName()
{
    HILOGI("start");
    std::shared_ptr<GetMechRealNameCmd> realNameCmd = factory.CreateGetMechRealNameCmd();
    CHECK_POINTER_RETURN(realNameCmd, "RealNameCmd is empty.");
    auto nameCallback = [this, realNameCmd]() {
        std::string realName = realNameCmd->GetParams();
        SetDeviceRealName(realName);
        HILOGI("device callback real name: %{public}s", GetAnonymStr(realName).c_str());
    };

    realNameCmd->SetResponseCallback(nameCallback);
    realNameCmd->SetTimeoutCallback(SetTimeout);
    CHECK_POINTER_RETURN(sendAdapter_, "sendAdapter_");
    sendAdapter_->SendCommand(realNameCmd);
    HILOGI("end");
}

int32_t MotionManager::GetDeviceBaseInfo()
{
    HILOGI("start");
    std::shared_ptr<NormalGetMechBaseInfoCmd> baseInfoCmd = factory.CreateGetMechBaseInfoCmd();
    CHECK_POINTER_RETURN_VALUE(baseInfoCmd, MECH_CONNECT_FAILED, "baseInfoCmd is empty.");

    auto baseInfoCallback = [this, baseInfoCmd]() {
        deviceBaseInfo_ = baseInfoCmd->GetParams();
        deviceRealName_ = baseInfoCmd->GetParams().realName;
        HILOGI("device callback real name: %{public}s, devType: %{public}d",
            GetAnonymStr(deviceRealName_).c_str(), deviceBaseInfo_.devType);
        if (deviceBaseInfo_.devType != static_cast<uint8_t>(MechType::PORTABLE_GIMBAL) &&
            deviceBaseInfo_.devType != static_cast<uint8_t>(MechType::WHEEL_BASE)) {
            std::shared_ptr<SetMechDisconnectCmd> disconnectCmd = factory.CreateSetMechDisconnectCmd(0x00);
            CHECK_POINTER_RETURN(disconnectCmd, "disconnectCmd is empty.");
            sendAdapter_->SendCommand(disconnectCmd);
        }
        MechConnectManager::GetInstance().SetMechType(mechId_, static_cast<MechType>(deviceBaseInfo_.devType));
        {
            std::lock_guard<std::mutex> lock(deviceBaseInfoMutex_);
            deviceBaseInfoReady_ = true;
        }
        deviceBaseInfoCon_.notify_all();
    };

    baseInfoCmd->SetResponseCallback(baseInfoCallback);
    baseInfoCmd->SetTimeoutCallback(SetTimeout);
    CHECK_POINTER_RETURN_VALUE(sendAdapter_, MECH_CONNECT_FAILED, "sendAdapter_");
    sendAdapter_->SendCommand(baseInfoCmd);
    {
        std::unique_lock<std::mutex> lock(deviceBaseInfoMutex_);
        bool ready = deviceBaseInfoCon_.wait_for(lock, std::chrono::seconds(3), [this] {
            return deviceBaseInfoReady_;
        });
        if (!ready) {
            HILOGE("GetDeviceBaseInfo timeout");
            return MECH_CONNECT_FAILED;
        }
    }
    HILOGI("end");
    return ERR_OK;
}

void MotionManager::GetDeviceCapabilityInfo()
{
    HILOGI("start");
    std::shared_ptr<NormalGetMechCapabilityInfoCmd> capabilityInfoCmd = factory.CreateGetMechCapabilityInfoCmd();
    CHECK_POINTER_RETURN(capabilityInfoCmd, "capabilityInfoCmd is empty.");
    auto capabilityInfoCallback = [this, capabilityInfoCmd]() {
        deviceCapabilityInfo_ = capabilityInfoCmd->GetParams();
        HILOGI("device callback deviceCapabilityInfo_ ok");
        };

    capabilityInfoCmd->SetResponseCallback(capabilityInfoCallback);
    capabilityInfoCmd->SetTimeoutCallback(SetTimeout);
    CHECK_POINTER_RETURN(sendAdapter_, "sendAdapter_");
    sendAdapter_->SendCommand(capabilityInfoCmd);
    HILOGI("end");
}

void MotionManager::GetDeviceStateInfo()
{
    HILOGI("start");
    std::shared_ptr<NormalGetMechStateInfoCmd> stateInfoCmd = factory.CreateGetMechStateInfoCmd();
    CHECK_POINTER_RETURN(stateInfoCmd, "stateInfoCmd is empty.");
    auto stateInfoCallback = [this, stateInfoCmd]() {
        DeviceStateInfo info = stateInfoCmd->GetParams();
        deviceStatus_->stateInfo.isPhoneOn = info.attached;
        if (MechConnectManager::GetInstance().GetMechState(mechId_) !=
            static_cast<AttachmentStateMap>(info.attached)) {
            HandleMechPlacementChange(info.attached);
        }
        HILOGI("device callback deviceStateInfo_ ok");
        };

    auto stateInfoTimeoutCallback = [this, stateInfoCmd, stateInfoCallback]() {
        stateInfoCmd->SetResponseCallback(stateInfoCallback);
        CHECK_POINTER_RETURN(sendAdapter_, "sendAdapter_");
        sendAdapter_->SendCommand(stateInfoCmd);
        };

    stateInfoCmd->SetResponseCallback(stateInfoCallback);
    stateInfoCmd->SetTimeoutResetCallback(stateInfoTimeoutCallback);
    CHECK_POINTER_RETURN(sendAdapter_, "sendAdapter_");
    sendAdapter_->SendCommand(stateInfoCmd);
    HILOGI("end");
}

void MotionManager::GetMechLimitInfo()
{
    HILOGI("start");
    std::shared_ptr<CommonGetMechLimitInfoCmd> limitCmd = factory
        .CreateGetMechLimitInfoCmd();
    CHECK_POINTER_RETURN(limitCmd, "CapabilityInfoCmd is empty.");
    auto limitCallback = [this, limitCmd]() {
        auto rawParams = limitCmd->GetParams();
        auto params = rawParams;
        {
            std::unique_lock <std::mutex> lock(this->deviceStatusMutex_);
            this->deviceStatus_->rotationLimit = rawParams;
            FormatLimit(params);
            this->deviceStatus_->rotationLimitWithOffset = params;
        }
        JudgingYawLimit(params);
    };

    limitCmd->SetResponseCallback(limitCallback);
    limitCmd->SetTimeoutCallback(SetTimeout);
    CHECK_POINTER_RETURN(sendAdapter_, "sendAdapter_");
    sendAdapter_->SendCommand(limitCmd);
    HILOGI("end");
}

void MotionManager::GetWheelCapabilityInfo()
{
    HILOGI("start");
    std::shared_ptr<WheelGetMechCapabilityInfoCmd> wheelCapabilityInfoCmd =
        factory.CreateWheelGetMechCapabilityInfoCmd();
    CHECK_POINTER_RETURN(wheelCapabilityInfoCmd, "wheelCapabilityInfoCmd is empty.");
    auto wheelCapabilityInfoCallback = [this, wheelCapabilityInfoCmd]() {
        wheelCapabilityInfo_ = wheelCapabilityInfoCmd->GetParams();
        HILOGI("device callback wheelCapabilityInfo_ ok");
    };

    wheelCapabilityInfoCmd->SetResponseCallback(wheelCapabilityInfoCallback);
    wheelCapabilityInfoCmd->SetTimeoutCallback(SetTimeout);
    CHECK_POINTER_RETURN(sendAdapter_, "sendAdapter_");
    sendAdapter_->SendCommand(wheelCapabilityInfoCmd);
    HILOGI("end");
}

void MotionManager::FormatLimit(RotateDegreeLimit &params)
{
    bool flag = params.posMax.yaw > NO_LIMIT_MAX;
    while (flag) {
        params.posMax.yaw -= DEGREE_CIRCLED_MAX;
        flag = params.posMax.yaw > NO_LIMIT_MAX;
    }
    flag = params.posMax.yaw < NO_LIMIT_MIN;
    while (flag) {
        params.posMax.yaw += DEGREE_CIRCLED_MAX;
        flag = params.posMax.yaw < NO_LIMIT_MIN;
    }
    flag = params.negMax.yaw > NO_LIMIT_MAX;
    while (flag) {
        params.negMax.yaw -= DEGREE_CIRCLED_MAX;
        flag = params.negMax.yaw > NO_LIMIT_MAX;
    }
    flag = params.negMax.yaw < NO_LIMIT_MIN;
    while (flag) {
        params.negMax.yaw += DEGREE_CIRCLED_MAX;
        flag = params.negMax.yaw < NO_LIMIT_MIN;
    }
}

void MotionManager::SetDevicePairingInfo(const uint32_t deviceIdentifier)
{
    HILOGI("start");
    std::shared_ptr<SetMechDevicePairingInfoCmd> pairingInfoCmd = factory
        .CreateSetMechDevicePairingInfoCmd(deviceIdentifier);
    CHECK_POINTER_RETURN(pairingInfoCmd, "pairingInfoCmd is empty.");
    auto pairingInfoCallback = [this, pairingInfoCmd]() {
        auto result = pairingInfoCmd->GetResult();
        HILOGI("device callback devicePairingInfo_  result: %{public}d", result);
    };

    pairingInfoCmd->SetResponseCallback(pairingInfoCallback);
    pairingInfoCmd->SetTimeoutCallback(SetTimeout);
    CHECK_POINTER_RETURN(sendAdapter_, "sendAdapter_");
    sendAdapter_->SendCommand(pairingInfoCmd);
    HILOGI("end");
}

void MotionManager::RegisterEventListener()
{
    HILOGI("RegisterEventListener start.");
    if (mechEventListener_ == nullptr) {
        HILOGI("RegisterReceiveListener create.");
        mechEventListener_ = std::make_shared<MechEventListenerImpl>(shared_from_this());
    }
    RegisterEventListenerV01();
    RegisterBaseEvents();
    RegisterExtendedAndTrackingEvents();
    HILOGI("RegisterEventListener end.");
}

void MotionManager::RegisterBaseEvents()
{
    std::shared_ptr<CommandBase> attitudeCmd = factory.CreateRegisterMechPositionInfoCmd();
    CHECK_POINTER_RETURN(attitudeCmd, "PositionInfoCmd is empty.");
    {
        std::lock_guard<std::mutex> lock(notifyListenerMutex_);
        notifyListenerType_.push_back(attitudeCmd->GetCmdType());
    }
    SubscriptionCenter::GetInstance().Subscribe(attitudeCmd->GetCmdType(), mechEventListener_);

    std::shared_ptr<CommandBase> buttonEventCmd = factory.CreateRegisterMechCameraKeyEventCmd();
    CHECK_POINTER_RETURN(buttonEventCmd, "CameraKeyEventCmd is empty.");
    {
        std::lock_guard<std::mutex> lock(notifyListenerMutex_);
        notifyListenerType_.push_back(buttonEventCmd->GetCmdType());
    }
    SubscriptionCenter::GetInstance().Subscribe(buttonEventCmd->GetCmdType(), mechEventListener_);

    std::shared_ptr<CommandBase> paramCmd = factory.CreateRegisterMechStateInfoCmd();
    CHECK_POINTER_RETURN(paramCmd, "StateInfoCmd is empty.");
    {
        std::lock_guard<std::mutex> lock(notifyListenerMutex_);
        notifyListenerType_.push_back(paramCmd->GetCmdType());
    }
    SubscriptionCenter::GetInstance().Subscribe(paramCmd->GetCmdType(), mechEventListener_);
}

void MotionManager::RegisterExtendedAndTrackingEvents()
{
    if (protocolVer_ >= 0x02) {
        std::shared_ptr<CommandBase> paramCmd = factory.CreateRegisterMechGenericEventCmd();
        CHECK_POINTER_RETURN(paramCmd, "GenericInfoCmd is empty.");
        {
            std::lock_guard<std::mutex> lock(notifyListenerMutex_);
            notifyListenerType_.push_back(paramCmd->GetCmdType());
        }
        SubscriptionCenter::GetInstance().Subscribe(paramCmd->GetCmdType(), mechEventListener_);
    }
    if (protocolVer_ >= 0x02 && deviceBaseInfo_.devType == static_cast<uint8_t>(MechType::WHEEL_BASE)) {
        std::shared_ptr<CommandBase> cliffInfoCmd = factory.CreateRegisterMechCliffInfoCmd();
        CHECK_POINTER_RETURN(cliffInfoCmd, "CliffInfoCmd is empty.");
        {
            std::lock_guard<std::mutex> lock(notifyListenerMutex_);
            notifyListenerType_.push_back(cliffInfoCmd->GetCmdType());
        }
        SubscriptionCenter::GetInstance().Subscribe(cliffInfoCmd->GetCmdType(), mechEventListener_);

        std::shared_ptr<CommandBase> obstacleInfoCmd = factory.CreateRegisterMechObstacleInfoCmd();
        CHECK_POINTER_RETURN(obstacleInfoCmd, "ObstacleInfoCmd is empty.");
        {
            std::lock_guard<std::mutex> lock(notifyListenerMutex_);
            notifyListenerType_.push_back(obstacleInfoCmd->GetCmdType());
        }
        SubscriptionCenter::GetInstance().Subscribe(obstacleInfoCmd->GetCmdType(), mechEventListener_);
    }
    std::shared_ptr<CommandBase> tkEnableCmd = factory.CreateRegisterMechTrackingEnableCmd();
    CHECK_POINTER_RETURN(tkEnableCmd, "tkEnableCmd is empty.");
    {
        std::lock_guard<std::mutex> lock(notifyListenerMutex_);
        notifyListenerType_.push_back(tkEnableCmd->GetCmdType());
    }
    SubscriptionCenter::GetInstance().Subscribe(tkEnableCmd->GetCmdType(), mechEventListener_);
}

void MotionManager::RegisterEventListenerV01()
{
    HILOGI("RegisterEventListenerV01 start.");
    if (protocolVer_ == 0x01) {
        std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
        CHECK_POINTER_RETURN(executionResultCmd, "ControlResultCmd is empty.");
        {
            std::lock_guard<std::mutex> lock(notifyListenerMutex_);
            notifyListenerType_.push_back(executionResultCmd->GetCmdType());
        }
        SubscriptionCenter::GetInstance().Subscribe(executionResultCmd->GetCmdType(), mechEventListener_);
    }
    if (protocolVer_ == 0x01) {
        std::shared_ptr<CommandBase> wheelCmd = factory.CreateRegisterMechWheelDataCmd();
        CHECK_POINTER_RETURN(wheelCmd, "WheelDataCmd is empty.");
        {
            std::lock_guard<std::mutex> lock(notifyListenerMutex_);
            notifyListenerType_.push_back(wheelCmd->GetCmdType());
        }
        SubscriptionCenter::GetInstance().Subscribe(wheelCmd->GetCmdType(), mechEventListener_);
    }

    HILOGI("RegisterEventListenerV01 end.");
}

void MotionManager::StartEvent()
{
    HILOGI("StartEvent start");
    prctl(PR_SET_NAME, EVENT_THREAD_NAME.c_str());
    auto runner = AppExecFwk::EventRunner::Create(false);
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        if (runner != nullptr) {
            HILOGE("Runner create success");
            eventHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
            eventCon_.notify_one();
        } else {
            HILOGE("Runner create failed");
        }
    }
    if (eventHandler_) {
        HILOGI("eventHandler_ create success");
        CHECK_POINTER_RETURN(runner, "runner");
        runner->Run();
    } else {
        HILOGI("eventHandler_ create failed");
    }
    HILOGI("StartEvent end");
}

MotionManager::~MotionManager()
{
    HILOGI("~MotionManager start.");
    if (eventHandler_ != nullptr) {
        eventHandler_->GetEventRunner()->Stop();
        if (eventThread_.joinable()) {
            eventThread_.join();
        }
        eventHandler_ = nullptr;
    } else {
        HILOGE("eventHandler_ is nullptr");
    }

    if (trackingCheckerThread_ != nullptr) {
        startTrackingChecker_ = false;
        if (trackingCheckerThread_->joinable()) {
            trackingCheckerThread_->join();
        }
    }
    HILOGI("~MotionManager end.");
}

void MotionManager::UnRegisterNotifyEvent()
{
    if (notifyListenerType_.empty()) {
        HILOGE("The notifyListenerType_ is empty.");
        return;
    }
    {
        std::lock_guard<std::mutex> lock(notifyListenerMutex_);
        for (int32_t notifyListenerType : notifyListenerType_) {
            HILOGD("Start UnSubscribe. Type: %{public}d.", notifyListenerType);
            SubscriptionCenter::GetInstance().UnSubscribe(notifyListenerType, mechEventListener_);
            HILOGD("UnSubscribe over. Type: %{public}d.", notifyListenerType);
        }
    }
    mechEventListener_ = nullptr;
}

RotateToLocationParam GenerateRotateToLocationParam(const RotateParam &param)
{
    RotateToLocationParam rotateToLocationParam;
    rotateToLocationParam.yawRadian = param.degree.yaw;
    rotateToLocationParam.rollRadian = param.degree.roll;
    rotateToLocationParam.pitchRadian = param.degree.pitch;
    rotateToLocationParam.rotateTime = param.duration;
    if (param.isRelative) {
        rotateToLocationParam.rotateMap = 0b00010111;
    } else {
        rotateToLocationParam.rotateMap = 0b00010111;
    }
    return rotateToLocationParam;
}

int32_t MotionManager::Rotate(std::shared_ptr<RotateParam> rotateParam,
    uint32_t &tokenId, std::string &napiCmdId)
{
    HILOGI("Rotate start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    CHECK_POINTER_RETURN_VALUE(rotateParam, INVALID_PARAMETERS_ERR, "rotateParam");
    if (!CheckRotatePointParam(rotateParam->degree)) {
        HILOGE("The angle exceeds the limit.");
        MechBodyControllerService::GetInstance().NotifyOperationResult(tokenId, napiCmdId, ExecResult::SYSTEM_ERROR);
        return INVALID_PARAMETERS_ERR;
    }
    MechNapiCommandCallbackInfo callbackInfo = {tokenId, napiCmdId};
    if (rotateParam->isRelative) {
        HILOGI("Rotate isRelative.");
        HandelRotateParam(rotateParam, callbackInfo.willLimitChange);
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        AbsolutelyEulerAnglesJudgingLimitLocked(rotateParam->degree);
    }
    RotateParam param = *rotateParam;
    uint8_t taskId = CreateResponseTaskId();
    {
        std::unique_lock<std::mutex> lock(seqCallbackMutex_);
        HILOGD("Insert callback id: %{public}d", taskId);
        seqCallbacks_.insert(std::make_pair(taskId, callbackInfo));
    }
    std::shared_ptr<CommandBase> rotationCmd = ExecuteRotateCommand(param, taskId);
    CHECK_POINTER_RETURN_VALUE(rotationCmd, INVALID_PARAMETERS_ERR, "RotationCmd is empty.");
    sendAdapter_->SendCommand(rotationCmd);
    auto timeoutFunc = [this, taskId]() {
        std::unique_lock<std::mutex> lock(seqCallbackMutex_);
        auto it = seqCallbacks_.find(taskId);
        if (it != seqCallbacks_.end()) {
            auto cbInfo = it->second;
            HILOGI("notify rotate timeout. tokenid: %{public}s; napicmdid: %{public}s",
                GetAnonymUint32(cbInfo.tokenId).c_str(), cbInfo.napiCmdId.c_str());
            seqCallbacks_.erase(it);
            lock.unlock();
            MechBodyControllerService::GetInstance().NotifyOperationResult(
                cbInfo.tokenId, cbInfo.napiCmdId, ExecResult::TIMEOUT);
        }
    };

    std::string taskName = EVENT_THREAD_NAME + std::to_string(taskId);
    if (eventHandler_ != nullptr && (rotateParam->duration + RESPONSE_TIMEOUT) > 0) {
        eventHandler_->PostTask(timeoutFunc, taskName, rotateParam->duration + RESPONSE_TIMEOUT);
    }
    return ERR_OK;
}

std::shared_ptr<CommandBase> MotionManager::ExecuteRotateCommand(const RotateParam &param, uint8_t taskId)
{
    std::shared_ptr<CommandBase> rotationCmd;
    if (protocolVer_ == 0x02) {
        RotateToLocationParam rotateToLocationParam = GenerateRotateToLocationParam(param);
        rotateToLocationParam.taskId = taskId;
        rotationCmd = factory.CreateSetMechRotationToLocationCmd(rotateToLocationParam);
    } else {
        RotateParam paramCopy = param;
        paramCopy.taskId = taskId;
        rotationCmd = factory.CreateSetMechRotationCmd(paramCopy);
    }
    if (rotationCmd == nullptr) {
        return nullptr;
    }
    rotationCmd->SetResponseCallback([this, taskId, rotationCmd]() {
        HILOGI("Rotate command completed, taskId=%d", taskId);
        std::unique_lock<std::mutex> lock(seqCallbackMutex_);
        auto it = seqCallbacks_.find(taskId);
        if (it != seqCallbacks_.end()) {
            uint8_t cmdResult = 0;
            if (protocolVer_ == 0x02) {
                cmdResult = static_cast<NormalSetMechRotationToLocationCmd*>(rotationCmd.get())->GetResult();
            } else {
                cmdResult = static_cast<SetMechRotationCmd*>(rotationCmd.get())->GetResult();
            }
            ExecResult result = static_cast<ExecResult>(cmdResult);
            auto cbInfo = it->second;
            seqCallbacks_.erase(it);
            lock.unlock();
            MechBodyControllerService::GetInstance().NotifyOperationResult(
                cbInfo.tokenId, cbInfo.napiCmdId, result);
        }
    });
    return rotationCmd;
}

void MotionManager::HandelRotateParam(std::shared_ptr<RotateParam> &rotateParam, bool &willLimitChange)
{
    if (deviceStatus_ == nullptr) {
        HILOGW("currentPosition is nullptr");
        return;
    }
    if (rotateParam == nullptr) {
        HILOGW("rotateParam is nullptr");
        return;
    }
    EulerAngles currentPosition = deviceStatus_->eulerAngles;
    auto limit = deviceStatus_->rotationLimitWithOffset;
    RotationAxesStatus status = status_;
    HILOGI("Limit is: %{public}s; YAW_OFFSET: %{public}f",
        deviceStatus_->rotationLimitWithOffset.ToString().c_str(), YAW_OFFSET);
    HILOGI("current position: %{public}s", currentPosition.ToString().c_str());
    HILOGI("rotateParam before: %{public}s", rotateParam->ToString().c_str());

    float yawResult = currentPosition.yaw + rotateParam->degree.yaw;
    float rollResult = currentPosition.roll + rotateParam->degree.roll;
    float pitchResult = currentPosition.pitch + rotateParam->degree.pitch;
    HILOGI("yawResult = %{public}f; rollResult = %{public}f; pitchResult = %{public}f;",
        yawResult, rollResult, pitchResult);
    CheckYawDegree(rotateParam, limit, yawResult);

    CheckRollDegree(rotateParam, limit, rollResult);

    CheckPitchDegree(rotateParam, limit, pitchResult);

    HILOGI("rotateParam after: %{public}s", rotateParam->ToString().c_str());
    if (status.yawLimited != status_.yawLimited || status.rollLimited != status_.rollLimited ||
        status.pitchLimited != status_.pitchLimited) {
        HILOGI("limit state changed.");
        willLimitChange = true;
    }
}

void MotionManager::CheckYawDegree(std::shared_ptr<RotateParam> &rotateParam, const RotateDegreeLimit &limit,
    float yawResult)
{
    if (rotateParam == nullptr) {
        HILOGW("rotateParam is nullptr");
        return;
    }
    yawResult += rotateParam->degree.yaw > 0 ? YAW_OFFSET : -YAW_OFFSET;
    bool flag = yawResult < NO_LIMIT_MIN;
    while (flag) {
        yawResult += DEGREE_CIRCLED_MAX;
        flag = yawResult < NO_LIMIT_MIN;
    }
    flag = yawResult > NO_LIMIT_MAX;
    while (flag) {
        yawResult -= DEGREE_CIRCLED_MAX;
        flag = yawResult > NO_LIMIT_MAX;
    }
    if (IsLimited(limit.negMax.yaw, limit.posMax.yaw, yawResult)) {
        if (rotateParam->degree.yaw > 0) {
            rotateParam->degree.yaw -= (yawResult - limit.posMax.yaw);
            status_.yawLimited = RotationAxisLimited::POS_LIMITED;
        } else {
            rotateParam->degree.yaw -= (yawResult - limit.negMax.yaw);
            status_.yawLimited = RotationAxisLimited::NEG_LIMITED;
        }
    } else {
        status_.yawLimited = RotationAxisLimited::NOT_LIMITED;
    }
}

void MotionManager::CheckRollDegree(std::shared_ptr<RotateParam> &rotateParam, const RotateDegreeLimit &limit,
    float rollResult)
{
    if (rotateParam == nullptr) {
        HILOGW("rotateParam is nullptr");
        return;
    }
    bool flag = rollResult < NO_LIMIT_MIN;
    while (flag) {
        rollResult += DEGREE_CIRCLED_MAX;
        flag = rollResult < NO_LIMIT_MIN;
    }
    flag = rollResult > NO_LIMIT_MIN;
    while (flag) {
        rollResult -= DEGREE_CIRCLED_MAX;
        flag = rollResult > NO_LIMIT_MIN;
    }
    if (IsLimited(limit.negMax.roll, limit.posMax.roll, rollResult)) {
        if (rotateParam->degree.roll > 0) {
            rotateParam->degree.roll -= (rollResult - limit.posMax.roll);
            status_.rollLimited = RotationAxisLimited::POS_LIMITED;
        } else {
            rotateParam->degree.roll -= (rollResult - limit.negMax.roll);
            status_.rollLimited = RotationAxisLimited::NEG_LIMITED;
        }
    } else {
        status_.rollLimited = RotationAxisLimited::NOT_LIMITED;
    }
}

void MotionManager::CheckPitchDegree(std::shared_ptr<RotateParam> &rotateParam, const RotateDegreeLimit &limit,
    float pitchResult)
{
    if (rotateParam == nullptr) {
        HILOGW("rotateParam is nullptr");
        return;
    }
    bool flag = pitchResult < NO_LIMIT_MIN;
    while (flag) {
        pitchResult += DEGREE_CIRCLED_MAX;
        flag = pitchResult < NO_LIMIT_MIN;
    }
    flag = pitchResult > NO_LIMIT_MIN;
    while (flag) {
        pitchResult -= DEGREE_CIRCLED_MAX;
        flag = pitchResult > NO_LIMIT_MIN;
    }
    if (IsLimited(limit.negMax.pitch, limit.posMax.pitch, pitchResult)) {
        if (rotateParam->degree.pitch > 0) {
            rotateParam->degree.pitch -= (pitchResult - limit.posMax.pitch);
            status_.pitchLimited = RotationAxisLimited::POS_LIMITED;
        } else {
            rotateParam->degree.pitch -= (pitchResult - limit.negMax.pitch);
            status_.pitchLimited = RotationAxisLimited::NEG_LIMITED;
        }
    } else {
        status_.pitchLimited = RotationAxisLimited::NOT_LIMITED;
    }
}

int32_t MotionManager::CheckWheelSpeedLimit(const std::vector<RotateParam> &rotateParams)
{
    float maxSpeed = wheelCapabilityInfo_.maxForwardSpeed;
    float minSpeed = 0 - wheelCapabilityInfo_.maxForwardSpeed;
    float maxRotationSpeed = wheelCapabilityInfo_.maxRotationSpeed;
    for (const auto &param : rotateParams) {
        if (param.forwardSpeed > maxSpeed) {
            HILOGW("ForwardSpeed: %{public}d, maximum forwardSpeed:%{public}d",
                param.forwardSpeed,
                static_cast<int16_t>(maxSpeed));
            return WHEEL_SPEED_EXCEED_LIMIT;
        } else if (param.forwardSpeed < minSpeed) {
            HILOGW("ForwardSpeed: %{public}d, minimum forwardSpeed:%{public}d",
                param.forwardSpeed,
                static_cast<int16_t>(minSpeed));
            return WHEEL_SPEED_EXCEED_LIMIT;
        } else if (param.turningSpeed > maxRotationSpeed) {
            HILOGW("TurningSpeed: %{public}f, maximum turningSpeed: %{public}f",
                param.turningSpeed, maxRotationSpeed);
            return WHEEL_SPEED_EXCEED_LIMIT;
        }
    }
    return ERR_OK;
}

std::vector<RotateParam> MotionManager::ConvertMoveParamsToRotateParams(const std::shared_ptr<MoveParams> &moveParams)
{
    std::vector<RotateParam> result;
    bool hasDistance = (moveParams->distance != 0);
    bool hasAngle = (moveParams->angle != 0);

    if (!hasDistance && !hasAngle) {
        return result;
    }

    if (moveParams->mode == MarchingMode::TURN_THEN_MOVE) {
        // 转弯阶段
        if (hasAngle) {
            uint64_t angleTimeMs = static_cast<uint64_t>(std::abs(moveParams->angle) * 1000 / ANGLE_SPEED_DEFAULT);
            uint16_t angleTime = SafeMsToUint16(angleTimeMs);
            int16_t turnSpeed = GetFinalSpeed(std::signbit(moveParams->angle), ANGLE_SPEED_DEFAULT);
            result.emplace_back(angleTime, 0, turnSpeed);
        }
        // 移动阶段
        if (hasDistance) {
            int16_t speed = DISTANCE_SPEED_MAP[moveParams->speedGear];
            uint64_t distanceTimeMs = DistanceToMs(moveParams->distance, speed);
            uint16_t distanceTime = SafeMsToUint16(distanceTimeMs);
            int16_t forwardSpeed = GetFinalSpeed(std::signbit(moveParams->distance), speed);
            result.emplace_back(distanceTime, forwardSpeed, 0);
        }
    } else if (hasDistance != 0) {
        // 同时移动和转弯
        int16_t speed = DISTANCE_SPEED_MAP[moveParams->speedGear];
        uint64_t moveTimeMs = DistanceToMs(moveParams->distance, speed);
        uint16_t moveTime = SafeMsToUint16(moveTimeMs);
        int16_t forwardSpeed = GetFinalSpeed(std::signbit(moveParams->distance), speed);
        float turningSpeed = 0.0f;
        if (hasAngle) {
            // 转弯速度（度/秒）= 角度（度） / 时间（秒）
            turningSpeed = moveParams->angle / (moveTime / 1000.0f);
        }
        result.emplace_back(moveTime, forwardSpeed, turningSpeed);
    } else {
        // 仅转弯
        uint64_t moveTimeMs = static_cast<uint64_t>(std::abs(moveParams->angle) * 1000 / ANGLE_SPEED_DEFAULT);
        uint16_t moveTime = SafeMsToUint16(moveTimeMs);
        int16_t turnSpeed = GetFinalSpeed(std::signbit(moveParams->angle), ANGLE_SPEED_DEFAULT);
        result.emplace_back(moveTime, 0, turnSpeed);
    }
    return result;
}
std::vector<RotateParam> MotionManager::ConvertSpeedParamsToRotateParams(
    const std::shared_ptr<SpeedParams> &speedParams, int32_t moveDurationMs)
{
    std::vector<RotateParam> result;
    bool hasSpeed = (speedParams->speed != 0);
    bool hasAngle = (speedParams->angle != 0);
    if (!hasSpeed && !hasAngle) {
        return result;
    }

    if (!hasSpeed) {
        // 仅转动
        uint64_t turnTimeMs = static_cast<uint64_t>(std::abs(speedParams->angle) * 1000 / ANGLE_SPEED_DEFAULT);
        uint16_t turnTime = SafeMsToUint16(turnTimeMs);
        int16_t turnSpeed = GetFinalSpeed(std::signbit(speedParams->angle), ANGLE_SPEED_DEFAULT);
        result.emplace_back(turnTime, 0, turnSpeed);
    } else if (!hasAngle) {
        // 仅移动
        uint16_t moveTime = SafeMsToUint16(moveDurationMs);
        result.emplace_back(moveTime, speedParams->speed, 0);
    } else {
        // 同时有移动和转动
        uint64_t turnTimeMs = static_cast<uint64_t>(std::abs(speedParams->angle) * 1000 / ANGLE_SPEED_DEFAULT);
        uint16_t turnTime = SafeMsToUint16(turnTimeMs);
        uint16_t moveTime = SafeMsToUint16(moveDurationMs);

        if (speedParams->mode == MarchingMode::TURN_THEN_MOVE) {
            // 顺序执行：先转后移
            int16_t turnSpeed = GetFinalSpeed(std::signbit(speedParams->angle), ANGLE_SPEED_DEFAULT);
            result.emplace_back(turnTime, 0, turnSpeed);
            result.emplace_back(moveTime, speedParams->speed, 0);
        } else {
            float turningSpeed = speedParams->angle / (moveTime / 1000.0f);
            result.emplace_back(moveTime, speedParams->speed, turningSpeed);
        }
    }
    return result;
}

int32_t MotionManager::SendRotateCommand(
    uint32_t tokenId, std::string &napiCmdId, const std::vector<RotateParam> &rotateParams)
{
    CHECK_POINTER_RETURN_VALUE(sendAdapter_, INVALID_PARAMETERS_ERR, "sendAdapter_");
    if (rotateParams.empty()) {
        HILOGI("No move parameters, command completed immediately.");
        MechBodyControllerService::GetInstance().NotifyOperationResult(tokenId, napiCmdId, ExecResult::COMPLETED);
        return ERR_OK;
    }
    for (size_t i = 0; i < rotateParams.size(); ++i) {
        const auto &rp = rotateParams[i];
        HILOGI("rotateParams[%{public}zu]: duration=%{public}d, forwardSpeed=%{public}d, turningSpeed=%{public}f",
            i, rp.duration, rp.forwardSpeed, rp.turningSpeed);
    }
    uint8_t responseTaskId = CreateResponseTaskId();
    MechNapiCommandCallbackInfo callbackInfo = {tokenId, napiCmdId};
    {
        std::unique_lock<std::mutex> lock(seqCallbackMutex_);
        seqCallbacks_.insert({responseTaskId, callbackInfo});
    }
    uint32_t totalDuration = 0;
    for (const auto &p : rotateParams) totalDuration += p.duration;
    uint16_t rotateTaskId = CreateRotateTaskId();
    SendRotateCommandImpl(responseTaskId, rotateTaskId, rotateParams);

    auto timeoutFunc = [this, responseTaskId]() {
        std::unique_lock<std::mutex> lock(seqCallbackMutex_);
        auto it = seqCallbacks_.find(responseTaskId);
        if (it != seqCallbacks_.end()) {
            auto cbInfo = it->second;
            seqCallbacks_.erase(it);
            lock.unlock();
            MechBodyControllerService::GetInstance().NotifyOperationResult(
                cbInfo.tokenId, cbInfo.napiCmdId, ExecResult::TIMEOUT);
        }
    };
    std::string taskName = EVENT_THREAD_NAME + std::to_string(responseTaskId);
    if (eventHandler_ && totalDuration + RESPONSE_TIMEOUT > 0) {
        eventHandler_->PostTask(timeoutFunc, taskName, totalDuration + RESPONSE_TIMEOUT);
    }
    return ERR_OK;
}

void MotionManager::SendRotateCommandImpl(
    uint8_t responseTaskId, uint16_t rotateTaskId, const std::vector<RotateParam> &rotateParams)
{
    if (rotateParams.size() == 1) {
        const auto &rp = rotateParams[0];
        RotateToBaseParam baseParam(rotateTaskId,
                                     static_cast<uint16_t>(std::clamp(rp.duration, 0, UINT16_MAX)),
                                     rp.forwardSpeed,
                                     rp.turningSpeed);
        auto cmd = factory.CreateWheelSetMechRotationToBaseCmd(baseParam);
        cmd->SetResponseCallback([cmd, this, responseTaskId]() {
            HILOGI("RotationToBase end.");
            std::unique_lock<std::mutex> lock(seqCallbackMutex_);
            auto it = seqCallbacks_.find(responseTaskId);
            if (it != seqCallbacks_.end()) {
                ExecResult result = static_cast<ExecResult>(cmd->GetResult());
                auto cbInfo = it->second;
                seqCallbacks_.erase(it);
                lock.unlock();
                MechBodyControllerService::GetInstance().NotifyOperationResult(
                    cbInfo.tokenId, cbInfo.napiCmdId, result);
            }
        });
        sendAdapter_->SendCommand(cmd);
    } else {
        auto cmd = factory.CreateSetWheelMechRotationTraceCmd(rotateTaskId, rotateParams);
        cmd->SetResponseCallback([cmd, this, responseTaskId]() {
            HILOGI("RotationTrace end.");
            std::unique_lock<std::mutex> lock(seqCallbackMutex_);
            auto it = seqCallbacks_.find(responseTaskId);
            if (it != seqCallbacks_.end()) {
                ExecResult result = static_cast<ExecResult>(cmd->GetResult());
                auto cbInfo = it->second;
                seqCallbacks_.erase(it);
                lock.unlock();
                MechBodyControllerService::GetInstance().NotifyOperationResult(
                    cbInfo.tokenId, cbInfo.napiCmdId, result);
            }
        });
        sendAdapter_->SendCommand(cmd);
    }
}

int32_t MotionManager::Move(uint32_t tokenId, std::string &napiCmdId, const std::shared_ptr<MoveParams> &moveParams)
{
    HILOGI("Move start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    CHECK_POINTER_RETURN_VALUE(moveParams, INVALID_PARAMETERS_ERR, "moveParams");
    std::vector<RotateParam> rotateParams = ConvertMoveParamsToRotateParams(moveParams);
    int32_t ret = CheckWheelSpeedLimit(rotateParams);
    if (ret != ERR_OK) {
        return ret;
    }
    return SendRotateCommand(tokenId, napiCmdId, rotateParams);
}

int32_t MotionManager::MoveBySpeed(
    uint32_t tokenId, std::string &napiCmdId, uint16_t duration, const std::shared_ptr<SpeedParams> &speedParams)
{
    HILOGI("MoveBySpeed start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    CHECK_POINTER_RETURN_VALUE(speedParams, INVALID_PARAMETERS_ERR, "MoveBySpeedParam");
    std::vector<RotateParam> rotateParams = ConvertSpeedParamsToRotateParams(speedParams, duration);
    int32_t ret = CheckWheelSpeedLimit(rotateParams);
    if (ret != ERR_OK) {
        return ret;
    }
    if (wheelCapabilityInfo_.maxWheelCtrlTime != 0 && duration > wheelCapabilityInfo_.maxWheelCtrlTime) {
        HILOGW("Duration: %{public}u, maximum wheelCtrlTime: %{public}u",
            duration, wheelCapabilityInfo_.maxWheelCtrlTime);
        return WHEEL_SPEED_EXCEED_LIMIT;
    }
    return SendRotateCommand(tokenId, napiCmdId, rotateParams);
}

int32_t MotionManager::TurnBySpeed(uint32_t tokenId, std::string &napiCmdId, float angleSpeed, uint16_t duration)
{
    HILOGI("TurnBySpeed start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }

    std::vector<RotateParam> rotateParams;
    rotateParams.emplace_back(duration, 0, angleSpeed);
    int32_t ret = CheckWheelSpeedLimit(rotateParams);
    if (ret != ERR_OK) {
        return ret;
    }
    if (wheelCapabilityInfo_.maxWheelCtrlTime != 0 && duration > wheelCapabilityInfo_.maxWheelCtrlTime) {
        HILOGW("Duration: %{public}u, maximum wheelCtrlTime: %{public}u",
            duration, wheelCapabilityInfo_.maxWheelCtrlTime);
        return WHEEL_SPEED_EXCEED_LIMIT;
    }
    return SendRotateCommand(tokenId, napiCmdId, rotateParams);
}

int32_t MotionManager::DoAction(uint32_t tokenId, std::string &napiCmdId, ActionType actionType)
{
    HILOGI("DoAction start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    auto sceneControlCmd = factory.CreateWheelSetMechSceneControlCmd(actionType);
    CHECK_POINTER_RETURN_VALUE(sceneControlCmd, INVALID_PARAMETERS_ERR, "sceneControlCmd is empty.");

    uint8_t taskId = CreateResponseTaskId();
    MechNapiCommandCallbackInfo callbackInfo = {tokenId, napiCmdId};
    {
        std::unique_lock<std::mutex> lock(seqCallbackMutex_);
        seqCallbacks_.insert({taskId, callbackInfo});
    }
    sceneControlCmd->SetResponseCallback([this, taskId, sceneControlCmd]() {
        HILOGI("DoAction end, taskId=%{public}d", taskId);
        std::unique_lock<std::mutex> lock(seqCallbackMutex_);
        auto it = seqCallbacks_.find(taskId);
        if (it != seqCallbacks_.end()) {
            ExecResult result =
                static_cast<ExecResult>(static_cast<WheelSetMechSceneControlCmd *>(sceneControlCmd.get())->GetResult());
            auto cbInfo = it->second;
            seqCallbacks_.erase(it);
            lock.unlock();
            MechBodyControllerService::GetInstance().NotifyOperationResult(
                cbInfo.tokenId, cbInfo.napiCmdId, result);
        }
    });
    sendAdapter_->SendCommand(sceneControlCmd);
    auto timeoutFunc = [this, taskId]() {
        std::unique_lock<std::mutex> lock(seqCallbackMutex_);
        auto it = seqCallbacks_.find(taskId);
        if (it != seqCallbacks_.end()) {
            auto cbInfo = it->second;
            seqCallbacks_.erase(it);
            lock.unlock();
            MechBodyControllerService::GetInstance().NotifyOperationResult(
                cbInfo.tokenId, cbInfo.napiCmdId, ExecResult::TIMEOUT);
        }
    };
    std::string taskName = EVENT_THREAD_NAME + std::to_string(taskId);
    if (eventHandler_) eventHandler_->PostTask(timeoutFunc, taskName, RESPONSE_TIMEOUT);
    return ERR_OK;
}

int32_t MotionManager::RotateBySpeed(std::shared_ptr<RotateBySpeedParam> rotateSpeedParam,
    uint32_t &tokenId, std::string &napiCmdId)
{
    HILOGI("Rotate by speed start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    CHECK_POINTER_RETURN_VALUE(rotateSpeedParam, INVALID_PARAMETERS_ERR, "rotateSpeedParam");
    MechNapiCommandCallbackInfo callbackInfo = {tokenId, napiCmdId};
    uint8_t responseTaskId = CreateResponseTaskId();
    {
        std::unique_lock<std::mutex> lock(seqCallbackMutex_);
        seqCallbacks_.insert({responseTaskId, callbackInfo});
    }
    std::shared_ptr<CommonSetMechRotationBySpeedCmd> rotationBySpeedCmd =
        CreateAndSendRotateBySpeedCommand(rotateSpeedParam, responseTaskId);
    CHECK_POINTER_RETURN_VALUE(rotationBySpeedCmd, INVALID_PARAMETERS_ERR, "RotationBySpeedCmd is empty.");
    sendAdapter_->SendCommand(rotationBySpeedCmd);
    auto timeoutFunc = [this, responseTaskId]() {
        std::unique_lock<std::mutex> lock(seqCallbackMutex_);
        auto it = seqCallbacks_.find(responseTaskId);
        if (it != seqCallbacks_.end()) {
            auto cbInfo = it->second;
            seqCallbacks_.erase(it);
            lock.unlock();
            MechBodyControllerService::GetInstance().NotifyOperationResult(
                cbInfo.tokenId, cbInfo.napiCmdId, ExecResult::TIMEOUT);
        }
    };
    std::string taskName = EVENT_THREAD_NAME + std::to_string(responseTaskId);
    if (eventHandler_ != nullptr && (rotateSpeedParam->duration + RESPONSE_TIMEOUT) > 0) {
        eventHandler_->PostTask(timeoutFunc, taskName, rotateSpeedParam->duration + RESPONSE_TIMEOUT);
    }
    HILOGI("Rotate by speed end.");
    return ERR_OK;
}

std::shared_ptr<CommonSetMechRotationBySpeedCmd> MotionManager::CreateAndSendRotateBySpeedCommand(
    std::shared_ptr<RotateBySpeedParam> rotateSpeedParam, uint8_t responseTaskId)
{
    rotateSpeedParam->taskId = CreateRotateTaskId();
    auto rotationBySpeedCmd = factory.CreateSetMechRotationBySpeedCmd(*rotateSpeedParam);
    if (rotationBySpeedCmd == nullptr) {
        return nullptr;
    }
    auto limitCallback = [rotationBySpeedCmd, this]() {
        RotationAxesStatus limitInfo = static_cast<CommonSetMechRotationBySpeedCmd*>(rotationBySpeedCmd.get())
            ->GetRotationAxesStatus();
        HILOGI("Rotate By Speed callback. limit info: %{public}s", limitInfo.ToString().c_str());
        if (limitInfo.IsChange(this->deviceStatus_->rotationAxesStatus)) {
            HILOGI("Rotate By Speed callback. Notify limited");
            std::unique_lock <std::mutex> lock(deviceStatusMutex_);
            this->deviceStatus_->rotationAxesStatus.yawLimited = limitInfo.yawLimited;
            this->deviceStatus_->rotationAxesStatus.rollLimited = limitInfo.rollLimited;
            this->deviceStatus_->rotationAxesStatus.pitchLimited = limitInfo.pitchLimited;
            MechBodyControllerService::GetInstance().OnRotationAxesStatusChange(this->mechId_,
                this->deviceStatus_->rotationAxesStatus);
        }
    };
    auto completionCallback = [this, responseTaskId, rotationBySpeedCmd]() {
        std::unique_lock<std::mutex> lock(seqCallbackMutex_);
        auto it = seqCallbacks_.find(responseTaskId);
        if (it != seqCallbacks_.end()) {
            ExecResult result = static_cast<ExecResult>(
                static_cast<NormalSetMechRotationBySpeedCmd *>(rotationBySpeedCmd.get())->GetResult());
            auto cbInfo = it->second;
            seqCallbacks_.erase(it);
            lock.unlock();
            MechBodyControllerService::GetInstance().NotifyOperationResult(
                cbInfo.tokenId, cbInfo.napiCmdId, result);
        }
        if (protocolVer_ == 0x01) {
            std::shared_ptr<CommandBase> motionCmd = factory.CreateSetMechStopCmd();
            if (motionCmd != nullptr) {
                sendAdapter_->SendCommand(motionCmd);
            } else {
                HILOGE("StopCmd is empty.");
            }
        }
    };
    rotationBySpeedCmd->SetResponseCallback([limitCallback, completionCallback]() {
        limitCallback();
        completionCallback();
    });
    return rotationBySpeedCmd;
}

void MotionManager::HandelRotateSpeedParam(std::shared_ptr<RotateBySpeedParam> &rotateBySpeedParam,
    bool &willLimitChange)
{
    if (deviceStatus_ == nullptr) {
        HILOGW("currentPosition is nullptr");
        return;
    }
    if (rotateBySpeedParam == nullptr) {
        HILOGW("rotateBySpeedParam is nullptr");
        return;
    }
    EulerAngles currentPosition = deviceStatus_->eulerAngles;
    auto limit = deviceStatus_->rotationLimitWithOffset;
    RotationAxesStatus status = status_;
    HILOGI("Limit is: %{public}s; YAW_OFFSET: %{public}f",
        deviceStatus_->rotationLimitWithOffset.ToString().c_str(), YAW_OFFSET);
    HILOGI("current position: %{public}s", currentPosition.ToString().c_str());
    HILOGI("rotateBySpeedParam before: %{public}s", rotateBySpeedParam->ToString().c_str());
    HILOGI("status before: %{public}s", status.ToString().c_str());

    float duration = rotateBySpeedParam->duration;
    float yawResult =
        currentPosition.yaw + rotateBySpeedParam->speed.yawSpeed * duration / MILLISECONDS_TO_SECONDS;
    float rollResult =
        currentPosition.roll - rotateBySpeedParam->speed.rollSpeed * duration / MILLISECONDS_TO_SECONDS;
    float pitchResult =
        currentPosition.pitch + rotateBySpeedParam->speed.pitchSpeed * duration / MILLISECONDS_TO_SECONDS;
    HILOGI("yawResult = %{public}f; rollResult = %{public}f; pitchResult = %{public}f;", yawResult, rollResult,
        pitchResult);

    CheckYawSpeed(rotateBySpeedParam, limit, yawResult);

    CheckRollSpeed(rotateBySpeedParam, limit, rollResult);

    CheckPitchSpeed(rotateBySpeedParam, limit, pitchResult);

    HILOGI("status after: %{public}s", status.ToString().c_str());
    HILOGI("rotateBySpeedParam after: %{public}s", rotateBySpeedParam->ToString().c_str());
    if (status.yawLimited != status_.yawLimited || status.rollLimited != status_.rollLimited ||
        status.pitchLimited != status_.pitchLimited) {
        HILOGI("limit state changed.");
        MechBodyControllerService::GetInstance().OnRotationAxesStatusChange(mechId_, status_);
    }
}

void MotionManager::CheckYawSpeed(const std::shared_ptr<RotateBySpeedParam> &rotateBySpeedParam,
    const RotateDegreeLimit &limit, float yawResult)
{
    if (rotateBySpeedParam == nullptr) {
        HILOGW("rotateBySpeedParam is nullptr");
        return;
    }
    yawResult += rotateBySpeedParam->speed.yawSpeed > 0 ? YAW_OFFSET : -YAW_OFFSET;
    bool flag = yawResult < NO_LIMIT_MIN;
    while (flag) {
        yawResult += DEGREE_CIRCLED_MAX;
        flag = yawResult < NO_LIMIT_MIN;
    }
    flag = yawResult > NO_LIMIT_MAX;
    while (flag) {
        yawResult -= DEGREE_CIRCLED_MAX;
        flag = yawResult > NO_LIMIT_MAX;
    }

    if (IsLimited(limit.negMax.yaw, limit.posMax.yaw, yawResult)) {
        if (status_.yawLimited == RotationAxisLimited::POS_LIMITED  ||
            status_.yawLimited == RotationAxisLimited::NEG_LIMITED) {
            return;
        }
        if (rotateBySpeedParam->speed.yawSpeed > 0) {
            status_.yawLimited = RotationAxisLimited::POS_LIMITED;
        } else {
            status_.yawLimited = RotationAxisLimited::NEG_LIMITED;
        }
    } else {
        status_.yawLimited = RotationAxisLimited::NOT_LIMITED;
    }
}

void MotionManager::CheckRollSpeed(const std::shared_ptr<RotateBySpeedParam> &rotateBySpeedParam,
    const RotateDegreeLimit &limit, float rollResult)
{
    if (rotateBySpeedParam == nullptr) {
        HILOGW("rotateBySpeedParam is nullptr");
        return;
    }
    bool flag = rollResult < NO_LIMIT_MIN;
    while (flag) {
        rollResult += DEGREE_CIRCLED_MAX;
        flag = rollResult < NO_LIMIT_MIN;
    }
    flag = rollResult > NO_LIMIT_MAX;
    while (flag) {
        rollResult -= DEGREE_CIRCLED_MAX;
        flag = rollResult > NO_LIMIT_MAX;
    }
    if (IsLimited(limit.negMax.roll, limit.posMax.roll, rollResult)) {
        if (rotateBySpeedParam->speed.rollSpeed > 0) {
            status_.rollLimited = RotationAxisLimited::POS_LIMITED;
        } else {
            status_.rollLimited = RotationAxisLimited::NEG_LIMITED;
        }
    } else {
        status_.rollLimited = RotationAxisLimited::NOT_LIMITED;
    }
}

void MotionManager::CheckPitchSpeed(const std::shared_ptr<RotateBySpeedParam> &rotateBySpeedParam,
    const RotateDegreeLimit &limit, float pitchResult)
{
    if (rotateBySpeedParam == nullptr) {
        HILOGW("rotateBySpeedParam is nullptr");
        return;
    }
    bool flag = pitchResult < NO_LIMIT_MIN;
    while (flag) {
        pitchResult += DEGREE_CIRCLED_MAX;
        flag = pitchResult < NO_LIMIT_MIN;
    }
    flag = pitchResult > NO_LIMIT_MAX;
    while (flag) {
        pitchResult -= DEGREE_CIRCLED_MAX;
        flag = pitchResult > NO_LIMIT_MAX;
    }
    if (IsLimited(limit.negMax.pitch, limit.posMax.pitch, pitchResult)) {
        if (status_.pitchLimited == RotationAxisLimited::POS_LIMITED  ||
            status_.pitchLimited == RotationAxisLimited::NEG_LIMITED) {
            return;
        }
        if (rotateBySpeedParam->speed.pitchSpeed > 0) {
            status_.pitchLimited = RotationAxisLimited::POS_LIMITED;
        } else {
            status_.pitchLimited = RotationAxisLimited::NEG_LIMITED;
        }
    } else {
        status_.pitchLimited = RotationAxisLimited::NOT_LIMITED;
    }
}

int32_t MotionManager::StopRotate(uint32_t &tokenId, std::string &napiCmdId)
{
    HILOGI("Stop rotate start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    if (protocolVer_ == 0x01) {
        std::shared_ptr<CommandBase> motionCmd = factory.CreateSetMechStopCmd();
        CHECK_POINTER_RETURN_VALUE(motionCmd, INVALID_PARAMETERS_ERR, "StopCmd is empty.");
        sendAdapter_->SendCommand(motionCmd);
        MechBodyControllerService::GetInstance().NotifyOperationResult(tokenId, napiCmdId, ExecResult::COMPLETED);
    } else {
        std::shared_ptr<CommandBase> motionCmd = factory.CreateSetMechMotionControlCmd(ControlCommand::STOP);
        CHECK_POINTER_RETURN_VALUE(motionCmd, INVALID_PARAMETERS_ERR, "StopCmd is empty.");
        sendAdapter_->SendCommand(motionCmd);
        if (deviceBaseInfo_.devType == static_cast<uint8_t>(MechType::WHEEL_BASE)) {
            std::shared_ptr<CommandBase> motionWheelCmd =
                factory.CreateWheelSetMechMotionControlCmd(ControlCommand::STOP);
            CHECK_POINTER_RETURN_VALUE(motionWheelCmd, INVALID_PARAMETERS_ERR, "Wheel StopCmd is empty.");
            sendAdapter_->SendCommand(motionWheelCmd);
        }
        MechBodyControllerService::GetInstance().NotifyOperationResult(tokenId, napiCmdId, ExecResult::COMPLETED);
    }
    HILOGI("Stop rotate end.");
    return ERR_OK;
}

int32_t MotionManager::GetSpeedControlTimeLimit(std::shared_ptr<TimeLimit> &timeLimit)
{
    HILOGI("GetSpeedControlTimeLimit start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    if (timeLimit == nullptr) {
        timeLimit = std::make_shared<TimeLimit>();
    }
    timeLimit->min = 0;
    timeLimit->max = DEFAULT_DURATION;
    return ERR_OK;
}

int32_t MotionManager::GetRotateSpeedLimit(RotateSpeedLimit &speedLimit)
{
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        speedLimit = deviceStatus_->rotateSpeedLimit;
    }
    return ERR_OK;
}

int32_t MotionManager::GetCurrentPosition(std::shared_ptr<EulerAngles> &eulerAngles)
{
    HILOGI("GetCurrentPosition start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }

    {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        eulerAngles = std::make_shared<EulerAngles>(deviceStatus_->eulerAngles);
    }
    return ERR_OK;
}

int32_t MotionManager::GetRotationLimit(RotateDegreeLimit &rotationLimit)
{
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }

    {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        rotationLimit = deviceStatus_->rotationLimit;
    }
    return ERR_OK;
}

void MotionManager::UpdateTrackingTime()
{
    lastTrackingFrameTime_ = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(deviceStatusMutex_);
    if (deviceStatus_->trackingStatus == MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET) {
        deviceStatus_->trackingStatus = MechTrackingStatus::MECH_TK_ENABLE_WITH_TARGET;
        std::shared_ptr<CommonSetMechCameraTrackingEnableCmd> tkCmd = nullptr;
        tkCmd = factory.CreateSetMechCameraTrackingEnableCmd(deviceStatus_->trackingStatus);
        HILOGI("tkCmd, is:MECH_TK_ENABLE_WITH_TARGET.");
        if (sendAdapter_ != nullptr) {
            sendAdapter_->SendCommand(tkCmd);
        }
    }
}

int32_t MotionManager::SetMechCameraTrackingFrame(const std::shared_ptr<TrackingFrameParams> trackingFrameParams)
{
    HILOGI("SetMechCameraTrackingFrame start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    CHECK_POINTER_RETURN_VALUE(trackingFrameParams, INVALID_PARAMETERS_ERR, "trackingFrameParams");

    auto cameraTrackingFrameCallback = [this]() {
    };

    if (!deviceStatus_->isEnabled || deviceStatus_->trackingStatus == MechTrackingStatus::MECH_TK_DISABLE) {
        HILOGE("device tracking is not enabled");
        return ERR_OK;
    }

    if (protocolVer_ >= 0x02 && !SetMechCameraInfo_) {
        HILOGE("device cameraInfo is not enabled");
        return ERR_OK;
    }

    UpdateTrackingTime();

    std::shared_ptr<CommandBase> cameraTrackingFrameCmd = factory
            .CreateSetMechCameraTrackingFrameCmd(*trackingFrameParams);
    CHECK_POINTER_RETURN_VALUE(cameraTrackingFrameCmd, INVALID_PARAMETERS_ERR, "CameraTrackingFrameCmd is empty.");
    cameraTrackingFrameCmd->SetResponseCallback(cameraTrackingFrameCallback);
    cameraTrackingFrameCmd->SetTimeoutCallback(SetTimeout);
    std::chrono::microseconds nowUs =
        std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
    cameraTrackingFrameCmd->SetTimestamp(nowUs.count());

    sendAdapter_->SendCommand(cameraTrackingFrameCmd);
    return ERR_OK;
}

int32_t MotionManager::GetMechCameraTrackingEnabled(bool &isEnabled)
{
    HILOGI("GetCameraTrackingEnabled start.");
    {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        isEnabled = deviceStatus_->isEnabled;
    }
    return ERR_OK;
}

int32_t MotionManager::SetMechCameraTrackingEnabled(bool &isEnabled)
{
    HILOGI("SetMechCameraTrackingEnabled start.");
    {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        if (deviceStatus_ == nullptr) {
            HILOGE("device Status is nullptr");
            return ERR_OK;
        }
        deviceStatus_->isEnabled = isEnabled;
        deviceStatus_->trackingStatus =
            isEnabled ? MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET : MechTrackingStatus::MECH_TK_DISABLE;
        std::shared_ptr<CommonSetMechCameraTrackingEnableCmd> tkCmd =
            factory.CreateSetMechCameraTrackingEnableCmd(deviceStatus_->trackingStatus);
        sendAdapter_->SendCommand(tkCmd);
        HILOGI("device Status is enable: %{public}s", deviceStatus_->isEnabled ? "true" : "false");
    }
    McControllerManager::GetInstance().OnTrackingEvent(mechId_,
        isEnabled ? TrackingEvent::CAMERA_TRACKING_USER_ENABLED : TrackingEvent::CAMERA_TRACKING_USER_DISABLED);
    return ERR_OK;
}

int32_t MotionManager::SetMechCameraTrackingLayout(const std::shared_ptr<LayoutParams> layoutParams)
{
    HILOGI("SetCameraTrackingLayout start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    CHECK_POINTER_RETURN_VALUE(layoutParams, INVALID_PARAMETERS_ERR, "layoutParams");
    auto cameraTrackingLayoutCallback = [this]() {
    };

    std::shared_ptr<CommandBase> cameraTrackingLayoutCmd = factory
            .CreateSetMechCameraTrackingLayoutCmd(*layoutParams);
    CHECK_POINTER_RETURN_VALUE(cameraTrackingLayoutCmd, INVALID_PARAMETERS_ERR, "cameraTrackingLayoutCmd");
    cameraTrackingLayoutCmd->SetResponseCallback(cameraTrackingLayoutCallback);
    cameraTrackingLayoutCmd->SetTimeoutCallback(SetTimeout);
    sendAdapter_->SendCommand(cameraTrackingLayoutCmd);
    {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        deviceStatus_->layoutParams = *layoutParams;
    }
    HILOGI("SetCameraTrackingLayout end.");
    return ERR_OK;
}

int32_t MotionManager::GetMechCameraTrackingLayout(std::shared_ptr<LayoutParams> &layoutParams)
{
    HILOGI("GetCameraTrackingLayout start.");
    std::shared_ptr<GetMechCameraTrackingLayoutCmd> cameraTrackingLayoutCmd = factory
            .CreateGetMechCameraTrackingLayoutCmd();
    auto cameraTrackingLayoutCallback = [this, cameraTrackingLayoutCmd]() {
        HILOGI("GetCameraTrackingLayout: %{public}s", cameraTrackingLayoutCmd->GetParams().ToString().c_str());
    };

    CHECK_POINTER_RETURN_VALUE(cameraTrackingLayoutCmd, INVALID_PARAMETERS_ERR, "cameraTrackingLayoutCmd");
    cameraTrackingLayoutCmd->SetResponseCallback(cameraTrackingLayoutCallback);
    cameraTrackingLayoutCmd->SetTimeoutCallback(SetTimeout);
    sendAdapter_->SendCommand(cameraTrackingLayoutCmd);
    {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        layoutParams = std::make_shared<LayoutParams>(deviceStatus_->layoutParams);
    }
    return ERR_OK;
}

int32_t MotionManager::SetMechCameraInfo(const CameraInfoParams &mechCameraInfo)
{
    HILOGI("SetMechCameraInfo start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    auto setMechCameraInfoCallback = [this]() {
        SetMechCameraInfo_ = true;
        };
    std::shared_ptr<CommandBase> cameraInfoCmd = factory.CreateSetMechCameraInfoCmd(mechCameraInfo);
    CHECK_POINTER_RETURN_VALUE(cameraInfoCmd, INVALID_PARAMETERS_ERR, "cameraInfoCmd is empty.");
    cameraInfoCmd->SetResponseCallback(setMechCameraInfoCallback);
    sendAdapter_->SendCommand(cameraInfoCmd);
    HILOGI("SetMechCameraInfo end.");
    return ERR_OK;
}

int32_t MotionManager::SetMechScreenInfo(const ScreenInfoParams &screenInfo)
{
    HILOGI("SetScreenInfo start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    if (deviceBaseInfo_.devType != static_cast<uint8_t>(MechType::WHEEL_BASE)) {
        HILOGE("Device is not need screen info.");
        return DEVICE_NOT_NEED_SCREEN_INFO;
    }
    auto setScreenInfoCallback = [this]() {
        setMechScreenInfo_ = true;
    };
    auto screenInfoCmd = factory.CreateSetMechPhoneStatusCmd(screenInfo);
    CHECK_POINTER_RETURN_VALUE(screenInfoCmd, INVALID_PARAMETERS_ERR, "screenInfoCmd is empty.");
    screenInfoCmd->SetResponseCallback(setScreenInfoCallback);
    sendAdapter_->SendCommand(screenInfoCmd);
    HILOGI("SetScreenInfo end.");
    return ERR_OK;
}

int32_t MotionManager::GetMechBaseInfo(std::shared_ptr<MechBaseInfo> &mechBaseInfo)
{
    HILOGI("GetMechBaseInfo start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }

    if (deviceStatus_->mechBaseInfo.obtainable) {
        mechBaseInfo = std::make_shared<MechBaseInfo>(deviceStatus_->mechBaseInfo);
    } else {
        HILOGE("No basic platform information is found.");
        return INVALID_PARAMETERS_ERR;
    }
    return ERR_OK;
}

int32_t MotionManager::GetMechCapabilityInfo(std::shared_ptr<MechCapabilityInfo> &mechCapabilityInfo)
{
    HILOGI("GetMechCapabilityInfo start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }

    if (deviceStatus_->mechCapabilityInfo.obtainable) {
        mechCapabilityInfo = std::make_shared<MechCapabilityInfo>(deviceStatus_->mechCapabilityInfo);
    } else {
        HILOGE("No capability platform information is found.");
        return INVALID_PARAMETERS_ERR;
    }
    return ERR_OK;
}

int32_t MotionManager::GetRotationAxesStatus(const int32_t &mechId, RotationAxesStatus &axesStatus)
{
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }

    {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        axesStatus = deviceStatus_->rotationAxesStatus;
    }
    return ERR_OK;
}

int32_t MotionManager::PerformPresetAction(PresetAction action, int32_t delay)
{
    HILOGI("called, action %{public}d", static_cast<int32_t>(action));
    std::vector<RotateParam> trace;
    std::shared_ptr<CommonSetMechRotationTraceCmd> setMechRotationTraceCmd = nullptr;
    uint16_t taskId = CreateRotateTaskId();
    switch (action) {
        case PresetAction::NOD: {
            for (auto action : NOD_ACTIONS) {
                trace.push_back(action);
            }
            setMechRotationTraceCmd = factory.CreateSetMechRotationTraceCmd(taskId, trace);
            CHECK_POINTER_RETURN_VALUE(sendAdapter_, INVALID_PARAMETERS_ERR, "sendAdapter_");
            sendAdapter_->SendCommand(setMechRotationTraceCmd, delay);
            break;
        }
        default:
            break;
    }
    return ERR_OK;
}

void MotionManager::JudgingYawLimit(RotateDegreeLimit& limit)
{
    if (limit.posMax.yaw == NO_LIMIT_MAX && limit.negMax.yaw == NO_LIMIT_MIN) {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        deviceStatus_->yawLimit = false;
    } else {
        std::unique_lock <std::mutex> lock(this->deviceStatusMutex_);
        deviceStatus_->yawLimit = true;
    }

    if (limit.posMax.pitch == NO_LIMIT_MAX && limit.negMax.pitch == NO_LIMIT_MIN) {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        deviceStatus_->pitchLimit = false;
    } else {
        std::unique_lock <std::mutex> lock(this->deviceStatusMutex_);
        deviceStatus_->pitchLimit = true;
    }

    if (limit.posMax.roll == NO_LIMIT_MAX && limit.negMax.roll == NO_LIMIT_MIN) {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        deviceStatus_->rollLimit = false;
    } else {
        std::unique_lock <std::mutex> lock(this->deviceStatusMutex_);
        deviceStatus_->rollLimit = true;
    }
}

void MotionManager::AbsolutelyEulerAnglesJudgingLimitLocked(EulerAngles& eulerAngles)
{
    HILOGI("called begin, EulerAngles %{public}s", eulerAngles.ToString().c_str());
    if (deviceStatus_ == nullptr) {
        HILOGW("deviceStatus_ is empty.");
        return;
    }
    eulerAngles.yaw += deviceStatus_->eulerAngles.yaw;
    eulerAngles.roll = 0;
    eulerAngles.pitch += deviceStatus_->eulerAngles.pitch;
    auto status = deviceStatus_->rotationAxesStatus;
    auto limit = deviceStatus_->rotationLimit;
    HILOGI("called end, EulerAngles %{public}s", eulerAngles.ToString().c_str());
}

void MotionManager::LimitCalculationLocked(EulerAngles& position, RotationAxesStatus& status, bool callback)
{
    HILOGI("called begin, EulerAngles %{public}s", position.ToString().c_str());
    if (this->deviceStatus_ == nullptr) {
        HILOGE("deviceStatus_ is nullptr, can not get limit info");
        return;
    }
    float roll = position.roll;
    float pitch = position.pitch;
    float yaw = position.yaw;
    auto limit = this->deviceStatus_->rotationLimitWithOffset;
    HILOGI("Limit is: %{public}s; YAW_OFFSET: %{public}f",
        this->deviceStatus_->rotationLimitWithOffset.ToString().c_str(), YAW_OFFSET);
    if (deviceStatus_->rollLimit) {
        if (std::abs(roll - limit.posMax.roll) <= YAW_DISCREPANCY) {
            status.rollLimited = RotationAxisLimited::POS_LIMITED;
        } else if (std::abs(roll - limit.negMax.roll) <= YAW_DISCREPANCY) {
            status.rollLimited = RotationAxisLimited::NEG_LIMITED;
        } else {
            status.rollLimited = RotationAxisLimited::NOT_LIMITED;
        }
    }
    if (deviceStatus_->pitchLimit) {
        if (pitch > limit.posMax.pitch) {
            status.pitchLimited = RotationAxisLimited::POS_LIMITED;
        } else if (pitch < limit.negMax.pitch) {
            status.pitchLimited = RotationAxisLimited::NEG_LIMITED;
        } else {
            status.pitchLimited = RotationAxisLimited::NOT_LIMITED;
        }
    }
    if (deviceStatus_->yawLimit) {
        float yawOff = yaw - YAW_OFFSET < NO_LIMIT_MIN ? yaw - YAW_OFFSET + DEGREE_CIRCLED_MAX : yaw - YAW_OFFSET;
        if (yawOff > limit.posMax.yaw) {
            status.yawLimited = RotationAxisLimited::POS_LIMITED;
        } else if (yawOff < limit.negMax.yaw) {
            status.yawLimited = RotationAxisLimited::NEG_LIMITED;
        } else {
            status.yawLimited = RotationAxisLimited::NOT_LIMITED;
        }
    }
    if (callback && deviceStatus_->rotationAxesStatus != status) {
        deviceStatus_->rotationAxesStatus = status;
    }
}

int32_t MotionManager::ActionGimbalFeatureControl(const ActionControlParams &actionControlParams)
{
    HILOGI("ActionGimbalFeatureControl start.");
    if (protocolVer_ >= 0x02) {
        return ERR_OK;
    }
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }

    std::shared_ptr<CommandBase> actionControlCmd = factory.CreateActionGimbalFeatureControlCmd(actionControlParams);
    CHECK_POINTER_RETURN_VALUE(actionControlCmd, INVALID_PARAMETERS_ERR, "actionControlCmd is empty.");
    sendAdapter_->SendCommand(actionControlCmd);
    HILOGI("ActionGimbalFeatureControl end.");
    return ERR_OK;
}

const std::string &MotionManager::GetDeviceRealName() const
{
    return deviceRealName_;
}

void MotionManager::SetDeviceRealName(const std::string &deviceRealName)
{
    std::lock_guard<std::mutex> lock(deviceRealNameVisitMutex_);
    deviceRealName_ = deviceRealName;
    deviceRealNameVisitCon_.notify_all();
}

const std::string &MotionManager::TryGetDeviceRealNameSync(uint32_t timeoutMs)
{
    std::unique_lock<std::mutex> lock(deviceRealNameVisitMutex_);
    if (!deviceRealNameVisitCon_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
        [this] { return !deviceRealName_.empty(); })) {
        HILOGE("wait timeout, deviceRealName not set.");
        return deviceRealName_;
    }
    HILOGI("deviceRealName not empty return.");
    return deviceRealName_;
}

int32_t MotionManager::IsSupportAction(uint32_t tokenId, ActionType actionType, bool &isSupport)
{
    HILOGI("isSupportAction start.");
    if (MechConnectManager::GetInstance().GetMechState(mechId_) != AttachmentStateMap::ATTACHED) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    if (actionType == ActionType::LANDSCAPE_PORTRAIT_SWITCH) {
        isSupport = (deviceCapabilityInfo_.switchAble != 0);
    } else if (actionType == ActionType::GREET_MODE) {
        isSupport = (wheelCapabilityInfo_.followPersonMode != 0);
    } else if (actionType == ActionType::PATROL_MODE) {
        isSupport = (wheelCapabilityInfo_.tablePatrolMode != 0);
    } else  {
        isSupport = false;
    }
    HILOGI("actionType:%{public}d, isSupport: %{public}d .", actionType, isSupport);
    return ERR_OK;
}

MechEventListenerImpl::MechEventListenerImpl(std::shared_ptr<MotionManager> motionManager)
{
    HILOGI("MechEventListenerImpl called");
    motionManager_ = motionManager;
}

MechEventListenerImpl::~MechEventListenerImpl()
{
    HILOGI("MechEventListenerImpl called");
}

void MechEventListenerImpl::MechAttitudeNotify(const std::shared_ptr<CommonRegisterMechPositionInfoCmd> &cmd)
{
    CHECK_POINTER_RETURN(motionManager_, "motionManager_");
    motionManager_->MechAttitudeNotify(cmd);
}

void MechEventListenerImpl::MechButtonEventNotify(const std::shared_ptr<CommonRegisterMechKeyEventCmd> &cmd)
{
    CHECK_POINTER_RETURN(motionManager_, "motionManager_");
    motionManager_->MechButtonEventNotify(cmd);
}

void MechEventListenerImpl::MechParamNotify(const std::shared_ptr<CommonRegisterMechStateInfoCmd> &cmd)
{
    CHECK_POINTER_RETURN(motionManager_, "motionManager_");
    motionManager_->MechParamNotify(cmd);
}

void MechEventListenerImpl::MechGenericEventNotify(const std::shared_ptr<NormalRegisterMechGenericEventCmd>& cmd)
{
    CHECK_POINTER_RETURN(motionManager_, "motionManager_");
    motionManager_->MechGenericEventNotify(cmd);
}

void MechEventListenerImpl::MechCliffInfoNotify(const std::shared_ptr<RegisterMechCliffInfoCmd>& cmd)
{
    CHECK_POINTER_RETURN(motionManager_, "motionManager_");
    motionManager_->MechCliffInfoNotify(cmd);
}
void MechEventListenerImpl::MechObstacleInfoNotify(const std::shared_ptr<RegisterMechObstacleInfoCmd>& cmd)
{
    CHECK_POINTER_RETURN(motionManager_, "motionManager_");
    motionManager_->MechObstacleInfoNotify(cmd);
}

void MechEventListenerImpl::MechExecutionResultNotify(const std::shared_ptr<RegisterMechControlResultCmd> &cmd)
{
    CHECK_POINTER_RETURN(motionManager_, "motionManager_");
    motionManager_->MechExecutionResultNotify(cmd);
}

void MechEventListenerImpl::MechWheelZoomNotify(const std::shared_ptr<RegisterMechWheelDataCmd> &cmd)
{
    CHECK_POINTER_RETURN(motionManager_, "motionManager_");
    motionManager_->MechWheelZoomNotify(cmd);
}

void MechEventListenerImpl::MechTrackingStatusNotify(const std::shared_ptr<CommonRegisterMechTrackingEnableCmd> &cmd)
{
    CHECK_POINTER_RETURN(motionManager_, "motionManager_");
    motionManager_->MechTrackingStatusNotify(cmd);
}

MechNapiCommandCallbackInfo::MechNapiCommandCallbackInfo(uint32_t &tokenId, std::string &napiCmdId)
    : tokenId(tokenId), napiCmdId(napiCmdId)
{
}
} // namespace MechBodyController
} // namespace OHOS
