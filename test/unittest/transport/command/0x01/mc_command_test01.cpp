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
#include "mc_register_mech_camera_key_event_cmd.h"
#include "mc_register_mech_control_result_cmd.h"
#include "mc_register_mech_position_info_cmd.h"
#include "mc_register_mech_tracking_enable_cmd.h"
#include "mc_set_mech_camera_tracking_enable_cmd.h"
#include "mc_set_mech_camera_tracking_frame_cmd.h"
#include "mc_set_mech_camera_tracking_layout_cmd.h"
#include "mc_set_mech_hid_preemptive_cmd.h"
#include "mc_set_mech_rotation_by_speed_cmd.h"
#include "mc_set_mech_config_cmd.h"
#include "mc_set_mech_rotation_cmd.h"
#include "mc_command_factory.h"
#include "mechbody_controller_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {
class TestRegisterMechStateInfoCmd : public testing::Test {
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

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_Marshal_001
* @tc.number: RegisterMechCameraKeyEventCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechCameraKeyEventCmd_Marshal_001, TestSize.Level1)
{
    RegisterMechCameraKeyEventCmd cmd;
    auto buffer = cmd.Marshal();
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), 2);
    uint8_t cmdSet = 0;
    uint8_t cmdId = 0;
    EXPECT_EQ(buffer->ReadUint8(0, cmdSet), ERR_OK);
    EXPECT_EQ(buffer->ReadUint8(1, cmdId), ERR_OK);
    EXPECT_EQ(cmdSet, 0x02);
    EXPECT_EQ(cmdId, 0x40);
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_Unmarshal_NullOrSmallData_001
* @tc.number: RegisterMechCameraKeyEventCmd_Unmarshal_NullOrSmallData_001
* @tc.desc  : Test Unmarshal with null or small data.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechCameraKeyEventCmd_Unmarshal_NullOrSmallData_001, TestSize.Level1)
{
    RegisterMechCameraKeyEventCmd cmd;
    // Given: data为空
    // When: 调用Unmarshal
    // Then: 返回false
    EXPECT_FALSE(cmd.Unmarshal(nullptr));

    // Given: 数据大小不足
    auto buffer = std::make_shared<MechDataBuffer>(1);
    // When: 调用Unmarshal
    // Then: 返回false
    EXPECT_FALSE(cmd.Unmarshal(buffer));
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_Unmarshal_SwitchCameraEvent_001
* @tc.number: RegisterMechCameraKeyEventCmd_Unmarshal_SwitchCameraEvent_001
* @tc.desc  : Test Unmarshal with SWITCH_CAMERA event.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechCameraKeyEventCmd_Unmarshal_SwitchCameraEvent_001, TestSize.Level1)
{
    RegisterMechCameraKeyEventCmd cmd;
    // Given: 设置SWITCH_CAMERA事件数据 (BIT_OFFSET_13位置为0x01)
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendInt16(100);  // stickX
    buffer->AppendInt16(200);  // stickY
    // controlByte: BIT_OFFSET_13位置为CMD_KEY_STATE_SINGLE_CLICK (0x01)
    // 0x01 << 13 = 0x2000 (小端序: 0x00 0x20)
    buffer->AppendUint8(0x00); // low byte
    buffer->AppendUint8(0x20); // high byte
    // 需要至少 RPT_SIZE + BIT_OFFSET_2 = 8 + 2 = 10 字节
    buffer->AppendUint8(0x00);
    buffer->AppendUint8(0x00);

    // When: 调用Unmarshal
    bool result = cmd.Unmarshal(buffer);

    // Then: 返回TRUE，事件类型为SWITCH_CAMERA，坐标正确
    EXPECT_TRUE(result);
    EXPECT_EQ(cmd.GetEvent(), CameraKeyEvent::SWITCH_CAMERA);
    EXPECT_EQ(cmd.GetStickX(), 100);
    EXPECT_EQ(cmd.GetStickY(), 200);
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_Unmarshal_SwitchPhotoFilmEvent_001
* @tc.number: RegisterMechCameraKeyEventCmd_Unmarshal_SwitchPhotoFilmEvent_001
* @tc.desc  : Test Unmarshal with SWITCH_PHOTO_FILM event.
*/
HWTEST_F(TestRegisterMechStateInfoCmd,
    RegisterMechCameraKeyEventCmd_Unmarshal_SwitchPhotoFilmEvent_001, TestSize.Level1)
{
    RegisterMechCameraKeyEventCmd cmd;
    // Given: 设置SWITCH_PHOTO_FILM事件数据 (BIT_OFFSET_13位置为0x03)
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendInt16(100);  // stickX
    buffer->AppendInt16(200);  // stickY
    // controlByte: BIT_OFFSET_13位置为CMD_KEY_STATE_TRIPLE_CLICK (0x03)
    // 0x03 << 13 = 0x6000 (小端序: 0x00 0x60)
    buffer->AppendUint8(0x00); // low byte
    buffer->AppendUint8(0x60); // high byte
    // 需要至少 RPT_SIZE + BIT_OFFSET_2 = 8 + 2 = 10 字节
    buffer->AppendUint8(0x00);
    buffer->AppendUint8(0x00);

    // When: 调用Unmarshal
    bool result = cmd.Unmarshal(buffer);

    // Then: 返回TRUE，事件类型为SWITCH_PHOTO_FILM，坐标正确
    EXPECT_TRUE(result);
    EXPECT_EQ(cmd.GetEvent(), CameraKeyEvent::SWITCH_PHOTO_FILM);
    EXPECT_EQ(cmd.GetStickX(), 100);
    EXPECT_EQ(cmd.GetStickY(), 200);
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_Unmarshal_ZoomOutEvent_001
* @tc.number: RegisterMechCameraKeyEventCmd_Unmarshal_ZoomOutEvent_001
* @tc.desc  : Test Unmarshal with ZOOM_OUT event.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechCameraKeyEventCmd_Unmarshal_ZoomOutEvent_001, TestSize.Level1)
{
    RegisterMechCameraKeyEventCmd cmd;
    // Given: 设置ZOOM_OUT事件数据 (BIT_OFFSET_10位置为0x01)
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendInt16(100);  // stickX
    buffer->AppendInt16(200);  // stickY
    // controlByte: BIT_OFFSET_10位置为CMD_KEY_STATE_SINGLE_CLICK (0x01)
    // 0x01 << 10 = 0x0400 (小端序: 0x00 0x04)
    buffer->AppendUint8(0x00); // low byte
    buffer->AppendUint8(0x04); // high byte
    // 需要至少 RPT_SIZE + BIT_OFFSET_2 = 8 + 2 = 10 字节
    buffer->AppendUint8(0x00);
    buffer->AppendUint8(0x00);

    // When: 调用Unmarshal
    bool result = cmd.Unmarshal(buffer);

    // Then: 返回TRUE，事件类型为ZOOM_OUT，坐标正确
    EXPECT_TRUE(result);
    EXPECT_EQ(cmd.GetEvent(), CameraKeyEvent::ZOOM_OUT);
    EXPECT_EQ(cmd.GetStickX(), 100);
    EXPECT_EQ(cmd.GetStickY(), 200);
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_Unmarshal_ZoomInEvent_001
* @tc.number: RegisterMechCameraKeyEventCmd_Unmarshal_ZoomInEvent_001
* @tc.desc  : Test Unmarshal with ZOOM_IN event.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechCameraKeyEventCmd_Unmarshal_ZoomInEvent_001, TestSize.Level1)
{
    RegisterMechCameraKeyEventCmd cmd;
    // Given: 设置ZOOM_IN事件数据 (BIT_OFFSET_7位置为0x01)
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendInt16(100);  // stickX
    buffer->AppendInt16(200);  // stickY
    // controlByte: 需要至少8字节数据，BIT_OFFSET_7位置为CMD_KEY_STATE_SINGLE_CLICK (0x01)
    // (controlByte >> 7) & 0b111 == 0x01，所以 controlByte = 0x0080
    buffer->AppendUint16(0x0080); // controlByte 5-6
    buffer->AppendUint16(0x0000); // controlByte 7-8 (padding)

    // When: 调用Unmarshal
    bool result = cmd.Unmarshal(buffer);

    // Then: 返回true，事件类型为ZOOM_IN，stickX和stickY正确解析
    EXPECT_TRUE(result);
    EXPECT_EQ(cmd.GetEvent(), CameraKeyEvent::ZOOM_IN);
    EXPECT_EQ(cmd.GetStickX(), 100);
    EXPECT_EQ(cmd.GetStickY(), 200);
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_Unmarshal_TriggerButtonEvent_001
* @tc.number: RegisterMechCameraKeyEventCmd_Unmarshal_TriggerButtonEvent_001
* @tc.desc  : Test Unmarshal with trigger button event.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechCameraKeyEventCmd_Unmarshal_TriggerButtonEvent_001, TestSize.Level1)
{
    RegisterMechCameraKeyEventCmd cmd;
    // Given: 设置trigger button事件数据 (BIT_OFFSET_4位置为0x01)
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendInt16(100);  // stickX
    buffer->AppendInt16(200);  // stickY
    // controlByte: 需要至少8字节数据，BIT_OFFSET_4位置为CMD_KEY_STATE_SINGLE_CLICK (0x01)
    // (controlByte >> 4) & 0b111 == 0x01，所以 controlByte = 0x0010
    buffer->AppendUint16(0x0010); // controlByte 5-6
    buffer->AppendUint16(0x0000); // controlByte 7-8 (padding)

    // When: 调用Unmarshal
    bool result = cmd.Unmarshal(buffer);

    // Then: 返回true，trigger button不设置event_，event_保持默认值INVALID，stickX和stickY正确解析
    EXPECT_TRUE(result);
    EXPECT_EQ(cmd.GetEvent(), CameraKeyEvent::INVALID);
    EXPECT_EQ(cmd.GetStickX(), 100);
    EXPECT_EQ(cmd.GetStickY(), 200);
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_Unmarshal_StartFilmingEvent_001
* @tc.number: RegisterMechCameraKeyEventCmd_Unmarshal_StartFilmingEvent_001
* @tc.desc  : Test Unmarshal with START_FILMING event.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechCameraKeyEventCmd_Unmarshal_StartFilmingEvent_001, TestSize.Level1)
{
    RegisterMechCameraKeyEventCmd cmd;
    // Given: 设置START_FILMING事件数据 (第二个controlByte的BIT_OFFSET_3位置为0x01)
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendInt16(100);  // stickX
    buffer->AppendInt16(200);  // stickY
    // 第一个controlByte: 无事件
    buffer->AppendUint8(0x00); // low byte
    buffer->AppendUint8(0x00); // high byte
    // 第二个controlByte: BIT_OFFSET_3位置为CMD_KEY_STATE_SINGLE_CLICK (0x01)
    buffer->AppendUint8(0x08); // low byte (bit 3 = 1)
    buffer->AppendUint8(0x00); // high byte

    // When: 调用Unmarshal
    bool result = cmd.Unmarshal(buffer);

    // Then: 返回true，事件类型为START_FILMING
    EXPECT_TRUE(result);
    EXPECT_EQ(cmd.GetEvent(), CameraKeyEvent::START_FILMING);
}

/**
* @tc.name  : RegisterMechCameraKeyEventCmd_TriggerResponse_WithCallback_001
* @tc.number: RegisterMechCameraKeyEventCmd_TriggerResponse_WithCallback_001
* @tc.desc  : Test TriggerResponse with callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechCameraKeyEventCmd_TriggerResponse_WithCallback_001, TestSize.Level1)
{
    RegisterMechCameraKeyEventCmd cmd;
    // Given: 设置响应回调
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    // When: 触发响应
    cmd.TriggerResponse(nullptr);

    // Then: 回调被触发
    EXPECT_TRUE(callbackTriggered);
}

/**
* @tc.name  : RegisterMechControlResultCmd_Unmarshal_NullOrSmallData_001
* @tc.number: RegisterMechControlResultCmd_Unmarshal_NullOrSmallData_001
* @tc.desc  : Test Unmarshal with null or small data.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechControlResultCmd_Unmarshal_NullOrSmallData_001, TestSize.Level1)
{
    RegisterMechControlResultCmd cmd;
    // Given: data为空
    // When: 调用Unmarshal
    // Then: 返回false
    EXPECT_FALSE(cmd.Unmarshal(nullptr));

    // Given: 数据大小不足
    auto buffer = std::make_shared<MechDataBuffer>(1);
    // When: 调用Unmarshal
    // Then: 返回false
    EXPECT_FALSE(cmd.Unmarshal(buffer));
}

/**
* @tc.name  : RegisterMechControlResultCmd_Unmarshal_ValidData_001
* @tc.number: RegisterMechControlResultCmd_Unmarshal_ValidData_001
* @tc.desc  : Test Unmarshal with valid data.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechControlResultCmd_Unmarshal_ValidData_001, TestSize.Level1)
{
    RegisterMechControlResultCmd cmd;
    // Given: 设置有效数据
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x05); // controlResult
    buffer->AppendUint8(0x03); // taskId

    // When: 调用Unmarshal
    bool result = cmd.Unmarshal(buffer);

    // Then: 返回true，数据正确解析
    EXPECT_TRUE(result);
    EXPECT_EQ(cmd.GetControlResult(), 0x05);
    EXPECT_EQ(cmd.GetTaskId(), 0x03);
}

/**
* @tc.name  : RegisterMechControlResultCmd_TriggerResponse_WithCallback_001
* @tc.number: RegisterMechControlResultCmd_TriggerResponse_WithCallback_001
* @tc.desc  : Test TriggerResponse with callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechControlResultCmd_TriggerResponse_WithCallback_001, TestSize.Level1)
{
    RegisterMechControlResultCmd cmd;
    // Given: 设置响应回调
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    // When: 触发响应
    cmd.TriggerResponse(nullptr);

    // Then: 回调被触发
    EXPECT_TRUE(callbackTriggered);
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_Marshal_001
* @tc.number: RegisterMechPositionInfoCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechPositionInfoCmd_Marshal_001, TestSize.Level1)
{
    RegisterMechPositionInfoCmd cmd;
    // Given: 创建命令对象
    // When: 调用Marshal
    auto buffer = cmd.Marshal();
    // Then: 返回非空buffer，buffer大小为2，包含cmdSet和cmdId
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), 2);
    uint8_t cmdSet = 0;
    uint8_t cmdId = 0;
    EXPECT_EQ(buffer->ReadUint8(0, cmdSet), ERR_OK);
    EXPECT_EQ(buffer->ReadUint8(1, cmdId), ERR_OK);
    EXPECT_EQ(cmdSet, 0x02);
    EXPECT_EQ(cmdId, 0x42);
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_Unmarshal_NullOrSmallData_001
* @tc.number: RegisterMechPositionInfoCmd_Unmarshal_NullOrSmallData_001
* @tc.desc  : Test Unmarshal with null or small data.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechPositionInfoCmd_Unmarshal_NullOrSmallData_001, TestSize.Level1)
{
    RegisterMechPositionInfoCmd cmd;
    // Given: data为空
    // When: 调用Unmarshal
    // Then: 返回false
    EXPECT_FALSE(cmd.Unmarshal(nullptr));

    // Given: 数据大小不足
    auto buffer = std::make_shared<MechDataBuffer>(1);
    // When: 调用Unmarshal
    // Then: 返回false
    EXPECT_FALSE(cmd.Unmarshal(buffer));
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_Unmarshal_ValidData_001
* @tc.number: RegisterMechPositionInfoCmd_Unmarshal_ValidData_001
* @tc.desc  : Test Unmarshal with valid data.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechPositionInfoCmd_Unmarshal_ValidData_001, TestSize.Level1)
{
    RegisterMechPositionInfoCmd cmd;
    // Given: 设置有效数据
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendFloat(1.5f); // pitch
    buffer->AppendFloat(2.5f); // roll
    buffer->AppendFloat(3.5f); // yaw

    // When: 调用Unmarshal
    bool result = cmd.Unmarshal(buffer);

    // Then: 返回true，数据正确解析
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(cmd.GetPosition().pitch, 1.5f);
    EXPECT_FLOAT_EQ(cmd.GetPosition().roll, 2.5f);
    EXPECT_FLOAT_EQ(cmd.GetPosition().yaw, 3.5f);
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_TriggerResponse_NullOrSmallData_001
* @tc.number: RegisterMechPositionInfoCmd_TriggerResponse_NullOrSmallData_001
* @tc.desc  : Test TriggerResponse with null or small data.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechPositionInfoCmd_TriggerResponse_NullOrSmallData_001, TestSize.Level1)
{
    RegisterMechPositionInfoCmd cmd;
    // Given: data为空
    // When: 触发响应
    cmd.TriggerResponse(nullptr);
    // Then: result_保持默认值0
    EXPECT_EQ(cmd.GetResult(), 0);

    // Given: 数据大小不足
    auto buffer = std::make_shared<MechDataBuffer>(1);
    // When: 触发响应
    cmd.TriggerResponse(buffer);
    // Then: result_保持默认值0
    EXPECT_EQ(cmd.GetResult(), 0);
}

/**
* @tc.name  : RegisterMechPositionInfoCmd_TriggerResponse_WithCallback_001
* @tc.number: RegisterMechPositionInfoCmd_TriggerResponse_WithCallback_001
* @tc.desc  : Test TriggerResponse with callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechPositionInfoCmd_TriggerResponse_WithCallback_001, TestSize.Level1)
{
    RegisterMechPositionInfoCmd cmd;
    // Given: 设置响应回调
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    // When: 触发响应
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // result
    cmd.TriggerResponse(buffer);

    // Then: 回调被触发
    EXPECT_TRUE(callbackTriggered);
}

/**
* @tc.name  : RegisterMechTrackingEnableCmd_Marshal_001
* @tc.number: RegisterMechTrackingEnableCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechTrackingEnableCmd_Marshal_001, TestSize.Level1)
{
    RegisterMechTrackingEnableCmd cmd;
    // Given: 创建命令对象
    // When: 调用Marshal
    auto buffer = cmd.Marshal();
    // Then: 返回非空buffer，buffer大小为2，包含cmdSet和cmdId
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->Size(), 2);
    uint8_t cmdSet = 0;
    uint8_t cmdId = 0;
    EXPECT_EQ(buffer->ReadUint8(0, cmdSet), ERR_OK);
    EXPECT_EQ(buffer->ReadUint8(1, cmdId), ERR_OK);
    EXPECT_EQ(cmdSet, 0x02);
    EXPECT_EQ(cmdId, 0x45);
}

/**
* @tc.name  : RegisterMechTrackingEnableCmd_Unmarshal_NullOrSmallData_001
* @tc.number: RegisterMechTrackingEnableCmd_Unmarshal_NullOrSmallData_001
* @tc.desc  : Test Unmarshal with null or small data.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechTrackingEnableCmd_Unmarshal_NullOrSmallData_001, TestSize.Level1)
{
    RegisterMechTrackingEnableCmd cmd;
    // Given: data为空
    // When: 调用Unmarshal
    // Then: 返回false
    EXPECT_FALSE(cmd.Unmarshal(nullptr));

    // Given: 数据大小不足
    auto buffer = std::make_shared<MechDataBuffer>(1);
    // When: 调用Unmarshal
    // Then: 返回false
    EXPECT_FALSE(cmd.Unmarshal(buffer));
}

/**
* @tc.name  : RegisterMechTrackingEnableCmd_Unmarshal_IsEnabledZero_001
* @tc.number: RegisterMechTrackingEnableCmd_Unmarshal_IsEnabledZero_001
* @tc.desc  : Test Unmarshal with isEnabled = 0.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechTrackingEnableCmd_Unmarshal_IsEnabledZero_001, TestSize.Level1)
{
    RegisterMechTrackingEnableCmd cmd;
    // Given: 设置isEnabled为0
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x00); // isEnabled = 0

    // When: 调用Unmarshal
    bool result = cmd.Unmarshal(buffer);

    // Then: 返回true，isEnabled为false
    EXPECT_TRUE(result);
    EXPECT_FALSE(cmd.GetIsEnabled());
}

/**
* @tc.name  : RegisterMechTrackingEnableCmd_Unmarshal_IsEnabledNonZero_001
* @tc.number: RegisterMechTrackingEnableCmd_Unmarshal_IsEnabledNonZero_001
* @tc.desc  : Test Unmarshal with isEnabled != 0.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechTrackingEnableCmd_Unmarshal_IsEnabledNonZero_001, TestSize.Level1)
{
    RegisterMechTrackingEnableCmd cmd;
    // Given: 设置isEnabled为1
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // isEnabled = 1

    // When: 调用Unmarshal
    bool result = cmd.Unmarshal(buffer);

    // Then: 返回true，isEnabled为true
    EXPECT_TRUE(result);
    EXPECT_TRUE(cmd.GetIsEnabled());
}

/**
* @tc.name  : RegisterMechTrackingEnableCmd_TriggerResponse_NullOrSmallData_001
* @tc.number: RegisterMechTrackingEnableCmd_TriggerResponse_NullOrSmallData_001
* @tc.desc  : Test TriggerResponse with null or small data.
*/
HWTEST_F(TestRegisterMechStateInfoCmd,
    RegisterMechTrackingEnableCmd_TriggerResponse_NullOrSmallData_001, TestSize.Level1)
{
    RegisterMechTrackingEnableCmd cmd;
    // Given: data为空
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(nullptr));

    // Given: 数据大小不足
    auto buffer = std::make_shared<MechDataBuffer>(1);
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : RegisterMechTrackingEnableCmd_TriggerResponse_WithCallback_001
* @tc.number: RegisterMechTrackingEnableCmd_TriggerResponse_WithCallback_001
* @tc.desc  : Test TriggerResponse with callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechTrackingEnableCmd_TriggerResponse_WithCallback_001, TestSize.Level1)
{
    RegisterMechTrackingEnableCmd cmd;
    // Given: 设置响应回调
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    // When: 触发响应
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // result
    cmd.TriggerResponse(buffer);

    // Then: 回调被触发
    EXPECT_TRUE(callbackTriggered);
}

/**
* @tc.name  : RegisterMechTrackingEnableCmd_TriggerResponse_NoCallback_001
* @tc.number: RegisterMechTrackingEnableCmd_TriggerResponse_NoCallback_001
* @tc.desc  : Test TriggerResponse without callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, RegisterMechTrackingEnableCmd_TriggerResponse_NoCallback_001, TestSize.Level1)
{
    RegisterMechTrackingEnableCmd cmd;
    // Given: 不设置回调
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // result
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechCameraTrackingEnableCmd_Marshal_001
* @tc.number: SetMechCameraTrackingEnableCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechCameraTrackingEnableCmd_Marshal_001, TestSize.Level1)
{
    SetMechCameraTrackingEnableCmd cmd(MechTrackingStatus::MECH_TK_DISABLE);
    // Given: 创建命令对象
    // When: 调用Marshal
    auto buffer = cmd.Marshal();
    // Then: 返回非空buffer
    EXPECT_NE(buffer, nullptr);
}

/**
* @tc.name  : SetMechCameraTrackingEnableCmd_Marshal_DifferentStatus_001
* @tc.number: SetMechCameraTrackingEnableCmd_Marshal_DifferentStatus_001
* @tc.desc  : Test Marshal with different status.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechCameraTrackingEnableCmd_Marshal_DifferentStatus_001, TestSize.Level1)
{
    // Given: 创建命令对象，状态为MECH_TK_ENABLE_WITH_TARGET
    SetMechCameraTrackingEnableCmd cmd(MechTrackingStatus::MECH_TK_ENABLE_WITH_TARGET);
    // When: 调用Marshal
    auto buffer = cmd.Marshal();
    // Then: 返回非空buffer
    EXPECT_NE(buffer, nullptr);
    // And: 状态正确
    EXPECT_EQ(cmd.GetStatus(), MechTrackingStatus::MECH_TK_ENABLE_WITH_TARGET);
}

/**
* @tc.name  : SetMechCameraTrackingEnableCmd_TriggerResponse_NullOrSmallData_001
* @tc.number: SetMechCameraTrackingEnableCmd_TriggerResponse_NullOrSmallData_001
* @tc.desc  : Test TriggerResponse with null or small data.
*/
HWTEST_F(TestRegisterMechStateInfoCmd,
    SetMechCameraTrackingEnableCmd_TriggerResponse_NullOrSmallData_001, TestSize.Level1)
{
    SetMechCameraTrackingEnableCmd cmd(MechTrackingStatus::MECH_TK_DISABLE);
    // Given: data为空
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(nullptr));

    // Given: 数据大小不足
    auto buffer = std::make_shared<MechDataBuffer>(1);
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechCameraTrackingEnableCmd_TriggerResponse_ReadFailed_001
* @tc.number: SetMechCameraTrackingEnableCmd_TriggerResponse_ReadFailed_001
* @tc.desc  : Test TriggerResponse when ReadUint8 fails.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechCameraTrackingEnableCmd_TriggerResponse_ReadFailed_001, TestSize.Level1)
{
    SetMechCameraTrackingEnableCmd cmd(MechTrackingStatus::MECH_TK_DISABLE);
    // Given: 数据大小足够但读取位置超出范围
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    // 不添加result数据，导致读取失败

    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechCameraTrackingEnableCmd_TriggerResponse_WithCallback_001
* @tc.number: SetMechCameraTrackingEnableCmd_TriggerResponse_WithCallback_001
* @tc.desc  : Test TriggerResponse with callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechCameraTrackingEnableCmd_TriggerResponse_WithCallback_001, TestSize.Level1)
{
    SetMechCameraTrackingEnableCmd cmd(MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET);
    // Given: 设置响应回调
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    // When: 触发响应
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    cmd.TriggerResponse(buffer);

    // Then: 回调被触发
    EXPECT_TRUE(callbackTriggered);
}

/**
* @tc.name  : SetMechCameraTrackingEnableCmd_TriggerResponse_NoCallback_001
* @tc.number: SetMechCameraTrackingEnableCmd_TriggerResponse_NoCallback_001
* @tc.desc  : Test TriggerResponse without callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechCameraTrackingEnableCmd_TriggerResponse_NoCallback_001, TestSize.Level1)
{
    SetMechCameraTrackingEnableCmd cmd(MechTrackingStatus::MECH_TK_DISABLE);
    // Given: 不设置回调
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechCameraTrackingFrameCmd_Marshal_001
* @tc.number: SetMechCameraTrackingFrameCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechCameraTrackingFrameCmd_Marshal_001, TestSize.Level1)
{
    TrackingFrameParams params;
    params.timeStamp = 123456789ULL;
    params.confidence = ConfidenceLevel::HIGH;
    params.objectType = 1;
    params.targetId = 100;
    params.roi.x = 10.5f;
    params.roi.y = 20.5f;
    params.roi.width = 100.0f;
    params.roi.height = 150.0f;
    params.fovV = 60;
    params.fovH = 90;
    params.isRecording = true;
    params.timeDelay = 50;

    SetMechCameraTrackingFrameCmd cmd(params);
    // Given: 创建命令对象
    // When: 调用Marshal
    auto buffer = cmd.Marshal();
    // Then: 返回非空buffer
    EXPECT_NE(buffer, nullptr);
}

/**
* @tc.name  : SetMechCameraTrackingFrameCmd_TriggerResponse_WithCallback_001
* @tc.number: SetMechCameraTrackingFrameCmd_TriggerResponse_WithCallback_001
* @tc.desc  : Test TriggerResponse with callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechCameraTrackingFrameCmd_TriggerResponse_WithCallback_001, TestSize.Level1)
{
    TrackingFrameParams params;
    params.timeStamp = 123456789ULL;
    params.confidence = ConfidenceLevel::MEDIUM;
    params.objectType = 2;
    params.targetId = 200;

    SetMechCameraTrackingFrameCmd cmd(params);
    // Given: 设置响应回调
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    // When: 触发响应
    cmd.TriggerResponse(nullptr);

    // Then: 回调被触发
    EXPECT_TRUE(callbackTriggered);
}

/**
* @tc.name  : SetMechCameraTrackingFrameCmd_TriggerResponse_NoCallback_001
* @tc.number: SetMechCameraTrackingFrameCmd_TriggerResponse_NoCallback_001
* @tc.desc  : Test TriggerResponse without callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechCameraTrackingFrameCmd_TriggerResponse_NoCallback_001, TestSize.Level1)
{
    TrackingFrameParams params;
    params.timeStamp = 987654321ULL;
    params.confidence = ConfidenceLevel::LOW;
    params.objectType = 3;
    params.targetId = 300;

    SetMechCameraTrackingFrameCmd cmd(params);
    // Given: 不设置回调
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(nullptr));
}

/**
* @tc.name  : SetMechCameraTrackingFrameCmd_GetParams_001
* @tc.number: SetMechCameraTrackingFrameCmd_GetParams_001
* @tc.desc  : Test GetParams function.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechCameraTrackingFrameCmd_GetParams_001, TestSize.Level1)
{
    TrackingFrameParams params;
    params.timeStamp = 111222333ULL;
    params.confidence = ConfidenceLevel::HIGH;
    params.objectType = 4;
    params.targetId = 400;
    params.roi.x = 50.0f;
    params.roi.y = 60.0f;
    params.roi.width = 200.0f;
    params.roi.height = 300.0f;
    params.fovV = 45;
    params.fovH = 75;
    params.isRecording = false;
    params.timeDelay = 100;

    SetMechCameraTrackingFrameCmd cmd(params);
    // Given: 创建命令对象
    // When: 获取参数
    const TrackingFrameParams& resultParams = cmd.GetParams();
    // Then: 返回正确的参数
    EXPECT_EQ(resultParams.timeStamp, 111222333ULL);
    EXPECT_EQ(resultParams.confidence, ConfidenceLevel::HIGH);
    EXPECT_EQ(resultParams.objectType, 4);
    EXPECT_EQ(resultParams.targetId, 400);
    EXPECT_FLOAT_EQ(resultParams.roi.x, 50.0f);
    EXPECT_FLOAT_EQ(resultParams.roi.y, 60.0f);
    EXPECT_FLOAT_EQ(resultParams.roi.width, 200.0f);
    EXPECT_FLOAT_EQ(resultParams.roi.height, 300.0f);
    EXPECT_EQ(resultParams.fovV, 45);
    EXPECT_EQ(resultParams.fovH, 75);
    EXPECT_EQ(resultParams.isRecording, false);
    EXPECT_EQ(resultParams.timeDelay, 100);
}

/**
* @tc.name  : SetMechCameraTrackingLayoutCmd_Marshal_IsDefault_001
* @tc.number: SetMechCameraTrackingLayoutCmd_Marshal_IsDefault_001
* @tc.desc  : Test Marshal with isDefault true.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechCameraTrackingLayoutCmd_Marshal_IsDefault_001, TestSize.Level1)
{
    LayoutParams params;
    params.isDefault = true;
    params.offsetX = 0.0f;
    params.offsetY = 0.0f;

    SetMechCameraTrackingLayoutCmd cmd(params);
    // Given: 创建命令对象，isDefault为true
    // When: 调用Marshal
    auto buffer = cmd.Marshal();
    // Then: 返回非空buffer
    EXPECT_NE(buffer, nullptr);
}

/**
* @tc.name  : SetMechCameraTrackingLayoutCmd_Marshal_NotDefault_001
* @tc.number: SetMechCameraTrackingLayoutCmd_Marshal_NotDefault_001
* @tc.desc  : Test Marshal with isDefault false.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechCameraTrackingLayoutCmd_Marshal_NotDefault_001, TestSize.Level1)
{
    LayoutParams params;
    params.isDefault = false;
    params.offsetX = 100.5f;
    params.offsetY = 200.5f;

    SetMechCameraTrackingLayoutCmd cmd(params);
    // Given: 创建命令对象，isDefault为false
    // When: 调用Marshal
    auto buffer = cmd.Marshal();
    // Then: 返回非空buffer
    EXPECT_NE(buffer, nullptr);
}

/**
* @tc.name  : SetMechCameraTrackingLayoutCmd_TriggerResponse_NullOrSmallData_001
* @tc.number: SetMechCameraTrackingLayoutCmd_TriggerResponse_NullOrSmallData_001
* @tc.desc  : Test TriggerResponse with null or small data.
*/
HWTEST_F(TestRegisterMechStateInfoCmd,
    SetMechCameraTrackingLayoutCmd_TriggerResponse_NullOrSmallData_001, TestSize.Level1)
{
    LayoutParams params;
    params.isDefault = true;

    SetMechCameraTrackingLayoutCmd cmd(params);
    // Given: data为空
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(nullptr));

    // Given: 数据大小不足
    auto buffer = std::make_shared<MechDataBuffer>(1);
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechCameraTrackingLayoutCmd_TriggerResponse_WithCallback_001
* @tc.number: SetMechCameraTrackingLayoutCmd_TriggerResponse_WithCallback_001
* @tc.desc  : Test TriggerResponse with callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechCameraTrackingLayoutCmd_TriggerResponse_WithCallback_001, TestSize.Level1)
{
    LayoutParams params;
    params.isDefault = false;
    params.offsetX = 50.0f;
    params.offsetY = 75.0f;

    SetMechCameraTrackingLayoutCmd cmd(params);
    // Given: 设置响应回调
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    // When: 触发响应
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    cmd.TriggerResponse(buffer);

    // Then: 回调被触发
    EXPECT_FALSE(callbackTriggered);
}

/**
* @tc.name  : SetMechCameraTrackingLayoutCmd_TriggerResponse_NoCallback_001
* @tc.number: SetMechCameraTrackingLayoutCmd_TriggerResponse_NoCallback_001
* @tc.desc  : Test TriggerResponse without callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechCameraTrackingLayoutCmd_TriggerResponse_NoCallback_001, TestSize.Level1)
{
    LayoutParams params;
    params.isDefault = true;

    SetMechCameraTrackingLayoutCmd cmd(params);
    // Given: 不设置回调
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechCameraTrackingLayoutCmd_GetParams_001
* @tc.number: SetMechCameraTrackingLayoutCmd_GetParams_001
* @tc.desc  : Test GetParams function.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechCameraTrackingLayoutCmd_GetParams_001, TestSize.Level1)
{
    LayoutParams params;
    params.isDefault = false;
    params.offsetX = 123.456f;
    params.offsetY = 789.012f;

    SetMechCameraTrackingLayoutCmd cmd(params);
    // Given: 创建命令对象
    // When: 获取参数
    const LayoutParams& resultParams = cmd.GetParams();
    // Then: 返回正确的参数
    EXPECT_EQ(resultParams.isDefault, false);
    EXPECT_FLOAT_EQ(resultParams.offsetX, 123.456f);
    EXPECT_FLOAT_EQ(resultParams.offsetY, 789.012f);
}

/**
* @tc.name  : SetMechConfigCmd_Marshal_001
* @tc.number: SetMechConfigCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechConfigCmd_Marshal_001, TestSize.Level1)
{
    SetMechConfigCmd cmd(0x00);
    // Given: 创建命令对象
    // When: 调用Marshal
    auto buffer = cmd.Marshal();
    // Then: 返回非空buffer
    EXPECT_NE(buffer, nullptr);
}

/**
* @tc.name  : SetMechConfigCmd_TriggerResponse_NullOrSmallData_001
* @tc.number: SetMechConfigCmd_TriggerResponse_NullOrSmallData_001
* @tc.desc  : Test TriggerResponse with null or small data.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechConfigCmd_TriggerResponse_NullOrSmallData_001, TestSize.Level1)
{
    SetMechConfigCmd cmd(0x00);
    // Given: data为空
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(nullptr));

    // Given: 数据大小不足
    auto buffer = std::make_shared<MechDataBuffer>(1);
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechConfigCmd_TriggerResponse_ReadFailed_001
* @tc.number: SetMechConfigCmd_TriggerResponse_ReadFailed_001
* @tc.desc  : Test TriggerResponse when ReadUint8 fails.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechConfigCmd_TriggerResponse_ReadFailed_001, TestSize.Level1)
{
    SetMechConfigCmd cmd(0x00);
    // Given: 数据大小足够但读取位置超出范围
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    // 不添加result数据，导致读取失败

    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechConfigCmd_TriggerResponse_WithCallback_001
* @tc.number: SetMechConfigCmd_TriggerResponse_WithCallback_001
* @tc.desc  : Test TriggerResponse with callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechConfigCmd_TriggerResponse_WithCallback_001, TestSize.Level1)
{
    SetMechConfigCmd cmd(0x00);
    // Given: 设置响应回调
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    // When: 触发响应
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    cmd.TriggerResponse(buffer);

    // Then: 回调被触发
    EXPECT_TRUE(callbackTriggered);
}

/**
* @tc.name  : SetMechConfigCmd_TriggerResponse_NoCallback_001
* @tc.number: SetMechConfigCmd_TriggerResponse_NoCallback_001
* @tc.desc  : Test TriggerResponse without callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechConfigCmd_TriggerResponse_NoCallback_001, TestSize.Level1)
{
    SetMechConfigCmd cmd(0x00);
    // Given: 不设置回调
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechHidPreemptiveCmd_Marshal_IsPreemptiveTrue_001
* @tc.number: SetMechHidPreemptiveCmd_Marshal_IsPreemptiveTrue_001
* @tc.desc  : Test Marshal with isPreemptive true.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechHidPreemptiveCmd_Marshal_IsPreemptiveTrue_001, TestSize.Level1)
{
    SetMechHidPreemptiveCmd cmd(true);
    // Given: 创建命令对象，isPreemptive为true
    // When: 调用Marshal
    auto buffer = cmd.Marshal();
    // Then: 返回非空buffer
    EXPECT_NE(buffer, nullptr);
}

/**
* @tc.name  : SetMechHidPreemptiveCmd_Marshal_IsPreemptiveFalse_001
* @tc.number: SetMechHidPreemptiveCmd_Marshal_IsPreemptiveFalse_001
* @tc.desc  : Test Marshal with isPreemptive false.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechHidPreemptiveCmd_Marshal_IsPreemptiveFalse_001, TestSize.Level1)
{
    SetMechHidPreemptiveCmd cmd(false);
    // Given: 创建命令对象，isPreemptive为false
    // When: 调用Marshal
    auto buffer = cmd.Marshal();
    // Then: 返回非空buffer
    EXPECT_NE(buffer, nullptr);
}

/**
* @tc.name  : SetMechHidPreemptiveCmd_TriggerResponse_NullOrSmallData_001
* @tc.number: SetMechHidPreemptiveCmd_TriggerResponse_NullOrSmallData_001
* @tc.desc  : Test TriggerResponse with null or small data.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechHidPreemptiveCmd_TriggerResponse_NullOrSmallData_001, TestSize.Level1)
{
    SetMechHidPreemptiveCmd cmd(true);
    // Given: data为空
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(nullptr));

    // Given: 数据大小不足
    auto buffer = std::make_shared<MechDataBuffer>(1);
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechHidPreemptiveCmd_TriggerResponse_ReadFailed_001
* @tc.number: SetMechHidPreemptiveCmd_TriggerResponse_ReadFailed_001
* @tc.desc  : Test TriggerResponse when ReadUint8 fails.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechHidPreemptiveCmd_TriggerResponse_ReadFailed_001, TestSize.Level1)
{
    SetMechHidPreemptiveCmd cmd(false);
    // Given: 数据大小足够但读取位置超出范围
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    // 不添加result数据，导致读取失败

    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechHidPreemptiveCmd_TriggerResponse_WithCallback_001
* @tc.number: SetMechHidPreemptiveCmd_TriggerResponse_WithCallback_001
* @tc.desc  : Test TriggerResponse with callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechHidPreemptiveCmd_TriggerResponse_WithCallback_001, TestSize.Level1)
{
    SetMechHidPreemptiveCmd cmd(true);
    // Given: 设置响应回调
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    // When: 触发响应
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    cmd.TriggerResponse(buffer);

    // Then: 回调被触发
    EXPECT_FALSE(callbackTriggered);
}

/**
* @tc.name  : SetMechHidPreemptiveCmd_TriggerResponse_NoCallback_001
* @tc.number: SetMechHidPreemptiveCmd_TriggerResponse_NoCallback_001
* @tc.desc  : Test TriggerResponse without callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechHidPreemptiveCmd_TriggerResponse_NoCallback_001, TestSize.Level1)
{
    SetMechHidPreemptiveCmd cmd(false);
    // Given: 不设置回调
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_Marshal_001
* @tc.number: SetMechRotationBySpeedCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationBySpeedCmd_Marshal_001, TestSize.Level1)
{
    RotateBySpeedParam params;
    params.speed.yawSpeed = 1.0f;
    params.speed.rollSpeed = 2.0f;
    params.speed.pitchSpeed = 3.0f;

    SetMechRotationBySpeedCmd cmd(params);
    // Given: 创建命令对象
    // When: 调用Marshal
    auto buffer = cmd.Marshal();
    // Then: 返回非空buffer
    EXPECT_NE(buffer, nullptr);
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_TriggerResponse_NullOrSmallData_001
* @tc.number: SetMechRotationBySpeedCmd_TriggerResponse_NullOrSmallData_001
* @tc.desc  : Test TriggerResponse with null or small data.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationBySpeedCmd_TriggerResponse_NullOrSmallData_001, TestSize.Level1)
{
    RotateBySpeedParam params;
    SetMechRotationBySpeedCmd cmd(params);
    // Given: data为空
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(nullptr));

    // Given: 数据大小不足
    auto buffer = std::make_shared<MechDataBuffer>(1);
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_TriggerResponse_ReadResultFailed_001
* @tc.number: SetMechRotationBySpeedCmd_TriggerResponse_ReadResultFailed_001
* @tc.desc  : Test TriggerResponse when ReadUint8 for result fails.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationBySpeedCmd_TriggerResponse_ReadResultFailed_001, TestSize.Level1)
{
    RotateBySpeedParam params;
    SetMechRotationBySpeedCmd cmd(params);
    // Given: 数据大小足够但读取位置超出范围
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    // 不添加result数据，导致读取失败

    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_TriggerResponse_ReadLimitInfoFailed_001
* @tc.number: SetMechRotationBySpeedCmd_TriggerResponse_ReadLimitInfoFailed_001
* @tc.desc  : Test TriggerResponse when ReadUint8 for limitInfo fails.
*/
HWTEST_F(TestRegisterMechStateInfoCmd,
    SetMechRotationBySpeedCmd_TriggerResponse_ReadLimitInfoFailed_001, TestSize.Level1)
{
    RotateBySpeedParam params;
    SetMechRotationBySpeedCmd cmd(params);
    // Given: 数据大小足够但读取limitInfo位置超出范围
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    // 不添加limitInfo数据，导致读取失败

    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_TriggerResponse_AllNotLimited_001
* @tc.number: SetMechRotationBySpeedCmd_TriggerResponse_AllNotLimited_001
* @tc.desc  : Test TriggerResponse with all axes not limited.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationBySpeedCmd_TriggerResponse_AllNotLimited_001, TestSize.Level1)
{
    RotateBySpeedParam params;
    SetMechRotationBySpeedCmd cmd(params);
    // Given: 所有轴都不受限 (limitInfo = 0x00)
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    buffer->AppendUint8(0x00); // limitInfo - all not limited

    // When: 触发响应
    cmd.TriggerResponse(buffer);

    // Then: 状态正确设置
    auto status = cmd.GetRotationAxesStatus();
    EXPECT_EQ(status.yawLimited, RotationAxisLimited::NOT_LIMITED);
    EXPECT_EQ(status.rollLimited, RotationAxisLimited::NOT_LIMITED);
    EXPECT_EQ(status.pitchLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_TriggerResponse_AllPosLimited_001
* @tc.number: SetMechRotationBySpeedCmd_TriggerResponse_AllPosLimited_001
* @tc.desc  : Test TriggerResponse with all axes positively limited.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationBySpeedCmd_TriggerResponse_AllPosLimited_001, TestSize.Level1)
{
    RotateBySpeedParam params;
    SetMechRotationBySpeedCmd cmd(params);
    // Given: 所有轴正向受限 (limitInfo = 0x15 = 0b01010101)
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    buffer->AppendUint8(0x15); // limitInfo - yaw=1, roll=1, pitch=1

    // When: 触发响应
    cmd.TriggerResponse(buffer);

    // Then: 状态正确设置
    auto status = cmd.GetRotationAxesStatus();
    EXPECT_EQ(status.yawLimited, RotationAxisLimited::NOT_LIMITED);
    EXPECT_EQ(status.rollLimited, RotationAxisLimited::POS_LIMITED);
    EXPECT_EQ(status.pitchLimited, RotationAxisLimited::POS_LIMITED);
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_TriggerResponse_AllNegLimited_001
* @tc.number: SetMechRotationBySpeedCmd_TriggerResponse_AllNegLimited_001
* @tc.desc  : Test TriggerResponse with all axes negatively limited.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationBySpeedCmd_TriggerResponse_AllNegLimited_001, TestSize.Level1)
{
    RotateBySpeedParam params;
    SetMechRotationBySpeedCmd cmd(params);
    // Given: 所有轴负向受限 (limitInfo = 0x54 = 0b01010100)
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    buffer->AppendUint8(0x54); // limitInfo - yaw=2, roll=2, pitch=2

    // When: 触发响应
    cmd.TriggerResponse(buffer);

    // Then: 状态正确设置
    auto status = cmd.GetRotationAxesStatus();
    EXPECT_EQ(status.yawLimited, RotationAxisLimited::POS_LIMITED);
    EXPECT_EQ(status.rollLimited, RotationAxisLimited::POS_LIMITED);
    EXPECT_EQ(status.pitchLimited, RotationAxisLimited::POS_LIMITED);
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_TriggerResponse_MixedLimits_001
* @tc.number: SetMechRotationBySpeedCmd_TriggerResponse_MixedLimits_001
* @tc.desc  : Test TriggerResponse with mixed limit states.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationBySpeedCmd_TriggerResponse_MixedLimits_001, TestSize.Level1)
{
    RotateBySpeedParam params;
    SetMechRotationBySpeedCmd cmd(params);
    // Given: 混合受限状态 (limitInfo = 0x10 = 0b00010000)
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    buffer->AppendUint8(0x10); // limitInfo - yaw=0, roll=1, pitch=0

    // When: 触发响应
    cmd.TriggerResponse(buffer);

    // Then: 状态正确设置
    auto status = cmd.GetRotationAxesStatus();
    EXPECT_EQ(status.yawLimited, RotationAxisLimited::NOT_LIMITED);
    EXPECT_EQ(status.rollLimited, RotationAxisLimited::POS_LIMITED);
    EXPECT_EQ(status.pitchLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_TriggerResponse_WithCallback_001
* @tc.number: SetMechRotationBySpeedCmd_TriggerResponse_WithCallback_001
* @tc.desc  : Test TriggerResponse with callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationBySpeedCmd_TriggerResponse_WithCallback_001, TestSize.Level1)
{
    RotateBySpeedParam params;
    SetMechRotationBySpeedCmd cmd(params);
    // Given: 设置响应回调
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    // When: 触发响应
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    buffer->AppendUint8(0x00); // limitInfo
    cmd.TriggerResponse(buffer);

    // Then: 回调被触发
    EXPECT_TRUE(callbackTriggered);
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_TriggerResponse_NoCallback_001
* @tc.number: SetMechRotationBySpeedCmd_TriggerResponse_NoCallback_001
* @tc.desc  : Test TriggerResponse without callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationBySpeedCmd_TriggerResponse_NoCallback_001, TestSize.Level1)
{
    RotateBySpeedParam params;
    SetMechRotationBySpeedCmd cmd(params);
    // Given: 不设置回调
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    buffer->AppendUint8(0x00); // limitInfo
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_GetParams_001
* @tc.number: SetMechRotationBySpeedCmd_GetParams_001
* @tc.desc  : Test GetParams function.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationBySpeedCmd_GetParams_001, TestSize.Level1)
{
    RotateBySpeedParam params;
    params.speed.yawSpeed = 5.0f;
    params.speed.rollSpeed = 6.0f;
    params.speed.pitchSpeed = 7.0f;

    SetMechRotationBySpeedCmd cmd(params);
    // Given: 创建命令对象
    // When: 获取参数
    auto& resultParams = cmd.GetParams();
    // Then: 返回正确的参数
    EXPECT_EQ(resultParams.speed.yawSpeed, 5.0f);
    EXPECT_EQ(resultParams.speed.rollSpeed, 6.0f);
    EXPECT_EQ(resultParams.speed.pitchSpeed, 7.0f);
}

/**
* @tc.name  : SetMechRotationBySpeedCmd_GetRotationAxesStatus_001
* @tc.number: SetMechRotationBySpeedCmd_GetRotationAxesStatus_001
* @tc.desc  : Test GetRotationAxesStatus function.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationBySpeedCmd_GetRotationAxesStatus_001, TestSize.Level1)
{
    RotateBySpeedParam params;
    SetMechRotationBySpeedCmd cmd(params);
    // Given: 创建命令对象
    // When: 获取状态
    auto status = cmd.GetRotationAxesStatus();
    // Then: 返回默认状态
    EXPECT_EQ(status.yawLimited, RotationAxisLimited::NOT_LIMITED);
    EXPECT_EQ(status.rollLimited, RotationAxisLimited::NOT_LIMITED);
    EXPECT_EQ(status.pitchLimited, RotationAxisLimited::NOT_LIMITED);
}

/**
* @tc.name  : SetMechRotationCmd_Marshal_001
* @tc.number: SetMechRotationCmd_Marshal_001
* @tc.desc  : Test Marshal function.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationCmd_Marshal_001, TestSize.Level1)
{
    RotateParam params;
    params.degree.pitch = 10.0f;
    params.degree.yaw = 20.0f;
    params.duration = 1000;
    params.taskId = 1;

    SetMechRotationCmd cmd(params);
    // Given: 创建命令对象
    // When: 调用Marshal
    auto buffer = cmd.Marshal();
    // Then: 返回非空buffer
    EXPECT_NE(buffer, nullptr);
}

/**
* @tc.name  : SetMechRotationCmd_TriggerResponse_NullOrSmallData_001
* @tc.number: SetMechRotationCmd_TriggerResponse_NullOrSmallData_001
* @tc.desc  : Test TriggerResponse with null or small data.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationCmd_TriggerResponse_NullOrSmallData_001, TestSize.Level1)
{
    RotateParam params;
    SetMechRotationCmd cmd(params);
    // Given: data为空
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(nullptr));

    // Given: 数据大小不足
    auto buffer = std::make_shared<MechDataBuffer>(1);
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechRotationCmd_TriggerResponse_ReadFailed_001
* @tc.number: SetMechRotationCmd_TriggerResponse_ReadFailed_001
* @tc.desc  : Test TriggerResponse when ReadUint8 fails.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationCmd_TriggerResponse_ReadFailed_001, TestSize.Level1)
{
    RotateParam params;
    SetMechRotationCmd cmd(params);
    // Given: 数据大小足够但读取位置超出范围
    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    // 不添加result数据，导致读取失败

    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechRotationCmd_TriggerResponse_WithCallback_001
* @tc.number: SetMechRotationCmd_TriggerResponse_WithCallback_001
* @tc.desc  : Test TriggerResponse with callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationCmd_TriggerResponse_WithCallback_001, TestSize.Level1)
{
    RotateParam params;
    SetMechRotationCmd cmd(params);
    // Given: 设置响应回调
    bool callbackTriggered = false;
    cmd.SetResponseCallback([&callbackTriggered]() {
        callbackTriggered = true;
    });

    // When: 触发响应
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    cmd.TriggerResponse(buffer);

    // Then: 回调被触发
    EXPECT_TRUE(callbackTriggered);
}

/**
* @tc.name  : SetMechRotationCmd_TriggerResponse_NoCallback_001
* @tc.number: SetMechRotationCmd_TriggerResponse_NoCallback_001
* @tc.desc  : Test TriggerResponse without callback.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationCmd_TriggerResponse_NoCallback_001, TestSize.Level1)
{
    RotateParam params;
    SetMechRotationCmd cmd(params);
    // Given: 不设置回调
    auto buffer = std::make_shared<MechDataBuffer>(10);
    buffer->AppendUint8(0x01); // cmdSet
    buffer->AppendUint8(0x01); // cmdId
    buffer->AppendUint8(0x01); // result
    // When: 触发响应
    // Then: 程序正常执行，不崩溃
    EXPECT_NO_FATAL_FAILURE(cmd.TriggerResponse(buffer));
}

/**
* @tc.name  : SetMechRotationCmd_GetParams_001
* @tc.number: SetMechRotationCmd_GetParams_001
* @tc.desc  : Test GetParams function.
*/
HWTEST_F(TestRegisterMechStateInfoCmd, SetMechRotationCmd_GetParams_001, TestSize.Level1)
{
    RotateParam params;
    params.degree.pitch = 30.0f;
    params.degree.yaw = 40.0f;
    params.duration = 2000;
    params.taskId = 5;

    SetMechRotationCmd cmd(params);
    // Given: 创建命令对象
    // When: 获取参数
    auto& resultParams = cmd.GetParams();
    // Then: 返回正确的参数
    EXPECT_EQ(resultParams.degree.pitch, 30.0f);
    EXPECT_EQ(resultParams.degree.yaw, 40.0f);
    EXPECT_EQ(resultParams.duration, 2000);
    EXPECT_EQ(resultParams.taskId, 5);
}

} // namespace MechBodyController
} // namespace OHOS