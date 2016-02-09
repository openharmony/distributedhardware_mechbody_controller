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

#include "connect_manager_test.h"
#include "mc_controller_manager.h"

#include "../test_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string BASEDIR = "/data/service/el1/public/database/MechBodyController";
    const int32_t WAITTIME = 2000;
}

void MechConnectManagerTest::SetUpTestCase()
{
    DTEST_LOG << "MechConnectManagerTest::SetUpTestCase" << std::endl;
    mkdir(BASEDIR.c_str(), (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
}

void MechConnectManagerTest::TearDownTestCase()
{
    DTEST_LOG << "MechConnectManagerTest::TearDownTestCase" << std::endl;
    (void)remove(BASEDIR.c_str());
}

void MechConnectManagerTest::TearDown()
{
    DTEST_LOG << "MechConnectManagerTest::TearDown" << std::endl;
    usleep(WAITTIME);
}

void MechConnectManagerTest::SetUp()
{
    usleep(WAITTIME);
    DTEST_LOG << "MechConnectManagerTest::SetUp" << std::endl;
}

/**
 * @tc.name: Init_001
 * @tc.desc: test Init func
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, Init_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest Init_001 begin" << std::endl;
    MechConnectManager::GetInstance().Init();
    int32_t mechId = 0;
    MechInfo mechInfo;
    bool ret = MechConnectManager::GetInstance().GetMechBasicInfo(mechId, mechInfo);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "MechConnectManagerTest Init_001 end" << std::endl;
}

/**
 * @tc.name: UnInit_001
 * @tc.desc: test UnInit func
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, UnInit_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest UnInit_001 begin" << std::endl;
    MechConnectManager::GetInstance().UnInit();
    int32_t mechId = 0;
    MechInfo mechInfo;
    bool ret = MechConnectManager::GetInstance().GetMechBasicInfo(mechId, mechInfo);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "MechConnectManagerTest UnInit_001 end" << std::endl;
}

/**
 * @tc.name: AddDeviceChangeListener_001
 * @tc.desc: test AddDeviceChangeListener func
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, AddDeviceChangeListener_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest AddDeviceChangeListener_001 begin" << std::endl;
    MechConnectManager::GetInstance().Init();
    std::shared_ptr<IMechConnectListener> listener = nullptr;
    int32_t ret = MechConnectManager::GetInstance().AddDeviceChangeListener(listener);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);

    std::shared_ptr<IMechConnectListener> listenerSecond = std::make_shared<MechConnectListenerImpl>();
    ret = MechConnectManager::GetInstance().AddDeviceChangeListener(listenerSecond);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechConnectManagerTest AddDeviceChangeListener_001 end" << std::endl;
}

/**
 * @tc.name: RemoveDeviceChangeListener_001
 * @tc.desc: test RemoveDeviceChangeListener func
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, RemoveDeviceChangeListener_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest RemoveDeviceChangeListener_001 begin" << std::endl;
    const std::shared_ptr<IMechConnectListener> listener = nullptr;
    int32_t ret = MechConnectManager::GetInstance().RemoveDeviceChangeListener(listener);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);

    const std::shared_ptr<IMechConnectListener> listenerSecond = std::make_shared<MechConnectListenerImpl>();
    ret = MechConnectManager::GetInstance().RemoveDeviceChangeListener(listenerSecond);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "MechConnectManagerTest RemoveDeviceChangeListener_001 end" << std::endl;
}

/**
 * @tc.name: NotifyMechDosconnect_001
 * @tc.desc: test NotifyMechDosconnect func
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, NotifyMechDosconnect_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest NotifyMechDosconnect_001 begin" << std::endl;
    MechInfo mechInfo;
    int32_t ret = MechConnectManager::GetInstance().NotifyMechDisconnect(mechInfo);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechConnectManagerTest NotifyMechDosconnect_001 end" << std::endl;
}

/**
 * @tc.name: GetMechBasicInfo_001
 * @tc.desc: test GetMechBasicInfo func
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechBasicInfo_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechBasicInfo_001 begin" << std::endl;
    int32_t mechId = 0;
    MechInfo mechInfo;
    bool ret = MechConnectManager::GetInstance().GetMechBasicInfo(mechId, mechInfo);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "MechConnectManagerTest GetMechBasicInfo_001 end" << std::endl;
}

/**
 * @tc.name: GetMechBasicInfo_002
 * @tc.desc: test GetMechBasicInfo func
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechBasicInfo_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechBasicInfo_002 begin" << std::endl;
    const std::shared_ptr<IMechConnectListener> listener = nullptr;
    MechInfo mechInfo;
    MechConnectManager::GetInstance().mechInfos_.insert(mechInfo);

    int32_t mechId = 0;
    int32_t ret = MechConnectManager::GetInstance().GetMechBasicInfo(mechId, mechInfo);
    EXPECT_EQ(ret, true);
    DTEST_LOG << "MechConnectManagerTest GetMechBasicInfo_002 end" << std::endl;
}

/**
 * @tc.name: GetConnectMechList_001
 * @tc.desc: test GetConnectMechList func
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetConnectMechList_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetConnectMechList_001 begin" << std::endl;
    MechInfo mechInfo;
    MechConnectManager::GetInstance().mechInfos_.insert(mechInfo);

    std::set<MechInfo> mechInfos;
    int32_t ret = MechConnectManager::GetInstance().GetConnectMechList(mechInfos);
    EXPECT_EQ(ret, true);
    DTEST_LOG << "MechConnectManagerTest GetConnectMechList_001 end" << std::endl;
}

/**
 * @tc.name: NotifyMechState_001
 * @tc.desc: test NotifyMechState func
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, NotifyMechState_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest NotifyMechState_001 begin" << std::endl;
    MechConnectManager& mechConnectManager = MechConnectManager::GetInstance();
    MechInfo mechInfo;
    int32_t mechId = 1;
    bool isconnect = false;
    mechInfo.mechId = mechId;
    mechConnectManager.mechInfos_.erase(mechInfo);
    bool ret = mechConnectManager.NotifyMechState(mechId, isconnect);
    EXPECT_EQ(ret, false);

    mechConnectManager.mechInfos_.insert(mechInfo);
    ret = mechConnectManager.NotifyMechState(mechId, isconnect);
    EXPECT_EQ(ret, true);

    isconnect = true;
    ret = mechConnectManager.NotifyMechState(mechId, isconnect);
    EXPECT_EQ(ret, true);
    DTEST_LOG << "MechConnectManagerTest NotifyMechState_001 end" << std::endl;
}

/**
 * @tc.name: GetMechState_001
 * @tc.desc: test GetMechState func
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechState_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechState_001 begin" << std::endl;
    MechConnectManager& mechConnectManager = MechConnectManager::GetInstance();
    MechInfo mechInfo;
    int32_t mechId = 1;
    mechInfo.mechId = mechId;
    mechInfo.state = AttachmentState::ATTACHED;
    mechConnectManager.mechInfos_.erase(mechInfo);
    AttachmentStateMap ret = mechConnectManager.GetMechState(mechId);
    EXPECT_EQ(ret, AttachmentStateMap::UNKNOWN);

    mechConnectManager.mechInfos_.insert(mechInfo);
    ret = mechConnectManager.GetMechState(mechId);
    EXPECT_EQ(ret, AttachmentStateMap::ATTACHED);
    DTEST_LOG << "MechConnectManagerTest GetMechState_001 end" << std::endl;
}
}
}
