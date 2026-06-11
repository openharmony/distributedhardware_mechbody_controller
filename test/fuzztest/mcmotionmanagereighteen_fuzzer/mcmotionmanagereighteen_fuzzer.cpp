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
#include "mcmotionmanagereighteen_fuzzer.h"
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
const std::string TAG = "MotionManagerEighteenFuzz";

class TestMotionManager : public MotionManager {
public:
    TestMotionManager(std::shared_ptr<TransportSendAdapter> sendAdapter, int32_t mechId, bool isWheel,
        uint32_t deviceIdentifier) : MotionManager(sendAdapter, mechId, isWheel, deviceIdentifier) {}
    void SetProtocolVerForTest(uint8_t ver) {
        protocolVer_ = ver;
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

void FuzzExecuteRotateCommand(FuzzedDataProvider &provider)
{
    InitMotionManager();
    RotateParam param;
    param.degree.yaw = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    param.degree.roll = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    param.degree.pitch = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    param.duration = provider.ConsumeIntegral<uint16_t>();
    param.isRelative = provider.ConsumeBool();
    
    uint8_t taskId = provider.ConsumeIntegral<uint8_t>();
    std::shared_ptr<CommandBase> result = g_motionManager->ExecuteRotateCommand(param, taskId);
    g_motionManager->SetProtocolVerForTest(0x01);
    g_motionManager->RegisterEventListenerV01();

    g_motionManager->RegisterEventListener();
    g_motionManager->UnRegisterNotifyEvent();
}

void FuzzHandelRotateParam(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    rotateParam->degree.roll = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    rotateParam->degree.pitch = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    rotateParam->duration = provider.ConsumeIntegral<uint16_t>();
    rotateParam->isRelative = provider.ConsumeBool();
    
    bool willLimitChange = false;
    g_motionManager->HandelRotateParam(rotateParam, willLimitChange);
}

void FuzzCheckYawDegree(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    rotateParam->degree.roll = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    rotateParam->degree.pitch = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    rotateParam->duration = provider.ConsumeIntegral<uint16_t>();
    rotateParam->isRelative = provider.ConsumeBool();
    
    RotateDegreeLimit limit;
    limit.posMax.yaw = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.yaw = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.roll = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.roll = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.pitch = provider.ConsumeFloatingPointInRange<float>(0.0f, 1.57f);
    limit.negMax.pitch = provider.ConsumeFloatingPointInRange<float>(-1.57f, 0.0f);
    
    float yawResult = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    g_motionManager->CheckYawDegree(rotateParam, limit, yawResult);
}

void FuzzCheckRollDegree(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    rotateParam->degree.roll = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    rotateParam->degree.pitch = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    rotateParam->duration = provider.ConsumeIntegral<uint16_t>();
    rotateParam->isRelative = provider.ConsumeBool();
    
    RotateDegreeLimit limit;
    limit.posMax.yaw = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.yaw = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.roll = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.roll = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.pitch = provider.ConsumeFloatingPointInRange<float>(0.0f, 1.57f);
    limit.negMax.pitch = provider.ConsumeFloatingPointInRange<float>(-1.57f, 0.0f);
    
    float rollResult = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    g_motionManager->CheckRollDegree(rotateParam, limit, rollResult);
}

void FuzzCheckPitchDegree(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    rotateParam->degree.roll = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    rotateParam->degree.pitch = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    rotateParam->duration = provider.ConsumeIntegral<uint16_t>();
    rotateParam->isRelative = provider.ConsumeBool();
    
    RotateDegreeLimit limit;
    limit.posMax.yaw = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.yaw = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.roll = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.roll = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.pitch = provider.ConsumeFloatingPointInRange<float>(0.0f, 1.57f);
    limit.negMax.pitch = provider.ConsumeFloatingPointInRange<float>(-1.57f, 0.0f);
    
    float pitchResult = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    g_motionManager->CheckPitchDegree(rotateParam, limit, pitchResult);
}

void FuzzRegisterEventsMultipleTimes(FuzzedDataProvider &provider)
{
    InitMotionManager();
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 10);
    for (int32_t i = 0; i < iterations; ++i) {
        g_motionManager->RegisterEventListener();
    }
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    FuzzExecuteRotateCommand(provider);
    FuzzHandelRotateParam(provider);
    FuzzCheckYawDegree(provider);
    FuzzCheckRollDegree(provider);
    FuzzCheckPitchDegree(provider);
    FuzzRegisterEventsMultipleTimes(provider);
    return 0;
}