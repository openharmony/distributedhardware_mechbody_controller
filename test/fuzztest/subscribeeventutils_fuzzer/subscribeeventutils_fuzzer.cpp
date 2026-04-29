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
#include "subscribe_event_utils.h"

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

class MockBaseService : public BaseService {
public:
    MockBaseService() = default;
    virtual ~MockBaseService() = default;

    void OnReceiveEvent(std::string action, const EventFwk::CommonEventData &data) override
    {
        (void)action;
        (void)data;
    }
};

enum class TestFunctionId {
    FUZZ_GET_INSTANCE = 0,
    FUZZ_INIT = 1,
    FUZZ_UNINIT = 2,
    FUZZ_ADD_EVENT_SERVICE = 3,
    FUZZ_ON_RECEIVE_EVENT = 4,
    FUZZ_INIT_UNINIT_SEQUENCE = 5,
    FUZZ_ADD_MULTIPLE_SERVICES = 6,
    FUZZ_SUBSCRIBER_ON_RECEIVE_EVENT = 7,
    FUZZ_ALL_FUNCTIONS = 8,
};

void FuzzGetInstance(FuzzedDataProvider &provider)
{
    (void)provider;
    SubscribeEventUtils::GetInstance();
}

void FuzzInit(FuzzedDataProvider &provider)
{
    (void)provider;
    SubscribeEventUtils::GetInstance().Init();
}

void FuzzUnInit(FuzzedDataProvider &provider)
{
    (void)provider;
    SubscribeEventUtils::GetInstance().UnInit();
}

void FuzzAddEventService(FuzzedDataProvider &provider)
{
    (void)provider;
    static MockBaseService service;
    SubscribeEventUtils::GetInstance().AddEventService(&service);
}

void FuzzOnReceiveEvent(FuzzedDataProvider &provider)
{
    std::string action = provider.ConsumeRandomLengthString();

    EventFwk::Want want;
    want.SetAction(action);
    EventFwk::CommonEventData data(want);

    SubscribeEventUtils::GetInstance().OnReceiveEvent(action, data);
}

void FuzzInitUnInitSequence(FuzzedDataProvider &provider)
{
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 3);

    for (int32_t i = 0; i < iterations; i++) {
        SubscribeEventUtils::GetInstance().Init();
        SubscribeEventUtils::GetInstance().UnInit();
    }
}

void FuzzAddMultipleServices(FuzzedDataProvider &provider)
{
    int32_t serviceCount = provider.ConsumeIntegralInRange<int32_t>(1, 3);

    static MockBaseService services[3];

    for (int32_t i = 0; i < serviceCount; i++) {
        SubscribeEventUtils::GetInstance().AddEventService(&services[i]);
    }
}

void FuzzSubscriberOnReceiveEvent(FuzzedDataProvider &provider)
{
    std::string action = provider.ConsumeRandomLengthString();

    EventFwk::Want want;
    want.SetAction(action);
    EventFwk::CommonEventData data(want);

    EventFwk::MatchingSkills matchingSkills;
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    ExtCommonEventSubscriber subscriber(subscribeInfo);

    subscriber.OnReceiveEvent(data);
}

void FuzzAllFunctions(FuzzedDataProvider &provider)
{
    SubscribeEventUtils::GetInstance().Init();

    static MockBaseService service;
    SubscribeEventUtils::GetInstance().AddEventService(&service);

    std::string action = provider.ConsumeRandomLengthString();
    EventFwk::Want want;
    want.SetAction(action);
    EventFwk::CommonEventData data(want);

    SubscribeEventUtils::GetInstance().OnReceiveEvent(action, data);

    SubscribeEventUtils::GetInstance().UnInit();
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0, 8);

    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_GET_INSTANCE:
            FuzzGetInstance(provider);
            break;
        case TestFunctionId::FUZZ_INIT:
            FuzzInit(provider);
            break;
        case TestFunctionId::FUZZ_UNINIT:
            FuzzUnInit(provider);
            break;
        case TestFunctionId::FUZZ_ADD_EVENT_SERVICE:
            FuzzAddEventService(provider);
            break;
        case TestFunctionId::FUZZ_ON_RECEIVE_EVENT:
            FuzzOnReceiveEvent(provider);
            break;
        case TestFunctionId::FUZZ_INIT_UNINIT_SEQUENCE:
            FuzzInitUnInitSequence(provider);
            break;
        case TestFunctionId::FUZZ_ADD_MULTIPLE_SERVICES:
            FuzzAddMultipleServices(provider);
            break;
        case TestFunctionId::FUZZ_SUBSCRIBER_ON_RECEIVE_EVENT:
            FuzzSubscriberOnReceiveEvent(provider);
            break;
        case TestFunctionId::FUZZ_ALL_FUNCTIONS:
            FuzzAllFunctions(provider);
            break;
        default:
            break;
    }

    return 0;
}