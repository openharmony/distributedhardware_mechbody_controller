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

#include "bluetooth_state_adapter.h"

#include "mc_connect_manager.h"
#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "BluetoothStateAdapter";
}

BluetoothStateAdapter& BluetoothStateAdapter::GetInstance()
{
    static auto instance = new BluetoothStateAdapter();
    return *instance;
}

bool BluetoothStateAdapter::IsBluetoothActive()
{
    HILOGI("current BT state cache is: %{public}s; BLE state cache is: %{public}s", isBTActive_ ? "true" : "false",
           isBLEActive_ ? "true" : "false");
    return isBTActive_.load() || isBLEActive_.load();
}

void BluetoothStateAdapter::UpdateBTState(bool isBTActive)
{
    HILOGI("update BT state: %{public}s", isBTActive ? "true" : "false");
    isBTActive_.store(isBTActive);
    MechConnectManager::GetInstance().UpdateBleStatus(isBTActive_.load() || isBLEActive_.load());
}

void BluetoothStateAdapter::UpdateBLEState(bool isBLEActive)
{
    HILOGI("update BLE state: %{public}s", isBLEActive ? "true" : "false");
    isBLEActive_.store(isBLEActive);
    MechConnectManager::GetInstance().UpdateBleStatus(isBLEActive_.load());
}
} // namespace MechBodyController
} // namespace OHOS
