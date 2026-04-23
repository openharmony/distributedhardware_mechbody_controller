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

#include "mc_wheel_set_mech_relative_position_cmd.h"

#include "mechbody_controller_log.h"
#include "mechbody_controller_utils.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "WheelSetMechRelativePositionCmd";
    constexpr size_t CMD_MAX_POINT_SIZE = 10;
    constexpr uint16_t CMD_SIZE_PER_ROTATE_POINT = 4;
    constexpr uint8_t CMD_SET_WHEEL_CONTROL_RALEVANT = 0b00000000;
}

WheelSetMechRelativePositionCmd::WheelSetMechRelativePositionCmd(uint16_t taskId,
    const std::vector<WheelPositionInfo>& params)
    : taskId_(taskId), params_(params)
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

std::shared_ptr<MechDataBuffer> WheelSetMechRelativePositionCmd::Marshal() const
{
    HILOGE("start.");
    auto buffer = std::make_shared<MechDataBuffer>(reqSize_ + BIT_OFFSET_2);
    if (buffer == nullptr) {
        HILOGE("Failed to allocate memory for Marshal buffer");
        return nullptr;
    }

    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdSet_), nullptr, "append cmdSet_");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdId_), nullptr, "append cmdId_");

    uint8_t pointNum = static_cast<uint8_t>(params_.size());
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint16(taskId_), nullptr, "append taskId");

    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(CMD_SET_WHEEL_CONTROL_RALEVANT), nullptr, "append control byte");

    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(pointNum), nullptr, "append point number");

    for (uint8_t i = 0; i < pointNum; i++) {
        CHECK_ERR_RETURN_VALUE(buffer->AppendInt16(params_[i].xCoordinates), nullptr, "append xCoordinates");
        CHECK_ERR_RETURN_VALUE(buffer->AppendInt16(params_[i].yCoordinates), nullptr, "append yCoordinates");
    }
    HILOGE("end.");
    return buffer;
}

void WheelSetMechRelativePositionCmd::TriggerResponse(std::shared_ptr<MechDataBuffer> data)
{
    if (data == nullptr || data->Size() < RSP_SIZE + BIT_OFFSET_2) {
        HILOGE("Invalid input data for response");
        return;
    }

    CHECK_ERR_RETURN(data->ReadUint16(BIT_OFFSET_2, taskId_), "read taskId_");
    HILOGI("response code: %{public}u", taskId_);

    CHECK_ERR_RETURN(data->ReadUint8(BIT_OFFSET_4, errCode_), "read errCode_");
    HILOGI("response code: %{public}u", errCode_);
    if (responseCb_) {
        HILOGI("trigger response callback.");
        responseCb_();
    }
}

const std::vector<WheelPositionInfo>& WheelSetMechRelativePositionCmd::GetParams() const
{
    return params_;
}

uint16_t WheelSetMechRelativePositionCmd::GetTaskId() const
{
    return taskId_;
}

uint8_t WheelSetMechRelativePositionCmd::GetErrCode() const
{
    return errCode_;
}
} // namespace MechBodyController
} // namespace OHOS
