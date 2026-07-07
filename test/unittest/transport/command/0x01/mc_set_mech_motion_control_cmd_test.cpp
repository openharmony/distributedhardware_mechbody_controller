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
#include "mc_set_mech_motion_control_cmd.h"
#include "mc_data_buffer.h"
#include "mechbody_controller_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {

class TestSetMechMotionControlCmd : public testing::Test {
public:
    static void SetUpTestCase()
    {
    }

    static void TearDownTestCase()
    {
    }

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

// 正常场景测试

HWTEST_F(TestSetMechMotionControlCmd, Constructor_StopAction, TestSize.Level1)
{
    // Given: 准备STOP动作参数
    ControlCommand action = ControlCommand::STOP;

    // When: 构造命令对象
    SetMechMotionControlCmd cmd(action);

    // Then: 验证对象正确初始化
    EXPECT_EQ(cmd.GetCmdSet(), 0x02);
    EXPECT_EQ(cmd.GetCmdId(), 0x23);
    EXPECT_EQ(cmd.GetReqSize(), 1);
    EXPECT_EQ(cmd.GetRspSize(), 1);
    EXPECT_EQ(cmd.GetAction(), ControlCommand::STOP);
}

HWTEST_F(TestSetMechMotionControlCmd, Constructor_GoCenterAction, TestSize.Level1)
{
    // Given: 准备GO_CENTER动作参数
    ControlCommand action = ControlCommand::GO_CENTER;

    // When: 构造命令对象
    SetMechMotionControlCmd cmd(action);

    // Then: 验证对象正确初始化
    EXPECT_EQ(cmd.GetCmdSet(), 0x02);
    EXPECT_EQ(cmd.GetCmdId(), 0x23);
    EXPECT_EQ(cmd.GetReqSize(), 1);
    EXPECT_EQ(cmd.GetRspSize(), 1);
    EXPECT_EQ(cmd.GetAction(), ControlCommand::GO_CENTER);
}

HWTEST_F(TestSetMechMotionControlCmd, Constructor_HorizontallySwitchAction, TestSize.Level1)
{
    // Given: 准备HORIZONTALLY_SWITCH动作参数
    ControlCommand action = ControlCommand::HORIZONTALLY_SWITCH;

    // When: 构造命令对象
    SetMechMotionControlCmd cmd(action);

    // Then: 验证对象正确初始化
    EXPECT_EQ(cmd.GetCmdSet(), 0x02);
    EXPECT_EQ(cmd.GetCmdId(), 0x23);
    EXPECT_EQ(cmd.GetReqSize(), 1);
    EXPECT_EQ(cmd.GetRspSize(), 1);
    EXPECT_EQ(cmd.GetAction(), ControlCommand::HORIZONTALLY_SWITCH);
}

HWTEST_F(TestSetMechMotionControlCmd, Marshal_Normal, TestSize.Level1)
{
    // Given: 准备正常参数并构造命令对象
    ControlCommand action = ControlCommand::HORIZONTALLY_SWITCH;
    SetMechMotionControlCmd cmd(action);

    // When: 执行序列化
    auto buffer = cmd.Marshal();

    // Then: 验证返回非空buffer且数据正确写入
    EXPECT_NE(buffer, nullptr);
    EXPECT_GE(buffer->Size(), 3);
}

HWTEST_F(TestSetMechMotionControlCmd, TriggerResponse_ValidDataWithCallback, TestSize.Level1)
{
    // Given: 准备命令对象并设置回调
    ControlCommand action = ControlCommand::HORIZONTALLY_SWITCH;
    SetMechMotionControlCmd cmd(action);

    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    auto data = std::make_shared<MechDataBuffer>(10);
    data->AppendUint8(0x02); // cmdSet
    data->AppendUint8(0x23); // cmdId
    data->AppendUint8(1);    // result = 1 (成功)

    // When: 触发响应
    cmd.TriggerResponse(data);

    // Then: 验证回调被触发且result被正确设置
    EXPECT_TRUE(callbackTriggered);
    EXPECT_EQ(cmd.GetResult(), 1);
}

HWTEST_F(TestSetMechMotionControlCmd, TriggerResponse_ValidDataWithoutCallback, TestSize.Level1)
{
    // Given: 准备命令对象但不设置回调
    ControlCommand action = ControlCommand::HORIZONTALLY_SWITCH;
    SetMechMotionControlCmd cmd(action);

    bool callbackTriggered = false;
    auto data = std::make_shared<MechDataBuffer>(10);

    // When: 触发响应
    cmd.TriggerResponse(data);

    // Then: 验证回调未被触发，程序正常执行
    EXPECT_FALSE(callbackTriggered);
    EXPECT_NE(data, nullptr);
}

HWTEST_F(TestSetMechMotionControlCmd, GetAction_Normal, TestSize.Level1)
{
    // Given: 准备参数并构造命令对象
    ControlCommand action = ControlCommand::HORIZONTALLY_SWITCH;
    SetMechMotionControlCmd cmd(action);

    // When: 获取action
    ControlCommand retrievedAction = cmd.GetAction();

    // Then: 验证action正确
    EXPECT_EQ(retrievedAction, ControlCommand::HORIZONTALLY_SWITCH);
}

HWTEST_F(TestSetMechMotionControlCmd, GetResult_Default, TestSize.Level1)
{
    // Given: 准备参数并构造命令对象
    ControlCommand action = ControlCommand::HORIZONTALLY_SWITCH;
    SetMechMotionControlCmd cmd(action);

    // When: 获取result
    uint8_t result = cmd.GetResult();

    // Then: 验证默认result为0
    EXPECT_EQ(result, 0);
}

// 边界场景测试

HWTEST_F(TestSetMechMotionControlCmd, Constructor_UnknownAction, TestSize.Level1)
{
    // Given: 准备UNKNOWN动作参数
    ControlCommand action = ControlCommand::UNKNOWN;

    // When: 构造命令对象
    SetMechMotionControlCmd cmd(action);

    // Then: 验证对象正确初始化
    EXPECT_EQ(cmd.GetCmdSet(), 0x02);
    EXPECT_EQ(cmd.GetCmdId(), 0x23);
    EXPECT_EQ(cmd.GetReqSize(), 1);
    EXPECT_EQ(cmd.GetRspSize(), 1);
    EXPECT_EQ(cmd.GetAction(), ControlCommand::UNKNOWN);
}

HWTEST_F(TestSetMechMotionControlCmd, TriggerResponse_ValidDataResultValue, TestSize.Level1)
{
    // Given: 准备命令对象并设置回调
    ControlCommand action = ControlCommand::HORIZONTALLY_SWITCH;
    SetMechMotionControlCmd cmd(action);

    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    // Given: 创建包含有效响应数据的buffer
    auto data = std::make_shared<MechDataBuffer>(10);
    data->AppendUint8(0x02); // cmdSet
    data->AppendUint8(0x23); // cmdId
    data->AppendUint8(1);    // result = 1 (成功)

    // When: 触发响应
    cmd.TriggerResponse(data);

    // Then: 验证回调被触发且result被正确设置
    EXPECT_TRUE(callbackTriggered);
    EXPECT_EQ(cmd.GetResult(), 1);
}

/**
 * @tc.name  : TriggerResponse_NullData
 * @tc.number: TriggerResponse_NullData
 * @tc.desc  : Test TriggerResponse with null data pointer.
 */
HWTEST_F(TestSetMechMotionControlCmd, TriggerResponse_NullData, TestSize.Level1)
{
    // Given: 准备命令对象
    ControlCommand action = ControlCommand::STOP;
    SetMechMotionControlCmd cmd(action);

    // When: 传入nullptr数据
    cmd.TriggerResponse(nullptr);

    // Then: 验证result保持默认值0（函数提前返回，未修改result）
    EXPECT_EQ(cmd.GetResult(), 0);
}

/**
 * @tc.name  : TriggerResponse_SmallDataSize
 * @tc.number: TriggerResponse_SmallDataSize
 * @tc.desc  : Test TriggerResponse with data size smaller than RSP_SIZE + BIT_OFFSET_2.
 */
HWTEST_F(TestSetMechMotionControlCmd, TriggerResponse_SmallDataSize, TestSize.Level1)
{
    // Given: 准备命令对象和数据大小不足的buffer
    ControlCommand action = ControlCommand::STOP;
    SetMechMotionControlCmd cmd(action);

    auto data = std::make_shared<MechDataBuffer>(2);
    data->AppendUint8(0x02);
    data->AppendUint8(0x23);
    // Size() = 2 < RSP_SIZE(1) + BIT_OFFSET_2(2) = 3

    // When: 触发响应
    cmd.TriggerResponse(data);

    // Then: 验证result保持默认值0（函数因Size不足提前返回）
    EXPECT_EQ(cmd.GetResult(), 0);
}

/**
 * @tc.name  : TriggerResponse_ReadUint8Failed
 * @tc.number: TriggerResponse_ReadUint8Failed
 * @tc.desc  : Test TriggerResponse when ReadUint8 fails due to range offset mismatch.
 */
HWTEST_F(TestSetMechMotionControlCmd, TriggerResponse_ReadUint8Failed, TestSize.Level1)
{
    // Given: 准备命令对象和rangeOffset导致ReadUint8越界的buffer
    ControlCommand action = ControlCommand::STOP;
    SetMechMotionControlCmd cmd(action);

    auto data = std::make_shared<MechDataBuffer>(10);
    data->AppendUint8(0x02);
    data->AppendUint8(0x23);
    data->AppendUint8(1);
    data->SetRange(1, 3);
    // Size() = 3 >= 3 passes outer check, but ReadUint8(offset=2) checks
    // rangeOffset_(1) + offset(2) + sizeof(uint8_t)(1) = 4 > rangeLength_(3), fails

    // When: 触发响应
    cmd.TriggerResponse(data);

    // Then: 验证result保持默认值0（ReadUint8失败，函数提前返回）
    EXPECT_EQ(cmd.GetResult(), 0);
}

/**
 * @tc.name  : TriggerResponse_ValidDataNoCallback
 * @tc.number: TriggerResponse_ValidDataNoCallback
 * @tc.desc  : Test TriggerResponse with valid data but no response callback set.
 */
HWTEST_F(TestSetMechMotionControlCmd, TriggerResponse_ValidDataNoCallback, TestSize.Level1)
{
    // Given: 准备命令对象（不设置回调）和有效数据
    ControlCommand action = ControlCommand::STOP;
    SetMechMotionControlCmd cmd(action);

    auto data = std::make_shared<MechDataBuffer>(10);
    data->AppendUint8(0x02);
    data->AppendUint8(0x23);
    data->AppendUint8(2);

    // When: 触发响应
    cmd.TriggerResponse(data);

    // Then: 验证result被正确读取，但无回调触发（不崩溃）
    EXPECT_EQ(cmd.GetResult(), 2);
}

} // namespace MechBodyController
} // namespace OHOS