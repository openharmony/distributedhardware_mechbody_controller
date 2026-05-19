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
#include "mcmotionmanagernine_fuzzer.h"
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
#include "mc_get_mech_protocol_ver_cmd.h"
#include "mc_normal_get_mech_base_info_cmd.h"
#include "mc_normal_get_mech_capability_info_cmd.h"
#include "mc_get_mech_limit_info_cmd.h"
#include "mc_normal_set_mech_protocol_ver_cmd.h"
#include "mc_data_buffer.h"
#include <memory>
#include <map>
#include "mock_transport_send_adapter.h"
#include "mechbody_controller_log.h"

namespace {
constexpr int32_t TEST_MECH_ID = 1;
constexpr int32_t MAX_ACTION_TYPE_VALUE = 2006;
}

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {
const std::string TAG = "MotionManagerNineFuzz";

std::shared_ptr<MotionManager> g_motionManager = nullptr;
std::shared_ptr<MockTransportSendAdapter> g_mockAdapter = nullptr;

void InitMotionManager()
{
    if (g_motionManager == nullptr) {
        g_mockAdapter = std::make_shared<MockTransportSendAdapter>();
        g_motionManager = std::make_shared<MotionManager>(g_mockAdapter, TEST_MECH_ID, false, 0x00000000);

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

void FuzzDoAction(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    ActionType actionType = static_cast<ActionType>(provider.ConsumeIntegralInRange<int32_t>(0, MAX_ACTION_TYPE_VALUE));
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

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzDoAction(data, size);
    FuzzSetMechCameraTrackingFrame(data, size);
    FuzzSetMechCameraInfo(data, size);
    FuzzSetMechScreenInfo(data, size);
    FuzzPerformPresetAction(data, size);
    FuzzActionGimbalFeatureControl(data, size);
    FuzzGetDeviceRealName(data, size);
    FuzzConvertMoveParamsToRotateParams(data, size);
    FuzzConvertSpeedParamsToRotateParams(data, size);
    return 0;
}