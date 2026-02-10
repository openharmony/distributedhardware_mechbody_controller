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

#include "mc_command_factory.h"
#include "mechbody_controller_enums.h"
#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "CommandFactory";
    constexpr uint8_t CREATE_FROM_DATA_MIN_SIZE = 2;
}

void CommandFactory::SetFactoryProtocolVer(uint8_t protocolVer)
{
    protocolVer_ = protocolVer;
}

std::shared_ptr<GetMechCameraTrackingLayoutCmd> CommandFactory::CreateGetMechCameraTrackingLayoutCmd()
{
    return std::make_shared<GetMechCameraTrackingLayoutCmd>();
}

std::shared_ptr<CommonGetMechLimitInfoCmd> CommandFactory::CreateGetMechLimitInfoCmd()
{
    if (protocolVer_ == 0x01) {
        return std::make_shared<GetMechLimitInfoCmd>();
    } else if (protocolVer_ == 0x02) {
        return std::make_shared<NormalGetMechLimitInfoCmd>();
    } else {
        return std::make_shared<GetMechLimitInfoCmd>();
    }
}

std::shared_ptr<NormalGetMechCapabilityInfoCmd> CommandFactory::CreateGetMechCapabilityInfoCmd()
{
    return std::make_shared<NormalGetMechCapabilityInfoCmd>();
}

std::shared_ptr<GetMechProtocolVerCmd> CommandFactory::CreateGetMechProtocolVerCmd()
{
    return std::make_shared<GetMechProtocolVerCmd>();
}

std::shared_ptr<SetMechDisconnectCmd> CommandFactory::CreateSetMechDisconnectCmd(uint8_t reason)
{
    return std::make_shared<SetMechDisconnectCmd>(reason);
}

std::shared_ptr<GetMechRealNameCmd> CommandFactory::CreateGetMechRealNameCmd()
{
    return std::make_shared<GetMechRealNameCmd>();
}

std::shared_ptr<NormalGetMechBaseInfoCmd> CommandFactory::CreateGetMechBaseInfoCmd()
{
    return std::make_shared<NormalGetMechBaseInfoCmd>();
}

std::shared_ptr<NormalGetMechPoseInfoCmd> CommandFactory::CreateGetMechPoseInfoCmd()
{
    return std::make_shared<NormalGetMechPoseInfoCmd>();
}

std::shared_ptr<NormalGetMechStateInfoCmd> CommandFactory::CreateGetMechStateInfoCmd()
{
    return std::make_shared<NormalGetMechStateInfoCmd>();
}

std::shared_ptr<NormalGetMechCoordinateInfoCmd> CommandFactory::CreateGetMechCoordinateInfoCmd()
{
    return std::make_shared<NormalGetMechCoordinateInfoCmd>();
}

std::shared_ptr<NormalSetMechProtocolVerCmd> CommandFactory::CreateNormalSetMechProtocolVerCmd(
    uint8_t protocolVer)
{
    return std::make_shared<NormalSetMechProtocolVerCmd>(protocolVer);
}

std::shared_ptr<CommonSetMechCameraInfoCmd> CommandFactory::CreateSetMechCameraInfoCmd(
    const CameraInfoParams& params)
{
    if (protocolVer_ == 0x01) {
        return std::make_shared<SetMechCameraInfoCmd>(params);
    } else if (protocolVer_ == 0x02) {
        return std::make_shared<NormalSetMechCameraInfoCmd>(params);
    } else {
        return std::make_shared<SetMechCameraInfoCmd>(params);
    }
}

std::shared_ptr<CommonSetMechCameraTrackingEnableCmd> CommandFactory::CreateSetMechCameraTrackingEnableCmd(
    MechTrackingStatus status)
{
    if (protocolVer_ == 0x01) {
        return std::make_shared<SetMechCameraTrackingEnableCmd>(status);
    } else if (protocolVer_ == 0x02) {
        return std::make_shared<NormalSetMechCameraTrackingEnableCmd>(status);
    } else {
        return std::make_shared<SetMechCameraTrackingEnableCmd>(status);
    }
}

std::shared_ptr<CommonSetMechCameraTrackingFrameCmd> CommandFactory::CreateSetMechCameraTrackingFrameCmd(
    const TrackingFrameParams& params)
{
    if (protocolVer_ == 0x01) {
        return std::make_shared<SetMechCameraTrackingFrameCmd>(params);
    } else if (protocolVer_ == 0x02) {
        return std::make_shared<NormalSetMechCameraTrackingFrameCmd>(params);
    } else {
        return std::make_shared<SetMechCameraTrackingFrameCmd>(params);
    }
}

std::shared_ptr<SetMechCameraTrackingLayoutCmd> CommandFactory::CreateSetMechCameraTrackingLayoutCmd(
    const LayoutParams& params)
{
    return std::make_shared<SetMechCameraTrackingLayoutCmd>(params);
}

std::shared_ptr<SetMechConfigCmd> CommandFactory::CreateSetMechConfigCmd(
    uint8_t configVersion)
{
    return std::make_shared<SetMechConfigCmd>(configVersion);
}

std::shared_ptr<SetMechHidPreemptiveCmd> CommandFactory::CreateSetMechHidPreemptiveCmd(
    bool isPreemptive)
{
    return std::make_shared<SetMechHidPreemptiveCmd>(isPreemptive);
}

std::shared_ptr<CommonSetMechRotationBySpeedCmd> CommandFactory::CreateSetMechRotationBySpeedCmd(
    const RotateBySpeedParam& params)
{
    if (protocolVer_ == 0x01) {
        return std::make_shared<SetMechRotationBySpeedCmd>(params);
    } else if (protocolVer_ == 0x02) {
        return std::make_shared<NormalSetMechRotationBySpeedCmd>(params);
    } else {
        return std::make_shared<SetMechRotationBySpeedCmd>(params);
    }
}

std::shared_ptr<NormalSetMechRotationToLocationCmd> CommandFactory::CreateSetMechRotationToLocationCmd(
    const RotateToLocationParam& params)
{
    return std::make_shared<NormalSetMechRotationToLocationCmd>(params);
}

std::shared_ptr<SetMechRotationCmd> CommandFactory::CreateSetMechRotationCmd(
    const RotateParam& params)
{
    return std::make_shared<SetMechRotationCmd>(params);
}

std::shared_ptr<CommonSetMechRotationTraceCmd> CommandFactory::CreateSetMechRotationTraceCmd(
    uint16_t taskId, const std::vector<RotateParam>& params)
{
    if (protocolVer_ == 0x01) {
        return std::make_shared<SetMechRotationTraceCmd>(params);
    } else if (protocolVer_ == 0x02) {
        return std::make_shared<NormalSetMechRotationTraceCmd>(taskId, params);
    } else {
        return std::make_shared<SetMechRotationTraceCmd>(params);
    }
}

std::shared_ptr<SetMechStopCmd> CommandFactory::CreateSetMechStopCmd()
{
    return std::make_shared<SetMechStopCmd>();
}

std::shared_ptr<NormalSetMechMotionControlCmd> CommandFactory::CreateSetMechMotionControlCmd(ControlCommand action)
{
    return std::make_shared<NormalSetMechMotionControlCmd>(action);
}

std::shared_ptr<NormalSetMechLocationReportCmd> CommandFactory::CreateSetMechLocationReportCmd(
    uint8_t reportSwitch, uint8_t reportFrequency)
{
    return std::make_shared<NormalSetMechLocationReportCmd>(reportSwitch, reportFrequency);
}

std::shared_ptr<CommonRegisterMechKeyEventCmd> CommandFactory::CreateRegisterMechCameraKeyEventCmd()
{
    if (protocolVer_ == 0x01) {
        return std::make_shared<RegisterMechCameraKeyEventCmd>();
    } else if (protocolVer_ == 0x02) {
        return std::make_shared<NormalRegisterMechKeyEventCmd>();
    } else {
        return std::make_shared<RegisterMechCameraKeyEventCmd>();
    }
}

std::shared_ptr<RegisterMechControlResultCmd> CommandFactory::CreateRegisterMechControlResultCmd()
{
    return std::make_shared<RegisterMechControlResultCmd>();
}

std::shared_ptr<CommonRegisterMechPositionInfoCmd> CommandFactory::CreateRegisterMechPositionInfoCmd()
{
    if (protocolVer_ == 0x01) {
        return std::make_shared<RegisterMechPositionInfoCmd>();
    } else if (protocolVer_ == 0x02) {
        return std::make_shared<NormalRegisterMechPositionInfoCmd>();
    } else {
        return std::make_shared<RegisterMechPositionInfoCmd>();
    }
}

std::shared_ptr<CommonRegisterMechStateInfoCmd> CommandFactory::CreateRegisterMechStateInfoCmd()
{
    if (protocolVer_ == 0x01) {
        return std::make_shared<RegisterMechStateInfoCmd>();
    } else if (protocolVer_ == 0x02) {
        return std::make_shared<NormalRegisterMechStateInfoCmd>();
    } else {
        return std::make_shared<RegisterMechStateInfoCmd>();
    }
}

std::shared_ptr<NormalRegisterMechGenericEventCmd> CommandFactory::CreateRegisterMechGenericEventCmd()
{
    return std::make_shared<NormalRegisterMechGenericEventCmd>();
}

std::shared_ptr<RegisterMechWheelDataCmd> CommandFactory::CreateRegisterMechWheelDataCmd()
{
    return std::make_shared<RegisterMechWheelDataCmd>();
}

std::shared_ptr<CommonRegisterMechTrackingEnableCmd> CommandFactory::CreateRegisterMechTrackingEnableCmd()
{
    return std::make_shared<NormalRegisterMechTrackingEnableCmd>();
}

std::shared_ptr<ActionGimbalFeatureControlCmd> CommandFactory::CreateActionGimbalFeatureControlCmd(
    const ActionControlParams &params)
{
    return std::make_shared<ActionGimbalFeatureControlCmd>(params);
}

std::shared_ptr<CommandBase> CommandFactory::CreateFromData(std::shared_ptr<MechDataBuffer> data)
{
    if (data->Size() < CREATE_FROM_DATA_MIN_SIZE) {
        return nullptr;
    }

    uint8_t cmdType = 0;
    uint8_t cmdId = 0;
    CHECK_ERR_RETURN_VALUE(data->ReadUint8(0, cmdType), nullptr, "read cmdType");
    CHECK_ERR_RETURN_VALUE(data->ReadUint8(BIT_OFFSET_1, cmdId), nullptr, "read cmdId");

    uint16_t type = (static_cast<uint16_t>(cmdType) << BIT_OFFSET_8) | cmdId;
    HILOGD("CmdType 0x%{public}x", type);

    switch (type) {
        case CMD_TYPE_BUTTON_EVENT_NOTIFY:
            return CreateAndUnmarshal<RegisterMechCameraKeyEventCmd>(data);
        case CMD_TYPE_PARAM_NOTIFY:
            return CreateAndUnmarshal<RegisterMechStateInfoCmd>(data);
        case CMD_TYPE_ATTITUDE_NOTIFY:
            return CreateAndUnmarshal<RegisterMechPositionInfoCmd>(data);
        case CMD_TYPE_EXE_RESULT_NOTIFY:
            return CreateAndUnmarshal<RegisterMechControlResultCmd>(data);
        case CMD_TYPE_WHEEL_DATA_NOTIFY:
            return CreateAndUnmarshal<RegisterMechWheelDataCmd>(data);
        case CMD_TYPE_TRACKING_ENABLED_NOTIFY:
            return CreateAndUnmarshal<NormalRegisterMechTrackingEnableCmd>(data);
        case CMD_TYPE_NORMAL_BUTTON_EVENT_NOTIFY:
            return CreateAndUnmarshal<NormalRegisterMechKeyEventCmd>(data);
        case CMD_TYPE_NORMAL_ATTITUDE_NOTIFY:
            return CreateAndUnmarshal<NormalRegisterMechPositionInfoCmd>(data);
        case CMD_TYPE_NORMAL_GENERIC_NOTIFY:
            return CreateAndUnmarshal<NormalRegisterMechGenericEventCmd>(data);
        default:
            return nullptr;
    }
    return nullptr;
}
} // namespace MechBodyController
} // namespace OHOS
