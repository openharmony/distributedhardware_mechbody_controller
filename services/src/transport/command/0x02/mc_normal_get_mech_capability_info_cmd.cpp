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

#include "mc_normal_get_mech_capability_info_cmd.h"

#include "mechbody_controller_log.h"

namespace OHOS {
    namespace MechBodyController {
        namespace {
            const std::string TAG = "NormalGetMechCapabilityInfoCmd";
        }

        NormalGetMechCapabilityInfoCmd::NormalGetMechCapabilityInfoCmd()
        {
            cmdSet_ = CMD_SET;
            cmdId_ = CMD_ID;
            reqSize_ = REQ_SIZE;
            rspSize_ = RSP_SIZE;
            needResponse_ = (RSP_SIZE > 0);
            timeoutMs_ = MECHBODY_MSG_TIMEOUT;
            retryTimes_ = CMD_PRIORITY_MIDDLE;
        }

        std::shared_ptr<MechDataBuffer> NormalGetMechCapabilityInfoCmd::Marshal() const
        {
            HILOGI("start.");
            auto buffer = std::make_shared<MechDataBuffer>(reqSize_ + BIT_OFFSET_2);
            if (buffer == nullptr) {
                HILOGE("Failed to allocate memory for Marshal buffer");
                return nullptr;
            }

            CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdSet_), nullptr, "append cmdSet_");
            CHECK_ERR_RETURN_VALUE(buffer->AppendUint8(cmdId_), nullptr, "append cmdId_");

            HILOGI("end.");
            return buffer;
        }

        void NormalGetMechCapabilityInfoCmd::TriggerResponse(std::shared_ptr<MechDataBuffer> data)
        {
            HILOGI("start.");
            if (data == nullptr || data->Size() < RSP_SIZE + BIT_OFFSET_2) {
                HILOGE("Invalid input data for response");
                return;
            }

            size_t offset = BIT_OFFSET_2;

            CHECK_ERR_RETURN(data->ReadUint8(offset, result_), "read result_");
            HILOGI("response code: %{public}u", result_);
            offset++;

            uint8_t bitResult;
            CHECK_ERR_RETURN(data->ReadUint8(offset, bitResult), "read bitResult");
            params_.centerAble = (bitResult >> 0) & 1;
            params_.switchAble = (bitResult >> 1) & 1;
            params_.trackAble = (bitResult >> 2) & 1;
            params_.absoluteCoordinate = (bitResult >> 3) & 1;
            params_.relativeCoordinate = (bitResult >> 4) & 1;
            params_.poseReport = (bitResult >> 6) & 1;
            params_.batteryReport = (bitResult >> 7) & 1;

            offset++;

            uint8_t placeholder;
            CHECK_ERR_RETURN(data->ReadUint8(offset, placeholder), "read placeholder");
            offset++;
            
            CHECK_ERR_RETURN(data->ReadUint8(offset, params_.trackPoints), "read track points");
            offset++;

            CHECK_ERR_RETURN(data->ReadUint8(offset, params_.yawable), "read yawable");
            offset++;
            CHECK_ERR_RETURN(data->ReadUint8(offset, params_.rollable), "read rollable");
            offset++;
            CHECK_ERR_RETURN(data->ReadUint8(offset, params_.pitchable), "read pitchable");
            offset++;

            CHECK_ERR_RETURN(data->ReadFloat(offset, params_.yawmaxspeed), "read yawmaxspeed");
            offset += sizeof(float);
            CHECK_ERR_RETURN(data->ReadFloat(offset, params_.rollmaxspeed), "read rollmaxspeed");
            offset += sizeof(float);
            CHECK_ERR_RETURN(data->ReadFloat(offset, params_.pitchmaxspeed), "read pitchmaxspeed");
            offset += sizeof(float);

            CHECK_ERR_RETURN(data->ReadUint16(offset, params_.maxturntime), "read maxturntime");

            if (responseCb_) {
                HILOGI("trigger response callback.");
                responseCb_();
            }
            HILOGI("end.");
        }

        DeviceCapabilityInfo NormalGetMechCapabilityInfoCmd::GetParams() const
        {
            return params_;
        }

        uint8_t NormalGetMechCapabilityInfoCmd::GetResult() const
        {
            return result_;
        }
    } // namespace MechBodyController
} // namespace OHOS
