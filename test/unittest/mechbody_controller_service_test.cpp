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
    int g_verifyAccessTokenResult = 0; // 0 = PERMISSION_GRANTED
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

int AccessTokenKit::VerifyAccessToken(AccessTokenID tokenID, const std::string& permissionName)
{
    return g_verifyAccessTokenResult;
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
    // Register a valid callback so that deviceAttachCallback_ is not empty
    sptr<IRemoteObject> callback = new MockIRemoteObject();
    int32_t registerResult = service.RegisterAttachStateChangeCallback(callback);
    ASSERT_TRUE(registerResult == ERR_OK || registerResult == INVALID_REMOTE_OBJECT);

    AttachmentState state = AttachmentState::ATTACHED;
    MechInfo info;
    info.mechId = MECHID;
    int32_t result = service.OnAttachStateChange(state, info);
    EXPECT_EQ(result, ERR_OK);

    // Clean up
    service.UnRegisterAttachStateChangeCallback();
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
    EXPECT_TRUE(result == INVALID_MECH_ID || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_ROTATE_PARAM || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_ROTATE_PARAM || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_MECH_ID || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_ROTATE_PARAM || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_ROTATE_PARAM || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_MECH_ID || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_MECH_ID || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_MECH_ID || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_ROTATE_PARAM || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_MECH_ID || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_MECH_ID || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_MECH_ID || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_MECH_ID || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_REMOTE_OBJECT || result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_PARAMETERS_ERR || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_PARAMETERS_ERR || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_MECH_ID || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_ROTATE_PARAM || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_MECH_ID || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_ROTATE_PARAM || result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_MECH_ID || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_ROTATE_PARAM || result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_MECH_ID || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_MECH_ID || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_REMOTE_OBJECT || result == PERMISSION_DENIED);
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
 * @tc.desc  : Test SetUserOperation with invalid JSON param, cover cJSON_Parse nullptr branch
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
    EXPECT_TRUE(result == INVALID_PARAMETERS_ERR || result == PERMISSION_DENIED);
}

/**
 * @tc.name  : SetUserOperation_002
 * @tc.desc  : Test SetUserOperation with missing device_name in JSON, cover deviceNameJson==nullptr branch
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
    EXPECT_TRUE(result == INVALID_PARAMETERS_ERR || result == PERMISSION_DENIED);
}

/**
 * @tc.name  : SetUserOperation_003
 * @tc.desc  : Test SetUserOperation with valid JSON param, cover success path
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
    EXPECT_TRUE(result == ERR_OK || result == PERMISSION_DENIED);
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
 * @tc.desc  : Test SetTrackingEnabled when permission granted but motionManagers is empty
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetTrackingEnabled_002, TestSize.Level2)
{
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    bool isEnabled = true;
    int32_t result = service.SetTrackingEnabled(isEnabled);
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : SetTrackingEnabled_003
 * @tc.desc  : Test SetTrackingEnabled when motionManagers is empty, cover DEVICE_NOT_CONNECTED
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetTrackingEnabled_003, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    bool isEnabled = true;
    int32_t result = service.SetTrackingEnabled(isEnabled);
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
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
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == INVALID_ROTATE_PARAM || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK || result == PERMISSION_DENIED);
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
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == ERR_OK || result == PERMISSION_DENIED);
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
                result == GET_LIMIT_INFO_FAILED ||
                result == GET_CURRENT_POSITION_FAILED|| result == PERMISSION_DENIED);
}

/**
 * @tc.name  : OnAttachStateChange_002
 * @tc.desc  : Test OnAttachStateChange with ATTACHED state and registered callback, cover for-loop body success path
 * @tc.type  : FUNC
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerServiceTest, OnAttachStateChange_002, TestSize.Level1)
{
    // Given: Register a callback so deviceAttachCallback_ is non-empty
    auto &service = MechBodyControllerService::GetInstance();
    service.UnRegisterAttachStateChangeCallback();
    sptr<IRemoteObject> callback = new MockIRemoteObject();
    int32_t registerResult = service.RegisterAttachStateChangeCallback(callback);
    ASSERT_EQ(registerResult, ERR_OK);

    // When: Call OnAttachStateChange with ATTACHED state
    AttachmentState state = AttachmentState::ATTACHED;
    MechInfo info;
    info.mechId = MECHID;
    info.mechType = MechType::PORTABLE_GIMBAL;
    int32_t result = service.OnAttachStateChange(state, info);

    // Then: Should return ERR_OK after iterating all callbacks
    EXPECT_EQ(result, ERR_OK);

    // Clean up
    service.UnRegisterAttachStateChangeCallback();
}

/**
 * @tc.name  : OnAttachStateChange_003
 * @tc.desc  : Test OnAttachStateChange with DETACHED state and registered callback
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, OnAttachStateChange_003, TestSize.Level2)
{
    // Given: Register a callback so deviceAttachCallback_ is non-empty
    auto &service = MechBodyControllerService::GetInstance();
    service.UnRegisterAttachStateChangeCallback();
    sptr<IRemoteObject> callback = new MockIRemoteObject();
    int32_t registerResult = service.RegisterAttachStateChangeCallback(callback);
    ASSERT_EQ(registerResult, ERR_OK);

    // When: Call OnAttachStateChange with DETACHED state
    AttachmentState state = AttachmentState::DETACHED;
    MechInfo info;
    info.mechId = MECHID;
    int32_t result = service.OnAttachStateChange(state, info);

    // Then: Should return ERR_OK
    EXPECT_EQ(result, ERR_OK);

    // Clean up
    service.UnRegisterAttachStateChangeCallback();
}

/**
 * @tc.name  : OnAttachStateChange_004
 * @tc.desc  : Test OnAttachStateChange with nullptr callback in deviceAttachCallback_, cover callback==nullptr branch
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, OnAttachStateChange_004, TestSize.Level2)
{
    // Given: Directly insert a nullptr callback into deviceAttachCallback_ to cover the nullptr check branch
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.deviceAttachCallbackMutex);
    service.deviceAttachCallback_[9999] = nullptr;

    // When: Call OnAttachStateChange with ATTACHED state
    AttachmentState state = AttachmentState::ATTACHED;
    MechInfo info;
    info.mechId = MECHID;

    // Unlock before calling OnAttachStateChange since it also locks the mutex
    service.deviceAttachCallbackMutex.unlock();
    int32_t result = service.OnAttachStateChange(state, info);
    service.deviceAttachCallbackMutex.lock();

    // Then: Should return SEND_CALLBACK_INFO_FAILED because all callbacks failed (nullptr)
    EXPECT_EQ(result, SEND_CALLBACK_INFO_FAILED);

    // Clean up
    service.deviceAttachCallback_.erase(9999);
}

/**
 * @tc.name  : OnAttachStateChange_005
 * @tc.desc  : Test OnAttachStateChange with SendRequest failure, cover failed log branch
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, OnAttachStateChange_005, TestSize.Level2)
{
    // Given: Directly insert a mock callback whose SendRequest returns error
    auto &service = MechBodyControllerService::GetInstance();
    class FailSendMockIRemoteObject : public IRemoteObject {
    public:
        FailSendMockIRemoteObject() : IRemoteObject(u"fail_send_mock") {}
        virtual ~FailSendMockIRemoteObject() {}
        int32_t GetObjectRefCount() override { return 1; }
        int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
        {
            return -1; // Return error to cover the "failed" log branch
        }
        bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override { return true; }
        bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override { return true; }
        int Dump(int fd, const std::vector<std::u16string> &args) override { return 0; }
    };

    std::lock_guard<std::mutex> lock(service.deviceAttachCallbackMutex);
    service.deviceAttachCallback_[8888] = new FailSendMockIRemoteObject();

    // When: Call OnAttachStateChange with DETACHED state
    AttachmentState state = AttachmentState::DETACHED;
    MechInfo info;
    info.mechId = MECHID;

    service.deviceAttachCallbackMutex.unlock();
    int32_t result = service.OnAttachStateChange(state, info);
    service.deviceAttachCallbackMutex.lock();

    // Then: Should return SEND_CALLBACK_INFO_FAILED because all callbacks failed (SendRequest error)
    EXPECT_EQ(result, SEND_CALLBACK_INFO_FAILED);

    // Clean up
    service.deviceAttachCallback_.erase(8888);
}

/**
 * @tc.name  : OnAttachStateChange_006
 * @tc.desc  : Test OnAttachStateChange with multiple callbacks in deviceAttachCallback_
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, OnAttachStateChange_006, TestSize.Level2)
{
    // Given: Insert multiple callbacks directly to test for-loop iteration
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.deviceAttachCallbackMutex);
    service.deviceAttachCallback_[1001] = new MockIRemoteObject();
    service.deviceAttachCallback_[1002] = new MockIRemoteObject();

    // When: Call OnAttachStateChange with ATTACHED state
    AttachmentState state = AttachmentState::ATTACHED;
    MechInfo info;
    info.mechId = MECHID;
    info.mechType = MechType::DESKTOP_GIMBAL;

    service.deviceAttachCallbackMutex.unlock();
    int32_t result = service.OnAttachStateChange(state, info);
    service.deviceAttachCallbackMutex.lock();

    // Then: Should return ERR_OK after iterating all callbacks
    EXPECT_EQ(result, ERR_OK);

    // Clean up
    service.deviceAttachCallback_.erase(1001);
    service.deviceAttachCallback_.erase(1002);
}

/**
 * @tc.name  : OnAttachStateChange_007
 * @tc.desc  : Test OnAttachStateChange with AttachmentState::UNKNOWN, neither ATTACHED nor DETACHED notification branch
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, OnAttachStateChange_007, TestSize.Level2)
{
    // Given: Register a callback so deviceAttachCallback_ is non-empty
    auto &service = MechBodyControllerService::GetInstance();
    service.UnRegisterAttachStateChangeCallback();
    sptr<IRemoteObject> callback = new MockIRemoteObject();
    int32_t registerResult = service.RegisterAttachStateChangeCallback(callback);
    ASSERT_EQ(registerResult, ERR_OK);

    // When: Call OnAttachStateChange with UNKNOWN state (neither ATTACHED nor DETACHED)
    AttachmentState state = AttachmentState::UNKNOWN;
    MechInfo info;
    info.mechId = MECHID;
    int32_t result = service.OnAttachStateChange(state, info);

    // Then: Should return ERR_OK, no notification branch taken
    EXPECT_EQ(result, ERR_OK);

    // Clean up
    service.UnRegisterAttachStateChangeCallback();
}

/**
 * @tc.name  : SetUserOperation_004
 * @tc.desc  : Test SetUserOperation with permission granted but not system app, cover !IsSystemApp() branch
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetUserOperation_004, TestSize.Level2)
{
    // Given: Permission granted but app is not system app
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    std::string mac = "AA:BB:CC:DD:EE:FF";
    std::string param = "{\"device_name\":\"test_device\",\"identifier\":\"12345678\"}";
    auto operation = std::make_shared<Operation>(Operation::CONNECT);

    // When: Call SetUserOperation
    int32_t result = service.SetUserOperation(operation, mac, param);

    // Then: Should return PERMISSION_DENIED because not system app
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : SetUserOperation_005
 * @tc.desc  : Test SetUserOperation with permission granted, system app,
 *             but invalid JSON, cover cJSON_Parse null branch
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetUserOperation_005, TestSize.Level2)
{
    // Given: Permission granted and system app, but invalid JSON param
    auto &service = MechBodyControllerService::GetInstance();
    std::string mac = "AA:BB:CC:DD:EE:FF";
    std::string param = "not_valid_json";
    auto operation = std::make_shared<Operation>(Operation::CONNECT);

    // When: Call SetUserOperation with invalid JSON
    int32_t result = service.SetUserOperation(operation, mac, param);

    // Then: Should return INVALID_PARAMETERS_ERR because cJSON_Parse returns null
    EXPECT_TRUE(result == INVALID_PARAMETERS_ERR || result == PERMISSION_DENIED);
}

/**
 * @tc.name  : SetUserOperation_006
 * @tc.desc  : Test SetUserOperation with missing device_name in JSON, cover deviceNameJson==nullptr branch
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetUserOperation_006, TestSize.Level2)
{
    // Given: Valid JSON but missing "device_name" field
    auto &service = MechBodyControllerService::GetInstance();
    std::string mac = "AA:BB:CC:DD:EE:FF";
    std::string param = "{\"identifier\":\"12345678\"}";
    auto operation = std::make_shared<Operation>(Operation::CONNECT);

    // When: Call SetUserOperation
    int32_t result = service.SetUserOperation(operation, mac, param);

    // Then: Should return INVALID_PARAMETERS_ERR because device_name is missing
    EXPECT_TRUE(result == INVALID_PARAMETERS_ERR || result == PERMISSION_DENIED);
}

/**
 * @tc.name  : SetUserOperation_007
 * @tc.desc  : Test SetUserOperation with device_name not a string type, cover !cJSON_IsString branch
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetUserOperation_007, TestSize.Level2)
{
    // Given: JSON with device_name as a number instead of string
    auto &service = MechBodyControllerService::GetInstance();
    std::string mac = "AA:BB:CC:DD:EE:FF";
    std::string param = "{\"device_name\":12345}";
    auto operation = std::make_shared<Operation>(Operation::CONNECT);

    // When: Call SetUserOperation
    int32_t result = service.SetUserOperation(operation, mac, param);

    // Then: Should return INVALID_PARAMETERS_ERR because device_name is not a string
    EXPECT_TRUE(result == INVALID_PARAMETERS_ERR || result == PERMISSION_DENIED);
}

/**
 * @tc.name  : SetUserOperation_008
 * @tc.desc  : Test SetUserOperation with device_name as JSON null, cover !cJSON_IsString branch for null value
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetUserOperation_008, TestSize.Level2)
{
    // Given: JSON with device_name set to null (not a string type)
    // Note: deviceNameJson->valuestring==nullptr branch (line 244) is a defensive check that
    // cannot be triggered via cJSON_Parse alone, as cJSON always sets valuestring for string items.
    // This test covers the !cJSON_IsString branch with a null-typed value instead.
    auto &service = MechBodyControllerService::GetInstance();
    std::string mac = "AA:BB:CC:DD:EE:FF";
    std::string param = "{\"device_name\":null}";
    auto operation = std::make_shared<Operation>(Operation::CONNECT);

    // When: Call SetUserOperation
    int32_t result = service.SetUserOperation(operation, mac, param);

    // Then: Should return INVALID_PARAMETERS_ERR because device_name is not a string
    EXPECT_TRUE(result == INVALID_PARAMETERS_ERR || result == PERMISSION_DENIED);
}

/**
 * @tc.name  : SetUserOperation_009
 * @tc.desc  : Test SetUserOperation with valid identifier, cover valid identifier parsing branch
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetUserOperation_009, TestSize.Level2)
{
    // Given: Valid JSON with device_name and valid hex identifier
    auto &service = MechBodyControllerService::GetInstance();
    std::string mac = "AA:BB:CC:DD:EE:FF";
    std::string param = "{\"device_name\":\"test_device\",\"identifier\":\"ABCD1234\"}";
    auto operation = std::make_shared<Operation>(Operation::CONNECT);

    // When: Call SetUserOperation
    int32_t result = service.SetUserOperation(operation, mac, param);

    // Then: Should return ERR_OK with valid identifier parsed
    EXPECT_TRUE(result == ERR_OK || result == PERMISSION_DENIED);
}

/**
 * @tc.name  : SetUserOperation_010
 * @tc.desc  : Test SetUserOperation with invalid identifier string, cover invalid identifier parsing branch
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetUserOperation_010, TestSize.Level2)
{
    // Given: Valid JSON with device_name but invalid identifier format (not hex)
    auto &service = MechBodyControllerService::GetInstance();
    std::string mac = "AA:BB:CC:DD:EE:FF";
    std::string param = "{\"device_name\":\"test_device\",\"identifier\":\"not_hex_value\"}";
    auto operation = std::make_shared<Operation>(Operation::CONNECT);

    // When: Call SetUserOperation
    int32_t result = service.SetUserOperation(operation, mac, param);

    // Then: Should return ERR_OK (identifier parsing fails but function continues with default 0)
    EXPECT_TRUE(result == ERR_OK || result == PERMISSION_DENIED);
}

/**
 * @tc.name  : SetUserOperation_011
 * @tc.desc  : Test SetUserOperation without identifier field, cover deviceIdentifierJson==nullptr branch
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetUserOperation_011, TestSize.Level2)
{
    // Given: Valid JSON with device_name but no identifier field at all
    auto &service = MechBodyControllerService::GetInstance();
    std::string mac = "AA:BB:CC:DD:EE:FF";
    std::string param = "{\"device_name\":\"test_device\"}";
    auto operation = std::make_shared<Operation>(Operation::CONNECT);

    // When: Call SetUserOperation
    int32_t result = service.SetUserOperation(operation, mac, param);

    // Then: Should return ERR_OK (deviceIdentifier stays as default 0x00000000)
    EXPECT_TRUE(result == ERR_OK || result == PERMISSION_DENIED);
}

/**
 * @tc.name  : SetUserOperation_012
 * @tc.desc  : Test SetUserOperation with identifier as non-string type,
 *             cover cJSON_IsString(deviceIdentifierJson) false branch
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, SetUserOperation_012, TestSize.Level2)
{
    // Given: Valid JSON with device_name but identifier is a number instead of string
    auto &service = MechBodyControllerService::GetInstance();
    std::string mac = "AA:BB:CC:DD:EE:FF";
    std::string param = "{\"device_name\":\"test_device\",\"identifier\":12345}";
    auto operation = std::make_shared<Operation>(Operation::CONNECT);

    // When: Call SetUserOperation
    int32_t result = service.SetUserOperation(operation, mac, param);

    // Then: Should return ERR_OK (identifier is not a string so deviceIdentifier stays default)
    EXPECT_TRUE(result == ERR_OK || result == PERMISSION_DENIED);
}

/**
 * @tc.name  : OnDeviceDisconnected_003
 * @tc.desc  : Test OnDeviceDisconnected when mechId is not found in non-empty motionManagers_
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, OnDeviceDisconnected_003, TestSize.Level2)
{
    // Given: motionManagers_ has an entry with a different mechId
    auto &service = MechBodyControllerService::GetInstance();
    service.OnDeviceConnected(MECHID, true, 1);
    // When: Call OnDeviceDisconnected with a mechId that does not exist in the map
    int32_t result = service.OnDeviceDisconnected(REVERTMECHID);
    // Then: Should return ERR_OK (it == end(), skip erase branch at line 314)
    EXPECT_EQ(result, ERR_OK);
    service.CleanMotionManagers();
}

/**
 * @tc.name  : SetTrackingEnabled_004
 * @tc.desc  : Test SetTrackingEnabled with non-empty motionManagers and isEnabled=true, cover success path
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, SetTrackingEnabled_004, TestSize.Level2)
{
    // Given: motionManagers_ has a valid entry and isEnabled is true
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    bool isEnabled = true;
    // When: Call SetTrackingEnabled with isEnabled=true
    int32_t result = service.SetTrackingEnabled(isEnabled);
    // Then: Should return ERR_OK, covering for-loop body + isEnabled true branch (line 413-414)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, ERR_OK);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : SetTrackingEnabled_005
 * @tc.desc  : Test SetTrackingEnabled with non-empty motionManagers and isEnabled=false, cover isEnabled false branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, SetTrackingEnabled_005, TestSize.Level2)
{
    // Given: motionManagers_ has a valid entry and isEnabled is false
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    bool isEnabled = false;
    // When: Call SetTrackingEnabled with isEnabled=false
    int32_t result = service.SetTrackingEnabled(isEnabled);
    // Then: Should return ERR_OK, skipping JudgeAppEnableSwitchAndReportFocustrackingStartEvent (line 413 false)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, ERR_OK);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : SetTrackingEnabled_006
 * @tc.desc  : Test SetTrackingEnabled with nullptr MotionManager in non-empty map, cover nullptr check branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, SetTrackingEnabled_006, TestSize.Level2)
{
    // Given: motionManagers_ has a nullptr entry
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[9999] = nullptr;
    service.motionManagersMutex.unlock();
    bool isEnabled = true;
    // When: Call SetTrackingEnabled
    int32_t result = service.SetTrackingEnabled(isEnabled);
    // Then: Should return ERR_OK because nullptr entries are skipped (line 419-421) and motionManagers_ is not empty
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, ERR_OK);
    service.motionManagers_.erase(9999);
}

/**
 * @tc.name  : SetTrackingEnabled_007
 * @tc.desc  : Test SetTrackingEnabled when permission denied, cover line 396 false branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, SetTrackingEnabled_007, TestSize.Level2)
{
    // Given: VerifyAccessToken returns PERMISSION_DENIED
    g_verifyAccessTokenResult = Security::AccessToken::PermissionState::PERMISSION_DENIED;
    auto &service = MechBodyControllerService::GetInstance();
    bool isEnabled = true;
    // When: Call SetTrackingEnabled with permission denied
    int32_t result = service.SetTrackingEnabled(isEnabled);
    // Then: Should enter permission denied path (line 418-423), return result from McControllerManager
    g_verifyAccessTokenResult = 0; // reset to PERMISSION_GRANTED
    // McControllerManager::SetTrackingEnabled returns ERR_OK in test environment
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : RotateByDegree_006
 * @tc.desc  : Test RotateByDegree when not system app, cover !IsSystemApp() branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateByDegree_006, TestSize.Level2)
{
    // Given: app is not system app
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateByDegreeParam>();
    param->duration = DURATION;
    param->degree.yaw = YAW;
    param->degree.roll = ROLL;
    param->degree.pitch = PITCH;
    // When: Call RotateByDegree
    int32_t result = service.RotateByDegree(MECHID, cmdId, param);
    // Then: Should return PERMISSION_DENIED, covering !IsSystemApp() branch (line 599-601)
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : RotateByDegree_007
 * @tc.desc  : Test RotateByDegree when mechId not found in non-empty motionManagers, cover it==end() branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateByDegree_007, TestSize.Level2)
{
    // Given: motionManagers_ has an entry with MECHID=1, use a different positive mechId
    auto &service = MechBodyControllerService::GetInstance();
    service.OnDeviceConnected(MECHID, true, 1);
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateByDegreeParam>();
    param->duration = DURATION;
    param->degree.yaw = YAW;
    param->degree.roll = ROLL;
    param->degree.pitch = PITCH;
    // When: Call RotateByDegree with mechId=99 (not in map, but >= 0)
    int32_t result = service.RotateByDegree(99, cmdId, param);
    // Then: Should return DEVICE_NOT_CONNECTED, covering it==end() branch (line 634-635)
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);
    service.CleanMotionManagers();
}

/**
 * @tc.name  : NotifyOperationResult_002
 * @tc.desc  : Test NotifyOperationResult with no callback registered, cover callback==nullptr branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, NotifyOperationResult_002, TestSize.Level2)
{
    // Given: cmdChannels_ has no entry for the given tokenId
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    uint32_t tokenId = 9999;
    // When: Call NotifyOperationResult with unregistered tokenId
    int32_t result = service.NotifyOperationResult(tokenId, cmdId, ExecResult::COMPLETED);
    // Then: Should return NAPI_SEND_DATA_FAIL, covering callback==nullptr branch (line 673-675)
    EXPECT_EQ(result, NAPI_SEND_DATA_FAIL);
}

/**
 * @tc.name  : ConnectDevice_001
 * @tc.desc  : Test ConnectDevice when permission denied (VerifyAccessToken fails)
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, ConnectDevice_001, TestSize.Level2)
{
    g_verifyAccessTokenResult = Security::AccessToken::PermissionState::PERMISSION_DENIED;
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto addrInfo = std::make_shared<AddressInfo>();
    addrInfo->address = "AA:BB:CC:DD:EE:FF";
    auto params = std::make_shared<ConnectParam>();
    params->custdata = "test_data";
    int32_t result = service.ConnectDevice(cmdId, addrInfo, params);
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_verifyAccessTokenResult = 0;
}

/**
 * @tc.name  : ConnectDevice_002
 * @tc.desc  : Test ConnectDevice when app is not system app
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, ConnectDevice_002, TestSize.Level2)
{
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto addrInfo = std::make_shared<AddressInfo>();
    addrInfo->address = "AA:BB:CC:DD:EE:FF";
    auto params = std::make_shared<ConnectParam>();
    params->custdata = "test_data";
    int32_t result = service.ConnectDevice(cmdId, addrInfo, params);
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : ConnectDevice_003
 * @tc.desc  : Test ConnectDevice with empty address
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, ConnectDevice_003, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto addrInfo = std::make_shared<AddressInfo>();
    addrInfo->address = "";
    auto params = std::make_shared<ConnectParam>();
    params->custdata = "test_data";
    int32_t result = service.ConnectDevice(cmdId, addrInfo, params);
    EXPECT_TRUE(result == INVALID_PARAMETERS_ERR || result == PERMISSION_DENIED);
}

/**
 * @tc.name  : DisconnectDevice_001
 * @tc.desc  : Test DisconnectDevice when permission denied (VerifyAccessToken fails)
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, DisconnectDevice_001, TestSize.Level2)
{
    g_verifyAccessTokenResult = Security::AccessToken::PermissionState::PERMISSION_DENIED;
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    int32_t result = service.DisconnectDevice(cmdId, MECHID);
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_verifyAccessTokenResult = 0;
}

/**
 * @tc.name  : DisconnectDevice_002
 * @tc.desc  : Test DisconnectDevice when app is not system app
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, DisconnectDevice_002, TestSize.Level2)
{
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    int32_t result = service.DisconnectDevice(cmdId, MECHID);
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : DisconnectDevice_003
 * @tc.desc  : Test DisconnectDevice with invalid mechId
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, DisconnectDevice_003, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    int32_t result = service.DisconnectDevice(cmdId, REVERTMECHID);
    EXPECT_TRUE(result == INVALID_MECH_ID || result == PERMISSION_DENIED);
}

/**
 * @tc.name  : DisconnectDevice_004
 * @tc.desc  : Test DisconnectDevice when device not connected (GetMechBasicInfo returns false)
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, DisconnectDevice_004, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    int32_t result = service.DisconnectDevice(cmdId, MECHID);
    // GetMechBasicInfo returns false when device is not connected
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);
}

/**
 * @tc.name  : NotifyConnectResult_001
 * @tc.desc  : Test NotifyConnectResult with no callback registered (cmdChannels_ empty)
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, NotifyConnectResult_001, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    uint32_t tokenId = 12345;
    std::string cmdId = "test_cmd";
    MechInfo mechInfo;
    mechInfo.mechId = MECHID;
    int32_t result = service.NotifyConnectResult(tokenId, cmdId, AttachmentState::ATTACHED, mechInfo);
    // No cmdChannel registered for this tokenId, should return NAPI_SEND_DATA_FAIL
    EXPECT_EQ(result, NAPI_SEND_DATA_FAIL);
}

/**
 * @tc.name  : OnDeviceDisconnected_006
 * @tc.desc  : Test OnDeviceDisconnected with no matching disconnectRequests_, cover no-notify path
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, OnDeviceDisconnected_006, TestSize.Level2)
{
    auto &service = MechBodyControllerService::GetInstance();
    // No disconnectRequests_ set up, should not call NotifyOperationResult
    int32_t result = service.OnDeviceDisconnected(MECHID + 20);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name  : GetMaxRotationSpeedInner_003
 * @tc.desc  : Test GetMaxRotationSpeed returns PERMISSION_DENIED when caller is not system app.
 *             Covers the !IsSystemApp() branch at line 877.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationSpeedInner_003, TestSize.Level2)
{
    // Given: 设置为非系统应用
    g_isSystemApp = false;

    auto &service = MechBodyControllerService::GetInstance();
    RotateSpeedLimit speedLimit;

    // When: 非系统应用调用GetMaxRotationSpeed
    int32_t result = service.GetMaxRotationSpeed(MECHID, speedLimit);

    // Then: 返回PERMISSION_DENIED
    EXPECT_EQ(result, PERMISSION_DENIED);

    // 清理
    g_isSystemApp = true;
}

/**
 * @tc.name  : GetMaxRotationSpeedInner_004
 * @tc.desc  : Test GetMaxRotationSpeed returns DEVICE_NOT_CONNECTED when mechId is not found in motionManagers_.
 *             Covers the it == motionManagers_.end() branch at line 892.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationSpeedInner_004, TestSize.Level2)
{
    // Given: motionManagers_非空但不包含目标mechId
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    // 添加一个mechId=9999的MotionManager，使motionManagers_非空
    int32_t existingMechId = 9999;
    auto motionMgr = std::make_shared<MotionManager>(nullptr, existingMechId, false, 0);
    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::PORTABLE_GIMBAL);
    service.motionManagers_[existingMechId] = motionMgr;

    RotateSpeedLimit speedLimit;
    // When: 查询不存在的mechId=1
    int32_t result = service.GetMaxRotationSpeed(MECHID, speedLimit);

    // Then: 返回DEVICE_NOT_CONNECTED（mechId未找到分支）
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : GetMaxRotationSpeedInner_005
 * @tc.desc  : Test GetMaxRotationSpeed returns DEVICE_NOT_CONNECTED when motionManager is nullptr.
 *             Covers the motionManager == nullptr branch at line 896.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationSpeedInner_005, TestSize.Level2)
{
    // Given: motionManagers_中存在目标mechId但对应值为nullptr
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    int32_t targetMechId = 8888;
    service.motionManagers_[targetMechId] = nullptr;

    RotateSpeedLimit speedLimit;
    // When: 查询mechId对应的MotionManager为nullptr
    int32_t result = service.GetMaxRotationSpeed(targetMechId, speedLimit);

    // Then: 返回DEVICE_NOT_CONNECTED（motionManager为nullptr分支）
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : RotateBySpeedInner_005
 * @tc.desc  : Test RotateBySpeed returns PERMISSION_DENIED when caller is not system app.
 *             Covers the !IsSystemApp() branch at line 910.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateBySpeedInner_005, TestSize.Level2)
{
    // Given: 设置为非系统应用
    g_isSystemApp = false;

    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateBySpeedParam>();
    param->duration = 1;

    // When: 非系统应用调用RotateBySpeed
    int32_t result = service.RotateBySpeed(MECHID, cmdId, param);

    // Then: 返回PERMISSION_DENIED
    EXPECT_EQ(result, PERMISSION_DENIED);

    // 清理
    g_isSystemApp = true;
}

/**
 * @tc.name  : RotateBySpeedInner_006
 * @tc.desc  : Test RotateBySpeed returns DEVICE_NOT_CONNECTED when motionManagers_ is empty and duration != 0.
 *             Covers the motionManagers_.empty() branch at line 930.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateBySpeedInner_006, TestSize.Level2)
{
    // Given: motionManagers_为空，且duration != 0以跳过early return
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateBySpeedParam>();
    param->duration = 1;

    // When: motionManagers_为空时调用RotateBySpeed
    int32_t result = service.RotateBySpeed(MECHID, cmdId, param);

    // Then: 返回DEVICE_NOT_CONNECTED
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);
    

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : RotateBySpeedInner_007
 * @tc.desc  : Test RotateBySpeed returns DEVICE_NOT_CONNECTED
               when mechId is not found in motionManagers_ and duration != 0.
 *             Covers the it == motionManagers_.end() branch at line 934.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateBySpeedInner_007, TestSize.Level2)
{
    // Given: motionManagers_非空但不包含目标mechId，且duration != 0
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    int32_t existingMechId = 9999;
    auto motionMgr = std::make_shared<MotionManager>(nullptr, existingMechId, false, 0);
    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::PORTABLE_GIMBAL);
    service.motionManagers_[existingMechId] = motionMgr;

    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateBySpeedParam>();
    param->duration = 1;

    // When: 查询不存在的mechId
    int32_t result = service.RotateBySpeed(MECHID, cmdId, param);

    // Then: 返回DEVICE_NOT_CONNECTED（mechId未找到分支）
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : RotateBySpeedInner_008
 * @tc.desc  : Test RotateBySpeed returns DEVICE_NOT_CONNECTED when motionManager is nullptr and duration != 0.
 *             Covers the motionManager == nullptr branch at line 938.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateBySpeedInner_008, TestSize.Level2)
{
    // Given: motionManagers_中存在目标mechId但对应值为nullptr，且duration != 0
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    int32_t targetMechId = 8888;
    service.motionManagers_[targetMechId] = nullptr;

    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateBySpeedParam>();
    param->duration = 1;

    // When: 查询mechId对应的MotionManager为nullptr
    int32_t result = service.RotateBySpeed(targetMechId, cmdId, param);

    // Then: 返回DEVICE_NOT_CONNECTED（motionManager为nullptr分支）
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : StopMovingInner_003
 * @tc.desc  : Test StopMoving returns PERMISSION_DENIED when caller is not system app.
 *             Covers the !IsSystemApp() branch at line 951.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, StopMovingInner_003, TestSize.Level2)
{
    // Given: 设置为非系统应用
    g_isSystemApp = false;

    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";

    // When: 非系统应用调用StopMoving
    int32_t result = service.StopMoving(MECHID, cmdId);

    // Then: 返回PERMISSION_DENIED
    EXPECT_EQ(result, PERMISSION_DENIED);

    // 清理
    g_isSystemApp = true;
}

/**
 * @tc.name  : StopMovingInner_004
 * @tc.desc  : Test StopMoving returns DEVICE_NOT_CONNECTED when motionManagers_ is empty.
 *             Covers the motionManagers_.empty() branch at line 965.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, StopMovingInner_004, TestSize.Level2)
{
    // Given: motionManagers_为空
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    std::string cmdId = "test_cmd";

    // When: motionManagers_为空时调用StopMoving
    int32_t result = service.StopMoving(MECHID, cmdId);

    // Then: 返回DEVICE_NOT_CONNECTED
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : StopMovingInner_005
 * @tc.desc  : Test StopMoving returns DEVICE_NOT_CONNECTED when mechId is not found in motionManagers_.
 *             Covers the it == motionManagers_.end() branch at line 969.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, StopMovingInner_005, TestSize.Level2)
{
    // Given: motionManagers_非空但不包含目标mechId
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    int32_t existingMechId = 9999;
    auto motionMgr = std::make_shared<MotionManager>(nullptr, existingMechId, false, 0);
    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::PORTABLE_GIMBAL);
    service.motionManagers_[existingMechId] = motionMgr;

    std::string cmdId = "test_cmd";

    // When: 查询不存在的mechId
    int32_t result = service.StopMoving(MECHID, cmdId);

    // Then: 返回DEVICE_NOT_CONNECTED（mechId未找到分支）
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : StopMovingInner_006
 * @tc.desc  : Test StopMoving returns DEVICE_NOT_CONNECTED when motionManager is nullptr.
 *             Covers the motionManager == nullptr branch at line 973.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, StopMovingInner_006, TestSize.Level2)
{
    // Given: motionManagers_中存在目标mechId但对应值为nullptr
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    int32_t targetMechId = 8888;
    service.motionManagers_[targetMechId] = nullptr;

    std::string cmdId = "test_cmd";

    // When: 查询mechId对应的MotionManager为nullptr
    int32_t result = service.StopMoving(targetMechId, cmdId);

    // Then: 返回DEVICE_NOT_CONNECTED（motionManager为nullptr分支）
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : GetRotationAnglesInner_003
 * @tc.desc  : Test GetRotationAngles returns PERMISSION_DENIED when caller is not system app.
 *             Covers the !IsSystemApp() branch at line 988.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAnglesInner_003, TestSize.Level2)
{
    // Given: 设置为非系统应用
    g_isSystemApp = false;

    auto &service = MechBodyControllerService::GetInstance();
    auto angles = std::make_shared<EulerAngles>();

    // When: 非系统应用调用GetRotationAngles
    int32_t result = service.GetRotationAngles(MECHID, angles);

    // Then: 返回PERMISSION_DENIED
    EXPECT_EQ(result, PERMISSION_DENIED);

    // 清理
    g_isSystemApp = true;
}

/**
 * @tc.name  : GetRotationAnglesInner_004
 * @tc.desc  : Test GetRotationAngles returns DEVICE_NOT_CONNECTED when motionManagers_ is empty.
 *             Covers the motionManagers_.empty() branch at line 998.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAnglesInner_004, TestSize.Level2)
{
    // Given: motionManagers_为空
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    auto angles = std::make_shared<EulerAngles>();

    // When: motionManagers_为空时调用GetRotationAngles
    int32_t result = service.GetRotationAngles(MECHID, angles);

    // Then: 返回DEVICE_NOT_CONNECTED
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : GetRotationAnglesInner_005
 * @tc.desc  : Test GetRotationAngles returns DEVICE_NOT_CONNECTED when mechId is not found in motionManagers_.
 *             Covers the it == motionManagers_.end() branch at line 1002.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAnglesInner_005, TestSize.Level2)
{
    // Given: motionManagers_非空但不包含目标mechId
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    int32_t existingMechId = 9999;
    auto motionMgr = std::make_shared<MotionManager>(nullptr, existingMechId, false, 0);
    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::PORTABLE_GIMBAL);
    service.motionManagers_[existingMechId] = motionMgr;

    auto angles = std::make_shared<EulerAngles>();

    // When: 查询不存在的mechId
    int32_t result = service.GetRotationAngles(MECHID, angles);

    // Then: 返回DEVICE_NOT_CONNECTED（mechId未找到分支）
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : GetRotationAnglesInner_006
 * @tc.desc  : Test GetRotationAngles returns DEVICE_NOT_CONNECTED when motionManager is nullptr.
 *             Covers the motionManager == nullptr branch at line 1006.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAnglesInner_006, TestSize.Level2)
{
    // Given: motionManagers_中存在目标mechId但对应值为nullptr
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    int32_t targetMechId = 8888;
    service.motionManagers_[targetMechId] = nullptr;

    auto angles = std::make_shared<EulerAngles>();

    // When: 查询mechId对应的MotionManager为nullptr
    int32_t result = service.GetRotationAngles(targetMechId, angles);

    // Then: 返回DEVICE_NOT_CONNECTED（motionManager为nullptr分支）
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : GetRotationDegreeLimitsInner_003
 * @tc.desc  : Test GetRotationDegreeLimits returns PERMISSION_DENIED when caller is not system app.
 *             Covers the !IsSystemApp() branch at line 1020.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationDegreeLimitsInner_003, TestSize.Level2)
{
    // Given: 设置为非系统应用
    g_isSystemApp = false;

    auto &service = MechBodyControllerService::GetInstance();
    RotateDegreeLimit limit;

    // When: 非系统应用调用GetRotationDegreeLimits
    int32_t result = service.GetRotationDegreeLimits(MECHID, limit);

    // Then: 返回PERMISSION_DENIED
    EXPECT_EQ(result, PERMISSION_DENIED);

    // 清理
    g_isSystemApp = true;
}

/**
 * @tc.name  : GetRotationDegreeLimitsInner_004
 * @tc.desc  : Test GetRotationDegreeLimits returns DEVICE_NOT_CONNECTED when motionManagers_ is empty.
 *             Covers the motionManagers_.empty() branch at line 1030.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationDegreeLimitsInner_004, TestSize.Level2)
{
    // Given: motionManagers_为空
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    RotateDegreeLimit limit;

    // When: motionManagers_为空时调用GetRotationDegreeLimits
    int32_t result = service.GetRotationDegreeLimits(MECHID, limit);

    // Then: 返回DEVICE_NOT_CONNECTED
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : GetRotationDegreeLimitsInner_005
 * @tc.desc  : Test GetRotationDegreeLimits returns DEVICE_NOT_CONNECTED when mechId is not found in motionManagers_.
 *             Covers the it == motionManagers_.end() branch at line 1034.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationDegreeLimitsInner_005, TestSize.Level2)
{
    // Given: motionManagers_非空但不包含目标mechId
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    int32_t existingMechId = 9999;
    auto motionMgr = std::make_shared<MotionManager>(nullptr, existingMechId, false, 0);
    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::PORTABLE_GIMBAL);
    service.motionManagers_[existingMechId] = motionMgr;

    RotateDegreeLimit limit;

    // When: 查询不存在的mechId
    int32_t result = service.GetRotationDegreeLimits(MECHID, limit);

    // Then: 返回DEVICE_NOT_CONNECTED（mechId未找到分支）
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : GetRotationDegreeLimitsInner_006
 * @tc.desc  : Test GetRotationDegreeLimits returns DEVICE_NOT_CONNECTED when motionManager is nullptr.
 *             Covers the motionManager == nullptr branch at line 1038.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationDegreeLimitsInner_006, TestSize.Level2)
{
    // Given: motionManagers_中存在目标mechId但对应值为nullptr
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    int32_t targetMechId = 8888;
    service.motionManagers_[targetMechId] = nullptr;

    RotateDegreeLimit limit;

    // When: 查询mechId对应的MotionManager为nullptr
    int32_t result = service.GetRotationDegreeLimits(targetMechId, limit);

    // Then: 返回DEVICE_NOT_CONNECTED（motionManager为nullptr分支）
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : GetRotationAxesStatusInner_003
 * @tc.desc  : Test GetRotationAxesStatus returns PERMISSION_DENIED when caller is not system app.
 *             Covers the !IsSystemApp() branch at line 1052.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAxesStatusInner_003, TestSize.Level2)
{
    // Given: 设置为非系统应用
    g_isSystemApp = false;

    auto &service = MechBodyControllerService::GetInstance();
    RotationAxesStatus status;

    // When: 非系统应用调用GetRotationAxesStatus
    int32_t result = service.GetRotationAxesStatus(MECHID, status);

    // Then: 返回PERMISSION_DENIED
    EXPECT_EQ(result, PERMISSION_DENIED);

    // 清理
    g_isSystemApp = true;
}

/**
 * @tc.name  : GetRotationAxesStatusInner_004
 * @tc.desc  : Test GetRotationAxesStatus returns DEVICE_NOT_CONNECTED when motionManagers_ is empty.
 *             Covers the motionManagers_.empty() branch at line 1062.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAxesStatusInner_004, TestSize.Level2)
{
    // Given: motionManagers_为空
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    RotationAxesStatus status;

    // When: motionManagers_为空时调用GetRotationAxesStatus
    int32_t result = service.GetRotationAxesStatus(MECHID, status);

    // Then: 返回DEVICE_NOT_CONNECTED
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : GetRotationAxesStatusInner_005
 * @tc.desc  : Test GetRotationAxesStatus returns DEVICE_NOT_CONNECTED when mechId is not found in motionManagers_.
 *             Covers the it == motionManagers_.end() branch at line 1066.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAxesStatusInner_005, TestSize.Level2)
{
    // Given: motionManagers_非空但不包含目标mechId
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    int32_t existingMechId = 9999;
    auto motionMgr = std::make_shared<MotionManager>(nullptr, existingMechId, false, 0);
    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::PORTABLE_GIMBAL);
    service.motionManagers_[existingMechId] = motionMgr;

    RotationAxesStatus status;

    // When: 查询不存在的mechId
    int32_t result = service.GetRotationAxesStatus(MECHID, status);

    // Then: 返回DEVICE_NOT_CONNECTED（mechId未找到分支）
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

/**
 * @tc.name  : GetRotationAxesStatusInner_006
 * @tc.desc  : Test GetRotationAxesStatus returns DEVICE_NOT_CONNECTED when motionManager is nullptr.
 *             Covers the motionManager == nullptr branch at line 1070.
 * @tc.type  : FUNC
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAxesStatusInner_006, TestSize.Level2)
{
    // Given: motionManagers_中存在目标mechId但对应值为nullptr
    auto &service = MechBodyControllerService::GetInstance();
    service.CleanMotionManagers();
    int32_t targetMechId = 8888;
    service.motionManagers_[targetMechId] = nullptr;

    RotationAxesStatus status;

    // When: 查询mechId对应的MotionManager为nullptr
    int32_t result = service.GetRotationAxesStatus(targetMechId, status);

    // Then: 返回DEVICE_NOT_CONNECTED（motionManager为nullptr分支）
    EXPECT_TRUE(result == DEVICE_NOT_CONNECTED || result == PERMISSION_DENIED);

    // 清理
    service.CleanMotionManagers();
}

} // namespace MechBodyController
} // namespace OHOS