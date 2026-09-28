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
#include "notification_button_option.h"
#include "configuration.h"
#include "global_configuration_key.h"
#include "mechbody_controller_service.h"
#include "mc_motion_manager.h"
#include "array_wrapper.h"
#include "string_wrapper.h"
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

HWTEST_F(NotificationUtilsTest, BuildConnectedCapsuleLiveViewContent_001, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest BuildConnectedCapsuleLiveViewContent_001 begin" << std::endl;

    // Given: 设置跟踪启用状态和Gimbal类型
    NotificationUtils::isTrackingEnabled_ = true;
    NotificationUtils::mechType_ = MechType::PORTABLE_GIMBAL;

    // When: 构建liveViewContent
    json liveViewContent = NotificationUtils::BuildConnectedCapsuleLiveViewContent(
        "SmartTrackingOn", "DeviceConnected", "TraceOn", "gimbal_icon");

    // Then: 验证基本字段
    EXPECT_EQ(liveViewContent["title"], "SmartTrackingOn");
    EXPECT_EQ(liveViewContent["text"], "DeviceConnected");
    EXPECT_EQ(liveViewContent["type"], 35);

    // Then: 验证capsule字段
    EXPECT_EQ(liveViewContent["capsule"]["title"], "TraceOn");
    EXPECT_EQ(liveViewContent["capsule"]["content"], "");
    EXPECT_EQ(liveViewContent["capsule"]["icon"], "gimbal_icon");
    EXPECT_EQ(liveViewContent["capsule"]["backgroundColor"], "#3B7DF0");

    DTEST_LOG << "NotificationUtilsTest BuildConnectedCapsuleLiveViewContent_001 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, BuildConnectedCapsuleLiveViewContent_002, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest BuildConnectedCapsuleLiveViewContent_002 begin" << std::endl;

    // Given: 设置跟踪禁用状态和WheelBase类型
    NotificationUtils::isTrackingEnabled_ = false;
    NotificationUtils::mechType_ = MechType::WHEEL_BASE;

    // When: 构建liveViewContent
    json liveViewContent = NotificationUtils::BuildConnectedCapsuleLiveViewContent(
        "SmartTrackingOff", "WheelBaseConnected", "TraceOff", "wheel_base_icon");

    // Then: 验证基本字段
    EXPECT_EQ(liveViewContent["title"], "SmartTrackingOff");
    EXPECT_EQ(liveViewContent["text"], "WheelBaseConnected");

    // Then: 验证buttons字段
    EXPECT_TRUE(liveViewContent["buttons"].is_array());
    EXPECT_EQ(liveViewContent["buttons"].size(), 1);

    // Then: 验证flags字段
    EXPECT_TRUE(liveViewContent["flags"].is_array());

    DTEST_LOG << "NotificationUtilsTest BuildConnectedCapsuleLiveViewContent_002 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, GetConnectedCapsuleNotificationConfig_TrackingEnabledGimbal, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest GetConnectedCapsuleNotificationConfig_TrackingEnabledGimbal begin" << std::endl;

    // Given: 设置跟踪启用状态和云台类型
    NotificationUtils::isTrackingEnabled_ = true;
    NotificationUtils::mechType_ = MechType::PORTABLE_GIMBAL;

    // When: 获取连接胶囊通知配置
    json config = NotificationUtils::GetConnectedCapsuleNotificationConfig();

    // Then: 验证固定字段
    EXPECT_EQ(config["creatorUid"], 7811);
    EXPECT_EQ(config["notificationId"], 1001);
    EXPECT_EQ(config["unremovable"], true);

    // Then: 验证mechType_=PORTABLE_GIMBAL时，icon为gimbal图标
    EXPECT_EQ(config["littleIcon"], "ic_gimbal_device");

    // Then: 验证liveViewContent存在且buttons中图标为tracking_open（isTrackingEnabled_=true）
    EXPECT_TRUE(config.contains("liveViewContent"));
    EXPECT_TRUE(config["liveViewContent"]["buttons"].is_array());
    EXPECT_EQ(config["liveViewContent"]["buttons"][0]["singleButtonIcon"], "intelligent_tracking_1");

    // Then: 验证extraInfo存在
    EXPECT_TRUE(config.contains("extraInfo"));
    EXPECT_TRUE(config["extraInfo"].contains("hw_button_accessibility_text"));
    EXPECT_TRUE(config["extraInfo"].contains("hw_live_view_hidden_when_keyguard"));

    DTEST_LOG << "NotificationUtilsTest GetConnectedCapsuleNotificationConfig_TrackingEnabledGimbal end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, GetConnectedCapsuleNotificationConfig_TrackingDisabledGimbal, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest GetConnectedCapsuleNotificationConfig_"
              << "TrackingDisabledGimbal begin" << std::endl;

    // Given: 设置跟踪禁用状态和云台类型
    NotificationUtils::isTrackingEnabled_ = false;
    NotificationUtils::mechType_ = MechType::PORTABLE_GIMBAL;

    // When: 获取连接胶囊通知配置
    json config = NotificationUtils::GetConnectedCapsuleNotificationConfig();

    // Then: 验证固定字段
    EXPECT_EQ(config["creatorUid"], 7811);
    EXPECT_EQ(config["notificationId"], 1001);

    // Then: 验证mechType_=PORTABLE_GIMBAL时，icon为gimbal图标
    EXPECT_EQ(config["littleIcon"], "ic_gimbal_device");

    // Then: 验证liveViewContent中buttons图标为tracking_close（isTrackingEnabled_=false）
    EXPECT_TRUE(config.contains("liveViewContent"));
    EXPECT_EQ(config["liveViewContent"]["buttons"][0]["singleButtonIcon"], "intelligent_tracking_0");

    // Then: 验证capsule中icon为gimbal图标
    EXPECT_EQ(config["liveViewContent"]["capsule"]["icon"], "ic_gimbal_device");

    DTEST_LOG << "NotificationUtilsTest GetConnectedCapsuleNotificationConfig_TrackingDisabledGimbal end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, GetConnectedCapsuleNotificationConfig_TrackingEnabledWheelBase, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest GetConnectedCapsuleNotificationConfig_"
              << "TrackingEnabledWheelBase begin" << std::endl;

    // Given: 设置跟踪启用状态和底盘类型
    NotificationUtils::isTrackingEnabled_ = true;
    NotificationUtils::mechType_ = MechType::WHEEL_BASE;

    // When: 获取连接胶囊通知配置
    json config = NotificationUtils::GetConnectedCapsuleNotificationConfig();

    // Then: 验证固定字段
    EXPECT_EQ(config["creatorUid"], 7811);
    EXPECT_EQ(config["notificationId"], 1001);

    // Then: 验证mechType_=WHEEL_BASE时，icon为wheel_base图标
    EXPECT_EQ(config["littleIcon"], "ic_wheel_base_device");

    // Then: 验证liveViewContent中buttons图标为tracking_open（isTrackingEnabled_=true）
    EXPECT_TRUE(config.contains("liveViewContent"));
    EXPECT_EQ(config["liveViewContent"]["buttons"][0]["singleButtonIcon"], "intelligent_tracking_1");

    // Then: 验证capsule中icon为wheel_base图标
    EXPECT_EQ(config["liveViewContent"]["capsule"]["icon"], "ic_wheel_base_device");

    DTEST_LOG << "NotificationUtilsTest GetConnectedCapsuleNotificationConfig_"
              << "TrackingEnabledWheelBase end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, GetConnectedCapsuleNotificationConfig_TrackingDisabledWheelBase, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest GetConnectedCapsuleNotificationConfig_"
              << "TrackingDisabledWheelBase begin" << std::endl;

    // Given: 设置跟踪禁用状态和底盘类型
    NotificationUtils::isTrackingEnabled_ = false;
    NotificationUtils::mechType_ = MechType::WHEEL_BASE;

    // When: 获取连接胶囊通知配置
    json config = NotificationUtils::GetConnectedCapsuleNotificationConfig();

    // Then: 验证固定字段
    EXPECT_EQ(config["creatorUid"], 7811);
    EXPECT_EQ(config["notificationId"], 1001);

    // Then: 验证mechType_=WHEEL_BASE时，icon为wheel_base图标
    EXPECT_EQ(config["littleIcon"], "ic_wheel_base_device");

    // Then: 验证liveViewContent中buttons图标为tracking_close（isTrackingEnabled_=false）
    EXPECT_TRUE(config.contains("liveViewContent"));
    EXPECT_EQ(config["liveViewContent"]["buttons"][0]["singleButtonIcon"], "intelligent_tracking_0");

    // Then: 验证capsule中icon为wheel_base图标
    EXPECT_EQ(config["liveViewContent"]["capsule"]["icon"], "ic_wheel_base_device");

    DTEST_LOG << "NotificationUtilsTest GetConnectedCapsuleNotificationConfig_"
              << "TrackingDisabledWheelBase end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseAdvancedFields_003, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_003 begin" << std::endl;

    // Given: 包含normalContent的配置JSON
    json configJson;
    configJson["normalContent"] = {{"title", "TestTitle"}, {"text", "TestText"}};

    // When: 解析高级字段
    Notification::NotificationRequest request;
    NotificationUtils::ParseAdvancedFields(request, configJson);

    // Then: 验证normalContent分支被覆盖，content被正确设置
    auto content = request.GetContent();
    ASSERT_NE(content, nullptr);
    auto basicContent = content->GetNotificationContent();
    ASSERT_NE(basicContent, nullptr);
    EXPECT_EQ(basicContent->GetTitle(), "TestTitle");
    EXPECT_EQ(basicContent->GetText(), "TestText");

    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_003 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseAdvancedFields_004, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_004 begin" << std::endl;

    // Given: 包含liveViewContent的配置JSON
    json configJson;
    configJson["liveViewContent"] = {{"title", "LVTitle"}, {"text", "LVText"}, {"type", 35}};

    // When: 解析高级字段
    Notification::NotificationRequest request;
    NotificationUtils::ParseAdvancedFields(request, configJson);

    // Then: 验证liveViewContent分支被覆盖，content被正确设置
    auto content = request.GetContent();
    ASSERT_NE(content, nullptr);
    auto basicContent = content->GetNotificationContent();
    ASSERT_NE(basicContent, nullptr);
    EXPECT_EQ(basicContent->GetTitle(), "LVTitle");
    EXPECT_EQ(basicContent->GetText(), "LVText");

    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_004 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseAdvancedFields_005, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_005 begin" << std::endl;

    // Given: iconPixelMaps_为空（缓存未命中），配置包含littleIcon和badgeIconStyle
    NotificationUtils::iconPixelMaps_.clear();
    json configJson = {
        {"littleIcon", "nonexistent_icon"},
        {"badgeIconStyle", Notification::NotificationRequest::BadgeStyle::LITTLE}
    };

    // When: 解析高级字段
    Notification::NotificationRequest request;
    NotificationUtils::ParseAdvancedFields(request, configJson);

    // Then: 验证littleIcon缓存未命中时GetPixelMapByName返回nullptr，icon未设置
    EXPECT_EQ(request.GetLittleIcon(), nullptr);
    // 验证badgeIconStyle被正确设置（证明ParseAdvancedFields执行了）
    EXPECT_EQ(request.GetBadgeIconStyle(), Notification::NotificationRequest::BadgeStyle::LITTLE);

    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_005 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseAdvancedFields_006, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_006 begin" << std::endl;

    // Given: iconPixelMaps_中预存一个PixelMap（缓存命中）
    NotificationUtils::iconPixelMaps_.clear();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    NotificationUtils::iconPixelMaps_["cached_icon"] = pixelMap;

    json configJson = {{"littleIcon", "cached_icon"}};

    // When: 解析高级字段
    Notification::NotificationRequest request;
    NotificationUtils::ParseAdvancedFields(request, configJson);

    // Then: 验证littleIcon缓存命中时，request持有的是缓存中同一个PixelMap对象
    EXPECT_EQ(request.GetLittleIcon().get(), pixelMap.get());

    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_006 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseAdvancedFields_007, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_007 begin" << std::endl;

    // Given: 包含badgeIconStyle的配置JSON
    json configJson = {
        {"badgeIconStyle", Notification::NotificationRequest::BadgeStyle::LITTLE}
    };

    // When: 解析高级字段
    Notification::NotificationRequest request;
    NotificationUtils::ParseAdvancedFields(request, configJson);

    // Then: 验证badgeIconStyle分支被覆盖，样式被正确设置
    EXPECT_EQ(request.GetBadgeIconStyle(), Notification::NotificationRequest::BadgeStyle::LITTLE);

    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_007 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseAdvancedFields_008, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_008 begin" << std::endl;

    // Given: 包含wantAgent的配置JSON，同时包含badgeIconStyle用于验证执行
    json configJson;
    configJson["wantAgent"] = json::object();
    configJson["wantAgent"]["requestCode"] = 0;
    configJson["badgeIconStyle"] = Notification::NotificationRequest::BadgeStyle::LITTLE;

    // When: 解析高级字段
    Notification::NotificationRequest request;
    NotificationUtils::ParseAdvancedFields(request, configJson);

    // Then: 验证wantAgent分支被覆盖，badgeIconStyle被正确设置证明ParseAdvancedFields执行
    EXPECT_EQ(request.GetBadgeIconStyle(), Notification::NotificationRequest::BadgeStyle::LITTLE);
    // wantAgent在测试环境中GetWantAgent可能返回nullptr，但分支已被覆盖
    auto wantAgent = request.GetWantAgent();
    EXPECT_EQ(wantAgent, nullptr);

    DTEST_LOG << "NotificationUtilsTest ParseAdvancedFields_008 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseExtraInfoArrayFields_001, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseExtraInfoArrayFields_001 begin" << std::endl;

    // Given: 包含字符串数组的extraInfo，走ParseExtraInfoArrayFields正常路径
    json arrValue = json::array({"str1", "str2", "str3"});
    json configJson;
    configJson["extraInfo"] = {{"arrKey", arrValue}};

    // When: 解析extraInfo字段
    Notification::NotificationRequest request;
    NotificationUtils::ParseExtraInfoFields(request, configJson);

    // Then: 验证additionalData被设置，arrKey被成功写入且数组内容正确
    auto additionalData = request.GetAdditionalData();
    ASSERT_NE(additionalData, nullptr);
    auto param = additionalData->GetParam("arrKey");
    ASSERT_NE(param, nullptr);
    auto array = AAFwk::IArray::Query(param);
    ASSERT_NE(array, nullptr);
    long length = 0;
    EXPECT_EQ(array->GetLength(length), 0);
    EXPECT_EQ(length, 3);
    sptr<AAFwk::IInterface> elem0 = nullptr;
    sptr<AAFwk::IInterface> elem1 = nullptr;
    sptr<AAFwk::IInterface> elem2 = nullptr;
    EXPECT_EQ(array->Get(0, elem0), 0);
    EXPECT_EQ(array->Get(1, elem1), 0);
    EXPECT_EQ(array->Get(2, elem2), 0);
    EXPECT_EQ(AAFwk::String::Unbox(AAFwk::IString::Query(elem0)), "str1");
    EXPECT_EQ(AAFwk::String::Unbox(AAFwk::IString::Query(elem1)), "str2");
    EXPECT_EQ(AAFwk::String::Unbox(AAFwk::IString::Query(elem2)), "str3");

    DTEST_LOG << "NotificationUtilsTest ParseExtraInfoArrayFields_001 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseExtraInfoArrayFields_002, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseExtraInfoArrayFields_002 begin" << std::endl;

    // Given: 数组中包含非字符串元素（数字），触发isValid=false提前返回
    json arrValue = json::array({42});
    json configJson;
    configJson["extraInfo"] = {{"strKey", "validStr"}, {"arrKey", arrValue}};

    // When: 解析extraInfo字段
    Notification::NotificationRequest request;
    NotificationUtils::ParseExtraInfoFields(request, configJson);

    // Then: 验证strKey被设置（证明ParseExtraInfoFields执行了）
    auto additionalData = request.GetAdditionalData();
    ASSERT_NE(additionalData, nullptr);
    EXPECT_NE(additionalData->GetParam("strKey"), nullptr);
    // 验证arrKey未被设置（ParseExtraInfoArrayFields因非字符串元素提前返回）
    EXPECT_EQ(additionalData->GetParam("arrKey"), nullptr);

    DTEST_LOG << "NotificationUtilsTest ParseExtraInfoArrayFields_002 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseExtraInfoArrayFields_003, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseExtraInfoArrayFields_003 begin" << std::endl;

    // Given: 空数组，触发size==0提前返回
    json configJson;
    configJson["extraInfo"] = {{"strKey", "validStr"}, {"arrKey", json::array()}};

    // When: 解析extraInfo字段
    Notification::NotificationRequest request;
    NotificationUtils::ParseExtraInfoFields(request, configJson);

    // Then: 验证strKey被设置（证明ParseExtraInfoFields执行了）
    auto additionalData = request.GetAdditionalData();
    ASSERT_NE(additionalData, nullptr);
    EXPECT_NE(additionalData->GetParam("strKey"), nullptr);
    // 验证arrKey未被设置（空数组导致size==0提前返回）
    EXPECT_EQ(additionalData->GetParam("arrKey"), nullptr);

    DTEST_LOG << "NotificationUtilsTest ParseExtraInfoArrayFields_003 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseExtraInfoArrayFields_004, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseExtraInfoArrayFields_004 begin" << std::endl;

    // Given: 数组中第一个元素为字符串，后续元素为非字符串（验证循环中先成功后中断的路径）
    json arrValue = json::array({"validStr1", 123});
    json configJson;
    configJson["extraInfo"] = {{"strKey", "validStr"}, {"arrKey", arrValue}};

    // When: 解析extraInfo字段
    Notification::NotificationRequest request;
    NotificationUtils::ParseExtraInfoFields(request, configJson);

    // Then: 验证strKey被设置（证明ParseExtraInfoFields执行了）
    auto additionalData = request.GetAdditionalData();
    ASSERT_NE(additionalData, nullptr);
    EXPECT_NE(additionalData->GetParam("strKey"), nullptr);
    // 验证arrKey未被设置（遇到非字符串元素后isValid=false提前返回）
    EXPECT_EQ(additionalData->GetParam("arrKey"), nullptr);

    DTEST_LOG << "NotificationUtilsTest ParseExtraInfoArrayFields_004 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseLiveViewContentJson_002, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseLiveViewContentJson_002 begin" << std::endl;

    // Given: 包含所有字段的liveViewContent（title, text, type, capsule, buttons, flags）
    json capsuleObj;
    capsuleObj["title"] = "CapTitle";
    capsuleObj["backgroundColor"] = "#3B7DF0";
    json buttonObj;
    buttonObj["singleButtonName"] = "BtnName";
    json buttonsArr = json::array();
    buttonsArr.push_back(buttonObj);
    json flagsArr = json::array();
    flagsArr.push_back(1);
    flagsArr.push_back(2);
    json contentJson;
    contentJson["title"] = "LVTitle";
    contentJson["text"] = "LVText";
    contentJson["type"] = 35;
    contentJson["capsule"] = capsuleObj;
    contentJson["buttons"] = buttonsArr;
    contentJson["flags"] = flagsArr;

    // When: 解析liveViewContent
    Notification::NotificationRequest request;
    NotificationUtils::ParseLiveViewContentJson(request, contentJson);

    // Then: 验证content被设置
    auto content = request.GetContent();
    ASSERT_NE(content, nullptr);
    // 验证title和text通过基础内容接口被正确设置
    auto basicContent = content->GetNotificationContent();
    ASSERT_NE(basicContent, nullptr);
    EXPECT_EQ(basicContent->GetTitle(), "LVTitle");
    EXPECT_EQ(basicContent->GetText(), "LVText");

    DTEST_LOG << "NotificationUtilsTest ParseLiveViewContentJson_002 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseLiveViewContentJson_003, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseLiveViewContentJson_003 begin" << std::endl;

    // Given: 空对象的liveViewContent（所有字段均缺失）
    json contentJson = json::object();

    // When: 解析liveViewContent
    Notification::NotificationRequest request;
    NotificationUtils::ParseLiveViewContentJson(request, contentJson);

    // Then: 验证content仍被设置（空对象不提前返回）
    auto content = request.GetContent();
    ASSERT_NE(content, nullptr);
    // 验证title和text为默认空值（各字段缺失分支被覆盖）
    auto basicContent = content->GetNotificationContent();
    ASSERT_NE(basicContent, nullptr);
    EXPECT_EQ(basicContent->GetTitle(), "");
    EXPECT_EQ(basicContent->GetText(), "");

    DTEST_LOG << "NotificationUtilsTest ParseLiveViewContentJson_003 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseLiveViewContentJson_004, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseLiveViewContentJson_004 begin" << std::endl;

    // Given: 仅包含title的liveViewContent（text/type/capsule/buttons/flags缺失）
    json contentJson = {{"title", "OnlyTitle"}};

    // When: 解析liveViewContent
    Notification::NotificationRequest request;
    NotificationUtils::ParseLiveViewContentJson(request, contentJson);

    // Then: 验证title被设置，其他字段为默认值
    auto content = request.GetContent();
    ASSERT_NE(content, nullptr);
    auto basicContent = content->GetNotificationContent();
    ASSERT_NE(basicContent, nullptr);
    EXPECT_EQ(basicContent->GetTitle(), "OnlyTitle");
    EXPECT_EQ(basicContent->GetText(), "");

    DTEST_LOG << "NotificationUtilsTest ParseLiveViewContentJson_004 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseLiveViewContentJson_005, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseLiveViewContentJson_005 begin" << std::endl;

    // Given: 包含capsule和buttons的liveViewContent（覆盖capsule/buttons present分支）
    // capsule不含icon避免PixelMap依赖，buttons不含singleButtonIcon同理
    json capsuleObj;
    capsuleObj["title"] = "CapTitle";
    capsuleObj["backgroundColor"] = "#3B7DF0";
    json buttonObj;
    buttonObj["singleButtonName"] = "Btn1";
    json buttonsArr = json::array();
    buttonsArr.push_back(buttonObj);
    json flagsArr = json::array();
    flagsArr.push_back(1);
    json contentJson;
    contentJson["title"] = "WithCapsuleBtns";
    contentJson["capsule"] = capsuleObj;
    contentJson["buttons"] = buttonsArr;
    contentJson["flags"] = flagsArr;

    // When: 解析liveViewContent
    Notification::NotificationRequest request;
    NotificationUtils::ParseLiveViewContentJson(request, contentJson);

    // Then: 验证content被设置
    auto content = request.GetContent();
    ASSERT_NE(content, nullptr);
    auto basicContent = content->GetNotificationContent();
    ASSERT_NE(basicContent, nullptr);
    EXPECT_EQ(basicContent->GetTitle(), "WithCapsuleBtns");

    DTEST_LOG << "NotificationUtilsTest ParseLiveViewContentJson_005 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseWantAgentJson_002, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseWantAgentJson_002 begin" << std::endl;

    // Given: 包含所有字段的wantAgentJson（requestCode/operationType/flags/wants含elementName）
    // 同时包含badgeIconStyle用于验证ParseAdvancedFields执行
    json wantAgentJson;
    wantAgentJson["requestCode"] = 1;
    wantAgentJson["operationType"] = 0;
    json flagsArr = json::array();
    flagsArr.push_back(1);
    wantAgentJson["flags"] = flagsArr;
    json elementNameObj;
    elementNameObj["bundleName"] = "com.test";
    elementNameObj["bundleNameAbility"] = "MainAbility";
    json wantObj;
    wantObj["elementName"] = elementNameObj;
    json wantsArr = json::array();
    wantsArr.push_back(wantObj);
    wantAgentJson["wants"] = wantsArr;

    json configJson;
    configJson["wantAgent"] = wantAgentJson;
    configJson["badgeIconStyle"] = Notification::NotificationRequest::BadgeStyle::LITTLE;

    // When: 解析高级字段
    Notification::NotificationRequest request;
    NotificationUtils::ParseAdvancedFields(request, configJson);

    // Then: 验证badgeIconStyle被设置（证明ParseAdvancedFields执行了）
    EXPECT_EQ(request.GetBadgeIconStyle(), Notification::NotificationRequest::BadgeStyle::LITTLE);
    // wantAgent在测试环境中GetWantAgent可能返回nullptr，验证函数无崩溃
    auto wantAgent = request.GetWantAgent();
    EXPECT_EQ(wantAgent, nullptr);

    DTEST_LOG << "NotificationUtilsTest ParseWantAgentJson_002 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseWantAgentJson_003, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseWantAgentJson_003 begin" << std::endl;

    // Given: 空对象的wantAgentJson（requestCode/operationType/flags/wants均缺失）
    // 同时包含badgeIconStyle用于验证ParseAdvancedFields执行
    json configJson;
    configJson["wantAgent"] = json::object();
    configJson["badgeIconStyle"] = Notification::NotificationRequest::BadgeStyle::LITTLE;

    // When: 解析高级字段
    Notification::NotificationRequest request;
    NotificationUtils::ParseAdvancedFields(request, configJson);

    // Then: 验证badgeIconStyle被设置（证明ParseAdvancedFields执行了）
    EXPECT_EQ(request.GetBadgeIconStyle(), Notification::NotificationRequest::BadgeStyle::LITTLE);
    // wantAgent各字段缺失分支被覆盖，GetWantAgent返回nullptr
    auto wantAgent = request.GetWantAgent();
    EXPECT_EQ(wantAgent, nullptr);

    DTEST_LOG << "NotificationUtilsTest ParseWantAgentJson_003 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseWantAgentJson_004, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseWantAgentJson_004 begin" << std::endl;

    // Given: wants存在但不含elementName（覆盖wants present + elementName absent分支）
    // 同时包含badgeIconStyle用于验证ParseAdvancedFields执行
    json wantsArr = json::array();
    wantsArr.push_back(json::object());
    json configJson;
    configJson["wantAgent"] = json::object();
    configJson["wantAgent"]["wants"] = wantsArr;
    configJson["badgeIconStyle"] = Notification::NotificationRequest::BadgeStyle::LITTLE;

    // When: 解析高级字段
    Notification::NotificationRequest request;
    NotificationUtils::ParseAdvancedFields(request, configJson);

    // Then: 验证badgeIconStyle被设置（证明ParseAdvancedFields执行了）
    EXPECT_EQ(request.GetBadgeIconStyle(), Notification::NotificationRequest::BadgeStyle::LITTLE);
    // wants present但elementName absent，GetWantAgent返回nullptr
    auto wantAgent = request.GetWantAgent();
    EXPECT_EQ(wantAgent, nullptr);

    DTEST_LOG << "NotificationUtilsTest ParseWantAgentJson_004 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseLiveViewButtonsJson_001, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseLiveViewButtonsJson_001 begin" << std::endl;

    // Given: 非数组格式的buttonsJson（覆盖is_array→false分支）
    json buttonsJson = json::object();

    // When: 解析buttons
    auto button = NotificationUtils::ParseLiveViewButtonsJson(buttonsJson);

    // Then: 非数组输入提前返回默认button，无singleButtonName
    EXPECT_EQ(button.GetAllButtonNames().size(), 0);

    DTEST_LOG << "NotificationUtilsTest ParseLiveViewButtonsJson_001 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseLiveViewButtonsJson_002, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseLiveViewButtonsJson_002 begin" << std::endl;

    // Given: 数组中元素不含singleButtonName和singleButtonIcon
    // （覆盖singleButtonName absent和singleButtonIcon absent分支）
    json buttonsArr = json::array();
    buttonsArr.push_back(json::object());

    // When: 解析buttons
    auto button = NotificationUtils::ParseLiveViewButtonsJson(buttonsArr);

    // Then: 无singleButtonName被添加，无singleButtonIcon被设置
    EXPECT_EQ(button.GetAllButtonNames().size(), 0);

    DTEST_LOG << "NotificationUtilsTest ParseLiveViewButtonsJson_002 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseLiveViewButtonsJson_003, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseLiveViewButtonsJson_003 begin" << std::endl;

    // Given: 包含singleButtonIcon但缓存未命中且GetPixelMapByName返回nullptr
    // （覆盖singleButtonIcon present + 缓存未命中 + iconPixelMap==nullptr分支）
    NotificationUtils::iconPixelMaps_.clear();
    json buttonObj;
    buttonObj["singleButtonName"] = "TestBtn";
    buttonObj["singleButtonIcon"] = "nonexistent_icon";
    json buttonsArr = json::array();
    buttonsArr.push_back(buttonObj);

    // When: 解析buttons
    auto button = NotificationUtils::ParseLiveViewButtonsJson(buttonsArr);

    // Then: singleButtonName被添加，singleButtonIcon因PixelMap为nullptr未被设置
    EXPECT_EQ(button.GetAllButtonNames().size(), 1);
    EXPECT_EQ(button.GetAllButtonNames()[0], "TestBtn");

    DTEST_LOG << "NotificationUtilsTest ParseLiveViewButtonsJson_003 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, ParseLiveViewButtonsJson_004, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest ParseLiveViewButtonsJson_004 begin" << std::endl;

    // Given: 包含singleButtonIcon且iconPixelMaps_缓存命中
    // （覆盖singleButtonIcon present + 缓存命中 + iconPixelMap!=nullptr分支）
    NotificationUtils::iconPixelMaps_.clear();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    NotificationUtils::iconPixelMaps_["cached_btn_icon"] = pixelMap;
    json buttonObj;
    buttonObj["singleButtonName"] = "CachedBtn";
    buttonObj["singleButtonIcon"] = "cached_btn_icon";
    json buttonsArr = json::array();
    buttonsArr.push_back(buttonObj);

    // When: 解析buttons
    auto button = NotificationUtils::ParseLiveViewButtonsJson(buttonsArr);

    // Then: singleButtonName被添加，singleButtonIcon缓存命中被设置
    EXPECT_EQ(button.GetAllButtonNames().size(), 1);
    EXPECT_EQ(button.GetAllButtonNames()[0], "CachedBtn");
    EXPECT_EQ(button.GetAllButtonIcons().size(), 1);

    DTEST_LOG << "NotificationUtilsTest ParseLiveViewButtonsJson_004 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, RegisterConfigurationObserver_001, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest RegisterConfigurationObserver_001 begin" << std::endl;

    // Given: configChangeObserver_已非空（覆盖configChangeObserver_!=nullptr→提前返回分支）
    auto observer = new ReminderConfigChangeObserver();
    NotificationUtils::configChangeObserver_ =
        sptr<AppExecFwk::IConfigurationObserver>(observer);

    // When: 调用RegisterConfigurationObserver
    NotificationUtils::RegisterConfigurationObserver();

    // Then: configChangeObserver_未被替换（仍是原来的observer）
    EXPECT_EQ(NotificationUtils::configChangeObserver_.GetRefPtr(),
        sptr<AppExecFwk::IConfigurationObserver>(observer).GetRefPtr());

    // 清理
    NotificationUtils::configChangeObserver_ = nullptr;

    DTEST_LOG << "NotificationUtilsTest RegisterConfigurationObserver_001 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, RegisterConfigurationObserver_002, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest RegisterConfigurationObserver_002 begin" << std::endl;

    // Given: configChangeObserver_为nullptr（覆盖configChangeObserver_==nullptr→进入连接分支）
    NotificationUtils::configChangeObserver_ = nullptr;

    // When: 调用RegisterConfigurationObserver，测试环境中ConnectAppMgrService连接失败
    // → SUT在连接失败处提前返回，不应创建observer
    NotificationUtils::RegisterConfigurationObserver();

    // Then: 连接失败后configChangeObserver_应仍为nullptr（未注册成功）
    EXPECT_EQ(NotificationUtils::configChangeObserver_, nullptr);

    // 清理
    NotificationUtils::configChangeObserver_ = nullptr;

    DTEST_LOG << "NotificationUtilsTest RegisterConfigurationObserver_002 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, RegisterConfigurationObserver_003, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest RegisterConfigurationObserver_003 begin" << std::endl;

    // Given: configChangeObserver_为nullptr，先调用一次触发连接失败路径（覆盖002场景）
    NotificationUtils::configChangeObserver_ = nullptr;
    NotificationUtils::RegisterConfigurationObserver();
    ASSERT_EQ(NotificationUtils::configChangeObserver_, nullptr);

    // When: 连接失败后预置非空observer，再次调用RegisterConfigurationObserver
    // 覆盖 configChangeObserver_ != nullptr → 守卫检查提前返回
    auto observer = new ReminderConfigChangeObserver();
    NotificationUtils::configChangeObserver_ =
        sptr<AppExecFwk::IConfigurationObserver>(observer);
    NotificationUtils::RegisterConfigurationObserver();

    // Then: 第二次调用应因守卫检查提前返回，observer未被替换
    EXPECT_EQ(NotificationUtils::configChangeObserver_.GetRefPtr(),
        sptr<AppExecFwk::IConfigurationObserver>(observer).GetRefPtr());

    // 清理
    NotificationUtils::configChangeObserver_ = nullptr;

    DTEST_LOG << "NotificationUtilsTest RegisterConfigurationObserver_003 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, SubscribeLocalLiveViewNotification_001, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest SubscribeLocalLiveViewNotification_001 begin" << std::endl;

    // Given: isLocalLiveViewSubscriber_为true（已订阅），localLiveViewSubscriber_已存在
    // 预置isLocalLiveViewSubscriber_为true，使SubscribeLocalLiveViewNotification在守卫检查处跳过，
    // 避免跨DSO调用NotificationHelper::SubscribeLocalLiveViewNotification导致CFI check失败
    auto existingSubscriber = std::make_shared<LocalLiveViewSubscriber>();
    NotificationUtils::isLocalLiveViewSubscriber_ = true;
    NotificationUtils::localLiveViewSubscriber_ = existingSubscriber;

    // When: 调用SubscribeLocalLiveViewNotification
    // isLocalLiveViewSubscriber_==true → 跳过if块，不创建新subscriber
    NotificationUtils::SubscribeLocalLiveViewNotification();

    // Then: 验证isLocalLiveViewSubscriber_仍为true
    EXPECT_TRUE(NotificationUtils::isLocalLiveViewSubscriber_);
    // 验证localLiveViewSubscriber_未被重新创建（仍指向原对象）
    EXPECT_EQ(NotificationUtils::localLiveViewSubscriber_.get(), existingSubscriber.get());

    // 清理
    NotificationUtils::isLocalLiveViewSubscriber_ = false;
    NotificationUtils::localLiveViewSubscriber_ = nullptr;

    DTEST_LOG << "NotificationUtilsTest SubscribeLocalLiveViewNotification_001 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, SubscribeLocalLiveViewNotification_002, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest SubscribeLocalLiveViewNotification_002 begin" << std::endl;

    // Given: isLocalLiveViewSubscriber_为true（已订阅），localLiveViewSubscriber_已存在
    auto existingSubscriber = std::make_shared<LocalLiveViewSubscriber>();
    NotificationUtils::isLocalLiveViewSubscriber_ = true;
    NotificationUtils::localLiveViewSubscriber_ = existingSubscriber;

    // When: 调用SubscribeLocalLiveViewNotification
    // isLocalLiveViewSubscriber_==true → 跳过if块，不创建新subscriber
    NotificationUtils::SubscribeLocalLiveViewNotification();

    // Then: 验证localLiveViewSubscriber_未被重新创建（仍指向原对象）
    EXPECT_EQ(NotificationUtils::localLiveViewSubscriber_.get(), existingSubscriber.get());

    // 清理
    NotificationUtils::isLocalLiveViewSubscriber_ = false;
    NotificationUtils::localLiveViewSubscriber_ = nullptr;

    DTEST_LOG << "NotificationUtilsTest SubscribeLocalLiveViewNotification_002 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, OnResponse_001, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest OnResponse_001 begin" << std::endl;

    // Given: 设置motionManagers_使SetTrackingEnabled可执行，deviceStatus_->isEnabled初始为true
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, 1, true, 1);
    service.motionManagers_[1] = manager;

    LocalLiveViewSubscriber subscriber;

    // When: 调用OnResponse传入nullptr buttonOption
    // buttonOption==nullptr → line 675提前return，不执行SetTrackingEnabled
    subscriber.OnResponse(1001, nullptr);

    // Then: deviceStatus_->isEnabled未被修改（路径不修改验证：SUT存在修改分支但当前路径跳过）
    EXPECT_TRUE(manager->deviceStatus_->isEnabled);

    // 清理
    service.motionManagers_.erase(1);

    DTEST_LOG << "NotificationUtilsTest OnResponse_001 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, OnResponse_002, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest OnResponse_002 begin" << std::endl;

    // Given: 设置motionManagers_使SetTrackingEnabled可执行
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, 1, true, 1);
    service.motionManagers_[1] = manager;

    NotificationUtils::isTrackingEnabled_ = true;
    LocalLiveViewSubscriber subscriber;
    sptr<Notification::NotificationButtonOption> buttonOption = new Notification::NotificationButtonOption();
    buttonOption->SetButtonName("TrackingEnableChange");

    // When: 调用OnResponse
    // buttonName=="TrackingEnableChange" → 进入if块（line 680）
    // 计算trackingEnabled=!isTrackingEnabled_=false，调用SetTrackingEnabled(false, true)
    // SetTrackingEnabled遍历motionManagers_，调用SetMechCameraTrackingEnabled(false)
    // → deviceStatus_->isEnabled被设置为false
    subscriber.OnResponse(1001, buttonOption);

    // Then: 验证toggle逻辑生效，deviceStatus_->isEnabled从true变为false
    EXPECT_FALSE(manager->deviceStatus_->isEnabled);

    // 清理
    service.motionManagers_.erase(1);
    NotificationUtils::isTrackingEnabled_ = true;

    DTEST_LOG << "NotificationUtilsTest OnResponse_002 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, OnResponse_003, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest OnResponse_003 begin" << std::endl;

    // Given: 设置motionManagers_使SetTrackingEnabled可执行，deviceStatus_->isEnabled初始为true
    auto &service = MechBodyControllerService::GetInstance();
    auto sendAdapter = std::make_shared<TransportSendAdapter>();
    auto manager = std::make_shared<MotionManager>(sendAdapter, 1, true, 1);
    service.motionManagers_[1] = manager;

    LocalLiveViewSubscriber subscriber;
    sptr<Notification::NotificationButtonOption> buttonOption = new Notification::NotificationButtonOption();
    buttonOption->SetButtonName("OtherButton");

    // When: 调用OnResponse
    // buttonName!="TrackingEnableChange" → 跳过if块（line 680条件不满足）
    // 不调用SetTrackingEnabled
    subscriber.OnResponse(1001, buttonOption);

    // Then: deviceStatus_->isEnabled未被修改（路径不修改验证：SUT存在修改分支但当前路径跳过）
    EXPECT_TRUE(manager->deviceStatus_->isEnabled);

    // 清理
    service.motionManagers_.erase(1);

    DTEST_LOG << "NotificationUtilsTest OnResponse_003 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, OnConfigurationUpdated_001, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest OnConfigurationUpdated_001 begin" << std::endl;

    // Given: Configuration中SYSTEM_LANGUAGE为空字符串
    AppExecFwk::Configuration configuration;
    ReminderConfigChangeObserver observer;
    observer.languageInfo_ = "en_US";

    // When: 调用OnConfigurationUpdated
    // GetItem(SYSTEM_LANGUAGE)返回空字符串 → newLanguageInfo.empty()为true
    // → 跳过if块，不更新languageInfo_
    observer.OnConfigurationUpdated(configuration);

    // Then: languageInfo_未被修改（SUT因newLanguageInfo为空跳过了更新逻辑）
    EXPECT_EQ(observer.languageInfo_, "en_US");

    DTEST_LOG << "NotificationUtilsTest OnConfigurationUpdated_001 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, OnConfigurationUpdated_002, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest OnConfigurationUpdated_002 begin" << std::endl;

    // Given: Configuration中SYSTEM_LANGUAGE与当前languageInfo_相同
    AppExecFwk::Configuration configuration;
    configuration.AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE, "zh_CN");
    ReminderConfigChangeObserver observer;
    observer.languageInfo_ = "zh_CN";

    // When: 调用OnConfigurationUpdated
    // newLanguageInfo=="zh_CN" == languageInfo_ → 条件不满足
    // → 跳过if块，不更新languageInfo_，不调用OnLanguageChanged
    observer.OnConfigurationUpdated(configuration);

    // Then: languageInfo_未被修改（SUT因语言未变跳过了更新逻辑）
    EXPECT_EQ(observer.languageInfo_, "zh_CN");

    DTEST_LOG << "NotificationUtilsTest OnConfigurationUpdated_002 end" << std::endl;
}

HWTEST_F(NotificationUtilsTest, OnConfigurationUpdated_003, TestSize.Level3)
{
    DTEST_LOG << "NotificationUtilsTest OnConfigurationUpdated_003 begin" << std::endl;

    // Given: Configuration中SYSTEM_LANGUAGE与当前languageInfo_不同
    AppExecFwk::Configuration configuration;
    configuration.AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE, "en_US");
    ReminderConfigChangeObserver observer;
    observer.languageInfo_ = "zh_CN";
    NotificationUtils::isSendConnectedCapsule_ = false;

    // When: 调用OnConfigurationUpdated
    // newLanguageInfo=="en_US" != languageInfo_=="zh_CN" → 条件满足
    // → 进入if块：languageInfo_更新为"en_US"，调用OnLanguageChanged
    observer.OnConfigurationUpdated(configuration);

    // Then: languageInfo_被SUT更新为新语言值
    EXPECT_EQ(observer.languageInfo_, "en_US");

    // 清理
    NotificationUtils::isSendConnectedCapsule_ = false;

    DTEST_LOG << "NotificationUtilsTest OnConfigurationUpdated_003 end" << std::endl;
}

/**
 * @tc.name: SendNotification_CameraDisabled_Skip_001
 * @tc.desc: 测试 SendNotification 在 isCameraDisabled_=true 时跳过发送，isSendConnectedCapsule_保持false
 * @tc.type: FUNC
 */
HWTEST_F(NotificationUtilsTest, SendNotification_CameraDisabled_Skip_001, TestSize.Level2)
{
    DTEST_LOG << "NotificationUtilsTest SendNotification_CameraDisabled_Skip_001 begin" << std::endl;

    // Given: 设置相机禁用标志为 true，重置发送状态
    NotificationUtils::isCameraDisabled_.store(true);
    NotificationUtils::isSendConnectedCapsule_ = false;

    // When: 调用 SendNotification，isCameraDisabled_=true 时应跳过（return），不设置 isSendConnectedCapsule_
    NotificationUtils::SendNotification(NotificationType::NOTIFICATION_TYPE_CONNECTED_CAPSULE);

    // Then: 由于相机禁用跳过发送，isSendConnectedCapsule_ 保持 false
    EXPECT_EQ(NotificationUtils::isSendConnectedCapsule_, false);

    // Cleanup: 恢复状态
    NotificationUtils::isCameraDisabled_.store(false);
    NotificationUtils::isSendConnectedCapsule_ = false;

    DTEST_LOG << "NotificationUtilsTest SendNotification_CameraDisabled_Skip_001 end" << std::endl;
}

} // namespace OHOS