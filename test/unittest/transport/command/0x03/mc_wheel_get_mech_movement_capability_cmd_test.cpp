/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#include "mc_wheel_get_mech_movement_capability_cmd.h"
#include "mechbody_controller_log.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS {
namespace MechBodyController {
 
class WheelGetMechMovementCapabilityCmdTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
    }
 
    static void TearDownTestCase()
    {
    }
 
    void SetUp()
    {
        memset_s(g_capabilityInfo, MAX_CAPABILITY_BIT_NUM * sizeof(uint32_t), 0, sizeof(g_capabilityInfo));
    }
 
    void TearDown()
    {
    }
};
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_Constructor_001
 * @tc.number: WheelGetMechMovementCapabilityCmd_Constructor_001
 * @tc.desc  : Test constructor initialization.
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_Constructor_001, TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    EXPECT_EQ(cmd.GetCmdSet(), 0x03);
    EXPECT_EQ(cmd.GetCmdId(), 0x01);
    EXPECT_EQ(cmd.GetReqSize(), 2);
    EXPECT_EQ(cmd.GetRspSize(), 20);
    EXPECT_EQ(cmd.GetResult(), 0);
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_Marshal_001
 * @tc.number: WheelGetMechMovementCapabilityCmd_Marshal_001
 * @tc.desc  : Test Marshal function with normal flow.
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_Marshal_001, TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    auto buffer = cmd.Marshal();
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), 2);
 
    uint8_t cmdSet = 0;
    uint8_t cmdId = 0;
    EXPECT_EQ(buffer->ReadUint8(0, cmdSet), ERR_OK);
    EXPECT_EQ(buffer->ReadUint8(1, cmdId), ERR_OK);
    EXPECT_EQ(cmdSet, 0x03);
    EXPECT_EQ(cmdId, 0x01);
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_SetCapability_001
 * @tc.number: WheelGetMechMovementCapabilityCmd_SetCapability_001
 * @tc.desc  : Test SetCapability with cap value within first 32-bit segment.
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_SetCapability_001, TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    memset_s(g_capabilityInfo, MAX_CAPABILITY_BIT_NUM * sizeof(uint32_t), 0, sizeof(g_capabilityInfo));
    cmd.SetCapability(5);
    uint32_t index = 5 / 32;
    uint32_t bitIndex = 5 % 32;
    EXPECT_EQ(g_capabilityInfo[index], (1u << bitIndex));
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_SetCapability_002
 * @tc.number: WheelGetMechMovementCapabilityCmd_SetCapability_002
 * @tc.desc  : Test SetCapability with cap value spanning into second 32-bit segment.
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_SetCapability_002, TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    memset_s(g_capabilityInfo, MAX_CAPABILITY_BIT_NUM * sizeof(uint32_t), 0, sizeof(g_capabilityInfo));
    cmd.SetCapability(35);
    uint32_t index = 35 / 32;
    uint32_t bitIndex = 35 % 32;
    EXPECT_EQ(g_capabilityInfo[index], (1u << bitIndex));
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_SetCapabilityBitValue_001
 * @tc.number: WheelGetMechMovementCapabilityCmd_SetCapabilityBitValue_001
 * @tc.desc  : Test SetCapabilityBitValue with value having multiple bits set.
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_SetCapabilityBitValue_001,
    TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    memset_s(g_capabilityInfo, MAX_CAPABILITY_BIT_NUM * sizeof(uint32_t), 0, sizeof(g_capabilityInfo));
    uint8_t value = 0x85; // bits 0, 2, 7 set
    cmd.SetCapabilityBitValue(0, value, 0);
    uint32_t index0 = 0 / 32;
    uint32_t bitIndex0 = 0 % 32;
    uint32_t index2 = 2 / 32;
    uint32_t bitIndex2 = 2 % 32;
    uint32_t index7 = 7 / 32;
    uint32_t bitIndex7 = 7 % 32;
    EXPECT_TRUE(g_capabilityInfo[index0] & (1u << bitIndex0));
    EXPECT_TRUE(g_capabilityInfo[index2] & (1u << bitIndex2));
    EXPECT_TRUE(g_capabilityInfo[index7] & (1u << bitIndex7));
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_SetCapabilityBitValue_002
 * @tc.number: WheelGetMechMovementCapabilityCmd_SetCapabilityBitValue_002
 * @tc.desc  : Test SetCapabilityBitValue with value 0 (no bits set).
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_SetCapabilityBitValue_002,
    TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    memset_s(g_capabilityInfo, MAX_CAPABILITY_BIT_NUM * sizeof(uint32_t), 0, sizeof(g_capabilityInfo));
    cmd.SetCapabilityBitValue(0, 0, 0);
    EXPECT_EQ(g_capabilityInfo[0], 0u);
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_SetCapabilityBitValue_003
 * @tc.number: WheelGetMechMovementCapabilityCmd_SetCapabilityBitValue_003
 * @tc.desc  : Test SetCapabilityBitValue with non-zero start and index.
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_SetCapabilityBitValue_003,
    TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    memset_s(g_capabilityInfo, MAX_CAPABILITY_BIT_NUM * sizeof(uint32_t), 0, sizeof(g_capabilityInfo));
    uint8_t value = 0x01; // bit 0 set
    uint32_t start = 100;
    uint8_t index = 2;
    cmd.SetCapabilityBitValue(start, value, index);
    uint32_t expectedCap = start + index * 8 + 0;
    uint32_t capIndex = expectedCap / 32;
    uint32_t capBitIndex = expectedCap % 32;
    EXPECT_TRUE(g_capabilityInfo[capIndex] & (1u << capBitIndex));
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_OnDataReceive_001
 * @tc.number: WheelGetMechMovementCapabilityCmd_OnDataReceive_001
 * @tc.desc  : Test OnDataReceive with segNum > 32 (invalid).
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_OnDataReceive_001, TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    uint8_t data[] = {33}; // segNum = 33 > 32
    cmd.OnDataReceive(data);
    // Should return early without processing, g_capabilityInfo should remain 0
    EXPECT_EQ(g_capabilityInfo[0], 0u);
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_OnDataReceive_002
 * @tc.number: WheelGetMechMovementCapabilityCmd_OnDataReceive_002
 * @tc.desc  : Test OnDataReceive with valid segNum and len > 32 (invalid).
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_OnDataReceive_002, TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    // segNum=1, start=0x00000000, len=33 (> 32, invalid)
    uint8_t data[] = {1, 0x00, 0x00, 0x00, 0x00, 33};
    cmd.OnDataReceive(data);
    // Should return early due to len > 32
    EXPECT_EQ(g_capabilityInfo[0], 0u);
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_OnDataReceive_003
 * @tc.number: WheelGetMechMovementCapabilityCmd_OnDataReceive_003
 * @tc.desc  : Test OnDataReceive with segNum=0 (no segments to process).
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_OnDataReceive_003, TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    uint8_t data[] = {0}; // segNum = 0
    cmd.OnDataReceive(data);
    // Should set HEAD_UP(3), HEAD_DOWN(7), NOD(9), HEAD_SHAKE(10)
    EXPECT_TRUE(g_capabilityInfo[0] & (1u << 3));  // HEAD_UP
    EXPECT_TRUE(g_capabilityInfo[0] & (1u << 7));  // HEAD_DOWN
    EXPECT_TRUE(g_capabilityInfo[0] & (1u << 9));  // NOD
    EXPECT_TRUE(g_capabilityInfo[0] & (1u << 10)); // HEAD_SHAKE
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_OnDataReceive_004
 * @tc.number: WheelGetMechMovementCapabilityCmd_OnDataReceive_004
 * @tc.desc  : Test OnDataReceive with valid data containing one segment.
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_OnDataReceive_004, TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    // segNum=1, start=0x00000000, len=1, one byte=0xFF (all 8 bits set)
    uint8_t data[] = {1, 0x00, 0x00, 0x00, 0x00, 1, 0xFF};
    cmd.OnDataReceive(data);
    // Bits 0-7 should be set from segment, plus HEAD_UP(3), HEAD_DOWN(7), NOD(9), HEAD_SHAKE(10)
    // 0xFF at start=0, index=0 sets bits 0-7
    EXPECT_EQ(g_capabilityInfo[0], 0xFFu | (1u << 3) | (1u << 7) | (1u << 9) | (1u << 10));
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_OnDataReceive_005
 * @tc.number: WheelGetMechMovementCapabilityCmd_OnDataReceive_005
 * @tc.desc  : Test OnDataReceive with valid data containing multiple segments.
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_OnDataReceive_005, TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    uint8_t data[] = {
        2,                          // segNum = 2
        0x00, 0x00, 0x00, 0x00,    // start = 0
        1,                          // len = 1
        0x01,                       // data byte (bit 0 set)
        0x20, 0x00, 0x00, 0x00,    // start = 32
        1,                          // len = 1
        0x01                        // data byte (bit 0 set)
    };
    cmd.OnDataReceive(data);
    // Segment1: start=0, index=0, value=0x01 -> cap=0+0*8+0=0
    EXPECT_TRUE(g_capabilityInfo[0] & (1u << 0));
    // Segment2: start=32, index=0, value=0x01 -> cap=32+0*8+0=32
    EXPECT_TRUE(g_capabilityInfo[1] & (1u << 0));
    // Plus HEAD_UP(3), HEAD_DOWN(7), NOD(9), HEAD_SHAKE(10)
    EXPECT_TRUE(g_capabilityInfo[0] & (1u << 3));
    EXPECT_TRUE(g_capabilityInfo[0] & (1u << 7));
    EXPECT_TRUE(g_capabilityInfo[0] & (1u << 9));
    EXPECT_TRUE(g_capabilityInfo[0] & (1u << 10));
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_TriggerResponse_001
 * @tc.number: WheelGetMechMovementCapabilityCmd_TriggerResponse_001
 * @tc.desc  : Test TriggerResponse with nullptr data.
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_TriggerResponse_001, TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    uint8_t initialResult = cmd.GetResult();
    cmd.TriggerResponse(nullptr);
    EXPECT_EQ(cmd.GetResult(), initialResult);
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_TriggerResponse_002
 * @tc.number: WheelGetMechMovementCapabilityCmd_TriggerResponse_002
 * @tc.desc  : Test TriggerResponse with insufficient data size.
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_TriggerResponse_002, TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    uint8_t initialResult = cmd.GetResult();
    auto buffer = std::make_shared<MechDataBuffer>(1);
    ASSERT_NE(buffer, nullptr);
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), initialResult);
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_TriggerResponse_003
 * @tc.number: WheelGetMechMovementCapabilityCmd_TriggerResponse_003
 * @tc.desc  : Test TriggerResponse with valid data and no callback.
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_TriggerResponse_003, TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    // RSP_SIZE=20, BIT_OFFSET_2=2, so need at least 22 bytes
    auto buffer = std::make_shared<MechDataBuffer>(30);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(1);    // result = 1
    // OnDataReceive data: segNum=0 (only HEAD_UP/DOWN/NOD/SHAKE set)
    buffer->AppendUint8(0);    // segNum = 0
    // Fill remaining bytes to reach sufficient size
    for (int i = 0; i < 18; i++) {
        buffer->AppendUint8(0);
    }
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), 1);
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_TriggerResponse_004
 * @tc.number: WheelGetMechMovementCapabilityCmd_TriggerResponse_004
 * @tc.desc  : Test TriggerResponse with valid data and callback set.
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_TriggerResponse_004, TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    auto buffer = std::make_shared<MechDataBuffer>(30);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0);    // result = 0
    buffer->AppendUint8(0);    // segNum = 0
    for (int i = 0; i < 18; i++) {
        buffer->AppendUint8(0);
    }
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), 0);
    EXPECT_TRUE(callbackTriggered);
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_TriggerResponse_005
 * @tc.number: WheelGetMechMovementCapabilityCmd_TriggerResponse_005
 * @tc.desc  : Test TriggerResponse with valid data and callback not set.
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_TriggerResponse_005, TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    auto buffer = std::make_shared<MechDataBuffer>(30);
    ASSERT_NE(buffer, nullptr);
    buffer->AppendUint8(0x03); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(2);    // result = 2
    buffer->AppendUint8(0);    // segNum = 0
    for (int i = 0; i < 18; i++) {
        buffer->AppendUint8(0);
    }
    // No callback set (responseCb_ is empty)
    cmd.TriggerResponse(buffer);
    EXPECT_EQ(cmd.GetResult(), 2);
}
 
/**
 * @tc.name  : WheelGetMechMovementCapabilityCmd_GetResult_001
 * @tc.number: WheelGetMechMovementCapabilityCmd_GetResult_001
 * @tc.desc  : Test GetResult returns initial value 0.
 */
HWTEST_F(WheelGetMechMovementCapabilityCmdTest, WheelGetMechMovementCapabilityCmd_GetResult_001, TestSize.Level1)
{
    WheelGetMechMovementCapabilityCmd cmd;
    EXPECT_EQ(cmd.GetResult(), 0);
}
 
} // namespace MechBodyController
} // namespace OHOS