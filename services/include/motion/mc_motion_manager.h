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

#ifndef OHOS_EB_MOTION_MANAGER_H
#define OHOS_EB_MOTION_MANAGER_H

#include <string>
#include <mutex>
#include <thread>
#include <sys/prctl.h>

#include "event_handler.h"
#include "mc_send_adapter.h"
#include "iremote_broker.h"
#include "ipc_skeleton.h"
#include "mechbody_controller_types.h"
#include "key_event.h"
#include "input_manager.h"
#include "mc_command_factory.h"

namespace OHOS {
namespace MechBodyController {

struct DeviceStatus {
    bool isEnabled = true;
    MechStateInfo stateInfo;
    LayoutParams layoutParams;
    int32_t duration = 500;
    bool yawLimit = false;
    bool pitchLimit = false;
    bool rollLimit = false;
    EulerAngles eulerAngles;
    RotateDegreeLimit rotationLimit;
    RotateDegreeLimit rotationLimitWithOffset;
    RotateSpeedLimit rotateSpeedLimit;
    MechBaseInfo mechBaseInfo;
    MechCapabilityInfo mechCapabilityInfo;
    RotationAxesStatus rotationAxesStatus;
    MechTrackingStatus trackingStatus;
};

struct MechNapiCommandCallbackInfo {
    MechNapiCommandCallbackInfo(uint32_t &tokenId, std::string &napiCmdId);

    uint32_t tokenId;
    std::string napiCmdId;
    bool willLimitChange = false;

    std::string ToString() const
    {
        return "napiCmdId: " + napiCmdId +
               ", willLimitChange: " + std::to_string(willLimitChange);
    }
};

class MotionManager : public IMechEventListener, public std::enable_shared_from_this<MotionManager> {
public:
    MotionManager(const std::shared_ptr<TransportSendAdapter> sendAdapter, int32_t mechId);
    void Init();

public:
    ~MotionManager();
    MotionManager(const MotionManager&) = delete;
    MotionManager& operator=(const MotionManager&) = delete;

    void RegisterEventListener();
    void RegisterEventListenerV01();
    int32_t Rotate(std::shared_ptr<RotateParam> rotateParam, uint32_t &tokenId, std::string &napiCmdId);
    int32_t RotateBySpeed(std::shared_ptr<RotateBySpeedParam> rotateSpeedParam,
                          uint32_t &tokenId, std::string &napiCmdId);
    int32_t StopRotate(uint32_t &tokenId, std::string &napiCmdId);

    int32_t GetSpeedControlTimeLimit(std::shared_ptr<TimeLimit> &timeLimit);
    int32_t GetRotateSpeedLimit(RotateSpeedLimit &speedLimit);
    int32_t GetCurrentPosition(std::shared_ptr<EulerAngles> &eulerAngles);
    int32_t GetRotationLimit(RotateDegreeLimit &rotationLimit);
    int32_t SetMechCameraTrackingFrame(const std::shared_ptr<TrackingFrameParams> trackingFrameParams);
    int32_t GetMechCameraTrackingEnabled(bool &isEnabled);
    int32_t SetMechCameraTrackingEnabled(bool &isEnabled);
    int32_t SetMechCameraTrackingLayout(const std::shared_ptr<LayoutParams> layoutParams);
    int32_t GetMechCameraTrackingLayout(std::shared_ptr<LayoutParams> &layoutParams);
    int32_t SetMechCameraInfo(const CameraInfoParams &mechCameraInfo);
    int32_t GetMechBaseInfo(std::shared_ptr<MechBaseInfo> &mechBaseInfo);
    int32_t GetMechCapabilityInfo(std::shared_ptr<MechCapabilityInfo> &mechCapabilityInfo);
    int32_t GetRotationAxesStatus(const int32_t &mechId, RotationAxesStatus &axesStatus);
    int32_t PerformPresetAction(PresetAction action, int32_t delay);
    void MechAttitudeNotify(const std::shared_ptr<CommonRegisterMechPositionInfoCmd> &cmd);
    void MechParamNotify(const std::shared_ptr<CommonRegisterMechStateInfoCmd> &cmd);
    void MechGenericEventNotify(const std::shared_ptr<NormalRegisterMechGenericEventCmd> &cmd);
    void MechExecutionResultNotify(const std::shared_ptr<RegisterMechControlResultCmd> &cmd);
    void MechWheelZoomNotify(const std::shared_ptr<RegisterMechWheelDataCmd> &cmd);
    void MechButtonEventNotify(const std::shared_ptr<CommonRegisterMechKeyEventCmd> &cmd);
    void MechTrackingStatusNotify(const std::shared_ptr<CommonRegisterMechTrackingEnableCmd> &cmd);
    void UnRegisterNotifyEvent();
    int32_t ActionGimbalFeatureControl(const ActionControlParams &actionControlParams);
    const std::string &GetDeviceRealName() const;
    const std::string &TryGetDeviceRealNameSync(uint32_t timeoutMs);

private:
    void MMIKeyEvent(CameraKeyEvent eventType);
    void JudgingYawLimit(RotateDegreeLimit& limit);
    void AbsolutelyEulerAnglesJudgingLimitLocked(EulerAngles& eulerAngles);
    uint8_t CreateResponseTaskId();
    uint16_t CreateRotateTaskId();
    void FormatLimit(RotateDegreeLimit &params);
    bool IsLimited();
    bool IsLimited(const float &negMax, const float &posMax, const float &position);
    void LimitCalculationLocked(EulerAngles& position, RotationAxesStatus& status, bool callback);
    static int64_t GetSysClockTime();
    std::shared_ptr<MMI::KeyEvent> CreateKeyEvent(int32_t keyCode, int32_t keyAction);
    void StartEvent();
    void HandleMechPlacementChange(bool isPhoneOn);
    void HandelRotateParam(std::shared_ptr<RotateParam> &rotateParam, bool &willLimitChange);
    void CheckYawDegree(std::shared_ptr<RotateParam> &rotateParam, const RotateDegreeLimit &limit, float yawResult);
    void CheckRollDegree(std::shared_ptr<RotateParam> &rotateParam, const RotateDegreeLimit &limit, float rollResult);
    void CheckPitchDegree(std::shared_ptr<RotateParam> &rotateParam, const RotateDegreeLimit &limit, float pitchResult);
    void HandelRotateSpeedParam(std::shared_ptr<RotateBySpeedParam> &rotateBySpeedParam, bool &willLimitChange);
    void CheckPitchSpeed(const std::shared_ptr<RotateBySpeedParam> &rotateBySpeedParam, const RotateDegreeLimit &limit,
        float pitchResult);
    void CheckRollSpeed(const std::shared_ptr<RotateBySpeedParam> &rotateBySpeedParam, const RotateDegreeLimit &limit,
        float rollResult);
    void CheckYawSpeed(const std::shared_ptr<RotateBySpeedParam> &rotateBySpeedParam, const RotateDegreeLimit &limit,
        float yawResult);
    void SetMechTkEnableNoTarget();
    void TrackingChecker();
    void ProcessTrackingStatus();
    void GetProtocolVer();
    void SetProtocolVer();
    void GetMechRealName();
    void GetMechLimitInfo();
    void GetDeviceBaseInfo();
    void GetDeviceCapabilityInfo();
    void GetDeviceStateInfo();
    void UpdateTrackingTime();
    void SetDeviceRealName(const std::string &deviceRealName);

private:
    std::shared_ptr<DeviceStatus> deviceStatus_;
    std::mutex deviceStatusMutex_;
    RotationAxesStatus status_{};
    std::shared_ptr<TransportSendAdapter> sendAdapter_;

    std::vector<uint16_t> notifyListenerType_;
    std::mutex notifyListenerMutex_;
    int32_t mechId_;

    std::map<uint8_t, MechNapiCommandCallbackInfo> seqCallbacks_;
    std::mutex seqCallbackMutex_;

    std::shared_ptr<IMechEventListener> mechEventListener_;
    std::mutex eventListenerMutex_;

    std::thread eventThread_;
    std::condition_variable eventCon_;
    std::mutex eventMutex_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> eventHandler_ = nullptr;
    int32_t wheelFilterCnt_ = 0;
    int32_t wheelCorretCnt_ = 0;
    std::shared_mutex taskIdMutex_;
    uint8_t lastTaskId_ = 0;
    CommandFactory factory;

    // tracking state manager
    std::chrono::steady_clock::time_point lastTrackingFrameTime_;
    std::unique_ptr<std::thread> trackingCheckerThread_;
    std::atomic<bool> startTrackingChecker_;

    std::condition_variable deviceRealNameVisitCon_;
    std::mutex deviceRealNameVisitMutex_;
    std::string deviceRealName_;
    volatile uint8_t protocolVer_ = 0x00;
    std::condition_variable protocolVerCon_;
    std::mutex protocolVerMutex_;

    DeviceBaseInfo deviceBaseInfo_;
    DeviceCapabilityInfo deviceCapabilityInfo_;
    uint16_t taskId_ = 1;
    bool SetMechCameraInfo_ = false;
};

class MechEventListenerImpl : public IMechEventListener {
public:
    MechEventListenerImpl(std::shared_ptr<MotionManager> motionManager);
    ~MechEventListenerImpl();
    void MechAttitudeNotify(const std::shared_ptr<CommonRegisterMechPositionInfoCmd> &cmd) override;
    void MechButtonEventNotify(const std::shared_ptr<CommonRegisterMechKeyEventCmd> &cmd) override;
    void MechParamNotify(const std::shared_ptr<CommonRegisterMechStateInfoCmd> &cmd) override;
    void MechGenericEventNotify(const std::shared_ptr<NormalRegisterMechGenericEventCmd>& cmd) override;
    void MechExecutionResultNotify(const std::shared_ptr<RegisterMechControlResultCmd> &cmd) override;
    void MechWheelZoomNotify(const std::shared_ptr<RegisterMechWheelDataCmd> &cmd) override;
    void MechTrackingStatusNotify(const std::shared_ptr<CommonRegisterMechTrackingEnableCmd> &cmd) override;

private:
    std::shared_ptr<MotionManager> motionManager_;
};
} // namespace MechBodyController
} // namespace OHOS
#endif // OHOS_EB_MOTION_MANAGER_H
