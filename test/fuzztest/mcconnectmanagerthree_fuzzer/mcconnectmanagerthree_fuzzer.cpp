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

#include "mcconnectmanagerthree_fuzzer.h"

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
const std::string TAG = "McConnectManagerThreeFuzzTest";

// Fuzz test constants
constexpr uint32_t MIN_CLEAN_COUNT = 1;
constexpr uint32_t MAX_CLEAN_COUNT = 5;
}

// Mock listener for testing
class MockMechConnectListener : public IMechConnectListener {
public:
    void OnMechConnect(const MechInfo& mechInfo) override {}
    void OnMechDisconnect(const MechInfo& mechInfo) override {}
};

void GetMechStateFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    int32_t mechId = fdp.ConsumeIntegral<int32_t>();

    MechConnectManager::GetInstance().GetMechState(mechId);
}

void GetMechInfosFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);
    MechConnectManager::GetInstance().Init();
    uint32_t getCount = fdp.ConsumeIntegralInRange<uint32_t>(MIN_CLEAN_COUNT, MAX_CLEAN_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        const std::set<MechInfo>& mechInfos = MechConnectManager::GetInstance().GetMechInfos();
        (void)mechInfos;
    }
    // Test null listener
    std::shared_ptr<IMechConnectListener> nullListener = nullptr;
    MechConnectManager::GetInstance().AddDeviceChangeListener(nullListener);
    MechConnectManager::GetInstance().RemoveDeviceChangeListener(nullListener);

    // Test valid listener
    auto listener = std::make_shared<MockMechConnectListener>();
    MechConnectManager::GetInstance().AddDeviceChangeListener(listener);

    // Test duplicate listener (same shared_ptr)
    MechConnectManager::GetInstance().AddDeviceChangeListener(listener);
    MechConnectManager::GetInstance().RemoveDeviceChangeListener(listener);

    // Test another valid listener
    auto listener2 = std::make_shared<MockMechConnectListener>();
    MechConnectManager::GetInstance().AddDeviceChangeListener(listener2);
    MechConnectManager::GetInstance().RemoveDeviceChangeListener(listener2);

    MechConnectManager::GetInstance().UnInit();
}

void NotifyMechConnectFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);

    MechConnectManager::GetInstance().Init();

    MechInfo mechInfo;
    mechInfo.mechId = fdp.ConsumeIntegral<int32_t>();
    mechInfo.mechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());
    mechInfo.mechName = fdp.ConsumeRandomLengthString();
    mechInfo.mac = fdp.ConsumeRandomLengthString();
    mechInfo.state = static_cast<AttachmentState>(fdp.ConsumeIntegral<int32_t>());
    mechInfo.gattCoonectState = fdp.ConsumeBool();
    mechInfo.pairState = fdp.ConsumeBool();
    mechInfo.hidState = fdp.ConsumeBool();
    mechInfo.isFirstConnect = fdp.ConsumeBool();
    mechInfo.deviceIdentifier = fdp.ConsumeIntegral<uint32_t>();

    MechConnectManager::GetInstance().AddMechInfo(mechInfo);
    MechConnectManager::GetInstance().NotifyMechConnect(mechInfo);

    MechConnectManager::GetInstance().CleanMechInfo();
    MechConnectManager::GetInstance().UnInit();
}

void SetMechTypeFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider fdp(data, size);

    MechConnectManager::GetInstance().Init();

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

    int32_t targetMechId = fdp.ConsumeIntegral<int32_t>();
    MechType newMechType = static_cast<MechType>(fdp.ConsumeIntegral<int32_t>());
    MechConnectManager::GetInstance().SetMechType(targetMechId, newMechType);

    MechConnectManager::GetInstance().CleanMechInfo();
    MechConnectManager::GetInstance().UnInit();
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

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::GetMechStateFuzzTest(data, size);
    OHOS::GetMechInfosFuzzTest(data, size);
    OHOS::NotifyMechConnectFuzzTest(data, size);
    OHOS::SetMechTypeFuzzTest(data, size);
    OHOS::McConnectManagerCombinedFuzzTest(data, size);
    return 0;
}
} // namespace OHOS