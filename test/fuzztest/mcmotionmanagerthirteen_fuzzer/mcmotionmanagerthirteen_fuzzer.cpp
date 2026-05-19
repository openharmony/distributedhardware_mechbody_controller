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
#include "mcmotionmanagerthirteen_fuzzer.h"
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
const std::string TAG = "MotionManagerThirteenFuzz";

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

void FuzzHandlePhoneOffMultipleCalls(FuzzedDataProvider &provider)
{
    InitMotionManager();
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 10);
    for (int32_t i = 0; i < iterations; ++i) {
        bool isPhoneOn = provider.ConsumeBool();
        g_motionManager->HandlePhoneOff(isPhoneOn);
    }
}

void FuzzHandlePhoneOnMultipleCalls(FuzzedDataProvider &provider)
{
    InitMotionManager();
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 10);
    for (int32_t i = 0; i < iterations; ++i) {
        bool isPhoneOn = provider.ConsumeBool();
        g_motionManager->HandlePhoneOn(isPhoneOn);
    }
}

void FuzzProcessPhoneOffForegroundCheckMultipleCalls(FuzzedDataProvider &provider)
{
    InitMotionManager();
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 10);
    for (int32_t i = 0; i < iterations; ++i) {
        g_motionManager->ProcessPhoneOffForegroundCheck();
    }
}

void FuzzProcessPhoneOnForegroundCheckMultipleCalls(FuzzedDataProvider &provider)
{
    InitMotionManager();
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 10);
    for (int32_t i = 0; i < iterations; ++i) {
        g_motionManager->ProcessPhoneOnForegroundCheck();
    }
}

void FuzzGetAppManagerInstanceMultipleCalls(FuzzedDataProvider &provider)
{
    InitMotionManager();
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 10);
    for (int32_t i = 0; i < iterations; ++i) {
        g_motionManager->GetAppManagerInstance();
    }
}

void FuzzMMIKeyEventAndCreateKeyEvent(FuzzedDataProvider &provider)
{
    InitMotionManager();
    int32_t keyCode = provider.ConsumeIntegral<int32_t>();
    int32_t keyAction = provider.ConsumeIntegral<int32_t>();
    int32_t eventTypeValue = provider.ConsumeIntegral<int32_t>();
    
    // 先创建事件再处理
    g_motionManager->CreateKeyEvent(keyCode, keyAction);
    g_motionManager->MMIKeyEvent(static_cast<CameraKeyEvent>(eventTypeValue));
    
    // 反向顺序
    g_motionManager->MMIKeyEvent(static_cast<CameraKeyEvent>(keyAction));
    g_motionManager->CreateKeyEvent(keyAction, keyCode);
}

void FuzzHandlePhoneOffAndOnCombination(FuzzedDataProvider &provider)
{
    InitMotionManager();
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 5);
    
    for (int32_t i = 0; i < iterations; ++i) {
        bool handleOffFirst = provider.ConsumeBool();
        bool isPhoneOn = provider.ConsumeBool();
        if (handleOffFirst) {
            g_motionManager->HandlePhoneOff(isPhoneOn);
            g_motionManager->HandlePhoneOn(isPhoneOn);
        } else {
            g_motionManager->HandlePhoneOn(isPhoneOn);
            g_motionManager->HandlePhoneOff(isPhoneOn);
        }
    }
}

void FuzzHandleMechPlacementChangeWithPhoneState(FuzzedDataProvider &provider)
{
    InitMotionManager();
    bool handlePhoneOff = provider.ConsumeBool();
    bool isPhoneOn = provider.ConsumeBool();
    
    g_motionManager->HandleMechPlacementChange(isPhoneOn);
    if (handlePhoneOff) {
        g_motionManager->HandlePhoneOff(isPhoneOn);
    } else {
        g_motionManager->HandlePhoneOn(isPhoneOn);
    }
}

void FuzzProcessPhoneForegroundCheckCombination(FuzzedDataProvider &provider)
{
    InitMotionManager();
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 5);
    
    for (int32_t i = 0; i < iterations; ++i) {
        bool checkOffFirst = provider.ConsumeBool();
        if (checkOffFirst) {
            g_motionManager->ProcessPhoneOffForegroundCheck();
            g_motionManager->ProcessPhoneOnForegroundCheck();
        } else {
            g_motionManager->ProcessPhoneOnForegroundCheck();
            g_motionManager->ProcessPhoneOffForegroundCheck();
        }
    }
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    FuzzHandlePhoneOffMultipleCalls(provider);
    FuzzHandlePhoneOnMultipleCalls(provider);
    FuzzProcessPhoneOffForegroundCheckMultipleCalls(provider);
    FuzzProcessPhoneOnForegroundCheckMultipleCalls(provider);
    FuzzGetAppManagerInstanceMultipleCalls(provider);
    FuzzMMIKeyEventAndCreateKeyEvent(provider);
    FuzzHandlePhoneOffAndOnCombination(provider);
    FuzzHandleMechPlacementChangeWithPhoneState(provider);
    FuzzProcessPhoneForegroundCheckCombination(provider);
    return 0;
}