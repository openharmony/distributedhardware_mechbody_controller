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

#ifndef MC_COMMAND_FACTORY_H
#define MC_COMMAND_FACTORY_H

#include <memory>
#include <vector>
#include "mc_action_gimbal_feature_control_cmd.h"
#include "mc_command_base.h"
#include "mc_data_buffer.h"
#include "mc_get_mech_camera_tracking_layout_cmd.h"
#include "mc_get_mech_capability_info_cmd.h"
#include "mc_get_mech_real_name_cmd.h"
#include "mc_register_mech_camera_key_event_cmd.h"
#include "mc_register_mech_control_result_cmd.h"
#include "mc_register_mech_position_info_cmd.h"
#include "mc_register_mech_state_info_cmd.h"
#include "mc_register_mech_tracking_enable_cmd.h"
#include "mc_register_mech_wheel_data_cmd.h"
#include "mc_set_mech_camera_info_cmd.h"
#include "mc_set_mech_camera_tracking_enable_cmd.h"
#include "mc_set_mech_camera_tracking_frame_cmd.h"
#include "mc_set_mech_camera_tracking_layout_cmd.h"
#include "mc_set_mech_config_cmd.h"
#include "mc_set_mech_hid_preemptive_cmd.h"
#include "mc_set_mech_rotation_by_speed_cmd.h"
#include "mc_set_mech_rotation_cmd.h"
#include "mc_set_mech_rotation_trace_cmd.h"
#include "mc_set_mech_stop_cmd.h"

namespace OHOS {
namespace MechBodyController {

class CommandFactory {
public:
    std::shared_ptr<GetMechCameraTrackingLayoutCmd> CreateGetMechCameraTrackingLayoutCmd();
    std::shared_ptr<GetMechCapabilityInfoCmd> CreateGetMechCapabilityInfoCmd();
    std::shared_ptr<GetMechRealNameCmd> CreateGetMechRealNameCmd();
    std::shared_ptr<SetMechCameraInfoCmd> CreateSetMechCameraInfoCmd(
        const CameraInfoParams& params);
    std::shared_ptr<SetMechCameraTrackingEnableCmd> CreateSetMechCameraTrackingEnableCmd(
        MechTrackingStatus status);
    std::shared_ptr<SetMechCameraTrackingFrameCmd> CreateSetMechCameraTrackingFrameCmd(
        const TrackingFrameParams& params);
    std::shared_ptr<SetMechCameraTrackingLayoutCmd> CreateSetMechCameraTrackingLayoutCmd(
        const LayoutParams& params);
    std::shared_ptr<SetMechConfigCmd> CreateSetMechConfigCmd(
        uint8_t configVersion);
    std::shared_ptr<SetMechHidPreemptiveCmd> CreateSetMechHidPreemptiveCmd(bool isPreemptive);
    std::shared_ptr<SetMechRotationBySpeedCmd> CreateSetMechRotationBySpeedCmd(
        const RotateBySpeedParam& params);
    std::shared_ptr<SetMechRotationCmd> CreateSetMechRotationCmd(
        const RotateParam& params);
    std::shared_ptr<SetMechRotationTraceCmd> CreateSetMechRotationTraceCmd(
        const std::vector<RotateParam>& params);
    std::shared_ptr<SetMechStopCmd> CreateSetMechStopCmd();
    std::shared_ptr<RegisterMechCameraKeyEventCmd> CreateRegisterMechCameraKeyEventCmd();
    std::shared_ptr<RegisterMechControlResultCmd> CreateRegisterMechControlResultCmd();
    std::shared_ptr<RegisterMechPositionInfoCmd> CreateRegisterMechPositionInfoCmd();
    std::shared_ptr<RegisterMechStateInfoCmd> CreateRegisterMechStateInfoCmd();
    std::shared_ptr<RegisterMechWheelDataCmd> CreateRegisterMechWheelDataCmd();
    std::shared_ptr<RegisterMechTrackingEnableCmd> CreateRegisterMechTrackingEnableCmd();

    std::shared_ptr<CommandBase> CreateFromData(std::shared_ptr<MechDataBuffer> data);
    std::shared_ptr<ActionGimbalFeatureControlCmd> CreateActionGimbalFeatureControlCmd(
        const ActionControlParams& params);

private:
    template<typename CmdT>
    std::shared_ptr<CommandBase> CreateAndUnmarshal(std::shared_ptr<MechDataBuffer> data)
    {
        auto cmd = std::make_shared<CmdT>();
        if (!cmd->Unmarshal(data)) {
            return nullptr;
        }
        return cmd;
    }
};

} // namespace MechBodyController
} // namespace OHOS

#endif // MC_COMMAND_FACTORY_H
