/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef MC_MOTION_MANAGER_TWO_TEST_H
#define MC_MOTION_MANAGER_TWO_TEST_H

#include "event_handler.h"
#include "iremote_stub.h"
#include "gtest/gtest.h"

// NOLINTNEXTLINE
#define private public
#include "mc_motion_manager.h"
#undef private

#include "mc_send_adapter.h"
#include "mc_command_base.h"

namespace OHOS {
namespace MechBodyController {

// Mock TransportSendAdapter to track SendCommand calls
class MockTransportSendAdapter : public TransportSendAdapter {
public:
    MockTransportSendAdapter() : sendCommandCallCount(0) {}
    int32_t SendCommand(const std::shared_ptr<CommandBase> &cmd, int32_t delayMs = 0) override {
        sendCommandCallCount++;
        return ERR_OK;
    }
    int sendCommandCallCount;
    void ResetCount() { sendCommandCallCount = 0; }
};

class MotionManagerTwoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    // Helper function to initialize MotionManager with common settings
    std::shared_ptr<MotionManager> CreateInitializedMotionManager(int32_t mechId = 100);
};
} // namespace MechBodyController
} // namespace OHOS
#endif // MC_MOTION_MANAGER_TWO_TEST_H