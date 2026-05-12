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

#include "mcconnectmanager_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include <string>

#include "mc_connect_manager.h"
#include "mechbody_controller_log.h"
#include "mechbody_controller_types.h"

namespace OHOS {
using namespace OHOS::MechBodyController;
namespace {
const std::string TAG = "McConnectManagerFuzzTest";

// Fuzz test constants
constexpr uint32_t MIN_INIT_COUNT = 1;
constexpr uint32_t MAX_INIT_COUNT = 3;
constexpr uint32_t MIN_CHECK_COUNT = 1;
constexpr uint32_t MAX_CHECK_COUNT = 10;
constexpr uint32_t MIN_CLEAN_COUNT = 1;
constexpr uint32_t MAX_CLEAN_COUNT = 5;
constexpr uint32_t OPT_SIZE = 80;

constexpr uint8_t FUZZ_TEST_COUNT = 23;
}

// Mock listener for testing
class MockMechConnectListener : public IMechConnectListener {
public:
    void OnMechConnect(const MechInfo& mechInfo) override {}
    void OnMechDisconnect(const MechInfo& mechInfo) override {}
};

void InitFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t initCount = fdp.ConsumeIntegralInRange<uint32_t>(MIN_INIT_COUNT, MAX_INIT_COUNT);
    for (uint32_t i = 0; i < initCount; i++) {
        MechConnectManager::GetInstance().Init();
    }
    uint32_t uninitCount = fdp.ConsumeIntegralInRange<uint32_t>(MIN_INIT_COUNT, MAX_INIT_COUNT);
    for (uint32_t i = 0; i < uninitCount; i++) {
        MechConnectManager::GetInstance().UnInit();
    }
}

void UpdateBleStatusFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    bool isBLEActive = fdp.ConsumeBool();

    MechConnectManager::GetInstance().UpdateBleStatus(isBLEActive);
}

void GetLocalDeviceBleStatusFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t checkCount = fdp.ConsumeIntegralInRange<uint32_t>(MIN_CHECK_COUNT, MAX_CHECK_COUNT);
    for (uint32_t i = 0; i < checkCount; i++) {
        MechConnectManager::GetInstance().GetLocalDeviceBleStatus();
    }
}

void IsConnectFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t checkCount = fdp.ConsumeIntegralInRange<uint32_t>(MIN_CHECK_COUNT, MAX_CHECK_COUNT);
    for (uint32_t i = 0; i < checkCount; i++) {
        MechConnectManager::GetInstance().IsConnect();
    }
}

void AddMechInfoFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    
    MechInfo mechInfo;
    mechInfo.mechId = fdp.ConsumeIntegral<int32_t>();
    mechInfo.mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());
    mechInfo.mechName = fdp.ConsumeRandomLengthString();
    mechInfo.mac = fdp.ConsumeRandomLengthString();
    mechInfo.state = static_cast<AttachmentState>(fdp.ConsumeIntegral<int32_t>());
    mechInfo.gattCoonectState = fdp.ConsumeBool();
    mechInfo.pairState = fdp.ConsumeBool();
    mechInfo.hidState = fdp.ConsumeBool();

    MechConnectManager::GetInstance().AddMechInfo(mechInfo);
}

void RemoveMechInfoByMacFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    std::string mac = fdp.ConsumeRandomLengthString();

    MechConnectManager::GetInstance().RemoveMechInfoByMac(mac);
}

void CleanMechInfoFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t cleanCount = fdp.ConsumeIntegralInRange<uint32_t>(MIN_CLEAN_COUNT, MAX_CLEAN_COUNT);
    for (uint32_t i = 0; i < cleanCount; i++) {
        MechConnectManager::GetInstance().CleanMechInfo();
    }
}

void GetMechInfoFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    std::string mac = fdp.ConsumeRandomLengthString();
    MechInfo mechInfo;

    MechConnectManager::GetInstance().GetMechInfo(mac, mechInfo);
}

void SetRealMechNameFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    int32_t mechId = fdp.ConsumeIntegral<int32_t>();
    std::string name = fdp.ConsumeRandomLengthString();

    MechConnectManager::GetInstance().SetRealMechName(mechId, name);
}

void SetMechanicGattStateFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    std::string mac = fdp.ConsumeRandomLengthString();
    bool state = fdp.ConsumeBool();

    MechConnectManager::GetInstance().SetMechanicGattState(mac, state);
}

void GetMechanicGattStateFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    std::string mac = fdp.ConsumeRandomLengthString();
    bool state = false;

    MechConnectManager::GetInstance().GetMechanicGattState(mac, state);
}

void SetMechanicPairStateFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    std::string mac = fdp.ConsumeRandomLengthString();
    bool state = fdp.ConsumeBool();

    MechConnectManager::GetInstance().SetMechanicPairState(mac, state);
}

void GetMechanicPairStateFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    std::string mac = fdp.ConsumeRandomLengthString();
    bool state = false;

    MechConnectManager::GetInstance().GetMechanicPairState(mac, state);
}

void SetMechanicHidStateFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    std::string mac = fdp.ConsumeRandomLengthString();
    bool state = fdp.ConsumeBool();

    MechConnectManager::GetInstance().SetMechanicHidState(mac, state);
}

void GetMechanicHidStateFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    std::string mac = fdp.ConsumeRandomLengthString();
    bool state = false;

    MechConnectManager::GetInstance().GetMechanicHidState(mac, state);
}

void GetMechBasicInfoFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    int32_t mechId = fdp.ConsumeIntegral<int32_t>();
    MechInfo mechInfo;

    MechConnectManager::GetInstance().GetMechBasicInfo(mechId, mechInfo);
}

void GetConnectMechListFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t getCount = fdp.ConsumeIntegralInRange<uint32_t>(MIN_CLEAN_COUNT, MAX_CLEAN_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        std::set<MechInfo> mechInfos;
        MechConnectManager::GetInstance().GetConnectMechList(mechInfos);
    }
}

void NotifyMechStateFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    int32_t mechId = fdp.ConsumeIntegral<int32_t>();
    bool isAttached = fdp.ConsumeBool();

    MechConnectManager::GetInstance().NotifyMechState(mechId, isAttached);
}

void GetMechStateFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    int32_t mechId = fdp.ConsumeIntegral<int32_t>();

    MechConnectManager::GetInstance().GetMechState(mechId);
}

void AddDeviceChangeListenerFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    
    // Use fuzz data to decide which listener to test
    bool testNullListener = fdp.ConsumeBool();
    bool testValidListener = fdp.ConsumeBool();
    
    if (testNullListener) {
        std::shared_ptr<IMechConnectListener> nullListener = nullptr;
        MechConnectManager::GetInstance().AddDeviceChangeListener(nullListener);
    }
    
    if (testValidListener) {
        auto listener = std::make_shared<MockMechConnectListener>();
        MechConnectManager::GetInstance().AddDeviceChangeListener(listener);
    }
}

void RemoveDeviceChangeListenerFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    
    // Use fuzz data to decide which listener to test
    bool testNullListener = fdp.ConsumeBool();
    bool testValidListener = fdp.ConsumeBool();
    
    if (testNullListener) {
        std::shared_ptr<IMechConnectListener> nullListener = nullptr;
        MechConnectManager::GetInstance().RemoveDeviceChangeListener(nullListener);
    }
    
    if (testValidListener) {
        auto listener = std::make_shared<MockMechConnectListener>();
        MechConnectManager::GetInstance().RemoveDeviceChangeListener(listener);
    }
}

void GetMechInfosFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    uint32_t getCount = fdp.ConsumeIntegralInRange<uint32_t>(MIN_CLEAN_COUNT, MAX_CLEAN_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        const std::set<MechInfo>& mechInfos = MechConnectManager::GetInstance().GetMechInfos();
        (void)mechInfos;
    }
}

void McConnectManagerCombinedFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);

    MechConnectManager::GetInstance().Init();

    bool isBLEActive = fdp.ConsumeBool();
    MechConnectManager::GetInstance().UpdateBleStatus(isBLEActive);

    MechInfo mechInfo;
    mechInfo.mechId = fdp.ConsumeIntegral<int32_t>();
    mechInfo.mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());
    mechInfo.mechName = fdp.ConsumeRandomLengthString();
    mechInfo.mac = fdp.ConsumeRandomLengthString();
    mechInfo.state = static_cast<AttachmentState>(fdp.ConsumeIntegral<int32_t>());
    mechInfo.gattCoonectState = fdp.ConsumeBool();
    mechInfo.pairState = fdp.ConsumeBool();
    mechInfo.hidState = fdp.ConsumeBool();
    MechConnectManager::GetInstance().AddMechInfo(mechInfo);

    MechInfo resultInfo;
    MechConnectManager::GetInstance().GetMechInfo(mechInfo.mac, resultInfo);

    bool state = fdp.ConsumeBool();
    MechConnectManager::GetInstance().SetMechanicGattState(mechInfo.mac, state);
    MechConnectManager::GetInstance().SetMechanicPairState(mechInfo.mac, state);
    MechConnectManager::GetInstance().SetMechanicHidState(mechInfo.mac, state);

    bool resultState = false;
    MechConnectManager::GetInstance().GetMechanicGattState(mechInfo.mac, resultState);
    MechConnectManager::GetInstance().GetMechanicPairState(mechInfo.mac, resultState);
    MechConnectManager::GetInstance().GetMechanicHidState(mechInfo.mac, resultState);

    MechConnectManager::GetInstance().IsConnect();
    MechConnectManager::GetInstance().GetLocalDeviceBleStatus();

    MechConnectManager::GetInstance().GetMechState(mechInfo.mechId);

    std::string name = fdp.ConsumeRandomLengthString();
    MechConnectManager::GetInstance().SetRealMechName(mechInfo.mechId, name);

    MechConnectManager::GetInstance().RemoveMechInfoByMac(mechInfo.mac);

    MechConnectManager::GetInstance().CleanMechInfo();
    MechConnectManager::GetInstance().UnInit();
}

using FuzzTestFunc = void (*)(const uint8_t *, size_t);

FuzzTestFunc GetFuzzTestFunc(uint8_t selector)
{
    static const FuzzTestFunc fuzzTestFuncs[] = {
        InitFuzzTest,
        UpdateBleStatusFuzzTest,
        GetLocalDeviceBleStatusFuzzTest,
        IsConnectFuzzTest,
        AddMechInfoFuzzTest,
        RemoveMechInfoByMacFuzzTest,
        CleanMechInfoFuzzTest,
        GetMechInfoFuzzTest,
        SetRealMechNameFuzzTest,
        SetMechanicGattStateFuzzTest,
        GetMechanicGattStateFuzzTest,
        SetMechanicPairStateFuzzTest,
        GetMechanicPairStateFuzzTest,
        SetMechanicHidStateFuzzTest,
        GetMechanicHidStateFuzzTest,
        GetMechBasicInfoFuzzTest,
        GetConnectMechListFuzzTest,
        NotifyMechStateFuzzTest,
        GetMechStateFuzzTest,
        AddDeviceChangeListenerFuzzTest,
        RemoveDeviceChangeListenerFuzzTest,
        GetMechInfosFuzzTest,
        McConnectManagerCombinedFuzzTest
    };

    if (selector >= FUZZ_TEST_COUNT) {
        return nullptr;
    }
    return fuzzTestFuncs[selector];
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < OHOS::OPT_SIZE) {
        return 0;
    }
    FuzzedDataProvider fdp(data + size - OHOS::OPT_SIZE, OHOS::OPT_SIZE);
    uint8_t selector = fdp.ConsumeIntegral<uint8_t>() % FUZZ_TEST_COUNT;

    FuzzTestFunc func = GetFuzzTestFunc(selector);
    if (func != nullptr) {
        func(data, size - OHOS::OPT_SIZE);
    }
    return 0;
}
} // namespace OHOS