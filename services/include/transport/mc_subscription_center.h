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

#ifndef OHOS_MECH_SUBSCRIPTION_CENTER_H
#define OHOS_MECH_SUBSCRIPTION_CENTER_H

#include <map>
#include <mutex>
#include <string>

#include "mc_command_base.h"
#include "mc_event_listener.h"
#include "mechbody_controller_log.h"
#include "mc_data_buffer.h"

namespace OHOS {
namespace MechBodyController {
class SubscriptionCenter {
public:
    static SubscriptionCenter& GetInstance();
private:
    SubscriptionCenter() = default;
    ~SubscriptionCenter() = default;
    SubscriptionCenter(const SubscriptionCenter&) = delete;
    SubscriptionCenter& operator= (const SubscriptionCenter&) = delete;
    SubscriptionCenter(SubscriptionCenter&&) = delete;
    SubscriptionCenter& operator= (SubscriptionCenter&&) = delete;

public:
    int32_t Subscribe(uint16_t type, const std::shared_ptr<IMechEventListener> &listener);
    int32_t UnSubscribe(uint16_t type, const std::shared_ptr<IMechEventListener> &listener);
    int32_t Notify(const std::shared_ptr<CommandBase> &cmd);

private:
    mutable std::mutex mutex_;
    std::map<uint16_t, std::vector<std::shared_ptr<IMechEventListener>>> subscribers_;
};
}  // namespace MechBodyController
}  // namespace OHOS
#endif  // OHOS_MECH_SUBSCRIPTION_CENTER_H
