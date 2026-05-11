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
#include <memory>
#include <securec.h>
#include <cstdint>

#include "mcdatabuffer_fuzzer.h"
#include "mc_data_buffer.h"
#include "mechbody_controller_log.h"

using namespace OHOS::MechBodyController;

namespace {
    const std::string TAG = "McDataBufferFuzzTest";
    const uint32_t MAX_DATA_SIZE = 4096;

    enum class TestFunctionId {
        FUZZ_CONSTRUCTOR_AND_BASIC_OPS = 0,
        FUZZ_APPEND_OPERATIONS = 1,
        FUZZ_READ_OPERATIONS = 2,
        FUZZ_BOUNDARY_CONDITIONS = 3,
        FUZZ_LARGE_CAPACITY = 4
    };

    constexpr uint8_t TEST_FUNCTION_COUNT = 5;
    constexpr size_t MIN_INPUT_SIZE = 1;
    constexpr uint32_t MIN_BUFFER_SIZE = 1;
    constexpr uint32_t MIN_STRING_LENGTH = 1;
    constexpr uint32_t MIN_OFFSET = 0;
}

static void TestConstructorAndBasicOps(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    uint32_t capacity = provider.ConsumeIntegralInRange<uint32_t>(MIN_OFFSET, MAX_DATA_SIZE);
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(capacity);

    if (dataBuffer == nullptr) {
        return;
    }

    dataBuffer->Size();
    dataBuffer->Offset();
    dataBuffer->Capacity();
    dataBuffer->Data();

    uint32_t rangeOffset = provider.ConsumeIntegralInRange<uint32_t>(MIN_OFFSET, capacity);
    uint32_t rangeSize = provider.ConsumeIntegralInRange<uint32_t>(MIN_OFFSET, capacity - rangeOffset);
    dataBuffer->SetRange(rangeOffset, rangeSize);
}

static void TestAppendOperations(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    uint32_t capacity = provider.ConsumeIntegralInRange<uint32_t>(MIN_OFFSET, MAX_DATA_SIZE);
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(capacity);

    if (dataBuffer == nullptr) {
        return;
    }

    uint8_t uint8Val = provider.ConsumeIntegral<uint8_t>();
    uint16_t uint16Val = provider.ConsumeIntegral<uint16_t>();
    uint32_t uint32Val = provider.ConsumeIntegral<uint32_t>();
    uint64_t uint64Val = provider.ConsumeIntegral<uint64_t>();
    int16_t int16Val = provider.ConsumeIntegral<int16_t>();
    float floatVal = provider.ConsumeFloatingPoint<float>();

    dataBuffer->AppendUint8(uint8Val);
    dataBuffer->AppendUint16(uint16Val);
    dataBuffer->AppendUint32(uint32Val);
    dataBuffer->AppendUint64(uint64Val);
    dataBuffer->AppendInt16(int16Val);
    dataBuffer->AppendFloat(floatVal);

    bool shouldAppendBuffer = provider.ConsumeBool();
    if (shouldAppendBuffer) {
        uint32_t bufferCapacity = provider.ConsumeIntegralInRange<uint32_t>(MIN_OFFSET, MAX_DATA_SIZE);
        std::shared_ptr<MechDataBuffer> appendBuffer = std::make_shared<MechDataBuffer>(bufferCapacity);
        if (appendBuffer != nullptr && bufferCapacity > 0) {
            appendBuffer->SetRange(MIN_OFFSET, bufferCapacity);
            dataBuffer->AppendDataBuffer(appendBuffer);
        }
    }
}

static void TestReadOperations(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    uint32_t capacity = provider.ConsumeIntegralInRange<uint32_t>(MIN_OFFSET, MAX_DATA_SIZE);
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(capacity);

    if (dataBuffer == nullptr) {
        return;
    }

    if (capacity == MIN_OFFSET) {
        return;
    }

    uint32_t rangeOffset = provider.ConsumeIntegralInRange<uint32_t>(MIN_OFFSET, capacity);
    uint32_t rangeSize = provider.ConsumeIntegralInRange<uint32_t>(MIN_OFFSET, capacity - rangeOffset);
    dataBuffer->SetRange(rangeOffset, rangeSize);

    if (rangeSize == MIN_OFFSET) {
        return;
    }

    uint8_t uint8Val = 0;
    uint16_t uint16Val = 0;
    uint32_t uint32Val = 0;
    uint64_t uint64Val = 0;
    int16_t int16Val = 0;
    float floatVal = 0.0f;

    uint32_t readOffset = provider.ConsumeIntegralInRange<uint32_t>(MIN_OFFSET, rangeSize - MIN_BUFFER_SIZE);

    dataBuffer->ReadUint8(readOffset, uint8Val);
    dataBuffer->ReadUint16(readOffset, uint16Val);
    dataBuffer->ReadUint32(readOffset, uint32Val);
    dataBuffer->ReadUint64(readOffset, uint64Val);
    dataBuffer->ReadInt16(readOffset, int16Val);
    dataBuffer->ReadFloat(readOffset, floatVal);

    bool shouldReadString = provider.ConsumeBool();
    if (shouldReadString) {
        uint32_t strLen = provider.ConsumeIntegralInRange<uint32_t>(MIN_STRING_LENGTH, rangeSize - readOffset);
        char *strBuffer = new char[strLen];
        if (strBuffer != nullptr) {
            dataBuffer->ReadString(readOffset, strBuffer, strLen);
            delete[] strBuffer;
        }
    }
}

static void TestBoundaryConditions(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    constexpr uint32_t zeroCapacity = 0;
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(zeroCapacity);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(zeroCapacity, zeroCapacity);
        dataBuffer->AppendUint8(zeroCapacity);
    }

    uint32_t bufferSize = provider.ConsumeIntegralInRange<uint32_t>(MIN_BUFFER_SIZE, MAX_DATA_SIZE);
    dataBuffer = std::make_shared<MechDataBuffer>(bufferSize);
    if (dataBuffer != nullptr) {
    uint32_t rangeOffset = provider.ConsumeIntegralInRange<uint32_t>(MIN_OFFSET, bufferSize - MIN_BUFFER_SIZE);
    uint32_t rangeSize = provider.ConsumeIntegralInRange<uint32_t>(MIN_BUFFER_SIZE, bufferSize - rangeOffset);
    dataBuffer->SetRange(rangeOffset, rangeSize);

        uint8_t uint8Val = provider.ConsumeIntegral<uint8_t>();
        uint16_t uint16Val = provider.ConsumeIntegral<uint16_t>();
        uint32_t uint32Val = provider.ConsumeIntegral<uint32_t>();
        uint64_t uint64Val = provider.ConsumeIntegral<uint64_t>();
        int16_t int16Val = provider.ConsumeIntegral<int16_t>();
        float floatVal = provider.ConsumeFloatingPoint<float>();

        dataBuffer->AppendUint8(uint8Val);
        dataBuffer->AppendUint16(uint16Val);
        dataBuffer->AppendUint32(uint32Val);
        dataBuffer->AppendUint64(uint64Val);
        dataBuffer->AppendInt16(int16Val);
        dataBuffer->AppendFloat(floatVal);

        uint8_t val8 = 0;
        uint16_t val16 = 0;
        uint32_t val32 = 0;
        uint64_t val64 = 0;
int16_t valI16 = 0;
    float valF = 0.0f;

uint32_t readOffset = provider.ConsumeIntegralInRange<uint32_t>(MIN_OFFSET, rangeSize - MIN_BUFFER_SIZE);

    dataBuffer->ReadUint8(readOffset, val8);
        dataBuffer->ReadUint16(readOffset, val16);
        dataBuffer->ReadUint32(readOffset, val32);
        dataBuffer->ReadUint64(readOffset, val64);
        dataBuffer->ReadInt16(readOffset, valI16);
        dataBuffer->ReadFloat(readOffset, valF);
    }
}

static void TestLargeCapacity(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    uint32_t capacity = provider.ConsumeIntegralInRange<uint32_t>(MAX_DATA_SIZE, MAX_DATA_SIZE * 2);
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(capacity);

    if (dataBuffer == nullptr) {
        return;
    }

    dataBuffer->Size();
    dataBuffer->Offset();
    dataBuffer->Capacity();
    dataBuffer->Data();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < MIN_INPUT_SIZE) {
        return 0;
    }

    FuzzedDataProvider provider(data, size);
    uint8_t testType = provider.ConsumeIntegral<uint8_t>() % TEST_FUNCTION_COUNT;

    switch (static_cast<TestFunctionId>(testType)) {
        case TestFunctionId::FUZZ_CONSTRUCTOR_AND_BASIC_OPS:
            TestConstructorAndBasicOps(data, size);
            break;
        case TestFunctionId::FUZZ_APPEND_OPERATIONS:
            TestAppendOperations(data, size);
            break;
        case TestFunctionId::FUZZ_READ_OPERATIONS:
            TestReadOperations(data, size);
            break;
        case TestFunctionId::FUZZ_BOUNDARY_CONDITIONS:
            TestBoundaryConditions(data, size);
            break;
        case TestFunctionId::FUZZ_LARGE_CAPACITY:
            TestLargeCapacity(data, size);
            break;
        default:
            break;
    }

    return 0;
}