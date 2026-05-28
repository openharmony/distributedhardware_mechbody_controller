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
#include "mcmotionmanagersix_fuzzer.h"
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
constexpr int32_t DATA_LENGTH = 32;
}

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {
const std::string TAG = "MotionManagerSixFuzz";

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

void FuzzCheckRollDegreeWithNull(FuzzedDataProvider &provider)
{
    InitMotionManager();
    std::shared_ptr<RotateParam> rotateParam = nullptr;
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

void FuzzCheckPitchDegreeWithNull(FuzzedDataProvider &provider)
{
    InitMotionManager();
    std::shared_ptr<RotateParam> rotateParam = nullptr;
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

void FuzzHandelRotateSpeedParamWithNull(FuzzedDataProvider &provider)
{
    InitMotionManager();
    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = nullptr;
    bool willLimitChange = false;
    g_motionManager->HandelRotateSpeedParam(rotateBySpeedParam, willLimitChange);
}

void FuzzCheckYawSpeedWithNull(FuzzedDataProvider &provider)
{
    InitMotionManager();
    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = nullptr;
    RotateDegreeLimit limit;
    limit.posMax.yaw = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.yaw = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.roll = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.roll = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.pitch = provider.ConsumeFloatingPointInRange<float>(0.0f, 1.57f);
    limit.negMax.pitch = provider.ConsumeFloatingPointInRange<float>(-1.57f, 0.0f);
    
    float yawResult = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    g_motionManager->CheckYawSpeed(rotateBySpeedParam, limit, yawResult);
}

void FuzzCheckRollSpeedWithNull(FuzzedDataProvider &provider)
{
    InitMotionManager();
    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = nullptr;
    RotateDegreeLimit limit;
    limit.posMax.yaw = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.yaw = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.roll = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.roll = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.pitch = provider.ConsumeFloatingPointInRange<float>(0.0f, 1.57f);
    limit.negMax.pitch = provider.ConsumeFloatingPointInRange<float>(-1.57f, 0.0f);
    
    float rollResult = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    g_motionManager->CheckRollSpeed(rotateBySpeedParam, limit, rollResult);
}

void FuzzCheckPitchSpeedWithNull(FuzzedDataProvider &provider)
{
    InitMotionManager();
    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = nullptr;
    RotateDegreeLimit limit;
    limit.posMax.yaw = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.yaw = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.roll = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.roll = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.pitch = provider.ConsumeFloatingPointInRange<float>(0.0f, 1.57f);
    limit.negMax.pitch = provider.ConsumeFloatingPointInRange<float>(-1.57f, 0.0f);
    
    float pitchResult = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    g_motionManager->CheckPitchSpeed(rotateBySpeedParam, limit, pitchResult);
}

void FuzzSetDeviceRealNameWithEmptyString(FuzzedDataProvider &provider)
{
    InitMotionManager();
    std::string deviceRealName = "";
    g_motionManager->SetDeviceRealName(deviceRealName);
}

void FuzzSetDeviceRealNameWithLongString(FuzzedDataProvider &provider)
{
    InitMotionManager();
    std::string deviceRealName = provider.ConsumeRandomLengthString(256);
    g_motionManager->SetDeviceRealName(deviceRealName);
}

void FuzzJudgingYawLimitWithNoLimit(FuzzedDataProvider &provider)
{
    InitMotionManager();
    RotateDegreeLimit limit;
    limit.posMax.yaw = 3.14159f;
    limit.negMax.yaw = -3.14159f;
    limit.posMax.roll = 3.14159f;
    limit.negMax.roll = -3.14159f;
    limit.posMax.pitch = 3.14159f;
    limit.negMax.pitch = -3.14159f;
    
    g_motionManager->JudgingYawLimit(limit);
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    FuzzCheckRollDegreeWithNull(provider);
    FuzzCheckPitchDegreeWithNull(provider);
    FuzzHandelRotateSpeedParamWithNull(provider);
    FuzzCheckYawSpeedWithNull(provider);
    FuzzCheckRollSpeedWithNull(provider);
    FuzzCheckPitchSpeedWithNull(provider);
    FuzzSetDeviceRealNameWithEmptyString(provider);
    FuzzSetDeviceRealNameWithLongString(provider);
    FuzzJudgingYawLimitWithNoLimit(provider);
    return 0;
}