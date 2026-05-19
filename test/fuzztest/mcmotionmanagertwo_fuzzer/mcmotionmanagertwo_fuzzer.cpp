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
#include "mcmotionmanagertwo_fuzzer.h"
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
#include "mechbody_controller_log.h"
#include "mock_transport_send_adapter.h"

namespace {
constexpr int32_t TEST_MECH_ID = 1;
constexpr int32_t MAX_ACTION_TYPE_VALUE = 2006;
}

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {
const std::string TAG = "MotionManagerTwoFuzz";

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

void FuzzMechExecutionResultNotify(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    
    auto cmd = std::make_shared<RegisterMechControlResultCmd>();
    auto data = std::make_shared<MechDataBuffer>(3);
    if (data != nullptr) {
        uint8_t result = provider.ConsumeIntegral<uint8_t>();
        uint8_t taskId = provider.ConsumeIntegral<uint8_t>();
        data->AppendUint8(result);
        data->AppendUint8(taskId);
        data->AppendUint8(0x00);
        cmd->Unmarshal(data);
        listener->MechExecutionResultNotify(cmd);
    }
}

void FuzzMechExecutionResultNotifyWithNull(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    std::shared_ptr<RegisterMechControlResultCmd> cmd = nullptr;
    listener->MechExecutionResultNotify(cmd);
}

void FuzzMechExecutionResultNotifyWithCompleted(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    
    auto cmd = std::make_shared<RegisterMechControlResultCmd>();
    auto data = std::make_shared<MechDataBuffer>(3);
    if (data != nullptr) {
        data->AppendUint8(0x02);
        data->AppendUint8(0x01);
        data->AppendUint8(0x00);
        cmd->Unmarshal(data);
        listener->MechExecutionResultNotify(cmd);
    }
}

void FuzzMechWheelZoomNotify(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    
    auto cmd = std::make_shared<RegisterMechWheelDataCmd>();
    auto data = std::make_shared<MechDataBuffer>(5);
    if (data != nullptr) {
        int32_t degree = provider.ConsumeIntegral<int32_t>();
        int16_t speed = provider.ConsumeIntegral<int16_t>();
        uint8_t speedRatio = provider.ConsumeIntegral<uint8_t>();
        data->AppendUint32(static_cast<uint32_t>(degree));
        data->AppendInt16(speed);
        data->AppendUint8(speedRatio);
        cmd->Unmarshal(data);
        listener->MechWheelZoomNotify(cmd);
    }
}

void FuzzMechWheelZoomNotifyWithNull(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    std::shared_ptr<RegisterMechWheelDataCmd> cmd = nullptr;
    listener->MechWheelZoomNotify(cmd);
}

void FuzzMechWheelZoomNotifyWithZeroDegree(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    
    auto cmd = std::make_shared<RegisterMechWheelDataCmd>();
    auto data = std::make_shared<MechDataBuffer>(5);
    if (data != nullptr) {
        data->AppendUint32(0);
        data->AppendInt16(0);
        data->AppendUint8(0);
        cmd->Unmarshal(data);
        listener->MechWheelZoomNotify(cmd);
    }
}

void FuzzMechTrackingStatusNotify(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    
    auto cmd = std::make_shared<RegisterMechTrackingEnableCmd>();
    auto data = std::make_shared<MechDataBuffer>(2);
    if (data != nullptr) {
        uint8_t isEnabled = provider.ConsumeIntegral<uint8_t>();
        data->AppendUint8(isEnabled);
        data->AppendUint8(0x00);
        cmd->Unmarshal(data);
        listener->MechTrackingStatusNotify(cmd);
    }
}

void FuzzMechTrackingStatusNotifyWithNull(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    std::shared_ptr<RegisterMechTrackingEnableCmd> cmd = nullptr;
    listener->MechTrackingStatusNotify(cmd);
}

void FuzzMechTrackingStatusNotifyWithEnabled(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    
    auto cmd = std::make_shared<RegisterMechTrackingEnableCmd>();
    auto data = std::make_shared<MechDataBuffer>(2);
    if (data != nullptr) {
        data->AppendUint8(0x01);
        data->AppendUint8(0x00);
        cmd->Unmarshal(data);
        listener->MechTrackingStatusNotify(cmd);
    }
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    FuzzMechExecutionResultNotify(provider);
    FuzzMechExecutionResultNotifyWithNull(provider);
    FuzzMechExecutionResultNotifyWithCompleted(provider);
    FuzzMechWheelZoomNotify(provider);
    FuzzMechWheelZoomNotifyWithNull(provider);
    FuzzMechWheelZoomNotifyWithZeroDegree(provider);
    FuzzMechTrackingStatusNotify(provider);
    FuzzMechTrackingStatusNotifyWithNull(provider);
    FuzzMechTrackingStatusNotifyWithEnabled(provider);
    return 0;
}