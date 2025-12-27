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

#ifndef ANI_MECH_MANAGER_H
#define ANI_MECH_MANAGER_H

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <ani.h>
#include "ani_mech_manager_client.h"
#include "ani_mech_manager_stub.h"
#include "event_handler.h"
#include "ipc_skeleton.h"
#include "ohos.distributedHardware.mechanicManager.proj.hpp"
#include "ohos.distributedHardware.mechanicManager.impl.hpp"
#include "ohos.distributedHardware.mechanicManager.SearchResult.ani.1.hpp"
#include "stdexcept"
#include "taihe/runtime.hpp"

namespace OHOS {
namespace MechBodyController {
#define ACH_STATE_CHANGE_INFO_CB  void(AttachStateChangeInfoTaihe const& info)
#define TRACKING_EVENT_INFO_CB  void(TrackingEventInfoTaihe const& info)
#define ROT_AXES_STATE_CHANGE_INFO_CB  void(RotationAxesStateChangeInfoTaihe const& info)

using AttachStateChangeInfoTaihe = ::ohos::distributedHardware::mechanicManager::AttachStateChangeInfo;
using TrackingEventInfoTaihe = ::ohos::distributedHardware::mechanicManager::TrackingEventInfo;
using RotationAxesStateChangeInfoTaihe = ::ohos::distributedHardware::mechanicManager::RotationAxesStateChangeInfo;
using AttachStateTaihe = ::ohos::distributedHardware::mechanicManager::AttachState;
using TrackingEventTaihe = ::ohos::distributedHardware::mechanicManager::TrackingEvent;
using MechInfoTaihe = ::ohos::distributedHardware::mechanicManager::MechInfo;
using TargetInfoTaihe = ::ohos::distributedHardware::mechanicManager::TargetInfo;
using SearchParamsTaihe = ::ohos::distributedHardware::mechanicManager::SearchParams;
using RotationSpeedTaihe = ::ohos::distributedHardware::mechanicManager::RotationSpeed;
using ResultTaihe = ::ohos::distributedHardware::mechanicManager::Result;
using CameraTrackingLayoutTaihe = ::ohos::distributedHardware::mechanicManager::CameraTrackingLayout;
using RotationAnglesTaihe = ::ohos::distributedHardware::mechanicManager::RotationAngles;
using EulerAnglesTaihe = ::ohos::distributedHardware::mechanicManager::EulerAngles;
using RotationLimitsTaihe = ::ohos::distributedHardware::mechanicManager::RotationLimits;
using RotationAxesStatusTaihe = ::ohos::distributedHardware::mechanicManager::RotationAxesStatus;
using SearchResultTaihe = ::ohos::distributedHardware::mechanicManager::SearchResult;
using OperationTaihe = ::ohos::distributedHardware::mechanicManager::Operation;
using MechDeviceTypeTaihe = ::ohos::distributedHardware::mechanicManager::MechDeviceType;
using RotationAxisLimitedTaihe = ::ohos::distributedHardware::mechanicManager::RotationAxisLimited;
using TrackingEventTaihe = ::ohos::distributedHardware::mechanicManager::TrackingEvent;
using AttachStateCBTaihe = ::taihe::callback<ACH_STATE_CHANGE_INFO_CB>;
using TrackingEventCBTaihe = ::taihe::callback<TRACKING_EVENT_INFO_CB>;
using RotationAxesCBTaihe = ::taihe::callback<ROT_AXES_STATE_CHANGE_INFO_CB>;

struct AniRotatePrimiseFulfillmentParam {
    std::string cmdId;
    ani_env *env = nullptr;
    ani_resolver deferred = nullptr;
    ani_object promise = nullptr;
    ani_vm *vm = nullptr;
    bool retVoid = false;
};

class AniMechManager {
public:
    static AniMechManager& GetInstance();
private:
    AniMechManager(const AniMechManager&) = delete;
    AniMechManager& operator= (const AniMechManager&) = delete;
    AniMechManager(AniMechManager&&) = delete;
    AniMechManager& operator= (AniMechManager&&) = delete;
    AniMechManager();
public:
    ~AniMechManager();
    int32_t AttachStateChangeCallback(const AttachmentState &attachmentState,
        const std::shared_ptr<MechInfo> &mechInfo);
    int32_t TrackingEventCallback(const int32_t &mechId, const TrackingEvent &trackingEvent);
    int32_t RotationAxesStatusChangeCallback(const int32_t &mechId,
        const std::shared_ptr<RotationAxesStatus> &rotationAxesStatus);
    int32_t RotatePromiseFulfillment(const std::string &cmdId,
        const int32_t &result);
    int32_t SearchTargetCallback(std::string &cmdId, const int32_t &targetsNum, const int32_t &result);

    void OnAttachStateChangeRemoteDied(const wptr <IRemoteObject> &object);
    void OnTrackingEventRemoteDied(const wptr <IRemoteObject> &object);
    void OnRotationAxesStatusChangeRemoteDied(const wptr <IRemoteObject> &object);
    void OnCmdChannelRemoteDied(const wptr <IRemoteObject> &object);

    void OnAttachStateChange(const AttachStateCBTaihe &callback);
    void OffAttachStateChange(const ::taihe::optional_view<AttachStateCBTaihe> &callbck);
    ::taihe::array<MechInfoTaihe> GetAttachedDevices();
    void SetUserOperation(const OperationTaihe &operation, const std::string &mac, const std::string &params);
    void SetCameraTrackingEnabled(bool isEnabled);
    void GetCameraTrackingEnabled(bool &isEnabled);
    void OnTrackingStateChange(const TrackingEventCBTaihe &callback);
    void OffTrackingStateChange(const::taihe::optional_view<TrackingEventCBTaihe> &callback);
    void SetCameraTrackingLayout(const CameraTrackingLayoutTaihe &layout);
    void GetCameraTrackingLayout(CameraTrackingLayoutTaihe &layout);
    void Rotate(int32_t mechId, RotationAnglesTaihe const& angles, int32_t duration, uintptr_t &promise);
    void RotateToEulerAngles(int32_t mechId, EulerAnglesTaihe const& angles, int32_t duration, uintptr_t &promise);
    void GetMaxRotationTime(int32_t mechId, int32_t &maxTime);
    void GetMaxRotationSpeed(int32_t mechId, RotationSpeedTaihe &speed);
    void RotateBySpeed(int32_t mechId, const RotationSpeedTaihe &speed, int32_t duration, uintptr_t &promise);
    void StopMoving(int32_t mechId, uintptr_t &promise);
    void GetCurrentAngles(int32_t mechId, EulerAnglesTaihe &out);
    void GetRotationLimits(int32_t mechId, RotationLimitsTaihe &limits);
    void GetRotationAxesStatus(int32_t mechId, RotationAxesStatusTaihe &out);
    void OnRotationAxesStatusChange(const RotationAxesCBTaihe &callback);
    void OffRotationAxesStatusChange(const ::taihe::optional_view<RotationAxesCBTaihe> &callback);
    void SearchTarget(const TargetInfoTaihe &target, const SearchParamsTaihe &params, uintptr_t &promise);
private:
    int32_t ExecuteOnForAttachStateChange(const AttachStateCBTaihe &callback);
    int32_t ExecuteOffForAttachStateChange(const ::taihe::optional_view<AttachStateCBTaihe> &callbck);
    int32_t ExecuteOnForTrackingEvent(const TrackingEventCBTaihe &callback);
    int32_t ExecuteOffForTrackingEvent(const ::taihe::optional_view<TrackingEventCBTaihe> &callback);
    int32_t ExecuteOnForRotationAxesStatusChange(const RotationAxesCBTaihe &callback);
    int32_t ExecuteOffForRotationAxesStatusChange(const ::taihe::optional_view<RotationAxesCBTaihe> &callback);

    bool InitAttachStateChangeStub();
    bool InitTrackingEventStub();
    bool InitRotationAxesStatusChangeStub();

    void ProcessOnResultCode(int32_t &result);
    void ProcessOffResultCode(int32_t &result);

    bool IsSystemApp();
    bool InitMechClient();
    std::string GenerateUniqueID();
    bool RegisterCmdChannel();

    bool InitRotatePrimiseFulfillmentParam(AniRotatePrimiseFulfillmentParam &param, bool retVoid = false);
    void VoidPromise(ani_env *env, ani_resolver deferred);
    void ResultTaihePromise(ani_env *env, ani_resolver deferred, const int32_t &result);
    void SearchResultTaihePromise(ani_env *env, ani_resolver deferred, const int32_t &targetsNum);
    ani_object CreateInt(ani_env *env, int32_t num);
    ani_enum_item GetResultAni(ani_env *env, int32_t idx);
    void RotateSpeedToAni(const RotateSpeed &speed, RotationSpeedTaihe &out);
    void RotationAxesStatusToAni(const RotationAxesStatus &axesStatus, RotationAxesStatusTaihe &out);
    void RotationLimitsToAni(const RotateDegreeLimit &limit, RotationLimitsTaihe &out);
    bool AniSendEvent(std::function<void()> task);
    int32_t CheckControlL1();
    int32_t CheckDeviceL1();
    bool PreCheck();
private:
    std::mutex attachStateChangeStubMutex_;
    sptr<AniMechManagerStub> attachStateChangeStub_ { nullptr };
    std::mutex trackingEventStubMutex_;
    sptr<AniMechManagerStub> trackingEventStub_ { nullptr };
    std::mutex rotationAxesStatusChangeMutex_;
    sptr<AniMechManagerStub> rotationAxesStatusChangeStub_  { nullptr };;
    std::mutex cmdChannelMutex_;
    sptr<AniMechManagerStub> cmdChannel_  { nullptr };;
    std::mutex mechClientMutex_;
    std::shared_ptr<AniMechClient> mechClient_  { nullptr };

    std::mutex attachStateChangeCallbackMutex_;
    std::vector<AttachStateCBTaihe> attachStateChangeCallback_;
    std::mutex trackingEventCallbackMutex_;
    std::vector<TrackingEventCBTaihe> trackingEventCallback_;
    std::mutex rotateAxisStatusChangeCallbackMutex_;
    std::vector<RotationAxesCBTaihe> rotateAxisStatusChangeCallback_;
    std::mutex promiseParamsMutex_;
    std::map<std::string, std::shared_ptr<AniRotatePrimiseFulfillmentParam>> promiseParams_;
    std::mutex searchTargetPromiseParamMutex_;
    std::map<std::string, std::shared_ptr<AniRotatePrimiseFulfillmentParam>> searchTargetPromiseParam_;
    std::mutex mainHandlerMutex_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> mainHandler_ { nullptr };

    class AniAttachStateChangeStubDeathListener : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;
    };

    class AniTrackingEventStubDeathListener : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;
    };

    class AniRotationAxesStatusChangeStubDeathListener : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;
    };

    class AniCmdChannelDeathListener : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;
    };
};
} // namespace MechManager
} // namespace OHOS

#endif // MECH_MANAGER_NAPI_H