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

#ifndef MC_SET_MECH_ROTATION_CMD_V2_H
#define MC_SET_MECH_ROTATION_CMD_V2_H

#include "mc_command_base.h"
#include "mechbody_controller_types.h"
#include "mc_set_mech_rotation_cmd.h"

namespace OHOS {
namespace MechBodyController {

class SetMechRotationCmdV2 : public SetMechRotationCmd {
public:
    static constexpr uint8_t CMD_SET = 0x02;
    static constexpr uint8_t CMD_ID = 0x20;
    static constexpr uint16_t REQ_SIZE = 16;
    static constexpr uint16_t RSP_SIZE = 4;

    explicit SetMechRotationCmdV2(const RotateParam& params, const RotationAxesStatus& status);
    ~SetMechRotationCmdV2() override = default;

    std::shared_ptr<MechDataBuffer> Marshal() const override;
    bool Unmarshal(std::shared_ptr<MechDataBuffer> data) override;
    void TriggerResponse(std::shared_ptr<MechDataBuffer> data) override;

    const RotateParam& GetParams() const override;
    const RotationAxesStatus& GetRotationAxesStatus() const override;
    uint8_t GetResult() const override;
};
} // namespace MechBodyController
} // namespace OHOS

#endif // MC_SET_MECH_ROTATION_CMD_V2_H
