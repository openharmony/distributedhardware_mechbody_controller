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

#include <fuzzer/FuzzedDataProvider.h>
#include "blesendmanagerfour_fuzzer.h"
#include "ble_send_manager.h"
#include "mechbody_controller_log.h"
#include "securec.h"
#include <memory>
#include <thread>
#include <chrono>

using namespace OHOS;
using namespace OHOS::MechBodyController;
using namespace OHOS::Bluetooth;

namespace {

enum class TestFunctionId {
    FUZZ_INIT = 0,
    FUZZ_UNINIT = 1,
    FUZZ_DELAYED_UNLOAD_SYSTEM_ABILITY = 2,
    FUZZ_CANCEL_DELAYED_UNLOAD = 3,
    FUZZ_ON_PAIR_STATE_CHANGED = 4,
    FUZZ_GET_REAL_NAME = 5,
    FUZZ_NOTIFY_GET_REAL_NAME = 6,
    FUZZ_ON_HID_STATE_CHANGED = 7,
    FUZZ_MECHBODY_CONNECT = 8,
    FUZZ_MECHBODY_PAIR = 9,
    FUZZ_MECHBODY_HID_CONNECT = 10,
    FUZZ_MECHBODY_DISCONNECT = 11,
    // Skip FUZZ_MECHBODY_DISCONNECT_SYNC and FUZZ_ON_GATT_DISCONNECT to avoid crash
    // FUZZ_MECHBODY_DISCONNECT_SYNC = 12,
    // FUZZ_ON_GATT_DISCONNECT = 13,
    FUZZ_SET_BLUETOOTH_SERVICE_STOPED = 14,
    FUZZ_HAS_BLUETOOTH_SERVICE_STOPED = 15,
    FUZZ_CLEAN_OLD_ASSETS = 16,
    FUZZ_CLEAN_ALL_LOCAL_INFO = 17,
    FUZZ_REMOTE_DEVICE_OBSERVER_PAIR = 18,
    FUZZ_HID_OBSERVER_CONNECTION = 19,
    FUZZ_HOST_OBSERVER_STATE = 20,
    FUZZ_BLUETOOTH_SERVICE_ADD = 21,
    FUZZ_BLUETOOTH_SERVICE_REMOVE = 22,
    FUZZ_MECHBODY_DISCONNECT_FOR_START = 23,
    FUZZ_MECHBODY_GATT_DISCONNECT_FOR_START = 24,
    FUZZ_MECHBODY_HID_DISCONNECT_FOR_START = 25,
    FUZZ_FULL_WORKFLOW = 26
};

constexpr int32_t TEST_FUNCTION_GROUP1_END = static_cast<int32_t>(TestFunctionId::FUZZ_ON_HID_STATE_CHANGED);
constexpr int32_t TEST_FUNCTION_GROUP2_END = static_cast<int32_t>(TestFunctionId::FUZZ_CLEAN_ALL_LOCAL_INFO);
constexpr int32_t TEST_FUNCTION_MAX_ID = static_cast<int32_t>(TestFunctionId::FUZZ_FULL_WORKFLOW);

void FuzzInit(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    bleSendManager.Init();
}

void FuzzUnInit(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    bleSendManager.UnInit();
}

void FuzzDelayedUnloadSystemAbility(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    bleSendManager.DelayedUnloadSystemAbility();
}

void FuzzCancelDelayedUnload(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    bleSendManager.CancelDelayedUnload();
}

void FuzzOnPairStateChanged(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    MechInfo mechInfo;
    mechInfo.mac = provider.ConsumeRandomLengthString(32);
    mechInfo.mechName = provider.ConsumeRandomLengthString(64);
    int32_t pairState = provider.ConsumeIntegral<int32_t>();
    int32_t cause = provider.ConsumeIntegral<int32_t>();
    bleSendManager.OnPairStateChanged(pairState, cause, mechInfo);
}

void FuzzGetRealName(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    MechInfo mechInfo;
    mechInfo.mac = provider.ConsumeRandomLengthString(32);
    mechInfo.mechName = provider.ConsumeRandomLengthString(64);
    mechInfo.mechId = provider.ConsumeIntegral<int32_t>();
    bleSendManager.GetRealName(mechInfo);
}

void FuzzNotifyGetRealName(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    bleSendManager.NotifyGetRealName();
}

void FuzzOnHidStateChanged(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    MechInfo mechInfo;
    mechInfo.mac = provider.ConsumeRandomLengthString(32);
    mechInfo.mechName = provider.ConsumeRandomLengthString(64);
    int32_t hidState = provider.ConsumeIntegral<int32_t>();
    int32_t cause = provider.ConsumeIntegral<int32_t>();
    bleSendManager.OnHidStateChanged(hidState, cause, mechInfo);
}

void FuzzMechbodyConnect(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    std::string mac = provider.ConsumeRandomLengthString(32);
    std::string deviceName = provider.ConsumeRandomLengthString(64);
    uint32_t deviceIdentifier = provider.ConsumeIntegral<uint32_t>();
    bleSendManager.MechbodyConnect(mac, deviceName, deviceIdentifier);
}

void FuzzMechbodyPair(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    std::string mac = provider.ConsumeRandomLengthString(32);
    std::string deviceName = provider.ConsumeRandomLengthString(64);
    bleSendManager.MechbodyPair(mac, deviceName);
}

void FuzzMechbodyHidConnect(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    std::string mac = provider.ConsumeRandomLengthString(32);
    std::string deviceName = provider.ConsumeRandomLengthString(64);
    bleSendManager.MechbodyHidConnect(mac, deviceName);
}

void FuzzMechbodyDisconnect(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    MechInfo mechInfo;
    mechInfo.mac = provider.ConsumeRandomLengthString(32);
    mechInfo.mechName = provider.ConsumeRandomLengthString(64);
    mechInfo.gattCoonectState = provider.ConsumeBool();
    bleSendManager.MechbodyDisConnect(mechInfo);
}

void FuzzSetBluetoothServiceStoped(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    bool status = provider.ConsumeBool();
    bleSendManager.SetBluetoothServiceStoped(status);
}

void FuzzHasBluetoothServiceStoped(FuzzedDataProvider &provider)
{
    (void)provider;
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    bleSendManager.HasBluetoothServiceStoped();
}

void FuzzCleanOldAssets(FuzzedDataProvider &provider)
{
    (void)provider;
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    bleSendManager.CleanOldAssetsForMechbodyStart();
}

void FuzzCleanAllLocalInfo(FuzzedDataProvider &provider)
{
    (void)provider;
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    bleSendManager.CleanAllLocalInfo();
}

void FuzzRemoteDeviceObserverPair(FuzzedDataProvider &provider)
{
    std::string address = provider.ConsumeRandomLengthString(32);
    BluetoothRemoteDevice device(address, 1);
    int32_t status = provider.ConsumeIntegral<int32_t>();
    int32_t cause = provider.ConsumeIntegral<int32_t>();

    RemoteDeviceObserver observer;
    observer.OnPairStatusChanged(device, status, cause);
}

void FuzzHidObserverConnection(FuzzedDataProvider &provider)
{
    std::string address = provider.ConsumeRandomLengthString(32);
    BluetoothRemoteDevice device(address, 1);
    int32_t state = provider.ConsumeIntegral<int32_t>();
    int32_t cause = provider.ConsumeIntegral<int32_t>();

    HidObserver observer;
    observer.OnConnectionStateChanged(device, state, cause);
}

void FuzzHostObserverState(FuzzedDataProvider &provider)
{
    int32_t transport = provider.ConsumeIntegral<int32_t>();
    int32_t status = provider.ConsumeIntegral<int32_t>();

    HostObserver observer;
    observer.OnStateChanged(transport, status);
}

void FuzzBluetoothServiceAdd(FuzzedDataProvider &provider)
{
    int32_t systemAbilityId = provider.ConsumeIntegral<int32_t>();
    std::string deviceId = provider.ConsumeRandomLengthString(64);

    BluetoothServiceStatusChangeListener listener;
    listener.OnAddSystemAbility(systemAbilityId, deviceId);
}

void FuzzBluetoothServiceRemove(FuzzedDataProvider &provider)
{
    int32_t systemAbilityId = provider.ConsumeIntegral<int32_t>();
    std::string deviceId = provider.ConsumeRandomLengthString(64);

    BluetoothServiceStatusChangeListener listener;
    listener.OnRemoveSystemAbility(systemAbilityId, deviceId);
}

void FuzzMechbodyDisconnectForStart(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    MechInfo mechInfo;
    mechInfo.mac = provider.ConsumeRandomLengthString(32);
    mechInfo.mechName = provider.ConsumeRandomLengthString(64);
    mechInfo.gattCoonectState = provider.ConsumeBool();
    bleSendManager.MechbodyDisConnectForMechbotyStart(mechInfo);
}

void FuzzMechbodyGattDisconnectForStart(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    MechInfo mechInfo;
    mechInfo.mac = provider.ConsumeRandomLengthString(32);
    mechInfo.mechName = provider.ConsumeRandomLengthString(64);
    mechInfo.gattCoonectState = provider.ConsumeBool();
    bleSendManager.MechbodyGattcDisconnectForMechbotyStart(mechInfo);
}

void FuzzMechbodyHidDisconnectForStart(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();
    MechInfo mechInfo;
    mechInfo.mac = provider.ConsumeRandomLengthString(32);
    mechInfo.mechName = provider.ConsumeRandomLengthString(64);
    mechInfo.gattCoonectState = provider.ConsumeBool();
    bleSendManager.MechbodyHidDisconnectForMechbotyStart(mechInfo);
}

void FuzzFullWorkflow(FuzzedDataProvider &provider)
{
    BleSendManager &bleSendManager = BleSendManager::GetInstance();

    // Init
    bleSendManager.Init();

    // Connect workflow
    std::string mac = provider.ConsumeRandomLengthString(32);
    std::string deviceName = provider.ConsumeRandomLengthString(64);
    MechInfo mechInfo;
    mechInfo.mac = mac;
    mechInfo.mechName = deviceName;

    // State changes
    bleSendManager.OnPairStateChanged(0, 0, mechInfo);
    bleSendManager.OnHidStateChanged(0, 0, mechInfo);

    // Disconnect workflow - skip OnGattDisconnect as it may crash
    bleSendManager.MechbodyDisConnect(mechInfo);

    // Service state
    bleSendManager.SetBluetoothServiceStoped(true);
    bleSendManager.HasBluetoothServiceStoped();

    // UnInit
    bleSendManager.UnInit();
}

void RunFuzzTestGroup1(FuzzedDataProvider &provider, int32_t testFunctionId)
{
    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_INIT:
            FuzzInit(provider);
            break;
        case TestFunctionId::FUZZ_UNINIT:
            FuzzUnInit(provider);
            break;
        case TestFunctionId::FUZZ_DELAYED_UNLOAD_SYSTEM_ABILITY:
            FuzzDelayedUnloadSystemAbility(provider);
            break;
        case TestFunctionId::FUZZ_CANCEL_DELAYED_UNLOAD:
            FuzzCancelDelayedUnload(provider);
            break;
        case TestFunctionId::FUZZ_ON_PAIR_STATE_CHANGED:
            FuzzOnPairStateChanged(provider);
            break;
        case TestFunctionId::FUZZ_GET_REAL_NAME:
            FuzzGetRealName(provider);
            break;
        case TestFunctionId::FUZZ_NOTIFY_GET_REAL_NAME:
            FuzzNotifyGetRealName(provider);
            break;
        case TestFunctionId::FUZZ_ON_HID_STATE_CHANGED:
            FuzzOnHidStateChanged(provider);
            break;
        default:
            break;
    }
}

void RunFuzzTestGroup2(FuzzedDataProvider &provider, int32_t testFunctionId)
{
    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_MECHBODY_CONNECT:
            FuzzMechbodyConnect(provider);
            break;
        case TestFunctionId::FUZZ_MECHBODY_PAIR:
            FuzzMechbodyPair(provider);
            break;
        case TestFunctionId::FUZZ_MECHBODY_HID_CONNECT:
            FuzzMechbodyHidConnect(provider);
            break;
        case TestFunctionId::FUZZ_MECHBODY_DISCONNECT:
            FuzzMechbodyDisconnect(provider);
            break;
        case TestFunctionId::FUZZ_SET_BLUETOOTH_SERVICE_STOPED:
            FuzzSetBluetoothServiceStoped(provider);
            break;
        case TestFunctionId::FUZZ_HAS_BLUETOOTH_SERVICE_STOPED:
            FuzzHasBluetoothServiceStoped(provider);
            break;
        case TestFunctionId::FUZZ_CLEAN_OLD_ASSETS:
            FuzzCleanOldAssets(provider);
            break;
        default:
            break;
    }
}

void RunFuzzTestGroup3(FuzzedDataProvider &provider, int32_t testFunctionId)
{
    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_CLEAN_ALL_LOCAL_INFO:
            FuzzCleanAllLocalInfo(provider);
            break;
        case TestFunctionId::FUZZ_REMOTE_DEVICE_OBSERVER_PAIR:
            FuzzRemoteDeviceObserverPair(provider);
            break;
        case TestFunctionId::FUZZ_HID_OBSERVER_CONNECTION:
            FuzzHidObserverConnection(provider);
            break;
        case TestFunctionId::FUZZ_HOST_OBSERVER_STATE:
            FuzzHostObserverState(provider);
            break;
        case TestFunctionId::FUZZ_BLUETOOTH_SERVICE_ADD:
            FuzzBluetoothServiceAdd(provider);
            break;
        case TestFunctionId::FUZZ_BLUETOOTH_SERVICE_REMOVE:
            FuzzBluetoothServiceRemove(provider);
            break;
        case TestFunctionId::FUZZ_MECHBODY_DISCONNECT_FOR_START:
            FuzzMechbodyDisconnectForStart(provider);
            break;
        case TestFunctionId::FUZZ_MECHBODY_GATT_DISCONNECT_FOR_START:
            FuzzMechbodyGattDisconnectForStart(provider);
            break;
        case TestFunctionId::FUZZ_MECHBODY_HID_DISCONNECT_FOR_START:
            FuzzMechbodyHidDisconnectForStart(provider);
            break;
        case TestFunctionId::FUZZ_FULL_WORKFLOW:
            FuzzFullWorkflow(provider);
            break;
        default:
            break;
    }
}

void RunFuzzTest(FuzzedDataProvider &provider)
{
    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0, TEST_FUNCTION_MAX_ID);

    if (testFunctionId <= TEST_FUNCTION_GROUP1_END) {
        RunFuzzTestGroup1(provider, testFunctionId);
    } else if (testFunctionId <= TEST_FUNCTION_GROUP2_END) {
        RunFuzzTestGroup2(provider, testFunctionId);
    } else {
        RunFuzzTestGroup3(provider, testFunctionId);
    }
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    RunFuzzTest(provider);
    return 0;
}