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
#include <iostream>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "mc_command_test_0x02.h"

#include "../../mechbody_controller_log.h"
#include "../../test_log.h"

#include "mc_normal_get_mech_base_info_cmd.h"
#include "mc_normal_get_mech_capability_info_cmd.h"
#include "mc_normal_get_mech_coordinate_info_cmd.h"
#include "mc_normal_get_mech_limit_info_cmd.h"
#include "mc_normal_get_mech_pose_info_cmd.h"
#include "mc_normal_get_mech_state_info_cmd.h"
#include "mc_normal_register_mech_generic_event_cmd.h"
#include "mc_normal_register_mech_key_event_cmd.h"
#include "mc_normal_register_mech_position_info_cmd.h"
#include "mc_normal_register_mech_state_info_cmd.h"
#include "mc_normal_register_mech_tracking_enable_cmd.h"
#include "mc_normal_set_mech_camera_info_cmd.h"
#include "mc_normal_set_mech_camera_tracking_enable_cmd.h"
#include "mc_normal_set_mech_camera_tracking_frame_cmd.h"
#include "mc_normal_set_mech_location_report_cmd.h"
#include "mc_normal_set_mech_motion_control_cmd.h"
#include "mc_normal_set_mech_rotation_by_speed_cmd.h"
#include "mc_normal_set_mech_rotation_to_location_cmd.h"
#include "mc_normal_set_mech_protocol_ver_cmd.h"
#include "mc_normal_set_mech_rotation_trace_cmd.h"
#include "mc_normal_set_mech_protocol_ver_cmd.h"
#include "mc_normal_set_mech_phone_status_cmd.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace MechBodyController {
namespace {

}

void MechCommandTest0x02::SetUpTestCase()
{
}

void MechCommandTest0x02::TearDownTestCase()
{
}

void MechCommandTest0x02::SetUp()
{
}

void MechCommandTest0x02::TearDown()
{
}

void ResponseCb()
{
}
uint16_t g_taskId = 0;
void AppendUint8BySize(std::shared_ptr<MechDataBuffer> buffer, int size)
{
    for (int i = 0; i < size; i++) {
        buffer->AppendUint8(static_cast<uint8_t>(i));
    }
}


HWTEST_F(MechCommandTest0x02, NormalSetMechRotationTraceCmd_Marshal_001, TestSize.Level1)
{
    // Given: 准备旋转轨迹参数
    std::vector<RotateParam> paramsVector;

    RotateParam params;
    params.duration = 1000;
    params.degree.yaw = 1.1;
    params.degree.roll = 2.2;
    params.degree.pitch = 3.3;
    paramsVector.push_back(params);

    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonSetMechRotationTraceCmd> executionCmdCommon =
        factory.CreateSetMechRotationTraceCmd(++g_taskId, paramsVector);
    std::shared_ptr<NormalSetMechRotationTraceCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechRotationTraceCmd>(executionCmdCommon);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(executionCmd, nullptr);
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationToLocationCmd_Marshal_001, TestSize.Level1)
{
    // Given: 准备旋转到位置参数
    RotateToLocationParam params;
    params.rotateMap = 8;
    params.rotateTime = 16;
    params.yawRadian = 1.1;
    params.rollRadian = 2.2;
    params.pitchRadian = 3.3;

    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechRotationToLocationCmd> executionCmd =
        factory.CreateSetMechRotationToLocationCmd(params);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(executionCmd, nullptr);
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationToLocationCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 准备测试参数和命令对象
    RotateToLocationParam params;
    params.rotateMap = 8;
    params.rotateTime = 16;
    params.yawRadian = 1.1;
    params.rollRadian = 2.2;
    params.pitchRadian = 3.3;
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechRotationToLocationCmd> executionCmd =
        factory.CreateSetMechRotationToLocationCmd(params);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);

    executionCmd->TriggerResponse(nullptr);
    executionCmd->TriggerResponse(bufferEmpty);

    // Then: 验证异常情况下结果未改变
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationToLocationCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建旋转到位置命令对象
    RotateToLocationParam params;
    params.rotateMap = 8;
    params.rotateTime = 16;
    params.yawRadian = 1.1;
    params.rollRadian = 2.2;
    params.pitchRadian = 3.3;
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechRotationToLocationCmd> executionCmd =
        factory.CreateSetMechRotationToLocationCmd(params);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试yaw轴未限制状态
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(100);
        buffer->AppendUint8(1);
        buffer->AppendUint8(0b00000000);

        executionCmd->TriggerResponse(buffer);

        // Then: 验证yaw轴未限制
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().yawLimited, RotationAxisLimited::NOT_LIMITED);
    }
    // When: 测试yaw轴正向限制状态
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(100);
        buffer->AppendUint8(1);
        buffer->AppendUint8(0b00000001);

        executionCmd->TriggerResponse(buffer);

        // Then: 验证yaw轴正向限制
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().yawLimited, RotationAxisLimited::POS_LIMITED);
    }
    // When: 测试yaw轴负向限制状态
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(100);
        buffer->AppendUint8(1);
        buffer->AppendUint8(0b00000010);

        executionCmd->TriggerResponse(buffer);

        // Then: 验证yaw轴负向限制
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().yawLimited, RotationAxisLimited::NEG_LIMITED);
    }
    // When: 测试roll轴正向限制状态
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(100);
        buffer->AppendUint8(1);
        buffer->AppendUint8(0b00000100);

        executionCmd->TriggerResponse(buffer);

        // Then: 验证roll轴正向限制
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().rollLimited, RotationAxisLimited::POS_LIMITED);
    }
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationToLocationCmd_TriggerResponse_002_001, TestSize.Level1)
{
    // Given: 创建旋转到位置命令对象
    RotateToLocationParam params;
    params.rotateMap = 8;
    params.rotateTime = 16;
    params.yawRadian = 1.1;
    params.rollRadian = 2.2;
    params.pitchRadian = 3.3;
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechRotationToLocationCmd> executionCmd =
        factory.CreateSetMechRotationToLocationCmd(params);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试roll轴负向限制状态
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(100);
        buffer->AppendUint8(1);
        buffer->AppendUint8(0b00001000);

        executionCmd->TriggerResponse(buffer);

        // Then: 验证roll轴负向限制
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().rollLimited, RotationAxisLimited::NEG_LIMITED);
    }
    // When: 测试pitch轴正向限制状态
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(100);
        buffer->AppendUint8(1);
        buffer->AppendUint8(0b00010000);

        executionCmd->TriggerResponse(buffer);

        // Then: 验证pitch轴正向限制
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().pitchLimited, RotationAxisLimited::POS_LIMITED);
    }
    // When: 设置回调并测试pitch轴负向限制状态
    executionCmd->SetResponseCallback(ResponseCb);
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(100);
        buffer->AppendUint8(1);
        buffer->AppendUint8(0b00100000);

        executionCmd->TriggerResponse(buffer);

        // Then: 验证pitch轴负向限制
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().pitchLimited, RotationAxisLimited::NEG_LIMITED);
    }
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationBySpeedCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建按速度旋转命令对象
    RotateBySpeedParam params;
    params.speed.yawSpeed = 1.1;
    params.speed.rollSpeed = 2.2;
    params.speed.pitchSpeed = 3.3;
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonSetMechRotationBySpeedCmd> executionCmdCommon =
        factory.CreateSetMechRotationBySpeedCmd(params);
    std::shared_ptr<NormalSetMechRotationBySpeedCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechRotationBySpeedCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationBySpeedCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 准备测试参数和命令对象
    RotateBySpeedParam params;
    params.speed.yawSpeed = 1.1;
    params.speed.rollSpeed = 2.2;
    params.speed.pitchSpeed = 3.3;
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonSetMechRotationBySpeedCmd> executionCmdCommon =
        factory.CreateSetMechRotationBySpeedCmd(params);
    std::shared_ptr<NormalSetMechRotationBySpeedCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechRotationBySpeedCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);

    executionCmd->TriggerResponse(nullptr);
    executionCmd->TriggerResponse(bufferEmpty);

    // Then: 验证异常情况下结果未改变
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationBySpeedCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建按速度旋转命令对象
    RotateBySpeedParam params;
    params.speed.yawSpeed = 1.1;
    params.speed.rollSpeed = 2.2;
    params.speed.pitchSpeed = 3.3;
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonSetMechRotationBySpeedCmd> executionCmdCommon =
        factory.CreateSetMechRotationBySpeedCmd(params);
    std::shared_ptr<NormalSetMechRotationBySpeedCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechRotationBySpeedCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试yaw轴未限制状态
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(++g_taskId);
        buffer->AppendUint8(1);
        buffer->AppendUint8(0b00000000);

        executionCmd->TriggerResponse(buffer);

        // Then: 验证yaw轴未限制
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().yawLimited, RotationAxisLimited::NOT_LIMITED);
    }
    // When: 测试yaw轴正向限制状态
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(++g_taskId);
        buffer->AppendUint8(1);
        buffer->AppendUint8(0b00000001);

        executionCmd->TriggerResponse(buffer);

        // Then: 验证yaw轴正向限制
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().yawLimited, RotationAxisLimited::POS_LIMITED);
    }
    // When: 测试yaw轴负向限制状态
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(++g_taskId);
        buffer->AppendUint8(1);
        buffer->AppendUint8(0b00000010);

        executionCmd->TriggerResponse(buffer);

        // Then: 验证yaw轴负向限制
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().yawLimited, RotationAxisLimited::NEG_LIMITED);
    }
    // When: 测试roll轴正向限制状态
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(++g_taskId);
        buffer->AppendUint8(1);
        buffer->AppendUint8(0b00000100);

        executionCmd->TriggerResponse(buffer);

        // Then: 验证roll轴正向限制
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().rollLimited, RotationAxisLimited::POS_LIMITED);
    }
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationBySpeedCmd_TriggerResponse_002_001, TestSize.Level1)
{
    // Given: 创建按速度旋转命令对象
    RotateBySpeedParam params;
    params.speed.yawSpeed = 1.1;
    params.speed.rollSpeed = 2.2;
    params.speed.pitchSpeed = 3.3;
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonSetMechRotationBySpeedCmd> executionCmdCommon =
        factory.CreateSetMechRotationBySpeedCmd(params);
    std::shared_ptr<NormalSetMechRotationBySpeedCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechRotationBySpeedCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试不同的旋转轴限制状态
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(++g_taskId);
        buffer->AppendUint8(1);
        buffer->AppendUint8(0b00001000);

        executionCmd->TriggerResponse(buffer);

        // Then: 验证roll轴负方向限制
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().rollLimited, RotationAxisLimited::NEG_LIMITED);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(++g_taskId);
        buffer->AppendUint8(1);
        buffer->AppendUint8(0b00010000);

        executionCmd->TriggerResponse(buffer);

        // Then: 验证pitch轴正方向限制
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().pitchLimited, RotationAxisLimited::POS_LIMITED);
    }
    executionCmd->SetResponseCallback(ResponseCb);
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(++g_taskId);
        buffer->AppendUint8(1);
        buffer->AppendUint8(0b00100000);

        executionCmd->TriggerResponse(buffer);

        // Then: 验证pitch轴负方向限制
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().pitchLimited, RotationAxisLimited::NEG_LIMITED);
    }
}

HWTEST_F(MechCommandTest0x02, NormalSetMechProtocolVerCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建协议版本命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechProtocolVerCmd> executionCmd =
        factory.CreateNormalSetMechProtocolVerCmd(10);
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechProtocolVerCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建协议版本命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechProtocolVerCmd> executionCmd =
        factory.CreateNormalSetMechProtocolVerCmd(10);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针异常处理
    executionCmd->TriggerResponse(nullptr);
    executionCmd->SetResponseCallback(ResponseCb);
    executionCmd->TriggerResponse(nullptr);

    // Then: 验证命令属性
    EXPECT_EQ(executionCmd->GetCmdSet(), NormalSetMechProtocolVerCmd::CMD_SET);
    EXPECT_EQ(executionCmd->GetCmdId(), NormalSetMechProtocolVerCmd::CMD_ID);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechMotionControlCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建运动控制命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechMotionControlCmd> executionCmd =
        factory.CreateSetMechMotionControlCmd(ControlCommand::STOP);
    ASSERT_NE(executionCmd, nullptr);

    // When: 获取命令属性和执行序列化
    auto action = executionCmd->GetAction();
    auto result = executionCmd->GetResult();
    auto buffer = executionCmd->Marshal();

    // Then: 验证命令属性和序列化结果
    EXPECT_EQ(action, ControlCommand::STOP);
    EXPECT_EQ(result, 0);
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechMotionControlCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建运动控制命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechMotionControlCmd> executionCmd =
        factory.CreateSetMechMotionControlCmd(ControlCommand::STOP);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);

    executionCmd->TriggerResponse(nullptr);
    executionCmd->TriggerResponse(bufferEmpty);
    executionCmd->TriggerResponse(buffer);

    // Then: 验证结果
    EXPECT_EQ(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechMotionControlCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建运动控制命令对象并设置回调
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechMotionControlCmd> executionCmd =
        factory.CreateSetMechMotionControlCmd(ControlCommand::STOP);
    ASSERT_NE(executionCmd, nullptr);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);

    // When: 设置回调并触发响应
    executionCmd->SetResponseCallback(ResponseCb);
    executionCmd->TriggerResponse(buffer);

    // Then: 验证结果
    EXPECT_EQ(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechLocationReportCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建位置上报命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechLocationReportCmd> executionCmd =
        factory.CreateSetMechLocationReportCmd(10, 20);
    ASSERT_NE(executionCmd, nullptr);

    // When: 获取结果和执行序列化
    auto result = executionCmd->GetResult();
    auto buffer = executionCmd->Marshal();

    // Then: 验证结果和序列化
    EXPECT_EQ(result, 0);
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechLocationReportCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建位置上报命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechLocationReportCmd> executionCmd =
        factory.CreateSetMechLocationReportCmd(10, 20);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);

    executionCmd->TriggerResponse(nullptr);
    executionCmd->TriggerResponse(bufferEmpty);
    executionCmd->TriggerResponse(buffer);

    // Then: 验证结果
    EXPECT_EQ(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechLocationReportCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建位置上报命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechLocationReportCmd> executionCmd =
        factory.CreateSetMechLocationReportCmd(10, 20);
    ASSERT_NE(executionCmd, nullptr);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);

    // When: 设置回调并触发响应
    executionCmd->SetResponseCallback(ResponseCb);
    executionCmd->TriggerResponse(buffer);

    // Then: 验证结果
    EXPECT_EQ(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechCameraTrackingFrameCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建相机追踪帧命令对象
    TrackingFrameParams params;
    params.cameraType = CameraType::BACK;
    params.confidence = ConfidenceLevel::HIGH;
    params.objectType = 3;
    params.targetId = 21;
    params.roi.x = 3.12;
    params.roi.y = 1.23;
    params.roi.width = 10;
    params.roi.height = 20;
    params.fovV = 234;
    params.fovH = 5345;
    params.isRecording = true;
    params.timeDelay = 3000;

    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonSetMechCameraTrackingFrameCmd> executionCmdCommon =
        factory.CreateSetMechCameraTrackingFrameCmd(params);
    std::shared_ptr<NormalSetMechCameraTrackingFrameCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechCameraTrackingFrameCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechCameraTrackingFrameCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建追踪帧命令对象
    TrackingFrameParams params;
    params.cameraType = CameraType::BACK;
    params.confidence = ConfidenceLevel::HIGH;
    params.objectType = 1;
    params.targetId = 1;
    params.roi.x = 1.0;
    params.roi.y = 1.0;
    params.roi.width = 1.0;
    params.roi.height = 1.0;
    params.fovV = 1;
    params.fovH = 1;
    params.isRecording = 1;
    params.timeDelay = 1;

    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonSetMechCameraTrackingFrameCmd> executionCmdCommon =
        factory.CreateSetMechCameraTrackingFrameCmd(params);
    std::shared_ptr<NormalSetMechCameraTrackingFrameCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechCameraTrackingFrameCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针异常处理
    executionCmd->TriggerResponse(nullptr);
    executionCmd->SetResponseCallback(ResponseCb);
    executionCmd->TriggerResponse(nullptr);

    // Then: 验证命令属性
    EXPECT_EQ(executionCmd->GetCmdSet(), NormalSetMechCameraTrackingFrameCmd::CMD_SET);
    EXPECT_EQ(executionCmd->GetCmdId(), NormalSetMechCameraTrackingFrameCmd::CMD_ID);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechCameraTrackingEnableCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建相机追踪使能命令对象
    MechTrackingStatus status = MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET;

    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonSetMechCameraTrackingEnableCmd> executionCmdCommon =
        factory.CreateSetMechCameraTrackingEnableCmd(status);
    std::shared_ptr<NormalSetMechCameraTrackingEnableCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechCameraTrackingEnableCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 获取属性和执行序列化
    auto cmdStatus = executionCmd->GetStatus();
    auto result = executionCmd->GetResult();
    auto buffer = executionCmd->Marshal();

    // Then: 验证属性和序列化结果
    EXPECT_EQ(cmdStatus, MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET);
    EXPECT_EQ(result, 0);
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechCameraTrackingEnableCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建相机追踪使能命令对象
    MechTrackingStatus status = MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET;

    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonSetMechCameraTrackingEnableCmd> executionCmdCommon =
        factory.CreateSetMechCameraTrackingEnableCmd(status);
    std::shared_ptr<NormalSetMechCameraTrackingEnableCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechCameraTrackingEnableCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);

    executionCmd->TriggerResponse(nullptr);
    executionCmd->TriggerResponse(bufferEmpty);
    executionCmd->TriggerResponse(buffer);

    // Then: 验证结果
    EXPECT_EQ(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechCameraTrackingEnableCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建相机追踪使能命令对象
    MechTrackingStatus status = MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET;

    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonSetMechCameraTrackingEnableCmd> executionCmdCommon =
        factory.CreateSetMechCameraTrackingEnableCmd(status);
    std::shared_ptr<NormalSetMechCameraTrackingEnableCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechCameraTrackingEnableCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);

    // When: 设置回调并触发响应
    executionCmd->SetResponseCallback(ResponseCb);
    executionCmd->TriggerResponse(buffer);

    // Then: 验证结果
    EXPECT_EQ(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechCameraInfoCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建相机信息命令对象
    CameraInfoParams params;
    params.fovV = 1;
    params.fovH = 1;
    params.zoomFactor = 3.23;
    params.isRecording = 128;
    params.cameraType = CameraType::BACK;

    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonSetMechCameraInfoCmd> executionCmdCommon = factory.CreateSetMechCameraInfoCmd(params);
    std::shared_ptr<NormalSetMechCameraInfoCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechCameraInfoCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechTrackingEnableCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建注册追踪使能命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechTrackingEnableCmd> executionCmdCommon =
        factory.CreateRegisterMechTrackingEnableCmd();
    std::shared_ptr<NormalRegisterMechTrackingEnableCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechTrackingEnableCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechTrackingEnableCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建注册追踪使能命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechTrackingEnableCmd> executionCmdCommon =
        factory.CreateRegisterMechTrackingEnableCmd();
    std::shared_ptr<NormalRegisterMechTrackingEnableCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechTrackingEnableCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    size_t capacity = 100;
    auto bufferEmpty = std::make_shared<MechDataBuffer>(capacity);
    executionCmd->TriggerResponse(bufferEmpty);
    executionCmd->TriggerResponse(nullptr);
    executionCmd->SetResponseCallback(ResponseCb);
    executionCmd->TriggerResponse(nullptr);

    // Then: 验证异常情况下结果未改变
    EXPECT_EQ(executionCmd->GetResult(), 0);

    // When: 测试正常响应
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        buffer->AppendUint8(10);
        executionCmd->TriggerResponse(buffer);

        // Then: 验证结果
        EXPECT_EQ(executionCmd->GetResult(), 10);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        buffer->AppendUint8(10);
        executionCmd->SetResponseCallback(ResponseCb);
        executionCmd->TriggerResponse(buffer);

        // Then: 验证结果
        EXPECT_EQ(executionCmd->GetResult(), 10);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechTrackingEnableCmd_Unmarshal_001, TestSize.Level1)
{
    // Given: 创建注册追踪使能命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechTrackingEnableCmd> executionCmdCommon =
        factory.CreateRegisterMechTrackingEnableCmd();
    std::shared_ptr<NormalRegisterMechTrackingEnableCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechTrackingEnableCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    EXPECT_EQ(executionCmd->Unmarshal(nullptr), false);

    size_t capacity = 100;
    auto bufferEmpty = std::make_shared<MechDataBuffer>(capacity);
    EXPECT_EQ(executionCmd->Unmarshal(bufferEmpty), false);

    // When: 测试正常情况下的反序列化
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);
    EXPECT_EQ(executionCmd->Unmarshal(buffer), true);

    // Then: 验证反序列化结果
    EXPECT_EQ(executionCmd->isEnabled_, true);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechStateInfoCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建注册状态信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechStateInfoCmd> executionCmdCommon = factory.CreateRegisterMechStateInfoCmd();
    std::shared_ptr<NormalRegisterMechStateInfoCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechStateInfoCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechStateInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建注册状态信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechStateInfoCmd> executionCmdCommon = factory.CreateRegisterMechStateInfoCmd();
    std::shared_ptr<NormalRegisterMechStateInfoCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechStateInfoCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针异常处理
    executionCmd->TriggerResponse(nullptr);
    executionCmd->SetResponseCallback(ResponseCb);
    executionCmd->TriggerResponse(nullptr);

    // Then: 验证异常情况下结果未改变
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechStateInfoCmd_Unmarshal_001, TestSize.Level1)
{
    // Given: 创建注册状态信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechStateInfoCmd> executionCmdCommon = factory.CreateRegisterMechStateInfoCmd();
    std::shared_ptr<NormalRegisterMechStateInfoCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechStateInfoCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    EXPECT_EQ(executionCmd->Unmarshal(nullptr), false);

    size_t capacity = 100;
    auto bufferEmpty = std::make_shared<MechDataBuffer>(capacity);
    EXPECT_EQ(executionCmd->Unmarshal(bufferEmpty), false);

    // When: 测试正常情况下的反序列化
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);
    buffer->AppendUint8(1);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);
    EXPECT_EQ(executionCmd->Unmarshal(buffer), true);

    // Then: 验证反序列化结果
    EXPECT_EQ(executionCmd->info_.isPhoneOn, true);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechPositionInfoCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建注册位置信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechPositionInfoCmd> executionCmdCommon = factory.CreateRegisterMechPositionInfoCmd();
    std::shared_ptr<NormalRegisterMechPositionInfoCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechPositionInfoCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechPositionInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建注册位置信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonRegisterMechPositionInfoCmd> executionCmdCommon = factory.CreateRegisterMechPositionInfoCmd();
    std::shared_ptr<NormalRegisterMechPositionInfoCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechPositionInfoCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    executionCmd->TriggerResponse(nullptr);
    executionCmd->SetResponseCallback(ResponseCb);
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    executionCmd->TriggerResponse(bufferEmpty);

    // Then: 验证异常情况下结果未改变
    EXPECT_EQ(executionCmd->GetResult(), 0);

    // When: 测试正常响应
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(8);
    executionCmd->TriggerResponse(buffer);

    // Then: 验证结果
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechPositionInfoCmd_Unmarshal_001, TestSize.Level1)
{
    // Given: 创建注册位置信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonRegisterMechPositionInfoCmd> executionCmdCommon = factory.CreateRegisterMechPositionInfoCmd();
    std::shared_ptr<NormalRegisterMechPositionInfoCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechPositionInfoCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    EXPECT_EQ(executionCmd->Unmarshal(nullptr), false);

    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_EQ(executionCmd->Unmarshal(bufferEmpty), false);

    // When: 测试正常情况下的反序列化
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(8);
    buffer->AppendFloat(1.11);
    buffer->AppendFloat(2.22);
    buffer->AppendFloat(3.33);
    EXPECT_EQ(executionCmd->Unmarshal(buffer), true);

    // Then: 验证反序列化结果
    EXPECT_GT(executionCmd->position_.pitch, 3);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建注册按键事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建注册按键事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针异常处理
    executionCmd->TriggerResponse(nullptr);
    executionCmd->SetResponseCallback(ResponseCb);
    executionCmd->TriggerResponse(nullptr);

    // Then: 验证异常情况下结果未改变
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_001, TestSize.Level1)
{
    // Given: 创建注册按键事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    EXPECT_EQ(executionCmd->Unmarshal(nullptr), false);
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_EQ(executionCmd->Unmarshal(bufferEmpty), false);

    // When: 测试正常情况下的反序列化
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);
    buffer->AppendUint8(1);
    buffer->AppendUint8(1);
    EXPECT_EQ(executionCmd->Unmarshal(buffer), true);

    // Then: 验证反序列化结果
    EXPECT_EQ(executionCmd->buttonFrequency_, 1);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_002, TestSize.Level1)
{
    // Given: 创建注册按键事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试异常情况 - 按键数量不匹配
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(1);
        buffer->AppendUint8(2);
        buffer->AppendUint8(3);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }

    // When: 测试正常情况 - 按键频率为2
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        buffer->AppendUint8(2);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);

        // Then: 验证按键频率
        EXPECT_EQ(executionCmd->buttonFrequency_, 2);
    }
    // When: 测试正常情况 - 按键频率为1
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
    }
    // When: 测试正常情况 - 追踪切换事件
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(2);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);

        // Then: 验证事件类型
        EXPECT_EQ(executionCmd->event_, CameraKeyEvent::SWITCH_TRACKING);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_002_001, TestSize.Level1)
{
    // Given: 创建注册按键事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试缩放放大事件
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(3);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);

        // Then: 验证事件类型
        EXPECT_EQ(executionCmd->event_, CameraKeyEvent::ZOOM_IN);
    }
    // When: 测试缩放缩小事件
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(4);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);

        // Then: 验证事件类型
        EXPECT_EQ(executionCmd->event_, CameraKeyEvent::ZOOM_OUT);
    }
    // When: 测试切换相机事件
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(5);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);

        // Then: 验证事件类型
        EXPECT_EQ(executionCmd->event_, CameraKeyEvent::SWITCH_CAMERA);
    }
    // When: 测试开始录像事件
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(6);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);

        // Then: 验证事件类型
        EXPECT_EQ(executionCmd->event_, CameraKeyEvent::START_FILMING);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_003, TestSize.Level1)
{
    // Given: 创建注册按键事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试异常情况 - 滚轮数据长度不足
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(7);
        buffer->AppendUint8(4);
        buffer->AppendUint8(3);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
    // When: 测试正常情况 - 滚轮事件
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(7);
        buffer->AppendUint8(5);
        buffer->AppendUint16(3);
        buffer->AppendUint16(12);
        buffer->AppendUint8(4);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);

        // Then: 验证滚轮速度
        EXPECT_EQ(executionCmd->wheelData_.speed, 12);
    }
    // When: 测试异常情况 - 事件类型不匹配
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_003_001, TestSize.Level1)
{
    // Given: 创建注册按键事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试正常情况 - 摇杆事件
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(8);
        buffer->AppendUint8(4);
        buffer->AppendUint16(1);
        buffer->AppendUint16(2);

        // Then: 验证反序列化成功和摇杆Y值
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
        EXPECT_EQ(executionCmd->stickY_, 2);
    }
    // When: 测试异常情况 - 摇杆数据长度不足
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(8);
        buffer->AppendUint8(5);
        buffer->AppendUint16(1);
        buffer->AppendUint16(2);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建注册通用事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建注册通用事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    executionCmd->TriggerResponse(nullptr);
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    executionCmd->TriggerResponse(bufferEmpty);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);
    executionCmd->TriggerResponse(buffer);

    // Then: 验证结果
    EXPECT_NE(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建注册通用事件命令对象并设置回调
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    executionCmd->SetResponseCallback(ResponseCb);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);

    // When: 触发响应
    executionCmd->TriggerResponse(buffer);

    // Then: 验证结果
    EXPECT_NE(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_UnMarshal_001, TestSize.Level1)
{
    // Given: 创建注册通用事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    EXPECT_EQ(executionCmd->Unmarshal(nullptr), false);

    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_EQ(executionCmd->Unmarshal(bufferEmpty), false);

    // When: 测试正常情况下的反序列化
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(0);
    buffer->AppendUint8(1);
    buffer->AppendUint8(10);
    EXPECT_EQ(executionCmd->Unmarshal(buffer), true);

    // Then: 验证反序列化结果
    EXPECT_EQ(executionCmd->GetParams().attached, 10);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_UnMarshal_002, TestSize.Level1)
{
    // Given: 创建注册通用事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试异常情况 - 数据长度不足
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(0);
        buffer->AppendUint8(2);
        buffer->AppendUint8(1);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
    // When: 测试正常情况 - attached状态
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(0);
        buffer->AppendUint8(1);
        buffer->AppendUint8(10);

        // Then: 验证反序列化成功和attached值
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
        EXPECT_EQ(executionCmd->GetParams().attached, 10);
    }
    // When: 测试异常情况 - 事件类型不匹配
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(1);
        buffer->AppendUint8(2);
        buffer->AppendUint8(10);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_UnMarshal_002_001, TestSize.Level1)
{
    // Given: 创建注册通用事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    ASSERT_NE(executionCmd, nullptr);

// When: 测试正常情况 - pitch禁用状态
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        buffer->AppendUint8(7);

        // Then: 验证反序列化成功和pitch禁用状态
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
        EXPECT_EQ(executionCmd->GetParams().pitchDisable, 1);
    }
    // When: 测试异常情况 - 数据长度不足
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(2);
        buffer->AppendUint8(2);
        buffer->AppendUint8(7);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
    // When: 测试正常情况 - roll禁用状态
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(2);
        buffer->AppendUint8(1);
        buffer->AppendUint8(7);

        // Then: 验证反序列化成功
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
    }
    // When: 测试异常情况 - 数据长度不足
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(2);
        buffer->AppendUint8(2);
        buffer->AppendUint8(7);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
    // When: 测试正常情况 - roll禁用状态
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(2);
        buffer->AppendUint8(1);
        buffer->AppendUint8(7);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
    }
}


HWTEST_F(MechCommandTest0x02, NormalGetMechStateInfoCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建获取状态信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechStateInfoCmd> executionCmd = factory.CreateGetMechStateInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechStateInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建获取状态信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechStateInfoCmd> executionCmd = factory.CreateGetMechStateInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    executionCmd->TriggerResponse(nullptr);
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    executionCmd->TriggerResponse(bufferEmpty);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(2);
    buffer->AppendUint8(255);
    executionCmd->TriggerResponse(buffer);
    auto result = executionCmd->GetResult();
    auto params = executionCmd->GetParams();

    // Then: 验证结果
    EXPECT_EQ(result, 2);
    EXPECT_EQ(params.pitchDisable, 1);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechStateInfoCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建获取状态信息命令对象并设置回调
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechStateInfoCmd> executionCmd = factory.CreateGetMechStateInfoCmd();
    executionCmd->SetResponseCallback(ResponseCb);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(2);
    buffer->AppendUint8(255);

    // When: 触发响应
    executionCmd->TriggerResponse(buffer);
    auto result = executionCmd->GetResult();
    auto params = executionCmd->GetParams();

    // Then: 验证结果
    EXPECT_EQ(result, 2);
    EXPECT_EQ(params.pitchDisable, 1);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechPoseInfoCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建获取姿态信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechPoseInfoCmd> executionCmd = factory.CreateGetMechPoseInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechPoseInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建获取姿态信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechPoseInfoCmd> executionCmd = factory.CreateGetMechPoseInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    executionCmd->TriggerResponse(nullptr);
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    executionCmd->TriggerResponse(bufferEmpty);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 3);
    for (size_t i = 0; i < 3; i++) {
        buffer->AppendUint16(100);
    }

    executionCmd->TriggerResponse(buffer);
    auto result = executionCmd->GetResult();
    auto params = executionCmd->GetParams();

    // Then: 验证结果
    EXPECT_EQ(result, 2);
    EXPECT_EQ(params.accelerationZ, 100);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechPoseInfoCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建获取姿态信息命令对象并设置回调
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechPoseInfoCmd> executionCmd = factory.CreateGetMechPoseInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    executionCmd->SetResponseCallback(ResponseCb);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 3);
    for (size_t i = 0; i < 3; i++) {
        buffer->AppendUint16(100);
    }

    // When: 触发响应
    executionCmd->TriggerResponse(buffer);
    auto result = executionCmd->GetResult();
    auto params = executionCmd->GetParams();

    // Then: 验证结果
    EXPECT_EQ(result, 2);
    EXPECT_EQ(params.accelerationZ, 100);
}

HWTEST_F(MechCommandTest0x02, CommonGetMechLimitInfoCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建获取限制信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonGetMechLimitInfoCmd> executionCmd = factory.CreateGetMechLimitInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, CommonGetMechLimitInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建获取限制信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonGetMechLimitInfoCmd> executionCmd = factory.CreateGetMechLimitInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    executionCmd->TriggerResponse(nullptr);
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    executionCmd->TriggerResponse(bufferEmpty);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 3);
    for (int i = 0; i < 6; i++) {
        buffer->AppendFloat(3.2434);
    }

    executionCmd->TriggerResponse(buffer);
    auto result = executionCmd->GetResult();
    auto params = executionCmd->GetParams();

    // Then: 验证结果
    EXPECT_EQ(result, 2);
    EXPECT_GT(params.negMax.pitch, 3);
}


HWTEST_F(MechCommandTest0x02, CommonGetMechLimitInfoCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建获取限制信息命令对象并设置回调
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonGetMechLimitInfoCmd> executionCmd = factory.CreateGetMechLimitInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    executionCmd->SetResponseCallback(ResponseCb);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 3);
    for (int i = 0; i < 6; i++) {
        buffer->AppendFloat(3.2434);
    }

    // When: 触发响应
    executionCmd->TriggerResponse(buffer);
    auto result = executionCmd->GetResult();
    auto params = executionCmd->GetParams();

    // Then: 验证结果
    EXPECT_EQ(result, 2);
    EXPECT_GT(params.negMax.pitch, 3);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechBaseInfoCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建获取基础信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechBaseInfoCmd> executionCmd = factory.CreateGetMechBaseInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechBaseInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建获取设备基本信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechBaseInfoCmd> executionCmd = factory.CreateGetMechBaseInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    executionCmd->TriggerResponse(nullptr);
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    executionCmd->TriggerResponse(bufferEmpty);

    // Then: 验证异常情况下结果未改变
    EXPECT_EQ(executionCmd->GetResult(), 0);

    // When: 测试正常情况下的响应处理
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);

    size_t macAddressSize = executionCmd->GetParams().macAddress.size();
    for (int i = 0; i < macAddressSize; i++) {
        buffer->AppendUint8(i + 1);
    }
    std::string deviceName = "111112222233333444445555566666";
    for (const char ch : deviceName) {
        buffer->AppendUint8(ch);
    }
    executionCmd->TriggerResponse(buffer);

    // Then: 验证设备名称正确解析
    EXPECT_EQ(executionCmd->GetParams().realName, deviceName);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechBaseInfoCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建获取设备基本信息命令对象并设置回调
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechBaseInfoCmd> executionCmd = factory.CreateGetMechBaseInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    executionCmd->SetResponseCallback(ResponseCb);

    // When: 测试正常情况下的响应处理
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);

    size_t macAddressSize = executionCmd->GetParams().macAddress.size();
    for (int i = 0; i < macAddressSize; i++) {
        buffer->AppendUint8(i + 1);
    }
    std::string deviceName = "111112222233333444445555566666";
    for (const char ch : deviceName) {
        buffer->AppendUint8(ch);
    }
    executionCmd->TriggerResponse(buffer);

    // Then: 验证设备名称正确解析
    EXPECT_EQ(executionCmd->GetParams().realName, deviceName);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechCapabilityInfoCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建获取能力信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechCapabilityInfoCmd> executionCmd = factory.CreateGetMechCapabilityInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechCapabilityInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建获取能力信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechCapabilityInfoCmd> executionCmd = factory.CreateGetMechCapabilityInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    executionCmd->TriggerResponse(nullptr);
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    executionCmd->TriggerResponse(bufferEmpty);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    for (int i = 0; i < 9; i++) {
        buffer->AppendUint8(static_cast<uint8_t>(i));
    }
    for (int i = 0; i < 3; i++) {
        buffer->AppendFloat(2.12);
    }
    int maxturntime = 120;
    buffer->AppendUint16(maxturntime);

    executionCmd->TriggerResponse(buffer);
    auto result = executionCmd->GetResult();
    auto params = executionCmd->GetParams();

    // Then: 验证结果
    EXPECT_EQ(result, 2);
    EXPECT_EQ(params.maxturntime, maxturntime);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechCapabilityInfoCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建获取能力信息命令对象并设置回调
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechCapabilityInfoCmd> executionCmd = factory.CreateGetMechCapabilityInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    executionCmd->SetResponseCallback(ResponseCb);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    for (int i = 0; i < 9; i++) {
        buffer->AppendUint8(static_cast<uint8_t>(i));
    }
    for (int i = 0; i < 3; i++) {
        buffer->AppendFloat(2.12);
    }
    int maxturntime = 120;
    buffer->AppendUint16(maxturntime);

    // When: 触发响应
    executionCmd->TriggerResponse(buffer);
    auto result = executionCmd->GetResult();
    auto params = executionCmd->GetParams();

    // Then: 验证结果
    EXPECT_EQ(result, 2);
    EXPECT_EQ(params.maxturntime, maxturntime);
}

HWTEST_F(MechCommandTest0x02, GetMechProtocolVerCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建获取协议版本命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<GetMechProtocolVerCmd> executionCmd = factory.CreateGetMechProtocolVerCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, GetMechProtocolVerCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建获取协议版本命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<GetMechProtocolVerCmd> executionCmd = factory.CreateGetMechProtocolVerCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    executionCmd->TriggerResponse(nullptr);
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    executionCmd->TriggerResponse(bufferEmpty);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);
    buffer->AppendUint8(2);
    buffer->AppendUint8(3);

    executionCmd->TriggerResponse(buffer);
    auto result = executionCmd->GetResult();

    // Then: 验证结果
    EXPECT_EQ(result, 1);
    EXPECT_EQ(executionCmd->lowProtocolVer_, 3);
}

HWTEST_F(MechCommandTest0x02, GetMechProtocolVerCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建获取协议版本命令对象并设置回调
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<GetMechProtocolVerCmd> executionCmd = factory.CreateGetMechProtocolVerCmd();
    ASSERT_NE(executionCmd, nullptr);

    executionCmd->SetResponseCallback(ResponseCb);
    executionCmd->TriggerResponse(nullptr);
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    executionCmd->TriggerResponse(bufferEmpty);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);
    buffer->AppendUint8(2);
    buffer->AppendUint8(3);

    // When: 触发响应
    executionCmd->TriggerResponse(buffer);
    auto result = executionCmd->GetResult();

    // Then: 验证结果
    EXPECT_EQ(result, 1);
    EXPECT_EQ(executionCmd->lowProtocolVer_, 3);
}


HWTEST_F(MechCommandTest0x02, NormalGetMechCoordinateInfoCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建获取坐标信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechCoordinateInfoCmd> executionCmd = factory.CreateGetMechCoordinateInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechCoordinateInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建获取坐标信息命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechCoordinateInfoCmd> executionCmd = factory.CreateGetMechCoordinateInfoCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针和空buffer的异常处理
    executionCmd->TriggerResponse(nullptr);
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    executionCmd->TriggerResponse(bufferEmpty);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 4);
    for (int i = 0; i < 3; i++) {
        buffer->AppendFloat(5.10);
    }
    buffer->AppendUint8(1);

    executionCmd->TriggerResponse(buffer);
    auto result = executionCmd->GetResult();
    auto params = executionCmd->GetParams();

    // Then: 验证结果
    EXPECT_EQ(result, 2);
    EXPECT_GT(params.pitchPose, 5);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechCoordinateInfoCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建获取坐标信息命令对象并设置回调
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechCoordinateInfoCmd> executionCmd = factory.CreateGetMechCoordinateInfoCmd();
    ASSERT_NE(executionCmd, nullptr);
    executionCmd->SetResponseCallback(ResponseCb);
    executionCmd->TriggerResponse(nullptr);
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    executionCmd->TriggerResponse(bufferEmpty);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 4);
    for (int i = 0; i < 3; i++) {
        buffer->AppendFloat(5.10);
    }
    buffer->AppendUint8(1);

    // When: 触发响应
    executionCmd->TriggerResponse(buffer);
    auto result = executionCmd->GetResult();
    auto params = executionCmd->GetParams();

    // Then: 验证结果
    EXPECT_EQ(result, 2);
    EXPECT_GT(params.pitchPose, 5);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_Unmarshal_003, TestSize.Level1)
{
    // Given: 创建注册通用事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试正常情况 - 0xFF事件类型
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(0xFF);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);

        // Then: 验证反序列化成功
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_Unmarshal_004, TestSize.Level1)
{
    // Given: 创建注册通用事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试异常情况 - 数据长度不足
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(0);
        buffer->AppendUint8(1);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_Unmarshal_005, TestSize.Level1)
{
    // Given: 创建注册通用事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试异常情况 - 数据长度不足
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_Unmarshal_006, TestSize.Level1)
{
    // Given: 创建注册通用事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    EXPECT_NE(executionCmd, nullptr);

    // When: 测试数据长度不足
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(2);
        buffer->AppendUint8(1);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_TriggerResponse_003, TestSize.Level1)
{
    // Given: 创建注册通用事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空buffer的异常处理
    auto buffer = std::make_shared<MechDataBuffer>(100);
    executionCmd->TriggerResponse(buffer);

    // Then: 验证异常情况下结果未改变
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_004, TestSize.Level1)
{
    // Given: 创建注册按键事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    // When: 测试按键按下事件
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(0xFF);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);

        // Then: 验证反序列化成功
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_005, TestSize.Level1)
{
    // Given: 创建注册按键事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    // When: 测试数据长度不足
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
    // When: 测试正常按键事件
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);

        // Then: 验证反序列化成功
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_006, TestSize.Level1)
{
    // Given: 创建注册按键事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    // When: 测试切换照片/视频事件
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(5);
        buffer->AppendUint8(1);
        buffer->AppendUint8(3);

        // Then: 验证反序列化成功和事件类型
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
        EXPECT_EQ(executionCmd->event_, CameraKeyEvent::SWITCH_PHOTO_FILM);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_007, TestSize.Level1)
{
    // Given: 创建注册按键事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    // When: 测试数据长度不足
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(7);
        buffer->AppendUint8(1);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
    // When: 测试数据长度不足
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(7);
        buffer->AppendUint8(5);
        buffer->AppendUint16(3);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
    // When: 测试数据长度不足
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(7);
        buffer->AppendUint8(5);
        buffer->AppendUint16(3);
        buffer->AppendUint16(12);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
    // When: 测试正常按键事件
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(7);
        buffer->AppendUint8(5);
        buffer->AppendUint16(3);
        buffer->AppendUint16(12);
        buffer->AppendUint8(4);

        // Then: 验证反序列化成功
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_008, TestSize.Level1)
{
    // Given: 创建注册按键事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    // When: 测试数据长度不足
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(8);
        buffer->AppendUint8(1);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
    // When: 测试数据长度不足
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(8);
        buffer->AppendUint8(4);
        buffer->AppendUint16(1);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
    // When: 测试正常摇杆事件
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(8);
        buffer->AppendUint8(4);
        buffer->AppendUint16(1);
        buffer->AppendUint16(2);

        // Then: 验证反序列化成功
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_009, TestSize.Level1)
{
    // Given: 创建注册按键事件命令对象
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    // When: 测试空数据
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);

        // Then: 验证反序列化失败
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }

    // Then: 验证结果值
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechPhoneStatusCmd_Marshal_001, TestSize.Level1)
{
    // Given: 创建设置手机状态命令对象
    ScreenInfoParams screenInfo;
    screenInfo.isPortrait = 1;
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalSetMechPhoneStatusCmd> executionCmd = factory.CreateSetMechPhoneStatusCmd(screenInfo);
    ASSERT_NE(executionCmd, nullptr);

    // When: 执行序列化
    auto buffer = executionCmd->Marshal();

    // Then: 验证序列化结果
    ASSERT_NE(buffer, nullptr);
    EXPECT_GT(buffer->Size(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechPhoneStatusCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建设置手机状态命令对象
    ScreenInfoParams screenInfo;
    screenInfo.isPortrait = 1;

    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalSetMechPhoneStatusCmd> executionCmd = factory.CreateSetMechPhoneStatusCmd(screenInfo);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针异常处理
    executionCmd->TriggerResponse(nullptr);
    executionCmd->SetResponseCallback(ResponseCb);
    executionCmd->TriggerResponse(nullptr);

    // Then: 验证异常情况下结果未改变
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechPhoneStatusCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建设置手机状态命令对象并设置回调
    ScreenInfoParams screenInfo;
    screenInfo.isPortrait = 1;

    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechPhoneStatusCmd> executionCmd = factory.CreateSetMechPhoneStatusCmd(screenInfo);
    ASSERT_NE(executionCmd, nullptr);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x02);
    buffer->AppendUint8(0x2A);
    buffer->AppendUint8(1);

    // When: 设置回调并触发响应
    executionCmd->SetResponseCallback(ResponseCb);
    executionCmd->TriggerResponse(buffer);

    // Then: 验证结果
    EXPECT_EQ(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationTraceCmd_Constructor_001, TestSize.Level1)
{
    // Given: 准备旋转轨迹参数
    std::vector<RotateParam> paramsVector;
    RotateParam params;
    params.duration = 1000;
    params.degree.yaw = 1.1;
    params.degree.roll = 2.2;
    params.degree.pitch = 3.3;

    for (int i = 0; i < 11; i++) {
        paramsVector.push_back(params);
    }
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    // When: 创建旋转轨迹命令对象
    std::shared_ptr<CommonSetMechRotationTraceCmd> executionCmdCommon =
        factory.CreateSetMechRotationTraceCmd(++g_taskId, paramsVector);
    std::shared_ptr<NormalSetMechRotationTraceCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechRotationTraceCmd>(executionCmdCommon);

    // Then: 验证命令对象创建成功且参数正确
    ASSERT_NE(executionCmd, nullptr);
    auto cmdParams = executionCmd->GetParams();
    EXPECT_EQ(cmdParams.size(), 11);
    EXPECT_EQ(cmdParams[0].duration, 1000);
    EXPECT_FLOAT_EQ(cmdParams[0].degree.yaw, 1.1);
    EXPECT_FLOAT_EQ(cmdParams[0].degree.roll, 2.2);
    EXPECT_FLOAT_EQ(cmdParams[0].degree.pitch, 3.3);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationTraceCmd_TriggerResponse_001, TestSize.Level1)
{
    // Given: 创建旋转轨迹命令对象
    std::vector<RotateParam> paramsVector;
    RotateParam params;
    params.duration = 1000;
    params.degree.yaw = 1.1;
    params.degree.roll = 2.2;
    params.degree.pitch = 3.3;
    paramsVector.push_back(params);

    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonSetMechRotationTraceCmd> executionCmdCommon =
        factory.CreateSetMechRotationTraceCmd(++g_taskId, paramsVector);
    std::shared_ptr<NormalSetMechRotationTraceCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechRotationTraceCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    // When: 测试空指针异常处理
    executionCmd->TriggerResponse(nullptr);
    executionCmd->SetResponseCallback(ResponseCb);
    executionCmd->TriggerResponse(nullptr);

    // Then: 验证异常情况下结果未改变
    EXPECT_EQ(executionCmd->GetResult(), 0);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationTraceCmd_TriggerResponse_002, TestSize.Level1)
{
    // Given: 创建旋转轨迹命令对象
    std::vector<RotateParam> paramsVector;
    RotateParam params;
    params.duration = 1000;
    params.degree.yaw = 1.1;
    params.degree.roll = 2.2;
    params.degree.pitch = 3.3;
    paramsVector.push_back(params);

    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonSetMechRotationTraceCmd> executionCmdCommon =
        factory.CreateSetMechRotationTraceCmd(++g_taskId, paramsVector);
    std::shared_ptr<NormalSetMechRotationTraceCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechRotationTraceCmd>(executionCmdCommon);
    ASSERT_NE(executionCmd, nullptr);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    buffer->AppendUint8(0x02);
    buffer->AppendUint8(0x27);
    buffer->AppendUint8(1);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);

    // When: 设置回调并触发响应
    executionCmd->SetResponseCallback(ResponseCb);
    executionCmd->TriggerResponse(buffer);

    // Then: 验证结果
    EXPECT_NE(executionCmd->GetResult(), 1);
}
}
}