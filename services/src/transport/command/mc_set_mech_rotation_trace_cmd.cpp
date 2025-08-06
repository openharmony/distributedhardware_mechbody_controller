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

#include "mc_set_mech_rotation_trace_cmd.h"

#include "mechbody_controller_log.h"
#include "mechbody_controller_utils.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "SetMechRotationTraceCmd";
    constexpr size_t CMD_MAX_POINT_SIZE = 10;
    constexpr uint16_t CMD_SIZE_PER_ROTATE_POINT = 13 * 2;
    constexpr uint8_t CMD_SET_ROTATE_TYPE = 0x04;
    constexpr uint8_t CMD_SET_ROTATE_CONTROL_RALEVANT = 0b00000100;
    constexpr uint8_t CMD_SET_ROTATE_PITCH = 0x01;
    constexpr uint8_t CMD_SET_ROTATE_YAW = 0x02;
    constexpr float CMD_SET_ROTATE_MS_TO_S = 1000.0f;
}

SetMechRotationTraceCmd::SetMechRotationTraceCmd(const std::vector<RotateParam>& params)
    : params_(params)
{
    cmdSet_ = CMD_SET;
    cmdId_ = CMD_ID;
    if (params.size() > CMD_MAX_POINT_SIZE) {
        HILOGE("point size %{public}zu exceeds max size %{public}zu", params.size(), CMD_MAX_POINT_SIZE);
        return;
    }
    reqSize_ = REQ_SIZE + static_cast<uint8_t>(params.size()) * CMD_SIZE_PER_ROTATE_POINT;
    rspSize_ = RSP_SIZE;
    needResponse_ = (RSP_SIZE > 0);
    timeoutMs_ = MECHBODY_MSG_TIMEOUT;
    retryTimes_ = CMD_PRIORITY_MIDDLE;
}

std::shared_ptr<MechDataBuffer> SetMechRotationTraceCmd::Marshal() const
{
    HILOGD("start.");
    auto buffer = std::make_shared<MechDataBuffer>(reqSize_ + BIT_OFFSET_2);
    if (buffer == nullptr) {
        HILOGE("Failed to allocate memory for Marshal buffer");
        return nullptr;
    }

    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdSet_), nullptr, "append cmdSet_");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdId_), nullptr, "append cmdId_");

    uint8_t pointNum = static_cast<uint8_t>(params_.size());
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(CMD_SET_ROTATE_TYPE), nullptr, "append tlv type");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(BIT_5 + CMD_SIZE_PER_ROTATE_POINT * pointNum),
        nullptr, "append tlv size");

    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(CMD_SET_ROTATE_CONTROL_RALEVANT), nullptr, "append control byte");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(BIT_0), nullptr, "append reserve byte");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(BIT_0), nullptr, "append reserve byte");

    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(pointNum * BIT_2), nullptr, "append point number");

    for (uint8_t i = 0; i < pointNum; i++) {
        CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(CMD_SET_ROTATE_PITCH), nullptr, "append pitch sign");
        CHECK_ERR_RETURN_VALUE(buffer->AppendFloat(params_[i].duration / CMD_SET_ROTATE_MS_TO_S),
            nullptr, "append duration");
        CHECK_ERR_RETURN_VALUE(buffer->AppendFloat(params_[i].degree.pitch), nullptr, "append pitch");
        CHECK_ERR_RETURN_VALUE(buffer->AppendFloat(0.0f), nullptr, "append reserve float");
        CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(CMD_SET_ROTATE_YAW), nullptr, "append yaw sign");
        CHECK_ERR_RETURN_VALUE(buffer->AppendFloat(params_[i].duration / CMD_SET_ROTATE_MS_TO_S),
            nullptr, "append duration");
        CHECK_ERR_RETURN_VALUE(buffer->AppendFloat(params_[i].degree.yaw), nullptr, "append yaw");
        CHECK_ERR_RETURN_VALUE(buffer->AppendFloat(0.0f), nullptr, "append reserve float");
    }
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(BIT_0), nullptr, "append reserve byte");
    HILOGD("end.");
    return buffer;
}

void SetMechRotationTraceCmd::TriggerResponse(std::shared_ptr<MechDataBuffer> data)
{
    if (data == nullptr || data->Size() < RSP_SIZE + BIT_OFFSET_2) {
        HILOGE("Invalid input data for response");
        return;
    }

    CHECK_ERR_RETURN(data->ReadUint8(BIT_OFFSET_2, result_), "read result_");
    HILOGI("response code: %{public}u", result_);

    if (responseCb_) {
        HILOGI("trigger response callback.");
        responseCb_();
    }
}

const std::vector<RotateParam>& SetMechRotationTraceCmd::GetParams() const
{
    return params_;
}

uint8_t SetMechRotationTraceCmd::GetResult() const
{
    return result_;
}
} // namespace MechBodyController
} // namespace OHOS
