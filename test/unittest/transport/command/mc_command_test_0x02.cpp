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
    
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationToLocationCmd_Marshal_001, TestSize.Level1)
{
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
    
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationToLocationCmd_TriggerResponse_001, TestSize.Level1)
{
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
    EXPECT_NE(executionCmd, nullptr);

    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationToLocationCmd_TriggerResponse_002, TestSize.Level1)
{
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
    EXPECT_NE(executionCmd, nullptr);

    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(100);
        buffer->AppendUint16(1);
        buffer->AppendUint16(0b00000000);

        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().yawLimited, RotationAxisLimited::NOT_LIMITED);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(100);
        buffer->AppendUint16(1);
        buffer->AppendUint16(0b00000001);

        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().yawLimited, RotationAxisLimited::POS_LIMITED);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(100);
        buffer->AppendUint16(1);
        buffer->AppendUint16(0b00000010);

        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().yawLimited, RotationAxisLimited::NEG_LIMITED);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(100);
        buffer->AppendUint16(1);
        buffer->AppendUint16(0b00000100);

        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().rollLimited, RotationAxisLimited::POS_LIMITED);
    }
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationToLocationCmd_TriggerResponse_002_001, TestSize.Level1)
{
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
    EXPECT_NE(executionCmd, nullptr);
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(100);
        buffer->AppendUint16(1);
        buffer->AppendUint16(0b00001000);

        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().rollLimited, RotationAxisLimited::NEG_LIMITED);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(100);
        buffer->AppendUint16(1);
        buffer->AppendUint16(0b00010000);

        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().pitchLimited, RotationAxisLimited::POS_LIMITED);
    }
    executionCmd->SetResponseCallback(ResponseCb)j
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint16(100);
        buffer->AppendUint16(1);
        buffer->AppendUint16(0b00100000);

        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().pitchLimited, RotationAxisLimited::POS_LIMITED);
    }
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationBySpeedCmd_Marshal_001, TestSize.Level1)
{
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
    
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationBySpeedCmd_TriggerResponse_001, TestSize.Level1)
{
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
    EXPECT_NE(executionCmd, nullptr);

    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationBySpeedCmd_TriggerResponse_002, TestSize.Level1)
{
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
    EXPECT_NE(executionCmd, nullptr);

    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer.AppendUint16(++g_taskId);
        buffer.AppendUint8(1);
        buffer.AppendUint8(0b00000000);

        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().yawLimited, RatationAxisLimited::NOT_LIMITED);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer.AppendUint16(++g_taskId);
        buffer.AppendUint8(1);
        buffer.AppendUint8(0b00000001);

        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().yawLimited, RatationAxisLimited::POS_LIMITED);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer.AppendUint16(++g_taskId);
        buffer.AppendUint8(1);
        buffer.AppendUint8(0b00000010);

        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().yawLimited, RatationAxisLimited::NEG_LIMITED);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer.AppendUint16(++g_taskId);
        buffer.AppendUint8(1);
        buffer.AppendUint8(0b00000100);

        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().rollLimited, RatationAxisLimited::POS_LIMITED);
    }
}

HWTEST_F(MechCommandTest0x02, NormalSetMechRotationBySpeedCmd_TriggerResponse_002_001, TestSize.Level1)
{
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
    EXPECT_NE(executionCmd, nullptr);
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer.AppendUint16(++g_taskId);
        buffer.AppendUint8(1);
        buffer.AppendUint8(0b00001000);

        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().rollLimited, RatationAxisLimited::NEG_LIMITED);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer.AppendUint16(++g_taskId);
        buffer.AppendUint8(1);
        buffer.AppendUint8(0b00010000);

        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().pitchLimited, RatationAxisLimited::POS_LIMITED);
    }
    executionCmd->SetResponseCallback(ResponseCb);
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);
        buffer.AppendUint16(++g_taskId);
        buffer.AppendUint8(1);
        buffer.AppendUint8(0b00100000);

        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->GetRotationAxesStatus().pitchLimited, RatationAxisLimited::NEG_LIMITED);
    }
}

HWTEST_F(MechCommandTest0x02, NormalSetMechProtocolVerCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechProtocolVerCmd> executionCmd =
        factory.CreateNormalSetMechProtocolVerCmd(10);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechProtocolVerCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechProtocolVerCmd> executionCmd =
        factory.CreateNormalSetMechProtocolVerCmd(10);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    executionCmd->SetResponseCallback(ResponseCb);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
}

HWTEST_F(MechCommandTest0x02, NormalSetMechMotionControlCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechMotionControlCmd> executionCmd =
        factory.CreateSetMechMotionControlCmd(ControlCommand::STOP);
    EXPECT_NE(executionCmd, nullptr);
    executionCmd->GetAction();
    executionCmd->GetResult();
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechMotionControlCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechMotionControlCmd> executionCmd =
        factory.CreateSetMechMotionControlCmd(ControlCommand::STOP);
    EXPECT_NE(executionCmd, nullptr);
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    bffer->AppendUint8(1);

    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    EXPECT_EQ(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechMotionControlCmd_TriggerResponse_002, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechMotionControlCmd> executionCmd =
        factory.CreateSetMechMotionControlCmd(ControlCommand::STOP);
    EXPECT_NE(executionCmd, nullptr);
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    bffer->AppendUint8(1);

    EXPECT_NE(executionCmd, nullptr);
    executionCmd->SetResponseCallback(ResponseCb);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    EXPECT_EQ(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechLocationReportCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechLocationReportCmd> executionCmd =
        factory.CreateSetMechLocationReportCmd(10, 20);
    EXPECT_NE(executionCmd, nullptr);
    executionCmd->GetResult();
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechLocationReportCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechLocationReportCmd> executionCmd =
        factory.CreateSetMechLocationReportCmd(10, 20);
    EXPECT_NE(executionCmd, nullptr);

    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);

    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    EXPECT_NE(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechLocationReportCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<NormalSetMechLocationReportCmd> executionCmd =
        factory.CreateSetMechLocationReportCmd(10, 20);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);

    EXPECT_NE(executionCmd, nullptr);
    executionCmd->SetResponseCallback(ResponseCb);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    EXPECT_NE(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechCameraTrackingFrameCmd_Marshal_001, TestSize.Level1)
{
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
    EXPECT_NE(executionCmd, nullptr);
    executionCmd->GetResult();
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechCameraTrackingFrameCmd_TriggerResponse_001, TestSize.Level1)
{
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
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    executionCmd->SetResponseCallback(ResponseCb);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
}

HWTEST_F(MechCommandTest0x02, NormalSetMechCameraTrackingEnableCmd_Marshal_001, TestSize.Level1)
{
    MechTrackingStatus status = MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET;

    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonSetMechCameraTrackingEnableCmd> executionCmdCommon =
        factory.CreateSetMechCameraTrCreateSetMechCameraTrackingEnableCmdackingFrameCmd(status);
    std::shared_ptr<NormalSetMechCameraTrackingEnableCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechCameraTrackingEnableCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);
    executionCmd->GetStatus();
    executionCmd->GetResult();
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechCameraTrackingEnableCmd_TriggerResponse_001, TestSize.Level1)
{
    MechTrackingStatus status = MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET;

    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonSetMechCameraTrackingEnableCmd> executionCmdCommon =
        factory.CreateSetMechCameraTrCreateSetMechCameraTrackingEnableCmdackingFrameCmd(status);
    std::shared_ptr<NormalSetMechCameraTrackingEnableCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechCameraTrackingEnableCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    EXPECT_EQ(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechCameraTrackingEnableCmd_TriggerResponse_002, TestSize.Level1)
{
    MechTrackingStatus status = MechTrackingStatus::MECH_TK_ENABLE_NO_TARGET;

    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonSetMechCameraTrackingEnableCmd> executionCmdCommon =
        factory.CreateSetMechCameraTrCreateSetMechCameraTrackingEnableCmdackingFrameCmd(status);
    std::shared_ptr<NormalSetMechCameraTrackingEnableCmd> executionCmd =
        std::static_pointer_cast<NormalSetMechCameraTrackingEnableCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);
    EXPECT_NE(executionCmd, nullptr);
    executionCmd->SetResponseCallback(ResponseCb);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    EXPECT_EQ(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechCameraInfoCmd_Marshal_001, TestSize.Level1)
{
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

    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalSetMechCameraInfoCmd_TriggerResponse_001, TestSize.Level1)
{
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

    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    executionCmd->SetResponseCallback(ResponseCb);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechTrackingEnableCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechTrackingEnableCmd> executionCmdCommon =
        factory.CreateRegisterMechTrackingEnableCmd();
    std::shared_ptr<NormalRegisterMechTrackingEnableCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechTrackingEnableCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechTrackingEnableCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechTrackingEnableCmd> executionCmdCommon =
        factory.CreateRegisterMechTrackingEnableCmd();
    std::shared_ptr<NormalRegisterMechTrackingEnableCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechTrackingEnableCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    size_t capacity = 100;
    auto bufferEmpty = std::make_shared<MechDataBuffer>(capacity);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    executionCmd->SetResponseCallback(ResponseCb);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        buffer->AppendUint8(10);
        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->result_, 10);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        buffer->AppendUint8(10);
        executionCmd->SetResponseCallback(ResponseCb);
        EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
        EXPECT_EQ(executionCmd->result_, 10);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechTrackingEnableCmd_Unmarshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechTrackingEnableCmd> executionCmdCommon =
        factory.CreateRegisterMechTrackingEnableCmd(params);
    std::shared_ptr<NormalRegisterMechTrackingEnableCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechTrackingEnableCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_EQ(executionCmd->Unmarshal(nullptr), false);

    size_t capacity = 100;
    auto bufferEmpty = std::make_shared<MechDataBuffer>(capacity);
    EXPECT_NE(executionCmd->Unmarshal(bufferEmpty), false);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);
    EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
    EXPECT_EQ(executionCmd->isEnabled_, true);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechStateInfoCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechStateInfoCmd> executionCmdCommon = factory.CreateRegisterMechStateInfoCmd();
    std::shared_ptr<NormalRegisterMechStateInfoCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechStateInfoCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechStateInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechStateInfoCmd> executionCmdCommon = factory.CreateRegisterMechStateInfoCmd();
    std::shared_ptr<NormalRegisterMechStateInfoCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechStateInfoCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    executionCmd->SetResponseCallback(ResponseCb);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechStateInfoCmd_Unmarshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechStateInfoCmd> executionCmdCommon = factory.CreateRegisterMechStateInfoCmd();
    std::shared_ptr<NormalRegisterMechStateInfoCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechStateInfoCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_EQ(executionCmd->Unmarshal(nullptr), false);

    size_t capacity = 100;
    auto bufferEmpty = std::make_shared<MechDataBuffer>(capacity);
    EXPECT_EQ(executionCmd->Unmarshal(bufferEmpty), false);
    
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);
    buffer->AppendUint8(1);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);
    EXPECT_EQ(executionCmd->Unmarshal(bufferEmpty), true);
    EXPECT_EQ(executionCmd->info.isPhoneOn, true);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechPositionInfoCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechPositionInfoCmd> executionCmdCommon = factory.CreateRegisterMechPositionInfoCmd();
    std::shared_ptr<NormalRegisterMechPositionInfoCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechPositionInfoCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechPositionInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonRegisterMechPositionInfoCmd> executionCmdCommon = factory.CreateRegisterMechPositionInfoCmd();
    std::shared_ptr<NormalRegisterMechPositionInfoCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechPositionInfoCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    executionCmd->SetResponseCallback(ResponseCb);
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(8);
    executionCmd->TriggerResponse(buffer)
    EXPECT_EQ(executionCmd->result_, 0);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechPositionInfoCmd_Unmarshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonRegisterMechPositionInfoCmd> executionCmdCommon = factory.CreateRegisterMechPositionInfoCmd();
    std::shared_ptr<NormalRegisterMechPositionInfoCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechPositionInfoCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_EQ(executionCmd->Unmarshal(nullptr), false);

    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_EQ(executionCmd->Unmarshal(bufferEmpty), false);


    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(8);
    buffer->AppendUint8(1.11);
    buffer->AppendUint8(2.22);
    buffer->AppendUint8(3.33);
    EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
    EXPECT_GT(executionCmd->position_.pitch, 3);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    executionCmd->SetResponseCallback(ResponseCb);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);

    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->Unmarshal(nullptr));
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_NO_FATAL_FAILURE(executionCmd->Unmarshal(bufferEmpty));
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_002, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);

        buffer->AppendUint8(1);
        buffer->AppendUint8(2);
        buffer->AppendUint8(3);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }

    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);

        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        buffer->AppendUint8(2);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
        EXPECT_EQ(executionCmd->buttonFrequency_, true);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);

        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);

        buffer->AppendUint8(2);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
        EXPECT_EQ(executionCmd->event_, CameraKeyEvent::SWITCH_TRACKING);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_002_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);

    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);

        buffer->AppendUint8(3);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
        EXPECT_EQ(executionCmd->event_, CameraKeyEvent::ZOOM_IN);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);

        buffer->AppendUint8(4);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
        EXPECT_EQ(executionCmd->event_, CameraKeyEvent::ZOOM_OUT);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);

        buffer->AppendUint8(5);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
        EXPECT_EQ(executionCmd->event_, CameraKeyEvent::SWITCH_CAMERA);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);

        buffer->AppendUint8(6);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
        EXPECT_EQ(executionCmd->event_, CameraKeyEvent::START_FILMING);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_003, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);

        buffer->AppendUint8(7);
        buffer->AppendUint8(4);
        buffer->AppendUint8(3);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);

        buffer->AppendUint8(7);
        buffer->AppendUint8(5);
        buffer->AppendUint16(3);
        buffer->AppendUint16(12);
        buffer->AppendUint8(4);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
        EXPECT_EQ(executionCmd->wheelData_speed, 12);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);

        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechKeyEventCmd_Unmarshal_003_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonRegisterMechKeyEventCmd> executionCmdCommon = factory.CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd =
        std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(executionCmdCommon);
    EXPECT_NE(executionCmd, nullptr);
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);

        buffer->AppendUint8(8);
        buffer->AppendUint8(4);
        buffer->AppendUint16(1);
        buffer->AppendUint16(2);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
        EXPECT_EQ(executionCmd->stickY_, 2);
    }
    {
        auto buffer = std::make_shared<MechDataBuffer>(100);
        AppendUint8BySize(buffer, 2);

        buffer->AppendUint8(8);
        buffer->AppendUint8(5);
        buffer->AppendUint16(1);
        buffer->AppendUint16(2);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();

    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));


    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    executionCmd->GetParams();
    EXPECT_NE(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_TriggerResponse_002, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    executionCmd->SetResponseCallback(ResponseCb);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    executionCmd->GetParams();
    EXPECT_NE(executionCmd->GetResult(), 1);
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_UnMarshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->Unmarshal(nullptr));

    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_NO_FATAL_FAILURE(executionCmd->Unmarshal(bufferEmpty));
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_UnMarshal_002, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->Unmarshal(nullptr));
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(0);
        buffer->AppendUint8(2);
        buffer->AppendUint8(1);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(0);
        buffer->AppendUint8(1);
        buffer->AppendUint8(10);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
        EXPECT_EQ(executionCmd->GetParams().attached, 10);
    }
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(1);
        buffer->AppendUint8(2);
        buffer->AppendUint8(10);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
}

HWTEST_F(MechCommandTest0x02, NormalRegisterMechGenericEventCmd_UnMarshal_002_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalRegisterMechGenericEventCmd> executionCmd = factory.CreateRegisterMechGenericEventCmd();
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->Unmarshal(nullptr));
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(1);
        buffer->AppendUint8(1);
        buffer->AppendUint8(7);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), true);
        EXPECT_EQ(executionCmd->GetParams().pitchDisable, 1);
    }
    {
        int capacity = 100;
        auto buffer = std::make_shared<MechDataBuffer>(capacity);
        AppendUint8BySize(buffer, 2);
        buffer->AppendUint8(2);
        buffer->AppendUint8(2);
        buffer->AppendUint8(7);
        EXPECT_EQ(executionCmd->Unmarshal(buffer), false);
    }
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
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechStateInfoCmd> executionCmd = factory.CreateGetMechStateInfoCmd();

    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechStateInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechStateInfoCmd> executionCmd = factory.CreateGetMechStateInfoCmd();
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));

    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(2);
    buffer->AppendUint8(255);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    executionCmd->GetResult();
    EXPECT_EQ(executionCmd->GetParams().pitchDisable, 1);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechStateInfoCmd_TriggerResponse_002, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechStateInfoCmd> executionCmd = factory.CreateGetMechStateInfoCmd();
    executionCmd->SetResponseCallback(ResponseCb);
    
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(2);
    buffer->AppendUint8(255);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    executionCmd->GetResult();
    EXPECT_EQ(executionCmd->GetParams().pitchDisable, 1);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechPoseInfoCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechPoseInfoCmd> executionCmd = factory.CreateGetMechPoseInfoCmd();

    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechPoseInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechPoseInfoCmd> executionCmd = factory.CreateGetMechPoseInfoCmd();
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 3);
    for (size_t i = 0; i < 3; i++) {
        buffer->AppendUint16(100);
    }

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    executionCmd->GetResult();
    EXPECT_EQ(executionCmd->GetParams().accelerationZ, 100);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechPoseInfoCmd_TriggerResponse_002, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechPoseInfoCmd> executionCmd = factory.CreateGetMechPoseInfoCmd();
    EXPECT_NE(executionCmd, nullptr);
    executionCmd->SetResponseCallback(ResponseCb);
    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 3);
    for (size_t i = 0; i < 3; i++) {
        buffer->AppendUint16(100);
    }

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    executionCmd->GetResult();
    EXPECT_EQ(executionCmd->GetParams().accelerationZ, 100);
}

HWTEST_F(MechCommandTest0x02, CommonGetMechLimitInfoCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonGetMechLimitInfoCmd> executionCmd = factory.CreateGetMechLimitInfoCmd();

    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, CommonGetMechLimitInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonGetMechLimitInfoCmd> executionCmd = factory.CreateGetMechLimitInfoCmd();
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 3);
    for (int i = 0; i < 6; i++) {
        buffer->AppendFloat(3.2434);
    }
    
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    executionCmd->GetResult();
    EXPECT_GT(executionCmd->GetParams().negMax.pitch, 3);
}


HWTEST_F(MechCommandTest0x02, CommonGetMechLimitInfoCmd_TriggerResponse_002, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<CommonGetMechLimitInfoCmd> executionCmd = factory.CreateGetMechLimitInfoCmd();
    EXPECT_NE(executionCmd, nullptr);

    executionCmd->SetResponseCallback(ResponseCb);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 3);
    for (int i = 0; i < 6; i++) {
        buffer->AppendFloat(3.2434);
    }

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    executionCmd->GetResult();
    EXPECT_GT(executionCmd->GetParams().negMax.pitch, 3);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechBaseInfoCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechBaseInfoCmd> executionCmd = factory.NormalGetMechBaseInfoCmd();

    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechBaseInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechBaseInfoCmd> executionCmd = factory.NormalGetMechBaseInfoCmd();
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);

    size_t macAddressSize = executionCmd->GetParams.macAddress.size();
    for (int i = 0; i < macAddressSize; i++) {
        buffer->AppendUint8(i + 1);
    }
    std::string deviceName = "111112222233333444445555566666";
    for (const char ch : deviceName) {
        buffer->AppendUint8(ch);
    }
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    EXPECT_EQ(executionCmd->GetParams().realName, deviceName);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechBaseInfoCmd_TriggerResponse_002, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechBaseInfoCmd> executionCmd = factory.NormalGetMechBaseInfoCmd();
    EXPECT_NE(executionCmd, nullptr);

    executionCmd->SetResponseCallback(ResponseCb);

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);
    buffer->AppendUint8(0);

    size_t macAddressSize = executionCmd->GetParams.macAddress.size();
    for (int i = 0; i < macAddressSize; i++) {
        buffer->AppendUint8(i + 1);
    }
    std::string deviceName = "111112222233333444445555566666";
    for (const char ch : deviceName) {
        buffer->AppendUint8(ch);
    }
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    EXPECT_EQ(executionCmd->GetParams().realName, deviceName);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechCapabilityInfoCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechCapabilityInfoCmd> executionCmd = factory.CreateGetMechCapabilityInfoCmd();

    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechCapabilityInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechCapabilityInfoCmd> executionCmd = factory.CreateGetMechCapabilityInfoCmd();

    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));

    auto buffer = std::make_shared<MechDataBuffer>(100);
    for (int i = 0; i < 9; i++) {
        buffer->AppendUint8(static_cast<uint8_t>(i));
    }
    for (int i = 0; i < 3; i++) {
        buffer->AppendFloat(2.12);
    }
    int maxturntime = 120;
    buffer->AppendUint16(maxturntime);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    executionCmd->GetResult();
    EXPECT_EQ(executionCmd->GetParams().maxturntime, maxturntime);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechCapabilityInfoCmd_TriggerResponse_002, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechCapabilityInfoCmd> executionCmd = factory.CreateGetMechCapabilityInfoCmd();

    EXPECT_NE(executionCmd, nullptr);

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

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    executionCmd->GetResult();
    EXPECT_EQ(executionCmd->GetParams().maxturntime, maxturntime);
}

HWTEST_F(MechCommandTest0x02, GetMechProtocolVerCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<GetMechProtocolVerCmd> executionCmd = factory.CreateGetMechProtocolVerCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, GetMechProtocolVerCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<GetMechProtocolVerCmd> executionCmd = factory.CreateGetMechProtocolVerCmd();
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);
    buffer->AppendUint8(2);
    buffer->AppendUint8(3);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    executionCmd->GetResult();
    executionCmd->GetParams();
    EXPECT_EQ(executionCmd->lowProtocoVer_, 3);
}

HWTEST_F(MechCommandTest0x02, GetMechProtocolVerCmd_TriggerResponse_002, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<GetMechProtocolVerCmd> executionCmd = factory.CreateGetMechProtocolVerCmd();
    EXPECT_NE(executionCmd, nullptr);

    executionCmd->SetResponseCallback(ResponseCb);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 2);
    buffer->AppendUint8(1);
    buffer->AppendUint8(2);
    buffer->AppendUint8(3);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    executionCmd->GetResult();
    executionCmd->GetParams();
    EXPECT_EQ(executionCmd->lowProtocoVer_, 3);
}


HWTEST_F(MechCommandTest0x02, NormalGetMechCoordinateInfoCmd_Marshal_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechCoordinateInfoCmd> executionCmd = factory.CreateGetMechCoordinateInfoCmd();
    EXPECT_NE(executionCmd, nullptr);
    EXPECT_NE(executionCmd->Marshal(), nullptr);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechCoordinateInfoCmd_TriggerResponse_001, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechCoordinateInfoCmd> executionCmd = factory.CreateGetMechCoordinateInfoCmd();
    EXPECT_NE(executionCmd, nullptr);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 4);
    for (int i = 0; i < 3; i++) {
        buffer->AppendFloat(5.10);
    }
    buffer->AppendUint8(1);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    executionCmd->GetResult();
    EXPECT_EQ(executionCmd->GetParams().pitchPose, 5);
}

HWTEST_F(MechCommandTest0x02, NormalGetMechCoordinateInfoCmd_TriggerResponse_002, TestSize.Level1)
{
    CommandFactory factory;
    factory.SetFactoryProtocolVer(0x02);
    std::shared_ptr<NormalGetMechCoordinateInfoCmd> executionCmd = factory.CreateGetMechCoordinateInfoCmd();
    EXPECT_NE(executionCmd, nullptr);
    executionCmd->SetResponseCallback(ResponseCb);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(nullptr));
    
    auto bufferEmpty = std::make_shared<MechDataBuffer>(100);
    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(bufferEmpty));

    auto buffer = std::make_shared<MechDataBuffer>(100);
    AppendUint8BySize(buffer, 4);
    for (int i = 0; i < 3; i++) {
        buffer->AppendFloat(5.10);
    }
    buffer->AppendUint8(1);

    EXPECT_NO_FATAL_FAILURE(executionCmd->TriggerResponse(buffer));
    executionCmd->GetResult();
    EXPECT_EQ(executionCmd->GetParams().pitchPose, 5);
}
}
}