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

#include "bluetoothstatelistener_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include <string>

#include "bluetooth_state_listener.h"
#include "bluetooth_state_adapter.h"
#include "mechbody_controller_log.h"

namespace OHOS {
using namespace OHOS::MechBodyController;
namespace {
const std::string TAG = "BluetoothStateListenerFuzzTest";
constexpr uint32_t MIN_INIT_COUNT = 1;
constexpr uint32_t MAX_INIT_COUNT = 5;
}

void OnStateChangedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    
    // Consume transport parameter
    int transport = fdp.ConsumeIntegral<int>();
    
    // Consume status parameter
    int status = fdp.ConsumeIntegral<int>();
    
    // Get BluetoothStateListener instance and call OnStateChanged
    auto listener = BluetoothStateListener::GetInstance();
    listener->OnStateChanged(transport, status);
}

void OnStateChangedBoundaryFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    
    // Test with boundary values for transport
    int transport = fdp.ConsumeIntegralInRange<int>(-1000, 1000);
    
    // Test with boundary values for status
    int status = fdp.ConsumeIntegralInRange<int>(-1000, 1000);
    
    auto listener = BluetoothStateListener::GetInstance();
    listener->OnStateChanged(transport, status);
}

void OnStateChangedCombinedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    
    auto listener = BluetoothStateListener::GetInstance();
    
    int length = 10;
    // Test multiple state changes in sequence
    for (int i = 0; i < length && fdp.remaining_bytes() > 0; i++) {
        int transport = fdp.ConsumeIntegral<int>();
        int status = fdp.ConsumeIntegral<int>();
        listener->OnStateChanged(transport, status);
    }
}

void InitBluetoothStateFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    
    // Get BluetoothStateListener instance
    auto listener = BluetoothStateListener::GetInstance();
    
    // Use fuzz data to control the number of times to initialize
    uint32_t initCount = fdp.ConsumeIntegralInRange<uint32_t>(MIN_INIT_COUNT, MAX_INIT_COUNT);
    for (uint32_t i = 0; i < initCount; i++) {
        listener->InitBluetoothState();
    }
}

void BluetoothStateListenerCombinedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    
    auto listener = BluetoothStateListener::GetInstance();
    
    // Initialize bluetooth state
    listener->InitBluetoothState();
    
    // Test various state changes
    int transport = fdp.ConsumeIntegral<int>();
    int status = fdp.ConsumeIntegral<int>();
    listener->OnStateChanged(transport, status);
    
    // Test with ADAPTER_BREDR transport
    listener->OnStateChanged(static_cast<int>(Bluetooth::BTTransport::ADAPTER_BREDR),
        fdp.ConsumeBool() ? Bluetooth::BTStateID::STATE_TURN_ON : Bluetooth::BTStateID::STATE_TURN_OFF);
    
    // Test with ADAPTER_BLE transport
    listener->OnStateChanged(static_cast<int>(Bluetooth::BTTransport::ADAPTER_BLE),
        fdp.ConsumeBool() ? Bluetooth::BTStateID::STATE_TURN_ON : Bluetooth::BTStateID::STATE_TURN_OFF);
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::OnStateChangedFuzzTest(data, size);
    OHOS::OnStateChangedBoundaryFuzzTest(data, size);
    OHOS::OnStateChangedCombinedFuzzTest(data, size);
    OHOS::InitBluetoothStateFuzzTest(data, size);
    OHOS::BluetoothStateListenerCombinedFuzzTest(data, size);
    return 0;
}
} // namespace OHOS