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

#ifndef OHOS_SUBSCRIBE_EVENT_UTILS_H
#define OHOS_SUBSCRIBE_EVENT_UTILS_H

#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "common_event.h"


namespace OHOS {
namespace MechBodyController {

class ExtCommonEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    explicit ExtCommonEventSubscriber(const EventFwk::CommonEventSubscribeInfo &info) : CommonEventSubscriber(info)
    {}
    ~ExtCommonEventSubscriber()
    {}
    void OnReceiveEvent(const EventFwk::CommonEventData &data) override;
};

class BaseService {
public:
    BaseService() = default;
    ~BaseService() = default;
    virtual void OnReceiveEvent(std::string action, const EventFwk::CommonEventData &data);
};

class SubscribeEventUtils {
public:
    static SubscribeEventUtils &GetInstance();
    void Init();
    void UnInit();
    void AddEventService(BaseService *service);
    void OnReceiveEvent(std::string action, const EventFwk::CommonEventData &data);

private:
    SubscribeEventUtils() = default;
    ~SubscribeEventUtils() = default;
    void CommonEventInit();

private:
    std::shared_ptr<ExtCommonEventSubscriber> commonEventMonitor_ = nullptr;
    std::vector<BaseService *> serviceVec_;
};
} // namespace MechBodyController
} // namespace OHOS
#endif