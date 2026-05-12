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

#include "mcgetmechrealnamecmd_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <memory>
#include <securec.h>

#include "mc_get_mech_real_name_cmd.h"
#include "mc_data_buffer.h"

namespace OHOS {
namespace MechBodyController {

void GetMechRealNameCmdMarshalFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
       
    GetMechRealNameCmd cmd;
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

void GetMechRealNameCmdTriggerResponseFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechRealNameCmd cmd;
    
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
    
    std::string params = cmd.GetParams();
    uint8_t result = cmd.GetResult();
    
    (void)params;
    (void)result;
}

void GetMechRealNameCmdGetParamsFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechRealNameCmd cmd;
    
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
    
    std::string params = cmd.GetParams();
    (void)params;
}

void GetMechRealNameCmdGetResultFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechRealNameCmd cmd;
    
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
    
    uint8_t result = cmd.GetResult();
    (void)result;
}

void GetMechRealNameCmdWithCallbackFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechRealNameCmd cmd;
    
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
    
    std::string params = cmd.GetParams();
    uint8_t result = cmd.GetResult();
    
    (void)params;
    (void)result;
}

void GetMechRealNameCmdWithValidResponseFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechRealNameCmd cmd;
    
    // Construct valid response data with variable length name
    std::string name = provider.ConsumeRandomLengthString(20);
    size_t nameLength = name.length();
    size_t dataSize = nameLength + 3; // result + resultType + resultLength + name
    
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);
        
        uint8_t result = provider.ConsumeIntegral<uint8_t>();
        uint8_t resultType = 0x06; // CMD_GET_MECH_NAME_TYPE
        uint8_t resultLength = static_cast<uint8_t>(nameLength);
        
        size_t offset = 0;
        if (dataBuffer->Data() != nullptr) {
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &result, sizeof(result));
            offset += sizeof(result);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &resultType, sizeof(resultType));
            offset += sizeof(resultType);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &resultLength, sizeof(resultLength));
            offset += sizeof(resultLength);
            if (nameLength > 0) {
                (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, name.c_str(), nameLength);
            }
        }
    }
    
    cmd.TriggerResponse(dataBuffer);
    
    std::string params = cmd.GetParams();
    uint8_t result = cmd.GetResult();
    
    (void)params;
    (void)result;
}

void GetMechRealNameCmdWithEmptyNameFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    GetMechRealNameCmd cmd;
    
    size_t dataSize = 3; // result + resultType + resultLength
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);
        
        uint8_t result = provider.ConsumeIntegral<uint8_t>();
        uint8_t resultType = 0x06; // CMD_GET_MECH_NAME_TYPE
        uint8_t resultLength = 0;
        
        size_t offset = 0;
        if (dataBuffer->Data() != nullptr) {
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &result, sizeof(result));
            offset += sizeof(result);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &resultType, sizeof(resultType));
            offset += sizeof(resultType);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &resultLength, sizeof(resultLength));
        }
    }
    
    cmd.TriggerResponse(dataBuffer);
    
    std::string params = cmd.GetParams();
    uint8_t result = cmd.GetResult();
    
    (void)params;
    (void)result;
}

} // namespace MechBodyController
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::MechBodyController::GetMechRealNameCmdMarshalFuzzTest(data, size);
    OHOS::MechBodyController::GetMechRealNameCmdTriggerResponseFuzzTest(data, size);
    OHOS::MechBodyController::GetMechRealNameCmdGetParamsFuzzTest(data, size);
    OHOS::MechBodyController::GetMechRealNameCmdGetResultFuzzTest(data, size);
    OHOS::MechBodyController::GetMechRealNameCmdWithCallbackFuzzTest(data, size);
    OHOS::MechBodyController::GetMechRealNameCmdWithValidResponseFuzzTest(data, size);
    OHOS::MechBodyController::GetMechRealNameCmdWithEmptyNameFuzzTest(data, size);
    return 0;
}