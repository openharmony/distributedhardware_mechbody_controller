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
#ifndef MC_NORMAL_REGISTER_MECH_GENERIC_EVENT_CMD_H
#define MC_NORMAL_REGISTER_MECH_GENERIC_EVENT_CMD_H
#include "mc_command_base.h"
#include "mechbody_controller_types.h"

namespace OHOS {
namespace MechBodyController {

class NormalRegisterMechGenericEventCmd : public CommandBase {
public:
    static constexpr uint8_t CMD_SET = 0x02;
    static constexpr uint8_t CMD_ID = 0x48;
    static constexpr uint16_t REQ_SIZE = 0;
    static constexpr uint16_t RSP_SIZE = 0;
    static constexpr uint16_t RPT_SIZE = 3;

    explicit NormalRegisterMechGenericEventCmd();
    ~NormalRegisterMechGenericEventCmd() override = default;

    std::shared_ptr<MechDataBuffer> Marshal() const override;
    bool Unmarshal(std::shared_ptr<MechDataBuffer> data);
    void TriggerResponse(std::shared_ptr<MechDataBuffer> data) override;

    DeviceStateInfo GetParams() const;
    uint8_t GetResult() const;

private:
    DeviceStateInfo params_;
    uint8_t result_ = 0;
    uint8_t lowPower;
    uint8_t isCharging;
};

} // namespace MechBodyController
} // namespace OHOS

#endif // MC_NORMAL_REGISTER_MECH_GENERIC_EVENT_CMD_H
