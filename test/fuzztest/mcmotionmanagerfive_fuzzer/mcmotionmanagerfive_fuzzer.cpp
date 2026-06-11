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
#include "mcmotionmanagerfive_fuzzer.h"
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
const std::string TAG = "MotionManagerFiveFuzz";
class MockCommandBase : public CommandBase {
public:
    MockCommandBase() = default;
    ~MockCommandBase() override = default;

    std::shared_ptr<MechDataBuffer> Marshal() const override
    {
        return nullptr;
    }

    void TriggerResponse(std::shared_ptr<MechDataBuffer> data) override {}

    void SetCmdSet(uint8_t cmdSet) { cmdSet_ = cmdSet; }
    void SetCmdId(uint8_t cmdId) { cmdId_ = cmdId; }
};

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

void FuzzAllCheckDegreeMethods(FuzzedDataProvider &provider)
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
    float rollResult = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    float pitchResult = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    
    g_motionManager->CheckRollDegree(rotateParam, limit, rollResult);
    g_motionManager->CheckPitchDegree(rotateParam, limit, pitchResult);
}

void FuzzAllCheckSpeedMethods(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.yawSpeed = provider.ConsumeFloatingPointInRange<float>(-1.0f, 1.0f);
    rotateBySpeedParam->speed.rollSpeed = provider.ConsumeFloatingPointInRange<float>(-1.0f, 1.0f);
    rotateBySpeedParam->speed.pitchSpeed = provider.ConsumeFloatingPointInRange<float>(-1.0f, 1.0f);
    rotateBySpeedParam->duration = provider.ConsumeIntegral<uint16_t>();
    
    RotateDegreeLimit limit;
    limit.posMax.yaw = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.yaw = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.roll = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.roll = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.pitch = provider.ConsumeFloatingPointInRange<float>(0.0f, 1.57f);
    limit.negMax.pitch = provider.ConsumeFloatingPointInRange<float>(-1.57f, 0.0f);
    
    float yawResult = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    float rollResult = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    float pitchResult = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    
    g_motionManager->CheckYawSpeed(rotateBySpeedParam, limit, yawResult);
    g_motionManager->CheckRollSpeed(rotateBySpeedParam, limit, rollResult);
    g_motionManager->CheckPitchSpeed(rotateBySpeedParam, limit, pitchResult);
}

void FuzzTryGetDeviceRealNameSync(FuzzedDataProvider &provider)
{
    InitMotionManager();
    uint32_t timeoutMs = provider.ConsumeIntegral<uint32_t>();
    g_motionManager->TryGetDeviceRealNameSync(timeoutMs);
    g_motionManager->TryGetDeviceRealNameSync(0);
}

void FuzzTryGetDeviceRealNameSyncWithTimeout(FuzzedDataProvider &provider)
{
    InitMotionManager();
    uint32_t timeoutMs = provider.ConsumeIntegralInRange<uint32_t>(1000, 5000);
    const std::string &deviceRealName = g_motionManager->TryGetDeviceRealNameSync(timeoutMs);
}

void FuzzTryGetDeviceRealNameSyncAfterSetName(FuzzedDataProvider &provider)
{
    InitMotionManager();
    std::string deviceRealName = provider.ConsumeRandomLengthString(32);
    g_motionManager->SetDeviceRealName(deviceRealName);
    
    uint32_t timeoutMs = provider.ConsumeIntegralInRange<uint32_t>(100, 1000);
    const std::string &result = g_motionManager->TryGetDeviceRealNameSync(timeoutMs);
}

void FuzzFormatLimit(FuzzedDataProvider &provider)
{
    InitMotionManager();
    RotateDegreeLimit params;
    params.posMax.yaw = provider.ConsumeFloatingPointInRange<float>(0.0f, 10.0f);
    params.negMax.yaw = provider.ConsumeFloatingPointInRange<float>(-10.0f, 0.0f);
    params.posMax.roll = provider.ConsumeFloatingPointInRange<float>(0.0f, 10.0f);
    params.negMax.roll = provider.ConsumeFloatingPointInRange<float>(-10.0f, 0.0f);
    params.posMax.pitch = provider.ConsumeFloatingPointInRange<float>(0.0f, 10.0f);
    params.negMax.pitch = provider.ConsumeFloatingPointInRange<float>(-10.0f, 0.0f);
    
    g_motionManager->FormatLimit(params);

    RotateDegreeLimit paramsNoLimit;
    paramsNoLimit.posMax.yaw = 3.1415927f;
    paramsNoLimit.negMax.yaw = -3.1415926f;
    paramsNoLimit.posMax.roll = 3.1415927f;
    paramsNoLimit.negMax.roll = -3.1415926f;
    paramsNoLimit.posMax.pitch = 3.1415927f;
    paramsNoLimit.negMax.pitch = -3.1415926f;
    
    g_motionManager->FormatLimit(paramsNoLimit);
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    FuzzAllCheckDegreeMethods(provider);
    FuzzAllCheckSpeedMethods(provider);
    FuzzTryGetDeviceRealNameSync(provider);
    FuzzTryGetDeviceRealNameSyncWithTimeout(provider);
    FuzzTryGetDeviceRealNameSyncAfterSetName(provider);
    FuzzFormatLimit(provider);
    return 0;
}