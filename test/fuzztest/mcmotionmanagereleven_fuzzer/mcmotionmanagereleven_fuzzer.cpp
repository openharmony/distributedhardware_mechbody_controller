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
#include "mcmotionmanagereleven_fuzzer.h"
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
const std::string TAG = "MotionManagerElevenFuzz";

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

void FuzzMechTrackingStatusNotify(const uint8_t *data, size_t size)
{
    InitMotionManager();
    auto cmd = std::make_shared<RegisterMechTrackingEnableCmd>();
    g_motionManager->MechTrackingStatusNotify(cmd);
}

void FuzzRotateWithExtremeValues(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    auto rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = provider.ConsumeFloatingPoint<float>();
    rotateParam->degree.roll = provider.ConsumeFloatingPoint<float>();
    rotateParam->degree.pitch = provider.ConsumeFloatingPoint<float>();
    rotateParam->duration = provider.ConsumeIntegral<uint16_t>();
    rotateParam->isRelative = provider.ConsumeBool();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    g_motionManager->Rotate(rotateParam, tokenId, napiCmdId);
}

void FuzzMoveWithZeroParams(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    auto moveParams = std::make_shared<MoveParams>();
    moveParams->distance = 0;
    moveParams->angle = 0.0f;
    moveParams->speedGear = SpeedGear::LOW_SPEED;
    moveParams->mode = MarchingMode::TURNING_MOVING;
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    g_motionManager->Move(tokenId, napiCmdId, moveParams);
}

void FuzzMoveBySpeedWithZeroParams(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    auto speedParams = std::make_shared<SpeedParams>();
    speedParams->speed = 0;
    speedParams->angle = 0.0f;
    speedParams->mode = MarchingMode::TURNING_MOVING;
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    uint16_t duration = provider.ConsumeIntegral<uint16_t>();
    g_motionManager->MoveBySpeed(tokenId, napiCmdId, duration, speedParams);
}

void FuzzRotateBySpeedWithZeroDuration(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    auto rotateSpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateSpeedParam->speed.yawSpeed = provider.ConsumeFloatingPoint<float>();
    rotateSpeedParam->speed.rollSpeed = provider.ConsumeFloatingPoint<float>();
    rotateSpeedParam->speed.pitchSpeed = provider.ConsumeFloatingPoint<float>();
    rotateSpeedParam->duration = 0;
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    g_motionManager->RotateBySpeed(rotateSpeedParam, tokenId, napiCmdId);
}

void FuzzTurnBySpeedWithZeroDuration(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    std::string napiCmdId = provider.ConsumeRandomLengthString();
    float angleSpeed = provider.ConsumeFloatingPoint<float>();
    g_motionManager->TurnBySpeed(tokenId, napiCmdId, angleSpeed, 0);
}

void FuzzInit(const uint8_t *data, size_t size)
{
    HILOGI("FuzzInit start.");
    FuzzedDataProvider provider(data, size);
    bool isFirstConnect = provider.ConsumeBool();
    uint32_t deviceIdentifier = provider.ConsumeIntegral<uint32_t>();

    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    auto motionManager = std::make_shared<MotionManager>(mockAdapter, TEST_MECH_ID, isFirstConnect, deviceIdentifier);

    MechInfo testMech;
    testMech.mechId = TEST_MECH_ID;
    testMech.mechType = MechType::PORTABLE_GIMBAL;
    testMech.mac = "AA:BB:CC:DD:EE:FF";
    testMech.state = AttachmentState::ATTACHED;
    testMech.deviceIdentifier = deviceIdentifier;
    MechConnectManager::GetInstance().AddMechInfo(testMech);
    MechConnectManager::GetInstance().NotifyMechState(TEST_MECH_ID, true);

    motionManager->Init();
}

void FuzzMMIKeyEvent(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    int32_t eventTypeValue = provider.ConsumeIntegral<int32_t>();
    CameraKeyEvent eventType = static_cast<CameraKeyEvent>(eventTypeValue);
    g_motionManager->MMIKeyEvent(eventType);
}

void FuzzCreateKeyEvent(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitMotionManager();
    int32_t keyCode = provider.ConsumeIntegral<int32_t>();
    int32_t keyAction = provider.ConsumeIntegral<int32_t>();
    g_motionManager->CreateKeyEvent(keyCode, keyAction);
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzMechTrackingStatusNotify(data, size);
    FuzzRotateWithExtremeValues(data, size);
    FuzzMoveWithZeroParams(data, size);
    FuzzMoveBySpeedWithZeroParams(data, size);
    FuzzRotateBySpeedWithZeroDuration(data, size);
    FuzzTurnBySpeedWithZeroDuration(data, size);
    FuzzMMIKeyEvent(data, size);
    FuzzCreateKeyEvent(data, size);
    return 0;
}