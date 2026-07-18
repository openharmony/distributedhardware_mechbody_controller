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

#ifndef OHOS_NOTIFICATION_UTILS_H
#define OHOS_NOTIFICATION_UTILS_H

#include <nlohmann/json.hpp>
#include <functional>
#include <atomic>
#include "notification_helper.h"
#include "notification_content.h"
#include "notification_request.h"
#include "configuration_observer_stub.h"
#include "pixel_map.h"
#include "mechbody_controller_enums.h"
using json = nlohmann::json;


enum NotificationType {
    NOTIFICATION_TYPE_CONNECTED_CAPSULE = 1
};


namespace OHOS {
namespace MechBodyController {
class NotificationUtils {
public:
    inline static std::atomic<bool> isTrackingEnabled_ = true;

    inline static MechType mechType_ = MechType::PORTABLE_GIMBAL;
    
    static void SendNotification(NotificationType notificationType);

    static void CancelNotification(NotificationType notificationType);

    static void OnLanguageChanged();

private:
    static json GetConnectedCapsuleNotificationConfig();
    static json BuildConnectedCapsuleLiveViewContent(const std::string &contentTitle,
        const std::string &contentText, const std::string &capsuleTitle, const std::string &icon);
    
    static void ParseNotificationConfigJson(Notification::NotificationRequest &request, json configJson);
    static void ParseBasicFields(Notification::NotificationRequest& request, const json& configJson);
    static void ParseBasicIntegerFields(Notification::NotificationRequest& request, const json& configJson);
    static void ParseBasicBooleanFields(Notification::NotificationRequest& request, const json& configJson);
    static void ParseAdvancedFields(Notification::NotificationRequest& request, const json& configJson);
    static void ParseExtraInfoFields(Notification::NotificationRequest& request, const json& configJson);
    static void ParseIntegerField(Notification::NotificationRequest& request,
        const json& configJson, const std::string& fieldName, std::function<void(int32_t)> setter);
    static void ParseUnsignedField(Notification::NotificationRequest& request,
        const json& configJson, const std::string& fieldName, std::function<void(uint32_t)> setter);
    static void ParseExtraInfoArrayFields(
        std::shared_ptr<AAFwk::WantParams>& extras, const std::string key, const json& value);

    static void ParseNormalContentJson(Notification::NotificationRequest &request, json contentJson);

    static void ParseLiveViewContentJson(Notification::NotificationRequest &request, json contentJson);

    static void ParseWantAgentJson(Notification::NotificationRequest &request, json wantAgentJson);

    static Notification::NotificationCapsule ParseCapsuleJson(json capsuleJson);

    static Notification::NotificationLocalLiveViewButton ParseLiveViewButtonsJson(json liveViewButtonsJson);

    static void SubscribeLocalLiveViewNotification();

    static void RegisterConfigurationObserver();

private:
    inline static bool isSendConnectedCapsule_ = false;

    inline static bool isLocalLiveViewSubscriber_ = false;

    inline static sptr<AppExecFwk::IConfigurationObserver> configChangeObserver_ = nullptr;

    inline static std::shared_ptr<Notification::NotificationLocalLiveViewSubscriber> localLiveViewSubscriber_ = nullptr;

    inline static std::map<std::string, std::shared_ptr<Media::PixelMap>> iconPixelMaps_;
};

class LocalLiveViewSubscriber : public Notification::NotificationLocalLiveViewSubscriber {
public:
    virtual void OnConnected() override;

    virtual void OnDisconnected() override;

    virtual void OnResponse(int32_t notificationId, sptr<Notification::NotificationButtonOption> buttonOption) override;

    virtual void OnDied() override;
};

class ReminderConfigChangeObserver final : public AppExecFwk::ConfigurationObserverStub {
public:
    ReminderConfigChangeObserver() = default;
    ~ReminderConfigChangeObserver() = default;

public:
    void OnConfigurationUpdated(const AppExecFwk::Configuration &configuration) override;

private:
    std::string languageInfo_;
};
} // namespace MechBodyController
} // namespace OHOS
#endif // OHOS_NOTIFICATION_UTILS_H