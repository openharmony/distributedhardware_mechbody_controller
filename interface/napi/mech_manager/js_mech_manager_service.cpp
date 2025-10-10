/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "mechbody_controller_log.h"
#include "js_mech_manager_service.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "JsMechManagerService";
}

JsMechManagerService& JsMechManagerService::GetInstance()
{
    static auto instance = new JsMechManagerService();
    return *instance;
}

JsMechManagerService::JsMechManagerService() {
}

JsMechManagerService::~JsMechManagerService() {
    HILOGI("~JsMechManagerService.");
}

int32_t JsMechManagerService::AttachStateChangeCallback(const AttachmentState &attachmentState,
    const std::shared_ptr<MechInfo> &mechInfo)
{
    CHECK_POINTER_RETURN_VALUE(mechInfo, INVALID_PARAMETERS_ERR, "mechInfo");
    for (const auto &item: attachStateChangeCallback_) {
        auto task = [item, attachmentState, mechInfo]() {
            napi_handle_scope scope;
            napi_open_handle_scope(item.env, &scope);

            napi_value jsEvent;
            napi_create_object(item.env, &jsEvent);

            napi_value jsState;
            napi_create_int32(item.env, static_cast<int32_t>(attachmentState), &jsState);
            napi_set_named_property(item.env, jsEvent, "state", jsState);

            napi_value jsMechInfo;
            napi_create_object(item.env, &jsMechInfo);
            napi_value jsMechId;
            napi_value jsMechType;
            napi_value jsMechName;
            napi_create_int32(item.env, mechInfo->mechId, &jsMechId);
            napi_create_int32(item.env, static_cast<int32_t>(mechInfo->mechType), &jsMechType);
            napi_create_string_utf8(item.env, mechInfo->mechName.c_str(), NAPI_AUTO_LENGTH, &jsMechName);
            napi_set_named_property(item.env, jsMechInfo, "mechId", jsMechId);
            napi_set_named_property(item.env, jsMechInfo, "mechType", jsMechType);
            napi_set_named_property(item.env, jsMechInfo, "mechName", jsMechName);
            napi_set_named_property(item.env, jsEvent, "mechInfo", jsMechInfo);

            napi_value callback;
            napi_get_reference_value(item.env, item.callbackRef, &callback);

            napi_value global;
            napi_get_global(item.env, &global);

            napi_value result;
            napi_call_function(item.env, global, callback, 1, &jsEvent, &result);

            napi_close_handle_scope(item.env, scope);
        };
        napi_send_event(item.env, task, napi_eprio_high);
    }
    HILOGI("end");
    return ERR_OK;
}

int32_t JsMechManagerService::TrackingEventCallback(const int32_t &mechId,
    const TrackingEvent &trackingEvent)
{
    HILOGI("start. mechId: %{public}d, TrackingEvent: %{public}d", mechId, static_cast<int32_t>(trackingEvent));
    for (const auto &item: trackingEventCallback) {
        auto task = [item, mechId, trackingEvent]() {
            napi_handle_scope scope;
            napi_open_handle_scope(item.env, &scope);

            napi_value jsEvent;
            napi_create_object(item.env, &jsEvent);

            napi_value jsMechId;
            napi_create_int32(item.env, mechId, &jsMechId);
            napi_set_named_property(item.env, jsEvent, "mechId", jsMechId);

            napi_value jsTrackingEvent;
            napi_create_int32(item.env, static_cast<int32_t>(trackingEvent), &jsTrackingEvent);
            napi_set_named_property(item.env, jsEvent, "event", jsTrackingEvent);

            napi_value callback;
            napi_get_reference_value(item.env, item.callbackRef, &callback);

            napi_value global;
            napi_get_global(item.env, &global);

            napi_value result;
            napi_call_function(item.env, global, callback, 1, &jsEvent, &result);

            napi_close_handle_scope(item.env, scope);
        };
        napi_send_event(item.env, task, napi_eprio_high);
    }

    HILOGI("end");
    return ERR_OK;
}

int32_t JsMechManagerService::RotationAxesStatusChangeCallback(const int32_t &mechId,
    const std::shared_ptr<RotationAxesStatus> &rotationAxesStatus)
{
    CHECK_POINTER_RETURN_VALUE(rotationAxesStatus, INVALID_PARAMETERS_ERR, "rotationAxesStatus");
    for (const auto &item: rotateAxisStatusChangeCallback) {
        auto task = [item, mechId, rotationAxesStatus]() {
            napi_handle_scope scope;
            napi_open_handle_scope(item.env, &scope);

            napi_value jsEvent;
            napi_create_object(item.env, &jsEvent);
            napi_value jsMechId;
            napi_create_int32(item.env, mechId, &jsMechId);
            napi_set_named_property(item.env, jsEvent, "mechId", jsMechId);

            napi_value jsStatus;
            napi_create_object(item.env, &jsStatus);
            napi_value jsYawEnabled;
            napi_value jsRollEnabled;
            napi_value jsPitchEnabled;
            napi_value jsYawLimited;
            napi_value jsRollLimited;
            napi_value jsPitchLimited;
            napi_get_boolean(item.env, rotationAxesStatus->yawEnabled, &jsYawEnabled);
            napi_get_boolean(item.env, rotationAxesStatus->rollEnabled, &jsRollEnabled);
            napi_get_boolean(item.env, rotationAxesStatus->pitchEnabled, &jsPitchEnabled);
            napi_create_int32(item.env, static_cast<int32_t>(rotationAxesStatus->yawLimited), &jsYawLimited);
            napi_create_int32(item.env, static_cast<int32_t>(rotationAxesStatus->rollLimited), &jsRollLimited);
            napi_create_int32(item.env, static_cast<int32_t>(rotationAxesStatus->pitchLimited),
                              &jsPitchLimited);
            napi_set_named_property(item.env, jsStatus, "yawEnabled", jsYawEnabled);
            napi_set_named_property(item.env, jsStatus, "rollEnabled", jsRollEnabled);
            napi_set_named_property(item.env, jsStatus, "pitchEnabled", jsPitchEnabled);
            napi_set_named_property(item.env, jsStatus, "yawLimited", jsYawLimited);
            napi_set_named_property(item.env, jsStatus, "rollLimited", jsRollLimited);
            napi_set_named_property(item.env, jsStatus, "pitchLimited", jsPitchLimited);
            napi_set_named_property(item.env, jsEvent, "status", jsStatus);

            napi_value callback;
            napi_get_reference_value(item.env, item.callbackRef, &callback);

            napi_value global;
            napi_get_global(item.env, &global);

            napi_value result;
            napi_call_function(item.env, global, callback, 1, &jsEvent, &result);

            napi_close_handle_scope(item.env, scope);
        };
        napi_send_event(item.env, task, napi_eprio_high);
    }

    HILOGI("end");
    return ERR_OK;
}

int32_t JsMechManagerService::RotatePromiseFulfillment(const std::string &cmdId,
    const int32_t &result)
{
    HILOGI("RotatePrimiseFulfillmentParam cmdId: %{public}s", cmdId.c_str());
    std::shared_ptr<RotatePrimiseFulfillmentParam> param = JsMechManagerService::promiseParams_[cmdId];
    if(param == nullptr){
        HILOGE("RotatePrimiseFulfillmentParam is nullptr, cmdId: %{public}s", cmdId.c_str());
        return ERR_OK;
    }
    auto task = [cmdId, param, result]() {
        napi_handle_scope scope;
        napi_open_handle_scope(param->env, &scope);

        // 创建 JavaScript 返回值
        napi_value jsResult;
        napi_create_int32(param->env, result, &jsResult);

        // 解析 Promise
        napi_status status = napi_resolve_deferred(param->env, param->deferred, jsResult);
        if (status != napi_ok) {
            HILOGI("Promise Fulfillment failed for cmdId: %{public}s", cmdId.c_str());
        }
        JsMechManagerService::GetInstance().promiseParams_.erase(cmdId);
        napi_close_handle_scope(param->env, scope);
    };
    napi_status status = napi_send_event(param->env, task, napi_eprio_high);
    HILOGI("RotatePromise send result: %{public}s", status == napi_ok ? "success" : "failed");
    return ERR_OK;
}

int32_t JsMechManagerService::SearchTargetCallback(std::string &cmdId, const int32_t &targetsNum, const int32_t &result)
{
    HILOGI("cmdId: %{public}s; targetCount: %{public}d", cmdId.c_str(), targetsNum);
    if (searchTargetCallback_.find(cmdId) == searchTargetCallback_.end()) {
        HILOGE("searchTarget Callback is nullptr, cmdId: %{public}s", cmdId.c_str());
        return ERR_OK;
    }
    std::shared_ptr<RotatePrimiseFulfillmentParam> param = searchTargetCallback_[cmdId];
    auto task = [cmdId, param, targetsNum]() {
        napi_handle_scope scope;
        napi_open_handle_scope(param->env, &scope);

        napi_value jsEvent;
        napi_create_object(param->env, &jsEvent);

        napi_value targetsNumJs;
        napi_create_int32(param->env, targetsNum, &targetsNumJs);
        napi_set_named_property(param->env, jsEvent, "targetCount", targetsNumJs);

        napi_status status = napi_resolve_deferred(param->env, param->deferred, jsEvent);
        if (status != napi_ok) {
            HILOGI("Promise Fulfillment failed for cmdId: %{public}s", cmdId.c_str());
        }
        JsMechManagerService::GetInstance().searchTargetCallback_.erase(cmdId);
        napi_close_handle_scope(param->env, scope);
    };
    napi_send_event(param->env, task, napi_eprio_high);

    return ERR_OK;
}
} // namespace MechManager
} // namespace OHOS