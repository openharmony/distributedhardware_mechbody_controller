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

    enum class TestFunctionId {
        FUZZ_VALIDATE = 0,
        FUZZ_CONVERT = 1,
        FUZZ_GET_DATA = 2,
        FUZZ_DATA_BUFFER_OPERATIONS = 3
    };
}

static void TestValidate(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    const uint8_t *dataPtr = nullptr;
    uint32_t dataLen = provider.ConsumeIntegralInRange<uint32_t>(0, MAX_DATA_SIZE);

    bool shouldBeNull = provider.ConsumeBool();
    if (!shouldBeNull && dataLen > 0) {
        std::vector<uint8_t> testData = provider.ConsumeBytes<uint8_t>(dataLen);
        if (!testData.empty()) {
            dataPtr = testData.data();
        }
    }

    ProtocolConverter protocolConverter;
    protocolConverter.Validate(dataPtr, dataLen);
}

static void TestConvert(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    OptType optType = static_cast<OptType>(provider.ConsumeIntegralInRange<uint8_t>(0, 3));
    uint16_t seqNo = provider.ConsumeIntegral<uint16_t>();

    bool shouldBeNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer = nullptr;

    if (!shouldBeNull) {
        uint32_t dataSize = provider.ConsumeIntegralInRange<uint32_t>(0, MAX_DATA_SIZE);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            std::vector<uint8_t> testData = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!testData.empty()) {
                dataBuffer->SetRange(0, dataSize);
                if (memcpy_s(dataBuffer->Data(), dataBuffer->Size(), testData.data(), dataSize) != 0) {
                    return;
                }
            }
        }
    }

    std::string serviceName = provider.ConsumeRandomLengthString();

    ProtocolConverter protocolConverter;
    protocolConverter.Convert(optType, seqNo, dataBuffer, serviceName);
}

static void TestGetData(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    std::string serviceName = provider.ConsumeRandomLengthString();
    if (serviceName.length() > 255) {
        serviceName = serviceName.substr(0, 255);
    }

    bool shouldBeNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> pclData = nullptr;

    if (!shouldBeNull) {
        uint32_t dataSize = provider.ConsumeIntegralInRange<uint32_t>(0, MAX_DATA_SIZE);
        pclData = std::make_shared<MechDataBuffer>(dataSize);
        if (pclData != nullptr && dataSize > 0) {
            std::vector<uint8_t> testData = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!testData.empty()) {
                pclData->SetRange(0, dataSize);
                if (memcpy_s(pclData->Data(), pclData->Size(), testData.data(), dataSize) != 0) {
                    return;
                }
            }
        }
    }

    uint16_t seqNo = 0;
    bool isAck = false;

    ProtocolConverter protocolConverter;
    protocolConverter.GetData(pclData, seqNo, isAck);
}

static void TestDataBufferOperations(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    uint32_t bufferSize = provider.ConsumeIntegralInRange<uint32_t>(0, MAX_DATA_SIZE);
    std::shared_ptr<MechDataBuffer> mechDataBuffer = std::make_shared<MechDataBuffer>(bufferSize);

    if (mechDataBuffer == nullptr) {
        return;
    }

    uint32_t rangeOffset = provider.ConsumeIntegralInRange<uint32_t>(0, bufferSize);
    uint32_t rangeSize = provider.ConsumeIntegralInRange<uint32_t>(0, bufferSize - rangeOffset);
    mechDataBuffer->SetRange(rangeOffset, rangeSize);

    mechDataBuffer->Size();
    mechDataBuffer->Offset();
    mechDataBuffer->Capacity();
    mechDataBuffer->Data();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < 1) {
        return 0;
    }

    FuzzedDataProvider provider(data, size);
    uint8_t testType = provider.ConsumeIntegral<uint8_t>() % 4;

    switch (static_cast<TestFunctionId>(testType)) {
        case TestFunctionId::FUZZ_VALIDATE:
            TestValidate(data, size);
            break;
        case TestFunctionId::FUZZ_CONVERT:
            TestConvert(data, size);
            break;
        case TestFunctionId::FUZZ_GET_DATA:
            TestGetData(data, size);
            break;
        case TestFunctionId::FUZZ_DATA_BUFFER_OPERATIONS:
            TestDataBufferOperations(data, size);
            break;
        default:
            break;
    }

    return 0;
}