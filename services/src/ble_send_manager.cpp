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

#include "c_header/ohos_bt_gatt.h"
#include "connect/mc_connect_manager.h"
#include "iservice_registry.h"

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
constexpr int32_t WAIT_TIME_TEN = 10;
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
    HILOGI("MECHBODY_EXEC_CONNECT BleGattClientCallback start. connectionState= %{public}d, ret= %{public}d",
        connectionState, ret);
    MechInfo mechInfo;
    MechConnectManager::GetInstance().GetMechInfo(mac, mechInfo);
    bleSendManager.OnConnectionStateChanged(connectionState, ret, mechInfo);
    HILOGI("MECHBODY_EXEC_CONNECT BleGattClientCallback end");
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
    MechInfo mechInfo;
    if (MechConnectManager::GetInstance().GetMechInfo(mac, mechInfo) != ERR_OK) {
        HILOGI("MECHBODY_EXEC_CONNECT Can not found mech info for mac: %{public}s", GetAnonymStr(mac).c_str());
        return;
    }
    HILOGI("called. status= %{public}d", status);
    std::lock_guard<std::mutex> lock(bleSendManager.isGattReadyMtx_);
    bleSendManager.isGattReady_ = (status == 1);
    HILOGI("end. IsGattReady= %{public}d", bleSendManager.isGattReady_);

    BleSendManager::GetInstance().OnGattReady(mechInfo);
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

const std::string &BleGattClientCallback::getMac() const
{
    return mac;
}

void BleGattClientCallback::setMac(const std::string &mac)
{
    BleGattClientCallback::mac = mac;
}

void BleSendManager::Init()
{
    HILOGI("Init start");
    {
        std::lock_guard<std::mutex> lock(observerRegisterMutex_);
        if (observer_ == nullptr) {
            observer_ = std::make_shared<HidObserver>();
            Bluetooth::HidHost::GetProfile()->RegisterObserver(observer_);
            HILOGI("MECHBODY_EXEC_CONNECT hid RegisterObserver registed");
        }
        if (hostObserver_ == nullptr) {
            hostObserver_ = std::make_shared<HostObserver>();
            Bluetooth::BluetoothHost::GetDefaultHost().RegisterRemoteDeviceObserver(hostObserver_);
            HILOGI("MECHBODY_EXEC_CONNECT host RegisterObserver registed");
        }
    }
    auto runner = AppExecFwk::EventRunner::Create("BleSenderManager");
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
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
}

void BleSendManager::OnConnectionStateChanged(int connectionState, int ret, MechInfo &mechInfo)
{
    HILOGI("MECHBODY_EXEC_CONNECT start. connectionState= %{public}d, ret= %{public}d",
        connectionState, ret);
    if (connectionState != (int)BTConnectState::CONNECTED && connectionState != (int)BTConnectState::DISCONNECTED) {
        HILOGE("MECHBODY_EXEC_CONNECT ignore, connectionState = %{public}d, connecting or disconnecting",
            connectionState);
        return;
    }
    if (connectionState == (int)BTConnectState::CONNECTED) {
        HILOGI("MECHBODY_EXEC_CONNECT connectionState is CONNECTED");
        CancelDelayedUnload();
        CHECK_POINTER_RETURN(gattClient_, "gattClient_");
        gattClient_->DiscoverServices();
        HILOGI("MECHBODY_EXEC_CONNECT called to discover service");
        gattClient_->RequestConnectionPriority(MECH_GATT_PRIORITY_HIGH);
        return;
    } else if (connectionState == (int)BTConnectState::DISCONNECTED) {
        HILOGI("MECHBODY_EXEC_CONNECT connectionState is DISCONNECTED");
        {
            std::unique_lock<std::mutex> lock(gattDisconnMutex_);
            MechConnectManager::GetInstance().SetMechanicGattState(mechInfo.mac, false);
            gattDisconnCv_.notify_all();
        }
        HILOGI("MECHBODY_EXEC_CONNECT Tracking uninit start.");
        McCameraTrackingController::GetInstance().UnInit();
        HILOGI("MECHBODY_EXEC_CONNECT Tracking uninit finish.");
        MechbodyDisConnect(mechInfo);
    }
    HILOGI("MECHBODY_EXEC_CONNECT end");
}

void BleSendManager::OnPairStateChanged(int pairState, int cause, MechInfo &mechInfo)
{
    HILOGI("MECHBODY_EXEC_CONNECT device Pair Status change. mech info: %{public}s; "
        "status: %{public}d; cause: %{public}d;", mechInfo.ToString().c_str(), pairState, cause);
    if (pairState == Bluetooth::PAIR_PAIRED) {
        HILOGI("MECHBODY_EXEC_CONNECT Set Device Alias start.");
        Bluetooth::BluetoothRemoteDevice remoteDevice =
            Bluetooth::BluetoothRemoteDevice(mechInfo.mac, Bluetooth::BT_TRANSPORT_BLE);
        std::string showName = mechInfo.mechName + "-mechanic";
        int32_t res = remoteDevice.SetDeviceAlias(showName);
        HILOGI("MECHBODY_EXEC_CONNECT SetDeviceAlias end. result: %{public}d", res);

        std::unique_lock<std::mutex> lock(pairMutex_);
        MechConnectManager::GetInstance().SetMechanicPairState(mechInfo.mac, true);
        pairCv_.notify_all();
    }
}

void BleSendManager::OnHidStateChanged(int hidState, int cause, MechInfo &mechInfo)
{
    HILOGI("MECHBODY_EXEC_CONNECT device HID connect state change. "
        "mech info: %{public}s; state: %{public}d", mechInfo.ToString().c_str(), hidState);
    if (hidState == static_cast<int>(Bluetooth::BTConnectState::DISCONNECTED)) {
        std::unique_lock<std::mutex> lock(hidDisconnMutex_);
        MechConnectManager::GetInstance().SetMechanicHidState(mechInfo.mac, false);
        hidDisconnCv_.notify_all();

        MechbodyDisConnect(mechInfo);
    } else if (hidState == static_cast<int>(Bluetooth::BTConnectState::CONNECTED)) {
        std::unique_lock<std::mutex> lock(hidMutex_);
        MechConnectManager::GetInstance().SetMechanicHidState(mechInfo.mac, true);
        hidCv_.notify_all();
    }
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
    // LCOV_EXCL_START
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
    // LCOV_EXCL_STOP
}

int32_t BleSendManager::UnRegisterTransportSendAdapter(const std::shared_ptr<BleReceviceListener> listener)
{
    // LCOV_EXCL_START
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
    // LCOV_EXCL_STOP
}

bool BleSendManager::CheckGattcIsReady()
{
    // LCOV_EXCL_START
    HILOGI("in. IsGattReady=%{public}s", isGattReady_ ? "true":"false");
    return isGattReady_;
    // LCOV_EXCL_STOP
}

void BleSendManager::OnGattReady(MechInfo &mechInfo)
{
    HILOGI("MECHBODY_EXEC_CONNECT called");
    MechConnectManager::GetInstance().NotifyMechConnect(mechInfo);
    {
        std::unique_lock<std::mutex> lock(gattMutex_);
        MechConnectManager::GetInstance().SetMechanicGattState(mechInfo.mac, true);
        gattCv_.notify_all();
    }
    HILOGI("MECHBODY_EXEC_CONNECT Tracking init start.");
    McCameraTrackingController::GetInstance().Init();
    HILOGI("MECHBODY_EXEC_CONNECT Tracking init finish.");
}

void BleSendManager::InitScanPara()
{
    // LCOV_EXCL_START
    scanParam_.settings.SetScanMode(BleScanMode::OHOS_BLE_SCAN_MODE_LOW_POWER);
    scanParam_.settings.SetReportDelay(SCAN_REPORT_DELAY_MILLIS);

    Bluetooth::BleScanFilter scanFilter;

    scanParam_.filters.clear();
    scanParam_.filters.push_back(scanFilter);

    std::shared_ptr<BleCentralManagerCallbackImpl> bleCentralManagerCallback =
        std::make_shared<BleCentralManagerCallbackImpl>();
    bleCentralManager_ = std::make_shared<Bluetooth::BleCentralManager>(bleCentralManagerCallback);
    // LCOV_EXCL_STOP
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
    // LCOV_EXCL_START
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
    // LCOV_EXCL_STOP
}

void BleSendManager::StopScan()
{
    // LCOV_EXCL_START
    if (bleCentralManager_ == nullptr) {
        HILOGE("StopScan bleCentralManager_ is empty");
        return;
    }
    int ret = bleCentralManager_->StopScan();
    HILOGI("StopScan ret: %{public}d", ret);
    // LCOV_EXCL_STOP
}

int32_t BleSendManager::MechbodyConnect(std::string mac, std::string deviceName)
{
    HILOGI("MECHBODY_EXEC_CONNECT satrt");
    auto connFunc = [this, mac, deviceName]() {
        if (MechConnectManager::GetInstance().IsConnect()) {
            HILOGW("MECHBODY_EXEC_CONNECT mechInfos has connected.");
            return;
        }
        MechInfo mechInfo;
        mechInfo.mac = mac;
        mechInfo.mechName = deviceName;
        if (MechConnectManager::GetInstance().GetMechInfo(mechInfo.mac, mechInfo) != ERR_OK) {
            if (MechConnectManager::GetInstance().AddMechInfo(mechInfo) != ERR_OK) {
                HILOGE("MECHBODY_EXEC_CONNECT save machInfo for mac: %{public}s", mechInfo.ToString().c_str());
                return;
            }
        }
        HILOGI("MECHBODY_EXEC_CONNECT async connect start, mech info for: %{public}s", mechInfo.ToString().c_str());
        if (mechInfo.gattCoonectState) {
            HILOGE("MECHBODY_EXEC_CONNECT mech has connected: %{public}s", GetAnonymStr(mechInfo.mac).c_str());
            return;
        }
        int32_t gattRet = ERR_OK;
        do {
            gattRet = MechbodyGattcConnect(mechInfo.mac, mechInfo.mechName);
            if (gattRet != ERR_OK) {
                HILOGE("MECHBODY_EXEC_CONNECT gatt connect failed mechInfo: %{public}s", mechInfo.ToString().c_str());
                break;
            }
            int32_t ret = MechbodyPair(mechInfo.mac, mechInfo.mechName);
            if (ret != ERR_OK) {
                HILOGE("MECHBODY_EXEC_CONNECT make pair failed mechInfo: %{public}s", mechInfo.ToString().c_str());
            }
            ret = MechbodyHidConnect(mechInfo.mac, mechInfo.mechName);
            if (ret != ERR_OK) {
                HILOGE("MECHBODY_EXEC_CONNECT hid connect failed mechInfo: %{public}s", mechInfo.ToString().c_str());
            }
        } while (false);

        if (gattRet != ERR_OK) {
            HILOGI("MECHBODY_EXEC_CONNECT async connect rollback, mechInfo: %{public}s", mechInfo.ToString().c_str());
            MechbodyDisConnectSync(mechInfo);
        }
        HILOGI("MECHBODY_EXEC_CONNECT async connect end, mech info for: %{public}s", mechInfo.ToString().c_str());
    };

    if (eventHandler_ != nullptr) {
        eventHandler_->PostTask(connFunc);
    }
    HILOGI("MECHBODY_EXEC_CONNECT end");
    return ERR_OK;
}

int32_t BleSendManager::MechbodyGattcConnect(std::string mac, std::string deviceName)
{
    HILOGI("MECHBODY_EXEC_CONNECT connect begin, mac: %{public}s", GetAnonymStr(mac).c_str());
    address_ = mac;
    deviceName_ = deviceName;
    BluetoothRemoteDevice device(mac, 1);
    gattClient_ = std::make_shared<OHOS::Bluetooth::GattClient>(device);
    bleGattClientCallBack_ = std::make_shared<BleGattClientCallback>();
    bleGattClientCallBack_->setMac(mac);
    gattClient_->Init();
    int result = gattClient_->Connect(bleGattClientCallBack_, false, BT_TRANSPORT_BLE);
    if (result != 0) {
        HILOGE("MECHBODY_EXEC_CONNECT Failed to connect, result: %{public}d", result);
        return MECHBODY_GATT_CONNECT_FAILED;
    }

    std::unique_lock<std::mutex> lock(gattMutex_);
    if (!gattCv_.wait_for(lock, std::chrono::seconds(WAIT_TIME_TEN),
                          [this, mac]() mutable {
                              bool gattState = false;
                              MechConnectManager::GetInstance().GetMechanicGattState(mac, gattState);
                              return gattState;
                          })) {
        HILOGE("MECHBODY_EXEC_CONNECT wait for gatt connect timeout");
        return MECHBODY_GATT_CONNECT_FAILED;
    }
    HILOGI("MECHBODY_EXEC_CONNECT connect end mac: %{public}s", GetAnonymStr(mac).c_str());
    return ERR_OK;
}

int32_t BleSendManager::MechbodyPair(std::string &mac, std::string &deviceName)
{
    HILOGI("MECHBODY_EXEC_CONNECT start, mac: %{public}s, device name: %{public}s;",
        GetAnonymStr(mac).c_str(), GetAnonymStr(deviceName).c_str());
    Bluetooth::BluetoothRemoteDevice device =
            Bluetooth::BluetoothRemoteDevice(mac, Bluetooth::BT_TRANSPORT_BLE);
    int32_t res = device.StartCrediblePair();
    HILOGI("MECHBODY_EXEC_CONNECT end. CrediblePair result: %{public}d", res);

    std::unique_lock<std::mutex> lock(pairMutex_);
    if (!pairCv_.wait_for(lock, std::chrono::seconds(WAIT_TIME_TEN),
                          [this, mac]()mutable {
                              bool pairState = false;
                              MechConnectManager::GetInstance().GetMechanicPairState(mac, pairState);
                              return pairState;
                          })) {
        HILOGE("MECHBODY_EXEC_CONNECT wait for pair timeout");
        return MECHBODY_GATT_CONNECT_FAILED;
    }
    HILOGI("MECHBODY_EXEC_CONNECT end, mac: %{public}s, device name: %{public}s;",
        GetAnonymStr(mac).c_str(), GetAnonymStr(deviceName).c_str());
    return ERR_OK;
}

int32_t BleSendManager::MechbodyHidConnect(std::string &mac, std::string &deviceName)
{
    HILOGI("MECHBODY_EXEC_CONNECT start, mac: %{public}s, device name: %{public}s;",
        GetAnonymStr(mac).c_str(), GetAnonymStr(deviceName).c_str());
    Bluetooth::BluetoothRemoteDevice remoteDevice = Bluetooth::BluetoothRemoteDevice(mac);
    int32_t res = Bluetooth::HidHost::GetProfile()->Connect(remoteDevice);
    std::unique_lock<std::mutex> lock(hidMutex_);
    if (!pairCv_.wait_for(lock, std::chrono::seconds(WAIT_TIME_TEN),
                          [this, mac]()mutable {
                              bool hidState = false;
                              MechConnectManager::GetInstance().GetMechanicHidState(mac, hidState);
                              return hidState;
                          })) {
        HILOGE("MECHBODY_EXEC_CONNECT wait for hid timeout");
        return MECHBODY_GATT_CONNECT_FAILED;
    }
    HILOGI("MECHBODY_EXEC_CONNECT end, mac: %{public}s, device name: %{public}s;",
        GetAnonymStr(mac).c_str(), GetAnonymStr(deviceName).c_str());
    return res;
}

int32_t BleSendManager::MechbodyDisConnect(MechInfo &mechInfo)
{
    HILOGI("MECHBODY_EXEC_DISCONNECT start, mech info for: %{public}s", mechInfo.ToString().c_str());
    auto connFunc = [this, mechInfo]() mutable {
        if (MechConnectManager::GetInstance().GetMechanicGattState(
            mechInfo.mac, mechInfo.gattCoonectState) != ERR_OK) {
            HILOGE("MECHBODY_EXEC_DISCONNECT Can not find mech info for amc: %{public}s",
                GetAnonymStr(mechInfo.mac).c_str());
            return;
        }
        HILOGI("MECHBODY_EXEC_DISCONNECT async disconnect start, mech info for: %{public}s",
            mechInfo.ToString().c_str());
        if (!mechInfo.gattCoonectState) {
            OnGattDisconnect(mechInfo);
            HILOGE("MECHBODY_EXEC_DISCONNECT Device has already disconnected for mechInfo: %{public}s",
                mechInfo.ToString().c_str());
            return;
        }
        MechbodyDisConnectSync(mechInfo);
        HILOGI("MECHBODY_EXEC_DISCONNECT async disconnect end, mech info for: %{public}s", mechInfo.ToString().c_str());
    };

    if (eventHandler_ != nullptr) {
        eventHandler_->PostTask(connFunc);
    }
    HILOGI("MECHBODY_EXEC_DISCONNECT end, mech info for: %{public}s", mechInfo.ToString().c_str());
    return ERR_OK;
}

int32_t BleSendManager::MechbodyDisConnectSync(MechInfo &mechInfo)
{
    HILOGI("MECHBODY_EXEC_DISCONNECT start, mech info for: %{public}s", mechInfo.ToString().c_str());
    int32_t ret = ERR_OK;
    ret = MechbodyHidDisconnect(mechInfo);
    if (ret != ERR_OK) {
        HILOGE("MECHBODY_EXEC_DISCONNECT hid disconnect failed for mechInfo: %{public}s",
            mechInfo.ToString().c_str());
    }
    ret = MechbodyGattcDisconnect(mechInfo);
    if (ret != ERR_OK) {
        HILOGE("MECHBODY_EXEC_DISCONNECT gatt disconnect failed for mechInfo: %{public}s",
            mechInfo.ToString().c_str());
    }
    HILOGI("MECHBODY_EXEC_DISCONNECT end, mech info for: %{public}s", mechInfo.ToString().c_str());
    return ret;
}

void BleSendManager::OnGattDisconnect(MechInfo &mechInfo)
{
    MechConnectManager::GetInstance().NotifyMechDisconnect(mechInfo);
    DelayedUnloadSystemAbility(std::chrono::seconds(WAIT_TIME));

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
    HILOGI("MECHBODY_EXEC_DISCONNECT begin");
    if (gattClient_ == nullptr) {
        HILOGE("gattClient_ is nullptr");
        return MECHBODY_GATT_INVALID_PARAM;
    }
    int status = gattClient_->Disconnect();
    HILOGI("MECHBODY_EXEC_DISCONNECT BleGattcDisconnect info, status: %{public}d", status);
    if (status != OHOS_BT_STATUS_SUCCESS) {
        HILOGE("MECHBODY_EXEC_DISCONNECT BleGattcDisconnect error, status: %{public}d", status);
        return MECHBODY_GATT_INVALID_PARAM;
    }
    HILOGI("MECHBODY_EXEC_DISCONNECT OnGattDisconnect will enter.");

    std::unique_lock<std::mutex> lock(gattDisconnMutex_);
    if (!gattDisconnCv_.wait_for(lock, std::chrono::seconds(WAIT_TIME_TEN),
        [this, mechInfo]() mutable {
            bool gattState = true;
            MechConnectManager::GetInstance().GetMechanicGattState(mechInfo.mac, gattState);
            return !gattState;
        })) {
        HILOGE("MECHBODY_EXEC_DISCONNECT wait for gatt connect timeout");
        return MECHBODY_GATT_CONNECT_FAILED;
    }
    OnGattDisconnect(mechInfo);
    HILOGI("MECHBODY_EXEC_DISCONNECT disconnect end mechInfo: %{public}s", mechInfo.ToString().c_str());
    return status;
}

int32_t BleSendManager::MechbodyHidDisconnect(MechInfo &mechInfo)
{
    HILOGI("MECHBODY_EXEC_DISCONNECT start");
    Bluetooth::BluetoothRemoteDevice remoteDevice =
            Bluetooth::BluetoothRemoteDevice(mechInfo.mac, Bluetooth::BT_TRANSPORT_BLE);
    int32_t res = Bluetooth::HidHost::GetProfile()->Disconnect(remoteDevice);
    HILOGI("MECHBODY_EXEC_DISCONNECT end. hid Disconnect result: %{public}d;", res);

    std::unique_lock<std::mutex> lock(hidDisconnMutex_);
    if (!hidDisconnCv_.wait_for(lock, std::chrono::seconds(WAIT_TIME_TEN),
        [this, mechInfo]() mutable {
            bool hidState = true;
            MechConnectManager::GetInstance().GetMechanicHidState(mechInfo.mac, hidState);
            return !hidState;
        })) {
        HILOGE("MECHBODY_EXEC_DISCONNECT wait for hid disconnect timeout");
        return ERR_OK;
    }
    HILOGI("MECHBODY_EXEC_DISCONNECT disconnect end mechInfo: %{public}s", mechInfo.ToString().c_str());
    HILOGI("MECHBODY_EXEC_CONNECT RemovePair start.");
    res = Bluetooth::BluetoothHost::GetDefaultHost().RemovePair(remoteDevice);
    HILOGI("MECHBODY_EXEC_CONNECT RemovePair end, result: %{public}d", res);
    return ERR_OK;
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
    HILOGD("ble send start.");
    if (data == NULL) {
        HILOGE("ble client send data failed, invalia param, data is null");
        return MECHBODY_GATT_INVALID_PARAM;
    }
    if (!(dataLen >= BUF_MIN_LEN && dataLen <= BUF_MAX_LEN)) {
        HILOGE("ble client send data failed, data length is out of range, data len: %{public}d", dataLen);
        return MECHBODY_GATT_INVALID_PARAM;
    }
    int32_t result = MechbodyGattcWriteCharacteristic(data, dataLen);
    HILOGD("write SendData dataLen= %{public}d, result = %{public}d", dataLen, result);
    return result;
}

void BleCentralManagerCallbackImpl::OnScanCallback(const Bluetooth::BleScanResult &result)
{
    HILOGI("ble impl OnScanCallback start");
    BleSendManager& bleSendManager = BleSendManager::GetInstance();
    bleSendManager.OnScanCallback(result);
    HILOGI("ble impl OnScanCallback end");
}

void HostObserver::OnAclStateChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device,
    int state, unsigned int reason)
{}

void HostObserver::OnPairStatusChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device, int status, int cause)
{
    std::string address = device.GetDeviceAddr();
    HILOGI("MECHBODY_EXEC_DISCONNECT address: %{public}s;", GetAnonymStr(address).c_str());
    MechInfo mechInfo;
    if (MechConnectManager::GetInstance().GetMechInfo(address, mechInfo) != ERR_OK) {
        HILOGI("MECHBODY_EXEC_DISCONNECT The equipment is not covered by this service. "
            "mac: %{public}s; state: %{public}d", GetAnonymStr(address).c_str(), status);
        return;
    }
    BleSendManager::GetInstance().OnPairStateChanged(status, cause, mechInfo);
}

void HostObserver::OnRemoteUuidChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device,
    const std::vector<Bluetooth::ParcelUuid> &uuids)
{}

void HostObserver::OnRemoteNameChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device,
    const std::string &deviceName)
{}

void HostObserver::OnRemoteAliasChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device, const std::string &alias)
{}

void HostObserver::OnRemoteCodChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device,
    const OHOS::Bluetooth::BluetoothDeviceClass &cod)
{}

void HostObserver::OnRemoteBatteryLevelChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device, int batteryLevel)
{}

void HostObserver::OnReadRemoteRssiEvent(const OHOS::Bluetooth::BluetoothRemoteDevice &device, int rssi, int status)
{}

void HostObserver::OnRemoteBatteryChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device,
    const OHOS::Bluetooth::DeviceBatteryInfo &batteryInfo)
{}

void HostObserver::OnRemoteDeviceCommonInfoReport(const OHOS::Bluetooth::BluetoothRemoteDevice &device,
    const std::vector<uint8_t> &value)
{}

void HidObserver::OnConnectionStateChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device, int state, int cause)
{
    std::string address = device.GetDeviceAddr();
    HILOGI("MECHBODY_EXEC_DISCONNECT address: %{public}s;", GetAnonymStr(address).c_str());
    MechInfo mechInfo;
    if (MechConnectManager::GetInstance().GetMechInfo(address, mechInfo) != ERR_OK) {
        HILOGI("MECHBODY_EXEC_DISCONNECT The equipment is not covered by this service. "
            "mac: %{public}s; state: %{public}d", GetAnonymStr(address).c_str(), state);
        return;
    }
    BleSendManager::GetInstance().OnHidStateChanged(state, cause, mechInfo);
}

} // namespace MechBodyController
} // namespace OHOS