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

#ifndef MC_WHEEL_GET_MECH_MOVEMENT_CAPABILITY_CMD_H
#define MC_WHEEL_GET_MECH_MOVEMENT_CAPABILITY_CMD_H

#include "mc_command_base.h"
#include "mechbody_controller_types.h"
#include <string>

namespace OHOS {
namespace MechBodyController {

class WheelGetMechMovementCapabilityCmd : public CommandBase {
public:
    static constexpr uint8_t CMD_SET = 0x03;
    static constexpr uint8_t CMD_ID = 0x01;
    static constexpr uint16_t REQ_SIZE = 2;
    static constexpr uint16_t RSP_SIZE = 20;

    explicit WheelGetMechMovementCapabilityCmd();
    ~WheelGetMechMovementCapabilityCmd() override = default;

    std::shared_ptr<MechDataBuffer> Marshal() const override;
    void TriggerResponse(std::shared_ptr<MechDataBuffer> data) override;
    uint8_t GetResult() const;
    void SetCapability(uint32_t cap);
    void SetCapabilityBitValue(uint32_t start, uint8_t value, uint8_t index);
    void OnDataReceive(uint8_t *data);
private:
    uint8_t result_ = 0;
};

} // namespace MechBodyController
} // namespace OHOS

#endif // MC_WHEEL_GET_MECH_MOVEMENT_CAPABILITY_CMD_H
