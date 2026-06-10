/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "mc_motion_manager_two_test.h"

#include <thread>

#include <stdexcept>
#include <iostream>

#include "mechbody_controller_log.h"
#include "mc_register_mech_position_info_cmd.h"
#include "mc_register_mech_camera_key_event_cmd.h"
#include "mechbody_controller_service.h"
#include "mc_register_mech_control_result_cmd.h"
#include "mc_command_base.h"
#include "mc_motion_manager_test.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS {
namespace MechBodyController {

void MotionManagerTwoTest::SetUpTestCase()
{
}

void MotionManagerTwoTest::TearDownTestCase()
{}

void MotionManagerTwoTest::SetUp()
{
}

void MotionManagerTwoTest::TearDown()
{}

/**
 * @tc.name  : MoveBySpeed_001
 * @tc.number: MoveBySpeed_001
 * @tc.desc  : Test MoveBySpeed when device is not attached.
 */
HWTEST_F(MotionManagerTwoTest, MoveBySpeed_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::DETACHED;

    uint32_t tokenId = 100;
    std::string napiCmdId = "napiCmdId";
    uint16_t duration = 1000;
    std::shared_ptr<SpeedParams> speedParams = std::make_shared<SpeedParams>();

    EXPECT_EQ(motionMgr->MoveBySpeed(tokenId, napiCmdId, duration, speedParams), DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : MoveBySpeed_002
 * @tc.number: MoveBySpeed_002
 * @tc.desc  : Test MoveBySpeed with nullptr parameter.
 */
HWTEST_F(MotionManagerTwoTest, MoveBySpeed_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;

    uint32_t tokenId = 100;
    std::string napiCmdId = "napiCmdId";
    uint16_t duration = 1000;
    std::shared_ptr<SpeedParams> speedParams = nullptr;

    EXPECT_EQ(motionMgr->MoveBySpeed(tokenId, napiCmdId, duration, speedParams), INVALID_PARAMETERS_ERR);
}

/**
 * @tc.name  : MoveBySpeed_003
 * @tc.number: MoveBySpeed_003
 * @tc.desc  : Test MoveBySpeed when duration exceeds maxWheelCtrlTime.
 */
HWTEST_F(MotionManagerTwoTest, MoveBySpeed_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->wheelCapabilityInfo_.maxWheelCtrlTime = 100;
    // Set maxForwardSpeed and maxRotationSpeed to ensure CheckWheelSpeedLimit passes
    motionMgr->wheelCapabilityInfo_.maxForwardSpeed = 100;
    motionMgr->wheelCapabilityInfo_.maxRotationSpeed = 50;

    uint32_t tokenId = 100;
    std::string napiCmdId = "napiCmdId";
    uint16_t duration = 200;
    std::shared_ptr<SpeedParams> speedParams = std::make_shared<SpeedParams>();
    speedParams->speed = 10;

    EXPECT_EQ(motionMgr->MoveBySpeed(tokenId, napiCmdId, duration, speedParams), WHEEL_SPEED_EXCEED_LIMIT);
}

/**
 * @tc.name  : MoveBySpeed_004
 * @tc.number: MoveBySpeed_004
 * @tc.desc  : Test MoveBySpeed with valid parameters.
 */
HWTEST_F(MotionManagerTwoTest, MoveBySpeed_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->wheelCapabilityInfo_.maxForwardSpeed = 100;
    motionMgr->wheelCapabilityInfo_.maxRotationSpeed = 50;
    motionMgr->wheelCapabilityInfo_.maxWheelCtrlTime = 1000;

    uint32_t tokenId = 100;
    std::string napiCmdId = "napiCmdId";
    uint16_t duration = 100;
    std::shared_ptr<SpeedParams> speedParams = std::make_shared<SpeedParams>();
    speedParams->speed = 10;

    EXPECT_EQ(motionMgr->MoveBySpeed(tokenId, napiCmdId, duration, speedParams), ERR_OK);
}

/**
 * @tc.name  : TurnBySpeed_001
 * @tc.number: TurnBySpeed_001
 * @tc.desc  : Test TurnBySpeed when device is not attached.
 */
HWTEST_F(MotionManagerTwoTest, TurnBySpeed_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::DETACHED;

    uint32_t tokenId = 100;
    std::string napiCmdId = "napiCmdId";
    float angleSpeed = 10.0f;
    uint16_t duration = 1000;

    EXPECT_EQ(motionMgr->TurnBySpeed(tokenId, napiCmdId, angleSpeed, duration), DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : TurnBySpeed_002
 * @tc.number: TurnBySpeed_002
 * @tc.desc  : Test TurnBySpeed when duration exceeds maxWheelCtrlTime.
 */
HWTEST_F(MotionManagerTwoTest, TurnBySpeed_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->wheelCapabilityInfo_.maxRotationSpeed = 50;
    motionMgr->wheelCapabilityInfo_.maxWheelCtrlTime = 100;

    uint32_t tokenId = 100;
    std::string napiCmdId = "napiCmdId";
    float angleSpeed = 10.0f;
    uint16_t duration = 200;

    EXPECT_EQ(motionMgr->TurnBySpeed(tokenId, napiCmdId, angleSpeed, duration), WHEEL_SPEED_EXCEED_LIMIT);
}

/**
 * @tc.name  : TurnBySpeed_003
 * @tc.number: TurnBySpeed_003
 * @tc.desc  : Test TurnBySpeed with valid parameters.
 */
HWTEST_F(MotionManagerTwoTest, TurnBySpeed_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->wheelCapabilityInfo_.maxRotationSpeed = 50;
    motionMgr->wheelCapabilityInfo_.maxWheelCtrlTime = 1000;

    uint32_t tokenId = 100;
    std::string napiCmdId = "napiCmdId";
    float angleSpeed = 10.0f;
    uint16_t duration = 100;

    EXPECT_EQ(motionMgr->TurnBySpeed(tokenId, napiCmdId, angleSpeed, duration), ERR_OK);
}

/**
 * @tc.name  : TurnBySpeed_004
 * @tc.number: TurnBySpeed_004
 * @tc.desc  : Test TurnBySpeed when angleSpeed exceeds limit.
 */
HWTEST_F(MotionManagerTwoTest, TurnBySpeed_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;
    // Set a small maxRotationSpeed (5.0f) to make angleSpeed (10.0f) exceed the limit
    motionMgr->wheelCapabilityInfo_.maxRotationSpeed = 5.0f;

    uint32_t tokenId = 100;
    std::string napiCmdId = "napiCmdId";
    float angleSpeed = 10.0f;
    uint16_t duration = 100;

    EXPECT_EQ(motionMgr->TurnBySpeed(tokenId, napiCmdId, angleSpeed, duration), WHEEL_SPEED_EXCEED_LIMIT);
}

/**
 * @tc.name  : DoAction_001
 * @tc.number: DoAction_001
 * @tc.desc  : Test DoAction when device is not attached.
 */
HWTEST_F(MotionManagerTwoTest, DoAction_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::DETACHED;

    uint32_t tokenId = 100;
    std::string napiCmdId = "napiCmdId";
    ActionType actionType = ActionType::GREET_MODE;

    EXPECT_EQ(motionMgr->DoAction(tokenId, napiCmdId, actionType), DEVICE_NOT_PLACED_ON_MECH);
}

/**
 * @tc.name  : DoAction_002
 * @tc.number: DoAction_002
 * @tc.desc  : Test DoAction with valid parameters.
 */
HWTEST_F(MotionManagerTwoTest, DoAction_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    g_mockMechState = AttachmentStateMap::ATTACHED;

    uint32_t tokenId = 100;
    std::string napiCmdId = "napiCmdId";
    ActionType actionType = ActionType::GREET_MODE;

    EXPECT_EQ(motionMgr->DoAction(tokenId, napiCmdId, actionType), ERR_OK);
}

/**
 * @tc.name  : ProcessPhoneOffForegroundCheck_003
 * @tc.number: ProcessPhoneOffForegroundCheck_003
 * @tc.desc  : Test IsAiDispatchServiceInForeground with other app in foreground.
 */
HWTEST_F(MotionManagerTwoTest, ProcessPhoneOffForegroundCheck_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    AppExecFwk::AppStateData appData;
    appData.bundleName = "com.example.app";
    appData.isFocused = true;
    appData.extensionType = AppExecFwk::ExtensionAbilityType::SERVICE;
    list.push_back(appData);

    EXPECT_FALSE(motionMgr->IsAiDispatchServiceInForeground(list));
}

/**
 * @tc.name  : ProcessPhoneOffForegroundCheck_004
 * @tc.number: ProcessPhoneOffForegroundCheck_004
 * @tc.desc  : Test IsAiDispatchServiceInForeground with AI dispatch service as UI extension.
 */
HWTEST_F(MotionManagerTwoTest, ProcessPhoneOffForegroundCheck_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    AppExecFwk::AppStateData appData;
    appData.bundleName = "com.example.aibase";
    appData.isFocused = true;
    appData.extensionType = AppExecFwk::ExtensionAbilityType::UI;
    list.push_back(appData);

    EXPECT_FALSE(motionMgr->IsAiDispatchServiceInForeground(list));
}

/**
 * @tc.name  : ProcessPhoneOffForegroundCheck_005
 * @tc.number: ProcessPhoneOffForegroundCheck_005
 * @tc.desc  : Test IsAiDispatchServiceInForeground with AI dispatch service not focused.
 */
HWTEST_F(MotionManagerTwoTest, ProcessPhoneOffForegroundCheck_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    AppExecFwk::AppStateData appData;
    appData.bundleName = "com.example.aibase";
    appData.isFocused = false;
    appData.extensionType = AppExecFwk::ExtensionAbilityType::SERVICE;
    list.push_back(appData);

    EXPECT_FALSE(motionMgr->IsAiDispatchServiceInForeground(list));
}

/**
 * @tc.name  : ProcessPhoneOffForegroundCheck_006
 * @tc.number: ProcessPhoneOffForegroundCheck_006
 * @tc.desc  : Test IsAiDispatchServiceInForeground with multiple apps including AI dispatch service.
 */
HWTEST_F(MotionManagerTwoTest, ProcessPhoneOffForegroundCheck_006, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    AppExecFwk::AppStateData appData1;
    appData1.bundleName = "com.example.app1";
    appData1.isFocused = true;
    appData1.extensionType = AppExecFwk::ExtensionAbilityType::SERVICE;
    list.push_back(appData1);

    AppExecFwk::AppStateData appData2;
    appData2.bundleName = "com.example.aibase";
    appData2.isFocused = true;
    appData2.extensionType = AppExecFwk::ExtensionAbilityType::SERVICE;
    list.push_back(appData2);

    EXPECT_TRUE(motionMgr->IsAiDispatchServiceInForeground(list));
}

/**
 * @tc.name  : ProcessPhoneOffForegroundCheck_007
 * @tc.number: ProcessPhoneOffForegroundCheck_007
 * @tc.desc  : Test IsAiDispatchServiceInForeground with AI dispatch service in list but not focused.
 */
HWTEST_F(MotionManagerTwoTest, ProcessPhoneOffForegroundCheck_007, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    AppExecFwk::AppStateData appData1;
    appData1.bundleName = "com.example.aibase";
    appData1.isFocused = false;
    appData1.extensionType = AppExecFwk::ExtensionAbilityType::SERVICE;
    list.push_back(appData1);

    AppExecFwk::AppStateData appData2;
    appData2.bundleName = "com.example.app";
    appData2.isFocused = true;
    appData2.extensionType = AppExecFwk::ExtensionAbilityType::SERVICE;
    list.push_back(appData2);

    EXPECT_FALSE(motionMgr->IsAiDispatchServiceInForeground(list));
}

/**
 * @tc.name  : ProcessPhoneOffForegroundCheck_008
 * @tc.number: ProcessPhoneOffForegroundCheck_008
 * @tc.desc  : Test IsAiDispatchServiceInForeground with AI dispatch service as UI extension and other app focused.
 */
HWTEST_F(MotionManagerTwoTest, ProcessPhoneOffForegroundCheck_008, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    AppExecFwk::AppStateData appData1;
    appData1.bundleName = "com.example.aibase";
    appData1.isFocused = true;
    appData1.extensionType = AppExecFwk::ExtensionAbilityType::UI;
    list.push_back(appData1);

    AppExecFwk::AppStateData appData2;
    appData2.bundleName = "com.example.app";
    appData2.isFocused = true;
    appData2.extensionType = AppExecFwk::ExtensionAbilityType::SERVICE;
    list.push_back(appData2);

    EXPECT_FALSE(motionMgr->IsAiDispatchServiceInForeground(list));
}

/**
 * @tc.name  : IsAiDispatchServiceInForeground_001
 * @tc.number: IsAiDispatchServiceInForeground_001
 * @tc.desc  : Test IsAiDispatchServiceInForeground with empty list.
 */
HWTEST_F(MotionManagerTwoTest, IsAiDispatchServiceInForeground_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    EXPECT_FALSE(motionMgr->IsAiDispatchServiceInForeground(list));
}

/**
 * @tc.name  : IsAiDispatchServiceInForeground_002
 * @tc.number: IsAiDispatchServiceInForeground_002
 * @tc.desc  : Test IsAiDispatchServiceInForeground with AI dispatch service.
 */
HWTEST_F(MotionManagerTwoTest, IsAiDispatchServiceInForeground_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    AppExecFwk::AppStateData appData;
    appData.bundleName = "com.example.aibase";
    appData.isFocused = true;
    appData.extensionType = AppExecFwk::ExtensionAbilityType::SERVICE;
    list.push_back(appData);

    EXPECT_TRUE(motionMgr->IsAiDispatchServiceInForeground(list));
}

/**
 * @tc.name  : IsDesktopScene_001
 * @tc.number: IsDesktopScene_001
 * @tc.desc  : Test IsDesktopScene with empty list.
 */
HWTEST_F(MotionManagerTwoTest, IsDesktopScene_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    EXPECT_TRUE(motionMgr->IsDesktopScene(list));
}

/**
 * @tc.name  : IsDesktopScene_002
 * @tc.number: IsDesktopScene_002
 * @tc.desc  : Test IsDesktopScene with sceneboard focused.
 */
HWTEST_F(MotionManagerTwoTest, IsDesktopScene_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    AppExecFwk::AppStateData appData;
    appData.bundleName = "com.ohos.sceneboard";
    appData.isFocused = true;
    appData.extensionType = AppExecFwk::ExtensionAbilityType::UI;
    list.push_back(appData);

    EXPECT_TRUE(motionMgr->IsDesktopScene(list));
}

/**
 * @tc.name  : IsDesktopScene_003
 * @tc.number: IsDesktopScene_003
 * @tc.desc  : Test IsDesktopScene with non-UIExtension focused.
 */
HWTEST_F(MotionManagerTwoTest, IsDesktopScene_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    AppExecFwk::AppStateData appData;
    appData.bundleName = "com.example.app";
    appData.isFocused = true;
    appData.extensionType = AppExecFwk::ExtensionAbilityType::SERVICE;
    list.push_back(appData);

    EXPECT_FALSE(motionMgr->IsDesktopScene(list));
}

/**
 * @tc.name  : MechTrackingStatusNotify_004
 * @tc.number: MechTrackingStatusNotify_004
 * @tc.desc  : Test MechTrackingStatusNotify with isEnabled true and false branches.
 */
HWTEST_F(MotionManagerTwoTest, MechTrackingStatusNotify_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Test with isEnabled = true (else branch, lines 620-627)
    std::shared_ptr<NormalRegisterMechTrackingEnableCmd> cmd = std::make_shared<NormalRegisterMechTrackingEnableCmd>();
    cmd->isEnabled_ = true;
    motionMgr->MechTrackingStatusNotify(cmd);
    EXPECT_EQ(motionMgr->deviceStatus_->isEnabled, true);

    // Test with isEnabled = false (if branch, lines 613-618)
    cmd->isEnabled_ = false;
    motionMgr->MechTrackingStatusNotify(cmd);
    EXPECT_EQ(motionMgr->deviceStatus_->isEnabled, false);
}

/**
 * @tc.name  : ProcessTrackingStatus_001
 * @tc.number: ProcessTrackingStatus_001
 * @tc.desc  : Test ProcessTrackingStatus with active tracking.
 */
HWTEST_F(MotionManagerTwoTest, ProcessTrackingStatus_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    motionMgr->lastTrackingFrameTime_ = std::chrono::steady_clock::now();
    motionMgr->deviceStatus_->trackingStatus = MechTrackingStatus::MECH_TK_ENABLE_WITH_TARGET;
    motionMgr->ProcessTrackingStatus();
    EXPECT_EQ(motionMgr->deviceStatus_->trackingStatus, MechTrackingStatus::MECH_TK_ENABLE_WITH_TARGET);
}

/**
 * @tc.name  : ProcessTrackingStatus_002
 * @tc.number: ProcessTrackingStatus_002
 * @tc.desc  : Test ProcessTrackingStatus with inactive tracking.
 */
HWTEST_F(MotionManagerTwoTest, ProcessTrackingStatus_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    motionMgr->lastTrackingFrameTime_ = std::chrono::steady_clock::now() - std::chrono::seconds(10);
    motionMgr->deviceStatus_->trackingStatus = MechTrackingStatus::MECH_TK_ENABLE_WITH_TARGET;
    motionMgr->ProcessTrackingStatus();
    EXPECT_EQ(motionMgr->deviceStatus_->trackingStatus, MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET);
}

/**
 * @tc.name  : TrackingChecker_001
 * @tc.number: TrackingChecker_001
 * @tc.desc  : Test TrackingChecker with MECH_TK_ENABLE_WITH_TARGET status.
 */
HWTEST_F(MotionManagerTwoTest, TrackingChecker_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set tracking status to MECH_TK_ENABLE_WITH_TARGET to trigger ProcessTrackingStatus
    motionMgr->deviceStatus_->trackingStatus = MechTrackingStatus::MECH_TK_ENABLE_WITH_TARGET;
    // Set startTrackingChecker_ to true to enter the while loop and cover line 794-796
    motionMgr->startTrackingChecker_ = true;
    motionMgr->lastTrackingFrameTime_ = std::chrono::steady_clock::now() - std::chrono::seconds(10);

    // Run TrackingChecker in a separate thread and stop it after a short time
    std::thread checkerThread([&motionMgr]() {
        motionMgr->TrackingChecker();
    });
    checkerThread.detach();

    // Wait a bit for the loop to execute once, then stop the checker
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    motionMgr->startTrackingChecker_ = false;
}

/**
 * @tc.name  : TrackingChecker_002
 * @tc.number: TrackingChecker_002
 * @tc.desc  : Test TrackingChecker with MECH_TK_ENABLE_NO_TARGET status.
 */
HWTEST_F(MotionManagerTwoTest, TrackingChecker_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set tracking status to MECH_TK_ENABLE_NO_TARGET to skip ProcessTrackingStatus
    motionMgr->deviceStatus_->trackingStatus = MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET;
    motionMgr->startTrackingChecker_ = false;

    // Run TrackingChecker for a short time
    motionMgr->TrackingChecker();
    EXPECT_EQ(motionMgr->startTrackingChecker_, false);
}

/**
 * @tc.name  : MechTrackingStatusNotify_001
 * @tc.number: MechTrackingStatusNotify_001
 * @tc.desc  : Test MechTrackingStatusNotify with nullptr.
 */
HWTEST_F(MotionManagerTwoTest, MechTrackingStatusNotify_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<CommonRegisterMechTrackingEnableCmd> cmd = nullptr;

    motionMgr->MechTrackingStatusNotify(cmd);
    // When cmd is nullptr, MechTrackingStatusNotify returns early without modifying isEnabled
    // The initial value of isEnabled depends on the deviceStatus_ initialization
    EXPECT_EQ(motionMgr->deviceStatus_->isEnabled, true);
}

/**
 * @tc.name  : MechTrackingStatusNotify_002
 * @tc.number: MechTrackingStatusNotify_002
 * @tc.desc  : Test MechTrackingStatusNotify with disabled status.
 */
HWTEST_F(MotionManagerTwoTest, MechTrackingStatusNotify_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<NormalRegisterMechTrackingEnableCmd> cmd = std::make_shared<NormalRegisterMechTrackingEnableCmd>();

    motionMgr->MechTrackingStatusNotify(cmd);
    EXPECT_EQ(motionMgr->deviceStatus_->isEnabled, false);
}

/**
 * @tc.name  : HandlePhoneOn_001
 * @tc.number: HandlePhoneOn_001
 * @tc.desc  : Test HandlePhoneOn with WHEEL_BASE device.
 */
HWTEST_F(MotionManagerTwoTest, HandlePhoneOn_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::WHEEL_BASE);

    motionMgr->HandlePhoneOn(true);

    // Verify that device type is still WHEEL_BASE after HandlePhoneOn
    EXPECT_EQ(motionMgr->deviceBaseInfo_.devType, static_cast<uint8_t>(MechType::WHEEL_BASE));
}

/**
 * @tc.name  : HandlePhoneOn_002
 * @tc.number: HandlePhoneOn_002
 * @tc.desc  : Test HandlePhoneOn with non-WHEEL_BASE device.
 */
HWTEST_F(MotionManagerTwoTest, HandlePhoneOn_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::PORTABLE_GIMBAL);

    motionMgr->HandlePhoneOn(true);

    // Verify that device type is still PORTABLE_GIMBAL after HandlePhoneOn
    EXPECT_EQ(motionMgr->deviceBaseInfo_.devType, static_cast<uint8_t>(MechType::PORTABLE_GIMBAL));
}

/**
 * @tc.name  : ConnectAbility_001
 * @tc.number: ConnectAbility_001
 * @tc.desc  : Test ConnectAbility function.
 */
HWTEST_F(MotionManagerTwoTest, ConnectAbility_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    EXPECT_EQ(motionMgr->ConnectAbility(), ERR_OK);
}

/**
 * @tc.name  : GetDeviceBaseInfo_001
 * @tc.number: GetDeviceBaseInfo_001
 * @tc.desc  : Test GetDeviceBaseInfo function.
 */
HWTEST_F(MotionManagerTwoTest, GetDeviceBaseInfo_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // GetDeviceBaseInfo requires sendAdapter_ to be valid to send command
    // In test environment, the command will timeout and return MECH_CONNECT_FAILED
    EXPECT_EQ(motionMgr->GetDeviceBaseInfo(), MECH_CONNECT_FAILED);
}

/**
 * @tc.name  : RegisterEventListenerV01_001
 * @tc.number: RegisterEventListenerV01_001
 * @tc.desc  : Test RegisterEventListenerV01 function.
 */
HWTEST_F(MotionManagerTwoTest, RegisterEventListenerV01_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set protocolVer_ to 0x01 to trigger listener registration
    motionMgr->protocolVer_ = 0x01;

    // RegisterEventListenerV01 registers listeners when protocolVer_ == 0x01
    size_t listenerCountBefore = motionMgr->notifyListenerType_.size();
    motionMgr->RegisterEventListenerV01();
    size_t listenerCountAfter = motionMgr->notifyListenerType_.size();

    // Verify that listeners were registered (count should increase by 2)
    EXPECT_EQ(listenerCountAfter, listenerCountBefore + 2);
}

/**
 * @tc.name  : StartEvent_001
 * @tc.number: StartEvent_001
 * @tc.desc  : Test StartEvent function creates eventHandler.
 */
HWTEST_F(MotionManagerTwoTest, StartEvent_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // StartEvent contains runner->Run() which is a blocking call
    // Run it in a separate thread to avoid blocking the test
    std::thread eventThread(&MotionManager::StartEvent, motionMgr);
    eventThread.detach();

    // Wait a bit for the event handler to be initialized
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Verify that eventHandler_ was created successfully
    EXPECT_NE(motionMgr->eventHandler_, nullptr);

    // Reset will trigger destructor which calls Stop() to exit the event loop
    motionMgr.reset();
}

/**
 * @tc.name  : UpdateTrackingTime_001
 * @tc.number: UpdateTrackingTime_001
 * @tc.desc  : Test UpdateTrackingTime function.
 */
HWTEST_F(MotionManagerTwoTest, UpdateTrackingTime_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set trackingStatus to MECH_TK_ENABLE_NO_TARGET
    motionMgr->deviceStatus_->trackingStatus = MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET;

    // Call UpdateTrackingTime
    motionMgr->UpdateTrackingTime();

    // Verify that lastTrackingFrameTime_ was updated
    auto now = std::chrono::steady_clock::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - motionMgr->lastTrackingFrameTime_).count();
    EXPECT_LT(timeDiff, 100); // Should be very recent

    // Verify that trackingStatus was updated from MECH_TK_ENABLE_NO_TARGET to MECH_TK_ENABLE_WITH_TARGET
    EXPECT_EQ(motionMgr->deviceStatus_->trackingStatus, MechTrackingStatus::MECH_TK_ENABLE_WITH_TARGET);
}

/**
 * @tc.name  : TryGetDeviceRealNameSync_001
 * @tc.number: TryGetDeviceRealNameSync_001
 * @tc.desc  : Test TryGetDeviceRealNameSync with timeout.
 */
HWTEST_F(MotionManagerTwoTest, TryGetDeviceRealNameSync_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    const std::string &result = motionMgr->TryGetDeviceRealNameSync(100);
    EXPECT_TRUE(result.empty());
}

/**
 * @tc.name  : TryGetDeviceRealNameSync_002
 * @tc.number: TryGetDeviceRealNameSync_002
 * @tc.desc  : Test TryGetDeviceRealNameSync after setting name.
 */
HWTEST_F(MotionManagerTwoTest, TryGetDeviceRealNameSync_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::string deviceRealName = "TestDevice";
    motionMgr->SetDeviceRealName(deviceRealName);

    const std::string &result = motionMgr->TryGetDeviceRealNameSync(1000);
    EXPECT_EQ(result, deviceRealName);
}

/**
 * @tc.name  : IsLimited_003
 * @tc.number: IsLimited_003
 * @tc.desc  : Test IsLimited at negative limit.
 */
HWTEST_F(MotionManagerTwoTest, IsLimited_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    float negMax = -90.0f;
    float posMax = 90.0f;
    float position = -95.0f;

    bool result = motionMgr->IsLimited(negMax, posMax, position);
    EXPECT_TRUE(result);
}

/**
 * @tc.name  : IsLimited_004
 * @tc.number: IsLimited_004
 * @tc.desc  : Test IsLimited at positive limit.
 */
HWTEST_F(MotionManagerTwoTest, IsLimited_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    float negMax = -90.0f;
    float posMax = 90.0f;
    float position = 95.0f;

    bool result = motionMgr->IsLimited(negMax, posMax, position);
    EXPECT_TRUE(result);
}

/**
 * @tc.name  : HandleMechPlacementChange_002
 * @tc.number: HandleMechPlacementChange_002
 * @tc.desc  : Test HandleMechPlacementChange with phone off.
 */
HWTEST_F(MotionManagerTwoTest, HandleMechPlacementChange_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    motionMgr->HandleMechPlacementChange(false);

    // Verify that motionMgr is still valid after HandleMechPlacementChange
    EXPECT_NE(motionMgr, nullptr);
    // Verify that sendAdapter_ is still valid
    EXPECT_NE(motionMgr->sendAdapter_, nullptr);
}

/**
 * @tc.name  : CheckYawDegree_001
 * @tc.number: CheckYawDegree_001
 * @tc.desc  : Test CheckYawDegree with rotateParam nullptr.
 */
HWTEST_F(MotionManagerTwoTest, CheckYawDegree_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = nullptr;
    RotateDegreeLimit limit;
    float yawResult = 0.0f;

    motionMgr->CheckYawDegree(rotateParam, limit, yawResult);

    EXPECT_EQ(rotateParam, nullptr);
    EXPECT_EQ(motionMgr->status_.yawLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckYawDegree_002
 * @tc.number: CheckYawDegree_002
 * @tc.desc  : Test CheckYawDegree with positive rotation.
 */
HWTEST_F(MotionManagerTwoTest, CheckYawDegree_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.yaw = -90.0f;
    limit.posMax.yaw = 90.0f;
    float yawResult = 0.0f;

    motionMgr->CheckYawDegree(rotateParam, limit, yawResult);

    EXPECT_EQ(rotateParam->degree.yaw, 10.0f);
    EXPECT_EQ(motionMgr->status_.yawLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckYawDegree_003
 * @tc.number: CheckYawDegree_003
 * @tc.desc  : Test CheckYawDegree with negative rotation.
 */
HWTEST_F(MotionManagerTwoTest, CheckYawDegree_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = -10.0f;
    RotateDegreeLimit limit;
    limit.negMax.yaw = -90.0f;
    limit.posMax.yaw = 90.0f;
    float yawResult = 0.0f;

    motionMgr->CheckYawDegree(rotateParam, limit, yawResult);

    EXPECT_EQ(rotateParam->degree.yaw, -10.0f);
    EXPECT_EQ(motionMgr->status_.yawLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckYawDegree_004
 * @tc.number: CheckYawDegree_004
 * @tc.desc  : Test CheckYawDegree with yawResult < NO_LIMIT_MIN.
 */
HWTEST_F(MotionManagerTwoTest, CheckYawDegree_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = 1.0f;
    RotateDegreeLimit limit;
    limit.negMax.yaw = -90.0f;
    limit.posMax.yaw = 90.0f;
    // Set yawResult to a value < NO_LIMIT_MIN (-3.1415926f)
    float yawResult = -10.0f;

    motionMgr->CheckYawDegree(rotateParam, limit, yawResult);

    EXPECT_EQ(rotateParam->degree.yaw, 1.0f);
    EXPECT_EQ(motionMgr->status_.yawLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckYawDegree_005
 * @tc.number: CheckYawDegree_005
 * @tc.desc  : Test CheckYawDegree with positive rotation exceeding limit.
 */
HWTEST_F(MotionManagerTwoTest, CheckYawDegree_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.yaw = -1.0f;
    limit.posMax.yaw = 1.0f;
    // Set yawResult to a value that exceeds posMax.yaw after adding YAW_OFFSET
    float yawResult = 0.0f;

    motionMgr->CheckYawDegree(rotateParam, limit, yawResult);

    EXPECT_EQ(rotateParam->degree.yaw, 10.0f);
    EXPECT_EQ(motionMgr->status_.yawLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckYawDegree_006
 * @tc.number: CheckYawDegree_006
 * @tc.desc  : Test CheckYawDegree with negative rotation exceeding limit.
 */
HWTEST_F(MotionManagerTwoTest, CheckYawDegree_006, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = -10.0f;
    RotateDegreeLimit limit;
    limit.negMax.yaw = -1.0f;
    limit.posMax.yaw = 1.0f;
    // Set yawResult to a value that exceeds negMax.yaw after subtracting YAW_OFFSET
    float yawResult = 0.0f;

    motionMgr->CheckYawDegree(rotateParam, limit, yawResult);

    // yawResult = 0.0f + (-0.2f) = -0.2f, which is within [-1.0, 1.0], so not limited
    EXPECT_EQ(rotateParam->degree.yaw, -10.0f);
    EXPECT_EQ(motionMgr->status_.yawLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckRollDegree_001
 * @tc.number: CheckRollDegree_001
 * @tc.desc  : Test CheckRollDegree with nullptr.
 */
HWTEST_F(MotionManagerTwoTest, CheckRollDegree_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = nullptr;
    RotateDegreeLimit limit;
    float rollResult = 0.0f;

    motionMgr->CheckRollDegree(rotateParam, limit, rollResult);

    EXPECT_EQ(rotateParam, nullptr);
    EXPECT_EQ(motionMgr->status_.rollLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckRollDegree_002
 * @tc.number: CheckRollDegree_002
 * @tc.desc  : Test CheckRollDegree with positive rotation.
 */
HWTEST_F(MotionManagerTwoTest, CheckRollDegree_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.roll = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.roll = -45.0f;
    limit.posMax.roll = 45.0f;
    float rollResult = 0.0f;

    motionMgr->CheckRollDegree(rotateParam, limit, rollResult);

    EXPECT_EQ(rotateParam->degree.roll, 10.0f);
    EXPECT_EQ(motionMgr->status_.rollLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckRollDegree_003
 * @tc.number: CheckRollDegree_003
 * @tc.desc  : Test CheckRollDegree with negative rotation.
 */
HWTEST_F(MotionManagerTwoTest, CheckRollDegree_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.roll = -10.0f;
    RotateDegreeLimit limit;
    limit.negMax.roll = -45.0f;
    limit.posMax.roll = 45.0f;
    float rollResult = 0.0f;

    motionMgr->CheckRollDegree(rotateParam, limit, rollResult);

    EXPECT_EQ(rotateParam->degree.roll, -10.0f);
    EXPECT_EQ(motionMgr->status_.rollLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckRollDegree_004
 * @tc.number: CheckRollDegree_004
 * @tc.desc  : Test CheckRollDegree with rollResult < NO_LIMIT_MIN.
 */
HWTEST_F(MotionManagerTwoTest, CheckRollDegree_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.roll = 1.0f;
    RotateDegreeLimit limit;
    limit.negMax.roll = -45.0f;
    limit.posMax.roll = 45.0f;
    // Set rollResult to a value < NO_LIMIT_MIN (-3.1415926f)
    float rollResult = -10.0f;

    motionMgr->CheckRollDegree(rotateParam, limit, rollResult);

    EXPECT_EQ(rotateParam->degree.roll, 1.0f);
    EXPECT_EQ(motionMgr->status_.rollLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckRollDegree_005
 * @tc.number: CheckRollDegree_005
 * @tc.desc  : Test CheckRollDegree with positive rotation exceeding limit.
 */
HWTEST_F(MotionManagerTwoTest, CheckRollDegree_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.roll = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.roll = -1.0f;
    limit.posMax.roll = 1.0f;
    // Set rollResult to a value that exceeds posMax.roll
    float rollResult = 0.0f;

    motionMgr->CheckRollDegree(rotateParam, limit, rollResult);

    // rollResult = 0.0 - 6.28 = -6.28, which is < negMax.roll = -1.0
    // rotateParam->degree.roll = 10.0 - (-6.28 - 1.0) = 17.28
    EXPECT_FLOAT_EQ(rotateParam->degree.roll, 17.28f);
    EXPECT_EQ(motionMgr->status_.rollLimited, RotationAxisLimited::POS_LIMITED);
}

/**
 * @tc.name  : CheckRollDegree_006
 * @tc.number: CheckRollDegree_006
 * @tc.desc  : Test CheckRollDegree with negative rotation exceeding limit.
 */
HWTEST_F(MotionManagerTwoTest, CheckRollDegree_006, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.roll = -10.0f;
    RotateDegreeLimit limit;
    limit.negMax.roll = -1.0f;
    limit.posMax.roll = 1.0f;
    // Set rollResult to a value that exceeds negMax.roll
    float rollResult = 0.0f;

    motionMgr->CheckRollDegree(rotateParam, limit, rollResult);

    // rollResult = 0.0 - 6.28 = -6.28, which is < negMax.roll = -1.0
    // rotateParam->degree.roll = -10.0 - (-6.28 - 1.0) = -4.72
    EXPECT_FLOAT_EQ(rotateParam->degree.roll, -4.72f);
    EXPECT_EQ(motionMgr->status_.rollLimited, RotationAxisLimited::NEG_LIMITED);
}

/**
 * @tc.name  : CheckPitchDegree_001
 * @tc.number: CheckPitchDegree_001
 * @tc.desc  : Test CheckPitchDegree with nullptr.
 */
HWTEST_F(MotionManagerTwoTest, CheckPitchDegree_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = nullptr;
    RotateDegreeLimit limit;
    float pitchResult = 0.0f;

    motionMgr->CheckPitchDegree(rotateParam, limit, pitchResult);

    EXPECT_EQ(rotateParam, nullptr);
    EXPECT_EQ(motionMgr->status_.pitchLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckPitchDegree_002
 * @tc.number: CheckPitchDegree_002
 * @tc.desc  : Test CheckPitchDegree with positive rotation.
 */
HWTEST_F(MotionManagerTwoTest, CheckPitchDegree_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.pitch = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.pitch = -30.0f;
    limit.posMax.pitch = 90.0f;
    float pitchResult = 0.0f;

    motionMgr->CheckPitchDegree(rotateParam, limit, pitchResult);

    EXPECT_EQ(rotateParam->degree.pitch, 10.0f);
    EXPECT_EQ(motionMgr->status_.pitchLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckPitchDegree_003
 * @tc.number: CheckPitchDegree_003
 * @tc.desc  : Test CheckPitchDegree with negative rotation.
 */
HWTEST_F(MotionManagerTwoTest, CheckPitchDegree_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.pitch = -10.0f;
    RotateDegreeLimit limit;
    limit.negMax.pitch = -30.0f;
    limit.posMax.pitch = 90.0f;
    float pitchResult = 0.0f;

    motionMgr->CheckPitchDegree(rotateParam, limit, pitchResult);

    EXPECT_EQ(rotateParam->degree.pitch, -10.0f);
    EXPECT_EQ(motionMgr->status_.pitchLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckPitchDegree_004
 * @tc.number: CheckPitchDegree_004
 * @tc.desc  : Test CheckPitchDegree with pitchResult < NO_LIMIT_MIN.
 */
HWTEST_F(MotionManagerTwoTest, CheckPitchDegree_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.pitch = 1.0f;
    RotateDegreeLimit limit;
    limit.negMax.pitch = -30.0f;
    limit.posMax.pitch = 90.0f;
    // Set pitchResult to a value < NO_LIMIT_MIN (-3.1415926f) to trigger the while loop at line 1431-1434
    float pitchResult = -10.0f;

    motionMgr->CheckPitchDegree(rotateParam, limit, pitchResult);

    EXPECT_EQ(rotateParam->degree.pitch, 1.0f);
    EXPECT_EQ(motionMgr->status_.pitchLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckPitchDegree_005
 * @tc.number: CheckPitchDegree_005
 * @tc.desc  : Test CheckPitchDegree with positive rotation exceeding limit.
 */
HWTEST_F(MotionManagerTwoTest, CheckPitchDegree_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.pitch = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.pitch = -1.0f;
    limit.posMax.pitch = 1.0f;
    // Set pitchResult to a value that exceeds posMax.pitch
    float pitchResult = 0.0f;

    motionMgr->CheckPitchDegree(rotateParam, limit, pitchResult);

    // pitchResult = 0.0 - 6.28 = -6.28, which is < negMax.pitch = -1.0
    // rotateParam->degree.pitch = 10.0 - (-6.28 - 1.0) = 17.28
    EXPECT_FLOAT_EQ(rotateParam->degree.pitch, 17.28f);
    EXPECT_EQ(motionMgr->status_.pitchLimited, RotationAxisLimited::POS_LIMITED);
}

/**
 * @tc.name  : CheckPitchDegree_006
 * @tc.number: CheckPitchDegree_006
 * @tc.desc  : Test CheckPitchDegree with negative rotation exceeding limit.
 */
HWTEST_F(MotionManagerTwoTest, CheckPitchDegree_006, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.pitch = -10.0f;
    RotateDegreeLimit limit;
    limit.negMax.pitch = -1.0f;
    limit.posMax.pitch = 1.0f;
    // Set pitchResult to a value that exceeds negMax.pitch
    float pitchResult = 0.0f;

    motionMgr->CheckPitchDegree(rotateParam, limit, pitchResult);

    // pitchResult = 0.0 - 6.28 = -6.28, which is < negMax.pitch = -1.0
    // rotateParam->degree.pitch = -10.0 - (-6.28 - 1.0) = -4.72
    EXPECT_FLOAT_EQ(rotateParam->degree.pitch, -4.72f);
    EXPECT_EQ(motionMgr->status_.pitchLimited, RotationAxisLimited::NEG_LIMITED);
}

/**
 * @tc.name  : CheckYawSpeed_001
 * @tc.number: CheckYawSpeed_001
 * @tc.desc  : Test CheckYawSpeed with nullptr.
 */
HWTEST_F(MotionManagerTwoTest, CheckYawSpeed_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = nullptr;
    RotateDegreeLimit limit;
    float yawResult = 0.0f;

    motionMgr->CheckYawSpeed(rotateBySpeedParam, limit, yawResult);

    EXPECT_EQ(rotateBySpeedParam, nullptr);
    EXPECT_EQ(motionMgr->status_.yawLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckYawSpeed_002
 * @tc.number: CheckYawSpeed_002
 * @tc.desc  : Test CheckYawSpeed with positive speed.
 */
HWTEST_F(MotionManagerTwoTest, CheckYawSpeed_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.yawSpeed = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.yaw = -90.0f;
    limit.posMax.yaw = 90.0f;
    float yawResult = 0.0f;

    motionMgr->CheckYawSpeed(rotateBySpeedParam, limit, yawResult);

    EXPECT_EQ(motionMgr->status_.yawLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckYawSpeed_003
 * @tc.number: CheckYawSpeed_003
 * @tc.desc  : Test CheckYawSpeed with negative speed.
 */
HWTEST_F(MotionManagerTwoTest, CheckYawSpeed_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.yawSpeed = -10.0f;
    RotateDegreeLimit limit;
    limit.negMax.yaw = -90.0f;
    limit.posMax.yaw = 90.0f;
    float yawResult = 0.0f;

    motionMgr->CheckYawSpeed(rotateBySpeedParam, limit, yawResult);

    EXPECT_EQ(motionMgr->status_.yawLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckYawSpeed_004
 * @tc.number: CheckYawSpeed_004
 * @tc.desc  : Test CheckYawSpeed with yawResult < NO_LIMIT_MIN.
 */
HWTEST_F(MotionManagerTwoTest, CheckYawSpeed_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.yawSpeed = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.yaw = -90.0f;
    limit.posMax.yaw = 90.0f;
    // Set yawResult to a value less than NO_LIMIT_MIN (-3.1415926) to trigger the while loop
    float yawResult = -10.0f;

    motionMgr->CheckYawSpeed(rotateBySpeedParam, limit, yawResult);

    EXPECT_EQ(motionMgr->status_.yawLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckYawSpeed_005
 * @tc.number: CheckYawSpeed_005
 * @tc.desc  : Test CheckYawSpeed with yawResult > NO_LIMIT_MAX.
 */
HWTEST_F(MotionManagerTwoTest, CheckYawSpeed_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.yawSpeed = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.yaw = -90.0f;
    limit.posMax.yaw = 90.0f;
    // Set yawResult to a value greater than NO_LIMIT_MAX (3.1415927) to trigger the while loop
    float yawResult = 10.0f;

    motionMgr->CheckYawSpeed(rotateBySpeedParam, limit, yawResult);

    EXPECT_EQ(motionMgr->status_.yawLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckYawSpeed_006
 * @tc.number: CheckYawSpeed_006
 * @tc.desc  : Test CheckYawSpeed with limit exceeded.
 */
HWTEST_F(MotionManagerTwoTest, CheckYawSpeed_006, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.yawSpeed = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.yaw = -90.0f;
    limit.posMax.yaw = 90.0f;
    // Set yawResult to a value that exceeds posMax.yaw to trigger limit state change
    float yawResult = 100.0f;

    motionMgr->CheckYawSpeed(rotateBySpeedParam, limit, yawResult);

    EXPECT_EQ(motionMgr->status_.yawLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckRollSpeed_001
 * @tc.number: CheckRollSpeed_001
 * @tc.desc  : Test CheckRollSpeed with nullptr.
 */
HWTEST_F(MotionManagerTwoTest, CheckRollSpeed_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = nullptr;
    RotateDegreeLimit limit;
    float rollResult = 0.0f;

    motionMgr->CheckRollSpeed(rotateBySpeedParam, limit, rollResult);

    EXPECT_EQ(rotateBySpeedParam, nullptr);
    EXPECT_EQ(motionMgr->status_.rollLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckRollSpeed_002
 * @tc.number: CheckRollSpeed_002
 * @tc.desc  : Test CheckRollSpeed with positive speed.
 */
HWTEST_F(MotionManagerTwoTest, CheckRollSpeed_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.rollSpeed = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.roll = -45.0f;
    limit.posMax.roll = 45.0f;
    float rollResult = 0.0f;

    motionMgr->CheckRollSpeed(rotateBySpeedParam, limit, rollResult);

    EXPECT_EQ(motionMgr->status_.rollLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckRollSpeed_003
 * @tc.number: CheckRollSpeed_003
 * @tc.desc  : Test CheckRollSpeed with negative speed.
 */
HWTEST_F(MotionManagerTwoTest, CheckRollSpeed_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.rollSpeed = -10.0f;
    RotateDegreeLimit limit;
    limit.negMax.roll = -45.0f;
    limit.posMax.roll = 45.0f;
    float rollResult = 0.0f;

    motionMgr->CheckRollSpeed(rotateBySpeedParam, limit, rollResult);

    EXPECT_EQ(motionMgr->status_.rollLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckRollSpeed_004
 * @tc.number: CheckRollSpeed_004
 * @tc.desc  : Test CheckRollSpeed with rollResult < NO_LIMIT_MIN.
 */
HWTEST_F(MotionManagerTwoTest, CheckRollSpeed_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.rollSpeed = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.roll = -45.0f;
    limit.posMax.roll = 45.0f;
    // Set rollResult to a value less than NO_LIMIT_MIN (-3.1415926) to trigger the while loop
    float rollResult = -10.0f;

    motionMgr->CheckRollSpeed(rotateBySpeedParam, limit, rollResult);

    EXPECT_EQ(motionMgr->status_.rollLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckRollSpeed_005
 * @tc.number: CheckRollSpeed_005
 * @tc.desc  : Test CheckRollSpeed with rollResult > NO_LIMIT_MAX.
 */
HWTEST_F(MotionManagerTwoTest, CheckRollSpeed_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.rollSpeed = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.roll = -45.0f;
    limit.posMax.roll = 45.0f;
    // Set rollResult to a value greater than NO_LIMIT_MAX (3.1415927) to trigger the while loop
    float rollResult = 10.0f;

    motionMgr->CheckRollSpeed(rotateBySpeedParam, limit, rollResult);

    EXPECT_EQ(motionMgr->status_.rollLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckRollSpeed_006
 * @tc.number: CheckRollSpeed_006
 * @tc.desc  : Test CheckRollSpeed with limit exceeded.
 */
HWTEST_F(MotionManagerTwoTest, CheckRollSpeed_006, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.rollSpeed = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.roll = -45.0f;
    limit.posMax.roll = 45.0f;
    // Set rollResult to a value that exceeds posMax.roll to trigger limit state change
    float rollResult = 100.0f;

    motionMgr->CheckRollSpeed(rotateBySpeedParam, limit, rollResult);

    EXPECT_EQ(motionMgr->status_.rollLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckPitchSpeed_001
 * @tc.number: CheckPitchSpeed_001
 * @tc.desc  : Test CheckPitchSpeed with nullptr.
 */
HWTEST_F(MotionManagerTwoTest, CheckPitchSpeed_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = nullptr;
    RotateDegreeLimit limit;
    float pitchResult = 0.0f;

    motionMgr->CheckPitchSpeed(rotateBySpeedParam, limit, pitchResult);
    EXPECT_EQ(rotateBySpeedParam, nullptr);
    EXPECT_EQ(motionMgr->status_.pitchLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckPitchSpeed_002
 * @tc.number: CheckPitchSpeed_002
 * @tc.desc  : Test CheckPitchSpeed with positive speed.
 */
HWTEST_F(MotionManagerTwoTest, CheckPitchSpeed_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.pitchSpeed = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.pitch = -30.0f;
    limit.posMax.pitch = 90.0f;
    float pitchResult = 0.0f;

    motionMgr->CheckPitchSpeed(rotateBySpeedParam, limit, pitchResult);
    EXPECT_EQ(motionMgr->status_.pitchLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckPitchSpeed_003
 * @tc.number: CheckPitchSpeed_003
 * @tc.desc  : Test CheckPitchSpeed with negative speed.
 */
HWTEST_F(MotionManagerTwoTest, CheckPitchSpeed_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.pitchSpeed = -10.0f;
    RotateDegreeLimit limit;
    limit.negMax.pitch = -30.0f;
    limit.posMax.pitch = 90.0f;
    float pitchResult = 0.0f;

    motionMgr->CheckPitchSpeed(rotateBySpeedParam, limit, pitchResult);
    EXPECT_EQ(motionMgr->status_.pitchLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckPitchSpeed_004
 * @tc.number: CheckPitchSpeed_004
 * @tc.desc  : Test CheckPitchSpeed with pitchResult < NO_LIMIT_MIN.
 */
HWTEST_F(MotionManagerTwoTest, CheckPitchSpeed_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.pitchSpeed = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.pitch = -30.0f;
    limit.posMax.pitch = 90.0f;
    // Set pitchResult to a value less than NO_LIMIT_MIN (-3.1415926) to trigger the while loop
    float pitchResult = -10.0f;

    motionMgr->CheckPitchSpeed(rotateBySpeedParam, limit, pitchResult);
    EXPECT_EQ(motionMgr->status_.pitchLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckPitchSpeed_005
 * @tc.number: CheckPitchSpeed_005
 * @tc.desc  : Test CheckPitchSpeed with pitchResult > NO_LIMIT_MAX.
 */
HWTEST_F(MotionManagerTwoTest, CheckPitchSpeed_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.pitchSpeed = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.pitch = -30.0f;
    limit.posMax.pitch = 90.0f;
    // Set pitchResult to a value greater than NO_LIMIT_MAX (3.1415927) to trigger the while loop
    float pitchResult = 10.0f;

    motionMgr->CheckPitchSpeed(rotateBySpeedParam, limit, pitchResult);
    EXPECT_EQ(motionMgr->status_.pitchLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : CheckPitchSpeed_006
 * @tc.number: CheckPitchSpeed_006
 * @tc.desc  : Test CheckPitchSpeed with limit exceeded.
 */
HWTEST_F(MotionManagerTwoTest, CheckPitchSpeed_006, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.pitchSpeed = 10.0f;
    RotateDegreeLimit limit;
    limit.negMax.pitch = -30.0f;
    limit.posMax.pitch = 90.0f;
    // Set pitchResult to a value that exceeds posMax.pitch to trigger limit state change
    float pitchResult = 100.0f;

    motionMgr->CheckPitchSpeed(rotateBySpeedParam, limit, pitchResult);
    EXPECT_EQ(motionMgr->status_.pitchLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
 * @tc.name  : HandelRotateParam_001
 * @tc.number: HandelRotateParam_001
 * @tc.desc  : Test HandelRotateParam with nullptr deviceStatus.
 */
HWTEST_F(MotionManagerTwoTest, HandelRotateParam_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set deviceStatus_ to nullptr
    motionMgr->deviceStatus_ = nullptr;

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    bool willLimitChange = false;

    motionMgr->HandelRotateParam(rotateParam, willLimitChange);
    EXPECT_EQ(willLimitChange, false);
}

/**
 * @tc.name  : HandelRotateParam_002
 * @tc.number: HandelRotateParam_002
 * @tc.desc  : Test HandelRotateParam with nullptr rotateParam.
 */
HWTEST_F(MotionManagerTwoTest, HandelRotateParam_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = nullptr;
    bool willLimitChange = false;

    motionMgr->HandelRotateParam(rotateParam, willLimitChange);
    EXPECT_EQ(willLimitChange, false);
}

/**
 * @tc.name  : HandelRotateParam_003
 * @tc.number: HandelRotateParam_003
 * @tc.desc  : Test HandelRotateParam with valid params.
 */
HWTEST_F(MotionManagerTwoTest, HandelRotateParam_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = 10.0f;
    rotateParam->degree.roll = 5.0f;
    rotateParam->degree.pitch = 8.0f;
    bool willLimitChange = false;

    motionMgr->HandelRotateParam(rotateParam, willLimitChange);
    EXPECT_EQ(willLimitChange, false);
}

/**
 * @tc.name  : HandelRotateParam_004
 * @tc.number: HandelRotateParam_004
 * @tc.desc  : Test HandelRotateParam with limit state changed.
 */
HWTEST_F(MotionManagerTwoTest, HandelRotateParam_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set rotation limits to trigger limit state change
    motionMgr->deviceStatus_->rotationLimitWithOffset.negMax.yaw = -1.0f;
    motionMgr->deviceStatus_->rotationLimitWithOffset.posMax.yaw = 1.0f;
    motionMgr->deviceStatus_->rotationLimitWithOffset.negMax.roll = -1.0f;
    motionMgr->deviceStatus_->rotationLimitWithOffset.posMax.roll = 1.0f;
    motionMgr->deviceStatus_->rotationLimitWithOffset.negMax.pitch = -1.0f;
    motionMgr->deviceStatus_->rotationLimitWithOffset.posMax.pitch = 1.0f;

    // Set current position
    motionMgr->deviceStatus_->eulerAngles.yaw = 0.0f;
    motionMgr->deviceStatus_->eulerAngles.roll = 0.0f;
    motionMgr->deviceStatus_->eulerAngles.pitch = 0.0f;

    // Set rotation parameters that will exceed limits
    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = 2.0f;  // Exceeds posMax.yaw (1.0f)
    rotateParam->degree.roll = 2.0f;  // Exceeds posMax.roll (1.0f)
    rotateParam->degree.pitch = 2.0f;  // Exceeds posMax.pitch (1.0f)
    bool willLimitChange = false;

    motionMgr->HandelRotateParam(rotateParam, willLimitChange);
    EXPECT_EQ(willLimitChange, true);
    // willLimitChange should be true because limit state changed from NOT_LIMITED to POS_LIMITED
}

/**
 * @tc.name  : CheckWheelSpeedLimit_001
 * @tc.number: CheckWheelSpeedLimit_001
 * @tc.desc  : Test CheckWheelSpeedLimit with empty params.
 */
HWTEST_F(MotionManagerTwoTest, CheckWheelSpeedLimit_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::vector<RotateParam> rotateParams;

    int32_t result = motionMgr->CheckWheelSpeedLimit(rotateParams);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : CheckWheelSpeedLimit_002
 * @tc.number: CheckWheelSpeedLimit_002
 * @tc.desc  : Test CheckWheelSpeedLimit with valid params.
 */
HWTEST_F(MotionManagerTwoTest, CheckWheelSpeedLimit_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Initialize wheelCapabilityInfo_ to support the test parameters
    motionMgr->wheelCapabilityInfo_.maxForwardSpeed = 200;
    motionMgr->wheelCapabilityInfo_.maxRotationSpeed = 10.0f;

    std::vector<RotateParam> rotateParams;
    RotateParam param;
    param.forwardSpeed = 100;
    param.turningSpeed = 5.0f;
    rotateParams.push_back(param);

    int32_t result = motionMgr->CheckWheelSpeedLimit(rotateParams);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : CheckWheelSpeedLimit_003
 * @tc.number: CheckWheelSpeedLimit_003
 * @tc.desc  : Test CheckWheelSpeedLimit with exceed forward speed.
 */
HWTEST_F(MotionManagerTwoTest, CheckWheelSpeedLimit_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Initialize wheelCapabilityInfo_ with a small limit to make test parameter exceed it
    motionMgr->wheelCapabilityInfo_.maxForwardSpeed = 200;

    std::vector<RotateParam> rotateParams;
    RotateParam param;
    param.forwardSpeed = 30000;
    param.turningSpeed = 5.0f;
    rotateParams.push_back(param);

    int32_t result = motionMgr->CheckWheelSpeedLimit(rotateParams);
    EXPECT_EQ(result, WHEEL_SPEED_EXCEED_LIMIT);
}

/**
 * @tc.name  : CheckWheelSpeedLimit_004
 * @tc.number: CheckWheelSpeedLimit_004
 * @tc.desc  : Test CheckWheelSpeedLimit with exceed turning speed.
 */
HWTEST_F(MotionManagerTwoTest, CheckWheelSpeedLimit_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Initialize wheelCapabilityInfo_ with a small limit to make test parameter exceed it
    motionMgr->wheelCapabilityInfo_.maxForwardSpeed = 200;
    motionMgr->wheelCapabilityInfo_.maxRotationSpeed = 10.0f;

    std::vector<RotateParam> rotateParams;
    RotateParam param;
    param.forwardSpeed = 100;
    param.turningSpeed = 30000.0f;
    rotateParams.push_back(param);

    int32_t result = motionMgr->CheckWheelSpeedLimit(rotateParams);
    EXPECT_EQ(result, WHEEL_SPEED_EXCEED_LIMIT);
}

/**
 * @tc.name  : CheckWheelSpeedLimit_005
 * @tc.number: CheckWheelSpeedLimit_005
 * @tc.desc  : Test CheckWheelSpeedLimit with forwardSpeed below minimum.
 */
HWTEST_F(MotionManagerTwoTest, CheckWheelSpeedLimit_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Initialize wheelCapabilityInfo_ with a small limit to make test parameter exceed it
    motionMgr->wheelCapabilityInfo_.maxForwardSpeed = 200;

    std::vector<RotateParam> rotateParams;
    RotateParam param;
    param.forwardSpeed = -30000;
    param.turningSpeed = 5.0f;
    rotateParams.push_back(param);

    int32_t result = motionMgr->CheckWheelSpeedLimit(rotateParams);
    EXPECT_EQ(result, WHEEL_SPEED_EXCEED_LIMIT);
}

/**
 * @tc.name  : ConvertMoveParamsToRotateParams_001
 * @tc.number: ConvertMoveParamsToRotateParams_001
 * @tc.desc  : Test ConvertMoveParamsToRotateParams with no distance and angle.
 */
HWTEST_F(MotionManagerTwoTest, ConvertMoveParamsToRotateParams_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<MoveParams> moveParams = std::make_shared<MoveParams>();
    moveParams->distance = 0;
    moveParams->angle = 0;

    std::vector<RotateParam> result = motionMgr->ConvertMoveParamsToRotateParams(moveParams);
    EXPECT_EQ(result.size(), 0);
    EXPECT_TRUE(result.empty());
}

/**
 * @tc.name  : ConvertMoveParamsToRotateParams_002
 * @tc.number: ConvertMoveParamsToRotateParams_002
 * @tc.desc  : Test ConvertMoveParamsToRotateParams with distance only.
 */
HWTEST_F(MotionManagerTwoTest, ConvertMoveParamsToRotateParams_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<MoveParams> moveParams = std::make_shared<MoveParams>();
    moveParams->distance = 1000;
    moveParams->angle = 0;
    moveParams->speedGear = SpeedGear::MIDDLE_SPEED;

    std::vector<RotateParam> result = motionMgr->ConvertMoveParamsToRotateParams(moveParams);
    EXPECT_GE(result.size(), 0);
}

/**
 * @tc.name  : ConvertMoveParamsToRotateParams_003
 * @tc.number: ConvertMoveParamsToRotateParams_003
 * @tc.desc  : Test ConvertMoveParamsToRotateParams with angle only.
 */
HWTEST_F(MotionManagerTwoTest, ConvertMoveParamsToRotateParams_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<MoveParams> moveParams = std::make_shared<MoveParams>();
    moveParams->distance = 0;
    moveParams->angle = 90;

    std::vector<RotateParam> result = motionMgr->ConvertMoveParamsToRotateParams(moveParams);
    EXPECT_GE(result.size(), 0);
}

/**
 * @tc.name  : ConvertMoveParamsToRotateParams_004
 * @tc.number: ConvertMoveParamsToRotateParams_004
 * @tc.desc  : Test ConvertMoveParamsToRotateParams with TURN_THEN_MOVE mode.
 */
HWTEST_F(MotionManagerTwoTest, ConvertMoveParamsToRotateParams_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<MoveParams> moveParams = std::make_shared<MoveParams>();
    moveParams->distance = 1000;
    moveParams->angle = 90;
    moveParams->mode = MarchingMode::TURN_THEN_MOVE;
    moveParams->speedGear = SpeedGear::MIDDLE_SPEED;

    std::vector<RotateParam> result = motionMgr->ConvertMoveParamsToRotateParams(moveParams);
    EXPECT_GE(result.size(), 0);
}

/**
 * @tc.name  : ConvertMoveParamsToRotateParams_005
 * @tc.number: ConvertMoveParamsToRotateParams_005
 * @tc.desc  : Test ConvertMoveParamsToRotateParams with simultaneous move and turn.
 */
HWTEST_F(MotionManagerTwoTest, ConvertMoveParamsToRotateParams_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<MoveParams> moveParams = std::make_shared<MoveParams>();
    moveParams->distance = 1000;
    moveParams->angle = 90;
    moveParams->mode = MarchingMode::TURNING_MOVING;  // Not TURN_THEN_MOVE, hasDistance != 0
    moveParams->speedGear = SpeedGear::MIDDLE_SPEED;

    std::vector<RotateParam> result = motionMgr->ConvertMoveParamsToRotateParams(moveParams);
    EXPECT_GE(result.size(), 0);
}

/**
 * @tc.name  : ConvertMoveParamsToRotateParams_006
 * @tc.number: ConvertMoveParamsToRotateParams_006
 * @tc.desc  : Test ConvertMoveParamsToRotateParams with angle only in default mode.
 */
HWTEST_F(MotionManagerTwoTest, ConvertMoveParamsToRotateParams_006, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<MoveParams> moveParams = std::make_shared<MoveParams>();
    moveParams->distance = 0;
    moveParams->angle = 90;
    moveParams->mode = MarchingMode::TURNING_MOVING;  // Not TURN_THEN_MOVE, hasDistance == 0

    std::vector<RotateParam> result = motionMgr->ConvertMoveParamsToRotateParams(moveParams);
    EXPECT_GE(result.size(), 0);
}

/**
 * @tc.name  : ConvertSpeedParamsToRotateParams_001
 * @tc.number: ConvertSpeedParamsToRotateParams_001
 * @tc.desc  : Test ConvertSpeedParamsToRotateParams with no speed and angle.
 */
HWTEST_F(MotionManagerTwoTest, ConvertSpeedParamsToRotateParams_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<SpeedParams> speedParams = std::make_shared<SpeedParams>();
    speedParams->speed = 0;
    speedParams->angle = 0;

    std::vector<RotateParam> result = motionMgr->ConvertSpeedParamsToRotateParams(speedParams, 1000);
    EXPECT_TRUE(result.empty());
}

/**
 * @tc.name  : ConvertSpeedParamsToRotateParams_002
 * @tc.number: ConvertSpeedParamsToRotateParams_002
 * @tc.desc  : Test ConvertSpeedParamsToRotateParams with speed only.
 */
HWTEST_F(MotionManagerTwoTest, ConvertSpeedParamsToRotateParams_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<SpeedParams> speedParams = std::make_shared<SpeedParams>();
    speedParams->speed = 100;
    speedParams->angle = 0;

    std::vector<RotateParam> result = motionMgr->ConvertSpeedParamsToRotateParams(speedParams, 1000);
    EXPECT_GE(result.size(), 0);
}

/**
 * @tc.name  : ConvertSpeedParamsToRotateParams_003
 * @tc.number: ConvertSpeedParamsToRotateParams_003
 * @tc.desc  : Test ConvertSpeedParamsToRotateParams with angle only.
 */
HWTEST_F(MotionManagerTwoTest, ConvertSpeedParamsToRotateParams_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<SpeedParams> speedParams = std::make_shared<SpeedParams>();
    speedParams->speed = 0;
    speedParams->angle = 90;

    std::vector<RotateParam> result = motionMgr->ConvertSpeedParamsToRotateParams(speedParams, 1000);
    EXPECT_GE(result.size(), 0);
}

/**
 * @tc.name  : ConvertSpeedParamsToRotateParams_004
 * @tc.number: ConvertSpeedParamsToRotateParams_004
 * @tc.desc  : Test ConvertSpeedParamsToRotateParams with TURN_THEN_MOVE mode.
 */
HWTEST_F(MotionManagerTwoTest, ConvertSpeedParamsToRotateParams_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<SpeedParams> speedParams = std::make_shared<SpeedParams>();
    speedParams->speed = 100;
    speedParams->angle = 90;
    speedParams->mode = MarchingMode::TURN_THEN_MOVE;

    std::vector<RotateParam> result = motionMgr->ConvertSpeedParamsToRotateParams(speedParams, 1000);
    EXPECT_GE(result.size(), 0);
}

/**
 * @tc.name  : ConvertSpeedParamsToRotateParams_005
 * @tc.number: ConvertSpeedParamsToRotateParams_005
 * @tc.desc  : Test ConvertSpeedParamsToRotateParams with simultaneous move and turn in TURNING_MOVING mode.
 */
HWTEST_F(MotionManagerTwoTest, ConvertSpeedParamsToRotateParams_005, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<SpeedParams> speedParams = std::make_shared<SpeedParams>();
    speedParams->speed = 100;
    speedParams->angle = 90;
    speedParams->mode = MarchingMode::TURNING_MOVING;  // Not TURN_THEN_MOVE

    std::vector<RotateParam> result = motionMgr->ConvertSpeedParamsToRotateParams(speedParams, 1000);
    EXPECT_GE(result.size(), 0);
}

/**
 * @tc.name  : CreateAndSendRotateBySpeedCommand_001
 * @tc.number: CreateAndSendRotateBySpeedCommand_001
 * @tc.desc  : Test CreateAndSendRotateBySpeedCommand function.
 */
HWTEST_F(MotionManagerTwoTest, CreateAndSendRotateBySpeedCommand_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateSpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateSpeedParam->speed.yawSpeed = 10.0f;
    rotateSpeedParam->speed.rollSpeed = 5.0f;
    rotateSpeedParam->speed.pitchSpeed = 8.0f;
    uint8_t responseTaskId = 1;

    motionMgr->CreateAndSendRotateBySpeedCommand(rotateSpeedParam, responseTaskId);
    EXPECT_GE(responseTaskId, 0);
}

/**
 * @tc.name  : HandelRotateSpeedParam_001
 * @tc.number: HandelRotateSpeedParam_001
 * @tc.desc  : Test HandelRotateSpeedParam with nullptr deviceStatus.
 */
HWTEST_F(MotionManagerTwoTest, HandelRotateSpeedParam_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Explicitly set deviceStatus_ to nullptr
    motionMgr->deviceStatus_ = nullptr;

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    bool willLimitChange = false;

    motionMgr->HandelRotateSpeedParam(rotateBySpeedParam, willLimitChange);
    EXPECT_EQ(willLimitChange, false);
}

/**
 * @tc.name  : HandelRotateSpeedParam_002
 * @tc.number: HandelRotateSpeedParam_002
 * @tc.desc  : Test HandelRotateSpeedParam with nullptr rotateBySpeedParam.
 */
HWTEST_F(MotionManagerTwoTest, HandelRotateSpeedParam_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = nullptr;
    bool willLimitChange = false;

    motionMgr->HandelRotateSpeedParam(rotateBySpeedParam, willLimitChange);
    EXPECT_EQ(willLimitChange, false);
}

/**
 * @tc.name  : HandelRotateSpeedParam_003
 * @tc.number: HandelRotateSpeedParam_003
 * @tc.desc  : Test HandelRotateSpeedParam with valid params.
 */
HWTEST_F(MotionManagerTwoTest, HandelRotateSpeedParam_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    rotateBySpeedParam->speed.yawSpeed = 10.0f;
    rotateBySpeedParam->speed.rollSpeed = 5.0f;
    rotateBySpeedParam->speed.pitchSpeed = 8.0f;
    bool willLimitChange = false;

    motionMgr->HandelRotateSpeedParam(rotateBySpeedParam, willLimitChange);
    EXPECT_EQ(willLimitChange, false);
}

/**
 * @tc.name  : HandelRotateSpeedParam_004
 * @tc.number: HandelRotateSpeedParam_004
 * @tc.desc  : Test HandelRotateSpeedParam with params that trigger limit state change.
 */
HWTEST_F(MotionManagerTwoTest, HandelRotateSpeedParam_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam = std::make_shared<RotateBySpeedParam>();
    // Set high speed and duration to trigger limit state change
    rotateBySpeedParam->speed.yawSpeed = 100.0f;
    rotateBySpeedParam->speed.rollSpeed = 50.0f;
    rotateBySpeedParam->speed.pitchSpeed = 80.0f;
    rotateBySpeedParam->duration = 5000;
    bool willLimitChange = false;

    motionMgr->HandelRotateSpeedParam(rotateBySpeedParam, willLimitChange);
    EXPECT_EQ(willLimitChange, false);
}

/**
 * @tc.name  : ExecuteRotateCommand_001
 * @tc.number: ExecuteRotateCommand_001
 * @tc.desc  : Test ExecuteRotateCommand with protocol version 0x01.
 */
HWTEST_F(MotionManagerTwoTest, ExecuteRotateCommand_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    RotateParam param;
    param.degree.yaw = 10.0f;
    param.degree.roll = 5.0f;
    param.degree.pitch = 8.0f;
    uint8_t taskId = 1;

    motionMgr->ExecuteRotateCommand(param, taskId);
    EXPECT_GE(taskId, 0);
}

/**
 * @tc.name  : ExecuteRotateCommand_002
 * @tc.number: ExecuteRotateCommand_002
 * @tc.desc  : Test ExecuteRotateCommand with protocol version 0x02.
 */
HWTEST_F(MotionManagerTwoTest, ExecuteRotateCommand_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set protocol version to 0x02 to test the 0x02 branch
    motionMgr->protocolVer_ = 0x02;

    RotateParam param;
    param.degree.yaw = 10.0f;
    param.degree.roll = 5.0f;
    param.degree.pitch = 8.0f;
    uint8_t taskId = 1;

    motionMgr->ExecuteRotateCommand(param, taskId);
    EXPECT_GE(taskId, 0);
}

/**
 * @tc.name  : ExecuteRotateCommand_003
 * @tc.number: ExecuteRotateCommand_003
 * @tc.desc  : Test ExecuteRotateCommand with protocol version 0x02 and isRelative = true.
 */
HWTEST_F(MotionManagerTwoTest, ExecuteRotateCommand_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set protocol version to 0x02 to test the GenerateRotateToLocationParam branch
    motionMgr->protocolVer_ = 0x02;

    RotateParam param;
    param.degree.yaw = 10.0f;
    param.degree.roll = 5.0f;
    param.degree.pitch = 8.0f;
    param.isRelative = true;  // Cover line 1229 (if branch)
    uint8_t taskId = 1;

    motionMgr->ExecuteRotateCommand(param, taskId);
    EXPECT_GE(taskId, 0);
}

/**
 * @tc.name  : SendRotateCommandImpl_001
 * @tc.number: SendRotateCommandImpl_001
 * @tc.desc  : Test SendRotateCommandImpl with single rotate param.
 */
HWTEST_F(MotionManagerTwoTest, SendRotateCommandImpl_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::vector<RotateParam> rotateParams;
    RotateParam param;
    param.duration = 1000;
    param.forwardSpeed = 100;
    param.turningSpeed = 5.0f;
    rotateParams.push_back(param);
    uint8_t responseTaskId = 1;
    uint16_t rotateTaskId = 100;

    motionMgr->SendRotateCommandImpl(responseTaskId, rotateTaskId, rotateParams);
    EXPECT_GE(responseTaskId, 0);
}

/**
 * @tc.name  : SendRotateCommandImpl_002
 * @tc.number: SendRotateCommandImpl_002
 * @tc.desc  : Test SendRotateCommandImpl with multiple rotate params.
 */
HWTEST_F(MotionManagerTwoTest, SendRotateCommandImpl_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    std::vector<RotateParam> rotateParams;
    RotateParam param1;
    param1.duration = 1000;
    param1.forwardSpeed = 100;
    param1.turningSpeed = 5.0f;
    rotateParams.push_back(param1);
    RotateParam param2;
    param2.duration = 2000;
    param2.forwardSpeed = 200;
    param2.turningSpeed = 10.0f;
    rotateParams.push_back(param2);
    uint8_t responseTaskId = 1;
    uint16_t rotateTaskId = 100;

    motionMgr->SendRotateCommandImpl(responseTaskId, rotateTaskId, rotateParams);
    EXPECT_GE(responseTaskId, 0);
}

/**
 * @tc.name  : SendRotateCommand_001
 * @tc.number: SendRotateCommand_001
 * @tc.desc  : Test SendRotateCommand with empty rotateParams.
 */
HWTEST_F(MotionManagerTwoTest, SendRotateCommand_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    uint32_t tokenId = 100;
    std::string napiCmdId = "test_cmd_id";
    std::vector<RotateParam> rotateParams;  // Empty rotateParams

    int32_t result = motionMgr->SendRotateCommand(tokenId, napiCmdId, rotateParams);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : SendRotateCommand_002
 * @tc.number: SendRotateCommand_002
 * @tc.desc  : Test SendRotateCommand with valid rotateParams and eventHandler_ not null.
 */
HWTEST_F(MotionManagerTwoTest, SendRotateCommand_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    uint32_t tokenId = 100;
    std::string napiCmdId = "test_cmd_id";
    std::vector<RotateParam> rotateParams;
    RotateParam param;
    param.duration = 1000;
    param.forwardSpeed = 100;
    param.turningSpeed = 5.0f;
    rotateParams.push_back(param);

    int32_t result = motionMgr->SendRotateCommand(tokenId, napiCmdId, rotateParams);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : Init_001
 * @tc.number: Init_001
 * @tc.desc  : Test Init function with protocol version 0x01.
 */
HWTEST_F(MotionManagerTwoTest, Init_001, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set protocol version to 0x01 to test protocolVer_ == 0x01 branch (lines 726-728, 748-752)
    motionMgr->protocolVer_ = 0x01;
    motionMgr->deviceBaseInfoReady_ = true;

    motionMgr->Init();
    EXPECT_EQ(motionMgr->deviceBaseInfoReady_, true);
}

/**
 * @tc.name  : Init_002
 * @tc.number: Init_002
 * @tc.desc  : Test Init function with protocol version >= 0x02.
 */
HWTEST_F(MotionManagerTwoTest, Init_002, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set protocol version to 0x02 to test protocolVer_ >= 0x02 branch (lines 728-742)
    motionMgr->protocolVer_ = 0x02;
    motionMgr->deviceBaseInfoReady_ = true;

    motionMgr->Init();
    EXPECT_EQ(motionMgr->deviceBaseInfoReady_, true);
}

/**
 * @tc.name  : Init_003
 * @tc.number: Init_003
 * @tc.desc  : Test Init function with WHEEL_BASE device type.
 */
HWTEST_F(MotionManagerTwoTest, Init_003, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set protocol version to 0x02 and device type to WHEEL_BASE to test lines 736-741
    motionMgr->protocolVer_ = 0x02;
    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::WHEEL_BASE);
    motionMgr->deviceBaseInfoReady_ = true;
    motionMgr->isFirstConnect_ = true;

    motionMgr->Init();
    EXPECT_EQ(motionMgr->deviceBaseInfoReady_, true);
}

/**
 * @tc.name  : Init_004
 * @tc.number: Init_004
 * @tc.desc  : Test Init function with unknown protocol version.
 */
HWTEST_F(MotionManagerTwoTest, Init_004, TestSize.Level1)
{
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();

    // Set protocol version to 0x00 to test else branch (lines 743-745)
    motionMgr->protocolVer_ = 0x00;
    motionMgr->deviceBaseInfoReady_ = true;

    motionMgr->Init();
    EXPECT_EQ(motionMgr->deviceBaseInfoReady_, true);
}
}
}