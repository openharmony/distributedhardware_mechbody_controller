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

#include "mc_connect_manager.h"

#include <chrono>
#include <functional>
#include <thread>
#include <unistd.h>

#include "event_runner.h"
#include "mc_camera_tracking_controller.h"
#include "mechbody_controller_service.h"

namespace OHOS {
namespace MechBodyController {
MechConnectManager& MechConnectManager::GetInstance()
{
    static auto instance = new MechConnectManager();
    return *instance;
}

namespace {
const std::string TAG = "MechConnectManager";
}

void MechConnectManager::Init()
{
    HILOGI("called");
    auto runner = AppExecFwk::EventRunner::Create("dmsDnetwork");
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
}

void MechConnectManager::UnInit()
{
    HILOGI("called");
    if (eventHandler_ != nullptr) {
        eventHandler_->GetEventRunner()->Stop();
    } else {
        HILOGE("eventHandler_ is nullptr");
    }
}

int32_t MechConnectManager::NotifyMechConnect(MechInfo& mechInfo)
{
    MechBodyControllerService::GetInstance().OnDeviceConnected(mechInfo.mechId);
    auto connectNotifyTask = [this, mechInfo]() {
        std::lock_guard<std::mutex> autoLock(listenerSetMutex_);
        for (auto& listener : listenerSet_) {
            if (listener != nullptr) {
                listener->OnMechConnect(mechInfo);
            }
        }
    };
    if (eventHandler_ == nullptr || !eventHandler_->PostTask(connectNotifyTask)) {
        HILOGE("post task failed");
        return ERR_OK;
    }
    return ERR_OK;
}

int32_t MechConnectManager::NotifyMechDisconnect(const MechInfo& mechInfo)
{
    HILOGI("called, mechId: %{public}d.", mechInfo.mechId);
    McCameraTrackingController::GetInstance().OnConnectChange();
    {
        std::lock_guard<std::mutex> autoLock(mechInfosMutex_);
        auto it = mechInfos_.find(mechInfo);
        if (it != mechInfos_.end()) {
            mechInfos_.erase(it);
            MechBodyControllerService::GetInstance().OnDeviceDisconnected(mechInfo.mechId);
        }
    }
    auto offlineNotifyTask = [this, mechInfo]() {
        std::lock_guard<std::mutex> autoLock(listenerSetMutex_);
        for (auto& listener : listenerSet_) {
            if (listener != nullptr) {
                listener->OnMechDisconnect(mechInfo);
            }
        }
    };
    if (eventHandler_ == nullptr || !eventHandler_->PostTask(offlineNotifyTask)) {
        HILOGE("post task failed");
        return ERR_OK;
    }
    return ERR_OK;
}

int32_t MechConnectManager::AddDeviceChangeListener(const std::shared_ptr<IMechConnectListener>& listener)
{
    HILOGI("called");
    if (listener == nullptr) {
        HILOGI("listener is nullptr.");
        return INVALID_PARAMETERS_ERR;
    }
    std::lock_guard<std::mutex> autoLock(listenerSetMutex_);
    auto result = listenerSet_.insert(listener);
    if (!result.second) {
        HILOGI("listener is already.");
    }
    if (listenerSet_.find(listener) == listenerSet_.end()) {
        HILOGI("error.");
        return INVALID_PARAMETERS_ERR;
    }
    return ERR_OK;
}

int32_t MechConnectManager::RemoveDeviceChangeListener(const std::shared_ptr<IMechConnectListener>& listener)
{
    HILOGI("called");
    if (listener == nullptr) {
        HILOGI("listener is nullptr.");
        return INVALID_PARAMETERS_ERR;
    }
    {
        std::lock_guard<std::mutex> autoLock(listenerSetMutex_);
        auto it = listenerSet_.find(listener);
        if (it == listenerSet_.end()) {
            HILOGI("error.");
            return INVALID_PARAMETERS_ERR;
        }
        listenerSet_.erase(listener);
        if (listenerSet_.size() > 0) {
            return ERR_OK;
        }
    }
    HILOGI("remove ok");
    return ERR_OK;
}

bool MechConnectManager::GetMechBasicInfo(int32_t mechId, MechInfo& mechInfo)
{
    HILOGI("called, mechId: %{public}d", mechId);
    std::lock_guard<std::mutex> autoLock(mechInfosMutex_);
    auto it = std::find_if(mechInfos_.begin(), mechInfos_.end(), [mechId](const MechInfo& mechInfo) {
        return mechInfo.mechId == mechId;
    });
    if (it == mechInfos_.end()) {
        HILOGE("not found");
        return false;
    }
    HILOGI("found");
    mechInfo = *it;
    return true;
}

bool MechConnectManager::GetConnectMechList(std::set<MechInfo>& mechInfos)
{
    HILOGI("called");
    std::lock_guard<std::mutex> autoLock(mechInfosMutex_);
    if (mechInfos_.empty()) {
        HILOGE("list is empty");
        return false;
    }
    mechInfos = mechInfos_;
    return true;
}

bool MechConnectManager::NotifyMechState(int32_t mechId, bool isAttached)
{
    HILOGI("called, mechId: %{public}d. isAttached %{public}d", mechId, isAttached);
    std::lock_guard<std::mutex> autoLock(mechInfosMutex_);
    auto it = std::find_if(mechInfos_.begin(), mechInfos_.end(),
        [&](const MechInfo& mechInfo) { return mechInfo.mechId == mechId; });
    if (it == mechInfos_.end()) {
        HILOGE("can not find this mechInfo");
        return false;
    }
    MechInfo newMechInfo = *it;
    newMechInfo.state = isAttached ? AttachmentState::ATTACHED : AttachmentState::DETACHED;
    mechInfos_.erase(it);
    mechInfos_.insert(newMechInfo);

    if (!isAttached) {
        auto disconnectTask = [this, newMechInfo]() mutable {
            BleSendManager::GetInstance().MechbodyDisConnect(newMechInfo);
        };
        if (eventHandler_ == nullptr || !eventHandler_->PostTask(disconnectTask, "disconnect")) {
            HILOGE("MECHBODY_EXEC_CONNECT post task failed");
            return ERR_OK;
        }
    }
    return true;
}

bool MechConnectManager::GetMechState(int32_t mechId)
{
    HILOGI("called, mechId: %{public}d", mechId);
    std::lock_guard<std::mutex> autoLock(mechInfosMutex_);
    auto it = std::find_if(mechInfos_.begin(), mechInfos_.end(), [mechId](const MechInfo& mechInfo) {
        return mechInfo.mechId == mechId;
    });
    if (it == mechInfos_.end()) {
        HILOGE("not found");
        return false;
    }
    HILOGI("found");
    MechInfo mechInfo = *it;
    return mechInfo.state == AttachmentState::ATTACHED ? true : false;
}

bool MechConnectManager::UpdateBleStatus(bool isBLEActive)
{
    HILOGI("called");
    isBLEActive_.store(isBLEActive);
    return true;
}

bool MechConnectManager::GetLocalDeviceBleStatus()
{
    HILOGI("called");
    return isBLEActive_.load();
}

bool MechConnectManager::IsConnect()
{
    HILOGI("called");
    std::lock_guard<std::mutex> autoLock(mechInfosMutex_);
    if (mechInfos_.empty() || mechInfos_.size() == 0) {
        HILOGW("list is empty");
        return false;
    }
    return true;
}

int32_t MechConnectManager::AddMechInfo(MechInfo &mechInfo)
{
    {
        HILOGI("MECHBODY_EXEC_CONNECT mechInfo: %{public}s", mechInfo.ToString().c_str());
        std::lock_guard<std::mutex> autoLock(mechInfosMutex_);
        while (mechInfos_.count(mechInfo) != 0) {
            mechInfo.mechId = (mechInfo.mechId + 1) % INT32_MAX;
        }
        mechInfos_.insert(mechInfo);
    }
    return ERR_OK;
}


int32_t MechConnectManager::GetMechInfo(std::string &mac, MechInfo &mechInfo)
{
    std::lock_guard<std::mutex> autoLock(mechInfosMutex_);
    for (auto &info: mechInfos_) {
        if (info.mac == mac) {
            mechInfo = info;
            HILOGI("MECHBODY_EXEC_CONNECT mechInfo: %{public}s", info.ToString().c_str());
            return ERR_OK;
        }
    }
    return MECH_INFO_NOT_FOUND;
}

int32_t MechConnectManager::SetMechanicGattState(std::string &mac, bool state)
{
    std::lock_guard<std::mutex> autoLock(mechInfosMutex_);
    for (const auto& info : mechInfos_) {
        if (info.mac == mac) {
            const_cast<MechInfo&>(info).gattCoonectState = state;
            HILOGI("MECHBODY_EXEC_CONNECT mechInfo: %{public}s", info.ToString().c_str());
            return ERR_OK;
        }
    }
    return MECH_INFO_NOT_FOUND;
}

int32_t MechConnectManager::GetMechanicGattState(std::string &mac, bool &state)
{
    std::lock_guard<std::mutex> autoLock(mechInfosMutex_);
    for (auto &info: mechInfos_) {
        if (info.mac == mac) {
            state = info.gattCoonectState;
            HILOGI("MECHBODY_EXEC_CONNECT mechInfo: %{public}s", info.ToString().c_str());
            return ERR_OK;
        }
    }
    return MECH_INFO_NOT_FOUND;
}

int32_t MechConnectManager::SetMechanicPairState(std::string &mac, bool state)
{
    std::lock_guard<std::mutex> autoLock(mechInfosMutex_);
    for (const auto& info : mechInfos_) {
        if (info.mac == mac) {
            const_cast<MechInfo&>(info).pairState = state;
            HILOGI("MECHBODY_EXEC_CONNECT mechInfo: %{public}s", info.ToString().c_str());
            return ERR_OK;
        }
    }
    return MECH_INFO_NOT_FOUND;
}

int32_t MechConnectManager::GetMechanicPairState(std::string &mac, bool &state)
{
    std::lock_guard<std::mutex> autoLock(mechInfosMutex_);
    for (auto &info: mechInfos_) {
        if (info.mac == mac) {
            state = info.pairState;
            HILOGI("MECHBODY_EXEC_CONNECT mechInfo: %{public}s", info.ToString().c_str());
            return ERR_OK;
        }
    }
    return MECH_INFO_NOT_FOUND;
}

int32_t MechConnectManager::SetMechanicHidState(std::string &mac, bool state)
{
    std::lock_guard<std::mutex> autoLock(mechInfosMutex_);
    for (const auto& info : mechInfos_) {
        if (info.mac == mac) {
            const_cast<MechInfo&>(info).hidState = state;
            HILOGI("MECHBODY_EXEC_CONNECT mechInfo: %{public}s", info.ToString().c_str());
            return ERR_OK;
        }
    }
    return MECH_INFO_NOT_FOUND;
}

int32_t MechConnectManager::GetMechanicHidState(std::string &mac, bool &state)
{
    std::lock_guard<std::mutex> autoLock(mechInfosMutex_);
    for (auto &info: mechInfos_) {
        if (info.mac == mac) {
            state = info.hidState;
            HILOGI("MECHBODY_EXEC_CONNECT mechInfo: %{public}s", info.ToString().c_str());
            return ERR_OK;
        }
    }
    return MECH_INFO_NOT_FOUND;
}
} // namespace MechBodyController
} // namespace OHOS
