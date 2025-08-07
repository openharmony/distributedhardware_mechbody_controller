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

 #include "mechbodycontrollerstub_fuzzer.h"

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

void AttachStateChangeListenOnInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                            const uint8_t *data,
                                            size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_ON);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void AttachStateChangeListenOffInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                             const uint8_t *data,
                                             size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ATTACH_STATE_CHANGE_LISTEN_OFF);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void GetAttachedDevicesInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                     const uint8_t *data,
                                     size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_ATTACHED_DEVICES);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void SetUserOperationInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                   const uint8_t *data,
                                   size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::SET_USER_OPERATION);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void SetCameraTrackingEnabledInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                           const uint8_t *data,
                                           size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::SET_CAMERA_TRACKING_ENABLED);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void GetCameraTrackingEnabledInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                           const uint8_t *data,
                                           size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_CAMERA_TRACKING_ENABLED);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void TrackingEventListenOnInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                        const uint8_t *data,
                                        size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::TRACKING_EVENT_LISTEN_ON);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void TrackingEventListenOffInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                         const uint8_t *data,
                                         size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::TRACKING_EVENT_LISTEN_OFF);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void SetCameraTrackingLayoutInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                          const uint8_t *data,
                                          size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::SET_CAMERA_TRACKING_LAYOUT);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void GetCameraTrackingLayoutInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                          const uint8_t *data,
                                          size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_CAMERA_TRACKING_LAYOUT);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void RegisterCmdChannelInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                     const uint8_t *data,
                                     size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::REGISTER_CMD_CHANNEL);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void RotateByDegreeInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
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

void GetMaxRotationTimeInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                     const uint8_t *data,
                                     size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_MAX_ROTATION_TIME);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void GetMaxRotationSpeedInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                      const uint8_t *data,
                                      size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_MAX_ROTATION_SPEED);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void RotateBySpeedInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                const uint8_t *data,
                                size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ROTATE_BY_SPEED);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void StopMovingInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                             const uint8_t *data,
                             size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::STOP_MOVING);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void GetRotationAnglesInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                    const uint8_t *data,
                                    size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_ROTATION_ANGLES);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void GetRotationDegreeLimitsInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                          const uint8_t *data,
                                          size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_ROTATION_DEGREE_LIMITS);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void GetRotationAxesStatusInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                        const uint8_t *data,
                                        size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::GET_ROTATION_AXES_STATUS);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
}

void RotationAxesStatusChangeListenOnInnerFuzzTest(std::shared_ptr<MechBodyControllerStub> mechBodyControllerStubPtr,
                                                   const uint8_t *data,
                                                   size_t size)
{
    uint32_t code = static_cast<uint32_t>(IMechBodyControllerCode::ROTATION_AXES_STATUS_CHANGE_LISTEN_ON);
    MessageParcel datas;
    datas.WriteInterfaceToken(MECH_SERVICE_IPC_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    mechBodyControllerStubPtr->OnRemoteRequest(code, datas, reply, option);
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
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    auto mechBodyControllerStubPtr = std::make_shared<OHOS::MechBodyControllerService>();
    OHOS::AttachStateChangeListenOnInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::AttachStateChangeListenOffInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::GetAttachedDevicesInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::SetUserOperationInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::SetCameraTrackingEnabledInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::GetCameraTrackingEnabledInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::TrackingEventListenOnInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::TrackingEventListenOffInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::SetCameraTrackingLayoutInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::GetCameraTrackingLayoutInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::RegisterCmdChannelInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::RotateByDegreeInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::GetMaxRotationTimeInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::GetMaxRotationSpeedInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::RotateBySpeedInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::StopMovingInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::GetRotationAnglesInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::GetRotationDegreeLimitsInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::GetRotationAxesStatusInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::RotationAxesStatusChangeListenOnInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    OHOS::RotationAxesStatusChangeListenOffInnerFuzzTest(mechBodyControllerStubPtr, data, size);
    return 0;
}