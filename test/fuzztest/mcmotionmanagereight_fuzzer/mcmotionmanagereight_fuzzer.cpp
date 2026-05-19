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
#include "mcmotionmanagereight_fuzzer.h"
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
const std::string TAG = "MotionManagerEightFuzz";

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

void FuzzGetMechCapabilityInfo(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<MechCapabilityInfo> mechCapabilityInfo = nullptr;
    if (!shouldPassNull) {
        mechCapabilityInfo = std::make_shared<MechCapabilityInfo>();
    }
    g_motionManager->GetMechCapabilityInfo(mechCapabilityInfo);
}

void FuzzGetRotationAxesStatus(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    int32_t mechId = provider.ConsumeIntegral<int32_t>();
    RotationAxesStatus axesStatus;
    g_motionManager->GetRotationAxesStatus(mechId, axesStatus);
}

void FuzzIsSupportAction(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    ActionType actionType = static_cast<ActionType>(provider.ConsumeIntegralInRange<int32_t>(0, MAX_ACTION_TYPE_VALUE));
    bool isSupport = false;
    g_motionManager->IsSupportAction(tokenId, actionType, isSupport);
}

void FuzzRotate(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<RotateParam> rotateParam = nullptr;
    if (!shouldPassNull) {
        rotateParam = std::make_shared<RotateParam>();
        rotateParam->degree.yaw = provider.ConsumeFloatingPoint<float>();
        rotateParam->degree.roll = provider.ConsumeFloatingPoint<float>();
        rotateParam->degree.pitch = provider.ConsumeFloatingPoint<float>();
        rotateParam->duration = provider.ConsumeIntegral<uint16_t>();
        rotateParam->isRelative = provider.ConsumeBool();
    }
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    g_motionManager->Rotate(rotateParam, tokenId, napiCmdId);
}

void FuzzRotateBySpeed(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<RotateBySpeedParam> rotateSpeedParam = nullptr;
    if (!shouldPassNull) {
        rotateSpeedParam = std::make_shared<RotateBySpeedParam>();
        rotateSpeedParam->speed.yawSpeed = provider.ConsumeFloatingPoint<float>();
        rotateSpeedParam->speed.rollSpeed = provider.ConsumeFloatingPoint<float>();
        rotateSpeedParam->speed.pitchSpeed = provider.ConsumeFloatingPoint<float>();
        rotateSpeedParam->duration = provider.ConsumeIntegral<uint16_t>();
    }
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    g_motionManager->RotateBySpeed(rotateSpeedParam, tokenId, napiCmdId);
}

void FuzzStopRotate(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    g_motionManager->StopRotate(tokenId, napiCmdId);
}

void FuzzMove(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<MoveParams> moveParams = nullptr;
    if (!shouldPassNull) {
        moveParams = std::make_shared<MoveParams>();
        moveParams->distance = provider.ConsumeIntegral<int32_t>();
        moveParams->angle = provider.ConsumeFloatingPoint<float>();
        moveParams->speedGear = static_cast<SpeedGear>(provider.ConsumeIntegral<int32_t>());
        moveParams->mode = static_cast<MarchingMode>(provider.ConsumeIntegral<int32_t>());
    }
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    g_motionManager->Move(tokenId, napiCmdId, moveParams);
}

void FuzzMoveBySpeed(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    bool shouldPassNull = provider.ConsumeBool();
    std::shared_ptr<SpeedParams> speedParams = nullptr;
    if (!shouldPassNull) {
        speedParams = std::make_shared<SpeedParams>();
        speedParams->speed = provider.ConsumeIntegral<int16_t>();
        speedParams->angle = provider.ConsumeFloatingPoint<float>();
        speedParams->mode = static_cast<MarchingMode>(provider.ConsumeIntegral<int32_t>());
    }
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    uint16_t duration = provider.ConsumeIntegral<uint16_t>();
    g_motionManager->MoveBySpeed(tokenId, napiCmdId, duration, speedParams);
}

void FuzzTurnBySpeed(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    float angleSpeed = provider.ConsumeFloatingPoint<float>();
    uint16_t duration = provider.ConsumeIntegral<uint16_t>();
    g_motionManager->TurnBySpeed(tokenId, napiCmdId, angleSpeed, duration);
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzGetMechCapabilityInfo(data, size);
    FuzzGetRotationAxesStatus(data, size);
    FuzzIsSupportAction(data, size);
    FuzzRotate(data, size);
    FuzzRotateBySpeed(data, size);
    FuzzStopRotate(data, size);
    FuzzMove(data, size);
    FuzzMoveBySpeed(data, size);
    FuzzTurnBySpeed(data, size);
    return 0;
}