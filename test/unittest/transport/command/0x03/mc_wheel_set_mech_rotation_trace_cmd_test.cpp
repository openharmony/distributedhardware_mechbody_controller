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
#include <memory>

#include "mc_wheel_set_mech_rotation_trace_cmd.h"
#include "mc_command_factory.h"
#include "mechbody_controller_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {

namespace {
    constexpr uint8_t CMD_SET = 0x03;
    constexpr uint8_t CMD_ID = 0x21;
    constexpr uint8_t CONTROL_BYTE = 0x00;
    constexpr uint8_t RESERVE_BYTE = 0x00;
    constexpr uint16_t DEFAULT_DURATION = 1000;
    constexpr int16_t DEFAULT_FORWARD_SPEED = 500;
    constexpr float DEFAULT_TURNING_SPEED = 1.0f;
    constexpr uint16_t ROTATE_PARAM_SIZE = 8;
    constexpr uint16_t HEADER_SIZE = 8;
    constexpr uint16_t MAX_POINT_SIZE = 16;
}

class WheelSetMechRotationTraceCmdTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
    }

    static void TearDownTestCase()
    {
    }

    void SetUp()
    {
    }

    void TearDown()
    {
    }

protected:
    void VerifyRotateParamInBuffer(const std::shared_ptr<MechDataBuffer>& buffer,
        uint16_t offset, uint16_t expectedDuration, int16_t expectedForwardSpeed,
        float expectedTurningSpeed)
    {
        uint16_t duration = 0;
        int16_t forwardSpeed = 0;
        float turningSpeed = 0.0f;
        EXPECT_EQ(buffer->ReadUint16(offset, duration), ERR_OK);
        EXPECT_EQ(buffer->ReadInt16(offset + 2, forwardSpeed), ERR_OK);
        EXPECT_EQ(buffer->ReadFloat(offset + 4, turningSpeed), ERR_OK);
        EXPECT_EQ(duration, expectedDuration);
        EXPECT_EQ(forwardSpeed, expectedForwardSpeed);
        EXPECT_FLOAT_EQ(turningSpeed, expectedTurningSpeed);
    }

    void VerifyBufferHeader(const std::shared_ptr<MechDataBuffer>& buffer,
        uint16_t expectedTaskId, uint8_t expectedPointNum)
    {
        uint8_t cmdSet = 0;
        uint8_t cmdId = 0;
        uint16_t taskId = 0;
        uint8_t controlByte = 0;
        uint8_t reserve1 = 0;
        uint8_t reserve2 = 0;
        uint8_t pointNum = 0;
        EXPECT_EQ(buffer->ReadUint8(0, cmdSet), ERR_OK);
        EXPECT_EQ(buffer->ReadUint8(1, cmdId), ERR_OK);
        EXPECT_EQ(buffer->ReadUint16(2, taskId), ERR_OK);
        EXPECT_EQ(buffer->ReadUint8(4, controlByte), ERR_OK);
        EXPECT_EQ(buffer->ReadUint8(5, reserve1), ERR_OK);
        EXPECT_EQ(buffer->ReadUint8(6, reserve2), ERR_OK);
        EXPECT_EQ(buffer->ReadUint8(7, pointNum), ERR_OK);
        EXPECT_EQ(cmdSet, CMD_SET);
        EXPECT_EQ(cmdId, CMD_ID);
        EXPECT_EQ(taskId, expectedTaskId);
        EXPECT_EQ(controlByte, CONTROL_BYTE);
        EXPECT_EQ(reserve1, RESERVE_BYTE);
        EXPECT_EQ(reserve2, RESERVE_BYTE);
        EXPECT_EQ(pointNum, expectedPointNum);
    }

    void CreateRotateParam(RotateParam& param, uint16_t duration, int16_t forwardSpeed,
        float turningSpeed)
    {
        param.duration = duration;
        param.forwardSpeed = forwardSpeed;
        param.turningSpeed = turningSpeed;
    }
};

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_Constructor_001
 * @tc.number: WheelSetMechRotationTraceCmd_Constructor_001
 * @tc.desc  : Test constructor with single point.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_Constructor_001, TestSize.Level1)
{
    // Given: 创建包含单个点的参数
    std::vector<RotateParam> params;
    RotateParam param;
    CreateRotateParam(param, DEFAULT_DURATION, DEFAULT_FORWARD_SPEED, DEFAULT_TURNING_SPEED);
    params.push_back(param);

    // When: 创建命令对象
    WheelSetMechRotationTraceCmd cmd(100, params);

    // Then: 验证命令属性
    EXPECT_EQ(cmd.GetCmdSet(), CMD_SET);
    EXPECT_EQ(cmd.GetCmdId(), CMD_ID);
    EXPECT_EQ(cmd.GetReqSize(), HEADER_SIZE + ROTATE_PARAM_SIZE);
    EXPECT_EQ(cmd.GetRspSize(), 3);

    const auto& cmdParams = cmd.GetParams();
    EXPECT_EQ(cmdParams.size(), 1);
    EXPECT_EQ(cmdParams[0].duration, DEFAULT_DURATION);
    EXPECT_EQ(cmdParams[0].forwardSpeed, DEFAULT_FORWARD_SPEED);
    EXPECT_FLOAT_EQ(cmdParams[0].turningSpeed, DEFAULT_TURNING_SPEED);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_Constructor_002
 * @tc.number: WheelSetMechRotationTraceCmd_Constructor_002
 * @tc.desc  : Test constructor with multiple points.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_Constructor_002, TestSize.Level1)
{
    // Given: 创建包含多个点的参数
    std::vector<RotateParam> params;
    for (int i = 0; i < 3; i++) {
        RotateParam param;
        CreateRotateParam(param, DEFAULT_DURATION + i * 500, DEFAULT_FORWARD_SPEED + i * 100,
            DEFAULT_TURNING_SPEED + i * 0.5f);
        params.push_back(param);
    }

    // When: 创建命令对象
    WheelSetMechRotationTraceCmd cmd(200, params);

    // Then: 验证命令属性
    EXPECT_EQ(cmd.GetCmdSet(), CMD_SET);
    EXPECT_EQ(cmd.GetCmdId(), CMD_ID);
    EXPECT_EQ(cmd.GetReqSize(), HEADER_SIZE + 3 * ROTATE_PARAM_SIZE);
    EXPECT_EQ(cmd.GetRspSize(), 3);

    const auto& cmdParams = cmd.GetParams();
    EXPECT_EQ(cmdParams.size(), 3);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_Constructor_003
 * @tc.number: WheelSetMechRotationTraceCmd_Constructor_003
 * @tc.desc  : Test constructor with points exceeding max size.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_Constructor_003, TestSize.Level1)
{
    // Given: 创建超过最大点数的参数
    std::vector<RotateParam> params;
    for (int i = 0; i < MAX_POINT_SIZE + 1; i++) {
        RotateParam param;
        CreateRotateParam(param, DEFAULT_DURATION, DEFAULT_FORWARD_SPEED, DEFAULT_TURNING_SPEED);
        params.push_back(param);
    }

    // When: 创建命令对象
    WheelSetMechRotationTraceCmd cmd(300, params);

    // Then: 验证命令属性
    EXPECT_EQ(cmd.GetCmdSet(), CMD_SET);
    EXPECT_EQ(cmd.GetCmdId(), CMD_ID);
    const auto& cmdParams = cmd.GetParams();
    EXPECT_EQ(cmdParams.size(), MAX_POINT_SIZE + 1);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_Constructor_004
 * @tc.number: WheelSetMechRotationTraceCmd_Constructor_004
 * @tc.desc  : Test constructor with empty params.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_Constructor_004, TestSize.Level1)
{
    // Given: 创建空参数
    std::vector<RotateParam> params;

    // When: 创建命令对象
    WheelSetMechRotationTraceCmd cmd(400, params);

    // Then: 验证命令属性
    EXPECT_EQ(cmd.GetCmdSet(), CMD_SET);
    EXPECT_EQ(cmd.GetCmdId(), CMD_ID);
    EXPECT_EQ(cmd.GetReqSize(), HEADER_SIZE);
    EXPECT_EQ(cmd.GetRspSize(), 3);

    const auto& cmdParams = cmd.GetParams();
    EXPECT_EQ(cmdParams.size(), 0);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_Constructor_005
 * @tc.number: WheelSetMechRotationTraceCmd_Constructor_005
 * @tc.desc  : Test constructor with max points (16).
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_Constructor_005, TestSize.Level1)
{
    // Given: 创建包含最大点数的参数
    std::vector<RotateParam> params;
    for (int i = 0; i < MAX_POINT_SIZE; i++) {
        RotateParam param;
        CreateRotateParam(param, DEFAULT_DURATION, DEFAULT_FORWARD_SPEED, DEFAULT_TURNING_SPEED);
        params.push_back(param);
    }

    // When: 创建命令对象
    WheelSetMechRotationTraceCmd cmd(500, params);

    // Then: 验证命令属性
    EXPECT_EQ(cmd.GetCmdSet(), CMD_SET);
    EXPECT_EQ(cmd.GetCmdId(), CMD_ID);
    EXPECT_EQ(cmd.GetReqSize(), HEADER_SIZE + MAX_POINT_SIZE * ROTATE_PARAM_SIZE);
    EXPECT_EQ(cmd.GetRspSize(), 3);

    const auto& cmdParams = cmd.GetParams();
    EXPECT_EQ(cmdParams.size(), MAX_POINT_SIZE);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_Marshal_001
 * @tc.number: WheelSetMechRotationTraceCmd_Marshal_001
 * @tc.desc  : Test Marshal function with single point.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建包含单个点的命令对象
    std::vector<RotateParam> params;
    RotateParam param;
    CreateRotateParam(param, DEFAULT_DURATION, DEFAULT_FORWARD_SPEED, DEFAULT_TURNING_SPEED);
    params.push_back(param);
    WheelSetMechRotationTraceCmd cmd(600, params);

    // When: 调用Marshal方法
    auto buffer = cmd.Marshal();

    // Then: 验证返回值和buffer内容
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), HEADER_SIZE + ROTATE_PARAM_SIZE);
    VerifyBufferHeader(buffer, 600, 1);
    VerifyRotateParamInBuffer(buffer, HEADER_SIZE, DEFAULT_DURATION, DEFAULT_FORWARD_SPEED,
        DEFAULT_TURNING_SPEED);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_Marshal_002
 * @tc.number: WheelSetMechRotationTraceCmd_Marshal_002
 * @tc.desc  : Test Marshal function with multiple points.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_Marshal_002, TestSize.Level1)
{
    // Given: 创建包含多个点的命令对象
    std::vector<RotateParam> params;
    for (int i = 0; i < 3; i++) {
        RotateParam param;
        CreateRotateParam(param, DEFAULT_DURATION + i * 500, DEFAULT_FORWARD_SPEED + i * 100,
            DEFAULT_TURNING_SPEED + i * 0.5f);
        params.push_back(param);
    }
    WheelSetMechRotationTraceCmd cmd(700, params);

    // When: 调用Marshal方法
    auto buffer = cmd.Marshal();

    // Then: 验证返回值和buffer内容
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), HEADER_SIZE + 3 * ROTATE_PARAM_SIZE);
    VerifyBufferHeader(buffer, 700, 3);
    VerifyRotateParamInBuffer(buffer, HEADER_SIZE, DEFAULT_DURATION, DEFAULT_FORWARD_SPEED,
        DEFAULT_TURNING_SPEED);
    VerifyRotateParamInBuffer(buffer, HEADER_SIZE + ROTATE_PARAM_SIZE, 1500, 600, 1.5f);
    VerifyRotateParamInBuffer(buffer, HEADER_SIZE + 2 * ROTATE_PARAM_SIZE, 2000, 700, 2.0f);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_TriggerResponse_001
 * @tc.number: WheelSetMechRotationTraceCmd_TriggerResponse_001
 * @tc.desc  : Test TriggerResponse with nullptr data.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建命令对象
    std::vector<RotateParam> params;
    RotateParam param;
    CreateRotateParam(param, DEFAULT_DURATION, DEFAULT_FORWARD_SPEED, DEFAULT_TURNING_SPEED);
    params.push_back(param);
    WheelSetMechRotationTraceCmd cmd(900, params);
    uint8_t initialResult = cmd.GetResult();

    // When: 调用TriggerResponse方法，传入nullptr
    cmd.TriggerResponse(nullptr);

    // Then: 验证结果未改变
    EXPECT_EQ(cmd.GetResult(), initialResult);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_TriggerResponse_002
 * @tc.number: WheelSetMechRotationTraceCmd_TriggerResponse_002
 * @tc.desc  : Test TriggerResponse with insufficient data size.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建命令对象和小buffer
    std::vector<RotateParam> params;
    RotateParam param;
    CreateRotateParam(param, DEFAULT_DURATION, DEFAULT_FORWARD_SPEED, DEFAULT_TURNING_SPEED);
    params.push_back(param);
    WheelSetMechRotationTraceCmd cmd(1000, params);
    uint8_t initialResult = cmd.GetResult();
    auto buffer = std::make_shared<MechDataBuffer>(1);
    ASSERT_NE(buffer, nullptr);

    // When: 调用TriggerResponse方法，传入数据不足的buffer
    cmd.TriggerResponse(buffer);

    // Then: 验证结果未改变
    EXPECT_EQ(cmd.GetResult(), initialResult);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_TriggerResponse_003
 * @tc.number: WheelSetMechRotationTraceCmd_TriggerResponse_003
 * @tc.desc  : Test TriggerResponse with normal data and no callback.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_TriggerResponse_003, TestSize.Level1)
{
    // Given: 创建命令对象和正常buffer
    std::vector<RotateParam> params;
    RotateParam param;
    CreateRotateParam(param, DEFAULT_DURATION, DEFAULT_FORWARD_SPEED, DEFAULT_TURNING_SPEED);
    params.push_back(param);
    WheelSetMechRotationTraceCmd cmd(1100, params);
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(CMD_SET);
    buffer->AppendUint8(CMD_ID);
    buffer->AppendUint16(1100);
    buffer->AppendUint8(1);

    // When: 调用TriggerResponse方法
    cmd.TriggerResponse(buffer);

    // Then: 验证结果
    EXPECT_EQ(cmd.GetResult(), 1);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_TriggerResponse_004
 * @tc.number: WheelSetMechRotationTraceCmd_TriggerResponse_004
 * @tc.desc  : Test TriggerResponse with normal data and callback.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_TriggerResponse_004, TestSize.Level1)
{
    // Given: 创建命令对象、正常buffer和回调
    std::vector<RotateParam> params;
    RotateParam param;
    CreateRotateParam(param, DEFAULT_DURATION, DEFAULT_FORWARD_SPEED, DEFAULT_TURNING_SPEED);
    params.push_back(param);
    WheelSetMechRotationTraceCmd cmd(1200, params);
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(CMD_SET);
    buffer->AppendUint8(CMD_ID);
    buffer->AppendUint8(0);
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    // When: 调用TriggerResponse方法
    cmd.TriggerResponse(buffer);

    // Then: 验证结果和回调
    EXPECT_EQ(cmd.GetResult(), 0);
    EXPECT_FALSE(callbackTriggered);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_GetParams_001
 * @tc.number: WheelSetMechRotationTraceCmd_GetParams_001
 * @tc.desc  : Test GetParams function.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_GetParams_001, TestSize.Level1)
{
    // Given: 创建包含多个点的命令对象
    std::vector<RotateParam> params;
    for (int i = 0; i < 3; i++) {
        RotateParam param;
        CreateRotateParam(param, DEFAULT_DURATION + i * 500, DEFAULT_FORWARD_SPEED + i * 100,
            DEFAULT_TURNING_SPEED + i * 0.5f);
        params.push_back(param);
    }
    WheelSetMechRotationTraceCmd cmd(1400, params);

    // When: 调用GetParams方法
    const auto& cmdParams = cmd.GetParams();

    // Then: 验证参数
    EXPECT_EQ(cmdParams.size(), 3);
    EXPECT_EQ(cmdParams[0].duration, DEFAULT_DURATION);
    EXPECT_EQ(cmdParams[1].duration, 1500);
    EXPECT_EQ(cmdParams[2].duration, 2000);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_GetResult_002
 * @tc.number: WheelSetMechRotationTraceCmd_GetResult_002
 * @tc.desc  : Test GetResult function with failure.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_GetResult_002, TestSize.Level1)
{
    // Given: 创建命令对象和buffer
    std::vector<RotateParam> params;
    RotateParam param;
    CreateRotateParam(param, DEFAULT_DURATION, DEFAULT_FORWARD_SPEED, DEFAULT_TURNING_SPEED);
    params.push_back(param);
    WheelSetMechRotationTraceCmd cmd(1600, params);
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(CMD_SET);
    buffer->AppendUint8(CMD_ID);
    buffer->AppendUint8(0);

    // When: 调用TriggerResponse方法
    cmd.TriggerResponse(buffer);

    // Then: 验证结果
    EXPECT_EQ(cmd.GetResult(), 0);
}

} // namespace MechBodyController
} // namespace OHOS