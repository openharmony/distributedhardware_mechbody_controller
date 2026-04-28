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
#include "normalsetmechprotocolvercmd_fuzzer.h"
#include "mc_normal_set_mech_protocol_ver_cmd.h"
#include "mc_data_buffer.h"

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

enum class TestFunctionId {
    FUZZ_CONSTRUCTOR = 0,
    FUZZ_MARSHAL = 1,
    FUZZ_TRIGGER_RESPONSE_WITH_NULL = 2,
    FUZZ_TRIGGER_RESPONSE_WITH_DATA = 3,
    FUZZ_FULL_WORKFLOW = 4
};

void FuzzConstructor(FuzzedDataProvider &provider)
{
    uint8_t protocolVer = provider.ConsumeIntegral<uint8_t>();
    NormalSetMechProtocolVerCmd cmd(protocolVer);
}

void FuzzMarshal(FuzzedDataProvider &provider)
{
    uint8_t protocolVer = provider.ConsumeIntegral<uint8_t>();
    NormalSetMechProtocolVerCmd cmd(protocolVer);
    auto buffer = cmd.Marshal();
}

void FuzzTriggerResponseWithNull(FuzzedDataProvider &provider)
{
    uint8_t protocolVer = provider.ConsumeIntegral<uint8_t>();
    NormalSetMechProtocolVerCmd cmd(protocolVer);
    cmd.TriggerResponse(nullptr);
}

void FuzzTriggerResponseWithData(FuzzedDataProvider &provider)
{
    uint8_t protocolVer = provider.ConsumeIntegral<uint8_t>();
    NormalSetMechProtocolVerCmd cmd(protocolVer);

    size_t bufferSize = provider.ConsumeIntegralInRange<size_t>(0, 100);
    auto buffer = std::make_shared<MechDataBuffer>(bufferSize);
    if (buffer != nullptr) {
        cmd.TriggerResponse(buffer);
    }
}

void FuzzFullWorkflow(FuzzedDataProvider &provider)
{
    uint8_t protocolVer = provider.ConsumeIntegral<uint8_t>();
    NormalSetMechProtocolVerCmd cmd(protocolVer);

    auto marshalBuffer = cmd.Marshal();

    bool shouldTestNull = provider.ConsumeBool();
    if (shouldTestNull) {
        cmd.TriggerResponse(nullptr);
    } else {
        size_t bufferSize = provider.ConsumeIntegralInRange<size_t>(0, 100);
        auto buffer = std::make_shared<MechDataBuffer>(bufferSize);
        if (buffer != nullptr) {
            cmd.TriggerResponse(buffer);
        }
    }
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0, 4);

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
        case TestFunctionId::FUZZ_TRIGGER_RESPONSE_WITH_DATA:
            FuzzTriggerResponseWithData(provider);
            break;
        case TestFunctionId::FUZZ_FULL_WORKFLOW:
            FuzzFullWorkflow(provider);
            break;
        default:
            break;
    }

    return 0;
}