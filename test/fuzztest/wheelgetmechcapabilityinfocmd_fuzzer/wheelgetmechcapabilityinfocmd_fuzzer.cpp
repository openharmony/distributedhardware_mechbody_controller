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
#include "wheelgetmechcapabilityinfocmd_fuzzer.h"
#include "mc_wheel_get_mech_capability_info_cmd.h"
#include "mc_data_buffer.h"

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

constexpr size_t BIT_OFFSET_2 = 2;
constexpr size_t RSP_SIZE = 20;

enum class TestFunctionId {
    FUZZ_CONSTRUCTOR = 0,
    FUZZ_MARSHAL = 1,
    FUZZ_TRIGGER_RESPONSE_WITH_NULL = 2,
    FUZZ_TRIGGER_RESPONSE_WITH_SMALL_BUFFER = 3,
    FUZZ_TRIGGER_RESPONSE_WITH_ZERO_LENGTH = 4,
    FUZZ_TRIGGER_RESPONSE_WITH_FULL_LENGTH = 5,
    FUZZ_TRIGGER_RESPONSE_WITH_RANDOM_DATA = 6,
    FUZZ_GET_PARAMS = 7,
    FUZZ_GET_RESULT = 8,
    FUZZ_FULL_WORKFLOW = 9
};

void FuzzConstructor(FuzzedDataProvider &provider)
{
    (void)provider;
    WheelGetMechCapabilityInfoCmd cmd;
}

void FuzzMarshal(FuzzedDataProvider &provider)
{
    (void)provider;
    WheelGetMechCapabilityInfoCmd cmd;
    auto buffer = cmd.Marshal();
}

void FuzzTriggerResponseWithNull(FuzzedDataProvider &provider)
{
    (void)provider;
    WheelGetMechCapabilityInfoCmd cmd;
    cmd.TriggerResponse(nullptr);
}

void FuzzTriggerResponseWithSmallBuffer(FuzzedDataProvider &provider)
{
    WheelGetMechCapabilityInfoCmd cmd;
    size_t bufferSize = provider.ConsumeIntegralInRange<size_t>(0, 10);
    auto buffer = std::make_shared<MechDataBuffer>(bufferSize);
    if (buffer != nullptr) {
        cmd.TriggerResponse(buffer);
    }
}

void FuzzTriggerResponseWithZeroLength(FuzzedDataProvider &provider)
{
    WheelGetMechCapabilityInfoCmd cmd;

    auto buffer = std::make_shared<MechDataBuffer>(RSP_SIZE + BIT_OFFSET_2);
    if (buffer == nullptr) {
        return;
    }

    for (size_t i = 0; i < RSP_SIZE + BIT_OFFSET_2; i++) {
        buffer->AppendUint8(0x00);
    }

    cmd.TriggerResponse(buffer);
}

void FuzzTriggerResponseWithFullLength(FuzzedDataProvider &provider)
{
    WheelGetMechCapabilityInfoCmd cmd;

    auto buffer = std::make_shared<MechDataBuffer>(RSP_SIZE + BIT_OFFSET_2);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x00);

    buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    buffer->AppendInt16(provider.ConsumeIntegral<int16_t>());
    buffer->AppendInt16(provider.ConsumeIntegral<int16_t>());
    buffer->AppendInt16(provider.ConsumeIntegral<int16_t>());
    buffer->AppendInt16(provider.ConsumeIntegral<int16_t>());
    buffer->AppendFloat(provider.ConsumeFloatingPoint<float>());
    buffer->AppendUint16(provider.ConsumeIntegral<uint16_t>());

    cmd.TriggerResponse(buffer);
}

void FuzzTriggerResponseWithRandomData(FuzzedDataProvider &provider)
{
    WheelGetMechCapabilityInfoCmd cmd;

    size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 100);
    auto buffer = std::make_shared<MechDataBuffer>(dataSize);
    if (buffer == nullptr) {
        return;
    }

    for (size_t i = 0; i < dataSize; i++) {
        buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    }

    cmd.TriggerResponse(buffer);
}

void FuzzGetParams(FuzzedDataProvider &provider)
{
    (void)provider;
    WheelGetMechCapabilityInfoCmd cmd;
    WheelCapabilityInfo params = cmd.GetParams();
    (void)params;
}

void FuzzGetResult(FuzzedDataProvider &provider)
{
    (void)provider;
    WheelGetMechCapabilityInfoCmd cmd;
    uint8_t result = cmd.GetResult();
    (void)result;
}

void FuzzFullWorkflow(FuzzedDataProvider &provider)
{
    WheelGetMechCapabilityInfoCmd cmd;

    auto marshalBuffer = cmd.Marshal();

    bool shouldTestNull = provider.ConsumeBool();
    if (shouldTestNull) {
        cmd.TriggerResponse(nullptr);
    } else {
        auto buffer = std::make_shared<MechDataBuffer>(RSP_SIZE + BIT_OFFSET_2);
        if (buffer != nullptr) {
            buffer->AppendUint8(0x03);
            buffer->AppendUint8(0x00);

            buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
            buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
            buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
            buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
            buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
            buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
            buffer->AppendInt16(provider.ConsumeIntegral<int16_t>());
            buffer->AppendInt16(provider.ConsumeIntegral<int16_t>());
            buffer->AppendInt16(provider.ConsumeIntegral<int16_t>());
            buffer->AppendInt16(provider.ConsumeIntegral<int16_t>());
            buffer->AppendFloat(provider.ConsumeFloatingPoint<float>());
            buffer->AppendUint16(provider.ConsumeIntegral<uint16_t>());

            cmd.TriggerResponse(buffer);

            WheelCapabilityInfo params = cmd.GetParams();
            uint8_t result = cmd.GetResult();
            (void)params;
            (void)result;
        }
    }
}

void RunFuzzTest(FuzzedDataProvider &provider)
{
    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0, 9);

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
        case TestFunctionId::FUZZ_TRIGGER_RESPONSE_WITH_ZERO_LENGTH:
            FuzzTriggerResponseWithZeroLength(provider);
            break;
        case TestFunctionId::FUZZ_TRIGGER_RESPONSE_WITH_FULL_LENGTH:
            FuzzTriggerResponseWithFullLength(provider);
            break;
        case TestFunctionId::FUZZ_TRIGGER_RESPONSE_WITH_RANDOM_DATA:
            FuzzTriggerResponseWithRandomData(provider);
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
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    RunFuzzTest(provider);
    return 0;
}