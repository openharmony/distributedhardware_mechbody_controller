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

#include "mcregistermechcontrolresultcmd_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <memory>
#include <securec.h>

#include "mc_register_mech_control_result_cmd.h"
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

namespace OHOS {
namespace MechBodyController {
static constexpr uint32_t OPT_SIZE = 80;
FuzzedDataProvider *FDP;

void RegisterMechControlResultCmdMarshalFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    RegisterMechControlResultCmd cmd;
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

void RegisterMechControlResultCmdUnmarshalFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    RegisterMechControlResultCmd cmd;
    
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
    uint8_t controlResult = cmd.GetControlResult();
    uint8_t taskId = cmd.GetTaskId();
    
    (void)result;
    (void)controlResult;
    (void)taskId;
}

void RegisterMechControlResultCmdTriggerResponseFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    RegisterMechControlResultCmd cmd;
    
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

void RegisterMechControlResultCmdGetControlResultFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    RegisterMechControlResultCmd cmd;
    
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
    uint8_t controlResult = cmd.GetControlResult();
    (void)controlResult;
}

void RegisterMechControlResultCmdGetTaskIdFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    RegisterMechControlResultCmd cmd;
    
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
    uint8_t taskId = cmd.GetTaskId();
    (void)taskId;
}

void RegisterMechControlResultCmdGetResultFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    RegisterMechControlResultCmd cmd;
    
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
    uint8_t result = cmd.GetResult();
    (void)result;
}

void RegisterMechControlResultCmdWithCallbackFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    RegisterMechControlResultCmd cmd;
    
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

void RegisterMechControlResultCmdWithValidDataFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    RegisterMechControlResultCmd cmd;
    
    size_t dataSize = 3; // RPT_SIZE + BIT_OFFSET_2 = 1 + 2
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);
        
        uint8_t controlResult = provider.ConsumeIntegral<uint8_t>();
        uint8_t taskId = provider.ConsumeIntegral<uint8_t>();
        
        size_t offset = 0;
        if (dataBuffer->Data() != nullptr) {
            // Skip first 2 bytes (BIT_OFFSET_2)
            offset += BIT_OFFSET_2;
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &controlResult, sizeof(controlResult));
            offset += sizeof(controlResult);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &taskId, sizeof(taskId));
        }
    }

    bool result = cmd.Unmarshal(dataBuffer);
    uint8_t controlResult = cmd.GetControlResult();
    uint8_t taskId = cmd.GetTaskId();
    
    (void)result;
    (void)controlResult;
    (void)taskId;
}

void RegisterMechControlResultCmdWithAllValuesFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    RegisterMechControlResultCmd cmd;
    
    size_t dataSize = 3;
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);
        
        // Test with different control result values
        uint8_t controlResult = provider.ConsumeIntegralInRange<uint8_t>(0, 255);
        uint8_t taskId = provider.ConsumeIntegralInRange<uint8_t>(0, 255);
        
        size_t offset = 0;
        if (dataBuffer->Data() != nullptr) {
            offset += BIT_OFFSET_2;
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &controlResult, sizeof(controlResult));
            offset += sizeof(controlResult);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &taskId, sizeof(taskId));
        }
    }

    bool result = cmd.Unmarshal(dataBuffer);
    uint8_t controlResult = cmd.GetControlResult();
    uint8_t taskId = cmd.GetTaskId();
    
    (void)result;
    (void)controlResult;
    (void)taskId;
}

} // namespace MechBodyController
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    if (size < OHOS::MechBodyController::OPT_SIZE) {
        return 0;
    }
    FuzzedDataProvider fdp(data + size - OHOS::MechBodyController::OPT_SIZE, OHOS::MechBodyController::OPT_SIZE);
    // randomly select a test API
    uint8_t selector = fdp.ConsumeIntegral<uint8_t>() % FUZZ_TEST_NINE;
    switch (selector) {
        case FUZZ_TEST_ZERO:
            OHOS::MechBodyController::RegisterMechControlResultCmdMarshalFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_ONE:
            OHOS::MechBodyController::RegisterMechControlResultCmdUnmarshalFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_TWO:
            OHOS::MechBodyController::RegisterMechControlResultCmdTriggerResponseFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_THREE:
            OHOS::MechBodyController::RegisterMechControlResultCmdGetControlResultFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_FOUR:
            OHOS::MechBodyController::RegisterMechControlResultCmdGetTaskIdFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_FIVE:
            OHOS::MechBodyController::RegisterMechControlResultCmdGetResultFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_SIX:
            OHOS::MechBodyController::RegisterMechControlResultCmdWithCallbackFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_SEVEN:
            OHOS::MechBodyController::RegisterMechControlResultCmdWithValidDataFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_EIGHT:
            OHOS::MechBodyController::RegisterMechControlResultCmdWithAllValuesFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        default:
            break;
    }
    return 0;
}