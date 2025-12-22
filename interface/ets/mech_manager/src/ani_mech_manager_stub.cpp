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
#include "ani_mech_manager_stub.h"

#include "ani_mech_manager.h"
#include <iremote_stub.h>
#include "ipc_types.h"
#include "iremote_object.h"
#include "mechbody_controller_log.h"
#include "mechbody_controller_types.h"


namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "AniMechManagerStub";
    constexpr int32_t TRACK_MAX = 2;
}

int32_t AniMechManagerStub::OnRemoteRequest(uint32_t code,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    auto funcIter = mechManagerFuncMap_.find(static_cast<IMechBodyControllerCode>(code));
    if (funcIter != mechManagerFuncMap_.end()) {
        MechManagerFunc func = funcIter->second;
        std::u16string token = data.ReadInterfaceToken();
        if (token != MECH_SERVICE_IPC_TOKEN) {
            return IPC_TOKEN_DOES_NOT_MATCH;
        }
        if (func != nullptr) {
            return (this->*func)(data, reply);
        }
    }

    return NO_MATCHING_SERVICE_IMPL;
}

int32_t AniMechManagerStub::AttachStateChangeCallback(MessageParcel &data,
    MessageParcel &reply)
{
    AttachmentState attachmentState = static_cast<AttachmentState>(data.ReadInt32());

    std::shared_ptr<MechInfo> mechInfo(data.ReadParcelable<MechInfo>());
    if (!mechInfo) {
        return NAPI_RECV_DATA_FAIL;
    }
    return AniMechManager::GetInstance().AttachStateChangeCallback(attachmentState, mechInfo);
}

int32_t AniMechManagerStub::TrackingEventCallback(MessageParcel &data,
    MessageParcel &reply)
{
    int32_t mechId = data.ReadInt32();
    int32_t track = data.ReadInt32();
    HILOGE("mech id: %{public}d; trach num: %{public}d", mechId, track);
    if (track > TRACK_MAX || track < 0) {
        return NAPI_RECV_DATA_FAIL;
    }

    TrackingEvent trackingEvent = static_cast<TrackingEvent>(track);
    return AniMechManager::GetInstance().TrackingEventCallback(mechId, trackingEvent);
}

int32_t AniMechManagerStub::RotationAxesStatusChangeCallback(MessageParcel &data,
    MessageParcel &reply)
{
    int32_t mechId = data.ReadInt32();

    std::shared_ptr<RotationAxesStatus> rotationAxesStatus(data.ReadParcelable<RotationAxesStatus>());
    if (!rotationAxesStatus) {
        return NAPI_RECV_DATA_FAIL;
    }
    return AniMechManager::GetInstance().RotationAxesStatusChangeCallback(mechId, rotationAxesStatus);
}

int32_t AniMechManagerStub::RotatePromiseFulfillment(MessageParcel &data,
    MessageParcel &reply)
{
    std::string cmdId = data.ReadString();
    int32_t result = data.ReadInt32();
    return AniMechManager::GetInstance().RotatePromiseFulfillment(cmdId, result);
}

int32_t AniMechManagerStub::SearchTargetCallback(MessageParcel &data,
    MessageParcel &reply)
{
    std::string cmdId = data.ReadString();
    int32_t targetsNum = data.ReadInt32();
    int32_t result = data.ReadInt32();
    return AniMechManager::GetInstance().SearchTargetCallback(cmdId, targetsNum, result);
}

void AniMechManagerStub::InitMechManagerFunc()
{
    mechManagerFuncMap_[IMechBodyControllerCode::ATTACH_STATE_CHANGE_CALLBACK] =
        &AniMechManagerStub::AttachStateChangeCallback;
    mechManagerFuncMap_[IMechBodyControllerCode::TRACKING_EVENT_CALLBACK] = &AniMechManagerStub::TrackingEventCallback;
    mechManagerFuncMap_[IMechBodyControllerCode::ROTATION_AXES_STATUS_CHANGE_CALLBACK] =
        &AniMechManagerStub::RotationAxesStatusChangeCallback;
    mechManagerFuncMap_[IMechBodyControllerCode::ROTATE_CALLBACK] = &AniMechManagerStub::RotatePromiseFulfillment;
    mechManagerFuncMap_[IMechBodyControllerCode::SEARCH_TARGET_CALLBACK] = &AniMechManagerStub::SearchTargetCallback;
}

AniMechManagerStub::AniMechManagerStub()
{
    InitMechManagerFunc();
}

AniMechManagerStub::~AniMechManagerStub()
{
    HILOGE("Destry stub");
    if (deathRecipient_ != nullptr) {
        RemoveDeathRecipient(deathRecipient_);
    }
}

void AniMechManagerStub::SetDeathRecipient(sptr<IRemoteObject::DeathRecipient> deathRecipient)
{
    if (deathRecipient_ != nullptr) {
        RemoveDeathRecipient(deathRecipient_);
    }
    deathRecipient_ = deathRecipient;
    if (deathRecipient_ != nullptr) {
        AddDeathRecipient(deathRecipient_);
    }
}
} // namespace MechManager
} // namespace OHOS