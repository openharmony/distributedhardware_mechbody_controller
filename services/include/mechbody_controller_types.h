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

#ifndef MECHBODY_CONTROLLER_MECHBODY_CONTROLLER_TYPES_H
#define MECHBODY_CONTROLLER_MECHBODY_CONTROLLER_TYPES_H

#include <vector>
#include <functional>
#include <cstdint>
#include <memory>
#include "iremote_broker.h"
#include "ipc_skeleton.h"
#include "parcel.h"
#include "mechbody_controller_enums.h"
#include "mechbody_controller_utils.h"

namespace OHOS {
namespace MechBodyController {

struct TimeLimit {
    float min = 0;
    float max = 0;
};

struct MechInfo : public OHOS::Parcelable {
    int32_t mechId = 0;
    // device type
    MechType mechType = MechType::PORTABLE_GIMBAL;
    std::string mechName;
    std::string mac;
    AttachmentState state = AttachmentState::DETACHED;
    bool gattCoonectState = false;
    bool pairState = false;
    bool hidState = false;

public:
    MechInfo() = default;

    bool operator<(const MechInfo &other) const
    {
        return mechId < other.mechId;
    }

    bool Marshalling(OHOS::Parcel &parcel) const override
    {
        return parcel.WriteInt32(mechId) && parcel.WriteInt32(static_cast<int32_t>(mechType)) &&
               parcel.WriteString(mechName);
    }

    static MechInfo *Unmarshalling(OHOS::Parcel &parcel)
    {
        auto info = new MechInfo();
        int32_t type = 0;
        if (!parcel.ReadInt32(info->mechId) || !parcel.ReadInt32(type) || !parcel.ReadString(info->mechName)) {
            delete info;
            return nullptr;
        }
        info->mechType = static_cast<MechType>(type);
        return info;
    }

    std::string ToString() const
    {
        return "MechInfo { mechId=" + std::to_string(mechId) +
               ", mechType=" + std::to_string(static_cast<int>(mechType)) +
               ", mac=" + GetAnonymStr(mac) +
               ", mechName=" + GetAnonymStr(mechName) +
               ", state=" + std::to_string(static_cast<int>(state)) +
               ", gattCoonectState=" + std::to_string(gattCoonectState) +
               ", pairState=" + std::to_string(pairState) +
               ", hidState=" + std::to_string(hidState) +
               " }";
    }

    void SetGattConnectState(bool state)
    {
        gattCoonectState = state;
    }

    void SetPairState(bool state)
    {
        pairState = state;
    }

    void SetHidState(bool state)
    {
        hidState = state;
    }

    void SetAttachState(AttachmentState attachState)
    {
        state = attachState;
    }
};

struct EulerAngles : public OHOS::Parcelable {
    float yaw = 0;
    float roll = 0;
    float pitch = 0;

    EulerAngles() = default;
    EulerAngles(float yaw, float roll, float pitch) : yaw(yaw), roll(roll), pitch(pitch) {}

    bool operator!=(const EulerAngles& other) const
    {
        return (std::abs(yaw - other.yaw) > FLOAT_EPSILON) ||
            (std::abs(roll - other.roll) > FLOAT_EPSILON) ||
            (std::abs(pitch - other.pitch) > FLOAT_EPSILON);
    }

    bool Marshalling(OHOS::Parcel &parcel) const override
    {
        return parcel.WriteFloat(yaw) &&
                parcel.WriteFloat(roll) &&
                parcel.WriteFloat(pitch);
    }

    static EulerAngles *Unmarshalling(OHOS::Parcel &parcel)
    {
        auto obj = new EulerAngles();
        if (!parcel.ReadFloat(obj->yaw) ||
            !parcel.ReadFloat(obj->roll) ||
            !parcel.ReadFloat(obj->pitch)) {
            delete obj;
            return nullptr;
        }
        return obj;
    }

    std::string ToString() const
    {
        return "EulerAngles { yaw=" + std::to_string(yaw) +
                ", roll=" + std::to_string(roll) +
                ", pitch=" + std::to_string(pitch) + " }";
    }
};

struct RotateByDegreeParam : public OHOS::Parcelable {
    EulerAngles degree;
    int32_t duration = 0;
public:
    bool Marshalling(OHOS::Parcel &parcel) const override
    {
        return parcel.WriteParcelable(&degree) && parcel.WriteInt32(duration);
    }

    static RotateByDegreeParam *Unmarshalling(OHOS::Parcel &parcel)
    {
        auto param = new RotateByDegreeParam();
        auto degreePtr = parcel.ReadParcelable<EulerAngles>();
        if (!degreePtr) {
            delete param;
            return nullptr;
        }

        param->degree = *degreePtr;
        delete degreePtr;

        if (!parcel.ReadInt32(param->duration)) {
            delete param;
            return nullptr;
        }
        return param;
    }

    std::string ToString() const
    {
        return "RotateByDegreeParam { degree=" + degree.ToString() + ", duration=" + std::to_string(duration) + " }";
    }
};

struct RotateToEulerAnglesParam : public OHOS::Parcelable {
    EulerAngles angles;
    int32_t duration = 0;

public:
    bool Marshalling(OHOS::Parcel &parcel) const override
    {
        return parcel.WriteParcelable(&angles) &&
               parcel.WriteInt32(duration);
    }

    static RotateToEulerAnglesParam *Unmarshalling(OHOS::Parcel &parcel)
    {
        std::unique_ptr<RotateToEulerAnglesParam> param = std::make_unique<RotateToEulerAnglesParam>();
        if (param == nullptr) {
            return nullptr;
        }
        EulerAngles* angles = parcel.ReadParcelable<EulerAngles>();
        if (angles == nullptr) {
            return nullptr;
        }

        param->angles = *angles;
        if (!parcel.ReadInt32(param->duration)) {
            delete angles;
            return nullptr;
        }
        delete angles;
        return param.release();
    }

    std::string ToString() const
    {
        return "RotateToEulerAnglesParam { degree=" + angles.ToString() +
               ", duration=" + std::to_string(duration) +
               " }";
    }
};

struct RotateParam {
    EulerAngles degree;
    int32_t duration = 0;
    bool isRelative = false;
    uint8_t taskId = 0;

    RotateParam() = default;
    RotateParam(const EulerAngles &deg, int32_t dur = 0, bool isRel = false)
        : degree(deg),
          duration(dur),
          isRelative(isRel)
    {
    }

public:
    std::string ToString() const
    {
        return "RotateParam { degree=" + degree.ToString() + ", duration=" + std::to_string(duration) +
               ", isRelative=" + std::to_string(isRelative) + ", taskId=" + std::to_string(taskId) + " }";
    }
};

struct RotateSpeed : public OHOS::Parcelable {
    float yawSpeed = 0;
    float rollSpeed = 0;
    float pitchSpeed = 0;

    bool Marshalling(OHOS::Parcel &parcel) const override
    {
        return parcel.WriteFloat(yawSpeed) &&
               parcel.WriteFloat(rollSpeed) &&
               parcel.WriteFloat(pitchSpeed);
    }

    static RotateSpeed *Unmarshalling(OHOS::Parcel &parcel)
    {
        auto param = new RotateSpeed();
        if (!parcel.ReadFloat(param->yawSpeed) ||
            !parcel.ReadFloat(param->rollSpeed) ||
            !parcel.ReadFloat(param->pitchSpeed)) {
            delete param;
            return nullptr;
        }
        return param;
    }

    std::string ToString() const
    {
        return "RotateSpeed { yawSpeed=" + std::to_string(yawSpeed) +
               ", rollSpeed=" + std::to_string(rollSpeed) +
               ", pitchSpeed=" + std::to_string(pitchSpeed) + " }";
    }
};

struct RotateBySpeedParam : public OHOS::Parcelable {
    RotateSpeed speed;
    float duration = 0;

    bool Marshalling(OHOS::Parcel &parcel) const override
    {
        return speed.Marshalling(parcel) &&
               parcel.WriteFloat(duration);
    }

    static RotateBySpeedParam *Unmarshalling(OHOS::Parcel &parcel)
    {
        auto param = new RotateBySpeedParam();

        RotateSpeed *speed = RotateSpeed::Unmarshalling(parcel);
        if (speed == nullptr) {
            delete param;
            return nullptr;
        }
        param->speed = *speed;
        delete speed;

        if (!parcel.ReadFloat(param->duration)) {
            delete param;
            return nullptr;
        }
        return param;
    }

    std::string ToString() const
    {
        return "RotateBySpeedParam { " + speed.ToString() +
               ", duration=" + std::to_string(duration) + " }";
    }
};

struct RotateSpeedLimit : public OHOS::Parcelable {
    RotateSpeed speedMax;
    RotateSpeed speedMin;

    bool Marshalling(OHOS::Parcel &parcel) const override
    {
        return parcel.WriteParcelable(&speedMax) &&
               parcel.WriteParcelable(&speedMin);
    }

    static RotateSpeedLimit *Unmarshalling(OHOS::Parcel &parcel)
    {
        auto obj = new RotateSpeedLimit();
        auto speedMax = parcel.ReadParcelable<RotateSpeed>();
        auto speedMin = parcel.ReadParcelable<RotateSpeed>();
        if (!speedMax || !speedMin) {
            delete obj;
            delete speedMax;
            delete speedMin;
            return nullptr;
        }
        obj->speedMax = *speedMax;
        obj->speedMin = *speedMin;
        delete speedMax;
        delete speedMin;
        return obj;
    }

    std::string ToString() const
    {
        return "RotateSpeedLimit { speedMax=" + speedMax.ToString() +
               ", speedMin=" + speedMin.ToString() +
               " }";
    }
};

struct RotateDegreeLimit : public OHOS::Parcelable {
    EulerAngles negMax;
    EulerAngles posMax;

    bool Marshalling(OHOS::Parcel &parcel) const override
    {
        return parcel.WriteParcelable(&negMax) &&
               parcel.WriteParcelable(&posMax);
    }

    static RotateDegreeLimit *Unmarshalling(OHOS::Parcel &parcel)
    {
        auto obj = new RotateDegreeLimit();
        auto negMax = parcel.ReadParcelable<EulerAngles>();
        auto posMax = parcel.ReadParcelable<EulerAngles>();
        if (!negMax || !posMax) {
            delete obj;
            delete negMax;
            delete posMax;
            return nullptr;
        }
        obj->negMax = *negMax;
        obj->posMax = *posMax;
        delete negMax;
        delete posMax;
        return obj;
    }

    std::string ToString() const
    {
        return "RotateDegreeLimit { negMax=" + negMax.ToString() +
               ", posMax=" + posMax.ToString() +
               " }";
    }
};

struct RotationAxesStatus : public OHOS::Parcelable {
    bool yawEnabled = true;
    bool rollEnabled = true;
    bool pitchEnabled = true;
    RotationAxisLimited yawLimited = RotationAxisLimited::NOT_LIMITED;
    RotationAxisLimited rollLimited = RotationAxisLimited::NOT_LIMITED;
    RotationAxisLimited pitchLimited = RotationAxisLimited::NOT_LIMITED;

public:
    bool Marshalling(OHOS::Parcel &parcel) const override
    {
        return parcel.WriteBool(yawEnabled) &&
               parcel.WriteUint32(static_cast<uint32_t>(yawLimited)) &&
               parcel.WriteBool(rollEnabled) &&
               parcel.WriteUint32(static_cast<uint32_t>(rollLimited)) &&
               parcel.WriteBool(pitchEnabled) &&
               parcel.WriteUint32(static_cast<uint32_t>(pitchLimited));
    }

    static RotationAxesStatus *Unmarshalling(OHOS::Parcel &parcel)
    {
        RotationAxesStatus *status = new (std::nothrow) RotationAxesStatus();
        if (status == nullptr) {
            return nullptr;
        }

        uint32_t limitValue;
        if (!parcel.ReadBool(status->yawEnabled) ||
            !parcel.ReadUint32(limitValue)) {
            delete status;
            return nullptr;
        }
        status->yawLimited = static_cast<RotationAxisLimited>(limitValue);

        if (!parcel.ReadBool(status->rollEnabled) ||
            !parcel.ReadUint32(limitValue)) {
            delete status;
            return nullptr;
        }
        status->rollLimited = static_cast<RotationAxisLimited>(limitValue);

        if (!parcel.ReadBool(status->pitchEnabled) ||
            !parcel.ReadUint32(limitValue)) {
            delete status;
            return nullptr;
        }
        status->pitchLimited = static_cast<RotationAxisLimited>(limitValue);
        return status;
    }

    bool operator==(const RotationAxesStatus &other) const
    {
        return (yawEnabled == other.yawEnabled) && (rollEnabled == other.rollEnabled) &&
               (pitchEnabled == other.pitchEnabled) && (yawLimited == other.yawLimited) &&
               (rollLimited == other.rollLimited) && (pitchLimited == other.pitchLimited);
    }

    bool operator!=(const RotationAxesStatus &other) const
    {
        return (yawEnabled != other.yawEnabled) || (rollEnabled != other.rollEnabled) ||
               (pitchEnabled != other.pitchEnabled) || (yawLimited != other.yawLimited) ||
               (rollLimited != other.rollLimited) || (pitchLimited != other.pitchLimited);
    }

    bool IsChange(const RotationAxesStatus &other) const
    {
        return !((yawLimited == other.yawLimited) && (rollLimited == other.rollLimited)
            && (pitchLimited == other.pitchLimited));
    }

    std::string ToString() const
    {
        auto limitToString = [](RotationAxisLimited limit) {
            switch (limit) {
                case RotationAxisLimited::NOT_LIMITED:
                    return "NOT_LIMITED";
                case RotationAxisLimited::NEG_LIMITED:
                    return "NEG_LIMITED";
                case RotationAxisLimited::POS_LIMITED:
                    return "POS_LIMITED";
                default:
                    return "UNKNOWN";
            }
        };

        return "RotateAxisStatus{"
               "yawEnabled:" + std::to_string(yawEnabled) +
               ", yawLimited:" + limitToString(yawLimited) +
               ", rollEnabled:" + std::to_string(rollEnabled) +
               ", rollLimited:" + limitToString(rollLimited) +
               ", pitchEnabled:" + std::to_string(pitchEnabled) +
               ", pitchLimited:" + limitToString(pitchLimited) + "}";
    }
};

struct MechStateInfo {
    MechMode mechMode = MechMode::INVALID;
    bool isCaptureVertical = false;
    bool isPhoneOn = false;

    bool operator==(const MechStateInfo &other) const
    {
        return (mechMode == other.mechMode) && (isCaptureVertical == other.isCaptureVertical) &&
               (isPhoneOn == other.isPhoneOn);
    }
};

struct WheelData {
    int16_t degree = 0;
    int16_t speed = 0;
    uint8_t speed_ratio = 0;
};

struct LayoutParams {
    bool isDefault = true;
    float offsetX = 0;
    float offsetY = 0;

    std::string ToString() const
    {
        return "LayoutParams { isDefault=" + std::to_string(isDefault) +
               ", offsetX=" + std::to_string(offsetX) +
               ", offsetY=" + std::to_string(offsetY) +
               " }";
    }
};

struct CameraInfoParams {
    uint8_t fovV;
    uint8_t fovH;
    float zoomFactor = 0;
    bool isRecording = false;
    CameraType cameraType;
};

struct MechBaseInfo {
    bool obtainable = false;
    std::string mechType;
    std::string ctrlMode;
    std::string mechModel;
    std::string mechVendor;
};

struct CompositionRange {
    int32_t min = 0;
    int32_t max = 500;
};

struct MotionRange {
    int32_t min = 0;
    int32_t max = 500;
};

struct MechCapabilityInfo {
    bool obtainable = false;
    bool automaticReturn = false;
    bool hvSwitching = false;
    bool intelligentTracking = false;
    int32_t movingTrackQuantity = 0;
    CompositionRange x;
    CompositionRange y;
    MotionRange yaw;
    MotionRange roll;
    MotionRange pitch;
    int32_t yawMaxSpeed = 0;
    int32_t rollMaxSpeed = 0;
    int32_t pitchMaxSpeed = 0;
    int32_t reportingFrequency = 0;
};

struct ROI {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    std::string ToString() const
    {
        return "ROI {"
               "x=" + std::to_string(x) +
               ", y=" + std::to_string(y) +
               ", width=" + std::to_string(width) +
               ", height=" + std::to_string(height) +
               " }";
    }
};

struct TrackingFrameParams {
    uint16_t targetId = 0;
    uint64_t timeStamp;
    ConfidenceLevel confidence;
    uint8_t objectType;
    ROI roi;
    float fovV;
    float fovH;
    bool isRecording;
    uint32_t timeDelay;

    std::string ToString() const
    {
        return "TrackingFrameParams {"
               "targetId=" + std::to_string(targetId) +
               ", timeStamp=" + std::to_string(timeStamp) +
               ", confidence=" + std::to_string(static_cast<int>(confidence)) +
               ", objectType=" + std::to_string(objectType) +
               ", roi.x=" + std::to_string(roi.x) +
               ", roi.y=" + std::to_string(roi.y) +
               ", roi.width=" + std::to_string(roi.width) +
               ", roi.height=" + std::to_string(roi.height) +
               ", fovV=" + std::to_string(fovV) +
               ", fovH=" + std::to_string(fovH) +
               ", isRecording=" + std::to_string(isRecording) +
               ", timeDelay=" + std::to_string(timeDelay) +
               " }";
    }
};

struct TargetInfo : public OHOS::Parcelable {
    TargetType targetType = TargetType::FACE;

    bool Marshalling(OHOS::Parcel &parcel) const override
    {
        return parcel.WriteUint32(static_cast<uint32_t>(targetType));
    }

    static TargetInfo *Unmarshalling(OHOS::Parcel &parcel)
    {
        auto obj = new TargetInfo();
        uint32_t targetType;
        if (!parcel.ReadUint32(targetType)) {
            delete obj;
            return nullptr;
        }
        obj->targetType = static_cast<TargetType>(targetType);
        return obj;
    }

    std::string ToString() const
    {
        return "TargetInfo { targetType=" + std::to_string(static_cast<int>(targetType)) + " }";
    }
};

struct SearchParams : public OHOS::Parcelable {
    SearchDirection direction = SearchDirection::DEFAULT;

    bool Marshalling(OHOS::Parcel &parcel) const override
    {
        return parcel.WriteUint32(static_cast<uint32_t>(direction));
    }

    static SearchParams *Unmarshalling(OHOS::Parcel &parcel)
    {
        auto obj = new SearchParams();
        uint32_t direction;
        if (!parcel.ReadUint32(direction)) {
            delete obj;
            return nullptr;
        }
        obj->direction = static_cast<SearchDirection>(direction);
        return obj;
    }

    std::string ToString() const
    {
        return "SearchParams { direction=" + std::to_string(static_cast<int>(direction)) + " }";
    }
};

struct SearchResult : public OHOS::Parcelable {
    int32_t targetsNum = 0;
};

struct ActionControlParams {
    uint8_t controlReq = 0;
    uint16_t timeOut = 0;
    uint32_t yawControl = 0;
    uint32_t pitchControl = 0;
    uint32_t rollControl = 0;

    std::string ToString() const
    {
        return "ActionControlParams {"
               "controlReq=" + std::to_string(controlReq) +
               ", timeOut=" + std::to_string(timeOut) +
               ", yawControl=" + std::to_string(yawControl) +
               ", pitchControl=" + std::to_string(pitchControl) +
               ", rollControl=" + std::to_string(rollControl) + " }";
    }
};
}  // namespace MechBodyController
}  // namespace OHOS
#endif  // MECHBODY_CONTROLLER_MECHBODY_CONTROLLER_TYPES_H
