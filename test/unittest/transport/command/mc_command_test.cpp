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

#include <gtest/gtest.h>

#include "mc_command_test.h"

#include "../../mechbody_controller_log.h"
#include "../../test_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {
namespace {

void TestCreateFromDataButton(CommandFactory& factory, size_t capacity)
{
    auto bufferButton = std::make_shared<MechDataBuffer>(capacity);
    bufferButton->AppendUint8(0x02);  // cmdSet
    bufferButton->AppendUint8(0x40);  // cmdId
    bufferButton->AppendInt16(0);      // stickX
    bufferButton->AppendInt16(0);      // stickY
    bufferButton->AppendUint16(0);     // controlByte1
    bufferButton->AppendUint16(0);     // controlByte2
    auto cmdButton = factory.CreateFromData(bufferButton);
    ASSERT_NE(cmdButton, nullptr);
    EXPECT_EQ(cmdButton->GetCmdSet(), 0x02);
    EXPECT_EQ(cmdButton->GetCmdId(), 0x40);
}

void TestCreateFromDataAttitude(CommandFactory& factory, size_t capacity)
{
    auto bufferAttitude = std::make_shared<MechDataBuffer>(capacity);
    bufferAttitude->AppendUint8(0x02);  // cmdSet
    bufferAttitude->AppendUint8(0x42);  // cmdId
    bufferAttitude->AppendInt16(0);      // pitch
    bufferAttitude->AppendInt16(0);      // yaw
    bufferAttitude->AppendInt16(0);      // roll
    bufferAttitude->AppendInt16(0);      // reserved1
    bufferAttitude->AppendInt16(0);      // reserved2
    bufferAttitude->AppendInt16(0);      // reserved3
    auto cmdAttitude = factory.CreateFromData(bufferAttitude);
    ASSERT_NE(cmdAttitude, nullptr);
    EXPECT_EQ(cmdAttitude->GetCmdSet(), 0x02);
    EXPECT_EQ(cmdAttitude->GetCmdId(), 0x42);
}

void TestCreateFromDataParam(CommandFactory& factory, size_t capacity)
{
    auto bufferParam = std::make_shared<MechDataBuffer>(capacity);
    bufferParam->AppendUint8(0x02);  // cmdSet
    bufferParam->AppendUint8(0x41);  // cmdId
    bufferParam->AppendUint8(0);      // mechStatus
    bufferParam->AppendUint8(0);      // battery
    bufferParam->AppendUint8(0);      // errorCode
    bufferParam->AppendUint8(0);      // reserved1
    bufferParam->AppendUint8(0);      // reserved2
    auto cmdParam = factory.CreateFromData(bufferParam);
    ASSERT_NE(cmdParam, nullptr);
    EXPECT_EQ(cmdParam->GetCmdSet(), 0x02);
    EXPECT_EQ(cmdParam->GetCmdId(), 0x41);
}

void TestCreateFromDataWheel(CommandFactory& factory, size_t capacity)
{
    auto bufferWheel = std::make_shared<MechDataBuffer>(capacity);
    bufferWheel->AppendUint8(0x02);  // cmdSet
    bufferWheel->AppendUint8(0x44);  // cmdId
    bufferWheel->AppendInt16(0);      // degree
    bufferWheel->AppendInt16(0);      // speed
    bufferWheel->AppendUint8(0);      // speed_ratio
    auto cmdWheel = factory.CreateFromData(bufferWheel);
    ASSERT_NE(cmdWheel, nullptr);
    EXPECT_EQ(cmdWheel->GetCmdSet(), 0x02);
    EXPECT_EQ(cmdWheel->GetCmdId(), 0x44);
}

void TestCreateFromDataTracking(CommandFactory& factory, size_t capacity)
{
    auto bufferTracking = std::make_shared<MechDataBuffer>(capacity);
    bufferTracking->AppendUint8(0x02);  // cmdSet
    bufferTracking->AppendUint8(0x45);  // cmdId
    bufferTracking->AppendUint8(0);      // isEnabled
    auto cmdTracking = factory.CreateFromData(bufferTracking);
    ASSERT_NE(cmdTracking, nullptr);
    EXPECT_EQ(cmdTracking->GetCmdSet(), 0x02);
    EXPECT_EQ(cmdTracking->GetCmdId(), 0x45);
}

}

void MechCommandTest::SetUpTestCase()
{
}

void MechCommandTest::TearDownTestCase()
{
}

void MechCommandTest::SetUp()
{
}

void MechCommandTest::TearDown()
{
}

void SetTimeout()
{
}

uint16_t g_taskIdOldCommand = 0;

/**
* @tc.name  : TriggerTimeout_001
* @tc.number: TriggerTimeout_001
* @tc.desc  : Test TriggerTimeout function with callback.
*/
HWTEST_F(MechCommandTest, TriggerTimeout_001, TestSize.Level1)
{
    // Given: 创建命令对象并设置超时回调
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    
    bool callbackTriggered = false;
    executionResultCmd->SetTimeoutCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });
    
    // When: 触发超时
    executionResultCmd->TriggerTimeout();
    
    // Then: 回调被触发
    EXPECT_TRUE(callbackTriggered);
}

/**
* @tc.name  : GetCmdSet_001
* @tc.number: GetCmdSet_001
* @tc.desc  : Test GetCmdSet function returns correct command set.
*/
HWTEST_F(MechCommandTest, GetCmdSet_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    
    // When: 获取命令集
    uint8_t cmdSet = executionResultCmd->GetCmdSet();
    
    // Then: 返回正确的命令集值
    EXPECT_EQ(cmdSet, RegisterMechControlResultCmd::CMD_SET);
}

/**
* @tc.name  : GetCmdId_001
* @tc.number: GetCmdId_001
* @tc.desc  : Test GetCmdId function returns correct command ID.
*/
HWTEST_F(MechCommandTest, GetCmdId_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    
    // When: 获取命令ID
    uint8_t cmdId = executionResultCmd->GetCmdId();
    
    // Then: 返回正确的命令ID值
    EXPECT_EQ(cmdId, RegisterMechControlResultCmd::CMD_ID);
}

/**
* @tc.name  : GetCmdType_001
* @tc.number: GetCmdType_001
* @tc.desc  : Test GetCmdType function returns correct command type.
*/
HWTEST_F(MechCommandTest, GetCmdType_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    
    // When: 获取命令类型
    uint16_t cmdType = executionResultCmd->GetCmdType();
    
    // Then: 返回正确的命令类型值 (cmdSet << 8 | cmdId)
    uint16_t expectedType = (static_cast<uint16_t>(RegisterMechControlResultCmd::CMD_SET) << 8) |
                            RegisterMechControlResultCmd::CMD_ID;
    EXPECT_EQ(cmdType, expectedType);
}

/**
* @tc.name  : GetReqSize_001
* @tc.number: GetReqSize_001
* @tc.desc  : Test GetReqSize function returns correct request size.
*/
HWTEST_F(MechCommandTest, GetReqSize_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    
    // When: 获取请求大小
    uint16_t reqSize = executionResultCmd->GetReqSize();
    
    // Then: 返回正确的请求大小
    EXPECT_EQ(reqSize, RegisterMechControlResultCmd::REQ_SIZE);
}

/**
* @tc.name  : GetRspSize_001
* @tc.number: GetRspSize_001
* @tc.desc  : Test GetRspSize function returns correct response size.
*/
HWTEST_F(MechCommandTest, GetRspSize_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    
    // When: 获取响应大小
    uint16_t rspSize = executionResultCmd->GetRspSize();
    
    // Then: 返回正确的响应大小
    EXPECT_EQ(rspSize, RegisterMechControlResultCmd::RSP_SIZE);
}

/**
* @tc.name  : GetTimeoutMs_001
* @tc.number: GetTimeoutMs_001
* @tc.desc  : Test GetTimeoutMs function returns correct timeout value.
*/
HWTEST_F(MechCommandTest, GetTimeoutMs_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    
    // When: 获取超时时间
    uint32_t timeoutMs = executionResultCmd->GetTimeoutMs();
    
    // Then: 返回默认超时时间
    EXPECT_EQ(timeoutMs, MECHBODY_MSG_TIMEOUT);
}

/**
* @tc.name  : CreateGetMechCameraTrackingLayoutCmd_001
* @tc.number: CreateGetMechCameraTrackingLayoutCmd_001
* @tc.desc  : Test CreateGetMechCameraTrackingLayoutCmd creates command with correct attributes.
*/
HWTEST_F(MechCommandTest, CreateGetMechCameraTrackingLayoutCmd_001, TestSize.Level1)
{
    // Given: 命令工厂
    CommandFactory factory;
    
    // When: 创建命令
    auto executionCmd = factory.CreateGetMechCameraTrackingLayoutCmd();

    // Then: 命令创建成功且属性正确
    ASSERT_NE(executionCmd, nullptr);
    EXPECT_EQ(executionCmd->GetCmdSet(), GetMechCameraTrackingLayoutCmd::CMD_SET);
    EXPECT_EQ(executionCmd->GetCmdId(), GetMechCameraTrackingLayoutCmd::CMD_ID);
    EXPECT_EQ(executionCmd->GetReqSize(), GetMechCameraTrackingLayoutCmd::REQ_SIZE);
    EXPECT_EQ(executionCmd->GetRspSize(), GetMechCameraTrackingLayoutCmd::RSP_SIZE);
}

/**
* @tc.name  : CreateSetMechCameraInfoCmd_001
* @tc.number: CreateSetMechCameraInfoCmd_001
* @tc.desc  : Test CreateSetMechCameraInfoCmd creates command with correct parameters.
*/
HWTEST_F(MechCommandTest, CreateSetMechCameraInfoCmd_001, TestSize.Level1)
{
    // Given: 命令工厂和参数
    CommandFactory factory;
    CameraInfoParams params;
    params.fovH = 90;
    params.fovV = 60;
    params.cameraType = CameraType::FRONT;
    
    // When: 创建命令
    auto executionCmd = factory.CreateSetMechCameraInfoCmd(params);

    // Then: 命令创建成功且参数正确
    ASSERT_NE(executionCmd, nullptr);
    const CameraInfoParams& retrievedParams = executionCmd->GetParams();
    EXPECT_EQ(retrievedParams.fovH, params.fovH);
    EXPECT_EQ(retrievedParams.fovV, params.fovV);
    EXPECT_EQ(retrievedParams.cameraType, params.cameraType);
}

/**
* @tc.name  : CreateSetMechConfigCmd_001
* @tc.number: CreateSetMechConfigCmd_001
* @tc.desc  : Test CreateSetMechConfigCmd creates command with correct config version.
*/
HWTEST_F(MechCommandTest, CreateSetMechConfigCmd_001, TestSize.Level1)
{
    // Given: 命令工厂和配置版本
    CommandFactory factory;
    uint8_t configVersion = 2;
    
    // When: 创建命令
    auto executionCmd = factory.CreateSetMechConfigCmd(configVersion);

    // Then: 命令创建成功且属性正确
    ASSERT_NE(executionCmd, nullptr);
    EXPECT_EQ(executionCmd->GetCmdSet(), SetMechConfigCmd::CMD_SET);
    EXPECT_EQ(executionCmd->GetCmdId(), SetMechConfigCmd::CMD_ID);
    EXPECT_EQ(executionCmd->GetReqSize(), SetMechConfigCmd::REQ_SIZE);
    EXPECT_EQ(executionCmd->GetRspSize(), SetMechConfigCmd::RSP_SIZE);
}

/**
* @tc.name  : CreateSetMechRotationTraceCmd_001
* @tc.number: CreateSetMechRotationTraceCmd_001
* @tc.desc  : Test CreateSetMechRotationTraceCmd creates command with rotation parameters.
*/
HWTEST_F(MechCommandTest, CreateSetMechRotationTraceCmd_001, TestSize.Level1)
{
    // Given: 命令工厂和旋转参数
    CommandFactory factory;
    std::vector<RotateParam> params;
    RotateParam param;
    param.degree.pitch = 10.0f;
    param.degree.yaw = 20.0f;
    param.duration = 1000;
    params.push_back(param);
    
    // When: 创建命令
    auto executionCmd = factory.CreateSetMechRotationTraceCmd(++g_taskIdOldCommand, params);

    // Then: 命令创建成功且参数正确
    ASSERT_NE(executionCmd, nullptr);
    const std::vector<RotateParam>& retrievedParams = executionCmd->GetParams();
    EXPECT_EQ(retrievedParams.size(), params.size());
    EXPECT_FLOAT_EQ(retrievedParams[0].degree.pitch, params[0].degree.pitch);
    EXPECT_FLOAT_EQ(retrievedParams[0].degree.yaw, params[0].degree.yaw);
    EXPECT_EQ(retrievedParams[0].duration, params[0].duration);
}

/**
* @tc.name  : CreateFromData_001
* @tc.number: CreateFromData_001
* @tc.desc  : Test CreateFromData with various command types.
*/
HWTEST_F(MechCommandTest, CreateFromData_001, TestSize.Level1)
{
    // Given: 命令工厂和有效数据
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x01);
    size_t capacity = 100;

    // When & Then: 测试按钮事件通知
    TestCreateFromDataButton(factory, capacity);

    // When & Then: 测试姿态通知
    TestCreateFromDataAttitude(factory, capacity);

    // When & Then: 测试参数通知
    TestCreateFromDataParam(factory, capacity);

    // When & Then: 测试轮子数据通知
    TestCreateFromDataWheel(factory, capacity);

    // When & Then: 测试跟踪使能通知
    TestCreateFromDataTracking(factory, capacity);
}

/**
* @tc.name  : GetMechCameraTrackingLayoutCmd_Marshal_001
* @tc.number: GetMechCameraTrackingLayoutCmd_Marshal_001
* @tc.desc  : Test Marshal function returns valid buffer.
*/
HWTEST_F(MechCommandTest, GetMechCameraTrackingLayoutCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 序列化命令
    auto buffer = executionCmd->Marshal();
    
    // Then: 返回有效的缓冲区且大小正确
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), GetMechCameraTrackingLayoutCmd::REQ_SIZE + BIT_OFFSET_2);
}

/**
* @tc.name  : GetMechCameraTrackingLayoutCmd_TriggerResponse_001
* @tc.number: GetMechCameraTrackingLayoutCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function with null buffer.
*/
HWTEST_F(MechCommandTest, GetMechCameraTrackingLayoutCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发空缓冲区响应
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    // Then: 结果码为默认值
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : GetMechCameraTrackingLayoutCmd_TriggerResponse_ValidData_001
* @tc.number: GetMechCameraTrackingLayoutCmd_TriggerResponse_ValidData_001
* @tc.desc  : Test TriggerResponse function with valid data.
*/
HWTEST_F(MechCommandTest, GetMechCameraTrackingLayoutCmd_TriggerResponse_ValidData_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效响应数据
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // cmdSet and cmdId (offset 0-1)
    buffer->AppendUint8(0x02);
    buffer->AppendUint8(0x02);
    // result code = 0 (success) at offset 2
    buffer->AppendUint8(0x00);
    // resultType = 0x02
    buffer->AppendUint8(0x02);
    // resultLength = 9
    buffer->AppendUint8(0x09);
    // controlBit = 0x01 (isDefault = true)
    buffer->AppendUint8(0x01);
    // offsetX = 1.0f
    float offsetX = 1.0f;
    buffer->AppendFloat(offsetX);
    // offsetY = 2.0f
    float offsetY = 2.0f;
    buffer->AppendFloat(offsetY);
    
    auto executionCmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发有效响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为成功且参数正确解析
    EXPECT_EQ(executionCmd->GetResult(), 0);
    const LayoutParams& params = executionCmd->GetParams();
    // isDefault is set from (controlBit & BIT_1), where BIT_1 = 0x01
    EXPECT_EQ(params.isDefault, (0x01 & 0x01));
    EXPECT_EQ(params.offsetX, 1.0f);
    EXPECT_EQ(params.offsetY, 2.0f);
}

/**
* @tc.name  : GetMechCameraTrackingLayoutCmd_GetParams_001
* @tc.number: GetMechCameraTrackingLayoutCmd_GetParams_001
* @tc.desc  : Test GetParams function returns default parameters.
*/
HWTEST_F(MechCommandTest, GetMechCameraTrackingLayoutCmd_GetParams_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    ASSERT_NE(executionCmd, nullptr);
    
// When: 获取参数
    const LayoutParams& params = executionCmd->GetParams();
    
    // Then: 返回默认参数
    EXPECT_EQ(params.isDefault, true);
    EXPECT_EQ(params.offsetX, 0);
    EXPECT_EQ(params.offsetY, 0);
}

/**
* @tc.name  : GetMechLimitInfoCmd_TriggerResponse_ValidData_001
* @tc.number: GetMechLimitInfoCmd_TriggerResponse_ValidData_001
* @tc.desc  : Test GetMechLimitInfoCmd TriggerResponse with valid data.
*/
HWTEST_F(MechCommandTest, GetMechLimitInfoCmd_TriggerResponse_ValidData_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效响应数据
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // cmdSet and cmdId (offset 0-1)
    buffer->AppendUint8(0x02);
    buffer->AppendUint8(0x01);
    // result code = 0 (success) at offset 2
    buffer->AppendUint8(0x00);
    // resultType = 0x03 at offset 3
    buffer->AppendUint8(0x03);
    // resultLength = 24 at offset 4
    buffer->AppendUint8(24);
    // 6 floats (24 bytes) for posMax.roll, negMax.roll, posMax.pitch, negMax.pitch, posMax.yaw, negMax.yaw
    for (int i = 0; i < 6; i++) {
        buffer->AppendFloat(0.0f);
    }
    auto executionCmd = factory.CreateGetMechLimitInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发有效响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为成功且参数正确解析
    EXPECT_EQ(executionCmd->GetResult(), 0);
    const RotateDegreeLimit& params = executionCmd->GetParams();
    EXPECT_EQ(params.negMax.pitch, 0);
    EXPECT_EQ(params.negMax.roll, 0);
    EXPECT_EQ(params.negMax.yaw, 0);
}

/**
* @tc.name  : GetMechLimitInfoCmd_GetParams_001
* @tc.number: GetMechLimitInfoCmd_GetParams_001
* @tc.desc  : Test GetMechLimitInfoCmd GetParams returns default parameters.
*/
HWTEST_F(MechCommandTest, GetMechLimitInfoCmd_GetParams_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechLimitInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取参数
    const RotateDegreeLimit& params = executionCmd->GetParams();
    
    // Then: 返回默认参数
    EXPECT_EQ(params.negMax.pitch, 0);
    EXPECT_EQ(params.negMax.roll, 0);
    EXPECT_EQ(params.negMax.yaw, 0);
    EXPECT_EQ(params.posMax.pitch, 0);
    EXPECT_EQ(params.posMax.roll, 0);
    EXPECT_EQ(params.posMax.yaw, 0);
}

/**
* @tc.name  : GetMechLimitInfoCmd_GetResult_001
* @tc.number: GetMechLimitInfoCmd_GetResult_001
* @tc.desc  : Test GetMechLimitInfoCmd GetResult returns default result.
*/
HWTEST_F(MechCommandTest, GetMechLimitInfoCmd_GetResult_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechLimitInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取结果
    uint8_t result = executionCmd->GetResult();
    
    // Then: 返回默认结果
    EXPECT_EQ(result, 0);
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_Marshal_001
* @tc.number: RegisterMechCameraKeyEventCmd_Marshal_001
* @tc.desc  : Test RegisterMechCameraKeyEventCmd Marshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechCameraKeyEventCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechCameraKeyEventCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 序列化命令
    auto buffer = executionCmd->Marshal();
    
    // Then: 返回有效的缓冲区
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_Unmarshal_001
* @tc.number: RegisterMechCameraKeyEventCmd_Unmarshal_001
* @tc.desc  : Test RegisterMechCameraKeyEventCmd Unmarshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechCameraKeyEventCmd_Unmarshal_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 15; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = std::static_pointer_cast<RegisterMechCameraKeyEventCmd>(
        factory.CreateRegisterMechCameraKeyEventCmd());
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 反序列化数据
    bool ret = executionCmd->Unmarshal(buffer);
    
    // Then: 反序列化成功且数据正确解析
    EXPECT_TRUE(ret);
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_TriggerResponse_001
* @tc.number: RegisterMechCameraKeyEventCmd_TriggerResponse_001
* @tc.desc  : Test RegisterMechCameraKeyEventCmd TriggerResponse with null buffer.
*/
HWTEST_F(MechCommandTest, RegisterMechCameraKeyEventCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechCameraKeyEventCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发空缓冲区响应
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    // Then: 结果码为默认值
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_TriggerResponse_ValidData_001
* @tc.number: RegisterMechCameraKeyEventCmd_TriggerResponse_ValidData_001
* @tc.desc  : Test RegisterMechCameraKeyEventCmd TriggerResponse with valid data.
*/
HWTEST_F(MechCommandTest, RegisterMechCameraKeyEventCmd_TriggerResponse_ValidData_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 15; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechCameraKeyEventCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发有效响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为成功
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_GetResult_001
* @tc.number: RegisterMechCameraKeyEventCmd_GetResult_001
* @tc.desc  : Test RegisterMechCameraKeyEventCmd GetResult returns default result.
*/
HWTEST_F(MechCommandTest, RegisterMechCameraKeyEventCmd_GetResult_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechCameraKeyEventCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取结果
    uint8_t result = executionCmd->GetResult();
    
    // Then: 返回默认结果
    EXPECT_EQ(result, 0);
}

/**
* @tc.name  : RegisterMechControlResultCmd_Marshal_001
* @tc.number: RegisterMechControlResultCmd_Marshal_001
* @tc.desc  : Test RegisterMechControlResultCmd Marshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechControlResultCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 序列化命令
    auto buffer = executionCmd->Marshal();
    
    // Then: 返回有效的缓冲区
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), RegisterMechControlResultCmd::REQ_SIZE + BIT_OFFSET_2);
}

/**
* @tc.name  : RegisterMechControlResultCmd_Unmarshal_001
* @tc.number: RegisterMechControlResultCmd_Unmarshal_001
* @tc.desc  : Test RegisterMechControlResultCmd Unmarshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechControlResultCmd_Unmarshal_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 15; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 反序列化数据
    bool ret = executionCmd->Unmarshal(buffer);
    
    // Then: 反序列化成功
    EXPECT_TRUE(ret);
}

/**
* @tc.name  : RegisterMechControlResultCmd_TriggerResponse_001
* @tc.number: RegisterMechControlResultCmd_TriggerResponse_001
* @tc.desc  : Test RegisterMechControlResultCmd TriggerResponse with null buffer.
*/
HWTEST_F(MechCommandTest, RegisterMechControlResultCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发空缓冲区响应
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    // Then: 结果码为默认值
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : RegisterMechControlResultCmd_TriggerResponse_ValidData_001
* @tc.number: RegisterMechControlResultCmd_TriggerResponse_ValidData_001
* @tc.desc  : Test RegisterMechControlResultCmd TriggerResponse with valid data.
*/
HWTEST_F(MechCommandTest, RegisterMechControlResultCmd_TriggerResponse_ValidData_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 15; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发有效响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为成功
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : RegisterMechControlResultCmd_GetControlResult_001
* @tc.number: RegisterMechControlResultCmd_GetControlResult_001
* @tc.desc  : Test RegisterMechControlResultCmd GetControlResult returns default value.
*/
HWTEST_F(MechCommandTest, RegisterMechControlResultCmd_GetControlResult_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取控制结果
    uint8_t controlResult = executionCmd->GetControlResult();
    
    // Then: 返回默认值
    EXPECT_EQ(controlResult, 0);
}

/**
* @tc.name  : RegisterMechControlResultCmd_GetResult_001
* @tc.number: RegisterMechControlResultCmd_GetResult_001
* @tc.desc  : Test RegisterMechControlResultCmd GetResult returns default result.
*/
HWTEST_F(MechCommandTest, RegisterMechControlResultCmd_GetResult_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取结果
    uint8_t result = executionCmd->GetResult();
    
    // Then: 返回默认结果
    EXPECT_EQ(result, 0);
}

/**
* @tc.name  : RegisterMechControlResultCmd_GetTaskId_001
* @tc.number: RegisterMechControlResultCmd_GetTaskId_001
* @tc.desc  : Test RegisterMechControlResultCmd GetTaskId returns default value.
*/
HWTEST_F(MechCommandTest, RegisterMechControlResultCmd_GetTaskId_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取任务ID
    uint8_t taskId = executionCmd->GetTaskId();
    
    // Then: 返回默认值
    EXPECT_EQ(taskId, 0);
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_Marshal_001
* @tc.number: RegisterMechPositionInfoCmd_Marshal_001
* @tc.desc  : Test RegisterMechPositionInfoCmd Marshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechPositionInfoCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechPositionInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 序列化命令
    auto buffer = executionCmd->Marshal();
    
    // Then: 返回有效的缓冲区
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_Unmarshal_001
* @tc.number: RegisterMechPositionInfoCmd_Unmarshal_001
* @tc.desc  : Test RegisterMechPositionInfoCmd Unmarshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechPositionInfoCmd_Unmarshal_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 15; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechPositionInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 反序列化数据
    bool ret = executionCmd->Unmarshal(buffer);
    
    // Then: 反序列化成功
    EXPECT_TRUE(ret);
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_TriggerResponse_001
* @tc.number: RegisterMechPositionInfoCmd_TriggerResponse_001
* @tc.desc  : Test RegisterMechPositionInfoCmd TriggerResponse with null buffer.
*/
HWTEST_F(MechCommandTest, RegisterMechPositionInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechPositionInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发空缓冲区响应
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    // Then: 结果码为默认值
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_TriggerResponse_ValidData_001
* @tc.number: RegisterMechPositionInfoCmd_TriggerResponse_ValidData_001
* @tc.desc  : Test RegisterMechPositionInfoCmd TriggerResponse with valid data.
*/
HWTEST_F(MechCommandTest, RegisterMechPositionInfoCmd_TriggerResponse_ValidData_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // result code = 0 (success) at offset 0
    buffer->AppendUint8(0x00);
    auto executionCmd = factory.CreateRegisterMechPositionInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发有效响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为成功
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_GetPosition_001
* @tc.number: RegisterMechPositionInfoCmd_GetPosition_001
* @tc.desc  : Test RegisterMechPositionInfoCmd GetPosition returns default position.
*/
HWTEST_F(MechCommandTest, RegisterMechPositionInfoCmd_GetPosition_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechPositionInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取位置
    const EulerAngles& position = executionCmd->GetPosition();
    
    // Then: 返回默认位置
    EXPECT_EQ(position.pitch, 0);
    EXPECT_EQ(position.roll, 0);
    EXPECT_EQ(position.yaw, 0);
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_GetResult_001
* @tc.number: RegisterMechPositionInfoCmd_GetResult_001
* @tc.desc  : Test RegisterMechPositionInfoCmd GetResult returns default result.
*/
HWTEST_F(MechCommandTest, RegisterMechPositionInfoCmd_GetResult_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechPositionInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取结果
    uint8_t result = executionCmd->GetResult();
    
    // Then: 返回默认结果
    EXPECT_EQ(result, 0);
}

/**
* @tc.name  : RegisterMechStateInfoCmd_Marshal_001
* @tc.number: RegisterMechStateInfoCmd_Marshal_001
* @tc.desc  : Test RegisterMechStateInfoCmd Marshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechStateInfoCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechStateInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 序列化命令
    auto buffer = executionCmd->Marshal();
    
    // Then: 返回有效的缓冲区
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

/**
* @tc.name  : RegisterMechStateInfoCmd_Unmarshal_001
* @tc.number: RegisterMechStateInfoCmd_Unmarshal_001
* @tc.desc  : Test RegisterMechStateInfoCmd Unmarshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechStateInfoCmd_Unmarshal_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechStateInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 反序列化数据
    bool ret = executionCmd->Unmarshal(buffer);
    
    // Then: 反序列化成功
    EXPECT_TRUE(ret);
}

/**
* @tc.name  : RegisterMechStateInfoCmd_TriggerResponse_001
* @tc.number: RegisterMechStateInfoCmd_TriggerResponse_001
* @tc.desc  : Test RegisterMechStateInfoCmd TriggerResponse with null buffer.
*/
HWTEST_F(MechCommandTest, RegisterMechStateInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechStateInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发空缓冲区响应
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    // Then: 结果码为默认值
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : RegisterMechStateInfoCmd_TriggerResponse_ValidData_001
* @tc.number: RegisterMechStateInfoCmd_TriggerResponse_ValidData_001
* @tc.desc  : Test RegisterMechStateInfoCmd TriggerResponse with valid data.
*/
HWTEST_F(MechCommandTest, RegisterMechStateInfoCmd_TriggerResponse_ValidData_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // result code = 0 (success)
    buffer->AppendUint8(0x00);
    auto executionCmd = factory.CreateRegisterMechStateInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发有效响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为成功
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : RegisterMechStateInfoCmd_GetResult_001
* @tc.number: RegisterMechStateInfoCmd_GetResult_001
* @tc.desc  : Test RegisterMechStateInfoCmd GetResult returns default result.
*/
HWTEST_F(MechCommandTest, RegisterMechStateInfoCmd_GetResult_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechStateInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取结果
    uint8_t result = executionCmd->GetResult();
    
    // Then: 返回默认结果
    EXPECT_EQ(result, 0);
}

/**
* @tc.name  : RegisterMechWheelDataCmd_Marshal_001
* @tc.number: RegisterMechWheelDataCmd_Marshal_001
* @tc.desc  : Test RegisterMechWheelDataCmd Marshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechWheelDataCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechWheelDataCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 序列化命令
    auto buffer = executionCmd->Marshal();
    
    // Then: 返回有效的缓冲区
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

/**
* @tc.name  : RegisterMechWheelDataCmd_Unmarshal_001
* @tc.number: RegisterMechWheelDataCmd_Unmarshal_001
* @tc.desc  : Test RegisterMechWheelDataCmd Unmarshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechWheelDataCmd_Unmarshal_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechWheelDataCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 反序列化数据
    bool ret = executionCmd->Unmarshal(buffer);
    
    // Then: 反序列化成功
    EXPECT_TRUE(ret);
}

/**
* @tc.name  : RegisterMechWheelDataCmd_TriggerResponse_001
* @tc.number: RegisterMechWheelDataCmd_TriggerResponse_001
* @tc.desc  : Test RegisterMechWheelDataCmd TriggerResponse with null buffer.
*/
HWTEST_F(MechCommandTest, RegisterMechWheelDataCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechWheelDataCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发空缓冲区响应
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    // Then: 结果码为默认值
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : RegisterMechWheelDataCmd_TriggerResponse_ValidData_001
* @tc.number: RegisterMechWheelDataCmd_TriggerResponse_ValidData_001
* @tc.desc  : Test RegisterMechWheelDataCmd TriggerResponse with valid data.
*/
HWTEST_F(MechCommandTest, RegisterMechWheelDataCmd_TriggerResponse_ValidData_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // result code = 0 (success)
    buffer->AppendUint8(0x00);
    auto executionCmd = factory.CreateRegisterMechWheelDataCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发有效响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为成功
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : RegisterMechWheelDataCmd_GetWheelData_001
* @tc.number: RegisterMechWheelDataCmd_GetWheelData_001
* @tc.desc  : Test RegisterMechWheelDataCmd GetWheelData returns default data.
*/
HWTEST_F(MechCommandTest, RegisterMechWheelDataCmd_GetWheelData_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechWheelDataCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取轮子数据
    const WheelData& wheelData = executionCmd->GetWheelData();
    
    // Then: 返回默认数据
    EXPECT_EQ(wheelData.degree, 0);
    EXPECT_EQ(wheelData.speed, 0);
    EXPECT_EQ(wheelData.speed_ratio, 0);
}

/**
* @tc.name  : RegisterMechWheelDataCmd_GetResult_001
* @tc.number: RegisterMechWheelDataCmd_GetResult_001
* @tc.desc  : Test RegisterMechWheelDataCmd GetResult returns default result.
*/
HWTEST_F(MechCommandTest, RegisterMechWheelDataCmd_GetResult_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechWheelDataCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取结果
    uint8_t result = executionCmd->GetResult();
    
    // Then: 返回默认结果
    EXPECT_EQ(result, 0);
}

/**
* @tc.name  : SetMechCameraInfoCmd_Marshal_001
* @tc.number: SetMechCameraInfoCmd_Marshal_001
* @tc.desc  : Test SetMechCameraInfoCmd Marshal function.
*/
HWTEST_F(MechCommandTest, SetMechCameraInfoCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建命令对象和参数
    CommandFactory factory;
    CameraInfoParams params;
    params.fovH = 90;
    params.fovV = 60;
    params.cameraType = CameraType::FRONT;
    auto executionCmd = factory.CreateSetMechCameraInfoCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 序列化命令
    auto buffer = executionCmd->Marshal();
    
    // Then: 返回有效的缓冲区
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

/**
* @tc.name  : SetMechCameraInfoCmd_TriggerResponse_001
* @tc.number: SetMechCameraInfoCmd_TriggerResponse_001
* @tc.desc  : Test SetMechCameraInfoCmd TriggerResponse with null buffer.
*/
HWTEST_F(MechCommandTest, SetMechCameraInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建命令对象和参数
    CommandFactory factory;
    CameraInfoParams params;
    params.fovH = 90;
    params.fovV = 60;
    params.cameraType = CameraType::FRONT;
    auto executionCmd = factory.CreateSetMechCameraInfoCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发空缓冲区响应
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    // Then: 验证命令对象仍然有效
    ASSERT_NE(executionCmd, nullptr);
}

/**
* @tc.name  : SetMechCameraInfoCmd_TriggerResponse_ValidData_001
* @tc.number: SetMechCameraInfoCmd_TriggerResponse_ValidData_001
* @tc.desc  : Test SetMechCameraInfoCmd TriggerResponse with valid data.
*/
HWTEST_F(MechCommandTest, SetMechCameraInfoCmd_TriggerResponse_ValidData_001, TestSize.Level1)
{
    // Given: 创建命令对象、参数和有效数据
    CommandFactory factory;
    CameraInfoParams params;
    params.fovH = 90;
    params.fovV = 60;
    params.cameraType = CameraType::FRONT;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateSetMechCameraInfoCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发有效响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 验证命令对象仍然有效
    ASSERT_NE(executionCmd, nullptr);
}

/**
* @tc.name  : SetMechCameraInfoCmd_GetParams_001
* @tc.number: SetMechCameraInfoCmd_GetParams_001
* @tc.desc  : Test SetMechCameraInfoCmd GetParams returns correct parameters.
*/
HWTEST_F(MechCommandTest, SetMechCameraInfoCmd_GetParams_001, TestSize.Level1)
{
    // Given: 创建命令对象和参数
    CommandFactory factory;
    CameraInfoParams params;
    params.fovH = 90;
    params.fovV = 60;
    params.cameraType = CameraType::FRONT;
    auto executionCmd = factory.CreateSetMechCameraInfoCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取参数
    const CameraInfoParams& retrievedParams = executionCmd->GetParams();
    
    // Then: 返回正确的参数
    EXPECT_EQ(retrievedParams.fovH, params.fovH);
    EXPECT_EQ(retrievedParams.fovV, params.fovV);
    EXPECT_EQ(retrievedParams.cameraType, params.cameraType);
}

/**
* @tc.name  : SetMechCameraTrackingFrameCmd_TriggerResponse_001
* @tc.number: SetMechCameraTrackingFrameCmd_TriggerResponse_001
* @tc.desc  : Test SetMechCameraTrackingFrameCmd TriggerResponse with null buffer.
*/
HWTEST_F(MechCommandTest, SetMechCameraTrackingFrameCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    TrackingFrameParams params;
    auto executionCmd = factory.CreateSetMechCameraTrackingFrameCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发空缓冲区响应
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    // Then: 验证命令对象仍然有效
    ASSERT_NE(executionCmd, nullptr);
}

/**
* @tc.name  : SetMechCameraTrackingFrameCmd_TriggerResponse_ValidData_001
* @tc.number: SetMechCameraTrackingFrameCmd_TriggerResponse_ValidData_001
* @tc.desc  : Test SetMechCameraTrackingFrameCmd TriggerResponse with valid data.
*/
HWTEST_F(MechCommandTest, SetMechCameraTrackingFrameCmd_TriggerResponse_ValidData_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    TrackingFrameParams params;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateSetMechCameraTrackingFrameCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发有效响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 验证命令对象仍然有效
    ASSERT_NE(executionCmd, nullptr);
}

/**
* @tc.name  : SetMechCameraTrackingFrameCmd_GetParams_001
* @tc.number: SetMechCameraTrackingFrameCmd_GetParams_001
* @tc.desc  : Test SetMechCameraTrackingFrameCmd GetParams returns correct parameters.
*/
HWTEST_F(MechCommandTest, SetMechCameraTrackingFrameCmd_GetParams_001, TestSize.Level1)
{
    // Given: 创建命令对象和参数
    CommandFactory factory;
    TrackingFrameParams params;
    params.targetId = 1;
    params.timeStamp = 1000;
    params.cameraType = CameraType::FRONT;
    auto executionCmd = factory.CreateSetMechCameraTrackingFrameCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取参数
    const TrackingFrameParams& retrievedParams = executionCmd->GetParams();
    
    // Then: 返回正确的参数
    EXPECT_EQ(retrievedParams.targetId, params.targetId);
    EXPECT_EQ(retrievedParams.timeStamp, params.timeStamp);
    EXPECT_EQ(retrievedParams.cameraType, params.cameraType);
}

/**
* @tc.name  : SetMechCameraTrackingLayoutCmd_TriggerResponse_001
* @tc.number: SetMechCameraTrackingLayoutCmd_TriggerResponse_001
* @tc.desc  : Test SetMechCameraTrackingLayoutCmd TriggerResponse with null buffer.
*/
HWTEST_F(MechCommandTest, SetMechCameraTrackingLayoutCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    LayoutParams params;
    auto executionCmd = factory.CreateSetMechCameraTrackingLayoutCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发空缓冲区响应
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    // Then: 结果码为默认值
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : SetMechCameraTrackingLayoutCmd_TriggerResponse_ValidData_001
* @tc.number: SetMechCameraTrackingLayoutCmd_TriggerResponse_ValidData_001
* @tc.desc  : Test SetMechCameraTrackingLayoutCmd TriggerResponse with valid data.
*/
HWTEST_F(MechCommandTest, SetMechCameraTrackingLayoutCmd_TriggerResponse_ValidData_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    LayoutParams params;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // result code = 0 (success)
    buffer->AppendUint8(0x00);
    buffer->AppendUint8(0x00);
    buffer->AppendUint8(0x00);
    auto executionCmd = factory.CreateSetMechCameraTrackingLayoutCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发有效响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为成功
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : SetMechCameraTrackingLayoutCmd_GetParams_001
* @tc.number: SetMechCameraTrackingLayoutCmd_GetParams_001
* @tc.desc  : Test SetMechCameraTrackingLayoutCmd GetParams returns correct parameters.
*/
HWTEST_F(MechCommandTest, SetMechCameraTrackingLayoutCmd_GetParams_001, TestSize.Level1)
{
    // Given: 创建命令对象和参数
    CommandFactory factory;
    LayoutParams params;
    params.isDefault = false;
    params.offsetX = 10.0f;
    params.offsetY = 20.0f;
    auto executionCmd = factory.CreateSetMechCameraTrackingLayoutCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取参数
    const LayoutParams& retrievedParams = executionCmd->GetParams();
    
    // Then: 返回正确的参数
    EXPECT_EQ(retrievedParams.isDefault, params.isDefault);
    EXPECT_FLOAT_EQ(retrievedParams.offsetX, params.offsetX);
    EXPECT_FLOAT_EQ(retrievedParams.offsetY, params.offsetY);
}

/**
* @tc.name  : SetMechCameraTrackingLayoutCmd_GetResult_001
* @tc.number: SetMechCameraTrackingLayoutCmd_GetResult_001
* @tc.desc  : Test SetMechCameraTrackingLayoutCmd GetResult returns default result.
*/
HWTEST_F(MechCommandTest, SetMechCameraTrackingLayoutCmd_GetResult_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    LayoutParams params;
    auto executionCmd = factory.CreateSetMechCameraTrackingLayoutCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取结果
    uint8_t result = executionCmd->GetResult();
    
    // Then: 返回默认结果
    EXPECT_EQ(result, 0);
}

/**
* @tc.name  : SetMechConfigCmd_Marshal_001
* @tc.number: SetMechConfigCmd_Marshal_001
* @tc.desc  : Test SetMechConfigCmd Marshal function.
*/
HWTEST_F(MechCommandTest, SetMechConfigCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    uint8_t configVersion = 1;
    auto executionCmd = factory.CreateSetMechConfigCmd(configVersion);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 序列化命令
    auto buffer = executionCmd->Marshal();
    
    // Then: 返回有效的缓冲区
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), SetMechConfigCmd::REQ_SIZE + 1);
}

/**
* @tc.name  : SetMechConfigCmd_TriggerResponse_001
* @tc.number: SetMechConfigCmd_TriggerResponse_001
* @tc.desc  : Test SetMechConfigCmd TriggerResponse with null buffer.
*/
HWTEST_F(MechCommandTest, SetMechConfigCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    uint8_t configVersion = 1;
    auto executionCmd = factory.CreateSetMechConfigCmd(configVersion);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发空缓冲区响应
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    // Then: 结果码为默认值
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : SetMechConfigCmd_TriggerResponse_ValidData_001
* @tc.number: SetMechConfigCmd_TriggerResponse_ValidData_001
* @tc.desc  : Test SetMechConfigCmd TriggerResponse with valid data.
*/
HWTEST_F(MechCommandTest, SetMechConfigCmd_TriggerResponse_ValidData_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    uint8_t configVersion = 1;
    auto executionCmd = factory.CreateSetMechConfigCmd(configVersion);
    ASSERT_NE(executionCmd, nullptr);
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // result code = 0 (success)
    buffer->AppendUint8(0x00);
    buffer->AppendUint8(0x00);
    buffer->AppendUint8(0x00);
    
    // When: 触发有效响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为成功
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : SetMechConfigCmd_GetResult_001
* @tc.number: SetMechConfigCmd_GetResult_001
* @tc.desc  : Test SetMechConfigCmd GetResult returns default result.
*/
HWTEST_F(MechCommandTest, SetMechConfigCmd_GetResult_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    uint8_t configVersion = 1;
    auto executionCmd = factory.CreateSetMechConfigCmd(configVersion);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取结果
    uint8_t result = executionCmd->GetResult();
    
    // Then: 返回默认结果
    EXPECT_EQ(result, 0);
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_TriggerResponse_001
* @tc.number: SetMechRotationBySpeedCmd_TriggerResponse_001
* @tc.desc  : Test SetMechRotationBySpeedCmd TriggerResponse with null buffer.
*/
HWTEST_F(MechCommandTest, SetMechRotationBySpeedCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    RotateBySpeedParam params;
    auto executionCmd = factory.CreateSetMechRotationBySpeedCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发空缓冲区响应
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    // Then: 结果码为默认值
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_TriggerResponse_ValidData_001
* @tc.number: SetMechRotationBySpeedCmd_TriggerResponse_ValidData_001
* @tc.desc  : Test SetMechRotationBySpeedCmd TriggerResponse with valid data.
*/
HWTEST_F(MechCommandTest, SetMechRotationBySpeedCmd_TriggerResponse_ValidData_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    RotateBySpeedParam params;
    params.speed.yawSpeed = 10.0f;
    params.speed.rollSpeed = 5.0f;
    params.speed.pitchSpeed = 3.0f;
    params.duration = 1000;
    auto executionCmd = factory.CreateSetMechRotationBySpeedCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // result code = 0 (success)
    buffer->AppendUint8(0x00);
    buffer->AppendUint8(0x00);
    buffer->AppendUint8(0x00);
    
    // When: 触发有效响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为成功
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_GetParams_001
* @tc.number: SetMechRotationBySpeedCmd_GetParams_001
* @tc.desc  : Test SetMechRotationBySpeedCmd GetParams returns correct parameters.
*/
HWTEST_F(MechCommandTest, SetMechRotationBySpeedCmd_GetParams_001, TestSize.Level1)
{
    // Given: 创建命令对象和参数
    CommandFactory factory;
    RotateBySpeedParam params;
    params.speed.yawSpeed = 10.0f;
    params.speed.rollSpeed = 5.0f;
    params.speed.pitchSpeed = 3.0f;
    params.duration = 1000;
    auto executionCmd = factory.CreateSetMechRotationBySpeedCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取参数
    const RotateBySpeedParam& retrievedParams = executionCmd->GetParams();
    
    // Then: 返回正确的参数
    EXPECT_FLOAT_EQ(retrievedParams.speed.yawSpeed, params.speed.yawSpeed);
    EXPECT_FLOAT_EQ(retrievedParams.speed.rollSpeed, params.speed.rollSpeed);
    EXPECT_FLOAT_EQ(retrievedParams.speed.pitchSpeed, params.speed.pitchSpeed);
    EXPECT_FLOAT_EQ(retrievedParams.duration, params.duration);
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_GetResult_001
* @tc.number: SetMechRotationBySpeedCmd_GetResult_001
* @tc.desc  : Test SetMechRotationBySpeedCmd GetResult returns default result.
*/
HWTEST_F(MechCommandTest, SetMechRotationBySpeedCmd_GetResult_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    RotateBySpeedParam params;
    auto executionCmd = factory.CreateSetMechRotationBySpeedCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取结果
    uint8_t result = executionCmd->GetResult();
    
    // Then: 返回默认结果
    EXPECT_EQ(result, 0);
}

/**
* @tc.name  : SetMechRotationCmd_TriggerResponse_001
* @tc.number: SetMechRotationCmd_TriggerResponse_001
* @tc.desc  : Test SetMechRotationCmd TriggerResponse with null buffer.
*/
HWTEST_F(MechCommandTest, SetMechRotationCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    RotateParam params;
    auto executionCmd = factory.CreateSetMechRotationCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发空缓冲区响应
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    // Then: 结果码为默认值
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : SetMechRotationCmd_TriggerResponse_ValidData_001
* @tc.number: SetMechRotationCmd_TriggerResponse_ValidData_001
* @tc.desc  : Test SetMechRotationCmd TriggerResponse with valid data.
*/
HWTEST_F(MechCommandTest, SetMechRotationCmd_TriggerResponse_ValidData_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    RotateParam params;
    params.degree.pitch = 10.0f;
    params.degree.yaw = 20.0f;
    params.duration = 1000;
    auto executionCmd = factory.CreateSetMechRotationCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // result code = 0 (success)
    buffer->AppendUint8(0x00);
    buffer->AppendUint8(0x00);
    buffer->AppendUint8(0x00);
    
    // When: 触发有效响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为成功
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : SetMechRotationCmd_GetParams_001
* @tc.number: SetMechRotationCmd_GetParams_001
* @tc.desc  : Test SetMechRotationCmd GetParams returns correct parameters.
*/
HWTEST_F(MechCommandTest, SetMechRotationCmd_GetParams_001, TestSize.Level1)
{
    // Given: 创建命令对象和参数
    CommandFactory factory;
    RotateParam params;
    params.degree.pitch = 10.0f;
    params.degree.yaw = 20.0f;
    params.duration = 1000;
    auto executionCmd = factory.CreateSetMechRotationCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取参数
    const RotateParam& retrievedParams = executionCmd->GetParams();
    
    // Then: 返回正确的参数
    EXPECT_FLOAT_EQ(retrievedParams.degree.pitch, params.degree.pitch);
    EXPECT_FLOAT_EQ(retrievedParams.degree.yaw, params.degree.yaw);
    EXPECT_EQ(retrievedParams.duration, params.duration);
}

/**
* @tc.name  : SetMechRotationCmd_GetResult_001
* @tc.number: SetMechRotationCmd_GetResult_001
* @tc.desc  : Test SetMechRotationCmd GetResult returns default result.
*/
HWTEST_F(MechCommandTest, SetMechRotationCmd_GetResult_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    RotateParam params;
    auto executionCmd = factory.CreateSetMechRotationCmd(params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取结果
    uint8_t result = executionCmd->GetResult();
    
    // Then: 返回默认结果
    EXPECT_EQ(result, 0);
}

/**
* @tc.name  : SetMechRotationTraceCmd_TriggerResponse_001
* @tc.number: SetMechRotationTraceCmd_TriggerResponse_001
* @tc.desc  : Test SetMechRotationTraceCmd TriggerResponse with null buffer.
*/
HWTEST_F(MechCommandTest, SetMechRotationTraceCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    RotateParam param;
    std::vector<RotateParam> params;
    params.push_back(param);
    auto executionCmd = factory.CreateSetMechRotationTraceCmd(++g_taskIdOldCommand, params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发空缓冲区响应
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    // Then: 结果码为默认值
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : SetMechRotationTraceCmd_TriggerResponse_ValidData_001
* @tc.number: SetMechRotationTraceCmd_TriggerResponse_ValidData_001
* @tc.desc  : Test SetMechRotationTraceCmd TriggerResponse with valid data.
*/
HWTEST_F(MechCommandTest, SetMechRotationTraceCmd_TriggerResponse_ValidData_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    RotateParam param;
    param.degree.pitch = 10.0f;
    param.degree.yaw = 20.0f;
    param.duration = 1000;
    std::vector<RotateParam> params;
    params.push_back(param);
    auto executionCmd = factory.CreateSetMechRotationTraceCmd(++g_taskIdOldCommand, params);
    ASSERT_NE(executionCmd, nullptr);
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // cmdSet and cmdId (offset 0-1)
    buffer->AppendUint8(0x02);
    buffer->AppendUint8(0x03);
    // result code = 0 (success) at offset 2
    buffer->AppendUint8(0x00);
    
    // When: 触发有效响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为成功
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : SetMechRotationTraceCmd_GetParams_001
* @tc.number: SetMechRotationTraceCmd_GetParams_001
* @tc.desc  : Test SetMechRotationTraceCmd GetParams returns correct parameters.
*/
HWTEST_F(MechCommandTest, SetMechRotationTraceCmd_GetParams_001, TestSize.Level1)
{
    // Given: 创建命令对象和参数
    CommandFactory factory;
    RotateParam param;
    param.degree.pitch = 10.0f;
    param.degree.yaw = 20.0f;
    param.duration = 1000;
    std::vector<RotateParam> params;
    params.push_back(param);
    auto executionCmd = factory.CreateSetMechRotationTraceCmd(++g_taskIdOldCommand, params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取参数
    const std::vector<RotateParam>& retrievedParams = executionCmd->GetParams();
    
    // Then: 返回正确的参数
    EXPECT_EQ(retrievedParams.size(), params.size());
    EXPECT_FLOAT_EQ(retrievedParams[0].degree.pitch, params[0].degree.pitch);
    EXPECT_FLOAT_EQ(retrievedParams[0].degree.yaw, params[0].degree.yaw);
    EXPECT_EQ(retrievedParams[0].duration, params[0].duration);
}

/**
* @tc.name  : SetMechRotationTraceCmd_GetResult_001
* @tc.number: SetMechRotationTraceCmd_GetResult_001
* @tc.desc  : Test SetMechRotationTraceCmd GetResult returns default result.
*/
HWTEST_F(MechCommandTest, SetMechRotationTraceCmd_GetResult_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    RotateParam param;
    std::vector<RotateParam> params;
    params.push_back(param);
    auto executionCmd = factory.CreateSetMechRotationTraceCmd(++g_taskIdOldCommand, params);
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取结果
    uint8_t result = executionCmd->GetResult();
    
    // Then: 返回默认结果
    EXPECT_EQ(result, 0);
}

/**
* @tc.name  : SetMechStopCmd_TriggerResponse_001
* @tc.number: SetMechStopCmd_TriggerResponse_001
* @tc.desc  : Test SetMechStopCmd TriggerResponse with null buffer.
*/
HWTEST_F(MechCommandTest, SetMechStopCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateSetMechStopCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 触发空缓冲区响应
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    // Then: 结果码为默认值
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : SetMechStopCmd_TriggerResponse_ValidData_001
* @tc.number: SetMechStopCmd_TriggerResponse_ValidData_001
* @tc.desc  : Test SetMechStopCmd TriggerResponse with valid data.
*/
HWTEST_F(MechCommandTest, SetMechStopCmd_TriggerResponse_ValidData_001, TestSize.Level1)
{
    // Given: 创建命令对象和有效数据
    CommandFactory factory;
    auto executionCmd = factory.CreateSetMechStopCmd();
    ASSERT_NE(executionCmd, nullptr);
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // result code = 0 (success)
    buffer->AppendUint8(0x00);
    buffer->AppendUint8(0x00);
    buffer->AppendUint8(0x00);
    
    // When: 触发有效响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为成功
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : SetMechStopCmd_GetResult_001
* @tc.number: SetMechStopCmd_GetResult_001
* @tc.desc  : Test SetMechStopCmd GetResult returns default result.
*/
HWTEST_F(MechCommandTest, SetMechStopCmd_GetResult_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    auto executionCmd = factory.CreateSetMechStopCmd();
    ASSERT_NE(executionCmd, nullptr);
    
    // When: 获取结果
    uint8_t result = executionCmd->GetResult();
    
    // Then: 返回默认结果
    EXPECT_EQ(result, 0);
}

/**
* @tc.name  : TriggerTimeout_WithoutCallback_001
* @tc.number: TriggerTimeout_WithoutCallback_001
* @tc.desc  : Test TriggerTimeout function without callback.
*/
HWTEST_F(MechCommandTest, TriggerTimeout_WithoutCallback_001, TestSize.Level1)
{
    // Given: 创建命令对象，不设置超时回调
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    
    // When: 触发超时
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(executionResultCmd->TriggerTimeout());
}

/**
* @tc.name  : TriggerTimeoutReset_WithCallback_001
* @tc.number: TriggerTimeoutReset_WithCallback_001
* @tc.desc  : Test TriggerTimeoutReset function with callback.
*/
HWTEST_F(MechCommandTest, TriggerTimeoutReset_WithCallback_001, TestSize.Level1)
{
    // Given: 创建命令对象并设置超时重置回调
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    bool callbackTriggered = false;
    executionResultCmd->SetTimeoutResetCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });
    
    // When: 触发超时重置
    executionResultCmd->TriggerTimeoutReset();
    
    // Then: 回调被触发
    EXPECT_TRUE(callbackTriggered);
}

/**
* @tc.name  : TriggerTimeoutReset_WithoutCallback_001
* @tc.number: TriggerTimeoutReset_WithoutCallback_001
* @tc.desc  : Test TriggerTimeoutReset function without callback.
*/
HWTEST_F(MechCommandTest, TriggerTimeoutReset_WithoutCallback_001, TestSize.Level1)
{
    // Given: 创建命令对象，不设置超时重置回调
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    
    // When: 触发超时重置
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(executionResultCmd->TriggerTimeoutReset());
}

/**
* @tc.name  : GetRetryTimes_001
* @tc.number: GetRetryTimes_001
* @tc.desc  : Test GetRetryTimes function returns default value.
*/
HWTEST_F(MechCommandTest, GetRetryTimes_001, TestSize.Level1)
{
    // Given: 创建命令对象
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    
    // When: 获取重试次数
    int32_t retryTimes = executionResultCmd->GetRetryTimes();
    
    // Then: 返回默认重试次数
    EXPECT_EQ(retryTimes, 3);
}

/**
* @tc.name  : CreateFromData_SizeLessThanMin_001
* @tc.number: CreateFromData_SizeLessThanMin_001
* @tc.desc  : Test CreateFromData with data size less than minimum.
*/
HWTEST_F(MechCommandTest, CreateFromData_SizeLessThanMin_001, TestSize.Level1)
{
    // Given: 数据大小小于最小值2
    CommandFactory factory;
    auto buffer = std::make_shared<MechDataBuffer>(1);
    buffer->AppendUint8(0x01);
    
    // When: 调用CreateFromData
    auto executionCmd = factory.CreateFromData(buffer);
    
    // Then: 返回nullptr
    EXPECT_EQ(executionCmd, nullptr);
}

/**
* @tc.name  : CreateGetMechLimitInfoCmd_ProtocolVer_001
* @tc.number: CreateGetMechLimitInfoCmd_ProtocolVer_001
* @tc.desc  : Test CreateGetMechLimitInfoCmd with different protocol versions.
*/
HWTEST_F(MechCommandTest, CreateGetMechLimitInfoCmd_ProtocolVer_001, TestSize.Level1)
{
    // Given: 命令工厂
    CommandFactory factory;
    
    // When: 设置协议版本为0
    factory.SetFactoryProtocolVer(0);
    auto cmd0 = factory.CreateGetMechLimitInfoCmd();
    ASSERT_NE(cmd0, nullptr);
    EXPECT_EQ(cmd0->GetCmdSet(), 0x02);

    // When: 设置协议版本为0x01
    factory.SetFactoryProtocolVer(0x01);
    auto cmd1 = factory.CreateGetMechLimitInfoCmd();
    ASSERT_NE(cmd1, nullptr);
    EXPECT_EQ(cmd1->GetCmdSet(), 0x02);

    // When: 设置协议版本为0x02
    factory.SetFactoryProtocolVer(0x02);
    auto cmd2 = factory.CreateGetMechLimitInfoCmd();
    ASSERT_NE(cmd2, nullptr);
    EXPECT_EQ(cmd2->GetCmdSet(), 0x02);
}

/**
* @tc.name  : CreateSetMechCameraInfoCmd_ProtocolVer_001
* @tc.number: CreateSetMechCameraInfoCmd_ProtocolVer_001
* @tc.desc  : Test CreateSetMechCameraInfoCmd with different protocol versions.
*/
HWTEST_F(MechCommandTest, CreateSetMechCameraInfoCmd_ProtocolVer_001, TestSize.Level1)
{
    // Given: 命令工厂和参数
    CommandFactory factory;
    CameraInfoParams params;
    params.fovH = 90;
    params.fovV = 60;
    params.cameraType = CameraType::FRONT;

    // When: 设置协议版本为0
    factory.SetFactoryProtocolVer(0);
    auto cmd0 = factory.CreateSetMechCameraInfoCmd(params);
    ASSERT_NE(cmd0, nullptr);
    EXPECT_EQ(cmd0->GetParams().fovH, params.fovH);

    // When: 设置协议版本为0x01
    factory.SetFactoryProtocolVer(0x01);
    auto cmd1 = factory.CreateSetMechCameraInfoCmd(params);
    ASSERT_NE(cmd1, nullptr);
    EXPECT_EQ(cmd1->GetParams().fovH, params.fovH);

    // When: 设置协议版本为0x02
    factory.SetFactoryProtocolVer(0x02);
    auto cmd2 = factory.CreateSetMechCameraInfoCmd(params);
    ASSERT_NE(cmd2, nullptr);
    EXPECT_EQ(cmd2->GetParams().fovH, params.fovH);
}

/**
* @tc.name  : CreateSetMechCameraTrackingEnableCmd_ProtocolVer_001
* @tc.number: CreateSetMechCameraTrackingEnableCmd_ProtocolVer_001
* @tc.desc  : Test CreateSetMechCameraTrackingEnableCmd with different protocol versions.
*/
HWTEST_F(MechCommandTest, CreateSetMechCameraTrackingEnableCmd_ProtocolVer_001, TestSize.Level1)
{
    // Given: 命令工厂
    CommandFactory factory;
    MechTrackingStatus status = MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET;

    // When: 设置协议版本为0
    factory.SetFactoryProtocolVer(0);
    auto cmd0 = factory.CreateSetMechCameraTrackingEnableCmd(status);
    ASSERT_NE(cmd0, nullptr);

    // When: 设置协议版本为0x01
    factory.SetFactoryProtocolVer(0x01);
    auto cmd1 = factory.CreateSetMechCameraTrackingEnableCmd(status);
    ASSERT_NE(cmd1, nullptr);

    // When: 设置协议版本为0x02
    factory.SetFactoryProtocolVer(0x02);
    auto cmd2 = factory.CreateSetMechCameraTrackingEnableCmd(status);
    ASSERT_NE(cmd2, nullptr);
}

/**
* @tc.name  : CreateSetMechCameraTrackingFrameCmd_ProtocolVer_001
* @tc.number: CreateSetMechCameraTrackingFrameCmd_ProtocolVer_001
* @tc.desc  : Test CreateSetMechCameraTrackingFrameCmd with different protocol versions.
*/
HWTEST_F(MechCommandTest, CreateSetMechCameraTrackingFrameCmd_ProtocolVer_001, TestSize.Level1)
{
    // Given: 命令工厂
    CommandFactory factory;
    TrackingFrameParams params;

    // When: 设置协议版本为0
    factory.SetFactoryProtocolVer(0);
    auto cmd0 = factory.CreateSetMechCameraTrackingFrameCmd(params);
    ASSERT_NE(cmd0, nullptr);

    // When: 设置协议版本为0x01
    factory.SetFactoryProtocolVer(0x01);
    auto cmd1 = factory.CreateSetMechCameraTrackingFrameCmd(params);
    ASSERT_NE(cmd1, nullptr);

    // When: 设置协议版本为0x02
    factory.SetFactoryProtocolVer(0x02);
    auto cmd2 = factory.CreateSetMechCameraTrackingFrameCmd(params);
    ASSERT_NE(cmd2, nullptr);
}

/**
* @tc.name  : CreateSetMechRotationBySpeedCmd_ProtocolVer_001
* @tc.number: CreateSetMechRotationBySpeedCmd_ProtocolVer_001
* @tc.desc  : Test CreateSetMechRotationBySpeedCmd with different protocol versions.
*/
HWTEST_F(MechCommandTest, CreateSetMechRotationBySpeedCmd_ProtocolVer_001, TestSize.Level1)
{
    // Given: 命令工厂
    CommandFactory factory;
    RotateBySpeedParam params;

    // When: 设置协议版本为0
    factory.SetFactoryProtocolVer(0);
    auto cmd0 = factory.CreateSetMechRotationBySpeedCmd(params);
    ASSERT_NE(cmd0, nullptr);

    // When: 设置协议版本为0x01
    factory.SetFactoryProtocolVer(0x01);
    auto cmd1 = factory.CreateSetMechRotationBySpeedCmd(params);
    ASSERT_NE(cmd1, nullptr);

    // When: 设置协议版本为0x02
    factory.SetFactoryProtocolVer(0x02);
    auto cmd2 = factory.CreateSetMechRotationBySpeedCmd(params);
    ASSERT_NE(cmd2, nullptr);
}

/**
* @tc.name  : CreateSetMechRotationTraceCmd_ProtocolVer_001
* @tc.number: CreateSetMechRotationTraceCmd_ProtocolVer_001
* @tc.desc  : Test CreateSetMechRotationTraceCmd with different protocol versions.
*/
HWTEST_F(MechCommandTest, CreateSetMechRotationTraceCmd_ProtocolVer_001, TestSize.Level1)
{
    // Given: 命令工厂
    CommandFactory factory;
    std::vector<RotateParam> params;

    // When: 设置协议版本为0
    factory.SetFactoryProtocolVer(0);
    auto cmd0 = factory.CreateSetMechRotationTraceCmd(1, params);
    ASSERT_NE(cmd0, nullptr);

    // When: 设置协议版本为0x01
    factory.SetFactoryProtocolVer(0x01);
    auto cmd1 = factory.CreateSetMechRotationTraceCmd(1, params);
    ASSERT_NE(cmd1, nullptr);

    // When: 设置协议版本为0x02
    factory.SetFactoryProtocolVer(0x02);
    auto cmd2 = factory.CreateSetMechRotationTraceCmd(1, params);
    ASSERT_NE(cmd2, nullptr);
}

/**
* @tc.name  : CreateRegisterMechCameraKeyEventCmd_ProtocolVer_001
* @tc.number: CreateRegisterMechCameraKeyEventCmd_ProtocolVer_001
* @tc.desc  : Test CreateRegisterMechCameraKeyEventCmd with different protocol versions.
*/
HWTEST_F(MechCommandTest, CreateRegisterMechCameraKeyEventCmd_ProtocolVer_001, TestSize.Level1)
{
    // Given: 命令工厂
    CommandFactory factory;

    // When: 设置协议版本为0
    factory.SetFactoryProtocolVer(0);
    auto cmd0 = factory.CreateRegisterMechCameraKeyEventCmd();
    ASSERT_NE(cmd0, nullptr);

    // When: 设置协议版本为0x01
    factory.SetFactoryProtocolVer(0x01);
    auto cmd1 = factory.CreateRegisterMechCameraKeyEventCmd();
    ASSERT_NE(cmd1, nullptr);

    // When: 设置协议版本为0x02
    factory.SetFactoryProtocolVer(0x02);
    auto cmd2 = factory.CreateRegisterMechCameraKeyEventCmd();
    ASSERT_NE(cmd2, nullptr);
}

/**
* @tc.name  : CreateRegisterMechPositionInfoCmd_ProtocolVer_001
* @tc.number: CreateRegisterMechPositionInfoCmd_ProtocolVer_001
* @tc.desc  : Test CreateRegisterMechPositionInfoCmd with different protocol versions.
*/
HWTEST_F(MechCommandTest, CreateRegisterMechPositionInfoCmd_ProtocolVer_001, TestSize.Level1)
{
    // Given: 命令工厂
    CommandFactory factory;

    // When: 设置协议版本为0
    factory.SetFactoryProtocolVer(0);
    auto cmd0 = factory.CreateRegisterMechPositionInfoCmd();
    ASSERT_NE(cmd0, nullptr);

    // When: 设置协议版本为0x01
    factory.SetFactoryProtocolVer(0x01);
    auto cmd1 = factory.CreateRegisterMechPositionInfoCmd();
    ASSERT_NE(cmd1, nullptr);

    // When: 设置协议版本为0x02
    factory.SetFactoryProtocolVer(0x02);
    auto cmd2 = factory.CreateRegisterMechPositionInfoCmd();
    ASSERT_NE(cmd2, nullptr);
}

/**
* @tc.name  : CreateRegisterMechStateInfoCmd_ProtocolVer_001
* @tc.number: CreateRegisterMechStateInfoCmd_ProtocolVer_001
* @tc.desc  : Test CreateRegisterMechStateInfoCmd with different protocol versions.
*/
HWTEST_F(MechCommandTest, CreateRegisterMechStateInfoCmd_ProtocolVer_001, TestSize.Level1)
{
    // Given: 命令工厂
    CommandFactory factory;

    // When: 设置协议版本为0
    factory.SetFactoryProtocolVer(0);
    auto cmd0 = factory.CreateRegisterMechStateInfoCmd();
    ASSERT_NE(cmd0, nullptr);

    // When: 设置协议版本为0x01
    factory.SetFactoryProtocolVer(0x01);
    auto cmd1 = factory.CreateRegisterMechStateInfoCmd();
    ASSERT_NE(cmd1, nullptr);

    // When: 设置协议版本为0x02
    factory.SetFactoryProtocolVer(0x02);
    auto cmd2 = factory.CreateRegisterMechStateInfoCmd();
    ASSERT_NE(cmd2, nullptr);
}

/**
* @tc.name  : GetMechCameraTrackingLayoutCmd_TriggerResponse_SizeTooSmall_001
* @tc.number: GetMechCameraTrackingLayoutCmd_TriggerResponse_SizeTooSmall_001
* @tc.desc  : Test TriggerResponse with size too small.
*/
HWTEST_F(MechCommandTest, GetMechCameraTrackingLayoutCmd_TriggerResponse_SizeTooSmall_001, TestSize.Level1)
{
    // Given: 创建命令对象和小数据
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    ASSERT_NE(executionCmd, nullptr);

    size_t capacity = 10;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 3; i++) {
        buffer->AppendUint8(value);
    }

    // When: 触发响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为默认值
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : GetMechCameraTrackingLayoutCmd_TriggerResponse_InvalidResultType_001
* @tc.number: GetMechCameraTrackingLayoutCmd_TriggerResponse_InvalidResultType_001
* @tc.desc  : Test TriggerResponse with invalid resultType.
*/
HWTEST_F(MechCommandTest, GetMechCameraTrackingLayoutCmd_TriggerResponse_InvalidResultType_001, TestSize.Level1)
{
    // Given: 创建命令对象和无效resultType数据
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    ASSERT_NE(executionCmd, nullptr);

    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // cmdSet and cmdId (offset 0-1)
    buffer->AppendUint8(0x02);
    buffer->AppendUint8(0x02);
    // result code = 0 (success) at offset 2
    buffer->AppendUint8(0x00);
    // resultType = 0x09 (invalid, should be 0x02) at offset 3
    buffer->AppendUint8(0x09);
    // resultLength = 9 at offset 4
    buffer->AppendUint8(0x09);
    uint8_t value = 0x02;
    for (int i = 0; i < 20; i++) {
        buffer->AppendUint8(value);
    }

    // When: 触发响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为默认值
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : GetMechCameraTrackingLayoutCmd_TriggerResponse_InvalidResultLength_001
* @tc.number: GetMechCameraTrackingLayoutCmd_TriggerResponse_InvalidResultLength_001
* @tc.desc  : Test TriggerResponse with invalid resultLength.
*/
HWTEST_F(MechCommandTest, GetMechCameraTrackingLayoutCmd_TriggerResponse_InvalidResultLength_001, TestSize.Level1)
{
    // Given: 创建命令对象和无效resultLength数据
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    ASSERT_NE(executionCmd, nullptr);

    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // cmdSet and cmdId (offset 0-1)
    buffer->AppendUint8(0x02);
    buffer->AppendUint8(0x02);
    // result code = 0 (success) at offset 2
    buffer->AppendUint8(0x00);
    // resultType = 0x02 (correct) at offset 3
    buffer->AppendUint8(0x02);
    // resultLength = 0x01 (invalid, should be 9) at offset 4
    buffer->AppendUint8(0x01);
    uint8_t value = 0x02;
    for (int i = 0; i < 20; i++) {
        buffer->AppendUint8(value);
    }

    // When: 触发响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为默认值(0)
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : GetMechCameraTrackingLayoutCmd_TriggerResponse_WithCallback_001
* @tc.number: GetMechCameraTrackingLayoutCmd_TriggerResponse_WithCallback_001
* @tc.desc  : Test TriggerResponse with callback.
*/
HWTEST_F(MechCommandTest, GetMechCameraTrackingLayoutCmd_TriggerResponse_WithCallback_001, TestSize.Level1)
{
    // Given: 创建命令对象并设置响应回调
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    ASSERT_NE(executionCmd, nullptr);

    bool callbackTriggered = false;
    executionCmd->SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // cmdSet and cmdId (offset 0-1)
    buffer->AppendUint8(0x02);
    buffer->AppendUint8(0x02);
    // result code = 0 (success) at offset 2
    buffer->AppendUint8(0x00);
    // resultType = 0x02 at offset 3
    buffer->AppendUint8(0x02);
    // resultLength = 9 at offset 4
    buffer->AppendUint8(0x09);
    // controlBit = 0x01 (isDefault = true) at offset 5
    buffer->AppendUint8(0x01);
    // offsetX = 1.0f at offset 6
    float offsetX = 1.0f;
    buffer->AppendFloat(offsetX);
    // offsetY = 2.0f at offset 10
    float offsetY = 2.0f;
    buffer->AppendFloat(offsetY);

    // When: 触发响应
    executionCmd->TriggerResponse(buffer);

    // Then: 回调被触发
    EXPECT_TRUE(callbackTriggered);
}

/**
* @tc.name  : GetMechLimitInfoCmd_TriggerResponse_SizeTooSmall_001
* @tc.number: GetMechLimitInfoCmd_TriggerResponse_SizeTooSmall_001
* @tc.desc  : Test TriggerResponse with data size too small.
*/
HWTEST_F(MechCommandTest, GetMechLimitInfoCmd_TriggerResponse_SizeTooSmall_001, TestSize.Level1)
{
    // Given: 创建命令对象和小数据
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechLimitInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    size_t capacity = 10;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x03;
    for (int i = 0; i < 3; i++) {
        buffer->AppendUint8(value);
    }

    // When: 触发响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为默认值(0)
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : GetMechLimitInfoCmd_TriggerResponse_InvalidResultType_001
* @tc.number: GetMechLimitInfoCmd_TriggerResponse_InvalidResultType_001
* @tc.desc  : Test TriggerResponse with invalid resultType.
*/
HWTEST_F(MechCommandTest, GetMechLimitInfoCmd_TriggerResponse_InvalidResultType_001, TestSize.Level1)
{
    // Given: 创建命令对象和无效resultType数据
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechLimitInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // cmdSet and cmdId (offset 0-1)
    buffer->AppendUint8(0x02);
    buffer->AppendUint8(0x01);
    // result code = 0 (success) at offset 2
    buffer->AppendUint8(0x00);
    // resultType = 0x09 (invalid, should be 0x03) at offset 3
    buffer->AppendUint8(0x09);
    // resultLength = 24 at offset 4
    buffer->AppendUint8(24);
    // 6 floats (24 bytes) for posMax.roll, negMax.roll, posMax.pitch, negMax.pitch, posMax.yaw, negMax.yaw
    for (int i = 0; i < 6; i++) {
        buffer->AppendFloat(0.0f);
    }

    // When: 触发响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为默认值(0)
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : GetMechLimitInfoCmd_TriggerResponse_InvalidResultLength_001
* @tc.number: GetMechLimitInfoCmd_TriggerResponse_InvalidResultLength_001
* @tc.desc  : Test TriggerResponse with invalid resultLength.
*/
HWTEST_F(MechCommandTest, GetMechLimitInfoCmd_TriggerResponse_InvalidResultLength_001, TestSize.Level1)
{
    // Given: 创建命令对象和无效resultLength数据
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechLimitInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // cmdSet and cmdId (offset 0-1)
    buffer->AppendUint8(0x02);
    buffer->AppendUint8(0x01);
    // result code = 0 (success) at offset 2
    buffer->AppendUint8(0x00);
    // resultType = 0x03 (correct) at offset 3
    buffer->AppendUint8(0x03);
    // resultLength = 0x01 (invalid, should be 24) at offset 4
    buffer->AppendUint8(0x01);
    uint8_t value = 0x02;
    for (int i = 0; i < 30; i++) {
        buffer->AppendUint8(value);
    }

    // When: 触发响应
    executionCmd->TriggerResponse(buffer);
    
    // Then: 结果码为默认值(0)
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

/**
* @tc.name  : GetMechLimitInfoCmd_TriggerResponse_WithCallback_001
* @tc.number: GetMechLimitInfoCmd_TriggerResponse_WithCallback_001
* @tc.desc  : Test TriggerResponse with callback.
*/
HWTEST_F(MechCommandTest, GetMechLimitInfoCmd_TriggerResponse_WithCallback_001, TestSize.Level1)
{
    // Given: 创建命令对象并设置响应回调
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechLimitInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    bool callbackTriggered = false;
    executionCmd->SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    // cmdSet and cmdId (offset 0-1)
    buffer->AppendUint8(0x02);
    buffer->AppendUint8(0x01);
    // result code = 0 (success) at offset 2
    buffer->AppendUint8(0x00);
    // resultType = 0x03 at offset 3
    buffer->AppendUint8(0x03);
    // resultLength = 24 at offset 4
    buffer->AppendUint8(24);
    // 6 floats (24 bytes) for posMax.roll, negMax.roll, posMax.pitch, negMax.pitch, posMax.yaw, negMax.yaw
    for (int i = 0; i < 6; i++) {
        buffer->AppendFloat(0.0f);
    }

    // When: 触发响应
    executionCmd->TriggerResponse(buffer);

    // Then: 回调被触发
    EXPECT_TRUE(callbackTriggered);
}

/**
* @tc.name  : SetMechRotationTraceCmd_Constructor_ExceedMaxSize_001
* @tc.number: SetMechRotationTraceCmd_Constructor_ExceedMaxSize_001
* @tc.desc  : Test Constructor when params size exceeds max size.
*/
HWTEST_F(MechCommandTest, SetMechRotationTraceCmd_Constructor_ExceedMaxSize_001, TestSize.Level1)
{
    // Given: 超过最大大小的参数
    std::vector<RotateParam> params(11);
    for (auto& param : params) {
        param.degree.pitch = 10.0f;
        param.degree.yaw = 20.0f;
        param.duration = 1000;
    }
    
    // When: 创建命令
    SetMechRotationTraceCmd cmd(params);
    
    // Then: 程序正常执行，参数未被截断
    const std::vector<RotateParam>& retrievedParams = cmd.GetParams();
    EXPECT_EQ(retrievedParams.size(), 11); // 超过最大限制但未被截断
}

/**
* @tc.name  : SetMechRotationTraceCmd_Marshal_001
* @tc.number: SetMechRotationTraceCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(MechCommandTest, SetMechRotationTraceCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建命令对象和参数
    std::vector<RotateParam> params(3);
    for (size_t i = 0; i < params.size(); i++) {
        params[i].degree.pitch = 10.0f + i;
        params[i].degree.yaw = 20.0f + i;
        params[i].duration = 1000 + i * 100;
    }
    SetMechRotationTraceCmd cmd(params);
    
    // When: 序列化命令
    auto buffer = cmd.Marshal();
    
    // Then: 返回有效的缓冲区
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

/**
* @tc.name  : SetMechRotationTraceCmd_TriggerResponse_NullOrSmallData_001
* @tc.number: SetMechRotationTraceCmd_TriggerResponse_NullOrSmallData_001
* @tc.desc  : Test TriggerResponse with null or small data.
*/
HWTEST_F(MechCommandTest, SetMechRotationTraceCmd_TriggerResponse_NullOrSmallData_001, TestSize.Level1)
{
    // Given: 创建命令对象
    std::vector<RotateParam> params(1);
    SetMechRotationTraceCmd cmd(params);
    
    // When: 触发空响应
    cmd.TriggerResponse(nullptr);
    
    // Then: 结果码为默认值(0)
    EXPECT_EQ(cmd.GetResult(), 0);
    
    // When: 触发小数据响应
    auto buffer = std::make_shared<MechDataBuffer>(1);
    cmd.TriggerResponse(buffer);
    
    // Then: 结果码为默认值(0)
    EXPECT_EQ(cmd.GetResult(), 0);
}

/**
* @tc.name  : SetMechRotationTraceCmd_TriggerResponse_ReadFailed_001
* @tc.number: SetMechRotationTraceCmd_TriggerResponse_ReadFailed_001
* @tc.desc  : Test TriggerResponse when ReadUint8 fails.
*/
HWTEST_F(MechCommandTest, SetMechRotationTraceCmd_TriggerResponse_ReadFailed_001, TestSize.Level1)
{
    // Given: 创建命令对象和不足数据
    std::vector<RotateParam> params(1);
    SetMechRotationTraceCmd cmd(params);
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01);
    buffer->AppendUint8(0x01);
    
    // When: 触发响应
    cmd.TriggerResponse(buffer);
    
    // Then: 结果码为默认值(0)
    EXPECT_EQ(cmd.GetResult(), 0);
}

/**
* @tc.name  : SetMechRotationTraceCmd_TriggerResponse_WithCallback_001
* @tc.number: SetMechRotationTraceCmd_TriggerResponse_WithCallback_001
* @tc.desc  : Test TriggerResponse with callback.
*/
HWTEST_F(MechCommandTest, SetMechRotationTraceCmd_TriggerResponse_WithCallback_001, TestSize.Level1)
{
    // Given: 创建命令对象并设置回调
    std::vector<RotateParam> params(1);
    SetMechRotationTraceCmd cmd(params);
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01);
    buffer->AppendUint8(0x01);
    buffer->AppendUint8(0x01);
    
    // When: 触发响应
    cmd.TriggerResponse(buffer);
    
    // Then: 回调未被触发（因为数据不足）
    EXPECT_FALSE(callbackTriggered);
}

/**
* @tc.name  : SetMechRotationTraceCmd_TriggerResponse_NoCallback_001
* @tc.number: SetMechRotationTraceCmd_TriggerResponse_NoCallback_001
* @tc.desc  : Test TriggerResponse without callback.
*/
HWTEST_F(MechCommandTest, SetMechRotationTraceCmd_TriggerResponse_NoCallback_001, TestSize.Level1)
{
    // Given: 创建命令对象
    std::vector<RotateParam> params(1);
    SetMechRotationTraceCmd cmd(params);
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01);
    buffer->AppendUint8(0x01);
    buffer->AppendUint8(0x01);
    
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

} // namespace MechBodyController
} // namespace OHOS