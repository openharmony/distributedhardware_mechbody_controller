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

#include "mcsubscriptioncenter_fuzzer.h"
#include "mc_subscription_center.h"
#include "mc_event_listener.h"
#include "mc_command_base.h"
#include "mc_data_buffer.h"
#include "mechbody_controller_log.h"

using namespace OHOS::MechBodyController;

namespace {
    const std::string TAG = "McSubscriptionCenterFuzzTest";
    const uint8_t TEST_FUNCTION_COUNT = 5;
    const size_t MIN_INPUT_SIZE = 1;
    const uint32_t MIN_LISTENER_COUNT = 1;
    const uint32_t MAX_LISTENER_COUNT = 10;
    const uint32_t MIN_INDEX = 0;

    enum class TestFunctionId {
        FUZZ_SUBSCRIBE = 0,
        FUZZ_UNSUBSCRIBE = 1,
        FUZZ_NOTIFY = 2,
        FUZZ_SUBSCRIBE_UNSUBSCRIBE_CYCLE = 3,
        FUZZ_MULTIPLE_LISTENERS = 4
    };
}

class MockMechEventListener : public IMechEventListener {
public:
    MockMechEventListener() = default;
    ~MockMechEventListener() override = default;

    void MechAttitudeNotify(const std::shared_ptr<CommonRegisterMechPositionInfoCmd> &cmd) override {}
    void MechButtonEventNotify(const std::shared_ptr<CommonRegisterMechKeyEventCmd> &cmd) override {}
    void MechParamNotify(const std::shared_ptr<CommonRegisterMechStateInfoCmd> &cmd) override {}
    void MechGenericEventNotify(const std::shared_ptr<NormalRegisterMechGenericEventCmd>& cmd) override {}
    void MechCliffInfoNotify(const std::shared_ptr<RegisterMechCliffInfoCmd>& cmd) override {}
    void MechObstacleInfoNotify(const std::shared_ptr<RegisterMechObstacleInfoCmd>& cmd) override {}
    void MechExecutionResultNotify(const std::shared_ptr<RegisterMechControlResultCmd> &cmd) override {}
    void MechWheelZoomNotify(const std::shared_ptr<RegisterMechWheelDataCmd> &cmd) override {}
    void MechTrackingStatusNotify(const std::shared_ptr<CommonRegisterMechTrackingEnableCmd> &cmd) override {}
};

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
};

static void TestSubscribe(FuzzedDataProvider &provider)
{
    SubscriptionCenter &subscriptionCenter = SubscriptionCenter::GetInstance();

    bool shouldBeNull = provider.ConsumeBool();
    std::shared_ptr<IMechEventListener> listener = nullptr;

    if (!shouldBeNull) {
        listener = std::make_shared<MockMechEventListener>();
    }

    uint16_t type = provider.ConsumeIntegral<uint16_t>();
    subscriptionCenter.Subscribe(type, listener);
}

static void TestUnSubscribe(FuzzedDataProvider &provider)
{
    SubscriptionCenter &subscriptionCenter = SubscriptionCenter::GetInstance();

    bool shouldBeNull = provider.ConsumeBool();
    std::shared_ptr<IMechEventListener> listener = nullptr;

    if (!shouldBeNull) {
        listener = std::make_shared<MockMechEventListener>();
    }

    uint16_t type = provider.ConsumeIntegral<uint16_t>();
    subscriptionCenter.UnSubscribe(type, listener);
}

static void TestNotify(FuzzedDataProvider &provider)
{
    SubscriptionCenter &subscriptionCenter = SubscriptionCenter::GetInstance();

    bool shouldBeNull = provider.ConsumeBool();
    std::shared_ptr<CommandBase> cmd = nullptr;

    if (!shouldBeNull) {
        cmd = std::make_shared<MockCommandBase>();
        if (cmd != nullptr) {
            auto mockCmd = std::static_pointer_cast<MockCommandBase>(cmd);
            mockCmd->SetCmdSet(provider.ConsumeIntegral<uint8_t>());
            mockCmd->SetCmdId(provider.ConsumeIntegral<uint8_t>());
        }
    }

    subscriptionCenter.Notify(cmd);
}

static void TestSubscribeUnSubscribeCycle(FuzzedDataProvider &provider)
{
    SubscriptionCenter &subscriptionCenter = SubscriptionCenter::GetInstance();

    auto listener = std::make_shared<MockMechEventListener>();
    if (listener == nullptr) {
        return;
    }

    uint16_t type = provider.ConsumeIntegral<uint16_t>();

    subscriptionCenter.Subscribe(type, listener);
    subscriptionCenter.UnSubscribe(type, listener);
}

static void TestMultipleListeners(FuzzedDataProvider &provider)
{
    SubscriptionCenter &subscriptionCenter = SubscriptionCenter::GetInstance();

    uint16_t type = provider.ConsumeIntegral<uint16_t>();
    uint32_t listenerCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_LISTENER_COUNT, MAX_LISTENER_COUNT);

    std::vector<std::shared_ptr<IMechEventListener>> listeners;
    for (uint32_t i = MIN_INDEX; i < listenerCount; ++i) {
        auto listener = std::make_shared<MockMechEventListener>();
        if (listener != nullptr) {
            listeners.push_back(listener);
            subscriptionCenter.Subscribe(type, listener);
        }
    }

    for (auto &listener : listeners) {
        subscriptionCenter.UnSubscribe(type, listener);
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < MIN_INPUT_SIZE) {
        return 0;
    }

    FuzzedDataProvider provider(data, size);
    uint8_t testType = provider.ConsumeIntegral<uint8_t>() % TEST_FUNCTION_COUNT;

    switch (static_cast<TestFunctionId>(testType)) {
        case TestFunctionId::FUZZ_SUBSCRIBE:
            TestSubscribe(provider);
            break;
        case TestFunctionId::FUZZ_UNSUBSCRIBE:
            TestUnSubscribe(provider);
            break;
        case TestFunctionId::FUZZ_NOTIFY:
            TestNotify(provider);
            break;
        case TestFunctionId::FUZZ_SUBSCRIBE_UNSUBSCRIBE_CYCLE:
            TestSubscribeUnSubscribeCycle(provider);
            break;
        case TestFunctionId::FUZZ_MULTIPLE_LISTENERS:
            TestMultipleListeners(provider);
            break;
        default:
            break;
    }

    return 0;
}