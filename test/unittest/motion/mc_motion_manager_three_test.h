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

#ifndef MC_MOTION_MANAGER_THREE_TEST_H
#define MC_MOTION_MANAGER_THREE_TEST_H

#include "event_handler.h"
#include "iremote_stub.h"
#include "gtest/gtest.h"
#include "app_mgr_interface.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"

// NOLINTNEXTLINE
#define private public
#include "mc_motion_manager.h"
#undef private

#include "mc_send_adapter.h"
#include "mc_command_base.h"

namespace OHOS {
namespace MechBodyController {

class MotionManagerThreeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

} // namespace MechBodyController
} // namespace OHOS
#endif // MC_MOTION_MANAGER_THREE_TEST_H
