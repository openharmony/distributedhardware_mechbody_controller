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

#ifndef BLE_SEND_MANAGER_H
#define BLE_SEND_MANAGER_H

#include <string>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <cstdint>

#include "event_handler.h"

#include "bluetooth_ble_central_manager.h"
#include "bluetooth_gatt_client.h"
#include "mechbody_controller_types.h"

namespace OHOS {
namespace MechBodyController {

typedef enum {
    MECH_GATT_PRIORITY_BALANCED = 0x0,
    MECH_GATT_PRIORITY_HIGH,
    MECH_GATT_PRIORITY_LOW_POWER,
} MechBleGattPriority;

struct BleScanParam {
    std::vector<OHOS::Bluetooth::BleScanFilter> filters;
    OHOS::Bluetooth::BleScanSettings settings;
};

class BleReceviceListener {
public:
    virtual int32_t OnReceive(const uint8_t *data, uint32_t dataLen) = 0;
};

class BleGattClientCallback : public OHOS::Bluetooth::GattClientCallback {
public:
    BleGattClientCallback();
    ~BleGattClientCallback();
    void OnConnectionStateChanged(int connectionState, int ret);
    void OnCharacteristicWriteResult(const OHOS::Bluetooth::GattCharacteristic &characteristic, int ret);
    void OnServicesDiscovered(int status);
    void OnSetNotifyCharacteristic(const OHOS::Bluetooth::GattCharacteristic &characteristic, int status);
    void OnCharacteristicChanged(const OHOS::Bluetooth::GattCharacteristic &characteristic);

    const MechInfo &GetMechInfo() const;

    void SetMechInfo(const MechInfo &mechInfo);

private:
    MechInfo mechInfo_;
};

class BleSendManager {
public:
    static BleSendManager& GetInstance();
private:
    BleSendManager(const BleSendManager&) = delete;
    BleSendManager& operator= (const BleSendManager&) = delete;
    BleSendManager(BleSendManager&&) = delete;
    BleSendManager& operator= (BleSendManager&&) = delete;

public:
    BleSendManager();
    ~BleSendManager();

    void Init();
    void UnInit();

    void OnScanCallback(const Bluetooth::BleScanResult &result);

    int32_t SendData(uint8_t *data, uint32_t dataLen);
    int32_t OnReceive(uint8_t *data, size_t size);
    void OnConnectionStateChanged(int connectionState, int ret, MechInfo &mechInfo);
    bool CheckGattcIsReady();
    int32_t RegisterTransportSendAdapter(const std::shared_ptr<BleReceviceListener> listener);
    int32_t UnRegisterTransportSendAdapter(const std::shared_ptr<BleReceviceListener> listener);
    void OnGattReady(MechInfo &mechInfo);
    void OnGattDisconnect(MechInfo &mechInfo);
    int32_t MechbodyGattcConnect(std::string mac, std::string deviceName);
    int32_t MechbodyGattcDisconnect(MechInfo mechInfo);
    bool isGattReady_ = false;
    std::mutex isGattReadyMtx_;

    std::shared_ptr<OHOS::Bluetooth::GattClient> gattClient_ {nullptr};
    std::shared_ptr<BleGattClientCallback> bleGattClientCallBack_ {nullptr};
    /* write Character record */
    uint16_t handle_ = 0;
    int32_t permissions_ = 0;
    int32_t properties_ = 0;

private:
    void InitScanPara();
    void StartScan();
    void StopScan();

    // send
    int32_t MechbodyGattcWriteCharacteristic(uint8_t *data, uint32_t dataLen);

private:
    std::shared_ptr<Bluetooth::BleCentralManager> bleCentralManager_ = nullptr;
    BleScanParam scanParam_;
    std::string address_;
    std::string deviceName_;
    std::mutex bleReceviceMutex_;
    std::vector<std::shared_ptr<BleReceviceListener>> bleReceviceListeners_;
    std::atomic<bool> isBleConnected_ = false;
};

class BleCentralManagerCallbackImpl : public Bluetooth::BleCentralManagerCallback {
public:
    BleCentralManagerCallbackImpl() = default;
    ~BleCentralManagerCallbackImpl() = default;

    void OnScanCallback(const Bluetooth::BleScanResult &result) override;
    void OnFoundOrLostCallback(const Bluetooth::BleScanResult &result, uint8_t callbackType) override
    {}
    void OnBleBatchScanResultsEvent(const std::vector<Bluetooth::BleScanResult> &results) override
    {}
    void OnStartOrStopScanEvent(int32_t resultCode, bool isStartScan) override
    {}
    void OnNotifyMsgReportFromLpDevice(const Bluetooth::UUID &btUuid, int msgType,
     const std::vector<uint8_t> &value) override
    {}
};
} // namespace MechBodyController
} // namespace OHOS
#endif // BLE_SEND_MANAGER_H
