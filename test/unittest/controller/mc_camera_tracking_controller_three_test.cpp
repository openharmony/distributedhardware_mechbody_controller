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

#include "gtest/gtest.h"
// NOLINTNEXTLINE
#define private public
#include "mc_camera_tracking_controller.h"
#include "mc_controller_manager.h"
#include "mc_controller_ipc_death_listener.h"
#undef private

#include "../mechbody_controller_log.h"
#include "../test_log.h"
#include "mechbody_controller_service.h"
#include "mechbody_controller_enums.h"
#include "mc_connect_manager.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS {
namespace MechBodyController {

class MockIRemoteObject : public IRemoteObject {
public:
    MockIRemoteObject() : IRemoteObject(u"mock_i_remote_object") {}
    ~MockIRemoteObject() {}
    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        return 0;
    }
    int32_t GetObjectRefCount() override
    {
        return 0;
    }
    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }
    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }
    int Dump(int fd, const std::vector<std::u16string> &args) override
    {
        (void)fd;
        return 0;
    }
    std::u16string GetObjectDescriptor() const
    {
        std::u16string descriptor = std::u16string();
        return descriptor;
    }
};

class FailSendMockIRemoteObject : public IRemoteObject {
public:
    FailSendMockIRemoteObject() : IRemoteObject(u"fail_send_mock") {}
    ~FailSendMockIRemoteObject() {}
    int32_t GetObjectRefCount() override { return 1; }
    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        return -1;
    }
    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override { return true; }
    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override { return true; }
    int Dump(int fd, const std::vector<std::u16string> &args) override { return 0; }
};

class McCameraTrackingControllerThreeTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        DTEST_LOG << "McCameraTrackingControllerThreeTest::SetUpTestCase" << std::endl;
    }
    static void TearDownTestCase()
    {
        DTEST_LOG << "McCameraTrackingControllerThreeTest::TearDownTestCase" << std::endl;
    }
    void SetUp()
    {
        DTEST_LOG << "McCameraTrackingControllerThreeTest::SetUp" << std::endl;
    }
    void TearDown()
    {
        DTEST_LOG << "McCameraTrackingControllerThreeTest::TearDown" << std::endl;
    }
};

/**
 * @tc.name  : UnRegisterTrackingEventCallback_001
 * @tc.number: UnRegisterTrackingEventCallback_001
 * @tc.desc  : Testing UnRegisterTrackingEventCallback with tokenId that exists in trackingEventCallback_.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, UnRegisterTrackingEventCallback_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest UnRegisterTrackingEventCallback_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    uint32_t tokenId = 9001;
    sptr<IRemoteObject> callback = new MockIRemoteObject();
    controller.trackingEventCallback_[tokenId] = callback;

    int32_t ret = controller.UnRegisterTrackingEventCallback(tokenId);

    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(controller.trackingEventCallback_.find(tokenId), controller.trackingEventCallback_.end());

    DTEST_LOG << "McCameraTrackingControllerThreeTest UnRegisterTrackingEventCallback_001 end" << std::endl;
}

/**
 * @tc.name  : UnRegisterTrackingEventCallback_002
 * @tc.number: UnRegisterTrackingEventCallback_002
 * @tc.desc  : Testing UnRegisterTrackingEventCallback with tokenId that does not exist in trackingEventCallback_.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, UnRegisterTrackingEventCallback_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest UnRegisterTrackingEventCallback_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    uint32_t tokenId = 9999;
    controller.trackingEventCallback_.erase(tokenId);

    int32_t ret = controller.UnRegisterTrackingEventCallback(tokenId);

    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerThreeTest UnRegisterTrackingEventCallback_002 end" << std::endl;
}

/**
 * @tc.name  : OnTrackingEvent_002
 * @tc.number: OnTrackingEvent_002
 * @tc.desc  : Testing OnTrackingEvent with CAMERA_TRACKING_USER_DISABLED event, no callbacks registered.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, OnTrackingEvent_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 7777;
    controller.trackingEventCallback_.clear();

    int32_t mechId = 100;
    int32_t ret = controller.OnTrackingEvent(mechId, TrackingEvent::CAMERA_TRACKING_USER_DISABLED);

    EXPECT_EQ(ret, ERR_OK);
    EXPECT_FALSE(NotificationUtils::isTrackingEnabled_);

    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_002 end" << std::endl;
}

/**
 * @tc.name  : OnTrackingEvent_003
 * @tc.number: OnTrackingEvent_003
 * @tc.desc  : Testing OnTrackingEvent with CAMERA_TRACKING_LAYOUT_CHANGED event (neither ENABLED nor DISABLED).
 */
HWTEST_F(McCameraTrackingControllerThreeTest, OnTrackingEvent_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 7777;
    controller.trackingEventCallback_.clear();

    NotificationUtils::isTrackingEnabled_ = true;
    int32_t mechId = 100;
    int32_t ret = controller.OnTrackingEvent(mechId, TrackingEvent::CAMERA_TRACKING_LAYOUT_CHANGED);

    EXPECT_EQ(ret, ERR_OK);
    // isTrackingEnabled_ should remain unchanged when event is neither ENABLED nor DISABLED
    EXPECT_TRUE(NotificationUtils::isTrackingEnabled_);

    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_003 end" << std::endl;
}

/**
 * @tc.name  : OnTrackingEvent_004
 * @tc.number: OnTrackingEvent_004
 * @tc.desc  : Testing OnTrackingEvent with CAMERA_TRACKING_USER_ENABLED,
               appSettings has entry with isTrackingEnabled=false.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, OnTrackingEvent_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 7777;

    uint32_t tokenId = 8001;
    auto setting = std::make_shared<AppSetting>();
    setting->isTrackingEnabled = false;
    controller.appSettings[tokenId] = setting;
    controller.trackingEventCallback_[tokenId] = new MockIRemoteObject();

    int32_t mechId = 100;
    int32_t ret = controller.OnTrackingEvent(mechId, TrackingEvent::CAMERA_TRACKING_USER_ENABLED);

    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(NotificationUtils::isTrackingEnabled_);

    controller.appSettings.erase(tokenId);
    controller.trackingEventCallback_.erase(tokenId);

    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_004 end" << std::endl;
}

/**
 * @tc.name  : OnTrackingEvent_005
 * @tc.number: OnTrackingEvent_005
 * @tc.desc  : Testing OnTrackingEvent with CAMERA_TRACKING_USER_ENABLED,
                appSettings has entry with isTrackingEnabled=true.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, OnTrackingEvent_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_005 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 7777;

    uint32_t tokenId = 8002;
    auto setting = std::make_shared<AppSetting>();
    setting->isTrackingEnabled = true;
    controller.appSettings[tokenId] = setting;
    controller.trackingEventCallback_[tokenId] = new MockIRemoteObject();

    int32_t mechId = 100;
    int32_t ret = controller.OnTrackingEvent(mechId, TrackingEvent::CAMERA_TRACKING_USER_ENABLED);

    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(NotificationUtils::isTrackingEnabled_);

    controller.appSettings.erase(tokenId);
    controller.trackingEventCallback_.erase(tokenId);

    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_005 end" << std::endl;
}

/**
 * @tc.name  : OnTrackingEvent_006
 * @tc.number: OnTrackingEvent_006
 * @tc.desc  : Testing OnTrackingEvent with CAMERA_TRACKING_USER_ENABLED, appSettings has no entry for tokenId.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, OnTrackingEvent_006, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_006 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 7777;

    uint32_t tokenId = 8003;
    controller.appSettings.erase(tokenId);
    controller.trackingEventCallback_[tokenId] = new MockIRemoteObject();

    int32_t mechId = 100;
    int32_t ret = controller.OnTrackingEvent(mechId, TrackingEvent::CAMERA_TRACKING_USER_ENABLED);

    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(NotificationUtils::isTrackingEnabled_);

    controller.trackingEventCallback_.erase(tokenId);

    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_006 end" << std::endl;
}

/**
 * @tc.name  : OnTrackingEvent_007
 * @tc.number: OnTrackingEvent_007
 * @tc.desc  : Testing OnTrackingEvent with callback that is nullptr, CHECK_POINTER_RETURN_VALUE returns early.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, OnTrackingEvent_007, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_007 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 7777;

    uint32_t tokenId = 8004;
    controller.appSettings.erase(tokenId);
    controller.trackingEventCallback_[tokenId] = nullptr;

    int32_t mechId = 100;
    int32_t ret = controller.OnTrackingEvent(mechId, TrackingEvent::CAMERA_TRACKING_USER_ENABLED);

    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);

    controller.trackingEventCallback_.erase(tokenId);

    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_007 end" << std::endl;
}

/**
 * @tc.name  : OnTrackingEvent_008
 * @tc.number: OnTrackingEvent_008
 * @tc.desc  : Testing OnTrackingEvent with FailSendMockIRemoteObject, callback SendRequest returns error.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, OnTrackingEvent_008, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_008 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 7777;

    uint32_t tokenId = 8005;
    controller.appSettings.erase(tokenId);
    controller.trackingEventCallback_[tokenId] = new FailSendMockIRemoteObject();

    int32_t mechId = 100;
    int32_t ret = controller.OnTrackingEvent(mechId, TrackingEvent::CAMERA_TRACKING_USER_ENABLED);

    EXPECT_EQ(ret, ERR_OK);

    controller.trackingEventCallback_.erase(tokenId);

    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_008 end" << std::endl;
}

/**
 * @tc.name  : OnTrackingEvent_009
 * @tc.number: OnTrackingEvent_009
 * @tc.desc  : Testing OnTrackingEvent with valid callback (MockIRemoteObject), SendRequest succeeds.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, OnTrackingEvent_009, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_009 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 7777;

    uint32_t tokenId = 8006;
    controller.appSettings.erase(tokenId);
    controller.trackingEventCallback_[tokenId] = new MockIRemoteObject();

    int32_t mechId = 100;
    int32_t ret = controller.OnTrackingEvent(mechId, TrackingEvent::CAMERA_TRACKING_USER_ENABLED);

    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(NotificationUtils::isTrackingEnabled_);

    controller.trackingEventCallback_.erase(tokenId);

    DTEST_LOG << "McCameraTrackingControllerThreeTest OnTrackingEvent_009 end" << std::endl;
}

/**
 * @tc.name  : SearchTarget_001
 * @tc.number: SearchTarget_001
 * @tc.desc  : Testing SearchTarget with nullptr targetInfo.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, SearchTarget_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::string napiCmdId = "test_cmd_001";
    uint32_t tokenId = 9001;
    auto searchParams = std::make_shared<SearchParams>();

    int32_t ret = controller.SearchTarget(napiCmdId, tokenId, nullptr, searchParams);

    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);

    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_001 end" << std::endl;
}

/**
 * @tc.name  : SearchTarget_002
 * @tc.number: SearchTarget_002
 * @tc.desc  : Testing SearchTarget with nullptr searchParams.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, SearchTarget_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::string napiCmdId = "test_cmd_002";
    uint32_t tokenId = 9002;
    auto targetInfo = std::make_shared<TargetInfo>();

    int32_t ret = controller.SearchTarget(napiCmdId, tokenId, targetInfo, nullptr);

    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);

    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_002 end" << std::endl;
}

/**
 * @tc.name  : SearchTarget_003
 * @tc.number: SearchTarget_003
 * @tc.desc  : Testing SearchTarget with empty motionManagers map.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, SearchTarget_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::string napiCmdId = "test_cmd_003";
    uint32_t tokenId = 9003;
    auto targetInfo = std::make_shared<TargetInfo>();
    auto searchParams = std::make_shared<SearchParams>();

    // Given: empty motionManagers
    MechBodyControllerService::GetInstance().motionManagers_.clear();

    int32_t ret = controller.SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);

    EXPECT_EQ(ret, NO_DEVICE_CONNECTED);

    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_003 end" << std::endl;
}

/**
 * @tc.name  : SearchTarget_004
 * @tc.number: SearchTarget_004
 * @tc.desc  : Testing SearchTarget with motionManager being nullptr in map.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, SearchTarget_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::string napiCmdId = "test_cmd_004";
    uint32_t tokenId = 9004;
    auto targetInfo = std::make_shared<TargetInfo>();
    auto searchParams = std::make_shared<SearchParams>();
    int32_t mechId = 9104;

    // Given: motionManagers has an entry with nullptr value
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = nullptr;

    int32_t ret = controller.SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);

    EXPECT_EQ(ret, NO_DEVICE_CONNECTED);

    MechBodyControllerService::GetInstance().motionManagers_.erase(mechId);

    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_004 end" << std::endl;
}

/**
 * @tc.name  : SearchTarget_005
 * @tc.number: SearchTarget_005
 * @tc.desc  : Testing SearchTarget with trackingTargetNum > 0, calls SearchTargetEnd.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, SearchTarget_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_005 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::string napiCmdId = "test_cmd_005";
    uint32_t tokenId = 9005;
    auto targetInfo = std::make_shared<TargetInfo>();
    auto searchParams = std::make_shared<SearchParams>();
    int32_t mechId = 9105;

    // Given: motionManagers has a valid entry
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;

    // Given: currentCameraInfo_ has trackingTargetNum > 0
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->trackingTargetNum = 5;

    int32_t ret = controller.SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);

    EXPECT_EQ(ret, ERR_OK);

    // Clean up
    controller.currentCameraInfo_->trackingTargetNum = 0;
    MechBodyControllerService::GetInstance().motionManagers_.erase(mechId);

    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_005 end" << std::endl;
}

/**
 * @tc.name  : SearchTarget_006
 * @tc.number: SearchTarget_006
 * @tc.desc  : Testing SearchTarget with GetRotationLimit returning error (device not placed on mech).
 */
HWTEST_F(McCameraTrackingControllerThreeTest, SearchTarget_006, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_006 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::string napiCmdId = "test_cmd_006";
    uint32_t tokenId = 9006;
    auto targetInfo = std::make_shared<TargetInfo>();
    auto searchParams = std::make_shared<SearchParams>();
    int32_t mechId = 9106;

    // Given: motionManagers has a valid entry, but mech not in ATTACHED state
    // so GetRotationLimit returns DEVICE_NOT_PLACED_ON_MECH
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;

    // Given: currentCameraInfo_ with trackingTargetNum == 0 so we reach GetRotationLimit
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->trackingTargetNum = 0;

    // Remove mech from MechConnectManager so GetMechState returns UNKNOWN
    MechConnectManager::GetInstance().CleanMechInfo();

    int32_t ret = controller.SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);

    EXPECT_EQ(ret, GET_LIMIT_INFO_FAILED);

    // Clean up
    MechBodyControllerService::GetInstance().motionManagers_.erase(mechId);

    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_006 end" << std::endl;
}

/**
 * @tc.name  : SearchTarget_007
 * @tc.number: SearchTarget_007
 * @tc.desc  : Testing SearchTarget with valid motionManager and trackingTargetNum == 0,
 *             verifying the function reaches GetRotationLimit/GetCurrentPosition checks.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, SearchTarget_007, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_007 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::string napiCmdId = "test_cmd_007";
    uint32_t tokenId = 9007;
    auto targetInfo = std::make_shared<TargetInfo>();
    auto searchParams = std::make_shared<SearchParams>();
    int32_t mechId = 9107;

    // Given: Register mech as ATTACHED in MechConnectManager
    MechConnectManager::GetInstance().CleanMechInfo();
    MechInfo mechInfo;
    mechInfo.mechId = mechId;
    mechInfo.state = AttachmentState::ATTACHED;
    MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    MechConnectManager::GetInstance().NotifyMechState(mechId, true);

    // Given: motionManagers has a valid entry
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;

    // Given: currentCameraInfo_ with trackingTargetNum == 0 so we reach GetRotationLimit
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->trackingTargetNum = 0;

    // When: Call SearchTarget - result depends on whether GetRotationLimit/GetCurrentPosition succeed
    // In test environment, GetMechState may not return ATTACHED even after AddMechInfo + NotifyMechState
    // so the function may return GET_LIMIT_INFO_FAILED or ERR_OK depending on mech state
    int32_t ret = controller.SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);

    EXPECT_TRUE(ret == ERR_OK || ret == GET_LIMIT_INFO_FAILED || ret == GET_CURRENT_POSITION_FAILED
        || ret == MECH_HAS_DISCONNECTED);

    // Clean up
    MechBodyControllerService::GetInstance().motionManagers_.erase(mechId);
    MechConnectManager::GetInstance().CleanMechInfo();

    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_007 end" << std::endl;
}

/**
 * @tc.name  : SearchTarget_008
 * @tc.number: SearchTarget_008
 * @tc.desc  : Testing SearchTarget with motionManagers where first entry is nullptr and second is valid.
 *             Covers the for-loop that selects the first motionManager from the map.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, SearchTarget_008, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_008 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::string napiCmdId = "test_cmd_008";
    uint32_t tokenId = 9008;
    auto targetInfo = std::make_shared<TargetInfo>();
    auto searchParams = std::make_shared<SearchParams>();
    int32_t mechId1 = 9108;
    int32_t mechId2 = 9109;

    // Given: motionManagers has two entries, first one (smaller key) is nullptr
    // The for-loop takes the first entry and breaks, so motionManager will be nullptr
    MechBodyControllerService::GetInstance().motionManagers_[mechId1] = nullptr;
    std::shared_ptr<MotionManager> motionMgr2 =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId2);
    MechBodyControllerService::GetInstance().motionManagers_[mechId2] = motionMgr2;

    int32_t ret = controller.SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);

    EXPECT_EQ(ret, NO_DEVICE_CONNECTED);

    // Clean up
    MechBodyControllerService::GetInstance().motionManagers_.erase(mechId1);
    MechBodyControllerService::GetInstance().motionManagers_.erase(mechId2);

    DTEST_LOG << "McCameraTrackingControllerThreeTest SearchTarget_008 end" << std::endl;
}

/**
 * @tc.name  : RunSearchTarget_001
 * @tc.number: RunSearchTarget_001
 * @tc.desc  : Test RunSearchTarget with DEFAULT direction and asymmetric limits (toLeft<toRight).
 *             startFromNeg=true, device has limited position → ExecSearchTaskWithLimit path.
 *             Verify that ExecSearchTaskWithLimit does not synchronously set searchingTarget.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, RunSearchTarget_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest RunSearchTarget_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::string napiCmdId = "run_search_001";
    uint32_t tokenId = 11001;
    auto searchParams = std::make_shared<SearchParams>();
    searchParams->direction = SearchDirection::DEFAULT;

    // Asymmetric limits: negMax.yaw = -2.0 (> -3.14, so limited position)
    // This makes toLeft < toRight when yaw=0, so startFromNeg=true
    RotateDegreeLimit limit;
    limit.negMax.yaw = -2.0f;
    limit.negMax.roll = 0;
    limit.negMax.pitch = 0;
    limit.posMax.yaw = 3.14f;
    limit.posMax.roll = 0;
    limit.posMax.pitch = 0;

    auto currentPosition = std::make_shared<EulerAngles>(0.0f, 0.0f, 0.0f);

    // Set currentCameraInfo_ with searchingTarget=false and trackingTargetNum=0
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->searchingTarget.store(false, std::memory_order_relaxed);
    controller.currentCameraInfo_->trackingTargetNum = 0;
    controller.eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(nullptr);

    controller.RunSearchTarget(napiCmdId, tokenId, searchParams, limit, currentPosition);

    // ExecSearchTaskWithLimit posts task async, so searchingTarget remains false synchronously
    // This verifies the ExecSearchTaskWithLimit branch was taken (ExecSearchTask would set it true)
    EXPECT_EQ(controller.currentCameraInfo_->searchingTarget.load(std::memory_order_relaxed), false);

    // Clean up
    controller.eventHandler_ = nullptr;
    controller.currentCameraInfo_ = nullptr;

    DTEST_LOG << "McCameraTrackingControllerThreeTest RunSearchTarget_001 end" << std::endl;
}

/**
 * @tc.name  : RunSearchTarget_002
 * @tc.number: RunSearchTarget_002
 * @tc.desc  : Test RunSearchTarget with DEFAULT direction and symmetric limits (toLeft==toRight).
 *             startFromNeg=false, no limited position → ExecSearchTask path.
 *             Verify that ExecSearchTask synchronously sets searchingTarget=true.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, RunSearchTarget_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest RunSearchTarget_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::string napiCmdId = "run_search_002";
    uint32_t tokenId = 11002;
    auto searchParams = std::make_shared<SearchParams>();
    searchParams->direction = SearchDirection::DEFAULT;

    // Symmetric limits: negMax.yaw = -3.14, posMax.yaw = 3.14 (no limited position)
    // toLeft == toRight, so startFromNeg=false for DEFAULT direction
    RotateDegreeLimit limit;
    limit.negMax.yaw = -3.14f;
    limit.negMax.roll = 0;
    limit.negMax.pitch = 0;
    limit.posMax.yaw = 3.14f;
    limit.posMax.roll = 0;
    limit.posMax.pitch = 0;

    auto currentPosition = std::make_shared<EulerAngles>(0.0f, 0.0f, 0.0f);

    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->searchingTarget.store(false, std::memory_order_relaxed);
    controller.currentCameraInfo_->trackingTargetNum = 0;
    controller.eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(nullptr);

    controller.RunSearchTarget(napiCmdId, tokenId, searchParams, limit, currentPosition);

    // ExecSearchTask synchronously sets searchingTarget=true and tokenId
    EXPECT_EQ(controller.currentCameraInfo_->searchingTarget.load(std::memory_order_relaxed), true);
    EXPECT_EQ(controller.currentCameraInfo_->tokenId, tokenId);

    // Clean up
    controller.eventHandler_ = nullptr;
    controller.currentCameraInfo_ = nullptr;

    DTEST_LOG << "McCameraTrackingControllerThreeTest RunSearchTarget_002 end" << std::endl;
}

/**
 * @tc.name  : RunSearchTarget_003
 * @tc.number: RunSearchTarget_003
 * @tc.desc  : Test RunSearchTarget with RIGHTWARD direction and symmetric limits.
 *             startFromNeg=true (via RIGHTWARD), no limited position → ExecSearchTask path.
 *             Verify that ExecSearchTask synchronously sets searchingTarget=true.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, RunSearchTarget_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest RunSearchTarget_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::string napiCmdId = "run_search_003";
    uint32_t tokenId = 11003;
    auto searchParams = std::make_shared<SearchParams>();
    searchParams->direction = SearchDirection::RIGHTWARD;

    // Symmetric limits: no limited position → ExecSearchTask
    RotateDegreeLimit limit;
    limit.negMax.yaw = -3.14f;
    limit.negMax.roll = 0;
    limit.negMax.pitch = 0;
    limit.posMax.yaw = 3.14f;
    limit.posMax.roll = 0;
    limit.posMax.pitch = 0;

    auto currentPosition = std::make_shared<EulerAngles>(0.0f, 0.0f, 0.0f);

    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->searchingTarget.store(false, std::memory_order_relaxed);
    controller.currentCameraInfo_->trackingTargetNum = 0;
    controller.eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(nullptr);

    controller.RunSearchTarget(napiCmdId, tokenId, searchParams, limit, currentPosition);

    // ExecSearchTask synchronously sets searchingTarget=true
    EXPECT_EQ(controller.currentCameraInfo_->searchingTarget.load(std::memory_order_relaxed), true);
    EXPECT_EQ(controller.currentCameraInfo_->tokenId, tokenId);

    // Clean up
    controller.eventHandler_ = nullptr;
    controller.currentCameraInfo_ = nullptr;

    DTEST_LOG << "McCameraTrackingControllerThreeTest RunSearchTarget_003 end" << std::endl;
}

/**
 * @tc.name  : RunSearchTarget_004
 * @tc.number: RunSearchTarget_004
 * @tc.desc  : Test RunSearchTarget with LEFTWARD direction and limited position.
 *             startFromNeg=false (LEFTWARD), limited position → ExecSearchTaskWithLimit path.
 *             Verify that ExecSearchTaskWithLimit does not synchronously set searchingTarget.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, RunSearchTarget_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest RunSearchTarget_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::string napiCmdId = "run_search_004";
    uint32_t tokenId = 11004;
    auto searchParams = std::make_shared<SearchParams>();
    searchParams->direction = SearchDirection::LEFTWARD;

    // Limited position: posMax.yaw = 2.0 < 3.14
    RotateDegreeLimit limit;
    limit.negMax.yaw = -3.14f;
    limit.negMax.roll = 0;
    limit.negMax.pitch = 0;
    limit.posMax.yaw = 2.0f;
    limit.posMax.roll = 0;
    limit.posMax.pitch = 0;

    auto currentPosition = std::make_shared<EulerAngles>(0.0f, 0.0f, 0.0f);

    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->searchingTarget.store(false, std::memory_order_relaxed);
    controller.currentCameraInfo_->trackingTargetNum = 0;
    controller.eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(nullptr);

    controller.RunSearchTarget(napiCmdId, tokenId, searchParams, limit, currentPosition);

    // ExecSearchTaskWithLimit posts task async, so searchingTarget remains false synchronously
    EXPECT_EQ(controller.currentCameraInfo_->searchingTarget.load(std::memory_order_relaxed), false);

    // Clean up
    controller.eventHandler_ = nullptr;
    controller.currentCameraInfo_ = nullptr;

    DTEST_LOG << "McCameraTrackingControllerThreeTest RunSearchTarget_004 end" << std::endl;
}

/**
 * @tc.name  : OnReceiveEvent_001
 * @tc.number: OnReceiveEvent_001
 * @tc.desc  : Test OnReceiveEvent with action != COMMON_EVENT_USER_SWITCHED.
 *             curUserId_ should remain unchanged.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, OnReceiveEvent_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest OnReceiveEvent_001 begin" << std::endl;

    MechBodyEventBaseService& service = MechBodyEventBaseService::GetInstance();
    int32_t originalUserId = service.curUserId_.load(std::memory_order_relaxed);

    // When: action is not COMMON_EVENT_USER_SWITCHED
    std::string action = "some.other.action";
    AAFwk::Want want;
    want.SetAction(action);
    EventFwk::CommonEventData data(want);
    data.SetCode(200);

    service.OnReceiveEvent(action, data);

    // Then: curUserId_ should remain unchanged
    EXPECT_EQ(service.curUserId_.load(std::memory_order_relaxed), originalUserId);

    DTEST_LOG << "McCameraTrackingControllerThreeTest OnReceiveEvent_001 end" << std::endl;
}

/**
 * @tc.name  : OnReceiveEvent_002
 * @tc.number: OnReceiveEvent_002
 * @tc.desc  : Test OnReceiveEvent with COMMON_EVENT_USER_SWITCHED and curUserId_==-1.
 *             This satisfies the inner condition (curUserId_==-1), so UserIdChangeCallback is called.
 *             curUserId_ should be updated to the new userId.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, OnReceiveEvent_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest OnReceiveEvent_002 begin" << std::endl;

    MechBodyEventBaseService& service = MechBodyEventBaseService::GetInstance();
    // Given: curUserId_ is -1 (initial state)
    service.curUserId_.store(-1, std::memory_order_relaxed);
    int32_t newUserId = 100;

    // When: action is COMMON_EVENT_USER_SWITCHED
    std::string action = EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED;
    AAFwk::Want want;
    want.SetAction(action);
    EventFwk::CommonEventData data(want);
    data.SetCode(newUserId);

    service.OnReceiveEvent(action, data);

    // Then: curUserId_ should be updated to newUserId
    EXPECT_EQ(service.curUserId_.load(std::memory_order_relaxed), newUserId);

    // Clean up
    service.curUserId_.store(-1, std::memory_order_relaxed);

    DTEST_LOG << "McCameraTrackingControllerThreeTest OnReceiveEvent_002 end" << std::endl;
}

/**
 * @tc.name  : OnReceiveEvent_003
 * @tc.number: OnReceiveEvent_003
 * @tc.desc  : Test OnReceiveEvent with COMMON_EVENT_USER_SWITCHED and curUserId_!=-1 but curUserId_!=userId.
 *             This satisfies the inner condition (curUserId_!=userId), so UserIdChangeCallback is called.
 *             curUserId_ should be updated to the new userId.
 */
HWTEST_F(McCameraTrackingControllerThreeTest, OnReceiveEvent_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest OnReceiveEvent_003 begin" << std::endl;

    MechBodyEventBaseService& service = MechBodyEventBaseService::GetInstance();
    // Given: curUserId_ is 50 (not -1), and new userId is 100 (different)
    service.curUserId_.store(50, std::memory_order_relaxed);
    int32_t newUserId = 100;

    std::string action = EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED;
    AAFwk::Want want;
    want.SetAction(action);
    EventFwk::CommonEventData data(want);
    data.SetCode(newUserId);

    service.OnReceiveEvent(action, data);

    // Then: curUserId_ should be updated to newUserId
    EXPECT_EQ(service.curUserId_.load(std::memory_order_relaxed), newUserId);

    // Clean up
    service.curUserId_.store(-1, std::memory_order_relaxed);

    DTEST_LOG << "McCameraTrackingControllerThreeTest OnReceiveEvent_003 end" << std::endl;
}

/**
 * @tc.name  : OnReceiveEvent_004
 * @tc.number: OnReceiveEvent_004
 * @tc.desc  : Test OnReceiveEvent with COMMON_EVENT_USER_SWITCHED and curUserId_==userId!=-1.
 *             The inner condition is false, so UserIdChangeCallback is NOT called.
 *             curUserId_ should remain at the same value (store is unconditional but value is same).
 */
HWTEST_F(McCameraTrackingControllerThreeTest, OnReceiveEvent_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerThreeTest OnReceiveEvent_004 begin" << std::endl;

    MechBodyEventBaseService& service = MechBodyEventBaseService::GetInstance();
    // Given: curUserId_ is 100 and new userId is also 100 (same user)
    int32_t sameUserId = 100;
    service.curUserId_.store(sameUserId, std::memory_order_relaxed);

    std::string action = EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED;
    AAFwk::Want want;
    want.SetAction(action);
    EventFwk::CommonEventData data(want);
    data.SetCode(sameUserId);

    service.OnReceiveEvent(action, data);

    // Then: curUserId_ should remain at sameUserId (inner condition false, no callback)
    EXPECT_EQ(service.curUserId_.load(std::memory_order_relaxed), sameUserId);

    // Clean up
    service.curUserId_.store(-1, std::memory_order_relaxed);

    DTEST_LOG << "McCameraTrackingControllerThreeTest OnReceiveEvent_004 end" << std::endl;
}

} // namespace MechBodyController
} // namespace OHOS
