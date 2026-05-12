/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "mcregistermechpositioninfocmd_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <memory>
#include <securec.h>

#include "mc_register_mech_position_info_cmd.h"
#include "mc_data_buffer.h"

constexpr uint8_t FUZZ_TEST_ZERO = 0;
constexpr uint8_t FUZZ_TEST_ONE = 1;
constexpr uint8_t FUZZ_TEST_TWO = 2;
constexpr uint8_t FUZZ_TEST_THREE = 3;
constexpr uint8_t FUZZ_TEST_FOUR = 4;
constexpr uint8_t FUZZ_TEST_FIVE = 5;
constexpr uint8_t FUZZ_TEST_SIX = 6;
constexpr uint8_t FUZZ_TEST_SEVEN = 7;
constexpr uint8_t FUZZ_TEST_EIGHT = 8;
constexpr uint8_t FUZZ_TEST_NINE = 9;
constexpr uint8_t FUZZ_TEST_TEN = 10;

namespace OHOS {
namespace MechBodyController {
static constexpr uint32_t OPT_SIZE = 80;
FuzzedDataProvider *FDP;

void RegisterMechPositionInfoCmdMarshalFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechPositionInfoCmd cmd;
    auto buffer = cmd.Marshal();
    if (buffer != nullptr) {
        if (provider.ConsumeBool()) {
            uint8_t cmdSet = cmd.GetCmdSet();
            (void)cmdSet;
        }
        if (provider.ConsumeBool()) {
            uint8_t cmdId = cmd.GetCmdId();
            (void)cmdId;
        }
        if (provider.ConsumeBool()) {
            uint16_t reqSize = cmd.GetReqSize();
            (void)reqSize;
        }
        if (provider.ConsumeBool()) {
            uint16_t rspSize = cmd.GetRspSize();
            (void)rspSize;
        }
        if (provider.ConsumeBool()) {
            uint32_t timeoutMs = cmd.GetTimeoutMs();
            (void)timeoutMs;
        }
        if (provider.ConsumeBool()) {
            int32_t retryTimes = cmd.GetRetryTimes();
            (void)retryTimes;
        }
        if (provider.ConsumeBool()) {
            bool needResponse = cmd.NeedResponse();
            (void)needResponse;
        }
    }
}

void RegisterMechPositionInfoCmdUnmarshalFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechPositionInfoCmd cmd;

    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 20);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            dataBuffer->SetRange(0, dataSize);
            auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!bytes.empty() && dataBuffer->Data() != nullptr) {
                (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
            }
        }
    }

    bool result = cmd.Unmarshal(dataBuffer);
    const EulerAngles& position = cmd.GetPosition();
    
    (void)result;
    (void)position;
}

void RegisterMechPositionInfoCmdTriggerResponseFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechPositionInfoCmd cmd;

    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 20);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            dataBuffer->SetRange(0, dataSize);
            auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!bytes.empty() && dataBuffer->Data() != nullptr) {
                (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
            }
        }
    }

    cmd.TriggerResponse(dataBuffer);
}

void RegisterMechPositionInfoCmdGetPositionFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechPositionInfoCmd cmd;

    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 20);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            dataBuffer->SetRange(0, dataSize);
            auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!bytes.empty() && dataBuffer->Data() != nullptr) {
                (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
            }
        }
    }

    cmd.Unmarshal(dataBuffer);
    const EulerAngles& position = cmd.GetPosition();
    (void)position;
}

void RegisterMechPositionInfoCmdGetResultFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechPositionInfoCmd cmd;

    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 20);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            dataBuffer->SetRange(0, dataSize);
            auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!bytes.empty() && dataBuffer->Data() != nullptr) {
                (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
            }
        }
    }

    cmd.TriggerResponse(dataBuffer);
    uint8_t result = cmd.GetResult();
    (void)result;
}

void RegisterMechPositionInfoCmdWithCallbackFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechPositionInfoCmd cmd;

    bool shouldSetCallback = provider.ConsumeBool();
    if (shouldSetCallback) {
        cmd.SetResponseCallback([]() {
            // Callback function
        });
    }

    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 20);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            dataBuffer->SetRange(0, dataSize);
            auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!bytes.empty() && dataBuffer->Data() != nullptr) {
                (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
            }
        }
    }

    cmd.TriggerResponse(dataBuffer);
}

void RegisterMechPositionInfoCmdWithValidDataFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechPositionInfoCmd cmd;

    size_t dataSize = 14; // RPT_SIZE + BIT_OFFSET_2 = 12 + 2
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);

        float pitch = provider.ConsumeFloatingPoint<float>();
        float roll = provider.ConsumeFloatingPoint<float>();
        float yaw = provider.ConsumeFloatingPoint<float>();

        size_t offset = 0;
        if (dataBuffer->Data() != nullptr) {
            // Skip first 2 bytes (BIT_OFFSET_2)
            offset += BIT_OFFSET_2;
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &pitch, sizeof(pitch));
            offset += sizeof(pitch);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &roll, sizeof(roll));
            offset += sizeof(roll);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &yaw, sizeof(yaw));
        }
    }

    bool result = cmd.Unmarshal(dataBuffer);
    const EulerAngles& position = cmd.GetPosition();
    
    (void)result;
    (void)position;
}

void RegisterMechPositionInfoCmdWithAllValuesFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechPositionInfoCmd cmd;

    size_t dataSize = 14;
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);

        // Test with different position values
        float pitch = provider.ConsumeFloatingPoint<float>();
        float roll = provider.ConsumeFloatingPoint<float>();
        float yaw = provider.ConsumeFloatingPoint<float>();

        size_t offset = 0;
        if (dataBuffer->Data() != nullptr) {
            offset += BIT_OFFSET_2;
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &pitch, sizeof(pitch));
            offset += sizeof(pitch);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &roll, sizeof(roll));
            offset += sizeof(roll);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &yaw, sizeof(yaw));
        }
    }

    bool result = cmd.Unmarshal(dataBuffer);
    const EulerAngles& position = cmd.GetPosition();
    
    (void)result;
    (void)position;
}

void RegisterMechPositionInfoCmdWithValidResponseFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechPositionInfoCmd cmd;

    size_t dataSize = 1; // RSP_SIZE
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);

        uint8_t result = provider.ConsumeIntegral<uint8_t>();

        if (dataBuffer->Data() != nullptr) {
            (void)memcpy_s(dataBuffer->Data(), dataSize, &result, sizeof(result));
        }
    }

    cmd.TriggerResponse(dataBuffer);

    uint8_t result = cmd.GetResult();
    (void)result;
}

void RegisterMechPositionInfoCmdWithNullResponseFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechPositionInfoCmd cmd;

    cmd.TriggerResponse(nullptr);

    if (provider.ConsumeBool()) {
        uint8_t result = cmd.GetResult();
        (void)result;
    }
}

} // namespace MechBodyController
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < OHOS::MechBodyController::OPT_SIZE) {
        return 0;
    }
    FuzzedDataProvider fdp(data + size - OHOS::MechBodyController::OPT_SIZE, OHOS::MechBodyController::OPT_SIZE);
    uint8_t selector = fdp.ConsumeIntegral<uint8_t>() % FUZZ_TEST_TEN;
    size_t testSize = size - OHOS::MechBodyController::OPT_SIZE;

    using namespace OHOS::MechBodyController;
    switch (selector) {
        case FUZZ_TEST_ZERO: RegisterMechPositionInfoCmdMarshalFuzzTest(data, testSize); break;
        case FUZZ_TEST_ONE: RegisterMechPositionInfoCmdUnmarshalFuzzTest(data, testSize); break;
        case FUZZ_TEST_TWO: RegisterMechPositionInfoCmdTriggerResponseFuzzTest(data, testSize); break;
        case FUZZ_TEST_THREE: RegisterMechPositionInfoCmdGetPositionFuzzTest(data, testSize); break;
        case FUZZ_TEST_FOUR: RegisterMechPositionInfoCmdGetResultFuzzTest(data, testSize); break;
        case FUZZ_TEST_FIVE: RegisterMechPositionInfoCmdWithCallbackFuzzTest(data, testSize); break;
        case FUZZ_TEST_SIX: RegisterMechPositionInfoCmdWithValidDataFuzzTest(data, testSize); break;
        case FUZZ_TEST_SEVEN: RegisterMechPositionInfoCmdWithAllValuesFuzzTest(data, testSize); break;
        case FUZZ_TEST_EIGHT: RegisterMechPositionInfoCmdWithValidResponseFuzzTest(data, testSize); break;
        case FUZZ_TEST_NINE: RegisterMechPositionInfoCmdWithNullResponseFuzzTest(data, testSize); break;
        default: break;
    }
    return 0;
}