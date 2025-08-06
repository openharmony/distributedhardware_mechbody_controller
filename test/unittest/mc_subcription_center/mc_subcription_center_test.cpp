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

#include "mc_subcription_center_test.h"
#define private public
#define protected public
#include "mc_motion_manager.h"
#include "mc_command_factory.h"
#undef private
#undef protected

#include "../test_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string BASEDIR = "/data/service/el1/public/database/MechBodyController";
    const int32_t WAITTIME = 2000;
}

void SubscriptionCenterTest::SetUpTestCase()
{
    DTEST_LOG << "SubscriptionCenterTest::SetUpTestCase" << std::endl;
    mkdir(BASEDIR.c_str(), (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
}

void SubscriptionCenterTest::TearDownTestCase()
{
    DTEST_LOG << "SubscriptionCenterTest::TearDownTestCase" << std::endl;
    (void)remove(BASEDIR.c_str());
}

void SubscriptionCenterTest::TearDown()
{
    DTEST_LOG << "SubscriptionCenterTest::TearDown" << std::endl;
    usleep(WAITTIME);
}

void SubscriptionCenterTest::SetUp()
{
    usleep(WAITTIME);
    DTEST_LOG << "SubscriptionCenterTest::SetUp" << std::endl;
}

/**
 * @tc.name: Subscribe_001
 * @tc.desc: test Subscribe func
 * @tc.type: FUNC
 */
HWTEST_F(SubscriptionCenterTest, Subscribe_001, TestSize.Level3)
{
    DTEST_LOG << "SubscriptionCenterTest Subscribe_001 begin" << std::endl;
    uint16_t type = 0;
    const std::shared_ptr<IMechEventListener> listener = std::make_shared<SubMechEventListener>();
    int32_t ret = SubscriptionCenter::GetInstance().Subscribe(type, listener);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "SubscriptionCenterTest Subscribe_001 end" << std::endl;
}

/**
 * @tc.name: Subscribe_002
 * @tc.desc: test Subscribe func
 * @tc.type: FUNC
 */
HWTEST_F(SubscriptionCenterTest, Subscribe_002, TestSize.Level3)
{
    DTEST_LOG << "SubscriptionCenterTest Subscribe_002 begin" << std::endl;
    uint16_t type = 0;
    const std::shared_ptr<IMechEventListener> listener = std::make_shared<SubMechEventListener>();
    int32_t ret = SubscriptionCenter::GetInstance().Subscribe(type, listener);
    EXPECT_EQ(ret, ERR_OK);

    ret = SubscriptionCenter::GetInstance().Subscribe(type, listener);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "SubscriptionCenterTest Subscribe_002 end" << std::endl;
}

/**
 * @tc.name: UnSubscribe_001
 * @tc.desc: test UnSubscribe func
 * @tc.type: FUNC
 */
HWTEST_F(SubscriptionCenterTest, UnSubscribe_001, TestSize.Level3)
{
    DTEST_LOG << "SubscriptionCenterTest UnSubscribe_001 begin" << std::endl;
    uint16_t type = 0;
    const std::shared_ptr<IMechEventListener> listener = std::make_shared<SubMechEventListener>();
    int32_t ret = SubscriptionCenter::GetInstance().UnSubscribe(type, listener);
    EXPECT_EQ(ret, UNREGISTER_FAILED_ERR);
    DTEST_LOG << "SubscriptionCenterTest UnSubscribe_001 end" << std::endl;
}

/**
 * @tc.name: UnSubscribe_002
 * @tc.desc: test UnSubscribe func
 * @tc.type: FUNC
 */
HWTEST_F(SubscriptionCenterTest, UnSubscribe_002, TestSize.Level3)
{
    DTEST_LOG << "SubscriptionCenterTest UnSubscribe_002 begin" << std::endl;
    uint16_t type = 0;
    const std::shared_ptr<IMechEventListener> listener = std::make_shared<SubMechEventListener>();
    SubscriptionCenter::GetInstance().subscribers_.clear();
    int32_t ret = SubscriptionCenter::GetInstance().UnSubscribe(type, listener);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "SubscriptionCenterTest UnSubscribe_001 end" << std::endl;
}

/**
 * @tc.name: Notify_001
 * @tc.desc: test Notify func
 * @tc.type: FUNC
 */
HWTEST_F(SubscriptionCenterTest, Notify_001, TestSize.Level3)
{
    DTEST_LOG << "SubscriptionCenterTest Notify_001 begin" << std::endl;
    std::shared_ptr<CommandBase> cmd = nullptr;
    int32_t ret = SubscriptionCenter::GetInstance().Notify(cmd);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);

    CommandFactory factory;
    cmd = factory.CreateRegisterMechCameraKeyEventCmd();
    ret = SubscriptionCenter::GetInstance().Notify(cmd);
    EXPECT_EQ(ret, NOTIFICATION_FAILED_ERR);

    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    {
        std::lock_guard<std::mutex> lock(SubscriptionCenter::GetInstance().mutex_);
        std::vector<std::shared_ptr<IMechEventListener>> callbacks;
        callbacks.push_back(motionMgr->mechEventListener_);
        SubscriptionCenter::GetInstance().subscribers_[cmd->GetCmdType()] = callbacks;
    }
    ret = SubscriptionCenter::GetInstance().Notify(cmd);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "SubscriptionCenterTest Notify_001 end" << std::endl;
}

/**
 * @tc.name: Notify_002
 * @tc.desc: test Notify func
 * @tc.type: FUNC
 */
HWTEST_F(SubscriptionCenterTest, Notify_002, TestSize.Level3)
{
    DTEST_LOG << "SubscriptionCenterTest Notify_002 begin" << std::endl;
    CommandFactory factory;
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    motionMgr->RegisterEventListener();
    std::shared_ptr<CommandBase> cmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    {
        std::lock_guard<std::mutex> lock(SubscriptionCenter::GetInstance().mutex_);
        std::vector<std::shared_ptr<IMechEventListener>> callbacks;
        callbacks.push_back(motionMgr->mechEventListener_);
        SubscriptionCenter::GetInstance().subscribers_[cmd->GetCmdType()] = callbacks;
    }
    int32_t ret = SubscriptionCenter::GetInstance().Notify(cmd);
    EXPECT_EQ(ret, ERR_OK);

    cmd = factory.CreateRegisterMechStateInfoCmd();
    {
        std::lock_guard<std::mutex> lock(SubscriptionCenter::GetInstance().mutex_);
        std::vector<std::shared_ptr<IMechEventListener>> callbacks;
        callbacks.push_back(motionMgr->mechEventListener_);
        SubscriptionCenter::GetInstance().subscribers_[cmd->GetCmdType()] = callbacks;
    }
    ret = SubscriptionCenter::GetInstance().Notify(cmd);
    EXPECT_EQ(ret, ERR_OK);

    cmd = factory.CreateRegisterMechControlResultCmd();
    {
        std::lock_guard<std::mutex> lock(SubscriptionCenter::GetInstance().mutex_);
        std::vector<std::shared_ptr<IMechEventListener>> callbacks;
        callbacks.push_back(motionMgr->mechEventListener_);
        SubscriptionCenter::GetInstance().subscribers_[cmd->GetCmdType()] = callbacks;
    }
    ret = SubscriptionCenter::GetInstance().Notify(cmd);
    EXPECT_EQ(ret, ERR_OK);

    cmd = factory.CreateRegisterMechWheelDataCmd();
    {
        std::lock_guard<std::mutex> lock(SubscriptionCenter::GetInstance().mutex_);
        std::vector<std::shared_ptr<IMechEventListener>> callbacks;
        callbacks.push_back(motionMgr->mechEventListener_);
        SubscriptionCenter::GetInstance().subscribers_[cmd->GetCmdType()] = callbacks;
    }
    ret = SubscriptionCenter::GetInstance().Notify(cmd);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "SubscriptionCenterTest Notify_002 end" << std::endl;
}
}
}
