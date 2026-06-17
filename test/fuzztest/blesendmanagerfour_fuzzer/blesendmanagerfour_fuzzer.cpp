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

#include "blesendmanagerfour_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include <string>

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage, readability-identifier-naming)
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
const int32_t VALUE_TWO = 2;
const int32_t VALUE_THREE = 3;
std::unique_ptr<std::thread> delayThread;

UUID SERVICE_UUID = UUID::FromString("15f1e600-a277-43fc-a484-dd39ef8a9100");
UUID MECHBODY_CHARACTERISTIC_WRITE_UUID = UUID::FromString("15f1e602-a277-43fc-a484-dd39ef8a9100");
UUID MECHBODY_CHARACTERISTIC_NOTIFY_UUID = UUID::FromString("15f1e603-a277-43fc-a484-dd39ef8a9100");
BleSendManager& bleSendManager = BleSendManager::GetInstance();
}

void InitFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    int32_t testChoice = fdp.ConsumeIntegral<int32_t>();
    if (testChoice % VALUE_TWO == 0) {
        bleSendManager.observer_ = nullptr;
        bleSendManager.remoteDeviceObserver_ = nullptr;
        bleSendManager.hostObserver_ = nullptr;
    } else {
        bleSendManager.observer_ = std::make_shared<HidObserver>();
        bleSendManager.remoteDeviceObserver_ = std::make_shared<RemoteDeviceObserver>();
        bleSendManager.hostObserver_ = std::make_shared<HostObserver>();
    }

    bleSendManager.Init();
}

void CleanOldAssetsForMechbodyStartFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    int32_t testChoice = fdp.ConsumeIntegral<int32_t>();
    if (testChoice % VALUE_TWO == 0) {
        bleSendManager.eventHandler_ = nullptr;
    } else {
        auto runner = AppExecFwk::EventRunner::Create("BleSenderManager");
        bleSendManager.eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    }

    bleSendManager.CleanOldAssetsForMechbodyStart();
}

void CleanAllLocalInfoFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    // 场景1: eventHandler_ 为 nullptr
    int32_t testChoice = fdp.ConsumeIntegral<int32_t>();
    if (testChoice % VALUE_THREE == 0) {
        bleSendManager.eventHandler_ = nullptr;
        bleSendManager.CleanAllLocalInfo();
        return;
    }

    // 场景2和3: eventHandler_ 不为 nullptr
    auto runner = AppExecFwk::EventRunner::Create("BleSenderManager");
    bleSendManager.eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);

    // 场景2: 蓝牙服务未停止
    bool bluetoothServiceStatus = fdp.ConsumeBool();
    if (!bluetoothServiceStatus) {
        bleSendManager.SetBluetoothServiceStoped(false);
        bleSendManager.CleanAllLocalInfo();
        return;
    }

    // 场景3: 蓝牙服务已停止，执行完整清理逻辑
    bleSendManager.SetBluetoothServiceStoped(true);

    // 添加模拟设备信息到 MechConnectManager
    int32_t deviceCount = fdp.ConsumeIntegralInRange<int32_t>(0, 5);
    int32_t randomLengthMin = 17;
    int32_t randomLengthMax = 32;
    for (int32_t i = 0; i < deviceCount; i++) {
        MechInfo mechInfo;
        mechInfo.mac = fdp.ConsumeRandomLengthString(randomLengthMin);
        mechInfo.mechName = fdp.ConsumeRandomLengthString(randomLengthMax);
        mechInfo.mechId = fdp.ConsumeIntegral<int32_t>();
        mechInfo.mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());
        mechInfo.gattCoonectState = fdp.ConsumeBool();
        MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    }

    bleSendManager.CleanAllLocalInfo();

    // 清理：重置状态
    MechConnectManager::GetInstance().CleanMechInfo();
}

void UnInitFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    int32_t testChoice = fdp.ConsumeIntegral<int32_t>();
    if (testChoice % VALUE_TWO == 0) {
        bleSendManager.gattClient_ = nullptr;
    } else {
        std::string address = fdp.ConsumeRandomLengthString();
        BluetoothRemoteDevice device(address, 1);
        bleSendManager.gattClient_ = std::make_shared<GattClient>(device);
    }

    bleSendManager.UnInit();
}

void CancelDelayedUnloadFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    int32_t testChoice = fdp.ConsumeIntegral<int32_t>();
    if (testChoice % VALUE_TWO == 0) {
        bleSendManager.eventHandler_ = nullptr;
    } else {
        auto runner = AppExecFwk::EventRunner::Create("BleSenderManager");
        bleSendManager.eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    }

    bleSendManager.CancelDelayedUnload();
}

void HasBluetoothServiceStopedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    bool status = fdp.ConsumeBool();
    bleSendManager.SetBluetoothServiceStoped(status);

    bleSendManager.HasBluetoothServiceStoped();
}

void GetRealNameFuzzTest(const uint8_t *data, size_t size)
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

    bleSendManager.GetRealName(mechInfo);
    bleSendManager.NotifyGetRealName();
}
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::InitFuzzTest(data, size);
    OHOS::CleanOldAssetsForMechbodyStartFuzzTest(data, size);
    OHOS::CleanAllLocalInfoFuzzTest(data, size);
    OHOS::UnInitFuzzTest(data, size);
    OHOS::CancelDelayedUnloadFuzzTest(data, size);
    OHOS::HasBluetoothServiceStopedFuzzTest(data, size);
    OHOS::GetRealNameFuzzTest(data, size);
    return 0;
}