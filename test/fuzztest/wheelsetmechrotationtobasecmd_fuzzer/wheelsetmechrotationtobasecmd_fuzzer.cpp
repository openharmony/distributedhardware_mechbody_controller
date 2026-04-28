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
#include <cfloat>
#include "wheelsetmechrotationtobasecmd_fuzzer.h"
#include "mc_wheel_set_mech_rotation_to_base_cmd.h"
#include "mc_data_buffer.h"

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

constexpr size_t BIT_OFFSET_2 = 2;
constexpr size_t RSP_SIZE = 3;

enum class TestFunctionId {
    FUZZ_CONSTRUCTOR_DEFAULT = 0,
    FUZZ_CONSTRUCTOR_RANDOM = 1,
    FUZZ_CONSTRUCTOR_BOUNDARY = 2,
    FUZZ_MARSHAL = 3,
    FUZZ_TRIGGER_RESPONSE_WITH_NULL = 4,
    FUZZ_TRIGGER_RESPONSE_WITH_SMALL_BUFFER = 5,
    FUZZ_TRIGGER_RESPONSE_WITH_VALID_DATA = 6,
    FUZZ_GET_PARAMS = 7,
    FUZZ_GET_RESULT = 8,
    FUZZ_FULL_WORKFLOW = 9
};

void FuzzConstructorDefault(FuzzedDataProvider &provider)
{
    (void)provider;
    RotateToBaseParam params(0, 0, 0, 0.0f);
    WheelSetMechRotationToBaseCmd cmd(params);
}

void FuzzConstructorRandom(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    uint16_t rotateTime = provider.ConsumeIntegral<uint16_t>();
    int16_t forwardSpeed = provider.ConsumeIntegral<int16_t>();
    float turningSpeed = provider.ConsumeFloatingPoint<float>();
    RotateToBaseParam params(taskId, rotateTime, forwardSpeed, turningSpeed);
    WheelSetMechRotationToBaseCmd cmd(params);
}

void FuzzConstructorBoundary(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeBool() ? 0 : 0xFFFF;
    uint16_t rotateTime = provider.ConsumeBool() ? 0 : 0xFFFF;
    int16_t forwardSpeed = provider.ConsumeBool() ? INT16_MIN : INT16_MAX;
    float turningSpeed = provider.ConsumeBool() ? -FLT_MAX : FLT_MAX;
    RotateToBaseParam params(taskId, rotateTime, forwardSpeed, turningSpeed);
    WheelSetMechRotationToBaseCmd cmd(params);
}

void FuzzMarshal(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    uint16_t rotateTime = provider.ConsumeIntegral<uint16_t>();
    int16_t forwardSpeed = provider.ConsumeIntegral<int16_t>();
    float turningSpeed = provider.ConsumeFloatingPoint<float>();
    RotateToBaseParam params(taskId, rotateTime, forwardSpeed, turningSpeed);
    WheelSetMechRotationToBaseCmd cmd(params);
    auto buffer = cmd.Marshal();
}

void FuzzTriggerResponseWithNull(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    RotateToBaseParam params(taskId, 0, 0, 0.0f);
    WheelSetMechRotationToBaseCmd cmd(params);
    cmd.TriggerResponse(nullptr);
}

void FuzzTriggerResponseWithSmallBuffer(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    RotateToBaseParam params(taskId, 0, 0, 0.0f);
    WheelSetMechRotationToBaseCmd cmd(params);
    size_t bufferSize = provider.ConsumeIntegralInRange<size_t>(0, 5);
    auto buffer = std::make_shared<MechDataBuffer>(bufferSize);
    if (buffer != nullptr) {
        cmd.TriggerResponse(buffer);
    }
}

void FuzzTriggerResponseWithValidData(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    RotateToBaseParam params(taskId, 0, 0, 0.0f);
    WheelSetMechRotationToBaseCmd cmd(params);

    auto buffer = std::make_shared<MechDataBuffer>(RSP_SIZE + BIT_OFFSET_2);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x20);
    buffer->AppendUint16(provider.ConsumeIntegral<uint16_t>());
    buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());

    cmd.TriggerResponse(buffer);
}

void FuzzGetParams(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    uint16_t rotateTime = provider.ConsumeIntegral<uint16_t>();
    int16_t forwardSpeed = provider.ConsumeIntegral<int16_t>();
    float turningSpeed = provider.ConsumeFloatingPoint<float>();
    RotateToBaseParam params(taskId, rotateTime, forwardSpeed, turningSpeed);
    WheelSetMechRotationToBaseCmd cmd(params);
    const RotateToBaseParam& resultParams = cmd.GetParams();
    (void)resultParams;
}

void FuzzGetResult(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    RotateToBaseParam params(taskId, 0, 0, 0.0f);
    WheelSetMechRotationToBaseCmd cmd(params);
    uint8_t result = cmd.GetResult();
    (void)result;
}

void FuzzFullWorkflow(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    uint16_t rotateTime = provider.ConsumeIntegral<uint16_t>();
    int16_t forwardSpeed = provider.ConsumeIntegral<int16_t>();
    float turningSpeed = provider.ConsumeFloatingPoint<float>();
    RotateToBaseParam params(taskId, rotateTime, forwardSpeed, turningSpeed);
    WheelSetMechRotationToBaseCmd cmd(params);

    auto marshalBuffer = cmd.Marshal();

    bool shouldTestNull = provider.ConsumeBool();
    if (shouldTestNull) {
        cmd.TriggerResponse(nullptr);
    } else {
        auto buffer = std::make_shared<MechDataBuffer>(RSP_SIZE + BIT_OFFSET_2);
        if (buffer != nullptr) {
            buffer->AppendUint8(0x03);
            buffer->AppendUint8(0x20);
            buffer->AppendUint16(provider.ConsumeIntegral<uint16_t>());
            buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());

            cmd.TriggerResponse(buffer);

            const RotateToBaseParam& resultParams = cmd.GetParams();
            uint8_t result = cmd.GetResult();
            (void)resultParams;
            (void)result;
        }
    }
}

void RunFuzzTest(FuzzedDataProvider &provider)
{
    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0, 9);

    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_CONSTRUCTOR_DEFAULT:
            FuzzConstructorDefault(provider);
            break;
        case TestFunctionId::FUZZ_CONSTRUCTOR_RANDOM:
            FuzzConstructorRandom(provider);
            break;
        case TestFunctionId::FUZZ_CONSTRUCTOR_BOUNDARY:
            FuzzConstructorBoundary(provider);
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
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    RunFuzzTest(provider);
    return 0;
}