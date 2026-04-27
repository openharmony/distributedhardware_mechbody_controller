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
#include "registermechcliffinfocmd_fuzzer.h"
#include "mc_register_mech_cliff_info_cmd.h"
#include "mc_data_buffer.h"

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

constexpr size_t RPT_SIZE = 1;
constexpr size_t BIT_OFFSET_2 = 2;
constexpr size_t MAX_CLIFF_NUMS = 255;
constexpr size_t FUZZ_SMALL_BUFFER_SIZE = 10;
constexpr size_t FUZZ_BUFFER_SIZE = 20;
constexpr uint8_t FUZZ_MIN_VALUE = 1;
constexpr uint8_t FUZZ_MIN_MULTI_VALUE = 2;
constexpr size_t FUZZ_ZERO = 0;
constexpr size_t FUZZ_CLIFF_INFO_SIZE = 1;
constexpr int32_t FUZZ_MAX_UNMARSHAL_TEST_ID = 12;

enum class TestFunctionId {
    FUZZ_CONSTRUCTOR = 0,
    FUZZ_MARSHAL = 1,
    FUZZ_UNMARSHAL_WITH_NULL = 2,
    FUZZ_UNMARSHAL_WITH_SMALL_BUFFER = 3,
    FUZZ_UNMARSHAL_WITH_ZERO_CLIFF = 4,
    FUZZ_UNMARSHAL_WITH_ONE_CLIFF = 5,
    FUZZ_UNMARSHAL_WITH_MULTIPLE_CLIFFS = 6,
    FUZZ_UNMARSHAL_WITH_MAX_CLIFFS = 7,
    FUZZ_UNMARSHAL_WITH_INVALID_LENGTH = 8,
    FUZZ_UNMARSHAL_WITH_RANDOM_DATA = 9,
    FUZZ_TRIGGER_RESPONSE_WITH_NULL = 10,
    FUZZ_TRIGGER_RESPONSE_WITH_VALID_DATA = 11,
    FUZZ_GET_CLIFFS = 12,
    FUZZ_GET_CLIFF_NUMS = 13,
    FUZZ_GET_RESULT = 14,
    FUZZ_FULL_WORKFLOW = 15
};

void FuzzConstructor(FuzzedDataProvider &provider)
{
    (void)provider;
    RegisterMechCliffInfoCmd cmd;
}

void FuzzMarshal(FuzzedDataProvider &provider)
{
    (void)provider;
    RegisterMechCliffInfoCmd cmd;
    auto buffer = cmd.Marshal();
}

void FuzzUnmarshalWithNull(FuzzedDataProvider &provider)
{
    (void)provider;
    RegisterMechCliffInfoCmd cmd;
    bool result = cmd.Unmarshal(nullptr);
    (void)result;
}

void FuzzUnmarshalWithSmallBuffer(FuzzedDataProvider &provider)
{
    RegisterMechCliffInfoCmd cmd;
    size_t bufferSize = provider.ConsumeIntegralInRange<size_t>(FUZZ_ZERO, FUZZ_SMALL_BUFFER_SIZE);
    auto buffer = std::make_shared<MechDataBuffer>(bufferSize);
    if (buffer != nullptr) {
        bool result = cmd.Unmarshal(buffer);
        (void)result;
    }
}

void FuzzUnmarshalWithZeroCliff(FuzzedDataProvider &provider)
{
    RegisterMechCliffInfoCmd cmd;

    size_t totalSize = RPT_SIZE + BIT_OFFSET_2;

    auto buffer = std::make_shared<MechDataBuffer>(totalSize + FUZZ_SMALL_BUFFER_SIZE);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x40);

    for (size_t i = FUZZ_ZERO; i < BIT_OFFSET_2; i++) {
        buffer->AppendUint8(0x00);
    }

    buffer->AppendUint8(0x00);

    bool result = cmd.Unmarshal(buffer);
    (void)result;
}

void FuzzUnmarshalWithOneCliff(FuzzedDataProvider &provider)
{
    RegisterMechCliffInfoCmd cmd;

    size_t totalSize = RPT_SIZE + BIT_OFFSET_2 + BIT_OFFSET_2 + FUZZ_CLIFF_INFO_SIZE;

    auto buffer = std::make_shared<MechDataBuffer>(totalSize + FUZZ_SMALL_BUFFER_SIZE);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x40);

    for (size_t i = FUZZ_ZERO; i < BIT_OFFSET_2; i++) {
        buffer->AppendUint8(0x00);
    }

    buffer->AppendUint8(0x01);
    int16_t direction = provider.ConsumeIntegral<int16_t>();
    buffer->AppendInt16(direction);
    buffer->AppendUint8(0x00);

    bool result = cmd.Unmarshal(buffer);
    (void)result;
}

void FuzzUnmarshalWithMultipleCliffs(FuzzedDataProvider &provider)
{
    RegisterMechCliffInfoCmd cmd;

    uint8_t cliffNums = provider.ConsumeIntegralInRange<uint8_t>(FUZZ_MIN_MULTI_VALUE, FUZZ_SMALL_BUFFER_SIZE);
    size_t totalSize = RPT_SIZE + BIT_OFFSET_2 + cliffNums * (BIT_OFFSET_2 + FUZZ_CLIFF_INFO_SIZE);

    auto buffer = std::make_shared<MechDataBuffer>(totalSize + FUZZ_SMALL_BUFFER_SIZE);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x40);

    for (size_t i = FUZZ_ZERO; i < BIT_OFFSET_2; i++) {
        buffer->AppendUint8(0x00);
    }

    buffer->AppendUint8(cliffNums);

    for (uint8_t i = FUZZ_ZERO; i < cliffNums; i++) {
        int16_t direction = provider.ConsumeIntegral<int16_t>();
        buffer->AppendInt16(direction);
        buffer->AppendUint8(0x00);
    }

    bool result = cmd.Unmarshal(buffer);
    (void)result;
}

void FuzzUnmarshalWithMaxCliffs(FuzzedDataProvider &provider)
{
    (void)provider;
    RegisterMechCliffInfoCmd cmd;

    uint8_t cliffNums = MAX_CLIFF_NUMS;
    size_t totalSize = RPT_SIZE + BIT_OFFSET_2 + cliffNums * (BIT_OFFSET_2 + FUZZ_CLIFF_INFO_SIZE);

    auto buffer = std::make_shared<MechDataBuffer>(totalSize + FUZZ_SMALL_BUFFER_SIZE);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x40);

    for (size_t i = FUZZ_ZERO; i < BIT_OFFSET_2; i++) {
        buffer->AppendUint8(0x00);
    }

    buffer->AppendUint8(cliffNums);

    for (uint8_t i = FUZZ_ZERO; i < cliffNums; i++) {
        buffer->AppendInt16(0x0000);
        buffer->AppendUint8(0x00);
    }

    bool result = cmd.Unmarshal(buffer);
    (void)result;
}

void FuzzUnmarshalWithInvalidLength(FuzzedDataProvider &provider)
{
    RegisterMechCliffInfoCmd cmd;

    size_t totalSize = RPT_SIZE + BIT_OFFSET_2 + BIT_OFFSET_2 + FUZZ_CLIFF_INFO_SIZE;

    auto buffer = std::make_shared<MechDataBuffer>(totalSize + FUZZ_SMALL_BUFFER_SIZE);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x40);

    for (size_t i = FUZZ_ZERO; i < BIT_OFFSET_2; i++) {
        buffer->AppendUint8(0x00);
    }

    buffer->AppendUint8(0x01);
    buffer->AppendInt16(0x0000);
    buffer->AppendUint8(provider.ConsumeIntegralInRange<uint8_t>(FUZZ_MIN_VALUE, MAX_CLIFF_NUMS));

    bool result = cmd.Unmarshal(buffer);
    (void)result;
}

void FuzzUnmarshalWithRandomData(FuzzedDataProvider &provider)
{
    RegisterMechCliffInfoCmd cmd;

    uint8_t cliffNums = provider.ConsumeIntegral<uint8_t>();
    size_t totalSize = RPT_SIZE + BIT_OFFSET_2 + cliffNums * (BIT_OFFSET_2 + FUZZ_CLIFF_INFO_SIZE);

    auto buffer = std::make_shared<MechDataBuffer>(totalSize + FUZZ_SMALL_BUFFER_SIZE);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x40);

    for (size_t i = FUZZ_ZERO; i < BIT_OFFSET_2; i++) {
        buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    }

    buffer->AppendUint8(cliffNums);

    for (uint8_t i = FUZZ_ZERO; i < cliffNums; i++) {
        buffer->AppendInt16(provider.ConsumeIntegral<int16_t>());
        buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    }

    bool result = cmd.Unmarshal(buffer);
    (void)result;
}

void FuzzTriggerResponseWithNull(FuzzedDataProvider &provider)
{
    (void)provider;
    RegisterMechCliffInfoCmd cmd;
    cmd.TriggerResponse(nullptr);
}

void FuzzTriggerResponseWithValidData(FuzzedDataProvider &provider)
{
    RegisterMechCliffInfoCmd cmd;

    auto buffer = std::make_shared<MechDataBuffer>(FUZZ_BUFFER_SIZE);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());

    cmd.TriggerResponse(buffer);
}

void FuzzGetCliffs(FuzzedDataProvider &provider)
{
    (void)provider;
    RegisterMechCliffInfoCmd cmd;
    std::vector<CliffInfo> cliffs = cmd.GetCliffs();
    (void)cliffs;
}

void FuzzGetCliffNums(FuzzedDataProvider &provider)
{
    (void)provider;
    RegisterMechCliffInfoCmd cmd;
    uint8_t cliffNums = cmd.GetCliffNums();
    (void)cliffNums;
}

void FuzzGetResult(FuzzedDataProvider &provider)
{
    (void)provider;
    RegisterMechCliffInfoCmd cmd;
    uint8_t result = cmd.GetResult();
    (void)result;
}

void FuzzFullWorkflow(FuzzedDataProvider &provider)
{
    RegisterMechCliffInfoCmd cmd;

    auto marshalBuffer = cmd.Marshal();

    bool shouldTestNull = provider.ConsumeBool();
    if (shouldTestNull) {
        cmd.Unmarshal(nullptr);
        cmd.TriggerResponse(nullptr);
    } else {
        uint8_t cliffNums = provider.ConsumeIntegralInRange<uint8_t>(FUZZ_ZERO, FUZZ_SMALL_BUFFER_SIZE);
        size_t totalSize = RPT_SIZE + BIT_OFFSET_2 + cliffNums * (BIT_OFFSET_2 + FUZZ_CLIFF_INFO_SIZE);

        auto buffer = std::make_shared<MechDataBuffer>(totalSize + FUZZ_SMALL_BUFFER_SIZE);
        if (buffer != nullptr) {
            buffer->AppendUint8(0x03);
            buffer->AppendUint8(0x40);

            for (size_t i = FUZZ_ZERO; i < BIT_OFFSET_2; i++) {
                buffer->AppendUint8(0x00);
            }

            buffer->AppendUint8(cliffNums);

            for (uint8_t i = FUZZ_ZERO; i < cliffNums; i++) {
                buffer->AppendInt16(provider.ConsumeIntegral<int16_t>());
                buffer->AppendUint8(0x00);
            }

            bool result = cmd.Unmarshal(buffer);
            (void)result;

            std::vector<CliffInfo> cliffs = cmd.GetCliffs();
            uint8_t cliffNumsResult = cmd.GetCliffNums();
            (void)cliffs;
            (void)cliffNumsResult;

            auto responseBuffer = std::make_shared<MechDataBuffer>(FUZZ_BUFFER_SIZE);
            if (responseBuffer != nullptr) {
                responseBuffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
                cmd.TriggerResponse(responseBuffer);
            }

            uint8_t resultValue = cmd.GetResult();
            (void)resultValue;
        }
    }
}

void RunBasicFuzzTests(FuzzedDataProvider &provider, int32_t testFunctionId)
{
    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_CONSTRUCTOR:
            FuzzConstructor(provider);
            break;
        case TestFunctionId::FUZZ_MARSHAL:
            FuzzMarshal(provider);
            break;
        case TestFunctionId::FUZZ_GET_CLIFFS:
            FuzzGetCliffs(provider);
            break;
        case TestFunctionId::FUZZ_GET_CLIFF_NUMS:
            FuzzGetCliffNums(provider);
            break;
        case TestFunctionId::FUZZ_GET_RESULT:
            FuzzGetResult(provider);
            break;
        default:
            break;
    }
}

void RunUnmarshalFuzzTests(FuzzedDataProvider &provider, int32_t testFunctionId)
{
    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_UNMARSHAL_WITH_NULL:
            FuzzUnmarshalWithNull(provider);
            break;
        case TestFunctionId::FUZZ_UNMARSHAL_WITH_SMALL_BUFFER:
            FuzzUnmarshalWithSmallBuffer(provider);
            break;
        case TestFunctionId::FUZZ_UNMARSHAL_WITH_ZERO_CLIFF:
            FuzzUnmarshalWithZeroCliff(provider);
            break;
        case TestFunctionId::FUZZ_UNMARSHAL_WITH_ONE_CLIFF:
            FuzzUnmarshalWithOneCliff(provider);
            break;
        case TestFunctionId::FUZZ_UNMARSHAL_WITH_MULTIPLE_CLIFFS:
            FuzzUnmarshalWithMultipleCliffs(provider);
            break;
        case TestFunctionId::FUZZ_UNMARSHAL_WITH_MAX_CLIFFS:
            FuzzUnmarshalWithMaxCliffs(provider);
            break;
        case TestFunctionId::FUZZ_UNMARSHAL_WITH_INVALID_LENGTH:
            FuzzUnmarshalWithInvalidLength(provider);
            break;
        case TestFunctionId::FUZZ_UNMARSHAL_WITH_RANDOM_DATA:
            FuzzUnmarshalWithRandomData(provider);
            break;
        default:
            break;
    }
}

void RunTriggerResponseFuzzTests(FuzzedDataProvider &provider, int32_t testFunctionId)
{
    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_TRIGGER_RESPONSE_WITH_NULL:
            FuzzTriggerResponseWithNull(provider);
            break;
        case TestFunctionId::FUZZ_TRIGGER_RESPONSE_WITH_VALID_DATA:
            FuzzTriggerResponseWithValidData(provider);
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
    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0, 15);

    if (testFunctionId <= 4) {
        RunBasicFuzzTests(provider, testFunctionId);
    } else if (testFunctionId <= FUZZ_MAX_UNMARSHAL_TEST_ID) {
        RunUnmarshalFuzzTests(provider, testFunctionId);
    } else {
        RunTriggerResponseFuzzTests(provider, testFunctionId);
    }
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    RunFuzzTest(provider);
    return 0;
}