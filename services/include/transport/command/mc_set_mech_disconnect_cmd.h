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

#ifndef MC_SET_MECH_DISCONNECT_CMD_H
#define MC_SET_MECH_DISCONNECT_CMD_H

#include "mc_command_base.h"
#include "mechbody_controller_types.h"
#include <string>

namespace OHOS {
namespace MechBodyController {

class SetMechDisconnectCmd : public CommandBase {
public:
    static constexpr uint8_t CMD_SET = 0x01;
    static constexpr uint8_t CMD_ID = 0x04;
    static constexpr uint16_t REQ_SIZE = 3;
    static constexpr uint16_t RSP_SIZE = 1;

    explicit SetMechDisconnectCmd(uint8_t reason);
    ~SetMechDisconnectCmd() override = default;

    std::shared_ptr<MechDataBuffer> Marshal() const override;
    void TriggerResponse(std::shared_ptr<MechDataBuffer> data) override;

    uint8_t GetResult() const;

private:
    uint8_t reason_;
    uint8_t result_ = 0;
};

} // namespace MechBodyController
} // namespace OHOS

#endif // MC_SET_MECH_DISCONNECT_CMD_H
