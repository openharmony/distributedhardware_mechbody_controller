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

#define private public
#define protected public
#include <stdexcept>
#include <iostream>

#include "mechbody_controller_log.h"
#include "mc_register_mech_position_info_cmd.h"
#include "mc_register_mech_camera_key_event_cmd.h"
#include "mechbody_controller_service.h"
#include "mc_register_mech_control_result_cmd.h"
#include "mc_command_base.h"
#undef private
#undef protected

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS {
namespace MechBodyController {
constexpr float DEFAULT_DURATION = 500;

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

static bool g_mockMechState = false;

bool MechConnectManager::GetMechState(int32_t mechId)
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
    EulerAngles eulerAngles;
    eulerAngles.yaw = 100;
    eulerAngles.roll = 100;
    eulerAngles.pitch = 100;

    std::shared_ptr<RegisterMechPositionInfoCmd> cmdNull = nullptr;
    EXPECT_NO_FATAL_FAILURE(motionMgr->MechAttitudeNotify(cmdNull));

    CommandFactory factory;
    auto cmd = factory.CreateRegisterMechPositionInfoCmd();
    motionMgr->deviceStatus_->eulerAngles = cmd->GetPosition();
    EXPECT_NO_FATAL_FAILURE(motionMgr->MechAttitudeNotify(cmd));

    motionMgr->deviceStatus_->eulerAngles = eulerAngles;
    EXPECT_NO_FATAL_FAILURE(motionMgr->MechAttitudeNotify(cmd));
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

    EXPECT_NE(keyEvent, nullptr);
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

    std::shared_ptr<CameraInfo> cameraInfo = std::make_shared<CameraInfo>();
    cameraInfo->isCameraOn_ = true;
    McCameraTrackingController::GetInstance().currentCameraInfo_ = cameraInfo;

    std::shared_ptr<RegisterMechCameraKeyEventCmd> cmd = std::make_shared<RegisterMechCameraKeyEventCmd>();
    cmd->event_ = CameraKeyEvent::START_FILMING;
    motionMgr->MechButtonEventNotify(cmd);

    EXPECT_EQ(keyEvent_, CameraKeyEvent::START_FILMING);
}

/**
 * @tc.name  : MechButtonEventNotify_002
 * @tc.number: MechButtonEventNotify_002
 * @tc.desc  : Test MechButtonEventNotify with SWITCH_TRACKING event.
 */
HWTEST_F(MotionManagerTest, MechButtonEventNotify_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<CameraInfo> cameraInfo = std::make_shared<CameraInfo>();
    cameraInfo->isCameraOn_ = true;
    McCameraTrackingController::GetInstance().currentCameraInfo_ = cameraInfo;

    std::shared_ptr<RegisterMechCameraKeyEventCmd> cmd = std::make_shared<RegisterMechCameraKeyEventCmd>();
    cmd->event_ = CameraKeyEvent::SWITCH_TRACKING;
    motionMgr->MechButtonEventNotify(cmd);

    EXPECT_NE(keyEvent_, CameraKeyEvent::SWITCH_TRACKING);
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

    std::shared_ptr<CameraInfo> cameraInfo = std::make_shared<CameraInfo>();
    cameraInfo->isCameraOn_ = true;
    McCameraTrackingController::GetInstance().currentCameraInfo_ = cameraInfo;

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

    std::shared_ptr<CameraInfo> cameraInfo = std::make_shared<CameraInfo>();
    cameraInfo->isCameraOn_ = true;
    McCameraTrackingController::GetInstance().currentCameraInfo_ = cameraInfo;

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

    std::shared_ptr<CameraInfo> cameraInfo = std::make_shared<CameraInfo>();
    cameraInfo->isCameraOn_ = true;
    McCameraTrackingController::GetInstance().currentCameraInfo_ = cameraInfo;

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

    std::shared_ptr<CameraInfo> cameraInfo = std::make_shared<CameraInfo>();
    cameraInfo->isCameraOn_ = true;
    McCameraTrackingController::GetInstance().currentCameraInfo_ = cameraInfo;

    std::shared_ptr<RegisterMechCameraKeyEventCmd> cmd = std::make_shared<RegisterMechCameraKeyEventCmd>();
    cmd->event_ = CameraKeyEvent::SWITCH_CAMERA;
    motionMgr->MechButtonEventNotify(cmd);

    EXPECT_EQ(keyEvent_, CameraKeyEvent::SWITCH_CAMERA);
}

/**
 * @tc.name  : MechButtonEventNotify_007
 * @tc.number: MechButtonEventNotify_007
 * @tc.desc  : Test MechButtonEventNotify with Default event.
 */
HWTEST_F(MotionManagerTest, MechButtonEventNotify_007, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<CameraInfo> cameraInfo = std::make_shared<CameraInfo>();
    cameraInfo->isCameraOn_ = true;
    McCameraTrackingController::GetInstance().currentCameraInfo_ = cameraInfo;

    std::shared_ptr<RegisterMechCameraKeyEventCmd> cmd = std::make_shared<RegisterMechCameraKeyEventCmd>();
    cmd->event_ = CameraKeyEvent::INVALID;
    motionMgr->MechButtonEventNotify(cmd);

    EXPECT_NE(keyEvent_, CameraKeyEvent::INVALID);
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
    g_mockMechState = false;

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
    g_mockMechState = true;

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
    g_mockMechState = true;

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
    g_mockMechState = true;

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
    g_mockMechState = false;

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
    g_mockMechState = true;

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
    g_mockMechState = true;

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
    g_mockMechState = false;

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
    g_mockMechState = true;

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
    g_mockMechState = false;

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
    g_mockMechState = true;

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
    g_mockMechState = false;

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
    g_mockMechState = true;

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
    g_mockMechState = false;

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
    g_mockMechState = true;

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
    g_mockMechState = false;

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
    g_mockMechState = true;

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
    g_mockMechState = false;

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
    g_mockMechState = true;

    std::shared_ptr<TrackingFrameParams> trackingFrameParams = std::make_shared<TrackingFrameParams>();
    motionMgr->deviceStatus_->isEnabled = true;

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
    g_mockMechState = false;

    bool isEnabled = false;

    int32_t result = motionMgr->GetMechCameraTrackingEnabled(isEnabled);
    EXPECT_EQ(result, ERR_OK);
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
    g_mockMechState = true;

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
    g_mockMechState = false;

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
    g_mockMechState = true;

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
    g_mockMechState = false;

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
    g_mockMechState = true;

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
    g_mockMechState = false;

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
    g_mockMechState = true;
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
    motionMgr->RegisterEventListener();
    g_mockMechState = true;
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
    g_mockMechState = false;

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
    g_mockMechState = true;
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
    g_mockMechState = true;
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
    g_mockMechState = false;

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
    g_mockMechState = true;

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
 * @tc.desc  : Test MechExecutionResultNotify with invalid parameters.
 */
HWTEST_F(MotionManagerTest, MechExecutionResultNotify_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RegisterMechControlResultCmd> cmd = nullptr;

    EXPECT_NO_FATAL_FAILURE(motionMgr->MechExecutionResultNotify(cmd));
}

/**
 * @tc.name  : MechExecutionResultNotify_002
 * @tc.number: MechExecutionResultNotify_002
 * @tc.desc  : Test MechExecutionResultNotify with valid parameters.
 */
HWTEST_F(MotionManagerTest, MechExecutionResultNotify_002, TestSize.Level1)
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

    EXPECT_NO_FATAL_FAILURE(motionMgr->MechExecutionResultNotify(cmd));

    MechNapiCommandCallbackInfo callbackInfo = {tokenId, napiCmdId};
    motionMgr->seqCallbacks_.insert(std::make_pair(tokenId, callbackInfo));
    EXPECT_NO_FATAL_FAILURE(motionMgr->MechExecutionResultNotify(cmd));

    motionMgr->seqCallbacks_.insert(std::make_pair(taskId, callbackInfo));
    EXPECT_NO_FATAL_FAILURE(motionMgr->MechExecutionResultNotify(cmd));

    cmd->controlResult_ = 2;
    EXPECT_NO_FATAL_FAILURE(motionMgr->MechExecutionResultNotify(cmd));
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

    g_mockMechState = true;
    CommandFactory factory;
    auto cmd = factory.CreateRegisterMechWheelDataCmd();

    EXPECT_NO_FATAL_FAILURE(motionMgr->MechWheelZoomNotify(cmd));

    cmd->wheelData_.degree = 10;
    EXPECT_NO_FATAL_FAILURE(motionMgr->MechWheelZoomNotify(cmd));
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
 * @tc.name  : UnRegisterNotifyEvent_001
 * @tc.number: UnRegisterNotifyEvent_001
 * @tc.desc  : Test UnRegisterNotifyEvent function.
 */
HWTEST_F(MotionManagerTest, UnRegisterNotifyEvent_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    EXPECT_NO_FATAL_FAILURE(motionMgr->UnRegisterNotifyEvent());
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

    g_mockMechState = true;
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
    limit.negMax.yaw = 1;
    limit.negMax.roll = 1;
    limit.negMax.pitch = 1;
    limit.posMax.yaw = 1;
    limit.posMax.roll = 1;
    limit.posMax.pitch = 1;
    EXPECT_NO_FATAL_FAILURE(motionMgr->JudgingYawLimit(limit));

    limit.negMax.yaw = -3.14f;
    limit.negMax.roll = -3.14f;
    limit.negMax.pitch = -3.14f;
    EXPECT_NO_FATAL_FAILURE(motionMgr->JudgingYawLimit(limit));

    limit.posMax.yaw = 3.14f;
    limit.posMax.roll = 3.14f;
    limit.posMax.pitch = 3.14f;
    limit.negMax.yaw = 1;
    limit.negMax.roll = 1;
    limit.negMax.pitch = 1;
    EXPECT_NO_FATAL_FAILURE(motionMgr->JudgingYawLimit(limit));

    limit.negMax.yaw = -3.14f;
    limit.negMax.roll = -3.14f;
    limit.negMax.pitch = -3.14f;
    EXPECT_NO_FATAL_FAILURE(motionMgr->JudgingYawLimit(limit));
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
    EulerAngles eulerAngles;

    EXPECT_NO_FATAL_FAILURE(motionMgr->AbsolutelyEulerAnglesJudgingLimitLocked(eulerAngles));
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

    motionMgr->deviceStatus_ = nullptr;
    EulerAngles eulerAngles;

    EXPECT_NO_FATAL_FAILURE(motionMgr->AbsolutelyEulerAnglesJudgingLimitLocked(eulerAngles));
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
    EXPECT_NO_FATAL_FAILURE(
        motionMgr->LimitCalculationLocked(eulerAngles, motionMgr->deviceStatus_->rotationAxesStatus, callback));

    motionMgr->deviceStatus_->rotationLimit.negMax.yaw = 1;
    motionMgr->deviceStatus_->rotationLimit.negMax.roll = 1;
    motionMgr->deviceStatus_->rotationLimit.negMax.pitch = 1;
    eulerAngles.yaw = 1;
    eulerAngles.roll = 1;
    eulerAngles.pitch = 1;
    EXPECT_NO_FATAL_FAILURE(
        motionMgr->LimitCalculationLocked(eulerAngles, motionMgr->deviceStatus_->rotationAxesStatus, callback));

    motionMgr->deviceStatus_->rollLimit = false;
    motionMgr->deviceStatus_->pitchLimit = false;
    motionMgr->deviceStatus_->yawLimit = false;
    EXPECT_NO_FATAL_FAILURE(
        motionMgr->LimitCalculationLocked(eulerAngles, motionMgr->deviceStatus_->rotationAxesStatus, callback));
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

    EXPECT_NO_FATAL_FAILURE(motionMgr->HandleMechPlacementChange(false));
}
}
}
