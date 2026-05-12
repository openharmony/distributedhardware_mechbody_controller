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

#include "mcprotocolconvertor_fuzzer.h"
#include "mc_protocol_convertor.h"
#include "mc_data_buffer.h"
#include "mechbody_controller_log.h"
#include "mechbody_controller_types.h"

using namespace OHOS::MechBodyController;

namespace {
    const std::string TAG = "McProtocolConvertorFuzzTest";
    const uint32_t MAX_DATA_SIZE = 512;
    const size_t MAX_SERVICE_NAME_LENGTH = 255;
    const uint8_t TEST_FUNCTION_COUNT = 4;
    const uint32_t ZERO_VALUE = 0;
    const uint8_t MAX_OPT_TYPE_VALUE = 3;
    const size_t MIN_INPUT_SIZE = 1;

    enum class TestFunctionId {
        FUZZ_VALIDATE = 0,
        FUZZ_CONVERT = 1,
        FUZZ_GET_DATA = 2,
        FUZZ_DATA_BUFFER_OPERATIONS = 3
    };
}

static void TestValidate(FuzzedDataProvider &provider)
{
    const uint8_t *dataPtr = nullptr;
    uint32_t dataLen = provider.ConsumeIntegralInRange<uint32_t>(ZERO_VALUE, MAX_DATA_SIZE);

    bool shouldBeNull = provider.ConsumeBool();
    if (!shouldBeNull && dataLen > ZERO_VALUE) {
        std::vector<uint8_t> testData = provider.ConsumeBytes<uint8_t>(dataLen);
        if (!testData.empty()) {
            dataPtr = testData.data();
        }
    }

    ProtocolConverter protocolConverter;
    protocolConverter.Validate(dataPtr, dataLen);
}

static std::shared_ptr<MechDataBuffer> CreateDataBufferWithData(FuzzedDataProvider &provider)
{
    uint32_t dataSize = provider.ConsumeIntegralInRange<uint32_t>(ZERO_VALUE, MAX_DATA_SIZE);
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);

    if (dataBuffer == nullptr || dataSize == ZERO_VALUE) {
        return dataBuffer;
    }

    std::vector<uint8_t> testData = provider.ConsumeBytes<uint8_t>(dataSize);
    if (testData.empty()) {
        return dataBuffer;
    }

    dataBuffer->SetRange(ZERO_VALUE, dataSize);
    if (memcpy_s(dataBuffer->Data(), dataBuffer->Size(), testData.data(), dataSize) != 0) {
        return nullptr;
    }

    return dataBuffer;
}

static void TestConvert(FuzzedDataProvider &provider)
{
    OptType optType = static_cast<OptType>(provider.ConsumeIntegralInRange<uint8_t>(ZERO_VALUE, MAX_OPT_TYPE_VALUE));
    uint16_t seqNo = provider.ConsumeIntegral<uint16_t>();

    std::shared_ptr<MechDataBuffer> dataBuffer = nullptr;
    if (!provider.ConsumeBool()) {
        dataBuffer = CreateDataBufferWithData(provider);
    }

    std::string serviceName = provider.ConsumeRandomLengthString();

    ProtocolConverter protocolConverter;
    protocolConverter.Convert(optType, seqNo, dataBuffer, serviceName);
}

static void TestGetData(FuzzedDataProvider &provider)
{
    std::string serviceName = provider.ConsumeRandomLengthString();
    if (serviceName.length() > MAX_SERVICE_NAME_LENGTH) {
        serviceName = serviceName.substr(ZERO_VALUE, MAX_SERVICE_NAME_LENGTH);
    }

    std::shared_ptr<MechDataBuffer> pclData = nullptr;
    if (!provider.ConsumeBool()) {
        pclData = CreateDataBufferWithData(provider);
    }

    uint16_t seqNo = ZERO_VALUE;
    bool isAck = false;

    ProtocolConverter protocolConverter;
    protocolConverter.GetData(pclData, seqNo, isAck);
}

static void TestDataBufferOperations(FuzzedDataProvider &provider)
{
    uint32_t bufferSize = provider.ConsumeIntegralInRange<uint32_t>(ZERO_VALUE, MAX_DATA_SIZE);
    std::shared_ptr<MechDataBuffer> mechDataBuffer = std::make_shared<MechDataBuffer>(bufferSize);

    if (mechDataBuffer == nullptr) {
        return;
    }

    uint32_t rangeOffset = provider.ConsumeIntegralInRange<uint32_t>(ZERO_VALUE, bufferSize);
    uint32_t rangeSize = provider.ConsumeIntegralInRange<uint32_t>(ZERO_VALUE, bufferSize - rangeOffset);
    mechDataBuffer->SetRange(rangeOffset, rangeSize);

    mechDataBuffer->Size();
    mechDataBuffer->Offset();
    mechDataBuffer->Capacity();
    mechDataBuffer->Data();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < MIN_INPUT_SIZE) {
        return 0;
    }

    FuzzedDataProvider provider(data, size);
    uint8_t testType = provider.ConsumeIntegral<uint8_t>() % TEST_FUNCTION_COUNT;

    switch (static_cast<TestFunctionId>(testType)) {
        case TestFunctionId::FUZZ_VALIDATE:
            TestValidate(provider);
            break;
        case TestFunctionId::FUZZ_CONVERT:
            TestConvert(provider);
            break;
        case TestFunctionId::FUZZ_GET_DATA:
            TestGetData(provider);
            break;
        case TestFunctionId::FUZZ_DATA_BUFFER_OPERATIONS:
            TestDataBufferOperations(provider);
            break;
        default:
            break;
    }

    return 0;
}