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

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>
#include <securec.h>
#include <cstdint>

#include "mccommandbase_fuzzer.h"
#include "mc_command_base.h"
#include "mc_data_buffer.h"
#include "mechbody_controller_log.h"

using namespace OHOS::MechBodyController;

namespace {
    const std::string TAG = "McCommandBaseFuzzTest";

    enum class TestFunctionId {
        FUZZ_COMMAND_BASE_GETTERS = 0,
        FUZZ_COMMAND_BASE_CALLBACKS = 1,
        FUZZ_COMMAND_BASE_TIMESTAMP = 2
    };
}

class MockCommandBase : public CommandBase {
public:
    MockCommandBase() = default;
    ~MockCommandBase() override = default;

    std::shared_ptr<MechDataBuffer> Marshal() const override
    {
        return nullptr;
    }

    void TriggerResponse(std::shared_ptr<MechDataBuffer> data) override {}

    void SetCmdSet(uint8_t cmdSet) { cmdSet_ = cmdSet; }
    void SetCmdId(uint8_t cmdId) { cmdId_ = cmdId; }
    void SetReqSize(uint16_t reqSize) { reqSize_ = reqSize; }
    void SetRspSize(uint16_t rspSize) { rspSize_ = rspSize; }
    void SetNeedResponse(bool needResponse) { needResponse_ = needResponse; }
    void SetTimeoutMs(uint32_t timeoutMs) { timeoutMs_ = timeoutMs; }
    void SetRetryTimes(int32_t retryTimes) { retryTimes_ = retryTimes; }
};

static void TestCommandBaseGetters(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    MockCommandBase cmd;
    cmd.SetCmdSet(provider.ConsumeIntegral<uint8_t>());
    cmd.SetCmdId(provider.ConsumeIntegral<uint8_t>());
    cmd.SetReqSize(provider.ConsumeIntegral<uint16_t>());
    cmd.SetRspSize(provider.ConsumeIntegral<uint16_t>());
    cmd.SetNeedResponse(provider.ConsumeBool());
    cmd.SetTimeoutMs(provider.ConsumeIntegral<uint32_t>());
    cmd.SetRetryTimes(provider.ConsumeIntegral<int32_t>());

    cmd.GetCmdSet();
    cmd.GetCmdId();
    cmd.GetCmdType();
    cmd.GetReqSize();
    cmd.GetRspSize();
    cmd.GetTimeoutMs();
    cmd.NeedResponse();
    cmd.GetRetryTimes();
}

static void TestCommandBaseCallbacks(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    MockCommandBase cmd;

    bool shouldSetCallbacks = provider.ConsumeBool();
    if (shouldSetCallbacks) {
        cmd.SetResponseCallback([]() {});
        cmd.SetTimeoutCallback([]() {});
        cmd.SetTimeoutResetCallback([]() {});
    }

    cmd.TriggerTimeout();
    cmd.TriggerTimeoutReset();
}

static void TestCommandBaseTimestamp(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    MockCommandBase cmd;
    int64_t timestamp = provider.ConsumeIntegral<int64_t>();
    cmd.SetTimestamp(timestamp);
    cmd.GetTimestamp();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < 1) {
        return 0;
    }

    FuzzedDataProvider provider(data, size);
    uint8_t testType = provider.ConsumeIntegral<uint8_t>() % 3;

    switch (static_cast<TestFunctionId>(testType)) {
        case TestFunctionId::FUZZ_COMMAND_BASE_GETTERS:
            TestCommandBaseGetters(data, size);
            break;
        case TestFunctionId::FUZZ_COMMAND_BASE_CALLBACKS:
            TestCommandBaseCallbacks(data, size);
            break;
        case TestFunctionId::FUZZ_COMMAND_BASE_TIMESTAMP:
            TestCommandBaseTimestamp(data, size);
            break;
        default:
            break;
    }

    return 0;
}