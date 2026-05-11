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

#include <fuzzer/FuzzedDataProvider.h>
#include "mcmotionmanager_fuzzer.h"
#include "mc_motion_manager.h"
#include "mc_connect_manager.h"
#include "mechbody_controller_types.h"
#include "mechbody_controller_enums.h"
#include <memory>

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

// Fuzz test constants
constexpr uint32_t MIN_GET_COUNT = 1;
constexpr uint32_t MAX_GET_COUNT = 5;
constexpr int32_t MIN_ACTION_TYPE = 0;
constexpr int32_t MAX_ACTION_TYPE = 2003;
constexpr int32_t MIN_TEST_ID = 0;
constexpr int32_t MAX_TEST_ID = 15;
constexpr int32_t MOTION_MANAGER_TYPE = 1;

enum class TestFunctionId {
    FUZZ_GET_SPEED_CONTROL_TIME_LIMIT = 0,
    FUZZ_GET_ROTATE_SPEED_LIMIT = 1,
    FUZZ_GET_CURRENT_POSITION = 2,
    FUZZ_GET_ROTATION_LIMIT = 3,
    FUZZ_GET_MECH_CAMERA_TRACKING_ENABLED = 4,
    FUZZ_GET_MECH_BASE_INFO = 5,
    FUZZ_GET_MECH_CAPABILITY_INFO = 6,
    FUZZ_GET_ROTATION_AXES_STATUS = 7,
    FUZZ_IS_SUPPORT_ACTION = 8,
    FUZZ_GET_SPEED_CONTROL_TIME_LIMIT_WITH_NULL = 9,
    FUZZ_GET_CURRENT_POSITION_WITH_NULL = 10,
    FUZZ_GET_MECH_BASE_INFO_WITH_NULL = 11,
    FUZZ_GET_MECH_CAPABILITY_INFO_WITH_NULL = 12,
    FUZZ_IS_SUPPORT_ACTION_WITH_BOUNDARY_VALUES = 13,
    FUZZ_GET_ROTATION_AXES_STATUS_WITH_BOUNDARY_VALUES = 14,
    FUZZ_MULTIPLE_GET_OPERATIONS = 15
};

class MockTransportSendAdapter : public TransportSendAdapter {
public:
    MockTransportSendAdapter() = default;
    ~MockTransportSendAdapter() override = default;
    int32_t SendCommand(const std::shared_ptr<CommandBase>& command, int32_t delay = 0) { return 0; }
};

std::shared_ptr<MotionManager> g_motionManager = nullptr;
std::shared_ptr<MockTransportSendAdapter> g_mockAdapter = nullptr;

void InitMotionManager()
{
    if (g_motionManager == nullptr) {
        g_mockAdapter = std::make_shared<MockTransportSendAdapter>();
        g_motionManager = std::make_shared<MotionManager>(g_mockAdapter, MOTION_MANAGER_TYPE);
    }
}

void FuzzGetSpeedControlTimeLimit(FuzzedDataProvider &provider)
{
    InitMotionManager();
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        std::shared_ptr<TimeLimit> timeLimit = nullptr;
        g_motionManager->GetSpeedControlTimeLimit(timeLimit);
    }
}

void FuzzGetRotateSpeedLimit(FuzzedDataProvider &provider)
{
    InitMotionManager();
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        RotateSpeedLimit speedLimit;
        g_motionManager->GetRotateSpeedLimit(speedLimit);
    }
}

void FuzzGetCurrentPosition(FuzzedDataProvider &provider)
{
    InitMotionManager();
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        std::shared_ptr<EulerAngles> eulerAngles = nullptr;
        g_motionManager->GetCurrentPosition(eulerAngles);
    }
}

void FuzzGetRotationLimit(FuzzedDataProvider &provider)
{
    InitMotionManager();
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        RotateDegreeLimit rotationLimit;
        g_motionManager->GetRotationLimit(rotationLimit);
    }
}

void FuzzGetMechCameraTrackingEnabled(FuzzedDataProvider &provider)
{
    InitMotionManager();
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        bool isEnabled = false;
        g_motionManager->GetMechCameraTrackingEnabled(isEnabled);
    }
}

void FuzzGetMechBaseInfo(FuzzedDataProvider &provider)
{
    InitMotionManager();
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        std::shared_ptr<MechBaseInfo> mechBaseInfo = nullptr;
        g_motionManager->GetMechBaseInfo(mechBaseInfo);
    }
}

void FuzzGetMechCapabilityInfo(FuzzedDataProvider &provider)
{
    InitMotionManager();
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        std::shared_ptr<MechCapabilityInfo> mechCapabilityInfo = nullptr;
        g_motionManager->GetMechCapabilityInfo(mechCapabilityInfo);
    }
}

void FuzzGetRotationAxesStatus(FuzzedDataProvider &provider)
{
    InitMotionManager();
    int32_t mechId = provider.ConsumeIntegral<int32_t>();
    RotationAxesStatus axesStatus;
    g_motionManager->GetRotationAxesStatus(mechId, axesStatus);
}

void FuzzIsSupportAction(FuzzedDataProvider &provider)
{
    InitMotionManager();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    ActionType actionType = static_cast<ActionType>(
        provider.ConsumeIntegralInRange<int32_t>(MIN_ACTION_TYPE, MAX_ACTION_TYPE));
    bool isSupport = false;
    g_motionManager->IsSupportAction(tokenId, actionType, isSupport);
}

void FuzzGetSpeedControlTimeLimitWithNull(FuzzedDataProvider &provider)
{
    InitMotionManager();
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        std::shared_ptr<TimeLimit> timeLimit = nullptr;
        g_motionManager->GetSpeedControlTimeLimit(timeLimit);
        if (timeLimit != nullptr) {
            timeLimit->min = provider.ConsumeFloatingPoint<float>();
            timeLimit->max = provider.ConsumeFloatingPoint<float>();
        }
    }
}

void FuzzGetCurrentPositionWithNull(FuzzedDataProvider &provider)
{
    InitMotionManager();
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        std::shared_ptr<EulerAngles> eulerAngles = nullptr;
        g_motionManager->GetCurrentPosition(eulerAngles);
        if (eulerAngles != nullptr) {
            eulerAngles->yaw = provider.ConsumeFloatingPoint<float>();
            eulerAngles->roll = provider.ConsumeFloatingPoint<float>();
            eulerAngles->pitch = provider.ConsumeFloatingPoint<float>();
        }
    }
}

void FuzzGetMechBaseInfoWithNull(FuzzedDataProvider &provider)
{
    InitMotionManager();
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        std::shared_ptr<MechBaseInfo> mechBaseInfo = nullptr;
        g_motionManager->GetMechBaseInfo(mechBaseInfo);
        if (mechBaseInfo != nullptr) {
            mechBaseInfo->obtainable = provider.ConsumeBool();
            mechBaseInfo->mechType = provider.ConsumeRandomLengthString();
            mechBaseInfo->ctrlMode = provider.ConsumeRandomLengthString();
            mechBaseInfo->mechModel = provider.ConsumeRandomLengthString();
            mechBaseInfo->mechVendor = provider.ConsumeRandomLengthString();
        }
    }
}

void FuzzGetMechCapabilityInfoWithNull(FuzzedDataProvider &provider)
{
    InitMotionManager();
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        std::shared_ptr<MechCapabilityInfo> mechCapabilityInfo = nullptr;
        g_motionManager->GetMechCapabilityInfo(mechCapabilityInfo);
        if (mechCapabilityInfo != nullptr) {
            mechCapabilityInfo->obtainable = provider.ConsumeBool();
            mechCapabilityInfo->automaticReturn = provider.ConsumeBool();
            mechCapabilityInfo->hvSwitching = provider.ConsumeBool();
            mechCapabilityInfo->intelligentTracking = provider.ConsumeBool();
            mechCapabilityInfo->movingTrackQuantity = provider.ConsumeIntegral<int32_t>();
            mechCapabilityInfo->yawMaxSpeed = provider.ConsumeIntegral<int32_t>();
            mechCapabilityInfo->rollMaxSpeed = provider.ConsumeIntegral<int32_t>();
            mechCapabilityInfo->pitchMaxSpeed = provider.ConsumeIntegral<int32_t>();
            mechCapabilityInfo->reportingFrequency = provider.ConsumeIntegral<int32_t>();
        }
    }
}

void FuzzIsSupportActionWithBoundaryValues(FuzzedDataProvider &provider)
{
    InitMotionManager();
    uint32_t tokenId = provider.ConsumeBool() ? MIN_TEST_ID : UINT32_MAX;
    ActionType actionType = static_cast<ActionType>(
        provider.ConsumeIntegralInRange<int32_t>(MIN_ACTION_TYPE, MAX_ACTION_TYPE));
    bool isSupport = false;
    g_motionManager->IsSupportAction(tokenId, actionType, isSupport);
}

void FuzzGetRotationAxesStatusWithBoundaryValues(FuzzedDataProvider &provider)
{
    InitMotionManager();
    int32_t mechId = provider.ConsumeBool() ? INT32_MIN : INT32_MAX;
    RotationAxesStatus axesStatus;
    g_motionManager->GetRotationAxesStatus(mechId, axesStatus);
}

void FuzzMultipleGetOperations(FuzzedDataProvider &provider)
{
    InitMotionManager();
    
    // Use fuzz data to control number of iterations
    uint32_t iterations = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    
    for (uint32_t i = 0; i < iterations; i++) {
        std::shared_ptr<TimeLimit> timeLimit = nullptr;
        g_motionManager->GetSpeedControlTimeLimit(timeLimit);
        
        RotateSpeedLimit speedLimit;
        g_motionManager->GetRotateSpeedLimit(speedLimit);
        
        std::shared_ptr<EulerAngles> eulerAngles = nullptr;
        g_motionManager->GetCurrentPosition(eulerAngles);
        
        RotateDegreeLimit rotationLimit;
        g_motionManager->GetRotationLimit(rotationLimit);
        
        bool isEnabled = false;
        g_motionManager->GetMechCameraTrackingEnabled(isEnabled);
        
        int32_t mechId = provider.ConsumeIntegral<int32_t>();
        RotationAxesStatus axesStatus;
        g_motionManager->GetRotationAxesStatus(mechId, axesStatus);
    }
}

void RunFuzzTest(FuzzedDataProvider &provider, int32_t testFunctionId)
{
    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_GET_CURRENT_POSITION_WITH_NULL:
            FuzzGetCurrentPositionWithNull(provider);
            break;
        case TestFunctionId::FUZZ_GET_MECH_BASE_INFO_WITH_NULL:
            FuzzGetMechBaseInfoWithNull(provider);
            break;
        case TestFunctionId::FUZZ_GET_MECH_CAPABILITY_INFO_WITH_NULL:
            FuzzGetMechCapabilityInfoWithNull(provider);
            break;
        case TestFunctionId::FUZZ_IS_SUPPORT_ACTION_WITH_BOUNDARY_VALUES:
            FuzzIsSupportActionWithBoundaryValues(provider);
            break;
        case TestFunctionId::FUZZ_GET_ROTATION_AXES_STATUS_WITH_BOUNDARY_VALUES:
            FuzzGetRotationAxesStatusWithBoundaryValues(provider);
            break;
        case TestFunctionId::FUZZ_MULTIPLE_GET_OPERATIONS:
            FuzzMultipleGetOperations(provider);
            break;
        default:
            break;
    }
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(MIN_TEST_ID, MAX_TEST_ID);
    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_GET_SPEED_CONTROL_TIME_LIMIT:
            FuzzGetSpeedControlTimeLimit(provider);
            break;
        case TestFunctionId::FUZZ_GET_ROTATE_SPEED_LIMIT:
            FuzzGetRotateSpeedLimit(provider);
            break;
        case TestFunctionId::FUZZ_GET_CURRENT_POSITION:
            FuzzGetCurrentPosition(provider);
            break;
        case TestFunctionId::FUZZ_GET_ROTATION_LIMIT:
            FuzzGetRotationLimit(provider);
            break;
        case TestFunctionId::FUZZ_GET_MECH_CAMERA_TRACKING_ENABLED:
            FuzzGetMechCameraTrackingEnabled(provider);
            break;
        case TestFunctionId::FUZZ_GET_MECH_BASE_INFO:
            FuzzGetMechBaseInfo(provider);
            break;
        case TestFunctionId::FUZZ_GET_MECH_CAPABILITY_INFO:
            FuzzGetMechCapabilityInfo(provider);
            break;
        case TestFunctionId::FUZZ_GET_ROTATION_AXES_STATUS:
            FuzzGetRotationAxesStatus(provider);
            break;
        case TestFunctionId::FUZZ_IS_SUPPORT_ACTION:
            FuzzIsSupportAction(provider);
            break;
        case TestFunctionId::FUZZ_GET_SPEED_CONTROL_TIME_LIMIT_WITH_NULL:
            FuzzGetSpeedControlTimeLimitWithNull(provider);
            break;
        default:
            RunFuzzTest(provider, testFunctionId);
            break;
    }
    return 0;
}