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

#include <algorithm>
#include <cstdlib>
#include <securec.h> 
#include "mc_data_buffer.h"
#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "MechDataBuffer";
}
MechDataBuffer::MechDataBuffer(size_t capacity)
{
    if (capacity != 0 && capacity < MECH_MAX_BUFFER_SIZE) {
        data_ = new uint8_t[capacity] {0};
        if (data_ != nullptr) {
            capacity_ = capacity;
            rangeLength_ = 0;
        }
    }
}

MechDataBuffer::~MechDataBuffer()
{
    if (data_ != nullptr) {
        delete[] data_;
        data_ = nullptr;
    }
}

size_t MechDataBuffer::Size()
{
    return rangeLength_;
}

size_t MechDataBuffer::Offset()
{
    return rangeOffset_;
}

size_t MechDataBuffer::Capacity()
{
    return capacity_;
}

uint8_t *MechDataBuffer::Data()
{
    if (data_ == nullptr) {
        return nullptr;
    }
    return data_ + rangeOffset_;
}

int32_t MechDataBuffer::SetRange(size_t offset, size_t size)
{
    if (!(offset <= capacity_) || !(offset + size <= capacity_)) {
        return INVALID_PARAMETERS_ERR;
    }

    rangeOffset_ = offset;
    rangeLength_ = size;
    return ERR_OK;
}

int32_t MechDataBuffer::AppendUint8(uint8_t value)
{
    if (data_ == nullptr || rangeOffset_ + rangeLength_ + sizeof(value) > capacity_) {
        return INVALID_PARAMETERS_ERR;
    }
    if (memcpy_s(Data() + rangeLength_, capacity_ - rangeOffset_ - rangeLength_, &value, sizeof(value)) != 0) {
        return INVALID_PARAMETERS_ERR;
    }
    rangeLength_ += sizeof(value);
    return ERR_OK;
}

int32_t MechDataBuffer::AppendUint16(uint16_t value)
{
    if (data_ == nullptr || rangeOffset_ + rangeLength_ + sizeof(value) > capacity_) {
        return INVALID_PARAMETERS_ERR;
    }
    if (memcpy_s(Data() + rangeLength_, capacity_ - rangeOffset_ - rangeLength_, &value, sizeof(value)) != 0) {
        return INVALID_PARAMETERS_ERR;
    }
    rangeLength_ += sizeof(value);
    return ERR_OK;
}

int32_t MechDataBuffer::AppendUint32(uint32_t value)
{
    if (data_ == nullptr || rangeOffset_ + rangeLength_ + sizeof(value) > capacity_) {
        return INVALID_PARAMETERS_ERR;
    }
    if (memcpy_s(Data() + rangeLength_, capacity_ - rangeOffset_ - rangeLength_, &value, sizeof(value)) != 0) {
        return INVALID_PARAMETERS_ERR;
    }
    rangeLength_ += sizeof(value);
    return ERR_OK;
}

int32_t MechDataBuffer::AppendUint64(uint64_t value)
{
    if (data_ == nullptr || rangeOffset_ + rangeLength_ + sizeof(value) > capacity_) {
        return INVALID_PARAMETERS_ERR;
    }
    if (memcpy_s(Data() + rangeLength_, capacity_ - rangeOffset_ - rangeLength_, &value, sizeof(value)) != 0) {
        return INVALID_PARAMETERS_ERR;
    }
    rangeLength_ += sizeof(value);
    return ERR_OK;
}

int32_t MechDataBuffer::AppendFloat(float value)
{
    if (data_ == nullptr || rangeOffset_ + rangeLength_ + sizeof(value) > capacity_) {
        return INVALID_PARAMETERS_ERR;
    }
    if (memcpy_s(Data() + rangeLength_, capacity_ - rangeOffset_ - rangeLength_, &value, sizeof(value)) != 0) {
        return INVALID_PARAMETERS_ERR;
    }
    rangeLength_ += sizeof(value);
    return ERR_OK;
}

int32_t MechDataBuffer::AppendDataBuffer(std::shared_ptr<MechDataBuffer> data)
{
    if (data_ == nullptr || rangeOffset_ + rangeLength_ + data->Size() > capacity_) {
        return INVALID_PARAMETERS_ERR;
    }
    if (memcpy_s(Data() + rangeLength_, capacity_ - rangeOffset_ - rangeLength_, data->Data(), data->Size()) != 0) {
        return INVALID_PARAMETERS_ERR;
    }
    rangeLength_ += data->Size();
    return ERR_OK;
}

int32_t MechDataBuffer::ReadUint8(size_t offset, uint8_t& outValue)
{
    if (data_ == nullptr || rangeOffset_ + offset + sizeof(uint8_t) > rangeLength_) {
        return INVALID_PARAMETERS_ERR;
    }
    if (memcpy_s(&outValue, sizeof(outValue), Data() + offset, sizeof(uint8_t)) != 0) {
        return INVALID_PARAMETERS_ERR;
    }
    return ERR_OK;
}

int32_t MechDataBuffer::ReadUint16(size_t offset, uint16_t& outValue)
{
    if (data_ == nullptr || rangeOffset_ + offset + sizeof(uint16_t) > rangeLength_) {
        return INVALID_PARAMETERS_ERR;
    }
    if (memcpy_s(&outValue, sizeof(outValue), Data() + offset, sizeof(uint16_t)) != 0) {
        return INVALID_PARAMETERS_ERR;
    }
    return ERR_OK;
}

int32_t MechDataBuffer::ReadUint32(size_t offset, uint32_t& outValue)
{
    if (data_ == nullptr || rangeOffset_ + offset + sizeof(uint32_t) > rangeLength_) {
        return INVALID_PARAMETERS_ERR;
    }
    if (memcpy_s(&outValue, sizeof(outValue), Data() + offset, sizeof(uint32_t)) != 0) {
        return INVALID_PARAMETERS_ERR;
    }
    return ERR_OK;
}

int32_t MechDataBuffer::ReadUint64(size_t offset, uint64_t& outValue)
{
    if (data_ == nullptr || rangeOffset_ + offset + sizeof(uint64_t) > rangeLength_) {
        return INVALID_PARAMETERS_ERR;
    }
    if (memcpy_s(&outValue, sizeof(outValue), Data() + offset, sizeof(uint64_t)) != 0) {
        return INVALID_PARAMETERS_ERR;
    }
    return ERR_OK;
}

int32_t MechDataBuffer::ReadFloat(size_t offset, float& outValue)
{
    if (data_ == nullptr || rangeOffset_ + offset + sizeof(float) > rangeLength_) {
        return INVALID_PARAMETERS_ERR;
    }
    if (memcpy_s(&outValue, sizeof(outValue), Data() + offset, sizeof(float)) != 0) {
        return INVALID_PARAMETERS_ERR;
    }
    return ERR_OK;
}

int32_t MechDataBuffer::ReadInt16(size_t offset, int16_t& outValue)
{
    if (data_ == nullptr || rangeOffset_ + offset + sizeof(int16_t) > rangeLength_) {
        return INVALID_PARAMETERS_ERR;
    }
    if (memcpy_s(&outValue, sizeof(outValue), Data() + offset, sizeof(int16_t)) != 0) {
        return INVALID_PARAMETERS_ERR;
    }
    return ERR_OK;
}
}  // namespace MechBodyController
}  // namespace OHOS
