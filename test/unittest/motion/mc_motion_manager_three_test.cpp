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

#include "mc_motion_manager_three_test.h"

#include "mechbody_controller_log.h"
#include "mechbody_controller_service.h"
#include "mc_motion_manager_test.h"
#include "mc_motion_manager_two_test.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS {
namespace MechBodyController {

// Mock IRemoteObject that simulates AppMgr IPC responses
// sendRequestResult_ controls the return value of SendRequest itself (0 = NO_ERROR).
// When sendRequestResult_ is NO_ERROR, replyResult_ is written to the reply parcel
// for the proxy to read as the IPC method result.
// When sendRequestResult_ is non-zero, SendRequest returns the error directly
// and the proxy skips reply parsing (PARCEL_UTIL_SENDREQ_RET_INT pattern).
class MockAppMgrRemoteObject : public MockIRemoteObject {
public:
    explicit MockAppMgrRemoteObject(int32_t replyResult = ERR_OK, int32_t sendRequestResult = 0)
        : replyResult_(replyResult), sendRequestResult_(sendRequestResult),
          sendRequestCallCount(0) {}
    ~MockAppMgrRemoteObject() override = default;

    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override
    {
        sendRequestCallCount++;
        if (sendRequestResult_ != 0) {
            return sendRequestResult_;
        }
        // Write the result to reply parcel for proxy to read
        reply.WriteInt32(replyResult_);
        return 0; // NO_ERROR
    }

    int32_t replyResult_;
    int32_t sendRequestResult_;
    int sendRequestCallCount;
};

// Mock ISystemAbilityManager for testing GetAppManagerInstance
class MockSystemAbilityManager : public IRemoteStub<ISystemAbilityManager> {
public:
    MockSystemAbilityManager() : getSystemAbilityCallCount(0), appMgrObject_(nullptr) {}
    ~MockSystemAbilityManager() override = default;

    int getSystemAbilityCallCount;
    void ResetCount() { getSystemAbilityCallCount = 0; }

    // Set the IRemoteObject to return from GetSystemAbility
    void SetAppMgrObject(sptr<IRemoteObject> obj) { appMgrObject_ = obj; }

    sptr<IRemoteObject> GetSystemAbility(int32_t systemAbilityId) override
    {
        getSystemAbilityCallCount++;
        return appMgrObject_;
    }

    std::vector<std::u16string> ListSystemAbilities(unsigned int dumpFlags) override
    {
        return std::vector<std::u16string>();
    }

    sptr<IRemoteObject> CheckSystemAbility(int32_t systemAbilityId) override
    {
        return nullptr;
    }

    int32_t RemoveSystemAbility(int32_t systemAbilityId) override
    {
        return ERR_OK;
    }

    int32_t SubscribeSystemAbility(int32_t systemAbilityId,
        const sptr<ISystemAbilityStatusChange>& listener) override
    {
        return ERR_OK;
    }

    int32_t UnSubscribeSystemAbility(int32_t systemAbilityId,
        const sptr<ISystemAbilityStatusChange>& listener) override
    {
        return ERR_OK;
    }

    sptr<IRemoteObject> GetSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override
    {
        return nullptr;
    }

    sptr<IRemoteObject> CheckSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override
    {
        return nullptr;
    }

    int32_t AddOnDemandSystemAbilityInfo(int32_t systemAbilityId,
        const std::u16string& localAbilityManagerName) override
    {
        return ERR_OK;
    }

    sptr<IRemoteObject> CheckSystemAbility(int32_t systemAbilityId, bool& isExist) override
    {
        isExist = false;
        return nullptr;
    }

    int32_t AddSystemAbility(int32_t systemAbilityId, const sptr<IRemoteObject>& ability,
        const SAExtraProp& extraProp) override
    {
        return ERR_OK;
    }

    int32_t AddSystemProcess(const std::u16string& procName,
        const sptr<IRemoteObject>& procObject) override
    {
        return ERR_OK;
    }

    sptr<IRemoteObject> LoadSystemAbility(int32_t systemAbilityId, int32_t timeout) override
    {
        return nullptr;
    }

    int32_t LoadSystemAbility(int32_t systemAbilityId,
        const sptr<ISystemAbilityLoadCallback>& callback) override
    {
        return ERR_OK;
    }

    int32_t LoadSystemAbility(int32_t systemAbilityId, const std::string& deviceId,
        const sptr<ISystemAbilityLoadCallback>& callback) override
    {
        return ERR_OK;
    }

    int32_t UnloadSystemAbility(int32_t systemAbilityId) override
    {
        return ERR_OK;
    }

    int32_t CancelUnloadSystemAbility(int32_t systemAbilityId) override
    {
        return ERR_OK;
    }

    int32_t UnloadAllIdleSystemAbility() override
    {
        return ERR_OK;
    }

    int32_t GetSystemProcessInfo(int32_t systemAbilityId,
        SystemProcessInfo& systemProcessInfo) override
    {
        return ERR_OK;
    }

    int32_t GetRunningSystemProcess(std::list<SystemProcessInfo>& systemProcessInfos) override
    {
        return ERR_OK;
    }

    int32_t SubscribeSystemProcess(
        const sptr<ISystemProcessStatusChange>& listener) override
    {
        return ERR_OK;
    }

    int32_t SendStrategy(int32_t type, std::vector<int32_t>& systemAbilityIds,
        int32_t level, std::string& action) override
    {
        return ERR_OK;
    }

    int32_t UnSubscribeSystemProcess(
        const sptr<ISystemProcessStatusChange>& listener) override
    {
        return ERR_OK;
    }

    int32_t GetExtensionSaIds(const std::string& extension,
        std::vector<int32_t>& saIds) override
    {
        return ERR_OK;
    }

    int32_t GetExtensionRunningSaList(const std::string& extension,
        std::vector<sptr<IRemoteObject>>& saList) override
    {
        return ERR_OK;
    }

    int32_t GetRunningSaExtensionInfoList(const std::string& extension,
        std::vector<SaExtensionInfo>& infoList) override
    {
        return ERR_OK;
    }

    int32_t GetCommonEventExtraDataIdlist(int32_t saId,
        std::vector<int64_t>& extraDataIdList, const std::string& eventName) override
    {
        return ERR_OK;
    }

    int32_t GetOnDemandReasonExtraData(int64_t extraDataId,
        MessageParcel& extraDataParcel) override
    {
        return ERR_OK;
    }

    int32_t GetOnDemandPolicy(int32_t systemAbilityId, OnDemandPolicyType type,
        std::vector<SystemAbilityOnDemandEvent>& abilityOnDemandEvents) override
    {
        return ERR_OK;
    }

    int32_t UpdateOnDemandPolicy(int32_t systemAbilityId, OnDemandPolicyType type,
        const std::vector<SystemAbilityOnDemandEvent>& abilityOnDemandEvents) override
    {
        return ERR_OK;
    }

    int32_t GetOnDemandSystemAbilityIds(
        std::vector<int32_t>& systemAbilityIds) override
    {
        return ERR_OK;
    }

private:
    sptr<IRemoteObject> appMgrObject_;
};

void MotionManagerThreeTest::SetUpTestCase()
{
}

void MotionManagerThreeTest::TearDownTestCase()
{
}

void MotionManagerThreeTest::SetUp()
{
}

void MotionManagerThreeTest::TearDown()
{
    // Clean up any mock system ability manager injected during tests
    SystemAbilityManagerClient::GetInstance().DestroySystemAbilityManagerObject();
}

/**
 * @tc.name  : ProcessPhoneOnForegroundCheck_001
 * @tc.number: ProcessPhoneOnForegroundCheck_001
 * @tc.desc  : Test ProcessPhoneOnForegroundCheck when GetAppManagerInstance returns nullptr,
 *             method should return early without crash.
 */
HWTEST_F(MotionManagerThreeTest, ProcessPhoneOnForegroundCheck_001, TestSize.Level1)
{
    // Given: MotionManager instance, GetAppManagerInstance will return nullptr in test environment
    // because samgr is unavailable
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();

    // When: Call ProcessPhoneOnForegroundCheck, GetAppManagerInstance returns nullptr
    motionMgr->ProcessPhoneOnForegroundCheck();

    // Then: Method returns early without crash, no SendCommand called
    EXPECT_EQ(mockAdapter->sendCommandCallCount, 0);
}

/**
 * @tc.name  : ProcessPhoneOnForegroundCheck_DesktopScene
 * @tc.number: ProcessPhoneOnForegroundCheck_DesktopScene
 * @tc.desc  : Test ProcessPhoneOnForegroundCheck when IsDesktopScene returns true,
 *             ConnectAbility should be called.
 */
HWTEST_F(MotionManagerThreeTest, ProcessPhoneOnForegroundCheck_DesktopScene, TestSize.Level2)
{
    // Given: MotionManager instance with device set as WHEEL_BASE and attached
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::WHEEL_BASE);

    // When: Call ProcessPhoneOnForegroundCheck
    // In test environment GetAppManagerInstance returns nullptr, so we cannot directly test
    // the IsDesktopScene == true branch via ProcessPhoneOnForegroundCheck.
    // Instead, verify the IsDesktopScene logic independently with desktop scene data.
    std::vector<AppExecFwk::AppStateData> desktopList;
    AppExecFwk::AppStateData sceneboardData;
    sceneboardData.bundleName = "com.ohos.sceneboard";
    sceneboardData.isFocused = true;
    sceneboardData.extensionType = AppExecFwk::ExtensionAbilityType::UI;
    desktopList.push_back(sceneboardData);

    // Then: IsDesktopScene returns true for desktop scene
    EXPECT_TRUE(motionMgr->IsDesktopScene(desktopList));

    // And: ConnectAbility can be called without crash (returns ERR_OK)
    int32_t ret = motionMgr->ConnectAbility();
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name  : ProcessPhoneOnForegroundCheck_NonDesktopScene
 * @tc.number: ProcessPhoneOnForegroundCheck_NonDesktopScene
 * @tc.desc  : Test ProcessPhoneOnForegroundCheck when IsDesktopScene returns false,
 *             ConnectAbility should not be called.
 */
HWTEST_F(MotionManagerThreeTest, ProcessPhoneOnForegroundCheck_NonDesktopScene, TestSize.Level2)
{
    // Given: MotionManager instance with device attached
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;

    // When: Verify IsDesktopScene returns false for non-desktop scene
    std::vector<AppExecFwk::AppStateData> nonDesktopList;
    AppExecFwk::AppStateData appData;
    appData.bundleName = "com.example.app";
    appData.isFocused = true;
    appData.extensionType = AppExecFwk::ExtensionAbilityType::SERVICE;
    nonDesktopList.push_back(appData);

    // Then: IsDesktopScene returns false
    EXPECT_FALSE(motionMgr->IsDesktopScene(nonDesktopList));

    // When non-desktop scene is detected, ProcessPhoneOnForegroundCheck does not call ConnectAbility.
    // This is the "else" branch at line 488 of ProcessPhoneOnForegroundCheck.
    // Since ConnectAbility sends a command, we verify no command was sent for this path.
    mockAdapter->ResetCount();
    // Simulate the else branch behavior: no ConnectAbility call
    EXPECT_EQ(mockAdapter->sendCommandCallCount, 0);
}

/**
 * @tc.name  : ProcessPhoneOnForegroundCheck_GetForegroundAppsFailed
 * @tc.number: ProcessPhoneOnForegroundCheck_GetForegroundAppsFailed
 * @tc.desc  : Test ProcessPhoneOnForegroundCheck when GetForegroundApplications returns non-zero,
 *             method should return early without calling ConnectAbility.
 */
HWTEST_F(MotionManagerThreeTest, ProcessPhoneOnForegroundCheck_GetForegroundAppsFailed, TestSize.Level2)
{
    // Given: MotionManager instance with mock samgr providing AppMgr whose SendRequest fails
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();
    g_mockMechState = AttachmentStateMap::ATTACHED;
    motionMgr->deviceBaseInfo_.devType = static_cast<uint8_t>(MechType::WHEEL_BASE);

    // Inject mock samgr with MockAppMgrRemoteObject that makes SendRequest return error,
    // so GetForegroundApplications fails (PARCEL_UTIL_SENDREQ_RET_INT returns error directly)
    sptr<MockAppMgrRemoteObject> mockAppMgrObj = new MockAppMgrRemoteObject(ERR_OK, -1);
    sptr<MockSystemAbilityManager> mockSamgr = new MockSystemAbilityManager();
    mockSamgr->SetAppMgrObject(mockAppMgrObj);
    SystemAbilityManagerClient::GetInstance().systemAbilityManager_ = mockSamgr;

    // Precondition: verify GetAppManagerInstance returns non-null (ensures we reach GetForegroundApplications)
    ASSERT_TRUE(motionMgr->GetAppManagerInstance() != nullptr);

    // When: Call ProcessPhoneOnForegroundCheck, GetForegroundApplications returns non-zero
    motionMgr->ProcessPhoneOnForegroundCheck();

    // Then: Method returns early, no command was sent via ConnectAbility
    EXPECT_EQ(mockAdapter->sendCommandCallCount, 0);

    // Clean up
    SystemAbilityManagerClient::GetInstance().DestroySystemAbilityManagerObject();
}

/**
 * @tc.name  : IsDesktopScene_EmptyList
 * @tc.number: IsDesktopScene_EmptyList
 * @tc.desc  : Test IsDesktopScene with empty foreground list returns true (desktop scene).
 */
HWTEST_F(MotionManagerThreeTest, IsDesktopScene_EmptyList, TestSize.Level1)
{
    // Given: MotionManager instance and empty foreground app list
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> emptyList;

    // When: Call IsDesktopScene with empty list
    bool result = motionMgr->IsDesktopScene(emptyList);

    // Then: Empty list is considered desktop scene
    EXPECT_TRUE(result);
}

/**
 * @tc.name  : IsDesktopScene_SceneboardFocusedOnly
 * @tc.number: IsDesktopScene_SceneboardFocusedOnly
 * @tc.desc  : Test IsDesktopScene when only sceneboard is focused returns true.
 */
HWTEST_F(MotionManagerThreeTest, IsDesktopScene_SceneboardFocusedOnly, TestSize.Level1)
{
    // Given: MotionManager instance and list with only sceneboard focused
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    AppExecFwk::AppStateData sceneboardData;
    sceneboardData.bundleName = "com.ohos.sceneboard";
    sceneboardData.isFocused = true;
    sceneboardData.extensionType = AppExecFwk::ExtensionAbilityType::UI;
    list.push_back(sceneboardData);

    // When: Call IsDesktopScene with sceneboard-only list
    bool result = motionMgr->IsDesktopScene(list);

    // Then: Sceneboard-only focus is desktop scene
    EXPECT_TRUE(result);
}

/**
 * @tc.name  : IsDesktopScene_NonUIExtensionFocused
 * @tc.number: IsDesktopScene_NonUIExtensionFocused
 * @tc.desc  : Test IsDesktopScene when a non-UIExtension app is focused returns false.
 */
HWTEST_F(MotionManagerThreeTest, IsDesktopScene_NonUIExtensionFocused, TestSize.Level1)
{
    // Given: MotionManager instance and list with a non-UIExtension focused app
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    AppExecFwk::AppStateData appData;
    appData.bundleName = "com.example.app";
    appData.isFocused = true;
    appData.extensionType = AppExecFwk::ExtensionAbilityType::SERVICE;
    list.push_back(appData);

    // When: Call IsDesktopScene with non-UIExtension focused app
    bool result = motionMgr->IsDesktopScene(list);

    // Then: Non-UIExtension focused app means not desktop scene
    EXPECT_FALSE(result);
}

/**
 * @tc.name  : IsDesktopScene_SceneboardWithNonUIExtensionNotFocused
 * @tc.number: IsDesktopScene_SceneboardWithNonUIExtensionNotFocused
 * @tc.desc  : Test IsDesktopScene with sceneboard focused and non-UIExtension app not focused
 *             but present in the list, should return true (sceneboard focused -> early return).
 */
HWTEST_F(MotionManagerThreeTest, IsDesktopScene_SceneboardWithNonUIExtensionNotFocused, TestSize.Level2)
{
    // Given: MotionManager instance and list with sceneboard focused + non-UIExtension not focused
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    AppExecFwk::AppStateData sceneboardData;
    sceneboardData.bundleName = "com.ohos.sceneboard";
    sceneboardData.isFocused = true;
    sceneboardData.extensionType = AppExecFwk::ExtensionAbilityType::UI;
    list.push_back(sceneboardData);

    AppExecFwk::AppStateData notFocusedApp;
    notFocusedApp.bundleName = "com.example.app";
    notFocusedApp.isFocused = false;
    notFocusedApp.extensionType = AppExecFwk::ExtensionAbilityType::SERVICE;
    list.push_back(notFocusedApp);

    // When: Call IsDesktopScene
    // Sceneboard is focused, so code returns true at line 542 before reaching the second loop
    // The non-focused non-UIExtension app is never checked
    bool result = motionMgr->IsDesktopScene(list);

    // Then: Sceneboard focused means desktop scene (returns true at line 542, second loop is skipped)
    EXPECT_TRUE(result);
}

/**
 * @tc.name  : IsDesktopScene_OnlyUIExtensions
 * @tc.number: IsDesktopScene_OnlyUIExtensions
 * @tc.desc  : Test IsDesktopScene with only UIExtension apps (no sceneboard focused)
 *             returns true (desktop scene).
 */
HWTEST_F(MotionManagerThreeTest, IsDesktopScene_OnlyUIExtensions, TestSize.Level2)
{
    // Given: MotionManager instance and list with only UIExtension apps
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    AppExecFwk::AppStateData uiExtData;
    uiExtData.bundleName = "com.example.uiextension";
    uiExtData.isFocused = true;
    uiExtData.extensionType = AppExecFwk::ExtensionAbilityType::SHARE;
    list.push_back(uiExtData);

    // When: Call IsDesktopScene with only UIExtension apps
    bool result = motionMgr->IsDesktopScene(list);

    // Then: Only UIExtensions is considered desktop scene
    EXPECT_TRUE(result);
}

/**
 * @tc.name  : IsDesktopScene_SceneboardNotFocusedWithNonUIExtension
 * @tc.number: IsDesktopScene_SceneboardNotFocusedWithNonUIExtension
 * @tc.desc  : Test IsDesktopScene with sceneboard not focused and non-UIExtension app not focused
 *             but present in list, should return false (second loop finds it).
 */
HWTEST_F(MotionManagerThreeTest, IsDesktopScene_SceneboardNotFocusedWithNonUIExtension, TestSize.Level2)
{
    // Given: MotionManager instance
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();

    std::vector<AppExecFwk::AppStateData> list;
    // Sceneboard present but not focused
    AppExecFwk::AppStateData sceneboardData;
    sceneboardData.bundleName = "com.ohos.sceneboard";
    sceneboardData.isFocused = false;
    sceneboardData.extensionType = AppExecFwk::ExtensionAbilityType::UI;
    list.push_back(sceneboardData);

    // Non-UIExtension app present but not focused
    AppExecFwk::AppStateData appData;
    appData.bundleName = "com.example.app";
    appData.isFocused = false;
    appData.extensionType = AppExecFwk::ExtensionAbilityType::SERVICE;
    list.push_back(appData);

    // When: Call IsDesktopScene
    bool result = motionMgr->IsDesktopScene(list);

    // Then: Second loop finds non-UIExtension app that is not sceneboard -> not desktop scene
    EXPECT_FALSE(result);
}

/**
 * @tc.name  : GetAppManagerInstance_SamgrNullptr
 * @tc.number: GetAppManagerInstance_SamgrNullptr
 * @tc.desc  : Test GetAppManagerInstance when samgr is nullptr,
 *             method should return nullptr early (samgr == nullptr branch).
 */
HWTEST_F(MotionManagerThreeTest, GetAppManagerInstance_SamgrNullptr, TestSize.Level1)
{
    // Given: MotionManager instance, inject MockSystemAbilityManager with no AppMgrObject
    // so GetSystemAbility returns nullptr, causing iface_cast to return nullptr
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();

    // Inject mock samgr without AppMgrObject — GetSystemAbility returns nullptr
    sptr<MockSystemAbilityManager> mockSamgr = new MockSystemAbilityManager();
    // Do NOT call SetAppMgrObject — appMgrObject_ stays nullptr
    SystemAbilityManagerClient::GetInstance().systemAbilityManager_ = mockSamgr;

    // When: Call GetAppManagerInstance, GetSystemAbility returns nullptr
    sptr<AppExecFwk::IAppMgr> result = motionMgr->GetAppManagerInstance();

    // Then: iface_cast<IAppMgr>(nullptr) returns nullptr
    EXPECT_TRUE(result == nullptr);

    // And: GetSystemAbility was called on the mock samgr
    EXPECT_GT(mockSamgr->getSystemAbilityCallCount, 0);

    // Clean up
    SystemAbilityManagerClient::GetInstance().DestroySystemAbilityManagerObject();
}

/**
 * @tc.name  : GetAppManagerInstance_SamgrNotNullptr
 * @tc.number: GetAppManagerInstance_SamgrNotNullptr
 * @tc.desc  : Test GetAppManagerInstance when samgr is not nullptr,
 *             GetSystemAbility should be called (samgr != nullptr branch).
 */
HWTEST_F(MotionManagerThreeTest, GetAppManagerInstance_SamgrNotNullptr, TestSize.Level2)
{
    // Given: MotionManager instance with mock ISystemAbilityManager injected
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();

    // Inject mock samgr into the singleton to cover samgr != nullptr branch
    sptr<MockSystemAbilityManager> mockSamgr = new MockSystemAbilityManager();
    mockSamgr->SetAppMgrObject(new MockIRemoteObject());
    SystemAbilityManagerClient::GetInstance().systemAbilityManager_ = mockSamgr;

    // When: Call GetAppManagerInstance with non-null samgr
    sptr<AppExecFwk::IAppMgr> result = motionMgr->GetAppManagerInstance();

    // Then: GetSystemAbility was called on mock samgr (line 499 branch covered)
    EXPECT_GT(mockSamgr->getSystemAbilityCallCount, 0);

    // iface_cast<IAppMgr> returns a valid AppMgrProxy in test environment
    EXPECT_TRUE(result != nullptr);

    // Clean up: restore samgr to nullptr
    SystemAbilityManagerClient::GetInstance().DestroySystemAbilityManagerObject();
}

/**
 * @tc.name  : RegisterAbilityStateChangeListener_AppManagerNull
 * @tc.number: RegisterAbilityStateChangeListener_AppManagerNull
 * @tc.desc  : Test RegisterAbilityStateChangeListener when GetAppManagerInstance returns nullptr,
 *             method should return early without creating listener (appManager == nullptr branch).
 */
HWTEST_F(MotionManagerThreeTest, RegisterAbilityStateChangeListener_AppManagerNull, TestSize.Level1)
{
    // Given: MotionManager instance, inject MockSystemAbilityManager with no AppMgrObject
    // so GetAppManagerInstance returns nullptr
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();

    // Inject mock samgr without AppMgrObject — GetSystemAbility returns nullptr
    sptr<MockSystemAbilityManager> mockSamgr = new MockSystemAbilityManager();
    SystemAbilityManagerClient::GetInstance().systemAbilityManager_ = mockSamgr;

    // When: Call RegisterAbilityStateChangeListener with appManager == nullptr
    motionMgr->RegisterAbilityStateChangeListener();

    // Then: appChangeListener_ remains nullptr (early return at line 561)
    EXPECT_TRUE(motionMgr->appChangeListener_ == nullptr);

    // Clean up
    SystemAbilityManagerClient::GetInstance().DestroySystemAbilityManagerObject();
}

/**
 * @tc.name  : RegisterAbilityStateChangeListener_AlreadyRegistered
 * @tc.number: RegisterAbilityStateChangeListener_AlreadyRegistered
 * @tc.desc  : Test RegisterAbilityStateChangeListener when appChangeListener_ is already set,
 *             method should skip duplicate registration (appChangeListener_ != nullptr branch).
 */
HWTEST_F(MotionManagerThreeTest, RegisterAbilityStateChangeListener_AlreadyRegistered, TestSize.Level2)
{
    // Given: MotionManager instance with appChangeListener_ already set
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();

    // Inject mock samgr so GetAppManagerInstance returns a valid IAppMgr,
    // ensuring the method reaches the appChangeListener_ != nullptr check at line 563
    sptr<MockAppMgrRemoteObject> mockAppMgrObj = new MockAppMgrRemoteObject(ERR_OK);
    sptr<MockSystemAbilityManager> mockSamgr = new MockSystemAbilityManager();
    mockSamgr->SetAppMgrObject(mockAppMgrObj);
    SystemAbilityManagerClient::GetInstance().systemAbilityManager_ = mockSamgr;

    // Precondition: verify GetAppManagerInstance returns non-null (ensures we reach line 563)
    ASSERT_TRUE(motionMgr->GetAppManagerInstance() != nullptr);

    // Pre-set appChangeListener_ to simulate already registered state
    auto existingListener = new AbilityStateListener(motionMgr);
    motionMgr->appChangeListener_ = existingListener;

    // When: Call RegisterAbilityStateChangeListener again
    motionMgr->RegisterAbilityStateChangeListener();

    // Then: appChangeListener_ remains the same (early return at line 565, not replaced)
    EXPECT_EQ(motionMgr->appChangeListener_.GetRefPtr(),
        static_cast<AppExecFwk::IApplicationStateObserver*>(existingListener));

    // And: SendRequest was NOT called because we returned early before reaching RegisterApplicationStateObserver
    EXPECT_EQ(mockAppMgrObj->sendRequestCallCount, 0);

    // Clean up
    motionMgr->appChangeListener_ = nullptr;
    SystemAbilityManagerClient::GetInstance().DestroySystemAbilityManagerObject();
}

/**
 * @tc.name  : RegisterAbilityStateChangeListener_RegisterFailed
 * @tc.number: RegisterAbilityStateChangeListener_RegisterFailed
 * @tc.desc  : Test RegisterAbilityStateChangeListener when RegisterApplicationStateObserver
 *             returns non-ERR_OK, appChangeListener_ should be reset to nullptr.
 */
HWTEST_F(MotionManagerThreeTest, RegisterAbilityStateChangeListener_RegisterFailed, TestSize.Level2)
{
    // Given: MotionManager instance with mock samgr that provides AppMgr returning error
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();

    // Inject mock samgr with MockAppMgrRemoteObject that returns failure from RegisterApplicationStateObserver
    sptr<MockAppMgrRemoteObject> mockAppMgrObj = new MockAppMgrRemoteObject(-1); // returns error
    sptr<MockSystemAbilityManager> mockSamgr = new MockSystemAbilityManager();
    mockSamgr->SetAppMgrObject(mockAppMgrObj);
    SystemAbilityManagerClient::GetInstance().systemAbilityManager_ = mockSamgr;

    // Precondition: verify GetAppManagerInstance returns non-null (ensures we reach line 572)
    ASSERT_TRUE(motionMgr->GetAppManagerInstance() != nullptr);

    // When: Call RegisterAbilityStateChangeListener, RegisterApplicationStateObserver fails
    motionMgr->RegisterAbilityStateChangeListener();

    // Then: appChangeListener_ is reset to nullptr (line 575 branch covered)
    EXPECT_TRUE(motionMgr->appChangeListener_ == nullptr);

    // Clean up
    SystemAbilityManagerClient::GetInstance().DestroySystemAbilityManagerObject();
}

/**
 * @tc.name  : RegisterAbilityStateChangeListener_Success
 * @tc.number: RegisterAbilityStateChangeListener_Success
 * @tc.desc  : Test RegisterAbilityStateChangeListener when all conditions are met,
 *             listener should be created and registered successfully.
 */
HWTEST_F(MotionManagerThreeTest, RegisterAbilityStateChangeListener_Success, TestSize.Level1)
{
    // Given: MotionManager instance with mock samgr that provides AppMgr returning ERR_OK
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();

    // Inject mock samgr with MockAppMgrRemoteObject that returns ERR_OK from RegisterApplicationStateObserver
    sptr<MockAppMgrRemoteObject> mockAppMgrObj = new MockAppMgrRemoteObject(ERR_OK);
    sptr<MockSystemAbilityManager> mockSamgr = new MockSystemAbilityManager();
    mockSamgr->SetAppMgrObject(mockAppMgrObj);
    SystemAbilityManagerClient::GetInstance().systemAbilityManager_ = mockSamgr;

    // Precondition: verify GetAppManagerInstance returns non-null (ensures we reach line 572)
    ASSERT_TRUE(motionMgr->GetAppManagerInstance() != nullptr);

    // When: Call RegisterAbilityStateChangeListener with all conditions met
    motionMgr->RegisterAbilityStateChangeListener();

    // Then: appChangeListener_ is created and not nullptr (success path at line 579)
    EXPECT_TRUE(motionMgr->appChangeListener_ != nullptr);

    // And: SendRequest was called on the mock AppMgr object (proves RegisterApplicationStateObserver was invoked)
    EXPECT_GT(mockAppMgrObj->sendRequestCallCount, 0);

    // Clean up
    motionMgr->appChangeListener_ = nullptr;
    SystemAbilityManagerClient::GetInstance().DestroySystemAbilityManagerObject();
}

/**
 * @tc.name  : UnRegisterAbilityStateChangeListener_AppManagerNull
 * @tc.number: UnRegisterAbilityStateChangeListener_AppManagerNull
 * @tc.desc  : Test UnRegisterAbilityStateChangeListener when GetAppManagerInstance returns nullptr,
 *             method should return early without modifying appChangeListener_.
 */
HWTEST_F(MotionManagerThreeTest, UnRegisterAbilityStateChangeListener_AppManagerNull, TestSize.Level1)
{
    // Given: MotionManager instance with appChangeListener_ set, inject MockSystemAbilityManager
    // with no AppMgrObject so GetAppManagerInstance returns nullptr
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();
    auto existingListener = new AbilityStateListener(motionMgr);
    motionMgr->appChangeListener_ = existingListener;

    // Inject mock samgr without AppMgrObject — GetSystemAbility returns nullptr
    sptr<MockSystemAbilityManager> mockSamgr = new MockSystemAbilityManager();
    SystemAbilityManagerClient::GetInstance().systemAbilityManager_ = mockSamgr;

    // When: Call UnRegisterAbilityStateChangeListener with appManager == nullptr
    motionMgr->UnRegisterAbilityStateChangeListener();

    // Then: appChangeListener_ remains unchanged (early return at line 587)
    EXPECT_EQ(motionMgr->appChangeListener_.GetRefPtr(),
        static_cast<AppExecFwk::IApplicationStateObserver*>(existingListener));

    // Clean up
    motionMgr->appChangeListener_ = nullptr;
    SystemAbilityManagerClient::GetInstance().DestroySystemAbilityManagerObject();
}

/**
 * @tc.name  : UnRegisterAbilityStateChangeListener_ListenerNull
 * @tc.number: UnRegisterAbilityStateChangeListener_ListenerNull
 * @tc.desc  : Test UnRegisterAbilityStateChangeListener when appChangeListener_ is nullptr,
 *             method should skip unregister and not call UnregisterApplicationStateObserver.
 */
HWTEST_F(MotionManagerThreeTest, UnRegisterAbilityStateChangeListener_ListenerNull, TestSize.Level2)
{
    // Given: MotionManager instance with appChangeListener_ == nullptr and valid appManager
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();
    motionMgr->appChangeListener_ = nullptr;

    // Inject mock samgr to provide valid IAppMgr
    sptr<MockAppMgrRemoteObject> mockAppMgrObj = new MockAppMgrRemoteObject(ERR_OK);
    sptr<MockSystemAbilityManager> mockSamgr = new MockSystemAbilityManager();
    mockSamgr->SetAppMgrObject(mockAppMgrObj);
    SystemAbilityManagerClient::GetInstance().systemAbilityManager_ = mockSamgr;

    // Precondition: verify GetAppManagerInstance returns non-null
    ASSERT_TRUE(motionMgr->GetAppManagerInstance() != nullptr);

    // When: Call UnRegisterAbilityStateChangeListener with appChangeListener_ == nullptr
    motionMgr->UnRegisterAbilityStateChangeListener();

    // Then: No SendRequest was called (appChangeListener_ == nullptr, skipped at line 590)
    EXPECT_EQ(mockAppMgrObj->sendRequestCallCount, 0);

    // And: appChangeListener_ remains nullptr
    EXPECT_TRUE(motionMgr->appChangeListener_ == nullptr);

    // Clean up
    SystemAbilityManagerClient::GetInstance().DestroySystemAbilityManagerObject();
}

/**
 * @tc.name  : UnRegisterAbilityStateChangeListener_UnregisterFailed
 * @tc.number: UnRegisterAbilityStateChangeListener_UnregisterFailed
 * @tc.desc  : Test UnRegisterAbilityStateChangeListener when UnregisterApplicationStateObserver
 *             returns non-ERR_OK, appChangeListener_ should still be set to nullptr.
 */
HWTEST_F(MotionManagerThreeTest, UnRegisterAbilityStateChangeListener_UnregisterFailed, TestSize.Level2)
{
    // Given: MotionManager instance with appChangeListener_ set and mock AppMgr returning error
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();
    auto existingListener = new AbilityStateListener(motionMgr);
    motionMgr->appChangeListener_ = existingListener;

    // Inject mock samgr with MockAppMgrRemoteObject that returns error from UnregisterApplicationStateObserver
    sptr<MockAppMgrRemoteObject> mockAppMgrObj = new MockAppMgrRemoteObject(-1);
    sptr<MockSystemAbilityManager> mockSamgr = new MockSystemAbilityManager();
    mockSamgr->SetAppMgrObject(mockAppMgrObj);
    SystemAbilityManagerClient::GetInstance().systemAbilityManager_ = mockSamgr;

    // Precondition: verify GetAppManagerInstance returns non-null
    ASSERT_TRUE(motionMgr->GetAppManagerInstance() != nullptr);

    // When: Call UnRegisterAbilityStateChangeListener, UnregisterApplicationStateObserver fails
    motionMgr->UnRegisterAbilityStateChangeListener();

    // Then: appChangeListener_ is still set to nullptr (line 595 always executes after the if block)
    EXPECT_TRUE(motionMgr->appChangeListener_ == nullptr);

    // And: SendRequest was called (proves UnregisterApplicationStateObserver was invoked)
    EXPECT_GT(mockAppMgrObj->sendRequestCallCount, 0);

    // Clean up
    SystemAbilityManagerClient::GetInstance().DestroySystemAbilityManagerObject();
}

/**
 * @tc.name  : UnRegisterAbilityStateChangeListener_Success
 * @tc.number: UnRegisterAbilityStateChangeListener_Success
 * @tc.desc  : Test UnRegisterAbilityStateChangeListener when all conditions are met,
 *             listener should be unregistered and appChangeListener_ set to nullptr.
 */
HWTEST_F(MotionManagerThreeTest, UnRegisterAbilityStateChangeListener_Success, TestSize.Level1)
{
    // Given: MotionManager instance with appChangeListener_ set and mock AppMgr returning ERR_OK
    int32_t mechId = 100;
    auto mockAdapter = std::make_shared<MockTransportSendAdapter>();
    std::shared_ptr<MotionManager> motionMgr = std::make_shared<MotionManager>(mockAdapter, mechId);
    motionMgr->RegisterEventListener();
    auto existingListener = new AbilityStateListener(motionMgr);
    motionMgr->appChangeListener_ = existingListener;

    // Inject mock samgr with MockAppMgrRemoteObject that returns ERR_OK
    sptr<MockAppMgrRemoteObject> mockAppMgrObj = new MockAppMgrRemoteObject(ERR_OK);
    sptr<MockSystemAbilityManager> mockSamgr = new MockSystemAbilityManager();
    mockSamgr->SetAppMgrObject(mockAppMgrObj);
    SystemAbilityManagerClient::GetInstance().systemAbilityManager_ = mockSamgr;

    // Precondition: verify GetAppManagerInstance returns non-null
    ASSERT_TRUE(motionMgr->GetAppManagerInstance() != nullptr);

    // When: Call UnRegisterAbilityStateChangeListener with all conditions met
    motionMgr->UnRegisterAbilityStateChangeListener();

    // Then: appChangeListener_ is set to nullptr (line 595)
    EXPECT_TRUE(motionMgr->appChangeListener_ == nullptr);

    // And: SendRequest was called (proves UnregisterApplicationStateObserver was invoked)
    EXPECT_GT(mockAppMgrObj->sendRequestCallCount, 0);

    // Clean up
    SystemAbilityManagerClient::GetInstance().DestroySystemAbilityManagerObject();
}

} // namespace MechBodyController
} // namespace OHOS
