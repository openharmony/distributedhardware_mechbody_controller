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
#include <stdexcept>
#include <iostream>

#include "mechbody_controller_log.h"
#include "mc_subscription_center.h"
#include "mechbody_controller_service.h"
#include "mc_camera_tracking_controller.h"
#include "mc_connect_manager.h"
#include "mechbody_controller_utils.h"

namespace OHOS {
namespace MechBodyController {
namespace {
const std::string TAG = "MotionManager";
const std::string EVENT_THREAD_NAME = "MotionManager_EventHandler";
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
constexpr int32_t RESPONSE_TIMEOUT = 10000;
constexpr float YAW_OFFSET = 0.2f;
constexpr int32_t CMD_SEND_INTERVAL = 100;

const std::map<CameraKeyEvent, int32_t> MAP_KEY_EVENT_VALUE = {
    {CameraKeyEvent::START_FILMING, MMI::KeyEvent::KEYCODE_VOLUME_UP},
    {CameraKeyEvent::SWITCH_CAMERA, MMI::KeyEvent::KEYCODE_VIDEO_NEXT},
    {CameraKeyEvent::ZOOM_IN, MMI::KeyEvent::KEYCODE_ZOOMIN},
    {CameraKeyEvent::ZOOM_OUT, MMI::KeyEvent::KEYCODE_ZOOMOUT},
    {CameraKeyEvent::SWITCH_PHOTO_FILM, MMI::KeyEvent::KEYCODE_VCR2}
};

const std::vector<RotateParam> NOD_ACTIONS = {
    {{0.0f, 0.0f, -0.02f}, 30, false},
    {{0.0f, 0.0f, 0.2f}, 80, false},
    {{0.0f, 0.0f, 0.0f}, 100, false}
};
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

void MotionManager::MechAttitudeNotify(const std::shared_ptr<RegisterMechPositionInfoCmd> &cmd)
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

void MotionManager::MechButtonEventNotify(const std::shared_ptr<RegisterMechCameraKeyEventCmd> &cmd)
{
    CHECK_POINTER_RETURN(cmd, "cmd");
    CameraKeyEvent eventType = cmd->GetEvent();
    HILOGI("Received gimbal key press event. eventNo: %{public}d.", eventType);

    std::shared_ptr<CameraInfo> cameraInfo = McCameraTrackingController::GetInstance().GetCurrentCameraInfo();
    CHECK_POINTER_RETURN(cameraInfo, "cameraInfo");
    if (!cameraInfo->isCameraOn) {
        HILOGI("No camera working, ignore.");
        return;
    }

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
}

void MotionManager::MechParamNotify(const std::shared_ptr<RegisterMechStateInfoCmd> &cmd)
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
        if (MechConnectManager::GetInstance().GetMechState(mechId_) != info.isPhoneOn) {
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

void MotionManager::HandleMechPlacementChange(bool isPhoneOn)
{
    HILOGI("start, isPhoneOn = %{public}d.", isPhoneOn);
    if (!isPhoneOn) {
        auto hidCmd = factory.CreateSetMechHidPreemptiveCmd(true);
        CHECK_POINTER_RETURN(hidCmd, "hidCmd is empty.");
        auto callback = [this, hidCmd, isPhoneOn]() {
            uint8_t result = hidCmd->GetResult();
            HILOGI("SetMechHidPreemptiveCmd result: %{public}u.", result);
            MechConnectManager::GetInstance().NotifyMechState(mechId_, isPhoneOn);
        };
        hidCmd->SetResponseCallback(callback);
        CHECK_POINTER_RETURN(sendAdapter_, "sendAdapter_");
        sendAdapter_->SendCommand(hidCmd);
        MechConnectManager::GetInstance().NotifyMechState(mechId_, isPhoneOn);
    } else {
        MechConnectManager::GetInstance().NotifyMechState(mechId_, isPhoneOn);
    }
    HILOGI("end.");
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
    if (!MechConnectManager::GetInstance().GetMechState(mechId_)) {
        HILOGE("Access is not allowed if the phone is not placed on.");
        return;
    }

    CHECK_POINTER_RETURN(cmd, "cmd");
    WheelData data = cmd->GetWheelData();
    auto func = [this, data]() {
        std::shared_ptr<CameraInfo> cameraInfo = McCameraTrackingController::GetInstance().GetCurrentCameraInfo();
        if (cameraInfo == nullptr || !cameraInfo->isCameraOn) {
            HILOGE("MechWheelZoomNotify current camera info is nullptr or no camera working, ignore");
            return;
        }

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

void MotionManager::MechTrackingStatusNotify(const std::shared_ptr<RegisterMechTrackingEnableCmd> &cmd)
{
    CHECK_POINTER_RETURN(cmd, "cmd");
    bool isEnabled = cmd->GetIsEnabled();
    HILOGI("Received mech tracking status notify, isEnabled: %{public}d.", isEnabled);
    TrackingEvent trackingEvent;
    std::shared_ptr<SetMechCameraTrackingEnableCmd> tkCmd = nullptr;
    {
        std::unique_lock<std::mutex> lock(deviceStatusMutex_);
        CHECK_POINTER_RETURN(deviceStatus_, "deviceStatus_ is empty.");
        if (!isEnabled) {
            trackingEvent = TrackingEvent::CAMERA_TRACKING_USER_DISABLED;
            deviceStatus_->isEnabled = false;
            tkCmd = factory.CreateSetMechCameraTrackingEnableCmd(MechTrackingStatus::MECH_TK_DISABLE);
        } else {
            trackingEvent = TrackingEvent::CAMERA_TRACKING_USER_ENABLED;
            deviceStatus_->isEnabled = true;
            tkCmd = factory.CreateSetMechCameraTrackingEnableCmd(MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET);
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

MotionManager::MotionManager(const std::shared_ptr<TransportSendAdapter> sendAdapter, int32_t mechId)
    : sendAdapter_(sendAdapter), mechId_(mechId)
{
    HILOGI("MotionManager start.");

    deviceStatus_ = std::make_shared<DeviceStatus>();
    deviceStatus_->rotateSpeedLimit.speedMin.yawSpeed = DEGREE_CIRCLED_MIN;
    deviceStatus_->rotateSpeedLimit.speedMin.rollSpeed = DEGREE_CIRCLED_MIN;
    deviceStatus_->rotateSpeedLimit.speedMin.pitchSpeed = DEGREE_CIRCLED_MIN;
    deviceStatus_->rotateSpeedLimit.speedMax.yawSpeed = DEGREE_CIRCLED_MAX;
    deviceStatus_->rotateSpeedLimit.speedMax.rollSpeed = DEGREE_CIRCLED_MIN;
    deviceStatus_->rotateSpeedLimit.speedMax.pitchSpeed = DEGREE_CIRCLED_MIN;

    std::shared_ptr<GetMechCapabilityInfoCmd> limitCmd = factory
            .CreateGetMechCapabilityInfoCmd();
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

    auto hidCmd = factory.CreateSetMechHidPreemptiveCmd(false);
    CHECK_POINTER_RETURN(hidCmd, "hidCmd is empty.");
    sendAdapter_->SendCommand(hidCmd, CMD_SEND_INTERVAL);

    auto tkCmd = factory.CreateSetMechCameraTrackingEnableCmd(MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET);
    CHECK_POINTER_RETURN(tkCmd, "tkCmd is empty.");
    sendAdapter_->SendCommand(tkCmd, CMD_SEND_INTERVAL * BIT_OFFSET_2);

    PerformPresetAction(PresetAction::NOD, CMD_SEND_INTERVAL * BIT_OFFSET_3);

    eventThread_ = std::thread(&MotionManager::StartEvent, this);
    std::unique_lock<std::mutex> lock(eventMutex_);
    eventCon_.wait(lock, [this] {
        return eventHandler_ != nullptr;
    });
    HILOGI("MotionManager end.");
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

void MotionManager::RegisterEventListener()
{
    HILOGI("RegisterEventListener start.");
    if (mechEventListener_ == nullptr) {
        HILOGI("RegisterReceiveListener create.");
        mechEventListener_ = std::make_shared<MechEventListenerImpl>(shared_from_this());
    }
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    CHECK_POINTER_RETURN(executionResultCmd, "ControlResultCmd is empty.");
    {
        std::lock_guard<std::mutex> lock(notifyListenerMutex_);
        notifyListenerType_.push_back(executionResultCmd->GetCmdType());
    }
    SubscriptionCenter::GetInstance().Subscribe(executionResultCmd->GetCmdType(), mechEventListener_);

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

    std::shared_ptr<CommandBase> wheelCmd = factory.CreateRegisterMechWheelDataCmd();
    CHECK_POINTER_RETURN(wheelCmd, "WheelDataCmd is empty.");
    {
        std::lock_guard<std::mutex> lock(notifyListenerMutex_);
        notifyListenerType_.push_back(wheelCmd->GetCmdType());
    }
    SubscriptionCenter::GetInstance().Subscribe(wheelCmd->GetCmdType(), mechEventListener_);

    std::shared_ptr<CommandBase> tkEnableCmd = factory.CreateRegisterMechTrackingEnableCmd();
    CHECK_POINTER_RETURN(tkEnableCmd, "tkEnableCmd is empty.");
    {
        std::lock_guard<std::mutex> lock(notifyListenerMutex_);
        notifyListenerType_.push_back(tkEnableCmd->GetCmdType());
    }
    SubscriptionCenter::GetInstance().Subscribe(tkEnableCmd->GetCmdType(), mechEventListener_);
    HILOGI("RegisterEventListener end.");
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

int32_t MotionManager::Rotate(std::shared_ptr<RotateParam> rotateParam,
    uint32_t &tokenId, std::string &napiCmdId)
{
    HILOGI("Rotate start.");
    if (!MechConnectManager::GetInstance().GetMechState(mechId_)) {
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
    HandelRotateParam(rotateParam, callbackInfo.willLimitChange);
    RotateParam param = *rotateParam;
    if (rotateParam->isRelative) {
        HILOGI("Rotate isRelative.");
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        AbsolutelyEulerAnglesJudgingLimitLocked(param.degree);
    }
    uint8_t taskId = CreateResponseTaskId();
    param.taskId = taskId;
    std::shared_ptr<CommandBase> rotationCmd = factory.CreateSetMechRotationCmd(param);
    CHECK_POINTER_RETURN_VALUE(rotationCmd, INVALID_PARAMETERS_ERR, "RotationCmd is empty.");

    {
        std::unique_lock<std::mutex> lock(seqCallbackMutex_);
        HILOGD("Insert callback id: %{public}d", taskId);
        seqCallbacks_.insert(std::make_pair(taskId, callbackInfo));
    }
    sendAdapter_->SendCommand(rotationCmd);
    auto func = [callbackInfo]() {
        HILOGI("notify rotate timeout. tokenid: %{public}s; napicmdid: %{public}s",
            GetAnonymUint32(callbackInfo.tokenId).c_str(), callbackInfo.napiCmdId.c_str());
        MechBodyControllerService::GetInstance().NotifyOperationResult(callbackInfo.tokenId,
            callbackInfo.napiCmdId, ExecResult::TIMEOUT);
    };

    std::string taskName = EVENT_THREAD_NAME + std::to_string(taskId);
    if (eventHandler_ != nullptr && (rotateParam->duration + RESPONSE_TIMEOUT) > 0) {
        eventHandler_->PostTask(func, taskName, rotateParam->duration + RESPONSE_TIMEOUT);
    }
    return ERR_OK;
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

int32_t MotionManager::RotateBySpeed(std::shared_ptr<RotateBySpeedParam> rotateSpeedParam,
    uint32_t &tokenId, std::string &napiCmdId)
{
    HILOGI("Rotate by speed start.");
    if (!MechConnectManager::GetInstance().GetMechState(mechId_)) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    CHECK_POINTER_RETURN_VALUE(rotateSpeedParam, INVALID_PARAMETERS_ERR, "rotateSpeedParam");
    MechNapiCommandCallbackInfo callbackInfo = {tokenId, napiCmdId};
    HandelRotateSpeedParam(rotateSpeedParam, callbackInfo.willLimitChange);
    std::shared_ptr<CommandBase> rotationBySpeedCmd =
            factory.CreateSetMechRotationBySpeedCmd(*rotateSpeedParam);
    CHECK_POINTER_RETURN_VALUE(rotationBySpeedCmd, INVALID_PARAMETERS_ERR, "RotationBySpeedCmd is empty.");
    sendAdapter_->SendCommand(rotationBySpeedCmd);

    auto func = [this, &callbackInfo]() {
        HILOGI("RotateBySpeed completed. tokenid: %{public}s; napicmdid: %{public}s",
               GetAnonymUint32(callbackInfo.tokenId).c_str(), callbackInfo.napiCmdId.c_str());
        MechBodyControllerService::GetInstance().NotifyOperationResult(callbackInfo.tokenId,
            callbackInfo.napiCmdId, ExecResult::COMPLETED);
        std::shared_ptr<CommandBase> motionCmd = factory.CreateSetMechStopCmd();
        CHECK_POINTER_RETURN(motionCmd, "StopCmd is empty.");
        sendAdapter_->SendCommand(motionCmd);
    };
    uint8_t taskId = CreateResponseTaskId();
    std::string taskName = EVENT_THREAD_NAME + std::to_string(taskId);
    if (eventHandler_ != nullptr && rotateSpeedParam->duration < DEFAULT_DURATION) {
        eventHandler_->PostTask(func, taskName, rotateSpeedParam->duration);
    }
    HILOGI("Rotate by speed end.");
    return ERR_OK;
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
    if (!MechConnectManager::GetInstance().GetMechState(mechId_)) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    std::shared_ptr<CommandBase> motionCmd = factory.CreateSetMechStopCmd();
    CHECK_POINTER_RETURN_VALUE(motionCmd, INVALID_PARAMETERS_ERR, "StopCmd is empty.");
    sendAdapter_->SendCommand(motionCmd);
    MechBodyControllerService::GetInstance().NotifyOperationResult(tokenId, napiCmdId, ExecResult::COMPLETED);
    HILOGI("Stop rotate end.");
    return ERR_OK;
}

int32_t MotionManager::GetSpeedControlTimeLimit(std::shared_ptr<TimeLimit> &timeLimit)
{
    HILOGI("GetSpeedControlTimeLimit start.");
    if (!MechConnectManager::GetInstance().GetMechState(mechId_)) {
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
    if (!MechConnectManager::GetInstance().GetMechState(mechId_)) {
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
    if (!MechConnectManager::GetInstance().GetMechState(mechId_)) {
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
    if (!MechConnectManager::GetInstance().GetMechState(mechId_)) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }

    {
        std::unique_lock <std::mutex> lock(deviceStatusMutex_);
        rotationLimit = deviceStatus_->rotationLimit;
    }
    return ERR_OK;
}

int32_t MotionManager::SetMechCameraTrackingFrame(const std::shared_ptr<TrackingFrameParams> trackingFrameParams)
{
    HILOGD("start.");
    if (!MechConnectManager::GetInstance().GetMechState(mechId_)) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }
    CHECK_POINTER_RETURN_VALUE(trackingFrameParams, INVALID_PARAMETERS_ERR, "trackingFrameParams");

    auto cameraTrackingFrameCallback = [this]() {
    };

    if (deviceStatus_->isEnabled) {
        HILOGI("Start tracking.");
        std::shared_ptr<CommandBase> cameraTrackingFrameCmd = factory
                .CreateSetMechCameraTrackingFrameCmd(*trackingFrameParams);
        CHECK_POINTER_RETURN_VALUE(cameraTrackingFrameCmd, INVALID_PARAMETERS_ERR, "CameraTrackingFrameCmd is empty.");
        cameraTrackingFrameCmd->SetResponseCallback(cameraTrackingFrameCallback);
        cameraTrackingFrameCmd->SetTimeoutCallback(SetTimeout);
        sendAdapter_->SendCommand(cameraTrackingFrameCmd);
    }
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
        HILOGI("device Status is enable: %{public}s", deviceStatus_->isEnabled ? "true" : "false");
    }
    return ERR_OK;
}

int32_t MotionManager::SetMechCameraTrackingLayout(const std::shared_ptr<LayoutParams> layoutParams)
{
    HILOGI("SetCameraTrackingLayout start.");
    if (!MechConnectManager::GetInstance().GetMechState(mechId_)) {
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
    if (!MechConnectManager::GetInstance().GetMechState(mechId_)) {
        HILOGE("Access is not allowed if the phone is not placed on mech.");
        return DEVICE_NOT_PLACED_ON_MECH;
    }

    std::shared_ptr<CommandBase> cameraInfoCmd = factory.CreateSetMechCameraInfoCmd(mechCameraInfo);
    CHECK_POINTER_RETURN_VALUE(cameraInfoCmd, INVALID_PARAMETERS_ERR, "cameraInfoCmd is empty.");
    sendAdapter_->SendCommand(cameraInfoCmd);
    HILOGI("SetMechCameraInfo end.");
    return ERR_OK;
}

int32_t MotionManager::GetMechBaseInfo(std::shared_ptr<MechBaseInfo> &mechBaseInfo)
{
    HILOGI("GetMechBaseInfo start.");
    if (!MechConnectManager::GetInstance().GetMechState(mechId_)) {
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
    if (!MechConnectManager::GetInstance().GetMechState(mechId_)) {
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
    if (!MechConnectManager::GetInstance().GetMechState(mechId_)) {
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
    std::shared_ptr<SetMechRotationTraceCmd> setMechRotationTraceCmd = nullptr;
    switch (action) {
        case PresetAction::NOD: {
            for (auto action : NOD_ACTIONS) {
                trace.push_back(action);
            }
            setMechRotationTraceCmd = factory.CreateSetMechRotationTraceCmd(trace);
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

MechEventListenerImpl::MechEventListenerImpl(std::shared_ptr<MotionManager> motionManager)
{
    HILOGI("MechEventListenerImpl called");
    motionManager_ = motionManager;
}

MechEventListenerImpl::~MechEventListenerImpl()
{
    HILOGI("MechEventListenerImpl called");
}

void MechEventListenerImpl::MechAttitudeNotify(const std::shared_ptr<RegisterMechPositionInfoCmd> &cmd)
{
    CHECK_POINTER_RETURN(motionManager_, "motionManager_");
    motionManager_->MechAttitudeNotify(cmd);
}

void MechEventListenerImpl::MechButtonEventNotify(const std::shared_ptr<RegisterMechCameraKeyEventCmd> &cmd)
{
    CHECK_POINTER_RETURN(motionManager_, "motionManager_");
    motionManager_->MechButtonEventNotify(cmd);
}

void MechEventListenerImpl::MechParamNotify(const std::shared_ptr<RegisterMechStateInfoCmd> &cmd)
{
    CHECK_POINTER_RETURN(motionManager_, "motionManager_");
    motionManager_->MechParamNotify(cmd);
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

void MechEventListenerImpl::MechTrackingStatusNotify(const std::shared_ptr<RegisterMechTrackingEnableCmd> &cmd)
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
