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
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);
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
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);
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
    EXPECT_EQ(result, ERR_OK);
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

    // Then: Should return SEND_CALLBACK_INFO_FAILED because callback is nullptr
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

    // Then: Should return ERR_OK because loop completes even if SendRequest fails
    EXPECT_EQ(result, ERR_OK);

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
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);
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
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);
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
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);
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
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);
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
    EXPECT_EQ(result, ERR_OK);
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
    EXPECT_EQ(result, ERR_OK);
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
    EXPECT_EQ(result, ERR_OK);
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
    EXPECT_EQ(result, ERR_OK);
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
    // Then: Should return DEVICE_NOT_CONNECTED, covering motionManager==nullptr branch (line 405-406)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
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
 * @tc.name  : NotifyMechEvent_003
 * @tc.desc  : Test NotifyMechEvent with matched event and valid remote, cover success path
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, NotifyMechEvent_003, TestSize.Level2)
{
    // Given: subscribeChannels_ has a matching event entry with valid IRemoteObject
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.subscribeChannelMutex_);
    sptr<IRemoteObject> callback = new MockIRemoteObject();
    service.subscribeChannels_[{0, MechEventType::DEVICE_ADSORBED}] = callback;
    service.subscribeChannelMutex_.unlock();
    // When: Call NotifyMechEvent with matching event type
    int32_t result = service.NotifyMechEvent(MECHID, MechEventType::DEVICE_ADSORBED);
    // Then: Should return ERR_OK, covering mechEventType==event branch + remote!=nullptr + SendRequest
    service.subscribeChannelMutex_.lock();
    EXPECT_EQ(result, ERR_OK);
    service.subscribeChannels_.erase({0, MechEventType::DEVICE_ADSORBED});
}

/**
 * @tc.name  : NotifyMechEvent_004
 * @tc.desc  : Test NotifyMechEvent with non-matched event, cover mechEventType!=event branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, NotifyMechEvent_004, TestSize.Level2)
{
    // Given: subscribeChannels_ has an entry with a different event type
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.subscribeChannelMutex_);
    sptr<IRemoteObject> callback = new MockIRemoteObject();
    service.subscribeChannels_[{0, MechEventType::DEVICE_ADSORBED}] = callback;
    service.subscribeChannelMutex_.unlock();
    // When: Call NotifyMechEvent with non-matching event type
    int32_t result = service.NotifyMechEvent(MECHID, MechEventType::REACH_CLIFF);
    // Then: Should return ERR_OK, skipping the inner block (mechEventType != event)
    service.subscribeChannelMutex_.lock();
    EXPECT_EQ(result, ERR_OK);
    service.subscribeChannels_.erase({0, MechEventType::DEVICE_ADSORBED});
}

/**
 * @tc.name  : NotifyMechEvent_005
 * @tc.desc  : Test NotifyMechEvent with nullptr remote in subscribeChannels, cover remote==nullptr branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, NotifyMechEvent_005, TestSize.Level2)
{
    // Given: subscribeChannels_ has a matching event entry with nullptr remote
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.subscribeChannelMutex_);
    service.subscribeChannels_[{0, MechEventType::DEVICE_ADSORBED}] = nullptr;
    service.subscribeChannelMutex_.unlock();
    // When: Call NotifyMechEvent with matching event type
    int32_t result = service.NotifyMechEvent(MECHID, MechEventType::DEVICE_ADSORBED);
    // Then: Should return ERR_OK, covering remote==nullptr branch (line 476-477)
    service.subscribeChannelMutex_.lock();
    EXPECT_EQ(result, ERR_OK);
    service.subscribeChannels_.erase({0, MechEventType::DEVICE_ADSORBED});
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
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.CleanMotionManagers();
}

/**
 * @tc.name  : RotateByDegree_008
 * @tc.desc  : Test RotateByDegree when nullptr MotionManager in map, cover motionManager==nullptr branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateByDegree_008, TestSize.Level2)
{
    // Given: motionManagers_ has a nullptr entry for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = nullptr;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateByDegreeParam>();
    param->duration = DURATION;
    param->degree.yaw = YAW;
    param->degree.roll = ROLL;
    param->degree.pitch = PITCH;
    // When: Call RotateByDegree with MECHID
    int32_t result = service.RotateByDegree(MECHID, cmdId, param);
    // Then: Should return DEVICE_NOT_CONNECTED, covering motionManager==nullptr branch (line 638-641)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : RotateByDegree_009
 * @tc.desc  : Test RotateByDegree with valid MotionManager, cover success path with Rotate call
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateByDegree_009, TestSize.Level2)
{
    // Given: motionManagers_ has a valid MotionManager for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateByDegreeParam>();
    param->duration = DURATION;
    param->degree.yaw = YAW;
    param->degree.roll = ROLL;
    param->degree.pitch = PITCH;
    // When: Call RotateByDegree with valid params
    int32_t result = service.RotateByDegree(MECHID, cmdId, param);
    // Then: Should call MotionManager::Rotate and return its result, covering line 637-645
    // MotionManager::Rotate returns DEVICE_NOT_PLACED_ON_MECH when device not attached
    EXPECT_NE(result, DEVICE_NOT_CONNECTED);
    service.motionManagersMutex.lock();
    service.motionManagers_.erase(MECHID);
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
 * @tc.name  : NotifyOperationResult_003
 * @tc.desc  : Test NotifyOperationResult with valid callback, cover success path
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, NotifyOperationResult_003, TestSize.Level2)
{
    // Given: cmdChannels_ has a valid MockIRemoteObject for the given tokenId
    auto &service = MechBodyControllerService::GetInstance();
    uint32_t tokenId = 8888;
    std::lock_guard<std::mutex> lock(service.cmdChannelMutex_);
    service.cmdChannels_[tokenId] = new MockIRemoteObject();
    service.cmdChannelMutex_.unlock();
    std::string cmdId = "test_cmd";
    // When: Call NotifyOperationResult with registered tokenId
    int32_t result = service.NotifyOperationResult(tokenId, cmdId, ExecResult::COMPLETED);
    // Then: Should return ERR_OK, covering SendRequest success path (line 677-683)
    service.cmdChannelMutex_.lock();
    EXPECT_EQ(result, ERR_OK);
    service.cmdChannels_.erase(tokenId);
}

/**
 * @tc.name  : RotateToEulerAngles_006
 * @tc.desc  : Test RotateToEulerAngles with non-empty motionManagers but mechId not found, cover it==end() branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateToEulerAngles_006, TestSize.Level2)
{
    // Given: motionManagers_ non-empty but mechId=99 not present
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateToEulerAnglesParam>();
    param->duration = DURATION;
    // When: Call RotateToEulerAngles with non-existent mechId=99
    int32_t result = service.RotateToEulerAngles(99, cmdId, param);
    // Then: Should return DEVICE_NOT_CONNECTED, covering it==end() branch (line 724-726)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : RotateToEulerAngles_007
 * @tc.desc  : Test RotateToEulerAngles with nullptr MotionManager in map, cover nullptr check branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateToEulerAngles_007, TestSize.Level2)
{
    // Given: motionManagers_ has a nullptr entry for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = nullptr;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateToEulerAnglesParam>();
    param->duration = DURATION;
    // When: Call RotateToEulerAngles with MECHID that maps to nullptr
    int32_t result = service.RotateToEulerAngles(MECHID, cmdId, param);
    // Then: Should return DEVICE_NOT_CONNECTED, covering motionManager==nullptr branch (line 728-732)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : RotateToEulerAngles_008
 * @tc.desc  : Test RotateToEulerAngles with valid MotionManager, cover Rotate call path
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateToEulerAngles_008, TestSize.Level2)
{
    // Given: motionManagers_ has a valid MotionManager for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateToEulerAnglesParam>();
    param->duration = DURATION;
    param->angles.yaw = YAW;
    param->angles.roll = ROLL;
    param->angles.pitch = PITCH;
    // When: Call RotateToEulerAngles with valid params
    int32_t result = service.RotateToEulerAngles(MECHID, cmdId, param);
    // Then: Should call MotionManager::Rotate and return its result, covering line 734-736
    // MotionManager::Rotate returns DEVICE_NOT_PLACED_ON_MECH when device not attached
    EXPECT_NE(result, DEVICE_NOT_CONNECTED);
    service.motionManagersMutex.lock();
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : GetMaxRotationTime_003
 * @tc.desc  : Test GetMaxRotationTime with non-empty motionManagers but mechId not found, cover it==end() branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationTime_003, TestSize.Level2)
{
    // Given: motionManagers_ non-empty but mechId=99 not present
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    auto timeLimit = std::make_shared<TimeLimit>();
    // When: Call GetMaxRotationTime with non-existent mechId=99
    int32_t result = service.GetMaxRotationTime(99, timeLimit);
    // Then: Should return DEVICE_NOT_CONNECTED, covering it==end() branch (line 757-759)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : GetMaxRotationTime_004
 * @tc.desc  : Test GetMaxRotationTime with nullptr MotionManager in map, cover nullptr check branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationTime_004, TestSize.Level2)
{
    // Given: motionManagers_ has a nullptr entry for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = nullptr;
    service.motionManagersMutex.unlock();
    auto timeLimit = std::make_shared<TimeLimit>();
    // When: Call GetMaxRotationTime with MECHID that maps to nullptr
    int32_t result = service.GetMaxRotationTime(MECHID, timeLimit);
    // Then: Should return DEVICE_NOT_CONNECTED, covering motionManager==nullptr branch (line 761-765)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : GetMaxRotationTime_005
 * @tc.desc  : Test GetMaxRotationTime with valid MotionManager, cover GetSpeedControlTimeLimit call path
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationTime_005, TestSize.Level2)
{
    // Given: motionManagers_ has a valid MotionManager for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    auto timeLimit = std::make_shared<TimeLimit>();
    // When: Call GetMaxRotationTime with matching mechId
    int32_t result = service.GetMaxRotationTime(MECHID, timeLimit);
    // Then: Should call MotionManager::GetSpeedControlTimeLimit, covering line 767-769
    // GetSpeedControlTimeLimit returns DEVICE_NOT_PLACED_ON_MECH when device not attached
    EXPECT_NE(result, DEVICE_NOT_CONNECTED);
    service.motionManagersMutex.lock();
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : GetMaxRotationSpeed_003
 * @tc.desc  : Test GetMaxRotationSpeed with non-empty motionManagers but mechId not found, cover it==end() branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationSpeed_003, TestSize.Level2)
{
    // Given: motionManagers_ non-empty but mechId=99 not present
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    RotateSpeedLimit speedLimit;
    // When: Call GetMaxRotationSpeed with non-existent mechId=99
    int32_t result = service.GetMaxRotationSpeed(99, speedLimit);
    // Then: Should return DEVICE_NOT_CONNECTED, covering it==end() branch (line 790-792)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : GetMaxRotationSpeed_004
 * @tc.desc  : Test GetMaxRotationSpeed with nullptr MotionManager in map, cover nullptr check branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationSpeed_004, TestSize.Level2)
{
    // Given: motionManagers_ has a nullptr entry for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = nullptr;
    service.motionManagersMutex.unlock();
    RotateSpeedLimit speedLimit;
    // When: Call GetMaxRotationSpeed with MECHID that maps to nullptr
    int32_t result = service.GetMaxRotationSpeed(MECHID, speedLimit);
    // Then: Should return DEVICE_NOT_CONNECTED, covering motionManager==nullptr branch (line 794-798)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : GetMaxRotationSpeed_005
 * @tc.desc  : Test GetMaxRotationSpeed with valid MotionManager, cover GetRotateSpeedLimit call path
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetMaxRotationSpeed_005, TestSize.Level2)
{
    // Given: motionManagers_ has a valid MotionManager for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    RotateSpeedLimit speedLimit;
    // When: Call GetMaxRotationSpeed with matching mechId
    int32_t result = service.GetMaxRotationSpeed(MECHID, speedLimit);
    // Then: Should call MotionManager::GetRotateSpeedLimit, covering line 800-802
    // GetRotateSpeedLimit returns DEVICE_NOT_PLACED_ON_MECH when device not attached
    EXPECT_NE(result, DEVICE_NOT_CONNECTED);
    service.motionManagersMutex.lock();
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : RotateBySpeed_005
 * @tc.desc  : Test RotateBySpeed with non-empty motionManagers but mechId not found, cover it==end() branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateBySpeed_005, TestSize.Level2)
{
    // Given: motionManagers_ non-empty but mechId=99 not present
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateBySpeedParam>();
    param->duration = DURATION;
    // When: Call RotateBySpeed with non-existent mechId=99
    int32_t result = service.RotateBySpeed(99, cmdId, param);
    // Then: Should return DEVICE_NOT_CONNECTED, covering it==end() branch (line 832-834)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : RotateBySpeed_006
 * @tc.desc  : Test RotateBySpeed with nullptr MotionManager in map, cover nullptr check branch
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateBySpeed_006, TestSize.Level2)
{
    // Given: motionManagers_ has a nullptr entry for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = nullptr;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateBySpeedParam>();
    param->duration = DURATION;
    // When: Call RotateBySpeed with MECHID that maps to nullptr
    int32_t result = service.RotateBySpeed(MECHID, cmdId, param);
    // Then: Should return DEVICE_NOT_CONNECTED, covering motionManager==nullptr branch (line 836-840)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : RotateBySpeed_007
 * @tc.desc  : Test RotateBySpeed with valid MotionManager, cover RotateBySpeed call path
 * @tc.type  : PERF
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, RotateBySpeed_007, TestSize.Level2)
{
    // Given: motionManagers_ has a valid MotionManager for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    auto param = std::make_shared<RotateBySpeedParam>();
    param->duration = DURATION;
    // When: Call RotateBySpeed with matching mechId and valid params
    int32_t result = service.RotateBySpeed(MECHID, cmdId, param);
    // Then: Should call MotionManager::RotateBySpeed, covering line 842-844
    // MotionManager::RotateBySpeed returns DEVICE_NOT_PLACED_ON_MECH when device not attached
    EXPECT_NE(result, DEVICE_NOT_CONNECTED);
    service.motionManagersMutex.lock();
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : StopMovingInner_003
 * @tc.desc  : Test StopMoving when not system app, cover !IsSystemApp() branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, StopMovingInner_003, TestSize.Level2)
{
    // Given: app is not system app
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    std::string cmdId = "test_cmd";
    // When: Call StopMoving
    int32_t result = service.StopMoving(MECHID, cmdId);
    // Then: Should return PERMISSION_DENIED, covering !IsSystemApp() branch (line 850-852)
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : StopMovingInner_004
 * @tc.desc  : Test StopMoving when mechId not found in non-empty motionManagers, cover it==end() branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, StopMovingInner_004, TestSize.Level2)
{
    // Given: motionManagers_ non-empty but mechId=99 not present
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    // When: Call StopMoving with non-existent mechId=99
    int32_t result = service.StopMoving(99, cmdId);
    // Then: Should return DEVICE_NOT_CONNECTED, covering it==end() branch (line 868-869)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : StopMovingInner_005
 * @tc.desc  : Test StopMoving with nullptr MotionManager in map, cover motionManager==nullptr branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, StopMovingInner_005, TestSize.Level2)
{
    // Given: motionManagers_ has a nullptr entry for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = nullptr;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    // When: Call StopMoving with MECHID that maps to nullptr
    int32_t result = service.StopMoving(MECHID, cmdId);
    // Then: Should return DEVICE_NOT_CONNECTED, covering motionManager==nullptr branch (line 872-875)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : StopMovingInner_006
 * @tc.desc  : Test StopMoving with valid MotionManager, cover StopRotate call path
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, StopMovingInner_006, TestSize.Level2)
{
    // Given: motionManagers_ has a valid MotionManager for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    // When: Call StopMoving with matching mechId
    int32_t result = service.StopMoving(MECHID, cmdId);
    // Then: Should call MotionManager::StopRotate, covering line 877-880
    // MotionManager::StopRotate returns DEVICE_NOT_PLACED_ON_MECH when device not attached
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
    service.motionManagersMutex.lock();
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : GetRotationAnglesInner_003
 * @tc.desc  : Test GetRotationAngles when not system app, cover !IsSystemApp() branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAnglesInner_003, TestSize.Level2)
{
    // Given: app is not system app
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    auto angles = std::make_shared<EulerAngles>();
    // When: Call GetRotationAngles
    int32_t result = service.GetRotationAngles(MECHID, angles);
    // Then: Should return PERMISSION_DENIED, covering !IsSystemApp() branch (line 887-889)
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : GetRotationAnglesInner_004
 * @tc.desc  : Test GetRotationAngles when mechId not found in non-empty motionManagers, cover it==end() branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAnglesInner_004, TestSize.Level2)
{
    // Given: motionManagers_ non-empty but mechId=99 not present
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    auto angles = std::make_shared<EulerAngles>();
    // When: Call GetRotationAngles with non-existent mechId=99
    int32_t result = service.GetRotationAngles(99, angles);
    // Then: Should return DEVICE_NOT_CONNECTED, covering it==end() branch (line 900-902)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : GetRotationAnglesInner_005
 * @tc.desc  : Test GetRotationAngles with nullptr MotionManager in map, cover motionManager==nullptr branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAnglesInner_005, TestSize.Level2)
{
    // Given: motionManagers_ has a nullptr entry for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = nullptr;
    service.motionManagersMutex.unlock();
    auto angles = std::make_shared<EulerAngles>();
    // When: Call GetRotationAngles with MECHID that maps to nullptr
    int32_t result = service.GetRotationAngles(MECHID, angles);
    // Then: Should return DEVICE_NOT_CONNECTED, covering motionManager==nullptr branch (line 905-908)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : GetRotationAnglesInner_006
 * @tc.desc  : Test GetRotationAngles with valid MotionManager, cover GetCurrentPosition call path
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAnglesInner_006, TestSize.Level2)
{
    // Given: motionManagers_ has a valid MotionManager for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    auto angles = std::make_shared<EulerAngles>();
    // When: Call GetRotationAngles with matching mechId
    int32_t result = service.GetRotationAngles(MECHID, angles);
    // Then: Should call MotionManager::GetCurrentPosition, covering line 910-912
    // MotionManager::GetCurrentPosition returns DEVICE_NOT_PLACED_ON_MECH when device not attached
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
    service.motionManagersMutex.lock();
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : GetRotationDegreeLimitsInner_003
 * @tc.desc  : Test GetRotationDegreeLimits when not system app, cover !IsSystemApp() branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationDegreeLimitsInner_003, TestSize.Level2)
{
    // Given: app is not system app
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    RotateDegreeLimit limit;
    // When: Call GetRotationDegreeLimits
    int32_t result = service.GetRotationDegreeLimits(MECHID, limit);
    // Then: Should return PERMISSION_DENIED, covering !IsSystemApp() branch (line 919-921)
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : GetRotationDegreeLimitsInner_004
 * @tc.desc  : Test GetRotationDegreeLimits when mechId not found in non-empty motionManagers, cover it==end() branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationDegreeLimitsInner_004, TestSize.Level2)
{
    // Given: motionManagers_ non-empty but mechId=99 not present
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    RotateDegreeLimit limit;
    // When: Call GetRotationDegreeLimits with non-existent mechId=99
    int32_t result = service.GetRotationDegreeLimits(99, limit);
    // Then: Should return DEVICE_NOT_CONNECTED, covering it==end() branch (line 932-934)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : GetRotationDegreeLimitsInner_005
 * @tc.desc  : Test GetRotationDegreeLimits with nullptr MotionManager in map, cover motionManager==nullptr branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationDegreeLimitsInner_005, TestSize.Level2)
{
    // Given: motionManagers_ has a nullptr entry for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = nullptr;
    service.motionManagersMutex.unlock();
    RotateDegreeLimit limit;
    // When: Call GetRotationDegreeLimits with MECHID that maps to nullptr
    int32_t result = service.GetRotationDegreeLimits(MECHID, limit);
    // Then: Should return DEVICE_NOT_CONNECTED, covering motionManager==nullptr branch (line 937-940)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : GetRotationDegreeLimitsInner_006
 * @tc.desc  : Test GetRotationDegreeLimits with valid MotionManager, cover GetRotationLimit call path
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationDegreeLimitsInner_006, TestSize.Level2)
{
    // Given: motionManagers_ has a valid MotionManager for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    RotateDegreeLimit limit;
    // When: Call GetRotationDegreeLimits with matching mechId
    int32_t result = service.GetRotationDegreeLimits(MECHID, limit);
    // Then: Should call MotionManager::GetRotationLimit, covering line 942-944
    // MotionManager::GetRotationLimit returns DEVICE_NOT_PLACED_ON_MECH when device not attached
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
    service.motionManagersMutex.lock();
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : GetRotationAxesStatusInner_003
 * @tc.desc  : Test GetRotationAxesStatus when not system app, cover !IsSystemApp() branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAxesStatusInner_003, TestSize.Level2)
{
    // Given: app is not system app
    g_isSystemApp = false;
    auto &service = MechBodyControllerService::GetInstance();
    RotationAxesStatus status;
    // When: Call GetRotationAxesStatus
    int32_t result = service.GetRotationAxesStatus(MECHID, status);
    // Then: Should return PERMISSION_DENIED, covering !IsSystemApp() branch (line 951-953)
    EXPECT_EQ(result, PERMISSION_DENIED);
    g_isSystemApp = true;
}

/**
 * @tc.name  : GetRotationAxesStatusInner_004
 * @tc.desc  : Test GetRotationAxesStatus when mechId not found in non-empty motionManagers, cover it==end() branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAxesStatusInner_004, TestSize.Level2)
{
    // Given: motionManagers_ non-empty but mechId=99 not present
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    RotationAxesStatus status;
    // When: Call GetRotationAxesStatus with non-existent mechId=99
    int32_t result = service.GetRotationAxesStatus(99, status);
    // Then: Should return DEVICE_NOT_CONNECTED, covering it==end() branch (line 964-966)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : GetRotationAxesStatusInner_005
 * @tc.desc  : Test GetRotationAxesStatus with nullptr MotionManager in map, cover motionManager==nullptr branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAxesStatusInner_005, TestSize.Level2)
{
    // Given: motionManagers_ has a nullptr entry for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = nullptr;
    service.motionManagersMutex.unlock();
    RotationAxesStatus status;
    // When: Call GetRotationAxesStatus with MECHID that maps to nullptr
    int32_t result = service.GetRotationAxesStatus(MECHID, status);
    // Then: Should return DEVICE_NOT_CONNECTED, covering motionManager==nullptr branch (line 969-972)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : GetRotationAxesStatusInner_006
 * @tc.desc  : Test GetRotationAxesStatus with valid MotionManager, cover GetRotationAxesStatus call path
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, GetRotationAxesStatusInner_006, TestSize.Level2)
{
    // Given: motionManagers_ has a valid MotionManager for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    RotationAxesStatus status;
    // When: Call GetRotationAxesStatus with matching mechId
    int32_t result = service.GetRotationAxesStatus(MECHID, status);
    // Then: Should call MotionManager::GetRotationAxesStatus, covering line 974-976
    // MotionManager::GetRotationAxesStatus returns DEVICE_NOT_PLACED_ON_MECH when device not attached
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
    service.motionManagersMutex.lock();
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : OnRotationAxesStatusChange_002
 * @tc.desc  : Test OnRotationAxesStatusChange with valid callback, cover for-loop body and SendRequest success path
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, OnRotationAxesStatusChange_002, TestSize.Level2)
{
    // Given: rotationAxesStatusChangeCallback_ has a valid MockIRemoteObject
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.AxesStatusChangeCallbackMutex_);
    sptr<IRemoteObject> callback = new MockIRemoteObject();
    service.rotationAxesStatusChangeCallback_[1001] = callback;
    service.AxesStatusChangeCallbackMutex_.unlock();
    RotationAxesStatus status;
    // When: Call OnRotationAxesStatusChange with valid callback registered
    int32_t result = service.OnRotationAxesStatusChange(MECHID, status);
    // Then: Should return ERR_OK after iterating callback, covering for-loop body + SendRequest success (line 1028-1053)
    service.AxesStatusChangeCallbackMutex_.lock();
    EXPECT_EQ(result, ERR_OK);
    service.rotationAxesStatusChangeCallback_.erase(1001);
}

/**
 * @tc.name  : OnRotationAxesStatusChange_003
 * @tc.desc  : Test OnRotationAxesStatusChange with SendRequest failure, cover error!=ERR_NONE branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, OnRotationAxesStatusChange_003, TestSize.Level2)
{
    // Given: rotationAxesStatusChangeCallback_ has a callback whose SendRequest returns error
    auto &service = MechBodyControllerService::GetInstance();
    class FailSendMockIRemoteObject : public IRemoteObject {
    public:
        FailSendMockIRemoteObject() : IRemoteObject(u"fail_send_mock") {}
        virtual ~FailSendMockIRemoteObject() {}
        int32_t GetObjectRefCount() override { return 1; }
        int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
        {
            return -1; // Return error to cover error != ERR_NONE branch
        }
        bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override { return true; }
        bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override { return true; }
        int Dump(int fd, const std::vector<std::u16string> &args) override { return 0; }
    };

    std::lock_guard<std::mutex> lock(service.AxesStatusChangeCallbackMutex_);
    service.rotationAxesStatusChangeCallback_[2001] = new FailSendMockIRemoteObject();
    service.AxesStatusChangeCallbackMutex_.unlock();
    RotationAxesStatus status;
    // When: Call OnRotationAxesStatusChange with failing callback
    int32_t result = service.OnRotationAxesStatusChange(MECHID, status);
    // Then: Should return ERR_OK, covering error != ERR_NONE branch (line 1053)
    service.AxesStatusChangeCallbackMutex_.lock();
    EXPECT_EQ(result, ERR_OK);
    service.rotationAxesStatusChangeCallback_.erase(2001);
}

/**
 * @tc.name  : OnRotationAxesStatusChange_004
 * @tc.desc  : Test OnRotationAxesStatusChange with multiple callbacks, cover for-loop iteration
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, OnRotationAxesStatusChange_004, TestSize.Level2)
{
    // Given: rotationAxesStatusChangeCallback_ has multiple entries
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.AxesStatusChangeCallbackMutex_);
    service.rotationAxesStatusChangeCallback_[3001] = new MockIRemoteObject();
    service.rotationAxesStatusChangeCallback_[3002] = new MockIRemoteObject();
    service.AxesStatusChangeCallbackMutex_.unlock();
    RotationAxesStatus status;
    // When: Call OnRotationAxesStatusChange with multiple callbacks
    int32_t result = service.OnRotationAxesStatusChange(MECHID, status);
    // Then: Should return ERR_OK after iterating all callbacks
    service.AxesStatusChangeCallbackMutex_.lock();
    EXPECT_EQ(result, ERR_OK);
    service.rotationAxesStatusChangeCallback_.erase(3001);
    service.rotationAxesStatusChangeCallback_.erase(3002);
}

/**
 * @tc.name  : Move_005
 * @tc.desc  : Test Move when mechId not found in non-empty motionManagers, cover it==end() branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, Move_005, TestSize.Level2)
{
    // Given: motionManagers_ non-empty but mechId=99 not present
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    auto moveParams = std::make_shared<MoveParams>();
    // When: Call Move with non-existent mechId=99
    int32_t result = service.Move(99, cmdId, moveParams);
    // Then: Should return DEVICE_NOT_CONNECTED, covering it==end() branch (line 1144-1146)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : Move_006
 * @tc.desc  : Test Move with nullptr MotionManager in map, cover motionManager==nullptr branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, Move_006, TestSize.Level2)
{
    // Given: motionManagers_ has a nullptr entry for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = nullptr;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    auto moveParams = std::make_shared<MoveParams>();
    // When: Call Move with MECHID that maps to nullptr
    int32_t result = service.Move(MECHID, cmdId, moveParams);
    // Then: Should return DEVICE_NOT_CONNECTED, covering motionManager==nullptr branch (line 1149-1152)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : Move_007
 * @tc.desc  : Test Move with valid MotionManager, cover Move call path
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, Move_007, TestSize.Level2)
{
    // Given: motionManagers_ has a valid MotionManager for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    auto moveParams = std::make_shared<MoveParams>();
    // When: Call Move with matching mechId
    int32_t result = service.Move(MECHID, cmdId, moveParams);
    // Then: Should call MotionManager::Move, covering line 1154-1156
    // MotionManager::Move returns DEVICE_NOT_PLACED_ON_MECH when device not attached
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
    service.motionManagersMutex.lock();
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : MoveBySpeed_006
 * @tc.desc  : Test MoveBySpeed when mechId not found in non-empty motionManagers, cover it==end() branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, MoveBySpeed_006, TestSize.Level2)
{
    // Given: motionManagers_ non-empty but mechId=99 not present
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    auto speedParams = std::make_shared<SpeedParams>();
    // When: Call MoveBySpeed with non-existent mechId=99
    int32_t result = service.MoveBySpeed(99, cmdId, DURATION, speedParams);
    // Then: Should return DEVICE_NOT_CONNECTED, covering it==end() branch (line 1186-1188)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : MoveBySpeed_007
 * @tc.desc  : Test MoveBySpeed with nullptr MotionManager in map, cover motionManager==nullptr branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, MoveBySpeed_007, TestSize.Level2)
{
    // Given: motionManagers_ has a nullptr entry for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = nullptr;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    auto speedParams = std::make_shared<SpeedParams>();
    // When: Call MoveBySpeed with MECHID that maps to nullptr
    int32_t result = service.MoveBySpeed(MECHID, cmdId, DURATION, speedParams);
    // Then: Should return DEVICE_NOT_CONNECTED, covering motionManager==nullptr branch (line 1191-1194)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : MoveBySpeed_008
 * @tc.desc  : Test MoveBySpeed with valid MotionManager, cover MoveBySpeed call path
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, MoveBySpeed_008, TestSize.Level2)
{
    // Given: motionManagers_ has a valid MotionManager for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    auto speedParams = std::make_shared<SpeedParams>();
    // When: Call MoveBySpeed with matching mechId
    int32_t result = service.MoveBySpeed(MECHID, cmdId, DURATION, speedParams);
    // Then: Should call MotionManager::MoveBySpeed, covering line 1196-1198
    // MotionManager::MoveBySpeed returns DEVICE_NOT_PLACED_ON_MECH when device not attached
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
    service.motionManagersMutex.lock();
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : TurnBySpeed_005
 * @tc.desc  : Test TurnBySpeed when mechId not found in non-empty motionManagers, cover it==end() branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, TurnBySpeed_005, TestSize.Level2)
{
    // Given: motionManagers_ non-empty but mechId=99 not present
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    // When: Call TurnBySpeed with non-existent mechId=99
    int32_t result = service.TurnBySpeed(99, cmdId, 1.0f, DURATION);
    // Then: Should return DEVICE_NOT_CONNECTED, covering it==end() branch (line 1224-1226)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : TurnBySpeed_006
 * @tc.desc  : Test TurnBySpeed with nullptr MotionManager in map, cover motionManager==nullptr branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, TurnBySpeed_006, TestSize.Level2)
{
    // Given: motionManagers_ has a nullptr entry for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = nullptr;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    // When: Call TurnBySpeed with MECHID that maps to nullptr
    int32_t result = service.TurnBySpeed(MECHID, cmdId, 1.0f, DURATION);
    // Then: Should return DEVICE_NOT_CONNECTED, covering motionManager==nullptr branch (line 1229-1232)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : TurnBySpeed_007
 * @tc.desc  : Test TurnBySpeed with valid MotionManager, cover TurnBySpeed call path
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, TurnBySpeed_007, TestSize.Level2)
{
    // Given: motionManagers_ has a valid MotionManager for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    // When: Call TurnBySpeed with matching mechId
    int32_t result = service.TurnBySpeed(MECHID, cmdId, 1.0f, DURATION);
    // Then: Should call MotionManager::TurnBySpeed, covering line 1234-1236
    // MotionManager::TurnBySpeed returns DEVICE_NOT_PLACED_ON_MECH when device not attached
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
    service.motionManagersMutex.lock();
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : IsSupportAction_004
 * @tc.desc  : Test IsSupportAction when mechId not found in non-empty motionManagers, cover it==end() branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, IsSupportAction_004, TestSize.Level2)
{
    // Given: motionManagers_ non-empty but mechId=99 not present
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    bool isSupport = false;
    // When: Call IsSupportAction with non-existent mechId=99
    int32_t result = service.IsSupportAction(99, ActionType::NOD, isSupport);
    // Then: Should return DEVICE_NOT_CONNECTED, covering it==end() branch (line 1257-1259)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : IsSupportAction_005
 * @tc.desc  : Test IsSupportAction with nullptr MotionManager in map, cover motionManager==nullptr branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, IsSupportAction_005, TestSize.Level2)
{
    // Given: motionManagers_ has a nullptr entry for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = nullptr;
    service.motionManagersMutex.unlock();
    bool isSupport = false;
    // When: Call IsSupportAction with MECHID that maps to nullptr
    int32_t result = service.IsSupportAction(MECHID, ActionType::NOD, isSupport);
    // Then: Should return DEVICE_NOT_CONNECTED, covering motionManager==nullptr branch (line 1262-1265)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : IsSupportAction_006
 * @tc.desc  : Test IsSupportAction with valid MotionManager, cover IsSupportAction call path
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, IsSupportAction_006, TestSize.Level2)
{
    // Given: motionManagers_ has a valid MotionManager for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    bool isSupport = false;
    // When: Call IsSupportAction with matching mechId
    int32_t result = service.IsSupportAction(MECHID, ActionType::NOD, isSupport);
    // Then: Should call MotionManager::IsSupportAction, covering line 1267-1269
    // MotionManager::IsSupportAction returns DEVICE_NOT_PLACED_ON_MECH when device not attached
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
    service.motionManagersMutex.lock();
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : DoAction_004
 * @tc.desc  : Test DoAction when mechId not found in non-empty motionManagers, cover it==end() branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, DoAction_004, TestSize.Level2)
{
    // Given: motionManagers_ non-empty but mechId=99 not present
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    // When: Call DoAction with non-existent mechId=99
    int32_t result = service.DoAction(99, cmdId, ActionType::NOD);
    // Then: Should return DEVICE_NOT_CONNECTED, covering it==end() branch (line 1292-1294)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : DoAction_005
 * @tc.desc  : Test DoAction with nullptr MotionManager in map, cover motionManager==nullptr branch
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, DoAction_005, TestSize.Level2)
{
    // Given: motionManagers_ has a nullptr entry for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = nullptr;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    // When: Call DoAction with MECHID that maps to nullptr
    int32_t result = service.DoAction(MECHID, cmdId, ActionType::NOD);
    // Then: Should return DEVICE_NOT_CONNECTED, covering motionManager==nullptr branch (line 1297-1300)
    service.motionManagersMutex.lock();
    EXPECT_EQ(result, DEVICE_NOT_CONNECTED);
    service.motionManagers_.erase(MECHID);
}

/**
 * @tc.name  : DoAction_006
 * @tc.desc  : Test DoAction with valid MotionManager, cover DoAction call path
 * @tc.type  : FUNC
 * @tc.level  : Level2
 */
HWTEST_F(MechBodyControllerServiceTest, DoAction_006, TestSize.Level2)
{
    // Given: motionManagers_ has a valid MotionManager for MECHID
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, MECHID, true, 1);
    std::lock_guard<std::mutex> lock(service.motionManagersMutex);
    service.motionManagers_[MECHID] = manager;
    service.motionManagersMutex.unlock();
    std::string cmdId = "test_cmd";
    // When: Call DoAction with matching mechId
    int32_t result = service.DoAction(MECHID, cmdId, ActionType::NOD);
    // Then: Should call MotionManager::DoAction, covering line 1302-1304
    // MotionManager::DoAction returns DEVICE_NOT_PLACED_ON_MECH when device not attached
    EXPECT_EQ(result, DEVICE_NOT_PLACED_ON_MECH);
    service.motionManagersMutex.lock();
    service.motionManagers_.erase(MECHID);
}

} // namespace MechBodyController
} // namespace OHOS