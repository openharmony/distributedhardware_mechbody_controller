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
#include "normalgetmechlimitinfo_fuzzer.h"
#include "mc_normal_get_mech_limit_info_cmd.h"
#include "mc_data_buffer.h"

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

constexpr size_t RSP_SIZE = 25;
constexpr size_t BIT_OFFSET_2 = 2;

// Fuzz test constants
constexpr uint32_t MIN_COUNT = 1;
constexpr uint32_t MAX_COUNT = 5;
constexpr size_t MIN_BUFFER_SIZE = 0;
constexpr size_t MAX_SMALL_BUFFER_SIZE = 10;
constexpr size_t EXTRA_BUFFER_SIZE = 10;
constexpr uint8_t HEADER_BYTE1 = 0x02;
constexpr uint8_t HEADER_BYTE2 = 0x08;
constexpr int32_t MIN_TEST_ID = 0;
constexpr int32_t MAX_TEST_ID = 7;

enum class TestFunctionId {
    FUZZ_CONSTRUCTOR = 0,
    FUZZ_MARSHAL = 1,
    FUZZ_TRIGGER_RESPONSE_WITH_NULL = 2,
    FUZZ_TRIGGER_RESPONSE_WITH_SMALL_BUFFER = 3,
    FUZZ_TRIGGER_RESPONSE_WITH_VALID_DATA = 4,
    FUZZ_GET_PARAMS = 5,
    FUZZ_GET_RESULT = 6,
    FUZZ_FULL_WORKFLOW = 7
};

void FuzzConstructor(FuzzedDataProvider &provider)
{
    uint32_t count = provider.ConsumeIntegralInRange<uint32_t>(MIN_COUNT, MAX_COUNT);
    for (uint32_t i = 0; i < count; i++) {
        NormalGetMechLimitInfoCmd cmd;
    }
}

void FuzzMarshal(FuzzedDataProvider &provider)
{
    uint32_t count = provider.ConsumeIntegralInRange<uint32_t>(MIN_COUNT, MAX_COUNT);
    for (uint32_t i = 0; i < count; i++) {
        NormalGetMechLimitInfoCmd cmd;
        auto buffer = cmd.Marshal();
    }
}

void FuzzTriggerResponseWithNull(FuzzedDataProvider &provider)
{
    uint32_t count = provider.ConsumeIntegralInRange<uint32_t>(MIN_COUNT, MAX_COUNT);
    for (uint32_t i = 0; i < count; i++) {
        NormalGetMechLimitInfoCmd cmd;
        cmd.TriggerResponse(nullptr);
    }
}

void FuzzTriggerResponseWithSmallBuffer(FuzzedDataProvider &provider)
{
    NormalGetMechLimitInfoCmd cmd;
    size_t bufferSize = provider.ConsumeIntegralInRange<size_t>(MIN_BUFFER_SIZE, MAX_SMALL_BUFFER_SIZE);
    auto buffer = std::make_shared<MechDataBuffer>(bufferSize);
    if (buffer != nullptr) {
        cmd.TriggerResponse(buffer);
    }
}

void FuzzTriggerResponseWithValidData(FuzzedDataProvider &provider)
{
    NormalGetMechLimitInfoCmd cmd;

    size_t totalSize = RSP_SIZE + BIT_OFFSET_2;

    auto buffer = std::make_shared<MechDataBuffer>(totalSize + EXTRA_BUFFER_SIZE);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(HEADER_BYTE1);
    buffer->AppendUint8(HEADER_BYTE2);

    for (size_t i = 0; i < RSP_SIZE; ++i) {
        buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    }

    cmd.TriggerResponse(buffer);
}

void FuzzGetParams(FuzzedDataProvider &provider)
{
    uint32_t count = provider.ConsumeIntegralInRange<uint32_t>(MIN_COUNT, MAX_COUNT);
    for (uint32_t i = 0; i < count; i++) {
        NormalGetMechLimitInfoCmd cmd;
        const RotateDegreeLimit& params = cmd.GetParams();
        (void)params;
    }
}

void FuzzGetResult(FuzzedDataProvider &provider)
{
    uint32_t count = provider.ConsumeIntegralInRange<uint32_t>(MIN_COUNT, MAX_COUNT);
    for (uint32_t i = 0; i < count; i++) {
        NormalGetMechLimitInfoCmd cmd;
        uint8_t result = cmd.GetResult();
        (void)result;
    }
}

void FuzzFullWorkflow(FuzzedDataProvider &provider)
{
    NormalGetMechLimitInfoCmd cmd;

    auto marshalBuffer = cmd.Marshal();

    bool shouldTestNull = provider.ConsumeBool();
    if (shouldTestNull) {
        cmd.TriggerResponse(nullptr);
    } else {
        size_t totalSize = RSP_SIZE + BIT_OFFSET_2;

        auto buffer = std::make_shared<MechDataBuffer>(totalSize + EXTRA_BUFFER_SIZE);
        if (buffer != nullptr) {
            buffer->AppendUint8(HEADER_BYTE1);
            buffer->AppendUint8(HEADER_BYTE2);

            for (size_t i = 0; i < RSP_SIZE; ++i) {
                buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
            }

            cmd.TriggerResponse(buffer);
            const RotateDegreeLimit& params = cmd.GetParams();
            uint8_t result = cmd.GetResult();
            (void)params;
            (void)result;
        }
    }
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(MIN_TEST_ID, MAX_TEST_ID);

    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_CONSTRUCTOR:
            FuzzConstructor(provider);
            break;
        case TestFunctionId::FUZZ_MARSHAL:
            FuzzMarshal(provider);
            break;
        case TestFunctionId::FUZZ_TRIGGER_RESPONSE_WITH_NULL:
            FuzzTriggerResponseWithNull(provider);
            break;
        case TestFunctionId::FUZZ_TRIGGER_RESPONSE_WITH_SMALL_BUFFER:
            FuzzTriggerResponseWithSmallBuffer(provider);
            break;
        case TestFunctionId::FUZZ_TRIGGER_RESPONSE_WITH_VALID_DATA:
            FuzzTriggerResponseWithValidData(provider);
            break;
        case TestFunctionId::FUZZ_GET_PARAMS:
            FuzzGetParams(provider);
            break;
        case TestFunctionId::FUZZ_GET_RESULT:
            FuzzGetResult(provider);
            break;
        case TestFunctionId::FUZZ_FULL_WORKFLOW:
            FuzzFullWorkflow(provider);
            break;
        default:
            break;
    }

    return 0;
}