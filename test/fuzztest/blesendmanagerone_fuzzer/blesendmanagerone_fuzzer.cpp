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

#include "blesendmanagerone_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include <string>

#define private public
#include "ble_send_manager.h"
#undef private
#include "mechbody_controller_log.h"
#include "securec.h"

namespace OHOS {
using namespace OHOS::MechBodyController;
using namespace OHOS::Bluetooth;
namespace {
const std::string TAG = "BleSendManager";
const std::string TARGET_DEVICE_NAME = "";
std::unique_ptr<std::thread> delayThread;

UUID SERVICE_UUID = UUID::FromString("15f1e600-a277-43fc-a484-dd39ef8a9100"); // GATT Service uuid
UUID MECHBODY_CHARACTERISTIC_WRITE_UUID = UUID::FromString("15f1e602-a277-43fc-a484-dd39ef8a9100"); // write uuid
UUID MECHBODY_CHARACTERISTIC_NOTIFY_UUID = UUID::FromString("15f1e603-a277-43fc-a484-dd39ef8a9100"); // notify uuid
BleSendManager& bleSendManager = BleSendManager::GetInstance();
}

void OnConnectionStateChangedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    int connectionState = fdp.ConsumeIntegral<int>();
    int ret = fdp.ConsumeIntegral<int>();
    std::string address = fdp.ConsumeRandomLengthString();
    BluetoothRemoteDevice device(address, 1);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.gattClient_ = std::make_shared<GattClient>(device);

    auto bleGattClientCallBack = std::make_shared<BleGattClientCallback>();
    if (bleGattClientCallBack == nullptr) {
        return;
    }
    bleGattClientCallBack->OnConnectionStateChanged(connectionState, ret);
}

void OnServicesDiscoveredFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    int status = fdp.ConsumeIntegral<int>();
    std::string address = fdp.ConsumeRandomLengthString();
    BluetoothRemoteDevice device(address, 1);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.gattClient_ = std::make_shared<GattClient>(device);

    auto bleGattClientCallBack = std::make_shared<BleGattClientCallback>();
    if (bleGattClientCallBack == nullptr) {
        return;
    }
    bleGattClientCallBack->OnServicesDiscovered(status);
}

void OnSetNotifyCharacteristicFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    int status = fdp.ConsumeIntegral<int>();
    std::string address = fdp.ConsumeRandomLengthString();
    bleSendManager.handle_ = fdp.ConsumeIntegral<uint16_t>();
    bleSendManager.permissions_ = fdp.ConsumeIntegral<int32_t>();
    bleSendManager.properties_ = fdp.ConsumeIntegral<int32_t>();
    BluetoothRemoteDevice device(address, 1);
    bleSendManager.gattClient_ = std::make_shared<GattClient>(device);
    GattCharacteristic characteristic = GattCharacteristic(MECHBODY_CHARACTERISTIC_WRITE_UUID,
        bleSendManager.handle_, bleSendManager.permissions_, bleSendManager.properties_);

    auto bleGattClientCallBack = std::make_shared<BleGattClientCallback>();
    if (bleGattClientCallBack == nullptr) {
        return;
    }
    bleGattClientCallBack->OnSetNotifyCharacteristic(characteristic, status);
}

void OnCharacteristicChangedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.handle_ = fdp.ConsumeIntegral<uint16_t>();
    bleSendManager.permissions_ = fdp.ConsumeIntegral<int32_t>();
    bleSendManager.properties_ = fdp.ConsumeIntegral<int32_t>();
    GattCharacteristic characteristic = GattCharacteristic(MECHBODY_CHARACTERISTIC_WRITE_UUID,
        bleSendManager.handle_, bleSendManager.permissions_, bleSendManager.properties_);

    auto bleGattClientCallBack = std::make_shared<BleGattClientCallback>();
    if (bleGattClientCallBack == nullptr) {
        return;
    }
    bleGattClientCallBack->OnCharacteristicChanged(characteristic);
}

void OnCharacteristicWriteResultFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.handle_ = fdp.ConsumeIntegral<uint16_t>();
    bleSendManager.permissions_ = fdp.ConsumeIntegral<int32_t>();
    bleSendManager.properties_ = fdp.ConsumeIntegral<int32_t>();
    int ret = fdp.ConsumeIntegral<int>();
    GattCharacteristic characteristic = GattCharacteristic(MECHBODY_CHARACTERISTIC_WRITE_UUID,
        bleSendManager.handle_, bleSendManager.permissions_, bleSendManager.properties_);

    auto bleGattClientCallBack = std::make_shared<BleGattClientCallback>();
    if (bleGattClientCallBack == nullptr) {
        return;
    }
    bleGattClientCallBack->OnCharacteristicWriteResult(characteristic, ret);
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::OnConnectionStateChangedFuzzTest(data, size);
    OHOS::OnServicesDiscoveredFuzzTest(data, size);
    OHOS::OnSetNotifyCharacteristicFuzzTest(data, size);
    OHOS::OnCharacteristicChangedFuzzTest(data, size);
    OHOS::OnCharacteristicWriteResultFuzzTest(data, size);
    return 0;
}