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
};

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_Constructor_001
 * @tc.number: WheelSetMechRotationTraceCmd_Constructor_001
 * @tc.desc  : Test constructor with single point.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_Constructor_001, TestSize.Level1)
{
    std::vector<RotateParam> params;
    RotateParam param;
    param.duration = 1000;
    param.forwardSpeed = 500;
    param.turningSpeed = 1.0f;
    params.push_back(param);

    WheelSetMechRotationTraceCmd cmd(100, params);
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x21);
    EXPECT_EQ(cmd.GetReqSize(), 14); // 6 + 1 * 8
    EXPECT_EQ(cmd.GetRspSize(), 3);

    const auto& cmdParams = cmd.GetParams();
    EXPECT_EQ(cmdParams.size(), 1);
    EXPECT_EQ(cmdParams[0].duration, 1000);
    EXPECT_EQ(cmdParams[0].forwardSpeed, 500);
    EXPECT_FLOAT_EQ(cmdParams[0].turningSpeed, 1.0f);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_Constructor_002
 * @tc.number: WheelSetMechRotationTraceCmd_Constructor_002
 * @tc.desc  : Test constructor with multiple points.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_Constructor_002, TestSize.Level1)
{
    std::vector<RotateParam> params;
    for (int i = 0; i < 3; i++) {
        RotateParam param;
        param.duration = 1000 + i * 500;
        param.forwardSpeed = 500 + i * 100;
        param.turningSpeed = 1.0f + i * 0.5f;
        params.push_back(param);
    }

    WheelSetMechRotationTraceCmd cmd(200, params);
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x21);
    EXPECT_EQ(cmd.GetReqSize(), 30); // 6 + 3 * 8
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
    std::vector<RotateParam> params;
    for (int i = 0; i < 17; i++) { // CMD_MAX_POINT_SIZE = 16
        RotateParam param;
        param.duration = 1000;
        param.forwardSpeed = 500;
        param.turningSpeed = 1.0f;
        params.push_back(param);
    }

    WheelSetMechRotationTraceCmd cmd(300, params);
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x21);
    // reqSize_ should not be set when params exceed max size
    const auto& cmdParams = cmd.GetParams();
    EXPECT_EQ(cmdParams.size(), 17);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_Constructor_004
 * @tc.number: WheelSetMechRotationTraceCmd_Constructor_004
 * @tc.desc  : Test constructor with empty params.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_Constructor_004, TestSize.Level1)
{
    std::vector<RotateParam> params;

    WheelSetMechRotationTraceCmd cmd(400, params);
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x21);
    EXPECT_EQ(cmd.GetReqSize(), 6); // 6 + 0 * 8
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
    std::vector<RotateParam> params;
    for (int i = 0; i < 16; i++) {
        RotateParam param;
        param.duration = 1000;
        param.forwardSpeed = 500;
        param.turningSpeed = 1.0f;
        params.push_back(param);
    }

    WheelSetMechRotationTraceCmd cmd(500, params);
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x21);
    EXPECT_EQ(cmd.GetReqSize(), 134); // 6 + 16 * 8
    EXPECT_EQ(cmd.GetRspSize(), 3);

    const auto& cmdParams = cmd.GetParams();
    EXPECT_EQ(cmdParams.size(), 16);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_Marshal_001
 * @tc.number: WheelSetMechRotationTraceCmd_Marshal_001
 * @tc.desc  : Test Marshal function with single point.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_Marshal_001, TestSize.Level1)
{
    std::vector<RotateParam> params;
    RotateParam param;
    param.duration = 1000;
    param.forwardSpeed = 500;
    param.turningSpeed = 1.0f;
    params.push_back(param);

    WheelSetMechRotationTraceCmd cmd(600, params);
    auto buffer = cmd.Marshal();
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), 16);
    uint8_t cmdSet = 0;
    uint8_t cmdId = 0;
    uint16_t taskId = 0;
    uint8_t controlByte = 0;
    uint8_t reserve1 = 0;
    uint8_t reserve2 = 0;
    uint8_t pointNum = 0;
    uint16_t duration = 0;
    int16_t forwardSpeed = 0;
    float turningSpeed = 0.0f;
    EXPECT_EQ(buffer->ReadUint8(0, cmdSet), ERR_OK);
    EXPECT_EQ(buffer->ReadUint8(1, cmdId), ERR_OK);
    EXPECT_EQ(buffer->ReadUint16(2, taskId), ERR_OK);
    EXPECT_EQ(buffer->ReadUint8(4, controlByte), ERR_OK);
    EXPECT_EQ(buffer->ReadUint8(5, reserve1), ERR_OK);
    EXPECT_EQ(buffer->ReadUint8(6, reserve2), ERR_OK);
    EXPECT_EQ(buffer->ReadUint8(7, pointNum), ERR_OK);
    EXPECT_EQ(buffer->ReadUint16(8, duration), ERR_OK);
    EXPECT_EQ(buffer->ReadInt16(10, forwardSpeed), ERR_OK);
    EXPECT_EQ(buffer->ReadFloat(12, turningSpeed), ERR_OK);
    EXPECT_EQ(cmdSet, 0x03);
    EXPECT_EQ(cmdId, 0x21);
    EXPECT_EQ(taskId, 600);
    EXPECT_EQ(controlByte, 0x00);
    EXPECT_EQ(reserve1, 0x00);
    EXPECT_EQ(reserve2, 0x00);
    EXPECT_EQ(pointNum, 1);
    EXPECT_EQ(duration, 1000);
    EXPECT_EQ(forwardSpeed, 500);
    EXPECT_FLOAT_EQ(turningSpeed, 1.0f);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_Marshal_002
 * @tc.number: WheelSetMechRotationTraceCmd_Marshal_002
 * @tc.desc  : Test Marshal function with multiple points.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_Marshal_002, TestSize.Level1)
{
    std::vector<RotateParam> params;
    for (int i = 0; i < 3; i++) {
        RotateParam param;
        param.duration = 1000 + i * 500;
        param.forwardSpeed = 500 + i * 100;
        param.turningSpeed = 1.0f + i * 0.5f;
        params.push_back(param);
    }

    WheelSetMechRotationTraceCmd cmd(700, params);
    auto buffer = cmd.Marshal();
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), 32);
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
    EXPECT_EQ(cmdSet, 0x03);
    EXPECT_EQ(cmdId, 0x21);
    EXPECT_EQ(taskId, 700);
    EXPECT_EQ(controlByte, 0x00);
    EXPECT_EQ(reserve1, 0x00);
    EXPECT_EQ(reserve2, 0x00);
    EXPECT_EQ(pointNum, 3);

    uint16_t duration1 = 0, duration2 = 0, duration3 = 0;
    int16_t forwardSpeed1 = 0, forwardSpeed2 = 0, forwardSpeed3 = 0;
    float turningSpeed1 = 0.0f, turningSpeed2 = 0.0f, turningSpeed3 = 0.0f;
    EXPECT_EQ(buffer->ReadUint16(8, duration1), ERR_OK);
    EXPECT_EQ(buffer->ReadInt16(10, forwardSpeed1), ERR_OK);
    EXPECT_EQ(buffer->ReadFloat(12, turningSpeed1), ERR_OK);
    EXPECT_EQ(buffer->ReadUint16(16, duration2), ERR_OK);
    EXPECT_EQ(buffer->ReadInt16(18, forwardSpeed2), ERR_OK);
    EXPECT_EQ(buffer->ReadFloat(20, turningSpeed2), ERR_OK);
    EXPECT_EQ(buffer->ReadUint16(24, duration3), ERR_OK);
    EXPECT_EQ(buffer->ReadInt16(26, forwardSpeed3), ERR_OK);
    EXPECT_EQ(buffer->ReadFloat(28, turningSpeed3), ERR_OK);

    EXPECT_EQ(duration1, 1000);
    EXPECT_EQ(forwardSpeed1, 500);
    EXPECT_FLOAT_EQ(turningSpeed1, 1.0f);
    EXPECT_EQ(duration2, 1500);
    EXPECT_EQ(forwardSpeed2, 600);
    EXPECT_FLOAT_EQ(turningSpeed2, 1.5f);
    EXPECT_EQ(duration3, 2000);
    EXPECT_EQ(forwardSpeed3, 700);
    EXPECT_FLOAT_EQ(turningSpeed3, 2.0f);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_TriggerResponse_001
 * @tc.number: WheelSetMechRotationTraceCmd_TriggerResponse_001
 * @tc.desc  : Test TriggerResponse with nullptr data.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_TriggerResponse_001, TestSize.Level1)
{
    std::vector<RotateParam> params;
    RotateParam param;
    param.duration = 1000;
    param.forwardSpeed = 500;
    param.turningSpeed = 1.0f;
    params.push_back(param);

    WheelSetMechRotationTraceCmd cmd(900, params);
    uint8_t initialResult = cmd.GetResult();
    cmd.TriggerResponse(nullptr);
    EXPECT_EQ(cmd.GetResult(), initialResult);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_TriggerResponse_002
 * @tc.number: WheelSetMechRotationTraceCmd_TriggerResponse_002
 * @tc.desc  : Test TriggerResponse with insufficient data size.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_TriggerResponse_002, TestSize.Level1)
{
    std::vector<RotateParam> params;
    RotateParam param;
    param.duration = 1000;
    param.forwardSpeed = 500;
    param.turningSpeed = 1.0f;
    params.push_back(param);

    WheelSetMechRotationTraceCmd cmd(1000, params);
    uint8_t initialResult = cmd.GetResult();
    auto buffer = std::make_shared<MechDataBuffer>(1);
    ASSERT_NE(buffer, nullptr);
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), initialResult);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_TriggerResponse_003
 * @tc.number: WheelSetMechRotationTraceCmd_TriggerResponse_003
 * @tc.desc  : Test TriggerResponse with normal data and no callback.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_TriggerResponse_003, TestSize.Level1)
{
    std::vector<RotateParam> params;
    RotateParam param;
    param.duration = 1000;
    param.forwardSpeed = 500;
    param.turningSpeed = 1.0f;
    params.push_back(param);

    WheelSetMechRotationTraceCmd cmd(1100, params);
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x21);
    buffer->AppendUint16(1100);
    buffer->AppendUint8(1);
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), 1);
}

/**
 * @tc.name  : WheelSetMechRotationTraceCmd_TriggerResponse_004
 * @tc.number: WheelSetMechRotationTraceCmd_TriggerResponse_004
 * @tc.desc  : Test TriggerResponse with normal data and callback.
 */
HWTEST_F(WheelSetMechRotationTraceCmdTest, WheelSetMechRotationTraceCmd_TriggerResponse_004, TestSize.Level1)
{
    std::vector<RotateParam> params;
    RotateParam param;
    param.duration = 1000;
    param.forwardSpeed = 500;
    param.turningSpeed = 1.0f;
    params.push_back(param);

    WheelSetMechRotationTraceCmd cmd(1200, params);
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x21);
    buffer->AppendUint8(0);
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });
    cmd.TriggerResponse(buffer);
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
    std::vector<RotateParam> params;
    for (int i = 0; i < 3; i++) {
        RotateParam param;
        param.duration = 1000 + i * 500;
        param.forwardSpeed = 500 + i * 100;
        param.turningSpeed = 1.0f + i * 0.5f;
        params.push_back(param);
    }

    WheelSetMechRotationTraceCmd cmd(1400, params);
    const auto& cmdParams = cmd.GetParams();
    EXPECT_EQ(cmdParams.size(), 3);
    EXPECT_EQ(cmdParams[0].duration, 1000);
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
    std::vector<RotateParam> params;
    RotateParam param;
    param.duration = 1000;
    param.forwardSpeed = 500;
    param.turningSpeed = 1.0f;
    params.push_back(param);

    WheelSetMechRotationTraceCmd cmd(1600, params);
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x21);
    buffer->AppendUint8(0);
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), 0);
}

} // namespace MechBodyController
} // namespace OHOS