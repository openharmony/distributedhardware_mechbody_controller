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

#include "mc_controller_manager.h"
#include "mc_camera_tracking_controller.h"
#include "mc_controller_ipc_death_listener.h"
#include "mechbody_controller_service.h"
#include "mechbody_controller_utils.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "McControllerManager";
}

McControllerManager& McControllerManager::GetInstance()
{
    static auto instance = new McControllerManager();
    return *instance;
}

McControllerManager::McControllerManager() {}

int32_t McControllerManager::SetTrackingEnabled(const uint32_t &tokenId, bool &isEnabled)
{
    HILOGI("start, tokenId: %{public}s; isEnabled: %{public}s;",
        GetAnonymUint32(tokenId).c_str(), isEnabled ? "true" : "false");
    int32_t setResult = McCameraTrackingController::GetInstance().SetTrackingEnabled(tokenId, isEnabled);
    HILOGI("end. Tracking Enabled: %{public}s; Set result code: %{public}d", isEnabled ? "true" : "false", setResult);
    return setResult;
}

int32_t McControllerManager::GetTrackingEnabled(const uint32_t &tokenId, bool &isEnabled)
{
    HILOGI("start, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    int32_t getResult = McCameraTrackingController::GetInstance().GetTrackingEnabled(tokenId, isEnabled);
    HILOGI("end. Tracking Enabled: %{public}s; Get result code: %{public}d", isEnabled ? "true" : "false", getResult);
    return ERR_OK;
}

int32_t McControllerManager::RegisterTrackingEventCallback(const uint32_t &tokenId, sptr <IRemoteObject> callback)
{
    HILOGI("start, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    int32_t registerResult = McCameraTrackingController::GetInstance().RegisterTrackingEventCallback(tokenId, callback);
    HILOGI("end. result code: %{public}d.", registerResult);
    return registerResult;
}

int32_t McControllerManager::UnRegisterTrackingEventCallback(const uint32_t &tokenId)
{
    HILOGI("start, tokenId: %{public}s;", GetAnonymUint32(tokenId).c_str());
    int32_t unregisterResult = McCameraTrackingController::GetInstance().UnRegisterTrackingEventCallback(
        tokenId);
    HILOGI("end. result code: %{public}d.", unregisterResult);
    return unregisterResult;
}

int32_t McControllerManager::OnTrackingEvent(const int32_t &mechId, const TrackingEvent &event)
{
    return McCameraTrackingController::GetInstance().OnTrackingEvent(mechId, event);
}

int32_t McControllerManager::SetTrackingLayout(const uint32_t &tokenId,
    CameraTrackingLayout &cameraTrackingLayout)
{
    return McCameraTrackingController::GetInstance().SetTrackingLayout(cameraTrackingLayout);
}

int32_t McControllerManager::GetTrackingLayout(const uint32_t &tokenId, CameraTrackingLayout &cameraTrackingLayout)
{
    return McCameraTrackingController::GetInstance().GetTrackingLayout(cameraTrackingLayout);
}

int32_t McControllerManager::SearchTarget(std::string &cmdId, uint32_t &tokenId,
    const std::shared_ptr<TargetInfo> &targetInfo, const std::shared_ptr<SearchParams> &searchParams)
{
    return McCameraTrackingController::GetInstance().SearchTarget(cmdId, tokenId, targetInfo, searchParams);
}

void MechConnectListenerImpl::OnMechConnect(const MechInfo &mechInfo)
{
    MechBodyControllerService::GetInstance().OnAttachStateChange(AttachmentState::ATTACHED, mechInfo);
}

void MechConnectListenerImpl::OnMechDisconnect(const MechInfo &mechInfo)
{
    MechBodyControllerService::GetInstance().OnAttachStateChange(AttachmentState::DETACHED, mechInfo);
}
}
}