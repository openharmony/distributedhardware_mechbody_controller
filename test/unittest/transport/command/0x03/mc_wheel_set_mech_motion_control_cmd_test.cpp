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

#include "mc_wheel_set_mech_motion_control_cmd.h"
#include "mc_command_factory.h"
#include "mechbody_controller_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {

class WheelSetMechMotionControlCmdTest : public testing::Test {
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
 * @tc.name  : WheelSetMechMotionControlCmd_Constructor_001
 * @tc.number: WheelSetMechMotionControlCmd_Constructor_001
 * @tc.desc  : Test constructor with STOP action.
 */
HWTEST_F(WheelSetMechMotionControlCmdTest, WheelSetMechMotionControlCmd_Constructor_001, TestSize.Level1)
{
    WheelSetMechMotionControlCmd cmd(ControlCommand::STOP);
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x23);
    EXPECT_EQ(cmd.GetReqSize(), 1);
    EXPECT_EQ(cmd.GetRspSize(), 1);
    EXPECT_EQ(cmd.GetAction(), ControlCommand::STOP);
}

/**
 * @tc.name  : WheelSetMechMotionControlCmd_Constructor_002
 * @tc.number: WheelSetMechMotionControlCmd_Constructor_002
 * @tc.desc  : Test constructor with GO_CENTER action.
 */
HWTEST_F(WheelSetMechMotionControlCmdTest, WheelSetMechMotionControlCmd_Constructor_002, TestSize.Level1)
{
    WheelSetMechMotionControlCmd cmd(ControlCommand::GO_CENTER);
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x23);
    EXPECT_EQ(cmd.GetReqSize(), 1);
    EXPECT_EQ(cmd.GetRspSize(), 1);
    EXPECT_EQ(cmd.GetAction(), ControlCommand::GO_CENTER);
}

/**
 * @tc.name  : WheelSetMechMotionControlCmd_Constructor_003
 * @tc.number: WheelSetMechMotionControlCmd_Constructor_003
 * @tc.desc  : Test constructor with HORIZONTALLY_SWITCH action.
 */
HWTEST_F(WheelSetMechMotionControlCmdTest, WheelSetMechMotionControlCmd_Constructor_003, TestSize.Level1)
{
    WheelSetMechMotionControlCmd cmd(ControlCommand::HORIZONTALLY_SWITCH);
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x23);
    EXPECT_EQ(cmd.GetReqSize(), 1);
    EXPECT_EQ(cmd.GetRspSize(), 1);
    EXPECT_EQ(cmd.GetAction(), ControlCommand::HORIZONTALLY_SWITCH);
}

/**
 * @tc.name  : WheelSetMechMotionControlCmd_Constructor_004
 * @tc.number: WheelSetMechMotionControlCmd_Constructor_004
 * @tc.desc  : Test constructor with UNKNOWN action.
 */
HWTEST_F(WheelSetMechMotionControlCmdTest, WheelSetMechMotionControlCmd_Constructor_004, TestSize.Level1)
{
    WheelSetMechMotionControlCmd cmd(ControlCommand::UNKNOWN);
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x23);
    EXPECT_EQ(cmd.GetReqSize(), 1);
    EXPECT_EQ(cmd.GetRspSize(), 1);
    EXPECT_EQ(cmd.GetAction(), ControlCommand::UNKNOWN);
}

/**
 * @tc.name  : WheelSetMechMotionControlCmd_Marshal_001
 * @tc.number: WheelSetMechMotionControlCmd_Marshal_001
 * @tc.desc  : Test Marshal function with STOP action.
 */
HWTEST_F(WheelSetMechMotionControlCmdTest, WheelSetMechMotionControlCmd_Marshal_001, TestSize.Level1)
{
    WheelSetMechMotionControlCmd cmd(ControlCommand::STOP);
    auto buffer = cmd.Marshal();
    ASSERT_NE(buffer, nullptr);
    EXPECT_GE(buffer->Size(), 3);
}

/**
 * @tc.name  : WheelSetMechMotionControlCmd_Marshal_002
 * @tc.number: WheelSetMechMotionControlCmd_Marshal_002
 * @tc.desc  : Test Marshal function with GO_CENTER action.
 */
HWTEST_F(WheelSetMechMotionControlCmdTest, WheelSetMechMotionControlCmd_Marshal_002, TestSize.Level1)
{
    WheelSetMechMotionControlCmd cmd(ControlCommand::GO_CENTER);
    auto buffer = cmd.Marshal();
    ASSERT_NE(buffer, nullptr);
    EXPECT_GE(buffer->Size(), 3);
}

/**
 * @tc.name  : WheelSetMechMotionControlCmd_Marshal_003
 * @tc.number: WheelSetMechMotionControlCmd_Marshal_003
 * @tc.desc  : Test Marshal function with HORIZONTALLY_SWITCH action.
 */
HWTEST_F(WheelSetMechMotionControlCmdTest, WheelSetMechMotionControlCmd_Marshal_003, TestSize.Level1)
{
    WheelSetMechMotionControlCmd cmd(ControlCommand::HORIZONTALLY_SWITCH);
    auto buffer = cmd.Marshal();
    ASSERT_NE(buffer, nullptr);
    EXPECT_GE(buffer->Size(), 3);
}

/**
 * @tc.name  : WheelSetMechMotionControlCmd_TriggerResponse_001
 * @tc.number: WheelSetMechMotionControlCmd_TriggerResponse_001
 * @tc.desc  : Test TriggerResponse with nullptr data.
 */
HWTEST_F(WheelSetMechMotionControlCmdTest, WheelSetMechMotionControlCmd_TriggerResponse_001, TestSize.Level1)
{
    WheelSetMechMotionControlCmd cmd(ControlCommand::STOP);
    uint8_t initialResult = cmd.GetResult();
    cmd.TriggerResponse(nullptr);
    EXPECT_EQ(cmd.GetResult(), initialResult);
}

/**
 * @tc.name  : WheelSetMechMotionControlCmd_TriggerResponse_002
 * @tc.number: WheelSetMechMotionControlCmd_TriggerResponse_002
 * @tc.desc  : Test TriggerResponse with insufficient data size.
 */
HWTEST_F(WheelSetMechMotionControlCmdTest, WheelSetMechMotionControlCmd_TriggerResponse_002, TestSize.Level1)
{
    WheelSetMechMotionControlCmd cmd(ControlCommand::STOP);
    uint8_t initialResult = cmd.GetResult();
    auto buffer = std::make_shared<MechDataBuffer>(1);
    ASSERT_NE(buffer, nullptr);
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), initialResult);
}

/**
 * @tc.name  : WheelSetMechMotionControlCmd_TriggerResponse_003
 * @tc.number: WheelSetMechMotionControlCmd_TriggerResponse_003
 * @tc.desc  : Test TriggerResponse with normal data and no callback.
 */
HWTEST_F(WheelSetMechMotionControlCmdTest, WheelSetMechMotionControlCmd_TriggerResponse_003, TestSize.Level1)
{
    WheelSetMechMotionControlCmd cmd(ControlCommand::STOP);
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x23);
    buffer->AppendUint8(1);
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), 1);
}

/**
 * @tc.name  : WheelSetMechMotionControlCmd_TriggerResponse_004
 * @tc.number: WheelSetMechMotionControlCmd_TriggerResponse_004
 * @tc.desc  : Test TriggerResponse with normal data and callback.
 */
HWTEST_F(WheelSetMechMotionControlCmdTest, WheelSetMechMotionControlCmd_TriggerResponse_004, TestSize.Level1)
{
    WheelSetMechMotionControlCmd cmd(ControlCommand::GO_CENTER);
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x23);
    buffer->AppendUint8(0);
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), 0);
    EXPECT_TRUE(callbackTriggered);
}

/**
 * @tc.name  : WheelSetMechMotionControlCmd_GetResult_002
 * @tc.number: WheelSetMechMotionControlCmd_GetResult_002
 * @tc.desc  : Test GetResult function with zero result.
 */
HWTEST_F(WheelSetMechMotionControlCmdTest, WheelSetMechMotionControlCmd_GetResult_002, TestSize.Level1)
{
    WheelSetMechMotionControlCmd cmd(ControlCommand::GO_CENTER);
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x23);
    buffer->AppendUint8(0);
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), 0);
}

} // namespace MechBodyController
} // namespace OHOS