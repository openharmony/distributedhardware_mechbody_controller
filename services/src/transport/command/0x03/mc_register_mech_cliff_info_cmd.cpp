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

#include "mc_register_mech_cliff_info_cmd.h"

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "RegisterMechCliffInfoCmd";
}

RegisterMechCliffInfoCmd::RegisterMechCliffInfoCmd()
{
    cmdSet_ = CMD_SET;
    cmdId_ = CMD_ID;
    reqSize_ = REQ_SIZE;
    rspSize_ = RSP_SIZE;
    needResponse_ = (RSP_SIZE > 0);
    timeoutMs_ = MECHBODY_MSG_TIMEOUT;
    retryTimes_ = CMD_PRIORITY_MIDDLE;
}

std::shared_ptr<MechDataBuffer> RegisterMechCliffInfoCmd::Marshal() const
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

bool RegisterMechCliffInfoCmd::Unmarshal(std::shared_ptr<MechDataBuffer> data)
{
    HILOGD("start.");
    if (data == nullptr || data->Size() < RPT_SIZE + BIT_OFFSET_2) {
        HILOGE("Invalid input data for unmarshal");
        return false;
    }

    size_t offset = BIT_OFFSET_2;

    // 获取悬崖信息数量
    CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, cliffNums_), false, "read cliff num");
    offset++;
    // 没有悬崖信息的情况
    if (cliffNums_ == 0) {
        HILOGI("Cliff num is 0");
        return true;
    }

    for (int i = 1; i<= cliffNums_; i++) {
        CliffInfo cliffInfo;
        // 解析悬崖方位
        CHECK_ERR_RETURN_VALUE(data->ReadInt16(offset, cliffInfo.cliffDirection), false, "read cliff direction");
        offset += BIT_OFFSET_2;

        // 解析悬崖额外信息长度，当前不会上报，长度默认0
        CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, cliffInfo.cliffInfoLen), false, "read cliff info len");
        if (cliffInfo.cliffInfoLen != BIT_OFFSET_0) {
            HILOGE("cliff info len is invalid");
            return false;
        }
        offset++;

        cliffInfos_.push_back(cliffInfo);
    }
    return true;
}

void RegisterMechCliffInfoCmd::TriggerResponse(std::shared_ptr<MechDataBuffer> data)
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

const std::vector<CliffInfo> RegisterMechCliffInfoCmd::GetCliffs() const
{
    return cliffInfos_;
}
uint8_t RegisterMechCliffInfoCmd::GetCliffNums() const
{
    return cliffNums_;
}

uint8_t RegisterMechCliffInfoCmd::GetResult() const
{
    return result_;
}
} // namespace MechBodyController
} // namespace OHOS
