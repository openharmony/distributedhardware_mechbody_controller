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

#include "mc_controller_ipc_death_listener.h"
#include "mc_controller_manager.h"
#include "mechbody_controller_service.h"
#include "mc_camera_tracking_controller.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "MechControllerIpcDeathListener";
}

void MechControllerIpcDeathListener::OnRemoteDied(const wptr <IRemoteObject> &object)
{
    HILOGE("IRemoteObject dead; ");
    if(RemoteObjectType::DEVICE_ATTACH_CALLBACK == objectType_){
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().deviceAttachCallbackMutex);
        MechBodyControllerService::GetInstance().deviceAttachCallback_.erase(tokenId_);
        return;
    }

    if(RemoteObjectType::TRACKING_EVENT_CALLBACK == objectType_){
        std::lock_guard<std::mutex> lock(McCameraTrackingController::GetInstance().trackingEventCallbackMutex_);
        McCameraTrackingController::GetInstance().trackingEventCallback_.erase(tokenId_);
        return;
    }

    if(RemoteObjectType::ROTATION_AXES_STATUS_CALLBACK == objectType_){
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().AxesStatusChangeCallbackMutex_);
        MechBodyControllerService::GetInstance().rotationAxesStatusChangeCallback_.erase(tokenId_);
        return;
    }

    if(RemoteObjectType::COMMAND_CHANNEL == objectType_){
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().cmdChannelMutex_);
        MechBodyControllerService::GetInstance().cmdChannels_.erase(tokenId_);
        return;
    }

}
}
}
