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

#include "mc_set_mech_hid_preemptive_cmd.h"

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "SetMechHidPreemptiveCmd";
    constexpr uint8_t CMD_SET_HID_PREEMPTIVE_TYPE = 0x04;
    constexpr uint8_t CMD_SET_HID_PREEMPTIVE_LENGTH = 1;
}

SetMechHidPreemptiveCmd::SetMechHidPreemptiveCmd(bool isPreemptive)
{
    cmdSet_ = CMD_SET;
    cmdId_ = CMD_ID;
    reqSize_ = REQ_SIZE;
    rspSize_ = RSP_SIZE;
    needResponse_ = (RSP_SIZE > 0);
    timeoutMs_ = MECHBODY_MSG_TIMEOUT;
    isPreemptive_ = isPreemptive;
    retryTimes_ = CMD_PRIORITY_HIGH;
}

std::shared_ptr<MechDataBuffer> SetMechHidPreemptiveCmd::Marshal() const
{
    HILOGD("start.");
    auto buffer = std::make_shared<MechDataBuffer>(reqSize_ + BIT_OFFSET_2);
    if (buffer == nullptr) {
        HILOGE("Failed to allocate memory for Marshal buffer");
        return nullptr;
    }

    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdSet_), nullptr, "append cmdSet_");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdId_), nullptr, "append cmdId_");

    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(CMD_SET_HID_PREEMPTIVE_TYPE), nullptr, "append control type");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(CMD_SET_HID_PREEMPTIVE_LENGTH), nullptr, "append control len");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(static_cast<uint8_t>(isPreemptive_)), nullptr, "append isPreemptive_");

    HILOGD("end.");
    return buffer;
}

void SetMechHidPreemptiveCmd::TriggerResponse(std::shared_ptr<MechDataBuffer> data)
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

uint8_t SetMechHidPreemptiveCmd::GetResult() const
{
    return result_;
}
} // namespace MechBodyController
} // namespace OHOS
