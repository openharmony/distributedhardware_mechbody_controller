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

#ifndef OHOS_MECHBODY_CONTROLLER_IPC_INTERFACE_CODE_H
#define OHOS_MECHBODY_CONTROLLER_IPC_INTERFACE_CODE_H

#include <string>

namespace OHOS {
namespace MechBodyController {
    const std::u16string MECH_SERVICE_IPC_TOKEN = u"ohos.mechservice.accessToken";
    static constexpr int32_t MECH_SERVICE_SA_ID = 7811;

enum class IMechBodyControllerCode : uint32_t {
    MECHBODY_RESERVED = 0,
    ATTACH_STATE_CHANGE_LISTEN_ON = 1,
    ATTACH_STATE_CHANGE_LISTEN_OFF = 2,
    GET_ATTACHED_DEVICES = 3,
    SET_USER_OPERATION = 4,
    SET_CAMERA_TRACKING_ENABLED = 5,
    GET_CAMERA_TRACKING_ENABLED = 6,
    TRACKING_EVENT_LISTEN_ON = 7,
    TRACKING_EVENT_LISTEN_OFF = 8,
    SET_CAMERA_TRACKING_LAYOUT = 9,
    GET_CAMERA_TRACKING_LAYOUT = 10,
    ROTATE_BY_DEGREE = 11,
    REGISTER_CMD_CHANNEL = 12,
    ROTATE_TO_EULER_ANGLES = 13,
    GET_MAX_ROTATION_TIME = 14,
    GET_MAX_ROTATION_SPEED = 15,
    ROTATE_BY_SPEED = 16,
    STOP_MOVING = 17,
    GET_ROTATION_ANGLES = 18,
    GET_ROTATION_DEGREE_LIMITS = 19,
    GET_ROTATION_AXES_STATUS = 20,
    ROTATION_AXES_STATUS_CHANGE_LISTEN_ON = 21,
    ROTATION_AXES_STATUS_CHANGE_LISTEN_OFF = 22,

    ATTACH_STATE_CHANGE_CALLBACK = 24,
    TRACKING_EVENT_CALLBACK = 25,
    ROTATION_AXES_STATUS_CHANGE_CALLBACK = 26,
    ROTATE_CALLBACK = 27,
};
} // namespace MechBodyController
} // namespace OHOS
#endif // OHOS_MECHBODY_CONTROLLER_IPC_INTERFACE_CODE_H
