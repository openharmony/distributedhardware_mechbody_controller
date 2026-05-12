/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "mcgetmechcameratrackinglayoutcmd_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <memory>
#include <securec.h>

#include "mc_get_mech_camera_tracking_layout_cmd.h"
#include "mc_data_buffer.h"

constexpr uint8_t FUZZ_TEST_ZERO = 0;
constexpr uint8_t FUZZ_TEST_ONE = 1;
constexpr uint8_t FUZZ_TEST_TWO = 2;
constexpr uint8_t FUZZ_TEST_THREE = 3;
constexpr uint8_t FUZZ_TEST_FOUR = 4;
constexpr uint8_t FUZZ_TEST_FIVE = 5;
constexpr uint8_t FUZZ_TEST_SIX = 6;
constexpr uint8_t FUZZ_TEST_SEVEN = 7;
constexpr uint8_t FUZZ_TEST_EIGHT = 8;
constexpr uint8_t FUZZ_TEST_NINE = 9;
constexpr uint8_t FUZZ_TEST_TEN = 10;
constexpr uint8_t FUZZ_TEST_ELEVEN = 11;

namespace OHOS {
namespace MechBodyController {
static constexpr uint32_t OPT_SIZE = 80;
FuzzedDataProvider *FDP;

void GetMechCameraTrackingLayoutCmdConstructorFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechCameraTrackingLayoutCmd cmd;
    
    if (provider.ConsumeBool()) {
        uint8_t cmdSet = cmd.GetCmdSet();
        (void)cmdSet;
    }
    if (provider.ConsumeBool()) {
        uint8_t cmdId = cmd.GetCmdId();
        (void)cmdId;
    }
    if (provider.ConsumeBool()) {
        uint16_t reqSize = cmd.GetReqSize();
        (void)reqSize;
    }
    if (provider.ConsumeBool()) {
        uint16_t rspSize = cmd.GetRspSize();
        (void)rspSize;
    }
    if (provider.ConsumeBool()) {
        uint32_t timeoutMs = cmd.GetTimeoutMs();
        (void)timeoutMs;
    }
    if (provider.ConsumeBool()) {
        int32_t retryTimes = cmd.GetRetryTimes();
        (void)retryTimes;
    }
    if (provider.ConsumeBool()) {
        bool needResponse = cmd.NeedResponse();
        (void)needResponse;
    }
}

void GetMechCameraTrackingLayoutCmdMarshalFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechCameraTrackingLayoutCmd cmd;
    auto buffer = cmd.Marshal();
    if (buffer != nullptr && provider.ConsumeBool()) {
        size_t bufferSize = buffer->Size();
        (void)bufferSize;
    }
}

void GetMechCameraTrackingLayoutCmdTriggerResponseFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechCameraTrackingLayoutCmd cmd;
    
    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 50);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            dataBuffer->SetRange(0, dataSize);
            auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!bytes.empty() && dataBuffer->Data() != nullptr) {
                (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
            }
        }
    }
    
    cmd.TriggerResponse(dataBuffer);
}

void GetMechCameraTrackingLayoutCmdGetParamsFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechCameraTrackingLayoutCmd cmd;
    
    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 50);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            dataBuffer->SetRange(0, dataSize);
            auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!bytes.empty() && dataBuffer->Data() != nullptr) {
                (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
            }
        }
    }
    
    cmd.TriggerResponse(dataBuffer);
    
    const LayoutParams &params = cmd.GetParams();
    (void)params;
}

void GetMechCameraTrackingLayoutCmdGetResultFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechCameraTrackingLayoutCmd cmd;
    
    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 50);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            dataBuffer->SetRange(0, dataSize);
            auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!bytes.empty() && dataBuffer->Data() != nullptr) {
                (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
            }
        }
    }
    
    cmd.TriggerResponse(dataBuffer);
    
    uint8_t result = cmd.GetResult();
    (void)result;
}

void GetMechCameraTrackingLayoutCmdWithCallbackFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechCameraTrackingLayoutCmd cmd;
    
    bool shouldSetCallback = provider.ConsumeBool();
    if (shouldSetCallback) {
        cmd.SetResponseCallback([]() {
            // Callback function
        });
    }
    
    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 50);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            dataBuffer->SetRange(0, dataSize);
            auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!bytes.empty() && dataBuffer->Data() != nullptr) {
                (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
            }
        }
    }
    
    cmd.TriggerResponse(dataBuffer);
}

void GetMechCameraTrackingLayoutCmdValidDataFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechCameraTrackingLayoutCmd cmd;
    
    // Construct valid response data
    size_t dataSize = 14; // RSP_SIZE(12) + BIT_OFFSET_2(2)
    auto dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);
        
        // Fill with valid data
        uint8_t result = provider.ConsumeIntegral<uint8_t>();
        uint8_t resultType = 0x02; // CMD_GET_TRACKING_LAYOUT_TYPE
        uint8_t resultLength = 9; // CMD_GET_TRACKING_LAYOUT_REPLY_LENGTH
        uint8_t controlBit = provider.ConsumeIntegral<uint8_t>();
        float offsetX = provider.ConsumeFloatingPoint<float>();
        float offsetY = provider.ConsumeFloatingPoint<float>();

        dataBuffer->AppendUint8(result);
        dataBuffer->AppendUint8(resultType);
        dataBuffer->AppendUint8(resultLength);
        dataBuffer->AppendUint8(controlBit);
        dataBuffer->AppendFloat(offsetX);
        dataBuffer->AppendFloat(offsetY);
        
        cmd.TriggerResponse(dataBuffer);
        
        const LayoutParams &params = cmd.GetParams();
        uint8_t retResult = cmd.GetResult();
        
        (void)params;
        (void)retResult;
    }
}

void GetMechCameraTrackingLayoutCmdInvalidTypeFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechCameraTrackingLayoutCmd cmd;
    
    // Construct data with invalid resultType
    size_t dataSize = 14;
    auto dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);
        
        uint8_t result = provider.ConsumeIntegral<uint8_t>();
        uint8_t resultType = provider.ConsumeIntegral<uint8_t>(); // Random type (may be invalid)
        uint8_t resultLength = 9;
        uint8_t controlBit = provider.ConsumeIntegral<uint8_t>();
        float offsetX = provider.ConsumeFloatingPoint<float>();
        float offsetY = provider.ConsumeFloatingPoint<float>();
        
        dataBuffer->AppendUint8(result);
        dataBuffer->AppendUint8(resultType);
        dataBuffer->AppendUint8(resultLength);
        dataBuffer->AppendUint8(controlBit);
        dataBuffer->AppendFloat(offsetX);
        dataBuffer->AppendFloat(offsetY);
        
        cmd.TriggerResponse(dataBuffer);
    }
}

void GetMechCameraTrackingLayoutCmdInvalidLengthFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechCameraTrackingLayoutCmd cmd;
    
    // Construct data with invalid resultLength
    size_t dataSize = 14;
    auto dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);
        
        uint8_t result = provider.ConsumeIntegral<uint8_t>();
        uint8_t resultType = 0x02; // Valid type
        uint8_t resultLength = provider.ConsumeIntegral<uint8_t>(); // Random length (may be invalid)
        uint8_t controlBit = provider.ConsumeIntegral<uint8_t>();
        float offsetX = provider.ConsumeFloatingPoint<float>();
        float offsetY = provider.ConsumeFloatingPoint<float>();
        
        dataBuffer->AppendUint8(result);
        dataBuffer->AppendUint8(resultType);
        dataBuffer->AppendUint8(resultLength);
        dataBuffer->AppendUint8(controlBit);
        dataBuffer->AppendFloat(offsetX);
        dataBuffer->AppendFloat(offsetY);
        
        cmd.TriggerResponse(dataBuffer);
    }
}

void GetMechCameraTrackingLayoutCmdShortDataFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechCameraTrackingLayoutCmd cmd;
    
    // Construct data with short length
    size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 13); // Less than RSP_SIZE + BIT_OFFSET_2
    auto dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr && dataSize > 0) {
        dataBuffer->SetRange(0, dataSize);
        auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
        if (!bytes.empty() && dataBuffer->Data() != nullptr) {
            (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
        }
    }
    
    cmd.TriggerResponse(dataBuffer);
}

void GetMechCameraTrackingLayoutCmdFullWorkflowFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechCameraTrackingLayoutCmd cmd;
    
    auto buffer = cmd.Marshal();
    
    bool shouldSetCallback = provider.ConsumeBool();
    if (shouldSetCallback) {
        cmd.SetResponseCallback([]() {
            // Callback function
        });
    }
    
    // Construct valid response data
    size_t dataSize = 14;
    auto dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);
        
        uint8_t result = provider.ConsumeIntegral<uint8_t>();
        uint8_t resultType = 0x02;
        uint8_t resultLength = 9;
        uint8_t controlBit = provider.ConsumeIntegral<uint8_t>();
        float offsetX = provider.ConsumeFloatingPoint<float>();
        float offsetY = provider.ConsumeFloatingPoint<float>();
        
        dataBuffer->AppendUint8(result);
        dataBuffer->AppendUint8(resultType);
        dataBuffer->AppendUint8(resultLength);
        dataBuffer->AppendUint8(controlBit);
        dataBuffer->AppendFloat(offsetX);
        dataBuffer->AppendFloat(offsetY);
        
        cmd.TriggerResponse(dataBuffer);
        
        const LayoutParams &params = cmd.GetParams();
        uint8_t retResult = cmd.GetResult();
        
        (void)params;
        (void)retResult;
    }
}

} // namespace MechBodyController
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < OHOS::MechBodyController::OPT_SIZE) {
        return 0;
    }
    FuzzedDataProvider fdp(data + size - OHOS::MechBodyController::OPT_SIZE, OHOS::MechBodyController::OPT_SIZE);
    uint8_t selector = fdp.ConsumeIntegral<uint8_t>() % FUZZ_TEST_ELEVEN;
    size_t testSize = size - OHOS::MechBodyController::OPT_SIZE;

    using namespace OHOS::MechBodyController;
    switch (selector) {
        case FUZZ_TEST_ZERO: GetMechCameraTrackingLayoutCmdConstructorFuzzTest(data, testSize); break;
        case FUZZ_TEST_ONE: GetMechCameraTrackingLayoutCmdMarshalFuzzTest(data, testSize); break;
        case FUZZ_TEST_TWO: GetMechCameraTrackingLayoutCmdTriggerResponseFuzzTest(data, testSize); break;
        case FUZZ_TEST_THREE: GetMechCameraTrackingLayoutCmdGetParamsFuzzTest(data, testSize); break;
        case FUZZ_TEST_FOUR: GetMechCameraTrackingLayoutCmdGetResultFuzzTest(data, testSize); break;
        case FUZZ_TEST_FIVE: GetMechCameraTrackingLayoutCmdWithCallbackFuzzTest(data, testSize); break;
        case FUZZ_TEST_SIX: GetMechCameraTrackingLayoutCmdValidDataFuzzTest(data, testSize); break;
        case FUZZ_TEST_SEVEN: GetMechCameraTrackingLayoutCmdInvalidTypeFuzzTest(data, testSize); break;
        case FUZZ_TEST_EIGHT: GetMechCameraTrackingLayoutCmdInvalidLengthFuzzTest(data, testSize); break;
        case FUZZ_TEST_NINE: GetMechCameraTrackingLayoutCmdShortDataFuzzTest(data, testSize); break;
        case FUZZ_TEST_TEN: GetMechCameraTrackingLayoutCmdFullWorkflowFuzzTest(data, testSize); break;
        default: break;
    }
    return 0;
}