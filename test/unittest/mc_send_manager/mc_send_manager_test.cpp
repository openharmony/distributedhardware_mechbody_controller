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

#include "mc_send_manager_test.h"
#include "mc_command_base.h"
#include "mc_command_factory.h"

#include "../test_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string BASEDIR = "/data/service/el1/public/database/MechBodyController";
}

void MechSendManagerTest::SetUpTestCase()
{
    DTEST_LOG << "MechSendManagerTest::SetUpTestCase" << std::endl;
    mkdir(BASEDIR.c_str(), (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
}

void MechSendManagerTest::TearDownTestCase()
{
    DTEST_LOG << "MechSendManagerTest::TearDownTestCase" << std::endl;
    (void)remove(BASEDIR.c_str());
}

void MechSendManagerTest::TearDown()
{
    DTEST_LOG << "MechSendManagerTest::TearDown" << std::endl;
}

void MechSendManagerTest::SetUp()
{
    DTEST_LOG << "MechSendManagerTest::SetUp" << std::endl;
}

/**
 * @tc.name: SendCommand_001
 * @tc.desc: test SendCommand func
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, SendCommand_001, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest SendCommand_001 begin" << std::endl;
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();
    std::shared_ptr<CommandBase> cmd = nullptr;
    int32_t ret = transportSendAdapterTest->SendCommand(cmd);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "MechSendManagerTest SendCommand_001 end" << std::endl;
}

/**
 * @tc.name: RegisterBluetoothListener_001
 * @tc.desc: test RegisterBluetoothListener func
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, RegisterBluetoothListener_001, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest RegisterBluetoothListener_001 begin" << std::endl;
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();
    int32_t ret = transportSendAdapterTest->RegisterBluetoothListener();
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechSendManagerTest RegisterBluetoothListener_001 end" << std::endl;
}

/**
 * @tc.name: UnRegisterBluetoothListener_001
 * @tc.desc: test UnRegisterBluetoothListener func
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, UnRegisterBluetoothListener_001, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest UnRegisterBluetoothListener_001 begin" << std::endl;
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();
    int32_t ret = transportSendAdapterTest->UnRegisterBluetoothListener();
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechSendManagerTest UnRegisterBluetoothListener_001 end" << std::endl;
}

/**
 * @tc.name: GetCmdBySeqNo_001
 * @tc.desc: 测试 GetCmdBySeqNo 函数根据序列号获取命令对象
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, GetCmdBySeqNo_001, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest GetCmdBySeqNo_001 begin" << std::endl;

    // Given: 创建 TransportSendAdapter
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();

    // When: 调用 GetCmdBySeqNo 查询不存在的序列号
    uint16_t seqNo = 1;
    std::shared_ptr<CommandBase> result1 = transportSendAdapterTest->GetCmdBySeqNo(seqNo);

    // Then: 返回 nullptr
    EXPECT_EQ(result1, nullptr);

    // When: 通过 PushResponseTask 直接添加命令到 pendingRequests_
    CommandFactory factory;
    std::shared_ptr<CommandBase> cmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    ASSERT_NE(cmd, nullptr);

    int32_t ret = transportSendAdapterTest->PushResponseTask(cmd, seqNo);
    ASSERT_EQ(ret, ERR_OK) << "PushResponseTask should succeed";

    // When: 调用 GetCmdBySeqNo 查询序列号1
    std::shared_ptr<CommandBase> result2 = transportSendAdapterTest->GetCmdBySeqNo(seqNo);

    // Then: 返回对应的命令对象
    ASSERT_NE(result2, nullptr) << "GetCmdBySeqNo should return non-null after PushResponseTask";
    EXPECT_EQ(result2->GetCmdType(), cmd->GetCmdType());

    DTEST_LOG << "MechSendManagerTest GetCmdBySeqNo_001 end" << std::endl;
}

/**
 * @tc.name: ExeRespTimeoutTask_001
 * @tc.desc: 测试 ExeRespTimeoutTask 函数执行响应超时任务
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, ExeRespTimeoutTask_001, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest ExeRespTimeoutTask_001 begin" << std::endl;

    // Given: 创建 TransportSendAdapter
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();

    // When: 调用 ExeRespTimeoutTask 处理不存在的序列号
    uint16_t seqNo = 1;
    int32_t ret = transportSendAdapterTest->ExeRespTimeoutTask(seqNo);

    // Then: 返回成功
    EXPECT_EQ(ret, ERR_OK);

    // When: 通过 PushResponseTask 添加命令到 pendingRequests_
    CommandFactory factory;
    std::shared_ptr<CommandBase> cmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    ASSERT_NE(cmd, nullptr);
    ret = transportSendAdapterTest->PushResponseTask(cmd, seqNo);
    ASSERT_EQ(ret, ERR_OK) << "PushResponseTask should succeed";

    // When: 调用 ExeRespTimeoutTask 处理存在的序列号
    ret = transportSendAdapterTest->ExeRespTimeoutTask(seqNo);

    // Then: 返回成功，且 pending request 被删除
    EXPECT_EQ(ret, ERR_OK);
    std::shared_ptr<CommandBase> result = transportSendAdapterTest->GetCmdBySeqNo(seqNo);
    EXPECT_EQ(result, nullptr);

    DTEST_LOG << "MechSendManagerTest ExeRespTimeoutTask_001 end" << std::endl;
}

/**
 * @tc.name: RemoveRespTimeoutTask_001
 * @tc.desc: 测试 RemoveRespTimeoutTask 函数移除响应超时任务
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, RemoveRespTimeoutTask_001, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest RemoveRespTimeoutTask_001 begin" << std::endl;

    // Given: 创建 TransportSendAdapter
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();

    // When: 调用 RemoveRespTimeoutTask 移除不存在的序列号
    uint16_t seqNo = 1;
    int32_t ret = transportSendAdapterTest->RemoveRespTimeoutTask(seqNo);

    // Then: 返回成功
    EXPECT_EQ(ret, ERR_OK);

    // When: 通过 PushResponseTask 添加命令到 pendingRequests_
    CommandFactory factory;
    std::shared_ptr<CommandBase> cmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    ASSERT_NE(cmd, nullptr);
    ret = transportSendAdapterTest->PushResponseTask(cmd, seqNo);
    ASSERT_EQ(ret, ERR_OK) << "PushResponseTask should succeed";

    // When: 调用 RemoveRespTimeoutTask 移除存在的序列号
    ret = transportSendAdapterTest->RemoveRespTimeoutTask(seqNo);

    // Then: 返回成功，且 pending request 被删除
    EXPECT_EQ(ret, ERR_OK);
    std::shared_ptr<CommandBase> result = transportSendAdapterTest->GetCmdBySeqNo(seqNo);
    EXPECT_EQ(result, nullptr);

    DTEST_LOG << "MechSendManagerTest RemoveRespTimeoutTask_001 end" << std::endl;
}

/**
 * @tc.name: BleReceviceListenerImpl_OnReceive_001
 * @tc.desc: test BleReceviceListenerImpl_OnReceive func
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, BleReceviceListenerImpl_OnReceive_001, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest BleReceviceListenerImpl_OnReceive_001 begin" << std::endl;
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();
    auto receviceListener = std::make_shared<BleReceviceListenerImpl>(transportSendAdapterTest);
    uint8_t static_data[] = {1, 2, 3, 4};
    uint8_t *data = static_data;
    uint32_t dataLen = 4;
    int32_t ret = receviceListener->OnReceive(data, dataLen);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "MechSendManagerTest BleReceviceListenerImpl_OnReceive_001 end" << std::endl;
}

/**
 * @tc.name: BleReceviceListenerImpl_OnReceive_002
 * @tc.desc: test BleReceviceListenerImpl_OnReceive func
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, BleReceviceListenerImpl_OnReceive_002, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest BleReceviceListenerImpl_OnReceive_002 begin" << std::endl;
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();
    auto receviceListener = std::make_shared<BleReceviceListenerImpl>(transportSendAdapterTest);
    uint8_t *data = nullptr;
    uint32_t dataLen = 4;
    int32_t ret = receviceListener->OnReceive(data, dataLen);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "MechSendManagerTest BleReceviceListenerImpl_OnReceive_002 end" << std::endl;
}

/**
 * @tc.name: TransportSendAdapter_OnReceive_001
 * @tc.desc: test TransportSendAdapter_OnReceive func
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, TransportSendAdapter_OnReceive_001, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest TransportSendAdapter_OnReceive_001 begin" << std::endl;

    // Given: 创建 TransportSendAdapter 并通过 PushResponseTask 添加命令
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();
    CommandFactory factory;
    std::shared_ptr<CommandBase> cmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    uint16_t seqNo = 1;
    int32_t ret = transportSendAdapterTest->PushResponseTask(cmd, seqNo);
    ASSERT_EQ(ret, ERR_OK) << "PushResponseTask should succeed";

    // 验证命令已添加到 pendingRequests_
    std::shared_ptr<CommandBase> foundCmd = transportSendAdapterTest->GetCmdBySeqNo(seqNo);
    ASSERT_NE(foundCmd, nullptr) << "Command should be in pendingRequests_ after PushResponseTask";
    EXPECT_EQ(foundCmd->GetCmdType(), cmd->GetCmdType());

    // When: 调用 OnReceive 处理 ACK 响应
    bool isAck = true;
    uint32_t dataLen = 4;
    std::shared_ptr<MechDataBuffer> data = std::make_shared<MechDataBuffer>(static_cast<size_t>(dataLen));
    ret = transportSendAdapterTest->OnReceive(isAck, seqNo, data);

    // Then: 返回成功
    EXPECT_EQ(ret, ERR_OK);

    // 等待异步任务执行完成
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 验证 pending request 被删除（副作用）
    foundCmd = transportSendAdapterTest->GetCmdBySeqNo(seqNo);
    EXPECT_EQ(foundCmd, nullptr) << "Command should be removed from pendingRequests_ after OnReceive with ACK";

    DTEST_LOG << "MechSendManagerTest TransportSendAdapter_OnReceive_001 end" << std::endl;
}

/**
 * @tc.name: CreateResponseSeqNo_001
 * @tc.desc: 测试 CreateResponseSeqNo 函数创建响应序列号
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, CreateResponseSeqNo_001, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest CreateResponseSeqNo_001 begin" << std::endl;

    // Given: 创建 TransportSendAdapter
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();

    // When: 首次调用 CreateResponseSeqNo
    uint16_t seqNo1 = transportSendAdapterTest->CreateResponseSeqNo();

    // Then: 返回序列号1
    EXPECT_EQ(seqNo1, 1);

    // When: 再次调用 CreateResponseSeqNo
    uint16_t seqNo2 = transportSendAdapterTest->CreateResponseSeqNo();

    // Then: 返回递增的序列号
    EXPECT_EQ(seqNo2, 2);

    DTEST_LOG << "MechSendManagerTest CreateResponseSeqNo_001 end" << std::endl;
}

/**
 * @tc.name: PushResponseTask_001
 * @tc.desc: test PushResponseTask func
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, PushResponseTask_001, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest PushResponseTask_001 begin" << std::endl;
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();
    uint16_t seqNo = 1;
    int32_t ret = transportSendAdapterTest->PushResponseTask(nullptr, seqNo);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "MechSendManagerTest PushResponseTask_001 end" << std::endl;
}

/**
 * @tc.name: PushResponseTask_002
 * @tc.desc: test PushResponseTask func
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, PushResponseTask_002, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest PushResponseTask_002 begin" << std::endl;
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();
    CommandFactory factory;
    std::shared_ptr<CommandBase> cmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    uint16_t seqNo = 1;
    transportSendAdapterTest->recvEventHandler_ = nullptr;
    transportSendAdapterTest->RegisterBluetoothListener();
    int32_t ret = transportSendAdapterTest->PushResponseTask(cmd, seqNo);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "MechSendManagerTest PushResponseTask_002 end" << std::endl;
}
}
}
