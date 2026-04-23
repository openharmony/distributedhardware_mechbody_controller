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
#ifndef MC_WHEEL_SET_MECH_RELATIVE_POSITION_CMD_H
#define MC_WHEEL_SET_MECH_RELATIVE_POSITION_CMD_H

#include "mc_command_base.h"
#include "mechbody_controller_types.h"

namespace OHOS {
namespace MechBodyController {

class WheelSetMechRelativePositionCmd : public CommandBase {
public:
    static constexpr uint8_t CMD_SET = 0x03;
    static constexpr uint8_t CMD_ID = 0x22;
    static constexpr uint16_t REQ_SIZE = 6;
    static constexpr uint16_t RSP_SIZE = 3;

    explicit WheelSetMechRelativePositionCmd(uint16_t taskId, const std::vector<WheelPositionInfo>& params);
    ~WheelSetMechRelativePositionCmd() override = default;

    std::shared_ptr<MechDataBuffer> Marshal() const override;
    bool Unmarshal(std::shared_ptr<MechDataBuffer> data);
    void TriggerResponse(std::shared_ptr<MechDataBuffer> data) override;

    const std::vector<WheelPositionInfo>& GetParams() const;
    uint16_t GetTaskId() const;
    uint8_t GetErrCode() const;

private:
    uint16_t taskId_ = 0;
    std::vector<WheelPositionInfo> params_;
    uint8_t errCode_ = 0;
};

} // namespace MechBodyController
} // namespace OHOS

#endif // MC_WHEEL_SET_MECH_RELATIVE_POSITION_CMD_H
