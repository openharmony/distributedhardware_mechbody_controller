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
#include "mechbody_controller_service.h"

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
 * @tc.name: NotifyMechDisconnect_001
 * @tc.desc: test NotifyMechDosconnect func
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, NotifyMechDisconnect_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest NotifyMechDisconnect_001 begin" << std::endl;
    MechInfo mechInfo;
    int32_t ret = MechConnectManager::GetInstance().NotifyMechDisconnect(mechInfo);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechConnectManagerTest NotifyMechDisconnect_001 end" << std::endl;
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
 * @tc.desc: 测试获取已存在的设备基本信息
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechBasicInfo_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechBasicInfo_002 begin" << std::endl;

    // Given: 添加一个设备信息
    MechInfo mechInfo;
    mechInfo.mechId = 100;
    mechInfo.mechName = "TestMech";
    mechInfo.mac = "AA:BB:CC:DD:EE:FF";
    int32_t addRet = MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    ASSERT_EQ(addRet, ERR_OK);

    // When: 根据mechId获取设备基本信息
    MechInfo resultInfo;
    bool ret = MechConnectManager::GetInstance().GetMechBasicInfo(100, resultInfo);

    // Then: 应该返回成功，且获取的信息正确
    EXPECT_TRUE(ret);
    EXPECT_EQ(resultInfo.mechId, 100);
    EXPECT_EQ(resultInfo.mechName, "TestMech");
    EXPECT_EQ(resultInfo.mac, "AA:BB:CC:DD:EE:FF");

    DTEST_LOG << "MechConnectManagerTest GetMechBasicInfo_002 end" << std::endl;
}

/**
 * @tc.name: GetConnectMechList_001
 * @tc.desc: 测试获取已连接的设备列表
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetConnectMechList_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetConnectMechList_001 begin" << std::endl;

    // Given: 清理设备列表并添加两个设备信息
    int32_t cleanRet = MechConnectManager::GetInstance().CleanMechInfo();
    ASSERT_EQ(cleanRet, ERR_OK);

    MechInfo mechInfo1;
    mechInfo1.mechId = 101;
    mechInfo1.mechName = "TestMech1";
    mechInfo1.mac = "AA:BB:CC:DD:EE:01";
    int32_t addRet1 = MechConnectManager::GetInstance().AddMechInfo(mechInfo1);
    ASSERT_EQ(addRet1, ERR_OK);

    MechInfo mechInfo2;
    mechInfo2.mechId = 102;
    mechInfo2.mechName = "TestMech2";
    mechInfo2.mac = "AA:BB:CC:DD:EE:02";
    int32_t addRet2 = MechConnectManager::GetInstance().AddMechInfo(mechInfo2);
    ASSERT_EQ(addRet2, ERR_OK);

    // When: 获取已连接的设备列表
    std::set<MechInfo> mechInfos;
    bool ret = MechConnectManager::GetInstance().GetConnectMechList(mechInfos);

    // Then: 应该返回成功，且设备列表包含添加的设备
    EXPECT_TRUE(ret);
    EXPECT_EQ(mechInfos.size(), 2);

    auto it1 = std::find_if(mechInfos.begin(), mechInfos.end(),
        [](const MechInfo& info) { return info.mechId == 101; });
    ASSERT_NE(it1, mechInfos.end());
    EXPECT_EQ(it1->mechName, "TestMech1");
    EXPECT_EQ(it1->mac, "AA:BB:CC:DD:EE:01");

    auto it2 = std::find_if(mechInfos.begin(), mechInfos.end(),
        [](const MechInfo& info) { return info.mechId == 102; });
    ASSERT_NE(it2, mechInfos.end());
    EXPECT_EQ(it2->mechName, "TestMech2");
    EXPECT_EQ(it2->mac, "AA:BB:CC:DD:EE:02");

    DTEST_LOG << "MechConnectManagerTest GetConnectMechList_001 end" << std::endl;
}

/**
 * @tc.name: NotifyMechState_001
 * @tc.desc: 测试通知设备状态变更
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, NotifyMechState_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest NotifyMechState_001 begin" << std::endl;

    // Given: 添加一个设备信息
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.mechName = "TestMech";
    mechInfo.mac = "AA:BB:CC:DD:EE:03";
    mechInfo.state = AttachmentState::UNKNOWN;
    int32_t addRet = MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    ASSERT_EQ(addRet, ERR_OK);

    // When: 通知设备状态为已连接
    bool ret = MechConnectManager::GetInstance().NotifyMechState(1, true);

    // Then: 应该返回成功
    EXPECT_TRUE(ret);

    // 验证设备状态已更新
    MechInfo resultInfo;
    bool getRet = MechConnectManager::GetInstance().GetMechBasicInfo(1, resultInfo);
    EXPECT_TRUE(getRet);
    EXPECT_EQ(resultInfo.state, AttachmentState::ATTACHED);

    // When: 通知设备状态为已断开
    ret = MechConnectManager::GetInstance().NotifyMechState(1, false);

    // Then: 应该返回成功
    EXPECT_TRUE(ret);

    // 验证设备状态已更新
    getRet = MechConnectManager::GetInstance().GetMechBasicInfo(1, resultInfo);
    EXPECT_TRUE(getRet);
    EXPECT_EQ(resultInfo.state, AttachmentState::DETACHED);

    DTEST_LOG << "MechConnectManagerTest NotifyMechState_001 end" << std::endl;
}

/**
 * @tc.name: GetMechState_001
 * @tc.desc: 测试获取已连接的设备状态
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechState_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechState_001 begin" << std::endl;

    // Given: 清理设备列表并添加一个设备信息设置为已连接状态
    int32_t cleanRet = MechConnectManager::GetInstance().CleanMechInfo();
    ASSERT_EQ(cleanRet, ERR_OK);
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.mechName = "TestMech";
    mechInfo.mac = "AA:BB:CC:DD:EE:04";
    mechInfo.state = AttachmentState::ATTACHED;
    int32_t addRet = MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    ASSERT_EQ(addRet, ERR_OK);

    // When: 获取设备状态
    AttachmentStateMap ret = MechConnectManager::GetInstance().GetMechState(1);

    // Then: 应该返回已连接状态
    EXPECT_EQ(ret, AttachmentStateMap::ATTACHED);

    DTEST_LOG << "MechConnectManagerTest GetMechState_001 end" << std::endl;
}

/**
 * @tc.name: NotifyMechConnect_001
 * @tc.desc: test NotifyMechConnect with normal connection and listener
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, NotifyMechConnect_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest NotifyMechConnect_001 begin" << std::endl;

    // Given: 清理设备列表、MotionManager并初始化连接管理器
    int32_t cleanRet = MechConnectManager::GetInstance().CleanMechInfo();
    ASSERT_EQ(cleanRet, ERR_OK);
    MechBodyControllerService::GetInstance().CleanMotionManagers();
    MechConnectManager::GetInstance().Init();
    std::shared_ptr<IMechConnectListener> listener = std::make_shared<MechConnectListenerImpl>();
    int32_t addRet = MechConnectManager::GetInstance().AddDeviceChangeListener(listener);
    ASSERT_EQ(addRet, ERR_OK);

    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.isFirstConnect = true;
    mechInfo.mechName = "TestMech";
    mechInfo.mac = "AA:BB:CC:DD:EE:05";

    // When: 调用NotifyMechConnect通知设备连接
    int32_t ret = MechConnectManager::GetInstance().NotifyMechConnect(mechInfo);

    // Then: 验证返回值
    // NotifyMechConnect只在OnDeviceConnected返回MECH_CONNECT_FAILED时才返回失败
    // 其他情况（包括MotionManager::Init()返回其他错误码）都返回ERR_OK
    EXPECT_TRUE(ret == ERR_OK || ret == MECH_CONNECT_FAILED);

    // 验证设备信息状态
    MechInfo resultInfo;
    bool getRet = MechConnectManager::GetInstance().GetMechBasicInfo(1, resultInfo);

    // 根据返回值验证设备信息状态的一致性
    // 如果返回ERR_OK，设备信息应该已被添加
    // 如果返回MECH_CONNECT_FAILED，设备信息不应被添加
    if (ret == ERR_OK) {
        EXPECT_TRUE(getRet);
        EXPECT_EQ(resultInfo.mechId, 1);
        EXPECT_EQ(resultInfo.mechName, "TestMech");
        EXPECT_EQ(resultInfo.mac, "AA:BB:CC:DD:EE:05");
    } else {
        EXPECT_FALSE(getRet);
        EXPECT_EQ(resultInfo.mechId, 0);
        EXPECT_EQ(resultInfo.mechName, "");
        EXPECT_EQ(resultInfo.mac, "");
    }

    // 清理：移除监听器
    MechConnectManager::GetInstance().RemoveDeviceChangeListener(listener);

    DTEST_LOG << "MechConnectManagerTest NotifyMechConnect_001 end" << std::endl;
}

/**
 * @tc.name: NotifyMechConnect_002
 * @tc.desc: test NotifyMechConnect with nullptr eventHandler
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, NotifyMechConnect_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest NotifyMechConnect_002 begin" << std::endl;
    // 先调用Init()让OnDeviceConnected()成功创建MotionManager
    MechConnectManager::GetInstance().Init();
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.isFirstConnect = true;
    MechConnectManager::GetInstance().eventHandler_ = nullptr;
    int32_t ret = MechConnectManager::GetInstance().NotifyMechConnect(mechInfo);
    EXPECT_EQ(ret, MECH_CONNECT_FAILED);
    DTEST_LOG << "MechConnectManagerTest NotifyMechConnect_002 end" << std::endl;
}

/**
 * @tc.name: NotifyMechDisconnect_002
 * @tc.desc: test NotifyMechDisconnect with device found and erased
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, NotifyMechDisconnect_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest NotifyMechDisconnect_002 begin" << std::endl;
    // Given: 清理设备列表、MotionManager并初始化连接管理器
    int32_t cleanRet = MechConnectManager::GetInstance().CleanMechInfo();
    ASSERT_EQ(cleanRet, ERR_OK);
    MechBodyControllerService::GetInstance().CleanMotionManagers();
    MechConnectManager::GetInstance().Init();
    std::shared_ptr<IMechConnectListener> listener = std::make_shared<MechConnectListenerImpl>();
    int32_t addRet = MechConnectManager::GetInstance().AddDeviceChangeListener(listener);
    ASSERT_EQ(addRet, ERR_OK);
    // 通过NotifyMechConnect添加设备
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.isFirstConnect = true;
    mechInfo.mechName = "TestMech";
    mechInfo.mac = "AA:BB:CC:DD:EE:05";
    int32_t connectRet = MechConnectManager::GetInstance().NotifyMechConnect(mechInfo);
    // NotifyMechConnect可能返回ERR_OK或MECH_CONNECT_FAILED
    ASSERT_TRUE(connectRet == ERR_OK || connectRet == MECH_CONNECT_FAILED);
    // 验证设备已添加（仅当连接成功时）
    if (connectRet == ERR_OK) {
        MechInfo addedInfo;
        bool getRet = MechConnectManager::GetInstance().GetMechBasicInfo(1, addedInfo);
        ASSERT_TRUE(getRet);
    }
    // When: 调用NotifyMechDisconnect断开设备连接
    int32_t ret = MechConnectManager::GetInstance().NotifyMechDisconnect(mechInfo);
    // Then: 应该返回成功
    EXPECT_EQ(ret, ERR_OK);
    // 验证设备信息已被删除
    MechInfo resultInfo;
    bool getResult = MechConnectManager::GetInstance().GetMechBasicInfo(1, resultInfo);
    EXPECT_FALSE(getResult);
    // 清理：移除监听器
    MechConnectManager::GetInstance().RemoveDeviceChangeListener(listener);
    DTEST_LOG << "MechConnectManagerTest NotifyMechDisconnect_002 end" << std::endl;
}

/**
 * @tc.name: NotifyMechDisconnect_003
 * @tc.desc: test NotifyMechDisconnect with nullptr eventHandler
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, NotifyMechDisconnect_003, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest NotifyMechDisconnect_003 begin" << std::endl;
    MechConnectManager::GetInstance().Init();
    // 直接将eventHandler_设置为nullptr以覆盖对应分支
    MechConnectManager::GetInstance().eventHandler_ = nullptr;
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    int32_t ret = MechConnectManager::GetInstance().NotifyMechDisconnect(mechInfo);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechConnectManagerTest NotifyMechDisconnect_003 end" << std::endl;
}

/**
 * @tc.name: AddDeviceChangeListener_002
 * @tc.desc: test AddDeviceChangeListener with duplicate listener
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, AddDeviceChangeListener_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest AddDeviceChangeListener_002 begin" << std::endl;
    MechConnectManager::GetInstance().Init();
    std::shared_ptr<IMechConnectListener> listener = std::make_shared<MechConnectListenerImpl>();
    int32_t ret1 = MechConnectManager::GetInstance().AddDeviceChangeListener(listener);
    EXPECT_EQ(ret1, ERR_OK);
    int32_t ret2 = MechConnectManager::GetInstance().AddDeviceChangeListener(listener);
    EXPECT_EQ(ret2, ERR_OK);
    DTEST_LOG << "MechConnectManagerTest AddDeviceChangeListener_002 end" << std::endl;
}

/**
 * @tc.name: RemoveDeviceChangeListener_002
 * @tc.desc: test RemoveDeviceChangeListener with normal removal
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, RemoveDeviceChangeListener_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest RemoveDeviceChangeListener_002 begin" << std::endl;
    MechConnectManager::GetInstance().Init();
    std::shared_ptr<IMechConnectListener> listener1 = std::make_shared<MechConnectListenerImpl>();
    std::shared_ptr<IMechConnectListener> listener2 = std::make_shared<MechConnectListenerImpl>();
    MechConnectManager::GetInstance().AddDeviceChangeListener(listener1);
    MechConnectManager::GetInstance().AddDeviceChangeListener(listener2);
    int32_t ret = MechConnectManager::GetInstance().RemoveDeviceChangeListener(listener1);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechConnectManagerTest RemoveDeviceChangeListener_002 end" << std::endl;
}

/**
 * @tc.name: GetConnectMechList_002
 * @tc.desc: test GetConnectMechList with empty list
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetConnectMechList_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetConnectMechList_002 begin" << std::endl;
    MechConnectManager& mechConnectManager = MechConnectManager::GetInstance();
    mechConnectManager.mechInfos_.clear();
    std::set<MechInfo> mechInfos;
    bool ret = mechConnectManager.GetConnectMechList(mechInfos);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "MechConnectManagerTest GetConnectMechList_002 end" << std::endl;
}

/**
 * @tc.name: GetMechState_004
 * @tc.desc: test GetMechState with device not found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechState_004, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechState_004 begin" << std::endl;
    // Given: 初始化连接管理器
    MechConnectManager::GetInstance().Init();
    // When: 调用GetMechState获取不存在的设备状态
    AttachmentStateMap ret = MechConnectManager::GetInstance().GetMechState(999);
    // Then: 应该返回UNKNOWN状态
    EXPECT_EQ(ret, AttachmentStateMap::UNKNOWN);
    DTEST_LOG << "MechConnectManagerTest GetMechState_004 end" << std::endl;
}

/**
 * @tc.name: IsConnect_001
 * @tc.desc: test IsConnect with empty list
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, IsConnect_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest IsConnect_001 begin" << std::endl;
    // Given: 初始化连接管理器（未添加任何设备）
    MechConnectManager::GetInstance().Init();
    // When: 调用IsConnect检查是否有设备连接
    bool ret = MechConnectManager::GetInstance().IsConnect();
    // Then: 应该返回false（无设备连接）
    EXPECT_EQ(ret, false);
    DTEST_LOG << "MechConnectManagerTest IsConnect_001 end" << std::endl;
}

/**
 * @tc.name: IsConnect_002
 * @tc.desc: test IsConnect with non-empty list
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, IsConnect_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest IsConnect_002 begin" << std::endl;
    // Given: 清理设备列表、MotionManager并初始化连接管理器
    int32_t cleanRet = MechConnectManager::GetInstance().CleanMechInfo();
    ASSERT_EQ(cleanRet, ERR_OK);
    MechBodyControllerService::GetInstance().CleanMotionManagers();
    MechConnectManager::GetInstance().Init();
    std::shared_ptr<IMechConnectListener> listener = std::make_shared<MechConnectListenerImpl>();
    int32_t addRet = MechConnectManager::GetInstance().AddDeviceChangeListener(listener);
    ASSERT_EQ(addRet, ERR_OK);
    // 通过NotifyMechConnect添加设备
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.mechType = MechType::PORTABLE_GIMBAL;
    mechInfo.isFirstConnect = true;
    mechInfo.mechName = "TestMech";
    mechInfo.mac = "AA:BB:CC:DD:EE:05";
    int32_t connectRet = MechConnectManager::GetInstance().NotifyMechConnect(mechInfo);
    ASSERT_TRUE(connectRet == ERR_OK || connectRet == MECH_CONNECT_FAILED);
    // 验证设备已添加（仅在连接成功时）
    if (connectRet == ERR_OK) {
        MechInfo addedInfo;
        bool getRet = MechConnectManager::GetInstance().GetMechBasicInfo(1, addedInfo);
        ASSERT_TRUE(getRet);
        // When: 调用IsConnect检查是否有设备连接
        bool ret = MechConnectManager::GetInstance().IsConnect();
        // Then: 应该返回true（有设备连接）
        EXPECT_EQ(ret, true);
    }
    // 清理：移除监听器
    MechConnectManager::GetInstance().RemoveDeviceChangeListener(listener);
    DTEST_LOG << "MechConnectManagerTest IsConnect_002 end" << std::endl;
}

/**
 * @tc.name: AddMechInfo_001
 * @tc.desc: test AddMechInfo with ID conflict
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, AddMechInfo_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest AddMechInfo_001 begin" << std::endl;
    // Given: 初始化连接管理器
    MechConnectManager::GetInstance().Init();
    // 通过AddMechInfo添加第一个设备
    MechInfo mechInfo1;
    mechInfo1.mechId = 1;
    mechInfo1.mechType = MechType::PORTABLE_GIMBAL;
    mechInfo1.mechName = "TestMech1";
    mechInfo1.mac = "AA:BB:CC:DD:EE:FF";
    int32_t ret1 = MechConnectManager::GetInstance().AddMechInfo(mechInfo1);
    ASSERT_EQ(ret1, ERR_OK);
    // 验证第一个设备已添加
    MechInfo addedInfo1;
    bool getRet1 = MechConnectManager::GetInstance().GetMechBasicInfo(1, addedInfo1);
    ASSERT_TRUE(getRet1);
    // When: 再次调用AddMechInfo添加相同mechId的设备（ID冲突）
    MechInfo mechInfo2;
    mechInfo2.mechId = 1;
    mechInfo2.mechType = MechType::PORTABLE_GIMBAL;
    mechInfo2.mechName = "TestMech2";
    mechInfo2.mac = "11:22:33:44:55:66";
    int32_t ret2 = MechConnectManager::GetInstance().AddMechInfo(mechInfo2);
    // Then: 应该返回成功，且mechId会自动递增
    EXPECT_EQ(ret2, ERR_OK);
    // 验证两个设备都已添加
    auto mechInfos = MechConnectManager::GetInstance().GetMechInfos();
    EXPECT_EQ(mechInfos.size(), 2);
    // 验证第二个设备的mechId已自动递增
    MechInfo addedInfo2;
    bool getRet2 = MechConnectManager::GetInstance().GetMechBasicInfo(2, addedInfo2);
    EXPECT_TRUE(getRet2);
    EXPECT_EQ(addedInfo2.mechName, "TestMech2");
    DTEST_LOG << "MechConnectManagerTest AddMechInfo_001 end" << std::endl;
}

/**
 * @tc.name: AddMechInfo_002
 * @tc.desc: test AddMechInfo with normal addition
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, AddMechInfo_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest AddMechInfo_002 begin" << std::endl;
    // Given: 清理设备列表并初始化连接管理器
    int32_t cleanRet = MechConnectManager::GetInstance().CleanMechInfo();
    ASSERT_EQ(cleanRet, ERR_OK);
    MechConnectManager::GetInstance().Init();
    // When: 调用AddMechInfo添加设备
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.mechType = MechType::PORTABLE_GIMBAL;
    mechInfo.mechName = "TestMech";
    mechInfo.mac = "AA:BB:CC:DD:EE:FF";
    int32_t ret = MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    // Then: 应该返回成功
    EXPECT_EQ(ret, ERR_OK);
    // 验证设备已添加
    auto mechInfos = MechConnectManager::GetInstance().GetMechInfos();
    EXPECT_EQ(mechInfos.size(), 1);
    MechInfo addedInfo;
    bool getRet = MechConnectManager::GetInstance().GetMechBasicInfo(1, addedInfo);
    EXPECT_TRUE(getRet);
    EXPECT_EQ(addedInfo.mechName, "TestMech");
    DTEST_LOG << "MechConnectManagerTest AddMechInfo_002 end" << std::endl;
}

/**
 * @tc.name: RemoveMechInfoByMac_001
 * @tc.desc: test RemoveMechInfoByMac with device found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, RemoveMechInfoByMac_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest RemoveMechInfoByMac_001 begin" << std::endl;
    // Given: 清理设备列表并初始化连接管理器
    int32_t cleanRet = MechConnectManager::GetInstance().CleanMechInfo();
    ASSERT_EQ(cleanRet, ERR_OK);
    MechConnectManager::GetInstance().Init();
    // 通过AddMechInfo添加设备
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.mechType = MechType::PORTABLE_GIMBAL;
    mechInfo.mechName = "TestMech";
    mechInfo.mac = "AA:BB:CC:DD:EE:FF";
    int32_t addRet = MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    ASSERT_EQ(addRet, ERR_OK);
    // 验证设备已添加
    MechInfo addedInfo;
    bool getRet = MechConnectManager::GetInstance().GetMechBasicInfo(1, addedInfo);
    ASSERT_TRUE(getRet);
    // When: 调用RemoveMechInfoByMac删除设备
    std::string mac = "AA:BB:CC:DD:EE:FF";
    int32_t ret = MechConnectManager::GetInstance().RemoveMechInfoByMac(mac);
    // Then: 应该返回成功
    EXPECT_EQ(ret, ERR_OK);
    // 验证设备已被删除
    auto mechInfos = MechConnectManager::GetInstance().GetMechInfos();
    EXPECT_EQ(mechInfos.size(), 0);
    MechInfo resultInfo;
    bool getResult = MechConnectManager::GetInstance().GetMechBasicInfo(1, resultInfo);
    EXPECT_FALSE(getResult);
    DTEST_LOG << "MechConnectManagerTest RemoveMechInfoByMac_001 end" << std::endl;
}

/**
 * @tc.name: CleanMechInfo_001
 * @tc.desc: test CleanMechInfo functionality
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, CleanMechInfo_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest CleanMechInfo_001 begin" << std::endl;
    // Given: 清理设备列表并初始化连接管理器
    int32_t cleanRet = MechConnectManager::GetInstance().CleanMechInfo();
    ASSERT_EQ(cleanRet, ERR_OK);
    MechConnectManager::GetInstance().Init();
    // 通过AddMechInfo添加多个设备
    MechInfo mechInfo1;
    mechInfo1.mechId = 1;
    mechInfo1.mechType = MechType::PORTABLE_GIMBAL;
    mechInfo1.mechName = "TestMech1";
    mechInfo1.mac = "AA:BB:CC:DD:EE:FF";
    int32_t ret1 = MechConnectManager::GetInstance().AddMechInfo(mechInfo1);
    ASSERT_EQ(ret1, ERR_OK);

    MechInfo mechInfo2;
    mechInfo2.mechId = 2;
    mechInfo2.mechType = MechType::PORTABLE_GIMBAL;
    mechInfo2.mechName = "TestMech2";
    mechInfo2.mac = "11:22:33:44:55:66";
    int32_t ret2 = MechConnectManager::GetInstance().AddMechInfo(mechInfo2);
    ASSERT_EQ(ret2, ERR_OK);
    // 验证设备已添加
    auto mechInfos = MechConnectManager::GetInstance().GetMechInfos();
    ASSERT_EQ(mechInfos.size(), 2);
    // When: 调用CleanMechInfo清空所有设备信息
    int32_t ret = MechConnectManager::GetInstance().CleanMechInfo();
    // Then: 应该返回成功
    EXPECT_EQ(ret, ERR_OK);
    // 验证所有设备已被清空
    auto resultInfos = MechConnectManager::GetInstance().GetMechInfos();
    EXPECT_EQ(resultInfos.size(), 0);
    DTEST_LOG << "MechConnectManagerTest CleanMechInfo_001 end" << std::endl;
}

/**
 * @tc.name: GetMechInfo_001
 * @tc.desc: test GetMechInfo with device found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechInfo_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechInfo_001 begin" << std::endl;
    MechConnectManager& mechConnectManager = MechConnectManager::GetInstance();
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.mac = "AA:BB:CC:DD:EE:FF";
    mechConnectManager.mechInfos_.insert(mechInfo);
    
    std::string mac = "AA:BB:CC:DD:EE:FF";
    MechInfo resultInfo;
    int32_t ret = mechConnectManager.GetMechInfo(mac, resultInfo);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(resultInfo.mechId, 1);
    DTEST_LOG << "MechConnectManagerTest GetMechInfo_001 end" << std::endl;
}

/**
 * @tc.name: GetMechInfo_002
 * @tc.desc: test GetMechInfo with device not found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechInfo_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechInfo_002 begin" << std::endl;
    MechConnectManager& mechConnectManager = MechConnectManager::GetInstance();
    mechConnectManager.mechInfos_.clear();
    
    std::string mac = "AA:BB:CC:DD:EE:FF";
    MechInfo resultInfo;
    int32_t ret = mechConnectManager.GetMechInfo(mac, resultInfo);
    EXPECT_EQ(ret, MECH_INFO_NOT_FOUND);
    DTEST_LOG << "MechConnectManagerTest GetMechInfo_002 end" << std::endl;
}

/**
 * @tc.name: SetRealMechName_001
 * @tc.desc: test SetRealMechName with empty name
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, SetRealMechName_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest SetRealMechName_001 begin" << std::endl;
    // Given: 初始化连接管理器
    MechConnectManager::GetInstance().Init();
    // 通过AddMechInfo添加设备
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.mechType = MechType::PORTABLE_GIMBAL;
    mechInfo.mechName = "OriginalName";
    mechInfo.mac = "AA:BB:CC:DD:EE:FF";
    int32_t ret = MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    ASSERT_EQ(ret, ERR_OK);
    // 验证设备已添加
    MechInfo addedInfo;
    bool getRet = MechConnectManager::GetInstance().GetMechBasicInfo(1, addedInfo);
    ASSERT_TRUE(getRet);
    ASSERT_EQ(addedInfo.mechName, "OriginalName");
    // When: 调用SetRealMechName设置空名称
    std::string emptyName = "";
    MechConnectManager::GetInstance().SetRealMechName(1, emptyName);
    // Then: 名称应该保持不变（空名称不会更新）
    MechInfo resultInfo;
    bool getResult = MechConnectManager::GetInstance().GetMechBasicInfo(1, resultInfo);
    EXPECT_TRUE(getResult);
    EXPECT_EQ(resultInfo.mechName, "OriginalName");
    DTEST_LOG << "MechConnectManagerTest SetRealMechName_001 end" << std::endl;
}

/**
 * @tc.name: SetRealMechName_002
 * @tc.desc: test SetRealMechName with normal name
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, SetRealMechName_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest SetRealMechName_002 begin" << std::endl;
    // Given: 初始化连接管理器
    MechConnectManager::GetInstance().Init();
    // 通过AddMechInfo添加设备
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.mechType = MechType::PORTABLE_GIMBAL;
    mechInfo.mechName = "OriginalName";
    mechInfo.mac = "AA:BB:CC:DD:EE:FF";
    int32_t ret = MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    ASSERT_EQ(ret, ERR_OK);
    // 验证设备已添加
    MechInfo addedInfo;
    bool getRet = MechConnectManager::GetInstance().GetMechBasicInfo(1, addedInfo);
    ASSERT_TRUE(getRet);
    ASSERT_EQ(addedInfo.mechName, "OriginalName");
    // When: 调用SetRealMechName设置新名称
    std::string newName = "TestDevice";
    MechConnectManager::GetInstance().SetRealMechName(1, newName);
    // Then: 名称应该被更新
    MechInfo resultInfo;
    bool getResult = MechConnectManager::GetInstance().GetMechBasicInfo(1, resultInfo);
    EXPECT_TRUE(getResult);
    EXPECT_EQ(resultInfo.mechName, "TestDevice");
    DTEST_LOG << "MechConnectManagerTest SetRealMechName_002 end" << std::endl;
}

/**
 * @tc.name: SetMechType_001
 * @tc.desc: test SetMechType functionality
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, SetMechType_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest SetMechType_001 begin" << std::endl;
    // Given: 清理设备列表并初始化连接管理器
    int32_t cleanRet = MechConnectManager::GetInstance().CleanMechInfo();
    ASSERT_EQ(cleanRet, ERR_OK);
    MechConnectManager::GetInstance().Init();
    // 通过AddMechInfo添加设备
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.mechType = MechType::UNKNOWN;
    mechInfo.mechName = "TestMech";
    mechInfo.mac = "AA:BB:CC:DD:EE:FF";
    int32_t ret = MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    ASSERT_EQ(ret, ERR_OK);
    // 验证设备已添加
    MechInfo addedInfo;
    bool getRet = MechConnectManager::GetInstance().GetMechBasicInfo(1, addedInfo);
    ASSERT_TRUE(getRet);
    ASSERT_EQ(addedInfo.mechType, MechType::UNKNOWN);
    // When: 调用SetMechType设置设备类型
    MechConnectManager::GetInstance().SetMechType(1, MechType::PORTABLE_GIMBAL);
    // Then: 设备类型应该被更新
    MechInfo resultInfo;
    bool getResult = MechConnectManager::GetInstance().GetMechBasicInfo(1, resultInfo);
    EXPECT_TRUE(getResult);
    EXPECT_EQ(resultInfo.mechType, MechType::PORTABLE_GIMBAL);
    DTEST_LOG << "MechConnectManagerTest SetMechType_001 end" << std::endl;
}

/**
 * @tc.name: SetMechanicGattState_001
 * @tc.desc: test SetMechanicGattState with device found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, SetMechanicGattState_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest SetMechanicGattState_001 begin" << std::endl;
    // Given: 初始化连接管理器
    MechConnectManager::GetInstance().Init();
    // 通过AddMechInfo添加设备
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.mechType = MechType::PORTABLE_GIMBAL;
    mechInfo.mechName = "TestMech";
    mechInfo.mac = "AA:BB:CC:DD:EE:FF";
    mechInfo.gattCoonectState = false;
    int32_t ret = MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    ASSERT_EQ(ret, ERR_OK);
    // 验证设备已添加
    MechInfo addedInfo;
    bool getRet = MechConnectManager::GetInstance().GetMechBasicInfo(1, addedInfo);
    ASSERT_TRUE(getRet);
    ASSERT_EQ(addedInfo.gattCoonectState, false);
    // When: 调用SetMechanicGattState设置GATT状态
    std::string mac = "AA:BB:CC:DD:EE:FF";
    int32_t setRet = MechConnectManager::GetInstance().SetMechanicGattState(mac, true);
    // Then: 应该返回成功
    EXPECT_EQ(setRet, ERR_OK);
    // 验证GATT状态已更新
    MechInfo resultInfo;
    bool getResult = MechConnectManager::GetInstance().GetMechBasicInfo(1, resultInfo);
    EXPECT_TRUE(getResult);
    EXPECT_EQ(resultInfo.gattCoonectState, true);
    DTEST_LOG << "MechConnectManagerTest SetMechanicGattState_001 end" << std::endl;
}

/**
 * @tc.name: SetMechanicGattState_002
 * @tc.desc: test SetMechanicGattState with device not found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, SetMechanicGattState_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest SetMechanicGattState_002 begin" << std::endl;
    // Given: 清理设备列表并初始化连接管理器
    int32_t cleanRet = MechConnectManager::GetInstance().CleanMechInfo();
    ASSERT_EQ(cleanRet, ERR_OK);
    MechConnectManager::GetInstance().Init();
    // When: 调用SetMechanicGattState设置不存在的设备的GATT状态
    std::string mac = "AA:BB:CC:DD:EE:FF";
    int32_t ret = MechConnectManager::GetInstance().SetMechanicGattState(mac, true);
    // Then: 应该返回设备未找到错误
    EXPECT_EQ(ret, MECH_INFO_NOT_FOUND);
    DTEST_LOG << "MechConnectManagerTest SetMechanicGattState_002 end" << std::endl;
}

/**
 * @tc.name: GetMechanicGattState_001
 * @tc.desc: test GetMechanicGattState with device found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechanicGattState_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechanicGattState_001 begin" << std::endl;
    // Given: 初始化连接管理器
    MechConnectManager::GetInstance().Init();
    // 通过AddMechInfo添加设备
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.mechType = MechType::PORTABLE_GIMBAL;
    mechInfo.mechName = "TestMech";
    mechInfo.mac = "AA:BB:CC:DD:EE:FF";
    mechInfo.gattCoonectState = true;
    int32_t ret = MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    ASSERT_EQ(ret, ERR_OK);
    // When: 调用GetMechanicGattState获取GATT状态
    std::string mac = "AA:BB:CC:DD:EE:FF";
    bool state = false;
    int32_t getRet = MechConnectManager::GetInstance().GetMechanicGattState(mac, state);
    // Then: 应该返回成功且状态为true
    EXPECT_EQ(getRet, ERR_OK);
    EXPECT_EQ(state, true);
    DTEST_LOG << "MechConnectManagerTest GetMechanicGattState_001 end" << std::endl;
}

/**
 * @tc.name: GetMechanicGattState_002
 * @tc.desc: test GetMechanicGattState with device not found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechanicGattState_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechanicGattState_002 begin" << std::endl;
    // Given: 清理设备列表并初始化连接管理器
    int32_t cleanRet = MechConnectManager::GetInstance().CleanMechInfo();
    ASSERT_EQ(cleanRet, ERR_OK);
    MechConnectManager::GetInstance().Init();
    // When: 调用GetMechanicGattState获取不存在的设备的GATT状态
    std::string mac = "AA:BB:CC:DD:EE:FF";
    bool state = false;
    int32_t ret = MechConnectManager::GetInstance().GetMechanicGattState(mac, state);
    // Then: 应该返回设备未找到错误
    EXPECT_EQ(ret, MECH_INFO_NOT_FOUND);
    DTEST_LOG << "MechConnectManagerTest GetMechanicGattState_002 end" << std::endl;
}

/**
 * @tc.name: SetMechanicPairState_001
 * @tc.desc: test SetMechanicPairState with device found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, SetMechanicPairState_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest SetMechanicPairState_001 begin" << std::endl;
    // Given: 初始化连接管理器
    MechConnectManager::GetInstance().Init();
    // 通过AddMechInfo添加设备
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.mechType = MechType::PORTABLE_GIMBAL;
    mechInfo.mechName = "TestMech";
    mechInfo.mac = "AA:BB:CC:DD:EE:FF";
    mechInfo.pairState = false;
    int32_t ret = MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    ASSERT_EQ(ret, ERR_OK);
    // 验证设备已添加
    MechInfo addedInfo;
    bool getRet = MechConnectManager::GetInstance().GetMechBasicInfo(1, addedInfo);
    ASSERT_TRUE(getRet);
    ASSERT_EQ(addedInfo.pairState, false);
    // When: 调用SetMechanicPairState设置配对状态
    std::string mac = "AA:BB:CC:DD:EE:FF";
    int32_t setRet = MechConnectManager::GetInstance().SetMechanicPairState(mac, true);
    // Then: 应该返回成功
    EXPECT_EQ(setRet, ERR_OK);
    // 验证配对状态已更新
    MechInfo resultInfo;
    bool getResult = MechConnectManager::GetInstance().GetMechBasicInfo(1, resultInfo);
    EXPECT_TRUE(getResult);
    EXPECT_EQ(resultInfo.pairState, true);
    DTEST_LOG << "MechConnectManagerTest SetMechanicPairState_001 end" << std::endl;
}

/**
 * @tc.name: SetMechanicPairState_002
 * @tc.desc: test SetMechanicPairState with device not found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, SetMechanicPairState_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest SetMechanicPairState_002 begin" << std::endl;
    // Given: 清理设备列表并初始化连接管理器
    int32_t cleanRet = MechConnectManager::GetInstance().CleanMechInfo();
    ASSERT_EQ(cleanRet, ERR_OK);
    MechConnectManager::GetInstance().Init();
    // When: 调用SetMechanicPairState设置不存在的设备的配对状态
    std::string mac = "AA:BB:CC:DD:EE:FF";
    int32_t ret = MechConnectManager::GetInstance().SetMechanicPairState(mac, true);
    // Then: 应该返回设备未找到错误
    EXPECT_EQ(ret, MECH_INFO_NOT_FOUND);
    DTEST_LOG << "MechConnectManagerTest SetMechanicPairState_002 end" << std::endl;
}

/**
 * @tc.name: GetMechanicPairState_001
 * @tc.desc: test GetMechanicPairState with device found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechanicPairState_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechanicPairState_001 begin" << std::endl;
    // Given: 初始化连接管理器
    MechConnectManager::GetInstance().Init();
    // 通过AddMechInfo添加设备
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.mechType = MechType::PORTABLE_GIMBAL;
    mechInfo.mechName = "TestMech";
    mechInfo.mac = "AA:BB:CC:DD:EE:FF";
    mechInfo.pairState = true;
    int32_t ret = MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    ASSERT_EQ(ret, ERR_OK);
    // When: 调用GetMechanicPairState获取配对状态
    std::string mac = "AA:BB:CC:DD:EE:FF";
    bool state = false;
    int32_t getRet = MechConnectManager::GetInstance().GetMechanicPairState(mac, state);
    // Then: 应该返回成功且状态为true
    EXPECT_EQ(getRet, ERR_OK);
    EXPECT_EQ(state, true);
    DTEST_LOG << "MechConnectManagerTest GetMechanicPairState_001 end" << std::endl;
}

/**
 * @tc.name: GetMechanicPairState_002
 * @tc.desc: test GetMechanicPairState with device not found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechanicPairState_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechanicPairState_002 begin" << std::endl;
    // Given: 清理设备列表并初始化连接管理器
    int32_t cleanRet = MechConnectManager::GetInstance().CleanMechInfo();
    ASSERT_EQ(cleanRet, ERR_OK);
    MechConnectManager::GetInstance().Init();
    // When: 调用GetMechanicPairState获取不存在的设备的配对状态
    std::string mac = "AA:BB:CC:DD:EE:FF";
    bool state = false;
    int32_t ret = MechConnectManager::GetInstance().GetMechanicPairState(mac, state);
    // Then: 应该返回设备未找到错误
    EXPECT_EQ(ret, MECH_INFO_NOT_FOUND);
    DTEST_LOG << "MechConnectManagerTest GetMechanicPairState_002 end" << std::endl;
}

/**
 * @tc.name: SetMechanicHidState_001
 * @tc.desc: test SetMechanicHidState with device found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, SetMechanicHidState_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest SetMechanicHidState_001 begin" << std::endl;
    // Given: 初始化连接管理器
    MechConnectManager::GetInstance().Init();
    // 通过AddMechInfo添加设备
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.mechType = MechType::PORTABLE_GIMBAL;
    mechInfo.mechName = "TestMech";
    mechInfo.mac = "AA:BB:CC:DD:EE:FF";
    mechInfo.hidState = false;
    int32_t ret = MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    ASSERT_EQ(ret, ERR_OK);
    // 验证设备已添加
    MechInfo addedInfo;
    bool getRet = MechConnectManager::GetInstance().GetMechBasicInfo(1, addedInfo);
    ASSERT_TRUE(getRet);
    ASSERT_EQ(addedInfo.hidState, false);
    // When: 调用SetMechanicHidState设置HID状态
    std::string mac = "AA:BB:CC:DD:EE:FF";
    int32_t setRet = MechConnectManager::GetInstance().SetMechanicHidState(mac, true);
    // Then: 应该返回成功
    EXPECT_EQ(setRet, ERR_OK);
    // 验证HID状态已更新
    MechInfo resultInfo;
    bool getResult = MechConnectManager::GetInstance().GetMechBasicInfo(1, resultInfo);
    EXPECT_TRUE(getResult);
    EXPECT_EQ(resultInfo.hidState, true);
    DTEST_LOG << "MechConnectManagerTest SetMechanicHidState_001 end" << std::endl;
}

/**
 * @tc.name: SetMechanicHidState_002
 * @tc.desc: test SetMechanicHidState with device not found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, SetMechanicHidState_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest SetMechanicHidState_002 begin" << std::endl;
    // Given: 清理设备列表并初始化连接管理器
    int32_t cleanRet = MechConnectManager::GetInstance().CleanMechInfo();
    ASSERT_EQ(cleanRet, ERR_OK);
    MechConnectManager::GetInstance().Init();
    // When: 调用SetMechanicHidState设置不存在的设备的HID状态
    std::string mac = "AA:BB:CC:DD:EE:FF";
    int32_t ret = MechConnectManager::GetInstance().SetMechanicHidState(mac, true);
    // Then: 应该返回设备未找到错误
    EXPECT_EQ(ret, MECH_INFO_NOT_FOUND);
    DTEST_LOG << "MechConnectManagerTest SetMechanicHidState_002 end" << std::endl;
}

/**
 * @tc.name: GetMechanicHidState_001
 * @tc.desc: test GetMechanicHidState with device found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechanicHidState_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechanicHidState_001 begin" << std::endl;
    // Given: 初始化连接管理器
    MechConnectManager::GetInstance().Init();
    // 通过AddMechInfo添加设备
    MechInfo mechInfo;
    mechInfo.mechId = 1;
    mechInfo.mechType = MechType::PORTABLE_GIMBAL;
    mechInfo.mechName = "TestMech";
    mechInfo.mac = "AA:BB:CC:DD:EE:FF";
    mechInfo.hidState = true;
    int32_t ret = MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    ASSERT_EQ(ret, ERR_OK);
    // When: 调用GetMechanicHidState获取HID状态
    std::string mac = "AA:BB:CC:DD:EE:FF";
    bool state = false;
    int32_t getRet = MechConnectManager::GetInstance().GetMechanicHidState(mac, state);
    // Then: 应该返回成功且状态为true
    EXPECT_EQ(getRet, ERR_OK);
    EXPECT_EQ(state, true);
    DTEST_LOG << "MechConnectManagerTest GetMechanicHidState_001 end" << std::endl;
}

/**
 * @tc.name: GetMechanicHidState_002
 * @tc.desc: test GetMechanicHidState with device not found
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechanicHidState_002, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechanicHidState_002 begin" << std::endl;
    // Given: 清理设备列表并初始化连接管理器
    int32_t cleanRet = MechConnectManager::GetInstance().CleanMechInfo();
    ASSERT_EQ(cleanRet, ERR_OK);
    MechConnectManager::GetInstance().Init();
    // When: 调用GetMechanicHidState获取不存在的设备的HID状态
    std::string mac = "AA:BB:CC:DD:EE:FF";
    bool state = false;
    int32_t ret = MechConnectManager::GetInstance().GetMechanicHidState(mac, state);
    // Then: 应该返回设备未找到错误
    EXPECT_EQ(ret, MECH_INFO_NOT_FOUND);
    DTEST_LOG << "MechConnectManagerTest GetMechanicHidState_002 end" << std::endl;
}

/**
 * @tc.name: UpdateBleStatus_001
 * @tc.desc: test UpdateBleStatus functionality
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, UpdateBleStatus_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest UpdateBleStatus_001 begin" << std::endl;
    bool ret = MechConnectManager::GetInstance().UpdateBleStatus(true);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(MechConnectManager::GetInstance().GetLocalDeviceBleStatus(), true);
    DTEST_LOG << "MechConnectManagerTest UpdateBleStatus_001 end" << std::endl;
}

/**
 * @tc.name: GetLocalDeviceBleStatus_001
 * @tc.desc: test GetLocalDeviceBleStatus functionality
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetLocalDeviceBleStatus_001, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetLocalDeviceBleStatus_001 begin" << std::endl;
    MechConnectManager::GetInstance().UpdateBleStatus(true);
    bool ret = MechConnectManager::GetInstance().GetLocalDeviceBleStatus();
    EXPECT_EQ(ret, true);
    
    MechConnectManager::GetInstance().UpdateBleStatus(false);
    ret = MechConnectManager::GetInstance().GetLocalDeviceBleStatus();
    EXPECT_EQ(ret, false);
    DTEST_LOG << "MechConnectManagerTest GetLocalDeviceBleStatus_001 end" << std::endl;
}

/**
 * @tc.name: GetMechState_NegativeMechId
 * @tc.desc: test GetMechState with negative mechId
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechState_NegativeMechId, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechState_NegativeMechId begin" << std::endl;
    MechConnectManager& mechConnectManager = MechConnectManager::GetInstance();
    mechConnectManager.mechInfos_.clear();
    AttachmentStateMap ret = mechConnectManager.GetMechState(-1);
    EXPECT_EQ(ret, AttachmentStateMap::UNKNOWN);
    DTEST_LOG << "MechConnectManagerTest GetMechState_NegativeMechId end" << std::endl;
}

/**
 * @tc.name: GetMechInfo_EmptyMac
 * @tc.desc: test GetMechInfo with empty MAC address
 * @tc.type: FUNC
 */
HWTEST_F(MechConnectManagerTest, GetMechInfo_EmptyMac, TestSize.Level3)
{
    DTEST_LOG << "MechConnectManagerTest GetMechInfo_EmptyMac begin" << std::endl;
    MechConnectManager& mechConnectManager = MechConnectManager::GetInstance();
    mechConnectManager.mechInfos_.clear();
    
    std::string emptyMac = "";
    MechInfo resultInfo;
    int32_t ret = mechConnectManager.GetMechInfo(emptyMac, resultInfo);
    EXPECT_EQ(ret, MECH_INFO_NOT_FOUND);
    DTEST_LOG << "MechConnectManagerTest GetMechInfo_EmptyMac end" << std::endl;
}
}
}
