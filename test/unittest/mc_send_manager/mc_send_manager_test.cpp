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
    const int32_t WAITTIME = 2000;
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
    usleep(WAITTIME);
}

void MechSendManagerTest::SetUp()
{
    usleep(WAITTIME);
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
 * @tc.desc: test GetCmdBySeqNo func
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, GetCmdBySeqNo_001, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest GetCmdBySeqNo_001 begin" << std::endl;
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();
    uint16_t seqNo = 1;
    EXPECT_NO_FATAL_FAILURE(transportSendAdapterTest->GetCmdBySeqNo(seqNo));

    CommandFactory factory;
    std::shared_ptr<CommandBase> cmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    transportSendAdapterTest->pendingRequests_[seqNo] = cmd;
    EXPECT_NO_FATAL_FAILURE(transportSendAdapterTest->GetCmdBySeqNo(seqNo));
    DTEST_LOG << "MechSendManagerTest GetCmdBySeqNo_001 end" << std::endl;
}

/**
 * @tc.name: ExeRespTimeoutTask_001
 * @tc.desc: test ExeRespTimeoutTask func
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, ExeRespTimeoutTask_001, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest ExeRespTimeoutTask_001 begin" << std::endl;
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();
    uint16_t seqNo = 1;
    int32_t ret = transportSendAdapterTest->ExeRespTimeoutTask(seqNo);
    EXPECT_EQ(ret, ERR_OK);

    CommandFactory factory;
    std::shared_ptr<CommandBase> cmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    std::shared_ptr<CommandBase> cmdNull = nullptr;
    transportSendAdapterTest->pendingRequests_[seqNo] = cmdNull;
    ret = transportSendAdapterTest->ExeRespTimeoutTask(seqNo);
    EXPECT_EQ(ret, ERR_OK);

    transportSendAdapterTest->pendingRequests_[seqNo] = cmd;
    ret = transportSendAdapterTest->ExeRespTimeoutTask(seqNo);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechSendManagerTest ExeResponseTask_001 end" << std::endl;
}

/**
 * @tc.name: RemoveRespTimeoutTask_001
 * @tc.desc: test RemoveRespTimeoutTask func
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, RemoveRespTimeoutTask_001, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest RemoveRespTimeoutTask_001 begin" << std::endl;
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();
    uint16_t seqNo = 1;
    int32_t ret = transportSendAdapterTest->RemoveRespTimeoutTask(seqNo);
    EXPECT_EQ(ret, ERR_OK);

    CommandFactory factory;
    std::shared_ptr<CommandBase> cmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    transportSendAdapterTest->pendingRequests_[seqNo] = cmd;
    ret = transportSendAdapterTest->RemoveRespTimeoutTask(seqNo);
    EXPECT_EQ(ret, ERR_OK);
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
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();
    bool isAck = true;
    uint16_t seqNo = 1;
    uint32_t dataLen = 4;
    std::shared_ptr<MechDataBuffer> data = std::make_shared<MechDataBuffer>(static_cast<size_t>(dataLen));
    int32_t ret = transportSendAdapterTest->OnReceive(isAck, seqNo, data);
    EXPECT_EQ(ret, ERR_OK);

    isAck = false;
    ret = transportSendAdapterTest->OnReceive(isAck, seqNo, data);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechSendManagerTest TransportSendAdapter_OnReceive_001 end" << std::endl;
}

/**
 * @tc.name: CreateResponseSeqNo_001
 * @tc.desc: test CreateResponseSeqNo func
 * @tc.type: FUNC
 */
HWTEST_F(MechSendManagerTest, CreateResponseSeqNo_001, TestSize.Level3)
{
    DTEST_LOG << "MechSendManagerTest CreateResponseSeqNo_001 begin" << std::endl;
    std::shared_ptr<TransportSendAdapter> transportSendAdapterTest = std::make_shared<TransportSendAdapter>();
    transportSendAdapterTest->lastSeqNo_ = UINT16_MAX;
    int32_t ret = transportSendAdapterTest->CreateResponseSeqNo();
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechSendManagerTest PushResponseTask_001 end" << std::endl;
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
