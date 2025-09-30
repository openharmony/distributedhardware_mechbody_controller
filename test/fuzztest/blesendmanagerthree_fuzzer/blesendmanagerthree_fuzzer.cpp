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

#include "blesendmanagerthree_fuzzer.h"

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

void MechbodyGattcWriteCharacteristicFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t dataLen = fdp.ConsumeIntegral<uint8_t>();
    uint8_t dataInput[dataLen];
    for (uint32_t i = 0; i < dataLen; i++) {
        dataInput[i] = fdp.ConsumeIntegral<uint8_t>();
    }
    uint8_t *dataInputPtr = dataInput;
    std::string address = fdp.ConsumeRandomLengthString();
    BluetoothRemoteDevice device(address, 1);

    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.gattClient_ = std::make_shared<GattClient>(device);
    bleSendManager.MechbodyGattcWriteCharacteristic(dataInputPtr, dataLen);
    bleSendManager.MechbodyGattcWriteCharacteristic(nullptr, dataLen);
}

void SendDataFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t dataLen = fdp.ConsumeIntegral<uint8_t>();
    uint8_t dataInput[dataLen];
    for (uint32_t i = 0; i < dataLen; i++) {
        dataInput[i] = fdp.ConsumeIntegral<uint8_t>();
    }
    uint8_t *dataInputPtr = dataInput;

    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.SendData(dataInputPtr, dataLen);
    bleSendManager.SendData(nullptr, dataLen);
}

void OnScanCallbackBleCentralManagerCallbackImplFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    BleScanResult result;
    FuzzedDataProvider fdp(data, size);
    std::string name = fdp.ConsumeRandomLengthString();
    result.name_ = name;

    std::shared_ptr<BleCentralManagerCallbackImpl> bleCentralManagerCallback =
        std::make_shared<BleCentralManagerCallbackImpl>();
    bleCentralManagerCallback->OnScanCallback(result);
}

void OnGattDisconnectFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    MechInfo mechInfo;
    FuzzedDataProvider fdp(data, size);
    std::string macHash = fdp.ConsumeRandomLengthString();
    std::string mechName = fdp.ConsumeRandomLengthString();
    mechInfo.mechName = mechName;

    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.OnGattDisconnect(mechInfo);
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::MechbodyGattcWriteCharacteristicFuzzTest(data, size);
    OHOS::SendDataFuzzTest(data, size);
    OHOS::OnScanCallbackBleCentralManagerCallbackImplFuzzTest(data, size);
    OHOS::OnGattDisconnectFuzzTest(data, size);
    return 0;
}