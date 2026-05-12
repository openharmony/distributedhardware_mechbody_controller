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
#include "mcmotionmanager_fuzzer.h"
#include "mc_motion_manager.h"
#include "mc_connect_manager.h"
#include "mechbody_controller_types.h"
#include "mechbody_controller_enums.h"
#include "mc_send_adapter.h"
#include "mc_command_factory.h"
#include "mc_register_mech_position_info_cmd.h"
#include "mc_register_mech_camera_key_event_cmd.h"
#include "mc_register_mech_state_info_cmd.h"
#include "mc_normal_register_mech_generic_event_cmd.h"
#include "mc_register_mech_cliff_info_cmd.h"
#include "mc_register_mech_obstacle_info_cmd.h"
#include "mc_register_mech_control_result_cmd.h"
#include "mc_register_mech_wheel_data_cmd.h"
#include "mc_register_mech_tracking_enable_cmd.h"
#include <memory>
#include <map>

namespace {
constexpr int32_t TEST_MECH_ID = 1;
constexpr uint32_t OPT_SIZE = 80;

constexpr uint8_t FUZZ_TEST_GET_SPEED_CONTROL_TIME_LIMIT = 0;
constexpr uint8_t FUZZ_TEST_GET_ROTATE_SPEED_LIMIT = 1;
constexpr uint8_t FUZZ_TEST_GET_CURRENT_POSITION = 2;
constexpr uint8_t FUZZ_TEST_GET_ROTATION_LIMIT = 3;
constexpr uint8_t FUZZ_TEST_SET_MECH_CAMERA_TRACKING_ENABLED = 4;
constexpr uint8_t FUZZ_TEST_GET_MECH_CAMERA_TRACKING_ENABLED = 5;
constexpr uint8_t FUZZ_TEST_SET_MECH_CAMERA_TRACKING_LAYOUT = 6;
constexpr uint8_t FUZZ_TEST_GET_MECH_CAMERA_TRACKING_LAYOUT = 7;
constexpr uint8_t FUZZ_TEST_GET_MECH_BASE_INFO = 8;
constexpr uint8_t FUZZ_TEST_GET_MECH_CAPABILITY_INFO = 9;
constexpr uint8_t FUZZ_TEST_GET_ROTATION_AXES_STATUS = 10;
constexpr uint8_t FUZZ_TEST_IS_SUPPORT_ACTION = 11;
constexpr uint8_t FUZZ_TEST_ROTATE = 12;
constexpr uint8_t FUZZ_TEST_ROTATE_BY_SPEED = 13;
constexpr uint8_t FUZZ_TEST_STOP_ROTATE = 14;
constexpr uint8_t FUZZ_TEST_MOVE = 15;
constexpr uint8_t FUZZ_TEST_MOVE_BY_SPEED = 16;
constexpr uint8_t FUZZ_TEST_TURN_BY_SPEED = 17;
constexpr uint8_t FUZZ_TEST_DO_ACTION = 18;
constexpr uint8_t FUZZ_TEST_SET_MECH_CAMERA_TRACKING_FRAME = 19;
constexpr uint8_t FUZZ_TEST_SET_MECH_CAMERA_INFO = 20;
constexpr uint8_t FUZZ_TEST_SET_MECH_SCREEN_INFO = 21;
constexpr uint8_t FUZZ_TEST_PERFORM_PRESET_ACTION = 22;
constexpr uint8_t FUZZ_TEST_ACTION_GIMBAL_FEATURE_CONTROL = 23;
constexpr uint8_t FUZZ_TEST_GET_DEVICE_REAL_NAME = 24;
constexpr uint8_t FUZZ_TEST_CONVERT_MOVE_PARAMS_TO_ROTATE_PARAMS = 25;
constexpr uint8_t FUZZ_TEST_CONVERT_SPEED_PARAMS_TO_ROTATE_PARAMS = 26;
constexpr uint8_t FUZZ_TEST_CHECK_WHEEL_SPEED_LIMIT = 27;
constexpr uint8_t FUZZ_TEST_MECH_ATTITUDE_NOTIFY = 28;
constexpr uint8_t FUZZ_TEST_MECH_BUTTON_EVENT_NOTIFY = 29;
constexpr uint8_t FUZZ_TEST_MECH_PARAM_NOTIFY = 30;
constexpr uint8_t FUZZ_TEST_MECH_GENERIC_EVENT_NOTIFY = 31;
constexpr uint8_t FUZZ_TEST_MECH_CLIFF_INFO_NOTIFY = 32;
constexpr uint8_t FUZZ_TEST_MECH_OBSTACLE_INFO_NOTIFY = 33;
constexpr uint8_t FUZZ_TEST_MECH_EXECUTION_RESULT_NOTIFY = 34;
constexpr uint8_t FUZZ_TEST_MECH_WHEEL_ZOOM_NOTIFY = 35;
constexpr uint8_t FUZZ_TEST_MECH_TRACKING_STATUS_NOTIFY = 36;
constexpr uint8_t FUZZ_TEST_ROTATE_WITH_EXTREME_VALUES = 37;
constexpr uint8_t FUZZ_TEST_MOVE_WITH_ZERO_PARAMS = 38;
constexpr uint8_t FUZZ_TEST_MOVE_BY_SPEED_WITH_ZERO_PARAMS = 39;
constexpr uint8_t FUZZ_TEST_ROTATE_BY_SPEED_WITH_ZERO_DURATION = 40;
constexpr uint8_t FUZZ_TEST_TURN_BY_SPEED_WITH_ZERO_DURATION = 41;
constexpr uint8_t FUZZ_TEST_COUNT = 42;
}

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

class MockTransportSendAdapter : public TransportSendAdapter {
public:
    MockTransportSendAdapter() = default;
    ~MockTransportSendAdapter() override = default;
    int32_t SendCommand(const std::shared_ptr<CommandBase> &command, int32_t delay = 0)
    {
        return 0;
    }
};

std::shared_ptr<MotionManager> g_motionManager = nullptr;
std::shared_ptr<MockTransportSendAdapter> g_mockAdapter = nullptr;

void InitMotionManager()
{
    if (g_motionManager == nullptr) {
        g_mockAdapter = std::make_shared<MockTransportSendAdapter>();
        g_motionManager = std::make_shared<MotionManager>(g_mockAdapter, TEST_MECH_ID, false, 0x00000000);

        // 添加测试设备信息并设置为已连接状态，以便测试需要 ATTACHED 状态的函数
        MechInfo testMech;
        testMech.mechId = TEST_MECH_ID;
        testMech.mechType = MechType::PORTABLE_GIMBAL;
        testMech.mac = "AA:BB:CC:DD:EE:FF";
        testMech.state = AttachmentState::ATTACHED;
        testMech.deviceIdentifier = 0x00000000;
        MechConnectManager::GetInstance().AddMechInfo(testMech);
        MechConnectManager::GetInstance().NotifyMechState(TEST_MECH_ID, true);
    }
}

void FuzzGetSpeedControlTimeLimit(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<TimeLimit> timeLimit = nullptr;
    if (!shouldPassNull) {
        timeLimit = std::make_shared<TimeLimit>();
    }
    g_motionManager->GetSpeedControlTimeLimit(timeLimit);
}

void FuzzGetRotateSpeedLimit(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    RotateSpeedLimit speedLimit;
    g_motionManager->GetRotateSpeedLimit(speedLimit);
}

void FuzzGetCurrentPosition(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<EulerAngles> eulerAngles = nullptr;
    if (!shouldPassNull) {
        eulerAngles = std::make_shared<EulerAngles>();
    }
    g_motionManager->GetCurrentPosition(eulerAngles);
}

void FuzzGetRotationLimit(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    RotateDegreeLimit rotationLimit;
    g_motionManager->GetRotationLimit(rotationLimit);
}

void FuzzGetMechCameraTrackingEnabled(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool isEnabled = false;
    g_motionManager->GetMechCameraTrackingEnabled(isEnabled);
}

void FuzzSetMechCameraTrackingEnabled(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool isEnabled = provider.ConsumeBool();
    g_motionManager->SetMechCameraTrackingEnabled(isEnabled);
}

void FuzzGetMechCameraTrackingLayout(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<LayoutParams> layoutParams = nullptr;
    if (!shouldPassNull) {
        layoutParams = std::make_shared<LayoutParams>();
    }
    g_motionManager->GetMechCameraTrackingLayout(layoutParams);
}

void FuzzSetMechCameraTrackingLayout(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<LayoutParams> layoutParams = nullptr;
    if (!shouldPassNull) {
        layoutParams = std::make_shared<LayoutParams>();
        layoutParams->isDefault = provider.ConsumeBool();
        layoutParams->offsetX = provider.ConsumeFloatingPoint<float>();
        layoutParams->offsetY = provider.ConsumeFloatingPoint<float>();
    }
    g_motionManager->SetMechCameraTrackingLayout(layoutParams);
}

void FuzzGetMechBaseInfo(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<MechBaseInfo> mechBaseInfo = nullptr;
    if (!shouldPassNull) {
        mechBaseInfo = std::make_shared<MechBaseInfo>();
    }
    g_motionManager->GetMechBaseInfo(mechBaseInfo);
}

void FuzzGetMechCapabilityInfo(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<MechCapabilityInfo> mechCapabilityInfo = nullptr;
    if (!shouldPassNull) {
        mechCapabilityInfo = std::make_shared<MechCapabilityInfo>();
    }
    g_motionManager->GetMechCapabilityInfo(mechCapabilityInfo);
}

void FuzzGetRotationAxesStatus(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    int32_t mechId = provider.ConsumeIntegral<int32_t>();
    RotationAxesStatus axesStatus;
    g_motionManager->GetRotationAxesStatus(mechId, axesStatus);
}

void FuzzIsSupportAction(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    ActionType actionType = static_cast<ActionType>(provider.ConsumeIntegral<int32_t>());
    bool isSupport = false;
    g_motionManager->IsSupportAction(tokenId, actionType, isSupport);
}

void FuzzRotate(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<RotateParam> rotateParam = nullptr;
    if (!shouldPassNull) {
        rotateParam = std::make_shared<RotateParam>();
        rotateParam->degree.yaw = provider.ConsumeFloatingPoint<float>();
        rotateParam->degree.roll = provider.ConsumeFloatingPoint<float>();
        rotateParam->degree.pitch = provider.ConsumeFloatingPoint<float>();
        rotateParam->duration = provider.ConsumeIntegral<uint16_t>();
        rotateParam->isRelative = provider.ConsumeBool();
    }
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    g_motionManager->Rotate(rotateParam, tokenId, napiCmdId);
}

void FuzzRotateBySpeed(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<RotateBySpeedParam> rotateSpeedParam = nullptr;
    if (!shouldPassNull) {
        rotateSpeedParam = std::make_shared<RotateBySpeedParam>();
        rotateSpeedParam->speed.yawSpeed = provider.ConsumeFloatingPoint<float>();
        rotateSpeedParam->speed.rollSpeed = provider.ConsumeFloatingPoint<float>();
        rotateSpeedParam->speed.pitchSpeed = provider.ConsumeFloatingPoint<float>();
        rotateSpeedParam->duration = provider.ConsumeIntegral<uint16_t>();
    }
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    g_motionManager->RotateBySpeed(rotateSpeedParam, tokenId, napiCmdId);
}

void FuzzStopRotate(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    g_motionManager->StopRotate(tokenId, napiCmdId);
}

void FuzzMove(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<MoveParams> moveParams = nullptr;
    if (!shouldPassNull) {
        moveParams = std::make_shared<MoveParams>();
        moveParams->distance = provider.ConsumeIntegral<int32_t>();
        moveParams->angle = provider.ConsumeFloatingPoint<float>();
        moveParams->speedGear = static_cast<SpeedGear>(provider.ConsumeIntegral<int32_t>());
        moveParams->mode = static_cast<MarchingMode>(provider.ConsumeIntegral<int32_t>());
    }
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    g_motionManager->Move(tokenId, napiCmdId, moveParams);
}

void FuzzMoveBySpeed(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<SpeedParams> speedParams = nullptr;
    if (!shouldPassNull) {
        speedParams = std::make_shared<SpeedParams>();
        speedParams->speed = provider.ConsumeIntegral<int16_t>();
        speedParams->angle = provider.ConsumeFloatingPoint<float>();
        speedParams->mode = static_cast<MarchingMode>(provider.ConsumeIntegral<int32_t>());
    }
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    uint16_t duration = provider.ConsumeIntegral<uint16_t>();
    g_motionManager->MoveBySpeed(tokenId, napiCmdId, duration, speedParams);
}

void FuzzTurnBySpeed(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    float angleSpeed = provider.ConsumeFloatingPoint<float>();
    uint16_t duration = provider.ConsumeIntegral<uint16_t>();
    g_motionManager->TurnBySpeed(tokenId, napiCmdId, angleSpeed, duration);
}

void FuzzDoAction(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    ActionType actionType = static_cast<ActionType>(provider.ConsumeIntegral<int32_t>());
    g_motionManager->DoAction(tokenId, napiCmdId, actionType);
}

void FuzzSetMechCameraTrackingFrame(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<TrackingFrameParams> trackingFrameParams = nullptr;
    if (!shouldPassNull) {
        trackingFrameParams = std::make_shared<TrackingFrameParams>();
        trackingFrameParams->targetId = provider.ConsumeIntegral<uint16_t>();
        trackingFrameParams->timeStamp = provider.ConsumeIntegral<uint64_t>();
        trackingFrameParams->cameraType = static_cast<CameraType>(provider.ConsumeIntegral<int32_t>());
        trackingFrameParams->confidence = static_cast<ConfidenceLevel>(provider.ConsumeIntegral<int32_t>());
        trackingFrameParams->objectType = provider.ConsumeIntegral<uint8_t>();
        trackingFrameParams->roi.x = provider.ConsumeFloatingPoint<float>();
        trackingFrameParams->roi.y = provider.ConsumeFloatingPoint<float>();
        trackingFrameParams->roi.width = provider.ConsumeFloatingPoint<float>();
        trackingFrameParams->roi.height = provider.ConsumeFloatingPoint<float>();
        trackingFrameParams->fovV = provider.ConsumeFloatingPoint<float>();
        trackingFrameParams->fovH = provider.ConsumeFloatingPoint<float>();
        trackingFrameParams->isRecording = provider.ConsumeBool();
        trackingFrameParams->timeDelay = provider.ConsumeIntegral<uint32_t>();
    }
    g_motionManager->SetMechCameraTrackingFrame(trackingFrameParams);
}

void FuzzSetMechCameraInfo(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    CameraInfoParams mechCameraInfo;
    mechCameraInfo.fovV = provider.ConsumeIntegral<uint8_t>();
    mechCameraInfo.fovH = provider.ConsumeIntegral<uint8_t>();
    mechCameraInfo.zoomFactor = provider.ConsumeFloatingPoint<float>();
    mechCameraInfo.isRecording = provider.ConsumeBool();
    mechCameraInfo.cameraType = static_cast<CameraType>(provider.ConsumeIntegral<int32_t>());
    g_motionManager->SetMechCameraInfo(mechCameraInfo);
}

void FuzzSetMechScreenInfo(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    ScreenInfoParams screenInfo;
    screenInfo.isPortrait = provider.ConsumeBool();
    g_motionManager->SetMechScreenInfo(screenInfo);
}

void FuzzPerformPresetAction(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    PresetAction action = static_cast<PresetAction>(provider.ConsumeIntegral<int32_t>());
    int32_t delay = provider.ConsumeIntegral<int32_t>();
    g_motionManager->PerformPresetAction(action, delay);
}

void FuzzActionGimbalFeatureControl(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    ActionControlParams actionControlParams;
    actionControlParams.controlReq = provider.ConsumeIntegral<uint8_t>();
    actionControlParams.timeOut = provider.ConsumeIntegral<uint16_t>();
    actionControlParams.yawControl = provider.ConsumeIntegral<uint32_t>();
    actionControlParams.pitchControl = provider.ConsumeIntegral<uint32_t>();
    actionControlParams.rollControl = provider.ConsumeIntegral<uint32_t>();
    g_motionManager->ActionGimbalFeatureControl(actionControlParams);
}

void FuzzGetDeviceRealName(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    const std::string &realName = g_motionManager->GetDeviceRealName();
}

void FuzzConvertMoveParamsToRotateParams(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    std::shared_ptr<MoveParams> moveParams = std::make_shared<MoveParams>();
    moveParams->distance = provider.ConsumeIntegral<int32_t>();
    moveParams->angle = provider.ConsumeFloatingPoint<float>();
    moveParams->speedGear = static_cast<SpeedGear>(provider.ConsumeIntegral<int32_t>());
    moveParams->mode = static_cast<MarchingMode>(provider.ConsumeIntegral<int32_t>());
    std::vector<RotateParam> result = g_motionManager->ConvertMoveParamsToRotateParams(moveParams);
}

void FuzzConvertSpeedParamsToRotateParams(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    std::shared_ptr<SpeedParams> speedParams = std::make_shared<SpeedParams>();
    speedParams->speed = provider.ConsumeIntegral<int16_t>();
    speedParams->angle = provider.ConsumeFloatingPoint<float>();
    speedParams->mode = static_cast<MarchingMode>(provider.ConsumeIntegral<int32_t>());
    int32_t duration = provider.ConsumeIntegral<int32_t>();
    std::vector<RotateParam> result = g_motionManager->ConvertSpeedParamsToRotateParams(speedParams, duration);
}

void FuzzCheckWheelSpeedLimit(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    std::vector<RotateParam> rotateParams;
    size_t numParams = provider.ConsumeIntegralInRange<size_t>(0, 10);
    for (size_t i = 0; i < numParams; ++i) {
        RotateParam param;
        param.forwardSpeed = provider.ConsumeIntegral<int16_t>();
        param.turningSpeed = provider.ConsumeFloatingPoint<float>();
        rotateParams.push_back(param);
    }
    g_motionManager->CheckWheelSpeedLimit(rotateParams);
}

void FuzzMechAttitudeNotify(const uint8_t *data, size_t size)
{
    InitMotionManager();
    auto cmd = std::make_shared<RegisterMechPositionInfoCmd>();
    g_motionManager->MechAttitudeNotify(cmd);
}

void FuzzMechButtonEventNotify(const uint8_t *data, size_t size)
{
    InitMotionManager();
    auto cmd = std::make_shared<RegisterMechCameraKeyEventCmd>();
    g_motionManager->MechButtonEventNotify(cmd);
}

void FuzzMechParamNotify(const uint8_t *data, size_t size)
{
    InitMotionManager();
    auto cmd = std::make_shared<RegisterMechStateInfoCmd>();
    g_motionManager->MechParamNotify(cmd);
}

void FuzzMechGenericEventNotify(const uint8_t *data, size_t size)
{
    InitMotionManager();
    auto cmd = std::make_shared<NormalRegisterMechGenericEventCmd>();
    g_motionManager->MechGenericEventNotify(cmd);
}

void FuzzMechCliffInfoNotify(const uint8_t *data, size_t size)
{
    InitMotionManager();
    auto cmd = std::make_shared<RegisterMechCliffInfoCmd>();
    g_motionManager->MechCliffInfoNotify(cmd);
}

void FuzzMechObstacleInfoNotify(const uint8_t *data, size_t size)
{
    InitMotionManager();
    auto cmd = std::make_shared<RegisterMechObstacleInfoCmd>();
    g_motionManager->MechObstacleInfoNotify(cmd);
}

void FuzzMechExecutionResultNotify(const uint8_t *data, size_t size)
{
    InitMotionManager();
    auto cmd = std::make_shared<RegisterMechControlResultCmd>();
    g_motionManager->MechExecutionResultNotify(cmd);
}

void FuzzMechWheelZoomNotify(const uint8_t *data, size_t size)
{
    InitMotionManager();
    auto cmd = std::make_shared<RegisterMechWheelDataCmd>();
    g_motionManager->MechWheelZoomNotify(cmd);
}

void FuzzMechTrackingStatusNotify(const uint8_t *data, size_t size)
{
    InitMotionManager();
    auto cmd = std::make_shared<RegisterMechTrackingEnableCmd>();
    g_motionManager->MechTrackingStatusNotify(cmd);
}

void FuzzRotateWithExtremeValues(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    auto rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = provider.ConsumeFloatingPoint<float>();
    rotateParam->degree.roll = provider.ConsumeFloatingPoint<float>();
    rotateParam->degree.pitch = provider.ConsumeFloatingPoint<float>();
    rotateParam->duration = provider.ConsumeIntegral<uint16_t>();
    rotateParam->isRelative = provider.ConsumeBool();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    g_motionManager->Rotate(rotateParam, tokenId, napiCmdId);
}

void FuzzMoveWithZeroParams(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    auto moveParams = std::make_shared<MoveParams>();
    moveParams->distance = 0;
    moveParams->angle = 0.0f;
    moveParams->speedGear = SpeedGear::LOW_SPEED;
    moveParams->mode = MarchingMode::TURNING_MOVING;
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    g_motionManager->Move(tokenId, napiCmdId, moveParams);
}

void FuzzMoveBySpeedWithZeroParams(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    auto speedParams = std::make_shared<SpeedParams>();
    speedParams->speed = 0;
    speedParams->angle = 0.0f;
    speedParams->mode = MarchingMode::TURNING_MOVING;
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    uint16_t duration = provider.ConsumeIntegral<uint16_t>();
    g_motionManager->MoveBySpeed(tokenId, napiCmdId, duration, speedParams);
}

void FuzzRotateBySpeedWithZeroDuration(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    auto rotateSpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateSpeedParam->speed.yawSpeed = provider.ConsumeFloatingPoint<float>();
    rotateSpeedParam->speed.rollSpeed = provider.ConsumeFloatingPoint<float>();
    rotateSpeedParam->speed.pitchSpeed = provider.ConsumeFloatingPoint<float>();
    rotateSpeedParam->duration = 0;
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    g_motionManager->RotateBySpeed(rotateSpeedParam, tokenId, napiCmdId);
}

void FuzzTurnBySpeedWithZeroDuration(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    float angleSpeed = provider.ConsumeFloatingPoint<float>();
    g_motionManager->TurnBySpeed(tokenId, napiCmdId, angleSpeed, 0);
}

using FuzzTestFunc = void (*)(const uint8_t *, size_t);

FuzzTestFunc GetFuzzTestFunc(uint8_t selector)
{
    static const FuzzTestFunc fuzzTestFuncs[] = {
        FuzzGetSpeedControlTimeLimit,
        FuzzGetRotateSpeedLimit,
        FuzzGetCurrentPosition,
        FuzzGetRotationLimit,
        FuzzSetMechCameraTrackingEnabled,
        FuzzGetMechCameraTrackingEnabled,
        FuzzSetMechCameraTrackingLayout,
        FuzzGetMechCameraTrackingLayout,
        FuzzGetMechBaseInfo,
        FuzzGetMechCapabilityInfo,
        FuzzGetRotationAxesStatus,
        FuzzIsSupportAction,
        FuzzRotate,
        FuzzRotateBySpeed,
        FuzzStopRotate,
        FuzzMove,
        FuzzMoveBySpeed,
        FuzzTurnBySpeed,
        FuzzDoAction,
        FuzzSetMechCameraTrackingFrame,
        FuzzSetMechCameraInfo,
        FuzzSetMechScreenInfo,
        FuzzPerformPresetAction,
        FuzzActionGimbalFeatureControl,
        FuzzGetDeviceRealName,
        FuzzConvertMoveParamsToRotateParams,
        FuzzConvertSpeedParamsToRotateParams,
        FuzzCheckWheelSpeedLimit,
        FuzzMechAttitudeNotify,
        FuzzMechButtonEventNotify,
        FuzzMechParamNotify,
        FuzzMechGenericEventNotify,
        FuzzMechCliffInfoNotify,
        FuzzMechObstacleInfoNotify,
        FuzzMechExecutionResultNotify,
        FuzzMechWheelZoomNotify,
        FuzzMechTrackingStatusNotify,
        FuzzRotateWithExtremeValues,
        FuzzMoveWithZeroParams,
        FuzzMoveBySpeedWithZeroParams,
        FuzzRotateBySpeedWithZeroDuration,
        FuzzTurnBySpeedWithZeroDuration
    };

    if (selector >= FUZZ_TEST_COUNT) {
        return nullptr;
    }
    return fuzzTestFuncs[selector];
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < OPT_SIZE) {
        return 0;
    }
    FuzzedDataProvider fdp(data + size - OPT_SIZE, OPT_SIZE);
    uint8_t selector = fdp.ConsumeIntegral<uint8_t>() % FUZZ_TEST_COUNT;

    FuzzTestFunc func = GetFuzzTestFunc(selector);
    if (func != nullptr) {
        func(data, size - OPT_SIZE);
    }
    return 0;
}