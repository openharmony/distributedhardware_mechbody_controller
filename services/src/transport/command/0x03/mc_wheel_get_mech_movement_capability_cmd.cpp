/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "mc_wheel_get_mech_movement_capability_cmd.h"

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "WheelGetMechMovementCapabilityCmd";
    constexpr uint32_t HEAD_UP = 3;
    constexpr uint32_t HEAD_DOWN = 7;
    constexpr uint32_t NOD = 9;
    constexpr uint32_t HEAD_SHAKE = 10;
}

WheelGetMechMovementCapabilityCmd::WheelGetMechMovementCapabilityCmd()
{
    cmdSet_ = CMD_SET;
    cmdId_ = CMD_ID;
    reqSize_ = REQ_SIZE;
    rspSize_ = RSP_SIZE;
    needResponse_ = (RSP_SIZE > 0);
    timeoutMs_ = MECHBODY_MSG_TIMEOUT;
    retryTimes_ = CMD_PRIORITY_MIDDLE;
}

std::shared_ptr<MechDataBuffer> WheelGetMechMovementCapabilityCmd::Marshal() const
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

void WheelGetMechMovementCapabilityCmd::SetCapability(uint32_t cap)
{
    HILOGI("start");
    uint32_t index = cap / 32;
    uint32_t bit_index = cap % 32;
    g_capabilityInfo[index] = g_capabilityInfo[index] | (1 << bit_index);
}

void WheelGetMechMovementCapabilityCmd::SetCapabilityBitValue(uint32_t start, uint8_t value, uint8_t index)
{
    HILOGI("start");
    uint8_t bit_index = 0;
    while (bit_index < BIT_OFFSET_8) {
        if (value & (1 << bit_index)) {
            uint32_t cap = start + index * 8 + bit_index;
            SetCapability(cap);
        }
        bit_index ++;
    }
}

void WheelGetMechMovementCapabilityCmd::OnDataReceive(uint8_t *data)
{
    HILOGI("start");
    explicit_bzero(g_capabilityInfo, sizeof(g_capabilityInfo));
    uint8_t segNum = *(uint8_t *)data;
    constexpr uint8_t lenMax = 32;
    data ++;
    if (segNum > lenMax) {
        HILOGE("Invalid segNum: %{public}u", segNum);
        return;
    }
    for (uint8_t i = 0; i < segNum; i++) {
        uint32_t start = *(uint32_t *)data;
        data += sizeof(uint32_t);
        uint8_t len = *(uint8_t *)data;
        data ++;
        if (len > lenMax) {
            HILOGE("Invalid len: %{public}u", len);
            return;
        }
        for (uint8_t j = 0; j < len; j++) {
            uint8_t temp = *data;
            data++;
            SetCapabilityBitValue(start, temp, j);
        }
    }
    SetCapability(HEAD_UP);
    SetCapability(HEAD_DOWN);
    SetCapability(NOD);
    SetCapability(HEAD_SHAKE);
}

void WheelGetMechMovementCapabilityCmd::TriggerResponse(std::shared_ptr<MechDataBuffer> data)
{
    HILOGI("start.");
    if (data == nullptr || data->Size() < RSP_SIZE + BIT_OFFSET_2) {
        HILOGE("Invalid input data for response");
        return;
    }
    size_t offset = BIT_OFFSET_2;
    CHECK_ERR_RETURN(data->ReadUint8(offset, result_), "read result_");
    HILOGI("response code: %{public}u", result_);
    offset++;
    OnDataReceive(data->Data() + offset);
    if (responseCb_) {
        HILOGI("trigger response callback.");
        responseCb_();
    }
    HILOGI("end.");
}

uint8_t WheelGetMechMovementCapabilityCmd::GetResult() const
{
    return result_;
}

} // namespace MechBodyController
} // namespace OHOS
