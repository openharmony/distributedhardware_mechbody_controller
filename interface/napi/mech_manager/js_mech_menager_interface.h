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

#ifndef JS_MECH_MENAGER_INTERFACE_H
#define JS_MECH_MENAGER_INTERFACE_H

#include <iremote_broker.h>
#include <message_parcel.h>

namespace OHOS {
namespace MechBodyController {
class IJsMechManager : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.mechManager.IJsMechManager");

public:
    virtual int32_t AttachStateChangeCallback(MessageParcel &data, MessageParcel &reply) = 0;

    virtual int32_t TrackingEventCallback(MessageParcel &data, MessageParcel &reply) = 0;

    virtual int32_t RotationAxesStatusChangeCallback(MessageParcel &data, MessageParcel &reply) = 0;

    virtual int32_t RotatePromiseFulfillment(MessageParcel &data, MessageParcel &reply) = 0;
};
} // namespace MechBodyController
} // namespace OHOS
#endif //JS_MECH_MENAGER_INTERFACE_H