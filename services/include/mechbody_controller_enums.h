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

#ifndef MECHBODY_CONTROLLER_MECHBODY_CONTROLLER_ENUMS_H
#define MECHBODY_CONTROLLER_MECHBODY_CONTROLLER_ENUMS_H

namespace OHOS {
namespace MechBodyController {

constexpr uint8_t MC_CONFIG_VERSION = 0x00;

constexpr uint16_t CMD_TYPE_BUTTON_EVENT_NOTIFY = 0x0240;
constexpr uint16_t CMD_TYPE_PARAM_NOTIFY = 0x0241;
constexpr uint16_t CMD_TYPE_ATTITUDE_NOTIFY = 0x0242;
constexpr uint16_t CMD_TYPE_EXE_RESULT_NOTIFY = 0x0243;
constexpr uint16_t CMD_TYPE_WHEEL_DATA_NOTIFY = 0x0244;
constexpr uint16_t CMD_TYPE_TRACKING_ENABLED_NOTIFY = 0x0245;
constexpr uint16_t CMD_TYPE_NORMAL_BUTTON_EVENT_NOTIFY = 0x0246;
constexpr uint16_t CMD_TYPE_NORMAL_ATTITUDE_NOTIFY = 0x0247;
constexpr uint16_t CMD_TYPE_NORMAL_GENERIC_NOTIFY = 0x0248;

constexpr uint8_t BIT_0 = 0x00;
constexpr uint8_t BIT_1 = 0x01;
constexpr uint8_t BIT_2 = 0x02;
constexpr uint8_t BIT_3 = 0x03;
constexpr uint8_t BIT_5 = 0x05;
constexpr size_t BIT_OFFSET_0 = 0;
constexpr size_t BIT_OFFSET_1 = 1;
constexpr size_t BIT_OFFSET_2 = 2;
constexpr size_t BIT_OFFSET_3 = 3;
constexpr size_t BIT_OFFSET_4 = 4;
constexpr size_t BIT_OFFSET_6 = 6;
constexpr size_t BIT_OFFSET_7 = 7;
constexpr size_t BIT_OFFSET_8 = 8;
constexpr size_t BIT_OFFSET_10 = 10;
constexpr size_t BIT_OFFSET_13 = 13;
constexpr float FLOAT_EPSILON = 0.01f;
constexpr int32_t CMD_PRIORITY_HIGH = 5;
constexpr int32_t CMD_PRIORITY_MIDDLE = 3;
constexpr int32_t CMD_PRIORITY_LOW = 0;

constexpr int32_t DECIMAL_SCALE = 10;
constexpr int8_t ADDRESS_COUNT = 6;
enum class ProtocolCmdType : uint8_t {
    REQ = 0b0000,
    RSP = 0b0001,
    RPT = 0b0010
};

enum class OptType : uint8_t {
    PUT = 0x00,
    GET = 0x01,
    REPORT = 0x02
};

enum class CameraType : uint8_t {
    FRONT = 0x01,
    BACK = 0x02,
    OTHER = 0x03
};

enum class ConfidenceLevel : uint8_t {
    LOST = 0x00,
    HIGH = 0x01,
    MEDIUM = 0x02,
    LOW = 0x03
};

enum class CameraKeyEvent : int32_t {
    INVALID = -1,
    START_FILMING = 0,
    SWITCH_CAMERA = 1,
    ZOOM_IN = 2,
    ZOOM_OUT = 3,
    SWITCH_PHOTO_FILM = 4,
    SWITCH_TRACKING = 5,
};

enum class MechMode : uint8_t {
    FOLLOW = 0,
    PITCH_FREE = 1,
    FPV = 2,
    ROLL = 3,
    FREE = 4,
    INVALID = 0xFF,
};

enum class Operation : int32_t {
    CONNECT = 0,
    DISCONNECT = 1
};

enum class TrackingEvent : int32_t {
    CAMERA_TRACKING_USER_ENABLED = 0,
    CAMERA_TRACKING_USER_DISABLED = 1,
    CAMERA_TRACKING_LAYOUT_CHANGED = 2
};

enum class ExecResult : int32_t {
    COMPLETED = 0,
    INTERRUPTED = 1,
    LIMITED = 2,
    TIMEOUT = 3,
    SYSTEM_ERROR = 100
};

enum class MechType : int32_t {
    PORTABLE_GIMBAL = 0
};

enum class AttachmentState : int32_t {
    ATTACHED = 0,
    DETACHED = 1
};

enum class CameraTrackingLayout : int32_t {
    DEFAULT = 0,
    LEFT = 1,
    MIDDLE = 2,
    RIGHT = 3
};

enum class MovingStatus : int32_t {
    STILL = 0,
    MOVING = 1,
    LIMIT = 2
};

enum class RotationAxisLimited : uint32_t {
    NOT_LIMITED = 0,
    NEG_LIMITED = 1,
    POS_LIMITED = 2,
};

enum class PresetAction : int32_t {
    NOD = 0,
};

enum class MobileRotation : int32_t {
    INVALID = -1,
    UP = 0,
    RIGHT = 90,
    DOWN = 180,
    LEFT = 270,
};

enum class MechTrackingStatus : uint8_t {
    MECH_TK_DISABLE = 0,
    MECH_TK_ENABLE_NO_TARGET = 1,
    MECH_TK_ENABLE_WITH_TARGET = 2,
};

enum class TargetType : uint32_t {
    FACE = 0,
};

enum class SearchDirection : uint32_t {
    DEFAULT = 0,
    LEFTWARD = 1,
    RIGHTWARD = 2,
};

enum class MechKitStartResCode : int32_t {
    SUCCESS = 0,
    GATT_CONNECT_FAIL = 1,
    EVENT_HANDLER_INIT_FAIL = 2,
};

enum class ConnectType : int32_t {
    UNKNOWN = -1,
    BLUETOOTH = 0,
    NFC = 1
};

enum class ControlCommand : int32_t {
    UNKNOWN = -1,
    STOP = 0,
    GO_CENTER = 1,
    HORIZONTALLY_SWITCH = 2
};

enum class ProtocolVersion : int8_t {
    UNKNOWN = 0,
    V01 = 1,
    V02 = 2
};

}  // namespace MechBodyController
}  // namespace OHOS
#endif  // MECHBODY_CONTROLLER_MECHBODY_CONTROLLER_ENUMS_H
