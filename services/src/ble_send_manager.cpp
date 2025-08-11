/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "ble_send_manager.h"

#include <chrono>
#include <condition_variable>
#include <thread>
#include <unistd.h>
#include <atomic>

#include <sstream>
#include <sys/prctl.h>
#include "securec.h"

#include "bluetooth_ble_central_manager.h"
#include "bluetooth_gatt_client.h"
#include "bluetooth_host.h"
#include "bluetooth_errorcode.h"
#include "bt_uuid.h"
#include "c_header/ohos_bt_gatt.h"
#include "connect/mc_connect_manager.h"
#include "iservice_registry.h"
#include "mechbody_controller_log.h"
#include "system_ability_definition.h"
#include "uuid.h"
#include "mechbody_controller_service.h"

using namespace OHOS::Bluetooth;

namespace OHOS {
namespace MechBodyController {
namespace {
const std::string TAG = "BleSendManager";
const std::string TARGET_DEVICE_NAME = "TARGET_DEVICE_NAME";
constexpr long SCAN_REPORT_DELAY_MILLIS = 3 * 1000;
constexpr size_t NOTIFY_DATA_MAX_SIZE = 1024 * 1024;
constexpr int32_t MECH_SAID = 8550;
constexpr int32_t BUF_MIN_LEN = 8;
constexpr int32_t BUF_MAX_LEN = 251;
constexpr int32_t WAIT_TIME = 60;
constexpr int32_t ERROR_NO_LISTENERS = -10001;
constexpr int32_t ERROR_INVALID_PARAMETER = -10002;
constexpr int32_t MECHBODY_GATT_INVALID_PARAM = -10003;
constexpr int32_t ERROR_SIZE_TOO_LARGE = -10004;
constexpr int32_t MECHBODY_GATT_CONNECT_FAILED = -10005;
constexpr int32_t MECHBODY_HID_CONNECT_FAILED = -10006;
std::atomic<bool> g_isUnloadScheduled = false;
std::unique_ptr<std::thread> delayThread;

UUID SERVICE_UUID = UUID::FromString("15f1e600-a277-43fc-a484-dd39ef8a9100"); // GATT Service uuid
UUID MECHBODY_CHARACTERISTIC_WRITE_UUID = UUID::FromString("15f1e602-a277-43fc-a484-dd39ef8a9100"); // write uuid
UUID MECHBODY_CHARACTERISTIC_NOTIFY_UUID = UUID::FromString("15f1e603-a277-43fc-a484-dd39ef8a9100"); // notify uuid
BleSendManager& bleSendManager = BleSendManager::GetInstance();
}

BleSendManager& BleSendManager::GetInstance()
{
    static auto instance = new BleSendManager();
    return *instance;
}

BleSendManager::BleSendManager()
{}
BleSendManager::~BleSendManager()
{}

BleGattClientCallback::BleGattClientCallback()
{}
BleGattClientCallback::~BleGattClientCallback()
{}

void UnloadSystemAbility()
{
    HILOGI("start");
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        HILOGE("GetSystemAbilityManager return null.");
        return;
    }
    int32_t ret = sam->UnloadSystemAbility(MECH_SAID);
    if (ret != ERR_OK) {
        HILOGE("UnLoadSystemAbility mechbody sa failed.");
        return;
    }
    HILOGI("end");
}

void DelayedUnloadSystemAbility(std::chrono::seconds delay)
{
    HILOGI("start");
    if (g_isUnloadScheduled) {
        HILOGI("already Unload Scheduled. isUnloadScheduled = %{public}d", g_isUnloadScheduled.load());
        return;
    }
    if (delayThread != nullptr) {
        HILOGI("Thread is empty.");
        return;
    }
    delayThread = std::make_unique<std::thread>([delay]() {
        std::this_thread::sleep_for(delay);
        if (g_isUnloadScheduled) {
            HILOGI(" will Unload SystemAbility, isUnloadScheduled = %{public}d",
                g_isUnloadScheduled.load());
            UnloadSystemAbility();
        }
    });
    g_isUnloadScheduled = true;
    HILOGI("end");
}

void CancelDelayedUnload()
{
    HILOGI("start");
    if (!g_isUnloadScheduled) {
        HILOGI("No Unload Scheduled.");
        return;
    }
    g_isUnloadScheduled = false;
    HILOGI("isUnloadScheduled = %{public}d", g_isUnloadScheduled.load());
    if (delayThread && delayThread->joinable()) {
        HILOGI("delayThread is join. isUnloadScheduled = %{public}d", g_isUnloadScheduled.load());
        delayThread->detach();
    }
    HILOGI("end. isUnloadScheduled = %{public}d", g_isUnloadScheduled.load());
}

void BleGattClientCallback::OnConnectionStateChanged(int connectionState, int ret)
{
    HILOGI("BleGattClientCallback start. connectionState= %{public}d, ret= %{public}d", connectionState, ret);
    bleSendManager.OnConnectionStateChanged(connectionState, ret, mechInfo_);
    HILOGI("BleGattClientCallback end");
}

void BleGattClientCallback::OnServicesDiscovered(int status)
{
    HILOGI("called. status= %{public}d", status);
    if (status != OHOS::Bluetooth::GattStatus::GATT_SUCCESS) {
        HILOGE("OnServicesDiscovered state is not sucess");
        return;
    }
    if (bleSendManager.gattClient_ == nullptr) {
        HILOGE("gattClient_ is null");
        return;
    }
    auto mService = bleSendManager.gattClient_->GetService(SERVICE_UUID);
    if (!mService.has_value()) {
        HILOGE("Failed to get service.");
        return;
    }
    HILOGI("GetService success");
    auto notifyCharacteristic = mService->get().GetCharacteristic(MECHBODY_CHARACTERISTIC_NOTIFY_UUID);
    if (!notifyCharacteristic) {
        HILOGE("Failed to get notify characteristic.");
        return;
    }
    auto writeCharacteristic = mService->get().GetCharacteristic(MECHBODY_CHARACTERISTIC_WRITE_UUID);
    if (!writeCharacteristic) {
        HILOGE("Failed to get write  characteristic.");
        return;
    }
    int permissions = notifyCharacteristic->GetPermissions();
    int properties = notifyCharacteristic->GetProperties();
    uint16_t handle = notifyCharacteristic->GetHandle();
    GattCharacteristic characteristic = GattCharacteristic(MECHBODY_CHARACTERISTIC_NOTIFY_UUID, handle, permissions,
        properties);
    bleSendManager.gattClient_->SetNotifyCharacteristic(characteristic, true);
    int wPermissions = writeCharacteristic->GetPermissions();
    int wProperties = writeCharacteristic->GetProperties();
    uint16_t wHandle = writeCharacteristic->GetHandle();
    bleSendManager.handle_ = wHandle;
    bleSendManager.permissions_ = wPermissions;
    bleSendManager.properties_ = wProperties;
    HILOGI("end");
}

void BleGattClientCallback::OnSetNotifyCharacteristic(const GattCharacteristic &characteristic, int status)
{
    HILOGI("called. status= %{public}d", status);
    std::lock_guard<std::mutex> lock(bleSendManager.isGattReadyMtx_);
    bleSendManager.isGattReady_ = (status == 1);
    HILOGI("end. IsGattReady= %{public}d", bleSendManager.isGattReady_);

    BleSendManager::GetInstance().OnGattReady(mechInfo_);
}

void BleGattClientCallback::OnCharacteristicChanged(const GattCharacteristic &characteristic)
{
    HILOGD("called.");
    size_t size = 0;
    uint8_t *valueData = characteristic.GetValue(&size).get();
    HILOGD("size = %{public}zu", size);

    // notifydata
    BleSendManager::GetInstance().OnReceive(valueData, size);
}

void BleGattClientCallback::OnCharacteristicWriteResult(const GattCharacteristic &characteristic, int ret)
{
    HILOGI("GattClientCallback::OnCharacteristicWriteResult called ret = %{public}d\n", ret);
    if (ret == OHOS::Bluetooth::GattStatus::GATT_SUCCESS) {
        HILOGI("write success");
        size_t size = 0;
        const std::unique_ptr<uint8_t[]> &value = characteristic.GetValue(&size);
        HILOGI("size = %{public}zu", size);
        if (value == nullptr || size == 0) {
            HILOGI("value is empty or null");
            return;
        }
    } else if (ret == OHOS::Bluetooth::GattStatus::GATT_FAILURE) {
        HILOGI("write failed");
    } else if (ret == OHOS::Bluetooth::GattStatus::WRITE_NOT_PERMITTED) {
        HILOGI("write not permission");
    }
}

const MechInfo &BleGattClientCallback::GetMechInfo() const
{
    return mechInfo_;
}

void BleGattClientCallback::SetMechInfo(const MechInfo &mechInfo)
{
    mechInfo_ = mechInfo;
}

void BleSendManager::Init()
{
    HILOGI("Init start");
}

void BleSendManager::UnInit()
{
    HILOGI("UnInit start");
    if (gattClient_ != nullptr) {
        if (gattClient_->Close() != 0) {
            HILOGE("Close gattclient error");
        }
        gattClient_.reset();
        gattClient_ = nullptr;
    }

    if (bleGattClientCallBack_ != nullptr) {
        bleGattClientCallBack_.reset();
        bleGattClientCallBack_ = nullptr;
    }
}

void BleSendManager::OnConnectionStateChanged(int connectionState, int ret, MechInfo &mechInfo)
{
    HILOGI("start. connectionState= %{public}d, ret= %{public}d, isBleConnected_= %{public}d",
        connectionState, ret, isBleConnected_.load());
    if (connectionState != (int)BTConnectState::CONNECTED && connectionState != (int)BTConnectState::DISCONNECTED) {
        HILOGE("ignore, connectionState = %{public}d, connecting or disconnecting", connectionState);
    }
    if (connectionState == (int)BTConnectState::CONNECTED) {
        HILOGI("connectionState is CONNECTED");
        CancelDelayedUnload();
        isBleConnected_ = true;
        CHECK_POINTER_RETURN(gattClient_, "gattClient_");
        gattClient_->DiscoverServices();
        HILOGI("called to discover service");
        gattClient_->RequestConnectionPriority(MECH_GATT_PRIORITY_HIGH);
    } else if (connectionState == (int)BTConnectState::DISCONNECTED) {
        HILOGI("connectionState is DISCONNECTED");
        bleSendManager.OnGattDisconnect(mechInfo);
    }
    HILOGI("end");
}

int32_t BleSendManager::OnReceive(uint8_t *data, size_t size)
{
    HILOGD("OnReceive in. size = %{public}zu", size);
    if (data == nullptr) {
        HILOGE("data is null");
        return ERROR_INVALID_PARAMETER;
    }

    if (size == 0) {
        HILOGE("size is zero");
        return ERROR_INVALID_PARAMETER;
    }

    if (size > NOTIFY_DATA_MAX_SIZE) {
        HILOGE("size exceeds maximum allowed size: %{public}zu > %{public}zu", size, NOTIFY_DATA_MAX_SIZE);
        return ERROR_SIZE_TOO_LARGE;
    }

    std::lock_guard<std::mutex> lock(bleReceviceMutex_);
    if (bleReceviceListeners_.empty()) {
        HILOGE("bleReceviceListeners_ is null");
        return ERROR_NO_LISTENERS;
    }
    for (std::shared_ptr<BleReceviceListener> listener : bleReceviceListeners_) {
        if (listener == nullptr) {
            HILOGE("bleReceviceListeners_ is null");
            return ERROR_NO_LISTENERS;
        }
        listener->OnReceive(data, size);
    }
    return ERR_OK;
}

int32_t BleSendManager::RegisterTransportSendAdapter(const std::shared_ptr<BleReceviceListener> listener)
{
    if (listener == nullptr) {
        HILOGE("listener is null.");
        return ERROR_NO_LISTENERS;
    }
    std::lock_guard<std::mutex> lock(bleReceviceMutex_);
    auto it = std::find(bleReceviceListeners_.begin(), bleReceviceListeners_.end(), listener);
    if (it != bleReceviceListeners_.end()) {
        HILOGE("bleReceviceListeners_ repeat register");
        return ERR_OK;
    }

    bleReceviceListeners_.push_back(listener);
    return ERR_OK;
}

int32_t BleSendManager::UnRegisterTransportSendAdapter(const std::shared_ptr<BleReceviceListener> listener)
{
    std::lock_guard<std::mutex> lock(bleReceviceMutex_);
    if (bleReceviceListeners_.empty()) {
        HILOGE("bleReceviceListeners_ is null");
        return ERROR_NO_LISTENERS;
    }
    auto it = std::find(bleReceviceListeners_.begin(), bleReceviceListeners_.end(), listener);
    if (it == bleReceviceListeners_.end()) {
        HILOGE("bleReceviceListeners_ not found");
        return ERROR_NO_LISTENERS;
    }
    bleReceviceListeners_.erase(it);
    return ERR_OK;
}

bool BleSendManager::CheckGattcIsReady()
{
    HILOGI("in. IsGattReady=%{public}s", isGattReady_ ? "true":"false");
    return isGattReady_;
}

void BleSendManager::OnGattReady(MechInfo &mechInfo)
{
    HILOGI("called");
    mechInfo.mechName = deviceName_;
    MechConnectManager::GetInstance().NotifyMechConnect(mechInfo);
}

void BleSendManager::InitScanPara()
{
    scanParam_.settings.SetScanMode(BleScanMode::OHOS_BLE_SCAN_MODE_LOW_POWER);
    scanParam_.settings.SetReportDelay(SCAN_REPORT_DELAY_MILLIS);

    Bluetooth::BleScanFilter scanFilter;

    scanParam_.filters.clear();
    scanParam_.filters.push_back(scanFilter);

    std::shared_ptr<BleCentralManagerCallbackImpl> bleCentralManagerCallback =
        std::make_shared<BleCentralManagerCallbackImpl>();
    bleCentralManager_ = std::make_shared<Bluetooth::BleCentralManager>(bleCentralManagerCallback);
}

void BleSendManager::OnScanCallback(const Bluetooth::BleScanResult &result)
{
    HILOGI("OnScanCallback in");
    std::vector<uint8_t> advPayload = result.GetPayload();
    std::string address = result.GetPeripheralDevice().GetDeviceAddr();
    std::string deviceName = result.GetPeripheralDevice().GetDeviceName();
    if (deviceName == TARGET_DEVICE_NAME) {
        address_ = address;
        this->StopScan();
        int32_t ret = MechbodyGattcConnect(address, deviceName);
        if (ret != ERR_OK) {
            HILOGE("MechbodyGattcConnect failed, status= %{public}d", ret);
        }
    }
}

void BleSendManager::StartScan()
{
    HILOGI("startScan start");
    if (bleCentralManager_ == nullptr) {
        HILOGE("RemoteLogBleScanner StartScan bleCentralManager_ is empty");
        return;
    }
    this->StopScan();
    int32_t status = bleCentralManager_->StartScan(scanParam_.settings, scanParam_.filters);
    if (status == Bluetooth::BtErrCode::BT_NO_ERROR) {
        HILOGI("BleCentralManager startScan success");
    } else {
        HILOGE("BleCentralManager StartScan failed code = %{public}d.", status);
    }
    HILOGI("startScan end");
}

void BleSendManager::StopScan()
{
    if (bleCentralManager_ == nullptr) {
        HILOGE("StopScan bleCentralManager_ is empty");
        return;
    }
    int ret = bleCentralManager_->StopScan();
    HILOGI("StopScan ret: %{public}d", ret);
}

int32_t BleSendManager::MechbodyGattcConnect(std::string mac, std::string deviceName)
{
    HILOGI("MechbodyGattcConnect connect begin");
    address_ = mac;
    deviceName_ = deviceName;
    std::size_t hash = std::hash<std::string>()(mac);
    std::string macHash = std::to_string(hash);
    if (bleGattClientCallBack_ != nullptr && bleGattClientCallBack_->GetMechInfo().macHash == macHash) {
        HILOGI("device has connected.");
        return ERR_OK;
    }
    BluetoothRemoteDevice device(address_, 1);
    gattClient_ = std::make_shared<OHOS::Bluetooth::GattClient>(device);
    bleGattClientCallBack_ = std::make_shared<BleGattClientCallback>();
    if (gattClient_ == nullptr || bleGattClientCallBack_ == nullptr) {
        HILOGE("gattClient_ is nullptr or bleGattClientCallBack_ is nullptr");
        return MECHBODY_GATT_INVALID_PARAM;
    }
    auto mechInfo = bleGattClientCallBack_->GetMechInfo();
    mechInfo.macHash = macHash;
    bleGattClientCallBack_->SetMechInfo(mechInfo);
    gattClient_->Init();
    int result = gattClient_->Connect(bleGattClientCallBack_, false, BT_TRANSPORT_BLE);
    if (result != 0) {
        HILOGE("Failed to connect, result: %{public}d", result);
        return MECHBODY_GATT_CONNECT_FAILED;
    }

    // HID connect
    int res = Bluetooth::BluetoothHost::GetDefaultHost().ConnectAllowedProfiles(address_);
    if (result != 0) {
        HILOGI("BluetoothHost end, ConnectAllowedProfiles res: %{public}d", res);
        return MECHBODY_HID_CONNECT_FAILED;
    }

    return result;
}

void BleSendManager::OnGattDisconnect(MechInfo &mechInfo)
{
    HILOGI("begin. isBleConnected_ = %{public}d", isBleConnected_.load());
    if (isBleConnected_) {
        HILOGI("Gatt disconnect. Delayed Unload SystemAbility");
        MechConnectManager::GetInstance().NotifyMechDisconnect(mechInfo);

        DelayedUnloadSystemAbility(std::chrono::seconds(WAIT_TIME));
    }

    isBleConnected_ = false;

    HILOGI("destruct bleGattClientCallBack_ start");
    if (gattClient_ != nullptr) {
        if (gattClient_->Close() != 0) {
            HILOGE("Close gattclient error");
        }
        gattClient_.reset();
        gattClient_ = nullptr;
    }

    if (bleGattClientCallBack_ != nullptr) {
        bleGattClientCallBack_.reset();
        bleGattClientCallBack_ = nullptr;
    }
    HILOGI("end and destruct bleGattClientCallBack_ end");
}

int32_t BleSendManager::MechbodyGattcDisconnect(MechInfo mechInfo)
{
    HILOGI("begin");
    if (gattClient_ == nullptr) {
        HILOGE("gattClient_ is nullptr");
        return MECHBODY_GATT_INVALID_PARAM;
    }
    int status = gattClient_->Disconnect();
    HILOGI("BleGattcDisconnect info, status: %{public}d", status);
    if (status != OHOS_BT_STATUS_SUCCESS) {
        HILOGE("BleGattcDisconnect error, status: %{public}d", status);
        return MECHBODY_GATT_INVALID_PARAM;
    }
    HILOGI("OnGattDisconnect will enter.");
    OnGattDisconnect(mechInfo);
    HILOGI("end");
    return status;
}

int32_t BleSendManager::MechbodyGattcWriteCharacteristic(uint8_t *data, uint32_t dataLen)
{
    HILOGI("WriteCharacteristic start");
    int32_t result = -1;
    if (data == nullptr || dataLen <= BUF_MIN_LEN) {
        HILOGE("data is nullptr or len is not enough");
        return MECHBODY_GATT_INVALID_PARAM;
    }
    if (gattClient_ == nullptr) {
        HILOGE("gattClient_ is nullptr");
        return MECHBODY_GATT_INVALID_PARAM;
    }
    GattCharacteristic wrCharacteristic = GattCharacteristic(MECHBODY_CHARACTERISTIC_WRITE_UUID,
        handle_, permissions_, properties_);
    wrCharacteristic.SetValue(data, dataLen);
    wrCharacteristic.SetWriteType(GattCharacteristic::WriteType::DEFAULT);
    HILOGI("Mech writeC");
    result = gattClient_->WriteCharacteristic(wrCharacteristic);
    HILOGI("Mech writeC, result = %{public}d handle: %{public}d dataLen: %{public}d \n", \
        result, handle_, dataLen);
    return result;
}

int32_t BleSendManager::SendData(uint8_t *data, uint32_t dataLen)
{
    HILOGI("ble send start.");
    if (data == NULL) {
        HILOGE("ble client send data failed, invalia param, data is null");
        return MECHBODY_GATT_INVALID_PARAM;
    }
    if (!(dataLen >= BUF_MIN_LEN && dataLen <= BUF_MAX_LEN)) {
        HILOGE("ble client send data failed, data length is out of range, data len: %{public}d", dataLen);
        return MECHBODY_GATT_INVALID_PARAM;
    }
    int32_t result = MechbodyGattcWriteCharacteristic(data, dataLen);
    HILOGI("write SendData dataLen= %{public}d, result = %{public}d", dataLen, result);
    return result;
}

void BleCentralManagerCallbackImpl::OnScanCallback(const Bluetooth::BleScanResult &result)
{
    HILOGI("ble impl OnScanCallback start");
    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.OnScanCallback(result);
    HILOGI("ble impl OnScanCallback end");
}
} // namespace MechBodyController
} // namespace OHOS