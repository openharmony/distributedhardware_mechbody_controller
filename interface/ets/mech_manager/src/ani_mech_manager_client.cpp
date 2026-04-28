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
#include <fstream>

#include "iremote_object.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"

#include "mechbody_controller_log.h"
#include "mechbody_controller_ipc_interface_code.h"
#include "ani_mech_manager_client.h"
#include  "ani_mech_manager.h"
#include "parameters.h"


namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "AniMechClient";
    constexpr int32_t MAX_DEVICES = 65535;
    constexpr int32_t DEFAULT_MECH_ID = 0;
}

int32_t AniMechClient::AttachStateChangeListenOn(sptr<AniMechManagerStub> callback)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        if (SubscribeMechAbility() != ERR_OK) {
            return MECH_GET_SAMGR_EXCEPTION;
        }
        systemAbilityStatusChangeListener_->SetAttachStateCallback(callback);
        return ERR_OK;
    }
    if (systemAbilityStatusChangeListener_ == nullptr) {
        if (SubscribeMechAbility() != ERR_OK) {
            return MECH_GET_SAMGR_EXCEPTION;
        }
        systemAbilityStatusChangeListener_->SetAttachStateCallback(callback);
    }
    systemAbilityStatusChangeListener_->SendAttachStateChangeListenOn(callback);
    return ERR_OK;
}

int32_t AniMechClient::AttachStateChangeListenOff()
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_OFF),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }
    return reply.ReadInt32();
}

int32_t AniMechClient::GetAttachedDevices(std::vector<std::shared_ptr<MechInfo>> &attachedDevices)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::GET_ATTACHED_DEVICES),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    int32_t result = reply.ReadInt32();
    int32_t size = 0;
    if (result == ERR_OK && reply.ReadInt32(size)) {
        if (size < 0 || size > MAX_DEVICES) {
            return NAPI_RECV_DATA_FAIL;
        }
        attachedDevices.reserve(size);
        for (int32_t i = 0; i < size; ++i) {
            std::shared_ptr<MechInfo> info(reply.ReadParcelable<MechInfo>());
            if (!info) {
                return NAPI_RECV_DATA_FAIL;
            }
            attachedDevices.emplace_back(info);
        }
    }
    return result;
}

int32_t AniMechClient::SetUserOperation(
    const Operation &operation, const std::string &mac, const std::string &params)
{
    HILOGI("start Mech body service.");
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        HILOGE("Mech body service start fail: samgrProxy is nullptr.");
        return MECH_GET_SAMGR_EXCEPTION;
    }
    std::shared_ptr<AniMechClient> mechClient = shared_from_this();
    sptr <AniMechBodyServiceLoadCallback> callback(
        new AniMechBodyServiceLoadCallback(mechClient, operation, mac, params));
    int32_t executeStartResult = samgrProxy->LoadSystemAbility(MECH_SERVICE_SA_ID, callback);
    if (executeStartResult != ERR_OK) {
        HILOGE("LoadSystemAbility fail, code: %{public}d", executeStartResult);
        return START_MECH_BODY_SERVICE_FAILED;
    }
    SubscribeMechAbility();

    HILOGI("Mech body service start success.");
    return ERR_OK;
}

int32_t AniMechClient::SendUserOperation(const Operation &operation, const std::string &mac, const std::string &params)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(static_cast<int32_t>(operation))) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteString(mac)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteString(params)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::SET_USER_OPERATION), data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    int32_t result = reply.ReadInt32();
    HILOGI("User Operation send result: %{public}d.", result);
    return result;
}

int32_t AniMechClient::SetCameraTrackingEnabled(const bool &isEnabled)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteBool(isEnabled)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::SET_CAMERA_TRACKING_ENABLED),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    int32_t result = reply.ReadInt32();
    return result;
}

int32_t AniMechClient::GetCameraTrackingEnabled(bool &isEnabled)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::GET_CAMERA_TRACKING_ENABLED),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    int32_t result = reply.ReadInt32();
    if (result == ERR_OK) {
        isEnabled = reply.ReadBool();
    }
    return result;
}

int32_t AniMechClient::TrackingEventListenOn(sptr<AniMechManagerStub> callback)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr || SubscribeMechAbility() != ERR_OK) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    systemAbilityStatusChangeListener_->SetTrackingEventCallback(callback);
    systemAbilityStatusChangeListener_->SendTrackingEventListenOn(callback);
    return ERR_OK;
}

int32_t AniMechClient::TrackingEventListenOff()
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::TRACKING_EVENT_LISTEN_OFF),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    return reply.ReadInt32();
}

int32_t AniMechClient::SetCameraTrackingLayout(CameraTrackingLayout &cameraTrackingLayout)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(static_cast<int32_t>(cameraTrackingLayout))) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::SET_CAMERA_TRACKING_LAYOUT),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    return reply.ReadInt32();
}

int32_t AniMechClient::GetCameraTrackingLayout(CameraTrackingLayout &cameraTrackingLayout)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::GET_CAMERA_TRACKING_LAYOUT),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    int32_t result = reply.ReadInt32();
    if (result == ERR_OK) {
        cameraTrackingLayout = static_cast<CameraTrackingLayout>(reply.ReadInt32());
    }
    return result;
}

int32_t AniMechClient::RegisterCmdChannel(sptr<AniMechManagerStub> stub)
{
    sptr<IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr || SubscribeMechAbility() != ERR_OK) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    systemAbilityStatusChangeListener_->SetCmdChannel(stub);
    systemAbilityStatusChangeListener_->SendRegisterCmdChannel(stub);
    return ERR_OK;
}


int32_t AniMechClient::Rotate(const int32_t &mechId, const std::string &cmdId,
    const RotateByDegreeParam &rotateByDegreeParam)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(mechId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteString(cmdId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteParcelable(&rotateByDegreeParam)) {
        return NAPI_SEND_DATA_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::ROTATE_BY_DEGREE),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }
    return reply.ReadInt32();
}

int32_t AniMechClient::RotateToEulerAngles(const int32_t &mechId, const std::string &cmdId,
    const RotateToEulerAnglesParam &rotateToEulerAnglesParam)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(mechId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteString(cmdId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteParcelable(&rotateToEulerAnglesParam)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::ROTATE_TO_EULER_ANGLES),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }
    return reply.ReadInt32();
}

int32_t AniMechClient::GetMaxRotationTime(const int32_t &mechId, TimeLimit &timeLimit)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(mechId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::GET_MAX_ROTATION_TIME),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    int32_t result = reply.ReadInt32();
    if (result == ERR_OK) {
        timeLimit.min = reply.ReadFloat();
        timeLimit.max = reply.ReadFloat();
    }
    return result;
}

int32_t AniMechClient::GetMaxRotationSpeed(const int32_t &mechId, RotateSpeedLimit &rotateSpeedLimit)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(mechId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::GET_MAX_ROTATION_SPEED),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    int32_t result = reply.ReadInt32();
    if (result == ERR_OK) {
        std::shared_ptr<RotateSpeedLimit> speedLimit(reply.ReadParcelable<RotateSpeedLimit>());
        if (speedLimit == nullptr) {
            return result;
        }
        rotateSpeedLimit = *speedLimit;
}
    return result;
}

int32_t AniMechClient::RotateBySpeed(const int32_t &mechId, const std::string &cmdId,
    RotateBySpeedParam &rotateBySpeedParam)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(mechId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteString(cmdId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteParcelable(&rotateBySpeedParam)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::ROTATE_BY_SPEED),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    return reply.ReadInt32();
}

int32_t AniMechClient::StopMoving(const int32_t &mechId, const std::string &cmdId)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteString(cmdId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(mechId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::STOP_MOVING),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }
    return reply.ReadInt32();
}

int32_t AniMechClient::GetRotationAngles(const int32_t &mechId, EulerAngles &eulerAngles)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(mechId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::GET_ROTATION_ANGLES),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    int32_t result = reply.ReadInt32();
    if (result == ERR_OK) {
        std::shared_ptr<EulerAngles> currentPositionResult(reply.ReadParcelable<EulerAngles>());
        if (currentPositionResult == nullptr) {
            return result;
        }
        eulerAngles = *currentPositionResult;
    }
    return result;
}

int32_t AniMechClient::GetRotationDegreeLimits(const int32_t &mechId, RotateDegreeLimit &rotationLimit)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(mechId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::GET_ROTATION_DEGREE_LIMITS),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    int32_t result = reply.ReadInt32();
    if (result == ERR_OK) {
        std::shared_ptr<RotateDegreeLimit> rotationLimitResult(reply.ReadParcelable<RotateDegreeLimit>());
        if (rotationLimitResult == nullptr) {
            return result;
        }
        rotationLimit = *rotationLimitResult;
    }
    return result;
}

int32_t AniMechClient::GetRotationAxesStatus(const int32_t &mechId, RotationAxesStatus &axesStatus)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(mechId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::GET_ROTATION_AXES_STATUS),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    int32_t result = reply.ReadInt32();
    if (result == ERR_OK) {
        std::shared_ptr<RotationAxesStatus> axesStatusResult(reply.ReadParcelable<RotationAxesStatus>());
        if (axesStatusResult == nullptr) {
            return result;
        }
        axesStatus = *axesStatusResult;
    }
    return result;
}

int32_t AniMechClient::RotationAxesStatusChangeListenOn(sptr<AniMechManagerStub> callback)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr || SubscribeMechAbility() != ERR_OK) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    systemAbilityStatusChangeListener_->SetRotationAxesStatusCallback(callback);
    systemAbilityStatusChangeListener_->SendRotationAxesStatusChangeListenOn(callback);
    return ERR_OK;
}

int32_t AniMechClient::RotationAxesStatusChangeListenOff()
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::ROTATION_AXES_STATUS_CHANGE_LISTEN_OFF),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    return reply.ReadInt32();
}

int32_t AniMechClient::SearchTarget(const std::string &cmdId, const TargetInfo &targetInfo,
    const SearchParams &searchParams)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteString(cmdId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteParcelable(&targetInfo)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteParcelable(&searchParams)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::SEARCH_TARGET),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }
    return reply.ReadInt32();
}

int32_t AniMechClient::Move(const int32_t &mechId, const std::string &cmdId,
    MoveParams &moveParams)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(mechId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteString(cmdId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteParcelable(&moveParams)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::MOVE),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }
 
    return reply.ReadInt32();
}
 
int32_t AniMechClient::MoveBySpeed(const int32_t &mechId, const std::string &cmdId,
    SpeedParams &speedParams, int32_t duration)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(mechId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteString(cmdId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(duration)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteParcelable(&speedParams)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::MOVE_BY_SPEED),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }
 
    return reply.ReadInt32();
}

 
int32_t AniMechClient::TurnBySpeed(const int32_t &mechId, const std::string &cmdId,
    float angleSpeed, int32_t duration)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(mechId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteString(cmdId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteFloat(angleSpeed)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(duration)) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::TURN_BY_SPEED),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }
 
    return reply.ReadInt32();
}
 
int32_t AniMechClient::IsSupportAction(const int32_t &mechId, ActionType actionType, bool &isSupport)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(mechId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(static_cast<int32_t>(actionType))) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::IS_SUPPORT_ACTION),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }
    int result = reply.ReadInt32();
    if (result == ERR_OK) {
        isSupport = reply.ReadBool();
    }
 
    return result;
}

int32_t AniMechClient::DoAction(const int32_t &mechId, const std::string &cmdId,
    ActionType actionType)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(mechId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteString(cmdId)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(static_cast<int32_t>(actionType))) {
        return NAPI_SEND_DATA_FAIL;
    }
 
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::DO_ACTION),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }
 
    return reply.ReadInt32();
}

int32_t AniMechClient::RegisterSubscribeChannel(sptr<AniMechManagerStub> &stub, MechEventType mechEventType)
{
    sptr<IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr || SubscribeMechAbility() != ERR_OK) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    int32_t res = systemAbilityStatusChangeListener_->SendSubscribeCallbackStub(stub, mechEventType);
    if (res == ERR_OK) {
        systemAbilityStatusChangeListener_->SetSubscribeCallback(stub, mechEventType);
    }
    return res;
}

int32_t AniMechClient::UnRegisterSubscribeChannel(MechEventType mechEventType)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(static_cast<int32_t>(mechEventType))) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::UN_SUBSCRIBE_CALLBACK),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }
 
    int32_t unRegisterResult = reply.ReadInt32();
    if (unRegisterResult == ERR_OK) {
        systemAbilityStatusChangeListener_->RemoveSubscribeCallback(mechEventType);
    }
    return unRegisterResult;
}

sptr <IRemoteObject> AniMechClient::GetDmsProxy()
{
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        return nullptr;
    }

    sptr <IRemoteObject> proxy = samgrProxy->CheckSystemAbility(MECH_SERVICE_SA_ID);
    if (proxy != nullptr) {
        return proxy;
    }

    return nullptr;
}

int32_t AniMechClient::SubscribeMechAbility()
{
    std::lock_guard<std::mutex> lock(systemAbilityStatusChangeListenerMutex_);
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    if (systemAbilityStatusChangeListener_ == nullptr) {
        systemAbilityStatusChangeListener_ = new AniSystemAbilityStatusChangeListener();
        int32_t ret = samgrProxy->SubscribeSystemAbility(MECH_SERVICE_SA_ID, systemAbilityStatusChangeListener_);
        if (ret != ERR_OK) {
            HILOGE("SubscribeSystemAbility failed, ret:%d", ret);
            return MECH_GET_SAMGR_EXCEPTION;
        }
    }
    return ERR_OK;
}

void AniSystemAbilityStatusChangeListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId != MECH_SERVICE_SA_ID) {
        HILOGI("MechService SA:%{public}d added, but not mech body SA", systemAbilityId);
        return;
    }
    HILOGI("MechService SA:%{public}d added", systemAbilityId);
    if (attachStateCallback_ != nullptr) {
        int32_t result = SendAttachStateChangeListenOn(attachStateCallback_);
        HILOGI("SendAttachStateChangeListenOn result: %{public}d", result);
    }

    if (trackingEventCallback_ != nullptr) {
        int32_t result = SendTrackingEventListenOn(trackingEventCallback_);
        HILOGI("SendTrackingEventListenOn result: %{public}d", result);
    }

    if (cmdChannel_ != nullptr) {
        int32_t result = SendRegisterCmdChannel(cmdChannel_);
        HILOGI("SendRegisterCmdChannel result: %{public}d", result);
    }

    if (rotationAxesStatusCallback_ != nullptr) {
        int32_t result = SendRotationAxesStatusChangeListenOn(rotationAxesStatusCallback_);
        HILOGI("SendRotationAxesStatusChangeListenOn result: %{public}d", result);
    }

    std::lock_guard<std::mutex> lock(subscribeCallbackMutex_);
    for (const auto& pair : subscribeCallback_) {
        MechEventType mechEventType = pair.first;
        sptr<AniMechManagerStub> stub = pair.second;
        if (stub != nullptr) {
            int32_t result = SendSubscribeCallbackStub(stub, mechEventType);
            HILOGI("SendSubscribeCallbackStub result: %{public}d, mechEventType: %{public}d",
                result, static_cast<int32_t>(mechEventType));
        }
    }
}

void AniSystemAbilityStatusChangeListener::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId != MECH_SERVICE_SA_ID) {
        return;
    }
    std::shared_ptr<MechInfo> mechInfo = std::make_shared<MechInfo>();
    mechInfo->mechId = DEFAULT_MECH_ID;
    AniMechManager::GetInstance().AttachStateChangeCallback(AttachmentState::DETACHED, mechInfo);
    HILOGI("MechService SA:%{public}d removed", systemAbilityId);
}

void AniSystemAbilityStatusChangeListener::SetAttachStateCallback(const sptr<AniMechManagerStub> &attachStateCallback)
{
    attachStateCallback_ = attachStateCallback;
}

void AniSystemAbilityStatusChangeListener::SetTrackingEventCallback(
    const sptr<OHOS::MechBodyController::AniMechManagerStub> &trackingEventCallback)
{
    trackingEventCallback_ = trackingEventCallback;
}

void AniSystemAbilityStatusChangeListener::SetCmdChannel(const sptr<AniMechManagerStub> &cmdChannel)
{
    cmdChannel_ = cmdChannel;
}

void AniSystemAbilityStatusChangeListener::SetSubscribeCallback(
    const sptr<AniMechManagerStub> &stub, MechEventType mechEventType)
{
    std::lock_guard<std::mutex> lock(subscribeCallbackMutex_);
    subscribeCallback_[mechEventType] = stub;
}

void AniSystemAbilityStatusChangeListener::SetRotationAxesStatusCallback(
    const sptr<AniMechManagerStub> &rotationAxesStatusCallback)
{
    rotationAxesStatusCallback_ = rotationAxesStatusCallback;
}

int32_t AniSystemAbilityStatusChangeListener::SendAttachStateChangeListenOn(sptr<AniMechManagerStub> callback)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    CHECK_POINTER_RETURN_VALUE(callback, INVALID_PARAMETERS_ERR, "callback");
    if (!data.WriteRemoteObject(callback->AsObject())) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_ON),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }
    return reply.ReadInt32();
}

int32_t AniSystemAbilityStatusChangeListener::SendTrackingEventListenOn(sptr<AniMechManagerStub> callback)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    CHECK_POINTER_RETURN_VALUE(callback, INVALID_PARAMETERS_ERR, "callback");
    if (!data.WriteRemoteObject(callback->AsObject())) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::TRACKING_EVENT_LISTEN_ON),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    return reply.ReadInt32();
}

int32_t AniSystemAbilityStatusChangeListener::SendRegisterCmdChannel(sptr<AniMechManagerStub> stub)
{
    sptr<IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    CHECK_POINTER_RETURN_VALUE(stub, INVALID_PARAMETERS_ERR, "stub");
    if (!data.WriteRemoteObject(stub->AsObject())) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::REGISTER_CMD_CHANNEL),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }
    return reply.ReadInt32();
}

int32_t AniSystemAbilityStatusChangeListener::SendRotationAxesStatusChangeListenOn(sptr<AniMechManagerStub> callback)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    CHECK_POINTER_RETURN_VALUE(callback, INVALID_PARAMETERS_ERR, "callback");
    if (!data.WriteRemoteObject(callback->AsObject())) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::ROTATION_AXES_STATUS_CHANGE_LISTEN_ON),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    return reply.ReadInt32();
}

int32_t AniSystemAbilityStatusChangeListener::SendSubscribeCallbackStub(sptr<AniMechManagerStub> &callback,
    MechEventType mechEventType)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        return MECH_GET_SAMGR_EXCEPTION;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return NAPI_SEND_DATA_FAIL;
    }
    if (!data.WriteInt32(static_cast<int32_t>(mechEventType))) {
        return NAPI_SEND_DATA_FAIL;
    }
    // 设置callback的stub信息
    CHECK_POINTER_RETURN_VALUE(callback, INVALID_PARAMETERS_ERR, "callback");
    if (!data.WriteRemoteObject(callback->AsObject())) {
        return NAPI_SEND_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
        static_cast<int32_t>(IMechBodyControllerCode::SUBSCRIBE_CALLBACK),
        data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }
    return reply.ReadInt32();
}

void AniSystemAbilityStatusChangeListener::RemoveSubscribeCallback(MechEventType mechEventType)
{
    std::lock_guard<std::mutex> lock(subscribeCallbackMutex_);
    subscribeCallback_.erase(mechEventType);
}

sptr <IRemoteObject> AniSystemAbilityStatusChangeListener::GetDmsProxy()
{
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        return nullptr;
    }

    sptr <IRemoteObject> proxy = samgrProxy->CheckSystemAbility(MECH_SERVICE_SA_ID);
    if (proxy != nullptr) {
        return proxy;
    }
    return nullptr;
}

void AniMechBodyServiceLoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
    const sptr <IRemoteObject> &remoteObject)
{
    HILOGI("OnLoadSystemAbilitySuccess systemAbilityId: %{public}d, IRmoteObject result: %{public}s",
           systemAbilityId, (remoteObject != nullptr) ? "true" : "false");
    if (systemAbilityId != MECH_SERVICE_SA_ID) {
        HILOGE("start systemabilityId is not sourceSAId!");
        return;
    }
    if (remoteObject == nullptr) {
        HILOGE("remoteObject is null.");
        return;
    }
    std::shared_ptr<AniMechClient> mechClient = client.lock();
    if (mechClient == nullptr) {
        return;
    }
    mechClient->SendUserOperation(operation, mac, param);
}

void AniMechBodyServiceLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    HILOGI("OnLoadSystemAbilityFail systemAbilityId: %d.", systemAbilityId);
    if (systemAbilityId != MECH_SERVICE_SA_ID) {
        HILOGE("start systemabilityId is not sourceSAId!");
        return;
    }
    std::shared_ptr<AniMechClient> mechClient = client.lock();
    if (mechClient == nullptr) {
        return;
    }
}

int32_t AniMechClient::CheckAnyDeviceControlSupported(bool &isControlSupported)
{
    isControlSupported = DetectGimbalSupport();
    return ERR_OK;
}

int32_t AniMechClient::IsControlSupported(MechDeviceType mechDeviceType, bool &isControlSupported)
{
    switch (mechDeviceType) {
        case MechDeviceType::GIMBAL_DEVICE:
            isControlSupported = DetectGimbalSupport();
            break;
        default:
            HILOGE("Invalid device type");
            return MechNapiErrorCode::PARAMETER_CHECK_FAILED;
    }
    return ERR_OK;
}

bool AniMechClient::DetectGimbalSupport()
{
    // verifying the CCM configuration
    std::string getDeviceTypes = OHOS::system::GetParameter("persist.mechbody.unsupported_mechdevicetype", "");
    if (getDeviceTypes.empty()) {
        return true;
    }
    std::string gimbalType = std::to_string(static_cast<int32_t>(MechDeviceType::GIMBAL_DEVICE));
    if (getDeviceTypes.find(gimbalType) != std::string::npos) {
        return false;
    }
    return true;
}

AniMechBodyServiceLoadCallback::AniMechBodyServiceLoadCallback(const std::weak_ptr<AniMechClient> &client,
    Operation operation, const std::string &mac, const std::string &param) : client(client),
    operation(operation), mac(mac), param(param) {}

} // namespace MechManager
} // namespace OHOS