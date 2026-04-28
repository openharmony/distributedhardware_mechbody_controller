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
#include "mcsetmechdevicepairinginfocmd_fuzzer.h"
#include "mc_set_mech_device_pairing_info_cmd.h"
#include "mc_data_buffer.h"

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

constexpr size_t RSP_SIZE = 1;
constexpr size_t BIT_OFFSET_2 = 2;

enum class TestFunctionId {
    FUZZ_CONSTRUCTOR = 0,
    FUZZ_MARSHAL = 1,
    FUZZ_TRIGGER_RESPONSE_WITH_NULL = 2,
    FUZZ_TRIGGER_RESPONSE_WITH_SMALL_BUFFER = 3,
    FUZZ_TRIGGER_RESPONSE_WITH_VALID_DATA = 4,
    FUZZ_GET_RESULT = 5,
    FUZZ_FULL_WORKFLOW = 6
};

void FuzzConstructor(FuzzedDataProvider &provider)
{
    uint32_t deviceIdentifier = provider.ConsumeIntegral<uint32_t>();
    SetMechDevicePairingInfoCmd cmd(deviceIdentifier);
}

void FuzzMarshal(FuzzedDataProvider &provider)
{
    uint32_t deviceIdentifier = provider.ConsumeIntegral<uint32_t>();
    SetMechDevicePairingInfoCmd cmd(deviceIdentifier);
    auto buffer = cmd.Marshal();
}

void FuzzTriggerResponseWithNull(FuzzedDataProvider &provider)
{
    uint32_t deviceIdentifier = provider.ConsumeIntegral<uint32_t>();
    SetMechDevicePairingInfoCmd cmd(deviceIdentifier);
    cmd.TriggerResponse(nullptr);
}

void FuzzTriggerResponseWithSmallBuffer(FuzzedDataProvider &provider)
{
    uint32_t deviceIdentifier = provider.ConsumeIntegral<uint32_t>();
    SetMechDevicePairingInfoCmd cmd(deviceIdentifier);
    size_t bufferSize = provider.ConsumeIntegralInRange<size_t>(0, 10);
    auto buffer = std::make_shared<MechDataBuffer>(bufferSize);
    if (buffer != nullptr) {
        cmd.TriggerResponse(buffer);
    }
}

void FuzzTriggerResponseWithValidData(FuzzedDataProvider &provider)
{
    uint32_t deviceIdentifier = provider.ConsumeIntegral<uint32_t>();
    SetMechDevicePairingInfoCmd cmd(deviceIdentifier);

    size_t totalSize = RSP_SIZE + BIT_OFFSET_2;

    auto buffer = std::make_shared<MechDataBuffer>(totalSize + 10);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(0x01);
    buffer->AppendUint8(0x05);

    for (size_t i = 0; i < RSP_SIZE; ++i) {
        buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    }

    cmd.TriggerResponse(buffer);
}

void FuzzGetResult(FuzzedDataProvider &provider)
{
    uint32_t deviceIdentifier = provider.ConsumeIntegral<uint32_t>();
    SetMechDevicePairingInfoCmd cmd(deviceIdentifier);
    uint8_t result = cmd.GetResult();
    (void)result;
}

void FuzzFullWorkflow(FuzzedDataProvider &provider)
{
    uint32_t deviceIdentifier = provider.ConsumeIntegral<uint32_t>();
    SetMechDevicePairingInfoCmd cmd(deviceIdentifier);

    auto marshalBuffer = cmd.Marshal();

    bool shouldTestNull = provider.ConsumeBool();
    if (shouldTestNull) {
        cmd.TriggerResponse(nullptr);
    } else {
        size_t totalSize = RSP_SIZE + BIT_OFFSET_2;

        auto buffer = std::make_shared<MechDataBuffer>(totalSize + 10);
        if (buffer != nullptr) {
            buffer->AppendUint8(0x01);
            buffer->AppendUint8(0x05);

            for (size_t i = 0; i < RSP_SIZE; ++i) {
                buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
            }

            cmd.TriggerResponse(buffer);
            uint8_t result = cmd.GetResult();
            (void)result;
        }
    }
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
        case TestFunctionId::FUZZ_TRIGGER_RESPONSE_WITH_NULL:
            FuzzTriggerResponseWithNull(provider);
            break;
        case TestFunctionId::FUZZ_TRIGGER_RESPONSE_WITH_SMALL_BUFFER:
            FuzzTriggerResponseWithSmallBuffer(provider);
            break;
        case TestFunctionId::FUZZ_TRIGGER_RESPONSE_WITH_VALID_DATA:
            FuzzTriggerResponseWithValidData(provider);
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