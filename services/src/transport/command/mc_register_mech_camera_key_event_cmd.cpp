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

#include "mc_register_mech_camera_key_event_cmd.h"

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "RegisterMechCameraKeyEventCmd";
    constexpr uint8_t CMD_KEY_STATE_SINGLE_CLICK = 0x01;
    constexpr uint8_t CMD_KEY_STATE_TRIPLE_CLICK = 0x03;
    constexpr uint8_t CMD_3_BIT = 0b111;
}

RegisterMechCameraKeyEventCmd::RegisterMechCameraKeyEventCmd()
{
    cmdSet_ = CMD_SET;
    cmdId_ = CMD_ID;
    reqSize_ = REQ_SIZE;
    rspSize_ = RSP_SIZE;
    needResponse_ = (RSP_SIZE > 0);
    timeoutMs_ = MECHBODY_MSG_TIMEOUT;
    retryTimes_ = CMD_PRIORITY_MIDDLE;
}

std::shared_ptr<MechDataBuffer> RegisterMechCameraKeyEventCmd::Marshal() const
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

bool RegisterMechCameraKeyEventCmd::Unmarshal(std::shared_ptr<MechDataBuffer> data)
{
    HILOGD("start.");
    if (data == nullptr || data->Size() < RPT_SIZE + BIT_OFFSET_2) {
        HILOGE("Invalid input data for unmarshal");
        return false;
    }

    size_t offset = BIT_OFFSET_4 + BIT_OFFSET_2;
    uint8_t status = 0;
    uint16_t controlByte = 0;
    do {
        CHECK_ERR_RETURN_VALUE(data->ReadUint16(offset, controlByte), false, "read controlByte 5-6");
        status = (controlByte >> BIT_OFFSET_13) & CMD_3_BIT;
        if (status == CMD_KEY_STATE_SINGLE_CLICK) {
            event_ = CameraKeyEvent::SWITCH_CAMERA;
            break;
        }
        if (status == CMD_KEY_STATE_TRIPLE_CLICK) {
            event_ = CameraKeyEvent::SWITCH_PHOTO_FILM;
            break;
        }

        status = (controlByte >> BIT_OFFSET_10) & CMD_3_BIT;
        if (status == CMD_KEY_STATE_SINGLE_CLICK) {
            event_ = CameraKeyEvent::ZOOM_OUT;
            break;
        }

        status = (controlByte >> BIT_OFFSET_7) & CMD_3_BIT;
        if (status == CMD_KEY_STATE_SINGLE_CLICK) {
            event_ = CameraKeyEvent::ZOOM_IN;
            break;
        }

        status = (controlByte >> BIT_OFFSET_4) & CMD_3_BIT;
        if (status == CMD_KEY_STATE_SINGLE_CLICK) {
            // trigger button, implaced by tracking notify
            break;
        }

        offset += BIT_OFFSET_2;
        CHECK_ERR_RETURN_VALUE(data->ReadUint16(offset, controlByte), false, "read controlByte 7-8");
        status = (controlByte >> BIT_OFFSET_3) & CMD_3_BIT;
        if (status == CMD_KEY_STATE_SINGLE_CLICK) {
            event_ = CameraKeyEvent::START_FILMING;
        }
        break;
    } while (true);

    HILOGD("get event %{public}d", event_);
    return true;
}

void RegisterMechCameraKeyEventCmd::TriggerResponse(std::shared_ptr<MechDataBuffer> data)
{
    HILOGI("start.");
    if (responseCb_) {
        HILOGI("trigger response callback.");
        responseCb_();
    }
    HILOGI("end.");
}

CameraKeyEvent RegisterMechCameraKeyEventCmd::GetEvent() const
{
    return event_;
}

uint8_t RegisterMechCameraKeyEventCmd::GetResult() const
{
    return result_;
}
} // namespace MechBodyController
} // namespace OHOS
