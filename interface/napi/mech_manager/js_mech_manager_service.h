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

#ifndef JS_MECH_MANAGER_SERVICE_H
#define JS_MECH_MANAGER_SERVICE_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <set>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "mechbody_controller_types.h"

#include "js_mech_menager_interface.h"
#include "mechbody_controller_ipc_interface_code.h"

namespace OHOS {
namespace MechBodyController {

struct RotatePrimiseFulfillmentParam {
    std::string cmdId;
    napi_env env;
    napi_deferred deferred;
    bool isReturnVoid = false;
};

struct CallbackFunctionInfo {
    napi_env env;
    napi_ref callbackRef;

    bool operator<(const CallbackFunctionInfo &other) const
    {
        napi_value storedCallback;
        napi_get_reference_value(env, callbackRef, &storedCallback);

        napi_value compareCallback;
        napi_get_reference_value(other.env, other.callbackRef, &compareCallback);

        bool isSame = false;
        napi_strict_equals(env, storedCallback, compareCallback, &isSame);
        return !isSame && (callbackRef < other.callbackRef);
    }
};

class JsMechManagerService {
public:
    static JsMechManagerService& GetInstance();
private:
    JsMechManagerService(const JsMechManagerService&) = delete;
    JsMechManagerService& operator= (const JsMechManagerService&) = delete;
    JsMechManagerService(JsMechManagerService&&) = delete;
    JsMechManagerService& operator= (JsMechManagerService&&) = delete;

public:

    JsMechManagerService();

    ~JsMechManagerService();

    int32_t AttachStateChangeCallback(const AttachmentState &attachmentState,
        const std::shared_ptr<MechInfo> &mechInfo);

    int32_t TrackingEventCallback(const int32_t &mechId, const TrackingEvent &trackingEvent);

    int32_t RotationAxesStatusChangeCallback(const int32_t &mechId,
        const std::shared_ptr<RotationAxesStatus> &rotationAxesStatus);

    int32_t RotatePromiseFulfillment(const std::string &cmdId,
        const int32_t &result);

    int32_t SearchTargetCallback(std::string &cmdId, const int32_t &targetsNum, const int32_t &result);

public:
    std::mutex attachStateChangeCallbackMutex_;
    std::set<CallbackFunctionInfo> attachStateChangeCallback_;
    std::mutex trackingEventCallbackMutex_;
    std::set<CallbackFunctionInfo> trackingEventCallback;
    std::mutex rotateAxisStatusChangeCallbackMutex_;
    std::set<CallbackFunctionInfo> rotateAxisStatusChangeCallback;
    std::mutex promiseParamsMutex_;
    std::map<std::string, std::shared_ptr<RotatePrimiseFulfillmentParam>> promiseParams_;
    std::mutex searchTargetCallbackMutex_;
    std::map<std::string, std::shared_ptr<RotatePrimiseFulfillmentParam>> searchTargetCallback_;
};
} // namespace MechManager
} // namespace OHOS

#endif //JS_MECH_MANAGER_SERVICE_H