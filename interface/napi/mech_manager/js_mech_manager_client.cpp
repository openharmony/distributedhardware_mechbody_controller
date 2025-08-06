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

#include "iremote_object.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"

#include "mechbody_controller_log.h"
#include "mechbody_controller_ipc_interface_code.h"
#include "js_mech_manager_client.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "MechClient";
    constexpr int32_t MAX_DEVICES = 65535;
    constexpr int32_t DEFAULT_MECH_ID = 0;
}

int32_t MechClient::AttachStateChangeListenOn(sptr<JsMechManagerStub> callback)
{
    sptr <IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        std::lock_guard<std::mutex> lock(systemAbilityStatusChangeListenerMutex_);
        auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgrProxy == nullptr) {
            return MECH_GET_SAMGR_EXCEPTION;
        }
        if(systemAbilityStatusChangeListener_ == nullptr){
            systemAbilityStatusChangeListener_ = new SystemAbilityStatusChangeListener();
            int32_t ret = samgrProxy->SubscribeSystemAbility(MECH_SERVICE_SA_ID, systemAbilityStatusChangeListener_);
            if (ret != ERR_OK) {
                HILOGE("SubscribeSystemAbility failed, ret:%d", ret);
            }
        }
        systemAbilityStatusChangeListener_->SetCallback(callback);
        return ERR_OK;
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

int32_t MechClient::AttachStateChangeListenOff()
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

int32_t MechClient::GetAttachedDevices(std::vector<std::shared_ptr<MechInfo>> &attachedDevices)
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

int32_t MechClient::SetUserOperation(
    const Operation &operation, const std::string &mac, const std::string &params)
{
    HILOGI("start Mech body service.");
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        HILOGE("Mech body service start fail: samgrProxy is nullptr.");
        return MECH_GET_SAMGR_EXCEPTION;
    }
    std::shared_ptr<MechClient> mechClient = shared_from_this();
    sptr <MechBodyServiceLoadCallback> callback(new MechBodyServiceLoadCallback(mechClient, operation, mac, params));
    int32_t executeStartResult = samgrProxy->LoadSystemAbility(MECH_SERVICE_SA_ID, callback);
    if (executeStartResult != ERR_OK) {
        HILOGE("LoadSystemAbility fail, code: %{public}d", executeStartResult);
        return START_MECH_BODY_SERVICE_FAILED;
    }
    {
        std::lock_guard<std::mutex> lock(systemAbilityStatusChangeListenerMutex_);
        if(systemAbilityStatusChangeListener_ == nullptr){
            systemAbilityStatusChangeListener_ = new SystemAbilityStatusChangeListener();
            int32_t ret = samgrProxy->SubscribeSystemAbility(MECH_SERVICE_SA_ID, systemAbilityStatusChangeListener_);
            if (ret != ERR_OK) {
                HILOGE("SubscribeSystemAbility failed, ret:%{public}d", ret);
            }
        }
    }

    HILOGI("Mech body service start success.");
    return ERR_OK;
}

int32_t MechClient::SendUserOperation(const Operation &operation, const std::string &mac, const std::string &params)
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
            static_cast<int32_t>(IMechBodyControllerCode::SET_USER_OPERATION),
            data, reply, option);
    if (error != ERR_NONE) {
        return NAPI_SEND_DATA_FAIL;
    }

    int32_t result = reply.ReadInt32();
    HILOGI("User Operation send result: %{public}d.", result);
    return result;
}

int32_t MechClient::SetCameraTrackingEnabled(const bool &isEnabled)
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

int32_t MechClient::GetCameraTrackingEnabled(bool &isEnabled)
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

int32_t MechClient::TrackingEventListenOn(sptr<JsMechManagerStub> callback)
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

int32_t MechClient::TrackingEventListenOff()
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

int32_t MechClient::SetCameraTrackingLayout(CameraTrackingLayout &cameraTrackingLayout)
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

int32_t MechClient::GetCameraTrackingLayout(CameraTrackingLayout &cameraTrackingLayout)
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

int32_t MechClient::RegisterCmdChannel(sptr<JsMechManagerStub> stub)
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


int32_t MechClient::Rotate(const int32_t &mechId, const std::string &cmdId,
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

int32_t MechClient::RotateToEulerAngles(const int32_t &mechId, const std::string &cmdId,
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

int32_t MechClient::GetMaxRotationTime(const int32_t &mechId, TimeLimit &timeLimit)
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

int32_t MechClient::GetMaxRotationSpeed(const int32_t &mechId, RotateSpeedLimit &rotateSpeedLimit)
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

int32_t MechClient::RotateBySpeed(const int32_t &mechId, const std::string &cmdId,
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

int32_t MechClient::StopMoving(const int32_t &mechId, const std::string &cmdId)
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

int32_t MechClient::GetRotationAngles(const int32_t &mechId, EulerAngles &eulerAngles)
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

int32_t MechClient::GetRotationDegreeLimits(const int32_t &mechId, RotateDegreeLimit &rotationLimit)
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

int32_t MechClient::GetRotationAxesStatus(const int32_t &mechId, RotationAxesStatus &axesStatus)
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

int32_t MechClient::RotationAxesStatusChangeListenOn(sptr<JsMechManagerStub> callback)
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

int32_t MechClient::RotationAxesStatusChangeListenOff()
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

sptr <IRemoteObject> MechClient::GetDmsProxy()
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

void SystemAbilityStatusChangeListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId != MECH_SERVICE_SA_ID) {
        HILOGI("MechService SA:%{public}d added, but not mech body SA", systemAbilityId);
        return;
    }
    HILOGI("MechService SA:%{public}d added", systemAbilityId);
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        HILOGE("get samanager proxy failed;");
        return;
    }

    sptr <IRemoteObject> remote = samgrProxy->CheckSystemAbility(MECH_SERVICE_SA_ID);
    if (remote == nullptr) {
        HILOGE("get mech body proxy failed;");
        return;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        return;
    }
    CHECK_POINTER_RETURN(callback_, "callback_");
    if (!data.WriteRemoteObject(callback_->AsObject())) {
        return;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(
            static_cast<int32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_ON),
            data, reply, option);
    if (error != ERR_NONE) {
        return;
    }
    HILOGI("AttachStateChangeListenOn result: %{public}d", reply.ReadInt32());
}

void SystemAbilityStatusChangeListener::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId != MECH_SERVICE_SA_ID) {
        return;
    }
    std::shared_ptr<MechInfo> mechInfo = std::make_shared<MechInfo>();
    mechInfo->mechId = DEFAULT_MECH_ID;
    JsMechManagerService::GetInstance().AttachStateChangeCallback(AttachmentState::DETACHED, mechInfo);
    HILOGI("MechService SA:%{public}d removed", systemAbilityId);
}

void SystemAbilityStatusChangeListener::SetCallback(const sptr <JsMechManagerStub> &callback) {
    callback_ = callback;
}

void MechBodyServiceLoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
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
    std::shared_ptr<MechClient> mechClient = client.lock();
    if(mechClient == nullptr){
        return;
    }
    mechClient->SendUserOperation(operation, mac, param);
}

void MechBodyServiceLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    HILOGI("OnLoadSystemAbilityFail systemAbilityId: %d.", systemAbilityId);
    if (systemAbilityId != MECH_SERVICE_SA_ID) {
        HILOGE("start systemabilityId is not sourceSAId!");
        return;
    }
    std::shared_ptr<MechClient> mechClient = client.lock();
    if(mechClient == nullptr){
        return;
    }
}

MechBodyServiceLoadCallback::MechBodyServiceLoadCallback(const std::weak_ptr<MechClient> &client,
    Operation operation, const std::string &mac, const std::string &param) : client(client),
    operation(operation), mac(mac), param(param) {}

} // namespace MechManager
} // namespace OHOS