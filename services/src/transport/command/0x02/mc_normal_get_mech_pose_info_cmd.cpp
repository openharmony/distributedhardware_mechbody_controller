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

#include "mc_normal_get_mech_pose_info_cmd.h"

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "NormalGetMechPoseInfoCmd";
}

NormalGetMechPoseInfoCmd::NormalGetMechPoseInfoCmd()
{
    cmdSet_ = CMD_SET;
    cmdId_ = CMD_ID;
    reqSize_ = REQ_SIZE;
    rspSize_ = RSP_SIZE;
    needResponse_ = (RSP_SIZE > 0);
    timeoutMs_ = MECHBODY_MSG_TIMEOUT;
    retryTimes_ = CMD_PRIORITY_MIDDLE;
}

std::shared_ptr<MechDataBuffer> NormalGetMechPoseInfoCmd::Marshal() const
{
    HILOGI("start.");
    auto buffer = std::make_shared<MechDataBuffer>(reqSize_ + BIT_OFFSET_2);
    if (buffer == nullptr) {
        HILOGE("Failed to allocate memory for Marshal buffer");
        return nullptr;
    }

    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdSet_), nullptr, "append cmdSet_");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdId_), nullptr, "append cmdId_");

    HILOGI("end.");
    return buffer;
}

void NormalGetMechPoseInfoCmd::TriggerResponse(std::shared_ptr<MechDataBuffer> data)
{
    if (data == nullptr || data->Size() < RSP_SIZE + BIT_OFFSET_2) {
        HILOGE("Invalid input data for response");
        return;
    }

    size_t offset = BIT_OFFSET_2;

    CHECK_ERR_RETURN(data->ReadUint8(offset, result_), "read result_");
    HILOGI("response code: %{public}u", result_);
    offset++;

    CHECK_ERR_RETURN(data->ReadUint16(offset, params_.accelerationX), "read accelerationX");
    offset += BIT_OFFSET_2;

    CHECK_ERR_RETURN(data->ReadUint16(offset, params_.accelerationY), "read accelerationY");
    offset += BIT_OFFSET_2;

    CHECK_ERR_RETURN(data->ReadUint16(offset, params_.accelerationZ), "read accelerationZ");
    offset += BIT_OFFSET_2;

    if (responseCb_) {
        HILOGI("trigger response callback.");
        responseCb_();
    }
}

DeviceAccelerationInfo NormalGetMechPoseInfoCmd::GetParams() const
{
    return params_;
}

uint8_t NormalGetMechPoseInfoCmd::GetResult() const
{
    return result_;
}
} // namespace MechBodyController
} // namespace OHOS
