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

#include "mc_subscription_center.h"

#include <map>
#include <memory>
#include <string>

#include "mechbody_controller_enums.h"
#include "mechbody_controller_log.h"
#include "./command/mc_command_factory.h"

namespace OHOS {
namespace MechBodyController {
SubscriptionCenter& SubscriptionCenter::GetInstance()
{
    static auto instance = new SubscriptionCenter();
    return *instance;
}

namespace {
    const std::string TAG = "SubscriptionCenter";
}

int32_t SubscriptionCenter::Subscribe(uint16_t type, const std::shared_ptr<IMechEventListener> &listener)
{
    CHECK_POINTER_RETURN_VALUE(listener, INVALID_PARAMETERS_ERR, "listener");
    HILOGI("called, type: 0x%{public}x", type);
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::shared_ptr<IMechEventListener>> callbacks;
    auto it = subscribers_.find(type);
    if (it == subscribers_.end()) {
        callbacks.push_back(listener);
        subscribers_[type] = callbacks;
        return ERR_OK;
    }
    callbacks = it->second;
    for (auto callback : callbacks) {
        if (callback == listener) {
            HILOGW("this callback function has been registered.");
            return ERR_OK;
        }
    }
    callbacks.push_back(listener);
    subscribers_[type] = callbacks;
    return ERR_OK;
}

int32_t SubscriptionCenter::UnSubscribe(uint16_t type, const std::shared_ptr<IMechEventListener> &listener)
{
    CHECK_POINTER_RETURN_VALUE(listener, INVALID_PARAMETERS_ERR, "listener");
    HILOGI("called, type: %{public}d", type);
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = subscribers_.find(type);
    if (it == subscribers_.end()) {
        HILOGW("this callback function can not found.");
        return ERR_OK;
    }
    std::vector<std::shared_ptr<IMechEventListener>> callbacks = it->second;
    auto it1 = std::find(callbacks.begin(), callbacks.end(), listener);
    if (it1 != callbacks.end()) {
        HILOGW("this callback function has been found.");
        callbacks.erase(it1);
        subscribers_[type] = callbacks;
    } else {
        HILOGW("this callback function can not found.");
        return UNREGISTER_FAILED_ERR;
    }
    HILOGI("end");
    return ERR_OK;
}

int32_t SubscriptionCenter::Notify(const std::shared_ptr<CommandBase> &cmd)
{
    CHECK_POINTER_RETURN_VALUE(cmd, INVALID_PARAMETERS_ERR, "cmd");
    HILOGD("cmdType : 0x%{public}x, cmdId: %{public}d", cmd->GetCmdType(), cmd->GetCmdId());

    std::vector<std::shared_ptr<IMechEventListener>> callbacks;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = subscribers_.find(cmd->GetCmdType());
        if (it == subscribers_.end()) {
            return NOTIFICATION_FAILED_ERR;
        }
        callbacks = it->second;
    }
    for (auto callback : callbacks) {
        if (callback == nullptr) {
            continue;
        }
        switch (cmd->GetCmdType()) {
            case CMD_TYPE_BUTTON_EVENT_NOTIFY:
                callback->MechButtonEventNotify(std::static_pointer_cast<RegisterMechCameraKeyEventCmd>(cmd));
                break;
            case CMD_TYPE_PARAM_NOTIFY:
                callback->MechParamNotify(std::static_pointer_cast<RegisterMechStateInfoCmd>(cmd));
                break;
            case CMD_TYPE_ATTITUDE_NOTIFY:
                callback->MechAttitudeNotify(std::static_pointer_cast<RegisterMechPositionInfoCmd>(cmd));
                break;
            case CMD_TYPE_EXE_RESULT_NOTIFY:
                callback->MechExecutionResultNotify(std::static_pointer_cast<RegisterMechControlResultCmd>(cmd));
                break;
            case CMD_TYPE_WHEEL_DATA_NOTIFY:
                callback->MechWheelZoomNotify(std::static_pointer_cast<RegisterMechWheelDataCmd>(cmd));
                break;
            case CMD_TYPE_TRACKING_ENABLED_NOTIFY:
                callback->MechTrackingStatusNotify(std::static_pointer_cast<NormalRegisterMechTrackingEnableCmd>(cmd));
                break;
            case CMD_TYPE_NORMAL_BUTTON_EVENT_NOTIFY:
                callback->MechButtonEventNotify(std::static_pointer_cast<NormalRegisterMechKeyEventCmd>(cmd));
                break;
            case CMD_TYPE_NORMAL_ATTITUDE_NOTIFY:
                callback->MechAttitudeNotify(std::static_pointer_cast<NormalRegisterMechPositionInfoCmd>(cmd));
                break;
            case CMD_TYPE_NORMAL_GENERIC_NOTIFY:
                callback->MechGenericEventNotify(std::static_pointer_cast<NormalRegisterMechGenericEventCmd>(cmd));
                break;
            default:
                HILOGD("Not found cmdType : 0x%{public}x, cmdId: %{public}d", cmd->GetCmdType(), cmd->GetCmdId());
                break;
        }
    }
    HILOGD("end");
    return ERR_OK;
}
}  // namespace MechBodyController
}  // namespace OHOS
