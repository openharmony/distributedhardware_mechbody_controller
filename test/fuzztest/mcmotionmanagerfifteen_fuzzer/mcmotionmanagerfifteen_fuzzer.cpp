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
#include "mcmotionmanagerfifteen_fuzzer.h"
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
const std::string TAG = "MotionManagerFifteenFuzz";

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

void FuzzProcessTrackingStatusMultipleCalls(FuzzedDataProvider &provider)
{
    InitMotionManager();
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 10);
    for (int32_t i = 0; i < iterations; ++i) {
        g_motionManager->ProcessTrackingStatus();
    }
}

void FuzzGetMechRealNameMultipleCalls(FuzzedDataProvider &provider)
{
    InitMotionManager();
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 10);
    for (int32_t i = 0; i < iterations; ++i) {
        g_motionManager->GetMechRealName();
    }
}

void FuzzGetWheelCapabilityInfoMultipleCalls(FuzzedDataProvider &provider)
{
    InitMotionManager();
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 10);
    for (int32_t i = 0; i < iterations; ++i) {
        g_motionManager->GetWheelCapabilityInfo();
    }
}

void FuzzMechExecutionResultNotifyMultipleCalls(FuzzedDataProvider &provider)
{
    InitMotionManager();
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 10);
    for (int32_t i = 0; i < iterations; ++i) {
        auto cmd = std::make_shared<RegisterMechControlResultCmd>();
        g_motionManager->MechExecutionResultNotify(cmd);
    }
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    FuzzProcessTrackingStatusMultipleCalls(provider);
    FuzzGetMechRealNameMultipleCalls(provider);
    FuzzGetWheelCapabilityInfoMultipleCalls(provider);
    FuzzMechExecutionResultNotifyMultipleCalls(provider);
    return 0;
}