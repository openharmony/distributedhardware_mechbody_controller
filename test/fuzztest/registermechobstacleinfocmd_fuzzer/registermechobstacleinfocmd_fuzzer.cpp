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
#include "registermechobstacleinfocmd_fuzzer.h"
#include "mc_register_mech_obstacle_info_cmd.h"
#include "mc_data_buffer.h"
#include <memory>
#include <securec.h>

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

constexpr size_t RPT_SIZE = 1;
constexpr size_t BIT_OFFSET_2 = 2;
constexpr size_t BIT_OFFSET_5 = 5;

void FuzzUnmarshalWithNoObstacle(FuzzedDataProvider &provider)
{
    RegisterMechObstacleInfoCmd cmd;
    auto buffer = cmd.Marshal();
    size_t dataSize = RPT_SIZE + BIT_OFFSET_2;
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);
        if (dataBuffer->Data() != nullptr) {
            uint8_t obstacleNums = 0;
            size_t offset = 0;
            uint8_t reserved = provider.ConsumeIntegralInRange<uint8_t>(0, 3);
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved, sizeof(reserved));
            offset += sizeof(reserved);
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved, sizeof(reserved));
            offset += sizeof(reserved);
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &obstacleNums, sizeof(obstacleNums));
        }
    }
    bool result = cmd.Unmarshal(dataBuffer);
    (void)result;
}

void FuzzUnmarshalWithObstacleNoDetails(FuzzedDataProvider &provider)
{
    RegisterMechObstacleInfoCmd cmd;
    uint8_t obstacleNums = provider.ConsumeIntegralInRange<uint8_t>(1, 5);
    // 修复： dataSize = BIT_OFFSET_2(前2字节) + sizeof(obstacleNums) + obstacleNums * BIT_OFFSET_5
    size_t dataSize = BIT_OFFSET_2 + sizeof(obstacleNums) + obstacleNums * BIT_OFFSET_5;
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
            // obstacleNums写在offset=2位置
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &obstacleNums, sizeof(obstacleNums));
            offset++;
            for (uint8_t i = 0; i < obstacleNums; i++) {
                int16_t direction = provider.ConsumeIntegral<int16_t>();
                int16_t pitchAngle = provider.ConsumeIntegral<int16_t>();
                uint8_t detailLength = 0;
                uint8_t reserved2 = 0;
                memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved2, sizeof(reserved2));
                offset += sizeof(reserved2);
                memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved2, sizeof(reserved2));
                offset += sizeof(reserved2);
                (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &direction, sizeof(direction));
                offset += sizeof(direction);
                (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &pitchAngle, sizeof(pitchAngle));
                offset += sizeof(pitchAngle);
                (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &detailLength, sizeof(detailLength));
                offset += sizeof(detailLength);
            }
        }
    }
    bool result = cmd.Unmarshal(dataBuffer);
    (void)result;
}

std::shared_ptr<MechDataBuffer> FuzzCreateAndInitBufferForDetails(FuzzedDataProvider &provider, uint8_t obstacleNums,
                                                                  uint8_t detailLen)
{
    size_t dataSize = BIT_OFFSET_2 + sizeof(obstacleNums) + obstacleNums * (BIT_OFFSET_5 + detailLen);
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer == nullptr) {
        return nullptr;
    }

    dataBuffer->SetRange(0, dataSize);
    if (dataBuffer->Data() == nullptr) {
        return nullptr;
    }

    size_t offset = 0;
    uint8_t reserved = provider.ConsumeIntegralInRange<uint8_t>(0, 3);
    memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved, sizeof(reserved));
    offset += sizeof(reserved);
    memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved, sizeof(reserved));
    offset += sizeof(reserved);
    memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &obstacleNums, sizeof(obstacleNums));

    return dataBuffer;
}

void FuzzWriteObstacleDetails(FuzzedDataProvider &provider, std::shared_ptr<MechDataBuffer> dataBuffer, size_t dataSize,
                              uint8_t obstacleNums, uint8_t detailLen)
{
    size_t offset = BIT_OFFSET_2 + sizeof(obstacleNums);

    for (uint8_t i = 0; i < obstacleNums; i++) {
        int16_t direction = provider.ConsumeIntegral<int16_t>();
        int16_t pitchAngle = provider.ConsumeIntegral<int16_t>();
        uint8_t detailLength = detailLen;
        (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &direction, sizeof(direction));
        offset += sizeof(direction);
        (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &pitchAngle, sizeof(pitchAngle));
        offset += sizeof(pitchAngle);
        (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &detailLength, sizeof(detailLength));
        offset++;

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
}

void FuzzUnmarshalWithObstacleAndDetails(FuzzedDataProvider &provider)
{
    RegisterMechObstacleInfoCmd cmd;
    uint8_t obstacleNums = provider.ConsumeIntegralInRange<uint8_t>(1, 3);
    uint8_t detailLen = provider.ConsumeIntegralInRange<uint8_t>(1, 4);

    std::shared_ptr<MechDataBuffer> dataBuffer = FuzzCreateAndInitBufferForDetails(provider, obstacleNums, detailLen);
    if (dataBuffer == nullptr) {
        bool unmarshalResult = cmd.Unmarshal(dataBuffer);
        (void)unmarshalResult;
        return;
    }

    size_t dataSize = BIT_OFFSET_2 + sizeof(obstacleNums) + obstacleNums * (BIT_OFFSET_5 + detailLen);
    FuzzWriteObstacleDetails(provider, dataBuffer, dataSize, obstacleNums, detailLen);

    bool unmarshalResult = cmd.Unmarshal(dataBuffer);
    const std::vector<ObstacleInfo> obstacles = cmd.GetObstacles();
    uint8_t obstacleNumsResult = cmd.GetObstacleNums();
    uint8_t result = cmd.GetResult();
    (void)obstacles;
    (void)obstacleNumsResult;
    (void)unmarshalResult;
    (void)result;
}

void FuzzTriggerResponse(FuzzedDataProvider &provider)
{
    RegisterMechObstacleInfoCmd cmd;
    size_t dataSize = RPT_SIZE;
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);
        if (dataBuffer->Data() != nullptr) {
            uint8_t result = provider.ConsumeIntegral<uint8_t>();
            uint8_t reserved = 0;
            size_t offset = 0;
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved, sizeof(reserved));
            offset += sizeof(reserved);
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved, sizeof(reserved));
            offset += sizeof(reserved);
            memcpy_s(dataBuffer->Data(), dataSize, &result, sizeof(result));
        }
    }
    cmd.TriggerResponse(dataBuffer);
}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    FuzzUnmarshalWithNoObstacle(provider);
    FuzzUnmarshalWithObstacleNoDetails(provider);
    FuzzUnmarshalWithObstacleAndDetails(provider);
    FuzzTriggerResponse(provider);
    return 0;
}