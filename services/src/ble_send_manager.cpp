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
#include "hisysevent_utils.h"

using namespace OHOS::Bluetooth;

namespace OHOS {
namespace MechBodyController {
namespace {
const std::string TAG = "BleSendManager";
const std::string TARGET_DEVICE_NAME = "TARGET_DEVICE_NAME";
const std::string DEVICE_NAME_SUFFIX = "-mechanic";
constexpr long SCAN_REPORT_DELAY_MILLIS = 3 * 1000;
constexpr size_t NOTIFY_DATA_MAX_SIZE = 1024 * 1024;
constexpr int32_t MECH_SAID = 8550;
constexpr int32_t BUF_MIN_LEN = 8;
constexpr int32_t BUF_MAX_LEN = 251;
constexpr int32_t ABILITY_UNLOAD_WAIT_TIME = 60000; // ms
std::string g_abilityUnloadTaskName = "abilityUnloadTaskName";
constexpr int32_t ERROR_NO_LISTENERS = -10001;
constexpr int32_t ERROR_INVALID_PARAMETER = -10002;
constexpr int32_t MECHBODY_GATT_INVALID_PARAM = -10003;
constexpr int32_t ERROR_SIZE_TOO_LARGE = -10004;
constexpr int32_t MECHBODY_GATT_CONNECT_FAILED = -10005;
static constexpr int32_t BLUETOOTH_SERVICE_SERVICE_SA_ID = 1130;
constexpr int32_t CONNECT_TIMEOUT_SECOND = 10;
constexpr int32_t INIT_TIMEOUT_SECOND = 3;
constexpr int32_t GET_REAL_NAME_TIMEOUT_SECOND = 2;
constexpr uint32_t DOT_REPORT_GET_REAL_NAME_TIMEOUT_MS = 3000;

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

    std::thread([mechInfo]() mutable {
        BleSendManager::GetInstance().OnGattReady(mechInfo);
        }).detach();
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
    if (ret == OHOS::Bluetooth::GattStatus::GATT_SUCCESS) {
        size_t size = 0;
        const std::unique_ptr<uint8_t[]> &value = characteristic.GetValue(&size);
        HILOGI("valueNull: %{public}d, size = %{public}zu", value == nullptr, size);
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
        if (remoteDeviceObserver_ == nullptr) {
            remoteDeviceObserver_ = std::make_shared<RemoteDeviceObserver>();
            Bluetooth::BluetoothHost::GetDefaultHost().RegisterRemoteDeviceObserver(remoteDeviceObserver_);
            HILOGI("MECHBODY_EXEC_CONNECT host RegisterRemoteDeviceObserver registed");
        }
        if (hostObserver_ == nullptr) {
            hostObserver_ = std::make_shared<HostObserver>();
            Bluetooth::BluetoothHost::GetDefaultHost().RegisterObserver(hostObserver_);
            HILOGI("MECHBODY_EXEC_CONNECT host RegisterHostObserver registed");
        }
    }
    auto runner = AppExecFwk::EventRunner::Create("BleSenderManager");
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);

    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        return;
    }
    if (bluetoothServiceStatusChangeListener_ == nullptr) {
        bluetoothServiceStatusChangeListener_ = new BluetoothServiceStatusChangeListener();
        int32_t ret = samgrProxy->SubscribeSystemAbility(
            BLUETOOTH_SERVICE_SERVICE_SA_ID, bluetoothServiceStatusChangeListener_);
        if (ret != ERR_OK) {
            HILOGE("SubscribeSystemAbility failed, ret:%d", ret);
        }
        HILOGE("SubscribeSystemAbility success, ret:%d", ret);
    }
    std::unique_lock<std::mutex> lock(initMutex_);
    initCv_.notify_all();
}

void BleSendManager::CleanOldAssetsForMechbodyStart()
{
    HILOGI("MECH_CLEAN_CONN_MAIN clean connect start");
    std::vector<int> status;
    status.push_back(static_cast<int>(Bluetooth::BTConnectState::CONNECTED));
    std::vector<Bluetooth::BluetoothRemoteDevice> devices;
    Bluetooth::HidHost::GetProfile()->GetDevicesByStates(status, devices);
    HILOGI("MECH_CLEAN_CONN_MAIN current connected device num: %{public}zu", devices.size());
    for (Bluetooth::BluetoothRemoteDevice item: devices) {
        std::string deviceName = item.GetDeviceName();
        HILOGI("MECH_CLEAN_CONN_MAIN clean connect for device name: %{public}s",
               GetAnonymStr(deviceName).c_str());
        if (deviceName.find(DEVICE_NAME_SUFFIX) == std::string::npos) {
            HILOGW("MECH_CLEAN_CONN_MAIN is not covered by this service.");
            continue;
        }
        std::string mac = item.GetDeviceAddr();
        MechInfo mechInfo;
        mechInfo.mac = mac;
        mechInfo.mechName = deviceName;
        mechInfo.gattCoonectState = true;
        MechConnectManager::GetInstance().AddMechInfo(mechInfo);
        MechbodyDisConnectForMechbotyStart(mechInfo);
    }
    HILOGI("MECH_CLEAN_CONN_MAIN clean connect end");
}

void BleSendManager::CleanAllLocalInfo()
{
    HILOGI("MECH_CLEAN_LOCAL_INFO_MAIN exec clean local info task start");
    auto connFunc = [this]() mutable {
        if (!HasBluetoothServiceStoped()) {
            HILOGW("MECH_CLEAN_LOCAL_INFO_MAIN bluetooth service has not stopped");
            return;
        }
        std::set<MechInfo> mechinfos = MechConnectManager::GetInstance().GetMechInfos();
        HILOGI("MECH_CLEAN_LOCAL_INFO_MAIN start remove all pair, mechinfos size: %{public}zu",
               mechinfos.size());
        for (MechInfo item: mechinfos) {
            Bluetooth::BluetoothRemoteDevice remoteDevice =
                    Bluetooth::BluetoothRemoteDevice(item.mac, Bluetooth::BT_TRANSPORT_BLE);
            int32_t res = Bluetooth::BluetoothHost::GetDefaultHost().RemovePair(remoteDevice);
            HILOGI("MECH_CLEAN_LOCAL_INFO_MAIN RemovePair mac:%{public}s; result: %{public}d,",
                   GetAnonymStr(item.mac).c_str(), res);
        }
        MechConnectManager::GetInstance().CleanMechInfo();
        HILOGI("MECH_CLEAN_LOCAL_INFO_MAIN clean mech info end");
        MechBodyControllerService::GetInstance().CleanMotionManagers();
        HILOGI("MECH_CLEAN_LOCAL_INFO_MAIN clean motion managers end");
        BleSendManager::GetInstance().SetBluetoothServiceStoped(false);
        HILOGI("MECH_CLEAN_LOCAL_INFO_MAIN update bluetooth stop status to false end");
    };

    if (eventHandler_ != nullptr) {
        HILOGI("MECH_CLEAN_LOCAL_INFO_MAIN post clean local info task");
        eventHandler_->PostTask(connFunc);
    }
    HILOGI("MECH_CLEAN_LOCAL_INFO_MAIN exec clean local info task end");
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

void BleSendManager::DelayedUnloadSystemAbility()
{
    HILOGI("start");
    if (eventHandler_ == nullptr) {
        HILOGE("eventHandler_ is nullptr can not post unload ability task.");
        return;
    }
    eventHandler_->PostTask([]() {
            auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (sam == nullptr) {
                HILOGE("GetSystemAbilityManager return null.");
                return;
            }
            int32_t ret = sam->UnloadSystemAbility(MECH_SAID);
            if (ret != ERR_OK) {
                HILOGE("UnLoadSystemAbility mechbody sa failed.");
            }
        }, g_abilityUnloadTaskName, ABILITY_UNLOAD_WAIT_TIME);
    HILOGI("end");
}

void BleSendManager::CancelDelayedUnload()
{
    HILOGI("start");
    if (eventHandler_ != nullptr) {
        HILOGI("MECHBODY_EXEC_CONNECT remove ability unload task");
        eventHandler_->RemoveTask(g_abilityUnloadTaskName);
    }
    HILOGI("end.");
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
        GetRealName(mechInfo);
        MechConnectManager::GetInstance().GetMechInfo(mechInfo.mac, mechInfo);
        std::string showName = mechInfo.mechName + DEVICE_NAME_SUFFIX;
        int32_t res = remoteDevice.SetDeviceAlias(showName);
        HILOGI("MECHBODY_EXEC_CONNECT SetDeviceAlias end. result: %{public}d", res);

        std::unique_lock<std::mutex> lock(pairMutex_);
        MechConnectManager::GetInstance().SetMechanicPairState(mechInfo.mac, true);
        pairCv_.notify_all();
    }
}

void BleSendManager::GetRealName(MechInfo &mechInfo)
{
    std::unique_lock<std::mutex> lockRealName(getRealNameMutex_);
    auto func = [mechInfo]() {
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
        if (MechBodyControllerService::GetInstance().motionManagers_.find(mechInfo.mechId) !=
            MechBodyControllerService::GetInstance().motionManagers_.end()) {
            std::string deviceRealName =
                    MechBodyControllerService::GetInstance().motionManagers_[mechInfo.mechId]->GetDeviceRealName();
            if (!deviceRealName.empty()) {
                MechConnectManager::GetInstance().SetRealMechName(mechInfo.mechId, deviceRealName);
                return true;
            }
        }
        return false;
    };
    if (!getRealNameCv_.wait_for(lockRealName, std::chrono::seconds(GET_REAL_NAME_TIMEOUT_SECOND), func)) {
        HILOGE("MECHBODY_EXEC_CONNECT wait for get real name timeout");
    }
}

void BleSendManager::NotifyGetRealName()
{
    HILOGI(" start");
    std::unique_lock<std::mutex> lock(getRealNameMutex_);
    getRealNameCv_.notify_all();
    HILOGI(" end");
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

int32_t BleSendManager::OnGattReady(MechInfo &mechInfo)
{
    HILOGI("MECHBODY_EXEC_CONNECT called");
    {
        std::unique_lock<std::mutex> lock(gattMutex_);
        MechConnectManager::GetInstance().SetMechanicGattState(mechInfo.mac, true);
        gattCv_.notify_all();
    }
    int32_t result = MechConnectManager::GetInstance().NotifyMechConnect(mechInfo);
    if (result == MECH_CONNECT_FAILED) {
        return MECH_CONNECT_FAILED;
    }
    HILOGI("MECHBODY_EXEC_CONNECT Tracking init start.");
    McCameraTrackingController::GetInstance().Init();
    HILOGI("MECHBODY_EXEC_CONNECT Tracking init finish.");
    return ERR_OK;
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

void DotReportMechKitStartGattConnectFail(const MechInfo &mechInfo)
{
    MechKitStartReportInfo mechKitStartReportInfo;
    mechKitStartReportInfo.deviceType = mechInfo.mechType;
    mechKitStartReportInfo.deviceModel = mechInfo.mechName;
    mechKitStartReportInfo.mechkitStartResultCode = MechKitStartResCode::GATT_CONNECT_FAIL;
    HisyseventUtils::DotReportMechKitStartEvent(mechKitStartReportInfo);
}

void DotReportMechKitStartGattSuccess(const MechInfo &mechInfo)
{
    MechKitStartReportInfo mechKitStartReportInfo;
    mechKitStartReportInfo.deviceType = mechInfo.mechType;
    mechKitStartReportInfo.deviceModel = mechInfo.mechName;
    mechKitStartReportInfo.mechkitStartResultCode = MechKitStartResCode::SUCCESS;
    std::thread dotReportThread(
        [mechKitStartReportInfoParam = std::move(mechKitStartReportInfo),
            mechIdParam = mechInfo.mechId] () mutable {
        std::string deviceName;
        // thread safe visit
        {
            auto &instance = MechBodyControllerService::GetInstance();
            std::lock_guard<std::mutex> lock(instance.motionManagersMutex);
            auto it = instance.motionManagers_.find(mechIdParam);
            if (it != instance.motionManagers_.end()) {
                deviceName = it->second->TryGetDeviceRealNameSync(DOT_REPORT_GET_REAL_NAME_TIMEOUT_MS);
            }
        }

        mechKitStartReportInfoParam.deviceName = deviceName;
        HisyseventUtils::DotReportMechKitStartEvent(mechKitStartReportInfoParam);
    });
    dotReportThread.detach();
}

void DotReportMechKitStartEventHandlerInitFail(const std::string &deviceName)
{
    MechKitStartReportInfo mechKitStartReportInfo;
    mechKitStartReportInfo.deviceModel = deviceName;
    mechKitStartReportInfo.mechkitStartResultCode = MechKitStartResCode::EVENT_HANDLER_INIT_FAIL;
    HisyseventUtils::DotReportMechKitStartEvent(mechKitStartReportInfo);
}

int32_t BleSendManager::MechbodyConnect(std::string mac, std::string deviceName)
{
    HILOGI("MECHBODY_EXEC_CONNECT satrt");
    // once connection may involve multiple calls to the NAPI interface.
	// Mechkit employs a single-threaded task queue to ensure processing occurs only once
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

        do {
            int32_t gattRet = MechbodyGattcConnect(mechInfo.mac, mechInfo.mechName);
            HILOGE("MECHBODY_EXEC_CONNECT gatt connect result: %{public}d", gattRet);
            if (gattRet != ERR_OK) {
                MechbodyDisConnectSync(mechInfo);
                DotReportMechKitStartGattConnectFail(mechInfo);
                break;
            }
            // connect success
            int32_t pairRet = MechbodyPair(mechInfo.mac, mechInfo.mechName);
            int32_t hidRet = MechbodyHidConnect(mechInfo.mac, mechInfo.mechName);
            HILOGE("MECHBODY_EXEC_CONNECT pair result: %{public}d; hid result: %{public}d", pairRet, hidRet);
            DotReportMechKitStartGattSuccess(mechInfo);
        } while (false);
        HILOGI("MECHBODY_EXEC_CONNECT async connect end, mech info for: %{public}s", mechInfo.ToString().c_str());
    };

    if (eventHandler_ == nullptr) {
        HILOGI("MECHBODY_EXEC_CONNECT eventHandler_ is nullptr.");
        std::unique_lock<std::mutex> lock(initMutex_);
        if (!initCv_.wait_for(lock, std::chrono::seconds(INIT_TIMEOUT_SECOND),
            [this]() { return eventHandler_ != nullptr; })) {
            HILOGE("MECHBODY_EXEC_CONNECT wait for blemanager init timeout.");
            DotReportMechKitStartEventHandlerInitFail(deviceName);
            return MECH_CONNECT_FAILED;
        }
    }
    eventHandler_->PostTask(connFunc);
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
    if (!gattCv_.wait_for(lock, std::chrono::seconds(CONNECT_TIMEOUT_SECOND),
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
    if (res != OHOS_BT_STATUS_SUCCESS) {
        return MECH_CONNECT_FAILED;
    }
    std::unique_lock<std::mutex> lock(pairMutex_);
    if (!pairCv_.wait_for(lock, std::chrono::seconds(CONNECT_TIMEOUT_SECOND),
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
    HILOGI("MECHBODY_EXEC_CONNECT exec connect result: %{public}d;", res);
    if (res != OHOS_BT_STATUS_SUCCESS) {
        return MECH_CONNECT_FAILED;
    }
    std::unique_lock<std::mutex> lock(hidMutex_);
    if (!hidCv_.wait_for(lock, std::chrono::seconds(CONNECT_TIMEOUT_SECOND),
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
    HILOGI("MECHBODY_EXEC_CONNECT RemovePair start.");
    Bluetooth::BluetoothRemoteDevice remoteDevice =
            Bluetooth::BluetoothRemoteDevice(mechInfo.mac, Bluetooth::BT_TRANSPORT_BLE);
    int32_t res = Bluetooth::BluetoothHost::GetDefaultHost().RemovePair(remoteDevice);
    HILOGI("MECHBODY_EXEC_CONNECT RemovePair end, result: %{public}d", res);
    ret = MechbodyGattcDisconnect(mechInfo);
    if (ret != ERR_OK) {
        HILOGE("MECHBODY_EXEC_DISCONNECT gatt disconnect failed for mechInfo: %{public}s",
            mechInfo.ToString().c_str());
    }
    OnGattDisconnect(mechInfo);
    HILOGI("MECHBODY_EXEC_DISCONNECT end, mech info for: %{public}s", mechInfo.ToString().c_str());
    return ret;
}

void BleSendManager::OnGattDisconnect(MechInfo &mechInfo)
{
    MechConnectManager::GetInstance().NotifyMechDisconnect(mechInfo);
    DelayedUnloadSystemAbility();
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

    std::unique_lock<std::mutex> lock(gattDisconnMutex_);
    if (!gattDisconnCv_.wait_for(lock, std::chrono::seconds(CONNECT_TIMEOUT_SECOND),
        [this, mechInfo]() mutable {
            bool gattState = true;
            MechConnectManager::GetInstance().GetMechanicGattState(mechInfo.mac, gattState);
            return !gattState;
        })) {
        HILOGE("MECHBODY_EXEC_DISCONNECT wait for gatt connect timeout");
        return MECHBODY_GATT_CONNECT_FAILED;
    }
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
    if (res != OHOS_BT_STATUS_SUCCESS) {
        return MECH_DISCONNECT_FAILED;
    }
    std::unique_lock<std::mutex> lock(hidDisconnMutex_);
    if (!hidDisconnCv_.wait_for(lock, std::chrono::seconds(CONNECT_TIMEOUT_SECOND),
        [this, mechInfo]() mutable {
            bool hidState = true;
            MechConnectManager::GetInstance().GetMechanicHidState(mechInfo.mac, hidState);
            return !hidState;
        })) {
        HILOGE("MECHBODY_EXEC_DISCONNECT wait for hid disconnect timeout");
        return ERR_OK;
    }
    HILOGI("MECHBODY_EXEC_DISCONNECT disconnect end mechInfo: %{public}s", mechInfo.ToString().c_str());
    return ERR_OK;
}

int32_t BleSendManager::MechbodyDisConnectForMechbotyStart(MechInfo &mechInfo)
{
    HILOGI("MECH_CLEAN_CONN_MAIN exec disconnect tsak start, mech info: %{public}s", mechInfo.ToString().c_str());
    auto connFunc = [this, mechInfo]() mutable {
        HILOGI("MECH_CLEAN_CONN_MAIN All disconnect start for mech info: %{public}s;",
               mechInfo.ToString().c_str());
        if (MechConnectManager::GetInstance().GetMechInfo(mechInfo.mac, mechInfo) != ERR_OK) {
            HILOGE("MECH_CLEAN_CONN_MAIN Can not find mech info for amc: %{public}s",
                GetAnonymStr(mechInfo.mac).c_str());
            return;
        }
        MechbodyDisConnectSyncForMechbotyStart(mechInfo);
        HILOGI("MECH_CLEAN_CONN_MAIN All disconnect end, mech info: %{public}s;",
               mechInfo.ToString().c_str());
        HILOGI("MECH_CLEAN_CONN_MAIN clean mech Info by mac start, mech info: %{public}s;",
               mechInfo.ToString().c_str());
        MechConnectManager::GetInstance().RemoveMechInfoByMac(mechInfo.mac);
        HILOGI("MECH_CLEAN_CONN_MAIN clean mech Info by mac end, mech info: %{public}s;",
               mechInfo.ToString().c_str());
    };

    if (eventHandler_ != nullptr) {
        HILOGI("MECH_CLEAN_CONN_MAIN post disconnect task for mech info: %{public}s;", mechInfo.ToString().c_str());
        eventHandler_->PostTask(connFunc);
    }
    HILOGI("MECH_CLEAN_CONN_MAIN exec disconnect task end, mech info: %{public}s", mechInfo.ToString().c_str());
    return ERR_OK;
}

int32_t BleSendManager::MechbodyDisConnectSyncForMechbotyStart(MechInfo &mechInfo)
{
    int32_t ret = MechbodyHidDisconnectForMechbotyStart(mechInfo);
    HILOGI("MECH_CLEAN_CONN_MAIN hid disconnect result: %{public}d; for mechInfo: %{public}s",
           ret, mechInfo.ToString().c_str());
    ret = MechbodyGattcDisconnectForMechbotyStart(mechInfo);
    HILOGI("MECH_CLEAN_CONN_MAIN gatt disconnect result: %{public}d; for mechInfo: %{public}s",
           ret, mechInfo.ToString().c_str());
    return ret;
}

int32_t BleSendManager::MechbodyGattcDisconnectForMechbotyStart(MechInfo mechInfo)
{
    HILOGI("MECH_CLEAN_CONN_GATT exec disconnect start for mechInfo: %{public}s,", mechInfo.ToString().c_str());
    if (MechConnectManager::GetInstance().GetMechInfo(mechInfo.mac, mechInfo) != ERR_OK) {
        HILOGE("MECH_CLEAN_CONN_GATT mech info not exist for mac: %{public}s; device name: %{public}s;",
               GetAnonymStr(mechInfo.mac).c_str(), GetAnonymStr(mechInfo.mechName).c_str());
        return MECH_INFO_NOT_FOUND;
    }
    if (gattClient_ == nullptr) {
        HILOGE("MECH_CLEAN_CONN_GATT gattClient is nullptr for mech info: %{public}s;", mechInfo.ToString().c_str());
        return MECH_HAS_DISCONNECTED;
    }
    int result = gattClient_->Disconnect();
    HILOGI("MECH_CLEAN_CONN_GATT Gatt Disconnect exec result: %{public}d; mech info: %{public}s",
           result, mechInfo.ToString().c_str());
    if (result != OHOS_BT_STATUS_SUCCESS) {
        HILOGE("MECH_CLEAN_CONN_GATT Gatt Disconnect error, status: %{public}d; mech info: %{public}s",
               result, mechInfo.ToString().c_str());
        return MECH_DISCONNECT_FAILED;
    }
    std::unique_lock<std::mutex> lock(gattDisconnMutex_);
    if (!gattDisconnCv_.wait_for(lock, std::chrono::seconds(CONNECT_TIMEOUT_SECOND),
        [mechInfo]() mutable {
            bool gattState = true;
            MechConnectManager::GetInstance().GetMechanicGattState(mechInfo.mac, gattState);
            return !gattState;
        })) {
        MechConnectManager::GetInstance().SetMechanicGattState(mechInfo.mac, false);
        HILOGE("MECH_CLEAN_CONN_GATT wait for gatt disconnect timeout, mech info: %{public}s",
               mechInfo.ToString().c_str());
        return MECH_DISCONNECT_FAILED;
    }
    HILOGI("MECH_CLEAN_CONN_GATT exec disconnect end for mechInfo: %{public}s,", mechInfo.ToString().c_str());
    return ERR_OK;
}

int32_t BleSendManager::MechbodyHidDisconnectForMechbotyStart(MechInfo &mechInfo)
{
    HILOGI("MECH_CLEAN_CONN_HID exec disconnect start for mechInfo: %{public}s,", mechInfo.ToString().c_str());
    if (MechConnectManager::GetInstance().GetMechInfo(mechInfo.mac, mechInfo) != ERR_OK) {
        HILOGE("MECH_CLEAN_CONN_HID mech info not exist for mac: %{public}s; device name: %{public}s;",
               GetAnonymStr(mechInfo.mac).c_str(), GetAnonymStr(mechInfo.mechName).c_str());
        return MECH_INFO_NOT_FOUND;
    }
    Bluetooth::BluetoothRemoteDevice remoteDevice =
            Bluetooth::BluetoothRemoteDevice(mechInfo.mac, Bluetooth::BT_TRANSPORT_BLE);
    int32_t res = Bluetooth::HidHost::GetProfile()->Disconnect(remoteDevice);
    HILOGI("MECH_CLEAN_CONN_HID hid Disconnect exec result: %{public}d; mech info: %{public}s",
           res, mechInfo.ToString().c_str());
    if (res != OHOS_BT_STATUS_SUCCESS) {
        HILOGE("MECH_CLEAN_CONN_HID hid Disconnect error, status: %{public}d; mech info: %{public}s",
               res, mechInfo.ToString().c_str());
        return MECH_DISCONNECT_FAILED;
    }
    std::unique_lock<std::mutex> lock(hidDisconnMutex_);
    if (!hidDisconnCv_.wait_for(lock, std::chrono::seconds(CONNECT_TIMEOUT_SECOND),
                                [mechInfo]() mutable {
                                    bool hidState = true;
                                    MechConnectManager::GetInstance().GetMechanicHidState(mechInfo.mac, hidState);
                                    return !hidState;
                                })) {
        HILOGE("MECH_CLEAN_CONN_HID wait for hid disconnect timeout,  mech info: %{public}s",
               mechInfo.ToString().c_str());
        return ERR_OK;
    }

    res = Bluetooth::BluetoothHost::GetDefaultHost().RemovePair(remoteDevice);
    HILOGI("MECH_CLEAN_CONN_HID RemovePair end, result: %{public}d", res);
    MechConnectManager::GetInstance().GetMechInfo(mechInfo.mac, mechInfo);
    HILOGI("MECH_CLEAN_CONN_HID exec disconnect end for mechInfo: %{public}s,", mechInfo.ToString().c_str());
    return ERR_OK;
}

void BleSendManager::SetBluetoothServiceStoped(bool status)
{
    bluetoothServiceStoped.store(status);
}

bool BleSendManager::HasBluetoothServiceStoped()
{
    return bluetoothServiceStoped.load();
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
    wrCharacteristic.SetWriteType(GattCharacteristic::WriteType::NO_RESPONSE);
    HILOGI("Mech writeC");
    uint64_t beginTime = GetTimeStampMs();
    result = gattClient_->WriteCharacteristic(wrCharacteristic);
    uint64_t endTime = GetTimeStampMs();
    HILOGI("Mech writeC, result = %{public}d handle: %{public}d dataLen: %{public}d, castTime:%{public}s \n", \
        result, handle_, dataLen, std::to_string(endTime - beginTime).c_str());
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

void RemoteDeviceObserver::OnAclStateChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device,
    int state, unsigned int reason)
{}

void RemoteDeviceObserver::OnPairStatusChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device,
    int status, int cause)
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

void RemoteDeviceObserver::OnRemoteUuidChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device,
    const std::vector<Bluetooth::ParcelUuid> &uuids)
{}

void RemoteDeviceObserver::OnRemoteNameChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device,
    const std::string &deviceName)
{}

void RemoteDeviceObserver::OnRemoteAliasChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device,
    const std::string &alias)
{}

void RemoteDeviceObserver::OnRemoteCodChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device,
    const OHOS::Bluetooth::BluetoothDeviceClass &cod)
{}

void RemoteDeviceObserver::OnRemoteBatteryLevelChanged(
    const OHOS::Bluetooth::BluetoothRemoteDevice &device, int batteryLevel)
{}

void RemoteDeviceObserver::OnReadRemoteRssiEvent(
    const OHOS::Bluetooth::BluetoothRemoteDevice &device, int rssi, int status)
{}

void RemoteDeviceObserver::OnRemoteBatteryChanged(const OHOS::Bluetooth::BluetoothRemoteDevice &device,
    const OHOS::Bluetooth::DeviceBatteryInfo &batteryInfo)
{}

void RemoteDeviceObserver::OnRemoteDeviceCommonInfoReport(
    const OHOS::Bluetooth::BluetoothRemoteDevice &device, const std::vector<uint8_t> &value)
{}

void HostObserver::OnStateChanged(const int transport, const int status)
{
    HILOGI("MECH_CONN_BT_STATE transport: %{public}d; status: %{public}d;", transport, status);
    if (status == static_cast<int>(BTStateID::STATE_TURN_ON)) {
        BleSendManager::GetInstance().CleanAllLocalInfo();
    }
}

void HostObserver::OnDiscoveryStateChanged(int status)
{}

void HostObserver::OnDiscoveryResult(
    const Bluetooth::BluetoothRemoteDevice &device, int rssi, const std::string deviceName, int deviceClass)
{}

void HostObserver::OnPairRequested(const Bluetooth::BluetoothRemoteDevice &device)
{}

void HostObserver::OnPairConfirmed(const Bluetooth::BluetoothRemoteDevice &device, int reqType, int number)
{}

void HostObserver::OnScanModeChanged(int mode)
{}

void HostObserver::OnDeviceNameChanged(const std::string &deviceName)
{}

void HostObserver::OnDeviceAddrChanged(const std::string &address)
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

void BluetoothServiceStatusChangeListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId != BLUETOOTH_SERVICE_SERVICE_SA_ID) {
        return;
    }
    HILOGI("bluetooth service SA:%{public}d added", systemAbilityId);
}

void BluetoothServiceStatusChangeListener::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId != BLUETOOTH_SERVICE_SERVICE_SA_ID) {
        return;
    }
    HILOGI("bluetooth service SA:%{public}d removed", systemAbilityId);
    BleSendManager::GetInstance().SetBluetoothServiceStoped(true);
}
} // namespace MechBodyController
} // namespace OHOS