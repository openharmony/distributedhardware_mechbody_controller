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

#ifndef MC_COMMAND_BASE_H
#define MC_COMMAND_BASE_H

#include "mc_data_buffer.h"
#include <vector>
#include <string>
#include <cstdint>

namespace OHOS {
namespace MechBodyController {
namespace {
    constexpr int32_t MECHBODY_MSG_TIMEOUT = 10000;
}
using ResponseCallback = std::function<void()>;
using TimeoutCallback = std::function<void()>;

class CommandBase {
public:
    virtual ~CommandBase() = default;

    virtual std::shared_ptr<MechDataBuffer> Marshal() const = 0;
    bool Unmarshal(std::shared_ptr<MechDataBuffer> data);

    void SetResponseCallback(ResponseCallback cb);
    void SetTimeoutCallback(TimeoutCallback cb);
    virtual void TriggerResponse(std::shared_ptr<MechDataBuffer> data) = 0;
    void TriggerTimeout() const;

    uint8_t GetCmdSet() const;
    uint8_t GetCmdId() const;
    uint16_t GetCmdType() const;
    uint16_t GetReqSize() const;
    uint16_t GetRspSize() const;
    uint32_t GetTimeoutMs() const;
    int32_t GetRetryTimes() const;

    bool NeedResponse() const;

protected:
    uint8_t cmdSet_ = 0;
    uint8_t cmdId_ = 0;
    uint16_t reqSize_ = 0;
    uint16_t rspSize_ = 0;
    bool needResponse_ = 0;
    int32_t retryTimes_ = 0;
    uint32_t timeoutMs_ = MECHBODY_MSG_TIMEOUT;
    ResponseCallback responseCb_;
    TimeoutCallback timeoutCb_;
};
} // namespace MechBodyController
} // namespace OHOS

#endif // MC_COMMAND_BASE_H
