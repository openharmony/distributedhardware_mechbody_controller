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

#include "subscribe_event_utils_test.h"
#include "common_event_data.h"
#include "want.h"

#include "../test_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {

void SubscribeEventUtilsTest::SetUpTestCase()
{
    DTEST_LOG << "SubscribeEventUtilsTest::SetUpTestCase" << std::endl;
}

void SubscribeEventUtilsTest::TearDownTestCase()
{
    DTEST_LOG << "SubscribeEventUtilsTest::TearDownTestCase" << std::endl;
}

void SubscribeEventUtilsTest::SetUp()
{
    DTEST_LOG << "SubscribeEventUtilsTest::SetUp" << std::endl;
}

void SubscribeEventUtilsTest::TearDown()
{
    DTEST_LOG << "SubscribeEventUtilsTest::TearDown" << std::endl;
}

/**
 * @tc.name: OnReceiveEvent_001
 * @tc.desc: 测试接收事件时服务列表为空的情况（避免 CFI 跨模块调用问题）
 * @tc.type: FUNC
 */
HWTEST_F(SubscribeEventUtilsTest, OnReceiveEvent_001, TestSize.Level3)
{
    DTEST_LOG << "SubscribeEventUtilsTest OnReceiveEvent_001 begin" << std::endl;

    // Given: 创建测试服务并添加到 SubscribeEventUtils
    SubscribeEventUtils &instance = SubscribeEventUtils::GetInstance();
    instance.Init();

    // 创建一个测试服务来验证 OnReceiveEvent 被调用
    class TestService : public BaseService {
    public:
        TestService() : receiveCount_(0), lastAction_("") {}
        void OnReceiveEvent(std::string action, const EventFwk::CommonEventData &data) override
        {
            receiveCount_++;
            lastAction_ = action;
        }
        int32_t GetReceiveCount() const { return receiveCount_; }
        const std::string& GetLastAction() const { return lastAction_; }

    private:
        int32_t receiveCount_;
        std::string lastAction_;
    };

    TestService testService;
    instance.AddEventService(&testService);

    // When: 模拟接收事件
    std::string testAction = "test.event.action";
    AAFwk::Want want;
    want.SetAction(testAction);
    EventFwk::CommonEventData data(want);

    instance.OnReceiveEvent(testAction, data);

    // Then: 验证服务的 OnReceiveEvent 被正确调用
    EXPECT_EQ(testService.GetReceiveCount(), 1);
    EXPECT_EQ(testService.GetLastAction(), testAction);

    DTEST_LOG << "SubscribeEventUtilsTest OnReceiveEvent_001 end" << std::endl;
}

} // namespace MechBodyController
} // namespace OHOS