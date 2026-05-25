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

#include "blesendmanagersix_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include <string>

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage, readability-identifier-naming)
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
const int32_t VALUE_TWO = 2;
std::unique_ptr<std::thread> delayThread;

UUID SERVICE_UUID = UUID::FromString("15f1e600-a277-43fc-a484-dd39ef8a9100");
UUID MECHBODY_CHARACTERISTIC_WRITE_UUID = UUID::FromString("15f1e602-a277-43fc-a484-dd39ef8a9100");
UUID MECHBODY_CHARACTERISTIC_NOTIFY_UUID = UUID::FromString("15f1e603-a277-43fc-a484-dd39ef8a9100");
BleSendManager& bleSendManager = BleSendManager::GetInstance();
}

void MechbodyDisConnectFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    MechInfo mechInfo;
    mechInfo.mac = fdp.ConsumeRandomLengthString();
    mechInfo.mechName = fdp.ConsumeRandomLengthString();
    mechInfo.mechId = fdp.ConsumeIntegral<int32_t>();
    mechInfo.mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());
    mechInfo.gattCoonectState = fdp.ConsumeBool();

    int32_t testChoice = fdp.ConsumeIntegral<int32_t>();
    if (testChoice % VALUE_TWO == 0) {
        bleSendManager.eventHandler_ = nullptr;
    } else {
        auto runner = AppExecFwk::EventRunner::Create("BleSenderManager");
        bleSendManager.eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    }

    bleSendManager.MechbodyDisConnect(mechInfo);
}

void MechbodyDisConnectSyncFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    MechInfo mechInfo;
    mechInfo.mac = fdp.ConsumeRandomLengthString();
    mechInfo.mechName = fdp.ConsumeRandomLengthString();
    mechInfo.mechId = fdp.ConsumeIntegral<int32_t>();
    mechInfo.mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());
    mechInfo.gattCoonectState = fdp.ConsumeBool();

    bleSendManager.MechbodyDisConnectSync(mechInfo);
}

void MechbodyGattcDisconnectFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    MechInfo mechInfo;
    mechInfo.mac = fdp.ConsumeRandomLengthString();
    mechInfo.mechName = fdp.ConsumeRandomLengthString();
    mechInfo.mechId = fdp.ConsumeIntegral<int32_t>();
    mechInfo.mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());

    int32_t testChoice = fdp.ConsumeIntegral<int32_t>();
    if (testChoice % VALUE_TWO == 0) {
        bleSendManager.gattClient_ = nullptr;
    } else {
        std::string address = fdp.ConsumeRandomLengthString();
        BluetoothRemoteDevice device(address, 1);
        bleSendManager.gattClient_ = std::make_shared<GattClient>(device);
    }

    bleSendManager.MechbodyGattcDisconnect(mechInfo);
}

void MechbodyHidDisconnectFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    MechInfo mechInfo;
    mechInfo.mac = fdp.ConsumeRandomLengthString();
    mechInfo.mechName = fdp.ConsumeRandomLengthString();
    mechInfo.mechId = fdp.ConsumeIntegral<int32_t>();
    mechInfo.mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());

    bleSendManager.MechbodyHidDisconnect(mechInfo);
}

void MechbodyDisConnectForMechbotyStartFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    MechInfo mechInfo;
    mechInfo.mac = fdp.ConsumeRandomLengthString();
    mechInfo.mechName = fdp.ConsumeRandomLengthString();
    mechInfo.mechId = fdp.ConsumeIntegral<int32_t>();
    mechInfo.mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());
    mechInfo.gattCoonectState = fdp.ConsumeBool();

    int32_t testChoice = fdp.ConsumeIntegral<int32_t>();
    if (testChoice % VALUE_TWO == 0) {
        bleSendManager.eventHandler_ = nullptr;
    } else {
        auto runner = AppExecFwk::EventRunner::Create("BleSenderManager");
        bleSendManager.eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    }

    bleSendManager.MechbodyDisConnectForMechbotyStart(mechInfo);
}

void MechbodyDisConnectSyncForMechbotyStartFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    MechInfo mechInfo;
    mechInfo.mac = fdp.ConsumeRandomLengthString();
    mechInfo.mechName = fdp.ConsumeRandomLengthString();
    mechInfo.mechId = fdp.ConsumeIntegral<int32_t>();
    mechInfo.mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());
    mechInfo.gattCoonectState = fdp.ConsumeBool();

    bleSendManager.MechbodyDisConnectSyncForMechbotyStart(mechInfo);
}

void MechbodyGattcDisconnectForMechbotyStartFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    MechInfo mechInfo;
    mechInfo.mac = fdp.ConsumeRandomLengthString();
    mechInfo.mechName = fdp.ConsumeRandomLengthString();
    mechInfo.mechId = fdp.ConsumeIntegral<int32_t>();
    mechInfo.mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());
    mechInfo.gattCoonectState = fdp.ConsumeBool();

    int32_t testChoice = fdp.ConsumeIntegral<int32_t>();
    if (testChoice % VALUE_TWO == 0) {
        bleSendManager.gattClient_ = nullptr;
    } else {
        std::string address = fdp.ConsumeRandomLengthString();
        BluetoothRemoteDevice device(address, 1);
        bleSendManager.gattClient_ = std::make_shared<GattClient>(device);
    }

    bleSendManager.MechbodyGattcDisconnectForMechbotyStart(mechInfo);
}

void MechbodyHidDisconnectForMechbotyStartFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    MechInfo mechInfo;
    mechInfo.mac = fdp.ConsumeRandomLengthString();
    mechInfo.mechName = fdp.ConsumeRandomLengthString();
    mechInfo.mechId = fdp.ConsumeIntegral<int32_t>();
    mechInfo.mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());
    mechInfo.gattCoonectState = fdp.ConsumeBool();

    bleSendManager.MechbodyHidDisconnectForMechbotyStart(mechInfo);
}

void SetBluetoothServiceStopedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    bool status = fdp.ConsumeBool();

    bleSendManager.SetBluetoothServiceStoped(status);
}
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::MechbodyDisConnectFuzzTest(data, size);
    OHOS::MechbodyDisConnectSyncFuzzTest(data, size);
    OHOS::MechbodyGattcDisconnectFuzzTest(data, size);
    OHOS::MechbodyHidDisconnectFuzzTest(data, size);
    OHOS::MechbodyDisConnectForMechbotyStartFuzzTest(data, size);
    OHOS::MechbodyDisConnectSyncForMechbotyStartFuzzTest(data, size);
    OHOS::MechbodyGattcDisconnectForMechbotyStartFuzzTest(data, size);
    OHOS::MechbodyHidDisconnectForMechbotyStartFuzzTest(data, size);
    OHOS::SetBluetoothServiceStopedFuzzTest(data, size);
    return 0;
}