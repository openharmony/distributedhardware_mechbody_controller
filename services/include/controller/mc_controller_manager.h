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

#ifndef MC_CONTROLLER_MANAGER_H
#define MC_CONTROLLER_MANAGER_H

#include <map>
#include <mutex>
#include <vector>
#include <memory>
#include "mechbody_controller_ipc_interface_code.h"
#include "mechbody_controller_log.h"
#include "mechbody_controller_types.h"
#include "mc_camera_tracking_controller.h"
#include "mc_motion_manager.h"
#include "mc_connect_manager.h"

namespace OHOS {
namespace MechBodyController {

class MechConnectListenerImpl : public IMechConnectListener {
public:
    void OnMechConnect(const MechInfo& mechInfo) override;
    void OnMechDisconnect(const MechInfo& mechInfo) override;
};

class McControllerManager {
public:
    static McControllerManager& GetInstance();
private:
    McControllerManager(const McControllerManager&) = delete;
    McControllerManager& operator= (const McControllerManager&) = delete;
    McControllerManager(McControllerManager&&) = delete;
    McControllerManager& operator= (McControllerManager&&) = delete;

public:
    McControllerManager();
    ~McControllerManager();

    int32_t SetTrackingEnabled(const uint32_t &tokenId, bool &isEnabled);
    int32_t GetTrackingEnabled(const uint32_t &tokenId, bool &isEnabled);
    int32_t RegisterTrackingEventCallback(const uint32_t &tokenId, sptr<IRemoteObject> callback);
    int32_t UnRegisterTrackingEventCallback(const uint32_t &tokenId);
    int32_t OnTrackingEvent(const int32_t &mechId, const TrackingEvent &event);
    int32_t SetTrackingLayout(const uint32_t &tokenId, CameraTrackingLayout &cameraTrackingLayout);
    int32_t GetTrackingLayout(const uint32_t &tokenId, CameraTrackingLayout &cameraTrackingLayoutb);
    int32_t SearchTarget(std::string &cmdId, uint32_t &tokenId,
        const std::shared_ptr<TargetInfo> &targetInfo, const std::shared_ptr<SearchParams> &searchParams);

private:
    std::shared_ptr<IMechConnectListener> listener_ = nullptr;
    std::mutex userSettingsMutex_;
};

} // namespace MechBodyController
} // namespace OHOS
#endif