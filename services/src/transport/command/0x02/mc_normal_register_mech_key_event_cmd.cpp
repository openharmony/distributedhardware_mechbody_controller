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

#include "mc_normal_register_mech_key_event_cmd.h"

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "NormalRegisterMechKeyEventCmd";
    constexpr uint8_t SWITCH_MODE = 1;
    constexpr uint8_t SWITCH_TRACKING = 2;
    constexpr uint8_t ZOOM_IN = 3;
    constexpr uint8_t ZOOM_OUT = 4;
    constexpr uint8_t SWITCH_CAMERA = 5;
    constexpr uint8_t START_FILMING = 6;
    constexpr uint8_t CMD_GET_BUTTON_LENGTH = 1;
    constexpr uint8_t CMD_KEY_WHEEL_CLICK = 0x07;
    constexpr uint8_t CMD_GET_WHEEL_LENGTH = 5;
    constexpr uint8_t CMD_KEY_STICK_CLICK = 0x08;
    constexpr uint8_t CMD_GET_STICK_LENGTH = 4;
    constexpr uint8_t dataRedundant = 2;
}

NormalRegisterMechKeyEventCmd::NormalRegisterMechKeyEventCmd()
{
    cmdSet_ = CMD_SET;
    cmdId_ = CMD_ID;
    reqSize_ = REQ_SIZE;
    rspSize_ = RSP_SIZE;
    needResponse_ = (RSP_SIZE > 0);
    timeoutMs_ = MECHBODY_MSG_TIMEOUT;
    retryTimes_ = CMD_PRIORITY_MIDDLE;
}

std::shared_ptr<MechDataBuffer> NormalRegisterMechKeyEventCmd::Marshal() const
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

bool NormalRegisterMechKeyEventCmd::Unmarshal(std::shared_ptr<MechDataBuffer> data)
{
    HILOGD("start.");
    if (data == nullptr || data->Size() < RPT_SIZE + BIT_OFFSET_2) {
        HILOGE("Invalid input data for unmarshal");
        return false;
    }

    size_t offset = BIT_OFFSET_2;
    uint8_t keyType = 0;
    uint8_t resultLength = 0;
    do {
        CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, keyType), false, "read keyType");
        offset++;
        if (keyType >= SWITCH_MODE && keyType <= START_FILMING) {
            CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, resultLength), false, "read buttonLength");
            if (resultLength != CMD_GET_BUTTON_LENGTH) {
                HILOGE("Reply data buttonLength invalid");
                return false;
            }
            offset++;

            CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, buttonFrequency_), false, "read buttonFrequency");
            if (buttonFrequency_ == 1) {
                switch (keyType) {
                    case SWITCH_MODE :
                        break;
                    case SWITCH_TRACKING :
                        HILOGI("ButtonEvent SWITCH_CAMERA.");
                        event_ = CameraKeyEvent::SWITCH_TRACKING;
                        break;
                    case ZOOM_IN :
                        event_ = CameraKeyEvent::ZOOM_IN;
                        break;
                    case ZOOM_OUT :
                        event_ = CameraKeyEvent::ZOOM_OUT;
                        break;
                    case SWITCH_CAMERA :
                        event_ = CameraKeyEvent::SWITCH_CAMERA;
                        break;
                    case START_FILMING :
                        event_ = CameraKeyEvent::START_FILMING;
                        break;
                    default :
                        HILOGW("ButtonEvent undefined action.");
                        break;
                }
            }
            offset++;
            continue;
        }

        if (keyType == CMD_KEY_WHEEL_CLICK) {
            CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, resultLength), false, "read wheelLength");
            if (resultLength != CMD_GET_WHEEL_LENGTH) {
                HILOGE("Reply data wheelLength invalid");
                return false;
            }
            offset++;

            CHECK_ERR_RETURN_VALUE(data->ReadInt16(offset, wheelData_.degree), false, "read degree");
            HILOGD("get degree: %{public}d", wheelData_.degree);
            offset += BIT_OFFSET_2;

            CHECK_ERR_RETURN_VALUE(data->ReadInt16(offset, wheelData_.speed), false, "read speed");
            HILOGD("get speed: %{public}d", wheelData_.speed);
            offset += BIT_OFFSET_2;

            CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, wheelData_.speed_ratio), false, "read speed_ratio");
            HILOGD("get speed_ratio: %{public}u", wheelData_.speed_ratio);
            offset++;
            continue;
        }

        if (keyType == CMD_KEY_STICK_CLICK) {
            CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, resultLength), false, "read stickLength");
            if (resultLength != CMD_GET_STICK_LENGTH) {
                HILOGE("Reply data stickLength invalid");
                return false;
            }
            offset++;

            CHECK_ERR_RETURN_VALUE(data->ReadInt16(offset, stickX_), false, "read stickX 1-2");
            offset += BIT_OFFSET_2;
            CHECK_ERR_RETURN_VALUE(data->ReadInt16(offset, stickY_), false, "read stickY 3-4");
            offset += BIT_OFFSET_2;
            continue;
        }
    } while (offset < data->Size() - dataRedundant);
    return true;
}

void NormalRegisterMechKeyEventCmd::TriggerResponse(std::shared_ptr<MechDataBuffer> data)
{
    HILOGI("start.");
    if (responseCb_) {
        HILOGI("trigger response callback.");
        responseCb_();
    }
    HILOGI("end.");
}

CameraKeyEvent NormalRegisterMechKeyEventCmd::GetEvent() const
{
    return event_;
}

uint8_t NormalRegisterMechKeyEventCmd::GetResult() const
{
    return result_;
}

int16_t NormalRegisterMechKeyEventCmd::GetStickX() const
{
    return stickX_;
}

int16_t NormalRegisterMechKeyEventCmd::GetStickY() const
{
    return stickY_;
}
} // namespace MechBodyController
} // namespace OHOS
