/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef DISTRIBUTED_SCHED_STUB_TEST_H
#define DISTRIBUTED_SCHED_STUB_TEST_H

#include "event_handler.h"
#include "iremote_stub.h"
#include "gtest/gtest.h"

#define private public
#include "mc_motion_manager.h"
#undef private

namespace OHOS {
namespace MechBodyController {
class MockIRemoteObject : public IRemoteObject {
public:
    MockIRemoteObject() : IRemoteObject(u"mock_i_remote_object") {}
    ~MockIRemoteObject() {}
    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        return 0;
    }
    int32_t GetObjectRefCount() override
    {
        return 0;
    }
    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }
    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }
    int Dump(int fd, const std::vector<std::u16string> &args) override
    {
        (void)fd;
        return 0;
    }
    std::u16string GetObjectDescriptor() const
    {
        std::u16string descriptor = std::u16string();
        return descriptor;
    }
};

class MotionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
} // namespace MechBodyController
} // namespace OHOS
#endif // DISTRIBUTED_SCHED_STUB_TEST_H