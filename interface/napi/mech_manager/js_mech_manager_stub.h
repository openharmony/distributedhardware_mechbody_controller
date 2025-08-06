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

#ifndef JS_MECH_MANAGER_STUB_H
#define JS_MECH_MANAGER_STUB_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <mutex>

#include <iremote_stub.h>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "ipc_skeleton.h"

#include "mechbody_controller_ipc_interface_code.h"
#include "js_mech_menager_interface.h"
#include "js_mech_manager_service.h"

namespace OHOS {
namespace MechBodyController {

class JsMechManagerStub : public IRemoteStub<IJsMechManager> {

public:

    JsMechManagerStub();

    virtual ~JsMechManagerStub();

    void SetDeathRecipient(sptr<IRemoteObject::DeathRecipient> deathRecipient);

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data,
        MessageParcel &reply, MessageOption &option) override;

    int32_t AttachStateChangeCallback(MessageParcel &data, MessageParcel &reply) override;

    int32_t TrackingEventCallback(MessageParcel &data, MessageParcel &reply) override;

    int32_t RotationAxesStatusChangeCallback(MessageParcel &data, MessageParcel &reply) override;

    int32_t RotatePromiseFulfillment(MessageParcel &data, MessageParcel &reply) override;

private:
    void InitMechManagerFunc();

private:
    using MechManagerFunc = int32_t(JsMechManagerStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<IMechBodyControllerCode, MechManagerFunc> mechManagerFuncMap_;

    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
};
} // namespace MechBodyController
} // namespace OHOS

#endif //JS_MECH_MANAGER_STUB_H