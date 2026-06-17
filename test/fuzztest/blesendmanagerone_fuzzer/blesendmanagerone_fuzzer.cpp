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

UUID SERVICE_UUID = UUID::FromString("15f1e600-a277-43fc-a484-dd39ef8a9100"); // GATT Service uuid
UUID MECHBODY_CHARACTERISTIC_WRITE_UUID = UUID::FromString("15f1e602-a277-43fc-a484-dd39ef8a9100"); // write uuid
UUID MECHBODY_CHARACTERISTIC_NOTIFY_UUID = UUID::FromString("15f1e603-a277-43fc-a484-dd39ef8a9100"); // notify uuid
BleSendManager& bleSendManager = BleSendManager::GetInstance();
}

void OnConnectionStateChangedFuzzTestOne(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider fdp(data, size);
    BleSendManager &bleSendManager = BleSendManager::GetInstance();

    std::string mac = fdp.ConsumeRandomLengthString(17);
    std::string deviceName = fdp.ConsumeRandomLengthString(32);
    int32_t mechId = fdp.ConsumeIntegral<int32_t>();
    MechType mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());

    // 场景1: connectionState 既不是 CONNECTED 也不是 DISCONNECTED（覆盖346-349行）
    int connectionState = fdp.ConsumeIntegralInRange<int>(0, 1);  // CONNECTING 或 DISCONNECTING
    int ret = fdp.ConsumeIntegral<int>();

    MechInfo mechInfo;
    mechInfo.mac = mac;
    mechInfo.mechName = deviceName;
    mechInfo.mechId = mechId;
    mechInfo.mechType = mechType;

    bleSendManager.OnConnectionStateChanged(connectionState, ret, mechInfo);
    // 清理
    MechConnectManager::GetInstance().CleanMechInfo();
}

void OnConnectionStateChangedFuzzTestTwo(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    std::string mac = fdp.ConsumeRandomLengthString(17);
    std::string deviceName = fdp.ConsumeRandomLengthString(32);
    int32_t mechId = fdp.ConsumeIntegral<int32_t>();
    MechType mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());
    // 场景2: connectionState = CONNECTED（覆盖351-358行）
    int connectionState = static_cast<int>(Bluetooth::BTConnectState::CONNECTED);
    int ret = fdp.ConsumeIntegral<int>();
    std::string address = fdp.ConsumeRandomLengthString(17);
    BluetoothRemoteDevice device(address, 1);
    bleSendManager.gattClient_ = std::make_shared<GattClient>(device);
    MechInfo mechInfo;
    mechInfo.mac = mac;
    mechInfo.mechName = deviceName;
    mechInfo.mechId = mechId;
    mechInfo.mechType = mechType;
    bleSendManager.OnConnectionStateChanged(connectionState, ret, mechInfo);

    // 清理
    MechConnectManager::GetInstance().CleanMechInfo();
}

void OnConnectionStateChangedFuzzTestThree(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider fdp(data, size);
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    std::string mac = fdp.ConsumeRandomLengthString(17);
    std::string deviceName = fdp.ConsumeRandomLengthString(32);
    int32_t mechId = fdp.ConsumeIntegral<int32_t>();
    MechType mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());
    int connectionState = static_cast<int>(Bluetooth::BTConnectState::DISCONNECTED);
    int ret = fdp.ConsumeIntegral<int>();
    std::string address = fdp.ConsumeRandomLengthString(17);
    BluetoothRemoteDevice device(address, 1);
    bleSendManager.gattClient_ = std::make_shared<GattClient>(device);

    // 添加设备信息到 MechConnectManager
    MechInfo mechInfo;
    mechInfo.mac = mac;
    mechInfo.mechName = deviceName;
    mechInfo.mechId = mechId;
    mechInfo.mechType = mechType;
    mechInfo.gattCoonectState = true;
    MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    bleSendManager.OnConnectionStateChanged(connectionState, ret, mechInfo);

    // 清理
    MechConnectManager::GetInstance().CleanMechInfo();
}

void OnConnectionStateChangedFuzzTestFour(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider fdp(data, size);
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    std::string mac = fdp.ConsumeRandomLengthString(17);
    std::string deviceName = fdp.ConsumeRandomLengthString(32);
    int32_t mechId = fdp.ConsumeIntegral<int32_t>();
    MechType mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());

    // 场景4: connectionState = CONNECTED，gattClient_ 为 nullptr（触发 CHECK_POINTER_RETURN）
    int connectionState = static_cast<int>(Bluetooth::BTConnectState::CONNECTED);
    int ret = fdp.ConsumeIntegral<int>();
    bleSendManager.gattClient_ = nullptr;
    MechInfo mechInfo;
    mechInfo.mac = mac;
    mechInfo.mechName = deviceName;
    mechInfo.mechId = mechId;
    mechInfo.mechType = mechType;
    bleSendManager.OnConnectionStateChanged(connectionState, ret, mechInfo);

    // 清理
    MechConnectManager::GetInstance().CleanMechInfo();
}

void OnConnectionStateChangedFuzzTestFive(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    std::string mac = fdp.ConsumeRandomLengthString(17);
    std::string deviceName = fdp.ConsumeRandomLengthString(32);
    int32_t mechId = fdp.ConsumeIntegral<int32_t>();
    MechType mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());

    // 场景5: connectionState = DISCONNECTED，设备不在 MechConnectManager 中
    int connectionState = static_cast<int>(Bluetooth::BTConnectState::DISCONNECTED);
    int ret = fdp.ConsumeIntegral<int>();
    std::string address = fdp.ConsumeRandomLengthString(17);
    BluetoothRemoteDevice device(address, 1);
    bleSendManager.gattClient_ = std::make_shared<GattClient>(device);

    // 确保设备不在 MechConnectManager 中
    MechConnectManager::GetInstance().CleanMechInfo();
    MechInfo mechInfo;
    mechInfo.mac = mac;
    mechInfo.mechName = deviceName;
    mechInfo.mechId = mechId;
    mechInfo.mechType = mechType;
    bleSendManager.OnConnectionStateChanged(connectionState, ret, mechInfo);
    // 清理
    MechConnectManager::GetInstance().CleanMechInfo();
}

void BleGattClientCallbackOnConnectionStateChangedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    std::string mac = fdp.ConsumeRandomLengthString(17);
    std::string deviceName = fdp.ConsumeRandomLengthString(32);
    int32_t mechId = fdp.ConsumeIntegral<int32_t>();
    MechType mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());
    int connectionState = fdp.ConsumeIntegral<int>();
    int ret = fdp.ConsumeIntegral<int>();

    // 场景1: 设备不在 MechConnectManager 中
    MechConnectManager::GetInstance().CleanMechInfo();
    auto cb1 = std::make_shared<BleGattClientCallback>();
    cb1->setMac(mac);
    cb1->OnConnectionStateChanged(connectionState, ret);

    // 场景2: 设备在 MechConnectManager 中
    MechInfo mechInfo;
    mechInfo.mac = mac;
    mechInfo.mechName = deviceName;
    mechInfo.mechId = mechId;
    mechInfo.mechType = mechType;
    MechConnectManager::GetInstance().AddMechInfo(mechInfo);

    std::string address = fdp.ConsumeRandomLengthString(17);
    BluetoothRemoteDevice device(address, 1);
    bleSendManager.gattClient_ = std::make_shared<GattClient>(device);

    auto cb2 = std::make_shared<BleGattClientCallback>();
    cb2->setMac(mac);
    cb2->OnConnectionStateChanged(connectionState, ret);

    // 清理
    MechConnectManager::GetInstance().CleanMechInfo();
}

void OnConnectionStateChangedFuzzTest(const uint8_t *data, size_t size)
{
    OnConnectionStateChangedFuzzTestOne(data, size);
    OnConnectionStateChangedFuzzTestTwo(data, size);
    OnConnectionStateChangedFuzzTestThree(data, size);
    OnConnectionStateChangedFuzzTestFour(data, size);
    OnConnectionStateChangedFuzzTestFive(data, size);
}

void OnServicesDiscoveredFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider fdp(data, size);
    BleSendManager& bleSendManager = BleSendManager::GetInstance();

    // 场景1: status != GATT_SUCCESS (102-104行)
    auto cb1 = std::make_shared<BleGattClientCallback>();
    int32_t integerRangeMin = -99;
    cb1->OnServicesDiscovered(fdp.ConsumeIntegralInRange<int>(integerRangeMin, 0));

    // 场景2: gattClient_ == nullptr (106-109行)
    auto cb2 = std::make_shared<BleGattClientCallback>();
    bleSendManager.gattClient_ = nullptr;
    cb2->OnServicesDiscovered(OHOS::Bluetooth::GattStatus::GATT_SUCCESS);

    // 场景3: GetService失败+完整流程 (110-138行)
    auto cb3 = std::make_shared<BleGattClientCallback>();
    int32_t randomLength = 17;
    bleSendManager.gattClient_ = std::make_shared<GattClient>(
        BluetoothRemoteDevice(fdp.ConsumeRandomLengthString(randomLength), 1));
    cb3->OnServicesDiscovered(OHOS::Bluetooth::GattStatus::GATT_SUCCESS);
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
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::OnConnectionStateChangedFuzzTest(data, size);
    OHOS::BleGattClientCallbackOnConnectionStateChangedFuzzTest(data, size);
    OHOS::OnServicesDiscoveredFuzzTest(data, size);
    OHOS::OnSetNotifyCharacteristicFuzzTest(data, size);
    OHOS::OnCharacteristicChangedFuzzTest(data, size);
    OHOS::OnCharacteristicWriteResultFuzzTest(data, size);
    OHOS::MechbodyPairFuzzTest(data, size);
    return 0;
}