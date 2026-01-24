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

#ifndef MC_COMMON_SET_MECH_ROTATION_BY_SPEED_CMD_H
#define MC_COMMON_SET_MECH_ROTATION_BY_SPEED_CMD_H

#include "mc_command_base.h"
#include "mechbody_controller_types.h"

namespace OHOS {
namespace MechBodyController {

class CommonSetMechRotationBySpeedCmd : public CommandBase {
public:
    ~CommonSetMechRotationBySpeedCmd() override = default;

    std::shared_ptr<MechDataBuffer> Marshal() const override = 0;
    void TriggerResponse(std::shared_ptr<MechDataBuffer> data) override = 0;

    virtual const RotateBySpeedParam& GetParams() const = 0;
    virtual const RotationAxesStatus& GetRotationAxesStatus() const = 0;
    virtual uint8_t GetResult() const = 0;
};

} // namespace MechBodyController
} // namespace OHOS

#endif // MC_COMMON_SET_MECH_ROTATION_BY_SPEED_CMD_H
