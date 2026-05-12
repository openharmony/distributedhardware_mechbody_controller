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

#include "normalgetmechcapabilityinfocmd_fuzzer.h"
#include "mc_normal_get_mech_capability_info_cmd.h"
#include "mc_data_buffer.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include <string>

namespace OHOS {
using namespace OHOS::MechBodyController;

constexpr uint8_t FUZZ_CONSTRUCTOR = 0;
constexpr uint8_t FUZZ_MARSHAL = 1;
constexpr uint8_t FUZZ_MARSHAL_MULTIPLE_TIMES = 2;
constexpr uint8_t FUZZ_TRIGGER_RESPONSE_WITH_NULL = 3;
constexpr uint8_t FUZZ_TRIGGER_RESPONSE_WITH_SMALL_BUFFER = 4;
constexpr uint8_t FUZZ_TRIGGER_RESPONSE_WITH_VALID_DATA = 5;
constexpr uint8_t FUZZ_TEST_COUNT = 6;
constexpr size_t RSP_SIZE = 21;
constexpr size_t BIT_OFFSET_2 = 2;
constexpr size_t MIN_BUFFER_SIZE = 0;
constexpr size_t MAX_SMALL_BUFFER_SIZE = 10;
constexpr size_t EXTRA_BUFFER_SIZE = 10;
constexpr size_t MIN_INDEX = 0;
constexpr size_t MIN_INPUT_SIZE = 0;
constexpr uint8_t CMD_SET_VALUE = 0x02;
constexpr uint8_t CMD_ID_VALUE = 0x00;

void FuzzConstructor(FuzzedDataProvider &provider)
{
    (void)provider;
    NormalGetMechCapabilityInfoCmd cmd;
}

void FuzzMarshal(FuzzedDataProvider &provider)
{
    (void)provider;
    NormalGetMechCapabilityInfoCmd cmd;
    auto buffer = cmd.Marshal();
}

void FuzzMarshalMultipleTimes(FuzzedDataProvider &provider)
{
    (void)provider;
    NormalGetMechCapabilityInfoCmd cmd;
    auto buffer1 = cmd.Marshal();
    auto buffer2 = cmd.Marshal();
}

void FuzzTriggerResponseWithNull(FuzzedDataProvider &provider)
{
    (void)provider;
    NormalGetMechCapabilityInfoCmd cmd;
    cmd.TriggerResponse(nullptr);
}

void FuzzTriggerResponseWithSmallBuffer(FuzzedDataProvider &provider)
{
    NormalGetMechCapabilityInfoCmd cmd;
    size_t bufferSize = provider.ConsumeIntegralInRange<size_t>(MIN_BUFFER_SIZE, MAX_SMALL_BUFFER_SIZE);
    auto buffer = std::make_shared<MechDataBuffer>(bufferSize);
    if (buffer != nullptr) {
        cmd.TriggerResponse(buffer);
    }
}

void FuzzTriggerResponseWithValidData(FuzzedDataProvider &provider)
{
    NormalGetMechCapabilityInfoCmd cmd;

    size_t totalSize = RSP_SIZE + OHOS::BIT_OFFSET_2;

    auto buffer = std::make_shared<MechDataBuffer>(totalSize + EXTRA_BUFFER_SIZE);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(CMD_SET_VALUE);
    buffer->AppendUint8(CMD_ID_VALUE);

    for (size_t i = MIN_INDEX; i < RSP_SIZE; ++i) {
        buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());
    }

    cmd.TriggerResponse(buffer);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == MIN_INPUT_SIZE)) {
        return 0;
    }

    FuzzedDataProvider fdp(data, size);
    uint8_t selector = fdp.ConsumeIntegral<uint8_t>() % FUZZ_TEST_COUNT;

    switch (selector) {
        case FUZZ_CONSTRUCTOR:
            FuzzConstructor(fdp);
            break;
        case FUZZ_MARSHAL:
            FuzzMarshal(fdp);
            break;
        case FUZZ_MARSHAL_MULTIPLE_TIMES:
            FuzzMarshalMultipleTimes(fdp);
            break;
        case FUZZ_TRIGGER_RESPONSE_WITH_NULL:
            FuzzTriggerResponseWithNull(fdp);
            break;
        case FUZZ_TRIGGER_RESPONSE_WITH_SMALL_BUFFER:
            FuzzTriggerResponseWithSmallBuffer(fdp);
            break;
        case FUZZ_TRIGGER_RESPONSE_WITH_VALID_DATA:
            FuzzTriggerResponseWithValidData(fdp);
            break;
        default:
            break;
    }

    return 0;
}

} // namespace OHOS