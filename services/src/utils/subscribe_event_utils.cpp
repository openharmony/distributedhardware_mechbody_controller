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

#include "subscribe_event_utils.h"

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "SubscribeEventUtils";
}

SubscribeEventUtils &SubscribeEventUtils::GetInstance()
{
    static SubscribeEventUtils instance;
    return instance;
}

void SubscribeEventUtils::Init()
{
    HILOGI("enter");
    CommonEventInit();
}

void SubscribeEventUtils::UnInit()
{
    HILOGI("enter");
}

void SubscribeEventUtils::CommonEventInit()
{
    HILOGI("enter");
    if (commonEventMonitor_ != nullptr) {
        HILOGI("commonEvent already Subscribed");
        return;
    }
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    commonEventMonitor_ = std::make_shared<ExtCommonEventSubscriber>(subscribeInfo);
    bool ret = OHOS::EventFwk::CommonEventManager::SubscribeCommonEvent(commonEventMonitor_);
    HILOGI("commonEventMonitor subRet: %{public}d", ret);
}

void SubscribeEventUtils::AddEventService(BaseService *service)
{
    HILOGI("enter.");
    serviceVec_.push_back(service);
    HILOGI("end. serviceVec_ size %{public}zu", serviceVec_.size());
}

void SubscribeEventUtils::OnReceiveEvent(std::string action, const EventFwk::CommonEventData &data)
{
    HILOGI("enter. action = %{public}s, serviceVec_ size %{public}zu", action.c_str(), serviceVec_.size());
    for (auto &service : serviceVec_) {
        service->OnReceiveEvent(action, data);
    }
}

void ExtCommonEventSubscriber::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    HILOGI("enter.");
    std::string action = data.GetWant().GetAction();
    SubscribeEventUtils::GetInstance().OnReceiveEvent(action, data);
}
}
}