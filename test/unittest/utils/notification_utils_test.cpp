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

#include "notification_utils_test.h"
#include "notification_request.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "../test_log.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::MechBodyController;

namespace OHOS {

void NotificationUtilsTest::SetUpTestCase()
{
    DTEST_LOG << "NotificationUtilsTest::SetUpTestCase" << std::endl;
}

void NotificationUtilsTest::TearDownTestCase()
{
    DTEST_LOG << "NotificationUtilsTest::TearDownTestCase" << std::endl;
}

void NotificationUtilsTest::SetUp()
{
    DTEST_LOG << "NotificationUtilsTest::SetUp" << std::endl;
}

void NotificationUtilsTest::TearDown()
{
    DTEST_LOG << "NotificationUtilsTest::TearDown" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseNotificationConfigJson_001, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseNotificationConfigJson_001 begin" << std::endl;

    // Given: 创建非对象格式的配置JSON（数组）
    json configJson = json::array();

    // When: 解析非对象格式配置
    Notification::NotificationRequest request;
    NotificationUtils::ParseNotificationConfigJson(request, configJson);

    // Then: 验证request保持默认值（函数提前返回，未修改request）
    EXPECT_EQ(request.GetNotificationId(), 0);
    EXPECT_EQ(request.GetCreatorUid(), 0);

    DTEST_LOG << "NotificationUtilsTest ParseNotificationConfigJson_001 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseNotificationConfigJson_002, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseNotificationConfigJson_002 begin" << std::endl;

    // Given: 创建空对象配置JSON
    json configJson = json::object();

    // When: 解析空对象配置
    Notification::NotificationRequest request;
    NotificationUtils::ParseNotificationConfigJson(request, configJson);

    // Then: 验证request保持默认值（空对象不包含任何字段）
    EXPECT_EQ(request.GetNotificationId(), 0);
    EXPECT_EQ(request.GetCreatorUid(), 0);

    DTEST_LOG << "NotificationUtilsTest ParseNotificationConfigJson_002 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseNotificationConfigJson_003, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseNotificationConfigJson_003 begin" << std::endl;

    // Given: 创建包含基本字段的配置JSON
    json configJson = {
        {"creatorUid", 1001},
        {"notificationId", 1001}
    };

    // When: 解析包含基本字段的配置
    Notification::NotificationRequest request;
    NotificationUtils::ParseNotificationConfigJson(request, configJson);

    // Then: 验证request的字段被正确设置
    EXPECT_EQ(request.GetCreatorUid(), 1001);
    EXPECT_EQ(request.GetNotificationId(), 1001);

    DTEST_LOG << "NotificationUtilsTest ParseNotificationConfigJson_003 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseBasicFields_001, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseBasicFields_001 begin" << std::endl;

    // Given: 创建包含所有基本字段的配置JSON
    json configJson = {
        {"creatorUid", 1001},
        {"creatorPid", 1234},
        {"creatorUserId", 0},
        {"notificationId", 1001},
        {"notificationControlFlags", 512},
        {"tapDismissed", true},
        {"unremovable", false}
    };

    // When: 解析基本字段
    Notification::NotificationRequest request;
    NotificationUtils::ParseBasicFields(request, configJson);

    // Then: 验证所有字段被正确设置
    EXPECT_EQ(request.GetCreatorUid(), 1001);
    EXPECT_EQ(request.GetCreatorPid(), 1234);
    EXPECT_EQ(request.GetCreatorUserId(), 0);
    EXPECT_EQ(request.GetNotificationId(), 1001);
    EXPECT_EQ(request.GetNotificationControlFlags(), 512);
    EXPECT_EQ(request.IsTapDismissed(), true);
    EXPECT_EQ(request.IsUnremovable(), false);

    DTEST_LOG << "NotificationUtilsTest ParseBasicFields_001 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseBasicFields_002, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseBasicFields_002 begin" << std::endl;

    // Given: 创建空对象配置JSON
    json configJson = json::object();

    // When: 解析空对象配置
    Notification::NotificationRequest request;
    NotificationUtils::ParseBasicFields(request, configJson);

    // Then: 验证request保持默认值
    EXPECT_EQ(request.GetCreatorUid(), 0);
    EXPECT_EQ(request.GetCreatorPid(), 0);
    EXPECT_EQ(request.GetCreatorUserId(), -1);
    EXPECT_EQ(request.GetNotificationId(), 0);

    DTEST_LOG << "NotificationUtilsTest ParseBasicFields_002 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseBasicFields_003, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseBasicFields_003 begin" << std::endl;

    // Given: 创建只包含部分字段的配置JSON
    json configJson = {
        {"creatorUid", 1001},
        {"notificationId", 1001}
    };

    // When: 解析部分字段配置
    Notification::NotificationRequest request;
    NotificationUtils::ParseBasicFields(request, configJson);

    // Then: 验证包含的字段被正确设置，缺失字段保持默认值
    EXPECT_EQ(request.GetCreatorUid(), 1001);
    EXPECT_EQ(request.GetNotificationId(), 1001);
    EXPECT_EQ(request.GetCreatorPid(), 0);
    EXPECT_EQ(request.GetCreatorUserId(), -1);

    DTEST_LOG << "NotificationUtilsTest ParseBasicFields_003 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseAdvancedFields_001, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_001 begin" << std::endl;

    // Given: 创建包含slotType的配置JSON
    json configJson = {
        {"slotType", Notification::NotificationConstant::SlotType::SOCIAL_COMMUNICATION}
    };

    // When: 解析高级字段
    Notification::NotificationRequest request;
    NotificationUtils::ParseAdvancedFields(request, configJson);

    // Then: 验证slotType被正确设置
    auto slotType = request.GetSlotType();
    EXPECT_EQ(slotType, Notification::NotificationConstant::SlotType::SOCIAL_COMMUNICATION);

    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_001 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseAdvancedFields_002, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_002 begin" << std::endl;

    // Given: 创建空对象配置JSON
    json configJson = json::object();

    // When: 解析空对象配置
    Notification::NotificationRequest request;
    NotificationUtils::ParseAdvancedFields(request, configJson);

    // Then: 验证slotType保持默认值
    auto slotType = request.GetSlotType();
    EXPECT_EQ(slotType, Notification::NotificationConstant::SlotType::OTHER);

    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_002 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseExtraInfoFields_002, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseExtraInfoFields_002 begin" << std::endl;

    // Given: 创建不包含extraInfo的配置JSON
    json configJson = json::object();

    // When: 解析不包含extraInfo的配置
    Notification::NotificationRequest request;
    NotificationUtils::ParseExtraInfoFields(request, configJson);

    // Then: 验证additionalData未被设置（函数提前返回）
    auto additionalData = request.GetAdditionalData();
    EXPECT_EQ(additionalData, nullptr);

    DTEST_LOG << "NotificationUtilsTest ParseExtraInfoFields_002 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseExtraInfoFields_003, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseExtraInfoFields_003 begin" << std::endl;

    // Given: 创建extraInfo为非对象格式的配置JSON
    json configJson = {
        {"extraInfo", json::array()}
    };

    // When: 解析extraInfo为非对象格式的配置
    Notification::NotificationRequest request;
    NotificationUtils::ParseExtraInfoFields(request, configJson);

    // Then: 验证additionalData未被设置（函数提前返回）
    auto additionalData = request.GetAdditionalData();
    EXPECT_EQ(additionalData, nullptr);

    DTEST_LOG << "NotificationUtilsTest ParseExtraInfoFields_003 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseNormalContentJson_001, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseNormalContentJson_001 begin" << std::endl;

    // Given: 创建非对象格式的contentJson
    json contentJson = json::array();

    // When: 解析非对象格式contentJson
    Notification::NotificationRequest request;
    NotificationUtils::ParseNormalContentJson(request, contentJson);

    // Then: 验证content未被设置（函数提前返回）
    auto content = request.GetContent();
    EXPECT_EQ(content, nullptr);

    DTEST_LOG << "NotificationUtilsTest ParseNormalContentJson_001 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseLiveViewContentJson_001, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseLiveViewContentJson_001 begin" << std::endl;

    // Given: 创建非对象格式的contentJson
    json contentJson = json::array();

    // When: 解析非对象格式contentJson
    Notification::NotificationRequest request;
    NotificationUtils::ParseLiveViewContentJson(request, contentJson);

    // Then: 验证content未被设置（函数提前返回）
    auto content = request.GetContent();
    EXPECT_EQ(content, nullptr);

    DTEST_LOG << "NotificationUtilsTest ParseLiveViewContentJson_001 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseWantAgentJson_001, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseWantAgentJson_001 begin" << std::endl;

    // Given: 创建非对象格式的wantAgentJson
    json wantAgentJson = json::array();

    // When: 解析非对象格式wantAgentJson
    Notification::NotificationRequest request;
    NotificationUtils::ParseWantAgentJson(request, wantAgentJson);

    // Then: 验证wantAgent未被设置（函数提前返回）
    auto wantAgent = request.GetWantAgent();
    EXPECT_EQ(wantAgent, nullptr);

    DTEST_LOG << "NotificationUtilsTest ParseWantAgentJson_001 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseCapsuleJson_001, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseCapsuleJson_001 begin" << std::endl;

    // Given: 创建非对象格式的capsuleJson
    json capsuleJson = json::array();

    // When: 解析非对象格式capsuleJson
    Notification::NotificationCapsule capsule = NotificationUtils::ParseCapsuleJson(capsuleJson);

    // Then: 验证返回默认capsule
    EXPECT_EQ(capsule.GetTitle(), "");
    EXPECT_EQ(capsule.GetContent(), "");
    EXPECT_EQ(capsule.GetBackgroundColor(), "");

    DTEST_LOG << "NotificationUtilsTest ParseCapsuleJson_001 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseCapsuleJson_002, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseCapsuleJson_002 begin" << std::endl;

    // Given: 创建包含所有字段的capsuleJson
    json capsuleJson = {
        {"title", "CapsuleTitle"},
        {"content", "CapsuleContent"},
        {"backgroundColor", "#3B7DF0"}
    };

    // When: 解析capsule
    Notification::NotificationCapsule capsule = NotificationUtils::ParseCapsuleJson(capsuleJson);

    // Then: 验证capsule的属性被正确设置
    EXPECT_EQ(capsule.GetTitle(), "CapsuleTitle");
    EXPECT_EQ(capsule.GetContent(), "");
    EXPECT_EQ(capsule.GetBackgroundColor(), "#3B7DF0");

    DTEST_LOG << "NotificationUtilsTest ParseCapsuleJson_002 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseCapsuleJson_003, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseCapsuleJson_003 begin" << std::endl;

    // Given: 创建只包含title的capsuleJson
    json capsuleJson = {
        {"title", "CapsuleTitle"}
    };

    // When: 解析只包含title的capsuleJson
    Notification::NotificationCapsule capsule = NotificationUtils::ParseCapsuleJson(capsuleJson);

    // Then: 验证capsule的title被设置，其他字段为默认值
    EXPECT_EQ(capsule.GetTitle(), "CapsuleTitle");
    EXPECT_EQ(capsule.GetContent(), "");
    EXPECT_EQ(capsule.GetBackgroundColor(), "");

    DTEST_LOG << "NotificationUtilsTest ParseCapsuleJson_003 end" << std::endl;
}

/**
 * @tc.name  : ParseWantAgentJson_002
 * @tc.number: ParseWantAgentJson_002
 * @tc.desc  : Test ParseWantAgentJson with all fields present in wantAgentJson.
 */
HWTEST_F(NotificationUtilsTest, ParseWantAgentJson_002, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseWantAgentJson_002 begin" << std::endl;

    // Given: 创建包含所有字段的wantAgentJson
    json wantAgentJson = {
        {"requestCode", 1},
        {"operationType", 0},
        {"flags", {1}},
        {"wants", {
            {
                {"elementName", {
                    {"bundleName", "com.test.bundle"},
                    {"bundleNameAbility", "TestAbility"}
                }}
            }
        }}
    };

    // When: 解析包含所有字段的wantAgentJson
    Notification::NotificationRequest request;
    EXPECT_NO_FATAL_FAILURE(NotificationUtils::ParseWantAgentJson(request, wantAgentJson));

    DTEST_LOG << "NotificationUtilsTest ParseWantAgentJson_002 end" << std::endl;
}

/**
 * @tc.name  : ParseWantAgentJson_003
 * @tc.number: ParseWantAgentJson_003
 * @tc.desc  : Test ParseWantAgentJson with empty object (all fields missing).
 */
HWTEST_F(NotificationUtilsTest, ParseWantAgentJson_003, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseWantAgentJson_003 begin" << std::endl;

    // Given: 创建空对象wantAgentJson（所有字段都不存在）
    json wantAgentJson = json::object();

    // When: 解析空对象wantAgentJson
    Notification::NotificationRequest request;
    EXPECT_NO_FATAL_FAILURE(NotificationUtils::ParseWantAgentJson(request, wantAgentJson));

    DTEST_LOG << "NotificationUtilsTest ParseWantAgentJson_003 end" << std::endl;
}

/**
 * @tc.name  : ParseWantAgentJson_004
 * @tc.number: ParseWantAgentJson_004
 * @tc.desc  : Test ParseWantAgentJson with wants but no elementName.
 */
HWTEST_F(NotificationUtilsTest, ParseWantAgentJson_004, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseWantAgentJson_004 begin" << std::endl;

    // Given: 创建wants中不包含elementName的wantAgentJson
    json wantAgentJson = {
        {"requestCode", 2},
        {"operationType", 2},
        {"flags", json::array()},
        {"wants", {
            json::object()
        }}
    };

    // When: 解析wants中不包含elementName的wantAgentJson
    Notification::NotificationRequest request;
    EXPECT_NO_FATAL_FAILURE(NotificationUtils::ParseWantAgentJson(request, wantAgentJson));

    DTEST_LOG << "NotificationUtilsTest ParseWantAgentJson_004 end" << std::endl;
}

} // namespace OHOS