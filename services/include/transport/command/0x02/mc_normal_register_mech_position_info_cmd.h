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
#ifndef MC_NORMAL_REGISTER_MECH_POSITION_INFO_CMD_H
#define MC_NORMAL_REGISTER_MECH_POSITION_INFO_CMD_H
#include "mc_command_base.h"
#include "mechbody_controller_types.h"
#include "mc_common_register_mech_position_info_cmd.h"

namespace OHOS {
namespace MechBodyController {

class NormalRegisterMechPositionInfoCmd : public CommonRegisterMechPositionInfoCmd {
public:
    static constexpr uint8_t CMD_SET = 0x02;
    static constexpr uint8_t CMD_ID = 0x47;
    static constexpr uint16_t REQ_SIZE = 0;
    static constexpr uint16_t RSP_SIZE = 0;
    static constexpr uint16_t RPT_SIZE = 13;

    explicit NormalRegisterMechPositionInfoCmd();
    ~NormalRegisterMechPositionInfoCmd() override = default;

    std::shared_ptr<MechDataBuffer> Marshal() const override;
    bool Unmarshal(std::shared_ptr<MechDataBuffer> data) override;
    void TriggerResponse(std::shared_ptr<MechDataBuffer> data) override;

    const EulerAngles& GetPosition() const override;
    uint8_t GetResult() const override;

private:
    EulerAngles position_;
    uint8_t limitStatus_;
    uint8_t result_ = 0;
};

} // namespace MechBodyController
} // namespace OHOS

#endif // MC_NORMAL_REGISTER_MECH_POSITION_INFO_CMD_H
