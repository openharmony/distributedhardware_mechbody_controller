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

#ifndef MOCK_TRANSPORT_SEND_ADAPTER_H
#define MOCK_TRANSPORT_SEND_ADAPTER_H

#include "mc_send_adapter.h"
#include "mc_get_mech_protocol_ver_cmd.h"
#include "mc_normal_get_mech_base_info_cmd.h"
#include "mc_normal_get_mech_capability_info_cmd.h"
#include "mc_get_mech_limit_info_cmd.h"
#include "mc_normal_set_mech_protocol_ver_cmd.h"
#include "mc_data_buffer.h"
#include <memory>
#include <thread>
#include <chrono>
#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
const std::string TAG = "MockTransportSendAdapter";
class MockTransportSendAdapter : public TransportSendAdapter {
public:
    MockTransportSendAdapter() = default;
    ~MockTransportSendAdapter() override = default;
    
    int32_t SendCommand(const std::shared_ptr<CommandBase> &command, int32_t delay = 0) override
    {
        HILOGI("MockTransportSendAdapter SendCommand start");
        if (command == nullptr) {
            return -1;
        }
        
        if (delay > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
        SimulateResponse(command);

        return 0;
    }
    
private:
    static constexpr uint16_t CMD_TYPE_GET_PROTOCOL_VER = 0x202;
    static constexpr uint16_t CMD_TYPE_GET_BASE_INFO = 0x101;
    static constexpr uint16_t CMD_TYPE_GET_CAPABILITY = 0x302;
    static constexpr uint16_t CMD_TYPE_GET_LIMIT_INFO = 0x305;
    static constexpr uint16_t CMD_TYPE_SET_PROTOCOL_VER = 0x102;
    
    static constexpr int32_t BUFFER_SIZE_PROTOCOL_VER = 5;
    static constexpr int32_t BUFFER_SIZE_BASE_INFO = 35;
    static constexpr int32_t BUFFER_SIZE_CAPABILITY = 3;
    static constexpr int32_t BUFFER_SIZE_LIMIT_INFO = 26;
    static constexpr int32_t BUFFER_SIZE_SET_PROTOCOL = 3;
    static constexpr int32_t BASE_INFO_NAME_LENGTH = 32;
    
    static constexpr uint8_t STATUS_SUCCESS = 0x01;
    static constexpr uint8_t STATUS_BUSY = 0x02;
    
    void SimulateResponse(const std::shared_ptr<CommandBase> &command)
    {
        HILOGI("SimulateResponse start");
        auto cmdType = command->GetCmdType();
        HILOGI("SimulateResponse cmdType: 0x%{public}x", cmdType);
        
        switch (cmdType) {
            case CMD_TYPE_GET_PROTOCOL_VER:
                HandleGetProtocolVerCommand(command);
                break;
            case CMD_TYPE_GET_BASE_INFO:
                HandleGetBaseInfoCommand(command);
                break;
            case CMD_TYPE_GET_CAPABILITY:
                HandleGetCapabilityCommand(command);
                break;
            case CMD_TYPE_GET_LIMIT_INFO:
                HandleGetLimitInfoCommand(command);
                break;
            case CMD_TYPE_SET_PROTOCOL_VER:
                HandleSetProtocolVerCommand(command);
                break;
            default:
                break;
        }
    }
    
    void HandleGetProtocolVerCommand(const std::shared_ptr<CommandBase> &command)
    {
        auto protocolVerCmd = std::static_pointer_cast<GetMechProtocolVerCmd>(command);
        auto responseData = std::make_shared<MechDataBuffer>(BUFFER_SIZE_PROTOCOL_VER);
        if (responseData != nullptr) {
            responseData->AppendUint8(STATUS_BUSY);
            responseData->AppendUint8(0x02);
            responseData->AppendUint8(0x00);
            responseData->AppendUint8(0x00);
            responseData->AppendUint8(0x02);
            protocolVerCmd->TriggerResponse(responseData);
        }
    }
    
    void HandleGetBaseInfoCommand(const std::shared_ptr<CommandBase> &command)
    {
        auto baseInfoCmd = std::static_pointer_cast<NormalGetMechBaseInfoCmd>(command);
        auto responseData = std::make_shared<MechDataBuffer>(BUFFER_SIZE_BASE_INFO);
        if (responseData != nullptr) {
            responseData->AppendUint8(STATUS_SUCCESS);
            responseData->AppendUint8(0x01);
            responseData->AppendUint8(0x02);
            for (int i = 0; i < BASE_INFO_NAME_LENGTH; i++) {
                responseData->AppendUint8(0x00);
            }
            baseInfoCmd->TriggerResponse(responseData);
        }
    }
    
    void HandleGetCapabilityCommand(const std::shared_ptr<CommandBase> &command)
    {
        auto capabilityCmd = std::static_pointer_cast<NormalGetMechCapabilityInfoCmd>(command);
        auto responseData = std::make_shared<MechDataBuffer>(BUFFER_SIZE_CAPABILITY);
        if (responseData != nullptr) {
            responseData->AppendUint8(STATUS_BUSY);
            responseData->AppendUint8(0x00);
            responseData->AppendUint8(0x01);
            capabilityCmd->TriggerResponse(responseData);
        }
    }
    
    void HandleGetLimitInfoCommand(const std::shared_ptr<CommandBase> &command)
    {
        auto limitCmd = std::static_pointer_cast<GetMechLimitInfoCmd>(command);
        auto responseData = std::make_shared<MechDataBuffer>(BUFFER_SIZE_LIMIT_INFO);
        if (responseData != nullptr) {
            responseData->AppendFloat(3.14f);
            responseData->AppendFloat(0.0f);
            responseData->AppendFloat(1.57f);
            responseData->AppendFloat(-3.14f);
            responseData->AppendFloat(0.0f);
            responseData->AppendFloat(-1.57f);
            limitCmd->TriggerResponse(responseData);
        }
    }
    
    void HandleSetProtocolVerCommand(const std::shared_ptr<CommandBase> &command)
    {
        auto setProtocolCmd = std::static_pointer_cast<NormalSetMechProtocolVerCmd>(command);
        auto responseData = std::make_shared<MechDataBuffer>(BUFFER_SIZE_SET_PROTOCOL);
        if (responseData != nullptr) {
            responseData->AppendUint8(STATUS_SUCCESS);
            responseData->AppendUint8(0x02);
            responseData->AppendUint8(0x02);
            setProtocolCmd->TriggerResponse(responseData);
        }
    }
};

}  // namespace MechBodyController
}  // namespace OHOS

#endif  // MOCK_TRANSPORT_SEND_ADAPTER_H