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
#ifndef MC_REGISTER_MECH_CAMERA_KEY_EVENT_CMD_H
#define MC_REGISTER_MECH_CAMERA_KEY_EVENT_CMD_H
#include "mc_command_base.h"
#include "mechbody_controller_types.h"
#include "mc_common_register_mech_key_event_cmd.h"

namespace OHOS {
namespace MechBodyController {

class RegisterMechCameraKeyEventCmd : public CommonRegisterMechKeyEventCmd {
public:
    static constexpr uint8_t CMD_SET = 0x02;
    static constexpr uint8_t CMD_ID = 0x40;
    static constexpr uint16_t REQ_SIZE = 0;
    static constexpr uint16_t RSP_SIZE = 0;
    static constexpr uint16_t RPT_SIZE = 8;

    explicit RegisterMechCameraKeyEventCmd();
    ~RegisterMechCameraKeyEventCmd() override = default;

    std::shared_ptr<MechDataBuffer> Marshal() const override;
    bool Unmarshal(std::shared_ptr<MechDataBuffer> data) override;
    void TriggerResponse(std::shared_ptr<MechDataBuffer> data) override;

    CameraKeyEvent GetEvent() const override;
    uint8_t GetResult() const override;
    int16_t GetStickX() const override;
    int16_t GetStickY() const override;

private:
    CameraKeyEvent event_ = CameraKeyEvent::INVALID;
    uint8_t result_ = 0;
    int16_t stickX_ = 0;
    int16_t stickY_ = 0;
};

} // namespace MechBodyController
} // namespace OHOS

#endif // MC_REGISTER_MECH_CAMERA_KEY_EVENT_CMD_H
