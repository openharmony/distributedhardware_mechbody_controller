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

#ifndef OHOS_MECHBODY_CONTROLLER_SERVICE_H
#define OHOS_MECHBODY_CONTROLLER_SERVICE_H

#include "mechbody_controller_stub.h"
#include "system_ability.h"
#include "mc_controller_manager.h"

namespace OHOS {
namespace MechBodyController {

class MechBodyControllerService : public SystemAbility, public MechBodyControllerStub {
    DECLARE_SYSTEM_ABILITY(MechBodyControllerService);
public:
    static MechBodyControllerService& GetInstance();
private:
    MechBodyControllerService(const MechBodyControllerService&) = delete;
    MechBodyControllerService& operator= (const MechBodyControllerService&) = delete;
    MechBodyControllerService(MechBodyControllerService&&) = delete;
    MechBodyControllerService& operator= (MechBodyControllerService&&) = delete;
public:
    MechBodyControllerService();
    ~MechBodyControllerService();

    int32_t OnDeviceConnected(int32_t mechId) override;
    int32_t OnDeviceDisconnected(int32_t mechId) override;

    int32_t RegisterAttachStateChangeCallback(const sptr<IRemoteObject> callback) override;
    int32_t UnRegisterAttachStateChangeCallback() override;
    int32_t OnAttachStateChange(const AttachmentState &attachmentState, const MechInfo &mechInfo);
    int32_t GetAttachedDevices(std::set<MechInfo> &mechInfo) override;
    int32_t SetUserOperation(const std::shared_ptr<Operation> &operation,
        const std::string &mac, const std::string &param) override;

    int32_t SetTrackingEnabled(bool &isEnabled) override;
    int32_t GetTrackingEnabled(bool &isEnabled) override;
    int32_t RegisterTrackingEventCallback(sptr<IRemoteObject> callback) override;
    int32_t UnRegisterTrackingEventCallback() override;
    int32_t SetTrackingLayout(CameraTrackingLayout &cameraTrackingLayout) override;
    int32_t GetTrackingLayout(CameraTrackingLayout &cameraTrackingLayoutb) override;

    int32_t RegisterCmdChannel(const sptr<IRemoteObject> callback) override;
    int32_t RotateByDegree(const int32_t &mechId, std::string &cmdId,
        const std::shared_ptr<RotateByDegreeParam> &rotateByDegreeParam) override;
    int32_t NotifyOperationResult(const uint32_t &tokenId, const std::string &cmdId, const ExecResult &result);
    int32_t RotateToEulerAngles(const int32_t &mechId, std::string &cmdId,
        const std::shared_ptr<RotateToEulerAnglesParam> &rotateToEulerAnglesParam) override;
    int32_t GetMaxRotationTime(const int32_t &mechId, std::shared_ptr<TimeLimit> &speedLimit) override;
    int32_t GetMaxRotationSpeed(const int32_t &mechId, RotateSpeedLimit &speedLimit) override;
    int32_t RotateBySpeed(const int32_t &mechId, std::string &cmdId,
        const std::shared_ptr<RotateBySpeedParam> &rotateBySpeedParam) override;
    int32_t StopMoving(const int32_t &mechId, std::string &cmdId) override;
    int32_t GetRotationAngles(const int32_t &mechId, std::shared_ptr<EulerAngles> &eulerAngles) override;
    int32_t GetRotationDegreeLimits(const int32_t &mechId, RotateDegreeLimit &rotationLimit) override;
    int32_t GetRotationAxesStatus(const int32_t &mechId, RotationAxesStatus &axesStatus) override;
    int32_t RegisterRotationAxesStatusChangeCallback(const sptr<IRemoteObject> callback) override;
    int32_t UnRegisterRotationAxesStatusChangeCallback() override;
    int32_t OnRotationAxesStatusChange(const int32_t &mechId, const RotationAxesStatus &axesStatus);

private:
    bool IsSystemApp();

protected:
    void OnStart() override;
    void OnStop() override;

public:
    std::mutex motionManagersMutex;
    std::map<int32_t, std::shared_ptr<MotionManager>> motionManagers_;
    std::shared_ptr<IMechConnectListener> listener_ = nullptr;
    std::mutex deviceAttachCallbackMutex;
    std::map<uint32_t, sptr<IRemoteObject>> deviceAttachCallback_;
    std::mutex AxesStatusChangeCallbackMutex_;
    std::map<uint32_t, sptr<IRemoteObject>> rotationAxesStatusChangeCallback_;
    std::mutex cmdChannelMutex_;
    std::map<uint32_t, sptr<IRemoteObject>> cmdChannels_;
    std::shared_ptr<TransportSendAdapter> sendAdapter_;
};
} // namespace MechBodyController
} // namespace OHOS
#endif // OHOS_MECHBODY_CONTROLLER_SERVICE_H
