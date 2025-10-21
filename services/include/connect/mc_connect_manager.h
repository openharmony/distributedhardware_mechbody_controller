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

#ifndef OHOS_MECH_CONNECT_MANAGER_H
#define OHOS_MECH_CONNECT_MANAGER_H

#include <mutex>
#include <set>
#include <string>

#include "event_handler.h"
#include "mechbody_controller_log.h"
#include "mechbody_controller_types.h"

namespace OHOS {
namespace MechBodyController {

class IMechConnectListener {
public:
    IMechConnectListener() = default;
    virtual ~IMechConnectListener() = default;

    virtual void OnMechConnect(const MechInfo& mechInfo) = 0;
    virtual void OnMechDisconnect(const MechInfo& mechInfo) = 0;
};

class MechConnectManager {
public:
    static MechConnectManager& GetInstance();
private:
    MechConnectManager() = default;
    ~MechConnectManager() = default;
    MechConnectManager(const MechConnectManager&) = delete;
    MechConnectManager& operator= (const MechConnectManager&) = delete;
    MechConnectManager(MechConnectManager&&) = delete;
    MechConnectManager& operator= (MechConnectManager&&) = delete;

public:
    void Init();
    void UnInit();
    int32_t AddDeviceChangeListener(const std::shared_ptr<IMechConnectListener>& listener);
    int32_t RemoveDeviceChangeListener(const std::shared_ptr<IMechConnectListener>& listener);
    int32_t NotifyMechConnect(MechInfo& mechInfo);
    int32_t NotifyMechDisconnect(const MechInfo& mechInfo);
    bool GetMechBasicInfo(int32_t mechId, MechInfo& mechInfo);
    bool GetConnectMechList(std::set<MechInfo>& mechInfos);
    bool NotifyMechState(int32_t mechId, bool isAttached);
    bool GetMechState(int32_t mechId);
    bool UpdateBleStatus(bool isBLEActive);
    bool GetLocalDeviceBleStatus();
    bool IsConnect();

    int32_t AddMechInfo(MechInfo &mechInfo);
    int32_t RemoveMechInfoByMac(std::string &mac);
    int32_t CleanMechInfo();
    int32_t GetMechInfo(std::string &mac, MechInfo &mechInfo);
    const std::set<MechInfo> &GetMechInfos() const;
    int32_t SetMechanicGattState(std::string &mac, bool state);
    int32_t GetMechanicGattState(std::string &mac, bool &state);
    int32_t SetMechanicPairState(std::string &mac, bool state);
    int32_t GetMechanicPairState(std::string &mac, bool &state);
    int32_t SetMechanicHidState(std::string &mac, bool state);
    int32_t GetMechanicHidState(std::string &mac, bool &state);
    void SetRealMechName(int32_t mechId, std::string &name);

private:
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_;
    std::mutex listenerSetMutex_;
    std::mutex mechInfosMutex_;
    std::set<std::shared_ptr<IMechConnectListener>> listenerSet_;
    std::set<MechInfo> mechInfos_;
    std::atomic<bool> isBLEActive_ = true;
};
} // namespace MechBodyController
} // namespace OHOS
#endif // OHOS_MECH_CONNECT_MANAGER_H
