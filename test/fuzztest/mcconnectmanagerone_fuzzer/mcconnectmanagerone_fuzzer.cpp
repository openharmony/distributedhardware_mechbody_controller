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

#include "mcconnectmanagerone_fuzzer.h"

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
const std::string TAG = "McConnectManagerOneFuzzTest";

// Fuzz test constants
constexpr uint32_t MIN_INIT_COUNT = 1;
constexpr uint32_t MAX_INIT_COUNT = 3;
constexpr uint32_t MIN_CHECK_COUNT = 1;
constexpr uint32_t MAX_CHECK_COUNT = 10;
constexpr uint32_t MIN_CLEAN_COUNT = 1;
constexpr uint32_t MAX_CLEAN_COUNT = 5;
}

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

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::InitFuzzTest(data, size);
    OHOS::UpdateBleStatusFuzzTest(data, size);
    OHOS::GetLocalDeviceBleStatusFuzzTest(data, size);
    OHOS::IsConnectFuzzTest(data, size);
    OHOS::AddMechInfoFuzzTest(data, size);
    OHOS::RemoveMechInfoByMacFuzzTest(data, size);
    OHOS::CleanMechInfoFuzzTest(data, size);
    OHOS::GetMechInfoFuzzTest(data, size);
    OHOS::SetRealMechNameFuzzTest(data, size);
    return 0;
}
} // namespace OHOS