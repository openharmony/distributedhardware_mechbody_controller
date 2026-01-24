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

#include "mc_normal_register_mech_generic_event_cmd.h"

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "NormalRegisterMechGenericEventCmd";
    static constexpr uint8_t CMD_GET_MECH_ATTACH_TYPE = 0x00;
    constexpr uint8_t CMD_GET_MECH_ATTACH_REPLY_LENGTH = 1;
    static constexpr uint8_t CMD_GET_MECH_AXIS_TYPE = 0x01;
    constexpr uint8_t CMD_GET_MECH_AXIS_REPLY_LENGTH = 1;
    static constexpr uint8_t CMD_GET_MECH_POWER_TYPE = 0x02;
    constexpr uint8_t CMD_GET_MECH_POWER_REPLY_LENGTH = 1;
    constexpr uint8_t dataRedundant = 2;
}

NormalRegisterMechGenericEventCmd::NormalRegisterMechGenericEventCmd()
{
    cmdSet_ = CMD_SET;
    cmdId_ = CMD_ID;
    reqSize_ = REQ_SIZE;
    rspSize_ = RSP_SIZE;
    needResponse_ = (RSP_SIZE > 0);
    timeoutMs_ = MECHBODY_MSG_TIMEOUT;
    retryTimes_ = CMD_PRIORITY_MIDDLE;
}

std::shared_ptr<MechDataBuffer> NormalRegisterMechGenericEventCmd::Marshal() const
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

bool NormalRegisterMechGenericEventCmd::Unmarshal(std::shared_ptr<MechDataBuffer> data)
{
    HILOGD("start.");
    if (data == nullptr || data->Size() < RPT_SIZE + BIT_OFFSET_2) {
        HILOGE("Invalid input data for unmarshal");
        return false;
    }

    size_t offset = BIT_OFFSET_2;
    uint8_t resultLength = 0;
    uint8_t bitResult;

    do {
        uint8_t resultType = 0;
        CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, resultType), false, "read attach resultType");
        offset++;
        if (resultType == CMD_GET_MECH_ATTACH_TYPE) {
            CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, resultLength), false, "read attach resultLength");
            if (resultLength != CMD_GET_MECH_ATTACH_REPLY_LENGTH) {
                HILOGE("Reply data attach resultLength invalid");
                return false;
            }
            offset++;

            CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, params_.attached), false, "read isAttached");
            offset++;
        }

        if (resultType == CMD_GET_MECH_AXIS_TYPE) {
            CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, resultLength), false, "read axis resultLength");
            if (resultLength != CMD_GET_MECH_AXIS_REPLY_LENGTH) {
                HILOGE("Reply data axis resultLength invalid");
                return false;
            }
            offset++;

            CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, bitResult), false, "read bitResult");
            params_.yawDisable = (bitResult >> 0) & 1;
            params_.rollDisable = (bitResult >> 1) & 1;
            params_.pitchDisable = (bitResult >> 2) & 1;

            offset++;
        }

        if (resultType == CMD_GET_MECH_POWER_TYPE) {
            CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, resultLength), false, "read power resultLength");
            if (resultLength != CMD_GET_MECH_POWER_REPLY_LENGTH) {
                HILOGE("Reply data power resultLength invalid");
                return false;
            }
            offset++;

            CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, bitResult), false, "read bitResult");
            lowPower = (bitResult >> 0) & 1;
            isCharging = (bitResult >> 1) & 1;
            offset++;
        }
    } while (offset < data->Size() - dataRedundant);
    return true;
}

void NormalRegisterMechGenericEventCmd::TriggerResponse(std::shared_ptr<MechDataBuffer> data)
{
    HILOGI("start.");
    if (data == nullptr || data->Size() < RSP_SIZE) {
        HILOGE("Invalid input data for response");
        return;
    }

    CHECK_ERR_RETURN(data->ReadUint8(0, result_), "read result_");
    HILOGI("response code: %{public}u", result_);

    if (responseCb_) {
        HILOGI("trigger response callback.");
        responseCb_();
    }
    HILOGI("end.");
}

DeviceStateInfo NormalRegisterMechGenericEventCmd::GetParams() const
{
    return params_;
}

uint8_t NormalRegisterMechGenericEventCmd::GetResult() const
{
    return result_;
}
} // namespace MechBodyController
} // namespace OHOS
