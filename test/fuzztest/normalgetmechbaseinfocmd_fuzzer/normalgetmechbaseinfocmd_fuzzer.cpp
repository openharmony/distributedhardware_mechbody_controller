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
#include "normalgetmechbaseinfocmd_fuzzer.h"
#include "mc_normal_get_mech_base_info_cmd.h"
#include "mc_data_buffer.h"

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

constexpr size_t BIT_OFFSET_2 = 2;
constexpr size_t RSP_SIZE = 30;

enum class TestFunctionId {
    FUZZ_CONSTRUCTOR = 0,
    FUZZ_MARSHAL = 1,
    FUZZ_MARSHAL_MULTIPLE_TIMES = 2,
    FUZZ_TRIGGER_RESPONSE_WITH_NULL = 3,
    FUZZ_TRIGGER_RESPONSE_WITH_SMALL_BUFFER = 4,
    FUZZ_TRIGGER_RESPONSE_WITH_VALID_DATA = 5,
    FUZZ_TRIGGER_RESPONSE_WITH_CALLBACK = 6
};

void FuzzConstructor(FuzzedDataProvider &provider)
{
    if (provider.ConsumeBool()) {
        NormalGetMechBaseInfoCmd cmd;
    }
}

void FuzzMarshal(FuzzedDataProvider &provider)
{
    if (provider.ConsumeBool()) {
        NormalGetMechBaseInfoCmd cmd;
        auto buffer = cmd.Marshal();
    }
}

void FuzzMarshalMultipleTimes(FuzzedDataProvider &provider)
{
    NormalGetMechBaseInfoCmd cmd;
    uint32_t iterations = provider.ConsumeIntegralInRange<uint32_t>(1, 10);

    for (uint32_t i = 0; i < iterations; ++i) {
        auto buffer = cmd.Marshal();
    }
}

void FuzzTriggerResponseWithNull(FuzzedDataProvider &provider)
{
    if (provider.ConsumeBool()) {
        NormalGetMechBaseInfoCmd cmd;
        cmd.TriggerResponse(nullptr);
    }
}

void FuzzTriggerResponseWithSmallBuffer(FuzzedDataProvider &provider)
{
    NormalGetMechBaseInfoCmd cmd;
    size_t bufferSize = provider.ConsumeIntegralInRange<size_t>(0, 10);
    auto buffer = std::make_shared<MechDataBuffer>(bufferSize);
    if (buffer != nullptr) {
        cmd.TriggerResponse(buffer);
    }
}

void FuzzTriggerResponseWithValidData(FuzzedDataProvider &provider)
{
    NormalGetMechBaseInfoCmd cmd;

    size_t nameLength = provider.ConsumeIntegralInRange<size_t>(0, 100);
    size_t totalSize = BIT_OFFSET_2 + RSP_SIZE + nameLength;

    auto buffer = std::make_shared<MechDataBuffer>(totalSize + 10);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());

    for (size_t i = 0; i < RSP_SIZE; ++i) {
        buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    }

    cmd.TriggerResponse(buffer);
}

void FuzzTriggerResponseWithCallback(FuzzedDataProvider &provider)
{
    NormalGetMechBaseInfoCmd cmd;

    bool shouldSetCallback = provider.ConsumeBool();
    if (shouldSetCallback) {
        cmd.SetResponseCallback([]() {
            return;
        });
    }

    size_t nameLength = provider.ConsumeIntegralInRange<size_t>(0, 50);
    size_t totalSize = BIT_OFFSET_2 + RSP_SIZE + nameLength;

    auto buffer = std::make_shared<MechDataBuffer>(totalSize + 10);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());

    for (size_t i = 0; i < RSP_SIZE; ++i) {
        buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    }

    cmd.TriggerResponse(buffer);
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0, 6);

    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_CONSTRUCTOR:
            FuzzConstructor(provider);
            break;
        case TestFunctionId::FUZZ_MARSHAL:
            FuzzMarshal(provider);
            break;
        case TestFunctionId::FUZZ_MARSHAL_MULTIPLE_TIMES:
            FuzzMarshalMultipleTimes(provider);
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
        case TestFunctionId::FUZZ_TRIGGER_RESPONSE_WITH_CALLBACK:
            FuzzTriggerResponseWithCallback(provider);
            break;
        default:
            break;
    }

    return 0;
}