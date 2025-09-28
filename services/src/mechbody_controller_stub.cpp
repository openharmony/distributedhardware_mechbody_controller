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

#include <iremote_stub.h>
#include <memory>

#include "mechbody_controller_stub.h"
#include "mechbody_controller_ipc_interface_code.h"
#include "mechbody_controller_log.h"
#include "mechbody_controller_service.h"

namespace OHOS {
namespace MechBodyController {
namespace {
const std::string TAG = "MechBodyControllerStub";
}

MechBodyControllerStub::MechBodyControllerStub()
{
    InitFuncsInner();
}

MechBodyControllerStub::~MechBodyControllerStub()
{
    funcsMap_.clear();
}

void MechBodyControllerStub::InitFuncsInner()
{
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_ON)] =
            &MechBodyControllerStub::AttachStateChangeListenOnInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_OFF)] =
            &MechBodyControllerStub::AttachStateChangeListenOffInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::GET_ATTACHED_DEVICES)] =
            &MechBodyControllerStub::GetAttachedDevicesInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::SET_USER_OPERATION)] =
            &MechBodyControllerStub::SetUserOperationInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::SET_CAMERA_TRACKING_ENABLED)] =
            &MechBodyControllerStub::SetCameraTrackingEnabledInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::GET_CAMERA_TRACKING_ENABLED)] =
            &MechBodyControllerStub::GetCameraTrackingEnabledInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::TRACKING_EVENT_LISTEN_ON)] =
            &MechBodyControllerStub::TrackingEventListenOnInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::TRACKING_EVENT_LISTEN_OFF)] =
            &MechBodyControllerStub::TrackingEventListenOffInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::SET_CAMERA_TRACKING_LAYOUT)] =
            &MechBodyControllerStub::SetCameraTrackingLayoutInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::GET_CAMERA_TRACKING_LAYOUT)] =
            &MechBodyControllerStub::GetCameraTrackingLayoutInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::REGISTER_CMD_CHANNEL)] =
            &MechBodyControllerStub::RegisterCmdChannelInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::ROTATE_BY_DEGREE)] =
            &MechBodyControllerStub::RotateByDegreeInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::ROTATE_TO_EULER_ANGLES)] =
            &MechBodyControllerStub::RotateToEulerAnglesInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::GET_MAX_ROTATION_TIME)] =
            &MechBodyControllerStub::GetMaxRotationTimeInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::GET_MAX_ROTATION_SPEED)] =
            &MechBodyControllerStub::GetMaxRotationSpeedInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::ROTATE_BY_SPEED)] =
            &MechBodyControllerStub::RotateBySpeedInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::STOP_MOVING)] =
            &MechBodyControllerStub::StopMovingInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::GET_ROTATION_ANGLES)] =
            &MechBodyControllerStub::GetRotationAnglesInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::GET_ROTATION_DEGREE_LIMITS)] =
            &MechBodyControllerStub::GetRotationDegreeLimitsInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::GET_ROTATION_AXES_STATUS)] =
            &MechBodyControllerStub::GetRotationAxesStatusInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::ROTATION_AXES_STATUS_CHANGE_LISTEN_ON)] =
            &MechBodyControllerStub::RotationAxesStatusChangeListenOnInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::ROTATION_AXES_STATUS_CHANGE_LISTEN_OFF)] =
            &MechBodyControllerStub::RotationAxesStatusChangeListenOffInner;
    funcsMap_[static_cast<uint32_t>(IMechBodyControllerCode::SEARCH_TARGET)] =
            &MechBodyControllerStub::SearchTargetInner;
}

int32_t MechBodyControllerStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGI("OnRemoteRequest, code = %{public}u, flags = %{public}d", code, option.GetFlags());
    auto iter = funcsMap_.find(code);
    if (iter != funcsMap_.end()) {
        auto func = iter->second;
        std::u16string interfaceToken = data.ReadInterfaceToken();
        if (interfaceToken != MECH_SERVICE_IPC_TOKEN) {
            HILOGW("OnRemoteRequest interface token check failed!");
            return INVALID_PARAMETERS_ERR;
        }
        if (func != nullptr) {
            return (this->*func)(data, reply);
        }
    }

    HILOGW("OnRemoteRequest default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}


int32_t MechBodyControllerStub::AttachStateChangeListenOnInner(MessageParcel &data, MessageParcel &reply)
{
    sptr <IRemoteObject> callback = data.ReadRemoteObject();

    int32_t result = MechBodyControllerService::GetInstance().RegisterAttachStateChangeCallback(callback);
    bool writeResult = reply.WriteInt32(result);
    HILOGI("writeResult = %{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::AttachStateChangeListenOffInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = MechBodyControllerService::GetInstance().UnRegisterAttachStateChangeCallback();
    bool writeResult = reply.WriteInt32(result);
    HILOGI("%{public}d", result);
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::GetAttachedDevicesInner(MessageParcel &data, MessageParcel &reply)
{
    std::set<MechInfo> mechInfos;
    int32_t result = MechBodyControllerService::GetInstance().GetAttachedDevices(mechInfos);
    bool writeResult = reply.WriteInt32(result);
    if (result == ERR_OK && writeResult) {
        reply.WriteInt32(mechInfos.size());
        for (const auto &item : mechInfos) {
            HILOGE("mech id: %{public}d", item.mechId);
            reply.WriteParcelable(&item);
        }
    }
    return result;
}

int32_t MechBodyControllerStub::SetUserOperationInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<Operation> operation = std::make_shared<Operation>(
            static_cast<Operation>(data.ReadInt32()));
    std::string mac = data.ReadString();
    std::string param = data.ReadString();
    return MechBodyControllerService::GetInstance().SetUserOperation(operation, mac, param);
}

int32_t MechBodyControllerStub::SetCameraTrackingEnabledInner(MessageParcel &data, MessageParcel &reply)
{
    bool isEnable = data.ReadBool();
    int32_t result = MechBodyControllerService::GetInstance().SetTrackingEnabled(isEnable);
    bool writeResult = reply.WriteInt32(result);
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::GetCameraTrackingEnabledInner(MessageParcel &data, MessageParcel &reply)
{
    bool isEnable;
    int32_t result = MechBodyControllerService::GetInstance().GetTrackingEnabled(isEnable);
    bool writeResult = reply.WriteInt32(result);
    if (result == ERR_OK) {
        writeResult = reply.WriteBool(isEnable);
    }
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::TrackingEventListenOnInner(MessageParcel &data, MessageParcel &reply)
{
    sptr <IRemoteObject> callback = data.ReadRemoteObject();
    int32_t result = MechBodyControllerService::GetInstance().RegisterTrackingEventCallback(callback);
    bool writeResult = reply.WriteInt32(result);
    HILOGI("result %{public}d", result);
    HILOGI("writeResult %{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::TrackingEventListenOffInner(MessageParcel &data, MessageParcel &reply)
{
    sptr <IRemoteObject> callback = data.ReadRemoteObject();
    int32_t result = MechBodyControllerService::GetInstance().UnRegisterTrackingEventCallback();
    bool writeResult = reply.WriteInt32(result);
    HILOGI("result %{public}d", result);
    HILOGI("writeResult %{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::SetCameraTrackingLayoutInner(MessageParcel &data, MessageParcel &reply)
{
    auto trackingLayout = static_cast<CameraTrackingLayout>(data.ReadInt32());
    int32_t result = MechBodyControllerService::GetInstance().SetTrackingLayout(trackingLayout);
    bool writeResult = reply.WriteInt32(result);
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::GetCameraTrackingLayoutInner(MessageParcel &data, MessageParcel &reply)
{
    CameraTrackingLayout trackingLayout = CameraTrackingLayout::DEFAULT;
    int32_t result = MechBodyControllerService::GetInstance().GetTrackingLayout(trackingLayout);
    HILOGI("GetMechCameraTrackingLayout : %{public}d", static_cast<int32_t>(trackingLayout));
    bool writeResult = reply.WriteInt32(result);
    if (result == ERR_OK) {
        writeResult = reply.WriteInt32(static_cast<int32_t>(trackingLayout));
    }
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::RegisterCmdChannelInner(MessageParcel &data, MessageParcel &reply)
{
    sptr <IRemoteObject> callback = data.ReadRemoteObject();
    int32_t result = MechBodyControllerService::GetInstance().RegisterCmdChannel(callback);
    bool writeResult = reply.WriteInt32(result);
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::RotateByDegreeInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t mechId = data.ReadInt32();
    std::string cmdId = data.ReadString();
    std::shared_ptr<RotateByDegreeParam> rotateByDegreeParam(data.ReadParcelable<RotateByDegreeParam>());
    sptr <IRemoteObject> callback = data.ReadRemoteObject();
    int32_t result = MechBodyControllerService::GetInstance().RotateByDegree(mechId, cmdId, rotateByDegreeParam);
    bool writeResult = reply.WriteInt32(result);
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::RotateToEulerAnglesInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t mechId = data.ReadInt32();
    std::string cmdId = data.ReadString();
    std::shared_ptr<RotateToEulerAnglesParam> rotateToEulerAnglesParam(
        data.ReadParcelable<RotateToEulerAnglesParam>());
    sptr <IRemoteObject> callback = data.ReadRemoteObject();
    int32_t result = MechBodyControllerService::GetInstance()
        .RotateToEulerAngles(mechId, cmdId, rotateToEulerAnglesParam);
    bool writeResult = reply.WriteInt32(result);
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::GetMaxRotationTimeInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t mechId = data.ReadInt32();
    std::shared_ptr<TimeLimit> timeLimit = std::make_shared<TimeLimit>();
    int32_t result = MechBodyControllerService::GetInstance().GetMaxRotationTime(mechId, timeLimit);
    bool writeResult = reply.WriteInt32(result);
    HILOGI("result: %{public}d; max: %{public}f;", result, timeLimit->max);
    if (result == ERR_OK) {
        writeResult = reply.WriteFloat(timeLimit->min);
        writeResult = reply.WriteFloat(timeLimit->max);
    }
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::GetMaxRotationSpeedInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t mechId = data.ReadInt32();
    RotateSpeedLimit speedLimit;
    int32_t result = MechBodyControllerService::GetInstance().GetMaxRotationSpeed(mechId, speedLimit);
    bool writeResult = reply.WriteInt32(result);
    if (result == ERR_OK) {
        HILOGE("RotateSpeedLimit: %{public}s", speedLimit.ToString().c_str());
        reply.WriteParcelable(&speedLimit);
    }
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::RotateBySpeedInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t mechId = data.ReadInt32();
    std::string cmdId = data.ReadString();
    std::shared_ptr<RotateBySpeedParam> rotateBySpeedParam(data.ReadParcelable<RotateBySpeedParam>());
    sptr <IRemoteObject> callback = data.ReadRemoteObject();
    int32_t result = MechBodyControllerService::GetInstance().RotateBySpeed(mechId, cmdId, rotateBySpeedParam);
    bool writeResult = reply.WriteInt32(result);
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::StopMovingInner(MessageParcel &data, MessageParcel &reply)
{
    std::string cmdId = data.ReadString();
    int32_t mechId = data.ReadInt32();
    sptr <IRemoteObject> callback = data.ReadRemoteObject();
    int32_t result = MechBodyControllerService::GetInstance().StopMoving(mechId, cmdId);
    bool writeResult = reply.WriteInt32(result);
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::GetRotationAnglesInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t mechId = data.ReadInt32();
    std::shared_ptr<EulerAngles> currentPosition = std::make_shared<EulerAngles>();
    int32_t result = MechBodyControllerService::GetInstance().GetRotationAngles(mechId, currentPosition);
    bool writeResult = reply.WriteInt32(result);
    if (result == ERR_OK) {
        HILOGI("current position: %{public}s", currentPosition->ToString().c_str());
        writeResult = reply.WriteParcelable(currentPosition.get());
    }
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::GetRotationDegreeLimitsInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t mechId = data.ReadInt32();
    RotateDegreeLimit rotationLimit;
    int32_t result = MechBodyControllerService::GetInstance().GetRotationDegreeLimits(mechId, rotationLimit);
    bool writeResult = reply.WriteInt32(result);
    if (result == ERR_OK) {
        HILOGI("RotateDegreeLimit: %{public}s", rotationLimit.ToString().c_str());
        writeResult = reply.WriteParcelable(&rotationLimit);
    }
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::GetRotationAxesStatusInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t mechId = data.ReadInt32();
    RotationAxesStatus axesStatus;
    int32_t result = MechBodyControllerService::GetInstance().GetRotationAxesStatus(mechId, axesStatus);
    bool writeResult = reply.WriteInt32(result);
    if (result == ERR_OK) {
        HILOGI("RotationAxesStatus: %{public}s", axesStatus.ToString().c_str());
        writeResult = reply.WriteParcelable(&axesStatus);
    }
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::RotationAxesStatusChangeListenOnInner(MessageParcel &data, MessageParcel &reply)
{
    sptr <IRemoteObject> callback = data.ReadRemoteObject();

    int32_t result = MechBodyControllerService::GetInstance().RegisterRotationAxesStatusChangeCallback(callback);
    bool writeResult = reply.WriteInt32(result);
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::RotationAxesStatusChangeListenOffInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = MechBodyControllerService::GetInstance().UnRegisterRotationAxesStatusChangeCallback();
    bool writeResult = reply.WriteInt32(result);
    HILOGI("%{public}d", writeResult);
    return result;
}

int32_t MechBodyControllerStub::SearchTargetInner(MessageParcel &data, MessageParcel &reply)
{
    std::string cmdId = data.ReadString();
    std::shared_ptr<TargetInfo> targetInfo(data.ReadParcelable<TargetInfo>());
    std::shared_ptr<SearchParams> searchParams(data.ReadParcelable<SearchParams>());
    int32_t result = MechBodyControllerService::GetInstance().SearchTarget(cmdId, targetInfo, searchParams);
    bool writeResult = reply.WriteInt32(result);
    HILOGI("%{public}d", writeResult);
    return result;
}
} // namespace MechBodyController
} // namespace OHOS
