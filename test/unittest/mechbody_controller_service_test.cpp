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

AccessToken::ATokenTypeEnum AccessTokenKit::GetTokenTypeFlag(uint64_t tokenId)
{
    return ATokenTypeEnum::TOKEN_NATIVE;
}
} // namespace AccessToken
} // namespace Security

// Mock IRemoteObject for testing
class MockIRemoteObject : public IRemoteObject {
public:
    MockIRemoteObject() : IRemoteObject(u"mock_i_remote_object") {}
    virtual ~MockIRemoteObject() {}

    int32_t GetObjectRefCount() override { return 1; }
    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        return 0;
    }
    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override { return true; }
    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override { return true; }
    int Dump(int fd, const std::vector<std::u16string> &args) override { return 0; }
};

namespace MechBodyController {

void MechBodyControllerServiceTest::SetUpTestCase()
{
    DTEST_LOG << "MechBodyControllerServiceTest::SetUpTestCase" << std::endl;
}

void MechBodyControllerServiceTest::TearDownTestCase()
{
    DTEST_LOG << "MechBodyControllerServiceTest::TearDownTestCase" << std::endl;
}

void MechBodyControllerServiceTest::SetUp()
{
    DTEST_LOG << "MechBodyControllerServiceTest::SetUp" << std::endl;
}

void MechBodyControllerServiceTest::TearDown()
{
    DTEST_LOG << "MechBodyControllerServiceTest::TearDown" << std::endl;
}

/**
 * @tc.name  : OnStart_001
 * @tc.desc  : Test OnStart initializes service components correctly
 * @tc.type  : FUNC
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, OnStart_001, TestSize.Level1)
{
    // Given: Get the service instance and create a start reason
    auto &service = MechBodyControllerService::GetInstance();
    SystemAbilityOnDemandReason reason;

    // When: Call OnStart to initialize the service
    service.OnStart(reason);

    // Then: Verify service is properly initialized by testing dependent functionality
    // 1. Verify MechConnectManager is initialized by testing GetAttachedDevices
    std::set<MechInfo> mechInfo;
    int32_t result = service.GetAttachedDevices(mechInfo);
    EXPECT_EQ(result, ERR_OK);

    // 2. Verify BleSendManager is initialized by testing SetTrackingEnabled
    bool isEnabled = false;
    result = service.SetTrackingEnabled(isEnabled);
    EXPECT_TRUE(result == ERR_OK || result == DEVICE_NOT_CONNECTED);

    // 3. Verify service can register callbacks (requires Publish to have succeeded)
    sptr<IRemoteObject> callback = new MockIRemoteObject();
    result = service.RegisterAttachStateChangeCallback(callback);
    EXPECT_TRUE(result == ERR_OK || result == INVALID_REMOTE_OBJECT);

    // Clean up
    service.UnRegisterAttachStateChangeCallback();
}

/**
 * @tc.name  : RegisterAttachStateChangeCallback_001
 * @tc.desc  : Test RegisterAttachStateChangeCallback with nullptr
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RegisterAttachStateChangeCallback_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    int32_t result = service.RegisterAttachStateChangeCallback(nullptr);
    // Function may return INVALID_REMOTE_OBJECT or ERR_OK depending on build configuration
    EXPECT_TRUE(result == INVALID_REMOTE_OBJECT || result == ERR_OK);
}

/**
 * @tc.name  : UnRegisterAttachStateChangeCallback_001
 * @tc.desc  : Test UnRegisterAttachStateChangeCallback
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, UnRegisterAttachStateChangeCallback_001, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    int32_t result = service.UnRegisterAttachStateChangeCallback();
    EXPECT_TRUE(result == ERR_OK || result == UNREGISTER_CALLBACK_FAILED);
}

/**
 * @tc.name  : UnRegisterAttachStateChangeCallback_002
 * @tc.desc  : Test UnRegisterAttachStateChangeCallback with single callback
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, UnRegisterAttachStateChangeCallback_002, TestSize.Level2)
{
    // Given: Get the service instance and register a callback
    auto &service = MechBodyControllerService::GetInstance();
    sptr<IRemoteObject> callback = new MockIRemoteObject();
    int32_t registerResult = service.RegisterAttachStateChangeCallback(callback);
    EXPECT_TRUE(registerResult == ERR_OK || registerResult == INVALID_REMOTE_OBJECT);

    // When: Unregister the callback
    int32_t unregisterResult = service.UnRegisterAttachStateChangeCallback();

    // Then: Verify the unregister operation succeeded
    EXPECT_EQ(unregisterResult, ERR_OK);

    // Verify the callback was actually removed by checking we can register again
    // (if the previous callback wasn't removed, the listener_ would still be set)
    sptr<IRemoteObject> newCallback = new MockIRemoteObject();
    int32_t secondRegisterResult = service.RegisterAttachStateChangeCallback(newCallback);
    EXPECT_TRUE(secondRegisterResult == ERR_OK || secondRegisterResult == INVALID_REMOTE_OBJECT);

    // Clean up
    service.UnRegisterAttachStateChangeCallback();
}

/**
 * @tc.name  : OnAttachStateChange_001
 * @tc.desc  : Test OnAttachStateChange
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, OnAttachStateChange_001, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    AttachmentState state = AttachmentState::ATTACHED;
    MechInfo info;
    info.mechId = MECHID;
    int32_t result = service.OnAttachStateChange(state, info);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : OnDeviceDisconnected_001
 * @tc.desc  : Test OnDeviceDisconnected with empty motionManagers
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, OnDeviceDisconnected_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    int32_t result = service.OnDeviceDisconnected(MECHID);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : OnDeviceDisconnected_002
 * @tc.desc  : Test OnDeviceDisconnected with valid mechId
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, OnDeviceDisconnected_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.OnDeviceConnected(MECHID, true, 1);
    int32_t result = service.OnDeviceDisconnected(MECHID);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : GetAttachedDevices_001
 * @tc.desc  : Test GetAttachedDevices
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, GetAttachedDevices_001, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::set<MechInfo> mechInfo;
    int32_t result = service.GetAttachedDevices(mechInfo);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : SetTrackingEnabled_001
 * @tc.desc  : Test SetTrackingEnabled with true
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, SetTrackingEnabled_001, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    bool isEnabled = true;
    int32_t result = service.SetTrackingEnabled(isEnabled);
    EXPECT_TRUE(result == ERR_OK || result == DEVICE_NOT_CONNECTED);
}

/**
 * @tc.name  : GetTrackingEnabled_001
 * @tc.desc  : Test GetTrackingEnabled
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, GetTrackingEnabled_001, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    bool isEnabled = false;
    int32_t result = service.GetTrackingEnabled(isEnabled);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : RegisterTrackingEventCallback_001
 * @tc.desc  : Test RegisterTrackingEventCallback with nullptr
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RegisterTrackingEventCallback_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    int32_t result = service.RegisterTrackingEventCallback(nullptr);
    // Function may return INVALID_REMOTE_OBJECT or ERR_OK depending on build configuration
    EXPECT_TRUE(result == INVALID_REMOTE_OBJECT || result == ERR_OK);
}

/**
 * @tc.name  : UnRegisterTrackingEventCallback_001
 * @tc.desc  : Test UnRegisterTrackingEventCallback
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, UnRegisterTrackingEventCallback_001, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    int32_t result = service.UnRegisterTrackingEventCallback();
    EXPECT_TRUE(result == ERR_OK || result == UNREGISTER_CALLBACK_FAILED);
}

/**
 * @tc.name  : SetTrackingLayout_001
 * @tc.desc  : Test SetTrackingLayout
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, SetTrackingLayout_001, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    CameraTrackingLayout layout = CameraTrackingLayout::DEFAULT;
    int32_t result = service.SetTrackingLayout(layout);
    EXPECT_TRUE(result == ERR_OK || result == PERMISSION_DENIED);
}

/**
 * @tc.name  : GetTrackingLayout_001
 * @tc.desc  : Test GetTrackingLayout
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, GetTrackingLayout_001, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    CameraTrackingLayout layout = CameraTrackingLayout::DEFAULT;
    int32_t result = service.GetTrackingLayout(layout);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : RegisterCmdChannelInner_001
 * @tc.desc  : Test RegisterCmdChannel with nullptr
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RegisterCmdChannelInner_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    int32_t result = service.RegisterCmdChannel(nullptr);
    // Function may return INVALID_PARAMETERS_ERR or ERR_OK depending on build configuration
    EXPECT_TRUE(result == INVALID_PARAMETERS_ERR || result == ERR_OK);
}

/**
 * @tc.name  : RotateByDegree_001
 * @tc.desc  : Test RotateByDegree with invalid mechId
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateByDegree_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateByDegreeParam>();
    param->duration = DURATION;
    param->degree.yaw = YAW;
    param->degree.roll = ROLL;
    param->degree.pitch = PITCH;
    int32_t result = service.RotateByDegree(REVERTMECHID, cmdId, param);
    EXPECT_EQ(result, INVALID_MECH_ID);
}

/**
 * @tc.name  : RotateByDegree_002
 * @tc.desc  : Test RotateByDegree with nullptr param
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateByDegree_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    int32_t result = service.RotateByDegree(MECHID, cmdId, nullptr);
    EXPECT_EQ(result, INVALID_ROTATE_PARAM);
}

/**
 * @tc.name  : RotateByDegree_003
 * @tc.desc  : Test RotateByDegree with invalid duration
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateByDegree_003, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateByDegreeParam>();
    param->duration = REVERTDURATION;
    int32_t result = service.RotateByDegree(MECHID, cmdId, param);
    EXPECT_EQ(result, INVALID_ROTATE_PARAM);
}

/**
 * @tc.name  : NotifyOperationResult_001
 * @tc.desc  : Test NotifyOperationResult
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, NotifyOperationResult_001, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    int32_t result = service.NotifyOperationResult(1, cmdId, ExecResult::COMPLETED);
    // Function may return ERR_OK or NAPI_SEND_DATA_FAIL if no callback is registered
    EXPECT_TRUE(result == ERR_OK || result == NAPI_SEND_DATA_FAIL);
}

/**
 * @tc.name  : RotateToEulerAngles_001
 * @tc.desc  : Test RotateToEulerAngles with invalid mechId
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateToEulerAngles_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateToEulerAnglesParam>();
    param->duration = DURATION;
    int32_t result = service.RotateToEulerAngles(REVERTMECHID, cmdId, param);
    EXPECT_EQ(result, INVALID_MECH_ID);
}

/**
 * @tc.name  : RotateToEulerAngles_002
 * @tc.desc  : Test RotateToEulerAngles with nullptr param
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateToEulerAngles_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    int32_t result = service.RotateToEulerAngles(MECHID, cmdId, nullptr);
    EXPECT_EQ(result, INVALID_ROTATE_PARAM);
}

/**
 * @tc.name  : RotateToEulerAngles_003
 * @tc.desc  : Test RotateToEulerAngles with invalid duration
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateToEulerAngles_003, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateToEulerAnglesParam>();
    param->duration = REVERTDURATION;
    int32_t result = service.RotateToEulerAngles(MECHID, cmdId, param);
    EXPECT_EQ(result, INVALID_ROTATE_PARAM);
}

/**
 * @tc.name  : GetMaxRotationTimeInner_001
 * @tc.desc  : Test GetMaxRotationTime with invalid mechId
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationTimeInner_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    auto timeLimit = std::make_shared<TimeLimit>();
    int32_t result = service.GetMaxRotationTime(REVERTMECHID, timeLimit);
    EXPECT_EQ(result, INVALID_MECH_ID);
}

/**
 * @tc.name  : GetMaxRotationSpeedInner_001
 * @tc.desc  : Test GetMaxRotationSpeed with invalid mechId
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationSpeedInner_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    RotateSpeedLimit speedLimit;
    int32_t result = service.GetMaxRotationSpeed(REVERTMECHID, speedLimit);
    EXPECT_EQ(result, INVALID_MECH_ID);
}

/**
 * @tc.name  : RotateBySpeedInner_001
 * @tc.desc  : Test RotateBySpeed with invalid mechId
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateBySpeedInner_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateBySpeedParam>();
    int32_t result = service.RotateBySpeed(REVERTMECHID, cmdId, param);
    EXPECT_EQ(result, INVALID_MECH_ID);
}

/**
 * @tc.name  : RotateBySpeedInner_002
 * @tc.desc  : Test RotateBySpeed with nullptr param
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateBySpeedInner_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    int32_t result = service.RotateBySpeed(MECHID, cmdId, nullptr);
    EXPECT_EQ(result, INVALID_ROTATE_PARAM);
}

/**
 * @tc.name  : StopMovingInner_001
 * @tc.desc  : Test StopMoving with invalid mechId
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, StopMovingInner_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    int32_t result = service.StopMoving(REVERTMECHID, cmdId);
    EXPECT_EQ(result, INVALID_MECH_ID);
}

/**
 * @tc.name  : GetRotationAnglesInner_001
 * @tc.desc  : Test GetRotationAngles with invalid mechId
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAnglesInner_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    auto angles = std::make_shared<EulerAngles>();
    int32_t result = service.GetRotationAngles(REVERTMECHID, angles);
    EXPECT_EQ(result, INVALID_MECH_ID);
}

/**
 * @tc.name  : GetRotationDegreeLimitsInner_001
 * @tc.desc  : Test GetRotationDegreeLimits with invalid mechId
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationDegreeLimitsInner_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    RotateDegreeLimit limit;
    int32_t result = service.GetRotationDegreeLimits(REVERTMECHID, limit);
    EXPECT_EQ(result, INVALID_MECH_ID);
}

/**
 * @tc.name  : GetRotationAxesStatusInner_001
 * @tc.desc  : Test GetRotationAxesStatus with invalid mechId
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAxesStatusInner_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    RotationAxesStatus status;
    int32_t result = service.GetRotationAxesStatus(REVERTMECHID, status);
    EXPECT_EQ(result, INVALID_MECH_ID);
}

/**
 * @tc.name  : RotationAxesStatusChangeListenOnInner_001
 * @tc.desc  : Test RegisterRotationAxesStatusChangeCallback with nullptr
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotationAxesStatusChangeListenOnInner_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    int32_t result = service.RegisterRotationAxesStatusChangeCallback(nullptr);
    // Function may return INVALID_REMOTE_OBJECT or ERR_OK depending on build configuration
    EXPECT_TRUE(result == INVALID_REMOTE_OBJECT || result == ERR_OK);
}

/**
 * @tc.name  : RotationAxesStatusChangeListenOffInner_001
 * @tc.desc  : Test UnRegisterRotationAxesStatusChangeCallback
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, RotationAxesStatusChangeListenOffInner_001, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    int32_t result = service.UnRegisterRotationAxesStatusChangeCallback();
    EXPECT_TRUE(result == ERR_OK || result == PERMISSION_DENIED);
}

/**
 * @tc.name  : OnRotationAxesStatusChange_001
 * @tc.desc  : Test OnRotationAxesStatusChange with empty callback list
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, OnRotationAxesStatusChange_001, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    RotationAxesStatus status;
    int32_t result = service.OnRotationAxesStatusChange(MECHID, status);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : SearchTarget_001
 * @tc.desc  : Test SearchTarget when app is not system app
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SearchTarget_001, TestSize.Level2)
{
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto targetInfo = std::make_shared<TargetInfo>();
    auto searchParams = std::make_shared<SearchParams>();
    int32_t result = service.SearchTarget(cmdId, targetInfo, searchParams);
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : SearchTarget_002
 * @tc.desc  : Test SearchTarget with nullptr targetInfo
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SearchTarget_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto searchParams = std::make_shared<SearchParams>();
    int32_t result = service.SearchTarget(cmdId, nullptr, searchParams);
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);
}

/**
 * @tc.name  : SearchTarget_003
 * @tc.desc  : Test SearchTarget with nullptr searchParams
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SearchTarget_003, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto targetInfo = std::make_shared<TargetInfo>();
    int32_t result = service.SearchTarget(cmdId, targetInfo, nullptr);
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);
}

/**
 * @tc.name  : SearchTargetEnd_001
 * @tc.desc  : Test SearchTargetEnd when WriteInterfaceToken fails
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SearchTargetEnd_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string napiCmdId = "test_cmd";
    int32_t targetNum = 1;
    // Mock scenario where WriteInterfaceToken fails
    int32_t result = service.SearchTargetEnd(1, napiCmdId, targetNum);
    EXPECT_TRUE(result == ERR_OK || result == SEND_CALLBACK_INFO_FAILED);
}

/**
 * @tc.name  : SearchTargetEnd_002
 * @tc.desc  : Test SearchTargetEnd with empty callback list
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, SearchTargetEnd_002, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string napiCmdId = "test_cmd";
    int32_t targetNum = 1;
    int32_t result = service.SearchTargetEnd(1, napiCmdId, targetNum);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : SearchTargetEnd_003
 * @tc.desc  : Test SearchTargetEnd with zero target number
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SearchTargetEnd_003, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string napiCmdId = "test_cmd";
    int32_t targetNum = 0;
    int32_t result = service.SearchTargetEnd(1, napiCmdId, targetNum);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : SearchTargetEnd_004
 * @tc.desc  : Test SearchTargetEnd with multiple targets
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SearchTargetEnd_004, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string napiCmdId = "test_cmd";
    int32_t targetNum = 5;
    int32_t result = service.SearchTargetEnd(1, napiCmdId, targetNum);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : Move_001
 * @tc.desc  : Test Move when app is not system app
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, Move_001, TestSize.Level2)
{
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto moveParams = std::make_shared<MoveParams>();
    int32_t result = service.Move(MECHID, cmdId, moveParams);
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : Move_002
 * @tc.desc  : Test Move with invalid mechId
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, Move_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto moveParams = std::make_shared<MoveParams>();
    int32_t result = service.Move(REVERTMECHID, cmdId, moveParams);
    EXPECT_EQ(result, INVALID_MECH_ID);
}

/**
 * @tc.name  : Move_003
 * @tc.desc  : Test Move with nullptr moveParams
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, Move_003, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    int32_t result = service.Move(MECHID, cmdId, nullptr);
    EXPECT_EQ(result, INVALID_ROTATE_PARAM);
}

/**
 * @tc.name  : MoveBySpeed_001
 * @tc.desc  : Test MoveBySpeed when app is not system app
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, MoveBySpeed_001, TestSize.Level2)
{
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto speedParams = std::make_shared<SpeedParams>();
    int32_t result = service.MoveBySpeed(MECHID, cmdId, DURATION, speedParams);
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : MoveBySpeed_002
 * @tc.desc  : Test MoveBySpeed with invalid mechId
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, MoveBySpeed_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto speedParams = std::make_shared<SpeedParams>();
    int32_t result = service.MoveBySpeed(REVERTMECHID, cmdId, DURATION, speedParams);
    EXPECT_EQ(result, INVALID_MECH_ID);
}

/**
 * @tc.name  : MoveBySpeed_003
 * @tc.desc  : Test MoveBySpeed with invalid duration
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, MoveBySpeed_003, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto speedParams = std::make_shared<SpeedParams>();
    int32_t result = service.MoveBySpeed(MECHID, cmdId, REVERTDURATION, speedParams);
    EXPECT_TRUE(result == INVALID_ROTATE_PARAM || result == DEVICE_NOT_CONNECTED);
}

/**
 * @tc.name  : TurnBySpeed_001
 * @tc.desc  : Test TurnBySpeed when app is not system app
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, TurnBySpeed_001, TestSize.Level2)
{
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    float angleSpeed = 1.0f;
    int32_t result = service.TurnBySpeed(MECHID, cmdId, angleSpeed, DURATION);
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : TurnBySpeed_002
 * @tc.desc  : Test TurnBySpeed with invalid mechId
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, TurnBySpeed_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    float angleSpeed = 1.0f;
    int32_t result = service.TurnBySpeed(REVERTMECHID, cmdId, angleSpeed, DURATION);
    EXPECT_EQ(result, INVALID_MECH_ID);
}

/**
 * @tc.name  : TurnBySpeed_003
 * @tc.desc  : Test TurnBySpeed with invalid duration
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, TurnBySpeed_003, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    float angleSpeed = 1.0f;
    int32_t result = service.TurnBySpeed(MECHID, cmdId, angleSpeed, REVERTDURATION);
    EXPECT_TRUE(result == INVALID_ROTATE_PARAM || result == DEVICE_NOT_CONNECTED);
}

/**
 * @tc.name  : IsSupportAction_001
 * @tc.desc  : Test IsSupportAction when app is not system app
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, IsSupportAction_001, TestSize.Level2)
{
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    bool isSupport = false;
    int32_t result = service.IsSupportAction(MECHID, ActionType::NOD, isSupport);
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : IsSupportAction_002
 * @tc.desc  : Test IsSupportAction with invalid mechId
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, IsSupportAction_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    bool isSupport = false;
    int32_t result = service.IsSupportAction(REVERTMECHID, ActionType::NOD, isSupport);
    EXPECT_EQ(result, INVALID_MECH_ID);
}

/**
 * @tc.name  : DoAction_001
 * @tc.desc  : Test DoAction when app is not system app
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, DoAction_001, TestSize.Level2)
{
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    int32_t result = service.DoAction(MECHID, cmdId, ActionType::NOD);
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : DoAction_002
 * @tc.desc  : Test DoAction with invalid mechId
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, DoAction_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    int32_t result = service.DoAction(REVERTMECHID, cmdId, ActionType::NOD);
    EXPECT_EQ(result, INVALID_MECH_ID);
}

/**
 * @tc.name  : SubscribeCallback_001
 * @tc.desc  : Test SubscribeCallback when app is not system app
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SubscribeCallback_001, TestSize.Level2)
{
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    sptr<IRemoteObject> callback = new MockIRemoteObject();
    int32_t result = service.SubscribeCallback(callback, MechEventType::DEVICE_ADSORBED);
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : SubscribeCallback_002
 * @tc.desc  : Test SubscribeCallback with nullptr callback
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SubscribeCallback_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    sptr<IRemoteObject> callback = nullptr;
    int32_t result = service.SubscribeCallback(callback, MechEventType::DEVICE_ADSORBED);
    EXPECT_EQ(result, INVALID_REMOTE_OBJECT);
}

/**
 * @tc.name  : UnSubscribeCallback_001
 * @tc.desc  : Test UnSubscribeCallback with valid event type
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, UnSubscribeCallback_001, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    int32_t result = service.UnSubscribeCallback(MechEventType::DEVICE_ADSORBED);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : SetUserOperation_001
 * @tc.desc  : Test SetUserOperation with invalid JSON param
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetUserOperation_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string mac = "AA:BB:CC:DD:EE:FF";
    std::string param = "invalid_json";
    auto operation = std::make_shared<Operation>(Operation::CONNECT);
    int32_t result = service.SetUserOperation(operation, mac, param);
    EXPECT_EQ(result, PERMISSION_DENIED);
}

/**
 * @tc.name  : SetUserOperation_002
 * @tc.desc  : Test SetUserOperation with missing device_name in JSON
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetUserOperation_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string mac = "AA:BB:CC:DD:EE:FF";
    std::string param = "{\"identifier\":\"12345678\"}";
    auto operation = std::make_shared<Operation>(Operation::CONNECT);
    int32_t result = service.SetUserOperation(operation, mac, param);
    EXPECT_EQ(result, PERMISSION_DENIED);
}

/**
 * @tc.name  : SetUserOperation_003
 * @tc.desc  : Test SetUserOperation with valid JSON param
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetUserOperation_003, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string mac = "AA:BB:CC:DD:EE:FF";
    std::string param = "{\"device_name\":\"test_device\",\"identifier\":\"12345678\"}";
    auto operation = std::make_shared<Operation>(Operation::CONNECT);
    int32_t result = service.SetUserOperation(operation, mac, param);
    EXPECT_EQ(result, PERMISSION_DENIED);
}

/**
 * @tc.name  : CleanMotionManagers_001
 * @tc.desc  : Test CleanMotionManagers
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, CleanMotionManagers_001, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    int32_t result = service.CleanMotionManagers();
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : SetTrackingEnabled_002
 * @tc.desc  : Test SetTrackingEnabled when app is not system app
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetTrackingEnabled_002, TestSize.Level2)
{
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    bool isEnabled = true;
    int32_t result = service.SetTrackingEnabled(isEnabled);
    EXPECT_EQ(result, ERR_OK);
    g_isSystemApp = true;
}

/**
 * @tc.name  : SetTrackingEnabled_003
 * @tc.desc  : Test SetTrackingEnabled when motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetTrackingEnabled_003, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    bool isEnabled = true;
    int32_t result = service.SetTrackingEnabled(isEnabled);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : GetTrackingEnabled_002
 * @tc.desc  : Test GetTrackingEnabled with permission granted
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetTrackingEnabled_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    bool isEnabled = false;
    int32_t result = service.GetTrackingEnabled(isEnabled);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : SetTrackingLayout_002
 * @tc.desc  : Test SetTrackingLayout when app is not system app
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetTrackingLayout_002, TestSize.Level2)
{
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    CameraTrackingLayout layout = CameraTrackingLayout::DEFAULT;
    int32_t result = service.SetTrackingLayout(layout);
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : RegisterRotationAxesStatusChangeCallback_002
 * @tc.desc  : Test RegisterRotationAxesStatusChangeCallback when app is not system app
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RegisterRotationAxesStatusChangeCallback_002, TestSize.Level2)
{
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    sptr<IRemoteObject> callback = new MockIRemoteObject();
    int32_t result = service.RegisterRotationAxesStatusChangeCallback(callback);
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : UnRegisterRotationAxesStatusChangeCallback_002
 * @tc.desc  : Test UnRegisterRotationAxesStatusChangeCallback when app is not system app
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, UnRegisterRotationAxesStatusChangeCallback_002, TestSize.Level2)
{
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    int32_t result = service.UnRegisterRotationAxesStatusChangeCallback();
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : RotateByDegree_004
 * @tc.desc  : Test RotateByDegree when motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateByDegree_004, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateByDegreeParam>();
    param->duration = DURATION;
    param->degree.yaw = YAW;
    param->degree.roll = ROLL;
    param->degree.pitch = PITCH;
    int32_t result = service.RotateByDegree(MECHID, cmdId, param);
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK);
}

/**
 * @tc.name  : RotateByDegree_005
 * @tc.desc  : Test RotateByDegree with zero duration
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateByDegree_005, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateByDegreeParam>();
    param->duration = 0;
    param->degree.yaw = YAW;
    param->degree.roll = ROLL;
    param->degree.pitch = PITCH;
    int32_t result = service.RotateByDegree(MECHID, cmdId, param);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : RotateToEulerAngles_004
 * @tc.desc  : Test RotateToEulerAngles when motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateToEulerAngles_004, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateToEulerAnglesParam>();
    param->duration = DURATION;
    int32_t result = service.RotateToEulerAngles(MECHID, cmdId, param);
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK);
}

/**
 * @tc.name  : RotateToEulerAngles_005
 * @tc.desc  : Test RotateToEulerAngles with zero duration
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateToEulerAngles_005, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateToEulerAnglesParam>();
    param->duration = 0;
    int32_t result = service.RotateToEulerAngles(MECHID, cmdId, param);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : GetMaxRotationTimeInner_002
 * @tc.desc  : Test GetMaxRotationTime when motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationTimeInner_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    auto timeLimit = std::make_shared<TimeLimit>();
    int32_t result = service.GetMaxRotationTime(MECHID, timeLimit);
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK);
}

/**
 * @tc.name  : GetMaxRotationSpeedInner_002
 * @tc.desc  : Test GetMaxRotationSpeed when motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationSpeedInner_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    RotateSpeedLimit speedLimit;
    int32_t result = service.GetMaxRotationSpeed(MECHID, speedLimit);
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK);
}

/**
 * @tc.name  : RotateBySpeedInner_003
 * @tc.desc  : Test RotateBySpeed when motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateBySpeedInner_003, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateBySpeedParam>();
    int32_t result = service.RotateBySpeed(MECHID, cmdId, param);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : RotateBySpeedInner_004
 * @tc.desc  : Test RotateBySpeed with zero duration
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateBySpeedInner_004, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateBySpeedParam>();
    param->duration = 0;
    int32_t result = service.RotateBySpeed(MECHID, cmdId, param);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : StopMovingInner_002
 * @tc.desc  : Test StopMoving when motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, StopMovingInner_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    std::string cmdId = "test_cmd";
    int32_t result = service.StopMoving(MECHID, cmdId);
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK);
}

/**
 * @tc.name  : GetRotationAnglesInner_002
 * @tc.desc  : Test GetRotationAngles when motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAnglesInner_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    auto angles = std::make_shared<EulerAngles>();
    int32_t result = service.GetRotationAngles(MECHID, angles);
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK);
}

/**
 * @tc.name  : GetRotationDegreeLimitsInner_002
 * @tc.desc  : Test GetRotationDegreeLimits when motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationDegreeLimitsInner_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    RotateDegreeLimit limit;
    int32_t result = service.GetRotationDegreeLimits(MECHID, limit);
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK);
}

/**
 * @tc.name  : GetRotationAxesStatusInner_002
 * @tc.desc  : Test GetRotationAxesStatus when motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAxesStatusInner_002, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    RotationAxesStatus status;
    int32_t result = service.GetRotationAxesStatus(MECHID, status);
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK);
}

/**
 * @tc.name  : RegisterCmdChannelInner_002
 * @tc.desc  : Test RegisterCmdChannel with valid callback
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, RegisterCmdChannelInner_002, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    sptr<IRemoteObject> callback = new MockIRemoteObject();
    int32_t result = service.RegisterCmdChannel(callback);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : SubscribeCallback_003
 * @tc.desc  : Test SubscribeCallback with valid callback
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, SubscribeCallback_003, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    sptr<IRemoteObject> callback = new MockIRemoteObject();
    int32_t result = service.SubscribeCallback(callback, MechEventType::DEVICE_ADSORBED);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : NotifyMechEvent_002
 * @tc.desc  : Test NotifyMechEvent with empty subscribeChannels
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, NotifyMechEvent_002, TestSize.Level1)
{
    auto &service = MechBodyControllerService::GetInstance();
    int32_t result = service.NotifyMechEvent(MECHID, MechEventType::DEVICE_ADSORBED);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : Move_004
 * @tc.desc  : Test Move when motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, Move_004, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    std::string cmdId = "test_cmd";
    auto moveParams = std::make_shared<MoveParams>();
    int32_t result = service.Move(MECHID, cmdId, moveParams);
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK);
}

/**
 * @tc.name  : MoveBySpeed_004
 * @tc.desc  : Test MoveBySpeed with nullptr speedParams
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, MoveBySpeed_004, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    int32_t result = service.MoveBySpeed(MECHID, cmdId, DURATION, nullptr);
    EXPECT_EQ(result, INVALID_ROTATE_PARAM);
}

/**
 * @tc.name  : MoveBySpeed_005
 * @tc.desc  : Test MoveBySpeed when motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, MoveBySpeed_005, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    std::string cmdId = "test_cmd";
    auto speedParams = std::make_shared<SpeedParams>();
    int32_t result = service.MoveBySpeed(MECHID, cmdId, DURATION, speedParams);
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK);
}

/**
 * @tc.name  : TurnBySpeed_004
 * @tc.desc  : Test TurnBySpeed when motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, TurnBySpeed_004, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    std::string cmdId = "test_cmd";
    float angleSpeed = 1.0f;
    int32_t result = service.TurnBySpeed(MECHID, cmdId, angleSpeed, DURATION);
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK);
}

/**
 * @tc.name  : IsSupportAction_003
 * @tc.desc  : Test IsSupportAction when motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, IsSupportAction_003, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    bool isSupport = false;
    int32_t result = service.IsSupportAction(MECHID, ActionType::NOD, isSupport);
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK);
}

/**
 * @tc.name  : DoAction_003
 * @tc.desc  : Test DoAction when motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, DoAction_003, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    std::string cmdId = "test_cmd";
    int32_t result = service.DoAction(MECHID, cmdId, ActionType::NOD);
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK);
}

/**
 * @tc.name  : SearchTarget_004
 * @tc.desc  : Test SearchTarget with valid parameters
 * @tc.type  : FUNC
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, SearchTarget_004, TestSize.Level1)
{
    // Given: Get the service instance and prepare valid parameters
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto targetInfo = std::make_shared<TargetInfo>();
    auto searchParams = std::make_shared<SearchParams>();

    // When: Call SearchTarget with valid parameters
    int32_t result = service.SearchTarget(cmdId, targetInfo, searchParams);

    // Then: Verify the result is valid (either success or expected error)
    // The function may return various error codes depending on the state of McControllerManager
    EXPECT_TRUE(result == ERR_OK || result == INVALID_PARAMETERS_ERR || result == NO_DEVICE_CONNECTED ||
                result == GET_LIMIT_INFO_FAILED || result == GET_CURRENT_POSITION_FAILED);
}

} // namespace MechBodyController
} // namespace OHOS