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

#ifndef MC_COMMAND_TEST_0X02_H
#define MC_COMMAND_TEST_0X02_H

#include "gtest/gtest.h"
#include "mc_command_factory.h"

namespace OHOS {
namespace MechBodyController {
class MechCommandTest0x02 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

private:
    void TestRotationAxisLimit(std::shared_ptr<NormalSetMechRotationToLocationCmd> executionCmd,
                                uint8_t limitFlags, RotationAxisLimited expectedYaw,
                                RotationAxisLimited expectedRoll, RotationAxisLimited expectedPitch);
    void TestRotationBySpeedAxisLimit(std::shared_ptr<NormalSetMechRotationBySpeedCmd> executionCmd,
                                       uint8_t limitFlags, RotationAxisLimited expectedYaw,
                                       RotationAxisLimited expectedRoll, RotationAxisLimited expectedPitch);
    void TestKeyEventUnmarshal(std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd,
                               uint8_t eventType, uint8_t buttonCount, uint8_t buttonFrequency,
                               CameraKeyEvent expectedEvent);
    void TestWheelEventUnmarshal(std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd,
                                  uint8_t wheelDataLength, uint16_t wheelSpeed, uint8_t wheelDirection,
                                  bool expectedResult);
    void TestStickEventUnmarshal(std::shared_ptr<NormalRegisterMechKeyEventCmd> executionCmd,
                                  uint8_t stickDataLength, uint16_t stickX, uint16_t stickY,
                                  bool expectedResult);
};
} // namespace MechBodyController
} // namespace OHOS
#endif // MC_COMMAND_TEST_0X02_H