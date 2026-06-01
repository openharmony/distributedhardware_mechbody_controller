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

#include "mcconnectmanagertwo_fuzzer.h"

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
const std::string TAG = "McConnectManagerTwoFuzzTest";

// Fuzz test constants
constexpr uint32_t MIN_CLEAN_COUNT = 1;
constexpr uint32_t MAX_CLEAN_COUNT = 5;
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

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::SetMechanicGattStateFuzzTest(data, size);
    OHOS::GetMechanicGattStateFuzzTest(data, size);
    OHOS::SetMechanicPairStateFuzzTest(data, size);
    OHOS::GetMechanicPairStateFuzzTest(data, size);
    OHOS::SetMechanicHidStateFuzzTest(data, size);
    OHOS::GetMechanicHidStateFuzzTest(data, size);
    OHOS::GetMechBasicInfoFuzzTest(data, size);
    OHOS::GetConnectMechListFuzzTest(data, size);
    OHOS::NotifyMechStateFuzzTest(data, size);
    return 0;
}
} // namespace OHOS