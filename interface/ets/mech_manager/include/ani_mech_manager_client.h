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

#ifndef ANI_MECH_CLIENT_H
#define ANI_MECH_CLIENT_H

#include <condition_variable>
#include <atomic>
#include "iremote_broker.h"
#include "ani_mech_manager_stub.h"
#include "mechbody_controller_types.h"
#include "system_ability_load_callback_stub.h"
#include "system_ability_status_change_stub.h"

namespace OHOS {
namespace MechBodyController {

class AniSystemAbilityStatusChangeListener : public SystemAbilityStatusChangeStub {
public:

    void SetAttachStateCallback(const sptr<AniMechManagerStub> &attachStateCallback);

    void SetTrackingEventCallback(const sptr<OHOS::MechBodyController::AniMechManagerStub> &trackingEventCallback);

    void SetCmdChannel(const sptr<AniMechManagerStub> &cmdChannel);

    void SetRotationAxesStatusCallback(const sptr<AniMechManagerStub> &rotationAxesStatusCallback);

    int32_t SendAttachStateChangeListenOn(sptr<AniMechManagerStub> callback);

    int32_t SendTrackingEventListenOn(sptr<AniMechManagerStub> callback);

    int32_t SendRegisterCmdChannel(sptr<AniMechManagerStub> stub);

    int32_t SendRotationAxesStatusChangeListenOn(sptr<AniMechManagerStub> callback);

    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;

    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;

private:
    sptr <IRemoteObject> GetDmsProxy();

public:
    sptr<AniMechManagerStub> attachStateCallback_;
    sptr<AniMechManagerStub> trackingEventCallback_;
    sptr<AniMechManagerStub> cmdChannel_;
    sptr<AniMechManagerStub> rotationAxesStatusCallback_;
};

class AniMechClient : public std::enable_shared_from_this<AniMechClient>  {
public:
    int32_t AttachStateChangeListenOn(sptr<AniMechManagerStub> callback);

    int32_t AttachStateChangeListenOff();

    int32_t GetAttachedDevices(std::vector<std::shared_ptr<MechInfo>> &attachedDevices);

    int32_t SetUserOperation(const Operation &operation, const std::string &mac, const std::string &params = "");

    int32_t SendUserOperation(const Operation &operation, const std::string &mac, const std::string &params);

    int32_t SetCameraTrackingEnabled(const bool &isEnabled);

    int32_t GetCameraTrackingEnabled(bool &isEnabled);

    int32_t TrackingEventListenOn(sptr<AniMechManagerStub> callback);

    int32_t TrackingEventListenOff();

    int32_t SetCameraTrackingLayout(CameraTrackingLayout &cameraTrackingLayout);

    int32_t GetCameraTrackingLayout(CameraTrackingLayout &cameraTrackingLayout);

    int32_t RegisterCmdChannel(sptr<AniMechManagerStub> stub);

    int32_t Rotate(const int32_t &mechId, const std::string &cmdId, const RotateByDegreeParam &rotateByDegreeParam);

    int32_t RotateToEulerAngles(const int32_t &mechId, const std::string &cmdId,
        const RotateToEulerAnglesParam &rotateToEulerAnglesParam);

    int32_t GetMaxRotationTime(const int32_t &mechId, TimeLimit &timeLimit);

    int32_t GetMaxRotationSpeed(const int32_t &mechId, RotateSpeedLimit &rotateSpeedLimit);

    int32_t RotateBySpeed(const int32_t &mechId, const std::string &cmdId, RotateBySpeedParam &rotateBySpeedParam);

    int32_t StopMoving(const int32_t &mechId, const std::string &cmdId);

    int32_t GetRotationAngles(const int32_t &mechId, EulerAngles &currentPosition);

    int32_t GetRotationDegreeLimits(const int32_t &mechId, RotateDegreeLimit &rotationLimit);

    int32_t GetRotationAxesStatus(const int32_t &mechId, RotationAxesStatus &axesStatus);

    int32_t RotationAxesStatusChangeListenOn(sptr<AniMechManagerStub> callback);

    int32_t RotationAxesStatusChangeListenOff();

    int32_t SearchTarget(const std::string &cmdId, const TargetInfo &targetInfo, const SearchParams &searchParams);

private:
    sptr <IRemoteObject> GetDmsProxy();

    int32_t SubscribeMechAbility();

private:
    std::condition_variable producerCon_;
    std::mutex systemAbilityStatusChangeListenerMutex_;
    sptr<AniSystemAbilityStatusChangeListener> systemAbilityStatusChangeListener_;
};

class AniMechBodyServiceLoadCallback : public SystemAbilityLoadCallbackStub {
public:

    AniMechBodyServiceLoadCallback(const std::weak_ptr<AniMechClient> &client,
        Operation operation, const std::string &mac, const std::string &param);

    void OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr <IRemoteObject> &remoteObject) override;

    void OnLoadSystemAbilityFail(int32_t systemAbilityId) override;

private:
    std::weak_ptr<AniMechClient> client;
    Operation operation;
    std::string mac;
    std::string param;
};

} // namespace MechManager
} // namespace OHOS
#endif //MECH_CLIENT_H