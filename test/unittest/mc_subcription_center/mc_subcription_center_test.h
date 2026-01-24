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

#ifndef MECH_SUBCRIPTION_CENTER_TEST_H
#define MECH_SUBCRIPTION_CENTER_TEST_H
#include "mc_subscription_center.h"

#include "gtest/gtest.h"

namespace OHOS {
namespace MechBodyController {
class SubMechEventListener : public IMechEventListener {
public:
    SubMechEventListener() {}
    void MechAttitudeNotify(const std::shared_ptr<CommonRegisterMechPositionInfoCmd> &cmd) {};
    void MechButtonEventNotify(const std::shared_ptr<CommonRegisterMechKeyEventCmd> &cmd) {};
    void MechParamNotify(const std::shared_ptr<CommonRegisterMechStateInfoCmd> &cmd) {};
    void MechGenericEventNotify(const std::shared_ptr<NormalRegisterMechGenericEventCmd> &cmd) {};
    void MechExecutionResultNotify(const std::shared_ptr<RegisterMechControlResultCmd> &cmd) {};
    void MechWheelZoomNotify(const std::shared_ptr<RegisterMechWheelDataCmd> &cmd) {};
    void MechTrackingStatusNotify(const std::shared_ptr<CommonRegisterMechTrackingEnableCmd> &cmd) {};
};

class SubscriptionCenterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
} // namespace MechBodyController
} // namespace OHOS
#endif // MECH_SUBCRIPTION_CENTER_TEST_H
