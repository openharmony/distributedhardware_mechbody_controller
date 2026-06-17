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
#include "mcmotionmanagerthree_fuzzer.h"
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
constexpr int32_t MAX_MECH_MODE_VALUE = 4;
}

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {
const std::string TAG = "MotionManagerThreeFuzz";

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

void FuzzMechParamNotifyWithAllModes(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    std::shared_ptr<RegisterMechStateInfoCmd> cmdNull = nullptr;
    listener->MechParamNotify(cmdNull);

    for (uint8_t mechMode = 0; mechMode <= MAX_MECH_MODE_VALUE; mechMode++) {
        auto cmd = std::make_shared<RegisterMechStateInfoCmd>();
        auto data = std::make_shared<MechDataBuffer>(5);
        if (data != nullptr) {
            uint8_t isPhoneOn = provider.ConsumeIntegral<uint8_t>();
            data->AppendUint8(isPhoneOn);
            data->AppendUint8(mechMode);
            data->AppendUint8(0x00);
            data->AppendUint8(0x00);
            data->AppendUint8(0x00);
            cmd->Unmarshal(data);
            listener->MechParamNotify(cmd);
        }
    }
}

void FuzzMechGenericEventNotify(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> cmdNull = nullptr;
    listener->MechGenericEventNotify(cmdNull);

    auto cmd = std::make_shared<NormalRegisterMechGenericEventCmd>();
    auto data = std::make_shared<MechDataBuffer>(5);
    if (data != nullptr) {
        uint8_t lowPower = provider.ConsumeIntegral<uint8_t>();
        uint8_t attached = provider.ConsumeIntegral<uint8_t>();
        uint8_t yawDisable = provider.ConsumeIntegral<uint8_t>();
        uint8_t rollDisable = provider.ConsumeIntegral<uint8_t>();
        uint8_t pitchDisable = provider.ConsumeIntegral<uint8_t>();
        data->AppendUint8(lowPower);
        data->AppendUint8(attached);
        data->AppendUint8(yawDisable);
        data->AppendUint8(rollDisable);
        data->AppendUint8(pitchDisable);
        cmd->Unmarshal(data);
        listener->MechGenericEventNotify(cmd);
    }
}

void FuzzMechGenericEventNotifyWithLowPower(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    
    auto cmd = std::make_shared<NormalRegisterMechGenericEventCmd>();
    auto data = std::make_shared<MechDataBuffer>(5);
    if (data != nullptr) {
        data->AppendUint8(0x01);
        data->AppendUint8(0x00);
        data->AppendUint8(0x00);
        data->AppendUint8(0x00);
        data->AppendUint8(0x00);
        cmd->Unmarshal(data);
        listener->MechGenericEventNotify(cmd);
    }
}

void FuzzMechCliffInfoNotify(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    
    auto cmd = std::make_shared<RegisterMechCliffInfoCmd>();
    auto data = std::make_shared<MechDataBuffer>(1);
    if (data != nullptr) {
        data->AppendUint8(0x00);
        cmd->Unmarshal(data);
        listener->MechCliffInfoNotify(cmd);
    }
}

void FuzzMechObstacleInfoNotify(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    
    auto cmd = std::make_shared<RegisterMechObstacleInfoCmd>();
    auto data = std::make_shared<MechDataBuffer>(1);
    if (data != nullptr) {
        data->AppendUint8(0x00);
        cmd->Unmarshal(data);
        listener->MechObstacleInfoNotify(cmd);
    }
}
}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    FuzzMechParamNotifyWithAllModes(provider);
    FuzzMechGenericEventNotify(provider);
    FuzzMechGenericEventNotifyWithLowPower(provider);
    FuzzMechCliffInfoNotify(provider);
    FuzzMechObstacleInfoNotify(provider);
    return 0;
}