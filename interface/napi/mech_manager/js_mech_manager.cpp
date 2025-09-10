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

#include <unistd.h>

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <random>
#include <ios>
#include <sstream>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "tokenid_kit.h"

#include "mechbody_controller_log.h"
#include "js_mech_manager.h"
#include "js_mech_manager_service.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "MechManager";

    // all event type for on or off function
    const std::string ATTACH_STATE_CHANGE_EVENT = "attachStateChange";
    const std::string TRACKING_EVENT = "trackingStateChange";
    const std::string ROTATE_AXIS_STATUS_CHANGE_EVENT = "rotationAxesStatusChange";
    constexpr int32_t LAYOUT_MAX = 3;
    constexpr int32_t THIRD = 2;
    constexpr int32_t THREE = 3;
}

std::mutex MechManager::attachStateChangeStubMutex_;
sptr<JsMechManagerStub> MechManager::attachStateChangeStub_ = nullptr;
std::mutex MechManager::trackingEventStubMutex_;
sptr<JsMechManagerStub> MechManager::trackingEventStub_ = nullptr;
std::mutex MechManager::rotationAxesStatusChangeMutex_;
sptr<JsMechManagerStub> MechManager::rotationAxesStatusChangeStub_ = nullptr;
std::mutex MechManager::cmdChannelMutex_;
sptr<JsMechManagerStub> MechManager::cmdChannel_ = nullptr;
std::mutex MechManager::mechClientMutex_;
std::shared_ptr<MechClient> MechManager::mechClient_ = std::make_shared<MechClient>();

napi_value MechManager::On(napi_env env, napi_callback_info info)
{
    HILOGI("Start to register the callback function.");
    size_t argc = 2;
    napi_value args[2];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (argc < 2) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Wrong number of arguments");
        HILOGE("Wrong number of arguments.");
        return nullptr;
    }

    napi_valuetype type;
    napi_typeof(env, args[0], &type);
    if (type != napi_string) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "First argument must be a string");
        HILOGE("First argument must be a string.");
        return nullptr;
    }

    size_t eventTypeLength = 0;
    napi_get_value_string_utf8(env, args[0], nullptr, 0, &eventTypeLength);
    std::string eventType(eventTypeLength, '\0');
    if (napi_get_value_string_utf8(env, args[0], &eventType[0], eventTypeLength + 1, nullptr) != napi_ok) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Invalid event type.");
        HILOGE("Invalid event type.");
        return nullptr;
    }

    napi_typeof(env, args[1], &type);
    if (type != napi_function) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Second argument must be a function");
        HILOGE("Second argument must be a function.");
        return nullptr;
    }
    napi_ref callbackRef;
    napi_create_reference(env, args[1], 1, &callbackRef);

    CallbackFunctionInfo callbackFunctionInfo = {env, callbackRef};
    int32_t result = ExecuteOn(eventType, callbackFunctionInfo);
    ProcessOnResultCode(env, result);
    return nullptr;
}

int32_t MechManager::ExecuteOn(std::string &eventType, CallbackFunctionInfo &callbackFunctionInfo)
{
    HILOGI("Register the callback function, event type:%{public}s;", eventType.c_str());
    if (ATTACH_STATE_CHANGE_EVENT == eventType) {
        return ExecuteOnForAttachStateChange(callbackFunctionInfo);
    }

    if (TRACKING_EVENT == eventType) {
        return ExecuteOnForTrackingEvent(callbackFunctionInfo);
    }

    if (ROTATE_AXIS_STATUS_CHANGE_EVENT == eventType) {
        return ExecuteOnForRotationAxesStatusChange(callbackFunctionInfo);
    }
    HILOGE("Register listener, no match event type for %{public}s", eventType.c_str());
    return MechNapiErrorCode::PARAMETER_CHECK_FAILED;
}

int32_t MechManager::ExecuteOnForAttachStateChange(const CallbackFunctionInfo &callbackFunctionInfo)
{
    HILOGE("ATTACH_STATE_CHANGE_EVENT");
    int32_t registerResult = ERR_OK;
    std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().attachStateChangeCallbackMutex_);
    if (JsMechManagerService::GetInstance().attachStateChangeCallback_.empty()) {
        if (!InitMechClient()) {
            HILOGE("Init Mech Client failed.");
            return SYSTEM_WORK_ABNORMALLY;
        }
        if (!InitAttachStateChangeStub()) {
            HILOGE("Init AttachStateChangeStub failed.");
            return SYSTEM_WORK_ABNORMALLY;
        }
        registerResult = mechClient_->AttachStateChangeListenOn(attachStateChangeStub_);
    }
    if (registerResult != ERR_OK) {
        return registerResult;
    }
    JsMechManagerService::GetInstance().attachStateChangeCallback_.insert(callbackFunctionInfo);
    return registerResult;
}

int32_t MechManager::ExecuteOnForTrackingEvent(const CallbackFunctionInfo &callbackFunctionInfo)
{
    HILOGE("TRACKING_EVENT");
    int32_t registerResult = ERR_OK;
    std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().trackingEventCallbackMutex_);
    if (JsMechManagerService::GetInstance().trackingEventCallback.empty()) {
        if (!InitMechClient()) {
            HILOGE("Init Mech Client failed.");
            return MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY;
        }
        if (!InitTrackingEventStub()) {
            HILOGE("Init TrackingEventStu failed.");
            return MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY;
        }
        registerResult = mechClient_->TrackingEventListenOn(trackingEventStub_);
    }
    if (registerResult != ERR_OK) {
        return registerResult;
    }
    JsMechManagerService::GetInstance().trackingEventCallback.insert(callbackFunctionInfo);
    return registerResult;
}

int32_t MechManager::ExecuteOnForRotationAxesStatusChange(const CallbackFunctionInfo &callbackFunctionInfo)
{
    if (!IsSystemApp()) {
        return PERMISSION_DENIED;
    }
    int32_t registerResult = ERR_OK;
    std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().rotateAxisStatusChangeCallbackMutex_);
    if (JsMechManagerService::GetInstance().rotateAxisStatusChangeCallback.empty()) {
        if (!InitMechClient()) {
            HILOGE("Init Mech Client failed.");
            return MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY;
        }
        if (!InitRotationAxesStatusChangeStub()) {
            HILOGE("Init RotationAxesStatusChangeStub failed.");
            return MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY;
        }
        registerResult = mechClient_->RotationAxesStatusChangeListenOn(rotationAxesStatusChangeStub_);
    }
    if (registerResult != ERR_OK) {
        return registerResult;
    }
    JsMechManagerService::GetInstance().rotateAxisStatusChangeCallback.insert(callbackFunctionInfo);
    return registerResult;
}

bool MechManager::InitAttachStateChangeStub() {
    if (attachStateChangeStub_ != nullptr) {
        return true;
    }
    {
        std::lock_guard<std::mutex> lock(attachStateChangeStubMutex_);
        if (attachStateChangeStub_ != nullptr) {
            return true;
        }
        sptr<JsMechManagerStub> stub = new JsMechManagerStub();
        sptr <IRemoteObject::DeathRecipient> deathListener = new AttachStateChangeStubDeathListener();
        stub->SetDeathRecipient(deathListener);
        attachStateChangeStub_ = stub;
        return attachStateChangeStub_ != nullptr;
    }
}

void MechManager::ProcessOnResultCode(napi_env env, int32_t &result) {
    HILOGE("Register the callback function result code: %{public}d ", result);
    if (result == MechNapiErrorCode::PARAMETER_CHECK_FAILED) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Invalid event type.");
        return;
    }
    if (result == MechNapiErrorCode::PERMISSION_DENIED) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PERMISSION_DENIED).c_str(),
            "Not system application");
        return;
    }
    if (result != ERR_OK) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(),
            "System exception");
        return;
    }
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::DEVICE_NOT_CONNECTED).c_str(), "Device not connected");
        return;
    }
}

bool MechManager::InitTrackingEventStub(){
    if(trackingEventStub_ != nullptr){
        return true;
    }
    {
        std::lock_guard<std::mutex> lock(trackingEventStubMutex_);
        if(trackingEventStub_ != nullptr){
            return true;
        }
        sptr<JsMechManagerStub> stub = new JsMechManagerStub();
        sptr<IRemoteObject::DeathRecipient> deathListener = new TrackingEventStubDeathListener();
        stub->SetDeathRecipient(deathListener);
        trackingEventStub_ = stub;
        return trackingEventStub_ != nullptr;
    }
}

bool MechManager::InitRotationAxesStatusChangeStub(){
    if(rotationAxesStatusChangeStub_ != nullptr){
        return true;
    }
    {
        std::lock_guard<std::mutex> lock(rotationAxesStatusChangeMutex_);
        if(rotationAxesStatusChangeStub_ != nullptr){
            return true;
        }
        sptr<JsMechManagerStub> stub = new JsMechManagerStub();
        sptr<IRemoteObject::DeathRecipient> deathListener = new RotationAxesStatusChangeStubDeathListener();
        stub->SetDeathRecipient(deathListener);
        rotationAxesStatusChangeStub_ = stub;
        return rotationAxesStatusChangeStub_ != nullptr;
    }
}

napi_value MechManager::Off(napi_env env, napi_callback_info info)
{
    HILOGI("Start to unregister the callback function.");
    size_t argc = 2;
    napi_value args[2];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (argc < 1) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Wrong number of arguments");
        HILOGE("Wrong number of arguments.");
        return nullptr;
    }
    napi_valuetype type;
    napi_typeof(env, args[0], &type);
    if (type != napi_string) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Argument must be a string");
        HILOGE("Argument must be a string.");
        return nullptr;
    }
    size_t eventTypeLength = 0;
    napi_get_value_string_utf8(env, args[0], nullptr, 0, &eventTypeLength);
    std::string eventType(eventTypeLength, '\0');
    if (napi_get_value_string_utf8(env, args[0], &eventType[0], eventTypeLength + 1, nullptr) != napi_ok) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Invalid event type.");
        HILOGE("Invalid event type.");
        return nullptr;
    }
    int32_t result;
    if (argc == 2) {
        napi_typeof(env, args[1], &type);
        if (type != napi_function) {
            napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
                "Second argument must be a function");
            HILOGE("Second argument must be a function.");
            return nullptr;
        }
        napi_ref callbackRef;
        napi_create_reference(env, args[1], 1, &callbackRef);
        CallbackFunctionInfo callbackFunctionInfo = {env, callbackRef};
        result = ExecuteOff(eventType, callbackFunctionInfo);
    } else {
        result = ExecuteOff(eventType);
    }
    ProcessOffResultCode(env, result);
    return nullptr;
}

int32_t MechManager::ExecuteOff(std::string &eventType)
{
    HILOGE("Unregister listener, event type:%{public}s;", eventType.c_str());
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        return MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY;
    }
    if (ATTACH_STATE_CHANGE_EVENT == eventType) {
        std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().attachStateChangeCallbackMutex_);
        int32_t unRegisterResult = mechClient_->AttachStateChangeListenOff();
        if (unRegisterResult == ERR_OK) {
            JsMechManagerService::GetInstance().attachStateChangeCallback_.clear();
        }
        return unRegisterResult;
    }

    if (TRACKING_EVENT == eventType) {
        std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().trackingEventCallbackMutex_);
        int32_t unRegisterResult = mechClient_->TrackingEventListenOff();
        if (unRegisterResult == ERR_OK) {
            JsMechManagerService::GetInstance().trackingEventCallback.clear();
        }
        return unRegisterResult;
    }

    if (ROTATE_AXIS_STATUS_CHANGE_EVENT == eventType) {
        if (!IsSystemApp()) {
            return PERMISSION_DENIED;
        }
        std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().rotateAxisStatusChangeCallbackMutex_);
        int32_t unRegisterResult = mechClient_->RotationAxesStatusChangeListenOff();
        if (unRegisterResult == ERR_OK) {
            JsMechManagerService::GetInstance().rotateAxisStatusChangeCallback.clear();
        }
        return unRegisterResult;
    }
    HILOGE("Unregister listener, no match event type.");
    return MechNapiErrorCode::PARAMETER_CHECK_FAILED;
}

int32_t MechManager::ExecuteOff(std::string &eventType, CallbackFunctionInfo &callbackFunctionInfo)
{
    HILOGE("Unregister listener, event type:%{public}s;", eventType.c_str());
    if (ATTACH_STATE_CHANGE_EVENT == eventType) {
        return ExecuteOffForAttachStateChange(callbackFunctionInfo);
    }
    if (TRACKING_EVENT == eventType) {
        return ExecuteOffForTrackingEvent(callbackFunctionInfo);
    }
    if (ROTATE_AXIS_STATUS_CHANGE_EVENT == eventType) {
        return ExecuteOffForRotationAxesStatusChange(callbackFunctionInfo);
    }
    HILOGE("Unregister listener, no match event type.");
    return MechNapiErrorCode::PARAMETER_CHECK_FAILED;
}

int32_t MechManager::ExecuteOffForAttachStateChange(CallbackFunctionInfo &callbackFunctionInfo)
{
    std::lock_guard<std::mutex> lock(
            JsMechManagerService::GetInstance().attachStateChangeCallbackMutex_);
    if (JsMechManagerService::GetInstance().attachStateChangeCallback_.size() == 1 &&
        HasSameCallbackFunction(JsMechManagerService::GetInstance().attachStateChangeCallback_,
                                callbackFunctionInfo)) {
        int32_t unRegisterResult = mechClient_->AttachStateChangeListenOff();
        if (unRegisterResult == ERR_OK) {
            JsMechManagerService::GetInstance().attachStateChangeCallback_.clear();
        }
        return unRegisterResult;
    } else {
        RemoveSameCallbackFunction(JsMechManagerService::GetInstance().attachStateChangeCallback_,
                                   callbackFunctionInfo);
        return ERR_OK;
    }
    return MechNapiErrorCode::PARAMETER_CHECK_FAILED;
}

int32_t MechManager::ExecuteOffForTrackingEvent(CallbackFunctionInfo &callbackFunctionInfo)
{
    HILOGE("TRACKING_EVENT");
    std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().trackingEventCallbackMutex_);
    if (JsMechManagerService::GetInstance().trackingEventCallback.size() == 1 &&
        HasSameCallbackFunction(JsMechManagerService::GetInstance().trackingEventCallback,
                                callbackFunctionInfo)) {
        int32_t unRegisterResult = mechClient_->TrackingEventListenOff();
        if (unRegisterResult == ERR_OK) {
            JsMechManagerService::GetInstance().trackingEventCallback.clear();
        }
        return unRegisterResult;
    } else {
        RemoveSameCallbackFunction(JsMechManagerService::GetInstance().trackingEventCallback,
                                   callbackFunctionInfo);
        return ERR_OK;
    }
    return MechNapiErrorCode::PARAMETER_CHECK_FAILED;
}

int32_t MechManager::ExecuteOffForRotationAxesStatusChange(CallbackFunctionInfo &callbackFunctionInfo)
{
    HILOGE("ROTATE_AXIS_STATUS_CHANGE_EVENT");
    if (!IsSystemApp()) {
        return PERMISSION_DENIED;
    }
    std::lock_guard<std::mutex> lock(
            JsMechManagerService::GetInstance().rotateAxisStatusChangeCallbackMutex_);
    if (JsMechManagerService::GetInstance().rotateAxisStatusChangeCallback.size() == 1 &&
        HasSameCallbackFunction(JsMechManagerService::GetInstance().rotateAxisStatusChangeCallback,
                                callbackFunctionInfo)) {
        int32_t unRegisterResult = mechClient_->RotationAxesStatusChangeListenOff();
        if (unRegisterResult == ERR_OK) {
            JsMechManagerService::GetInstance().rotateAxisStatusChangeCallback.clear();
        }
        return unRegisterResult;
    } else {
        RemoveSameCallbackFunction(JsMechManagerService::GetInstance().rotateAxisStatusChangeCallback,
                                   callbackFunctionInfo);
        return ERR_OK;
    }
    return MechNapiErrorCode::PARAMETER_CHECK_FAILED;
}

bool MechManager::HasSameCallbackFunction(std::set<CallbackFunctionInfo> &cacheInfo,
    CallbackFunctionInfo &callbackFunctionInfo)
{
    auto it = cacheInfo.find(callbackFunctionInfo);
    return (it != cacheInfo.end());
}

bool MechManager::RemoveSameCallbackFunction(std::set<CallbackFunctionInfo> &cacheInfo,
    CallbackFunctionInfo &callbackFunctionInfo)
{
    auto it = cacheInfo.find(callbackFunctionInfo);
    if (it != cacheInfo.end()) {
        napi_delete_reference(it->env, it->callbackRef);
        cacheInfo.erase(it);
        HILOGE("Remove ok.");
        return true;
    }
    return false;
}

void MechManager::ProcessOffResultCode(napi_env env, int32_t &result)
{
    HILOGE("UnRegister the callback function result code: %{public}d ", result);
    if (result == MechNapiErrorCode::PARAMETER_CHECK_FAILED) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Invalid event type.");
        return;
    }
    if (result != ERR_OK) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(),
            "System exception");
        return;
    }
}

napi_value MechManager::GetAttachedDevices(napi_env env, napi_callback_info info)
{
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(),
            "System exception");
        return nullptr;
    }
    std::vector<std::shared_ptr<MechInfo>> mechInfos;
    int32_t result = mechClient_->GetAttachedDevices(mechInfos);
    HILOGE("result code: %{public}d ", result);
    if (result != ERR_OK) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(),
            "System exception");
        return nullptr;
    }

    napi_value mechInfosResult;
    napi_create_array_with_length(env, mechInfos.size(), &mechInfosResult);
    for (size_t i = 0; i < mechInfos.size(); i++) {
        napi_value element = MechInfoToNapiObject(env, mechInfos[i]);
        napi_set_element(env, mechInfosResult, i, element);
    }
    return mechInfosResult;
}

napi_value MechManager::MechInfoToNapiObject(napi_env env, const std::shared_ptr<MechInfo> &info)
{
    napi_value obj;
    napi_create_object(env, &obj);

    napi_value mechId;
    napi_create_int32(env, info->mechId, &mechId);
    napi_set_named_property(env, obj, "mechId", mechId);

    napi_value mechType;
    napi_create_int32(env, static_cast<int32_t>(info->mechType), &mechType);
    napi_set_named_property(env, obj, "mechDeviceType", mechType);

    napi_value mechName;
    napi_create_string_utf8(env, info->mechName.c_str(), info->mechName.size(), &mechName);
    napi_set_named_property(env, obj, "mechName", mechName);

    return obj;
}

napi_value MechManager::SetUserOperation(napi_env env, napi_callback_info info)
{
    if (!IsSystemApp()) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PERMISSION_DENIED).c_str(), "Not system application");
        return nullptr;
    }
    size_t argc = 3;
    napi_value args[3];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 3) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Wrong number of arguments");
        HILOGE("Wrong number of arguments.");
        return nullptr;
    }

    int32_t jsOperation;
    napi_get_value_int32(env, args[0], &jsOperation);
    if (jsOperation != 0 && jsOperation != 1) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(), "Operation error.");
        return nullptr;
    }

    auto operation = static_cast<Operation>(jsOperation);

    size_t macLength = 0;
    napi_get_value_string_utf8(env, args[1], nullptr, 0, &macLength);
    std::string mac(macLength, '\0');
    if (napi_get_value_string_utf8(env, args[1], &mac[0], macLength + 1, nullptr) != napi_ok
        || mac.empty() || mac == "") {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(), "Invalid mac.");
        return nullptr;
    }
    size_t paramLength = 0;
    napi_get_value_string_utf8(env, args[2], nullptr, 0, &paramLength);
    std::string param(paramLength, '\0');
    if (napi_get_value_string_utf8(env, args[2], &param[0], paramLength + 1, nullptr) != napi_ok) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(), "Invalid param.");
        HILOGE("Invalid param.");
        return nullptr;
    }
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    int32_t result = mechClient_->SetUserOperation(operation, mac, param);
    HILOGE("result code: %{public}d ", result);
    if (result != ERR_OK) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    return nullptr;
}

napi_value MechManager::SetCameraTrackingEnabled(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 1) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Wrong number of arguments.");
        HILOGE("Wrong number of arguments.");
        return nullptr;
    }

    bool isEnabled;
    napi_status status = napi_get_value_bool(env, args[0], &isEnabled);
    if (status != napi_ok) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Invalid camera tracking status.");
        HILOGE("Invalid camera tracking status.");
        return nullptr;
    }
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    int32_t result = mechClient_->SetCameraTrackingEnabled(isEnabled);
    HILOGI("result code: %{public}d ", result);
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::DEVICE_NOT_CONNECTED).c_str(), "Device not connected");
        return nullptr;
    }
    if (result == MechNapiErrorCode::DEVICE_NOT_SUPPORTED) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::DEVICE_NOT_SUPPORTED).c_str(), "Device not supported");
        return nullptr;
    }
    if (result != ERR_OK) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(),
                         "System exception");
        return nullptr;
    }
    return nullptr;
}

napi_value MechManager::GetCameraTrackingEnabled(napi_env env, napi_callback_info info)
{
    bool isEnabled;
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    int32_t result = mechClient_->GetCameraTrackingEnabled(isEnabled);
    HILOGI("result code: %{public}d ", result);
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::DEVICE_NOT_CONNECTED).c_str(), "Device not connected");
        return nullptr;
    }
    if (result != ERR_OK) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    napi_value isEnabledResult;
    napi_get_boolean(env, isEnabled, &isEnabledResult);
    return isEnabledResult;
}

napi_value MechManager::SetCameraTrackingLayout(napi_env env, napi_callback_info info)
{
    if (!IsSystemApp()) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PERMISSION_DENIED).c_str(), "Not system application");
        return nullptr;
    }
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 1) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Wrong number of arguments");
        return nullptr;
    }

    int32_t jsLayout;
    napi_status status = napi_get_value_int32(env, args[0], &jsLayout);
    if (status != napi_ok) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "trackingLayout must be a number");
        return nullptr;
    }
    if (jsLayout > LAYOUT_MAX || jsLayout < 0) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "trackingLayout out of range ");
        return nullptr;
    }
    auto layout = static_cast<CameraTrackingLayout>(jsLayout);
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    int32_t result = mechClient_->SetCameraTrackingLayout(layout);
    HILOGI("result code: %{public}d ", result);
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::DEVICE_NOT_CONNECTED).c_str(), "Device not connected");
        return nullptr;
    }
    if (result == MechNapiErrorCode::DEVICE_NOT_SUPPORTED) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::DEVICE_NOT_SUPPORTED).c_str(), "Device not supported");
        return nullptr;
    }
    if (result != ERR_OK) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    return nullptr;
}

napi_value MechManager::GetCameraTrackingLayout(napi_env env, napi_callback_info info)
{
    CameraTrackingLayout layout;
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    int32_t result = mechClient_->GetCameraTrackingLayout(layout);
    HILOGI("result code: %{public}d ", result);
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::DEVICE_NOT_CONNECTED).c_str(), "Device not connected");
        return nullptr;
    }
    if (result != ERR_OK) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }

    napi_value getCameraTrackingLayoutResult;
    napi_create_int32(env, static_cast<int32_t>(layout), &getCameraTrackingLayoutResult);
    return getCameraTrackingLayoutResult;
}

napi_value MechManager::Rotate(napi_env env, napi_callback_info info)
{
    HILOGI("start");
    if (!IsSystemApp()) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PERMISSION_DENIED).c_str(), "Not system application");
        return nullptr;
    }
    RotateByDegreeParam rotateParam;
    int32_t mechId;
    if (!GetRotateParam(env, info, rotateParam, mechId)) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "create param failed.");
        return nullptr;
    }
    napi_deferred deferred;
    napi_value promise;
    napi_create_promise(env, &deferred, &promise);
    std::shared_ptr<RotatePrimiseFulfillmentParam> rotatePromiseParam =
        std::make_shared<RotatePrimiseFulfillmentParam>();
    rotatePromiseParam->cmdId = GenerateUniqueID();
    rotatePromiseParam->deferred = deferred;
    rotatePromiseParam->env = env;
    {
        std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().promiseParamsMutex_);
        JsMechManagerService::GetInstance().promiseParams_[rotatePromiseParam->cmdId] = rotatePromiseParam;
    }
    if (!InitMechClient() || !RegisterCmdChannel()) {
        HILOGE("InitMechClient or RegisterCmdChannel failed;");
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    int32_t result = mechClient_->Rotate(mechId, rotatePromiseParam->cmdId, rotateParam);
    HILOGI("result code: %{public}d ", result);
    if (result != ERR_OK) {
        std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().promiseParamsMutex_);
        JsMechManagerService::GetInstance().promiseParams_.erase(rotatePromiseParam->cmdId);
    }
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::DEVICE_NOT_CONNECTED).c_str(), "Device not connected");
        return nullptr;
    }
    if (result != ERR_OK) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    return promise;
}

bool MechManager::RegisterCmdChannel(){
    if(cmdChannel_ != nullptr){
        return true;
    }
    {
        std::lock_guard<std::mutex> lock(cmdChannelMutex_);
        if(cmdChannel_ != nullptr){
            return true;
        }
        if(!InitMechClient()) {
            return false;
        }
        sptr<JsMechManagerStub> stub = new JsMechManagerStub();;
        sptr<IRemoteObject::DeathRecipient> deathListener = new CmdChannelDeathListener();
        stub->SetDeathRecipient(deathListener);
        int32_t result = mechClient_->RegisterCmdChannel(stub);
        if(result == ERR_OK){
            cmdChannel_ = stub;
            return true;
        }
    }
    return false;
}

bool MechManager::GetRotateParam(napi_env env, napi_callback_info info, RotateByDegreeParam &rotateParam,
    int32_t &mechId)
{
    size_t argc = 3;
    napi_value args[3];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 3) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Wrong number of arguments");
        return false;
    }

    napi_status status = napi_get_value_int32(env, args[0], &mechId);
    if (status != napi_ok) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "mechId must be a number");
        return false;
    }

    int32_t duration;
    status = napi_get_value_int32(env, args[2], &duration);
    if (status != napi_ok) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "mechId must be a number");
        return false;
    }

    napi_value degreeObj = args[1];
    rotateParam.duration = duration;

    napi_value yawValue;
    napi_value rollValue;
    napi_value pitchValue;
    double value = 0.0f;
    if (napi_get_named_property(env, degreeObj, "yaw", &yawValue) == napi_ok) {
        napi_get_value_double(env, yawValue, &value);
        rotateParam.degree.yaw = static_cast<float>(value);
    }
    if (napi_get_named_property(env, degreeObj, "roll", &rollValue) == napi_ok) {
        napi_get_value_double(env, rollValue, &value);
        rotateParam.degree.roll = static_cast<float>(value);
    }
    if (napi_get_named_property(env, degreeObj, "pitch", &pitchValue) == napi_ok) {
        napi_get_value_double(env, pitchValue, &value);
        rotateParam.degree.pitch = static_cast<float>(value);
    }
    return true;
}



napi_value MechManager::RotateToEulerAngles(napi_env env, napi_callback_info info)
{
    if (!IsSystemApp()) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PERMISSION_DENIED).c_str(), "Not system application");
        return nullptr;
    }
    RotateToEulerAnglesParam rotateToEulerAnglesParam;
    int32_t mechId;
    if (!GetRotateToEulerAnglesParam(env, info, rotateToEulerAnglesParam, mechId)) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "create param failed.");
        return nullptr;
    }

    napi_deferred deferred;
    napi_value promise;
    napi_create_promise(env, &deferred, &promise);

    std::shared_ptr<RotatePrimiseFulfillmentParam> rotatePromiseParam =
        std::make_shared<RotatePrimiseFulfillmentParam>();
    rotatePromiseParam->cmdId = GenerateUniqueID();
    rotatePromiseParam->deferred = deferred;
    rotatePromiseParam->env = env;
    {
        std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().promiseParamsMutex_);
        JsMechManagerService::GetInstance().promiseParams_[rotatePromiseParam->cmdId] = rotatePromiseParam;
    }

    if(!InitMechClient() || !RegisterCmdChannel()){
        HILOGE("RegisterCmdChannel failed;");
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(),
                         "System exception");
        return nullptr;
    }
    int32_t result = mechClient_->RotateToEulerAngles(mechId, rotatePromiseParam->cmdId, rotateToEulerAnglesParam);
    HILOGI("result code: %{public}d ", result);
    if (result != ERR_OK) {
        std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().promiseParamsMutex_);
        JsMechManagerService::GetInstance().promiseParams_.erase(rotatePromiseParam->cmdId);
    }
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::DEVICE_NOT_CONNECTED).c_str(),
                         "Device not connected");
        return nullptr;
    }
    if (result != ERR_OK) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(),
                         "System exception");
        return nullptr;
    }
    return promise;
}

bool MechManager::GetRotateToEulerAnglesParam(napi_env env, napi_callback_info info,
    RotateToEulerAnglesParam &rotateToEulerAnglesParam, int32_t &mechId)
{
    size_t argc = 3;
    napi_value args[3];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 3) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Wrong number of arguments");
        return false;
    }

    napi_status status = napi_get_value_int32(env, args[0], &mechId);
    if (status != napi_ok) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "mechId must be a number");
        return false;
    }

    int32_t duration;
    status = napi_get_value_int32(env, args[2], &duration);
    if (status != napi_ok) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "mechId must be a number");
        return false;
    }

    napi_value degreeObj = args[1];
    rotateToEulerAnglesParam.duration = duration;

    napi_value yawValue;
    napi_value rollValue;
    napi_value pitchValue;
    double value = 0.0f;
    if (napi_get_named_property(env, degreeObj, "yaw", &yawValue) == napi_ok) {
        napi_get_value_double(env, yawValue, &value);
        rotateToEulerAnglesParam.angles.yaw = static_cast<float>(value);
    }
    if (napi_get_named_property(env, degreeObj, "roll", &rollValue) == napi_ok) {
        napi_get_value_double(env, rollValue, &value);
        rotateToEulerAnglesParam.angles.roll = static_cast<float>(value);
    }
    if (napi_get_named_property(env, degreeObj, "pitch", &pitchValue) == napi_ok) {
        napi_get_value_double(env, pitchValue, &value);
        rotateToEulerAnglesParam.angles.pitch = static_cast<float>(value);
    }
    return true;
}

napi_value MechManager::GetMaxRotationTime(napi_env env, napi_callback_info info)
{
    if (!IsSystemApp()) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PERMISSION_DENIED).c_str(), "Not system application");
        return nullptr;
    }
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 1) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Wrong number of arguments");
        return nullptr;
    }

    int32_t mechId;
    napi_status status = napi_get_value_int32(env, args[0], &mechId);
    if (status != napi_ok) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "mechId must be a number");
        return nullptr;
    }

    TimeLimit timeLimit;
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(),
            "System exception");
        return nullptr;
    }
    int32_t result = mechClient_->GetMaxRotationTime(mechId, timeLimit);
    HILOGE("result: %{public}d;", result);
    if (result != ERR_OK) {
        HILOGE("time limit query failed, result: %{public}d;", result);
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    HILOGE("time limit query success, min: %{public}f; max: %{public}f;", timeLimit.min, timeLimit.max);
    napi_value getSpeedControlMaxTimeResult;
    napi_create_double(env, timeLimit.max, &getSpeedControlMaxTimeResult);
    return getSpeedControlMaxTimeResult;
}

napi_value MechManager::GetMaxRotationSpeed(napi_env env, napi_callback_info info)
{
    if (!IsSystemApp()) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PERMISSION_DENIED).c_str(), "Not system application");
        return nullptr;
    }
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 1) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Wrong number of arguments");
        return nullptr;
    }

    int32_t mechId;
    napi_status status = napi_get_value_int32(env, args[0], &mechId);
    if (status != napi_ok) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "mechId must be a number");
        return nullptr;
    }

    RotateSpeedLimit rotateSpeedLimit;
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    int32_t result = mechClient_->GetMaxRotationSpeed(mechId, rotateSpeedLimit);
    HILOGE("result: %{public}d;", result);
    if (result != ERR_OK) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }

    napi_value getRotateSpeedLimitResult;
    napi_create_object(env, &getRotateSpeedLimitResult);
    napi_value speedMax = RotateSpeedToNapi(env, rotateSpeedLimit.speedMax);
    napi_set_named_property(env, getRotateSpeedLimitResult, "speedMax", speedMax);
    napi_value speedMin = RotateSpeedToNapi(env, rotateSpeedLimit.speedMin);
    napi_set_named_property(env, getRotateSpeedLimitResult, "speedMin", speedMin);

    return getRotateSpeedLimitResult;
}

napi_value MechManager::RotateSpeedToNapi(napi_env env, const RotateSpeed &speed)
{
    napi_value obj;
    napi_create_object(env, &obj);

    napi_value yawSpeed;
    napi_create_double(env, speed.yawSpeed, &yawSpeed);
    napi_set_named_property(env, obj, "yawSpeed", yawSpeed);

    napi_value rollSpeed;
    napi_create_double(env, speed.rollSpeed, &rollSpeed);
    napi_set_named_property(env, obj, "rollSpeed", rollSpeed);

    napi_value pitchSpeed;
    napi_create_double(env, speed.pitchSpeed, &pitchSpeed);
    napi_set_named_property(env, obj, "pitchSpeed", pitchSpeed);

    return obj;
}

napi_value MechManager::RotateBySpeed(napi_env env, napi_callback_info info)
{
    if (!IsSystemApp()) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PERMISSION_DENIED).c_str(), "Not system application");
        return nullptr;
    }
    RotateBySpeedParam rotateBySpeedParam;
    int32_t mechId;
    if(!GetRotateBySpeedParam(env, info, rotateBySpeedParam, mechId)){
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "create param failed.");
        return nullptr;
    }

    napi_deferred deferred;
    napi_value promise;
    napi_create_promise(env, &deferred, &promise);

    std::shared_ptr<RotatePrimiseFulfillmentParam> rotatePromiseParam =
        std::make_shared<RotatePrimiseFulfillmentParam>();
    rotatePromiseParam->cmdId = GenerateUniqueID();
    rotatePromiseParam->deferred = deferred;
    rotatePromiseParam->env = env;
    {
        std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().promiseParamsMutex_);
        JsMechManagerService::GetInstance().promiseParams_[rotatePromiseParam->cmdId] = rotatePromiseParam;
    }

    if (!InitMechClient() || !RegisterCmdChannel()) {
        HILOGE("RegisterCmdChannel failed;");
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    int32_t result = mechClient_->RotateBySpeed(mechId, rotatePromiseParam->cmdId, rotateBySpeedParam);
    HILOGI("result code: %{public}d ", result);
    if (result != ERR_OK) {
        std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().promiseParamsMutex_);
        JsMechManagerService::GetInstance().promiseParams_.erase(rotatePromiseParam->cmdId);
    }
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::DEVICE_NOT_CONNECTED).c_str(), "Device not connected");
        return nullptr;
    }
    if (result != ERR_OK) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    return promise;
}

bool MechManager::GetRotateBySpeedParam(napi_env env, napi_callback_info info,
    RotateBySpeedParam &rotateBySpeedParam, int32_t &mechId)
{
    size_t argc = 3;
    napi_value args[3];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 3) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Wrong number of arguments");
        return false;
    }

    napi_status status = napi_get_value_int32(env, args[0], &mechId);
    if (status != napi_ok) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "mechId must be a number");
        return false;
    }

    double duration;
    status = napi_get_value_double(env, args[2], &duration);
    if (status != napi_ok) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "mechId must be a number");
        return false;
    }

    napi_value degreeObj = args[1];
    rotateBySpeedParam.duration = static_cast<float>(duration);

    napi_value yawValue;
    napi_value rollValue;
    napi_value pitchValue;
    double value = 0.0f;
    if (napi_get_named_property(env, degreeObj, "yawSpeed", &yawValue) == napi_ok) {
        napi_get_value_double(env, yawValue, &value);
        rotateBySpeedParam.speed.yawSpeed = static_cast<float>(value);
    }
    if (napi_get_named_property(env, degreeObj, "rollSpeed", &rollValue) == napi_ok) {
        napi_get_value_double(env, rollValue, &value);
        rotateBySpeedParam.speed.rollSpeed = static_cast<float>(value);
    }
    if (napi_get_named_property(env, degreeObj, "pitchSpeed", &pitchValue) == napi_ok) {
        napi_get_value_double(env, pitchValue, &value);
        rotateBySpeedParam.speed.pitchSpeed = static_cast<float>(value);
    }
    return true;
}

napi_value MechManager::StopMoving(napi_env env, napi_callback_info info)
{
    if (!IsSystemApp()) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PERMISSION_DENIED).c_str(), "Not system application");
        return nullptr;
    }
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (argc < 1) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Wrong number of arguments");
        return nullptr;
    }
    int32_t mechId;
    napi_status status = napi_get_value_int32(env, args[0], &mechId);
    if (status != napi_ok) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "mechId must be a number");
        return nullptr;
    }
    napi_deferred deferred;
    napi_value promise;
    napi_create_promise(env, &deferred, &promise);
    std::shared_ptr<RotatePrimiseFulfillmentParam> rotatePromiseParam =
        std::make_shared<RotatePrimiseFulfillmentParam>();
    rotatePromiseParam->cmdId = GenerateUniqueID();
    rotatePromiseParam->deferred = deferred;
    rotatePromiseParam->env = env;
    {
        std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().promiseParamsMutex_);
        JsMechManagerService::GetInstance().promiseParams_[rotatePromiseParam->cmdId] = rotatePromiseParam;
    }

    if (!InitMechClient() || !RegisterCmdChannel()) {
        HILOGE("RegisterCmdChannel failed;");
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    int32_t result = mechClient_->StopMoving(mechId, rotatePromiseParam->cmdId);
    HILOGI("result code: %{public}d ", result);
    if (result != ERR_OK) {
        std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().promiseParamsMutex_);
        JsMechManagerService::GetInstance().promiseParams_.erase(rotatePromiseParam->cmdId);
    }
    ProcessOnResultCode(env, result);
    return promise;
}

napi_value MechManager::GetCurrentAngles(napi_env env, napi_callback_info info)
{
    if (!IsSystemApp()) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PERMISSION_DENIED).c_str(), "Not system application");
        return nullptr;
    }
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 1) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Wrong number of arguments");
        return nullptr;
    }

    int32_t mechId;
    napi_status status = napi_get_value_int32(env, args[0], &mechId);
    if (status != napi_ok) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "mechId must be a number");
        return nullptr;
    }

    EulerAngles rotationAngles;
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    int32_t result = mechClient_->GetRotationAngles(mechId, rotationAngles);
    HILOGE("result: %{public}d;", result);
    if (result != ERR_OK) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    return CreateEulerAngles(env, rotationAngles);
}

napi_value MechManager::GetRotationLimits(napi_env env, napi_callback_info info)
{
    if (!IsSystemApp()) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PERMISSION_DENIED).c_str(), "Not system application");
        return nullptr;
    }
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 1) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Wrong number of arguments");
        return nullptr;
    }

    int32_t mechId;
    napi_status status = napi_get_value_int32(env, args[0], &mechId);
    if (status != napi_ok) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "mechId must be a number");
        return nullptr;
    }

    RotateDegreeLimit rotateDegreeLimit;
    if(!InitMechClient()){
        HILOGE("Init Mech Client failed.");
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(),
            "System exception");
        return nullptr;
    }
    int32_t result = mechClient_->GetRotationDegreeLimits(mechId, rotateDegreeLimit);
    HILOGE("result: %{public}d;", result);
    if (result != ERR_OK) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    HILOGE("RotateDegreeLimit query success: %{public}s;", rotateDegreeLimit.ToString().c_str());
    return CreateRotationLimit(env, rotateDegreeLimit);
}

napi_value MechManager::GetRotationAxesStatus(napi_env env, napi_callback_info info)
{
    if (!IsSystemApp()) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PERMISSION_DENIED).c_str(), "Not system application");
        return nullptr;
    }
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc < 1) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "Wrong number of arguments");
        return nullptr;
    }

    int32_t mechId;
    napi_status status = napi_get_value_int32(env, args[0], &mechId);
    if (status != napi_ok) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "mechId must be a number");
        return nullptr;
    }

    RotationAxesStatus axesStatus{};
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    int32_t result = mechClient_->GetRotationAxesStatus(mechId, axesStatus);
    HILOGE("result: %{public}d;", result);
    if (result != ERR_OK) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    return CreateRotationAxesStatus(env, axesStatus);
}

napi_value MechManager::SearchTarget(napi_env env, napi_callback_info info)
{
    if (!IsSystemApp()) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PERMISSION_DENIED).c_str(), "Not system application");
        return nullptr;
    }
    CallbackFunctionInfo callbackFunctionInfo;
    TargetInfo targetInfo;
    SearchParams searchParams;
    if (!GetSearchTargetParam(env, info, callbackFunctionInfo, targetInfo, searchParams)) {
        napi_throw_type_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
            "create param failed.");
        return nullptr;
    }
    std::string cmdId = GenerateUniqueID();
    {
        std::lock_guard<std::mutex> lock(JsMechManagerService::GetInstance().searchTargetCallbackMutex_);
        JsMechManagerService::GetInstance().searchTargetCallback_.insert(std::make_pair(cmdId, callbackFunctionInfo));
    }

    if (!InitMechClient() || !RegisterCmdChannel()) {
        HILOGE("InitMechClient or RegisterCmdChannel failed;");
        JsMechManagerService::GetInstance().searchTargetCallback_.erase(cmdId);
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }

    int32_t result = mechClient_->SearchTarget(cmdId, targetInfo, searchParams);
    HILOGI("result: %{public}d;", result);
    if (result != ERR_OK) {
        JsMechManagerService::GetInstance().searchTargetCallback_.erase(cmdId);
        napi_throw_error(env, std::to_string(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY).c_str(), "System exception");
        return nullptr;
    }
    return nullptr;
}

bool MechManager::GetSearchTargetParam(napi_env env, napi_callback_info info,
    CallbackFunctionInfo &callbackFunctionInfo, TargetInfo &targetInfo, SearchParams &searchParams)
{
    size_t argc = THREE;
    napi_value args[THREE];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    if (argc != THREE) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
                         "Wrong number of arguments");
        return false;
    }

    napi_value targetInfoObj = args[0];
    napi_value targetTypeValue;
    int32_t targetType = static_cast<int32_t>(TargetType::FACE);
    if (napi_get_named_property(env, targetInfoObj, "targetType", &targetTypeValue) == napi_ok) {
        napi_get_value_int32(env, targetTypeValue, &targetType);
    }

    napi_value searchParamsObj = args[1];
    napi_value directionValue;
    int32_t direction = static_cast<int32_t>(SearchDirection::DEFAULT);
    if (napi_get_named_property(env, searchParamsObj, "direction", &directionValue) == napi_ok) {
        napi_get_value_int32(env, directionValue, &direction);
    }

    napi_valuetype type;
    napi_typeof(env, args[THIRD], &type);
    if (type != napi_function) {
        napi_throw_error(env, std::to_string(MechNapiErrorCode::PARAMETER_CHECK_FAILED).c_str(),
                         "Three argument must be a function");
        HILOGE("Three argument must be a function.");
        return false;
    }
    napi_ref callbackRef;
    napi_create_reference(env, args[THIRD], 1, &callbackRef);
    callbackFunctionInfo = {env, callbackRef};
    targetInfo.targetType = static_cast<TargetType>(targetType);
    searchParams.direction = static_cast<SearchDirection>(direction);
    return true;
}

napi_value CreateEnumObject(napi_env env, const std::vector<std::pair<std::string, int32_t>> &pairs)
{
    napi_value result;
    napi_create_object(env, &result);

    for (const auto &pair: pairs) {
        napi_value value;
        napi_create_int32(env, pair.second, &value);
        napi_set_named_property(env, result, pair.first.c_str(), value);
    }

    return result;
}

napi_value MechManager::CreateEulerAngles(napi_env env, const EulerAngles &degree)
{
    napi_value obj;
    napi_create_object(env, &obj);

    napi_value yaw;
    napi_create_double(env, degree.yaw, &yaw);
    napi_set_named_property(env, obj, "yaw", yaw);

    napi_value roll;
    napi_create_double(env, degree.roll, &roll);
    napi_set_named_property(env, obj, "roll", roll);
    
    napi_value pitch;
    napi_create_double(env, degree.pitch, &pitch);
    napi_set_named_property(env, obj, "pitch", pitch);

    return obj;
}

napi_value MechManager::CreateRotationLimit(napi_env env, const RotateDegreeLimit &limit)
{
    napi_value obj;
    napi_create_object(env, &obj);

    napi_value negMax = CreateEulerAngles(env, limit.negMax);
    napi_set_named_property(env, obj, "negMax", negMax);

    napi_value posMax = CreateEulerAngles(env, limit.posMax);
    napi_set_named_property(env, obj, "posMax", posMax);

    return obj;
}

napi_value MechManager::CreateRotationAxesStatus(napi_env env, const RotationAxesStatus &status)
{
    napi_value obj;
    napi_create_object(env, &obj);

    napi_value yawEnabled;
    napi_get_boolean(env, status.yawEnabled, &yawEnabled);
    napi_set_named_property(env, obj, "yawEnabled", yawEnabled);

    napi_value rollEnabled;
    napi_get_boolean(env, status.rollEnabled, &rollEnabled);
    napi_set_named_property(env, obj, "rollEnabled", rollEnabled);

    napi_value pitchEnabled;
    napi_get_boolean(env, status.pitchEnabled, &pitchEnabled);
    napi_set_named_property(env, obj, "pitchEnabled", pitchEnabled);

    napi_value yawLimited;
    napi_create_int32(env, static_cast<int32_t>(status.yawLimited), &yawLimited);
    napi_set_named_property(env, obj, "yawLimited", yawLimited);

    napi_value rollLimited;
    napi_create_int32(env, static_cast<int32_t>(status.rollLimited), &rollLimited);
    napi_set_named_property(env, obj, "rollLimited", rollLimited);

    napi_value pitchLimited;
    napi_create_int32(env, static_cast<int32_t>(status.pitchLimited), &pitchLimited);
    napi_set_named_property(env, obj, "pitchLimited", pitchLimited);

    return obj;
}

bool MechManager::IsSystemApp()
{
    static bool isSystemApp = []() {
        uint64_t tokenId = OHOS::IPCSkeleton::GetSelfTokenID();
        return OHOS::Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(tokenId);
    }();
    return isSystemApp;
}

bool MechManager::InitMechClient()
{
    if(mechClient_ != nullptr){
        return true;
    }
    {
        std::lock_guard<std::mutex> lock(mechClientMutex_);
        if(mechClient_ == nullptr){
            mechClient_ = std::make_shared<MechClient>();
        }
    }
    return mechClient_ != nullptr;
}

std::string MechManager::GenerateUniqueID(){
    // 获取当前时间戳
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    // 初始化随机数生成器
    std::mt19937 generator(now_c);

    // 生成随机数
    std::uniform_int_distribution<int> distribution(0, 15);
    std::uniform_int_distribution<int> distribution2(8, 11);

    // 生成UUID
    std::stringstream ss;
    ss << std::hex << std::uppercase;
    for (int i = 0; i < 32; ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            ss << "-";
        }
        if (i == 13) {
            ss << distribution2(generator);
        } else {
            ss << distribution(generator);
        }
    }
    return ss.str();
}

void MechManager::AttachStateChangeStubDeathListener::OnRemoteDied(const wptr <IRemoteObject> &object)
{
    HILOGE("AttachStateChangeStub RemoteObject dead; ");
    std::lock_guard<std::mutex> lock(attachStateChangeStubMutex_);
    attachStateChangeStub_ = nullptr;
}

void MechManager::TrackingEventStubDeathListener::OnRemoteDied(const wptr <IRemoteObject> &object)
{
    HILOGE("TrackingEventStub RemoteObject dead; ");
    std::lock_guard<std::mutex> lock(trackingEventStubMutex_);
    trackingEventStub_ = nullptr;
}

void MechManager::RotationAxesStatusChangeStubDeathListener::OnRemoteDied(const wptr <IRemoteObject> &object)
{
    HILOGE("RotationAxesStatusChangeStub RemoteObject dead; ");
    std::lock_guard<std::mutex> lock(rotationAxesStatusChangeMutex_);
    rotationAxesStatusChangeStub_ = nullptr;
}

void MechManager::CmdChannelDeathListener::OnRemoteDied(const wptr <IRemoteObject> &object)
{
    HILOGE("CmdChannel RemoteObject dead; ");
    std::lock_guard<std::mutex> lock(cmdChannelMutex_);
    cmdChannel_ = nullptr;
}


napi_value Init(napi_env env, napi_value exports)
{
    napi_value rotationAxisLimitedEnum =
        CreateEnumObject(env, {{"NOT_LIMITED", 0}, {"NEGATIVE_LIMITED", 1}, {"POSITIVE_LIMITED", 2}});
    napi_set_named_property(env, exports, "RotationAxisLimited", rotationAxisLimitedEnum);
    napi_value operationEnum = CreateEnumObject(env, {{"CONNECT", 0}, {"DISCONNECT", 1}});
    napi_set_named_property(env, exports, "Operation", operationEnum);
    napi_value trackingEventEnum =
        CreateEnumObject(env, {{"CAMERA_TRACKING_USER_ENABLED", 0}, {"CAMERA_TRACKING_USER_DISABLED", 1},
                               {"CAMERA_TRACKING_LAYOUT_CHANGED", 2}});
    napi_set_named_property(env, exports, "TrackingEvent", trackingEventEnum);
    napi_value resultEnum =
        CreateEnumObject(env, {{"COMPLETED", 0}, {"INTERRUPTED", 1}, {"LIMITED", 2},
                               {"TIMEOUT",3}, {"SYSTEM_ERROR", 100}});
    napi_set_named_property(env, exports, "Result", resultEnum);
    napi_value mechDeviceTypeEnum = CreateEnumObject(env, {{"GIMBAL_DEVICE", 0}});
    napi_set_named_property(env, exports, "MechDeviceType", mechDeviceTypeEnum);
    napi_value attachStateEnum = CreateEnumObject(env, {{"ATTACHED", 0}, {"DETACHED", 1}});
    napi_set_named_property(env, exports, "AttachState", attachStateEnum);
    napi_value cameraTrackingLayoutEnum =
        CreateEnumObject(env, {{"DEFAULT", 0}, {"LEFT", 1}, {"MIDDLE", 2}, {"RIGHT", 3}});
    napi_set_named_property(env, exports, "CameraTrackingLayout", cameraTrackingLayoutEnum);
    napi_value targetTypeEnum = CreateEnumObject(env, {{"FACE", 0}});
    napi_set_named_property(env, exports, "TargetType", targetTypeEnum);
    napi_value searchDirectionEnum = CreateEnumObject(env, {{"DEFAULT", 0}, {"LEFTWARD", 1}, {"RIGHTWARD", 2}});
    napi_set_named_property(env, exports, "SearchDirectionEnum", searchDirectionEnum);
    napi_value MechManager;
    napi_create_object(env, &MechManager);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("on", MechManager::On),
        DECLARE_NAPI_FUNCTION("off", MechManager::Off),

        DECLARE_NAPI_FUNCTION("getAttachedMechDevices", MechManager::GetAttachedDevices),
        DECLARE_NAPI_FUNCTION("setUserOperation", MechManager::SetUserOperation),

        DECLARE_NAPI_FUNCTION("setCameraTrackingEnabled", MechManager::SetCameraTrackingEnabled),
        DECLARE_NAPI_FUNCTION("getCameraTrackingEnabled", MechManager::GetCameraTrackingEnabled),
        DECLARE_NAPI_FUNCTION("setCameraTrackingLayout", MechManager::SetCameraTrackingLayout),
        DECLARE_NAPI_FUNCTION("getCameraTrackingLayout", MechManager::GetCameraTrackingLayout),

        DECLARE_NAPI_FUNCTION("rotate", MechManager::Rotate),
        DECLARE_NAPI_FUNCTION("rotateToEulerAngles", MechManager::RotateToEulerAngles),
        DECLARE_NAPI_FUNCTION("getMaxRotationTime", MechManager::GetMaxRotationTime),
        DECLARE_NAPI_FUNCTION("getMaxRotationSpeed", MechManager::GetMaxRotationSpeed),
        DECLARE_NAPI_FUNCTION("rotateBySpeed", MechManager::RotateBySpeed),
        DECLARE_NAPI_FUNCTION("stopMoving", MechManager::StopMoving),
        DECLARE_NAPI_FUNCTION("getCurrentAngles", MechManager::GetCurrentAngles),
        DECLARE_NAPI_FUNCTION("getRotationLimits", MechManager::GetRotationLimits),
        DECLARE_NAPI_FUNCTION("getRotationAxesStatus", MechManager::GetRotationAxesStatus),
        DECLARE_NAPI_FUNCTION("searchTarget", MechManager::SearchTarget),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}

static napi_module mechManagerModule = {
        .nm_filename = "distributedHardware/libmechanicmanager_napi.so/mechanicmanager.js",
        .nm_register_func = Init,
        .nm_modname = "distributedHardware.mechanicManager",
};

extern "C" __attribute__((constructor)) void MechManagerModuleRegister()
{
    napi_module_register(&mechManagerModule);
}
} // namespace MechManager
} // namespace OHOS