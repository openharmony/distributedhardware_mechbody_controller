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
#include "wheelsetmechmotioncontrolcmd_fuzzer.h"
#include "mc_wheel_set_mech_motion_control_cmd.h"
#include "mc_data_buffer.h"

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

constexpr size_t BIT_OFFSET_2 = 2;
constexpr size_t RSP_SIZE = 1;

enum class TestFunctionId {
    FUZZ_CONSTRUCTOR_STOP = 0,
    FUZZ_CONSTRUCTOR_GO_CENTER = 1,
    FUZZ_CONSTRUCTOR_HORIZONTALLY_SWITCH = 2,
    FUZZ_CONSTRUCTOR_UNKNOWN = 3,
    FUZZ_CONSTRUCTOR_RANDOM = 4,
    FUZZ_MARSHAL = 5,
    FUZZ_TRIGGER_RESPONSE_WITH_NULL = 6,
    FUZZ_TRIGGER_RESPONSE_WITH_SMALL_BUFFER = 7,
    FUZZ_TRIGGER_RESPONSE_WITH_VALID_DATA = 8,
    FUZZ_GET_ACTION = 9,
    FUZZ_GET_RESULT = 10,
    FUZZ_FULL_WORKFLOW = 11
};

void FuzzConstructorStop(FuzzedDataProvider &provider)
{
    (void)provider;
    WheelSetMechMotionControlCmd cmd(ControlCommand::STOP);
}

void FuzzConstructorGoCenter(FuzzedDataProvider &provider)
{
    (void)provider;
    WheelSetMechMotionControlCmd cmd(ControlCommand::GO_CENTER);
}

void FuzzConstructorHorizontallySwitch(FuzzedDataProvider &provider)
{
    (void)provider;
    WheelSetMechMotionControlCmd cmd(ControlCommand::HORIZONTALLY_SWITCH);
}

void FuzzConstructorUnknown(FuzzedDataProvider &provider)
{
    (void)provider;
    WheelSetMechMotionControlCmd cmd(ControlCommand::UNKNOWN);
}

void FuzzConstructorRandom(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(-10, 10);
    WheelSetMechMotionControlCmd cmd(static_cast<ControlCommand>(actionValue));
}

void FuzzMarshal(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(-1, 3);
    WheelSetMechMotionControlCmd cmd(static_cast<ControlCommand>(actionValue));
    auto buffer = cmd.Marshal();
}

void FuzzTriggerResponseWithNull(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(-1, 3);
    WheelSetMechMotionControlCmd cmd(static_cast<ControlCommand>(actionValue));
    cmd.TriggerResponse(nullptr);
}

void FuzzTriggerResponseWithSmallBuffer(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(-1, 3);
    WheelSetMechMotionControlCmd cmd(static_cast<ControlCommand>(actionValue));
    size_t bufferSize = provider.ConsumeIntegralInRange<size_t>(0, 5);
    auto buffer = std::make_shared<MechDataBuffer>(bufferSize);
    if (buffer != nullptr) {
        cmd.TriggerResponse(buffer);
    }
}

void FuzzTriggerResponseWithValidData(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(-1, 3);
    WheelSetMechMotionControlCmd cmd(static_cast<ControlCommand>(actionValue));

    auto buffer = std::make_shared<MechDataBuffer>(RSP_SIZE + BIT_OFFSET_2);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x23);
    buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());

    cmd.TriggerResponse(buffer);
}

void FuzzGetAction(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(-1, 3);
    WheelSetMechMotionControlCmd cmd(static_cast<ControlCommand>(actionValue));
    ControlCommand action = cmd.GetAction();
    (void)action;
}

void FuzzGetResult(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(-1, 3);
    WheelSetMechMotionControlCmd cmd(static_cast<ControlCommand>(actionValue));
    uint8_t result = cmd.GetResult();
    (void)result;
}

void FuzzFullWorkflow(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(-1, 3);
    WheelSetMechMotionControlCmd cmd(static_cast<ControlCommand>(actionValue));

    auto marshalBuffer = cmd.Marshal();

    bool shouldTestNull = provider.ConsumeBool();
    if (shouldTestNull) {
        cmd.TriggerResponse(nullptr);
    } else {
        auto buffer = std::make_shared<MechDataBuffer>(RSP_SIZE + BIT_OFFSET_2);
        if (buffer != nullptr) {
            buffer->AppendUint8(0x03);
            buffer->AppendUint8(0x23);
            buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());

            cmd.TriggerResponse(buffer);

            ControlCommand action = cmd.GetAction();
            uint8_t result = cmd.GetResult();
            (void)action;
            (void)result;
        }
    }
}

void RunFuzzTest(FuzzedDataProvider &provider)
{
    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0, 11);

    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_CONSTRUCTOR_STOP:
            FuzzConstructorStop(provider);
            break;
        case TestFunctionId::FUZZ_CONSTRUCTOR_GO_CENTER:
            FuzzConstructorGoCenter(provider);
            break;
        case TestFunctionId::FUZZ_CONSTRUCTOR_HORIZONTALLY_SWITCH:
            FuzzConstructorHorizontallySwitch(provider);
            break;
        case TestFunctionId::FUZZ_CONSTRUCTOR_UNKNOWN:
            FuzzConstructorUnknown(provider);
            break;
        case TestFunctionId::FUZZ_CONSTRUCTOR_RANDOM:
            FuzzConstructorRandom(provider);
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
        case TestFunctionId::FUZZ_GET_ACTION:
            FuzzGetAction(provider);
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