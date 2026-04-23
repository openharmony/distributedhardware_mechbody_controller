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

#ifndef MC_WHEEL_SET_MECH_ROTATION_TRACE_CMD_H
#define MC_WHEEL_SET_MECH_ROTATION_TRACE_CMD_H

#include "mc_command_base.h"
#include "mechbody_controller_types.h"
#include "mc_common_set_mech_rotation_trace_cmd.h"

namespace OHOS {
namespace MechBodyController {

class WheelSetMechRotationTraceCmd : public CommonSetMechRotationTraceCmd {
public:
    static constexpr uint8_t CMD_SET = 0x03;
    static constexpr uint8_t CMD_ID = 0x21;
    static constexpr uint16_t REQ_SIZE = 6;
    static constexpr uint16_t RSP_SIZE = 3;

    explicit WheelSetMechRotationTraceCmd(uint16_t taskId, const std::vector<RotateParam>& params);
    ~WheelSetMechRotationTraceCmd() override = default;

    std::shared_ptr<MechDataBuffer> Marshal() const override;
    void TriggerResponse(std::shared_ptr<MechDataBuffer> data) override;

    const std::vector<RotateParam>& GetParams() const override;
    uint8_t GetResult() const override;

private:
    uint16_t taskId_;
    std::vector<RotateParam> params_;
    uint8_t result_ = 0;
};

} // namespace MechBodyController
} // namespace OHOS

#endif // MC_WHEEL_SET_MECH_ROTATION_TRACE_CMD_H
