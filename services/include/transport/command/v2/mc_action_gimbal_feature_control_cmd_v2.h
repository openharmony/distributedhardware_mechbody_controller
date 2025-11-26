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
#ifndef MC_ACTION_GIMBAL_FEATURE_CONTROL_CMD_V2_H
#define MC_ACTION_GIMBAL_FEATURE_CONTROL_CMD_V2_H

#include "mc_command_base.h"
#include "mechbody_controller_types.h"
#include "mc_action_gimbal_feature_control_cmd.h"

namespace OHOS {
namespace MechBodyController {

class ActionGimbalFeatureControlCmdV2 : public ActionGimbalFeatureControlCmd {
public:
    explicit ActionGimbalFeatureControlCmdV2(const ActionControlParams &params);
    ~ActionGimbalFeatureControlCmdV2() override = default;

    std::shared_ptr<MechDataBuffer> Marshal() const override;
    bool Unmarshal(std::shared_ptr<MechDataBuffer> data) override;
    void TriggerResponse(std::shared_ptr<MechDataBuffer> data) override;

    ActionControlParams &GetParams() override;
};
} // namespace MechBodyController
} // namespace OHOS

#endif // MC_ACTION_GIMBAL_FEATURE_CONTROL_CMD_V2_H
