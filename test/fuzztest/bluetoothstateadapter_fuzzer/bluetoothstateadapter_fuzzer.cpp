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

#include "bluetoothstateadapter_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include <string>

#include "bluetooth_state_adapter.h"
#include "mechbody_controller_log.h"

namespace OHOS {
using namespace OHOS::MechBodyController;
namespace {
const std::string TAG = "BluetoothStateAdapterFuzzTest";
}

void IsBluetoothActiveFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BluetoothStateAdapter& adapter = BluetoothStateAdapter::GetInstance();

    // Use fuzz data to control the number of times to check the state
    uint32_t checkCount = fdp.ConsumeIntegralInRange<uint32_t>(1, 10);
    for (uint32_t i = 0; i < checkCount && i < size; i++) {
        (void)adapter.IsBluetoothActive();
    }
}

void UpdateBTStateFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    bool isBTActive = fdp.ConsumeBool();

    BluetoothStateAdapter& adapter = BluetoothStateAdapter::GetInstance();
    adapter.UpdateBTState(isBTActive);
}

void UpdateBLEStateFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    bool isBLEActive = fdp.ConsumeBool();

    BluetoothStateAdapter& adapter = BluetoothStateAdapter::GetInstance();
    adapter.UpdateBLEState(isBLEActive);
}

void BluetoothStateAdapterCombinedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BluetoothStateAdapter& adapter = BluetoothStateAdapter::GetInstance();

    // Test various combinations of BT and BLE states
    bool isBTActive = fdp.ConsumeBool();
    bool isBLEActive = fdp.ConsumeBool();

    adapter.UpdateBTState(isBTActive);
    adapter.UpdateBLEState(isBLEActive);

    // Check the combined state
    (void)adapter.IsBluetoothActive();

    // Toggle states
    adapter.UpdateBTState(!isBTActive);
    adapter.UpdateBLEState(!isBLEActive);

    (void)adapter.IsBluetoothActive();
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::IsBluetoothActiveFuzzTest(data, size);
    OHOS::UpdateBTStateFuzzTest(data, size);
    OHOS::UpdateBLEStateFuzzTest(data, size);
    OHOS::BluetoothStateAdapterCombinedFuzzTest(data, size);
    return 0;
}
} // namespace OHOS