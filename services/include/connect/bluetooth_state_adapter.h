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

#ifndef OHOS_MECH_BLUETOOTH_STATE_ADAPTER_H
#define OHOS_MECH_BLUETOOTH_STATE_ADAPTER_H

#include <atomic>
#include <string>

namespace OHOS {
namespace MechBodyController {
class BluetoothStateAdapter {
public:
    static BluetoothStateAdapter& GetInstance();
private:
    BluetoothStateAdapter(const BluetoothStateAdapter&) = delete;
    BluetoothStateAdapter& operator= (const BluetoothStateAdapter&) = delete;
    BluetoothStateAdapter(BluetoothStateAdapter&&) = delete;
    BluetoothStateAdapter& operator= (BluetoothStateAdapter&&) = delete;

public:
    bool IsBluetoothActive();

    void UpdateBTState(bool isBTActive);

    void UpdateBLEState(bool isBLEActive);

private:
    BluetoothStateAdapter() {}

    std::atomic<bool> isBTActive_ = false;
    std::atomic<bool> isBLEActive_ = false;
};
} // namespace MechBodyController
} // namespace OHOS
#endif // OHOS_MECH_BLUETOOTH_STATE_ADAPTER_H
