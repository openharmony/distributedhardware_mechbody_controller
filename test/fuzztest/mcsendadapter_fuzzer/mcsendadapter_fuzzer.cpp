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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>
#include <securec.h>
#include <cstdint>

#include "mcsendadapter_fuzzer.h"
#include "mc_send_adapter.h"
#include "mc_data_buffer.h"
#include "mc_protocol_convertor.h"
#include "mechbody_controller_log.h"

using namespace OHOS::MechBodyController;

namespace {
    const std::string TAG = "McSendAdapterFuzzTest";
    const uint32_t MAX_DATA_SIZE = 4096;

    enum class TestFunctionId {
        FUZZ_BLE_RECEIVE_LISTENER = 0,
        FUZZ_DATA_BUFFER_OPERATIONS = 1,
        FUZZ_PROTOCOL_CONVERTER = 2
    };
}

class MockTransportSendAdapter : public std::enable_shared_from_this<MockTransportSendAdapter> {
public:
    MockTransportSendAdapter() = default;
    ~MockTransportSendAdapter() = default;

    int32_t MockOnReceive(bool isAck, uint16_t seqNo, std::shared_ptr<MechDataBuffer> dataBuffer)
    {
        if (dataBuffer == nullptr) {
            return INVALID_PARAMETERS_ERR;
        }
        return OHOS::ERR_OK;
    }
};

class MockBleReceviceListener : public BleReceviceListener {
public:
    MockBleReceviceListener(std::shared_ptr<MockTransportSendAdapter> adapter)
        : adapter_(adapter) {}
    ~MockBleReceviceListener() = default;

    int32_t OnReceive(const uint8_t *data, uint32_t dataLen) override
    {
        if (data == nullptr && dataLen > 0) {
            return INVALID_PARAMETERS_ERR;
        }

        ProtocolConverter protocolConverter;
        if (!protocolConverter.Validate(data, dataLen)) {
            return INVALID_PARAMETERS_ERR;
        }

        std::shared_ptr<MechDataBuffer> mechDataBuffer = std::make_shared<MechDataBuffer>(dataLen);
        if (mechDataBuffer == nullptr) {
            return INVALID_PARAMETERS_ERR;
        }

        mechDataBuffer->SetRange(0, dataLen);
        if (dataLen > 0 && memcpy_s(mechDataBuffer->Data(), mechDataBuffer->Size(), data, dataLen) != 0) {
            return INVALID_PARAMETERS_ERR;
        }

        uint16_t seqNo = 0;
        bool isAck = false;
        std::shared_ptr<MechDataBuffer> newMechDataBuffer = protocolConverter.GetData(mechDataBuffer, seqNo, isAck);

        if (adapter_ != nullptr) {
            return adapter_->MockOnReceive(isAck, seqNo, newMechDataBuffer);
        }
        return OHOS::ERR_OK;
    }

private:
    std::shared_ptr<MockTransportSendAdapter> adapter_;
};

static void TestBleReceiveListener(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    auto adapter = std::make_shared<MockTransportSendAdapter>();
    MockBleReceviceListener listener(adapter);

    uint32_t dataLen = provider.ConsumeIntegralInRange<uint32_t>(0, MAX_DATA_SIZE);
    std::vector<uint8_t> testData = provider.ConsumeBytes<uint8_t>(dataLen);

    const uint8_t *dataPtr = testData.empty() ? nullptr : testData.data();
    listener.OnReceive(dataPtr, dataLen);
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

static void TestProtocolConverter(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    uint32_t dataLen = provider.ConsumeIntegralInRange<uint32_t>(0, MAX_DATA_SIZE);
    std::vector<uint8_t> testData = provider.ConsumeBytes<uint8_t>(dataLen);

    const uint8_t *dataPtr = testData.empty() ? nullptr : testData.data();

    ProtocolConverter protocolConverter;
    protocolConverter.Validate(dataPtr, dataLen);

    if (dataLen > 0 && dataPtr != nullptr) {
        std::shared_ptr<MechDataBuffer> mechDataBuffer = std::make_shared<MechDataBuffer>(dataLen);
        if (mechDataBuffer != nullptr) {
            mechDataBuffer->SetRange(0, dataLen);
            if (memcpy_s(mechDataBuffer->Data(), mechDataBuffer->Size(), dataPtr, dataLen) == 0) {
                uint16_t seqNo = 0;
                bool isAck = false;
                protocolConverter.GetData(mechDataBuffer, seqNo, isAck);
            }
        }
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < 1) {
        return 0;
    }

    FuzzedDataProvider provider(data, size);
    uint8_t testType = provider.ConsumeIntegral<uint8_t>() % 3;

    switch (static_cast<TestFunctionId>(testType)) {
        case TestFunctionId::FUZZ_BLE_RECEIVE_LISTENER:
            TestBleReceiveListener(data, size);
            break;
        case TestFunctionId::FUZZ_DATA_BUFFER_OPERATIONS:
            TestDataBufferOperations(data, size);
            break;
        case TestFunctionId::FUZZ_PROTOCOL_CONVERTER:
            TestProtocolConverter(data, size);
            break;
        default:
            break;
    }

    return 0;
}