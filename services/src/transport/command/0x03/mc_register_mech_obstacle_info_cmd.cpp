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

#include "mc_register_mech_obstacle_info_cmd.h"

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "RegisterMechObstacleInfoCmd";
}

RegisterMechObstacleInfoCmd::RegisterMechObstacleInfoCmd()
{
    cmdSet_ = CMD_SET;
    cmdId_ = CMD_ID;
    reqSize_ = REQ_SIZE;
    rspSize_ = RSP_SIZE;
    needResponse_ = (RSP_SIZE > 0);
    timeoutMs_ = MECHBODY_MSG_TIMEOUT;
    retryTimes_ = CMD_PRIORITY_MIDDLE;
}

std::shared_ptr<MechDataBuffer> RegisterMechObstacleInfoCmd::Marshal() const
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

bool RegisterMechObstacleInfoCmd::Unmarshal(std::shared_ptr<MechDataBuffer> data)
{
    HILOGI("start.");
    if (data == nullptr || data->Size() < RPT_SIZE + BIT_OFFSET_2) {
        HILOGE("Invalid input data for unmarshal");
        return false;
    }

    size_t offset = BIT_OFFSET_2;

    // 获取障碍物数量
    CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, obstacleNums_), false, "read obstacle num");
    offset++;

    // 没有障碍物的情况
    if (obstacleNums_ == BIT_0) {
        HILOGI("obstacle num is 0");
        return true;
    }

        // 按照读取到的悬崖数量信息，重新校验最小长度
    if (data->Size() < offset + obstacleNums_* BIT_OFFSET_5) {
        HILOGE("Invalid input data for unmarshal");
        return false;
    }

    for (int i = 1; i <= obstacleNums_; i++) {
        ObstacleInfo obstacleInfo;
        // 解析障碍物水平方位
        CHECK_ERR_RETURN_VALUE(data->ReadInt16(offset, obstacleInfo.direction), false, "read horizontal direction");
        offset += BIT_OFFSET_2;

        // 解析障碍物俯仰角信息
        CHECK_ERR_RETURN_VALUE(data->ReadInt16(offset, obstacleInfo.pitchAngle), false, "read obstacle pitch angle");
        offset += BIT_OFFSET_2;

        // 解析障碍物额外信息长度,当前不上报额外信息，长度按0处理
        CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, obstacleInfo.detailLenth), false, "read obstacle detail lenth");
        if (obstacleInfo.detailLenth == BIT_OFFSET_0) {
            offset++;
            obstacleInfos_.push_back(obstacleInfo);
            continue;
        }
        HILOGW("obstacle info len is not zero, obstacle len %{public}d", obstacleInfo.detailLenth);
        // 重新校验长度
        if (obstacleInfo.detailLenth > data->Size() - offset) {
            HILOGE("obstacle info len is invalid");
            return false;
        }
        offset++;

         // 解析额外信息的值
        obstacleInfo.details = GetObstacleDetails(data, offset, obstacleInfo.detailLenth);
        offset += obstacleInfo.detailLenth;
        obstacleInfos_.push_back(obstacleInfo);
    }
    return true;
}

std::vector<ExtraDetail> RegisterMechObstacleInfoCmd::GetObstacleDetails(
    std::shared_ptr<MechDataBuffer> data, size_t offset, uint8_t detailLen)
{
    std::vector<ExtraDetail> details;
    HILOGD("start.");
    
    size_t endOffset = offset + detailLen;
    
    do {
        ExtraDetail detail;
        CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, detail.detailType), details, "read cliff extra info type");
        offset++;
        
        CHECK_ERR_RETURN_VALUE(data->ReadUint8(offset, detail.dataLen), details, "read cliff extra info data len");
        offset++;
        offset += detail.dataLen;
        details.push_back(detail);
    } while (offset < endOffset);
    return details;
}

void RegisterMechObstacleInfoCmd::TriggerResponse(std::shared_ptr<MechDataBuffer> data)
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

const std::vector<ObstacleInfo> RegisterMechObstacleInfoCmd::GetObstacles() const
{
    return obstacleInfos_;
}
uint8_t RegisterMechObstacleInfoCmd::GetObstacleNums() const
{
    return obstacleNums_;
}

uint8_t RegisterMechObstacleInfoCmd::GetResult() const
{
    return result_;
}
} // namespace MechBodyController
} // namespace OHOS
