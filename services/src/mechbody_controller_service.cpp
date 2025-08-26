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

#include "mechbody_controller_service.h"

#include "cJSON.h"
#include <string>

#include "accesstoken_kit.h"
#include "ble_send_manager.h"
#include "mechbody_controller_log.h"
#include "mechbody_controller_utils.h"
#include "mc_controller_manager.h"
#include "mc_controller_ipc_death_listener.h"
#include "mechbody_controller_ipc_interface_code.h"
#include "tokenid_kit.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "MechBodyControllerService";
    const std::string PERMISSION_NAME = "ohos.permission.CONNECT_MECHANIC_HARDWARE";
}

MechBodyControllerService& MechBodyControllerService::GetInstance()
{
    static auto instance = new MechBodyControllerService();
    return *instance;
}

const bool REGISTER_RESULT =
        SystemAbility::MakeAndRegisterAbility(&MechBodyControllerService::GetInstance());

MechBodyControllerService::MechBodyControllerService() : SystemAbility(MECH_SERVICE_SA_ID, true)
{
}

MechBodyControllerService::~MechBodyControllerService()
{
    HILOGI("~MechBodyControllerService.");
}

void MechBodyControllerService::OnStart()
{
    // LCOV_EXCL_START
    HILOGI("MechBodyControllerService start.");
    bool res = Publish(this);
    if (!res) {
        HILOGE("MechBodyControllerService start failed.");
    }
    MechConnectManager::GetInstance().Init();

    if (sendAdapter_ == nullptr) {
        sendAdapter_ = std::make_shared<TransportSendAdapter>();
        sendAdapter_->RegisterBluetoothListener();
    }
    BleSendManager::GetInstance().Init();

    HILOGI("MechBodyControllerService start end.");
    // LCOV_EXCL_STOP
}

void MechBodyControllerService::OnStop()
{
    if (sendAdapter_ != nullptr) {
        sendAdapter_->UnRegisterBluetoothListener();
        sendAdapter_ = nullptr;
    }
    BleSendManager::GetInstance().UnInit();
    McCameraTrackingController::GetInstance().UnInit();
    MechConnectManager::GetInstance().UnInit();
}

int32_t MechBodyControllerService::RegisterAttachStateChangeCallback(const sptr<IRemoteObject> callback)
{
    // LCOV_EXCL_START
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    HILOGI("start, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    if (callback == nullptr) {
        HILOGE("callback is nullptr");
        return INVALID_REMOTE_OBJECT;
    }

    sptr <MechControllerIpcDeathListener> diedListener = new MechControllerIpcDeathListener();
    diedListener->tokenId_ = tokenId;
    diedListener->objectType_ = RemoteObjectType::DEVICE_ATTACH_CALLBACK;
    callback->AddDeathRecipient(diedListener);
    {
        std::lock_guard<std::mutex> lock(deviceAttachCallbackMutex);
        deviceAttachCallback_[tokenId] = callback;
        HILOGI("callback add success, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
        if (listener_ == nullptr) {
            listener_ = std::make_shared<MechConnectListenerImpl>();
            int32_t registerListenerResult = MechConnectManager::GetInstance().AddDeviceChangeListener(listener_);
            HILOGI("Register listener to MechConnectManager %{public}s, tokenId: %{public}s;",
                   registerListenerResult == ERR_OK ? "success" : "failed", GetAnonymUint32(tokenId).c_str());
            if (registerListenerResult != ERR_OK) {
                deviceAttachCallback_.erase(tokenId);
                return REGISTER_CALLBACK_FAILED;
            }
        } else {
            HILOGI("DeviceChangeListener has registered");
        }
    }
    HILOGI("end, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    return ERR_OK;
    // LCOV_EXCL_STOP
}

int32_t MechBodyControllerService::UnRegisterAttachStateChangeCallback()
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    HILOGI("start, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    std::lock_guard<std::mutex> lock(deviceAttachCallbackMutex);
    if (deviceAttachCallback_.size() == 1 &&
        deviceAttachCallback_.find(tokenId) != deviceAttachCallback_.end()) {
        int32_t unRegisterResult = MechConnectManager::GetInstance().RemoveDeviceChangeListener(listener_);
        HILOGI("Unregister listener from MechConnectManager %{public}s, tokenId: %{public}s;",
               unRegisterResult == ERR_OK ? "success" : "failed", GetAnonymUint32(tokenId).c_str());
        if (unRegisterResult != ERR_OK) {
            return UNREGISTER_CALLBACK_FAILED;
        }
        listener_ = nullptr;
        deviceAttachCallback_.clear();
        HILOGI("Callback clean success, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    } else {
        deviceAttachCallback_.erase(tokenId);
        HILOGI("Callback remove success, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    }
    HILOGI("end, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    return ERR_OK;
}

int32_t MechBodyControllerService::OnAttachStateChange(const AttachmentState &attachmentState,
                                                       const MechInfo &mechInfo)
{
    HILOGI("start");
    std::lock_guard<std::mutex> lock(deviceAttachCallbackMutex);
    for (const auto &item: deviceAttachCallback_) {
        uint32_t tokenId = item.first;
        HILOGI("start send callback data to tokenId: %{public}s; mech info: %{public}s",
            GetAnonymUint32(tokenId).c_str(),
               mechInfo.ToString().c_str());
        sptr <IRemoteObject> callback = item.second;
        MessageParcel data;
        if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
            HILOGE("Write interface token failed, tokenId: %{public}s, mechId: %{public}d, event: %{public}s",
                GetAnonymUint32(tokenId).c_str(),
                mechInfo.mechId, mechInfo.ToString().c_str());
            continue;
        }

        if (!data.WriteInt32(static_cast<int32_t>(attachmentState))) {
            HILOGE("Write mechId failed, tokenId: %{public}s, mechId: %{public}d, attachmentState: %{public}d",
                GetAnonymUint32(tokenId).c_str(),
                mechInfo.mechId, static_cast<int32_t>(attachmentState));
            return SEND_CALLBACK_INFO_FAILED;
        }

        if (!data.WriteParcelable(&mechInfo)) {
            HILOGE("Write state failed");
            return SEND_CALLBACK_INFO_FAILED;
        }

        if (callback == nullptr) {
            HILOGE("Remote object is null");
            return SEND_CALLBACK_INFO_FAILED;
        }

        MessageParcel reply;
        MessageOption option;
        int32_t error =
            callback->SendRequest(static_cast<uint32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_CALLBACK),
            data, reply, option);
        HILOGI("Send callback data %{public}s, result code: %{public}d",
            error == ERR_NONE ? "success" : "failed", error);
    }
    HILOGI("end");
    return ERR_OK;
}

int32_t MechBodyControllerService::SetUserOperation(const std::shared_ptr<Operation> &operation,
                                                    const std::string &mac, const std::string &param)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    HILOGI("start,tokenId: %{public}s; user operation: %{public}d; mac: %{public}s;",
           GetAnonymUint32(tokenId).c_str(), static_cast<int32_t>(*operation),
           GetAnonymStr(mac).c_str());
    int32_t ret = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, PERMISSION_NAME);
    if (ret != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        HILOGE("%{public}s: PERMISSION_DENIED", PERMISSION_NAME.c_str());
        return PERMISSION_DENIED;
    }
    if (!IsSystemApp()) {
        HILOGE("current app is not system app.");
        return PERMISSION_DENIED;
    }
    cJSON *rootValue = cJSON_Parse(param.c_str());
    if (rootValue == nullptr) {
        HILOGE("Parse param json error.");
        return INVALID_PARAMETERS_ERR;
    }
    cJSON *deviceNameJson = cJSON_GetObjectItemCaseSensitive(rootValue, "device_name");
    if (deviceNameJson == nullptr || !cJSON_IsString(deviceNameJson) || (deviceNameJson->valuestring == nullptr)) {
        cJSON_Delete(rootValue);
        HILOGE("Parse deviceName error.");
        return INVALID_PARAMETERS_ERR;
    }
    std::string deviceName = deviceNameJson->valuestring;
    cJSON_Delete(rootValue);
    BleSendManager::GetInstance().MechbodyConnect(mac, deviceName);
    return ERR_OK;
}

int32_t MechBodyControllerService::OnDeviceConnected(int32_t mechId)
{
    HILOGI("enter. mechId %{public}d", mechId);
    std::lock_guard<std::mutex> lock(motionManagersMutex);
    if (motionManagers_.empty() || motionManagers_.find(mechId) == motionManagers_.end()) {
        HILOGI("create new. mechId %{public}d", mechId);
        if (sendAdapter_ == nullptr) {
            sendAdapter_ = std::make_shared<TransportSendAdapter>();
            sendAdapter_->RegisterBluetoothListener();
        }
        std::shared_ptr<MotionManager> manager = std::make_shared<MotionManager>(sendAdapter_, mechId);
        manager->RegisterEventListener();
        motionManagers_[mechId] = manager;
    }
    HILOGI("end");
    return ERR_OK;
}

int32_t MechBodyControllerService::OnDeviceDisconnected(int32_t mechId)
{
    HILOGI("enter. mechId %{public}d", mechId);
    std::lock_guard<std::mutex> lock(motionManagersMutex);
    if (motionManagers_.empty()) {
        HILOGD("motionManagers_ is empty.");
        return ERR_OK;
    }
    auto it = motionManagers_.find(mechId);
    if (it != motionManagers_.end()) {
        HILOGD("find to erase.");
        it->second->UnRegisterNotifyEvent();
        it->second = nullptr;
        motionManagers_.erase(it);
    }
    return ERR_OK;
}

int32_t MechBodyControllerService::GetAttachedDevices(std::set<MechInfo> &mechInfo)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    HILOGI("start, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    if (!MechConnectManager::GetInstance().GetConnectMechList(mechInfo)) {
        mechInfo.clear();
        HILOGE("Get attached devices failed.");
    }
    HILOGI("end, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    return ERR_OK;
}

int32_t MechBodyControllerService::SetTrackingEnabled(bool &isEnabled)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    HILOGI("start, tokenId: %{public}s; isEnabled: %{public}s;",
        GetAnonymUint32(tokenId).c_str(), isEnabled ? "true" : "false");
    int32_t setResult = McControllerManager::GetInstance().SetTrackingEnabled(tokenId, isEnabled);
    HILOGI("end. Set Tracking Enabled result: %{public}d.", setResult);
    return setResult;
}

int32_t MechBodyControllerService::GetTrackingEnabled(bool &isEnabled)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    HILOGI("start, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    int32_t getResult = McControllerManager::GetInstance().GetTrackingEnabled(tokenId, isEnabled);
    HILOGI("end. Get Tracking Enabled result: %{public}d.", getResult);
    return getResult;
}

int32_t MechBodyControllerService::RegisterTrackingEventCallback(sptr <IRemoteObject> callback)
{
    // LCOV_EXCL_START
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    HILOGI("start, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    if (callback == nullptr) {
        HILOGE("callback is nullptr");
        return INVALID_REMOTE_OBJECT;
    }
    int32_t registerResult = McControllerManager::GetInstance().RegisterTrackingEventCallback(tokenId,
                                                                                              callback);
    HILOGI("end. Register tracking event callback result: %{public}d.", registerResult);
    return registerResult;
    // LCOV_EXCL_STOP
}

int32_t MechBodyControllerService::UnRegisterTrackingEventCallback()
{
    // LCOV_EXCL_START
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    HILOGI("start, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    int32_t unregisterResult = McControllerManager::GetInstance().UnRegisterTrackingEventCallback(tokenId);
    HILOGI("end. Unregister tracking event callback result: %{public}d.", unregisterResult);
    return unregisterResult;
    // LCOV_EXCL_STOP
}

int32_t MechBodyControllerService::SetTrackingLayout(CameraTrackingLayout &cameraTrackingLayout)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    HILOGI("start, tokenId: %{public}s; CameraTrackingLayout: %{public}d;", GetAnonymUint32(tokenId).c_str(),
        static_cast<int32_t>(cameraTrackingLayout));
    if (!IsSystemApp()) {
        HILOGE("current app is not system app.");
        return PERMISSION_DENIED;
    }
    int32_t setTrackingLayoutResult = McControllerManager::GetInstance()
        .SetTrackingLayout(tokenId, cameraTrackingLayout);
    HILOGI("end. Set tracking layout result: %{public}d.", setTrackingLayoutResult);
    return setTrackingLayoutResult;
}

int32_t MechBodyControllerService::GetTrackingLayout(CameraTrackingLayout &cameraTrackingLayout)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    HILOGI("start, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    int32_t getTrackingLayoutResult =
        McControllerManager::GetInstance().GetTrackingLayout(tokenId, cameraTrackingLayout);
    HILOGI("end. Get tracking layout result: %{public}d.", getTrackingLayoutResult);
    return getTrackingLayoutResult;
}

int32_t MechBodyControllerService::RegisterCmdChannel(const sptr<IRemoteObject> callback)
{
    // LCOV_EXCL_START
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    HILOGI("start, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    sptr <MechControllerIpcDeathListener> diedListener = new MechControllerIpcDeathListener();
    diedListener->tokenId_ = tokenId;
    diedListener->objectType_ = RemoteObjectType::COMMAND_CHANNEL;
    CHECK_POINTER_RETURN_VALUE(callback, INVALID_PARAMETERS_ERR, "callback");
    callback->AddDeathRecipient(diedListener);
    {
        std::lock_guard<std::mutex> lock(cmdChannelMutex_);
        cmdChannels_[tokenId] = callback;
    }
    HILOGI("command channel add success, tokenid: %{public}s;", GetAnonymUint32(tokenId).c_str());
    return ERR_OK;
    // LCOV_EXCL_STOP
}

int32_t MechBodyControllerService::RotateByDegree(const int32_t &mechId, std::string &cmdId,
    const std::shared_ptr<RotateByDegreeParam> &rotateByDegreeParam)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    if (!IsSystemApp()) {
        HILOGE("current app is not system app.");
        return PERMISSION_DENIED;
    }
    if (mechId < 0) {
        HILOGE("Invalid mech id");
        return INVALID_MECH_ID;
    }
    if (!rotateByDegreeParam) {
        HILOGE("Invalid rotateByDegreeParam");
        return INVALID_ROTATE_PARAM;
    }
    if (rotateByDegreeParam->duration < 0) {
        HILOGE("Invalid duration");
        return INVALID_ROTATE_PARAM;
    }
    HILOGI("start, tokenId: %{public}s; mechId: %{public}d; rotate param: %{public}s",
        GetAnonymUint32(tokenId).c_str(), mechId, rotateByDegreeParam->ToString().c_str());
    if (rotateByDegreeParam->duration == 0) {
        NotifyOperationResult(tokenId, cmdId, ExecResult::COMPLETED);
        return ERR_OK;
    }
    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = rotateByDegreeParam->degree.yaw;
    rotateParam->degree.roll = rotateByDegreeParam->degree.roll;
    rotateParam->degree.pitch = rotateByDegreeParam->degree.pitch;
    rotateParam->duration = rotateByDegreeParam->duration;
    rotateParam->isRelative = true;
    HILOGI("RotateParam: %{public}s", rotateParam->ToString().c_str());

    std::lock_guard<std::mutex> lock(motionManagersMutex);
    if (motionManagers_.empty()) {
        return DEVICE_NOT_CONNECTED;
    }
    auto it = motionManagers_.find(mechId);
    if (it == motionManagers_.end()) {
        return DEVICE_NOT_CONNECTED;
    }
    std::shared_ptr<MotionManager> motionManager = it->second;
    if (motionManager == nullptr) {
        HILOGE("motionManager not exist. tokenId: %{public}s; mechId: %{public}d",
            GetAnonymUint32(tokenId).c_str(), mechId);
        return DEVICE_NOT_CONNECTED;
    }
    int32_t result = motionManager->Rotate(rotateParam, tokenId, cmdId);
    HILOGI("end. Rotate By Degree execute result: %{public}d.", result);
    return result;
}

int32_t MechBodyControllerService::NotifyOperationResult(const uint32_t &tokenId, const std::string &cmdId,
    const ExecResult &result)
{
    HILOGI("start, result: %{public}d;", result);
    MessageParcel data;
    if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
        HILOGE("Write interface token failed");
        return SEND_CALLBACK_INFO_FAILED;
    }

    if (!data.WriteString(cmdId)) {
        HILOGE("Write cmdId failed");
        return SEND_CALLBACK_INFO_FAILED;
    }
    if (!data.WriteInt32(static_cast<int32_t>(result))) {
        HILOGE("Write result failed");
        return SEND_CALLBACK_INFO_FAILED;
    }
    MessageParcel reply;
    MessageOption option;
    {
        std::lock_guard<std::mutex> lock(cmdChannelMutex_);
        sptr<IRemoteObject> callback =  cmdChannels_[tokenId];
        if (callback == nullptr) {
            HILOGI("command channel is nullptr for tokenId: %{public}s", GetAnonymUint32(tokenId).c_str());
            return NAPI_SEND_DATA_FAIL;
        }
        int32_t error =
            callback->SendRequest(static_cast<uint32_t>(IMechBodyControllerCode::ROTATE_CALLBACK),
            data, reply, option);
        HILOGI("Send callback data %{public}s", error == ERR_NONE ? "success" : "failed");
    }

    return ERR_OK;
}

int32_t MechBodyControllerService::RotateToEulerAngles(const int32_t &mechId, std::string &cmdId,
    const std::shared_ptr<RotateToEulerAnglesParam> &rotateToEulerAnglesParam)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    if (!IsSystemApp()) {
        HILOGE("current app is not system app.");
        return PERMISSION_DENIED;
    }
    if (mechId < 0) {
        HILOGE("Invalid mech id");
        return INVALID_MECH_ID;
    }
    if (!rotateToEulerAnglesParam) {
        HILOGE("Invalid rotateToEulerAnglesParam");
        return INVALID_ROTATE_PARAM;
    }
    if (rotateToEulerAnglesParam->duration < 0) {
        HILOGE("Invalid duration");
        return INVALID_ROTATE_PARAM;
    }
    HILOGI("start, tokenId: %{public}s; mechId: %{public}d; rotate param: %{public}s",
        GetAnonymUint32(tokenId).c_str(), mechId, rotateToEulerAnglesParam->ToString().c_str());
    if (rotateToEulerAnglesParam->duration == 0) {
        NotifyOperationResult(tokenId, cmdId, ExecResult::COMPLETED);
        return ERR_OK;
    }
    std::shared_ptr<RotateParam> rotateParam = std::make_shared<RotateParam>();
    rotateParam->degree.yaw = rotateToEulerAnglesParam->angles.yaw;
    rotateParam->degree.roll = rotateToEulerAnglesParam->angles.roll;
    rotateParam->degree.pitch = rotateToEulerAnglesParam->angles.pitch;
    rotateParam->duration = rotateToEulerAnglesParam->duration;
    rotateParam->isRelative = false;
    HILOGI("RotateParam: %{public}s", rotateParam->ToString().c_str());

    std::lock_guard<std::mutex> lock(motionManagersMutex);
    if (motionManagers_.empty()) {
        return DEVICE_NOT_CONNECTED;
    }
    auto it = motionManagers_.find(mechId);
    if (it == motionManagers_.end()) {
        return DEVICE_NOT_CONNECTED;
    }
    std::shared_ptr<MotionManager> motionManager = it->second;
    if (motionManager == nullptr) {
        HILOGE("motionManager not exist. tokenId: %{public}s; mechId: %{public}d",
            GetAnonymUint32(tokenId).c_str(), mechId);
        return DEVICE_NOT_CONNECTED;
    }
    int32_t result = motionManager->Rotate(rotateParam, tokenId, cmdId);
    HILOGI("end. execute result: %{public}d.", result);
    return result;
}

int32_t MechBodyControllerService::GetMaxRotationTime(const int32_t &mechId,
    std::shared_ptr<TimeLimit> &timeLimit)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    if (!IsSystemApp()) {
        HILOGE("current app is not system app.");
        return PERMISSION_DENIED;
    }
    if (mechId < 0) {
        HILOGE("Invalid mech id");
        return INVALID_MECH_ID;
    }
    HILOGI("start, tokenId: %{public}s; mechId: %{public}d;",  GetAnonymUint32(tokenId).c_str(), mechId);

    std::lock_guard<std::mutex> lock(motionManagersMutex);
    if (motionManagers_.empty()) {
        return DEVICE_NOT_CONNECTED;
    }
    auto it = motionManagers_.find(mechId);
    if (it == motionManagers_.end()) {
        return DEVICE_NOT_CONNECTED;
    }
    std::shared_ptr<MotionManager> motionManager = it->second;
    if (motionManager == nullptr) {
        HILOGE("motionManager not exist. tokenId: %{public}s; mechId: %{public}d",
            GetAnonymUint32(tokenId).c_str(), mechId);
        return DEVICE_NOT_CONNECTED;
    }
    int32_t result = motionManager->GetSpeedControlTimeLimit(timeLimit);
    HILOGI("end. result: %{public}d.", result);
    return result;
}

int32_t MechBodyControllerService::GetMaxRotationSpeed(const int32_t &mechId,
    RotateSpeedLimit &speedLimit)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    if (!IsSystemApp()) {
        HILOGE("current app is not system app.");
        return PERMISSION_DENIED;
    }
    if (mechId < 0) {
        HILOGE("Invalid mech id");
        return INVALID_MECH_ID;
    }
    HILOGI("start, tokenId: %{public}s; mechId: %{public}d;",  GetAnonymUint32(tokenId).c_str(), mechId);

    std::lock_guard<std::mutex> lock(motionManagersMutex);
    if (motionManagers_.empty()) {
        return DEVICE_NOT_CONNECTED;
    }
    auto it = motionManagers_.find(mechId);
    if (it == motionManagers_.end()) {
        return DEVICE_NOT_CONNECTED;
    }
    std::shared_ptr<MotionManager> motionManager = it->second;
    if (motionManager == nullptr) {
        HILOGE("motionManager not exist. tokenId: %{public}s; mechId: %{public}d",
            GetAnonymUint32(tokenId).c_str(), mechId);
        return DEVICE_NOT_CONNECTED;
    }
    int32_t result = motionManager->GetRotateSpeedLimit(speedLimit);
    HILOGI("end. result: %{public}d.", result);
    return result;
}

int32_t MechBodyControllerService::RotateBySpeed(const int32_t &mechId, std::string &cmdId,
    const std::shared_ptr<RotateBySpeedParam> &rotateBySpeedParam)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    if (!IsSystemApp()) {
        HILOGE("current app is not system app.");
        return PERMISSION_DENIED;
    }
    if (mechId < 0) {
        HILOGE("Invalid mech id");
        return INVALID_MECH_ID;
    }
    if (rotateBySpeedParam == nullptr) {
        HILOGE("rotateBySpeedParam is nullptr.");
        return INVALID_ROTATE_PARAM;
    }
    HILOGI("start, tokenId: %{public}s; mechId: %{public}d; rotate speed param: %{public}s",
        GetAnonymUint32(tokenId).c_str(), mechId, rotateBySpeedParam->ToString().c_str());

    if (rotateBySpeedParam->duration == 0) {
        NotifyOperationResult(tokenId, cmdId, ExecResult::COMPLETED);
        return ERR_OK;
    }
    std::lock_guard<std::mutex> lock(motionManagersMutex);
    if (motionManagers_.empty()) {
        return DEVICE_NOT_CONNECTED;
    }
    auto it = motionManagers_.find(mechId);
    if (it == motionManagers_.end()) {
        return DEVICE_NOT_CONNECTED;
    }
    std::shared_ptr<MotionManager> motionManager = it->second;
    if (motionManager == nullptr) {
        HILOGE("motionManager not exist. tokenId: %{public}s; mechId: %{public}d",
            GetAnonymUint32(tokenId).c_str(), mechId);
        return DEVICE_NOT_CONNECTED;
    }
    int32_t result = motionManager->RotateBySpeed(rotateBySpeedParam, tokenId, cmdId);
    HILOGI("end. execute result: %{public}d.", result);
    return result;
}

int32_t MechBodyControllerService::StopMoving(const int32_t &mechId, std::string &cmdId)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    if (!IsSystemApp()) {
        HILOGE("current app is not system app.");
        return PERMISSION_DENIED;
    }
    if (mechId < 0) {
        HILOGE("Invalid mech id");
        return INVALID_MECH_ID;
    }
    HILOGI("start, tokenId: %{public}s; mechId: %{public}d; cmdId: %{public}s",
        GetAnonymUint32(tokenId).c_str(), mechId, cmdId.c_str());

    std::lock_guard<std::mutex> lock(motionManagersMutex);
    if (motionManagers_.empty()) {
        return DEVICE_NOT_CONNECTED;
    }
    auto it = motionManagers_.find(mechId);
    if (it == motionManagers_.end()) {
        return DEVICE_NOT_CONNECTED;
    }
    std::shared_ptr<MotionManager> motionManager = it->second;
    if (motionManager == nullptr) {
        HILOGE("motionManager not exist. tokenId: %{public}s; mechId: %{public}d",
            GetAnonymUint32(tokenId).c_str(), mechId);
        return DEVICE_NOT_CONNECTED;
    }
    int32_t result = motionManager->StopRotate(tokenId, cmdId);
    HILOGI("end. execute result: %{public}d.", result);
    return result;
}

int32_t MechBodyControllerService::GetRotationAngles(const int32_t &mechId,
    std::shared_ptr<EulerAngles> &eulerAngles)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    if (!IsSystemApp()) {
        HILOGE("current app is not system app.");
        return PERMISSION_DENIED;
    }
    if (mechId < 0) {
        HILOGE("Invalid mech id");
        return INVALID_MECH_ID;
    }
    HILOGI("start, tokenId: %{public}s; mechId: %{public}d;", GetAnonymUint32(tokenId).c_str(), mechId);
    std::lock_guard<std::mutex> lock(motionManagersMutex);
    if (motionManagers_.empty()) {
        return DEVICE_NOT_CONNECTED;
    }
    auto it = motionManagers_.find(mechId);
    if (it == motionManagers_.end()) {
        return DEVICE_NOT_CONNECTED;
    }
    std::shared_ptr<MotionManager> motionManager = it->second;
    if (motionManager == nullptr) {
        HILOGE("motionManager not exist. tokenId: %{public}s; mechId: %{public}d",
            GetAnonymUint32(tokenId).c_str(), mechId);
        return DEVICE_NOT_CONNECTED;
    }
    int32_t result = motionManager->GetCurrentPosition(eulerAngles);
    HILOGI("end. query result: %{public}d.", result);
    return result;
}

int32_t MechBodyControllerService::GetRotationDegreeLimits(const int32_t &mechId,
    RotateDegreeLimit &rotationLimit)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    if (!IsSystemApp()) {
        HILOGE("current app is not system app.");
        return PERMISSION_DENIED;
    }
    if (mechId < 0) {
        HILOGE("Invalid mech id");
        return INVALID_MECH_ID;
    }
    HILOGI("start, tokenId: %{public}s; mechId: %{public}d;", GetAnonymUint32(tokenId).c_str(), mechId);
    std::lock_guard<std::mutex> lock(motionManagersMutex);
    if (motionManagers_.empty()) {
        return DEVICE_NOT_CONNECTED;
    }
    auto it = motionManagers_.find(mechId);
    if (it == motionManagers_.end()) {
        return DEVICE_NOT_CONNECTED;
    }
    std::shared_ptr<MotionManager> motionManager = it->second;
    if (motionManager == nullptr) {
        HILOGE("motionManager not exist. tokenId: %{public}s; mechId: %{public}d",
            GetAnonymUint32(tokenId).c_str(), mechId);
        return DEVICE_NOT_CONNECTED;
    }
    int32_t result = motionManager->GetRotationLimit(rotationLimit);
    HILOGI("end. query result: %{public}d.", result);
    return result;
}

int32_t MechBodyControllerService::GetRotationAxesStatus(const int32_t &mechId,
    RotationAxesStatus &axesStatus)
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    if (!IsSystemApp()) {
        HILOGE("current app is not system app.");
        return PERMISSION_DENIED;
    }
    if (mechId < 0) {
        HILOGE("Invalid mech id");
        return INVALID_MECH_ID;
    }
    HILOGI("start, tokenId: %{public}s; mechId: %{public}d;", GetAnonymUint32(tokenId).c_str(), mechId);
    std::lock_guard<std::mutex> lock(motionManagersMutex);
    if (motionManagers_.empty()) {
        return DEVICE_NOT_CONNECTED;
    }
    auto it = motionManagers_.find(mechId);
    if (it == motionManagers_.end()) {
        return DEVICE_NOT_CONNECTED;
    }
    std::shared_ptr<MotionManager> motionManager = it->second;
    if (motionManager == nullptr) {
        HILOGE("motionManager not exist. tokenId: %{public}s; mechId: %{public}d",
            GetAnonymUint32(tokenId).c_str(), mechId);
        return DEVICE_NOT_CONNECTED;
    }
    int32_t result = motionManager->GetRotationAxesStatus(mechId, axesStatus);
    HILOGI("end. query result: %{public}d.", result);
    return result;
}

int32_t MechBodyControllerService::RegisterRotationAxesStatusChangeCallback(const sptr <IRemoteObject> callback)
{
    // LCOV_EXCL_START
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    HILOGI("start tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    if (!IsSystemApp()) {
        HILOGE("current app is not system app.");
        return PERMISSION_DENIED;
    }
    if (callback == nullptr) {
        HILOGE("callback is nullptr");
        return INVALID_REMOTE_OBJECT;
    }

    sptr <MechControllerIpcDeathListener> diedListener = new MechControllerIpcDeathListener();
    diedListener->tokenId_ = tokenId;
    diedListener->objectType_ = RemoteObjectType::ROTATION_AXES_STATUS_CALLBACK;
    callback->AddDeathRecipient(diedListener);
    {
        std::lock_guard<std::mutex> lock(AxesStatusChangeCallbackMutex_);
        rotationAxesStatusChangeCallback_[tokenId] = callback;
    }
    HILOGI("end, callback add success, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    return ERR_OK;
    // LCOV_EXCL_STOP
}

int32_t MechBodyControllerService::UnRegisterRotationAxesStatusChangeCallback()
{
    // LCOV_EXCL_START
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    HILOGI("start, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    if (!IsSystemApp()) {
        HILOGE("current app is not system app.");
        return PERMISSION_DENIED;
    }
    std::lock_guard<std::mutex> lock(AxesStatusChangeCallbackMutex_);
    rotationAxesStatusChangeCallback_.erase(tokenId);
    HILOGI("end, callback remove success, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    return ERR_OK;
    // LCOV_EXCL_STOP
}

int32_t MechBodyControllerService::OnRotationAxesStatusChange(const int32_t &mechId,
                                                              const RotationAxesStatus &axesStatus)
{
    HILOGI("start notify Rotation Axes Status Change, mechId: %{public}d; axesStatus: %{public}s", mechId,
           axesStatus.ToString().c_str());
    std::lock_guard<std::mutex> lock(AxesStatusChangeCallbackMutex_);
    for (const auto &item: rotationAxesStatusChangeCallback_) {
        int32_t tokenId = item.first;
        HILOGI("notify Rotation Axes Status to tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
        sptr <IRemoteObject> callback = item.second;
        MessageParcel data;
        if (!data.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN)) {
            HILOGE("Write interface token failed");
            continue;
        }

        if (!data.WriteInt32(mechId)) {
            HILOGE("Write mechId failed.");
            continue;
        }

        if (!data.WriteParcelable(&axesStatus)) {
            HILOGE("Write axesStatus failed");
            continue;
        }
        MessageParcel reply;
        MessageOption option;
        int32_t error =
            callback->SendRequest(static_cast<uint32_t>(IMechBodyControllerCode::ROTATION_AXES_STATUS_CHANGE_CALLBACK),
            data, reply, option);
        HILOGI("notify tracking event to tokenId: %{public}s; axesStatus: %{public}s", GetAnonymUint32(tokenId).c_str(),
            error == ERR_NONE ? "success" : "failed");
    }
    return ERR_OK;
}

bool MechBodyControllerService::IsSystemApp()
{
    uint64_t accessTokenIDEx = IPCSkeleton::GetCallingFullTokenID();
    return OHOS::Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(accessTokenIDEx);
}

} // namespace MechBodyController
} // namespace OHOS
