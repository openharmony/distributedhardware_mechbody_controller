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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>
#include <securec.h>
#include <cstdint>

#include "mccommandfactory_fuzzer.h"
#include "mc_command_factory.h"
#include "mc_data_buffer.h"
#include "mechbody_controller_log.h"
#include "mechbody_controller_types.h"

using namespace OHOS::MechBodyController;

namespace {
    const std::string TAG = "McCommandFactoryFuzzTest";
    const uint32_t MAX_DATA_SIZE = 512;

    enum class TestFunctionId {
        FUZZ_FACTORY_SETTINGS = 0,
        FUZZ_FACTORY_CREATE_COMMANDS = 1,
        FUZZ_FACTORY_CREATE_COMMANDS_WITH_PARAMS = 2,
        FUZZ_FACTORY_CREATE_COMMANDS_WITH_COMPLEX_PARAMS = 3,
        FUZZ_FACTORY_CREATE_FROM_DATA = 4
    };
}

static void TestFactorySettings(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    CommandFactory factory;
    uint8_t protocolVer = provider.ConsumeIntegral<uint8_t>();
    uint8_t devType = provider.ConsumeIntegral<uint8_t>();

    factory.SetFactoryProtocolVer(protocolVer);
    factory.SetFactoryDevType(devType);
}

static void TestFactoryCreateCommands(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    CommandFactory factory;
    uint8_t protocolVer = provider.ConsumeIntegralInRange<uint8_t>(0, 3);
    factory.SetFactoryProtocolVer(protocolVer);

    factory.CreateGetMechCameraTrackingLayoutCmd();
    factory.CreateGetMechLimitInfoCmd();
    factory.CreateGetMechCapabilityInfoCmd();
    factory.CreateGetMechProtocolVerCmd();
    factory.CreateGetMechRealNameCmd();
    factory.CreateGetMechBaseInfoCmd();
    factory.CreateGetMechPoseInfoCmd();
    factory.CreateGetMechStateInfoCmd();
    factory.CreateGetMechCoordinateInfoCmd();
    factory.CreateSetMechStopCmd();
    factory.CreateRegisterMechControlResultCmd();
    factory.CreateRegisterMechWheelDataCmd();
    factory.CreateRegisterMechTrackingEnableCmd();
    factory.CreateRegisterMechGenericEventCmd();
    factory.CreateRegisterMechCliffInfoCmd();
    factory.CreateRegisterMechObstacleInfoCmd();
    factory.CreateRegisterMechCameraKeyEventCmd();
    factory.CreateRegisterMechPositionInfoCmd();
    factory.CreateRegisterMechStateInfoCmd();
    factory.CreateWheelGetMechCapabilityInfoCmd();
}

static void TestFactoryCreateCommandsWithParams(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    CommandFactory factory;
    uint8_t protocolVer = provider.ConsumeIntegralInRange<uint8_t>(0, 3);
    factory.SetFactoryProtocolVer(protocolVer);

    uint8_t reason = provider.ConsumeIntegral<uint8_t>();
    factory.CreateSetMechDisconnectCmd(reason);

    uint32_t deviceIdentifier = provider.ConsumeIntegral<uint32_t>();
    factory.CreateSetMechDevicePairingInfoCmd(deviceIdentifier);

    uint8_t cmdProtocolVer = provider.ConsumeIntegral<uint8_t>();
    factory.CreateNormalSetMechProtocolVerCmd(cmdProtocolVer);

    MechTrackingStatus status = static_cast<MechTrackingStatus>(provider.ConsumeIntegral<uint8_t>());
    factory.CreateSetMechCameraTrackingEnableCmd(status);

    uint8_t configVersion = provider.ConsumeIntegral<uint8_t>();
    factory.CreateSetMechConfigCmd(configVersion);

    bool isPreemptive = provider.ConsumeBool();
    factory.CreateSetMechHidPreemptiveCmd(isPreemptive);

    ControlCommand action = static_cast<ControlCommand>(provider.ConsumeIntegral<uint8_t>());
    factory.CreateSetMechMotionControlCmd(action);

    uint8_t reportSwitch = provider.ConsumeIntegral<uint8_t>();
    uint8_t reportFrequency = provider.ConsumeIntegral<uint8_t>();
    factory.CreateSetMechLocationReportCmd(reportSwitch, reportFrequency);

    ActionType actionType = static_cast<ActionType>(provider.ConsumeIntegral<uint8_t>());
    factory.CreateWheelSetMechSceneControlCmd(actionType);

    factory.CreateWheelSetMechMotionControlCmd(action);
}

static void TestCameraCommands(FuzzedDataProvider &provider, CommandFactory &factory)
{
    CameraInfoParams cameraParams;
    cameraParams.fovV = provider.ConsumeIntegral<uint8_t>();
    cameraParams.fovH = provider.ConsumeIntegral<uint8_t>();
    cameraParams.zoomFactor = provider.ConsumeFloatingPoint<float>();
    cameraParams.isRecording = provider.ConsumeBool();
    cameraParams.cameraType = static_cast<CameraType>(provider.ConsumeIntegral<uint8_t>());
    factory.CreateSetMechCameraInfoCmd(cameraParams);

    TrackingFrameParams trackingParams;
    trackingParams.targetId = provider.ConsumeIntegral<uint16_t>();
    trackingParams.timeStamp = provider.ConsumeIntegral<uint64_t>();
    trackingParams.cameraType = static_cast<CameraType>(provider.ConsumeIntegral<uint8_t>());
    trackingParams.confidence = static_cast<ConfidenceLevel>(provider.ConsumeIntegral<uint8_t>());
    trackingParams.objectType = provider.ConsumeIntegral<uint8_t>();
    trackingParams.roi.x = provider.ConsumeFloatingPoint<float>();
    trackingParams.roi.y = provider.ConsumeFloatingPoint<float>();
    trackingParams.roi.width = provider.ConsumeFloatingPoint<float>();
    trackingParams.roi.height = provider.ConsumeFloatingPoint<float>();
    trackingParams.fovV = provider.ConsumeFloatingPoint<float>();
    trackingParams.fovH = provider.ConsumeFloatingPoint<float>();
    trackingParams.isRecording = provider.ConsumeBool();
    trackingParams.timeDelay = provider.ConsumeIntegral<uint32_t>();
    factory.CreateSetMechCameraTrackingFrameCmd(trackingParams);

    LayoutParams layoutParams;
    layoutParams.isDefault = provider.ConsumeBool();
    layoutParams.offsetX = provider.ConsumeFloatingPoint<float>();
    layoutParams.offsetY = provider.ConsumeFloatingPoint<float>();
    factory.CreateSetMechCameraTrackingLayoutCmd(layoutParams);
}

static void TestRotationCommands(FuzzedDataProvider &provider, CommandFactory &factory)
{
    RotateBySpeedParam rotateSpeedParam;
    rotateSpeedParam.speed.yawSpeed = provider.ConsumeFloatingPoint<float>();
    rotateSpeedParam.speed.rollSpeed = provider.ConsumeFloatingPoint<float>();
    rotateSpeedParam.speed.pitchSpeed = provider.ConsumeFloatingPoint<float>();
    rotateSpeedParam.duration = provider.ConsumeFloatingPoint<float>();
    factory.CreateSetMechRotationBySpeedCmd(rotateSpeedParam);

    RotateToLocationParam rotateLocationParam;
    rotateLocationParam.taskId = provider.ConsumeIntegral<uint16_t>();
    rotateLocationParam.rotateMap = provider.ConsumeIntegral<uint8_t>();
    rotateLocationParam.rotateTime = provider.ConsumeIntegral<uint16_t>();
    rotateLocationParam.yawRadian = provider.ConsumeFloatingPoint<float>();
    rotateLocationParam.rollRadian = provider.ConsumeFloatingPoint<float>();
    rotateLocationParam.pitchRadian = provider.ConsumeFloatingPoint<float>();
    factory.CreateSetMechRotationToLocationCmd(rotateLocationParam);

    RotateParam rotateParam;
    rotateParam.degree.yaw = provider.ConsumeFloatingPoint<float>();
    rotateParam.degree.roll = provider.ConsumeFloatingPoint<float>();
    rotateParam.degree.pitch = provider.ConsumeFloatingPoint<float>();
    rotateParam.duration = provider.ConsumeIntegral<int32_t>();
    rotateParam.isRelative = provider.ConsumeBool();
    rotateParam.taskId = provider.ConsumeIntegral<uint8_t>();
    rotateParam.forwardSpeed = provider.ConsumeIntegral<int16_t>();
    rotateParam.turningSpeed = provider.ConsumeFloatingPoint<float>();
    factory.CreateSetMechRotationCmd(rotateParam);

    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();
    uint32_t vectorSize = provider.ConsumeIntegralInRange<uint32_t>(0, 5);
    std::vector<RotateParam> params;
    for (uint32_t i = 0; i < vectorSize; ++i) {
        RotateParam param;
        param.degree.yaw = provider.ConsumeFloatingPoint<float>();
        param.degree.roll = provider.ConsumeFloatingPoint<float>();
        param.degree.pitch = provider.ConsumeFloatingPoint<float>();
        param.duration = provider.ConsumeIntegral<int32_t>();
        param.isRelative = provider.ConsumeBool();
        param.taskId = provider.ConsumeIntegral<uint8_t>();
        param.forwardSpeed = provider.ConsumeIntegral<int16_t>();
        param.turningSpeed = provider.ConsumeFloatingPoint<float>();
        params.push_back(param);
    }
    factory.CreateSetMechRotationTraceCmd(taskId, params);
}

static void TestWheelCommands(FuzzedDataProvider &provider, CommandFactory &factory)
{
    uint16_t taskId = provider.ConsumeIntegral<uint16_t>();

    RotateToBaseParam rotateBaseParam(
        provider.ConsumeIntegral<uint16_t>(),
        provider.ConsumeIntegral<uint16_t>(),
        provider.ConsumeIntegral<int16_t>(),
        provider.ConsumeFloatingPoint<float>()
    );
    factory.CreateWheelSetMechRotationToBaseCmd(rotateBaseParam);

    uint32_t wheelVectorSize = provider.ConsumeIntegralInRange<uint32_t>(0, 5);
    std::vector<WheelPositionInfo> wheelParams;
    for (uint32_t i = 0; i < wheelVectorSize; ++i) {
        WheelPositionInfo wheelParam;
        wheelParam.xCoordinates = provider.ConsumeIntegral<int16_t>();
        wheelParam.yCoordinates = provider.ConsumeIntegral<int16_t>();
        wheelParams.push_back(wheelParam);
    }
    factory.CreateWheelSetMechRelativePositionCmd(taskId, wheelParams);
}

static void TestOtherCommands(FuzzedDataProvider &provider, CommandFactory &factory)
{
    ScreenInfoParams screenInfo;
    screenInfo.isPortrait = provider.ConsumeBool();
    factory.CreateSetMechPhoneStatusCmd(screenInfo);

    ActionControlParams actionParams;
    actionParams.controlReq = provider.ConsumeIntegral<uint8_t>();
    actionParams.timeOut = provider.ConsumeIntegral<uint16_t>();
    actionParams.yawControl = provider.ConsumeIntegral<uint32_t>();
    actionParams.pitchControl = provider.ConsumeIntegral<uint32_t>();
    actionParams.rollControl = provider.ConsumeIntegral<uint32_t>();
    factory.CreateActionGimbalFeatureControlCmd(actionParams);
}

static void TestFactoryCreateCommandsWithComplexParams(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    CommandFactory factory;
    uint8_t protocolVer = provider.ConsumeIntegralInRange<uint8_t>(0, 3);
    factory.SetFactoryProtocolVer(protocolVer);

    TestCameraCommands(provider, factory);
    TestRotationCommands(provider, factory);
    TestWheelCommands(provider, factory);
    TestOtherCommands(provider, factory);
}

static void TestFactoryCreateFromData(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    CommandFactory factory;

    uint32_t dataSize = provider.ConsumeIntegralInRange<uint32_t>(0, MAX_DATA_SIZE);
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);

    if (dataBuffer == nullptr) {
        return;
    }

    if (dataSize > 0) {
        std::vector<uint8_t> testData = provider.ConsumeBytes<uint8_t>(dataSize);
        if (!testData.empty()) {
            dataBuffer->SetRange(0, dataSize);
            if (memcpy_s(dataBuffer->Data(), dataBuffer->Size(), testData.data(), dataSize) != 0) {
                return;
            }
        }
    }

    factory.CreateFromData(dataBuffer);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < 1) {
        return 0;
    }

    FuzzedDataProvider provider(data, size);
    uint8_t testType = provider.ConsumeIntegral<uint8_t>() % 5;

    switch (static_cast<TestFunctionId>(testType)) {
        case TestFunctionId::FUZZ_FACTORY_SETTINGS:
            TestFactorySettings(data, size);
            break;
        case TestFunctionId::FUZZ_FACTORY_CREATE_COMMANDS:
            TestFactoryCreateCommands(data, size);
            break;
        case TestFunctionId::FUZZ_FACTORY_CREATE_COMMANDS_WITH_PARAMS:
            TestFactoryCreateCommandsWithParams(data, size);
            break;
        case TestFunctionId::FUZZ_FACTORY_CREATE_COMMANDS_WITH_COMPLEX_PARAMS:
            TestFactoryCreateCommandsWithComplexParams(data, size);
            break;
        case TestFunctionId::FUZZ_FACTORY_CREATE_FROM_DATA:
            TestFactoryCreateFromData(data, size);
            break;
        default:
            break;
    }

    return 0;
}