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
#include "notification_utils.h"

using namespace OHOS;

namespace {

enum class TestFunctionId {
    FUZZ_SEND_CELIA_NOTIFICATION = 0,
    FUZZ_SEND_CONNECTED_CAPSULE_NOTIFICATION = 1,
    FUZZ_CANCEL_NOTIFICATION = 2,
    FUZZ_SEND_AND_CANCEL = 3,
    FUZZ_CANCEL_MULTIPLE = 4,
    FUZZ_SEND_INVALID_TYPE = 5,
    FUZZ_CANCEL_BOUNDARY_VALUES = 6,
    FUZZ_SEND_CELIA_MULTIPLE = 7,
    FUZZ_SEND_CAPSULE_MULTIPLE = 8,
    FUZZ_ALL_NOTIFICATION_TYPES = 9,
    FUZZ_SUBSCRIBER_CALLBACKS = 10,
    FUZZ_NOTIFICATION_SEQUENCE = 11,
};

void FuzzSendCeliaNotification(FuzzedDataProvider &provider)
{
    (void)provider;
    NotificationUtils::SendNotification(NotificationType::NOTIFICATION_TYPE_CELIA);
}

void FuzzSendConnectedCapsuleNotification(FuzzedDataProvider &provider)
{
    (void)provider;
    NotificationUtils::SendNotification(NotificationType::NOTIFICATION_TYPE_CONNECTED_CAPSULE);
}

void FuzzCancelNotification(FuzzedDataProvider &provider)
{
    int32_t notificationId = provider.ConsumeIntegral<int32_t>();
    NotificationUtils::CancelNotification(notificationId);
}

void FuzzSendAndCancel(FuzzedDataProvider &provider)
{
    NotificationUtils::SendNotification(NotificationType::NOTIFICATION_TYPE_CELIA);
    int32_t notificationId = provider.ConsumeIntegral<int32_t>();
    NotificationUtils::CancelNotification(notificationId);
}

void FuzzCancelMultiple(FuzzedDataProvider &provider)
{
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 3);

    for (int32_t i = 0; i < iterations; i++) {
        int32_t notificationId = provider.ConsumeIntegral<int32_t>();
        NotificationUtils::CancelNotification(notificationId);
    }
}

void FuzzSendInvalidType(FuzzedDataProvider &provider)
{
    (void)provider;
    NotificationType invalidType = static_cast<NotificationType>(99);
    NotificationUtils::SendNotification(invalidType);
}

void FuzzCancelBoundaryValues(FuzzedDataProvider &provider)
{
    int32_t boundaryValues[] = {INT32_MIN, -1, 0, 1, INT32_MAX};

    for (auto notificationId : boundaryValues) {
        NotificationUtils::CancelNotification(notificationId);
    }
}

void FuzzSendCeliaMultiple(FuzzedDataProvider &provider)
{
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 3);

    for (int32_t i = 0; i < iterations; i++) {
        NotificationUtils::SendNotification(NotificationType::NOTIFICATION_TYPE_CELIA);
    }
}

void FuzzSendCapsuleMultiple(FuzzedDataProvider &provider)
{
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 3);

    for (int32_t i = 0; i < iterations; i++) {
        NotificationUtils::SendNotification(NotificationType::NOTIFICATION_TYPE_CONNECTED_CAPSULE);
    }
}

void FuzzAllNotificationTypes(FuzzedDataProvider &provider)
{
    (void)provider;
    NotificationUtils::SendNotification(NotificationType::NOTIFICATION_TYPE_CELIA);
    NotificationUtils::SendNotification(NotificationType::NOTIFICATION_TYPE_CONNECTED_CAPSULE);
}

void FuzzSubscriberCallbacks(FuzzedDataProvider &provider)
{
    (void)provider;
    LocalLiveViewSubscriber subscriber;

    subscriber.OnConnected();
    subscriber.OnDisconnected();
    subscriber.OnDied();

    int32_t notificationId = provider.ConsumeIntegral<int32_t>();
    sptr<OHOS::Notification::NotificationButtonOption> buttonOption =
        new OHOS::Notification::NotificationButtonOption();
    subscriber.OnResponse(notificationId, buttonOption);
}

void FuzzNotificationSequence(FuzzedDataProvider &provider)
{
    NotificationUtils::SendNotification(NotificationType::NOTIFICATION_TYPE_CELIA);

    int32_t notificationId = provider.ConsumeIntegral<int32_t>();
    NotificationUtils::CancelNotification(notificationId);

    NotificationUtils::SendNotification(NotificationType::NOTIFICATION_TYPE_CONNECTED_CAPSULE);

    notificationId = provider.ConsumeIntegral<int32_t>();
    NotificationUtils::CancelNotification(notificationId);
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0, 11);

    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_SEND_CELIA_NOTIFICATION:
            FuzzSendCeliaNotification(provider);
            break;
        case TestFunctionId::FUZZ_SEND_CONNECTED_CAPSULE_NOTIFICATION:
            FuzzSendConnectedCapsuleNotification(provider);
            break;
        case TestFunctionId::FUZZ_CANCEL_NOTIFICATION:
            FuzzCancelNotification(provider);
            break;
        case TestFunctionId::FUZZ_SEND_AND_CANCEL:
            FuzzSendAndCancel(provider);
            break;
        case TestFunctionId::FUZZ_CANCEL_MULTIPLE:
            FuzzCancelMultiple(provider);
            break;
        case TestFunctionId::FUZZ_SEND_INVALID_TYPE:
            FuzzSendInvalidType(provider);
            break;
        case TestFunctionId::FUZZ_CANCEL_BOUNDARY_VALUES:
            FuzzCancelBoundaryValues(provider);
            break;
        case TestFunctionId::FUZZ_SEND_CELIA_MULTIPLE:
            FuzzSendCeliaMultiple(provider);
            break;
        case TestFunctionId::FUZZ_SEND_CAPSULE_MULTIPLE:
            FuzzSendCapsuleMultiple(provider);
            break;
        case TestFunctionId::FUZZ_ALL_NOTIFICATION_TYPES:
            FuzzAllNotificationTypes(provider);
            break;
        case TestFunctionId::FUZZ_SUBSCRIBER_CALLBACKS:
            FuzzSubscriberCallbacks(provider);
            break;
        case TestFunctionId::FUZZ_NOTIFICATION_SEQUENCE:
            FuzzNotificationSequence(provider);
            break;
        default:
            break;
    }

    return 0;
}