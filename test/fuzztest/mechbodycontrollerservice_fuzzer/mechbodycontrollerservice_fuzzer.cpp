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

 #include "mechbodycontrollerservice_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include <string>

#define private public
#define protected public
#include "mechbody_controller_service.h"
#undef private
#undef protected
#include "ipc_skeleton.h"
#include "securec.h"
#include "accesstoken_kit.h"
#include "token_setproc.h"
#include "tokenid_kit.h"
#include "system_ability.h"

namespace OHOS {
using namespace OHOS::MechBodyController;
bool Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(uint64_t tokenId)
{
    return true;
}

void OnStartFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.OnStart();
}

void RegisterAttachStateChangeCallbackFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    const sptr<OHOS::IRemoteObject> listenerNull = nullptr;
    const sptr<OHOS::IRemoteObject> listener;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.RegisterAttachStateChangeCallback(listenerNull);
    mechBodyControllerService.RegisterAttachStateChangeCallback(listener);
}

void UnRegisterAttachStateChangeCallbackFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t tokenId = fdp.ConsumeIntegral<uint32_t>();
    const sptr<OHOS::IRemoteObject> callback;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.listener_ = std::shared_ptr<IMechConnectListener>();
    mechBodyControllerService.deviceAttachCallback_[tokenId] = callback;
    mechBodyControllerService.UnRegisterAttachStateChangeCallback();
}

void OnAttachStateChangeFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    std::string mechName = fdp.ConsumeRandomLengthString();
    uint32_t tokenId = fdp.ConsumeIntegral<uint32_t>();
    uint32_t enumIdx = fdp.ConsumeIntegral<uint32_t>() % 2;
    AttachmentState attachmentState = static_cast<AttachmentState>(enumIdx);
    AttachmentState state = static_cast<AttachmentState>(enumIdx);
    const sptr<OHOS::IRemoteObject> callback;
    MechInfo mechInfo;
    mechInfo.mechName = mechName;
    mechInfo.state = state;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.deviceAttachCallback_[tokenId] = callback;
    mechBodyControllerService.OnAttachStateChange(attachmentState, mechInfo);
}

void OnDeviceConnectedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    int32_t mechId = fdp.ConsumeIntegral<int32_t>();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.OnDeviceConnected(mechId);
}

void GetAttachedDevicesFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    std::set<MechInfo> mechInfo;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetAttachedDevices(mechInfo);
}

void SetTrackingEnabledFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    bool isEnabled = fdp.ConsumeIntegral<bool>();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.SetTrackingEnabled(isEnabled);
}

void GetTrackingEnabledFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    bool isEnabled = fdp.ConsumeIntegral<bool>();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetTrackingEnabled(isEnabled);
}

void RegisterTrackingEventCallbackFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    sptr<IRemoteObject> callback;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.RegisterTrackingEventCallback(callback);
}

void UnRegisterTrackingEventCallbackFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.UnRegisterTrackingEventCallback();
}

void SetTrackingLayoutFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t enumIdx = fdp.ConsumeIntegral<uint32_t>() % 4;
    CameraTrackingLayout cameraTrackingLayout = static_cast<CameraTrackingLayout>(enumIdx);

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.SetTrackingLayout(cameraTrackingLayout);
}

void GetTrackingLayoutFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t enumIdx = fdp.ConsumeIntegral<uint32_t>() % 4;
    CameraTrackingLayout cameraTrackingLayout = static_cast<CameraTrackingLayout>(enumIdx);

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetTrackingLayout(cameraTrackingLayout);
}

void RotateByDegreeFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t duration = fdp.ConsumeIntegral<uint32_t>();
    float yaw = fdp.ConsumeIntegral<uint32_t>();
    float roll = fdp.ConsumeIntegral<uint32_t>();
    float pitch = fdp.ConsumeIntegral<uint32_t>();
    RotateByDegreeParam rotateParam;
    rotateParam.duration = static_cast<int32_t>(duration);
    rotateParam.degree.yaw = yaw;
    rotateParam.degree.roll = roll;
    rotateParam.degree.pitch = pitch;
    auto rotateByDegreeParam = std::make_shared<RotateByDegreeParam>(rotateParam);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    std::string cmdId = fdp.ConsumeRandomLengthString();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.RotateByDegree(mechId, cmdId, rotateByDegreeParam);
}

void NotifyOperationResultFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t enumIdx = fdp.ConsumeIntegral<uint32_t>() % 4;
    ExecResult result = static_cast<ExecResult>(enumIdx);
    uint32_t tokenId = fdp.ConsumeIntegral<uint32_t>();
    std::string cmdId = fdp.ConsumeRandomLengthString();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.NotifyOperationResult(tokenId, cmdId, result);
}

void RotateToEulerAnglesFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t duration = fdp.ConsumeIntegral<uint32_t>();
    float yaw = fdp.ConsumeIntegral<uint32_t>();
    float roll = fdp.ConsumeIntegral<uint32_t>();
    float pitch = fdp.ConsumeIntegral<uint32_t>();
    RotateToEulerAnglesParam rotateParam;
    rotateParam.duration = static_cast<int32_t>(duration);
    rotateParam.angles.yaw = yaw;
    rotateParam.angles.roll = roll;
    rotateParam.angles.pitch = pitch;
    auto rotateToEulerAnglesParam = std::make_shared<RotateToEulerAnglesParam>(rotateParam);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    std::string cmdId = fdp.ConsumeRandomLengthString();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.RotateToEulerAngles(mechId, cmdId, rotateToEulerAnglesParam);
}

void GetMaxRotationTimeFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    auto timeLimit = std::shared_ptr<TimeLimit>();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetMaxRotationTime(mechId, timeLimit);
}

void GetMaxRotationSpeedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    float yawMin = fdp.ConsumeIntegral<uint32_t>();
    float rollMin = fdp.ConsumeIntegral<uint32_t>();
    float pitchMin = fdp.ConsumeIntegral<uint32_t>();
    float yawMax = fdp.ConsumeIntegral<uint32_t>();
    float rollMax = fdp.ConsumeIntegral<uint32_t>();
    float pitchMax = fdp.ConsumeIntegral<uint32_t>();
    RotateSpeedLimit rotateSpeedLimit;
    rotateSpeedLimit.speedMax.yawSpeed = yawMax;
    rotateSpeedLimit.speedMax.rollSpeed = rollMax;
    rotateSpeedLimit.speedMax.pitchSpeed = pitchMax;
    rotateSpeedLimit.speedMin.yawSpeed = yawMin;
    rotateSpeedLimit.speedMin.rollSpeed = rollMin;
    rotateSpeedLimit.speedMin.pitchSpeed = pitchMin;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetMaxRotationSpeed(mechId, rotateSpeedLimit);
}

void RotateBySpeedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t duration = fdp.ConsumeIntegral<uint32_t>();
    float yaw = fdp.ConsumeIntegral<uint32_t>();
    float roll = fdp.ConsumeIntegral<uint32_t>();
    float pitch = fdp.ConsumeIntegral<uint32_t>();
    RotateBySpeedParam rotateParam;
    rotateParam.duration = static_cast<float>(duration);
    rotateParam.speed.yawSpeed = yaw;
    rotateParam.speed.rollSpeed = roll;
    rotateParam.speed.pitchSpeed = pitch;
    auto rotateBySpeedParam = std::make_shared<RotateBySpeedParam>(rotateParam);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    std::string cmdId = fdp.ConsumeRandomLengthString();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.RotateBySpeed(mechId, cmdId, rotateBySpeedParam);
}

void StopMovingFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    std::string cmdId = fdp.ConsumeRandomLengthString();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.StopMoving(mechId, cmdId);
}

void GetRotationAnglesFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    float yaw = fdp.ConsumeIntegral<uint32_t>();
    float roll = fdp.ConsumeIntegral<uint32_t>();
    float pitch = fdp.ConsumeIntegral<uint32_t>();
    EulerAngles eulerAngles;
    eulerAngles.yaw = yaw;
    eulerAngles.roll = roll;
    eulerAngles.pitch = pitch;
    auto eulerAnglesPtr = std::make_shared<EulerAngles>(eulerAngles);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetRotationAngles(mechId, eulerAnglesPtr);
}

void GetRotationDegreeLimitsFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    float yawNeg = fdp.ConsumeIntegral<uint32_t>();
    float rollNeg = fdp.ConsumeIntegral<uint32_t>();
    float pitchNeg = fdp.ConsumeIntegral<uint32_t>();
    float yawPos = fdp.ConsumeIntegral<uint32_t>();
    float rollPos = fdp.ConsumeIntegral<uint32_t>();
    float pitchPos = fdp.ConsumeIntegral<uint32_t>();
    RotateDegreeLimit rotationLimit;
    rotationLimit.negMax.yaw = yawNeg;
    rotationLimit.negMax.roll = rollNeg;
    rotationLimit.negMax.pitch = pitchNeg;
    rotationLimit.posMax.yaw = yawPos;
    rotationLimit.posMax.roll = rollPos;
    rotationLimit.posMax.pitch = pitchPos;
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetRotationDegreeLimits(mechId, rotationLimit);
}

void GetRotationAxesStatusFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    bool boolIdx = fdp.ConsumeIntegral<uint32_t>() % 2;
    uint32_t enumIdx = fdp.ConsumeIntegral<uint32_t>() % 3;
    RotationAxisLimited limited = static_cast<RotationAxisLimited>(enumIdx);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    RotationAxesStatus axesStatus;
    axesStatus.pitchEnabled = boolIdx;
    axesStatus.rollEnabled = boolIdx;
    axesStatus.yawEnabled = boolIdx;
    axesStatus.pitchLimited = limited;
    axesStatus.rollLimited = limited;
    axesStatus.yawLimited = limited;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.GetRotationAxesStatus(mechId, axesStatus);
}

void RegisterRotationAxesStatusChangeCallbackFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    auto callback = sptr<IRemoteObject>();

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.RegisterRotationAxesStatusChangeCallback(callback);
}

void UnRegisterRotationAxesStatusChangeCallbackFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.UnRegisterRotationAxesStatusChangeCallback();
}

void OnRotationAxesStatusChangeFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    bool boolIdx = fdp.ConsumeIntegral<uint32_t>() % 2;
    uint32_t enumIdx = fdp.ConsumeIntegral<uint32_t>() % 3;
    RotationAxisLimited limited = static_cast<RotationAxisLimited>(enumIdx);
    uint32_t mechId = fdp.ConsumeIntegral<uint32_t>();
    RotationAxesStatus axesStatus;
    axesStatus.pitchEnabled = boolIdx;
    axesStatus.rollEnabled = boolIdx;
    axesStatus.yawEnabled = boolIdx;
    axesStatus.pitchLimited = limited;
    axesStatus.rollLimited = limited;
    axesStatus.yawLimited = limited;

    MechBodyControllerService& mechBodyControllerService = MechBodyControllerService::GetInstance();
    mechBodyControllerService.OnRotationAxesStatusChange(mechId, axesStatus);
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::OnStartFuzzTest(data, size);
    OHOS::RegisterAttachStateChangeCallbackFuzzTest(data, size);
    OHOS::UnRegisterAttachStateChangeCallbackFuzzTest(data, size);
    OHOS::OnAttachStateChangeFuzzTest(data, size);
    OHOS::OnDeviceConnectedFuzzTest(data, size);
    OHOS::GetAttachedDevicesFuzzTest(data, size);
    OHOS::SetTrackingEnabledFuzzTest(data, size);
    OHOS::GetTrackingEnabledFuzzTest(data, size);
    OHOS::RegisterTrackingEventCallbackFuzzTest(data, size);
    OHOS::UnRegisterTrackingEventCallbackFuzzTest(data, size);
    OHOS::SetTrackingLayoutFuzzTest(data, size);
    OHOS::GetTrackingLayoutFuzzTest(data, size);
    OHOS::RotateByDegreeFuzzTest(data, size);
    OHOS::NotifyOperationResultFuzzTest(data, size);
    OHOS::RotateToEulerAnglesFuzzTest(data, size);
    OHOS::GetMaxRotationTimeFuzzTest(data, size);
    OHOS::GetMaxRotationSpeedFuzzTest(data, size);
    OHOS::RotateBySpeedFuzzTest(data, size);
    OHOS::StopMovingFuzzTest(data, size);
    OHOS::GetRotationAnglesFuzzTest(data, size);
    OHOS::GetRotationDegreeLimitsFuzzTest(data, size);
    OHOS::GetRotationAxesStatusFuzzTest(data, size);
    OHOS::RegisterRotationAxesStatusChangeCallbackFuzzTest(data, size);
    OHOS::UnRegisterRotationAxesStatusChangeCallbackFuzzTest(data, size);
    OHOS::OnRotationAxesStatusChangeFuzzTest(data, size);
    return 0;
}