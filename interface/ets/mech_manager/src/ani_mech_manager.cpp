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

#include "ani_mech_manager.h"

#include <unistd.h>
#include <memory>
#include <utility>
#include <vector>
#include <random>
#include <ios>
#include <sstream>

#include "tokenid_kit.h"
#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "AniMechManager";
    [[ maybe_unused ]] constexpr int32_t LAYOUT_MAX = 3;
    [[ maybe_unused ]] constexpr int32_t EIGHT = 8;
    [[ maybe_unused ]] constexpr int32_t ELEVEN = 11;
    [[ maybe_unused ]] constexpr int32_t THIRTEEN = 13;
    [[ maybe_unused ]] constexpr int32_t FIFTEEN = 15;
    [[ maybe_unused ]] constexpr int32_t EIGHTEEN = 18;
    [[ maybe_unused ]] constexpr int32_t TWENTY_THREE = 23;
    [[ maybe_unused ]] constexpr int32_t THIRTY_TWO = 32;
    constexpr char CLASS_NAME_INT[] = "std.core.Int";
}

AniMechManager& AniMechManager::GetInstance()
{
    static auto instance = new AniMechManager();
    return *instance;
}

AniMechManager::AniMechManager()
{
    mechClient_ = std::make_shared<AniMechClient>();
}

AniMechManager::~AniMechManager()
{
    HILOGI("~AniMechManager.");
}

void AniMechManager::ProcessOnResultCode(int32_t &result)
{
    HILOGE("Register the callback function result code: %{public}d ", result);
    if (result == MechNapiErrorCode::PARAMETER_CHECK_FAILED) {
        ::taihe::set_business_error(result, "Invalid event type.");
        return;
    }
    if (result == MechNapiErrorCode::PERMISSION_DENIED) {
        ::taihe::set_business_error(result, "Not system application");
        return;
    }
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        ::taihe::set_business_error(result, "Device not connected");
        return;
    }
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
}

void AniMechManager::OnAttachStateChange(const AttachStateCBTaihe &callback)
{
    HILOGI("Start to register the callback function.");
    if (CheckControlL1()) {
        return;
    }
    int32_t result = ExecuteOnForAttachStateChange(callback);
    ProcessOnResultCode(result);
}

void AniMechManager::OffAttachStateChange(const ::taihe::optional_view<AttachStateCBTaihe> &callback)
{
    HILOGI("Start to unregister the callback function.");
    if (CheckControlL1()) {
        return;
    }
    int32_t result = ExecuteOffForAttachStateChange(!callback.has_value(), callback.value());
    ProcessOffResultCode(result);
}

::taihe::array<MechInfoTaihe> AniMechManager::GetAttachedDevices()
{
    std::vector<MechInfoTaihe> vecMechInfos;
    if (CheckControlL1()) {
        return ::taihe::array<MechInfoTaihe>(vecMechInfos);
    }
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return ::taihe::array<MechInfoTaihe>(vecMechInfos);
    }
    std::vector<std::shared_ptr<MechInfo>> mechInfos;
    int32_t result = mechClient_->GetAttachedDevices(mechInfos);
    HILOGE("result code: %{public}d ", result);
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return ::taihe::array<MechInfoTaihe>(vecMechInfos);
    }
    for (const auto &per : mechInfos) {
        MechInfoTaihe perTaihe {
            .mechId = per->mechId,
            .mechDeviceType = MechDeviceTypeTaihe::from_value(static_cast<int32_t>(per->mechType)),
            .mechName = per->mechName
        };
        vecMechInfos.push_back(perTaihe);
    }
    return ::taihe::array<MechInfoTaihe>(vecMechInfos);
}

void AniMechManager::SetUserOperation(const OperationTaihe &operation,
    const std::string &mac, const std::string &params)
{
    HILOGD("SetUserOperation enter");
    if (!PreCheck()) {
        return;
    }
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    Operation inOperation = static_cast<Operation>(operation.get_value());
    int32_t result = mechClient_->SetUserOperation(inOperation, mac, params);
    HILOGE("result code: %{public}d ", result);
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
    }
}

void AniMechManager::SetCameraTrackingEnabled(bool isEnabled)
{
    if (CheckDeviceL1()) {
        return;
    }
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    int32_t result = mechClient_->SetCameraTrackingEnabled(isEnabled);
    HILOGI("result code: %{public}d ", result);
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        ::taihe::set_business_error(MechNapiErrorCode::DEVICE_NOT_CONNECTED, "Device not connected");
        return;
    }
    if (result == MechNapiErrorCode::DEVICE_NOT_SUPPORTED) {
        ::taihe::set_business_error(MechNapiErrorCode::DEVICE_NOT_SUPPORTED, "Device not supported");
        return;
    }
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
    }
}

void AniMechManager::GetCameraTrackingEnabled(bool &isEnabled)
{
    if (CheckDeviceL1()) {
        return;
    }
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    isEnabled = false;
    int32_t result = mechClient_->GetCameraTrackingEnabled(isEnabled);
    HILOGI("result code: %{public}d ", result);
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        ::taihe::set_business_error(MechNapiErrorCode::DEVICE_NOT_CONNECTED, "Device not connected");
        return;
    }
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
}

void AniMechManager::OnTrackingStateChange(const TrackingEventCBTaihe &callback)
{
    HILOGI("Start to register the callback function.");
    if (CheckControlL1()) {
        return;
    }
    int32_t result = ExecuteOnForTrackingEvent(callback);
    ProcessOnResultCode(result);
}

void AniMechManager::OffTrackingStateChange(const::taihe::optional_view<TrackingEventCBTaihe> &callback)
{
    HILOGI("Start to unregister the callback function.");
    if (CheckControlL1()) {
        return;
    }
    int32_t result = ExecuteOffForTrackingEvent(!callback.has_value(), callback.value());
    ProcessOffResultCode(result);
}

void AniMechManager::GetMaxRotationTime(int32_t mechId, int32_t &maxTime)
{
    maxTime = 0;
    if (CheckDeviceL1()) {
        return;
    }
    if (!IsSystemApp()) {
        ::taihe::set_business_error(MechNapiErrorCode::PERMISSION_DENIED, "Not system application");
        return;
    }
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    TimeLimit timeLimit;
    int32_t result = mechClient_->GetMaxRotationTime(mechId, timeLimit);
    HILOGE("result: %{public}d;", result);
    if (result != ERR_OK) {
        HILOGE("time limit query failed, result: %{public}d;", result);
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    maxTime = static_cast<int32_t>(timeLimit.max);
}

void AniMechManager::GetMaxRotationSpeed(int32_t mechId, RotationSpeedTaihe &speed)
{
    if (CheckDeviceL1()) {
        return;
    }
    if (!IsSystemApp()) {
        ::taihe::set_business_error(MechNapiErrorCode::PERMISSION_DENIED, "Not system application");
        return;
    }
    RotateSpeedLimit rotateSpeedLimit;
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    int32_t result = mechClient_->GetMaxRotationSpeed(mechId, rotateSpeedLimit);
    HILOGE("result: %{public}d;", result);
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    RotateSpeedToAni(rotateSpeedLimit.speedMax, speed);
}

void AniMechManager::SetCameraTrackingLayout(const CameraTrackingLayoutTaihe &layout)
{
    if (CheckDeviceL1()) {
        return;
    }
    if (!IsSystemApp()) {
        ::taihe::set_business_error(MechNapiErrorCode::PERMISSION_DENIED, "Not system application");
        return;
    }
    int32_t layoutT = layout.get_value();
    if (layoutT > LAYOUT_MAX || layoutT < 0) {
        ::taihe::set_business_error(MechNapiErrorCode::PARAMETER_CHECK_FAILED, "trackingLayout out of range");
        return;
    }
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    CameraTrackingLayout layoutInner = static_cast<CameraTrackingLayout>(layoutT);
    int32_t result = mechClient_->SetCameraTrackingLayout(layoutInner);
    HILOGI("result code: %{public}d ", result);
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        ::taihe::set_business_error(MechNapiErrorCode::DEVICE_NOT_CONNECTED, "Device not connected");
        return;
    }
    if (result == MechNapiErrorCode::DEVICE_NOT_SUPPORTED) {
        ::taihe::set_business_error(MechNapiErrorCode::DEVICE_NOT_SUPPORTED, "Device not supported");
        return;
    }
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
}

void AniMechManager::GetCameraTrackingLayout(CameraTrackingLayoutTaihe &layout)
{
    if (CheckDeviceL1()) {
        return;
    }
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    CameraTrackingLayout layoutInner;
    int32_t result = mechClient_->GetCameraTrackingLayout(layoutInner);
    HILOGI("result code: %{public}d ", result);
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        ::taihe::set_business_error(MechNapiErrorCode::DEVICE_NOT_CONNECTED, "Device not connected");
        return;
    }
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    layout = CameraTrackingLayoutTaihe::from_value(static_cast<int32_t>(layoutInner));
}

void AniMechManager::RotateSpeedToAni(const RotateSpeed &speed, RotationSpeedTaihe &out)
{
    out.yawSpeed = ::taihe::optional<double>(std::in_place, static_cast<double>(speed.yawSpeed));
    out.rollSpeed = ::taihe::optional<double>(std::in_place, static_cast<double>(speed.rollSpeed));
    out.pitchSpeed = ::taihe::optional<double>(std::in_place, static_cast<double>(speed.pitchSpeed));
}

void AniMechManager::RotationAxesStatusToAni(const RotationAxesStatus &axesStatus, RotationAxesStatusTaihe &out)
{
    out.yawEnabled = axesStatus.yawEnabled;
    out.rollEnabled = axesStatus.rollEnabled;
    out.pitchEnabled = axesStatus.pitchEnabled;
    out.yawLimited = ::taihe::optional<RotationAxisLimitedTaihe>(std::in_place,
        RotationAxisLimitedTaihe::from_value(static_cast<int32_t>(axesStatus.yawLimited)));
    out.rollLimited = ::taihe::optional<RotationAxisLimitedTaihe>(std::in_place,
        RotationAxisLimitedTaihe::from_value(static_cast<int32_t>(axesStatus.rollLimited)));
    out.pitchLimited = ::taihe::optional<RotationAxisLimitedTaihe>(std::in_place,
        RotationAxisLimitedTaihe::from_value(static_cast<int32_t>(axesStatus.pitchLimited)));
}

void AniMechManager::RotationLimitsToAni(const RotateDegreeLimit &limit, RotationLimitsTaihe &out)
{
    out.negativeYawMax = limit.negMax.yaw;
    out.negativeRollMax = limit.negMax.roll;
    out.negativePitchMax = limit.negMax.pitch;
    out.positiveYawMax = limit.posMax.yaw;
    out.positiveRollMax = limit.posMax.roll;
    out.positivePitchMax = limit.posMax.yaw;
}

void AniMechManager::Rotate(int32_t mechId,
    RotationAnglesTaihe const& angles, int32_t duration, uintptr_t &promise)
{
    HILOGI("start");
    promise = 0;
    if (CheckDeviceL1()) {
        return;
    }
    if (!IsSystemApp()) {
        ::taihe::set_business_error(MechNapiErrorCode::PERMISSION_DENIED, "Not system application");
        return;
    }
    RotateByDegreeParam rotateParam;
    rotateParam.duration = duration;
    double defValue = 0.0;
    rotateParam.degree.yaw =
        static_cast<float>(angles.yaw.has_value()? angles.yaw.value() : defValue);
    rotateParam.degree.roll =
        static_cast<float>(angles.roll.has_value()? angles.roll.value() : defValue);
    rotateParam.degree.pitch =
        static_cast<float>(angles.pitch.has_value()? angles.pitch.value() : defValue);
    auto rotatePromiseParam = std::make_shared<AniRotatePrimiseFulfillmentParam>();
    if (!InitRotatePrimiseFulfillmentParam(*rotatePromiseParam)) {
        HILOGE("InitRotatePrimiseFulfillmentParam failed;");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    if (!InitMechClient() || !RegisterCmdChannel()) {
        HILOGE("InitMechClient or RegisterCmdChannel failed;");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    int32_t result = mechClient_->Rotate(mechId, rotatePromiseParam->cmdId, rotateParam);
    HILOGI("result code: %{public}d ", result);
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        ::taihe::set_business_error(MechNapiErrorCode::DEVICE_NOT_CONNECTED, "Device not connected");
        return;
    }
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    std::lock_guard<std::mutex> lock(promiseParamsMutex_);
    promiseParams_[rotatePromiseParam->cmdId] = rotatePromiseParam;
    promise = reinterpret_cast<uintptr_t>(rotatePromiseParam->promise);
}

void AniMechManager::RotateToEulerAngles(int32_t mechId,
    EulerAnglesTaihe const& angles, int32_t duration, uintptr_t &promise)
{
    HILOGI("start");
    if (!PreCheck()) {
        return;
    }
    RotateToEulerAnglesParam rotateToEulerAnglesParam;
    rotateToEulerAnglesParam.duration = duration;
    double defValue = 0.0;
    rotateToEulerAnglesParam.angles.yaw =
        static_cast<float>(angles.yaw.has_value()? angles.yaw.value() : defValue);
    rotateToEulerAnglesParam.angles.roll =
        static_cast<float>(angles.roll.has_value()? angles.roll.value() : defValue);
    rotateToEulerAnglesParam.angles.pitch =
        static_cast<float>(angles.pitch.has_value()? angles.pitch.value() : defValue);
    auto rotatePromiseParam = std::make_shared<AniRotatePrimiseFulfillmentParam>();
    if (!InitRotatePrimiseFulfillmentParam(*rotatePromiseParam)) {
        HILOGE("InitRotatePrimiseFulfillmentParam failed;");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    if (!InitMechClient() || !RegisterCmdChannel()) {
        HILOGE("InitMechClient or RegisterCmdChannel failed;");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    int32_t result = mechClient_->RotateToEulerAngles(mechId, rotatePromiseParam->cmdId, rotateToEulerAnglesParam);
    HILOGI("result code: %{public}d ", result);
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        ::taihe::set_business_error(MechNapiErrorCode::DEVICE_NOT_CONNECTED, "Device not connected");
        return;
    }
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    std::lock_guard<std::mutex> lock(promiseParamsMutex_);
    promiseParams_[rotatePromiseParam->cmdId] = rotatePromiseParam;
    promise = reinterpret_cast<uintptr_t>(rotatePromiseParam->promise);
}

void AniMechManager::RotateBySpeed(
    int32_t mechId, const RotationSpeedTaihe &speed, int32_t duration, uintptr_t &promise)
{
    HILOGI("start");
    if (CheckDeviceL1()) {
        return;
    }
    if (!IsSystemApp()) {
        ::taihe::set_business_error(MechNapiErrorCode::PERMISSION_DENIED, "Not system application");
        return;
    }
    RotateBySpeedParam rotateBySpeedParam;
    rotateBySpeedParam.duration = static_cast<float>(duration);
    double defValue = 0.0;
    rotateBySpeedParam.speed.yawSpeed =
        static_cast<float>(speed.yawSpeed.has_value()? speed.yawSpeed.value() : defValue);
    rotateBySpeedParam.speed.rollSpeed =
        static_cast<float>(speed.rollSpeed.has_value()? speed.rollSpeed.value() : defValue);
    rotateBySpeedParam.speed.pitchSpeed =
        static_cast<float>(speed.pitchSpeed.has_value()? speed.pitchSpeed.value() : defValue);
    auto rotatePromiseParam = std::make_shared<AniRotatePrimiseFulfillmentParam>();
    if (!InitRotatePrimiseFulfillmentParam(*rotatePromiseParam)) {
        HILOGE("InitRotatePrimiseFulfillmentParam failed;");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    if (!InitMechClient() || !RegisterCmdChannel()) {
        HILOGE("InitMechClient or RegisterCmdChannel failed;");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    int32_t result = mechClient_->RotateBySpeed(mechId, rotatePromiseParam->cmdId, rotateBySpeedParam);
    HILOGI("result code: %{public}d ", result);
    if (result == MechNapiErrorCode::DEVICE_NOT_CONNECTED) {
        ::taihe::set_business_error(MechNapiErrorCode::DEVICE_NOT_CONNECTED, "Device not connected");
        return;
    }
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    std::lock_guard<std::mutex> lock(promiseParamsMutex_);
    promiseParams_[rotatePromiseParam->cmdId] = rotatePromiseParam;
    promise = reinterpret_cast<uintptr_t>(rotatePromiseParam->promise);
}

void AniMechManager::StopMoving(int32_t mechId, uintptr_t &promise)
{
    promise = 0;
    if (CheckDeviceL1()) {
        return;
    }
    if (!IsSystemApp()) {
        ::taihe::set_business_error(MechNapiErrorCode::PERMISSION_DENIED, "Not system application");
        return;
    }
    auto rotatePromiseParam = std::make_shared<AniRotatePrimiseFulfillmentParam>();
    if (!InitRotatePrimiseFulfillmentParam(*rotatePromiseParam, true)) {
        HILOGE("InitRotatePrimiseFulfillmentParam failed;");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    if (!InitMechClient() || !RegisterCmdChannel()) {
        HILOGE("InitMechClient or RegisterCmdChannel failed;");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    int32_t result = mechClient_->StopMoving(mechId, rotatePromiseParam->cmdId);
    HILOGI("result code: %{public}d ", result);
    if (result != ERR_OK) {
        ProcessOnResultCode(result);
        return;
    }
    std::lock_guard<std::mutex> lock(promiseParamsMutex_);
    promiseParams_[rotatePromiseParam->cmdId] = rotatePromiseParam;
    promise = reinterpret_cast<uintptr_t>(rotatePromiseParam->promise);
}

void AniMechManager::GetCurrentAngles(int32_t mechId, EulerAnglesTaihe &out)
{
    if (CheckDeviceL1()) {
        return;
    }
    if (!IsSystemApp()) {
        ::taihe::set_business_error(MechNapiErrorCode::PERMISSION_DENIED, "Not system application");
        return;
    }
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    EulerAngles rAngles;
    int32_t result = mechClient_->GetRotationAngles(mechId, rAngles);
    HILOGE("result: %{public}d;", result);
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    out.yaw = ::taihe::optional<double>(std::in_place, static_cast<double>(rAngles.yaw));
    out.roll = ::taihe::optional<double>(std::in_place, static_cast<double>(rAngles.roll));
    out.pitch = ::taihe::optional<double>(std::in_place, static_cast<double>(rAngles.pitch));
}

void AniMechManager::GetRotationLimits(int32_t mechId, RotationLimitsTaihe &limits)
{
    if (CheckDeviceL1()) {
        return;
    }
    if (!IsSystemApp()) {
        ::taihe::set_business_error(MechNapiErrorCode::PERMISSION_DENIED, "Not system application");
        return;
    }
    RotateDegreeLimit rotateDegreeLimit;
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    int32_t result = mechClient_->GetRotationDegreeLimits(mechId, rotateDegreeLimit);
    HILOGE("result: %{public}d;", result);
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    HILOGI("RotateDegreeLimit query success: %{public}s;", rotateDegreeLimit.ToString().c_str());
    RotationLimitsToAni(rotateDegreeLimit, limits);
}

void AniMechManager::GetRotationAxesStatus(int32_t mechId, RotationAxesStatusTaihe &out)
{
    if (CheckDeviceL1()) {
        return;
    }
    if (!IsSystemApp()) {
        ::taihe::set_business_error(MechNapiErrorCode::PERMISSION_DENIED, "Not system application");
        return;
    }
    RotationAxesStatus axesStatus{};
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    int32_t result = mechClient_->GetRotationAxesStatus(mechId, axesStatus);
    HILOGE("result: %{public}d;", result);
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    RotationAxesStatusToAni(axesStatus, out);
}

void AniMechManager::OnRotationAxesStatusChange(const RotationAxesCBTaihe &callback)
{
    HILOGI("Start to register the callback function.");
    if (CheckControlL1()) {
        return;
    }
    int32_t result = ExecuteOnForRotationAxesStatusChange(callback);
    ProcessOnResultCode(result);
}

void AniMechManager::OffRotationAxesStatusChange(const ::taihe::optional_view<RotationAxesCBTaihe> &callback)
{
    HILOGI("Start to unregister the callback function.");
    if (CheckControlL1()) {
        return;
    }
    int32_t result = ExecuteOffForRotationAxesStatusChange(!callback.has_value(), callback.value());
    ProcessOffResultCode(result);
}

void AniMechManager::SearchTarget(const TargetInfoTaihe &target, const SearchParamsTaihe &params, uintptr_t &promise)
{
    promise = 0;
    if (CheckDeviceL1()) {
        return;
    }
    if (!IsSystemApp()) {
        ::taihe::set_business_error(MechNapiErrorCode::PERMISSION_DENIED, "Not system application");
        return;
    }
    TargetInfo targetInfo;
    targetInfo.targetType = static_cast<TargetType>(target.targetType.get_value());
    SearchParams searchParams;
    searchParams.direction = static_cast<SearchDirection>(params.direction.get_value());
    auto searchTargetPromiseParam = std::make_shared<AniRotatePrimiseFulfillmentParam>();
    if (!InitRotatePrimiseFulfillmentParam(*searchTargetPromiseParam)) {
        HILOGE("InitRotatePrimiseFulfillmentParam failed;");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    if (!InitMechClient() || !RegisterCmdChannel()) {
        HILOGE("InitMechClient or RegisterCmdChannel failed;");
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    int32_t result = mechClient_->SearchTarget(searchTargetPromiseParam->cmdId, targetInfo, searchParams);
    HILOGI("result: %{public}d;", result);
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
    std::lock_guard<std::mutex> lock(searchTargetPromiseParamMutex_);
    searchTargetPromiseParam_[searchTargetPromiseParam->cmdId] = searchTargetPromiseParam;
    promise = reinterpret_cast<uintptr_t>(searchTargetPromiseParam->promise);
}

int32_t AniMechManager::ExecuteOnForAttachStateChange(const AttachStateCBTaihe &callback)
{
    HILOGE("ATTACH_STATE_CHANGE_EVENT");
    int32_t registerResult = ERR_OK;
    if (callback.is_error()) {
        HILOGE("Callback error");
        return MechNapiErrorCode::PARAMETER_CHECK_FAILED;
    }
    std::lock_guard<std::mutex> lock(attachStateChangeCallbackMutex_);
    if (attachStateChangeCallback_.empty()) {
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
    auto it = std::find(attachStateChangeCallback_.begin(), attachStateChangeCallback_.end(), callback);
    if (it != attachStateChangeCallback_.end()) {
        HILOGI("Already exists");
        return registerResult;
    }
    attachStateChangeCallback_.push_back(callback);
    return registerResult;
}

int32_t AniMechManager::ExecuteOnForTrackingEvent(const TrackingEventCBTaihe &callback)
{
    HILOGE("TRACKING_EVENT");
    int32_t registerResult = ERR_OK;
    std::lock_guard<std::mutex> lock(trackingEventCallbackMutex_);
    if (trackingEventCallback_.empty()) {
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
    auto it = std::find(trackingEventCallback_.begin(), trackingEventCallback_.end(), callback);
    if (it != trackingEventCallback_.end()) {
        HILOGI("Already exists");
        return registerResult;
    }
    trackingEventCallback_.push_back(callback);
    return registerResult;
}

int32_t AniMechManager::ExecuteOnForRotationAxesStatusChange(const RotationAxesCBTaihe &callback)
{
    if (!IsSystemApp()) {
        return PERMISSION_DENIED;
    }
    int32_t registerResult = ERR_OK;
    std::lock_guard<std::mutex> lock(rotateAxisStatusChangeCallbackMutex_);
    if (rotateAxisStatusChangeCallback_.empty()) {
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
    auto it = std::find(rotateAxisStatusChangeCallback_.begin(), rotateAxisStatusChangeCallback_.end(), callback);
    if (it != rotateAxisStatusChangeCallback_.end()) {
        HILOGI("Already exists");
        return registerResult;
    }
    rotateAxisStatusChangeCallback_.push_back(callback);
    return registerResult;
}

bool AniMechManager::InitAttachStateChangeStub()
{
    if (attachStateChangeStub_ != nullptr) {
        return true;
    }
    {
        std::lock_guard<std::mutex> lock(attachStateChangeStubMutex_);
        if (attachStateChangeStub_ != nullptr) {
            return true;
        }
        sptr<AniMechManagerStub> stub = new (std::nothrow) AniMechManagerStub();
        sptr <IRemoteObject::DeathRecipient> deathListener =
            new (std::nothrow) AniAttachStateChangeStubDeathListener();
        if (stub == nullptr || deathListener == nullptr) {
            HILOGE("stub or deathListener is null.");
            return false;
        }
        stub->SetDeathRecipient(deathListener);
        attachStateChangeStub_ = stub;
        return attachStateChangeStub_ != nullptr;
    }
}

bool AniMechManager::InitTrackingEventStub()
{
    if (trackingEventStub_ != nullptr) {
        return true;
    }
    {
        std::lock_guard<std::mutex> lock(trackingEventStubMutex_);
        if (trackingEventStub_ != nullptr) {
            return true;
        }
        sptr<AniMechManagerStub> stub = new (std::nothrow) AniMechManagerStub();
        sptr<IRemoteObject::DeathRecipient> deathListener = new (std::nothrow) AniTrackingEventStubDeathListener();
        if (stub == nullptr || deathListener == nullptr) {
            HILOGE("stub or deathListener is null.");
            return false;
        }
        stub->SetDeathRecipient(deathListener);
        trackingEventStub_ = stub;
        return trackingEventStub_ != nullptr;
    }
}

bool AniMechManager::InitRotationAxesStatusChangeStub()
{
    if (rotationAxesStatusChangeStub_ != nullptr) {
        return true;
    }
    {
        std::lock_guard<std::mutex> lock(rotationAxesStatusChangeMutex_);
        if (rotationAxesStatusChangeStub_ != nullptr) {
            return true;
        }
        sptr<AniMechManagerStub> stub = new (std::nothrow) AniMechManagerStub();
        sptr<IRemoteObject::DeathRecipient> deathListener =
            new  (std::nothrow) AniRotationAxesStatusChangeStubDeathListener();
        if (stub == nullptr || deathListener == nullptr) {
            HILOGE("stub or deathListener is null.");
            return false;
        }
        stub->SetDeathRecipient(deathListener);
        rotationAxesStatusChangeStub_ = stub;
        return rotationAxesStatusChangeStub_ != nullptr;
    }
}

int32_t AniMechManager::ExecuteOffForAttachStateChange(bool isNull, const AttachStateCBTaihe &callback)
{
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        return MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY;
    }
    std::lock_guard<std::mutex> lock(attachStateChangeCallbackMutex_);
    if (isNull) {
        HILOGI("Cancel all registrations");
        mechClient_->AttachStateChangeListenOff();
        attachStateChangeCallback_.clear();
        return ERR_OK;
    }
    auto it = std::find(attachStateChangeCallback_.begin(), attachStateChangeCallback_.end(), callback);
    if (it == attachStateChangeCallback_.end()) {
        HILOGI("No found callback info.");
        return ERR_OK;
    }
    attachStateChangeCallback_.erase(it);
    if (attachStateChangeCallback_.size() <=0) {
        mechClient_->AttachStateChangeListenOff();
        return ERR_OK;
    }
    return ERR_OK;
}

int32_t AniMechManager::ExecuteOffForTrackingEvent(bool isNull, const TrackingEventCBTaihe &callback)
{
    HILOGE("TRACKING_EVENT");
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        return MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY;
    }
    std::lock_guard<std::mutex> lock(trackingEventCallbackMutex_);
    if (isNull) {
        HILOGI("Cancel all registrations");
        mechClient_->TrackingEventListenOff();
        trackingEventCallback_.clear();
        return ERR_OK;
    }
    auto it = std::find(trackingEventCallback_.begin(), trackingEventCallback_.end(), callback);
    if (it == trackingEventCallback_.end()) {
        HILOGI("No found callback info.");
        return ERR_OK;
    }
    trackingEventCallback_.erase(it);
    if (trackingEventCallback_.size() <=0) {
        mechClient_->TrackingEventListenOff();
        return ERR_OK;
    }
    return ERR_OK;
}

int32_t AniMechManager::ExecuteOffForRotationAxesStatusChange(bool isNull, const RotationAxesCBTaihe &callback)
{
    HILOGE("ROTATE_AXIS_STATUS_CHANGE_EVENT");
    if (!InitMechClient()) {
        HILOGE("Init Mech Client failed.");
        return MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY;
    }
    if (!IsSystemApp()) {
        return PERMISSION_DENIED;
    }
    std::lock_guard<std::mutex> lock(rotateAxisStatusChangeCallbackMutex_);
    if (isNull) {
        HILOGI("Cancel all registrations");
        mechClient_->RotationAxesStatusChangeListenOff();
        rotateAxisStatusChangeCallback_.clear();
        return ERR_OK;
    }
    auto it = std::find(rotateAxisStatusChangeCallback_.begin(), rotateAxisStatusChangeCallback_.end(), callback);
    if (it == rotateAxisStatusChangeCallback_.end()) {
        HILOGI("No found callback info.");
        return ERR_OK;
    }
    rotateAxisStatusChangeCallback_.erase(it);
    if (rotateAxisStatusChangeCallback_.size() <=0) {
        mechClient_->RotationAxesStatusChangeListenOff();
        return ERR_OK;
    }
    return ERR_OK;
}

void AniMechManager::ProcessOffResultCode(int32_t &result)
{
    HILOGE("UnRegister the callback function result code: %{public}d ", result);
    if (result == MechNapiErrorCode::PARAMETER_CHECK_FAILED) {
        ::taihe::set_business_error(MechNapiErrorCode::PARAMETER_CHECK_FAILED, "Invalid event type.");
        return;
    }
    if (result != ERR_OK) {
        ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
        return;
    }
}

bool AniMechManager::IsSystemApp()
{
    static bool isSystemApp = []() {
        uint64_t tokenId = OHOS::IPCSkeleton::GetSelfTokenID();
        return OHOS::Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(tokenId);
    }();
    return isSystemApp;
}

bool AniMechManager::InitMechClient()
{
    if (mechClient_ != nullptr) {
        return true;
    }
    {
        std::lock_guard<std::mutex> lock(mechClientMutex_);
        if (mechClient_ == nullptr) {
            mechClient_ = std::make_shared<AniMechClient>();
        }
    }
    return mechClient_ != nullptr;
}

std::string AniMechManager::GenerateUniqueID()
{
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::mt19937 generator(now_c);
    std::uniform_int_distribution<int> distribution(0, FIFTEEN);
    std::uniform_int_distribution<int> distribution2(EIGHT, ELEVEN);
    std::stringstream ss;
    ss << std::hex << std::uppercase;
    for (int i = 0; i < THIRTY_TWO; ++i) {
        if (i == EIGHT || i == THIRTEEN || i == EIGHTEEN || i == TWENTY_THREE) {
            ss << "-";
        }
        if (i == THIRTEEN) {
            ss << distribution2(generator);
        } else {
            ss << distribution(generator);
        }
    }
    return ss.str();
}

bool AniMechManager::InitRotatePrimiseFulfillmentParam(AniRotatePrimiseFulfillmentParam &param, bool retVoid)
{
    HILOGD("InitRotatePrimiseFulfillmentParam enter!");
    ani_env *env = taihe::get_env();
    if (env == nullptr) {
        HILOGE("env is null!");
        return false;
    }
    ani_status status = ANI_OK;
    ani_object promise;
    ani_resolver deferred = nullptr;
    ani_vm *vm = nullptr;
    status = env->GetVM(&vm);
    if (status != ANI_OK) {
        HILOGE("GetVM faild,status:%{public}d", status);
        return false;
    }
    if ((status = env->Promise_New(&deferred, &promise)) != ANI_OK) {
        HILOGE("create promise object failed,status:%{public}d", status);
        return false;
    }
    param.cmdId = GenerateUniqueID();
    param.vm = vm;
    param.promise = promise;
    param.env = env;
    param.deferred = deferred;
    param.retVoid = retVoid;
    return true;
}

bool AniMechManager::AniSendEvent(std::function<void()> task)
{
    HILOGD("AniSendEvent enter!");
    if (task == nullptr) {
        HILOGD("Task is null");
        return false;
    }
    std::lock_guard<std::mutex> lock(mainHandlerMutex_);
    if (!mainHandler_) {
        auto runner = AppExecFwk::EventRunner::GetMainEventRunner();
        if (!runner) {
            HILOGD("Runner create failed ");
            return false;
        }
        mainHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    }
    if (mainHandler_ == nullptr) {
        HILOGD("mainHandler_ is null");
        return false;
    }
    mainHandler_->PostTask(std::move(task));
    return true;
}

int32_t AniMechManager::CheckControlL1()
{
#ifdef MECHBODY_CONTROLLER_L1
    HILOGI("Device not support.");
    ::taihe::set_business_error(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY, "System exception");
    return static_cast<int32_t>(MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY);
#else
    return 0;
#endif
}

int32_t AniMechManager::CheckDeviceL1()
{
#ifdef MECHBODY_CONTROLLER_L1
    HILOGI("Device not support.");
    ::taihe::set_business_error(MechNapiErrorCode::DEVICE_NOT_CONNECTED, "Device not connected");
    return static_cast<int32_t>(MechNapiErrorCode::DEVICE_NOT_CONNECTED);
#else
    return 0;
#endif
}

bool AniMechManager::PreCheck()
{
    if (CheckControlL1()) {
        return false;
    }
    if (!IsSystemApp()) {
        ::taihe::set_business_error(MechNapiErrorCode::PERMISSION_DENIED, "Not system application");
        return false;
    }
    return true;
}

bool AniMechManager::RegisterCmdChannel()
{
    if (cmdChannel_ != nullptr) {
        return true;
    }
    {
        std::lock_guard<std::mutex> lock(cmdChannelMutex_);
        if (cmdChannel_ != nullptr) {
            return true;
        }
        if (!InitMechClient()) {
            return false;
        }
        sptr<AniMechManagerStub> stub = new  (std::nothrow) AniMechManagerStub();
        sptr<IRemoteObject::DeathRecipient> deathListener = new (std::nothrow) AniCmdChannelDeathListener();
        if (stub == nullptr ||  deathListener == nullptr) {
            HILOGE("stub or deathListener is null");
            return false;
        }
        stub->SetDeathRecipient(deathListener);
        int32_t result = mechClient_->RegisterCmdChannel(stub);
        if (result == ERR_OK) {
            cmdChannel_ = stub;
            return true;
        }
    }
    return false;
}

int32_t AniMechManager::AttachStateChangeCallback(const AttachmentState &attachmentState,
    const std::shared_ptr<MechInfo> &mechInfo)
{
    CHECK_POINTER_RETURN_VALUE(mechInfo, INVALID_PARAMETERS_ERR, "mechInfo");
    std::lock_guard<std::mutex> lock(attachStateChangeCallbackMutex_);
    for (const auto &item: attachStateChangeCallback_) {
        auto task = [item, attachmentState, mechInfo]() {
            AttachStateChangeInfoTaihe taiheItem {
                .state = AttachStateTaihe::from_value(static_cast<int32_t>(attachmentState)),
                .mechInfo = {
                    .mechId = mechInfo->mechId,
                    .mechDeviceType = MechDeviceTypeTaihe::from_value(static_cast<int32_t>(mechInfo->mechType)),
                    .mechName = mechInfo->mechName,
                }
            };
            item(taiheItem);
        };
        AniSendEvent(task);
    }
    HILOGI("end");
    return ERR_OK;
}

int32_t AniMechManager::TrackingEventCallback(const int32_t &mechId,
    const TrackingEvent &trackingEvent)
{
    HILOGI("start. mechId: %{public}d, TrackingEvent: %{public}d", mechId, static_cast<int32_t>(trackingEvent));
    std::lock_guard<std::mutex> lock(trackingEventCallbackMutex_);
    for (const auto &item: trackingEventCallback_) {
        auto task = [item, mechId, trackingEvent]() {
            TrackingEventInfoTaihe taiheItem {
                .event = TrackingEventTaihe::from_value(static_cast<int32_t>(trackingEvent))
            };
            item(taiheItem);
        };
        AniSendEvent(task);
    }
    HILOGI("end");
    return ERR_OK;
}

int32_t AniMechManager::RotationAxesStatusChangeCallback(const int32_t &mechId,
    const std::shared_ptr<RotationAxesStatus> &rotationAxesStatus)
{
    CHECK_POINTER_RETURN_VALUE(rotationAxesStatus, INVALID_PARAMETERS_ERR, "rotationAxesStatus");
    std::lock_guard<std::mutex> lock(rotateAxisStatusChangeCallbackMutex_);
    for (const auto &item: rotateAxisStatusChangeCallback_) {
        auto task = [this, item, mechId, rotationAxesStatus]() {
            RotationAxesStatusTaihe axesStatus;
            RotationAxesStatusToAni(*rotationAxesStatus, axesStatus);
            RotationAxesStateChangeInfoTaihe taiheItem {
                .mechId = mechId,
                .status = axesStatus,
            };
            item(taiheItem);
        };
        AniSendEvent(task);
    }
    HILOGI("end");
    return ERR_OK;
}

ani_object AniMechManager::CreateInt(ani_env *env, int32_t num)
{
    ani_class cls {};
    ani_object obj {};
    if (env == nullptr) {
        HILOGE("env or deferred is null!");
        return nullptr;
    }
    if (env->FindClass(CLASS_NAME_INT, &cls) != ANI_OK) {
        HILOGE("find class %{public}s failed", CLASS_NAME_INT);
        return nullptr;
    }
    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "i:", &ctor)) {
        HILOGE("Find method '<ctor>' failed");
        return nullptr;
    }
    if (ANI_OK != env->Object_New(cls, ctor, &obj, static_cast<ani_int>(num))) {
        HILOGE("Find method '<ctor>' failed");
        return nullptr;
    }
    return obj;
}

void AniMechManager::VoidPromise(ani_env *env, ani_resolver deferred)
{
    if (env == nullptr || deferred == nullptr) {
        HILOGE("env or deferred is null!");
        return;
    }
    ani_status status = ANI_OK;
    ani_ref promiseResult;
    if ((status = env->GetUndefined(&promiseResult)) != ANI_OK) {
        HILOGE("get undefined value failed, status = %{public}d", status);
        return;
    }
    if (promiseResult == nullptr) {
        HILOGE("promiseResult is null!");
        return;
    }
    if ((status = env->PromiseResolver_Resolve(deferred, promiseResult)) != ANI_OK) {
        HILOGE("PromiseResolver_Resolve failed, status = %{public}d", status);
        return;
    }
    return;
}

ani_enum_item AniMechManager::GetResultAni(ani_env *env, int32_t idx)
{
    HILOGE("begin");
    if (env == nullptr) {
        HILOGE("env is null!");
        return nullptr;
    }
    ani_enum enumType;
    ani_status status = ANI_OK;
    status = env->FindEnum("L@ohos/distributedHardware/mechanicManager/mechanicManager/Result;", &enumType);
    if (status != ANI_OK) {
        HILOGE("FindEnum error:%{public}d!", status);
        return nullptr;
    }
    ani_enum_item enumItem;
    status = env->Enum_GetEnumItemByIndex(enumType, ani_size(idx), &enumItem);
    if (status != ANI_OK) {
        HILOGE("Enum_GetEnumItemByIndex err:%{public}d!", status);
        return nullptr;
    }
    return enumItem;
}

void AniMechManager::ResultTaihePromise(ani_env *env, ani_resolver deferred, const int32_t &result)
{
    HILOGD("begin!");
    if (env == nullptr || deferred == nullptr) {
        HILOGE("env or deferred is null!");
        return;
    }
    ani_status status = ANI_OK;
    ResultTaihe info = ResultTaihe::from_value(result);
    auto idx =  static_cast<int32_t>(info.get_key());
    if (idx < 0) {
        HILOGE("PromiseResolver_Resolve failed, status = %{public}d", status);
        return;
    }
    auto promiseResult = GetResultAni(env, idx);
    if (promiseResult == nullptr) {
        HILOGE("promiseResult is null!");
        return;
    }
    if ((status = env->PromiseResolver_Resolve(deferred, promiseResult)) != ANI_OK) {
        HILOGE("PromiseResolver_Resolve failed, status = %{public}d", status);
        return;
    }
    return;
}

void AniMechManager::SearchResultTaihePromise(ani_env *env, ani_resolver deferred, const int32_t &targetsNum)
{
    if (env == nullptr || deferred == nullptr) {
        HILOGE("env or deferred is null!");
        return;
    }
    ani_status status = ANI_OK;
    SearchResultTaihe taiheItem {
        .targetCount = targetsNum
    };
    auto promiseResult = ::taihe::into_ani<SearchResultTaihe>(env, taiheItem);
    if (promiseResult == nullptr) {
        HILOGE("promiseResult is null!");
        return;
    }
    if ((status = env->PromiseResolver_Resolve(deferred, promiseResult)) != ANI_OK) {
        HILOGE("PromiseResolver_Resolve failed, status = %{public}d", status);
        return;
    }
    return;
}

int32_t AniMechManager::RotatePromiseFulfillment(const std::string &cmdId,
    const int32_t &result)
{
    HILOGI("AniRotatePrimiseFulfillmentParam cmdId: %{public}s", cmdId.c_str());
    std::shared_ptr<AniRotatePrimiseFulfillmentParam> param = nullptr;
    {
        std::lock_guard<std::mutex> lock(promiseParamsMutex_);
        auto it = promiseParams_.find(cmdId);
        if (it == promiseParams_.end()) {
            HILOGI("Not found, cmdId: %{public}s", cmdId.c_str());
            return ERR_OK;
        }
        param = it->second;
        if (param == nullptr) {
            HILOGI("param is nullptr, cmdId: %{public}s", cmdId.c_str());
            return ERR_OK;
        }
    }
   
    auto task = [this, param, result]() {
        if (param == nullptr || param->vm == nullptr) {
            HILOGE("param or etsVm is null!");
            return;
        }
        auto etsVm = param->vm;
        ani_env* etsEnv = nullptr;
        ani_options aniArgs { 0, nullptr };
        ani_status aniResult = etsVm->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &etsEnv);
        if (aniResult != ANI_OK) {
            HILOGE("AttachCurrentThread error!");
            return;
        }
        if (param->retVoid) {
            VoidPromise(etsEnv, param->deferred);
        } else {
            ResultTaihePromise(etsEnv, param->deferred, result);
        }
        if ((aniResult = etsVm->DetachCurrentThread()) != ANI_OK) {
            HILOGE("DetachCurrentThread error!");
            return;
        }
    };
    task();
    {
        std::lock_guard<std::mutex> lock(promiseParamsMutex_);
        promiseParams_.erase(param->cmdId);
    }
    return ERR_OK;
}

int32_t AniMechManager::SearchTargetCallback(std::string &cmdId, const int32_t &targetsNum, const int32_t &result)
{
    HILOGI("cmdId: %{public}s; targetCount: %{public}d", cmdId.c_str(), targetsNum);
    std::shared_ptr<AniRotatePrimiseFulfillmentParam> param = nullptr;
    {
        std::lock_guard<std::mutex> lock(searchTargetPromiseParamMutex_);
        auto it = searchTargetPromiseParam_.find(cmdId);
        if (it == searchTargetPromiseParam_.end()) {
            HILOGE("searchTarget Callback is nullptr, cmdId: %{public}s", cmdId.c_str());
            return ERR_OK;
        }
        param = it->second;
        if (param == nullptr) {
            HILOGI("param is nullptr, cmdId: %{public}s", cmdId.c_str());
            return ERR_OK;
        }
    }
    
    auto task = [this, param, targetsNum]() {
        if (param == nullptr || param->vm == nullptr) {
            HILOGE("param or etsVm is null!");
            return;
        }
        auto etsVm = param->vm;
        ani_env* etsEnv = nullptr;
        ani_options aniArgs { 0, nullptr };
        ani_status aniResult = etsVm->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &etsEnv);
        if (aniResult != ANI_OK) {
            HILOGE("AttachCurrentThread error!");
            return;
        }
        SearchResultTaihePromise(etsEnv, param->deferred, targetsNum);
        if ((aniResult = etsVm->DetachCurrentThread()) != ANI_OK) {
            HILOGE("DetachCurrentThread error!");
            return;
        }
    };
    task();
    {
        std::lock_guard<std::mutex> lock(searchTargetPromiseParamMutex_);
        searchTargetPromiseParam_.erase(param->cmdId);
    }
    HILOGI("end");
    return ERR_OK;
}

void AniMechManager::AniAttachStateChangeStubDeathListener::OnRemoteDied(const wptr <IRemoteObject> &object)
{
    AniMechManager::GetInstance().OnAttachStateChangeRemoteDied(object);
}

void AniMechManager::AniTrackingEventStubDeathListener::OnRemoteDied(const wptr <IRemoteObject> &object)
{
    AniMechManager::GetInstance().OnTrackingEventRemoteDied(object);
}

void AniMechManager::AniRotationAxesStatusChangeStubDeathListener::OnRemoteDied(const wptr <IRemoteObject> &object)
{
    AniMechManager::GetInstance().OnRotationAxesStatusChangeRemoteDied(object);
}

void AniMechManager::AniCmdChannelDeathListener::OnRemoteDied(const wptr <IRemoteObject> &object)
{
    AniMechManager::GetInstance().OnCmdChannelRemoteDied(object);
}

void AniMechManager::OnAttachStateChangeRemoteDied(const wptr <IRemoteObject> &object)
{
    HILOGE("AttachStateChangeStub RemoteObject dead; ");
    std::lock_guard<std::mutex> lock(attachStateChangeStubMutex_);
    attachStateChangeStub_ = nullptr;
}

void AniMechManager::OnTrackingEventRemoteDied(const wptr <IRemoteObject> &object)
{
    HILOGE("TrackingEventStub RemoteObject dead; ");
    std::lock_guard<std::mutex> lock(trackingEventStubMutex_);
    trackingEventStub_ = nullptr;
}

void AniMechManager::OnRotationAxesStatusChangeRemoteDied(const wptr <IRemoteObject> &object)
{
    HILOGE("RotationAxesStatusChangeStub RemoteObject dead; ");
    std::lock_guard<std::mutex> lock(rotationAxesStatusChangeMutex_);
    rotationAxesStatusChangeStub_ = nullptr;
}

void AniMechManager::OnCmdChannelRemoteDied(const wptr <IRemoteObject> &object)
{
    HILOGE("CmdChannel RemoteObject dead; ");
    std::lock_guard<std::mutex> lock(cmdChannelMutex_);
    cmdChannel_ = nullptr;
}
} // namespace MechBodyController
} // namespace OHOS