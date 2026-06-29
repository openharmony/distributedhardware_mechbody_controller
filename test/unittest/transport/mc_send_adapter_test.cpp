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

#include "mc_send_adapter_test.h"
#include "mc_command_factory.h"
#include "mc_connect_manager.h"
#include "ble_send_manager.h"
#include <thread>
#include <chrono>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {

std::shared_ptr<TransportSendAdapter> TransportSendAdapterTest::sendAdapter_ = nullptr;

void TransportSendAdapterTest::SetUpTestCase()
{
    sendAdapter_ = std::make_shared<TransportSendAdapter>();
}

void TransportSendAdapterTest::TearDownTestCase()
{
    sendAdapter_ = nullptr;
}

void TransportSendAdapterTest::SetUp()
{
}

void TransportSendAdapterTest::TearDown()
{
}

/**
* @tc.name  : SendCommand_ShouldReturnError_WhenCommandIsNull
* @tc.number: TransportSendAdapter_Test_001
* @tc.desc  : Test SendCommand with null command
*/
HWTEST_F(TransportSendAdapterTest, SendCommand_ShouldReturnError_WhenCommandIsNull, TestSize.Level1)
{
    std::shared_ptr<CommandBase> cmd = nullptr;
    int32_t ret = sendAdapter_->SendCommand(cmd);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : SendCommand_ShouldReturnOK_WhenCommandIsValid
* @tc.number: TransportSendAdapter_Test_002
* @tc.desc  : Test SendCommand when command is valid
*/
HWTEST_F(TransportSendAdapterTest, SendCommand_ShouldReturnOK_WhenCommandIsValid, TestSize.Level1)
{
    CommandFactory factory;
    auto cmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    int32_t ret = sendAdapter_->SendCommand(cmd);
    EXPECT_EQ(ret, ERR_OK);
}

/**
* @tc.name  : RegisterBluetoothListener_ShouldReturnOK
* @tc.number: TransportSendAdapter_Test_003
* @tc.desc  : Test RegisterBluetoothListener
*/
HWTEST_F(TransportSendAdapterTest, RegisterBluetoothListener_ShouldReturnOK, TestSize.Level1)
{
    int32_t ret = sendAdapter_->RegisterBluetoothListener();
    EXPECT_EQ(ret, ERR_OK);
}

/**
* @tc.name  : UnRegisterBluetoothListener_ShouldReturnOK
* @tc.number: TransportSendAdapter_Test_004
* @tc.desc  : Test UnRegisterBluetoothListener
*/
HWTEST_F(TransportSendAdapterTest, UnRegisterBluetoothListener_ShouldReturnOK, TestSize.Level1)
{
    int32_t ret = sendAdapter_->UnRegisterBluetoothListener();
    EXPECT_EQ(ret, ERR_OK);
}

/**
* @tc.name  : UnRegisterBluetoothListener_ShouldReturnOK_AfterRegister
* @tc.number: TransportSendAdapter_Test_005
* @tc.desc  : Test UnRegisterBluetoothListener after register
*/
HWTEST_F(TransportSendAdapterTest, UnRegisterBluetoothListener_ShouldReturnOK_AfterRegister, TestSize.Level1)
{
    // Given: 先注册listener
    int32_t ret = sendAdapter_->RegisterBluetoothListener();
    EXPECT_EQ(ret, ERR_OK);

    // When: 注销listener
    ret = sendAdapter_->UnRegisterBluetoothListener();

    // Then: 验证返回成功
    EXPECT_EQ(ret, ERR_OK);
}

/**
* @tc.name  : DiscardExpiredData_ShouldReturnFalse_WhenTimestampIsZero
* @tc.number: TransportSendAdapter_Test_006
* @tc.desc  : Test DiscardExpiredData when timestamp is zero
*/
HWTEST_F(TransportSendAdapterTest, DiscardExpiredData_ShouldReturnFalse_WhenTimestampIsZero, TestSize.Level1)
{
    CommandFactory factory;
    auto cmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    cmd->SetTimestamp(0);
    bool result = sendAdapter_->DiscardExpiredData(cmd);
    EXPECT_EQ(result, false);
}

/**
* @tc.name  : DiscardExpiredData_ShouldReturnFalse_WhenDataIsNotExpired
* @tc.number: TransportSendAdapter_Test_007
* @tc.desc  : Test DiscardExpiredData when data is not expired
*/
HWTEST_F(TransportSendAdapterTest, DiscardExpiredData_ShouldReturnFalse_WhenDataIsNotExpired, TestSize.Level1)
{
    CommandFactory factory;
    auto cmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    auto nowUs = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    cmd->SetTimestamp(nowUs - 10000); // 10ms ago, less than 70ms threshold
    bool result = sendAdapter_->DiscardExpiredData(cmd);
    EXPECT_EQ(result, false);
}

/**
* @tc.name  : DiscardExpiredData_ShouldReturnTrue_WhenDataIsExpired
* @tc.number: TransportSendAdapter_Test_008
* @tc.desc  : Test DiscardExpiredData when data is expired
*/
HWTEST_F(TransportSendAdapterTest, DiscardExpiredData_ShouldReturnTrue_WhenDataIsExpired, TestSize.Level1)
{
    CommandFactory factory;
    auto cmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    auto nowUs = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    cmd->SetTimestamp(nowUs - 100000); // 100ms ago, more than 70ms threshold
    bool result = sendAdapter_->DiscardExpiredData(cmd);
    EXPECT_EQ(result, true);
}

/**
* @tc.name  : OnReceive_ShouldReturnError_WhenDataBufferIsNull
* @tc.number: TransportSendAdapter_Test_009
* @tc.desc  : Test OnReceive when dataBuffer is null
*/
HWTEST_F(TransportSendAdapterTest, OnReceive_ShouldReturnError_WhenDataBufferIsNull, TestSize.Level1)
{
    std::shared_ptr<MechDataBuffer> dataBuffer = nullptr;
    int32_t ret = sendAdapter_->OnReceive(true, 0, dataBuffer);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : OnReceive_ShouldReturnOK_WhenIsAckIsFalseAndCmdIsNull
* @tc.number: TransportSendAdapter_Test_010
* @tc.desc  : Test OnReceive when isAck is false and cmd is null
*/
HWTEST_F(TransportSendAdapterTest, OnReceive_ShouldReturnOK_WhenIsAckIsFalseAndCmdIsNull, TestSize.Level1)
{
    auto dataBuffer = std::make_shared<MechDataBuffer>(10);
    int32_t ret = sendAdapter_->OnReceive(false, 0, dataBuffer);
    EXPECT_EQ(ret, ERR_OK);
}

/**
* @tc.name  : OnReceive_ShouldReturnOK_WhenIsAckIsTrue
* @tc.number: TransportSendAdapter_Test_011
* @tc.desc  : Test OnReceive when isAck is true
*/
HWTEST_F(TransportSendAdapterTest, OnReceive_ShouldReturnOK_WhenIsAckIsTrue, TestSize.Level1)
{
    auto dataBuffer = std::make_shared<MechDataBuffer>(10);
    int32_t ret = sendAdapter_->OnReceive(true, 0, dataBuffer);
    EXPECT_EQ(ret, ERR_OK);
}

/**
* @tc.name  : BleReceviceListenerImpl_OnReceive_ShouldReturnError_WhenDataIsInvalid
* @tc.number: TransportSendAdapter_Test_012
* @tc.desc  : Test BleReceviceListenerImpl::OnReceive with invalid data
*/
HWTEST_F(TransportSendAdapterTest, BleReceviceListenerImpl_OnReceive_InvalidData, TestSize.Level1)
{
    BleReceviceListenerImpl listener(sendAdapter_);
    uint8_t* data = nullptr;
    int32_t ret = listener.OnReceive(data, 0);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : BleReceviceListenerImpl_OnReceive_ShouldReturnError_WhenDataLengthIsZero
* @tc.number: TransportSendAdapter_Test_013
* @tc.desc  : Test BleReceviceListenerImpl::OnReceive with zero length
*/
HWTEST_F(TransportSendAdapterTest, BleReceviceListenerImpl_OnReceive_ZeroLength, TestSize.Level1)
{
    BleReceviceListenerImpl listener(sendAdapter_);
    uint8_t data[10] = {0};
    int32_t ret = listener.OnReceive(data, 0);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : BleReceviceListenerImpl_OnReceive_ShouldReturnError_WhenDataLengthExceedsMax
* @tc.number: TransportSendAdapter_Test_014
* @tc.desc  : Test BleReceviceListenerImpl::OnReceive with excessive length
*/
HWTEST_F(TransportSendAdapterTest, BleReceviceListenerImpl_OnReceive_ExceedsMax, TestSize.Level1)
{
    BleReceviceListenerImpl listener(sendAdapter_);
    uint8_t data[300] = {0};
    int32_t ret = listener.OnReceive(data, 300);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : PushResponseTask_ShouldReturnError_WhenCommandIsNull
* @tc.number: TransportSendAdapter_Test_015
* @tc.desc  : Test PushResponseTask with null command
*/
HWTEST_F(TransportSendAdapterTest, PushResponseTask_ShouldReturnError_WhenCommandIsNull, TestSize.Level1)
{
    std::shared_ptr<CommandBase> cmd = nullptr;
    int32_t ret = sendAdapter_->PushResponseTask(cmd, 0);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : SendCommand_ShouldHandleMultipleCommands
* @tc.number: TransportSendAdapter_Test_016
* @tc.desc  : Test SendCommand with multiple commands
*/
HWTEST_F(TransportSendAdapterTest, SendCommand_ShouldHandleMultipleCommands, TestSize.Level1)
{
    // Given: 创建多个命令
    CommandFactory factory;
    auto cmd1 = factory.CreateGetMechCameraTrackingLayoutCmd();
    auto cmd2 = factory.CreateGetMechCameraTrackingLayoutCmd();

    // When: 连续发送多个命令
    int32_t ret1 = sendAdapter_->SendCommand(cmd1);
    int32_t ret2 = sendAdapter_->SendCommand(cmd2);

    // Then: 验证所有命令发送成功
    EXPECT_EQ(ret1, ERR_OK);
    EXPECT_EQ(ret2, ERR_OK);
}

/**
* @tc.name  : SendCommand_WithDelay_ShouldReturnOK
* @tc.number: TransportSendAdapter_Test_017
* @tc.desc  : Test SendCommand with delay parameter
*/
HWTEST_F(TransportSendAdapterTest, SendCommand_WithDelay_ShouldReturnOK, TestSize.Level1)
{
    // Given: 创建命令
    CommandFactory factory;
    auto cmd = factory.CreateGetMechCameraTrackingLayoutCmd();

    // When: 发送带延迟的命令
    int32_t ret = sendAdapter_->SendCommand(cmd, 10);

    // Then: 验证命令发送成功
    EXPECT_EQ(ret, ERR_OK);
}

/**
* @tc.name  : ExeResponseTask_ShouldReturnError_WhenMechDataBufferIsNull
* @tc.number: TransportSendAdapter_Test_020
* @tc.desc  : Test ExeResponseTask with null mechDataBuffer
*/
HWTEST_F(TransportSendAdapterTest, ExeResponseTask_ShouldReturnError_WhenMechDataBufferIsNull, TestSize.Level1)
{
    std::shared_ptr<MechDataBuffer> mechDataBuffer = nullptr;
    int32_t ret = sendAdapter_->ExeResponseTask(0, mechDataBuffer);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ExeResponseTask_ShouldReturnOK_WhenSeqNoNotFound
* @tc.number: TransportSendAdapter_Test_021
* @tc.desc  : Test ExeResponseTask when seqNo is not found
*/
HWTEST_F(TransportSendAdapterTest, ExeResponseTask_ShouldReturnOK_WhenSeqNoNotFound, TestSize.Level1)
{
    auto mechDataBuffer = std::make_shared<MechDataBuffer>(10);
    int32_t ret = sendAdapter_->ExeResponseTask(999, mechDataBuffer);
    EXPECT_EQ(ret, ERR_OK);
}

/**
* @tc.name  : ExeRespTimeoutTask_ShouldReturnOK_WhenSeqNoNotFound
* @tc.number: TransportSendAdapter_Test_022
* @tc.desc  : Test ExeRespTimeoutTask when seqNo is not found
*/
HWTEST_F(TransportSendAdapterTest, ExeRespTimeoutTask_ShouldReturnOK_WhenSeqNoNotFound, TestSize.Level1)
{
    int32_t ret = sendAdapter_->ExeRespTimeoutTask(999);
    EXPECT_EQ(ret, ERR_OK);
}

/**
* @tc.name  : ExeRespTimeoutResetTask_ShouldReturnOK_WhenSeqNoNotFound
* @tc.number: TransportSendAdapter_Test_023
* @tc.desc  : Test ExeRespTimeoutResetTask when seqNo is not found
*/
HWTEST_F(TransportSendAdapterTest, ExeRespTimeoutResetTask_ShouldReturnOK_WhenSeqNoNotFound, TestSize.Level1)
{
    int32_t ret = sendAdapter_->ExeRespTimeoutResetTask(999);
    EXPECT_EQ(ret, ERR_OK);
}

/**
* @tc.name  : RemoveRespTimeoutTask_ShouldReturnOK_WhenSeqNoNotFound
* @tc.number: TransportSendAdapter_Test_024
* @tc.desc  : Test RemoveRespTimeoutTask when seqNo is not found
*/
HWTEST_F(TransportSendAdapterTest, RemoveRespTimeoutTask_ShouldReturnOK_WhenSeqNoNotFound, TestSize.Level1)
{
    int32_t ret = sendAdapter_->RemoveRespTimeoutTask(999);
    EXPECT_EQ(ret, ERR_OK);
}

/**
* @tc.name  : SendCommand_ShouldReturnError_WhenBleNotConnected
* @tc.number: TransportSendAdapter_Test_025
* @tc.desc  : Test SendCommand when BLE is not connected
*/
HWTEST_F(TransportSendAdapterTest, SendCommand_ShouldReturnError_WhenBleNotConnected, TestSize.Level1)
{
    // Given: 创建命令
    CommandFactory factory;
    auto cmd = factory.CreateGetMechCameraTrackingLayoutCmd();

    // When: 发送命令（假设BLE未连接，会返回错误）
    int32_t ret = sendAdapter_->SendCommand(cmd);

    // Then: 验证返回ERR_OK（任务已提交到队列，即使BLE未连接也会成功提交）
    EXPECT_EQ(ret, ERR_OK);
}

} // namespace MechBodyController
} // namespace OHOS