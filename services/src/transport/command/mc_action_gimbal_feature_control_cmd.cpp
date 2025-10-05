/*

Copyright (c) 2025 Huawei Device Co., Ltd.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "mc_action_gimbal_feature_control_cmd.h"

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "ActionGimbalFeatureControlCmd";
    // constexpr uint8_t CMD_CAMERA_TYPE_DEFAULT = 0x02;
}

ActionGimbalFeatureControlCmd::ActionGimbalFeatureControlCmd(const ActionControlParams &params)
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

std::shared_ptr ActionGimbalFeatureControlCmd::Marshal() const
{
    HILOGI("start.");
    auto buffer = std::make_shared(reqSize_ + BIT_OFFSET_2);
    if (buffer == nullptr)
    {
        HILOGE("Failed to allocate memory for Marshal buffer");
        return nullptr;
    }

    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdSet_), nullptr, "append cmdSet_");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdId_), nullptr, "append cmdId_");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(params_.controlReq), nullptr, "append controlReq");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint16(params_.timeOut), nullptr, "append timeOut");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint32(0), nullptr, "append reserved");
    uint32_t joystick = 0;
    joystick |= (params_.yawControl & 0xFF) << 14;
    joystick |= (params_.pitchControl & 0xFF) << 16;
    joystick |= (params_.rollControl & 0xFF) << 18;
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint32(joystick), nullptr, "append timeDelay");

    HILOGI("end.");
    return buffer;
}

void ActionGimbalFeatureControlCmd::TriggerResponse(std::shared_ptr data)
{
    if (responseCb_)
    {
        HILOGI("trigger response callback.");
        responseCb_();
    }
}

const ActionControlParams& ActionGimbalFeatureControlCmd::GetParams() const
{
    return params_;
}
} // namespace MechBodyController
} // namespace OHOS