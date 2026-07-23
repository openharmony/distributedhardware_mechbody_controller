/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "notification_utils.h"

#include "mechbody_controller_log.h"
#include "os_account_manager.h"
#include "image_source.h"
#include "want_agent_info.h"
#include "want_agent_helper.h"
#include "resource_manager_utils.h"
#include "ipc_skeleton.h"
#include "app_mgr_client.h"
#include "array_wrapper.h"
#include "parameters.h"
#include "mechbody_controller_service.h"
#include "load_mechbody_adapter.h"
#include "bool_wrapper.h"


namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "NotificationUtils";
}
static constexpr int32_t MECHBODY_UID = 7811;
const int32_t MECHBODY_CONNECTED_CAPSULE_NOTIFICATION_ID = 1001;

const std::string TRACE_ENABLED = "focus_tracking";
const std::string TRACE_DISENABLED = "focus_paused";
const std::string SMART_TRACKING_ENABLED = "smart_focus_tracking";
const std::string SMART_TRACKING_DISENABLED = "smart_focus_paused";
const std::string GIMBAL_DEVICE_CONNECTED = "gimbal_connected";
const std::string WHEEL_BASE_DEVICE_CONNECTED = "wheel_base_connected";
const std::string TRACKING_ENABLED_CHANGE_BUTTON_CALL = "mech_intelligent_tracking";
const std::string MECHBODY_CONNECTED_CAPSULE_GIMBAL_ICON_NAME = "ic_gimbal_device";
const std::string MECHBODY_CONNECTED_CAPSULE_WHEEL_BASE_ICON_NAME = "ic_wheel_base_device";
const std::string MECHBODY_TRACKING_OPEN_BUTTON_ICON_NAME = "intelligent_tracking_1";
const std::string MECHBODY_TRACKING_CLOSE_BUTTON_ICON_NAME = "intelligent_tracking_0";
static constexpr const char* TRACKING_ENABLE_CHANGE_BUTTON_NAME = "TrackingEnableChange";

void NotificationUtils::SendNotification(NotificationType notificationType)
{
    HILOGI("start notificationType: %{public}d", notificationType);

    json notificationConfig;
    switch (notificationType) {
        case NotificationType::NOTIFICATION_TYPE_CONNECTED_CAPSULE:
            HILOGI("NOTIFICATION_TYPE_CONNECTED_CAPSULE, TrackingEnabled: %{public}d", isTrackingEnabled_.load());
            if (!MechbodyAdapterUtils::IsSupportBackground()) {
                HILOGW("NOTIFICATION_TYPE_CONNECTED_CAPSULE, not support sw!");
                return;
            }
            SubscribeLocalLiveViewNotification();
            notificationConfig = GetConnectedCapsuleNotificationConfig();
            isSendConnectedCapsule_ = true;
            break;
        default:
            HILOGE("notificationType is not exist!");
            return;
    }
    // 注册系统语言变化监听
    RegisterConfigurationObserver();

    Notification::NotificationRequest request;
    ParseNotificationConfigJson(request, notificationConfig);

    int32_t result = Notification::NotificationHelper::PublishNotification(request);
    HILOGI("PublishNotification result : %{public}d", result);
}

void NotificationUtils::CancelNotification(NotificationType notificationType)
{
    HILOGI("start notificationType: %{public}d", notificationType);
    int32_t ret = ERR_OK;
    if (notificationType == NotificationType::NOTIFICATION_TYPE_CONNECTED_CAPSULE) {
        ret = Notification::NotificationHelper::CancelNotification(MECHBODY_CONNECTED_CAPSULE_NOTIFICATION_ID);
        isSendConnectedCapsule_ = false;
    }
    HILOGI("result = %{public}d", ret);
}

void NotificationUtils::OnLanguageChanged()
{
    HILOGI("enter OnLanguageChanged, isSendConnectedCapsule_: %{public}d", isSendConnectedCapsule_);
    if (isSendConnectedCapsule_) {
        SendNotification(NotificationType::NOTIFICATION_TYPE_CONNECTED_CAPSULE);
    }
}

json NotificationUtils::GetConnectedCapsuleNotificationConfig()
{
    HILOGI("start");
    int32_t userId;
    AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(MECHBODY_UID, userId);

    std::string contentTitle = ResourceManagerUtils::GetSystemStringByName(
        isTrackingEnabled_ ? SMART_TRACKING_ENABLED : SMART_TRACKING_DISENABLED);
    std::string capsuleTitle = ResourceManagerUtils::GetSystemStringByName(
        isTrackingEnabled_ ? TRACE_ENABLED : TRACE_DISENABLED);
    std::string trackingEnableChangeButtonCall = ResourceManagerUtils::GetSystemStringByName(
        TRACKING_ENABLED_CHANGE_BUTTON_CALL);

    std::string icon;
    std::string contentText;
    if (mechType_ == MechType::WHEEL_BASE) {
        icon = MECHBODY_CONNECTED_CAPSULE_WHEEL_BASE_ICON_NAME;
        contentText = ResourceManagerUtils::GetSystemStringByName(WHEEL_BASE_DEVICE_CONNECTED);
    } else {
        icon = MECHBODY_CONNECTED_CAPSULE_GIMBAL_ICON_NAME;
        contentText = ResourceManagerUtils::GetSystemStringByName(GIMBAL_DEVICE_CONNECTED);
    }

    json connectedCapsuleNotificationConfig = {
        {"creatorUid", MECHBODY_UID},
        {"creatorPid", getpid()},
        {"creatorUserId", userId},
        {"notificationId", MECHBODY_CONNECTED_CAPSULE_NOTIFICATION_ID},
        {"unremovable", true},
        {"slotType", Notification::NotificationConstant::SlotType::LIVE_VIEW},
        {"extraInfo", {
            {"hw_live_view_hidden_when_keyguard", true}
        }},
        {"liveViewContent", BuildConnectedCapsuleLiveViewContent(contentTitle, contentText, capsuleTitle, icon)},
        {"littleIcon", icon},
        {"badgeIconStyle", Notification::NotificationRequest::BadgeStyle::LITTLE}
    };
    return connectedCapsuleNotificationConfig;
}

json NotificationUtils::BuildConnectedCapsuleLiveViewContent(const std::string &contentTitle,
    const std::string &contentText, const std::string &capsuleTitle, const std::string &icon)
{
    json liveViewContent = {
        {"title", contentTitle},
        {"text", contentText},
        {"type", 35},
        {"capsule", {
            {"title", capsuleTitle},
            {"content", ""},
            {"icon", icon},
            {"backgroundColor", "#3B7DF0"}
        }},
        {"buttons", {
            {
                {"singleButtonName", TRACKING_ENABLE_CHANGE_BUTTON_NAME},
                {"singleButtonIcon", isTrackingEnabled_ ?
                    MECHBODY_TRACKING_OPEN_BUTTON_ICON_NAME :
                    MECHBODY_TRACKING_CLOSE_BUTTON_ICON_NAME},
            }
        }},
        {"flags", {
            Notification::NotificationLocalLiveViewContent::LiveViewContentInner::CAPSULE,
            Notification::NotificationLocalLiveViewContent::LiveViewContentInner::BUTTON,
        }}
    };
    return liveViewContent;
}

void NotificationUtils::ParseNotificationConfigJson(Notification::NotificationRequest& request, json configJson)
{
    HILOGI("start");
    if (!configJson.is_object()) {
        HILOGE("configJson Incorrect format!");
        return;
    }

    ParseBasicFields(request, configJson);
    ParseAdvancedFields(request, configJson);
    ParseExtraInfoFields(request, configJson);
}

void NotificationUtils::ParseBasicFields(Notification::NotificationRequest& request, const json& configJson)
{
    ParseBasicIntegerFields(request, configJson);
    ParseBasicBooleanFields(request, configJson);
}

void NotificationUtils::ParseBasicIntegerFields(Notification::NotificationRequest& request, const json& configJson)
{
    ParseIntegerField(request, configJson, "creatorUid",
        [&request](int32_t value) { request.SetCreatorUid(value); });
    ParseIntegerField(request, configJson, "creatorPid",
        [&request](pid_t value) { request.SetCreatorPid(value); });
    ParseIntegerField(request, configJson, "creatorUserId",
        [&request](int32_t value) { request.SetCreatorUserId(value); });
    ParseIntegerField(request, configJson, "notificationId",
        [&request](int32_t value) { request.SetNotificationId(value); });
    ParseUnsignedField(request, configJson, "notificationControlFlags",
        [&request](uint32_t value) { request.SetNotificationControlFlags(value); });
}

void NotificationUtils::ParseIntegerField(Notification::NotificationRequest &request, const json &configJson,
    const std::string &fieldName, std::function<void(int32_t)> setter)
{
    if (configJson.contains(fieldName)) {
        const auto &value = configJson[fieldName];
        if (value.is_number_integer()) {
            setter(value.get<int32_t>());
        } else {
            HILOGW("%{public}s type mismatch, expected integer!", fieldName.c_str());
        }
    } else {
        HILOGW("%{public}s is not exist!", fieldName.c_str());
    }
}

void NotificationUtils::ParseUnsignedField(Notification::NotificationRequest &request, const json &configJson,
    const std::string &fieldName, std::function<void(uint32_t)> setter)
{
    if (configJson.contains(fieldName)) {
        const auto &value = configJson[fieldName];
        if (value.is_number_unsigned()) {
            setter(value.get<uint32_t>());
        } else if (value.is_number_integer() && value.get<int64_t>() >= 0) {
            setter(static_cast<uint32_t>(value.get<int64_t>()));
        } else {
            HILOGW("%{public}s type mismatch or negative value, expected non-negative integer!", fieldName.c_str());
        }
    } else {
        HILOGW("%{public}s is not exist!", fieldName.c_str());
    }
}

void NotificationUtils::ParseBasicBooleanFields(Notification::NotificationRequest &request, const json &configJson)
{
    auto parseBool = [&](const std::string& fieldName, auto setter) {
        if (configJson.contains(fieldName)) {
            const auto &value = configJson[fieldName];
            if (value.is_boolean()) {
                setter(value.get<bool>());
            } else {
                HILOGW("%{public}s type mismatch , expected boolean!", fieldName.c_str());
            }
        } else {
            HILOGW("%{public}s is not exist!", fieldName.c_str());
        }
    };
    parseBool("tapDismissed", [&request](bool v) { request.SetTapDismissed(v); });
    parseBool("unremovable", [&request](bool v) { request.SetUnremovable(v); });
}

void NotificationUtils::ParseAdvancedFields(Notification::NotificationRequest& request, const json& configJson)
{
    if (configJson.contains("slotType")) {
        Notification::NotificationConstant::SlotType slotType = configJson["slotType"];
        request.SetSlotType(slotType);
    } else {
        HILOGW("slotType is not exist!");
    }

    if (configJson.contains("normalContent")) {
        json normalContentJson = configJson["normalContent"];
        ParseNormalContentJson(request, normalContentJson);
    } else {
        HILOGW("normalContent is not exist!");
    }

    if (configJson.contains("liveViewContent")) {
        json liveViewContentJson = configJson["liveViewContent"];
        ParseLiveViewContentJson(request, liveViewContentJson);
    } else {
        HILOGW("liveViewContent is not exist!");
    }

    if (configJson.contains("littleIcon")) {
        std::string iconName = configJson["littleIcon"];
        std::shared_ptr<Media::PixelMap> iconPixelMap;
        if (iconPixelMaps_.find(iconName) != iconPixelMaps_.end()) {
            iconPixelMap = iconPixelMaps_[iconName];
        } else {
            iconPixelMap = ResourceManagerUtils::GetPixelMapByName(iconName);
        }
        if (iconPixelMap != nullptr) {
            HILOGI("littleIcon is exist!");
            request.SetLittleIcon(iconPixelMap);
            iconPixelMaps_[iconName] = iconPixelMap;
        }
    } else {
        HILOGW("littleIcon is not exist!");
    }

    if (configJson.contains("badgeIconStyle")) {
        NotificationRequest::BadgeStyle badgeIconStyle = configJson["badgeIconStyle"];
        request.SetBadgeIconStyle(badgeIconStyle);
    } else {
        HILOGW("badgeIconStyle is not exist!");
    }

    if (configJson.contains("wantAgent")) {
        json wantAgent = configJson["wantAgent"];
        ParseWantAgentJson(request, wantAgent);
    } else {
        HILOGW("wantAgent is not exist!");
    }
}

void NotificationUtils::ParseExtraInfoFields(Notification::NotificationRequest& request, const json& configJson)
{
    std::shared_ptr<AAFwk::WantParams> extras = std::make_shared<AAFwk::WantParams>();

    json extraInfoJson = nullptr;
    if (configJson.contains("extraInfo")) {
        extraInfoJson = configJson["extraInfo"];
    } else {
        HILOGW("extraInfo is not exist!");
        return;
    }
    if (!extraInfoJson.is_object()) {
        HILOGE("extraInfoJson Incorrect format!");
        return;
    }

    for (auto& [key, value] : extraInfoJson.items()) {
        if (value.is_string()) {
            // 处理字符串类型
            extras->SetParam(key, AAFwk::String::Box(value.get<std::string>()));
        } else if (value.is_number()) {
            // 处理数字类型
            int64_t numValue = value.get<int64_t>();
            if (numValue < INT32_MIN || numValue > INT32_MAX) {
                HILOGE("Number value out of int32_t range for key: %{public}s", key.c_str());
                continue;
            }
            extras->SetParam(key, AAFwk::Integer::Box(static_cast<int32_t>(numValue)));
        } else if (value.is_array()) {
            // 处理数组类型
            ParseExtraInfoArrayFields(extras, key, value);
        } else if (value.is_boolean()) {
            // 处理布尔类型
            extras->SetParam(key, AAFwk::Boolean::Box(value));
        } else {
            HILOGW("Unsupported type for key: %{public}s", key.c_str());
        }
    }
    request.SetAdditionalData(extras);
}

void NotificationUtils::ParseExtraInfoArrayFields(
    std::shared_ptr<AAFwk::WantParams>& extras, const std::string key, const json& value)
{
    // 处理数组类型
    std::vector<std::string> arr;
    bool isValid = true;
    for (const auto& item : value) {
        if (!item.is_string()) {
            HILOGE("Array element is not string for key: %{public}s", key.c_str());
            isValid = false;
            break;
        }
        arr.push_back(item.get<std::string>());
    }
    if (!isValid) {
        HILOGE("isValid false key: %{public}s", key.c_str());
        return;
    }
    size_t size = arr.size();
    // 检查数组是否为空
    if (size == 0) {
        HILOGE("size is 0 key: %{public}s", key.c_str());
        return;
    }
    sptr<AAFwk::IArray> array = new (std::nothrow) AAFwk::Array(size, AAFwk::g_IID_IString);
    if (array == nullptr) {
        HILOGE("Failed to allocate array for key: %{public}s", key.c_str());
        return;
    }
    for (size_t i = 0; i < size; i++) {
        array->Set(i, AAFwk::String::Box(arr[i]));
    }
    extras->SetParam(key, array);
}

void NotificationUtils::ParseNormalContentJson(Notification::NotificationRequest &request, json contentJson)
{
    HILOGI("start");
    if (!contentJson.is_object()) {
        HILOGE("contentJson Incorrect format!");
        return;
    }

    auto normalContent = std::make_shared<Notification::NotificationNormalContent>();

    if (contentJson.contains("title")) {
        std::string title = contentJson["title"];
        normalContent->SetTitle(title);
    } else {
        HILOGW("title is not exist!");
    }

    if (contentJson.contains("text")) {
        std::string text = contentJson["text"];
        normalContent->SetText(text);
    } else {
        HILOGW("text is not exist!");
    }

    auto content = std::make_shared<Notification::NotificationContent>(normalContent);
    request.SetContent(content);
}

void NotificationUtils::ParseLiveViewContentJson(Notification::NotificationRequest &request, json contentJson)
{
    HILOGI("start");
    if (!contentJson.is_object()) {
        HILOGE("contentJson Incorrect format!");
        return;
    }

    auto liveViewContent = std::make_shared<Notification::NotificationLocalLiveViewContent>();

    if (contentJson.contains("title")) {
        std::string title = contentJson["title"];
        liveViewContent->SetTitle(title);
    } else {
        HILOGW("title is not exist!");
    }

    if (contentJson.contains("text")) {
        std::string text = contentJson["text"];
        liveViewContent->SetText(text);
    } else {
        HILOGW("text is not exist!");
    }

    if (contentJson.contains("type")) {
        int32_t type = contentJson["type"];
        liveViewContent->SetType(type);
    } else {
        HILOGW("type is not exist!");
    }

    if (contentJson.contains("capsule")) {
        json capsuleJson = contentJson["capsule"];
        auto capsule = ParseCapsuleJson(capsuleJson);
        liveViewContent->SetCapsule(capsule);
    } else {
        HILOGW("capsule is not exist!");
    }

    if (contentJson.contains("buttons")) {
        json buttonsJson = contentJson["buttons"];
        auto button = ParseLiveViewButtonsJson(buttonsJson);
        liveViewContent->SetButton(button);
    } else {
        HILOGW("buttons is not exist!");
    }

    if (contentJson.contains("flags")) {
        for (const int32_t flag : contentJson["flags"]) {
            liveViewContent->addFlag(flag);
        }
    } else {
        HILOGW("flags is not exist!");
    }

    auto content = std::make_shared<Notification::NotificationContent>(liveViewContent);
    request.SetContent(content);
}

void NotificationUtils::ParseWantAgentJson(Notification::NotificationRequest &request, json wantAgentJson)
{
    HILOGI("start");
    if (!wantAgentJson.is_object()) {
        HILOGE("wantAgentJson Incorrect format!");
        return;
    }

    int requestCode = 0;
    auto operationType = AbilityRuntime::WantAgent::WantAgentConstant::OperationType::START_ABILITY;
    std::vector<AbilityRuntime::WantAgent::WantAgentConstant::Flags> flags;
    std::vector<std::shared_ptr<AAFwk::Want>> wants;
    std::shared_ptr<AAFwk::WantParams> extraInfo = nullptr;

    if (wantAgentJson.contains("requestCode")) {
        requestCode = wantAgentJson["requestCode"];
    } else {
        HILOGW("requestCode is not exist!");
    }

    if (wantAgentJson.contains("operationType")) {
        operationType = wantAgentJson["operationType"];
    } else {
        HILOGW("operationType is not exist!");
    }

    if (wantAgentJson.contains("flags")) {
        for (const AbilityRuntime::WantAgent::WantAgentConstant::Flags item : wantAgentJson["flags"]) {
            flags.push_back(item);
        }
    } else {
        HILOGW("flags is not exist!");
    }

    if (wantAgentJson.contains("wants")) {
        for (const json& wantJson : wantAgentJson["wants"]) {
            auto want = std::make_shared<AAFwk::Want>();
            if (wantJson.contains("elementName")) {
                std::string bundleName = wantJson["elementName"]["bundleName"];
                std::string bundleNameAbility = wantJson["elementName"]["bundleNameAbility"];
                want->SetElementName(bundleName, bundleNameAbility);
            } else {
                HILOGW("elementName is not exist!");
            }
            wants.push_back(want);
        }
    } else {
        HILOGW("wants is not exist!");
    }
    
    AbilityRuntime::WantAgent::WantAgentInfo wantAgentInfo(requestCode, operationType, flags, wants, extraInfo);
    auto wantAgent = AbilityRuntime::WantAgent::WantAgentHelper::GetWantAgent(wantAgentInfo);
    request.SetWantAgent(wantAgent);
}

Notification::NotificationCapsule NotificationUtils::ParseCapsuleJson(json capsuleJson)
{
    HILOGI("start");
    Notification::NotificationCapsule capsule;

    if (!capsuleJson.is_object()) {
        HILOGE("capsuleJson Incorrect format!");
        return capsule;
    }

    if (capsuleJson.contains("title")) {
        std::string title = capsuleJson["title"];
        capsule.SetTitle(title);
    } else {
        HILOGW("title is not exist!");
    }

    if (capsuleJson.contains("content")) {
        std::string content = capsuleJson["content"];
        capsule.SetContent("");
    } else {
        HILOGW("content is not exist!");
    }

    if (capsuleJson.contains("icon")) {
        std::string iconName = capsuleJson["icon"];
        std::shared_ptr<Media::PixelMap> iconPixelMap;
        if (iconPixelMaps_.find(iconName) != iconPixelMaps_.end()) {
            iconPixelMap = iconPixelMaps_[iconName];
        } else {
            iconPixelMap = ResourceManagerUtils::GetPixelMapByName(iconName);
        }
        if (iconPixelMap != nullptr) {
            HILOGI("icon is exist!");
            capsule.SetIcon(iconPixelMap);
            iconPixelMaps_[iconName] = iconPixelMap;
        }
    } else {
        HILOGW("icon is not exist!");
    }

    if (capsuleJson.contains("backgroundColor")) {
        std::string backgroundColor = capsuleJson["backgroundColor"];
        capsule.SetBackgroundColor(backgroundColor);
    } else {
        HILOGW("backgroundColor is not exist!");
    }
    return capsule;
}

Notification::NotificationLocalLiveViewButton NotificationUtils::ParseLiveViewButtonsJson(json liveViewButtonsJson)
{
    HILOGI("start");
    Notification::NotificationLocalLiveViewButton button;

    if (!liveViewButtonsJson.is_array()) {
        HILOGE("liveViewButtonsJson Incorrect format!");
        return button;
    }
    
    for (const json& buttonJson : liveViewButtonsJson) {
        if (buttonJson.contains("singleButtonName")) {
            std::string singleButtonName = buttonJson["singleButtonName"];
            button.addSingleButtonName(singleButtonName);
        } else {
            HILOGW("singleButtonName is not exist!");
        }

        if (buttonJson.contains("singleButtonIcon")) {
            std::string iconName = buttonJson["singleButtonIcon"];
            std::shared_ptr<Media::PixelMap> iconPixelMap;
            if (iconPixelMaps_.find(iconName) != iconPixelMaps_.end()) {
                iconPixelMap = iconPixelMaps_[iconName];
            } else {
                iconPixelMap = ResourceManagerUtils::GetPixelMapByName(iconName);
            }
            if (iconPixelMap != nullptr) {
                HILOGI("singleButtonIcon is exist!");
                button.addSingleButtonIcon(iconPixelMap);
                iconPixelMaps_[iconName] = iconPixelMap;
            }
        } else {
            HILOGW("singleButtonIcon is not exist!");
        }
    }
    return button;
}

void NotificationUtils::SubscribeLocalLiveViewNotification()
{
    HILOGI("start isLocalLiveViewSubscriber: %{public}d", isLocalLiveViewSubscriber_);
    if (!isLocalLiveViewSubscriber_) {
        isLocalLiveViewSubscriber_ = true;
        localLiveViewSubscriber_ = std::make_shared<LocalLiveViewSubscriber>();
        int32_t result = Notification::NotificationHelper::SubscribeLocalLiveViewNotification(
            *localLiveViewSubscriber_, true);
        HILOGI("SubscribeLocalLiveViewNotification result : %{public}d", result);
    }
}

void NotificationUtils::RegisterConfigurationObserver()
{
    HILOGI("Enter.");
    if (configChangeObserver_ != nullptr) {
        return;
    }

    auto appMgrClient = std::make_shared<AppExecFwk::AppMgrClient>();
    if (appMgrClient->ConnectAppMgrService() != ERR_OK) {
        HILOGW("Connect to app mgr service failed.");
        return;
    }

    configChangeObserver_ = sptr<AppExecFwk::IConfigurationObserver>(
        new (std::nothrow) ReminderConfigChangeObserver());
    if (appMgrClient->RegisterConfigurationObserver(configChangeObserver_) != ERR_OK) {
        HILOGE("Register configuration observer failed.");
    }
}

void LocalLiveViewSubscriber::OnConnected()
{
    HILOGI("LocalLiveViewSubscriber OnConnected");
}

void LocalLiveViewSubscriber::OnDisconnected()
{
    HILOGI("LocalLiveViewSubscriber OnDisconnected");
}

void LocalLiveViewSubscriber::OnDied()
{
    HILOGI("LocalLiveViewSubscriber OnDied");
}

void LocalLiveViewSubscriber::OnResponse(
    int32_t notificationId, sptr<Notification::NotificationButtonOption> buttonOption)
{
    HILOGI("LocalLiveViewSubscriber enter notificationId: %{public}d", notificationId);

    auto buttonName = buttonOption->GetButtonName();
    if (buttonName == TRACKING_ENABLE_CHANGE_BUTTON_NAME) {
        HILOGI("TRACKING_ENABLE_CHANGE_BUTTON_NAME, TrackingEnabled: %{public}d",
            NotificationUtils::isTrackingEnabled_.load());
        bool trackingEnabled = !NotificationUtils::isTrackingEnabled_;
        MechBodyControllerService::GetInstance().SetTrackingEnabled(trackingEnabled, true);
    }
}

void ReminderConfigChangeObserver::OnConfigurationUpdated(const AppExecFwk::Configuration &configuration)
{
    HILOGI("called");
    std::string newLanguageInfo = configuration.GetItem(AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE);
    if (!newLanguageInfo.empty() && newLanguageInfo != languageInfo_) {
        HILOGI("language change: %{public}s -> %{public}s", languageInfo_.c_str(), newLanguageInfo.c_str());
        languageInfo_ = newLanguageInfo;
        NotificationUtils::OnLanguageChanged();
    }
}
}
}