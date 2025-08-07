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

#include "blesendmanager_fuzzer.h"

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

class MyBleReceiveListener : public BleReceviceListener {
public:
    MyBleReceiveListener() {}

    int32_t OnReceive(const uint8_t *data, uint32_t dataLen) override
    {
        return ERR_OK;
    }
    virtual ~MyBleReceiveListener() {}
};

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

void OnReceiveFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint8_t dataLen = fdp.ConsumeIntegral<uint8_t>();
    uint8_t dataInput[dataLen];
    for (uint8_t i = 0; i < dataLen; i++) {
        dataInput[i] = fdp.ConsumeIntegral<uint8_t>();
    }
    uint8_t *dataInputPtr = dataInput;

    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.OnReceive(dataInputPtr, size);
}

void RegisterTransportSendAdapterFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    auto listener = std::make_shared<MyBleReceiveListener>();

    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.RegisterTransportSendAdapter(listener);
    bleSendManager.RegisterTransportSendAdapter(listener);
}

void UnRegisterTransportSendAdapterFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    auto listener = std::make_shared<MyBleReceiveListener>();

    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.UnRegisterTransportSendAdapter(listener);
    bleSendManager.RegisterTransportSendAdapter(listener);
    bleSendManager.UnRegisterTransportSendAdapter(listener);
}

void CheckGattcIsReadyFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.CheckGattcIsReady();
}

void OnGattReadyFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    MechInfo mechInfo;
    bleSendManager.OnGattReady(mechInfo);
}

void InitScanParaFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.InitScanPara();
}

void OnScanCallbackFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    BleScanResult result;

    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.OnScanCallback(result);
}

void StartScanFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.StartScan();
}

void StopScanFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.StopScan();
}

void MechbodyGattcConnectFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    std::string mac = fdp.ConsumeRandomLengthString();
    std::string deviceName = fdp.ConsumeRandomLengthString();

    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.MechbodyGattcConnect(mac, deviceName);
}

void MechbodyGattcDisconnectFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    std::string address = fdp.ConsumeRandomLengthString();
    BluetoothRemoteDevice device(address, 1);

    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    MechInfo mechInfo;
    bleSendManager.MechbodyGattcDisconnect(mechInfo);
    bleSendManager.gattClient_ = std::make_shared<GattClient>(device);
    bleSendManager.MechbodyGattcDisconnect(mechInfo);
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
}

void OnScanCallbackBleCentralManagerCallbackImplFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    BleScanResult result;

    std::shared_ptr<BleCentralManagerCallbackImpl> bleCentralManagerCallback =
        std::make_shared<BleCentralManagerCallbackImpl>();
    bleCentralManagerCallback->OnScanCallback(result);
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
    OHOS::OnReceiveFuzzTest(data, size);
    OHOS::RegisterTransportSendAdapterFuzzTest(data, size);
    OHOS::UnRegisterTransportSendAdapterFuzzTest(data, size);
    OHOS::CheckGattcIsReadyFuzzTest(data, size);
    OHOS::OnGattReadyFuzzTest(data, size);
    OHOS::InitScanParaFuzzTest(data, size);
    OHOS::OnScanCallbackFuzzTest(data, size);
    OHOS::StartScanFuzzTest(data, size);
    OHOS::StopScanFuzzTest(data, size);
    OHOS::MechbodyGattcConnectFuzzTest(data, size);
    OHOS::MechbodyGattcDisconnectFuzzTest(data, size);
    OHOS::MechbodyGattcWriteCharacteristicFuzzTest(data, size);
    OHOS::SendDataFuzzTest(data, size);
    OHOS::OnScanCallbackBleCentralManagerCallbackImplFuzzTest(data, size);
    return 0;
}