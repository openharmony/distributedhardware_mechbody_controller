/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_MECHBODY_CONTROLLER_INTERFACE_H
#define OHOS_MECHBODY_CONTROLLER_INTERFACE_H

#include <vector>
#include <set>
#include "mechbody_controller_types.h"

namespace OHOS {
namespace MechBodyController {
class IMechBodyController : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.MechBodyController.IMechBodyController");

public:
    virtual int32_t OnDeviceConnected(int32_t mechId) = 0;
    virtual int32_t OnDeviceDisconnected(int32_t mechId) = 0;

    virtual int32_t RegisterAttachStateChangeCallback(const sptr<IRemoteObject> callback) = 0;
    virtual int32_t UnRegisterAttachStateChangeCallback() = 0;
    virtual int32_t GetAttachedDevices(std::set<MechInfo> &mechInfo) = 0;
    virtual int32_t SetUserOperation(const std::shared_ptr<Operation> &operation,
        const std::string &mac, const std::string &param) = 0;

    virtual int32_t SetTrackingEnabled(bool &isEnabled) = 0;
    virtual int32_t GetTrackingEnabled(bool &isEnabled) = 0;
    virtual int32_t RegisterTrackingEventCallback(sptr<IRemoteObject> callback) = 0;
    virtual int32_t UnRegisterTrackingEventCallback() = 0;
    virtual int32_t SetTrackingLayout(CameraTrackingLayout &cameraTrackingLayout) = 0;
    virtual int32_t GetTrackingLayout(CameraTrackingLayout &cameraTrackingLayout) = 0;

    virtual int32_t RegisterCmdChannel(const sptr<IRemoteObject> callback) = 0;
    virtual int32_t RotateByDegree(const int32_t &mechId, std::string &cmdId,
        const std::shared_ptr<RotateByDegreeParam> &rotateByDegreeParam) = 0;
    virtual int32_t RotateToEulerAngles(const int32_t &mechId, std::string &cmdId,
        const std::shared_ptr<RotateToEulerAnglesParam> &rotateToEulerAnglesParam) = 0;
    virtual int32_t GetMaxRotationTime(const int32_t &mechId, std::shared_ptr<TimeLimit> &speedLimit) = 0;
    virtual int32_t GetMaxRotationSpeed(const int32_t &mechId, RotateSpeedLimit &speedLimit) = 0;
    virtual int32_t RotateBySpeed(const int32_t &mechId, std::string &cmdId,
        const std::shared_ptr<RotateBySpeedParam> &rotateBySpeedParam) = 0;
    virtual int32_t StopMoving(const int32_t &mechId, std::string &cmdId) = 0;
    virtual int32_t GetRotationAngles(const int32_t &mechId, std::shared_ptr<EulerAngles> &eulerAngles) = 0;
    virtual int32_t GetRotationDegreeLimits(const int32_t &mechId, RotateDegreeLimit &rotationLimit) = 0;
    virtual int32_t GetRotationAxesStatus(const int32_t &mechId, RotationAxesStatus &axesStatus) = 0;
    virtual int32_t RegisterRotationAxesStatusChangeCallback(const sptr<IRemoteObject> callback) = 0;
    virtual int32_t UnRegisterRotationAxesStatusChangeCallback() = 0;
};
} // namespace MechBodyController
} // namespace OHOS
#endif // OHOS_MECHBODY_CONTROLLER_INTERFACE_H
