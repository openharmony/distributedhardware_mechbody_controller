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
#include "wheelsetmechrelativepositioncmd_fuzzer.h"
#include "mc_wheel_set_mech_relative_position_cmd.h"
#include "mc_data_buffer.h"

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

constexpr size_t BIT_OFFSET_2 = 2;
constexpr size_t RSP_SIZE = 3;
constexpr size_t CMD_MAX_POINT_SIZE = 10;

enum class TestFunctionId {
    FUZZ_CONSTRUCTOR_EMPTY = 0,
    FUZZ_CONSTRUCTOR_SINGLE_POINT = 1,
    FUZZ_CONSTRUCTOR_MULTIPLE_POINTS = 2,
    FUZZ_CONSTRUCTOR_MAX_POINTS = 3,
    FUZZ_CONSTRUCTOR_EXCEED_MAX = 4,
    FUZZ_MARSHAL = 5,
    FUZZ_TRIGGER_RESPONSE_WITH_NULL = 6,
    FUZZ_TRIGGER_RESPONSE_WITH_SMALL_BUFFER = 7,
    FUZZ_TRIGGER_RESPONSE_WITH_VALID_DATA = 8,
    FUZZ_GET_PARAMS = 9,
    FUZZ_GET_TASK_ID = 10,
    FUZZ_GET_ERR_CODE = 11,
    FUZZ_FULL_WORKFLOW = 12
};

void FuzzConstructorEmpty(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    std::vector<WheelPositionInfo> params;
    WheelSetMechRelativePositionCmd cmd(taskId, params);
}

void FuzzConstructorSinglePoint(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    std::vector<WheelPositionInfo> params;
    WheelPositionInfo point;
    point.xCoordinates = provider.ConsumeIntegral<int16_t>();
    point.yCoordinates = provider.ConsumeIntegral<int16_t>();
    params.push_back(point);
    WheelSetMechRelativePositionCmd cmd(taskId, params);
}

void FuzzConstructorMultiplePoints(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    size_t pointNum = provider.ConsumeIntegralInRange<size_t>(2, 5);
    std::vector<WheelPositionInfo> params;
    for (size_t i = 0; i < pointNum; i++) {
        WheelPositionInfo point;
        point.xCoordinates = provider.ConsumeIntegral<int16_t>();
        point.yCoordinates = provider.ConsumeIntegral<int16_t>();
        params.push_back(point);
    }
    WheelSetMechRelativePositionCmd cmd(taskId, params);
}

void FuzzConstructorMaxPoints(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    std::vector<WheelPositionInfo> params;
    for (size_t i = 0; i < CMD_MAX_POINT_SIZE; i++) {
        WheelPositionInfo point;
        point.xCoordinates = provider.ConsumeIntegral<int16_t>();
        point.yCoordinates = provider.ConsumeIntegral<int16_t>();
        params.push_back(point);
    }
    WheelSetMechRelativePositionCmd cmd(taskId, params);
}

void FuzzConstructorExceedMax(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    std::vector<WheelPositionInfo> params;
    size_t pointNum = provider.ConsumeIntegralInRange<size_t>(11, 20);
    for (size_t i = 0; i < pointNum; i++) {
        WheelPositionInfo point;
        point.xCoordinates = provider.ConsumeIntegral<int16_t>();
        point.yCoordinates = provider.ConsumeIntegral<int16_t>();
        params.push_back(point);
    }
    WheelSetMechRelativePositionCmd cmd(taskId, params);
}

void FuzzMarshal(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    size_t pointNum = provider.ConsumeIntegralInRange<size_t>(0, 10);
    std::vector<WheelPositionInfo> params;
    for (size_t i = 0; i < pointNum; i++) {
        WheelPositionInfo point;
        point.xCoordinates = provider.ConsumeIntegral<int16_t>();
        point.yCoordinates = provider.ConsumeIntegral<int16_t>();
        params.push_back(point);
    }
    WheelSetMechRelativePositionCmd cmd(taskId, params);
    auto buffer = cmd.Marshal();
}

void FuzzTriggerResponseWithNull(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    std::vector<WheelPositionInfo> params;
    WheelSetMechRelativePositionCmd cmd(taskId, params);
    cmd.TriggerResponse(nullptr);
}

void FuzzTriggerResponseWithSmallBuffer(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    std::vector<WheelPositionInfo> params;
    WheelSetMechRelativePositionCmd cmd(taskId, params);
    size_t bufferSize = provider.ConsumeIntegralInRange<size_t>(0, 5);
    auto buffer = std::make_shared<MechDataBuffer>(bufferSize);
    if (buffer != nullptr) {
        cmd.TriggerResponse(buffer);
    }
}

void FuzzTriggerResponseWithValidData(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    std::vector<WheelPositionInfo> params;
    WheelSetMechRelativePositionCmd cmd(taskId, params);

    auto buffer = std::make_shared<MechDataBuffer>(RSP_SIZE + BIT_OFFSET_2);
    if (buffer == nullptr) {
        return;
    }

    buffer->AppendUint8(0x03);
    buffer->AppendUint8(0x22);
    buffer->AppendUint16(provider.ConsumeIntegral<uint16_t>());
    buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());

    cmd.TriggerResponse(buffer);
}

void FuzzGetParams(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    size_t pointNum = provider.ConsumeIntegralInRange<size_t>(0, 5);
    std::vector<WheelPositionInfo> params;
    for (size_t i = 0; i < pointNum; i++) {
        WheelPositionInfo point;
        point.xCoordinates = provider.ConsumeIntegral<int16_t>();
        point.yCoordinates = provider.ConsumeIntegral<int16_t>();
        params.push_back(point);
    }
    WheelSetMechRelativePositionCmd cmd(taskId, params);
    const std::vector<WheelPositionInfo>& resultParams = cmd.GetParams();
    (void)resultParams;
}

void FuzzGetTaskId(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    std::vector<WheelPositionInfo> params;
    WheelSetMechRelativePositionCmd cmd(taskId, params);
    uint16_t resultTaskId = cmd.GetTaskId();
    (void)resultTaskId;
}

void FuzzGetErrCode(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    std::vector<WheelPositionInfo> params;
    WheelSetMechRelativePositionCmd cmd(taskId, params);
    uint8_t errCode = cmd.GetErrCode();
    (void)errCode;
}

void FuzzFullWorkflow(FuzzedDataProvider &provider)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    size_t pointNum = provider.ConsumeIntegralInRange<size_t>(0, 5);
    std::vector<WheelPositionInfo> params;
    for (size_t i = 0; i < pointNum; i++) {
        WheelPositionInfo point;
        point.xCoordinates = provider.ConsumeIntegral<int16_t>();
        point.yCoordinates = provider.ConsumeIntegral<int16_t>();
        params.push_back(point);
    }
    WheelSetMechRelativePositionCmd cmd(taskId, params);

    auto marshalBuffer = cmd.Marshal();

    bool shouldTestNull = provider.ConsumeBool();
    if (shouldTestNull) {
        cmd.TriggerResponse(nullptr);
    } else {
        auto buffer = std::make_shared<MechDataBuffer>(RSP_SIZE + BIT_OFFSET_2);
        if (buffer != nullptr) {
            buffer->AppendUint8(0x03);
            buffer->AppendUint8(0x22);
            buffer->AppendUint16(provider.ConsumeIntegral<uint16_t>());
            buffer->AppendUint8(provider.ConsumeIntegral<uint8_t>());

            cmd.TriggerResponse(buffer);

            const std::vector<WheelPositionInfo>& resultParams = cmd.GetParams();
            uint16_t resultTaskId = cmd.GetTaskId();
            uint8_t errCode = cmd.GetErrCode();
            (void)resultParams;
            (void)resultTaskId;
            (void)errCode;
        }
    }
}

void RunFuzzTest(FuzzedDataProvider &provider)
{
    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0, 12);

    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_CONSTRUCTOR_EMPTY:
            FuzzConstructorEmpty(provider);
            break;
        case TestFunctionId::FUZZ_CONSTRUCTOR_SINGLE_POINT:
            FuzzConstructorSinglePoint(provider);
            break;
        case TestFunctionId::FUZZ_CONSTRUCTOR_MULTIPLE_POINTS:
            FuzzConstructorMultiplePoints(provider);
            break;
        case TestFunctionId::FUZZ_CONSTRUCTOR_MAX_POINTS:
            FuzzConstructorMaxPoints(provider);
            break;
        case TestFunctionId::FUZZ_CONSTRUCTOR_EXCEED_MAX:
            FuzzConstructorExceedMax(provider);
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
        case TestFunctionId::FUZZ_GET_TASK_ID:
            FuzzGetTaskId(provider);
            break;
        case TestFunctionId::FUZZ_GET_ERR_CODE:
            FuzzGetErrCode(provider);
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