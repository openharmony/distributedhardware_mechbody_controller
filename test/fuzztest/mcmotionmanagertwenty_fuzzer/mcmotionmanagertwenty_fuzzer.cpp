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
#include "mcmotionmanagertwenty_fuzzer.h"
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
constexpr int32_t TWO_VALUE = 2;
}

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {
const std::string TAG = "MotionManagerTwentyFuzz";

std::shared_ptr<MotionManager> g_motionManager = nullptr;
std::shared_ptr<MockTransportSendAdapter> g_mockAdapter = nullptr;

class TestMotionManager : public MotionManager {
public:
    TestMotionManager(const std::shared_ptr<TransportSendAdapter> sendAdapter, int32_t mechId, bool isFirstConnect,
        const uint32_t &deviceIdentifier)
        : MotionManager(sendAdapter, mechId, isFirstConnect, deviceIdentifier) {}

    void SetTrackingStatusForTest(MechTrackingStatus status)
    {
        std::lock_guard<std::mutex> lock(deviceStatusMutex_);
        deviceStatus_->trackingStatus = status;
    }
};

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

void FuzzHandelRotateSpeedParam(FuzzedDataProvider &provider)
{
    InitMotionManager();
    auto rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.yawSpeed = provider.ConsumeFloatingPointInRange<float>(-1.0f, 1.0f);
    rotateBySpeedParam->speed.rollSpeed = provider.ConsumeFloatingPointInRange<float>(-1.0f, 1.0f);
    rotateBySpeedParam->speed.pitchSpeed = provider.ConsumeFloatingPointInRange<float>(-1.0f, 1.0f);
    rotateBySpeedParam->duration = provider.ConsumeIntegral<uint16_t>();
    
    bool willLimitChange = false;
    g_motionManager->HandelRotateSpeedParam(rotateBySpeedParam, willLimitChange);
}

void FuzzCheckYawSpeed(FuzzedDataProvider &provider)
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
    g_motionManager->CheckYawSpeed(rotateBySpeedParam, limit, yawResult);
}

void FuzzCheckRollSpeed(FuzzedDataProvider &provider)
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
    
    float rollResult = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    g_motionManager->CheckRollSpeed(rotateBySpeedParam, limit, rollResult);
}

void FuzzCheckPitchSpeed(FuzzedDataProvider &provider)
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
    
    float pitchResult = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    g_motionManager->CheckPitchSpeed(rotateBySpeedParam, limit, pitchResult);

    auto testManager = std::static_pointer_cast<TestMotionManager>(g_motionManager);
    testManager->SetTrackingStatusForTest(MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET);
    g_motionManager->UpdateTrackingTime();
}

void FuzzJudgingYawLimit(FuzzedDataProvider &provider)
{
    InitMotionManager();
    RotateDegreeLimit limit;
    limit.posMax.yaw = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.yaw = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.roll = provider.ConsumeFloatingPointInRange<float>(0.0f, 3.14f);
    limit.negMax.roll = provider.ConsumeFloatingPointInRange<float>(-3.14f, 0.0f);
    limit.posMax.pitch = provider.ConsumeFloatingPointInRange<float>(0.0f, 1.57f);
    limit.negMax.pitch = provider.ConsumeFloatingPointInRange<float>(-1.57f, 0.0f);
    
    g_motionManager->JudgingYawLimit(limit);
}

void FuzzAbsolutelyEulerAnglesJudgingLimitLocked(FuzzedDataProvider &provider)
{
    InitMotionManager();
    EulerAngles eulerAngles;
    eulerAngles.yaw = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    eulerAngles.roll = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    eulerAngles.pitch = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    
    g_motionManager->AbsolutelyEulerAnglesJudgingLimitLocked(eulerAngles);
}

void FuzzLimitCalculationLocked(FuzzedDataProvider &provider)
{
    InitMotionManager();
    EulerAngles position;
    position.yaw = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    position.roll = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    position.pitch = provider.ConsumeFloatingPointInRange<float>(-6.28f, 6.28f);
    
    RotationAxesStatus status;
    status.yawLimited = static_cast<RotationAxisLimited>(provider.ConsumeIntegralInRange<int32_t>(0, TWO_VALUE));
    status.rollLimited = static_cast<RotationAxisLimited>(provider.ConsumeIntegralInRange<int32_t>(0, TWO_VALUE));
    status.pitchLimited = static_cast<RotationAxisLimited>(provider.ConsumeIntegralInRange<int32_t>(0, TWO_VALUE));
    
    bool callback = provider.ConsumeBool();
    g_motionManager->LimitCalculationLocked(position, status, callback);
}

void FuzzSetDeviceRealName(FuzzedDataProvider &provider)
{
    InitMotionManager();
    std::string deviceRealName = provider.ConsumeRandomLengthString(32);
    g_motionManager->SetDeviceRealName(deviceRealName);
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    FuzzHandelRotateSpeedParam(provider);
    FuzzCheckYawSpeed(provider);
    FuzzCheckRollSpeed(provider);
    FuzzCheckPitchSpeed(provider);
    FuzzJudgingYawLimit(provider);
    FuzzAbsolutelyEulerAnglesJudgingLimitLocked(provider);
    FuzzLimitCalculationLocked(provider);
    FuzzSetDeviceRealName(provider);
    return 0;
}