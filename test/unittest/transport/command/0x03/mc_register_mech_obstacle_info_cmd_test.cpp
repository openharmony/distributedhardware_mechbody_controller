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

#include "mc_register_mech_obstacle_info_cmd.h"
#include "mc_command_factory.h"
#include "mechbody_controller_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {

class RegisterMechObstacleInfoCmdTest : public testing::Test {
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
 * @tc.name  : RegisterMechObstacleInfoCmd_Constructor_001
 * @tc.number: RegisterMechObstacleInfoCmd_Constructor_001
 * @tc.desc  : Test constructor initialization.
 */
HWTEST_F(RegisterMechObstacleInfoCmdTest, RegisterMechObstacleInfoCmd_Constructor_001, TestSize.Level1)
{
    RegisterMechObstacleInfoCmd cmd;
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x41);
    EXPECT_EQ(cmd.GetReqSize(), 0);
    EXPECT_EQ(cmd.GetRspSize(), 0);
}

/**
 * @tc.name  : RegisterMechObstacleInfoCmd_Marshal_001
 * @tc.number: RegisterMechObstacleInfoCmd_Marshal_001
 * @tc.desc  : Test Marshal function with normal flow.
 */
HWTEST_F(RegisterMechObstacleInfoCmdTest, RegisterMechObstacleInfoCmd_Marshal_001, TestSize.Level1)
{
    RegisterMechObstacleInfoCmd cmd;
    auto buffer = cmd.Marshal();
    ASSERT_NE(buffer, nullptr);
    EXPECT_GE(buffer->Size(), 2);
}

/**
 * @tc.name  : RegisterMechObstacleInfoCmd_Unmarshal_001
 * @tc.number: RegisterMechObstacleInfoCmd_Unmarshal_001
 * @tc.desc  : Test Unmarshal with nullptr data.
 */
HWTEST_F(RegisterMechObstacleInfoCmdTest, RegisterMechObstacleInfoCmd_Unmarshal_001, TestSize.Level1)
{
    RegisterMechObstacleInfoCmd cmd;
    bool result = cmd.Unmarshal(nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.name  : RegisterMechObstacleInfoCmd_Unmarshal_002
 * @tc.number: RegisterMechObstacleInfoCmd_Unmarshal_002
 * @tc.desc  : Test Unmarshal with insufficient data size.
 */
HWTEST_F(RegisterMechObstacleInfoCmdTest, RegisterMechObstacleInfoCmd_Unmarshal_002, TestSize.Level1)
{
    RegisterMechObstacleInfoCmd cmd;
    auto buffer = std::make_shared<MechDataBuffer>(1);
    ASSERT_NE(buffer, nullptr);
    bool result = cmd.Unmarshal(buffer);
    EXPECT_FALSE(result);
}

/**
 * @tc.name  : RegisterMechObstacleInfoCmd_Unmarshal_003
 * @tc.number: RegisterMechObstacleInfoCmd_Unmarshal_003
 * @tc.desc  : Test Unmarshal with obstacleNums = 0.
 */
HWTEST_F(RegisterMechObstacleInfoCmdTest, RegisterMechObstacleInfoCmd_Unmarshal_003, TestSize.Level1)
{
    RegisterMechObstacleInfoCmd cmd;
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x41);
    buffer->AppendUint8(0);
    bool result = cmd.Unmarshal(buffer);
    EXPECT_TRUE(result);
    EXPECT_EQ(cmd.GetObstacleNums(), 0);
    EXPECT_EQ(cmd.GetObstacles().size(), 0);
}

/**
 * @tc.name  : RegisterMechObstacleInfoCmd_Unmarshal_004
 * @tc.number: RegisterMechObstacleInfoCmd_Unmarshal_004
 * @tc.desc  : Test Unmarshal with one obstacle info.
 */
HWTEST_F(RegisterMechObstacleInfoCmdTest, RegisterMechObstacleInfoCmd_Unmarshal_004, TestSize.Level1)
{
    RegisterMechObstacleInfoCmd cmd;
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x41);
    buffer->AppendUint8(1);
    buffer->AppendInt16(100);
    buffer->AppendInt16(45);
    buffer->AppendUint8(0);
    bool result = cmd.Unmarshal(buffer);
    EXPECT_TRUE(result);
    EXPECT_EQ(cmd.GetObstacleNums(), 1);
    EXPECT_EQ(cmd.GetObstacles().size(), 1);
    EXPECT_EQ(cmd.GetObstacles()[0].direction, 100);
    EXPECT_EQ(cmd.GetObstacles()[0].pitchAngle, 45);
    EXPECT_EQ(cmd.GetObstacles()[0].detailLenth, 0);
}

/**
 * @tc.name  : RegisterMechObstacleInfoCmd_Unmarshal_005
 * @tc.number: RegisterMechObstacleInfoCmd_Unmarshal_005
 * @tc.desc  : Test Unmarshal with invalid detailLenth.
 */
HWTEST_F(RegisterMechObstacleInfoCmdTest, RegisterMechObstacleInfoCmd_Unmarshal_005, TestSize.Level1)
{
    RegisterMechObstacleInfoCmd cmd;
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x41);
    buffer->AppendUint8(1);
    buffer->AppendInt16(100);
    buffer->AppendInt16(45);
    buffer->AppendUint8(1);
    bool result = cmd.Unmarshal(buffer);
    EXPECT_TRUE(result);
}

/**
 * @tc.name  : RegisterMechObstacleInfoCmd_Unmarshal_006
 * @tc.number: RegisterMechObstacleInfoCmd_Unmarshal_006
 * @tc.desc  : Test Unmarshal with multiple obstacle infos.
 */
HWTEST_F(RegisterMechObstacleInfoCmdTest, RegisterMechObstacleInfoCmd_Unmarshal_006, TestSize.Level1)
{
    RegisterMechObstacleInfoCmd cmd;
    auto buffer = std::make_shared<MechDataBuffer>(20);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x41);
    buffer->AppendUint8(3);
    buffer->AppendInt16(100);
    buffer->AppendInt16(45);
    buffer->AppendUint8(0);
    buffer->AppendInt16(200);
    buffer->AppendInt16(60);
    buffer->AppendUint8(0);
    buffer->AppendInt16(300);
    buffer->AppendInt16(30);
    buffer->AppendUint8(0);
    bool result = cmd.Unmarshal(buffer);
    EXPECT_TRUE(result);
    EXPECT_EQ(cmd.GetObstacleNums(), 3);
    EXPECT_EQ(cmd.GetObstacles().size(), 3);
    EXPECT_EQ(cmd.GetObstacles()[0].direction, 100);
    EXPECT_EQ(cmd.GetObstacles()[0].pitchAngle, 45);
    EXPECT_EQ(cmd.GetObstacles()[1].direction, 200);
    EXPECT_EQ(cmd.GetObstacles()[1].pitchAngle, 60);
    EXPECT_EQ(cmd.GetObstacles()[2].direction, 300);
    EXPECT_EQ(cmd.GetObstacles()[2].pitchAngle, 30);
}

/**
 * @tc.name  : RegisterMechObstacleInfoCmd_TriggerResponse_001
 * @tc.number: RegisterMechObstacleInfoCmd_TriggerResponse_001
 * @tc.desc  : Test TriggerResponse with nullptr data.
 */
HWTEST_F(RegisterMechObstacleInfoCmdTest, RegisterMechObstacleInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    RegisterMechObstacleInfoCmd cmd;
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x41);
    buffer->AppendUint8(1);
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), 0x03);
}

/**
 * @tc.name  : RegisterMechObstacleInfoCmd_TriggerResponse_002
 * @tc.number: RegisterMechObstacleInfoCmd_TriggerResponse_002
 * @tc.desc  : Test TriggerResponse with insufficient data size.
 */
HWTEST_F(RegisterMechObstacleInfoCmdTest, RegisterMechObstacleInfoCmd_TriggerResponse_002, TestSize.Level1)
{
    RegisterMechObstacleInfoCmd cmd;
    uint8_t initialResult = cmd.GetResult();
    auto buffer = std::make_shared<MechDataBuffer>(1);
    ASSERT_NE(buffer, nullptr);
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), initialResult);
}

/**
 * @tc.name  : RegisterMechObstacleInfoCmd_TriggerResponse_003
 * @tc.number: RegisterMechObstacleInfoCmd_TriggerResponse_003
 * @tc.desc  : Test TriggerResponse with normal data and no callback.
 */
HWTEST_F(RegisterMechObstacleInfoCmdTest, RegisterMechObstacleInfoCmd_TriggerResponse_003, TestSize.Level1)
{
    RegisterMechObstacleInfoCmd cmd;
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x41);
    buffer->AppendUint8(1);
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), 0x03);
    EXPECT_NE(cmd.GetResult(), 0);
}

/**
 * @tc.name  : RegisterMechObstacleInfoCmd_TriggerResponse_004
 * @tc.number: RegisterMechObstacleInfoCmd_TriggerResponse_004
 * @tc.desc  : Test TriggerResponse with normal data and callback.
 */
HWTEST_F(RegisterMechObstacleInfoCmdTest, RegisterMechObstacleInfoCmd_TriggerResponse_004, TestSize.Level1)
{
    RegisterMechObstacleInfoCmd cmd;
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x41);
    buffer->AppendUint8(1);
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });
    cmd.TriggerResponse(buffer);
    EXPECT_NE(cmd.GetResult(), 1);
    EXPECT_TRUE(callbackTriggered);
}

/**
 * @tc.name  : RegisterMechObstacleInfoCmd_GetObstacles_001
 * @tc.number: RegisterMechObstacleInfoCmd_GetObstacles_001
 * @tc.desc  : Test GetObstacles function.
 */
HWTEST_F(RegisterMechObstacleInfoCmdTest, RegisterMechObstacleInfoCmd_GetObstacles_001, TestSize.Level1)
{
    RegisterMechObstacleInfoCmd cmd;
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x41);
    buffer->AppendUint8(1);
    buffer->AppendInt16(100);
    buffer->AppendInt16(45);
    buffer->AppendUint8(0);
    cmd.Unmarshal(buffer);
    std::vector<ObstacleInfo> obstacles = cmd.GetObstacles();
    EXPECT_EQ(obstacles.size(), 1);
    EXPECT_EQ(obstacles[0].direction, 100);
    EXPECT_EQ(obstacles[0].pitchAngle, 45);
}

/**
 * @tc.name  : RegisterMechObstacleInfoCmd_GetObstacleNums_001
 * @tc.number: RegisterMechObstacleInfoCmd_GetObstacleNums_001
 * @tc.desc  : Test GetObstacleNums function.
 */
HWTEST_F(RegisterMechObstacleInfoCmdTest, RegisterMechObstacleInfoCmd_GetObstacleNums_001, TestSize.Level1)
{
    RegisterMechObstacleInfoCmd cmd;
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x41);
    buffer->AppendUint8(2);
    buffer->AppendInt16(100);
    buffer->AppendInt16(45);
    buffer->AppendUint8(0);
    buffer->AppendInt16(200);
    buffer->AppendInt16(60);
    buffer->AppendUint8(0);
    cmd.Unmarshal(buffer);
    EXPECT_EQ(cmd.GetObstacleNums(), 2);
}

/**
 * @tc.name  : RegisterMechObstacleInfoCmd_GetResult_001
 * @tc.number: RegisterMechObstacleInfoCmd_GetResult_001
 * @tc.desc  : Test GetResult function.
 */
HWTEST_F(RegisterMechObstacleInfoCmdTest, RegisterMechObstacleInfoCmd_GetResult_001, TestSize.Level1)
{
    RegisterMechObstacleInfoCmd cmd;
    auto buffer = std::make_shared<MechDataBuffer>(10);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x41);
    buffer->AppendUint8(0);
    cmd.TriggerResponse(buffer);
    EXPECT_NE(cmd.GetResult(), 0);
}

} // namespace MechBodyController
} // namespace OHOS