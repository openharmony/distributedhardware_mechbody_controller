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

#ifndef OHOS_MECHBODY_CONTROLLER_STUB_H
#define OHOS_MECHBODY_CONTROLLER_STUB_H

#include <iremote_stub.h>
#include <map>

#include "mechbody_controller_interface.h"

namespace OHOS {
namespace MechBodyController {
class MechBodyControllerStub : public IRemoteStub<IMechBodyController> {
public:
    MechBodyControllerStub();
    ~MechBodyControllerStub();
    int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

private:
    void InitFuncsInner();

    int32_t AttachStateChangeListenOnInner(MessageParcel &data, MessageParcel &reply);
    int32_t AttachStateChangeListenOffInner(MessageParcel &data, MessageParcel &reply);
    int32_t GetAttachedDevicesInner(MessageParcel &data, MessageParcel &reply);
    int32_t SetUserOperationInner(MessageParcel &data, MessageParcel &reply);

    int32_t SetCameraTrackingEnabledInner(MessageParcel &data, MessageParcel &reply);
    int32_t GetCameraTrackingEnabledInner(MessageParcel &data, MessageParcel &reply);
    int32_t TrackingEventListenOnInner(MessageParcel &data, MessageParcel &reply);
    int32_t TrackingEventListenOffInner(MessageParcel &data, MessageParcel &reply);
    int32_t SetCameraTrackingLayoutInner(MessageParcel &data, MessageParcel &reply);
    int32_t GetCameraTrackingLayoutInner(MessageParcel &data, MessageParcel &reply);

    int32_t RegisterCmdChannelInner(MessageParcel &data, MessageParcel &reply);
    int32_t RotateByDegreeInner(MessageParcel &data, MessageParcel &reply);
    int32_t RotateToEulerAnglesInner(MessageParcel &data, MessageParcel &reply);
    int32_t GetMaxRotationTimeInner(MessageParcel &data, MessageParcel &reply);
    int32_t GetMaxRotationSpeedInner(MessageParcel &data, MessageParcel &reply);
    int32_t RotateBySpeedInner(MessageParcel &data, MessageParcel &reply);
    int32_t StopMovingInner(MessageParcel &data, MessageParcel &reply);
    int32_t GetRotationAnglesInner(MessageParcel &data, MessageParcel &reply);
    int32_t GetRotationDegreeLimitsInner(MessageParcel &data, MessageParcel &reply);
    int32_t GetRotationAxesStatusInner(MessageParcel &data, MessageParcel &reply);
    int32_t RotationAxesStatusChangeListenOnInner(MessageParcel &data, MessageParcel &reply);
    int32_t RotationAxesStatusChangeListenOffInner(MessageParcel &data, MessageParcel &reply);

    int32_t Test(MessageParcel &data, MessageParcel &reply);

private:
    using MechBodyControllerFunc = int32_t(MechBodyControllerStub::*)(MessageParcel& data, MessageParcel& reply);
    std::map<uint32_t, MechBodyControllerFunc> funcsMap_;
};
} // namespace MechBodyController
} // namespace OHOS
#endif // OHOS_MECHBODY_CONTROLLER_STUB_H
