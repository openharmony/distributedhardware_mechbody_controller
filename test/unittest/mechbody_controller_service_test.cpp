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
#include <chrono>
#include <thread>
#include <atomic>

#include "mechbody_controller_service_test.h"
#include "mc_motion_manager.h"
#include "bluetooth_errorcode.h"
#include "mechbody_controller_log.h"
#include "test_log.h"
#include "tokenid_kit.h"
#include "ipc_skeleton.h"
#include "securec.h"
#include "accesstoken_kit.h"
#include "token_setproc.h"
#include "tokenid_kit.h"
#include "system_ability.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace {
    constexpr int32_t MECHID = 1;
    constexpr int32_t REVERTMECHID = -1;
    constexpr int32_t DURATION = 1;
    constexpr int32_t REVERTDURATION = -1;
    constexpr float YAW = 1;
    constexpr float ROLL = 1;
    constexpr float PITCH = 1;
    bool g_isSystemApp = true;
}

namespace OHOS {
namespace Security {
namespace AccessToken {
bool TokenIdKit::IsSystemAppByFullTokenID(uint64_t tokenId)
{
    return g_isSystemApp;
}
}  // namespace AccessToken
}  // namespace Security
}  // namespace OHOS

namespace OHOS {
namespace MechBodyController {
void MechBodyControllerServiceTest::SetUpTestCase()
{
    DTEST_LOG << "MechBodyControllerServiceTest::SetUpTestCase" << std::endl;
}

void MechBodyControllerServiceTest::TearDownTestCase()
{
    DTEST_LOG << "MechBodyControllerServiceTest::TearDownTestCase" << std::endl;
}

void MechBodyControllerServiceTest::TearDown()
{
    DTEST_LOG << "MechBodyControllerServiceTest::TearDown" << std::endl;
}

void MechBodyControllerServiceTest::SetUp()
{
    DTEST_LOG << "MechBodyControllerServiceTest::SetUp" << std::endl;
}

/**
 * @tc.name  : OnStart_001
 * @tc.number: OnStart_001
 * @tc.desc  : Test OnStart function.
 */
HWTEST_F(MechBodyControllerServiceTest, OnStart_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest OnStart_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.OnStart();
    EXPECT_NE(mechBodyControllerService.sendAdapter_, nullptr);
    DTEST_LOG << "MechBodyControllerServiceTest OnStart_001 end" << std::endl;
}

/**
 * @tc.name  : OnStop_001
 * @tc.number: OnStop_001
 * @tc.desc  : Test OnStop function.
 */
HWTEST_F(MechBodyControllerServiceTest, OnStop_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest OnStop_001 begin" << std::endl;
    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    EXPECT_NO_FATAL_FAILURE(mechBodyControllerService.OnStop());
    DTEST_LOG << "MechBodyControllerServiceTest OnStop_001 end" << std::endl;
}

/**
 * @tc.name  : RegisterAttachStateChangeCallback_001
 * @tc.number: RegisterAttachStateChangeCallback_001
 * @tc.desc  : Test RegisterAttachStateChangeCallback function when callback is nullptr.
 */
HWTEST_F(MechBodyControllerServiceTest, RegisterAttachStateChangeCallback_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest RegisterAttachStateChangeCallback_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    const sptr<IRemoteObject> listener;
    int32_t ret = mechBodyControllerService.RegisterAttachStateChangeCallback(listener);
    EXPECT_EQ(ret, INVALID_REMOTE_OBJECT);
    DTEST_LOG << "MechBodyControllerServiceTest RegisterAttachStateChangeCallback_001 end" << std::endl;
}

/**
 * @tc.name  : UnRegisterAttachStateChangeCallback_001
 * @tc.number: UnRegisterAttachStateChangeCallback_001
 * @tc.desc  : Test UnRegisterAttachStateChangeCallback function.
 */
HWTEST_F(MechBodyControllerServiceTest, UnRegisterAttachStateChangeCallback_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest UnRegisterAttachStateChangeCallback_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    sptr<IRemoteObject> listener;
    mechBodyControllerService.deviceAttachCallback_[0] = listener;
    int32_t ret = mechBodyControllerService.UnRegisterAttachStateChangeCallback();
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerServiceTest UnRegisterAttachStateChangeCallback_001 end" << std::endl;
}

/**
 * @tc.name  : UnRegisterAttachStateChangeCallback_002
 * @tc.number: UnRegisterAttachStateChangeCallback_002
 * @tc.desc  : Test UnRegisterAttachStateChangeCallback function.
 */
HWTEST_F(MechBodyControllerServiceTest, UnRegisterAttachStateChangeCallback_002, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest UnRegisterAttachStateChangeCallback_002 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    sptr<IRemoteObject> listener;
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    mechBodyControllerService.deviceAttachCallback_.clear();
    mechBodyControllerService.deviceAttachCallback_[tokenId] = listener;
    mechBodyControllerService.listener_ = nullptr;
    int32_t ret = mechBodyControllerService.UnRegisterAttachStateChangeCallback();
    EXPECT_EQ(ret, UNREGISTER_CALLBACK_FAILED);
    DTEST_LOG << "MechBodyControllerServiceTest UnRegisterAttachStateChangeCallback_002 end" << std::endl;
}

/**
 * @tc.name  : OnAttachStateChange_001
 * @tc.number: OnAttachStateChange_001
 * @tc.desc  : Test OnAttachStateChange function.
 */
HWTEST_F(MechBodyControllerServiceTest, OnAttachStateChange_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest OnAttachStateChange_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    AttachmentState attachmentState = AttachmentState::DETACHED;
    MechInfo mechInfo;
    mechInfo.mechName = "mechName";
    mechInfo.state = AttachmentState::ATTACHED;
    int32_t ret = mechBodyControllerService.OnAttachStateChange(attachmentState, mechInfo);
    EXPECT_EQ(ret, SEND_CALLBACK_INFO_FAILED);
    DTEST_LOG << "MechBodyControllerServiceTest OnAttachStateChange_001 end" << std::endl;
}

/**
 * @tc.name  : OnDeviceConnected_001
 * @tc.number: OnDeviceConnected_001
 * @tc.desc  : Test OnDeviceConnected function.
 */
HWTEST_F(MechBodyControllerServiceTest, OnDeviceConnected_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest OnDeviceConnected_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    int32_t mechId = MECHID;
    int32_t ret = mechBodyControllerService.OnDeviceConnected(mechId);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerServiceTest OnDeviceConnected_001 end" << std::endl;
}

/**
 * @tc.name  : OnDeviceDisconnected_001
 * @tc.number: OnDeviceDisconnected_001
 * @tc.desc  : Test OnDeviceDisconnected function.
 */
HWTEST_F(MechBodyControllerServiceTest, OnDeviceDisconnected_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest OnDeviceDisconnected_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    int32_t mechId = MECHID;
    int32_t ret = mechBodyControllerService.OnDeviceDisconnected(mechId);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerServiceTest OnDeviceDisconnected_001 end" << std::endl;
}

/**
 * @tc.name  : OnDeviceDisconnected_002
 * @tc.number: OnDeviceDisconnected_002
 * @tc.desc  : Test OnDeviceDisconnected function.
 */
HWTEST_F(MechBodyControllerServiceTest, OnDeviceDisconnected_002, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest OnDeviceDisconnected_002 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    int32_t mechId = MECHID;
    mechBodyControllerService.motionManagers_.clear();
    int32_t ret = mechBodyControllerService.OnDeviceDisconnected(mechId);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerServiceTest OnDeviceDisconnected_002 end" << std::endl;
}

/**
 * @tc.name  : GetAttachedDevices_001
 * @tc.number: GetAttachedDevices_001
 * @tc.desc  : Test GetAttachedDevices function.
 */
HWTEST_F(MechBodyControllerServiceTest, GetAttachedDevices_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest GetAttachedDevices_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    std::set<MechInfo> mechInfo;
    int32_t ret = mechBodyControllerService.GetAttachedDevices(mechInfo);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerServiceTest GetAttachedDevices_001 end" << std::endl;
}

/**
 * @tc.name  : SetTrackingEnabled_001
 * @tc.number: SetTrackingEnabled_001
 * @tc.desc  : Test SetTrackingEnabled function.
 */
HWTEST_F(MechBodyControllerServiceTest, SetTrackingEnabled_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest SetTrackingEnabled_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    bool isEnabled = true;
    int32_t ret = mechBodyControllerService.SetTrackingEnabled(isEnabled);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerServiceTest SetTrackingEnabled_001 end" << std::endl;
}

/**
 * @tc.name  : GetTrackingEnabled_001
 * @tc.number: GetTrackingEnabled_001
 * @tc.desc  : Test GetTrackingEnabled function.
 */
HWTEST_F(MechBodyControllerServiceTest, GetTrackingEnabled_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest GetTrackingEnabled_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    bool isEnabled = true;
    int32_t ret = mechBodyControllerService.GetTrackingEnabled(isEnabled);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerServiceTest GetTrackingEnabled_001 end" << std::endl;
}

/**
 * @tc.name  : RegisterTrackingEventCallback_001
 * @tc.number: RegisterTrackingEventCallback_001
 * @tc.desc  : Test RegisterTrackingEventCallback function when callback is nullptr.
 */
HWTEST_F(MechBodyControllerServiceTest, RegisterTrackingEventCallback_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest RegisterTrackingEventCallback_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    sptr<IRemoteObject> callback;
    int32_t ret = mechBodyControllerService.RegisterTrackingEventCallback(callback);
    EXPECT_EQ(ret, INVALID_REMOTE_OBJECT);
    DTEST_LOG << "MechBodyControllerServiceTest RegisterTrackingEventCallback_001 end" << std::endl;
}

/**
 * @tc.name  : UnRegisterTrackingEventCallback_001
 * @tc.number: UnRegisterTrackingEventCallback_001
 * @tc.desc  : Test UnRegisterTrackingEventCallback function.
 */
HWTEST_F(MechBodyControllerServiceTest, UnRegisterTrackingEventCallback_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest UnRegisterTrackingEventCallback_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    int32_t ret = mechBodyControllerService.UnRegisterTrackingEventCallback();
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerServiceTest UnRegisterTrackingEventCallback_001 end" << std::endl;
}

/**
 * @tc.name  : SetTrackingLayout_001
 * @tc.number: SetTrackingLayout_001
 * @tc.desc  : Test SetTrackingLayout function.
 */
HWTEST_F(MechBodyControllerServiceTest, SetTrackingLayout_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest SetTrackingLayout_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    CameraTrackingLayout cameraTrackingLayout = CameraTrackingLayout::DEFAULT;
    g_isSystemApp = false;
    int32_t ret = mechBodyControllerService.SetTrackingLayout(cameraTrackingLayout);
    EXPECT_EQ(ret, PERMISSION_DENIED);

    g_isSystemApp = true;
    ret = mechBodyControllerService.SetTrackingLayout(cameraTrackingLayout);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerServiceTest SetTrackingLayout_001 end" << std::endl;
}

/**
 * @tc.name  : GetTrackingLayout_001
 * @tc.number: GetTrackingLayout_001
 * @tc.desc  : Test GetTrackingLayout function.
 */
HWTEST_F(MechBodyControllerServiceTest, GetTrackingLayout_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest GetTrackingLayout_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    CameraTrackingLayout cameraTrackingLayout = CameraTrackingLayout::DEFAULT;
    int32_t ret = mechBodyControllerService.GetTrackingLayout(cameraTrackingLayout);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerServiceTest GetTrackingLayout_001 end" << std::endl;
}

/**
 * @tc.name  : RotateByDegree_001
 * @tc.number: RotateByDegree_001
 * @tc.desc  : Test RotateByDegree function.
 */
HWTEST_F(MechBodyControllerServiceTest, RotateByDegree_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest RotateByDegree_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    int32_t mechId = REVERTMECHID;
    std::string cmdId = "cmdId";
    RotateByDegreeParam rotateParam;
    rotateParam.duration = REVERTDURATION;
    rotateParam.degree.yaw = YAW;
    rotateParam.degree.roll = ROLL;
    rotateParam.degree.pitch = PITCH;
    auto rotateByDegreeParam = std::make_shared<RotateByDegreeParam>(rotateParam);
    g_isSystemApp = false;
    int32_t ret = mechBodyControllerService.RotateByDegree(mechId, cmdId, rotateByDegreeParam);
    EXPECT_EQ(ret, PERMISSION_DENIED);

    g_isSystemApp = true;
    ret = mechBodyControllerService.RotateByDegree(mechId, cmdId, rotateByDegreeParam);
    EXPECT_EQ(ret, INVALID_MECH_ID);

    mechId = MECHID;
    ret = mechBodyControllerService.RotateByDegree(mechId, cmdId, nullptr);
    EXPECT_EQ(ret, INVALID_ROTATE_PARAM);

    ret = mechBodyControllerService.RotateByDegree(mechId, cmdId, rotateByDegreeParam);
    EXPECT_EQ(ret, INVALID_ROTATE_PARAM);

    rotateParam.duration = DURATION;
    auto rotateByDegreeParamSecond = std::make_shared<RotateByDegreeParam>(rotateParam);

    mechBodyControllerService.motionManagers_[mechId] = nullptr;
    ret = mechBodyControllerService.RotateByDegree(mechId, cmdId, rotateByDegreeParamSecond);
    EXPECT_EQ(ret, DEVICE_NOT_CONNECTED);

    rotateParam.duration = 0;
    auto rotateByDegreeParamThird = std::make_shared<RotateByDegreeParam>(rotateParam);
    ret = mechBodyControllerService.RotateByDegree(mechId, cmdId, rotateByDegreeParamThird);
    EXPECT_EQ(ret, ERR_OK);

    rotateParam.duration = DURATION;
    auto rotateByDegreeParamFourth = std::make_shared<RotateByDegreeParam>(rotateParam);
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    mechBodyControllerService.motionManagers_[mechId] = motionMgr;
    ret = mechBodyControllerService.RotateByDegree(mechId, cmdId, rotateByDegreeParamFourth);
    EXPECT_EQ(ret, DEVICE_NOT_PLACED_ON_MECH);
    DTEST_LOG << "MechBodyControllerServiceTest RotateByDegree_001 end" << std::endl;
}

/**
 * @tc.name  : NotifyOperationResult_001
 * @tc.number: NotifyOperationResult_001
 * @tc.desc  : Test NotifyOperationResult function.
 */
HWTEST_F(MechBodyControllerServiceTest, NotifyOperationResult_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest NotifyOperationResult_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    int32_t tokenId = MECHID;
    std::string cmdId = "cmdId";
    ExecResult result = ExecResult::COMPLETED;
    int32_t ret = mechBodyControllerService.NotifyOperationResult(tokenId, cmdId, result);
    EXPECT_EQ(ret, NAPI_SEND_DATA_FAIL);
    DTEST_LOG << "MechBodyControllerServiceTest NotifyOperationResult_001 end" << std::endl;
}

/**
 * @tc.name  : RotateToEulerAngles_001
 * @tc.number: RotateToEulerAngles_001
 * @tc.desc  : Test RotateToEulerAngles function when mechId < 0.
 */
HWTEST_F(MechBodyControllerServiceTest, RotateToEulerAngles_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest RotateToEulerAngles_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = true;
    int32_t mechId = REVERTMECHID;
    std::string cmdId = "cmdId";
    RotateToEulerAnglesParam rotateParam;
    rotateParam.duration = DURATION;
    rotateParam.angles.yaw = YAW;
    rotateParam.angles.roll = ROLL;
    rotateParam.angles.pitch = PITCH;
    auto rotateToEulerAnglesParam = std::make_shared<RotateToEulerAnglesParam>(rotateParam);
    int32_t ret = mechBodyControllerService.RotateToEulerAngles(mechId, cmdId, rotateToEulerAnglesParam);
    EXPECT_EQ(ret, INVALID_MECH_ID);
    DTEST_LOG << "MechBodyControllerServiceTest RotateToEulerAngles_001 end" << std::endl;
}

/**
 * @tc.name  : RotateToEulerAngles_002
 * @tc.number: RotateToEulerAngles_002
 * @tc.desc  : Test RotateToEulerAngles function when duration < 0.
 */
HWTEST_F(MechBodyControllerServiceTest, RotateToEulerAngles_002, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest RotateToEulerAngles_002 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = true;
    int32_t mechId = MECHID;
    std::string cmdId = "cmdId";
    RotateToEulerAnglesParam rotateParam;
    rotateParam.duration = REVERTDURATION;
    rotateParam.angles.yaw = YAW;
    rotateParam.angles.roll = ROLL;
    rotateParam.angles.pitch = PITCH;
    auto rotateToEulerAnglesParam = std::make_shared<RotateToEulerAnglesParam>(rotateParam);
    int32_t ret = mechBodyControllerService.RotateToEulerAngles(mechId, cmdId, rotateToEulerAnglesParam);
    EXPECT_EQ(ret, INVALID_ROTATE_PARAM);
    DTEST_LOG << "MechBodyControllerServiceTest RotateToEulerAngles_002 end" << std::endl;
}

/**
 * @tc.name  : RotateToEulerAngles_003
 * @tc.number: RotateToEulerAngles_003
 * @tc.desc  : Test RotateToEulerAngles function.
 */
HWTEST_F(MechBodyControllerServiceTest, RotateToEulerAngles_003, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest RotateToEulerAngles_003 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = true;
    int32_t mechId = MECHID;
    std::string cmdId = "cmdId";
    RotateToEulerAnglesParam rotateParam;
    rotateParam.duration = DURATION;
    rotateParam.angles.yaw = YAW;
    rotateParam.angles.roll = ROLL;
    rotateParam.angles.pitch = PITCH;
    auto rotateToEulerAnglesParam = std::make_shared<RotateToEulerAnglesParam>(rotateParam);
    mechBodyControllerService.motionManagers_[mechId] = nullptr;
    int32_t ret = mechBodyControllerService.RotateToEulerAngles(mechId, cmdId, rotateToEulerAnglesParam);
    EXPECT_EQ(ret, DEVICE_NOT_CONNECTED);

    g_isSystemApp = false;
    ret = mechBodyControllerService.RotateToEulerAngles(mechId, cmdId, rotateToEulerAnglesParam);
    EXPECT_EQ(ret, PERMISSION_DENIED);

    g_isSystemApp = true;
    rotateParam.duration = 0;
    auto rotateToEulerAnglesParamSecond = std::make_shared<RotateToEulerAnglesParam>(rotateParam);
    ret = mechBodyControllerService.RotateToEulerAngles(mechId, cmdId, rotateToEulerAnglesParamSecond);
    EXPECT_EQ(ret, ERR_OK);

    rotateParam.duration = DURATION;
    ret = mechBodyControllerService.RotateToEulerAngles(mechId, cmdId, nullptr);
    EXPECT_EQ(ret, INVALID_ROTATE_PARAM);

    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    mechBodyControllerService.motionManagers_[mechId] = motionMgr;
    ret = mechBodyControllerService.RotateToEulerAngles(mechId, cmdId, rotateToEulerAnglesParam);
    EXPECT_EQ(ret, DEVICE_NOT_PLACED_ON_MECH);
    DTEST_LOG << "MechBodyControllerServiceTest RotateToEulerAngles_003 end" << std::endl;
}

/**
 * @tc.name  : GetMaxRotationTime_001
 * @tc.number: GetMaxRotationTime_001
 * @tc.desc  : Test GetMaxRotationTime function when mechId < 0.
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationTime_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest GetMaxRotationTime_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = true;
    int32_t mechId = REVERTMECHID;
    auto timeLimit = std::shared_ptr<TimeLimit>();
    int32_t ret = mechBodyControllerService.GetMaxRotationTime(mechId, timeLimit);
    EXPECT_EQ(ret, INVALID_MECH_ID);
    DTEST_LOG << "MechBodyControllerServiceTest GetMaxRotationTime_001 end" << std::endl;
}

/**
 * @tc.name  : GetMaxRotationTime_002
 * @tc.number: GetMaxRotationTime_002
 * @tc.desc  : Test GetMaxRotationTime function.
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationTime_002, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest GetMaxRotationTime_002 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = false;
    int32_t mechId = MECHID;
    auto timeLimit = std::shared_ptr<TimeLimit>();
    int32_t ret = mechBodyControllerService.GetMaxRotationTime(mechId, timeLimit);
    EXPECT_EQ(ret, PERMISSION_DENIED);

    g_isSystemApp = true;
    mechBodyControllerService.motionManagers_[mechId] = nullptr;
    ret = mechBodyControllerService.GetMaxRotationTime(mechId, timeLimit);
    EXPECT_EQ(ret, DEVICE_NOT_CONNECTED);

    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    mechBodyControllerService.motionManagers_[mechId] = motionMgr;
    ret = mechBodyControllerService.GetMaxRotationTime(mechId, timeLimit);
    EXPECT_EQ(ret, DEVICE_NOT_PLACED_ON_MECH);
    DTEST_LOG << "MechBodyControllerServiceTest GetMaxRotationTime_002 end" << std::endl;
}

/**
 * @tc.name  : GetMaxRotationSpeed_001
 * @tc.number: GetMaxRotationSpeed_001
 * @tc.desc  : Test GetMaxRotationSpeed function when mechId < 0.
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationSpeed_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest GetMaxRotationSpeed_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = true;
    int32_t mechId = REVERTMECHID;
    RotateSpeedLimit rotateSpeedLimit;
    rotateSpeedLimit.speedMax.yawSpeed = YAW;
    rotateSpeedLimit.speedMax.rollSpeed = ROLL;
    rotateSpeedLimit.speedMax.pitchSpeed = PITCH;
    rotateSpeedLimit.speedMin.yawSpeed = YAW;
    rotateSpeedLimit.speedMin.rollSpeed = ROLL;
    rotateSpeedLimit.speedMin.pitchSpeed = PITCH;
    int32_t ret = mechBodyControllerService.GetMaxRotationSpeed(mechId, rotateSpeedLimit);
    EXPECT_EQ(ret, INVALID_MECH_ID);
    DTEST_LOG << "MechBodyControllerServiceTest GetMaxRotationSpeed_001 end" << std::endl;
}

/**
 * @tc.name  : GetMaxRotationSpeed_002
 * @tc.number: GetMaxRotationSpeed_002
 * @tc.desc  : Test GetMaxRotationSpeed function.
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationSpeed_002, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest GetMaxRotationSpeed_002 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = false;
    int32_t mechId = MECHID;
    RotateSpeedLimit rotateSpeedLimit;
    rotateSpeedLimit.speedMax.yawSpeed = YAW;
    rotateSpeedLimit.speedMax.rollSpeed = ROLL;
    rotateSpeedLimit.speedMax.pitchSpeed = PITCH;
    rotateSpeedLimit.speedMin.yawSpeed = YAW;
    rotateSpeedLimit.speedMin.rollSpeed = ROLL;
    rotateSpeedLimit.speedMin.pitchSpeed = PITCH;
    int32_t ret = mechBodyControllerService.GetMaxRotationSpeed(mechId, rotateSpeedLimit);
    EXPECT_EQ(ret, PERMISSION_DENIED);

    g_isSystemApp = true;
    mechBodyControllerService.motionManagers_[mechId] = nullptr;
    ret = mechBodyControllerService.GetMaxRotationSpeed(mechId, rotateSpeedLimit);
    EXPECT_EQ(ret, DEVICE_NOT_CONNECTED);

    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    mechBodyControllerService.motionManagers_[mechId] = motionMgr;
    ret = mechBodyControllerService.GetMaxRotationSpeed(mechId, rotateSpeedLimit);
    EXPECT_EQ(ret, DEVICE_NOT_PLACED_ON_MECH);
    DTEST_LOG << "MechBodyControllerServiceTest GetMaxRotationSpeed_002 end" << std::endl;
}

/**
 * @tc.name  : RotateBySpeed_001
 * @tc.number: RotateBySpeed_001
 * @tc.desc  : Test RotateBySpeed function when mechId < 0.
 */
HWTEST_F(MechBodyControllerServiceTest, RotateBySpeed_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest RotateBySpeed_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = true;
    int32_t mechId = REVERTMECHID;
    std::string cmdId = "cmdId";
    RotateBySpeedParam rotateParam;
    rotateParam.duration = DURATION;
    rotateParam.speed.yawSpeed = YAW;
    rotateParam.speed.rollSpeed = ROLL;
    rotateParam.speed.pitchSpeed = PITCH;
    auto rotateBySpeedParam = std::make_shared<RotateBySpeedParam>(rotateParam);
    int32_t ret = mechBodyControllerService.RotateBySpeed(mechId, cmdId, rotateBySpeedParam);
    EXPECT_EQ(ret, INVALID_MECH_ID);
    DTEST_LOG << "MechBodyControllerServiceTest RotateBySpeed_001 end" << std::endl;
}

/**
 * @tc.name  : RotateBySpeed_002
 * @tc.number: RotateBySpeed_002
 * @tc.desc  : Test RotateBySpeed function.
 */
HWTEST_F(MechBodyControllerServiceTest, RotateBySpeed_002, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest RotateBySpeed_002 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = false;
    int32_t mechId = MECHID;
    std::string cmdId = "cmdId";
    RotateBySpeedParam rotateParam;
    rotateParam.duration = DURATION;
    rotateParam.speed.yawSpeed = YAW;
    rotateParam.speed.rollSpeed = ROLL;
    rotateParam.speed.pitchSpeed = PITCH;
    auto rotateBySpeedParam = std::make_shared<RotateBySpeedParam>(rotateParam);
    int32_t ret = mechBodyControllerService.RotateBySpeed(mechId, cmdId, rotateBySpeedParam);
    EXPECT_EQ(ret, PERMISSION_DENIED);

    g_isSystemApp = true;
    ret = mechBodyControllerService.RotateBySpeed(mechId, cmdId, nullptr);
    EXPECT_EQ(ret, INVALID_ROTATE_PARAM);


    mechBodyControllerService.motionManagers_[mechId] = nullptr;
    ret = mechBodyControllerService.RotateBySpeed(mechId, cmdId, rotateBySpeedParam);
    EXPECT_EQ(ret, DEVICE_NOT_CONNECTED);

    rotateParam.duration = 0;
    auto rotateBySpeedParamSecond = std::make_shared<RotateBySpeedParam>(rotateParam);
    ret = mechBodyControllerService.RotateBySpeed(mechId, cmdId, rotateBySpeedParamSecond);
    EXPECT_EQ(ret, ERR_OK);

    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    mechBodyControllerService.motionManagers_[mechId] = motionMgr;
    ret = mechBodyControllerService.RotateBySpeed(mechId, cmdId, rotateBySpeedParam);
    EXPECT_EQ(ret, DEVICE_NOT_PLACED_ON_MECH);
    DTEST_LOG << "MechBodyControllerServiceTest RotateBySpeed_002 end" << std::endl;
}

/**
 * @tc.name  : StopMoving_001
 * @tc.number: StopMoving_001
 * @tc.desc  : Test StopMoving function when mechId < 0.
 */
HWTEST_F(MechBodyControllerServiceTest, StopMoving_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest StopMoving_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = true;
    int32_t mechId = REVERTMECHID;
    std::string cmdId = "cmdId";
    int32_t ret = mechBodyControllerService.StopMoving(mechId, cmdId);
    EXPECT_EQ(ret, INVALID_MECH_ID);
    DTEST_LOG << "MechBodyControllerServiceTest StopMoving_001 end" << std::endl;
}

/**
 * @tc.name  : StopMoving_002
 * @tc.number: StopMoving_002
 * @tc.desc  : Test StopMoving function.
 */
HWTEST_F(MechBodyControllerServiceTest, StopMoving_002, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest StopMoving_002 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = false;
    int32_t mechId = MECHID;
    std::string cmdId = "cmdId";
    int32_t ret = mechBodyControllerService.StopMoving(mechId, cmdId);
    EXPECT_EQ(ret, PERMISSION_DENIED);

    g_isSystemApp = true;
    mechBodyControllerService.motionManagers_[mechId] = nullptr;
    ret = mechBodyControllerService.StopMoving(mechId, cmdId);
    EXPECT_EQ(ret, DEVICE_NOT_CONNECTED);

    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    mechBodyControllerService.motionManagers_[mechId] = motionMgr;
    ret = mechBodyControllerService.StopMoving(mechId, cmdId);
    EXPECT_EQ(ret, DEVICE_NOT_PLACED_ON_MECH);
    DTEST_LOG << "MechBodyControllerServiceTest StopMoving_002 end" << std::endl;
}

/**
 * @tc.name  : GetRotationAngles_001
 * @tc.number: GetRotationAngles_001
 * @tc.desc  : Test GetRotationAngles function when mechId < 0.
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAngles_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest GetRotationAngles_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = true;
    int32_t mechId = REVERTMECHID;
    EulerAngles eulerAngles;
    eulerAngles.yaw = YAW;
    eulerAngles.roll = ROLL;
    eulerAngles.pitch = PITCH;
    auto eulerAnglesPtr = std::make_shared<EulerAngles>(eulerAngles);
    int32_t ret = mechBodyControllerService.GetRotationAngles(mechId, eulerAnglesPtr);
    EXPECT_EQ(ret, INVALID_MECH_ID);
    DTEST_LOG << "MechBodyControllerServiceTest GetRotationAngles_001 end" << std::endl;
}

/**
 * @tc.name  : GetRotationAngles_002
 * @tc.number: GetRotationAngles_002
 * @tc.desc  : Test GetRotationAngles function.
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAngles_002, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest GetRotationAngles_002 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = false;
    int32_t mechId = MECHID;
    EulerAngles eulerAngles;
    eulerAngles.yaw = YAW;
    eulerAngles.roll = ROLL;
    eulerAngles.pitch = PITCH;
    auto eulerAnglesPtr = std::make_shared<EulerAngles>(eulerAngles);
    int32_t ret = mechBodyControllerService.GetRotationAngles(mechId, eulerAnglesPtr);
    EXPECT_EQ(ret, PERMISSION_DENIED);

    g_isSystemApp = true;
    mechBodyControllerService.motionManagers_[mechId] = nullptr;
    ret = mechBodyControllerService.GetRotationAngles(mechId, eulerAnglesPtr);
    EXPECT_EQ(ret, DEVICE_NOT_CONNECTED);

    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    mechBodyControllerService.motionManagers_[mechId] = motionMgr;
    ret = mechBodyControllerService.GetRotationAngles(mechId, eulerAnglesPtr);
    EXPECT_EQ(ret, DEVICE_NOT_PLACED_ON_MECH);
    DTEST_LOG << "MechBodyControllerServiceTest GetRotationAngles_002 end" << std::endl;
}

/**
 * @tc.name  : GetRotationDegreeLimits_001
 * @tc.number: GetRotationDegreeLimits_001
 * @tc.desc  : Test GetRotationDegreeLimits function when mechId < 0.
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationDegreeLimits_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest GetRotationDegreeLimits_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = true;
    int32_t mechId = REVERTMECHID;
    RotateDegreeLimit rotationLimit;
    rotationLimit.negMax.yaw = YAW;
    rotationLimit.negMax.roll = ROLL;
    rotationLimit.negMax.pitch = PITCH;
    rotationLimit.posMax.yaw = YAW;
    rotationLimit.posMax.roll = ROLL;
    rotationLimit.posMax.pitch = PITCH;
    int32_t ret = mechBodyControllerService.GetRotationDegreeLimits(mechId, rotationLimit);
    EXPECT_EQ(ret, INVALID_MECH_ID);
    DTEST_LOG << "MechBodyControllerServiceTest GetRotationDegreeLimits_001 end" << std::endl;
}

/**
 * @tc.name  : GetRotationDegreeLimits_002
 * @tc.number: GetRotationDegreeLimits_002
 * @tc.desc  : Test GetRotationDegreeLimits function.
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationDegreeLimits_002, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest GetRotationDegreeLimits_002 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = false;
    int32_t mechId = MECHID;
    RotateDegreeLimit rotationLimit;
    rotationLimit.negMax.yaw = YAW;
    rotationLimit.negMax.roll = ROLL;
    rotationLimit.negMax.pitch = PITCH;
    rotationLimit.posMax.yaw = YAW;
    rotationLimit.posMax.roll = ROLL;
    rotationLimit.posMax.pitch = PITCH;
    int32_t ret = mechBodyControllerService.GetRotationDegreeLimits(mechId, rotationLimit);
    EXPECT_EQ(ret, PERMISSION_DENIED);

    g_isSystemApp = true;
    mechBodyControllerService.motionManagers_[mechId] = nullptr;
    ret = mechBodyControllerService.GetRotationDegreeLimits(mechId, rotationLimit);
    EXPECT_EQ(ret, DEVICE_NOT_CONNECTED);

    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    mechBodyControllerService.motionManagers_[mechId] = motionMgr;
    ret = mechBodyControllerService.GetRotationDegreeLimits(mechId, rotationLimit);
    EXPECT_EQ(ret, DEVICE_NOT_PLACED_ON_MECH);
    DTEST_LOG << "MechBodyControllerServiceTest GetRotationDegreeLimits_002 end" << std::endl;
}

/**
 * @tc.name  : GetRotationAxesStatus_001
 * @tc.number: GetRotationAxesStatus_001
 * @tc.desc  : Test GetRotationAxesStatus function when mechId < 0.
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAxesStatus_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest GetRotationAxesStatus_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = true;
    int32_t mechId = REVERTMECHID;
    RotationAxisLimited limited = RotationAxisLimited::NOT_LIMITED;
    RotationAxesStatus axesStatus;
    axesStatus.pitchEnabled = true;
    axesStatus.rollEnabled = true;
    axesStatus.yawEnabled = true;
    axesStatus.pitchLimited = limited;
    axesStatus.rollLimited = limited;
    axesStatus.yawLimited = limited;
    int32_t ret = mechBodyControllerService.GetRotationAxesStatus(mechId, axesStatus);
    EXPECT_EQ(ret, INVALID_MECH_ID);
    DTEST_LOG << "MechBodyControllerServiceTest GetRotationAxesStatus_001 end" << std::endl;
}

/**
 * @tc.name  : GetRotationAxesStatus_002
 * @tc.number: GetRotationAxesStatus_002
 * @tc.desc  : Test GetRotationAxesStatus function.
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAxesStatus_002, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest GetRotationAxesStatus_002 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = false;
    int32_t mechId = MECHID;
    RotationAxisLimited limited = RotationAxisLimited::NOT_LIMITED;
    RotationAxesStatus axesStatus;
    axesStatus.pitchEnabled = true;
    axesStatus.rollEnabled = true;
    axesStatus.yawEnabled = true;
    axesStatus.pitchLimited = limited;
    axesStatus.rollLimited = limited;
    axesStatus.yawLimited = limited;
    int32_t ret = mechBodyControllerService.GetRotationAxesStatus(mechId, axesStatus);
    EXPECT_EQ(ret, PERMISSION_DENIED);

    g_isSystemApp = true;
    mechBodyControllerService.motionManagers_[mechId] = nullptr;
    ret = mechBodyControllerService.GetRotationAxesStatus(mechId, axesStatus);
    EXPECT_EQ(ret, DEVICE_NOT_CONNECTED);

    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    mechBodyControllerService.motionManagers_[mechId] = motionMgr;
    ret = mechBodyControllerService.GetRotationAxesStatus(mechId, axesStatus);
    EXPECT_EQ(ret, DEVICE_NOT_PLACED_ON_MECH);
    DTEST_LOG << "MechBodyControllerServiceTest GetRotationAxesStatus_002 end" << std::endl;
}

/**
 * @tc.name  : RegisterRotationAxesStatusChangeCallback_001
 * @tc.number: RegisterRotationAxesStatusChangeCallback_001
 * @tc.desc  : Test RegisterRotationAxesStatusChangeCallback function when callback is nullptr.
 */
HWTEST_F(MechBodyControllerServiceTest, RegisterRotationAxesStatusChangeCallback_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest RegisterRotationAxesStatusChangeCallback_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = true;
    auto callback = sptr<IRemoteObject>();
    int32_t ret = mechBodyControllerService.RegisterRotationAxesStatusChangeCallback(callback);
    EXPECT_EQ(ret, INVALID_REMOTE_OBJECT);

    g_isSystemApp = false;
    ret = mechBodyControllerService.RegisterRotationAxesStatusChangeCallback(callback);
    EXPECT_EQ(ret, PERMISSION_DENIED);
    DTEST_LOG << "MechBodyControllerServiceTest RegisterRotationAxesStatusChangeCallback_001 end" << std::endl;
}

/**
 * @tc.name  : UnRegisterRotationAxesStatusChangeCallback_001
 * @tc.number: UnRegisterRotationAxesStatusChangeCallback_001
 * @tc.desc  : Test UnRegisterRotationAxesStatusChangeCallback function.
 */
HWTEST_F(MechBodyControllerServiceTest, UnRegisterRotationAxesStatusChangeCallback_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest UnRegisterRotationAxesStatusChangeCallback_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    g_isSystemApp = true;
    int32_t ret = mechBodyControllerService.UnRegisterRotationAxesStatusChangeCallback();
    EXPECT_EQ(ret, ERR_OK);

    g_isSystemApp = false;
    ret = mechBodyControllerService.UnRegisterRotationAxesStatusChangeCallback();
    EXPECT_EQ(ret, PERMISSION_DENIED);
    DTEST_LOG << "MechBodyControllerServiceTest UnRegisterRotationAxesStatusChangeCallback_001 end" << std::endl;
}

/**
 * @tc.name  : OnRotationAxesStatusChange_001
 * @tc.number: OnRotationAxesStatusChange_001
 * @tc.desc  : Test OnRotationAxesStatusChange function.
 */
HWTEST_F(MechBodyControllerServiceTest, OnRotationAxesStatusChange_001, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerServiceTest OnRotationAxesStatusChange_001 begin" << std::endl;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    int32_t mechId = MECHID;
    RotationAxisLimited limited = RotationAxisLimited::NOT_LIMITED;
    RotationAxesStatus axesStatus;
    axesStatus.pitchEnabled = true;
    axesStatus.rollEnabled = true;
    axesStatus.yawEnabled = true;
    axesStatus.pitchLimited = limited;
    axesStatus.rollLimited = limited;
    axesStatus.yawLimited = limited;
    int32_t ret = mechBodyControllerService.OnRotationAxesStatusChange(mechId, axesStatus);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "MechBodyControllerServiceTest OnRotationAxesStatusChange_001 end" << std::endl;
}
}
}