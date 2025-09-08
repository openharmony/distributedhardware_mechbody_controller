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

#include "mc_set_mech_camera_info_cmd.h"

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "SetMechCameraInfoCmd";
}

SetMechCameraInfoCmd::SetMechCameraInfoCmd(const CameraInfoParams& params)
    : params_(params)
{
    cmdSet_ = CMD_SET;
    cmdId_ = CMD_ID;
    reqSize_ = REQ_SIZE;
    rspSize_ = RSP_SIZE;
    needResponse_ = (RSP_SIZE > 0);
    timeoutMs_ = MECHBODY_MSG_TIMEOUT;
    retryTimes_ = CMD_PRIORITY_HIGH;
}

std::shared_ptr<MechDataBuffer> SetMechCameraInfoCmd::Marshal() const
{
    HILOGI("start.");
    auto buffer = std::make_shared<MechDataBuffer>(reqSize_ + BIT_OFFSET_2);
    if (buffer == nullptr) {
        HILOGE("Failed to allocate memory for Marshal buffer");
        return nullptr;
    }

    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdSet_), nullptr, "append cmdSet_");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdId_), nullptr, "append cmdId_");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(params_.fovH), nullptr, "append fovH");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(params_.fovV), nullptr, "append fovV");
    CHECK_ERR_RETURN_VALUE(buffer->AppendFloat(params_.zoomFactor), nullptr, "append zoomFactor");

    uint8_t controlByte = 0;
    controlByte |= params_.isRecording << BIT_5;
    controlByte |= (params_.cameraType != CameraType::BACK);
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(controlByte), nullptr, "append controlByte");

    HILOGI("end. fovH: %{public}d; fovV: %{public}d; zoomFactor: %{public}f",
        params_.fovH, params_.fovV, params_.zoomFactor);
    return buffer;
}

void SetMechCameraInfoCmd::TriggerResponse(std::shared_ptr<MechDataBuffer> data)
{
    if (responseCb_) {
        HILOGI("trigger response callback.");
        responseCb_();
    }
}

const CameraInfoParams& SetMechCameraInfoCmd::GetParams() const
{
    return params_;
}
} // namespace MechBodyController
} // namespace OHOS
