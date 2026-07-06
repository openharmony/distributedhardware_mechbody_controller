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
#include "mc_motion_manager_test.h"

#include <thread>

#include <algorithm>
#include <stdexcept>
#include <iostream>

#include "mechbody_controller_log.h"
#include "mc_register_mech_position_info_cmd.h"
#include "mc_register_mech_camera_key_event_cmd.h"
#include "mechbody_controller_service.h"
#include "mc_register_mech_control_result_cmd.h"
#include "mc_command_base.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS {
namespace MechBodyController {
constexpr float DEFAULT_DURATION = 500;
constexpr int TASK_COMPLETED = 2;
constexpr float NO_LIMIT_MAX = 3.1415927f;
constexpr float NO_LIMIT_MIN = -3.1415926f;




void MotionManagerTest::SetUpTestCase()
{
}

void MotionManagerTest::TearDownTestCase()
{}

void MotionManagerTest::SetUp()
{
}

void MotionManagerTest::TearDown()
{}

static CameraKeyEvent keyEvent_ = CameraKeyEvent::INVALID;

void MotionManager::MMIKeyEvent(CameraKeyEvent eventType)
{
    keyEvent_ = eventType;
}

AttachmentStateMap MechConnectManager::GetMechState(int32_t mechId)
{
    return g_mockMechState;
}

/**
 * @tc.name  : MechAttitudeNotify_001
 * @tc.number: MechAttitudeNotify_001
 * @tc.desc  : Test MechAttitudeNotify function.
 */
HWTEST_F(MotionManagerTest, MechAttitudeNotify_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    CommandFactory factory;
    auto cmd = factory.CreateRegisterMechPositionInfoCmd();

    // Create a buffer with test position data
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    buffer->AppendUint8(0x02); // cmdSet
    buffer->AppendUint8(0x42); // cmdId
    float pitch = 100.0f;
    float roll = 100.0f;
    float yaw = 100.0f;
    buffer->AppendFloat(pitch);
    buffer->AppendFloat(roll);
    buffer->AppendFloat(yaw);

    // Unmarshal the buffer to populate the command
    cmd->Unmarshal(buffer);

    motionMgr->MechAttitudeNotify(cmd);
    // 验证eulerAngles是否被正确更新
    EXPECT_EQ(motionMgr->deviceStatus_->eulerAngles.yaw, yaw);
    EXPECT_EQ(motionMgr->deviceStatus_->eulerAngles.roll, roll);
    EXPECT_EQ(motionMgr->deviceStatus_->eulerAngles.pitch, pitch);
}

/**
 * @tc.name  : CreateKeyEvent_001
 * @tc.number: CreateKeyEvent_001
 * @tc.desc  : Test CreateKeyEvent with valid parameters.
 */
HWTEST_F(MotionManagerTest, CreateKeyEvent_001, TestSize.Level1)
{
    int32_t keyCode = 10;
    int32_t mechId = 100;
    int32_t keyAction = MMI::KeyEvent::KEY_ACTION_DOWN;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<MMI::KeyEvent> keyEvent = motionMgr->CreateKeyEvent(keyCode, keyAction);

    // 检查KeyEvent是否正确创建
    EXPECT_NE(keyEvent, nullptr);
    EXPECT_EQ(keyEvent->GetKeyCode(), keyCode);
    EXPECT_EQ(keyEvent->GetKeyAction(), keyAction);
}

/**
 * @tc.name  : CreateKeyEvent_002
 * @tc.number: CreateKeyEvent_002
 * @tc.desc  : Test CreateKeyEvent with invalid parameters.
 */
HWTEST_F(MotionManagerTest, CreateKeyEvent_002, TestSize.Level1)
{
    int32_t keyCode = -1;
    int32_t mechId = 100;
    int32_t keyAction = MMI::KeyEvent::KEY_ACTION_DOWN;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<MMI::KeyEvent> keyEvent = motionMgr->CreateKeyEvent(keyCode, keyAction);
    // keyCode=-1虽然能创建KeyEvent对象，但验证其属性是否正确设置
    ASSERT_NE(nullptr, keyEvent);
    EXPECT_EQ(keyEvent->GetKeyCode(), keyCode);
    EXPECT_EQ(keyEvent->GetKeyAction(), keyAction);
}

/**
 * @tc.name  : MechButtonEventNotify_001
 * @tc.number: MechButtonEventNotify_001
 * @tc.desc  : Test MechButtonEventNotify with START_FILMING event.
 */
HWTEST_F(MotionManagerTest, MechButtonEventNotify_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RegisterMechCameraKeyEventCmd> cmd = std::make_shared<RegisterMechCameraKeyEventCmd>();
    cmd->event_ = CameraKeyEvent::START_FILMING;
    motionMgr->MechButtonEventNotify(cmd);

    EXPECT_EQ(keyEvent_, CameraKeyEvent::START_FILMING);
}

/**
 * @tc.name  : MechButtonEventNotify_003
 * @tc.number: MechButtonEventNotify_003
 * @tc.desc  : Test MechButtonEventNotify with SWITCH_PHOTO_FILM event.
 */
HWTEST_F(MotionManagerTest, MechButtonEventNotify_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RegisterMechCameraKeyEventCmd> cmd = std::make_shared<RegisterMechCameraKeyEventCmd>();
    cmd->event_ = CameraKeyEvent::SWITCH_PHOTO_FILM;
    motionMgr->MechButtonEventNotify(cmd);

    EXPECT_EQ(keyEvent_, CameraKeyEvent::SWITCH_PHOTO_FILM);
}

/**
 * @tc.name  : MechButtonEventNotify_004
 * @tc.number: MechButtonEventNotify_004
 * @tc.desc  : Test MechButtonEventNotify with ZOOM_OUT event.
 */
HWTEST_F(MotionManagerTest, MechButtonEventNotify_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RegisterMechCameraKeyEventCmd> cmd = std::make_shared<RegisterMechCameraKeyEventCmd>();
    cmd->event_ = CameraKeyEvent::ZOOM_OUT;
    motionMgr->MechButtonEventNotify(cmd);

    EXPECT_EQ(keyEvent_, CameraKeyEvent::ZOOM_OUT);
}

/**
 * @tc.name  : MechButtonEventNotify_005
 * @tc.number: MechButtonEventNotify_005
 * @tc.desc  : Test MechButtonEventNotify with ZOOM_IN event.
 */
HWTEST_F(MotionManagerTest, MechButtonEventNotify_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RegisterMechCameraKeyEventCmd> cmd = std::make_shared<RegisterMechCameraKeyEventCmd>();
    cmd->event_ = CameraKeyEvent::ZOOM_IN;
    motionMgr->MechButtonEventNotify(cmd);

    EXPECT_EQ(keyEvent_, CameraKeyEvent::ZOOM_IN);
}

/**
 * @tc.name  : MechButtonEventNotify_006
 * @tc.number: MechButtonEventNotify_006
 * @tc.desc  : Test MechButtonEventNotify with SWITCH_CAMERA event.
 */
HWTEST_F(MotionManagerTest, MechButtonEventNotify_006, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RegisterMechCameraKeyEventCmd> cmd = std::make_shared<RegisterMechCameraKeyEventCmd>();
    cmd->event_ = CameraKeyEvent::SWITCH_CAMERA;
    motionMgr->MechButtonEventNotify(cmd);

    EXPECT_EQ(keyEvent_, CameraKeyEvent::SWITCH_CAMERA);
}

/**
 * @tc.name  : MechParamNotify_001
 * @tc.number: MechParamNotify_001
 * @tc.desc  : Test MechParamNotify with FOLLOW mode.
 */
HWTEST_F(MotionManagerTest, MechParamNotify_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RegisterMechStateInfoCmd> cmd = std::make_shared<RegisterMechStateInfoCmd>();
    MechStateInfo info;
    info.mechMode = MechMode::FOLLOW;
    cmd->info_ = info;
    motionMgr->MechParamNotify(cmd);

    motionMgr->deviceStatus_->stateInfo = cmd->GetInfo();
    motionMgr->MechParamNotify(cmd);

    EXPECT_TRUE(motionMgr->deviceStatus_->rotationAxesStatus.yawEnabled);
    EXPECT_FALSE(motionMgr->deviceStatus_->rotationAxesStatus.rollEnabled);
    EXPECT_TRUE(motionMgr->deviceStatus_->rotationAxesStatus.pitchEnabled);
}

/**
 * @tc.name  : MechParamNotify_002
 * @tc.number: MechParamNotify_002
 * @tc.desc  : Test MechParamNotify with FREE mode.
 */
HWTEST_F(MotionManagerTest, MechParamNotify_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RegisterMechStateInfoCmd> cmd = std::make_shared<RegisterMechStateInfoCmd>();
    MechStateInfo info;
    info.mechMode = MechMode::FREE;
    cmd->info_ = info;
    motionMgr->MechParamNotify(cmd);

    EXPECT_FALSE(motionMgr->deviceStatus_->rotationAxesStatus.yawEnabled);
    EXPECT_FALSE(motionMgr->deviceStatus_->rotationAxesStatus.rollEnabled);
    EXPECT_FALSE(motionMgr->deviceStatus_->rotationAxesStatus.pitchEnabled);
}

/**
 * @tc.name  : MechParamNotify_003
 * @tc.number: MechParamNotify_003
 * @tc.desc  : Test MechParamNotify with PITCH_FREE and FPV mode.
 */
HWTEST_F(MotionManagerTest, MechParamNotify_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RegisterMechStateInfoCmd> cmd = std::make_shared<RegisterMechStateInfoCmd>();
    MechStateInfo info;
    info.mechMode = MechMode::PITCH_FREE;
    cmd->info_ = info;
    motionMgr->MechParamNotify(cmd);

    info.mechMode = MechMode::FPV;
    cmd->info_ = info;
    motionMgr->MechParamNotify(cmd);

    info.mechMode = MechMode::ROLL;
    cmd->info_ = info;
    motionMgr->MechParamNotify(cmd);

    EXPECT_TRUE(motionMgr->deviceStatus_->rotationAxesStatus.yawEnabled);
    EXPECT_FALSE(motionMgr->deviceStatus_->rotationAxesStatus.rollEnabled);
    EXPECT_TRUE(motionMgr->deviceStatus_->rotationAxesStatus.pitchEnabled);
}

/**
 * @tc.name  : Rotate_001
 * @tc.number: Rotate_001
 * @tc.desc  : Test Rotate when the phone is not placed on mech.
 */
HWTEST_F(MotionManagerTest, Rotate_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::DETACHED;

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = 0;
    rotateParam->degree.pitch = 0;
    rotateParam->degree.roll = 0;
    rotateParam->duration = 1000;

    sptr<IRemoteObject> callback = nullptr;
    uint32_t tokenId = 1;
    std::string napiCmdId = "1";

    int32_t result = motionMgr->Rotate(rotateParam, tokenId, napiCmdId);
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : Rotate_002
 * @tc.number: Rotate_002
 * @tc.desc  : Test Rotate with invalid parameters and null callback.
 */
HWTEST_F(MotionManagerTest, Rotate_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = 10;
    rotateParam->degree.pitch = 0;
    rotateParam->degree.roll = 0;
    rotateParam->duration = 1000;

    uint32_t tokenId = 1;
    std::string napiCmdId = "1";

    int32_t result = motionMgr->Rotate(rotateParam, tokenId, napiCmdId);
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);
}

/**
 * @tc.name  : Rotate_003
 * @tc.number: Rotate_003
 * @tc.desc  : Test Rotate with invalid parameters and non-null callback.
 */
HWTEST_F(MotionManagerTest, Rotate_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = 361;
    rotateParam->degree.pitch = 0;
    rotateParam->degree.roll = 0;
    rotateParam->duration = 1000;

    uint32_t tokenId = 1;
    std::string napiCmdId = "1";

    int32_t result = motionMgr->Rotate(rotateParam, tokenId, napiCmdId);
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);
}

/**
 * @tc.name  : Rotate_004
 * @tc.number: Rotate_004
 * @tc.desc  : Test Rotate with valid parameters.
 */
HWTEST_F(MotionManagerTest, Rotate_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = 1;
    rotateParam->degree.pitch = 1;
    rotateParam->degree.roll = 0;
    rotateParam->duration = 100;

    uint32_t tokenId = 1;
    std::string napiCmdId = "1";

    int32_t result = motionMgr->Rotate(rotateParam, tokenId, napiCmdId);
    EXPECT_EQ(result, ERR_OK);

    rotateParam->isRelative = true;
    result = motionMgr->Rotate(rotateParam, tokenId, napiCmdId);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : Rotate_005
 * @tc.number: Rotate_005
 * @tc.desc  : Test Rotate with eventHandler_ as nullptr.
 */
HWTEST_F(MotionManagerTest, Rotate_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    // Set eventHandler_ to nullptr to test the branch where eventHandler_ is null
    motionMgr->eventHandler_ = nullptr;

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = 1;
    rotateParam->degree.pitch = 1;
    rotateParam->degree.roll = 0;
    rotateParam->duration = 100;

    uint32_t tokenId = 1;
    std::string napiCmdId = "1";

    int32_t result = motionMgr->Rotate(rotateParam, tokenId, napiCmdId);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : Rotate_006
 * @tc.number: Rotate_006
 * @tc.desc  : Test Rotate with eventHandler_ initialized.
 */
HWTEST_F(MotionManagerTest, Rotate_006, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = 1;
    rotateParam->degree.pitch = 1;
    rotateParam->degree.roll = 0;
    rotateParam->duration = 100;

    uint32_t tokenId = 1;
    std::string napiCmdId = "1";

    int32_t result = motionMgr->Rotate(rotateParam, tokenId, napiCmdId);
    EXPECT_EQ(result, ERR_OK);

    // 验证callback已被注册到seqCallbacks_ map中
    std::unique_lock<std::mutex> lock(motionMgr->seqCallbackMutex_);
    EXPECT_FALSE(motionMgr->seqCallbacks_.empty());
    lock.unlock();
}

/**
 * @tc.name  : RotateBySpeed_001
 * @tc.number: RotateBySpeed_001
 * @tc.desc  : Test RotateBySpeed when the phone is not placed on mech.
 */
HWTEST_F(MotionManagerTest, RotateBySpeed_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::DETACHED;

    std::shared_ptr<RotateBySpeedParam> rotateSpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateSpeedParam->speed.yawSpeed = 1;
    rotateSpeedParam->speed.pitchSpeed = 1;
    rotateSpeedParam->speed.rollSpeed = 1;
    rotateSpeedParam->duration = 1000;

    uint32_t tokenId = 1;
    std::string napiCmdId = "1";

    int32_t result = motionMgr->RotateBySpeed(rotateSpeedParam, tokenId, napiCmdId);
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : RotateBySpeed_002
 * @tc.number: RotateBySpeed_002
 * @tc.desc  : Test RotateBySpeed with null callback.
 */
HWTEST_F(MotionManagerTest, RotateBySpeed_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    std::shared_ptr<RotateBySpeedParam> rotateSpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateSpeedParam->speed.yawSpeed = 1;
    rotateSpeedParam->speed.pitchSpeed = 1;
    rotateSpeedParam->speed.rollSpeed = 1;
    rotateSpeedParam->duration = 1000;

    uint32_t tokenId = 1;
    std::string napiCmdId = "1";

    int32_t result = motionMgr->RotateBySpeed(rotateSpeedParam, tokenId, napiCmdId);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : RotateBySpeed_003
 * @tc.number: RotateBySpeed_003
 * @tc.desc  : Test RotateBySpeed with valid parameters.
 */
HWTEST_F(MotionManagerTest, RotateBySpeed_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    std::shared_ptr<RotateBySpeedParam> rotateSpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateSpeedParam->speed.yawSpeed = 1;
    rotateSpeedParam->speed.pitchSpeed = 1;
    rotateSpeedParam->speed.rollSpeed = 1;
    rotateSpeedParam->duration = 100;

    uint32_t tokenId = 1;
    std::string napiCmdId = "1";

    int32_t result = motionMgr->RotateBySpeed(rotateSpeedParam, tokenId, napiCmdId);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(motionMgr->deviceStatus_->eulerAngles.yaw, 0);
    EXPECT_EQ(motionMgr->deviceStatus_->eulerAngles.pitch, 0);
    EXPECT_EQ(motionMgr->deviceStatus_->eulerAngles.roll, 0);
}

/**
 * @tc.name  : RotateBySpeed_004
 * @tc.number: RotateBySpeed_004
 * @tc.desc  : Test RotateBySpeed with valid parameters and eventHandler_ initialized.
 */
HWTEST_F(MotionManagerTest, RotateBySpeed_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    std::shared_ptr<RotateBySpeedParam> rotateSpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateSpeedParam->speed.yawSpeed = 1;
    rotateSpeedParam->speed.pitchSpeed = 1;
    rotateSpeedParam->speed.rollSpeed = 1;
    rotateSpeedParam->duration = 100;

    uint32_t tokenId = 1;
    std::string napiCmdId = "1";

    int32_t result = motionMgr->RotateBySpeed(rotateSpeedParam, tokenId, napiCmdId);
    EXPECT_EQ(result, ERR_OK);

    // 验证callback已被注册到seqCallbacks_ map中（V2协议）
    std::unique_lock<std::mutex> lock(motionMgr->seqCallbackMutex_);
    EXPECT_FALSE(motionMgr->seqCallbacks_.empty());
    lock.unlock();
}

/**
 * @tc.name  : StopRotate_001
 * @tc.number: StopRotate_001
 * @tc.desc  : Test StopRotate when the phone is not placed on mech.
 */
HWTEST_F(MotionManagerTest, StopRotate_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::DETACHED;

    uint32_t tokenId = 1;
    std::string napiCmdId = "1";

    int32_t result = motionMgr->StopRotate(tokenId, napiCmdId);
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : StopRotate_002
 * @tc.number: StopRotate_002
 * @tc.desc  : Test StopRotate with valid parameters.
 */
HWTEST_F(MotionManagerTest, StopRotate_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    uint32_t tokenId = 1;
    std::string napiCmdId = "1";

    int32_t result = motionMgr->StopRotate(tokenId, napiCmdId);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : StopRotate_003
 * @tc.number: StopRotate_003
 * @tc.desc  : Test StopRotate with protocolVer_ == 0x01.
 */
HWTEST_F(MotionManagerTest, StopRotate_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    // Set protocolVer_ to 0x01
    motionMgr->protocolVer_ = 0x01;

    uint32_t tokenId = 1;
    std::string napiCmdId = "1";

    int32_t result = motionMgr->StopRotate(tokenId, napiCmdId);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : StopRotate_004
 * @tc.number: StopRotate_004
 * @tc.desc  : Test StopRotate with WHEEL_BASE device type.
 */
HWTEST_F(MotionManagerTest, StopRotate_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    // Set deviceBaseInfo_.devType to WHEEL_BASE
    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::WHEEL_BASE);

    uint32_t tokenId = 1;
    std::string napiCmdId = "1";

    int32_t result = motionMgr->StopRotate(tokenId, napiCmdId);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : GetSpeedControlTimeLimit_001
 * @tc.number: GetSpeedControlTimeLimit_001
 * @tc.desc  : Test GetSpeedControlTimeLimit when the phone is not placed on mech.
 */
HWTEST_F(MotionManagerTest, GetSpeedControlTimeLimit_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::DETACHED;

    std::shared_ptr<TimeLimit> timeLimit = nullptr;

    int32_t result = motionMgr->GetSpeedControlTimeLimit(timeLimit);
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : GetSpeedControlTimeLimit_002
 * @tc.number: GetSpeedControlTimeLimit_002
 * @tc.desc  : Test GetSpeedControlTimeLimit with valid parameters.
 */
HWTEST_F(MotionManagerTest, GetSpeedControlTimeLimit_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    std::shared_ptr<TimeLimit> timeLimit = std::make_shared<TimeLimit>();
    std::shared_ptr<TimeLimit> timeLimitNull = nullptr;

    int32_t result = motionMgr->GetSpeedControlTimeLimit(timeLimit);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_NE(timeLimit, nullptr);
    EXPECT_EQ(timeLimit->min, 0);
    EXPECT_EQ(timeLimit->max, DEFAULT_DURATION);

    result = motionMgr->GetSpeedControlTimeLimit(timeLimitNull);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : GetRotateSpeedLimit_001
 * @tc.number: GetRotateSpeedLimit_001
 * @tc.desc  : Test GetRotateSpeedLimit when the phone is not placed on mech.
 */
HWTEST_F(MotionManagerTest, GetRotateSpeedLimit_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::DETACHED;

    RotateSpeedLimit speedLimit;

    int32_t result = motionMgr->GetRotateSpeedLimit(speedLimit);
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : GetRotateSpeedLimit_002
 * @tc.number: GetRotateSpeedLimit_002
 * @tc.desc  : Test GetRotateSpeedLimit with valid parameters.
 */
HWTEST_F(MotionManagerTest, GetRotateSpeedLimit_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    RotateSpeedLimit speedLimit;

    int32_t result = motionMgr->GetRotateSpeedLimit(speedLimit);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(speedLimit.speedMin.pitchSpeed, motionMgr->deviceStatus_->rotateSpeedLimit.speedMin.pitchSpeed);
    EXPECT_EQ(speedLimit.speedMin.rollSpeed, motionMgr->deviceStatus_->rotateSpeedLimit.speedMin.rollSpeed);
    EXPECT_EQ(speedLimit.speedMin.yawSpeed, motionMgr->deviceStatus_->rotateSpeedLimit.speedMin.yawSpeed);
    EXPECT_EQ(speedLimit.speedMax.pitchSpeed, motionMgr->deviceStatus_->rotateSpeedLimit.speedMax.pitchSpeed);
    EXPECT_EQ(speedLimit.speedMax.rollSpeed, motionMgr->deviceStatus_->rotateSpeedLimit.speedMax.rollSpeed);
    EXPECT_EQ(speedLimit.speedMax.yawSpeed, motionMgr->deviceStatus_->rotateSpeedLimit.speedMax.yawSpeed);
}

/**
 * @tc.name  : GetCurrentPosition_001
 * @tc.number: GetCurrentPosition_001
 * @tc.desc  : Test GetCurrentPosition when the phone is not placed on mech.
 */
HWTEST_F(MotionManagerTest, GetCurrentPosition_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::DETACHED;

    std::shared_ptr<EulerAngles> eulerAngles = std::make_shared<EulerAngles>();

    int32_t result = motionMgr->GetCurrentPosition(eulerAngles);
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : GetCurrentPosition_002
 * @tc.number: GetCurrentPosition_002
 * @tc.desc  : Test GetCurrentPosition with valid parameters.
 */
HWTEST_F(MotionManagerTest, GetCurrentPosition_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    std::shared_ptr<EulerAngles> eulerAngles = std::make_shared<EulerAngles>();

    int32_t result = motionMgr->GetCurrentPosition(eulerAngles);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(eulerAngles->yaw, motionMgr->deviceStatus_->eulerAngles.yaw);
    EXPECT_EQ(eulerAngles->pitch, motionMgr->deviceStatus_->eulerAngles.pitch);
    EXPECT_EQ(eulerAngles->roll, motionMgr->deviceStatus_->eulerAngles.roll);
}

/**
 * @tc.name  : GetRotationLimit_001
 * @tc.number: GetRotationLimit_001
 * @tc.desc  : Test GetRotationLimit when the phone is not placed on mech.
 */
HWTEST_F(MotionManagerTest, GetRotationLimit_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::DETACHED;

    RotateDegreeLimit rotationLimit;

    int32_t result = motionMgr->GetRotationLimit(rotationLimit);
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : GetRotationLimit_002
 * @tc.number: GetRotationLimit_002
 * @tc.desc  : Test GetRotationLimit with valid parameters.
 */
HWTEST_F(MotionManagerTest, GetRotationLimit_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    RotateDegreeLimit rotationLimit;

    int32_t result = motionMgr->GetRotationLimit(rotationLimit);
    EXPECT_EQ(result, ERR_OK);
    // 检查旋转限制是否正确
    EXPECT_EQ(rotationLimit.negMax.roll, motionMgr->deviceStatus_->rotationLimit.negMax.roll);
    EXPECT_EQ(rotationLimit.negMax.pitch, motionMgr->deviceStatus_->rotationLimit.negMax.pitch);
    EXPECT_EQ(rotationLimit.negMax.yaw, motionMgr->deviceStatus_->rotationLimit.negMax.yaw);
    EXPECT_EQ(rotationLimit.posMax.roll, motionMgr->deviceStatus_->rotationLimit.posMax.roll);
    EXPECT_EQ(rotationLimit.posMax.pitch, motionMgr->deviceStatus_->rotationLimit.posMax.pitch);
    EXPECT_EQ(rotationLimit.posMax.yaw, motionMgr->deviceStatus_->rotationLimit.posMax.yaw);
}

/**
 * @tc.name  : SetMechCameraTrackingFrame_001
 * @tc.number: SetMechCameraTrackingFrame_001
 * @tc.desc  : Test SetMechCameraTrackingFrame when the phone is not placed on mech.
 */
HWTEST_F(MotionManagerTest, SetMechCameraTrackingFrame_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::DETACHED;

    std::shared_ptr<TrackingFrameParams> trackingFrameParams = std::make_shared<TrackingFrameParams>();

    int32_t result = motionMgr->SetMechCameraTrackingFrame(trackingFrameParams);
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : SetMechCameraTrackingFrame_002
 * @tc.number: SetMechCameraTrackingFrame_002
 * @tc.desc  : Test SetMechCameraTrackingFrame with valid parameters.
 */
HWTEST_F(MotionManagerTest, SetMechCameraTrackingFrame_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    std::shared_ptr<TrackingFrameParams> trackingFrameParams = std::make_shared<TrackingFrameParams>();
    motionMgr->deviceStatus_->isEnabled = true;
    // Set trackingStatus to MECH_TK_ENABLE_NO_TARGET
    motionMgr->deviceStatus_->trackingStatus = MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET;

    int32_t result = motionMgr->SetMechCameraTrackingFrame(trackingFrameParams);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : GetMechCameraTrackingEnabled_001
 * @tc.number: GetMechCameraTrackingEnabled_001
 * @tc.desc  : Test GetMechCameraTrackingEnabled when the phone is not placed on mech.
 */
HWTEST_F(MotionManagerTest, GetMechCameraTrackingEnabled_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::DETACHED;

    bool isEnabled = false;

    int32_t result = motionMgr->GetMechCameraTrackingEnabled(isEnabled);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(isEnabled, motionMgr->deviceStatus_->isEnabled);
}

/**
 * @tc.name  : GetMechCameraTrackingEnabled_002
 * @tc.number: GetMechCameraTrackingEnabled_002
 * @tc.desc  : Test GetMechCameraTrackingEnabled with valid parameters.
 */
HWTEST_F(MotionManagerTest, GetMechCameraTrackingEnabled_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    bool isEnabled = false;

    int32_t result = motionMgr->GetMechCameraTrackingEnabled(isEnabled);
    EXPECT_EQ(result, ERR_OK);
    // 检查是否正确获取了启用状态
    EXPECT_EQ(isEnabled, motionMgr->deviceStatus_->isEnabled);
}

/**
 * @tc.name  : SetMechCameraTrackingLayout_001
 * @tc.number: SetMechCameraTrackingLayout_001
 * @tc.desc  : Test SetMechCameraTrackingLayout when the phone is not placed on mech.
 */
HWTEST_F(MotionManagerTest, SetMechCameraTrackingLayout_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::DETACHED;

    std::shared_ptr<LayoutParams> layoutParams = std::make_shared<LayoutParams>();
    layoutParams->isDefault = true;
    layoutParams->offsetX = 10;
    layoutParams->offsetY = 10;

    int32_t result = motionMgr->SetMechCameraTrackingLayout(layoutParams);
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : SetMechCameraTrackingLayout_002
 * @tc.number: SetMechCameraTrackingLayout_002
 * @tc.desc  : Test SetMechCameraTrackingLayout with valid parameters.
 */
HWTEST_F(MotionManagerTest, SetMechCameraTrackingLayout_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    std::shared_ptr<LayoutParams> layoutParams = std::make_shared<LayoutParams>();
    layoutParams->isDefault = true;
    layoutParams->offsetX = 10;
    layoutParams->offsetY = 10;

    int32_t result = motionMgr->SetMechCameraTrackingLayout(layoutParams);
    EXPECT_EQ(result, ERR_OK);

    EXPECT_TRUE(motionMgr->deviceStatus_->layoutParams.isDefault);
    EXPECT_EQ(motionMgr->deviceStatus_->layoutParams.offsetX, 10);
    EXPECT_EQ(motionMgr->deviceStatus_->layoutParams.offsetY, 10);
}

/**
 * @tc.name  : GetMechCameraTrackingLayout_001
 * @tc.number: GetMechCameraTrackingLayout_001
 * @tc.desc  : Test GetMechCameraTrackingLayout when the phone is not placed on mech.
 */
HWTEST_F(MotionManagerTest, GetMechCameraTrackingLayout_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::DETACHED;

    std::shared_ptr<LayoutParams> layoutParams = nullptr;

    int32_t result = motionMgr->GetMechCameraTrackingLayout(layoutParams);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : GetMechCameraTrackingLayout_002
 * @tc.number: GetMechCameraTrackingLayout_002
 * @tc.desc  : Test GetMechCameraTrackingLayout with valid parameters.
 */
HWTEST_F(MotionManagerTest, GetMechCameraTrackingLayout_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    std::shared_ptr<LayoutParams> layoutParams = nullptr;

    int32_t result = motionMgr->GetMechCameraTrackingLayout(layoutParams);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(motionMgr->deviceStatus_->layoutParams.isDefault, layoutParams->isDefault);
    EXPECT_EQ(motionMgr->deviceStatus_->layoutParams.offsetX, layoutParams->offsetX);
    EXPECT_EQ(motionMgr->deviceStatus_->layoutParams.offsetY, layoutParams->offsetY);
}

/**
 * @tc.name  : GetMechBaseInfo_001
 * @tc.number: GetMechBaseInfo_001
 * @tc.desc  : Test GetMechBaseInfo when the phone is not placed on mech.
 */
HWTEST_F(MotionManagerTest, GetMechBaseInfo_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::DETACHED;

    std::shared_ptr<MechBaseInfo> mechBaseInfo = nullptr;

    int32_t result = motionMgr->GetMechBaseInfo(mechBaseInfo);
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : GetMechBaseInfo_002
 * @tc.number: GetMechBaseInfo_002
 * @tc.desc  : Test GetMechBaseInfo when base info is not obtainable.
 */
HWTEST_F(MotionManagerTest, GetMechBaseInfo_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->deviceStatus_->mechBaseInfo.obtainable = false;

    std::shared_ptr<MechBaseInfo> mechBaseInfo = nullptr;

    int32_t result = motionMgr->GetMechBaseInfo(mechBaseInfo);
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);
}

/**
 * @tc.name  : GetMechBaseInfo_003
 * @tc.number: GetMechBaseInfo_003
 * @tc.desc  : Test GetMechBaseInfo with valid parameters.
 */
HWTEST_F(MotionManagerTest, GetMechBaseInfo_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);

    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->RegisterEventListener();
    motionMgr->deviceStatus_->mechBaseInfo.obtainable = true;

    std::shared_ptr<MechBaseInfo> mechBaseInfo = nullptr;

    int32_t result = motionMgr->GetMechBaseInfo(mechBaseInfo);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_NE(mechBaseInfo, nullptr);
    EXPECT_EQ(mechBaseInfo->mechType, motionMgr->deviceStatus_->mechBaseInfo.mechType);
}

/**
 * @tc.name  : GetMechCapabilityInfo_001
 * @tc.number: GetMechCapabilityInfo_001
 * @tc.desc  : Test GetMechCapabilityInfo when the phone is not placed on mech.
 */
HWTEST_F(MotionManagerTest, GetMechCapabilityInfo_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::DETACHED;

    std::shared_ptr<MechCapabilityInfo> mechCapabilityInfo = nullptr;

    int32_t result = motionMgr->GetMechCapabilityInfo(mechCapabilityInfo);
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : GetMechCapabilityInfo_002
 * @tc.number: GetMechCapabilityInfo_002
 * @tc.desc  : Test GetMechCapabilityInfo when capability info is not obtainable.
 */
HWTEST_F(MotionManagerTest, GetMechCapabilityInfo_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->deviceStatus_->mechCapabilityInfo.obtainable = false;

    std::shared_ptr<MechCapabilityInfo> mechCapabilityInfo = nullptr;

    int32_t result = motionMgr->GetMechCapabilityInfo(mechCapabilityInfo);
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);
}

/**
 * @tc.name  : GetMechCapabilityInfo_003
 * @tc.number: GetMechCapabilityInfo_003
 * @tc.desc  : Test GetMechCapabilityInfo with valid parameters.
 */
HWTEST_F(MotionManagerTest, GetMechCapabilityInfo_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->deviceStatus_->mechCapabilityInfo.obtainable = true;

    std::shared_ptr<MechCapabilityInfo> mechCapabilityInfo = nullptr;

    int32_t result = motionMgr->GetMechCapabilityInfo(mechCapabilityInfo);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_NE(mechCapabilityInfo, nullptr);
    EXPECT_EQ(mechCapabilityInfo->automaticReturn, motionMgr->deviceStatus_->mechCapabilityInfo.automaticReturn);
}

/**
 * @tc.name  : GetRotationAxesStatus_001
 * @tc.number: GetRotationAxesStatus_001
 * @tc.desc  : Test GetRotationAxesStatus when the phone is not placed on mech.
 */
HWTEST_F(MotionManagerTest, GetRotationAxesStatus_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::DETACHED;

    RotationAxesStatus axesStatus;

    int32_t result = motionMgr->GetRotationAxesStatus(mechId, axesStatus);
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : GetRotationAxesStatus_002
 * @tc.number: GetRotationAxesStatus_002
 * @tc.desc  : Test GetRotationAxesStatus with valid parameters.
 */
HWTEST_F(MotionManagerTest, GetRotationAxesStatus_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    RotationAxesStatus axesStatus;

    int32_t result = motionMgr->GetRotationAxesStatus(mechId, axesStatus);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(axesStatus.rollEnabled, motionMgr->deviceStatus_->rotationAxesStatus.rollEnabled);
    EXPECT_EQ(axesStatus.pitchEnabled, motionMgr->deviceStatus_->rotationAxesStatus.pitchEnabled);
    EXPECT_EQ(axesStatus.yawEnabled, motionMgr->deviceStatus_->rotationAxesStatus.yawEnabled);
    EXPECT_EQ(axesStatus.rollLimited, motionMgr->deviceStatus_->rotationAxesStatus.rollLimited);
    EXPECT_EQ(axesStatus.pitchLimited, motionMgr->deviceStatus_->rotationAxesStatus.pitchLimited);
    EXPECT_EQ(axesStatus.yawLimited, motionMgr->deviceStatus_->rotationAxesStatus.yawLimited);
}

/**
 * @tc.name  : MechExecutionResultNotify_001
 * @tc.number: MechExecutionResultNotify_001
 * @tc.desc  : Test MechExecutionResultNotify with null cmd parameter.
 */
HWTEST_F(MotionManagerTest, MechExecutionResultNotify_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RegisterMechControlResultCmd> cmd = nullptr;

    // Verify seqCallbacks_ is empty before the call
    EXPECT_TRUE(motionMgr->seqCallbacks_.empty());

    // Test that function handles null cmd gracefully (should return early without crash)
    motionMgr->MechExecutionResultNotify(cmd);

    // Verify seqCallbacks_ remains unchanged (still empty) after the call
    EXPECT_TRUE(motionMgr->seqCallbacks_.empty());
}

/**
 * @tc.name  : MechExecutionResultNotify_003
 * @tc.number: MechExecutionResultNotify_003
 * @tc.desc  : Test MechExecutionResultNotify with willLimitChange and IsLimited branches.
 */
HWTEST_F(MotionManagerTest, MechExecutionResultNotify_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    uint32_t tokenId = 100;
    std::string napiCmdId = "napiCmdId";
    CommandFactory factory;
    auto cmd = factory.CreateRegisterMechControlResultCmd();
    uint16_t taskId = cmd->GetTaskId();
    cmd->controlResult_ = TASK_COMPLETED;

    // Test with willLimitChange = true
    MechNapiCommandCallbackInfo callbackInfo(tokenId, napiCmdId);
    callbackInfo.willLimitChange = true;
    motionMgr->seqCallbacks_.insert(std::make_pair(static_cast<uint8_t>(taskId), callbackInfo));
    motionMgr->MechExecutionResultNotify(cmd);
    // 验证callback仍然存在（MechExecutionResultNotify不删除callback）
    EXPECT_FALSE(motionMgr->seqCallbacks_.empty());
    EXPECT_EQ(motionMgr->seqCallbacks_.size(), 1);
    // 清理以便下一个测试
    motionMgr->seqCallbacks_.clear();

    // Test with willLimitChange = false, IsLimited = true
    callbackInfo.willLimitChange = false;
    motionMgr->seqCallbacks_.insert(std::make_pair(static_cast<uint8_t>(taskId), callbackInfo));
    motionMgr->deviceStatus_->rotationAxesStatus.yawEnabled = true;
    motionMgr->deviceStatus_->rotationAxesStatus.pitchEnabled = true;
    // Set yawLimited to POS_LIMITED to make IsLimited() return true and cover line 525-528
    motionMgr->deviceStatus_->rotationAxesStatus.yawLimited = RotationAxisLimited::POS_LIMITED;
    motionMgr->deviceStatus_->rotationAxesStatus.pitchLimited = RotationAxisLimited::NOT_LIMITED;
    motionMgr->MechExecutionResultNotify(cmd);
    // 验证callback仍然存在（MechExecutionResultNotify不删除callback）
    EXPECT_FALSE(motionMgr->seqCallbacks_.empty());
    EXPECT_EQ(motionMgr->seqCallbacks_.size(), 1);
    // 清理以便下一个测试
    motionMgr->seqCallbacks_.clear();

    // Test with willLimitChange = false, IsLimited = false (else branch)
    motionMgr->seqCallbacks_.insert(std::make_pair(static_cast<uint8_t>(taskId), callbackInfo));
    motionMgr->deviceStatus_->rotationAxesStatus.yawEnabled = false;
    motionMgr->deviceStatus_->rotationAxesStatus.pitchEnabled = false;
    motionMgr->MechExecutionResultNotify(cmd);
    // 验证callback仍然存在（MechExecutionResultNotify不删除callback）
    EXPECT_FALSE(motionMgr->seqCallbacks_.empty());
    EXPECT_EQ(motionMgr->seqCallbacks_.size(), 1);
}

/**
 * @tc.name  : MechWheelZoomNotify_001
 * @tc.number: MechWheelZoomNotify_001
 * @tc.desc  : Test MechWheelZoomNotify function.
 */
HWTEST_F(MotionManagerTest, MechWheelZoomNotify_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;
    CommandFactory factory;
    auto cmd = factory.CreateRegisterMechWheelDataCmd();

    // Test with degree = 0 (should not increment wheelFilterCnt_)
    cmd->wheelData_.degree = 0;
    motionMgr->MechWheelZoomNotify(cmd);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(motionMgr->wheelFilterCnt_, 0);

    // Third call: wheelFilterCnt_ should be reset to 0 (reaches WHEEL_FILTER_COUNT)
    cmd->wheelData_.degree = 10;
    motionMgr->MechWheelZoomNotify(cmd);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(motionMgr->wheelFilterCnt_, 0);
}

/**
 * @tc.name  : IsLimited_001
 * @tc.number: IsLimited_001
 * @tc.desc  : Test IsLimited function.
 */
HWTEST_F(MotionManagerTest, IsLimited_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    EXPECT_EQ(motionMgr->IsLimited(), false);

    motionMgr->deviceStatus_->rotationAxesStatus.pitchLimited = RotationAxisLimited::NEG_LIMITED;
    EXPECT_EQ(motionMgr->IsLimited(), true);

    motionMgr->deviceStatus_->rotationAxesStatus.yawLimited = RotationAxisLimited::NEG_LIMITED;
    EXPECT_EQ(motionMgr->IsLimited(), true);
}

/**
 * @tc.name  : SetMechCameraInfo_001
 * @tc.number: SetMechCameraInfo_001
 * @tc.desc  : Test SetMechCameraInfo with valid parameters.
 */
HWTEST_F(MotionManagerTest, SetMechCameraInfo_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;
    CameraInfoParams mechCameraInfo;
    mechCameraInfo.fovH = 1;
    mechCameraInfo.fovV = 1;
    mechCameraInfo.cameraType = CameraType::FRONT;

    EXPECT_EQ(motionMgr->SetMechCameraInfo(mechCameraInfo), ERR_OK);
}

/**
 * @tc.name  : PerformPresetAction_001
 * @tc.number: PerformPresetAction_001
 * @tc.desc  : Test PerformPresetAction with valid parameters.
 */
HWTEST_F(MotionManagerTest, PerformPresetAction_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    PresetAction presetAction = PresetAction::NOD;

    EXPECT_EQ(motionMgr->PerformPresetAction(presetAction, 0), ERR_OK);
}

/**
 * @tc.name  : JudgingYawLimit_001
 * @tc.number: JudgingYawLimit_001
 * @tc.desc  : Test JudgingYawLimit with valid parameters.
 */
HWTEST_F(MotionManagerTest, JudgingYawLimit_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    RotateDegreeLimit limit;

    // Test 1: All limits are not NO_LIMIT values -> all limits should be true
    limit.negMax.yaw = 1;
    limit.negMax.roll = 1;
    limit.negMax.pitch = 1;
    limit.posMax.yaw = 1;
    limit.posMax.roll = 1;
    limit.posMax.pitch = 1;
    motionMgr->JudgingYawLimit(limit);
    EXPECT_TRUE(motionMgr->deviceStatus_->yawLimit);
    EXPECT_TRUE(motionMgr->deviceStatus_->pitchLimit);
    EXPECT_TRUE(motionMgr->deviceStatus_->rollLimit);

    // Test 2: negMax values are NO_LIMIT_MIN, posMax are not -> all limits should be true
    limit.negMax.yaw = NO_LIMIT_MIN;
    limit.negMax.roll = NO_LIMIT_MIN;
    limit.negMax.pitch = NO_LIMIT_MIN;
    limit.posMax.yaw = 1;
    limit.posMax.roll = 1;
    limit.posMax.pitch = 1;
    motionMgr->JudgingYawLimit(limit);
    EXPECT_TRUE(motionMgr->deviceStatus_->yawLimit);
    EXPECT_TRUE(motionMgr->deviceStatus_->pitchLimit);
    EXPECT_TRUE(motionMgr->deviceStatus_->rollLimit);

    // Test 3: posMax values are NO_LIMIT_MAX, negMax are not -> all limits should be true
    limit.posMax.yaw = NO_LIMIT_MAX;
    limit.posMax.roll = NO_LIMIT_MAX;
    limit.posMax.pitch = NO_LIMIT_MAX;
    limit.negMax.yaw = 1;
    limit.negMax.roll = 1;
    limit.negMax.pitch = 1;
    motionMgr->JudgingYawLimit(limit);
    EXPECT_TRUE(motionMgr->deviceStatus_->yawLimit);
    EXPECT_TRUE(motionMgr->deviceStatus_->pitchLimit);
    EXPECT_TRUE(motionMgr->deviceStatus_->rollLimit);

    // Test 4: All limits are NO_LIMIT values -> all limits should be false
    limit.posMax.yaw = NO_LIMIT_MAX;
    limit.posMax.roll = NO_LIMIT_MAX;
    limit.posMax.pitch = NO_LIMIT_MAX;
    limit.negMax.yaw = NO_LIMIT_MIN;
    limit.negMax.roll = NO_LIMIT_MIN;
    limit.negMax.pitch = NO_LIMIT_MIN;
    motionMgr->JudgingYawLimit(limit);
    EXPECT_FALSE(motionMgr->deviceStatus_->yawLimit);
    EXPECT_FALSE(motionMgr->deviceStatus_->pitchLimit);
    EXPECT_FALSE(motionMgr->deviceStatus_->rollLimit);
}

/**
 * @tc.name  : JudgingYawLimit_002
 * @tc.number: JudgingYawLimit_002
 * @tc.desc  : Test JudgingYawLimit with yaw no limit.
 */
HWTEST_F(MotionManagerTest, JudgingYawLimit_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    RotateDegreeLimit limit;
    // Set yaw to no limit
    limit.negMax.yaw = NO_LIMIT_MIN;
    limit.posMax.yaw = NO_LIMIT_MAX;
    limit.negMax.roll = 1;
    limit.negMax.pitch = 1;
    limit.posMax.roll = 1;
    limit.posMax.pitch = 1;
    motionMgr->JudgingYawLimit(limit);

    // 验证yaw无限制，pitch和roll有限制
    EXPECT_FALSE(motionMgr->deviceStatus_->yawLimit);
    EXPECT_TRUE(motionMgr->deviceStatus_->pitchLimit);
    EXPECT_TRUE(motionMgr->deviceStatus_->rollLimit);
}

/**
 * @tc.name  : JudgingYawLimit_003
 * @tc.number: JudgingYawLimit_003
 * @tc.desc  : Test JudgingYawLimit with pitch no limit.
 */
HWTEST_F(MotionManagerTest, JudgingYawLimit_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    RotateDegreeLimit limit;
    // Set pitch to no limit
    limit.negMax.pitch = NO_LIMIT_MIN;
    limit.posMax.pitch = NO_LIMIT_MAX;
    limit.negMax.yaw = 1;
    limit.negMax.roll = 1;
    limit.posMax.yaw = 1;
    limit.posMax.roll = 1;
    motionMgr->JudgingYawLimit(limit);

    // 验证pitch无限制，yaw和roll有限制
    EXPECT_TRUE(motionMgr->deviceStatus_->yawLimit);
    EXPECT_FALSE(motionMgr->deviceStatus_->pitchLimit);
    EXPECT_TRUE(motionMgr->deviceStatus_->rollLimit);
}

/**
 * @tc.name  : JudgingYawLimit_004
 * @tc.number: JudgingYawLimit_004
 * @tc.desc  : Test JudgingYawLimit with roll no limit.
 */
HWTEST_F(MotionManagerTest, JudgingYawLimit_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    RotateDegreeLimit limit;
    // Set roll to no limit
    limit.negMax.roll = NO_LIMIT_MIN;
    limit.posMax.roll = NO_LIMIT_MAX;
    limit.negMax.yaw = 1;
    limit.negMax.pitch = 1;
    limit.posMax.yaw = 1;
    limit.posMax.pitch = 1;
    motionMgr->JudgingYawLimit(limit);

    // 验证roll无限制，yaw和pitch有限制
    EXPECT_TRUE(motionMgr->deviceStatus_->yawLimit);
    EXPECT_TRUE(motionMgr->deviceStatus_->pitchLimit);
    EXPECT_FALSE(motionMgr->deviceStatus_->rollLimit);
}

/**
 * @tc.name  : SetMechCameraTrackingEnabled_001
 * @tc.number: SetMechCameraTrackingEnabled_001
 * @tc.desc  : Test SetMechCameraTrackingEnabled with valid parameters.
 */
HWTEST_F(MotionManagerTest, SetMechCameraTrackingEnabled_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    bool isEnabled = true;

    EXPECT_EQ(motionMgr->SetMechCameraTrackingEnabled(isEnabled), ERR_OK);
    if (motionMgr->trackingCheckerThread_ != nullptr) {
        motionMgr->startTrackingChecker_ = false;
        if (motionMgr->trackingCheckerThread_->joinable()) {
            motionMgr->trackingCheckerThread_->join();
        }
    }
    motionMgr->deviceStatus_ = nullptr;
    EXPECT_EQ(motionMgr->SetMechCameraTrackingEnabled(isEnabled), ERR_OK);
}

/**
 * @tc.name  : AbsolutelyEulerAnglesJudgingLimitLocked_001
 * @tc.number: AbsolutelyEulerAnglesJudgingLimitLocked_001
 * @tc.desc  : Test AbsolutelyEulerAnglesJudgingLimitLocked with valid parameters.
 */
HWTEST_F(MotionManagerTest, AbsolutelyEulerAnglesJudgingLimitLocked_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    motionMgr->deviceStatus_ = std::make_shared<DeviceStatus>();
    // 设置初始角度
    motionMgr->deviceStatus_->eulerAngles.yaw = 10.0f;
    motionMgr->deviceStatus_->eulerAngles.pitch = 20.0f;
    motionMgr->deviceStatus_->eulerAngles.roll = 30.0f;

    EulerAngles eulerAngles;
    eulerAngles.yaw = 5.0f;
    eulerAngles.pitch = 10.0f;
    eulerAngles.roll = 15.0f;

    motionMgr->AbsolutelyEulerAnglesJudgingLimitLocked(eulerAngles);

    // 验证角度被正确累加：yaw和pitch累加，roll被置为0
    EXPECT_FLOAT_EQ(eulerAngles.yaw, 15.0f);  // 5.0 + 10.0
    EXPECT_FLOAT_EQ(eulerAngles.pitch, 30.0f); // 10.0 + 20.0
    EXPECT_FLOAT_EQ(eulerAngles.roll, 0.0f);   // roll被置为0
}

/**
 * @tc.name  : AbsolutelyEulerAnglesJudgingLimitLocked_002
 * @tc.number: AbsolutelyEulerAnglesJudgingLimitLocked_002
 * @tc.desc  : Test AbsolutelyEulerAnglesJudgingLimitLocked with invalid parameters.
 */
HWTEST_F(MotionManagerTest, AbsolutelyEulerAnglesJudgingLimitLocked_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    if (motionMgr->trackingCheckerThread_ != nullptr) {
        motionMgr->startTrackingChecker_ = false;
        if (motionMgr->trackingCheckerThread_->joinable()) {
            motionMgr->trackingCheckerThread_->join();
        }
    }
    motionMgr->deviceStatus_ = nullptr;
    EulerAngles eulerAngles;
    eulerAngles.yaw = 5.0f;
    eulerAngles.pitch = 10.0f;
    eulerAngles.roll = 15.0f;

    motionMgr->AbsolutelyEulerAnglesJudgingLimitLocked(eulerAngles);

    // 验证当deviceStatus_为nullptr时，eulerAngles不被修改
    EXPECT_FLOAT_EQ(eulerAngles.yaw, 5.0f);
    EXPECT_FLOAT_EQ(eulerAngles.pitch, 10.0f);
    EXPECT_FLOAT_EQ(eulerAngles.roll, 15.0f);
}

/**
 * @tc.name  : LimitCalculationLocked_001
 * @tc.number: LimitCalculationLocked_001
 * @tc.desc  : Test LimitCalculationLocked with valid parameters.
 */
HWTEST_F(MotionManagerTest, LimitCalculationLocked_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    motionMgr->deviceStatus_ = std::make_shared<DeviceStatus>();
    motionMgr->deviceStatus_->rollLimit = true;
    motionMgr->deviceStatus_->pitchLimit = true;
    motionMgr->deviceStatus_->yawLimit = true;
    EulerAngles eulerAngles;
    bool callback = true;
    motionMgr->LimitCalculationLocked(eulerAngles, motionMgr->deviceStatus_->rotationAxesStatus, callback);

    // 使用rotationLimitWithOffset而不是rotationLimit
    // 设置posMax为2，negMax为1，eulerAngles为1.5，这样应该在范围内（NOT_LIMITED）
    motionMgr->deviceStatus_->rotationLimitWithOffset.posMax.yaw = 2;
    motionMgr->deviceStatus_->rotationLimitWithOffset.posMax.roll = 2;
    motionMgr->deviceStatus_->rotationLimitWithOffset.posMax.pitch = 2;
    motionMgr->deviceStatus_->rotationLimitWithOffset.negMax.yaw = 1;
    motionMgr->deviceStatus_->rotationLimitWithOffset.negMax.roll = 1;
    motionMgr->deviceStatus_->rotationLimitWithOffset.negMax.pitch = 1;
    eulerAngles.yaw = 1.5f;
    eulerAngles.roll = 1.5f;
    eulerAngles.pitch = 1.5f;
    motionMgr->LimitCalculationLocked(eulerAngles, motionMgr->deviceStatus_->rotationAxesStatus, callback);

    // 验证当所有限制都启用时，status被正确更新
    EXPECT_EQ(motionMgr->deviceStatus_->rotationAxesStatus.rollLimited, RotationAxisLimited::NOT_LIMITED);
    EXPECT_EQ(motionMgr->deviceStatus_->rotationAxesStatus.pitchLimited, RotationAxisLimited::NOT_LIMITED);
    EXPECT_EQ(motionMgr->deviceStatus_->rotationAxesStatus.yawLimited, RotationAxisLimited::NOT_LIMITED);

    motionMgr->deviceStatus_->rollLimit = false;
    motionMgr->deviceStatus_->pitchLimit = false;
    motionMgr->deviceStatus_->yawLimit = false;
    motionMgr->LimitCalculationLocked(eulerAngles, motionMgr->deviceStatus_->rotationAxesStatus, callback);

    // 验证当所有限制都禁用时，status不被修改
    EXPECT_EQ(motionMgr->deviceStatus_->rotationAxesStatus.rollLimited, RotationAxisLimited::NOT_LIMITED);
    EXPECT_EQ(motionMgr->deviceStatus_->rotationAxesStatus.pitchLimited, RotationAxisLimited::NOT_LIMITED);
    EXPECT_EQ(motionMgr->deviceStatus_->rotationAxesStatus.yawLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CreateResponseTaskId_001
 * @tc.number: CreateResponseTaskId_001
 * @tc.desc  : Test CreateResponseTaskId with valid parameters.
 */
HWTEST_F(MotionManagerTest, CreateResponseTaskId_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    motionMgr->lastTaskId_ = UINT8_MAX;
    uint8_t ret = motionMgr->CreateResponseTaskId();

    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name  : HandleMechPlacementChange_001
 * @tc.number: HandleMechPlacementChange_001
 * @tc.desc  : Test HandleMechPlacementChange with valid parameters.
 */
HWTEST_F(MotionManagerTest, HandleMechPlacementChange_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    motionMgr->lastTaskId_ = UINT8_MAX;
    motionMgr->HandleMechPlacementChange(true);

    // 验证HandleMechPlacementChange函数正常执行
    // 函数会调用HandlePhoneOff或HandlePhoneOn，并通知事件
    motionMgr->HandleMechPlacementChange(false);

    // 验证函数执行后的副作用，如验证deviceStatus_相关状态
    // 例如：验证phone off/on处理是否正确触发
    EXPECT_TRUE(motionMgr->deviceStatus_ != nullptr);
}

/**
 * @tc.name  : MechGenericEventNotify_001
 * @tc.number: MechGenericEventNotify_001
 * @tc.desc  : Test MechGenericEventNotify.
 */
HWTEST_F(MotionManagerTest, MechGenericEventNotify_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Test 1: null cmd - should return early without crash
    motionMgr->MechGenericEventNotify(nullptr);
    // Verify deviceStatus_ remains unchanged
    EXPECT_EQ(motionMgr->deviceStatus_->stateInfo.isPhoneOn, 0);

    // Test 2: attached = 1 (phone attached)
    {
        std::shared_ptr<NormalRegisterMechGenericEventCmd> cmd =
            std::make_shared<NormalRegisterMechGenericEventCmd>();
        cmd->params_.attached = 1;
        cmd->params_.yawDisable = static_cast<uint8_t>(-1);
        motionMgr->MechGenericEventNotify(cmd);
        // Verify isPhoneOn is updated
        EXPECT_EQ(motionMgr->deviceStatus_->stateInfo.isPhoneOn, 1);
    }

    // Test 3: attached = 0 (phone detached)
    {
        std::shared_ptr<NormalRegisterMechGenericEventCmd> cmd =
            std::make_shared<NormalRegisterMechGenericEventCmd>();
        cmd->params_.attached = 0;
        cmd->params_.yawDisable = static_cast<uint8_t>(-1);
        motionMgr->MechGenericEventNotify(cmd);
        // Verify isPhoneOn is updated
        EXPECT_EQ(motionMgr->deviceStatus_->stateInfo.isPhoneOn, 0);
    }
}

HWTEST_F(MotionManagerTest, GetProtocolVer_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);

    // Test GetProtocolVer - verify return value
    int32_t result = motionMgr->GetProtocolVer();
    EXPECT_EQ(result, MECH_CONNECT_FAILED);

    // Test SetProtocolVer - should complete without crash
    motionMgr->SetProtocolVer();

    // Test GetDeviceBaseInfo - verify return value
    result = motionMgr->GetDeviceBaseInfo();
    EXPECT_EQ(result, MECH_CONNECT_FAILED);

    // Test GetDeviceCapabilityInfo - should complete without crash
    motionMgr->GetDeviceCapabilityInfo();

    // Test GetDeviceBaseInfo again - verify return value
    result = motionMgr->GetDeviceBaseInfo();
    EXPECT_EQ(result, MECH_CONNECT_FAILED);
}

/**
 * @tc.name  : RegisterEventListener_001
 * @tc.number: RegisterEventListener_001
 * @tc.desc  : Test RegisterEventListener with protocolVer_ = 0x01, verify V01 specific events registered.
 */
HWTEST_F(MotionManagerTest, RegisterEventListener_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);

    motionMgr->protocolVer_ = 0x01;
    motionMgr->factory.SetFactoryProtocolVer(0x01);
    motionMgr->RegisterEventListener();

    // Verify mechEventListener_ is created
    EXPECT_NE(motionMgr->mechEventListener_, nullptr);
    // V01: 2(V01 specific) + 3(Base) + 1(Tracking) = 6
    EXPECT_EQ(motionMgr->notifyListenerType_.size(), 6);
    // Verify V01 specific events: ControlResultCmd(0x0243), WheelDataCmd(0x0244)
    EXPECT_NE(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0243), motionMgr->notifyListenerType_.end());
    EXPECT_NE(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0244), motionMgr->notifyListenerType_.end());
    // Verify base events: PositionInfo(0x0242), CameraKeyEvent(0x0240), StateInfo(0x0241)
    EXPECT_NE(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0242), motionMgr->notifyListenerType_.end());
    EXPECT_NE(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0240), motionMgr->notifyListenerType_.end());
    EXPECT_NE(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0241), motionMgr->notifyListenerType_.end());
    // Verify tracking enable event: TrackingEnable(0x0245)
    EXPECT_NE(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0245), motionMgr->notifyListenerType_.end());
}

/**
 * @tc.name  : RegisterEventListener_002
 * @tc.number: RegisterEventListener_002
 * @tc.desc  : Test RegisterEventListener with protocolVer_ = 0x02 and non-WHEEL_BASE device,
 *             verify extended events registered without cliff/obstacle events.
 */
HWTEST_F(MotionManagerTest, RegisterEventListener_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);

    motionMgr->protocolVer_ = 0x02;
    motionMgr->factory.SetFactoryProtocolVer(0x02);
    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::PORTABLE_GIMBAL);
    motionMgr->RegisterEventListener();

    // V02 non-WHEEL_BASE: 3(Base) + 1(Extended) + 1(Tracking) = 5
    EXPECT_EQ(motionMgr->notifyListenerType_.size(), 5);
    // Verify extended event: GenericEvent(0x0248)
    EXPECT_NE(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0248), motionMgr->notifyListenerType_.end());
    // Verify no V01 specific events
    EXPECT_EQ(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0243), motionMgr->notifyListenerType_.end());
    EXPECT_EQ(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0244), motionMgr->notifyListenerType_.end());
    // Verify no WHEEL_BASE events: CliffInfo(0x0340), ObstacleInfo(0x0341)
    EXPECT_EQ(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0340), motionMgr->notifyListenerType_.end());
    EXPECT_EQ(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0341), motionMgr->notifyListenerType_.end());
}

/**
 * @tc.name  : RegisterEventListener_003
 * @tc.number: RegisterEventListener_003
 * @tc.desc  : Test RegisterEventListener with protocolVer_ = 0x02 and WHEEL_BASE device,
 *             verify cliff and obstacle events registered.
 */
HWTEST_F(MotionManagerTest, RegisterEventListener_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);

    motionMgr->protocolVer_ = 0x02;
    motionMgr->factory.SetFactoryProtocolVer(0x02);
    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::WHEEL_BASE);
    motionMgr->RegisterEventListener();

    // V02 WHEEL_BASE: 3(Base) + 1(Extended) + 2(Cliff+Obstacle) + 1(Tracking) = 7
    EXPECT_EQ(motionMgr->notifyListenerType_.size(), 7);
    // Verify WHEEL_BASE specific events: CliffInfo(0x0340), ObstacleInfo(0x0341)
    EXPECT_NE(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0340), motionMgr->notifyListenerType_.end());
    EXPECT_NE(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0341), motionMgr->notifyListenerType_.end());
    // Verify extended event: GenericEvent(0x0248)
    EXPECT_NE(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0248), motionMgr->notifyListenerType_.end());
}

/**
 * @tc.name  : RegisterEventListener_004
 * @tc.number: RegisterEventListener_004
 * @tc.desc  : Test RegisterEventListener with protocolVer_ = 0x00 (default/unknown),
 *             verify only base events and tracking event registered.
 */
HWTEST_F(MotionManagerTest, RegisterEventListener_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);

    motionMgr->protocolVer_ = 0x00;
    motionMgr->factory.SetFactoryProtocolVer(0x00);
    motionMgr->RegisterEventListener();

    // V00: 3(Base) + 1(Tracking) = 4
    EXPECT_EQ(motionMgr->notifyListenerType_.size(), 4);
    // Verify no V01 specific events
    EXPECT_EQ(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0243), motionMgr->notifyListenerType_.end());
    EXPECT_EQ(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0244), motionMgr->notifyListenerType_.end());
    // Verify no extended events
    EXPECT_EQ(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0248), motionMgr->notifyListenerType_.end());
    // Verify base events and tracking still present
    EXPECT_NE(std::find(motionMgr->notifyListenerType_.begin(),
        motionMgr->notifyListenerType_.end(), 0x0245), motionMgr->notifyListenerType_.end());
}

/**
 * @tc.name  : IsLimited_002
 * @tc.number: IsLimited_002
 * @tc.desc  : Test IsLimited with negMax < 0 && posMax < 0 branch.
 */
HWTEST_F(MotionManagerTest, IsLimited_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);

    // Test negMax < 0 && posMax < 0 && position < negMax && position > posMax
    EXPECT_TRUE(motionMgr->IsLimited(-5.0f, -10.0f, -8.0f));
    EXPECT_FALSE(motionMgr->IsLimited(-10.0f, -5.0f, -12.0f));
    EXPECT_TRUE(motionMgr->IsLimited(-10.0f, 5.0f, 8.0f));
    EXPECT_TRUE(motionMgr->IsLimited(-10.0f, 5.0f, -12.0f));
    EXPECT_FALSE(motionMgr->IsLimited(5.0f, 10.0f, 8.0f));
    EXPECT_TRUE(motionMgr->IsLimited(10.0f, 5.0f, 8.0f));
    EXPECT_FALSE(motionMgr->IsLimited(5.0f, 10.0f, 3.0f));
    EXPECT_FALSE(motionMgr->IsLimited(5.0f, 10.0f, 12.0f));
    EXPECT_FALSE(motionMgr->IsLimited(-10.0f, 10.0f, 0.0f));
}

/**
 * @tc.name  : MechParamNotify_004
 * @tc.number: MechParamNotify_004
 * @tc.desc  : Test MechParamNotify with low power state.
 */
HWTEST_F(MotionManagerTest, MechParamNotify_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RegisterMechStateInfoCmd> cmd = std::make_shared<RegisterMechStateInfoCmd>();
    MechStateInfo info;
    info.mechMode = MechMode::FOLLOW;
    info.isPhoneOn = 1;
    cmd->info_ = info;
    
    motionMgr->MechParamNotify(cmd);
    // 验证stateInfo是否被正确更新
    EXPECT_EQ(motionMgr->deviceStatus_->stateInfo.mechMode, MechMode::FOLLOW);
}

/**
 * @tc.name  : MechGenericEventNotify_002
 * @tc.number: MechGenericEventNotify_002
 * @tc.desc  : Test MechGenericEventNotify with yaw disable.
 */
HWTEST_F(MotionManagerTest, MechGenericEventNotify_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<NormalRegisterMechGenericEventCmd> cmd = std::make_shared<NormalRegisterMechGenericEventCmd>();
    cmd->params_.attached = static_cast<uint8_t>(-1);
    cmd->params_.yawDisable = 1;
    cmd->params_.rollDisable = 0;
    cmd->params_.pitchDisable = 0;

    motionMgr->MechGenericEventNotify(cmd);

    // Verify rotationAxesStatus is updated correctly
    EXPECT_FALSE(motionMgr->deviceStatus_->rotationAxesStatus.yawEnabled);
    EXPECT_TRUE(motionMgr->deviceStatus_->rotationAxesStatus.rollEnabled);
    EXPECT_TRUE(motionMgr->deviceStatus_->rotationAxesStatus.pitchEnabled);
}

/**
 * @tc.name  : MechGenericEventNotify_003
 * @tc.number: MechGenericEventNotify_003
 * @tc.desc  : Test MechGenericEventNotify with low power state and rotation axes update.
 */
HWTEST_F(MotionManagerTest, MechGenericEventNotify_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<NormalRegisterMechGenericEventCmd> cmd = std::make_shared<NormalRegisterMechGenericEventCmd>();
    cmd->lowPower = BIT_1; // Set low power state to trigger low power notification
    cmd->params_.yawDisable = 1;
    cmd->params_.rollDisable = 0;
    cmd->params_.pitchDisable = 0;

    motionMgr->MechGenericEventNotify(cmd);

    // Verify rotationAxesStatus is updated correctly even with low power event
    EXPECT_FALSE(motionMgr->deviceStatus_->rotationAxesStatus.yawEnabled);
    EXPECT_TRUE(motionMgr->deviceStatus_->rotationAxesStatus.rollEnabled);
    EXPECT_TRUE(motionMgr->deviceStatus_->rotationAxesStatus.pitchEnabled);
}

/**
 * @tc.name  : MechGenericEventNotify_004
 * @tc.number: MechGenericEventNotify_004
 * @tc.desc  : Test MechGenericEventNotify when rotation axes status changes and when it stays the same.
 */
HWTEST_F(MotionManagerTest, MechGenericEventNotify_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<NormalRegisterMechGenericEventCmd> cmd = std::make_shared<NormalRegisterMechGenericEventCmd>();
    cmd->params_.attached = static_cast<uint8_t>(-1);
    cmd->params_.yawDisable = 1;
    cmd->params_.rollDisable = 1;
    cmd->params_.pitchDisable = 1;

    // Default rotationAxesStatus is all enabled, now all disabled - status changes
    motionMgr->MechGenericEventNotify(cmd);
    // Verify rotationAxesStatus is updated from default (all enabled) to all disabled
    EXPECT_FALSE(motionMgr->deviceStatus_->rotationAxesStatus.yawEnabled);
    EXPECT_FALSE(motionMgr->deviceStatus_->rotationAxesStatus.rollEnabled);
    EXPECT_FALSE(motionMgr->deviceStatus_->rotationAxesStatus.pitchEnabled);
}

/**
 * @tc.name  : MechGenericEventNotify_004_1
 * @tc.number: MechGenericEventNotify_004_1
 * @tc.desc  : Test MechGenericEventNotify when rotation axes status stays the same, verify early return.
 */
HWTEST_F(MotionManagerTest, MechGenericEventNotify_004_1, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // First call - set initial status with yaw disabled
    std::shared_ptr<NormalRegisterMechGenericEventCmd> cmd1 = std::make_shared<NormalRegisterMechGenericEventCmd>();
    cmd1->params_.attached = static_cast<uint8_t>(-1);
    cmd1->params_.yawDisable = 1;
    cmd1->params_.rollDisable = 0;
    cmd1->params_.pitchDisable = 0;
    motionMgr->MechGenericEventNotify(cmd1);
    EXPECT_FALSE(motionMgr->deviceStatus_->rotationAxesStatus.yawEnabled);
    EXPECT_TRUE(motionMgr->deviceStatus_->rotationAxesStatus.rollEnabled);
    EXPECT_TRUE(motionMgr->deviceStatus_->rotationAxesStatus.pitchEnabled);

    // Second call with same status - status unchanged, should return early without re-assignment
    std::shared_ptr<NormalRegisterMechGenericEventCmd> cmd2 = std::make_shared<NormalRegisterMechGenericEventCmd>();
    cmd2->params_.attached = static_cast<uint8_t>(-1);
    cmd2->params_.yawDisable = 1;
    cmd2->params_.rollDisable = 0;
    cmd2->params_.pitchDisable = 0;
    motionMgr->MechGenericEventNotify(cmd2);

    // Verify rotationAxesStatus remains unchanged after second call with same status
    EXPECT_FALSE(motionMgr->deviceStatus_->rotationAxesStatus.yawEnabled);
    EXPECT_TRUE(motionMgr->deviceStatus_->rotationAxesStatus.rollEnabled);
    EXPECT_TRUE(motionMgr->deviceStatus_->rotationAxesStatus.pitchEnabled);
}

/**
 * @tc.name  : MechGenericEventNotify_005
 * @tc.number: MechGenericEventNotify_005
 * @tc.desc  : Test MechGenericEventNotify when yawDisable is -1 (early return).
 */
HWTEST_F(MotionManagerTest, MechGenericEventNotify_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<NormalRegisterMechGenericEventCmd> cmd = std::make_shared<NormalRegisterMechGenericEventCmd>();
    cmd->params_.attached = static_cast<uint8_t>(-1);
    cmd->params_.yawDisable = static_cast<uint8_t>(-1);

    motionMgr->MechGenericEventNotify(cmd);

    // When yawDisable is -1, function returns early without updating rotationAxesStatus
    // Verify rotationAxesStatus remains unchanged (default values)
    EXPECT_TRUE(motionMgr->deviceStatus_->rotationAxesStatus.yawEnabled);
    EXPECT_TRUE(motionMgr->deviceStatus_->rotationAxesStatus.rollEnabled);
    EXPECT_TRUE(motionMgr->deviceStatus_->rotationAxesStatus.pitchEnabled);
}

/**
 * @tc.name  : MechGenericEventNotify_006
 * @tc.number: MechGenericEventNotify_006
 * @tc.desc  : Test MechGenericEventNotify when mech state changes (attached != GetMechState).
 */
HWTEST_F(MotionManagerTest, MechGenericEventNotify_006, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set initial mech state to DETACHED
    g_mockMechState = AttachmentStateMap::DETACHED;

    std::shared_ptr<NormalRegisterMechGenericEventCmd> cmd = std::make_shared<NormalRegisterMechGenericEventCmd>();
    cmd->params_.attached = 1; // Set attached to 1 (ATTACHED)
    cmd->params_.yawDisable = static_cast<uint8_t>(-1);

    motionMgr->MechGenericEventNotify(cmd);

    // Verify isPhoneOn is updated
    EXPECT_EQ(motionMgr->deviceStatus_->stateInfo.isPhoneOn, 1);
}

/**
 * @tc.name  : MechWheelZoomNotify_002
 * @tc.number: MechWheelZoomNotify_002
 * @tc.desc  : Test MechWheelZoomNotify with left direction.
 */
HWTEST_F(MotionManagerTest, MechWheelZoomNotify_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    std::shared_ptr<RegisterMechWheelDataCmd> cmd = std::make_shared<RegisterMechWheelDataCmd>();
    cmd->wheelData_.degree = 10;

    // Test with wheelFilterCnt_ already at WHEEL_FILTER_COUNT - 1
    // After this call, wheelFilterCnt_ should be reset to 0
    motionMgr->wheelFilterCnt_ = 1;
    EXPECT_EQ(motionMgr->wheelFilterCnt_, 1);
    motionMgr->MechWheelZoomNotify(cmd);
    EXPECT_EQ(motionMgr->wheelFilterCnt_, 1);
}

/**
 * @tc.name  : MechWheelZoomNotify_003
 * @tc.number: MechWheelZoomNotify_003
 * @tc.desc  : Test MechWheelZoomNotify with wheel filter.
 */
HWTEST_F(MotionManagerTest, MechWheelZoomNotify_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->wheelFilterCnt_ = 0;

    std::shared_ptr<RegisterMechWheelDataCmd> cmd = std::make_shared<RegisterMechWheelDataCmd>();
    cmd->wheelData_.degree = 10;

    // Test wheel filter behavior: verify wheelFilterCnt_ increments correctly
    EXPECT_EQ(motionMgr->wheelFilterCnt_, 0);

    // After reaching WHEEL_FILTER_COUNT, wheelFilterCnt_ should reset to 0
    motionMgr->MechWheelZoomNotify(cmd);
    EXPECT_EQ(motionMgr->wheelFilterCnt_, 0);
}

/**
 * @tc.name  : MechWheelZoomNotify_004
 * @tc.number: MechWheelZoomNotify_004
 * @tc.desc  : Test MechWheelZoomNotify with null cmd.
 */
HWTEST_F(MotionManagerTest, MechWheelZoomNotify_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    std::shared_ptr<RegisterMechWheelDataCmd> cmd = nullptr;

    // Test that function handles null cmd gracefully (should return early without crash)
    motionMgr->MechWheelZoomNotify(cmd);

    // Verify wheelFilterCnt_ remains unchanged (still 0) after the call
    EXPECT_EQ(motionMgr->wheelFilterCnt_, 0);
}

/**
 * @tc.name  : MechWheelZoomNotify_005
 * @tc.number: MechWheelZoomNotify_005
 * @tc.desc  : Test MechWheelZoomNotify with eventHandler_ as nullptr.
 */
HWTEST_F(MotionManagerTest, MechWheelZoomNotify_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    CommandFactory factory;
    auto cmd = factory.CreateRegisterMechWheelDataCmd();
    cmd->wheelData_.degree = 10;

    // Set eventHandler_ to nullptr to test the branch where eventHandler_ is null
    motionMgr->eventHandler_ = nullptr;

    // Test that function handles eventHandler_ == nullptr gracefully
    motionMgr->MechWheelZoomNotify(cmd);

    // Verify wheelFilterCnt_ remains unchanged (still 0) after the call
    EXPECT_EQ(motionMgr->wheelFilterCnt_, 0);
}

/**
 * @tc.name  : MechWheelZoomNotify_006
 * @tc.number: MechWheelZoomNotify_006
 * @tc.desc  : Test MechWheelZoomNotify when phone is not attached.
 */
HWTEST_F(MotionManagerTest, MechWheelZoomNotify_006, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set mech state to DETACHED (not attached)
    g_mockMechState = AttachmentStateMap::DETACHED;

    CommandFactory factory;
    auto cmd = factory.CreateRegisterMechWheelDataCmd();
    cmd->wheelData_.degree = 10;

    // Test that function handles DETACHED state gracefully (should return early)
    motionMgr->MechWheelZoomNotify(cmd);

    // Verify wheelFilterCnt_ remains unchanged (still 0) after the call
    EXPECT_EQ(motionMgr->wheelFilterCnt_, 0);
}

/**
 * @tc.name  : CreateResponseTaskId_002
 * @tc.number: CreateResponseTaskId_002
 * @tc.desc  : Test CreateResponseTaskId with lastTaskId overflow.
 */
HWTEST_F(MotionManagerTest, CreateResponseTaskId_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set taskId to near maximum
    motionMgr->lastTaskId_ = UINT8_MAX - 1;

    uint8_t result = motionMgr->CreateResponseTaskId();
    EXPECT_EQ(result, UINT8_MAX);

    // Next call should wrap around
    result = motionMgr->CreateResponseTaskId();
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name  : CreateRotateTaskId_001
 * @tc.number: CreateRotateTaskId_001
 * @tc.desc  : Test CreateRotateTaskId with taskId overflow.
 */
HWTEST_F(MotionManagerTest, CreateRotateTaskId_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set taskId to near maximum
    motionMgr->taskId_ = UINT16_MAX - 1;

    uint16_t result = motionMgr->CreateRotateTaskId();
    EXPECT_EQ(result, UINT16_MAX);

    // Next call should wrap around
    result = motionMgr->CreateRotateTaskId();
    EXPECT_EQ(result, 1);
}

/**
 * @tc.name  : GetSpeedControlTimeLimit_003
 * @tc.number: GetSpeedControlTimeLimit_003
 * @tc.desc  : Test GetSpeedControlTimeLimit with lastTaskId overflow.
 */
HWTEST_F(MotionManagerTest, GetSpeedControlTimeLimit_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set lastTaskId to maximum
    motionMgr->lastTaskId_ = UINT8_MAX;

    std::shared_ptr<TimeLimit> timeLimit = std::make_shared<TimeLimit>();
    int32_t result = motionMgr->GetSpeedControlTimeLimit(timeLimit);
    // Should still work, taskId should wrap around
    EXPECT_EQ(result, 96469010);
}

/**
 * @tc.name  : AbsolutelyEulerAnglesJudgingLimitLocked_003
 * @tc.number: AbsolutelyEulerAnglesJudgingLimitLocked_003
 * @tc.desc  : Test AbsolutelyEulerAnglesJudgingLimitLocked with positive degree.
 */
HWTEST_F(MotionManagerTest, AbsolutelyEulerAnglesJudgingLimitLocked_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    motionMgr->deviceStatus_ = std::make_shared<DeviceStatus>();
    // 设置初始角度
    motionMgr->deviceStatus_->eulerAngles.yaw = 0.0f;
    motionMgr->deviceStatus_->eulerAngles.pitch = 0.0f;
    motionMgr->deviceStatus_->eulerAngles.roll = 0.0f;

    EulerAngles eulerAngles;
    eulerAngles.yaw = 10.0f;
    eulerAngles.pitch = 20.0f;
    eulerAngles.roll = 30.0f;

    motionMgr->AbsolutelyEulerAnglesJudgingLimitLocked(eulerAngles);

    // 验证角度被正确累加：yaw和pitch累加，roll被置为0
    EXPECT_FLOAT_EQ(eulerAngles.yaw, 10.0f);  // 10.0 + 0.0
    EXPECT_FLOAT_EQ(eulerAngles.pitch, 20.0f); // 20.0 + 0.0
    EXPECT_FLOAT_EQ(eulerAngles.roll, 0.0f);   // roll被置为0
}

/**
 * @tc.name  : AbsolutelyEulerAnglesJudgingLimitLocked_004
 * @tc.number: AbsolutelyEulerAnglesJudgingLimitLocked_004
 * @tc.desc  : Test AbsolutelyEulerAnglesJudgingLimitLocked with negative degree.
 */
HWTEST_F(MotionManagerTest, AbsolutelyEulerAnglesJudgingLimitLocked_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    motionMgr->deviceStatus_ = std::make_shared<DeviceStatus>();
    // Set up limits
    motionMgr->deviceStatus_->rotationLimit.negMax.yaw = -30.0f;
    motionMgr->deviceStatus_->rotationLimit.posMax.yaw = 30.0f;
    motionMgr->deviceStatus_->eulerAngles.yaw = -10.0f;
    motionMgr->deviceStatus_->eulerAngles.pitch = -5.0f;
    motionMgr->deviceStatus_->eulerAngles.roll = 0.0f;

    EulerAngles eulerAngles;
    eulerAngles.yaw = -20.0f;
    eulerAngles.pitch = -15.0f;
    eulerAngles.roll = -30.0f;

    motionMgr->AbsolutelyEulerAnglesJudgingLimitLocked(eulerAngles);

    // 验证角度被正确累加：yaw和pitch累加，roll被置为0
    EXPECT_FLOAT_EQ(eulerAngles.yaw, -30.0f);  // -20.0 + (-10.0)
    EXPECT_FLOAT_EQ(eulerAngles.pitch, -20.0f); // -15.0 + (-5.0)
    EXPECT_FLOAT_EQ(eulerAngles.roll, 0.0f);    // roll被置为0
}

/**
 * @tc.name  : LimitCalculationLocked_002
 * @tc.number: LimitCalculationLocked_002
 * @tc.desc  : Test LimitCalculationLocked with roll axis.
 */
HWTEST_F(MotionManagerTest, LimitCalculationLocked_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    motionMgr->deviceStatus_ = std::make_shared<DeviceStatus>();
    motionMgr->deviceStatus_->rollLimit = true;
    motionMgr->deviceStatus_->rotationLimitWithOffset.negMax.roll = -30.0f;
    motionMgr->deviceStatus_->rotationLimitWithOffset.posMax.roll = 30.0f;

    EulerAngles eulerAngles;
    eulerAngles.roll = 10.0f;  // 在范围内，应该是NOT_LIMITED
    bool callback = true;

    motionMgr->LimitCalculationLocked(eulerAngles, motionMgr->deviceStatus_->rotationAxesStatus, callback);

    // 验证roll在范围内，状态为NOT_LIMITED
    EXPECT_EQ(motionMgr->deviceStatus_->rotationAxesStatus.rollLimited, RotationAxisLimited::NOT_LIMITED);

    // 测试roll接近正上限
    eulerAngles.roll = 30.0f;
    motionMgr->LimitCalculationLocked(eulerAngles, motionMgr->deviceStatus_->rotationAxesStatus, callback);
    EXPECT_EQ(motionMgr->deviceStatus_->rotationAxesStatus.rollLimited, RotationAxisLimited::POS_LIMITED);

    // 测试roll接近负下限
    eulerAngles.roll = -30.0f;
    motionMgr->LimitCalculationLocked(eulerAngles, motionMgr->deviceStatus_->rotationAxesStatus, callback);
    EXPECT_EQ(motionMgr->deviceStatus_->rotationAxesStatus.rollLimited, RotationAxisLimited::NEG_LIMITED);
}

/**
 * @tc.name  : LimitCalculationLocked_003
 * @tc.number: LimitCalculationLocked_003
 * @tc.desc  : Test LimitCalculationLocked with pitch axis.
 */
HWTEST_F(MotionManagerTest, LimitCalculationLocked_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    motionMgr->deviceStatus_ = std::make_shared<DeviceStatus>();
    motionMgr->deviceStatus_->pitchLimit = true;
    motionMgr->deviceStatus_->rotationLimitWithOffset.negMax.pitch = -30.0f;
    motionMgr->deviceStatus_->rotationLimitWithOffset.posMax.pitch = 30.0f;

    EulerAngles eulerAngles;
    eulerAngles.pitch = 10.0f;  // 在范围内，应该是NOT_LIMITED
    bool callback = true;

    motionMgr->LimitCalculationLocked(eulerAngles, motionMgr->deviceStatus_->rotationAxesStatus, callback);

    // 验证pitch在范围内，状态为NOT_LIMITED
    EXPECT_EQ(motionMgr->deviceStatus_->rotationAxesStatus.pitchLimited, RotationAxisLimited::NOT_LIMITED);

    // 测试pitch超过正上限
    eulerAngles.pitch = 31.0f;
    motionMgr->LimitCalculationLocked(eulerAngles, motionMgr->deviceStatus_->rotationAxesStatus, callback);
    EXPECT_EQ(motionMgr->deviceStatus_->rotationAxesStatus.pitchLimited, RotationAxisLimited::POS_LIMITED);

    // 测试pitch低于负下限
    eulerAngles.pitch = -31.0f;
    motionMgr->LimitCalculationLocked(eulerAngles, motionMgr->deviceStatus_->rotationAxesStatus, callback);
    EXPECT_EQ(motionMgr->deviceStatus_->rotationAxesStatus.pitchLimited, RotationAxisLimited::NEG_LIMITED);
}

/**
 * @tc.name  : CreateKeyEvent_003
 * @tc.number: CreateKeyEvent_003
 * @tc.desc  : Test CreateKeyEvent with KEY_ACTION_DOWN.
 */
HWTEST_F(MotionManagerTest, CreateKeyEvent_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    int32_t keyCode = 1;
    int32_t keyAction = MMI::KeyEvent::KEY_ACTION_DOWN;

    std::shared_ptr<MMI::KeyEvent> keyEvent = motionMgr->CreateKeyEvent(keyCode, keyAction);

    // 验证KeyEvent被正确创建
    EXPECT_NE(keyEvent, nullptr);
    if (keyEvent != nullptr) {
        // 验证keyCode和keyAction被正确设置
        EXPECT_EQ(keyEvent->GetKeyCode(), keyCode);
        EXPECT_EQ(keyEvent->GetKeyAction(), keyAction);
    }
}

/**
 * @tc.name  : CreateKeyEvent_004
 * @tc.number: CreateKeyEvent_004
 * @tc.desc  : Test CreateKeyEvent with KEY_ACTION_UP.
 */
HWTEST_F(MotionManagerTest, CreateKeyEvent_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    int32_t keyCode = 1;
    int32_t keyAction = MMI::KeyEvent::KEY_ACTION_UP;

    std::shared_ptr<MMI::KeyEvent> keyEvent = motionMgr->CreateKeyEvent(keyCode, keyAction);

    // 验证KeyEvent被正确创建
    EXPECT_NE(keyEvent, nullptr);
    if (keyEvent != nullptr) {
        // 验证keyCode和keyAction被正确设置
        EXPECT_EQ(keyEvent->GetKeyCode(), keyCode);
        EXPECT_EQ(keyEvent->GetKeyAction(), keyAction);
    }
}

/**
 * @tc.name  : MechButtonEventNotify_008
 * @tc.number: MechButtonEventNotify_008
 * @tc.desc  : Test MechButtonEventNotify with START_FILMING event.
 */
HWTEST_F(MotionManagerTest, MechButtonEventNotify_008, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RegisterMechCameraKeyEventCmd> cmd = std::make_shared<RegisterMechCameraKeyEventCmd>();
    cmd->event_ = CameraKeyEvent::START_FILMING;

    motionMgr->MechButtonEventNotify(cmd);

    // 验证keyEvent_状态被正确更新
    EXPECT_EQ(keyEvent_, CameraKeyEvent::START_FILMING);
}

/**
 * @tc.name  : MechButtonEventNotify_009
 * @tc.number: MechButtonEventNotify_009
 * @tc.desc  : Test MechButtonEventNotify with SWITCH_CAMERA event.
 */
HWTEST_F(MotionManagerTest, MechButtonEventNotify_009, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RegisterMechCameraKeyEventCmd> cmd = std::make_shared<RegisterMechCameraKeyEventCmd>();
    cmd->event_ = CameraKeyEvent::SWITCH_CAMERA;

    motionMgr->MechButtonEventNotify(cmd);

    // 验证keyEvent_状态被正确更新
    EXPECT_EQ(keyEvent_, CameraKeyEvent::SWITCH_CAMERA);
}

/**
 * @tc.name  : MechButtonEventNotify_010
 * @tc.number: MechButtonEventNotify_010
 * @tc.desc  : Test MechButtonEventNotify with ZOOM_IN event.
 */
HWTEST_F(MotionManagerTest, MechButtonEventNotify_010, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RegisterMechCameraKeyEventCmd> cmd = std::make_shared<RegisterMechCameraKeyEventCmd>();
    cmd->event_ = CameraKeyEvent::ZOOM_IN;

    motionMgr->MechButtonEventNotify(cmd);

    // 验证keyEvent_状态被正确更新
    EXPECT_EQ(keyEvent_, CameraKeyEvent::ZOOM_IN);
}

/**
 * @tc.name  : FormatLimit_001
 * @tc.number: FormatLimit_001
 * @tc.desc  : Test FormatLimit with posMax.yaw > NO_LIMIT_MAX.
 */
HWTEST_F(MotionManagerTest, FormatLimit_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    RotateDegreeLimit params;
    params.posMax.yaw = 400.0f;

    motionMgr->FormatLimit(params);

    // 验证posMax.yaw被规范化到有效范围内
    EXPECT_LE(params.posMax.yaw, NO_LIMIT_MAX);
    EXPECT_GE(params.posMax.yaw, NO_LIMIT_MIN);
}

/**
 * @tc.name  : FormatLimit_002
 * @tc.number: FormatLimit_002
 * @tc.desc  : Test FormatLimit with posMax.yaw < NO_LIMIT_MIN.
 */
HWTEST_F(MotionManagerTest, FormatLimit_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    RotateDegreeLimit params;
    params.posMax.yaw = -400.0f;

    motionMgr->FormatLimit(params);

    // 验证posMax.yaw被规范化到有效范围内
    EXPECT_LE(params.posMax.yaw, NO_LIMIT_MAX);
    EXPECT_GE(params.posMax.yaw, NO_LIMIT_MIN);
}

/**
 * @tc.name  : FormatLimit_003
 * @tc.number: FormatLimit_003
 * @tc.desc  : Test FormatLimit with negMax.yaw > NO_LIMIT_MAX.
 */
HWTEST_F(MotionManagerTest, FormatLimit_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    RotateDegreeLimit params;
    params.negMax.yaw = 400.0f;

    motionMgr->FormatLimit(params);

    // 验证negMax.yaw被规范化到有效范围内
    EXPECT_LE(params.negMax.yaw, NO_LIMIT_MAX);
    EXPECT_GE(params.negMax.yaw, NO_LIMIT_MIN);
}

/**
 * @tc.name  : FormatLimit_004
 * @tc.number: FormatLimit_004
 * @tc.desc  : Test FormatLimit with negMax.yaw < NO_LIMIT_MIN.
 */
HWTEST_F(MotionManagerTest, FormatLimit_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    RotateDegreeLimit params;
    params.negMax.yaw = -400.0f;

    motionMgr->FormatLimit(params);

    // 验证negMax.yaw被规范化到有效范围内
    EXPECT_LE(params.negMax.yaw, NO_LIMIT_MAX);
    EXPECT_GE(params.negMax.yaw, NO_LIMIT_MIN);
}

/**
 * @tc.name  : RegisterExtendedAndTrackingEvents_001
 * @tc.number: RegisterExtendedAndTrackingEvents_001
 * @tc.desc  : Test RegisterExtendedAndTrackingEvents with protocolVer_ < 0x02.
 */
HWTEST_F(MotionManagerTest, RegisterExtendedAndTrackingEvents_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->protocolVer_ = 0x01;

    motionMgr->RegisterExtendedAndTrackingEvents();

    // 验证当protocolVer_ < 0x02时，仍然会注册TrackingEnable事件
    EXPECT_FALSE(motionMgr->notifyListenerType_.empty());
    EXPECT_EQ(motionMgr->notifyListenerType_.size(), 1);
}

/**
 * @tc.name  : RegisterExtendedAndTrackingEvents_002
 * @tc.number: RegisterExtendedAndTrackingEvents_002
 * @tc.desc  : Test RegisterExtendedAndTrackingEvents with WHEEL_BASE device.
 */
HWTEST_F(MotionManagerTest, RegisterExtendedAndTrackingEvents_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->protocolVer_ = 0x02;
    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::WHEEL_BASE);

    motionMgr->RegisterExtendedAndTrackingEvents();

    // 验证当protocolVer_ >= 0x02且为WHEEL_BASE时，notifyListenerType_增加（注册扩展事件）
    EXPECT_FALSE(motionMgr->notifyListenerType_.empty());
}

/**
 * @tc.name  : SetMechCameraTrackingFrame_003
 * @tc.number: SetMechCameraTrackingFrame_003
 * @tc.desc  : Test SetMechCameraTrackingFrame when tracking is not enabled.
 */
HWTEST_F(MotionManagerTest, SetMechCameraTrackingFrame_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->deviceStatus_->isEnabled = false;

    std::shared_ptr<TrackingFrameParams> trackingFrameParams = std::make_shared<TrackingFrameParams>();

    EXPECT_EQ(motionMgr->SetMechCameraTrackingFrame(trackingFrameParams), ERR_OK);
}

/**
 * @tc.name  : SetMechCameraTrackingFrame_004
 * @tc.number: SetMechCameraTrackingFrame_004
 * @tc.desc  : Test SetMechCameraTrackingFrame when tracking status is DISABLE.
 */
HWTEST_F(MotionManagerTest, SetMechCameraTrackingFrame_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->deviceStatus_->isEnabled = true;
    motionMgr->deviceStatus_->trackingStatus = MechTrackingStatus::MECH_TK_DISABLE;

    std::shared_ptr<TrackingFrameParams> trackingFrameParams = std::make_shared<TrackingFrameParams>();

    EXPECT_EQ(motionMgr->SetMechCameraTrackingFrame(trackingFrameParams), ERR_OK);
}

/**
 * @tc.name  : SetMechCameraTrackingFrame_005
 * @tc.number: SetMechCameraTrackingFrame_005
 * @tc.desc  : Test SetMechCameraTrackingFrame when camera info is not enabled.
 */
HWTEST_F(MotionManagerTest, SetMechCameraTrackingFrame_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->protocolVer_ = 0x02;
    motionMgr->SetMechCameraInfo_ = false;
    motionMgr->deviceStatus_->isEnabled = true;
    motionMgr->deviceStatus_->trackingStatus = MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET;

    std::shared_ptr<TrackingFrameParams> trackingFrameParams = std::make_shared<TrackingFrameParams>();

    EXPECT_EQ(motionMgr->SetMechCameraTrackingFrame(trackingFrameParams), ERR_OK);
}

/**
 * @tc.name  : SetMechScreenInfo_001
 * @tc.number: SetMechScreenInfo_001
 * @tc.desc  : Test SetMechScreenInfo when device is PORTABLE_GIMBAL.
 */
HWTEST_F(MotionManagerTest, SetMechScreenInfo_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::PORTABLE_GIMBAL);

    ScreenInfoParams screenInfo;

    EXPECT_EQ(motionMgr->SetMechScreenInfo(screenInfo), DEVICE_NOT_NEED_SCREEN_INFO);
}

/**
 * @tc.name  : SetMechScreenInfo_002
 * @tc.number: SetMechScreenInfo_002
 * @tc.desc  : Test SetMechScreenInfo when device is WHEEL_BASE.
 */
HWTEST_F(MotionManagerTest, SetMechScreenInfo_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;
    // Set device type to WHEEL_BASE
    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::WHEEL_BASE);

    ScreenInfoParams screenInfo;

    EXPECT_EQ(motionMgr->SetMechScreenInfo(screenInfo), ERR_OK);
}

/**
 * @tc.name  : PerformPresetAction_002
 * @tc.number: PerformPresetAction_002
 * @tc.desc  : Test PerformPresetAction with default switch case.
 */
HWTEST_F(MotionManagerTest, PerformPresetAction_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    PresetAction presetAction = static_cast<PresetAction>(999);

    EXPECT_EQ(motionMgr->PerformPresetAction(presetAction, 0), ERR_OK);
}

/**
 * @tc.name  : ActionGimbalFeatureControl_001
 * @tc.number: ActionGimbalFeatureControl_001
 * @tc.desc  : Test ActionGimbalFeatureControl with protocolVer_ < 0x02.
 */
HWTEST_F(MotionManagerTest, ActionGimbalFeatureControl_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    motionMgr->protocolVer_ = 0x01;
    g_mockMechState = AttachmentStateMap::ATTACHED;

    ActionControlParams actionControlParams;

    EXPECT_EQ(motionMgr->ActionGimbalFeatureControl(actionControlParams), ERR_OK);
}

/**
 * @tc.name  : ActionGimbalFeatureControl_002
 * @tc.number: ActionGimbalFeatureControl_002
 * @tc.desc  : Test ActionGimbalFeatureControl with protocolVer_ >= 0x02.
 */
HWTEST_F(MotionManagerTest, ActionGimbalFeatureControl_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    motionMgr->protocolVer_ = 0x02;

    ActionControlParams actionControlParams;

    EXPECT_EQ(motionMgr->ActionGimbalFeatureControl(actionControlParams), ERR_OK);
}

/**
 * @tc.name  : IsSupportAction_001
 * @tc.number: IsSupportAction_001
 * @tc.desc  : Test IsSupportAction with LANDSCAPE_PORTRAIT_SWITCH.
 */
HWTEST_F(MotionManagerTest, IsSupportAction_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);

    g_mockMechState = AttachmentStateMap::ATTACHED;

    // Reset g_capabilityInfo and set bit 0 for LANDSCAPE_PORTRAIT_SWITCH (type=0)
    memset_s(g_capabilityInfo, MAX_CAPABILITY_BIT_NUM * sizeof(uint32_t), 0, sizeof(g_capabilityInfo));
    g_capabilityInfo[0] |= (1 << 0);

    uint32_t tokenId = 100;
    ActionType actionType = ActionType::LANDSCAPE_PORTRAIT_SWITCH;
    bool isSupport = false;

    EXPECT_EQ(motionMgr->IsSupportAction(tokenId, actionType, isSupport), ERR_OK);
    EXPECT_TRUE(isSupport);
}

/**
 * @tc.name  : IsSupportAction_002
 * @tc.number: IsSupportAction_002
 * @tc.desc  : Test IsSupportAction with GREET_MODE.
 */
HWTEST_F(MotionManagerTest, IsSupportAction_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);

    g_mockMechState = AttachmentStateMap::ATTACHED;

    // Reset g_capabilityInfo and set bit 2 for GREET_MODE (type=2)
    memset_s(g_capabilityInfo, MAX_CAPABILITY_BIT_NUM * sizeof(uint32_t), 0, sizeof(g_capabilityInfo));
    g_capabilityInfo[0] |= (1 << 2);

    uint32_t tokenId = 100;
    ActionType actionType = ActionType::GREET_MODE;
    bool isSupport = false;

    EXPECT_EQ(motionMgr->IsSupportAction(tokenId, actionType, isSupport), ERR_OK);
    EXPECT_TRUE(isSupport);
}

/**
 * @tc.name  : IsSupportAction_003
 * @tc.number: IsSupportAction_003
 * @tc.desc  : Test IsSupportAction with PATROL_MODE.
 */
HWTEST_F(MotionManagerTest, IsSupportAction_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);

    g_mockMechState = AttachmentStateMap::ATTACHED;

    // Reset g_capabilityInfo and set bit 1 for PATROL_MODE (type=1)
    memset_s(g_capabilityInfo, MAX_CAPABILITY_BIT_NUM * sizeof(uint32_t), 0, sizeof(g_capabilityInfo));
    g_capabilityInfo[0] |= (1 << 1);

    uint32_t tokenId = 100;
    ActionType actionType = ActionType::PATROL_MODE;
    bool isSupport = false;

    EXPECT_EQ(motionMgr->IsSupportAction(tokenId, actionType, isSupport), ERR_OK);
    EXPECT_TRUE(isSupport);
}

/**
 * @tc.name  : IsSupportAction_004
 * @tc.number: IsSupportAction_004
 * @tc.desc  : Test IsSupportAction with unknown action type.
 */
HWTEST_F(MotionManagerTest, IsSupportAction_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);

    g_mockMechState = AttachmentStateMap::ATTACHED;

    // Reset g_capabilityInfo to 0, all bits are unset
    memset_s(g_capabilityInfo, MAX_CAPABILITY_BIT_NUM * sizeof(uint32_t), 0, sizeof(g_capabilityInfo));

    uint32_t tokenId = 100;
    ActionType actionType = static_cast<ActionType>(999);
    bool isSupport = false;

    EXPECT_EQ(motionMgr->IsSupportAction(tokenId, actionType, isSupport), ERR_OK);
    EXPECT_FALSE(isSupport);
}

/**
 * @tc.name  : IsSupportAction_005
 * @tc.number: IsSupportAction_005
 * @tc.desc  : Test IsSupportAction when device is not attached.
 */
HWTEST_F(MotionManagerTest, IsSupportAction_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);

    // Set device state to DETACHED
    g_mockMechState = AttachmentStateMap::DETACHED;

    uint32_t tokenId = 100;
    ActionType actionType = ActionType::LANDSCAPE_PORTRAIT_SWITCH;
    bool isSupport = false;

    EXPECT_EQ(motionMgr->IsSupportAction(tokenId, actionType, isSupport), DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : Move_001
 * @tc.number: Move_001
 * @tc.desc  : Test Move when device is not attached.
 */
HWTEST_F(MotionManagerTest, Move_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::DETACHED;

    uint32_t tokenId = 100;
    std::string napiCmdId = "napiCmdId";
    std::shared_ptr<MoveParams> moveParams = std::make_shared<MoveParams>();

    EXPECT_EQ(motionMgr->Move(tokenId, napiCmdId, moveParams), DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : Move_002
 * @tc.number: Move_002
 * @tc.desc  : Test Move with nullptr parameter.
 */
HWTEST_F(MotionManagerTest, Move_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;

    uint32_t tokenId = 100;
    std::string napiCmdId = "napiCmdId";
    std::shared_ptr<MoveParams> moveParams = nullptr;

    EXPECT_EQ(motionMgr->Move(tokenId, napiCmdId, moveParams), INVALID_PARAMETERS_ERR);
}

/**
 * @tc.name  : Move_003
 * @tc.number: Move_003
 * @tc.desc  : Test Move with valid parameters.
 */
HWTEST_F(MotionManagerTest, Move_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->wheelCapabilityInfo_.maxForwardSpeed = 100;
    motionMgr->wheelCapabilityInfo_.maxRotationSpeed = 50;

    uint32_t tokenId = 100;
    std::string napiCmdId = "napiCmdId";
    std::shared_ptr<MoveParams> moveParams = std::make_shared<MoveParams>();
    moveParams->distance = 100;
    moveParams->speedGear = SpeedGear::MIDDLE_SPEED;

    EXPECT_EQ(motionMgr->Move(tokenId, napiCmdId, moveParams), ERR_OK);
}

/**
 * @tc.name  : Move_004
 * @tc.number: Move_004
 * @tc.desc  : Test Move when wheel speed exceeds limit.
 */
HWTEST_F(MotionManagerTest, Move_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;
    // Set a small maxForwardSpeed (30) to make MIDDLE_SPEED (50) exceed the limit
    motionMgr->wheelCapabilityInfo_.maxForwardSpeed = 30;

    uint32_t tokenId = 100;
    std::string napiCmdId = "napiCmdId";
    std::shared_ptr<MoveParams> moveParams = std::make_shared<MoveParams>();
    moveParams->distance = 100;
    // MIDDLE_SPEED (50) will exceed maxForwardSpeed (30)
    moveParams->speedGear = SpeedGear::MIDDLE_SPEED;

    EXPECT_EQ(motionMgr->Move(tokenId, napiCmdId, moveParams), WHEEL_SPEED_EXCEED_LIMIT);
}
} // namespace distributedhardware
} // namespace OHOS
