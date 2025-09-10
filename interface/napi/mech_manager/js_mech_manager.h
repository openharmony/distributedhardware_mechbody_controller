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

#ifndef MECH_MANAGER_NAPI_H
#define MECH_MANAGER_NAPI_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "ipc_skeleton.h"
#include "js_mech_manager_client.h"
#include "js_mech_manager_stub.h"
#include "js_mech_manager_service.h"

namespace OHOS {
namespace MechBodyController {

class MechManager {
public:
    static napi_value On(napi_env env, napi_callback_info info);

    static napi_value Off(napi_env env, napi_callback_info info);

    static napi_value GetAttachedDevices(napi_env env, napi_callback_info info);

    static napi_value SetUserOperation(napi_env env, napi_callback_info info);

    static napi_value SetCameraTrackingEnabled(napi_env env, napi_callback_info info);

    static napi_value GetCameraTrackingEnabled(napi_env env, napi_callback_info info);

    static napi_value SetCameraTrackingLayout(napi_env env, napi_callback_info info);

    static napi_value GetCameraTrackingLayout(napi_env env, napi_callback_info info);

    static napi_value Rotate(napi_env env, napi_callback_info info);

    static napi_value RotateToEulerAngles(napi_env env, napi_callback_info info);

    static napi_value GetMaxRotationTime(napi_env env, napi_callback_info info);

    static napi_value GetMaxRotationSpeed(napi_env env, napi_callback_info info);

    static napi_value RotateBySpeed(napi_env env, napi_callback_info info);

    static napi_value StopMoving(napi_env env, napi_callback_info info);

    static napi_value GetCurrentAngles(napi_env env, napi_callback_info info);

    static napi_value GetRotationLimits(napi_env env, napi_callback_info info);

    static napi_value GetRotationAxesStatus(napi_env env, napi_callback_info info);

    static napi_value SearchTarget(napi_env env, napi_callback_info info);

private:
    static int32_t ExecuteOn(std::string &eventType, CallbackFunctionInfo &callbackFunctionInfo);
    static int32_t ExecuteOnForAttachStateChange(const CallbackFunctionInfo &callbackFunctionInfo);
    static int32_t ExecuteOnForTrackingEvent(const CallbackFunctionInfo &callbackFunctionInfo);
    static int32_t ExecuteOnForRotationAxesStatusChange(const CallbackFunctionInfo &callbackFunctionInfo);
    static bool InitAttachStateChangeStub();
    static void ProcessOnResultCode(napi_env env, int32_t &result);

    static int32_t ExecuteOff(std::string &eventType);
    static int32_t ExecuteOff(std::string &eventType, CallbackFunctionInfo &callbackFunctionInfo);
    static int32_t ExecuteOffForAttachStateChange(CallbackFunctionInfo &callbackFunctionInfo);
    static int32_t ExecuteOffForTrackingEvent(CallbackFunctionInfo &callbackFunctionInfo);
    static int32_t ExecuteOffForRotationAxesStatusChange(CallbackFunctionInfo &callbackFunctionInfo);
    static bool HasSameCallbackFunction(std::set<CallbackFunctionInfo> &cacheInfo,
        CallbackFunctionInfo &callbackFunctionInfo);
    static bool RemoveSameCallbackFunction(std::set<CallbackFunctionInfo> &cacheInfo,
        CallbackFunctionInfo &callbackFunctionInfo);
    static void ProcessOffResultCode(napi_env env, int32_t &result);

    static napi_value MechInfoToNapiObject(napi_env env, const std::shared_ptr<MechInfo> &info);

    static bool RegisterCmdChannel();

    static bool GetRotateParam(napi_env env, napi_callback_info info,
        RotateByDegreeParam &rotateParam, int32_t &mechId);

    static bool GetRotateToEulerAnglesParam(napi_env env, napi_callback_info info,
        RotateToEulerAnglesParam &rotateToEulerAnglesParam, int32_t &mechId);
    static napi_value RotateSpeedToNapi(napi_env env, const RotateSpeed &speed);

    static bool GetRotateBySpeedParam(napi_env env, napi_callback_info info,
        RotateBySpeedParam &rotateBySpeedParam, int32_t &mechId);

    static napi_value CreateEulerAngles(napi_env env, const EulerAngles &degree);

    static bool InitTrackingEventStub();

    static bool InitRotationAxesStatusChangeStub();

    static napi_value CreateRotationLimit(napi_env env, const RotateDegreeLimit &limit);

    static napi_value CreateRotationAxesStatus(napi_env env, const RotationAxesStatus &status);

    static bool GetSearchTargetParam(napi_env env, napi_callback_info info,
        CallbackFunctionInfo &callbackFunctionInfo, TargetInfo &targetInfo, SearchParams &searchParams);

    static bool IsSystemApp();

    static bool InitMechClient();

    static std::string GenerateUniqueID();

private:
    static std::mutex attachStateChangeStubMutex_;
    static sptr<JsMechManagerStub> attachStateChangeStub_;
    static std::mutex trackingEventStubMutex_;
    static sptr<JsMechManagerStub> trackingEventStub_;
    static std::mutex rotationAxesStatusChangeMutex_;
    static sptr<JsMechManagerStub> rotationAxesStatusChangeStub_;
    static std::mutex cmdChannelMutex_;
    static sptr<JsMechManagerStub> cmdChannel_;
    static std::mutex mechClientMutex_;
    static std::shared_ptr<MechClient> mechClient_;

    class AttachStateChangeStubDeathListener : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;

    };

    class TrackingEventStubDeathListener : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;

    };

    class RotationAxesStatusChangeStubDeathListener : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;

    };

    class CmdChannelDeathListener : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;

    };

};

napi_value Init(napi_env env, napi_value exports);

} // namespace MechManager
} // namespace OHOS

#endif // MECH_MANAGER_NAPI_H