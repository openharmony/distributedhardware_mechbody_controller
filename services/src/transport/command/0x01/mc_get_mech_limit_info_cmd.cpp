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

#include "mc_get_mech_limit_info_cmd.h"

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "GetMechLimitInfoCmd";
    static constexpr uint8_t CMD_GET_MECH_LIMIT_TYPE = 0x03;
    constexpr uint8_t CMD_GET_MECH_LIMIT_REPLY_LENGTH = 24;
}

GetMechLimitInfoCmd::GetMechLimitInfoCmd()
{
    cmdSet_ = CMD_SET;
    cmdId_ = CMD_ID;
    reqSize_ = REQ_SIZE;
    rspSize_ = RSP_SIZE;
    needResponse_ = (RSP_SIZE > 0);
    timeoutMs_ = MECHBODY_MSG_TIMEOUT;
    retryTimes_ = CMD_PRIORITY_MIDDLE;
}

std::shared_ptr<MechDataBuffer> GetMechLimitInfoCmd::Marshal() const
{
    HILOGI("start.");
    auto buffer = std::make_shared<MechDataBuffer>(reqSize_ + BIT_OFFSET_2);
    if (buffer == nullptr) {
        HILOGE("Failed to allocate memory for Marshal buffer");
        return nullptr;
    }

    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdSet_), nullptr, "append cmdSet_");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdId_), nullptr, "append cmdId_");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(CMD_GET_MECH_LIMIT_TYPE), nullptr, "append get mech limit type");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(BIT_0), nullptr, "append get mech limit len");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(BIT_0), nullptr, "append get mech limit value");

    HILOGI("end.");
    return buffer;
}

void GetMechLimitInfoCmd::TriggerResponse(std::shared_ptr<MechDataBuffer> data)
{
    if (data == nullptr || data->Size() < RSP_SIZE + BIT_OFFSET_2) {
        HILOGE("Invalid input data for response");
        return;
    }

    size_t offset = BIT_OFFSET_2;

    CHECK_ERR_RETURN(data->ReadUint8(offset, result_), "read result_");
    HILOGI("response code: %{public}u", result_);
    offset++;

    uint8_t resultType = 0;
    CHECK_ERR_RETURN(data->ReadUint8(offset, resultType), "read resultType");
    if (resultType != CMD_GET_MECH_LIMIT_TYPE) {
        HILOGE("Reply data resultType invalid");
        return;
    }
    offset++;

    uint8_t resultLength = 0;
    CHECK_ERR_RETURN(data->ReadUint8(offset, resultLength), "read resultLength");
    if (resultLength != CMD_GET_MECH_LIMIT_REPLY_LENGTH) {
        HILOGE("Reply data resultLength invalid");
        return;
    }
    offset++;

    CHECK_ERR_RETURN(data->ReadFloat(offset, params_.posMax.roll), "read rollMax");
    offset += sizeof(float);

    CHECK_ERR_RETURN(data->ReadFloat(offset, params_.negMax.roll), "read rollMin");
    offset += sizeof(float);

    CHECK_ERR_RETURN(data->ReadFloat(offset, params_.posMax.pitch), "read pitchMax");
    offset += sizeof(float);

    CHECK_ERR_RETURN(data->ReadFloat(offset, params_.negMax.pitch), "read pitchMin");
    offset += sizeof(float);

    CHECK_ERR_RETURN(data->ReadFloat(offset, params_.posMax.yaw), "read yawMax");
    offset += sizeof(float);

    CHECK_ERR_RETURN(data->ReadFloat(offset, params_.negMax.yaw), "read yawMin");
    offset += sizeof(float);

    if (responseCb_) {
        HILOGI("trigger response callback.");
        responseCb_();
    }
}

const RotateDegreeLimit& GetMechLimitInfoCmd::GetParams() const
{
    return params_;
}

uint8_t GetMechLimitInfoCmd::GetResult() const
{
    return result_;
}
} // namespace MechBodyController
} // namespace OHOS
