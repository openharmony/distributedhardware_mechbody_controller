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
#include "registermechcliffinfocmd_fuzzer.h"
#include "mc_register_mech_cliff_info_cmd.h"
#include "mc_data_buffer.h"
#include <memory>
#include <securec.h>

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

constexpr size_t RPT_SIZE = 1;
constexpr size_t BIT_OFFSET_2 = 2;
constexpr size_t BIT_OFFSET_3 = 3;

void FuzzUnmarshalWithNoCliff(FuzzedDataProvider &provider)
{
    RegisterMechCliffInfoCmd cmd;
    auto buffer = cmd.Marshal();
    size_t dataSize = RPT_SIZE + BIT_OFFSET_2;
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);
        if (dataBuffer->Data() != nullptr) {
            uint8_t cliffNums = 0;
            size_t offset = 0;
            uint8_t reserved = provider.ConsumeIntegralInRange<uint8_t>(0, 3);
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved, sizeof(reserved));
            offset += sizeof(reserved);
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved, sizeof(reserved));
            offset += sizeof(reserved);
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &cliffNums, sizeof(cliffNums));
        }
    }
    bool result = cmd.Unmarshal(dataBuffer);
    (void)result;
}

void FuzzUnmarshalWithCliffNoDetails(FuzzedDataProvider &provider)
{
    RegisterMechCliffInfoCmd cmd;
    uint8_t cliffNums = provider.ConsumeIntegralInRange<uint8_t>(1, 5);
    // 修复： dataSize = BIT_OFFSET_2(前2字节) + sizeof(cliffNums) + cliffNums * BIT_OFFSET_3
    size_t dataSize = BIT_OFFSET_2 + sizeof(cliffNums) + cliffNums * BIT_OFFSET_3;
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);
        if (dataBuffer->Data() != nullptr) {
            size_t offset = 0;
            uint8_t reserved = 0;
            // 前2个保留字节
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved, sizeof(reserved));
            offset += sizeof(reserved);
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved, sizeof(reserved));
            offset += sizeof(reserved);
            // cliffNums写在offset=2位置
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &cliffNums, sizeof(cliffNums));
            offset++;
            for (uint8_t i = 0; i < cliffNums; i++) {
                int16_t cliffDirection = provider.ConsumeIntegral<int16_t>();
                uint8_t cliffInfoLen = 0;
                uint8_t reserved2 = 0;
                memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved2, sizeof(reserved2));
                offset += sizeof(reserved2);
                memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved2, sizeof(reserved2));
                offset += sizeof(reserved2);
                (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &cliffDirection, sizeof(cliffDirection));
                offset += sizeof(cliffDirection);
                (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &cliffInfoLen, sizeof(cliffInfoLen));
                offset += sizeof(cliffInfoLen);
            }
        }
    }
    bool unmarshalResult = cmd.Unmarshal(dataBuffer);
    const std::vector<CliffInfo> cliffs = cmd.GetCliffs();
    uint8_t cliffNumsResult = cmd.GetCliffNums();
    uint8_t result = cmd.GetResult();
    (void)cliffs;
    (void)cliffNumsResult;
    (void)unmarshalResult;
    (void)result;
}

void FillDetailData(FuzzedDataProvider &provider, std::shared_ptr<MechDataBuffer> dataBuffer,
    size_t dataSize, uint8_t detailLen)
{
    size_t offset = BIT_OFFSET_2 + sizeof(uint8_t);
    for (uint8_t j = 0; j < detailLen; j++) {
        uint8_t detailType = provider.ConsumeIntegral<uint8_t>();
        uint8_t dataLen = provider.ConsumeIntegral<uint8_t>();
        uint8_t reserved2 = 0;
        memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved2, sizeof(reserved2));
        offset += sizeof(reserved2);
        memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved2, sizeof(reserved2));
        offset += sizeof(reserved2);
        (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &detailType, sizeof(detailType));
        offset++;
        (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &dataLen, sizeof(dataLen));
        offset++;
        offset += dataLen;
    }
}

void FuzzUnmarshalWithCliffAndDetails(FuzzedDataProvider &provider)
{
    RegisterMechCliffInfoCmd cmd;
    uint8_t cliffNums = provider.ConsumeIntegralInRange<uint8_t>(1, 3);
    uint8_t detailLen = provider.ConsumeIntegralInRange<uint8_t>(1, 4);
    size_t dataSize = BIT_OFFSET_2 + sizeof(cliffNums) + cliffNums * (BIT_OFFSET_3 + detailLen);
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer == nullptr) {
        bool result = cmd.Unmarshal(dataBuffer);
        (void)result;
        return;
    }
    dataBuffer->SetRange(0, dataSize);
    if (dataBuffer->Data() == nullptr) {
        bool result = cmd.Unmarshal(dataBuffer);
        (void)result;
        return;
    }

    size_t offset = 0;
    uint8_t reserved = 0;
    memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved, sizeof(reserved));
    offset += sizeof(reserved);
    memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved, sizeof(reserved));
    offset += sizeof(reserved);
    memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &cliffNums, sizeof(cliffNums));
    offset++;

    for (uint8_t i = 0; i < cliffNums; i++) {
        int16_t cliffDirection = provider.ConsumeIntegral<int16_t>();
        uint8_t cliffInfoLen = detailLen;
        (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &cliffDirection, sizeof(cliffDirection));
        offset += sizeof(cliffDirection);
        (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &cliffInfoLen, sizeof(cliffInfoLen));
        offset++;
        FillDetailData(provider, dataBuffer, dataSize, detailLen);
    }

    bool result = cmd.Unmarshal(dataBuffer);
    (void)result;
}

void FuzzTriggerResponse(FuzzedDataProvider &provider)
{
    RegisterMechCliffInfoCmd cmd;
    size_t dataSize = RPT_SIZE;
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);
        if (dataBuffer->Data() != nullptr) {
            uint8_t result = provider.ConsumeIntegral<uint8_t>();
            memcpy_s(dataBuffer->Data(), dataSize, &result, sizeof(result));
        }
    }
    cmd.TriggerResponse(dataBuffer);
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    FuzzUnmarshalWithNoCliff(provider);
    FuzzUnmarshalWithCliffNoDetails(provider);
    FuzzUnmarshalWithCliffAndDetails(provider);
    FuzzTriggerResponse(provider);
    return 0;
}