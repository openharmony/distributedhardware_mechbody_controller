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

#include "mechbodycontrollerstubfive_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

#include "mechbody_controller_stub.h"
#include "mechbody_controller_service.h"
#include "mechbody_controller_ipc_interface_code.h"
#include "mechbody_controller_log.h"
#include "ipc_skeleton.h"
#include "message_option.h"
#include "message_parcel.h"
#include "securec.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "system_ability.h"

namespace OHOS {
using namespace OHOS::MechBodyController;
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

int32_t MechBodyControllerService::OnDeviceConnected(int32_t mechId)
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

int32_t MechBodyControllerService::SetTrackingEnabled(bool &isEnabled)
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

int32_t MechBodyControllerService::GetTrackingLayout(CameraTrackingLayout &cameraTrackingLayoutb)
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

void RotationAxesStatusChangeListenOffInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                                    const uint8_t *data,
                                                    size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ROTATION_AXES_STATUS_CHANGE_LISTEN_OFF);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void RotateToEulerAnglesInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
    const uint8_t *data,
    size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ROTATE_BY_DEGREE);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    auto mechBodyControllerStubPtr = std::make_shared<OHOS::MechBodyControllerService>();
    OHOS::RotationAxesStatusChangeListenOffInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::RotateToEulerAnglesInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    return 0;
}