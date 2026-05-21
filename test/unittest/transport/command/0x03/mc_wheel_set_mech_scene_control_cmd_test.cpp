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

#include "mc_wheel_set_mech_scene_control_cmd.h"
#include "mc_command_factory.h"
#include "mechbody_controller_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {

class WheelSetMechSceneControlCmdTest : public testing::Test {
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
 * @tc.name  : WheelSetMechSceneControlCmd_Constructor_001
 * @tc.number: WheelSetMechSceneControlCmd_Constructor_001
 * @tc.desc  : Test constructor with LANDSCAPE_PORTRAIT_SWITCH action.
 */
HWTEST_F(WheelSetMechSceneControlCmdTest, WheelSetMechSceneControlCmd_Constructor_001, TestSize.Level1)
{
    WheelSetMechSceneControlCmd cmd(ActionType::LANDSCAPE_PORTRAIT_SWITCH);
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x24);
    EXPECT_EQ(cmd.GetReqSize(), 4);
    EXPECT_EQ(cmd.GetRspSize(), 1);
    EXPECT_EQ(cmd.GetActionType(), ActionType::LANDSCAPE_PORTRAIT_SWITCH);
}

/**
 * @tc.name  : WheelSetMechSceneControlCmd_Constructor_002
 * @tc.number: WheelSetMechSceneControlCmd_Constructor_002
 * @tc.desc  : Test constructor with PATROL_MODE action.
 */
HWTEST_F(WheelSetMechSceneControlCmdTest, WheelSetMechSceneControlCmd_Constructor_002, TestSize.Level1)
{
    WheelSetMechSceneControlCmd cmd(ActionType::PATROL_MODE);
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x24);
    EXPECT_EQ(cmd.GetReqSize(), 4);
    EXPECT_EQ(cmd.GetRspSize(), 1);
    EXPECT_EQ(cmd.GetActionType(), ActionType::PATROL_MODE);
}

/**
 * @tc.name  : WheelSetMechSceneControlCmd_Constructor_003
 * @tc.number: WheelSetMechSceneControlCmd_Constructor_003
 * @tc.desc  : Test constructor with HEAD_UP action.
 */
HWTEST_F(WheelSetMechSceneControlCmdTest, WheelSetMechSceneControlCmd_Constructor_003, TestSize.Level1)
{
    WheelSetMechSceneControlCmd cmd(ActionType::HEAD_UP);
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x24);
    EXPECT_EQ(cmd.GetReqSize(), 4);
    EXPECT_EQ(cmd.GetRspSize(), 1);
    EXPECT_EQ(cmd.GetActionType(), ActionType::HEAD_UP);
}

/**
 * @tc.name  : WheelSetMechSceneControlCmd_Constructor_004
 * @tc.number: WheelSetMechSceneControlCmd_Constructor_004
 * @tc.desc  : Test constructor with HAPPY action.
 */
HWTEST_F(WheelSetMechSceneControlCmdTest, WheelSetMechSceneControlCmd_Constructor_004, TestSize.Level1)
{
    WheelSetMechSceneControlCmd cmd(ActionType::HAPPY);
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x24);
    EXPECT_EQ(cmd.GetReqSize(), 4);
    EXPECT_EQ(cmd.GetRspSize(), 1);
    EXPECT_EQ(cmd.GetActionType(), ActionType::HAPPY);
}

/**
 * @tc.name  : WheelSetMechSceneControlCmd_Constructor_005
 * @tc.number: WheelSetMechSceneControlCmd_Constructor_005
 * @tc.desc  : Test constructor with DANCE action.
 */
HWTEST_F(WheelSetMechSceneControlCmdTest, WheelSetMechSceneControlCmd_Constructor_005, TestSize.Level1)
{
    WheelSetMechSceneControlCmd cmd(ActionType::DANCE);
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x24);
    EXPECT_EQ(cmd.GetReqSize(), 4);
    EXPECT_EQ(cmd.GetRspSize(), 1);
    EXPECT_EQ(cmd.GetActionType(), ActionType::DANCE);
}

/**
 * @tc.name  : WheelSetMechSceneControlCmd_Marshal_001
 * @tc.number: WheelSetMechSceneControlCmd_Marshal_001
 * @tc.desc  : Test Marshal function with LANDSCAPE_PORTRAIT_SWITCH action.
 */
HWTEST_F(WheelSetMechSceneControlCmdTest, WheelSetMechSceneControlCmd_Marshal_001, TestSize.Level1)
{
    WheelSetMechSceneControlCmd cmd(ActionType::LANDSCAPE_PORTRAIT_SWITCH);
    auto buffer = cmd.Marshal();
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), 6);
    uint8_t cmdSet = 0;
    uint8_t cmdId = 0;
    uint32_t actionType = 0;
    EXPECT_EQ(buffer->ReadUint8(0, cmdSet), ERR_OK);
    EXPECT_EQ(buffer->ReadUint8(1, cmdId), ERR_OK);
    EXPECT_EQ(buffer->ReadUint32(2, actionType), ERR_OK);
    EXPECT_EQ(cmdSet, 0x03);
    EXPECT_EQ(cmdId, 0x24);
    EXPECT_EQ(actionType, static_cast<uint32_t>(ActionType::LANDSCAPE_PORTRAIT_SWITCH));
}

/**
 * @tc.name  : WheelSetMechSceneControlCmd_Marshal_002
 * @tc.number: WheelSetMechSceneControlCmd_Marshal_002
 * @tc.desc  : Test Marshal function with HEAD_UP action.
 */
HWTEST_F(WheelSetMechSceneControlCmdTest, WheelSetMechSceneControlCmd_Marshal_002, TestSize.Level1)
{
    WheelSetMechSceneControlCmd cmd(ActionType::HEAD_UP);
    auto buffer = cmd.Marshal();
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), 6);
    uint8_t cmdSet = 0;
    uint8_t cmdId = 0;
    uint32_t actionType = 0;
    EXPECT_EQ(buffer->ReadUint8(0, cmdSet), ERR_OK);
    EXPECT_EQ(buffer->ReadUint8(1, cmdId), ERR_OK);
    EXPECT_EQ(buffer->ReadUint32(2, actionType), ERR_OK);
    EXPECT_EQ(cmdSet, 0x03);
    EXPECT_EQ(cmdId, 0x24);
    EXPECT_EQ(actionType, static_cast<uint32_t>(ActionType::HEAD_UP));
}

/**
 * @tc.name  : WheelSetMechSceneControlCmd_Marshal_003
 * @tc.number: WheelSetMechSceneControlCmd_Marshal_003
 * @tc.desc  : Test Marshal function with HAPPY action.
 */
HWTEST_F(WheelSetMechSceneControlCmdTest, WheelSetMechSceneControlCmd_Marshal_003, TestSize.Level1)
{
    WheelSetMechSceneControlCmd cmd(ActionType::HAPPY);
    auto buffer = cmd.Marshal();
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), 6);
    uint8_t cmdSet = 0;
    uint8_t cmdId = 0;
    uint32_t actionType = 0;
    EXPECT_EQ(buffer->ReadUint8(0, cmdSet), ERR_OK);
    EXPECT_EQ(buffer->ReadUint8(1, cmdId), ERR_OK);
    EXPECT_EQ(buffer->ReadUint32(2, actionType), ERR_OK);
    EXPECT_EQ(cmdSet, 0x03);
    EXPECT_EQ(cmdId, 0x24);
    EXPECT_EQ(actionType, static_cast<uint32_t>(ActionType::HAPPY));
}

/**
 * @tc.name  : WheelSetMechSceneControlCmd_Marshal_004
 * @tc.number: WheelSetMechSceneControlCmd_Marshal_004
 * @tc.desc  : Test Marshal function with PATROL_MODE action.
 */
HWTEST_F(WheelSetMechSceneControlCmdTest, WheelSetMechSceneControlCmd_Marshal_004, TestSize.Level1)
{
    WheelSetMechSceneControlCmd cmd(ActionType::PATROL_MODE);
    auto buffer = cmd.Marshal();
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), 6);
    uint8_t cmdSet = 0;
    uint8_t cmdId = 0;
    uint32_t actionType = 0;
    EXPECT_EQ(buffer->ReadUint8(0, cmdSet), ERR_OK);
    EXPECT_EQ(buffer->ReadUint8(1, cmdId), ERR_OK);
    EXPECT_EQ(buffer->ReadUint32(2, actionType), ERR_OK);
    EXPECT_EQ(cmdSet, 0x03);
    EXPECT_EQ(cmdId, 0x24);
    EXPECT_EQ(actionType, static_cast<uint32_t>(ActionType::PATROL_MODE));
}

/**
 * @tc.name  : WheelSetMechSceneControlCmd_Marshal_005
 * @tc.number: WheelSetMechSceneControlCmd_Marshal_005
 * @tc.desc  : Test Marshal function with DANCE action.
 */
HWTEST_F(WheelSetMechSceneControlCmdTest, WheelSetMechSceneControlCmd_Marshal_005, TestSize.Level1)
{
    WheelSetMechSceneControlCmd cmd(ActionType::DANCE);
    auto buffer = cmd.Marshal();
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), 6);
    uint8_t cmdSet = 0;
    uint8_t cmdId = 0;
    uint32_t actionType = 0;
    EXPECT_EQ(buffer->ReadUint8(0, cmdSet), ERR_OK);
    EXPECT_EQ(buffer->ReadUint8(1, cmdId), ERR_OK);
    EXPECT_EQ(buffer->ReadUint32(2, actionType), ERR_OK);
    EXPECT_EQ(cmdSet, 0x03);
    EXPECT_EQ(cmdId, 0x24);
    EXPECT_EQ(actionType, static_cast<uint32_t>(ActionType::DANCE));
}

/**
 * @tc.name  : WheelSetMechSceneControlCmd_TriggerResponse_001
 * @tc.number: WheelSetMechSceneControlCmd_TriggerResponse_001
 * @tc.desc  : Test TriggerResponse with nullptr data.
 */
HWTEST_F(WheelSetMechSceneControlCmdTest, WheelSetMechSceneControlCmd_TriggerResponse_001, TestSize.Level1)
{
    WheelSetMechSceneControlCmd cmd(ActionType::LANDSCAPE_PORTRAIT_SWITCH);
    uint8_t initialResult = cmd.GetResult();
    cmd.TriggerResponse(nullptr);
    EXPECT_EQ(cmd.GetResult(), initialResult);
}

/**
 * @tc.name  : WheelSetMechSceneControlCmd_TriggerResponse_002
 * @tc.number: WheelSetMechSceneControlCmd_TriggerResponse_002
 * @tc.desc  : Test TriggerResponse with insufficient data size.
 */
HWTEST_F(WheelSetMechSceneControlCmdTest, WheelSetMechSceneControlCmd_TriggerResponse_002, TestSize.Level1)
{
    WheelSetMechSceneControlCmd cmd(ActionType::PATROL_MODE);
    uint8_t initialResult = cmd.GetResult();
    auto buffer = std::make_shared<MechDataBuffer>(1);
    ASSERT_NE(buffer, nullptr);
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), initialResult);
}

/**
 * @tc.name  : WheelSetMechSceneControlCmd_TriggerResponse_003
 * @tc.number: WheelSetMechSceneControlCmd_TriggerResponse_003
 * @tc.desc  : Test TriggerResponse with normal data and no callback.
 */
HWTEST_F(WheelSetMechSceneControlCmdTest, WheelSetMechSceneControlCmd_TriggerResponse_003, TestSize.Level1)
{
    WheelSetMechSceneControlCmd cmd(ActionType::HEAD_UP);
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x24);
    buffer->AppendUint8(1);
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), 1);
}

/**
 * @tc.name  : WheelSetMechSceneControlCmd_TriggerResponse_004
 * @tc.number: WheelSetMechSceneControlCmd_TriggerResponse_004
 * @tc.desc  : Test TriggerResponse with normal data and callback.
 */
HWTEST_F(WheelSetMechSceneControlCmdTest, WheelSetMechSceneControlCmd_TriggerResponse_004, TestSize.Level1)
{
    WheelSetMechSceneControlCmd cmd(ActionType::HAPPY);
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x24);
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
 * @tc.name  : WheelSetMechSceneControlCmd_TriggerResponse_005
 * @tc.number: WheelSetMechSceneControlCmd_TriggerResponse_005
 * @tc.desc  : Test TriggerResponse with error code.
 */
HWTEST_F(WheelSetMechSceneControlCmdTest, WheelSetMechSceneControlCmd_TriggerResponse_005, TestSize.Level1)
{
    WheelSetMechSceneControlCmd cmd(ActionType::DANCE);
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x24);
    buffer->AppendUint8(255);
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), 255);
}

} // namespace MechBodyController
} // namespace OHOS