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

#ifndef OHOS_MECH_DATA_BUFFER_H
#define OHOS_MECH_DATA_BUFFER_H

#include <string>

namespace OHOS {
namespace MechBodyController {
class MechDataBuffer {
public:
    explicit MechDataBuffer(size_t capacity);
    ~MechDataBuffer();

    size_t Size();
    size_t Offset();
    size_t Capacity();
    uint8_t *Data();
    int32_t SetRange(size_t offset, size_t size);

    int32_t AppendUint8(uint8_t value);
    int32_t AppendUint16(uint16_t value);
    int32_t AppendUint32(uint32_t value);
    int32_t AppendUint64(uint64_t value);
    int32_t AppendFloat(float value);
    int32_t AppendDouble(double value);
    int32_t AppendDataBuffer(std::shared_ptr<MechDataBuffer> data);

    int32_t ReadUint8(size_t offset, uint8_t& outValue);
    int32_t ReadUint16(size_t offset, uint16_t& outValue);
    int32_t ReadUint32(size_t offset, uint32_t& outValue);
    int32_t ReadUint64(size_t offset, uint64_t& outValue);
    int32_t ReadFloat(size_t offset, float& outValue);
    int32_t ReadInt16(size_t offset, int16_t& outValue);
    int32_t ReadString(size_t offset, char *outValue, size_t length);

private:
    static const uint32_t MECH_MAX_BUFFER_SIZE = 80 * 1024 * 1024;

    size_t capacity_ = 0;
    size_t rangeOffset_ = 0;
    size_t rangeLength_ = 0;
    uint8_t *data_ = nullptr;
};
}  // namespace MechBodyController
}  // namespace OHOS
#endif  // OHOS_MECH_DATA_BUFFER_H
