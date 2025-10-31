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

/**
* @tc.name  : TriggerTimeout_001
* @tc.number: TriggerTimeout_001
* @tc.desc  : Test TriggerTimeout function.
*/
HWTEST_F(MechCommandTest, TriggerTimeout_001, TestSize.Level1)
{
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    executionResultCmd->SetTimeoutCallback(SetTimeout);
    EXPECT_NO_FATAL_FAILURE(executionResultCmd->TriggerTimeout());
}

/**
* @tc.name  : GetCmdSet_001
* @tc.number: GetCmdSet_001
* @tc.desc  : Test GetCmdSet function.
*/
HWTEST_F(MechCommandTest, GetCmdSet_001, TestSize.Level1)
{
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    executionResultCmd->SetTimeoutCallback(SetTimeout);
    EXPECT_EQ(executionResultCmd->GetCmdSet(), executionResultCmd->cmdSet_);
}

/**
* @tc.name  : GetCmdId_001
* @tc.number: GetCmdId_001
* @tc.desc  : Test GetCmdId function.
*/
HWTEST_F(MechCommandTest, GetCmdId_001, TestSize.Level1)
{
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    executionResultCmd->SetTimeoutCallback(SetTimeout);
    EXPECT_EQ(executionResultCmd->GetCmdId(), executionResultCmd->cmdId_);
}

/**
* @tc.name  : GetReqSize_001
* @tc.number: GetReqSize_001
* @tc.desc  : Test GetReqSize function.
*/
HWTEST_F(MechCommandTest, GetReqSize_001, TestSize.Level1)
{
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    executionResultCmd->SetTimeoutCallback(SetTimeout);
    EXPECT_EQ(executionResultCmd->GetReqSize(), executionResultCmd->reqSize_);
}

/**
* @tc.name  : GetRspSize_001
* @tc.number: GetRspSize_001
* @tc.desc  : Test GetRspSize function.
*/
HWTEST_F(MechCommandTest, GetRspSize_001, TestSize.Level1)
{
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    executionResultCmd->SetTimeoutCallback(SetTimeout);
    EXPECT_EQ(executionResultCmd->GetRspSize(), executionResultCmd->rspSize_);
}

/**
* @tc.name  : GetTimeoutMs_001
* @tc.number: GetTimeoutMs_001
* @tc.desc  : Test GetTimeoutMs function.
*/
HWTEST_F(MechCommandTest, GetTimeoutMs_001, TestSize.Level1)
{
    CommandFactory factory;
    std::shared_ptr<CommandBase> executionResultCmd = factory.CreateRegisterMechControlResultCmd();
    ASSERT_NE(executionResultCmd, nullptr);
    executionResultCmd->SetTimeoutCallback(SetTimeout);
    EXPECT_EQ(executionResultCmd->GetTimeoutMs(), executionResultCmd->timeoutMs_);
}

/**
* @tc.name  : CreateGetMechCameraTrackingLayoutCmd_001
* @tc.number: CreateGetMechCameraTrackingLayoutCmd_001
* @tc.desc  : Test CreateGetMechCameraTrackingLayoutCmd function.
*/
HWTEST_F(MechCommandTest, CreateGetMechCameraTrackingLayoutCmd_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechCameraTrackingLayoutCmd();

    EXPECT_NE(executionCmd, nullptr);
}

/**
* @tc.name  : CreateSetMechCameraInfoCmd_001
* @tc.number: CreateSetMechCameraInfoCmd_001
* @tc.desc  : Test CreateSetMechCameraInfoCmd function.
*/
HWTEST_F(MechCommandTest, CreateSetMechCameraInfoCmd_001, TestSize.Level1)
{
    CommandFactory factory;
    CameraInfoParams params;
    params.fovH = 1;
    params.fovV = 1;
    params.cameraType = CameraType::FRONT;
    auto executionCmd = factory.CreateSetMechCameraInfoCmd(params);

    EXPECT_NE(executionCmd, nullptr);
}

/**
* @tc.name  : CreateSetMechConfigCmd_001
* @tc.number: CreateSetMechConfigCmd_001
* @tc.desc  : Test CreateSetMechConfigCmd function.
*/
HWTEST_F(MechCommandTest, CreateSetMechConfigCmd_001, TestSize.Level1)
{
    CommandFactory factory;
    uint8_t configVersion = 1;
    auto executionCmd = factory.CreateSetMechConfigCmd(configVersion);

    EXPECT_NE(executionCmd, nullptr);
}

/**
* @tc.name  : CreateSetMechRotationTraceCmd_001
* @tc.number: CreateSetMechRotationTraceCmd_001
* @tc.desc  : Test CreateSetMechRotationTraceCmd function.
*/
HWTEST_F(MechCommandTest, CreateSetMechRotationTraceCmd_001, TestSize.Level1)
{
    CommandFactory factory;
    std::vector<RotateParam> params;
    auto executionCmd = factory.CreateSetMechRotationTraceCmd(params);

    EXPECT_NE(executionCmd, nullptr);
}

/**
* @tc.name  : CreateFromData_001
* @tc.number: CreateFromData_001
* @tc.desc  : Test CreateFromData function.
*/
HWTEST_F(MechCommandTest, CreateFromData_001, TestSize.Level1)
{
    CommandFactory factory;
    size_t capacity = 100;
    auto bufferFirst = std::make_shared<MechDataBuffer>(capacity);
    uint8_t cmdTypeFirst = 0x02;
    uint8_t cmdIdFirst = 0x40;
    bufferFirst->AppendUint8(cmdTypeFirst);
    bufferFirst->AppendUint8(cmdIdFirst);
    auto executionCmdFirst = factory.CreateFromData(bufferFirst);
    EXPECT_EQ(executionCmdFirst, nullptr);

    auto bufferSecond = std::make_shared<MechDataBuffer>(capacity);
    uint8_t cmdTypeSecond = 0x02;
    uint8_t cmdIdSecond = 0x41;
    bufferSecond->AppendUint8(cmdTypeSecond);
    bufferSecond->AppendUint8(cmdIdSecond);
    auto executionCmdSecond = factory.CreateFromData(bufferSecond);
    EXPECT_EQ(executionCmdSecond, nullptr);

    auto bufferThird = std::make_shared<MechDataBuffer>(capacity);
    uint8_t cmdTypeThird = 0x02;
    uint8_t cmdIdThird = 0x42;
    bufferThird->AppendUint8(cmdTypeThird);
    bufferThird->AppendUint8(cmdIdThird);
    auto executionCmdThird = factory.CreateFromData(bufferThird);
    EXPECT_EQ(executionCmdThird, nullptr);

    auto bufferFourth = std::make_shared<MechDataBuffer>(capacity);
    uint8_t cmdTypeFourth = 0x02;
    uint8_t cmdIdFourth = 0x43;
    bufferFourth->AppendUint8(cmdTypeFourth);
    bufferFourth->AppendUint8(cmdIdFourth);
    auto executionCmdFourth = factory.CreateFromData(bufferFourth);
    EXPECT_EQ(executionCmdFourth, nullptr);

    auto bufferFifth = std::make_shared<MechDataBuffer>(capacity);
    uint8_t cmdTypeFifth = 0x02;
    uint8_t cmdIdFifth = 0x44;
    bufferFifth->AppendUint8(cmdTypeFifth);
    bufferFifth->AppendUint8(cmdIdFifth);
    auto executionCmdFifth = factory.CreateFromData(bufferFifth);
    EXPECT_EQ(executionCmdFifth, nullptr);

    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t cmdTypeNull = 0x02;
    uint8_t cmdIdNull = 0x45;
    bufferNull->AppendUint8(cmdTypeNull);
    bufferNull->AppendUint8(cmdIdNull);
    auto executionCmdNull = factory.CreateFromData(bufferNull);
    EXPECT_EQ(executionCmdNull, nullptr);
}

/**
* @tc.name  : GetMechCameraTrackingLayoutCmd_Marshal_001
* @tc.number: GetMechCameraTrackingLayoutCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(MechCommandTest, GetMechCameraTrackingLayoutCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

/**
* @tc.name  : GetMechCameraTrackingLayoutCmd_TriggerResponse_001
* @tc.number: GetMechCameraTrackingLayoutCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function.
*/
HWTEST_F(MechCommandTest, GetMechCameraTrackingLayoutCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t valueFirst = 0x02;
    uint8_t valueSecond = 9;
    buffer->AppendUint8(valueFirst);
    buffer->AppendUint8(valueFirst);
    buffer->AppendUint8(valueFirst);
    buffer->AppendUint8(valueFirst);
    buffer->AppendUint8(valueSecond);
    uint8_t value = 0x02;
    for (int i = 0; i < 20; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferNull));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
}

/**
* @tc.name  : GetMechCameraTrackingLayoutCmd_GetParams_001
* @tc.number: GetMechCameraTrackingLayoutCmd_GetParams_001
* @tc.desc  : Test GetParams function.
*/
HWTEST_F(MechCommandTest, GetMechCameraTrackingLayoutCmd_GetParams_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetParams());
}

/**
* @tc.name  : GetMechCameraTrackingLayoutCmd_GetResult_001
* @tc.number: GetMechCameraTrackingLayoutCmd_GetResult_001
* @tc.desc  : Test GetResult function.
*/
HWTEST_F(MechCommandTest, GetMechCameraTrackingLayoutCmd_GetResult_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechCameraTrackingLayoutCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetResult());
}

/**
* @tc.name  : GetMechCapabilityInfoCmd_TriggerResponse_001
* @tc.number: GetMechCapabilityInfoCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function.
*/
HWTEST_F(MechCommandTest, GetMechCapabilityInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t valueFirst = 0x03;
    uint8_t valueSecond = 24;
    buffer->AppendUint8(valueFirst);
    buffer->AppendUint8(valueFirst);
    buffer->AppendUint8(valueFirst);
    buffer->AppendUint8(valueFirst);
    buffer->AppendUint8(valueSecond);
    uint8_t value = 0x02;
    for (int i = 0; i < 30; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateGetMechCapabilityInfoCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferNull));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
}

/**
* @tc.name  : GetMechCapabilityInfoCmd_GetParams_001
* @tc.number: GetMechCapabilityInfoCmd_GetParams_001
* @tc.desc  : Test GetParams function.
*/
HWTEST_F(MechCommandTest, GetMechCapabilityInfoCmd_GetParams_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechCapabilityInfoCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetParams());
}

/**
* @tc.name  : GetMechCapabilityInfoCmd_GetResult_001
* @tc.number: GetMechCapabilityInfoCmd_GetResult_001
* @tc.desc  : Test GetResult function.
*/
HWTEST_F(MechCommandTest, GetMechCapabilityInfoCmd_GetResult_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateGetMechCapabilityInfoCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetResult());
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_Marshal_001
* @tc.number: RegisterMechCameraKeyEventCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechCameraKeyEventCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechCameraKeyEventCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_Unmarshal_001
* @tc.number: RegisterMechCameraKeyEventCmd_Unmarshal_001
* @tc.desc  : Test Unmarshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechCameraKeyEventCmd_Unmarshal_001, TestSize.Level1)
{
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 15; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechCameraKeyEventCmd();
    EXPECT_NE(executionCmd, nullptr);
    executionCmd->Unmarshal(bufferNull);
    bool ret = executionCmd->Unmarshal(buffer);
    EXPECT_EQ(ret, true);
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_TriggerResponse_001
* @tc.number: RegisterMechCameraKeyEventCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function.
*/
HWTEST_F(MechCommandTest, RegisterMechCameraKeyEventCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 15; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechCameraKeyEventCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferNull));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_GetResult_001
* @tc.number: RegisterMechCameraKeyEventCmd_GetResult_001
* @tc.desc  : Test GetResult function.
*/
HWTEST_F(MechCommandTest, RegisterMechCameraKeyEventCmd_GetResult_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechCameraKeyEventCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetResult());
}

/**
* @tc.name  : RegisterMechControlResultCmd_Marshal_001
* @tc.number: RegisterMechControlResultCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechControlResultCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechControlResultCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

/**
* @tc.name  : RegisterMechControlResultCmd_Unmarshal_001
* @tc.number: RegisterMechControlResultCmd_Unmarshal_001
* @tc.desc  : Test Unmarshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechControlResultCmd_Unmarshal_001, TestSize.Level1)
{
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 15; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechControlResultCmd();
    EXPECT_NE(executionCmd, nullptr);
    executionCmd->Unmarshal(bufferNull);
    bool ret = executionCmd->Unmarshal(buffer);
    EXPECT_EQ(ret, true);
}

/**
* @tc.name  : RegisterMechControlResultCmd_TriggerResponse_001
* @tc.number: RegisterMechControlResultCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function.
*/
HWTEST_F(MechCommandTest, RegisterMechControlResultCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 15; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechControlResultCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferNull));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
}

/**
* @tc.name  : RegisterMechControlResultCmd_GetControlResult_001
* @tc.number: RegisterMechControlResultCmd_GetControlResult_001
* @tc.desc  : Test GetControlResult function.
*/
HWTEST_F(MechCommandTest, RegisterMechControlResultCmd_GetControlResult_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechControlResultCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetControlResult());
}

/**
* @tc.name  : RegisterMechControlResultCmd_GetResult_001
* @tc.number: RegisterMechControlResultCmd_GetResult_001
* @tc.desc  : Test GetResult function.
*/
HWTEST_F(MechCommandTest, RegisterMechControlResultCmd_GetResult_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechControlResultCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetResult());
}

/**
* @tc.name  : RegisterMechControlResultCmd_GetTaskId_001
* @tc.number: RegisterMechControlResultCmd_GetTaskId_001
* @tc.desc  : Test GetTaskId function.
*/
HWTEST_F(MechCommandTest, RegisterMechControlResultCmd_GetTaskId_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechControlResultCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetTaskId());
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_Marshal_001
* @tc.number: RegisterMechPositionInfoCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechPositionInfoCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechPositionInfoCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_Unmarshal_001
* @tc.number: RegisterMechPositionInfoCmd_Unmarshal_001
* @tc.desc  : Test Unmarshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechPositionInfoCmd_Unmarshal_001, TestSize.Level1)
{
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 15; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechPositionInfoCmd();
    EXPECT_NE(executionCmd, nullptr);
    executionCmd->Unmarshal(bufferNull);
    bool ret = executionCmd->Unmarshal(buffer);
    EXPECT_EQ(ret, true);
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_TriggerResponse_001
* @tc.number: RegisterMechPositionInfoCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function.
*/
HWTEST_F(MechCommandTest, RegisterMechPositionInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechPositionInfoCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferNull));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_GetPosition_001
* @tc.number: RegisterMechPositionInfoCmd_GetPosition_001
* @tc.desc  : Test GetPosition function.
*/
HWTEST_F(MechCommandTest, RegisterMechPositionInfoCmd_GetPosition_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechPositionInfoCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetPosition());
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_GetResult_001
* @tc.number: RegisterMechPositionInfoCmd_GetResult_001
* @tc.desc  : Test GetResult function.
*/
HWTEST_F(MechCommandTest, RegisterMechPositionInfoCmd_GetResult_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechPositionInfoCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetResult());
}

/**
* @tc.name  : RegisterMechStateInfoCmd_Marshal_001
* @tc.number: RegisterMechStateInfoCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechStateInfoCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechStateInfoCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

/**
* @tc.name  : RegisterMechStateInfoCmd_Unmarshal_001
* @tc.number: RegisterMechStateInfoCmd_Unmarshal_001
* @tc.desc  : Test Unmarshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechStateInfoCmd_Unmarshal_001, TestSize.Level1)
{
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechStateInfoCmd();
    EXPECT_NE(executionCmd, nullptr);
    executionCmd->Unmarshal(bufferNull);
    bool ret = executionCmd->Unmarshal(buffer);
    EXPECT_EQ(ret, true);
}

/**
* @tc.name  : RegisterMechStateInfoCmd_TriggerResponse_001
* @tc.number: RegisterMechStateInfoCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function.
*/
HWTEST_F(MechCommandTest, RegisterMechStateInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechStateInfoCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferNull));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
}

/**
* @tc.name  : RegisterMechStateInfoCmd_GetResult_001
* @tc.number: RegisterMechStateInfoCmd_GetResult_001
* @tc.desc  : Test GetResult function.
*/
HWTEST_F(MechCommandTest, RegisterMechStateInfoCmd_GetResult_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechStateInfoCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetResult());
}

/**
* @tc.name  : RegisterMechWheelDataCmd_Marshal_001
* @tc.number: RegisterMechWheelDataCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechWheelDataCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechWheelDataCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

/**
* @tc.name  : RegisterMechWheelDataCmd_Unmarshal_001
* @tc.number: RegisterMechWheelDataCmd_Unmarshal_001
* @tc.desc  : Test Unmarshal function.
*/
HWTEST_F(MechCommandTest, RegisterMechWheelDataCmd_Unmarshal_001, TestSize.Level1)
{
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechWheelDataCmd();
    EXPECT_NE(executionCmd, nullptr);
    executionCmd->Unmarshal(bufferNull);
    bool ret = executionCmd->Unmarshal(buffer);
    EXPECT_EQ(ret, true);
}

/**
* @tc.name  : RegisterMechWheelDataCmd_TriggerResponse_001
* @tc.number: RegisterMechWheelDataCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function.
*/
HWTEST_F(MechCommandTest, RegisterMechWheelDataCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateRegisterMechWheelDataCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferNull));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
}

/**
* @tc.name  : RegisterMechWheelDataCmd_GetWheelData_001
* @tc.number: RegisterMechWheelDataCmd_GetWheelData_001
* @tc.desc  : Test GetWheelData function.
*/
HWTEST_F(MechCommandTest, RegisterMechWheelDataCmd_GetWheelData_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechWheelDataCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetWheelData());
}

/**
* @tc.name  : RegisterMechWheelDataCmd_GetResult_001
* @tc.number: RegisterMechWheelDataCmd_GetResult_001
* @tc.desc  : Test GetResult function.
*/
HWTEST_F(MechCommandTest, RegisterMechWheelDataCmd_GetResult_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateRegisterMechWheelDataCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetResult());
}

/**
* @tc.name  : SetMechCameraInfoCmd_Marshal_001
* @tc.number: SetMechCameraInfoCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(MechCommandTest, SetMechCameraInfoCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    CameraInfoParams params;
    params.fovH = 1;
    params.fovV = 1;
    params.cameraType = CameraType::FRONT;
    auto executionCmd = factory.CreateSetMechCameraInfoCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

/**
* @tc.name  : SetMechCameraInfoCmd_TriggerResponse_001
* @tc.number: SetMechCameraInfoCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function.
*/
HWTEST_F(MechCommandTest, SetMechCameraInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    CameraInfoParams params;
    params.fovH = 1;
    params.fovV = 1;
    params.cameraType = CameraType::FRONT;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateSetMechCameraInfoCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferNull));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechCameraInfoCmd_GetParams_001
* @tc.number: SetMechCameraInfoCmd_GetParams_001
* @tc.desc  : Test GetParams function.
*/
HWTEST_F(MechCommandTest, SetMechCameraInfoCmd_GetParams_001, TestSize.Level1)
{
    CommandFactory factory;
    CameraInfoParams params;
    params.fovH = 1;
    params.fovV = 1;
    params.cameraType = CameraType::FRONT;
    auto executionCmd = factory.CreateSetMechCameraInfoCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetParams());
}

/**
* @tc.name  : SetMechCameraTrackingFrameCmd_TriggerResponse_001
* @tc.number: SetMechCameraTrackingFrameCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function.
*/
HWTEST_F(MechCommandTest, SetMechCameraTrackingFrameCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    TrackingFrameParams params;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateSetMechCameraTrackingFrameCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferNull));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechCameraTrackingFrameCmd_GetParams_001
* @tc.number: SetMechCameraTrackingFrameCmd_GetParams_001
* @tc.desc  : Test GetParams function.
*/
HWTEST_F(MechCommandTest, SetMechCameraTrackingFrameCmd_GetParams_001, TestSize.Level1)
{
    CommandFactory factory;
    TrackingFrameParams params;
    auto executionCmd = factory.CreateSetMechCameraTrackingFrameCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetParams());
}

/**
* @tc.name  : SetMechCameraTrackingLayoutCmd_TriggerResponse_001
* @tc.number: SetMechCameraTrackingLayoutCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function.
*/
HWTEST_F(MechCommandTest, SetMechCameraTrackingLayoutCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    LayoutParams params;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateSetMechCameraTrackingLayoutCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferNull));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechCameraTrackingLayoutCmd_GetParams_001
* @tc.number: SetMechCameraTrackingLayoutCmd_GetParams_001
* @tc.desc  : Test GetParams function.
*/
HWTEST_F(MechCommandTest, SetMechCameraTrackingLayoutCmd_GetParams_001, TestSize.Level1)
{
    CommandFactory factory;
    LayoutParams params;
    auto executionCmd = factory.CreateSetMechCameraTrackingLayoutCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetParams());
}

/**
* @tc.name  : SetMechCameraTrackingLayoutCmd_GetResult_001
* @tc.number: SetMechCameraTrackingLayoutCmd_GetResult_001
* @tc.desc  : Test GetResult function.
*/
HWTEST_F(MechCommandTest, SetMechCameraTrackingLayoutCmd_GetResult_001, TestSize.Level1)
{
    CommandFactory factory;
    LayoutParams params;
    auto executionCmd = factory.CreateSetMechCameraTrackingLayoutCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetResult());
}

/**
* @tc.name  : SetMechConfigCmd_Marshal_001
* @tc.number: SetMechConfigCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(MechCommandTest, SetMechConfigCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    uint8_t configVersion = 1;
    auto executionCmd = factory.CreateSetMechConfigCmd(configVersion);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

/**
* @tc.name  : SetMechConfigCmd_TriggerResponse_001
* @tc.number: SetMechConfigCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function.
*/
HWTEST_F(MechCommandTest, SetMechConfigCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    uint8_t configVersion = 1;
    auto executionCmd = factory.CreateSetMechConfigCmd(configVersion);
    EXPECT_NE(executionCmd, nullptr);
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferNull));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechConfigCmd_GetResult_001
* @tc.number: SetMechConfigCmd_GetResult_001
* @tc.desc  : Test GetResult function.
*/
HWTEST_F(MechCommandTest, SetMechConfigCmd_GetResult_001, TestSize.Level1)
{
    CommandFactory factory;
    uint8_t configVersion = 1;
    auto executionCmd = factory.CreateSetMechConfigCmd(configVersion);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetResult());
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_TriggerResponse_001
* @tc.number: SetMechRotationBySpeedCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function.
*/
HWTEST_F(MechCommandTest, SetMechRotationBySpeedCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    RotateBySpeedParam params;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateSetMechRotationBySpeedCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferNull));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_GetParams_001
* @tc.number: SetMechRotationBySpeedCmd_GetParams_001
* @tc.desc  : Test GetParams function.
*/
HWTEST_F(MechCommandTest, SetMechRotationBySpeedCmd_GetParams_001, TestSize.Level1)
{
    CommandFactory factory;
    RotateBySpeedParam params;
    auto executionCmd = factory.CreateSetMechRotationBySpeedCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetParams());
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_GetResult_001
* @tc.number: SetMechRotationBySpeedCmd_GetResult_001
* @tc.desc  : Test GetResult function.
*/
HWTEST_F(MechCommandTest, SetMechRotationBySpeedCmd_GetResult_001, TestSize.Level1)
{
    CommandFactory factory;
    RotateBySpeedParam params;
    auto executionCmd = factory.CreateSetMechRotationBySpeedCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetResult());
}

/**
* @tc.name  : SetMechRotationCmd_TriggerResponse_001
* @tc.number: SetMechRotationCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function.
*/
HWTEST_F(MechCommandTest, SetMechRotationCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    RotateParam params;
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateSetMechRotationCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferNull));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechRotationCmd_GetParams_001
* @tc.number: SetMechRotationCmd_GetParams_001
* @tc.desc  : Test GetParams function.
*/
HWTEST_F(MechCommandTest, SetMechRotationCmd_GetParams_001, TestSize.Level1)
{
    CommandFactory factory;
    RotateParam params;
    auto executionCmd = factory.CreateSetMechRotationCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetParams());
}

/**
* @tc.name  : SetMechRotationCmd_GetResult_001
* @tc.number: SetMechRotationCmd_GetResult_001
* @tc.desc  : Test GetResult function.
*/
HWTEST_F(MechCommandTest, SetMechRotationCmd_GetResult_001, TestSize.Level1)
{
    CommandFactory factory;
    RotateParam params;
    auto executionCmd = factory.CreateSetMechRotationCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetResult());
}

/**
* @tc.name  : SetMechRotationTraceCmd_TriggerResponse_001
* @tc.number: SetMechRotationTraceCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function.
*/
HWTEST_F(MechCommandTest, SetMechRotationTraceCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    RotateParam param;
    std::vector<RotateParam> params;
    params.push_back(param);
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    auto executionCmd = factory.CreateSetMechRotationTraceCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferNull));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechRotationTraceCmd_GetParams_001
* @tc.number: SetMechRotationTraceCmd_GetParams_001
* @tc.desc  : Test GetParams function.
*/
HWTEST_F(MechCommandTest, SetMechRotationTraceCmd_GetParams_001, TestSize.Level1)
{
    CommandFactory factory;
    RotateParam param;
    std::vector<RotateParam> params;
    params.push_back(param);
    auto executionCmd = factory.CreateSetMechRotationTraceCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetParams());
}

/**
* @tc.name  : SetMechRotationTraceCmd_GetResult_001
* @tc.number: SetMechRotationTraceCmd_GetResult_001
* @tc.desc  : Test GetResult function.
*/
HWTEST_F(MechCommandTest, SetMechRotationTraceCmd_GetResult_001, TestSize.Level1)
{
    CommandFactory factory;
    RotateParam param;
    std::vector<RotateParam> params;
    params.push_back(param);
    auto executionCmd = factory.CreateSetMechRotationTraceCmd(params);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetResult());
}

/**
* @tc.name  : SetMechStopCmd_TriggerResponse_001
* @tc.number: SetMechStopCmd_TriggerResponse_001
* @tc.desc  : Test TriggerResponse function.
*/
HWTEST_F(MechCommandTest, SetMechStopCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateSetMechStopCmd();
    EXPECT_NE(executionCmd, nullptr);
    size_t capacity = 100;
    auto buffer = std::make_shared<MechDataBuffer>(capacity);
    auto bufferNull = std::make_shared<MechDataBuffer>(capacity);
    uint8_t value = 0x02;
    for (int i = 0; i < 10; i++) {
        buffer->AppendUint8(value);
    }
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferNull));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechStopCmd_GetResult_001
* @tc.number: SetMechStopCmd_GetResult_001
* @tc.desc  : Test GetResult function.
*/
HWTEST_F(MechCommandTest, SetMechStopCmd_GetResult_001, TestSize.Level1)
{
    CommandFactory factory;
    auto executionCmd = factory.CreateSetMechStopCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->GetResult());
}
}
}