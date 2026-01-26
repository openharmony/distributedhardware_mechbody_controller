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

#include "mc_command_base.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    constexpr uint16_t OFFSET_8 = 8;
}

void CommandBase::SetResponseCallback(ResponseCallback cb)
{
    responseCb_ = cb;
}

void CommandBase::SetTimeoutCallback(TimeoutCallback cb)
{
    timeoutCb_ = cb;
}

void CommandBase::SetTimeoutResetCallback(TimeoutResetCallback cb)
{
    timeoutResetCb_ = cb;
}

void CommandBase::TriggerTimeout() const
{
    if (timeoutCb_) {
        timeoutCb_();
    }
}

void CommandBase::TriggerTimeoutReset() const
{
    if (timeoutResetCb_) {
        timeoutResetCb_();
    }
}

uint8_t CommandBase::GetCmdSet() const
{
    return cmdSet_;
}

uint8_t CommandBase::GetCmdId() const
{
    return cmdId_;
}

uint16_t CommandBase::GetCmdType() const
{
    return (static_cast<uint16_t>(cmdSet_) << OFFSET_8) | static_cast<uint16_t>(cmdId_);
}

uint16_t CommandBase::GetReqSize() const
{
    return reqSize_;
}

uint16_t CommandBase::GetRspSize() const
{
    return rspSize_;
}

uint32_t CommandBase::GetTimeoutMs() const
{
    return timeoutMs_;
}

bool CommandBase::NeedResponse() const
{
    return needResponse_;
}

int32_t CommandBase::GetRetryTimes() const
{
    return retryTimes_;
}

} // namespace MechBodyController
} // namespace OHOS
