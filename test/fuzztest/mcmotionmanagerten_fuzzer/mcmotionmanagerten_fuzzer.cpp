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
#include "mcmotionmanagerten_fuzzer.h"
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
const std::string TAG = "MotionManagerTenFuzz";

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
    FuzzedDataProvider provider(data, size);
    
    auto cmd = std::make_shared<RegisterMechStateInfoCmd>();
    
    auto buffer = std::make_shared<MechDataBuffer>(BIT_OFFSET_2 + 10);
    if (buffer == nullptr) {
        return;
    }
    
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);
    buffer->AppendUint8(RegisterMechStateInfoCmd::CMD_SET);
    buffer->AppendUint8(RegisterMechStateInfoCmd::CMD_ID);
    
    uint8_t mechMode = provider.ConsumeIntegralInRange<uint8_t>(0, 4);
    uint8_t isCaptureVertical = provider.ConsumeBool() ? 1 : 0;
    uint8_t isPhoneOn = provider.ConsumeBool() ? 1 : 0;
    
    buffer->AppendUint8(mechMode);
    buffer->AppendUint8(isCaptureVertical);
    buffer->AppendUint8(isPhoneOn);
    
    cmd->Unmarshal(buffer);
    
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

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzCheckWheelSpeedLimit(data, size);
    FuzzMechAttitudeNotify(data, size);
    FuzzMechButtonEventNotify(data, size);
    FuzzMechParamNotify(data, size);
    FuzzMechGenericEventNotify(data, size);
    FuzzMechCliffInfoNotify(data, size);
    FuzzMechObstacleInfoNotify(data, size);
    FuzzMechExecutionResultNotify(data, size);
    FuzzMechWheelZoomNotify(data, size);
    return 0;
}