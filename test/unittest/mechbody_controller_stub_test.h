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

#ifndef MECHBODY_CONTROLLER_STUB_TEST_H
#define MECHBODY_CONTROLLER_STUB_TEST_H

#include "gtest/gtest.h"
#include "mechbody_controller_stub.h"
#include "mechbody_controller_service.h"

namespace OHOS {
namespace MechBodyController {

class MechBodyControllerStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

MechBodyControllerService& MechBodyControllerService::GetInstance()
{
    static auto instance = new MechBodyControllerService();
    return *instance;
}

MechBodyControllerService::MechBodyControllerService()
{
}

MechBodyControllerService::~MechBodyControllerService()
{
}

int32_t MechBodyControllerService::OnDeviceConnected(
    int32_t mechId, bool isFirstConnect, const uint32_t &deviceIdentifier)
{
    return 0;
}

int32_t MechBodyControllerService::RegisterAttachStateChangeCallback(const sptr<IRemoteObject> callback)
{
    return 0;
}

int32_t MechBodyControllerService::UnRegisterAttachStateChangeCallback()
{
    return 0;
}

int32_t MechBodyControllerService::OnAttachStateChange(const AttachmentState &attachmentState,
    const MechInfo &mechInfo)
{
    return 0;
}

int32_t MechBodyControllerService::GetAttachedDevices(std::set<MechInfo> &mechInfo)
{
    return 0;
}

int32_t MechBodyControllerService::SetUserOperation(const std::shared_ptr<Operation> &operation,
    const std::string &mac, const std::string &param)
{
    return 0;
}

int32_t MechBodyControllerService::SetTrackingEnabled(bool &isEnabled, bool isCapsuleSet)
{
    return 0;
}

int32_t MechBodyControllerService::GetTrackingEnabled(bool &isEnabled)
{
    return 0;
}

int32_t MechBodyControllerService::RegisterTrackingEventCallback(sptr<IRemoteObject> callback)
{
    return 0;
}

int32_t MechBodyControllerService::UnRegisterTrackingEventCallback()
{
    return 0;
}

int32_t MechBodyControllerService::SetTrackingLayout(CameraTrackingLayout &cameraTrackingLayout)
{
    return 0;
}

int32_t MechBodyControllerService::GetTrackingLayout(CameraTrackingLayout &cameraTrackingLayout)
{
    return 0;
}

int32_t MechBodyControllerService::RegisterCmdChannel(const sptr<IRemoteObject> callback)
{
    return 0;
}

int32_t MechBodyControllerService::RotateByDegree(const int32_t &mechId, std::string &cmdId,
    const std::shared_ptr<RotateByDegreeParam> &rotateByDegreeParam)
{
    return 0;
}
    
int32_t MechBodyControllerService::RotateToEulerAngles(const int32_t &mechId, std::string &cmdId,
    const std::shared_ptr<RotateToEulerAnglesParam> &rotateToEulerAnglesParam)
{
    return 0;
}

int32_t MechBodyControllerService::GetMaxRotationTime(const int32_t &mechId, std::shared_ptr<TimeLimit> &speedLimit)
{
    return 0;
}

int32_t MechBodyControllerService::GetMaxRotationSpeed(const int32_t &mechId, RotateSpeedLimit &speedLimit)
{
    return 0;
}

int32_t MechBodyControllerService::RotateBySpeed(const int32_t &mechId, std::string &cmdId,
    const std::shared_ptr<RotateBySpeedParam> &rotateBySpeedParam)
{
    return 0;
}

int32_t MechBodyControllerService::StopMoving(const int32_t &mechId, std::string &cmdId)
{
    return 0;
}

int32_t MechBodyControllerService::GetRotationAngles(const int32_t &mechId, std::shared_ptr<EulerAngles> &eulerAngles)
{
    return 0;
}

int32_t MechBodyControllerService::GetRotationDegreeLimits(const int32_t &mechId, RotateDegreeLimit &rotationLimit)
{
    return 0;
}

int32_t MechBodyControllerService::GetRotationAxesStatus(const int32_t &mechId, RotationAxesStatus &axesStatus)
{
    return 0;
}

int32_t MechBodyControllerService::RegisterRotationAxesStatusChangeCallback(const sptr<IRemoteObject> callback)
{
    return 0;
}

int32_t MechBodyControllerService::UnRegisterRotationAxesStatusChangeCallback()
{
    return 0;
}

int32_t MechBodyControllerService::SearchTarget(std::string &napiCmdId,
    const std::shared_ptr<TargetInfo> &targetInfo, const std::shared_ptr<SearchParams> &searchParams)
{
    return 0;
}

int32_t MechBodyControllerService::Move(const int32_t &mechId, std::string &cmdId,
    const std::shared_ptr<MoveParams> &moveParams)
{
    return 0;
}

int32_t MechBodyControllerService::MoveBySpeed(const int32_t &mechId, std::string &cmdId, uint16_t duration,
    const std::shared_ptr<SpeedParams> &speedParams)
{
    return 0;
}

int32_t MechBodyControllerService::TurnBySpeed(const int32_t &mechId, std::string &cmdId,
    float angleSpeed, uint16_t duration)
{
    return 0;
}

int32_t MechBodyControllerService::IsSupportAction(const int32_t &mechId, ActionType actionType, bool &isSupport)
{
    return 0;
}

int32_t MechBodyControllerService::DoAction(const int32_t &mechId, std::string &cmdId, ActionType actionType)
{
    return 0;
}

int32_t MechBodyControllerService::SubscribeCallback(sptr <IRemoteObject> &callback, MechEventType mechEventType)
{
    return 0;
}

int32_t MechBodyControllerService::UnSubscribeCallback(MechEventType mechEventType)
{
    return 0;
}

int32_t MechBodyControllerService::OnDeviceDisconnected(int32_t mechId)
{
    return 0;
}

} // namespace MechBodyController
} // namespace OHOS
#endif // MECHBODY_CONTROLLER_STUB_TEST_H