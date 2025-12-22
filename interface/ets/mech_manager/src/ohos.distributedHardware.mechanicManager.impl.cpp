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


#include "ani_mech_manager.h"

using namespace OHOS::MechBodyController;
namespace {
// To be implemented.

void OnAttachStateChangeInner(::taihe::callback_view<ACH_STATE_CHANGE_INFO_CB> callback)
{
    AniMechManager::GetInstance().OnAttachStateChange(callback);
}

void OffAttachStateChangeInner(::taihe::optional_view<AttachStateCBTaihe> callback)
{
    AniMechManager::GetInstance().OffAttachStateChange(callback);
}

::taihe::array<MechInfoTaihe> GetAttachedMechDevicesInner()
{
    return AniMechManager::GetInstance().GetAttachedDevices();
}

void SetUserOperationInner(OperationTaihe operation, ::taihe::string_view mac, ::taihe::string_view params)
{
    std::string inMac(mac);
    std::string inParams(params);
    AniMechManager::GetInstance().SetUserOperation(operation, inMac, inParams);
}

void SetCameraTrackingEnabledInner(bool isEnabled)
{
    AniMechManager::GetInstance().SetCameraTrackingEnabled(isEnabled);
}

bool GetCameraTrackingEnabledInner()
{
    bool isEnabled = false;
    AniMechManager::GetInstance().GetCameraTrackingEnabled(isEnabled);
    return isEnabled;
}

void OnTrackingStateChangeInner(::taihe::callback_view<TRACKING_EVENT_INFO_CB> callback)
{
    AniMechManager::GetInstance().OnTrackingStateChange(callback);
}

void OffTrackingStateChangeInner(::taihe::optional_view<TrackingEventCBTaihe> callback)
{
    AniMechManager::GetInstance().OffTrackingStateChange(callback);
}

void SetCameraTrackingLayoutInner(CameraTrackingLayoutTaihe trackingLayout)
{
    AniMechManager::GetInstance().SetCameraTrackingLayout(trackingLayout);
}

CameraTrackingLayoutTaihe GetCameraTrackingLayoutInner()
{
    CameraTrackingLayoutTaihe result = CameraTrackingLayoutTaihe::key_t::DEFAULT;
    AniMechManager::GetInstance().GetCameraTrackingLayout(result);
    return result;
}

uintptr_t RotateInner(int32_t mechId, RotationAnglesTaihe const& angles, int32_t duration)
{
    uintptr_t promise = 0;
    AniMechManager::GetInstance().Rotate(mechId, angles, duration, promise);
    return promise;
}

uintptr_t RotateToEulerAnglesInner(int32_t mechId, EulerAnglesTaihe const& angles, int32_t duration)
{
    uintptr_t promise = 0;
    AniMechManager::GetInstance().RotateToEulerAngles(mechId, angles, duration, promise);
    return promise;
}

int32_t GetMaxRotationTimeInner(int32_t mechId)
{
    int32_t maxTime = 0;
    AniMechManager::GetInstance().GetMaxRotationTime(mechId, maxTime);
    return maxTime;
}

RotationSpeedTaihe GetMaxRotationSpeedInner(int32_t mechId)
{
    RotationSpeedTaihe result;
    AniMechManager::GetInstance().GetMaxRotationSpeed(mechId, result);
    return result;
}

uintptr_t RotateBySpeedInner(int32_t mechId, RotationSpeedTaihe const& speed, int32_t duration)
{
    uintptr_t promise = 0;
    AniMechManager::GetInstance().RotateBySpeed(mechId, speed, duration, promise);
    return promise;
}

uintptr_t StopMovingInner(int32_t mechId)
{
    uintptr_t promise = 0;
    AniMechManager::GetInstance().StopMoving(mechId, promise);
    return promise;
}

EulerAnglesTaihe GetCurrentAnglesInner(int32_t mechId)
{
    EulerAnglesTaihe result;
    AniMechManager::GetInstance().GetCurrentAngles(mechId, result);
    return result;
}

RotationLimitsTaihe GetRotationLimitsInner(int32_t mechId)
{
    RotationLimitsTaihe result;
    AniMechManager::GetInstance().GetRotationLimits(mechId, result);
    return result;
}

RotationAxesStatusTaihe GetRotationAxesStatusInner(int32_t mechId)
{
    RotationAxesStatusTaihe result;
    AniMechManager::GetInstance().GetRotationAxesStatus(mechId, result);
    return result;
}

void OnRotationAxesStatusChangeInner(::taihe::callback_view<ROT_AXES_STATE_CHANGE_INFO_CB> callback)
{
    AniMechManager::GetInstance().OnRotationAxesStatusChange(callback);
}

void OffRotationAxesStatusChangeInner(
    ::taihe::optional_view<RotationAxesCBTaihe> callback)
{
    AniMechManager::GetInstance().OffRotationAxesStatusChange(callback);
}

uintptr_t SearchTargetInner(TargetInfoTaihe const& target, SearchParamsTaihe const& params)
{
    uintptr_t promise = 0;
    AniMechManager::GetInstance().SearchTarget(target, params, promise);
    return promise;
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_OnAttachStateChangeInner(OnAttachStateChangeInner);
TH_EXPORT_CPP_API_OffAttachStateChangeInner(OffAttachStateChangeInner);
TH_EXPORT_CPP_API_GetAttachedMechDevicesInner(GetAttachedMechDevicesInner);
TH_EXPORT_CPP_API_SetUserOperationInner(SetUserOperationInner);
TH_EXPORT_CPP_API_SetCameraTrackingEnabledInner(SetCameraTrackingEnabledInner);
TH_EXPORT_CPP_API_GetCameraTrackingEnabledInner(GetCameraTrackingEnabledInner);
TH_EXPORT_CPP_API_OnTrackingStateChangeInner(OnTrackingStateChangeInner);
TH_EXPORT_CPP_API_OffTrackingStateChangeInner(OffTrackingStateChangeInner);
TH_EXPORT_CPP_API_SetCameraTrackingLayoutInner(SetCameraTrackingLayoutInner);
TH_EXPORT_CPP_API_GetCameraTrackingLayoutInner(GetCameraTrackingLayoutInner);
TH_EXPORT_CPP_API_RotateInner(RotateInner);
TH_EXPORT_CPP_API_RotateToEulerAnglesInner(RotateToEulerAnglesInner);
TH_EXPORT_CPP_API_GetMaxRotationTimeInner(GetMaxRotationTimeInner);
TH_EXPORT_CPP_API_GetMaxRotationSpeedInner(GetMaxRotationSpeedInner);
TH_EXPORT_CPP_API_RotateBySpeedInner(RotateBySpeedInner);
TH_EXPORT_CPP_API_StopMovingInner(StopMovingInner);
TH_EXPORT_CPP_API_GetCurrentAnglesInner(GetCurrentAnglesInner);
TH_EXPORT_CPP_API_GetRotationLimitsInner(GetRotationLimitsInner);
TH_EXPORT_CPP_API_GetRotationAxesStatusInner(GetRotationAxesStatusInner);
TH_EXPORT_CPP_API_OnRotationAxesStatusChangeInner(OnRotationAxesStatusChangeInner);
TH_EXPORT_CPP_API_OffRotationAxesStatusChangeInner(OffRotationAxesStatusChangeInner);
TH_EXPORT_CPP_API_SearchTargetInner(SearchTargetInner);
// NOLINTEND

