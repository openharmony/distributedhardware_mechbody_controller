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

#ifndef MECHBODY_CONTROLLER_MECH_CONTROLLER_IPC_LISTENER_H
#define MECHBODY_CONTROLLER_MECH_CONTROLLER_IPC_LISTENER_H

#include <cstdint>
#include "ipc_skeleton.h"

namespace OHOS {
namespace MechBodyController {

enum class RemoteObjectType : int32_t{
    DEVICE_ATTACH_CALLBACK = 0,
    TRACKING_EVENT_CALLBACK = 1,
    ROTATION_AXES_STATUS_CALLBACK = 2,
    COMMAND_CHANNEL = 3,
};
class MechControllerIpcDeathListener : public IRemoteObject::DeathRecipient {
public:
    void OnRemoteDied(const wptr<IRemoteObject> &object) override;

public:
    uint64_t tokenId_;
    RemoteObjectType objectType_;
};
}
}

#endif //MECHBODY_CONTROLLER_MECH_CONTROLLER_IPC_LISTENER_H
