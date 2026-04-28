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
#include "wheelsetmechscenecontrolcmd_fuzzer.h"
#include "mc_wheel_set_mech_scene_control_cmd.h"
#include "mc_data_buffer.h"

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

constexpr size_t BIT_OFFSET_2 = 2;
constexpr size_t RSP_SIZE = 1;

enum class TestFunctionId {
    FUZZ_CONSTRUCTOR_LANDSCAPE_PORTRAIT_SWITCH = 0,
    FUZZ_CONSTRUCTOR_PATROL_MODE = 1,
    FUZZ_CONSTRUCTOR_GREET_MODE = 2,
    FUZZ_CONSTRUCTOR_HEAD_UP = 3,
    FUZZ_CONSTRUCTOR_HAPPY = 4,
    FUZZ_CONSTRUCTOR_DANCE = 5,
    FUZZ_CONSTRUCTOR_RANDOM = 6,
    FUZZ_MARSHAL = 7,
    FUZZ_TRIGGER_RESPONSE_WITH_NULL = 8,
    FUZZ_TRIGGER_RESPONSE_WITH_SMALL_BUFFER = 9,
    FUZZ_TRIGGER_RESPONSE_WITH_VALID_DATA = 10,
    FUZZ_GET_ACTION_TYPE = 11,
    FUZZ_GET_RESULT = 12,
    FUZZ_FULL_WORKFLOW = 13
};

void FuzzConstructorLandscapePortraitSwitch(FuzzedDataProvider &provider)
{
    (void)provider;
    WheelSetMechSceneControlCmd cmd(ActionType::LANDSCAPE_PORTRAIT_SWITCH);
}

void FuzzConstructorPatrolMode(FuzzedDataProvider &provider)
{
    (void)provider;
    WheelSetMechSceneControlCmd cmd(ActionType::PATROL_MODE);
}

void FuzzConstructorGreetMode(FuzzedDataProvider &provider)
{
    (void)provider;
    WheelSetMechSceneControlCmd cmd(ActionType::GREET_MODE);
}

void FuzzConstructorHeadUp(FuzzedDataProvider &provider)
{
    (void)provider;
    WheelSetMechSceneControlCmd cmd(ActionType::HEAD_UP);
}

void FuzzConstructorHappy(FuzzedDataProvider &provider)
{
    (void)provider;
    WheelSetMechSceneControlCmd cmd(ActionType::HAPPY);
}

void FuzzConstructorDance(FuzzedDataProvider &provider)
{
    (void)provider;
    WheelSetMechSceneControlCmd cmd(ActionType::DANCE);
}

void FuzzConstructorRandom(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(-10, 3000);
    WheelSetMechSceneControlCmd cmd(static_cast<ActionType>(actionValue));
}

void FuzzMarshal(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(0, 3000);
    WheelSetMechSceneControlCmd cmd(static_cast<ActionType>(actionValue));
    auto buffer = cmd.Marshal();
}

void FuzzTriggerResponseWithNull(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(0, 3000);
    WheelSetMechSceneControlCmd cmd(static_cast<ActionType>(actionValue));
    cmd.TriggerResponse(nullptr);
}

void FuzzTriggerResponseWithSmallBuffer(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(0, 3000);
    WheelSetMechSceneControlCmd cmd(static_cast<ActionType>(actionValue));
    size_t bufferSize = provider.ConsumeIntegralInRange<size_t>(0, 5);
    auto buffer = std::make_shared<MechDataBuffer>(bufferSize);
    if (buffer != nullptr) {
        cmd.TriggerResponse(buffer);
    }
}

void FuzzTriggerResponseWithValidData(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(0, 3000);
    WheelSetMechSceneControlCmd cmd(static_cast<ActionType>(actionValue));

    auto buffer = std::make_shared<MechDataBuffer>(RSP_SIZE + BIT_OFFSET_2);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x24);
    buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());

    cmd.TriggerResponse(buffer);
}

void FuzzGetActionType(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(0, 3000);
    WheelSetMechSceneControlCmd cmd(static_cast<ActionType>(actionValue));
    ActionType actionType = cmd.GetActionType();
    (void)actionType;
}

void FuzzGetResult(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(0, 3000);
    WheelSetMechSceneControlCmd cmd(static_cast<ActionType>(actionValue));
    uint8_t result = cmd.GetResult();
    (void)result;
}

void FuzzFullWorkflow(FuzzedDataProvider &provider)
{
    int32_t actionValue = provider.ConsumeIntegralInRange<int32_t>(0, 3000);
    WheelSetMechSceneControlCmd cmd(static_cast<ActionType>(actionValue));

    auto marshalBuffer = cmd.Marshal();

    bool shouldTestNull = provider.ConsumeBool();
    if (shouldTestNull) {
        cmd.TriggerResponse(nullptr);
    } else {
        auto buffer = std::make_shared<MechDataBuffer>(RSP_SIZE + BIT_OFFSET_2);
        if (buffer != nullptr) {
            buffer->AppendUint8(0x03);
            buffer->AppendUint8(0x24);
            buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());

            cmd.TriggerResponse(buffer);

            ActionType actionType = cmd.GetActionType();
            uint8_t result = cmd.GetResult();
            (void)actionType;
            (void)result;
        }
    }
}

void RunFuzzTest(FuzzedDataProvider &provider)
{
    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0, 13);

    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_CONSTRUCTOR_LANDSCAPE_PORTRAIT_SWITCH:
            FuzzConstructorLandscapePortraitSwitch(provider);
            break;
        case TestFunctionId::FUZZ_CONSTRUCTOR_PATROL_MODE:
            FuzzConstructorPatrolMode(provider);
            break;
        case TestFunctionId::FUZZ_CONSTRUCTOR_GREET_MODE:
            FuzzConstructorGreetMode(provider);
            break;
        case TestFunctionId::FUZZ_CONSTRUCTOR_HEAD_UP:
            FuzzConstructorHeadUp(provider);
            break;
        case TestFunctionId::FUZZ_CONSTRUCTOR_HAPPY:
            FuzzConstructorHappy(provider);
            break;
        case TestFunctionId::FUZZ_CONSTRUCTOR_DANCE:
            FuzzConstructorDance(provider);
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
        case TestFunctionId::FUZZ_GET_ACTION_TYPE:
            FuzzGetActionType(provider);
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