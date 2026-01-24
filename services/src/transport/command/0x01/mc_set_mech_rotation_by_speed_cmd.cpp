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

#include "mc_set_mech_rotation_by_speed_cmd.h"

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "SetMechRotationBySpeedCmd";
    constexpr uint8_t CMD_SET_ROTATE_BY_SPEED_CONTROL = 0b10000000;
    constexpr uint8_t CMD_SET_ROTATE_BY_SPEED_CONTROL_EXTEND = 0x1;
    constexpr uint8_t NEG_LIMITED = 2;
}

SetMechRotationBySpeedCmd::SetMechRotationBySpeedCmd(const RotateBySpeedParam& params)
    : params_(params)
{
    cmdSet_ = CMD_SET;
    cmdId_ = CMD_ID;
    reqSize_ = REQ_SIZE;
    rspSize_ = RSP_SIZE;
    needResponse_ = (RSP_SIZE > 0);
    timeoutMs_ = MECHBODY_MSG_TIMEOUT;
    retryTimes_ = CMD_PRIORITY_MIDDLE;
}

std::shared_ptr<MechDataBuffer> SetMechRotationBySpeedCmd::Marshal() const
{
    HILOGW("start.");
    auto buffer = std::make_shared<MechDataBuffer>(reqSize_ + BIT_OFFSET_2);
    if (buffer == nullptr) {
        HILOGE("Failed to allocate memory for Marshal buffer");
        return nullptr;
    }

    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdSet_), nullptr, "append cmdSet_");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdId_), nullptr, "append cmdId_");

    CHECK_ERR_RETURN_VALUE(buffer->AppendFloat(params_.speed.yawSpeed), nullptr, "append yawSpeed");
    CHECK_ERR_RETURN_VALUE(buffer->AppendFloat(params_.speed.rollSpeed), nullptr, "append rollSpeed");
    CHECK_ERR_RETURN_VALUE(buffer->AppendFloat(params_.speed.pitchSpeed), nullptr, "append pitchSpeed");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(CMD_SET_ROTATE_BY_SPEED_CONTROL), nullptr, "append control byte");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(CMD_SET_ROTATE_BY_SPEED_CONTROL_EXTEND), nullptr, "append extend");

    HILOGW("end.");
    return buffer;
}

void SetMechRotationBySpeedCmd::TriggerResponse(std::shared_ptr<MechDataBuffer> data)
{
    if (data == nullptr || data->Size() < RSP_SIZE + BIT_OFFSET_2) {
        HILOGE("Invalid input data for response");
        return;
    }

    CHECK_ERR_RETURN(data->ReadUint8(BIT_OFFSET_2, result_), "read result_");
    HILOGI("response code: %{public}u", result_);

    uint8_t limitInfo = 0;
    CHECK_ERR_RETURN(data->ReadUint8(BIT_OFFSET_3, limitInfo), "read limit info");
    HILOGI("limitInfo : %{public}u", limitInfo);
 
    uint8_t yawLimit = (limitInfo >> BIT_OFFSET_6) & BIT_3;
    uint8_t rollLimit = (limitInfo >> BIT_OFFSET_4) & BIT_3;
    uint8_t pitchLimit = (limitInfo >> BIT_OFFSET_2) & BIT_3;

    if (yawLimit == 0) {
        status_.yawLimited = RotationAxisLimited::NOT_LIMITED;
    } else if (yawLimit == 1) {
        status_.yawLimited = RotationAxisLimited::POS_LIMITED;
    } else if (yawLimit == NEG_LIMITED) {
        status_.yawLimited = RotationAxisLimited::NEG_LIMITED;
    }

    if (rollLimit == 0) {
        status_.rollLimited = RotationAxisLimited::NOT_LIMITED;
    } else if (rollLimit == 1) {
        status_.rollLimited = RotationAxisLimited::POS_LIMITED;
    } else if (rollLimit == NEG_LIMITED) {
        status_.rollLimited = RotationAxisLimited::NEG_LIMITED;
    }

    if (pitchLimit == 0) {
        status_.pitchLimited = RotationAxisLimited::NOT_LIMITED;
    } else if (pitchLimit == 1) {
        status_.pitchLimited = RotationAxisLimited::POS_LIMITED;
    } else if (pitchLimit == NEG_LIMITED) {
        status_.pitchLimited = RotationAxisLimited::NEG_LIMITED;
    }

    if (responseCb_) {
        HILOGI("trigger response callback.");
        responseCb_();
    }
}

const RotateBySpeedParam& SetMechRotationBySpeedCmd::GetParams() const
{
    return params_;
}

const RotationAxesStatus& SetMechRotationBySpeedCmd::GetRotationAxesStatus() const
{
    return status_;
}

uint8_t SetMechRotationBySpeedCmd::GetResult() const
{
    return result_;
}
} // namespace MechBodyController
} // namespace OHOS
