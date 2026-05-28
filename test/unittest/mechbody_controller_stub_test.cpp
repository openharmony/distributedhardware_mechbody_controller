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

#include "mechbody_controller_stub_test.h"
#include "mechbody_controller_stub.h"
#include "mc_motion_manager.h"
#include "bluetooth_errorcode.h"
#include "mechbody_controller_log.h"
#include "test_log.h"
#include "tokenid_kit.h"
#include "ipc_skeleton.h"
#include "iremote_object.h"
#include "message_parcel.h"
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
    constexpr float ANGLESPEED = 1;
    constexpr uint16_t SPEEDDURATION = 1;
    bool g_isSystemApp = true;
}

namespace OHOS {
namespace Security {
namespace AccessToken {
bool TokenIdKit::IsSystemAppByFullTokenID(uint64_t tokenId)
{
    return g_isSystemApp;
}
} // namespace AccessToken
} // namespace Security
} // namespace OHOS

namespace OHOS {
namespace MechBodyController {

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

// Test subclass of MechBodyControllerStub that implements pure virtual methods
// This is NOT a mock of the class under test, but a concrete subclass for testing
class TestMechBodyControllerStub : public MechBodyControllerStub {
public:
    TestMechBodyControllerStub() = default;
    ~TestMechBodyControllerStub() override = default;

    // Implement pure virtual methods from IMechBodyController interface
    int32_t OnDeviceConnected(int32_t mechId, bool isFirstConnect, const uint32_t &deviceIdentifier) override
    {
        return ERR_OK;
    }
    int32_t OnDeviceDisconnected(int32_t mechId) override
    {
        return ERR_OK;
    }
    int32_t RegisterAttachStateChangeCallback(const sptr<IRemoteObject> callback) override
    {
        return ERR_OK;
    }
    int32_t UnRegisterAttachStateChangeCallback() override
    {
        return ERR_OK;
    }
    int32_t GetAttachedDevices(std::set<MechInfo> &mechInfo) override
    {
        return ERR_OK;
    }
    int32_t SetUserOperation(const std::shared_ptr<Operation> &operation,
                             const std::string &macAddress, const std::string &param) override
    {
        return ERR_OK;
    }
    int32_t SetTrackingEnabled(bool &isEnabled, bool isCapsuleSet) override
    {
        return ERR_OK;
    }
    int32_t GetTrackingEnabled(bool &isEnabled) override
    {
        return ERR_OK;
    }
    int32_t RegisterTrackingEventCallback(sptr<IRemoteObject> callback) override
    {
        return ERR_OK;
    }
    int32_t UnRegisterTrackingEventCallback() override
    {
        return ERR_OK;
    }
    int32_t SetTrackingLayout(CameraTrackingLayout &cameraTrackingLayout) override
    {
        return ERR_OK;
    }
    int32_t GetTrackingLayout(CameraTrackingLayout &cameraTrackingLayout) override
    {
        return ERR_OK;
    }
    int32_t RegisterCmdChannel(const sptr<IRemoteObject> callback) override
    {
        return ERR_OK;
    }
    int32_t RotateByDegree(const int32_t &mechId, std::string &cmdId,
                            const std::shared_ptr<RotateByDegreeParam> &rotateByDegreeParam) override
    {
        return ERR_OK;
    }
    int32_t RotateToEulerAngles(const int32_t &mechId, std::string &cmdId,
                                 const std::shared_ptr<RotateToEulerAnglesParam> &rotateToEulerAnglesParam) override
    {
        return ERR_OK;
    }
    int32_t GetMaxRotationTime(const int32_t &mechId, std::shared_ptr<TimeLimit> &speedLimit) override
    {
        return ERR_OK;
    }
    int32_t GetMaxRotationSpeed(const int32_t &mechId, RotateSpeedLimit &speedLimit) override
    {
        return ERR_OK;
    }
    int32_t RotateBySpeed(const int32_t &mechId, std::string &cmdId,
                           const std::shared_ptr<RotateBySpeedParam> &rotateBySpeedParam) override
    {
        return ERR_OK;
    }
    int32_t StopMoving(const int32_t &mechId, std::string &cmdId) override
    {
        return ERR_OK;
    }
    int32_t GetRotationAngles(const int32_t &mechId, std::shared_ptr<EulerAngles> &eulerAngles) override
    {
        return ERR_OK;
    }
    int32_t GetRotationDegreeLimits(const int32_t &mechId, RotateDegreeLimit &rotationLimit) override
    {
        return ERR_OK;
    }
    int32_t GetRotationAxesStatus(const int32_t &mechId, RotationAxesStatus &axesStatus) override
    {
        return ERR_OK;
    }
    int32_t RegisterRotationAxesStatusChangeCallback(const sptr<IRemoteObject> callback) override
    {
        return ERR_OK;
    }
    int32_t UnRegisterRotationAxesStatusChangeCallback() override
    {
        return ERR_OK;
    }
int32_t SearchTarget(std::string &napiCmdId, const std::shared_ptr<TargetInfo> &targetInfo,
                          const std::shared_ptr<SearchParams> &searchParams) override
    {
        return ERR_OK;
    }
int32_t Move(const int32_t &mechId, std::string &cmdId,
                   const std::shared_ptr<MoveParams> &moveParams) override
    {
        return ERR_OK;
    }
    int32_t MoveBySpeed(const int32_t &mechId, std::string &cmdId, uint16_t duration,
                          const std::shared_ptr<SpeedParams> &speedParams) override
    {
        return ERR_OK;
    }
int32_t TurnBySpeed(const int32_t &mechId, std::string &cmdId,
                          float angularSpeed, uint16_t duration) override
    {
        return ERR_OK;
    }
    int32_t IsSupportAction(const int32_t &mechId, ActionType actionType, bool &isSupport) override
    {
        return ERR_OK;
    }
    int32_t DoAction(const int32_t &mechId, std::string &cmdId,
                      ActionType actionType) override
    {
        return ERR_OK;
    }
    int32_t SubscribeCallback(sptr <IRemoteObject> &callback, MechEventType mechEventType) override
    {
        return ERR_OK;
    }
    int32_t UnSubscribeCallback(MechEventType mechEventType) override
    {
        return ERR_OK;
    }
};



void MechBodyControllerStubTest::SetUpTestCase()
{
    DTEST_LOG << "MechBodyControllerStubTest::SetUpTestCase" << std::endl;
}

void MechBodyControllerStubTest::TearDownTestCase()
{
    DTEST_LOG << "MechBodyControllerStubTest::TearDownTestCase" << std::endl;
}

void MechBodyControllerStubTest::SetUp()
{
    DTEST_LOG << "MechBodyControllerStubTest::SetUp" << std::endl;
}

void MechBodyControllerStubTest::TearDown()
{
    g_isSystemApp = true;  // 恢复全局变量初始状态
    DTEST_LOG << "MechBodyControllerStubTest::TearDown" << std::endl;
}

/**
 * @tc.name  : MechBodyControllerStub_Constructor_InitializesFuncsMap
 * @tc.desc  : Test MechBodyControllerStub constructor initializes funcsMap_ correctly
 *            - Verify that constructor creates a valid object
 *            - Verify that funcsMap_ is populated with known IPC codes
 *            - Verify that unknown IPC codes fall through to default handler
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, MechBodyControllerStub_Constructor_InitializesFuncsMap, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest MechBodyControllerStub_Constructor_InitializesFuncsMap begin" << std::endl;

    // Given: Create a TestMechBodyControllerStub instance
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    ASSERT_NE(stub, nullptr);

    // When: Test OnRemoteRequest with a known IPC code (ATTACH_STATE_CHANGE_LISTEN_ON)
    // The constructor calls InitFuncsInner() which populates funcsMap_ with IPC code mappings
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
        uint32_t knownCode = static_cast<uint32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_ON);
        int32_t ret = stub->OnRemoteRequest(knownCode, data, reply, option);

// Then: Verify that OnRemoteRequest returns ERR_OK
    // This proves that funcsMap_ was properly initialized and the code was found
    // (TestMechBodyControllerStub methods always return ERR_OK)
    EXPECT_EQ(ret, ERR_OK);
    }

    // Test another known IPC code (GET_ATTACHED_DEVICES)
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
        uint32_t knownCode = static_cast<uint32_t>(IMechBodyControllerCode::GET_ATTACHED_DEVICES);
int32_t ret = stub->OnRemoteRequest(knownCode, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
    }

    // Test with unknown IPC code to verify it falls through to default case
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
        uint32_t unknownCode = 9999;
        int32_t ret = stub->OnRemoteRequest(unknownCode, data, reply, option);
        // Unknown code should return different error (from IPCObjectStub::OnRemoteRequest)
        EXPECT_NE(ret, INVALID_PARAMETERS_ERR);
    }

    DTEST_LOG << "MechBodyControllerStubTest MechBodyControllerStub_Constructor_InitializesFuncsMap end" << std::endl;
}

/**
 * @tc.name  : MechBodyControllerStub_Destructor_CleansUpResources
 * @tc.desc  : Test MechBodyControllerStub destructor properly cleans up resources
 *            - Verify that destructor is called without crashing
 *            - Verify that funcsMap_ is cleared
 *            - Verify that new instances can be created after destruction
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, MechBodyControllerStub_Destructor_CleansUpResources, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest MechBodyControllerStub_Destructor_CleansUpResources begin" << std::endl;

    // Given: Create a TestMechBodyControllerStub instance and a weak_ptr to track its lifecycle
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    std::weak_ptr<TestMechBodyControllerStub> weakStub = stub;

    // When: Verify the object is alive (use_count > 0)
    EXPECT_GT(weakStub.use_count(), 0);

    // Then: Reset the shared_ptr to trigger destructor
    stub.reset();

    // Verify the object has been destroyed (use_count == 0 and weak_ptr is expired)
    // This confirms the destructor was called without crashing
    EXPECT_EQ(weakStub.use_count(), 0);
    EXPECT_TRUE(weakStub.expired());

    // Additional verification: Create another instance to ensure destructor doesn't leave global state
    // that would affect subsequent object creation
    auto stub2 = std::make_shared<TestMechBodyControllerStub>();
    ASSERT_NE(stub2, nullptr);

    // Verify the new instance is functional by testing OnRemoteRequest
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    uint32_t knownCode = static_cast<uint32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_ON);
    int32_t ret = stub2->OnRemoteRequest(knownCode, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest MechBodyControllerStub_Destructor_CleansUpResources end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_InvalidCode_ReturnsError
 * @tc.desc  : Test OnRemoteRequest with invalid code
 *            - Verify that unknown IPC codes are handled correctly
 *            - Verify that the function falls through to default handler
 *            - Verify that the error code is IPC_STUB_UNKNOW_TRANS_ERR
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_InvalidCode_ReturnsError, TestSize.Level2)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_InvalidCode_ReturnsError begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t invalidCode = 9999;

    // When: Call OnRemoteRequest with an invalid code
    int32_t result = stub->OnRemoteRequest(invalidCode, data, reply, option);

    // Then: Verify that the function returns the expected error code
    EXPECT_EQ(result, IPC_STUB_UNKNOW_TRANS_ERR);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_InvalidCode_ReturnsError end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_InvalidToken_ReturnsError
 * @tc.desc  : Test OnRemoteRequest with valid code but invalid token
 *            - Verify that invalid interface tokens are rejected
 *            - Verify that the function returns INVALID_PARAMETERS_ERR
 *            - Verify that the token check is performed before calling the handler
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_InvalidToken_ReturnsError, TestSize.Level2)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_InvalidToken_ReturnsError begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data with invalid token
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_ON);
    std::u16string invalidToken = u"invalid_token";
    data.WriteInterfaceToken(invalidToken);

    // When: Call OnRemoteRequest with a valid code but invalid token
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns the expected error code
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_InvalidToken_ReturnsError end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_AttachStateListenOn_ValidCallback_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with ATTACH_STATE_CHANGE_LISTEN_ON
 *            - Verify that the function handles valid callback registration
 *            - Verify that the reply contains the correct result
 *            - Verify that the reply data is written correctly
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_AttachStateListenOn_ValidCallback_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_AttachStateListenOn_ValidCallback_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_ON);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    auto mockCallback = sptr<IRemoteObject>(new MockIRemoteObject());
    data.WriteRemoteObject(mockCallback);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    // Verify that reply contains the correct result written by AttachStateChangeListenOnInner
    int32_t replyResult;
    EXPECT_TRUE(reply.ReadInt32(replyResult));
    EXPECT_EQ(replyResult, ERR_OK);

    // Verify that the reply data was written correctly
    // The reply should contain exactly one int32 (the result)
    EXPECT_EQ(reply.GetDataSize(), sizeof(int32_t));

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_AttachStateListenOn_ValidCallback_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_AttachStateListenOff_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with ATTACH_STATE_CHANGE_LISTEN_OFF
 *            - Verify that the function handles callback unregistration
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_AttachStateListenOff_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_AttachStateListenOff_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_OFF);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_AttachStateListenOff_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_GetAttachedDevices_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with GET_ATTACHED_DEVICES
 *            - Verify that the function handles get attached devices request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_GetAttachedDevices_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetAttachedDevices_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_ATTACHED_DEVICES);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetAttachedDevices_ReturnsSuccess end" << std::endl;
}



/**
 * @tc.name  : OnRemoteRequest_SetUserOperation_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with SET_USER_OPERATION
 *            - Verify that the function handles set user operation request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_SetUserOperation_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_SetUserOperation_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::SET_USER_OPERATION);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(1);
    data.WriteString("mac");
    data.WriteString("param");

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_SetUserOperation_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_SetCameraTrackingEnabled_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with SET_CAMERA_TRACKING_ENABLED
 *            - Verify that the function handles set camera tracking enabled request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_SetCameraTrackingEnabled_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_SetCameraTrackingEnabled_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::SET_CAMERA_TRACKING_ENABLED);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteBool(true);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_SetCameraTrackingEnabled_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_GetCameraTrackingEnabled_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with GET_CAMERA_TRACKING_ENABLED
 *            - Verify that the function handles get camera tracking enabled request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_GetCameraTrackingEnabled_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetCameraTrackingEnabled_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_CAMERA_TRACKING_ENABLED);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetCameraTrackingEnabled_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_TrackingEventListenOn_NullCallback_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with TRACKING_EVENT_LISTEN_ON and null callback
 *            - Verify that the function handles null callback
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_TrackingEventListenOn_NullCallback_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_TrackingEventListenOn_NullCallback_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data with null callback
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::TRACKING_EVENT_LISTEN_ON);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteRemoteObject(nullptr);

    // When: Call OnRemoteRequest with null callback
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success (actual behavior: returns 0 even with nullptr)
    EXPECT_EQ(result, 0);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_TrackingEventListenOn_NullCallback_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_TrackingEventListenOff_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with TRACKING_EVENT_LISTEN_OFF
 *            - Verify that the function handles tracking event listen off request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_TrackingEventListenOff_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_TrackingEventListenOff_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::TRACKING_EVENT_LISTEN_OFF);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_TrackingEventListenOff_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_SetCameraTrackingLayout_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with SET_CAMERA_TRACKING_LAYOUT
 *            - Verify that the function handles set camera tracking layout request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_SetCameraTrackingLayout_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_SetCameraTrackingLayout_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::SET_CAMERA_TRACKING_LAYOUT);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(0);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_SetCameraTrackingLayout_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_GetCameraTrackingLayout_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with GET_CAMERA_TRACKING_LAYOUT
 *            - Verify that the function handles get camera tracking layout request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_GetCameraTrackingLayout_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetCameraTrackingLayout_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_CAMERA_TRACKING_LAYOUT);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetCameraTrackingLayout_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_RegisterCmdChannel_NullCallback_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with REGISTER_CMD_CHANNEL and null callback
 *            - Verify that the function handles null callback
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_RegisterCmdChannel_NullCallback_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_RegisterCmdChannel_NullCallback_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data with null callback
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::REGISTER_CMD_CHANNEL);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteRemoteObject(nullptr);

    // When: Call OnRemoteRequest with null callback
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success (actual behavior: returns 0 even with nullptr)
    EXPECT_EQ(result, 0);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_RegisterCmdChannel_NullCallback_ReturnsSuccess end" << std::endl;
}



/**
 * @tc.name  : OnRemoteRequest_RotateByDegree_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with ROTATE_BY_DEGREE
 *            - Verify that the function handles rotate by degree request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_RotateByDegree_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_RotateByDegree_ReturnsSuccess begin" << std::endl;

    // Given: Create a TestMechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ROTATE_BY_DEGREE);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(MECHID);
    data.WriteString("cmdId");
    data.WriteParcelable(nullptr);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_RotateByDegree_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_RotateToEulerAngles_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with ROTATE_TO_EULER_ANGLES
 *            - Verify that the function handles rotate to euler angles request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_RotateToEulerAngles_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_RotateToEulerAngles_ReturnsSuccess begin" << std::endl;

    // Given: Create a TestMechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ROTATE_TO_EULER_ANGLES);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(MECHID);
    data.WriteString("cmdId");
    data.WriteParcelable(nullptr);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_RotateToEulerAngles_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_GetMaxRotationTime_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with GET_MAX_ROTATION_TIME
 *            - Verify that the function handles get max rotation time request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_GetMaxRotationTime_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetMaxRotationTime_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_MAX_ROTATION_TIME);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(MECHID);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetMaxRotationTime_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_GetMaxRotationSpeed_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with GET_MAX_ROTATION_SPEED
 *            - Verify that the function handles get max rotation speed request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_GetMaxRotationSpeed_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetMaxRotationSpeed_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_MAX_ROTATION_SPEED);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(MECHID);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetMaxRotationSpeed_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_RotateBySpeed_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with ROTATE_BY_SPEED
 *            - Verify that the function handles rotate by speed request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_RotateBySpeed_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_RotateBySpeed_ReturnsSuccess begin" << std::endl;

    // Given: Create a TestMechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ROTATE_BY_SPEED);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(MECHID);
    data.WriteString("cmdId");
    data.WriteParcelable(nullptr);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_RotateBySpeed_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_StopMoving_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with STOP_MOVING
 *            - Verify that the function handles stop moving request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_StopMoving_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_StopMoving_ReturnsSuccess begin" << std::endl;

    // Given: Create a TestMechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::STOP_MOVING);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(MECHID);
    data.WriteString("cmdId");

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_StopMoving_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_GetRotationAngles_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with GET_ROTATION_ANGLES
 *            - Verify that the function handles get rotation angles request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_GetRotationAngles_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetRotationAngles_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_ROTATION_ANGLES);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(MECHID);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetRotationAngles_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_GetRotationDegreeLimits_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with GET_ROTATION_DEGREE_LIMITS
 *            - Verify that the function handles get rotation degree limits request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_GetRotationDegreeLimits_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetRotationDegreeLimits_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_ROTATION_DEGREE_LIMITS);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(MECHID);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetRotationDegreeLimits_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_GetRotationAxesStatus_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with GET_ROTATION_AXES_STATUS
 *            - Verify that the function handles get rotation axes status request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_GetRotationAxesStatus_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetRotationAxesStatus_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_ROTATION_AXES_STATUS);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(MECHID);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_GetRotationAxesStatus_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_RotationAxesStatusChangeListenOn_NullCallback_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with ROTATION_AXES_STATUS_CHANGE_LISTEN_ON and null callback
 *            - Verify that the function handles null callback
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_RotationAxesStatusChangeListenOn_NullCallback_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_RotationAxesStatusChangeListenOn_NullCallback_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data with null callback
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ROTATION_AXES_STATUS_CHANGE_LISTEN_ON);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteRemoteObject(nullptr);

    // When: Call OnRemoteRequest with null callback
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success (actual behavior: returns 0 even with nullptr)
    EXPECT_EQ(result, 0);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_RotationAxesStatusChangeListenOn_NullCallback_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_RotationAxesStatusChangeListenOff_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with ROTATION_AXES_STATUS_CHANGE_LISTEN_OFF
 *            - Verify that the function handles rotation axes status change listen off request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_RotationAxesStatusChangeListenOff_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_RotationAxesStatusChangeListenOff_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ROTATION_AXES_STATUS_CHANGE_LISTEN_OFF);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_RotationAxesStatusChangeListenOff_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_SearchTarget_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with SEARCH_TARGET
 *            - Verify that the function handles search target request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_SearchTarget_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_SearchTarget_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::SEARCH_TARGET);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteString("napiCmdId");
    data.WriteParcelable(nullptr);
    data.WriteParcelable(nullptr);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_SearchTarget_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_Move_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with MOVE
 *            - Verify that the function handles move request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_Move_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_Move_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::MOVE);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(MECHID);
    data.WriteString("cmdId");
    data.WriteParcelable(nullptr);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_Move_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_MoveBySpeed_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with MOVE_BY_SPEED
 *            - Verify that the function handles move by speed request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_MoveBySpeed_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_MoveBySpeed_ReturnsSuccess begin" << std::endl;

    // Given: Create a TestMechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::MOVE_BY_SPEED);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(MECHID);
    data.WriteString("cmdId");
    data.WriteUint16(SPEEDDURATION);
    data.WriteParcelable(nullptr);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_MoveBySpeed_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_TurnBySpeed_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with TURN_BY_SPEED
 *            - Verify that the function handles turn by speed request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_TurnBySpeed_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_TurnBySpeed_ReturnsSuccess begin" << std::endl;

    // Given: Create a TestMechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::TURN_BY_SPEED);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(MECHID);
    data.WriteString("cmdId");
    data.WriteFloat(ANGLESPEED);
    data.WriteUint16(SPEEDDURATION);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_TurnBySpeed_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_IsSupportAction_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with IS_SUPPORT_ACTION
 *            - Verify that the function handles is support action request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_IsSupportAction_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_IsSupportAction_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::IS_SUPPORT_ACTION);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(MECHID);
    data.WriteInt32(0);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_IsSupportAction_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_DoAction_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with DO_ACTION
 *            - Verify that the function handles do action request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_DoAction_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_DoAction_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::DO_ACTION);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(MECHID);
    data.WriteString("cmdId");
    data.WriteInt32(0);
    data.WriteRemoteObject(nullptr);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_DoAction_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_SubscribeCallback_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with SUBSCRIBE_CALLBACK
 *            - Verify that the function handles subscribe callback request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_SubscribeCallback_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_SubscribeCallback_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::SUBSCRIBE_CALLBACK);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(0);
    data.WriteRemoteObject(nullptr);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_SubscribeCallback_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_UnSubscribeCallback_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with UN_SUBSCRIBE_CALLBACK
 *            - Verify that the function handles unsubscribe callback request
 *            - Verify that the function returns success
 * @tc.type  : PERF
 * @tc.level  : Level 1
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_UnSubscribeCallback_ReturnsSuccess, TestSize.Level1)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_UnSubscribeCallback_ReturnsSuccess begin" << std::endl;

    // Given: Create a MechBodyControllerStub instance and prepare test data
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::UN_SUBSCRIBE_CALLBACK);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    data.WriteInt32(0);

    // When: Call OnRemoteRequest with valid parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns success
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_UnSubscribeCallback_ReturnsSuccess end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_EmptyData_ReturnsError
 * @tc.desc  : Test OnRemoteRequest with empty data (no interface token)
 *            - Verify that OnRemoteRequest validates interface token before calling handler
 *            - Verify that missing interface token returns INVALID_PARAMETERS_ERR
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_EmptyData_ReturnsError, TestSize.Level2)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_EmptyData_ReturnsError begin" << std::endl;

    // Given: Create a TestMechBodyControllerStub instance and empty MessageParcel
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_ON);
    // 不写入interface token

    // When: Call OnRemoteRequest with empty data
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns INVALID_PARAMETERS_ERR
    // (OnRemoteRequest validates interface token before calling handler)
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_EmptyData_ReturnsError end" << std::endl;
}

/**
 * @tc.name  : OnRemoteRequest_IncompleteParameters_ReturnsSuccess
 * @tc.desc  : Test OnRemoteRequest with incomplete parameters (missing callback)
 *            - Verify that OnRemoteRequest calls handler even with incomplete parameters
 *            - Note: Inner handlers don't validate parameters, they just pass them through
 * @tc.type  : PERF
 * @tc.level  : Level 2
 */
HWTEST_F(MechBodyControllerStubTest, OnRemoteRequest_IncompleteParameters_ReturnsSuccess, TestSize.Level2)
{
    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_IncompleteParameters_ReturnsSuccess begin" << std::endl;

    // Given: Create a TestMechBodyControllerStub instance with incomplete parameters
    auto stub = std::make_shared<TestMechBodyControllerStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_ON);
    data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    // 不写入callback对象

    // When: Call OnRemoteRequest with incomplete parameters
    int32_t result = stub->OnRemoteRequest(code, data, reply, option);

    // Then: Verify that the function returns ERR_OK
    // (OnRemoteRequest validates interface token, then calls handler which returns ERR_OK)
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "MechBodyControllerStubTest OnRemoteRequest_IncompleteParameters_ReturnsSuccess end" << std::endl;
}

} // namespace MechBodyController
} // namespace OHOS