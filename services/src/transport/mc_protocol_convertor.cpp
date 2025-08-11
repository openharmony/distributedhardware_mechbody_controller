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

#include "mc_protocol_convertor.h"

#include <securec.h> 

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "ProtocolConverter";
    constexpr size_t MAX_DATA_SIZE = 251;
    constexpr size_t MAX_SERVICE_NAME_SIZE = 0xFF;
    constexpr size_t BT_HEADER_LENTH = 8;
    constexpr size_t BT_SERVICE_HEADER_LENTH = 4;
    constexpr size_t BT_COMMAND_HEADER_LENTH = 2;
    constexpr uint8_t BT_PROTOCOL_VERSION = 0;
    constexpr uint8_t BT_PROTOCOL_MSG_ID = 0;
    constexpr uint8_t BT_PROTOCOL_TOTAL_FRAME = 1;
    constexpr uint8_t BT_PROTOCOL_FRAME_SEQ = 0;
    constexpr uint8_t BT_PROTOCOL_REV = 0;
    constexpr uint8_t BT_PROTOCOL_ENCRY = 0x00;
    constexpr uint16_t BT_PROTOCOL_RETURN = 0x00;
    constexpr uint8_t BT_PAYLOAD_DATA_FORMAT = 0x02;
    constexpr uint8_t BT_PROTOCOL_CMD_TYPE_MASK = 0x0F;
}

std::shared_ptr<MechDataBuffer> ProtocolConverter::Convert(
    OptType optType, uint16_t seqNo, std::shared_ptr<MechDataBuffer> data) const
{
    HILOGD("start.");
    if (data == nullptr) {
        HILOGE("data is nullptr!");
        return nullptr;
    }

    if (serviceName_.size() > MAX_SERVICE_NAME_SIZE) {
        HILOGE("serviceName size %{public}zu exceeds maximum size %{public}zu!", serviceName_.size(), MAX_DATA_SIZE);
        return nullptr;
    }

    size_t protocolSize =
        BT_HEADER_LENTH + BT_SERVICE_HEADER_LENTH + serviceName_.size() + BT_COMMAND_HEADER_LENTH + data->Size();
    if (protocolSize > MAX_DATA_SIZE) {
        HILOGE("protocol size %{public}zu exceeds maximum size %{public}zu!", data->Size(), MAX_DATA_SIZE);
        return nullptr;
    }
    std::shared_ptr<MechDataBuffer> pclData = std::make_shared<MechDataBuffer>(protocolSize);

    /** Header defination (BT_HEADER_LENTH + payload)
     *  | Version | CmdType | MsgID | TotalFrame | FrameSeq | Rev | Encry | Return | Payload |
     *  |     0.5 |     0.5 |     1 |          1 |        1 |   1 |     1 |      2 |       n |
     */
    uint8_t versionAndType = (BT_PROTOCOL_VERSION << BIT_OFFSET_4) | static_cast<uint8_t>(ProtocolCmdType::REQ);
    CHECK_ERR_RETURN_VALUE(pclData->AppendUint8(versionAndType), nullptr, "append versionAndType");
    CHECK_ERR_RETURN_VALUE(pclData->AppendUint8(BT_PROTOCOL_MSG_ID), nullptr, "append MsgID");
    CHECK_ERR_RETURN_VALUE(pclData->AppendUint8(BT_PROTOCOL_TOTAL_FRAME), nullptr, "append TotalFrame");
    CHECK_ERR_RETURN_VALUE(pclData->AppendUint8(BT_PROTOCOL_FRAME_SEQ), nullptr, "append Rev");
    CHECK_ERR_RETURN_VALUE(pclData->AppendUint8(BT_PROTOCOL_REV), nullptr, "append Encry");
    CHECK_ERR_RETURN_VALUE(pclData->AppendUint8(BT_PROTOCOL_ENCRY), nullptr, "append Return");
    CHECK_ERR_RETURN_VALUE(pclData->AppendUint16(BT_PROTOCOL_RETURN), nullptr, "append Payload");

    /** Payload defination (BT_SERVICE_HEADER_LENTH + nameLength(n) + Body)
     *  | DataFormat | OptType | ServiceLenth | ServiceName | BodyLenth | Body |
     *  |        0.5 |     0.5 |            1 |           n |         2 |    N |
     */
    uint8_t formatAndOptType = (BT_PAYLOAD_DATA_FORMAT << BIT_OFFSET_4) | static_cast<uint8_t>(optType);
    CHECK_ERR_RETURN_VALUE(pclData->AppendUint8(formatAndOptType), nullptr, "append formatAndOptType");
    CHECK_ERR_RETURN_VALUE(pclData->AppendUint8(
        static_cast<uint8_t>(serviceName_.size())), nullptr, "append ServiceLenth");
    for (char ch : serviceName_) {
        CHECK_ERR_RETURN_VALUE(pclData->AppendUint8(static_cast<uint8_t>(ch)), nullptr, "append ServiceName");
    }
    CHECK_ERR_RETURN_VALUE(pclData->AppendUint16(static_cast<uint16_t>(data->Size()) + BT_COMMAND_HEADER_LENTH),
        nullptr, "append BodyLenth");

    /** Body defination (BT_COMMAND_HEADER_LENTH + n)
     *  | SeqNo | Data |
     *  |     2 |    n |
     */
    CHECK_ERR_RETURN_VALUE(pclData->AppendUint16(seqNo), nullptr, "append SeqNo");
    CHECK_ERR_RETURN_VALUE(pclData->AppendDataBuffer(data), nullptr, "append Data");

    HILOGD("end.");
    return pclData;
}

std::shared_ptr<MechDataBuffer> ProtocolConverter::Convert(
    OptType optType, uint16_t seqNo, std::shared_ptr<MechDataBuffer> data, std::string serviceName)
{
    serviceName_ = serviceName;
    return Convert(optType, seqNo, data);
}

std::shared_ptr<MechDataBuffer> ProtocolConverter::GetData(
    std::shared_ptr<MechDataBuffer> pclData, uint16_t &seqNo, bool &isAck) const
{
    HILOGD("start.");
    CHECK_POINTER_RETURN_VALUE(pclData, nullptr, "pclData");
    uint8_t versionAndType = 0;
    CHECK_ERR_RETURN_VALUE(pclData->ReadUint8(0, versionAndType), nullptr, "read cmdType");
    uint8_t cmdType = versionAndType & BT_PROTOCOL_CMD_TYPE_MASK;
    switch (cmdType) {
        case static_cast<uint8_t>(ProtocolCmdType::RSP):
            isAck = true;
            break;
        default:
            isAck = false;
            break;
    }

    // Get seqNo
    size_t seqNoOffset = BT_HEADER_LENTH + BT_SERVICE_HEADER_LENTH + serviceName_.size();
    CHECK_ERR_RETURN_VALUE(pclData->ReadUint16(seqNoOffset, seqNo), nullptr, "read seqNo");

    // Get data lenth
    size_t dataLenOffset = BT_HEADER_LENTH + BT_SERVICE_HEADER_LENTH + serviceName_.size() - BIT_OFFSET_2;
    uint16_t dataLen = 0;
    CHECK_ERR_RETURN_VALUE(pclData->ReadUint16(dataLenOffset, dataLen), nullptr, "read dataLen");

    if (dataLen > pclData->Size() - seqNoOffset) {
        HILOGE("datalen larger than actual length!");
        return nullptr;
    }

    // Copy data
    std::shared_ptr<MechDataBuffer> data = std::make_shared<MechDataBuffer>(static_cast<size_t>(dataLen));
    data->SetRange(0, static_cast<size_t>(dataLen));
    if (memcpy_s(
        data->Data(), data->Capacity(), pclData->Data() + seqNoOffset + BT_COMMAND_HEADER_LENTH, dataLen) != 0) {
        HILOGE("memcpy failed.");
        return nullptr;
    }
    HILOGD("end.");
    return data;
}

bool ProtocolConverter::Validate(const uint8_t* data, size_t length) const
{
    if (data == nullptr) {
        HILOGE("data is nullptr!");
        return false;
    }
    if (length == 0 || length > MAX_DATA_SIZE) {
        HILOGE("data size %{public}zu is 0 or exceeds maximum size %{public}zu!", length, MAX_DATA_SIZE);
        return false;
    }
    return true;
}
} // namespace MechBodyController
} // namespace OHOS
