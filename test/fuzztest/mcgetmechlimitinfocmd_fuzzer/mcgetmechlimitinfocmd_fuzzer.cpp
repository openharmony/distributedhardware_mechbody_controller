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

#include "mcgetmechlimitinfocmd_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <memory>
#include <securec.h>

#include "mc_get_mech_limit_info_cmd.h"
#include "mc_data_buffer.h"

namespace OHOS {
namespace MechBodyController {

void GetMechLimitInfoCmdMarshalFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechLimitInfoCmd cmd;
    auto buffer = cmd.Marshal();
    if (buffer != nullptr) {
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
}

void GetMechLimitInfoCmdTriggerResponseFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechLimitInfoCmd cmd;
    
    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 100);
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
    
    const RotateDegreeLimit& params = cmd.GetParams();
    uint8_t result = cmd.GetResult();
    
    (void)params;
    (void)result;
}

void GetMechLimitInfoCmdGetParamsFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechLimitInfoCmd cmd;
    
    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 100);
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
    
    const RotateDegreeLimit& params = cmd.GetParams();
    (void)params;
}

void GetMechLimitInfoCmdGetResultFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
GetMechLimitInfoCmd cmd;

    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 100);
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
    const RotateDegreeLimit& params = cmd.GetParams();
    uint8_t result = cmd.GetResult();
    (void)params;
    (void)result;
}

void GetMechLimitInfoCmdWithCallbackFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechLimitInfoCmd cmd;
    
    bool shouldSetCallback = provider.ConsumeBool();
    if (shouldSetCallback) {
        cmd.SetResponseCallback([]() {
            // Callback function
        });
    }
    
    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 100);
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
    
    const RotateDegreeLimit& params = cmd.GetParams();
    uint8_t result = cmd.GetResult();
    
    (void)params;
    (void)result;
}

void GetMechLimitInfoCmdWithValidResponseFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechLimitInfoCmd cmd;
    
    size_t dataSize = 29; // RSP_SIZE + BIT_OFFSET_2 = 27 + 2
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);
        
        // Construct valid response data
        uint8_t result = provider.ConsumeIntegral<uint8_t>();
        uint8_t resultType = 0x03; // CMD_GET_MECH_LIMIT_TYPE
        uint8_t resultLength = 24; // CMD_GET_MECH_LIMIT_REPLY_LENGTH
        float rollMax = provider.ConsumeFloatingPoint<float>();
        float rollMin = provider.ConsumeFloatingPoint<float>();
        float pitchMax = provider.ConsumeFloatingPoint<float>();
        float pitchMin = provider.ConsumeFloatingPoint<float>();
        float yawMax = provider.ConsumeFloatingPoint<float>();
        float yawMin = provider.ConsumeFloatingPoint<float>();
        
        size_t offset = 0;
        if (dataBuffer->Data() != nullptr) {
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &result, sizeof(result));
            offset += sizeof(result);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &resultType, sizeof(resultType));
            offset += sizeof(resultType);
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &resultLength, sizeof(resultLength));
            offset += sizeof(resultLength);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &rollMax, sizeof(rollMax));
            offset += sizeof(rollMax);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &rollMin, sizeof(rollMin));
            offset += sizeof(rollMin);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &pitchMax, sizeof(pitchMax));
            offset += sizeof(pitchMax);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &pitchMin, sizeof(pitchMin));
            offset += sizeof(pitchMin);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &yawMax, sizeof(yawMax));
            offset += sizeof(yawMax);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &yawMin, sizeof(yawMin));
        }
    }
    
    cmd.TriggerResponse(dataBuffer);
    
    const RotateDegreeLimit& params = cmd.GetParams();
    uint8_t result = cmd.GetResult();
    
    (void)params;
    (void)result;
}

} // namespace MechBodyController
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::MechBodyController::GetMechLimitInfoCmdMarshalFuzzTest(data, size);
    OHOS::MechBodyController::GetMechLimitInfoCmdTriggerResponseFuzzTest(data, size);
    OHOS::MechBodyController::GetMechLimitInfoCmdGetParamsFuzzTest(data, size);
    OHOS::MechBodyController::GetMechLimitInfoCmdGetResultFuzzTest(data, size);
    OHOS::MechBodyController::GetMechLimitInfoCmdWithCallbackFuzzTest(data, size);
    OHOS::MechBodyController::GetMechLimitInfoCmdWithValidResponseFuzzTest(data, size);
    return 0;
}