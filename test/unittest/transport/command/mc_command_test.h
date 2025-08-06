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

#ifndef MC_COMMAND_TEST_H
#define MC_COMMAND_TEST_H

#include "gtest/gtest.h"

#include "mc_command_base.h"
#include "mc_command_factory.h"
#include "mc_get_mech_camera_tracking_layout_cmd.h"
#include "mc_get_mech_capability_info_cmd.h"
#include "mc_register_mech_camera_key_event_cmd.h"
#include "mc_register_mech_control_result_cmd.h"
#include "mc_register_mech_position_info_cmd.h"
#include "mc_register_mech_state_info_cmd.h"
#include "mc_register_mech_wheel_data_cmd.h"
#include "mc_set_mech_camera_info_cmd.h"
#include "mc_set_mech_camera_tracking_frame_cmd.h"
#include "mc_set_mech_camera_tracking_layout_cmd.h"
#include "mc_set_mech_config_cmd.h"
#include "mc_set_mech_rotation_by_speed_cmd.h"
#include "mc_set_mech_rotation_cmd.h"
#include "mc_set_mech_rotation_trace_cmd.h"
#include "mc_set_mech_stop_cmd.h"

namespace OHOS {
namespace MechBodyController {
class MechCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
} // namespace MechBodyController
} // namespace OHOS
#endif // MC_COMMAND_TEST_H
