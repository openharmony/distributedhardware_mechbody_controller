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
#include "mc_action_gimbal_feature_control_cmd.h"
#include "mc_command_factory.h"
#include "mechbody_controller_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {

class TestActionGimbalFeatureControlCmd : public testing::Test {
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

HWTEST_F(TestActionGimbalFeatureControlCmd, Constructor_NormalParams, TestSize.Level1)
{
    // Given: 准备正常参数
    ActionControlParams params;
    params.controlReq = 1;
    params.timeOut = 1000;
    params.yawControl = 50;
    params.pitchControl = 30;
    params.rollControl = 20;

    // When: 构造命令对象
    ActionGimbalFeatureControlCmd cmd(params);

    // Then: 验证对象正确初始化
    EXPECT_EQ(cmd.GetCmdSet(), 0x02);
    EXPECT_EQ(cmd.GetCmdId(), 0x26);
    EXPECT_EQ(cmd.GetReqSize(), 11);
    EXPECT_EQ(cmd.GetRspSize(), 1);
    EXPECT_EQ(cmd.GetParams().controlReq, 1);
}

HWTEST_F(TestActionGimbalFeatureControlCmd, Marshal_Normal, TestSize.Level1)
{
    // Given: 准备正常参数并构造命令对象
    ActionControlParams params;
    params.controlReq = 1;
    params.timeOut = 1000;
    params.yawControl = 50;
    params.pitchControl = 30;
    params.rollControl = 20;
    ActionGimbalFeatureControlCmd cmd(params);

    // When: 执行序列化
    auto buffer = cmd.Marshal();

    // Then: 验证返回非空buffer且数据正确写入
    EXPECT_NE(buffer, nullptr);
    EXPECT_GE(buffer->Size(), 13);
    EXPECT_EQ(cmd.GetParams().controlReq, 1);
}

HWTEST_F(TestActionGimbalFeatureControlCmd, TriggerResponse_WithCallback, TestSize.Level1)
{
    // Given: 准备命令对象并设置回调
    ActionControlParams params;
    params.controlReq = 1;
    params.timeOut = 1000;
    params.yawControl = 50;
    params.pitchControl = 30;
    params.rollControl = 20;
    ActionGimbalFeatureControlCmd cmd(params);

    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    auto data = std::make_shared<MechDataBuffer>(10);

    // When: 触发响应
    cmd.TriggerResponse(data);

    // Then: 验证回调被触发
    EXPECT_TRUE(callbackTriggered);
    EXPECT_NE(data, nullptr);
}

HWTEST_F(TestActionGimbalFeatureControlCmd, TriggerResponse_WithoutCallback, TestSize.Level1)
{
    // Given: 准备命令对象但不设置回调
    ActionControlParams params;
    params.controlReq = 1;
    params.timeOut = 1000;
    params.yawControl = 50;
    params.pitchControl = 30;
    params.rollControl = 20;
    ActionGimbalFeatureControlCmd cmd(params);

    bool callbackTriggered = false;
    auto data = std::make_shared<MechDataBuffer>(10);

    // When: 触发响应
    cmd.TriggerResponse(data);

    // Then: 验证回调未被触发，程序正常执行
    EXPECT_FALSE(callbackTriggered);
    EXPECT_NE(data, nullptr);
}

HWTEST_F(TestActionGimbalFeatureControlCmd, GetParams_Normal, TestSize.Level1)
{
    // Given: 准备参数并构造命令对象
    ActionControlParams params;
    params.controlReq = 1;
    params.timeOut = 1000;
    params.yawControl = 50;
    params.pitchControl = 30;
    params.rollControl = 20;
    ActionGimbalFeatureControlCmd cmd(params);

    // When: 获取参数
    const ActionControlParams& retrievedParams = cmd.GetParams();

    // Then: 验证参数正确
    EXPECT_EQ(retrievedParams.controlReq, 1);
    EXPECT_EQ(retrievedParams.timeOut, 1000);
    EXPECT_EQ(retrievedParams.yawControl, 50);
    EXPECT_EQ(retrievedParams.pitchControl, 30);
    EXPECT_EQ(retrievedParams.rollControl, 20);
}

HWTEST_F(TestActionGimbalFeatureControlCmd, Constructor_AllMinValues, TestSize.Level1)
{
    // Given: 准备各字段最小值
    ActionControlParams params;
    params.controlReq = 0;
    params.timeOut = 0;
    params.yawControl = 0;
    params.pitchControl = 0;
    params.rollControl = 0;

    // When: 构造命令对象
    ActionGimbalFeatureControlCmd cmd(params);

    // Then: 验证最小值正确保存
    EXPECT_EQ(cmd.GetParams().controlReq, 0);
    EXPECT_EQ(cmd.GetParams().timeOut, 0);
    EXPECT_EQ(cmd.GetParams().yawControl, 0);
    EXPECT_EQ(cmd.GetParams().pitchControl, 0);
    EXPECT_EQ(cmd.GetParams().rollControl, 0);
}

HWTEST_F(TestActionGimbalFeatureControlCmd, Constructor_AllMaxValues, TestSize.Level1)
{
    // Given: 准备各字段最大值
    ActionControlParams params;
    params.controlReq = 255;
    params.timeOut = 65535;
    params.yawControl = 4294967295;
    params.pitchControl = 4294967295;
    params.rollControl = 4294967295;

    // When: 构造命令对象
    ActionGimbalFeatureControlCmd cmd(params);

    // Then: 验证最大值正确保存
    EXPECT_EQ(cmd.GetParams().controlReq, 255);
    EXPECT_EQ(cmd.GetParams().timeOut, 65535);
    EXPECT_EQ(cmd.GetParams().yawControl, 4294967295);
    EXPECT_EQ(cmd.GetParams().pitchControl, 4294967295);
    EXPECT_EQ(cmd.GetParams().rollControl, 4294967295);
}

// 极端值和边界值测试

HWTEST_F(TestActionGimbalFeatureControlCmd, Constructor_ControlReq_Min, TestSize.Level1)
{
    // Given: controlReq为最小值0
    ActionControlParams params;
    params.controlReq = 0;
    params.timeOut = 1000;
    params.yawControl = 50;
    params.pitchControl = 30;
    params.rollControl = 20;

    // When: 构造命令对象
    ActionGimbalFeatureControlCmd cmd(params);

    // Then: 验证controlReq为0
    EXPECT_EQ(cmd.GetParams().controlReq, 0);
    EXPECT_EQ(cmd.GetCmdSet(), 0x02);
    EXPECT_EQ(cmd.GetCmdId(), 0x26);
}

HWTEST_F(TestActionGimbalFeatureControlCmd, Constructor_ControlReq_Max, TestSize.Level1)
{
    // Given: controlReq为最大值255
    ActionControlParams params;
    params.controlReq = 255;
    params.timeOut = 1000;
    params.yawControl = 50;
    params.pitchControl = 30;
    params.rollControl = 20;

    // When: 构造命令对象
    ActionGimbalFeatureControlCmd cmd(params);

    // Then: 验证controlReq为255
    EXPECT_EQ(cmd.GetParams().controlReq, 255);
    EXPECT_EQ(cmd.GetCmdSet(), 0x02);
    EXPECT_EQ(cmd.GetCmdId(), 0x26);
}

HWTEST_F(TestActionGimbalFeatureControlCmd, Constructor_TimeOut_Min, TestSize.Level1)
{
    // Given: timeOut为最小值0
    ActionControlParams params;
    params.controlReq = 1;
    params.timeOut = 0;
    params.yawControl = 50;
    params.pitchControl = 30;
    params.rollControl = 20;

    // When: 构造命令对象
    ActionGimbalFeatureControlCmd cmd(params);

    // Then: 验证timeOut为0
    EXPECT_EQ(cmd.GetParams().timeOut, 0);
    EXPECT_EQ(cmd.GetCmdSet(), 0x02);
    EXPECT_EQ(cmd.GetCmdId(), 0x26);
}

HWTEST_F(TestActionGimbalFeatureControlCmd, Constructor_TimeOut_Max, TestSize.Level1)
{
    // Given: timeOut为最大值65535
    ActionControlParams params;
    params.controlReq = 1;
    params.timeOut = 65535;
    params.yawControl = 50;
    params.pitchControl = 30;
    params.rollControl = 20;

    // When: 构造命令对象
    ActionGimbalFeatureControlCmd cmd(params);

    // Then: 验证timeOut为65535
    EXPECT_EQ(cmd.GetParams().timeOut, 65535);
    EXPECT_EQ(cmd.GetCmdSet(), 0x02);
    EXPECT_EQ(cmd.GetCmdId(), 0x26);
}

HWTEST_F(TestActionGimbalFeatureControlCmd, Constructor_YawControl_Min, TestSize.Level1)
{
    // Given: yawControl为最小值0
    ActionControlParams params;
    params.controlReq = 1;
    params.timeOut = 1000;
    params.yawControl = 0;
    params.pitchControl = 30;
    params.rollControl = 20;

    // When: 构造命令对象
    ActionGimbalFeatureControlCmd cmd(params);

    // Then: 验证yawControl为0
    EXPECT_EQ(cmd.GetParams().yawControl, 0);
    EXPECT_EQ(cmd.GetCmdSet(), 0x02);
    EXPECT_EQ(cmd.GetCmdId(), 0x26);
}

HWTEST_F(TestActionGimbalFeatureControlCmd, Constructor_YawControl_Max, TestSize.Level1)
{
    // Given: yawControl为最大值4294967295
    ActionControlParams params;
    params.controlReq = 1;
    params.timeOut = 1000;
    params.yawControl = 4294967295;
    params.pitchControl = 30;
    params.rollControl = 20;

    // When: 构造命令对象
    ActionGimbalFeatureControlCmd cmd(params);

    // Then: 验证yawControl为4294967295
    EXPECT_EQ(cmd.GetParams().yawControl, 4294967295);
    EXPECT_EQ(cmd.GetCmdSet(), 0x02);
    EXPECT_EQ(cmd.GetCmdId(), 0x26);
}

HWTEST_F(TestActionGimbalFeatureControlCmd, Constructor_PitchControl_Min, TestSize.Level1)
{
    // Given: pitchControl为最小值0
    ActionControlParams params;
    params.controlReq = 1;
    params.timeOut = 1000;
    params.yawControl = 50;
    params.pitchControl = 0;
    params.rollControl = 20;

    // When: 构造命令对象
    ActionGimbalFeatureControlCmd cmd(params);

    // Then: 验证pitchControl为0
    EXPECT_EQ(cmd.GetParams().pitchControl, 0);
    EXPECT_EQ(cmd.GetCmdSet(), 0x02);
    EXPECT_EQ(cmd.GetCmdId(), 0x26);
}

HWTEST_F(TestActionGimbalFeatureControlCmd, Constructor_PitchControl_Max, TestSize.Level1)
{
    // Given: pitchControl为最大值4294967295
    ActionControlParams params;
    params.controlReq = 1;
    params.timeOut = 1000;
    params.yawControl = 50;
    params.pitchControl = 4294967295;
    params.rollControl = 20;

    // When: 构造命令对象
    ActionGimbalFeatureControlCmd cmd(params);

    // Then: 验证pitchControl为4294967295
    EXPECT_EQ(cmd.GetParams().pitchControl, 4294967295);
    EXPECT_EQ(cmd.GetCmdSet(), 0x02);
    EXPECT_EQ(cmd.GetCmdId(), 0x26);
}

HWTEST_F(TestActionGimbalFeatureControlCmd, Constructor_RollControl_Min, TestSize.Level1)
{
    // Given: rollControl为最小值0
    ActionControlParams params;
    params.controlReq = 1;
    params.timeOut = 1000;
    params.yawControl = 50;
    params.pitchControl = 30;
    params.rollControl = 0;

    // When: 构造命令对象
    ActionGimbalFeatureControlCmd cmd(params);

    // Then: 验证rollControl为0
    EXPECT_EQ(cmd.GetParams().rollControl, 0);
    EXPECT_EQ(cmd.GetCmdSet(), 0x02);
    EXPECT_EQ(cmd.GetCmdId(), 0x26);
}

HWTEST_F(TestActionGimbalFeatureControlCmd, Constructor_RollControl_Max, TestSize.Level1)
{
    // Given: rollControl为最大值4294967295
    ActionControlParams params;
    params.controlReq = 1;
    params.timeOut = 1000;
    params.yawControl = 50;
    params.pitchControl = 30;
    params.rollControl = 4294967295;

    // When: 构造命令对象
    ActionGimbalFeatureControlCmd cmd(params);

    // Then: 验证rollControl为4294967295
    EXPECT_EQ(cmd.GetParams().rollControl, 4294967295);
    EXPECT_EQ(cmd.GetCmdSet(), 0x02);
    EXPECT_EQ(cmd.GetCmdId(), 0x26);
}

// 异常场景测试

HWTEST_F(TestActionGimbalFeatureControlCmd, TriggerResponse_NullData, TestSize.Level1)
{
    // Given: 准备命令对象并设置回调
    ActionControlParams params;
    params.controlReq = 1;
    params.timeOut = 1000;
    params.yawControl = 50;
    params.pitchControl = 30;
    params.rollControl = 20;
    ActionGimbalFeatureControlCmd cmd(params);

    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    std::shared_ptr<MechDataBuffer> data = nullptr;

    // When: 触发响应，传入nullptr
    cmd.TriggerResponse(data);

    // Then: 验证回调被触发
    EXPECT_TRUE(callbackTriggered);
}

} // namespace MechBodyController
} // namespace OHOS