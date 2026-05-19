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
#include "mcmotionmanagerfour_fuzzer.h"
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
const std::string TAG = "MotionManagerFourFuzz";

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

void FuzzFormatLimitWithExtremeValues(FuzzedDataProvider &provider)
{
    InitMotionManager();
    RotateDegreeLimit params;
    params.posMax.yaw = provider.ConsumeFloatingPointInRange<float>(10.0f, 100.0f);
    params.negMax.yaw = provider.ConsumeFloatingPointInRange<float>(-100.0f, -10.0f);
    params.posMax.roll = provider.ConsumeFloatingPointInRange<float>(10.0f, 100.0f);
    params.negMax.roll = provider.ConsumeFloatingPointInRange<float>(-100.0f, -10.0f);
    params.posMax.pitch = provider.ConsumeFloatingPointInRange<float>(10.0f, 100.0f);
    params.negMax.pitch = provider.ConsumeFloatingPointInRange<float>(-100.0f, -10.0f);
    
    g_motionManager->FormatLimit(params);
}

void FuzzAllThreeMethods(FuzzedDataProvider &provider)
{
    InitMotionManager();
    
    std::string deviceRealName = provider.ConsumeRandomLengthString(32);
    g_motionManager->SetDeviceRealName(deviceRealName);
    uint32_t timeoutMs = provider.ConsumeIntegralInRange<uint32_t>(100, 1000);
    const std::string &result = g_motionManager->TryGetDeviceRealNameSync(timeoutMs);
    
    RotateDegreeLimit params;
    params.posMax.yaw = provider.ConsumeFloatingPointInRange<float>(0.0f, 10.0f);
    params.negMax.yaw = provider.ConsumeFloatingPointInRange<float>(-10.0f, 0.0f);
    params.posMax.roll = provider.ConsumeFloatingPointInRange<float>(0.0f, 10.0f);
    params.negMax.roll = provider.ConsumeFloatingPointInRange<float>(-10.0f, 0.0f);
    params.posMax.pitch = provider.ConsumeFloatingPointInRange<float>(0.0f, 10.0f);
    params.negMax.pitch = provider.ConsumeFloatingPointInRange<float>(-10.0f, 0.0f);
    g_motionManager->FormatLimit(params);
}

void FuzzMechEventListenerImpl(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
}

void FuzzMechAttitudeNotify(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    
    auto cmd = std::make_shared<RegisterMechPositionInfoCmd>();
    auto data = std::make_shared<MechDataBuffer>(12);
    if (data != nullptr) {
        float yaw = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
        float roll = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
        float pitch = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
        data->AppendFloat(yaw);
        data->AppendFloat(roll);
        data->AppendFloat(pitch);
        cmd->Unmarshal(data);
        listener->MechAttitudeNotify(cmd);
    }
}

void FuzzMechAttitudeNotifyWithNull(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    std::shared_ptr<RegisterMechPositionInfoCmd> cmd = nullptr;
    listener->MechAttitudeNotify(cmd);
}

void FuzzMechButtonEventNotify(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    
    auto cmd = std::make_shared<RegisterMechCameraKeyEventCmd>();
    auto data = std::make_shared<MechDataBuffer>(8);
    if (data != nullptr) {
        uint8_t event = provider.ConsumeIntegralInRange<uint8_t>(0, 5);
        int16_t stickX = provider.ConsumeIntegral<int16_t>();
        int16_t stickY = provider.ConsumeIntegral<int16_t>();
        data->AppendUint8(event);
        data->AppendUint8(0x00);
        data->AppendInt16(stickX);
        data->AppendInt16(stickY);
        cmd->Unmarshal(data);
        listener->MechButtonEventNotify(cmd);
    }
}

void FuzzMechButtonEventNotifyWithNull(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    std::shared_ptr<RegisterMechCameraKeyEventCmd> cmd = nullptr;
    listener->MechButtonEventNotify(cmd);
}

void FuzzMechButtonEventNotifyWithAllEvents(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    
    for (uint8_t event = 0; event <= 5; event++) {
        auto cmd = std::make_shared<RegisterMechCameraKeyEventCmd>();
        auto data = std::make_shared<MechDataBuffer>(8);
        if (data != nullptr) {
            int16_t stickX = provider.ConsumeIntegral<int16_t>();
            int16_t stickY = provider.ConsumeIntegral<int16_t>();
            data->AppendUint8(event);
            data->AppendUint8(0x00);
            data->AppendInt16(stickX);
            data->AppendInt16(stickY);
            cmd->Unmarshal(data);
            listener->MechButtonEventNotify(cmd);
        }
    }
}

void FuzzMechParamNotify(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto listener = std::make_shared<MechEventListenerImpl>(g_motionManager);
    
    auto cmd = std::make_shared<RegisterMechStateInfoCmd>();
    auto data = std::make_shared<MechDataBuffer>(5);
    if (data != nullptr) {
        uint8_t isPhoneOn = provider.ConsumeIntegral<uint8_t>();
        uint8_t mechMode = provider.ConsumeIntegralInRange<uint8_t>(0, 4);
        data->AppendUint8(isPhoneOn);
        data->AppendUint8(mechMode);
        data->AppendUint8(0x00);
        data->AppendUint8(0x00);
        data->AppendUint8(0x00);
        cmd->Unmarshal(data);
        listener->MechParamNotify(cmd);
    }
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    FuzzFormatLimitWithExtremeValues(provider);
    FuzzAllThreeMethods(provider);
    FuzzMechEventListenerImpl(provider);
    FuzzMechAttitudeNotify(provider);
    FuzzMechAttitudeNotifyWithNull(provider);
    FuzzMechButtonEventNotify(provider);
    FuzzMechButtonEventNotifyWithNull(provider);
    FuzzMechButtonEventNotifyWithAllEvents(provider);
    FuzzMechParamNotify(provider);
    return 0;
}