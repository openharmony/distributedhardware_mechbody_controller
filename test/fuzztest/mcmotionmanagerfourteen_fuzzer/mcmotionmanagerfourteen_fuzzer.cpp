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
#include "mcmotionmanagerfourteen_fuzzer.h"
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
constexpr int TASK_COMPLETED = 2;
}

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {
const std::string TAG = "MotionManagerFourteenFuzz";

class TestMotionManager : public MotionManager {
public:
    TestMotionManager(std::shared_ptr<TransportSendAdapter> sendAdapter, int32_t mechId, bool isWheel,
        uint32_t deviceIdentifier) : MotionManager(sendAdapter, mechId, isWheel, deviceIdentifier) {}
    void AddCallbackForTest(uint8_t taskId, const MechNapiCommandCallbackInfo& callbackInfo)
    {
        std::unique_lock<std::mutex> lock(seqCallbackMutex_);
        seqCallbacks_.insert(std::make_pair(taskId, callbackInfo));
    }
};

std::shared_ptr<TestMotionManager> g_motionManager = nullptr;
std::shared_ptr<MockTransportSendAdapter> g_mockAdapter = nullptr;

void InitMotionManager()
{
    if (g_motionManager == nullptr) {
        g_mockAdapter = std::make_shared<MockTransportSendAdapter>();
        g_motionManager = std::make_shared<TestMotionManager>(g_mockAdapter, TEST_MECH_ID, false, 0x00000000);

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

void FuzzIsLimitedWithParams(FuzzedDataProvider &provider)
{
    InitMotionManager();
    g_motionManager->IsLimited();
    float negMax = provider.ConsumeFloatingPoint<float>();
    float posMax = provider.ConsumeFloatingPoint<float>();
    float position = provider.ConsumeFloatingPoint<float>();
    g_motionManager->IsLimited(negMax, posMax, position);
    g_motionManager->ProcessTrackingStatus();
    g_motionManager->GetMechRealName();
    g_motionManager->GetWheelCapabilityInfo();
    g_motionManager->ConnectAbility();
    std::vector<AppExecFwk::AppStateData> list;
    g_motionManager->IsAiDispatchServiceInForeground(list);
}

void FuzzMechExecutionResultNotify(FuzzedDataProvider &provider)
{
    InitMotionManager();
    
    uint8_t taskId = provider.ConsumeIntegralInRange<uint8_t>(1, 255);
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = "test_cmd_" + std::to_string(taskId);
    
    MechNapiCommandCallbackInfo callbackInfo(tokenId, napiCmdId);
    callbackInfo.willLimitChange = provider.ConsumeBool();
    callbackInfo.needRestoreTracking = provider.ConsumeBool();
    
    g_motionManager->AddCallbackForTest(taskId, callbackInfo);
    
    auto cmd = std::make_shared<RegisterMechControlResultCmd>();
    
    auto buffer = std::make_shared<MechDataBuffer>(BIT_OFFSET_3 + 10);
    if (buffer == nullptr) {
        return;
    }
    
    buffer->AppendUint8(RegisterMechControlResultCmd::CMD_SET);
    buffer->AppendUint8(RegisterMechControlResultCmd::CMD_ID);
    buffer->AppendUint8(TASK_COMPLETED);
    buffer->AppendUint8(taskId);
    
    cmd->Unmarshal(buffer);
    
    g_motionManager->MechExecutionResultNotify(cmd);
    auto registerMechWheelDataCmd = std::make_shared<RegisterMechWheelDataCmd>();
    g_motionManager->MechWheelZoomNotify(registerMechWheelDataCmd);
}
}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    FuzzIsLimitedWithParams(provider);
    FuzzMechExecutionResultNotify(provider);
    return 0;
}