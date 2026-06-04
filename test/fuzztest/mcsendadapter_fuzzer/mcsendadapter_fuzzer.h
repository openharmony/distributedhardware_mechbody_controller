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

#ifndef MC_SEND_ADAPTER_FUZZER_H
#define MC_SEND_ADAPTER_FUZZER_H

#include <memory>
#include <cstdint>

#include "mc_send_adapter.h"
#include "mc_command_base.h"
#include "mc_data_buffer.h"

namespace OHOS {
namespace MechBodyController {

class MockCommand : public CommandBase {
public:
    MockCommand() = default;
    ~MockCommand() override = default;

    std::shared_ptr<MechDataBuffer> Marshal() const override
    {
        auto buffer = std::make_shared<MechDataBuffer>(32);
        return buffer;
    }

    void TriggerResponse(std::shared_ptr<MechDataBuffer> data) override
    {
        responseTriggered_ = true;
    }

    void SetCmdSet(uint8_t cmdSet)
    {
        cmdSet_ = cmdSet;
    }

    void SetCmdId(uint8_t cmdId)
    {
        cmdId_ = cmdId;
    }

    void SetReqSize(uint16_t reqSize)
    {
        reqSize_ = reqSize;
    }

    void SetRspSize(uint16_t rspSize)
    {
        rspSize_ = rspSize;
    }

    void SetNeedResponse(bool need)
    {
        needResponse_ = need;
    }

    void SetRetryTimes(int32_t times)
    {
        retryTimes_ = times;
    }

    void SetTimeoutMs(uint32_t timeout)
    {
        timeoutMs_ = timeout;
    }

private:
    bool responseTriggered_ = false;
};

} // namespace MechBodyController
} // namespace OHOS

#endif // MC_SEND_ADAPTER_FUZZER_H