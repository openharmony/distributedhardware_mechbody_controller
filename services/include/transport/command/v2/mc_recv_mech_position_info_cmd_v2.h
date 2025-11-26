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

#ifndef MC_RECV_MECH_POSITION_INFO_CMD_V2_H
#define MC_RECV_MECH_POSITION_INFO_CMD_V2_H

#include "mc_recv_mech_position_info_cmd.h"
#include "mechbody_controller_types.h"

namespace OHOS {
namespace MechBodyController {

class RecvMechPositionInfoCmdV2 : public RecvMechPositionInfoCmd {
    static constexpr uint8_t CMD_SET = 0x02;
    static constexpr uint8_t CMD_ID = 0x47;
    static constexpr uint16_t REQ_SIZE = 0;
    static constexpr uint16_t RSP_SIZE = 0;
    static constexpr uint16_t RPT_SIZE = 12;

    explicit RecvMechPositionInfoCmdV2();
    ~RecvMechPositionInfoCmdV2() override = default;

    std::shared_ptr<MechDataBuffer> Marshal() const override;
    bool Unmarshal(std::shared_ptr<MechDataBuffer> data) override;
    void TriggerResponse(std::shared_ptr<MechDataBuffer> data) override;

    const EulerAngles& GetPosition() const override;
    const RotationAxesStatus& GetRotationAxesStatus() const override;
    uint8_t GetResult() const override;
};
} // namespace MechBodyController
} // namespace OHOS

#endif // MC_RECV_MECH_POSITION_INFO_CMD_V2_H
