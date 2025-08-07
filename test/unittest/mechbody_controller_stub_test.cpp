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
#include "mechbody_controller_stub_test.h"
#include "bluetooth_errorcode.h"
#include "mechbody_controller_log.h"
#include "test_log.h"
#include <chrono>
#include <thread>
#include <atomic>

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS {
namespace MechBodyController {
namespace {
    MechBodyControllerService& mechBodyControllerService_ = MechBodyControllerService::GetInstance();
    uint8_t g_staticData[] = {1, 2, 3, 4};
    uint8_t *g_data = g_staticData;
    constexpr size_t DATA_SIZE = 1024;
}

void MechBodyControllerStubTest::SetUpTestCase()
{
    DTEST_LOG << "MechBodyControllerStubTest::SetUpTestCase" << std::endl;
}

void MechBodyControllerStubTest::TearDownTestCase()
{
    DTEST_LOG << "MechBodyControllerStubTest::TearDownTestCase" << std::endl;
}

void MechBodyControllerStubTest::TearDown()
{
    DTEST_LOG << "MechBodyControllerStubTest::TearDown" << std::endl;
}

void MechBodyControllerStubTest::SetUp()
{
    DTEST_LOG << "MechBodyControllerStubTest::SetUp" << std::endl;
}

/**
 * @tc.name  : AttachStateChangeListenOnInner_001
 * @tc.number: AttachStateChangeListenOnInner_001
 * @tc.desc  : Test AttachStateChangeListenOnInner function.
 */
HWTEST_F(MechBodyControllerStubTest, AttachStateChangeListenOnInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest AttachStateChangeListenOnInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_ON);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest AttachStateChangeListenOnInner_001 end" << std::endl;
}

/**
 * @tc.name  : AttachStateChangeListenOffInner_001
 * @tc.number: AttachStateChangeListenOffInner_001
 * @tc.desc  : Test AttachStateChangeListenOffInner function.
 */
HWTEST_F(MechBodyControllerStubTest, AttachStateChangeListenOffInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest AttachStateChangeListenOffInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_OFF);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest AttachStateChangeListenOffInner_001 end" << std::endl;
}

/**
 * @tc.name  : GetAttachedDevicesInnerr_001
 * @tc.number: GetAttachedDevicesInner_001
 * @tc.desc  : Test GetAttachedDevicesInner function.
 */
HWTEST_F(MechBodyControllerStubTest, GetAttachedDevicesInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest GetAttachedDevicesInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_ATTACHED_DEVICES);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest GetAttachedDevicesInner_001 end" << std::endl;
}

/**
 * @tc.name  : SetUserOperationInner_001
 * @tc.number: SetUserOperationInner_001
 * @tc.desc  : Test SetUserOperationInner function.
 */
HWTEST_F(MechBodyControllerStubTest, SetUserOperationInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest SetUserOperationInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::SET_USER_OPERATION);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest SetUserOperationInner_001 end" << std::endl;
}

/**
 * @tc.name  : SetCameraTrackingEnabledInner_001
 * @tc.number: SetCameraTrackingEnabledInner_001
 * @tc.desc  : Test SetCameraTrackingEnabledInner function.
 */
HWTEST_F(MechBodyControllerStubTest, SetCameraTrackingEnabledInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest SetCameraTrackingEnabledInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::SET_CAMERA_TRACKING_ENABLED);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest SetCameraTrackingEnabledInner_001 end" << std::endl;
}

/**
 * @tc.name  : GetCameraTrackingEnabledInner_001
 * @tc.number: GetCameraTrackingEnabledInner_001
 * @tc.desc  : Test GetCameraTrackingEnabledInner function.
 */
HWTEST_F(MechBodyControllerStubTest, GetCameraTrackingEnabledInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest GetCameraTrackingEnabledInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_CAMERA_TRACKING_ENABLED);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest GetCameraTrackingEnabledInner_001 end" << std::endl;
}

/**
 * @tc.name  : TrackingEventListenOnInner_001
 * @tc.number: TrackingEventListenOnInner_001
 * @tc.desc  : Test TrackingEventListenOnInner function.
 */
HWTEST_F(MechBodyControllerStubTest, TrackingEventListenOnInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest TrackingEventListenOnInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::TRACKING_EVENT_LISTEN_ON);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest TrackingEventListenOnInner_001 end" << std::endl;
}

/**
 * @tc.name  : TrackingEventListenOffInner_001
 * @tc.number: TrackingEventListenOffInner_001
 * @tc.desc  : Test TrackingEventListenOffInner function.
 */
HWTEST_F(MechBodyControllerStubTest, TrackingEventListenOffInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest TrackingEventListenOffInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::TRACKING_EVENT_LISTEN_OFF);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest TrackingEventListenOffInner_001 end" << std::endl;
}

/**
 * @tc.name  : SetCameraTrackingLayoutInner_001
 * @tc.number: SetCameraTrackingLayoutInner_001
 * @tc.desc  : Test SetCameraTrackingLayoutInner function.
 */
HWTEST_F(MechBodyControllerStubTest, SetCameraTrackingLayoutInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest SetCameraTrackingLayoutInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::SET_CAMERA_TRACKING_LAYOUT);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest SetCameraTrackingLayoutInner_001 end" << std::endl;
}

/**
 * @tc.name  : GetCameraTrackingLayoutInner_001
 * @tc.number: GetCameraTrackingLayoutInner_001
 * @tc.desc  : Test GetCameraTrackingLayoutInner function.
 */
HWTEST_F(MechBodyControllerStubTest, GetCameraTrackingLayoutInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest GetCameraTrackingLayoutInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_CAMERA_TRACKING_LAYOUT);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest GetCameraTrackingLayoutInner_001 end" << std::endl;
}

/**
 * @tc.name  : RegisterCmdChannelInner_001
 * @tc.number: RegisterCmdChannelInner_001
 * @tc.desc  : Test RegisterCmdChannelInner function.
 */
HWTEST_F(MechBodyControllerStubTest, RegisterCmdChannelInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest RegisterCmdChannelInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::REGISTER_CMD_CHANNEL);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest RegisterCmdChannelInner_001 end" << std::endl;
}

/**
 * @tc.name  : RotateByDegreeInner_001
 * @tc.number: RotateByDegreeInner_001
 * @tc.desc  : Test RotateByDegreeInner function.
 */
HWTEST_F(MechBodyControllerStubTest, RotateByDegreeInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest RotateByDegreeInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ROTATE_BY_DEGREE);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest RotateByDegreeInner_001 end" << std::endl;
}

/**
 * @tc.name  : GetMaxRotationTimeInner_001
 * @tc.number: GetMaxRotationTimeInner_001
 * @tc.desc  : Test GetMaxRotationTimeInner function.
 */
HWTEST_F(MechBodyControllerStubTest, GetMaxRotationTimeInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest GetMaxRotationTimeInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_MAX_ROTATION_TIME);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest GetMaxRotationTimeInner_001 end" << std::endl;
}

/**
 * @tc.name  : GetMaxRotationSpeedInner_001
 * @tc.number: GetMaxRotationSpeedInner_001
 * @tc.desc  : Test GetMaxRotationSpeedInner function.
 */
HWTEST_F(MechBodyControllerStubTest, GetMaxRotationSpeedInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest GetMaxRotationSpeedInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_MAX_ROTATION_SPEED);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest GetMaxRotationSpeedInner_001 end" << std::endl;
}

/**
 * @tc.name  : RotateBySpeedInner_001
 * @tc.number: RotateBySpeedInner_001
 * @tc.desc  : Test RotateBySpeedInner function.
 */
HWTEST_F(MechBodyControllerStubTest, RotateBySpeedInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest RotateBySpeedInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ROTATE_BY_SPEED);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest RotateBySpeedInner_001 end" << std::endl;
}

/**
 * @tc.name  : StopMovingInner_001
 * @tc.number: StopMovingInner_001
 * @tc.desc  : Test StopMovingInner function.
 */
HWTEST_F(MechBodyControllerStubTest, StopMovingInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest StopMovingInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::STOP_MOVING);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest StopMovingInner_001 end" << std::endl;
}

/**
 * @tc.name  : GetRotationAnglesInner_001
 * @tc.number: GetRotationAnglesInner_001
 * @tc.desc  : Test GetRotationAnglesInner function.
 */
HWTEST_F(MechBodyControllerStubTest, GetRotationAnglesInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest GetRotationAnglesInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_ROTATION_ANGLES);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest GetRotationAnglesInner_001 end" << std::endl;
}

/**
 * @tc.name  : GetRotationDegreeLimitsInner_001
 * @tc.number: GetRotationDegreeLimitsInner_001
 * @tc.desc  : Test GetRotationDegreeLimitsInner function.
 */
HWTEST_F(MechBodyControllerStubTest, GetRotationDegreeLimitsInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest GetRotationDegreeLimitsInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_ROTATION_DEGREE_LIMITS);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest GetRotationDegreeLimitsInner_001 end" << std::endl;
}

/**
 * @tc.name  : GetRotationAxesStatusInner_001
 * @tc.number: GetRotationAxesStatusInner_001
 * @tc.desc  : Test GetRotationAxesStatusInner function.
 */
HWTEST_F(MechBodyControllerStubTest, GetRotationAxesStatusInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest GetRotationAxesStatusInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_ROTATION_AXES_STATUS);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest GetRotationAxesStatusInner_001 end" << std::endl;
}

/**
 * @tc.name  : RotationAxesStatusChangeListenOnInner_001
 * @tc.number: RotationAxesStatusChangeListenOnInner_001
 * @tc.desc  : Test RotationAxesStatusChangeListenOnInner function.
 */
HWTEST_F(MechBodyControllerStubTest, RotationAxesStatusChangeListenOnInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest RotationAxesStatusChangeListenOnInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ROTATION_AXES_STATUS_CHANGE_LISTEN_ON);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest RotationAxesStatusChangeListenOnInner_001 end" << std::endl;
}

/**
 * @tc.name  : RotationAxesStatusChangeListenOffInner_001
 * @tc.number: RotationAxesStatusChangeListenOffInner_001
 * @tc.desc  : Test RotationAxesStatusChangeListenOffInner function.
 */
HWTEST_F(MechBodyControllerStubTest, RotationAxesStatusChangeListenOffInner_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest RotationAxesStatusChangeListenOffInner_001 begin" << std::endl;

    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ROTATION_AXES_STATUS_CHANGE_LISTEN_OFF);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);

    datas.WriteBuffer(g_data, DATA_SIZE);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    int32_t ret = mechBodyControllerService_.OnRemoteRequest(code, datas, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerStubTest RotationAxesStatusChangeListenOffInner_001 end" << std::endl;
}
}
}
