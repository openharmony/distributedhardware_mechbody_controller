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

#include "blesendmanagerfive_fuzzer.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <thread>

#define private public
#include "ble_send_manager.h"
#undef private
#include "connect/mc_connect_manager.h"
#include "mechbody_controller_log.h"
#include "securec.h"

namespace OHOS {
using namespace OHOS::MechBodyController;
using namespace OHOS::Bluetooth;
namespace {
const std::string TAG = "BleSendManager";
const std::string TARGET_DEVICE_NAME = "";
std::unique_ptr<std::thread> delayThread;

UUID SERVICE_UUID = UUID::FromString("15f1e600-a277-43fc-a484-dd39ef8a9100");
UUID MECHBODY_CHARACTERISTIC_WRITE_UUID = UUID::FromString("15f1e602-a277-43fc-a484-dd39ef8a9100");
UUID MECHBODY_CHARACTERISTIC_NOTIFY_UUID = UUID::FromString("15f1e603-a277-43fc-a484-dd39ef8a9100");
BleSendManager& bleSendManager = BleSendManager::GetInstance();

class TestBleReceviceListener : public BleReceviceListener {
public:
    virtual ~TestBleReceviceListener() = default;
    int32_t OnReceive(const uint8_t *data, uint32_t dataLen) override
    {
        return ERR_OK;
    }
};
}

void OnPairStateChangedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    int pairState = fdp.ConsumeIntegral<int>();
    int cause = fdp.ConsumeIntegral<int>();

    MechInfo mechInfo;
    mechInfo.mac = fdp.ConsumeRandomLengthString();
    mechInfo.mechName = fdp.ConsumeRandomLengthString();
    mechInfo.mechId = fdp.ConsumeIntegral<int32_t>();
    mechInfo.mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());

    bleSendManager.OnPairStateChanged(pairState, cause, mechInfo);
}

void OnHidStateChangedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    int hidState = fdp.ConsumeIntegral<int>();
    int cause = fdp.ConsumeIntegral<int>();

    MechInfo mechInfo;
    mechInfo.mac = fdp.ConsumeRandomLengthString();
    mechInfo.mechName = fdp.ConsumeRandomLengthString();
    mechInfo.mechId = fdp.ConsumeIntegral<int32_t>();
    mechInfo.mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());

    bleSendManager.OnHidStateChanged(hidState, cause, mechInfo);
}

void RegisterTransportSendAdapterFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    int32_t testChoice = fdp.ConsumeIntegral<int32_t>();
    std::shared_ptr<BleReceviceListener> listener;

    if (testChoice % 3 == 0) {
        listener = nullptr;
    } else if (testChoice % 3 == 1) {
        listener = std::make_shared<TestBleReceviceListener>();
    } else {
        listener = std::make_shared<TestBleReceviceListener>();
        bleSendManager.RegisterTransportSendAdapter(listener);
    }

    bleSendManager.RegisterTransportSendAdapter(listener);
}

void UnRegisterTransportSendAdapterFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    int32_t testChoice = fdp.ConsumeIntegral<int32_t>();
    std::shared_ptr<BleReceviceListener> listener;

    if (testChoice % 3 == 0) {
        listener = nullptr;
    } else if (testChoice % 3 == 1) {
        listener = std::make_shared<TestBleReceviceListener>();
    } else {
        listener = std::make_shared<TestBleReceviceListener>();
        bleSendManager.RegisterTransportSendAdapter(listener);
    }

    bleSendManager.UnRegisterTransportSendAdapter(listener);
}

void MechbodyConnectFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    std::string mac = fdp.ConsumeRandomLengthString(17);
    std::string deviceName = fdp.ConsumeRandomLengthString(32);
    uint32_t deviceIdentifier = fdp.ConsumeIntegral<uint32_t>();
    // 场景1: eventHandler_ 为 nullptr，等待超时（覆盖676-684行）
    bleSendManager.eventHandler_ = nullptr;
    bleSendManager.MechbodyConnect(mac, deviceName, deviceIdentifier);
}

void MechbodyConnectFuzzTestOne(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager &bleSendManager = BleSendManager::GetInstance();

    std::string mac = fdp.ConsumeRandomLengthString(17);
    std::string deviceName = fdp.ConsumeRandomLengthString(32);
    // 场景2: eventHandler_ 不为 nullptr，已有设备连接且非首次连接（覆盖698-707行）
    auto runner = AppExecFwk::EventRunner::Create("BleSenderManager");
    bleSendManager.eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);

    // 模拟已有设备连接
    MechInfo existingMech;
    existingMech.mac = "AA:BB:CC:DD:EE:FF";
    existingMech.mechName = "ExistingDevice";
    existingMech.gattCoonectState = true;
    MechConnectManager::GetInstance().AddMechInfo(existingMech);

    // 非首次连接（deviceIdentifier = 0），会提前返回
    bleSendManager.MechbodyConnect(mac, deviceName, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    MechConnectManager::GetInstance().CleanMechInfo();
}

void MechbodyConnectFuzzTestTwo(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager &bleSendManager = BleSendManager::GetInstance();

    std::string mac = fdp.ConsumeRandomLengthString(17);
    std::string deviceName = fdp.ConsumeRandomLengthString(32);

    // 场景3: eventHandler_ 不为 nullptr，已有设备连接且首次连接（覆盖704-707行）
    auto runner = AppExecFwk::EventRunner::Create("BleSenderManager");
    bleSendManager.eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);

    // 模拟已有设备连接
    MechInfo existingMech;
    existingMech.mac = "AA:BB:CC:DD:EE:FF";
    existingMech.mechName = "ExistingDevice";
    existingMech.gattCoonectState = true;
    MechConnectManager::GetInstance().AddMechInfo(existingMech);

    // 首次连接（deviceIdentifier != 0），会断开旧设备
    bleSendManager.MechbodyConnect(mac, deviceName, 12345);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    MechConnectManager::GetInstance().CleanMechInfo();
}

void MechbodyConnectFuzzTestThree(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager &bleSendManager = BleSendManager::GetInstance();

    std::string mac = fdp.ConsumeRandomLengthString(17);
    std::string deviceName = fdp.ConsumeRandomLengthString(32);
    uint32_t deviceIdentifier = fdp.ConsumeIntegral<uint32_t>();
    // 场景4: eventHandler_ 不为 nullptr，无设备连接（覆盖709-745行）
    auto runner = AppExecFwk::EventRunner::Create("BleSenderManager");
    bleSendManager.eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);

    // 确保无设备连接
    MechConnectManager::GetInstance().CleanMechInfo();

    // 添加设备信息（覆盖714-719行）
    MechInfo mechInfo;
    mechInfo.mac = mac;
    mechInfo.mechName = deviceName;
    mechInfo.isFirstConnect = (deviceIdentifier != 0);
    mechInfo.deviceIdentifier = deviceIdentifier;
    mechInfo.gattCoonectState = false;
    MechConnectManager::GetInstance().AddMechInfo(mechInfo);

    // 执行连接（覆盖722-745行）
    bleSendManager.MechbodyConnect(mac, deviceName, deviceIdentifier);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void MechbodyPairFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    std::string mac = fdp.ConsumeRandomLengthString();
    std::string deviceName = fdp.ConsumeRandomLengthString();

    bleSendManager.MechbodyPair(mac, deviceName);
}

void MechbodyHidConnectFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    std::string mac = fdp.ConsumeRandomLengthString();
    std::string deviceName = fdp.ConsumeRandomLengthString();

    bleSendManager.MechbodyHidConnect(mac, deviceName);
}
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::OnPairStateChangedFuzzTest(data, size);
    OHOS::OnHidStateChangedFuzzTest(data, size);
    OHOS::RegisterTransportSendAdapterFuzzTest(data, size);
    OHOS::UnRegisterTransportSendAdapterFuzzTest(data, size);
    OHOS::MechbodyConnectFuzzTest(data, size);
    OHOS::MechbodyConnectFuzzTestOne(data, size);
    OHOS::MechbodyConnectFuzzTestTwo(data, size);
    OHOS::MechbodyConnectFuzzTestThree(data, size);
    OHOS::MechbodyPairFuzzTest(data, size);
    OHOS::MechbodyHidConnectFuzzTest(data, size);
    return 0;
}