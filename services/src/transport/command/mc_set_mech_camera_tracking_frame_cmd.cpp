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

#include "mc_set_mech_camera_tracking_frame_cmd.h"

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "SetMechCameraTrackingFrameCmd";
    constexpr uint8_t CMD_CAMERA_TYPE_DEFAULT = 0x02;
}

SetMechCameraTrackingFrameCmd::SetMechCameraTrackingFrameCmd(const TrackingFrameParams& params)
    : params_(params)
{
    cmdSet_ = CMD_SET;
    cmdId_ = CMD_ID;
    reqSize_ = REQ_SIZE;
    rspSize_ = RSP_SIZE;
    needResponse_ = (RSP_SIZE > 0);
    timeoutMs_ = MECHBODY_MSG_TIMEOUT;
    retryTimes_ = CMD_PRIORITY_LOW;
}

std::shared_ptr<MechDataBuffer> SetMechCameraTrackingFrameCmd::Marshal() const
{
    HILOGI("start.");
    auto buffer = std::make_shared<MechDataBuffer>(reqSize_ + BIT_OFFSET_2);
    if (buffer == nullptr) {
        HILOGE("Failed to allocate memory for Marshal buffer");
        return nullptr;
    }

    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdSet_), nullptr, "append cmdSet_");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdId_), nullptr, "append cmdId_");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint64(params_.timeStamp), nullptr, "append timeStamp");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(CMD_CAMERA_TYPE_DEFAULT), nullptr, "append cameraType");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(static_cast<uint8_t>(params_.confidence)), nullptr, "append confidence");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(params_.objectType), nullptr, "append objectType");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint16(params_.targetId), nullptr, "append targetId");
    CHECK_ERR_RETURN_VALUE(buffer->AppendFloat(params_.roi.x), nullptr, "append x");
    CHECK_ERR_RETURN_VALUE(buffer->AppendFloat(params_.roi.y), nullptr, "append y");
    CHECK_ERR_RETURN_VALUE(buffer->AppendFloat(params_.roi.width), nullptr, "append width");
    CHECK_ERR_RETURN_VALUE(buffer->AppendFloat(params_.roi.height), nullptr, "append height");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint16(params_.fovV), nullptr, "append fovV");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint16(params_.fovH), nullptr, "append fovH");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(params_.isRecording), nullptr, "append isRecording");
    CHECK_ERR_RETURN_VALUE(buffer->AppendUint32(params_.timeDelay), nullptr, "append timeDelay");

    HILOGI("end.");
    return buffer;
}

void SetMechCameraTrackingFrameCmd::TriggerResponse(std::shared_ptr<MechDataBuffer> data)
{
    if (responseCb_) {
        HILOGI("trigger response callback.");
        responseCb_();
    }
}

const TrackingFrameParams& SetMechCameraTrackingFrameCmd::GetParams() const
{
    return params_;
}
} // namespace MechBodyController
} // namespace OHOS
