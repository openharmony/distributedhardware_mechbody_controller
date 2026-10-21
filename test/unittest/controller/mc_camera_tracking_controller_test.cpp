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
#include "mc_camera_tracking_controller_test.h"
#include "bluetooth_errorcode.h"
#include "c_header/ohos_bt_gatt.h"
#include "../mechbody_controller_log.h"
#include "../test_log.h"
#include "mechbody_controller_service.h"
#include "mc_connect_manager.h"
#include "subscribe_event_utils.h"
#include "notification_utils.h"
#include <chrono>
#include <thread>
#include <atomic>

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS {
namespace MechBodyController {

void McCameraTrackingControllerTest::SetUpTestCase()
{
    DTEST_LOG << "McCameraTrackingControllerTest::SetUpTestCase" << std::endl;
}

void McCameraTrackingControllerTest::TearDownTestCase()
{
    DTEST_LOG << "McCameraTrackingControllerTest::TearDownTestCase" << std::endl;
}

void McCameraTrackingControllerTest::TearDown()
{
    DTEST_LOG << "McCameraTrackingControllerTest::TearDown" << std::endl;
}

void McCameraTrackingControllerTest::SetUp()
{
    DTEST_LOG << "McCameraTrackingControllerTest::SetUp" << std::endl;
}

// 预置SubscribeEventUtils和McCameraTrackingController的守卫变量，
// 阻止Init()中的跨DSO系统调用（SubscribeCommonEvent/RegisterPermDisablePolicyCallback/
// RegisterCameraMuteListener等），避免测试环境下CFI check失败。
// 返回原始值用于调用结束后恢复。
struct InitGuardSnapshot {
    std::shared_ptr<ExtCommonEventSubscriber> commonEventMonitor;
    std::shared_ptr<CameraDisablePolicyCallback> cameraDisableCallback;
    std::shared_ptr<MechCameraMuteListener> cameraMuteListener;
    RecordSensorCallback sensorCallback;
};

static InitGuardSnapshot PreSetInitGuards()
{
    InitGuardSnapshot snapshot;
    auto &subscribeUtils = SubscribeEventUtils::GetInstance();
    snapshot.commonEventMonitor = subscribeUtils.commonEventMonitor_;
    snapshot.cameraDisableCallback = subscribeUtils.cameraDisablePolicyCallback_;
    snapshot.cameraMuteListener = subscribeUtils.cameraMuteListener_;
    if (subscribeUtils.commonEventMonitor_ == nullptr) {
        EventFwk::MatchingSkills matchingSkills;
        matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
        EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
        subscribeUtils.commonEventMonitor_ = std::make_shared<ExtCommonEventSubscriber>(subscribeInfo);
    }
    if (subscribeUtils.cameraDisablePolicyCallback_ == nullptr) {
        subscribeUtils.cameraDisablePolicyCallback_ = std::make_shared<CameraDisablePolicyCallback>();
    }
    if (subscribeUtils.cameraMuteListener_ == nullptr) {
        subscribeUtils.cameraMuteListener_ = std::make_shared<MechCameraMuteListener>();
    }
    auto &cameraController = McCameraTrackingController::GetInstance();
    snapshot.sensorCallback = cameraController.user.callback;
    if (cameraController.user.callback == nullptr) {
        cameraController.user.callback = McCameraTrackingController::SensorCallback;
    }
    return snapshot;
}

static void RestoreInitGuards(const InitGuardSnapshot &snapshot)
{
    auto &subscribeUtils = SubscribeEventUtils::GetInstance();
    subscribeUtils.commonEventMonitor_ = snapshot.commonEventMonitor;
    subscribeUtils.cameraDisablePolicyCallback_ = snapshot.cameraDisableCallback;
    subscribeUtils.cameraMuteListener_ = snapshot.cameraMuteListener;
    McCameraTrackingController::GetInstance().user.callback = snapshot.sensorCallback;
}

// 清空守卫变量，使UnInit()中的UnRegister*函数提前返回，避免跨DSO调用
static void ClearInitGuards()
{
    auto &subscribeUtils = SubscribeEventUtils::GetInstance();
    subscribeUtils.cameraDisablePolicyCallback_ = nullptr;
    subscribeUtils.cameraMuteListener_ = nullptr;
    subscribeUtils.commonEventMonitor_ = nullptr;
}


/**
 * @tc.name  : UpdateMotionManagers_001
 * @tc.number: UpdateMotionManagers_001
 * @tc.desc  : Testing UpdateMotionManagers function.
 */
HWTEST_F(McCameraTrackingControllerTest, UpdateMotionManagers_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest UpdateMotionManagers_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();

    int32_t ret = mcCameraTrackingController.UpdateMotionManagers();
    EXPECT_EQ(ret, ERR_OK);

    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;
    ret = mcCameraTrackingController.UpdateMotionManagers();
    EXPECT_EQ(ret, DEVICE_NOT_PLACED_ON_MECH);
    
    DTEST_LOG << "McCameraTrackingControllerTest UpdateMotionManagers_001 end" << std::endl;
}

/**
 * @tc.name  : OnMetadataInfo_001
 * @tc.number: OnMetadataInfo_001
 * @tc.desc  : Testing OnMetadataInfo function with null metadata.
 */
HWTEST_F(McCameraTrackingControllerTest, OnMetadataInfo_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest OnMetadataInfo_001 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();
    std::shared_ptr<OHOS::Camera::CameraMetadata> metadata = nullptr;

    int32_t ret = mcCameraTrackingController.OnMetadataInfo(metadata);
    EXPECT_EQ(ret, METADATA_INFO_IS_EMPTY);

    DTEST_LOG << "McCameraTrackingControllerTest OnMetadataInfo_001 end" << std::endl;
}

/**
 * @tc.name  : OnFocusTracking_002
 * @tc.number: OnFocusTracking_002
 * @tc.desc  : Testing OnFocusTracking function with empty detected objects.
 */
HWTEST_F(McCameraTrackingControllerTest, OnFocusTracking_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest OnFocusTracking_002 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();
    CameraStandard::FocusTrackingMetaInfo info;
    mcCameraTrackingController.currentCameraInfo_->currentTrackingEnable = true;

    // Test with empty detected objects - function should return ERR_OK
    int32_t ret = mcCameraTrackingController.OnFocusTracking(info);
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest OnFocusTracking_002 end" << std::endl;
}

/**
 * @tc.name  : OnFocusTracking_004
 * @tc.number: OnFocusTracking_004
 * @tc.desc  : Testing OnFocusTracking with valid ROI.
 */
HWTEST_F(McCameraTrackingControllerTest, OnFocusTracking_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest OnFocusTracking_004 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();
    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->currentTrackingEnable = true;
    mcCameraTrackingController.currentCameraInfo_->cameraType = CameraType::BACK;
    mcCameraTrackingController.currentCameraInfo_->width = 1920;
    mcCameraTrackingController.currentCameraInfo_->height  =1080;

    CameraStandard::FocusTrackingMetaInfo info;
    CameraStandard::Rect trackingRegion = {0.1f, 0.1f, 0.2f, 0.2f};
    info.SetTrackingRegion(trackingRegion);
    info.SetTrackingObjectId(1);

    // Test with valid tracking region - function should return ERR_OK
    int32_t ret = mcCameraTrackingController.OnFocusTracking(info);
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest OnFocusTracking_004 end" << std::endl;
}

/**
 * @tc.name  : OnFocusTracking_005
 * @tc.number: OnFocusTracking_005
 * @tc.desc  : Testing OnFocusTracking with invalid ROI and ignored frame limit.
 */
HWTEST_F(McCameraTrackingControllerTest, OnFocusTracking_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest OnFocusTracking_005 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();
    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->currentTrackingEnable = true;
    mcCameraTrackingController.currentCameraInfo_->cameraType = CameraType::BACK;
    mcCameraTrackingController.currentCameraInfo_->width = 1920;
    mcCameraTrackingController.currentCameraInfo_->height = 1080;
    mcCameraTrackingController.eventHandler_ = nullptr;

    CameraStandard::FocusTrackingMetaInfo info;
    CameraStandard::Rect trackingRegion = {0.0f, 0.0f, 0.0f, 0.0f};
    info.SetTrackingRegion(trackingRegion);
    info.SetTrackingObjectId(1);

    // Test with invalid ROI (zero width/height) - function should return ERR_OK
    int32_t ret = mcCameraTrackingController.OnFocusTracking(info);
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest OnFocusTracking_005 end" << std::endl;
}

/**
 * @tc.name  : OnFocusTracking_006
 * @tc.number: OnFocusTracking_006
 * @tc.desc  : Testing OnFocusTracking when BuildTrackingParams returns nullptr.
 */
HWTEST_F(McCameraTrackingControllerTest, OnFocusTracking_006, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest OnFocusTracking_006 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();
    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->currentTrackingEnable = true;
    mcCameraTrackingController.currentCameraInfo_->cameraType = CameraType::BACK;
    mcCameraTrackingController.currentCameraInfo_->width = 1920;
    mcCameraTrackingController.currentCameraInfo_->height = 1080;
    // 设置sessionMode为CINEMATIC_VIDEO，让CinematicVideoModeTrackingTargetFilter返回错误
    mcCameraTrackingController.currentCameraInfo_->sessionMode =
        static_cast<int32_t>(CameraStandard::SceneMode::CINEMATIC_VIDEO);
    mcCameraTrackingController.currentCameraInfo_->pauseFrameCount = 0;
    mcCameraTrackingController.currentCameraInfo_->searchingTarget = false;
    mcCameraTrackingController.currentCameraInfo_->trackingTargetNum = 0;

    CameraStandard::FocusTrackingMetaInfo info;
    // 设置tracking mode为LOCKED且region为0，触发CinematicVideoModeTrackingTargetFilter返回错误
    info.SetTrackingMode(CameraStandard::FocusTrackingMode::FOCUS_TRACKING_MODE_LOCKED);
    CameraStandard::Rect rect = {0.0f, 0.0f, 0.0f, 0.0f};
    info.SetTrackingRegion(rect);

    // Test when BuildTrackingParams returns nullptr - function should return ERR_OK
    int32_t ret = mcCameraTrackingController.OnFocusTracking(info);
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest OnFocusTracking_006 end" << std::endl;
}

/**
 * @tc.name  : BuildTrackingParams_001
 * @tc.number: BuildTrackingParams_001
 * @tc.desc  : Testing BuildTrackingParams function.
 */
HWTEST_F(McCameraTrackingControllerTest, BuildTrackingParams_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest BuildTrackingParams_001 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();
    CameraStandard::FocusTrackingMetaInfo info;

    // Test BuildTrackingParams with empty info - should return a valid TrackingFrameParams
    std::shared_ptr<TrackingFrameParams> trackingParams = mcCameraTrackingController.BuildTrackingParams(info);
    ASSERT_NE(trackingParams, nullptr);
    // targetId is uint16_t; info.GetTrackingObjectId() returns int32_t(-1), so: the test1 = 0xFFFF = 65535
    EXPECT_EQ(trackingParams->targetId, 65535);
    EXPECT_EQ(trackingParams->objectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE));
    EXPECT_EQ(trackingParams->confidence, ConfidenceLevel::HIGH);
    DTEST_LOG << "McCameraTrackingControllerTest BuildTrackingParams_001 end" << std::endl;
}

/**
 * @tc.name  : BuildTrackingParams_002
 * @tc.number: BuildTrackingParams_002
 * @tc.desc  : Test BuildTrackingParams when GetTrackingTarget returns error.
 */
HWTEST_F(McCameraTrackingControllerTest, BuildTrackingParams_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest BuildTrackingParams_002 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->fovH = 90;
    mcCameraTrackingController.currentCameraInfo_->fovV = 60;
    mcCameraTrackingController.currentCameraInfo_->isRecording = false;
    mcCameraTrackingController.currentCameraInfo_->trackingTargetNum = 0;
    mcCameraTrackingController.currentCameraInfo_->searchingTarget = false;

    CameraStandard::FocusTrackingMetaInfo info;

    mcCameraTrackingController.eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(nullptr);

    mcCameraTrackingController.lastTrackingFrame_ = std::make_shared<TrackingFrameParams>();
    mcCameraTrackingController.lastTrackingFrame_->timeStamp = 0;
    mcCameraTrackingController.lastTrackingFrame_->targetId = -1;
    mcCameraTrackingController.lastTrackingFrame_->objectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE);

    std::shared_ptr<TrackingFrameParams> result = mcCameraTrackingController.BuildTrackingParams(info);

    EXPECT_NE(result, nullptr);
    // Verify business properties when GetTrackingTarget returns error
    EXPECT_EQ(result->confidence, ConfidenceLevel::HIGH);
    EXPECT_EQ(result->objectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE));
    EXPECT_EQ(result->targetId, 65535);
    EXPECT_EQ(result->fovH, 90);
    EXPECT_EQ(result->fovV, 60);
    EXPECT_EQ(result->isRecording, false);

    DTEST_LOG << "McCameraTrackingControllerTest BuildTrackingParams_002 end" << std::endl;
}

/**
 * @tc.name  : BuildTrackingParams_003
 * @tc.number: BuildTrackingParams_003
 * @tc.desc  : Test BuildTrackingParams when searching target found.
 */
HWTEST_F(McCameraTrackingControllerTest, BuildTrackingParams_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest BuildTrackingParams_003 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->fovH = 90;
    mcCameraTrackingController.currentCameraInfo_->fovV = 60;
    mcCameraTrackingController.currentCameraInfo_->searchingTarget = true;
    mcCameraTrackingController.currentCameraInfo_->trackingTargetNum = 1;
    mcCameraTrackingController.currentCameraInfo_->sessionMode = 0;

    CameraStandard::FocusTrackingMetaInfo info;
    mcCameraTrackingController.eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(nullptr);

    mcCameraTrackingController.lastTrackingFrame_ = std::make_shared<TrackingFrameParams>();
    mcCameraTrackingController.lastTrackingFrame_->timeStamp = 0;
    mcCameraTrackingController.lastTrackingFrame_->targetId = -1;

    std::shared_ptr<TrackingFrameParams> result = mcCameraTrackingController.BuildTrackingParams(info);

    EXPECT_NE(result, nullptr);
    // Verify business properties when searching target found
    EXPECT_EQ(result->confidence, ConfidenceLevel::HIGH);
    EXPECT_EQ(result->objectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE));
    EXPECT_EQ(result->targetId, 65535);
    EXPECT_EQ(result->fovH, 90);
    EXPECT_EQ(result->fovV, 60);

    DTEST_LOG << "McCameraTrackingControllerTest BuildTrackingParams_003 end" << std::endl;
}

/**
 * @tc.name  : BuildTrackingParams_004
 * @tc.number: BuildTrackingParams_004
 * @tc.desc  : Testing BuildTrackingParams function with valid detected objects.
 */
HWTEST_F(McCameraTrackingControllerTest, BuildTrackingParams_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest BuildTrackingParams_004 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->fovH = 90;
    mcCameraTrackingController.currentCameraInfo_->fovV = 60;
    mcCameraTrackingController.currentCameraInfo_->isRecording = false;
    mcCameraTrackingController.currentCameraInfo_->trackingTargetNum = 0;
    mcCameraTrackingController.currentCameraInfo_->searchingTarget = false;
    mcCameraTrackingController.currentCameraInfo_->sessionMode = 0;

    CameraStandard::FocusTrackingMetaInfo info;
    CameraStandard::Rect targetRect;
    targetRect.topLeftX = 0.3f;
    targetRect.topLeftY = 0.3f;
    targetRect.width = 0.35f;
    targetRect.height = 0.35f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> mockObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE,
            timestamp,
            targetRect,
            100,
            0
        );

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects = {mockObject};

    std::shared_ptr<TrackingFrameParams> result = mcCameraTrackingController.BuildTrackingParams(info);

    EXPECT_NE(result, nullptr);
    if (result != nullptr) {
        EXPECT_EQ(result->confidence, ConfidenceLevel::HIGH);
        EXPECT_EQ(result->objectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE));
    }

    DTEST_LOG << "McCameraTrackingControllerTest BuildTrackingParams_004 end" << std::endl;
}

/**
 * @tc.name  : BuildTrackingParams_005
 * @tc.number: BuildTrackingParams_005
 * @tc.desc  : Testing BuildTrackingParams function when CinematicVideoMode check fails.
 */
HWTEST_F(McCameraTrackingControllerTest, BuildTrackingParams_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest BuildTrackingParams_005 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->sessionMode =
        static_cast<int32_t>(CameraStandard::SceneMode::CINEMATIC_VIDEO);
    mcCameraTrackingController.currentCameraInfo_->pauseFrameCount = 0;
    mcCameraTrackingController.currentCameraInfo_->fovV = 45.0f;
    mcCameraTrackingController.currentCameraInfo_->fovH = 60.0f;
    mcCameraTrackingController.currentCameraInfo_->isRecording = false;
    mcCameraTrackingController.currentCameraInfo_->trackingTargetNum = 0;
    mcCameraTrackingController.currentCameraInfo_->searchingTarget = false;

    CameraStandard::FocusTrackingMetaInfo info;
    info.SetTrackingMode(CameraStandard::FocusTrackingMode::FOCUS_TRACKING_MODE_LOCKED);
    
    CameraStandard::Rect trackingRegion;
    trackingRegion.topLeftX = 0.5f;
    trackingRegion.topLeftY = 0.5f;
    trackingRegion.width = 0.5f;
    trackingRegion.height = 0.5f;
    info.SetTrackingRegion(trackingRegion);

    std::shared_ptr<TrackingFrameParams> lastFrame = std::make_shared<TrackingFrameParams>();
    lastFrame->targetId = 100;
    lastFrame->objectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE);
    lastFrame->timeStamp = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    mcCameraTrackingController.lastTrackingFrame_ = lastFrame;

    CameraStandard::Rect lastRect;
    lastRect.topLeftX = 0.3f;
    lastRect.topLeftY = 0.3f;
    lastRect.width = 0.4f;
    lastRect.height = 0.4f;
    mcCameraTrackingController.lastTrackingRect_ = lastRect;

    std::shared_ptr<TrackingFrameParams> result = mcCameraTrackingController.BuildTrackingParams(info);

    // 验证返回结果非空
    EXPECT_NE(result, nullptr);
    // 验证返回的业务属性
    if (result != nullptr) {
        EXPECT_EQ(result->fovH, 60.0f);
        EXPECT_EQ(result->fovV, 45.0f);
        EXPECT_EQ(result->isRecording, false);
    }
    // 验证trackingTargetNum
    EXPECT_EQ(mcCameraTrackingController.currentCameraInfo_->trackingTargetNum, 0);

    DTEST_LOG << "McCameraTrackingControllerTest BuildTrackingParams_005 end" << std::endl;
}

/**
 * @tc.name  : BuildTrackingParams_006
 * @tc.number: BuildTrackingParams_006
 * @tc.desc  : Testing BuildTrackingParams when searchingTarget and detected objects exist.
 */
HWTEST_F(McCameraTrackingControllerTest, BuildTrackingParams_006, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest BuildTrackingParams_006 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->fovH = 90;
    mcCameraTrackingController.currentCameraInfo_->fovV = 60;
    mcCameraTrackingController.currentCameraInfo_->isRecording = false;
    mcCameraTrackingController.currentCameraInfo_->trackingTargetNum = 0;
    mcCameraTrackingController.currentCameraInfo_->searchingTarget = true;

    CameraStandard::FocusTrackingMetaInfo info;
    CameraStandard::Rect rect = {0.1f, 0.1f, 0.2f, 0.2f};
    uint64_t timestamp = static_cast<uint64_t>(std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()).time_since_epoch().count());
    sptr<CameraStandard::MetadataObject> obj = new CameraStandard::MetadataObject(
        CameraStandard::MetadataObjectType::FACE, timestamp, rect, 1, 95);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;
    detectedObjects.push_back(obj);
    info.SetDetectedObjects(detectedObjects);

    mcCameraTrackingController.eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(nullptr);

    mcCameraTrackingController.lastTrackingFrame_ = std::make_shared<TrackingFrameParams>();
    mcCameraTrackingController.lastTrackingFrame_->timeStamp = 0;
    mcCameraTrackingController.lastTrackingFrame_->targetId = -1;
    mcCameraTrackingController.lastTrackingFrame_->objectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE);

    std::shared_ptr<TrackingFrameParams> result = mcCameraTrackingController.BuildTrackingParams(info);

    EXPECT_NE(result, nullptr);
    // Verify business properties when searchingTarget and detected objects exist
    EXPECT_EQ(result->confidence, ConfidenceLevel::HIGH);
    EXPECT_EQ(result->targetId, 1);
    EXPECT_EQ(result->objectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE));
    EXPECT_EQ(result->fovH, 90);
    EXPECT_EQ(result->fovV, 60);
    EXPECT_EQ(result->isRecording, false);

    DTEST_LOG << "McCameraTrackingControllerTest BuildTrackingParams_006 end" << std::endl;
}
/**
 * @tc.name  : GetTrackingTarget_001
 * @tc.number: GetTrackingTarget_001
 * @tc.desc  : Testing GetTrackingTarget function with empty detected objects.
 */
HWTEST_F(McCameraTrackingControllerTest, GetTrackingTarget_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingTarget_001 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;
    sptr<CameraStandard::MetadataObject> targetObject;
    int32_t trackingObjectId = 100;

    int32_t ret =
        mcCameraTrackingController.GetTrackingTarget(trackingRegion, detectedObjects, trackingObjectId, targetObject);

    EXPECT_EQ(ret, DETECTED_OBJECT_IS_EMPTY);
    EXPECT_EQ(targetObject, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingTarget_001 end" << std::endl;
}

/**
 * @tc.name  : GetTrackingTarget_002
 * @tc.number: GetTrackingTarget_002
 * @tc.desc  : Testing GetTrackingTarget function with matching trackingObjectId.
 */
HWTEST_F(McCameraTrackingControllerTest, GetTrackingTarget_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingTarget_002 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;

    int64_t timestamp = 0;
    CameraStandard::Rect rect1;
    rect1.topLeftX = 0.1f;
    rect1.topLeftY = 0.1f;
    rect1.width = 0.2f;
    rect1.height = 0.2f;

    sptr<CameraStandard::MetadataObject> mockObject1 =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE, timestamp, rect1, 100, 0);

    CameraStandard::Rect rect2;
    rect2.topLeftX = 0.3f;
    rect2.topLeftY = 0.3f;
    rect2.width = 0.2f;
    rect2.height = 0.2f;

    sptr<CameraStandard::MetadataObject> mockObject2 =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::HUMAN_BODY, timestamp, rect2, 200, 0);

    detectedObjects.push_back(mockObject1);
    detectedObjects.push_back(mockObject2);

    sptr<CameraStandard::MetadataObject> targetObject;
    int32_t trackingObjectId = 100;
    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->sessionMode = 0;

    int32_t ret =
        mcCameraTrackingController.GetTrackingTarget(trackingRegion, detectedObjects, trackingObjectId, targetObject);
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingTarget_002 end" << std::endl;
}

/**
 * @tc.name  : GetTrackingTarget_003
 * @tc.number: GetTrackingTarget_003
 * @tc.desc  : Test GetTrackingTarget with trackingObjectId not found.
 */
HWTEST_F(McCameraTrackingControllerTest, GetTrackingTarget_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingTarget_003 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;
    CameraStandard::Rect rect;
    rect.topLeftX = 0.3f;
    rect.topLeftY = 0.3f;
    rect.width = 0.4f;
    rect.height = 0.4f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> mockObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE, timestamp, rect, 100, 0);

    detectedObjects.push_back(mockObject);

    sptr<CameraStandard::MetadataObject> targetObject;
    int32_t trackingObjectId = 999;

    mcCameraTrackingController.currentCameraInfo_ = nullptr;

    int32_t ret =
        mcCameraTrackingController.GetTrackingTarget(trackingRegion, detectedObjects, trackingObjectId, targetObject);

    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingTarget_003 end" << std::endl;
}

/**
 * @tc.name  : ProcessTargetByType_001
 * @tc.number: ProcessTargetByType_001
 * @tc.desc  : Testing ProcessTargetByType function with HUMAN_HEAD type.
 */
HWTEST_F(McCameraTrackingControllerTest, ProcessTargetByType_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest ProcessTargetByType_001 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    int64_t timestamp = 0;
    CameraStandard::Rect headRect;
    headRect.topLeftX = 0.3f;
    headRect.topLeftY = 0.3f;
    headRect.width = 0.4f;
    headRect.height = 0.4f;

    sptr<CameraStandard::MetadataObject> headObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::HUMAN_HEAD, timestamp, headRect, 100, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;
    
    
    CameraStandard::Rect rect;
    rect.topLeftX = 0.3f;
    rect.topLeftY = 0.3f;
    rect.width = 0.4f;
    rect.height = 0.4f;

    sptr<CameraStandard::MetadataObject> mockObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE, timestamp, rect, 100, 0);
    detectedObjects.push_back(mockObject);
    
    CameraStandard::Rect faceRect;
    faceRect.topLeftX = 0.32f;
    faceRect.topLeftY = 0.32f;
    faceRect.width = 0.38f;
    faceRect.height = 0.38f;

    sptr<CameraStandard::MetadataObject> faceObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE, timestamp, faceRect, 200, 0);

    detectedObjects.push_back(faceObject);
    sptr<CameraStandard::MetadataObject> targetObject;
    int32_t ret = mcCameraTrackingController.ProcessTargetByType(headObject, detectedObjects, targetObject);
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest ProcessTargetByType_001 end" << std::endl;
}

/**
 * @tc.name  : ProcessTargetByType_002
 * @tc.number: ProcessTargetByType_002
 * @tc.desc  : Testing ProcessTargetByType function with HUMAN_BODY type.
 */
HWTEST_F(McCameraTrackingControllerTest, ProcessTargetByType_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest ProcessTargetByType_002 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect bodyRect;
    bodyRect.topLeftX = 0.3f;
    bodyRect.topLeftY = 0.3f;
    bodyRect.width = 0.5f;
    bodyRect.height = 0.6f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> bodyObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::HUMAN_BODY, timestamp, bodyRect, 100, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;
    
    CameraStandard::Rect faceRect;
    faceRect.topLeftX = 0.35f;
    faceRect.topLeftY = 0.35f;
    faceRect.width = 0.2f;
    faceRect.height = 0.2f;

    sptr<CameraStandard::MetadataObject> faceObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE, timestamp, faceRect, 200, 0);

    detectedObjects.push_back(faceObject);

    sptr<CameraStandard::MetadataObject> targetObject;

    int32_t ret = mcCameraTrackingController.ProcessTargetByType(bodyObject, detectedObjects, targetObject);

    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest ProcessTargetByType_002 end" << std::endl;
}

/**
 * @tc.name  : ProcessTargetByType_003
 * @tc.number: ProcessTargetByType_003
 * @tc.desc  : Test ProcessTargetByType with default case.
 */
HWTEST_F(McCameraTrackingControllerTest, ProcessTargetByType_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest ProcessTargetByType_003 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect rect;
    rect.topLeftX = 0.3f;
    rect.topLeftY = 0.3f;
    rect.width = 0.4f;
    rect.height = 0.4f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> selectedObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE, timestamp, rect, 100, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;
    sptr<CameraStandard::MetadataObject> targetObject;

    int32_t ret = mcCameraTrackingController.ProcessTargetByType(selectedObject, detectedObjects, targetObject);

    EXPECT_EQ(ret, ERR_OK);
    EXPECT_NE(targetObject, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTest ProcessTargetByType_003 end" << std::endl;
}

/**
 * @tc.name  : FindFaceForHead_001
 * @tc.number: FindFaceForHead_001
 * @tc.desc  : Testing FindFaceForHead function.
 */
HWTEST_F(McCameraTrackingControllerTest, FindFaceForHead_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest FindFaceForHead_001 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect headRect;
    headRect.topLeftX = 0.3f;
    headRect.topLeftY = 0.3f;
    headRect.width = 0.4f;
    headRect.height = 0.4f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> headObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::HUMAN_HEAD, timestamp, headRect, 100, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;

    CameraStandard::Rect faceRect1;
    faceRect1.topLeftX = 0.32f;
    faceRect1.topLeftY = 0.32f;
    faceRect1.width = 0.38f;
    faceRect1.height = 0.38f;

    sptr<CameraStandard::MetadataObject> faceObject1 =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE, timestamp, faceRect1, 200, 0);

    CameraStandard::Rect faceRect2;
    faceRect2.topLeftX = 0.7f;
    faceRect2.topLeftY = 0.7f;
    faceRect2.width = 0.2f;
    faceRect2.height = 0.2f;

    sptr<CameraStandard::MetadataObject> faceObject2 =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE, timestamp, faceRect2, 300, 0);

    detectedObjects.push_back(faceObject1);
    detectedObjects.push_back(faceObject2);

    sptr<CameraStandard::MetadataObject> result =
        mcCameraTrackingController.FindFaceForHead(*headObject, detectedObjects);

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->GetObjectId(), 200);
    EXPECT_EQ(result->GetType(), CameraStandard::MetadataObjectType::FACE);

    DTEST_LOG << "McCameraTrackingControllerTest FindFaceForHead_001 end" << std::endl;
}

/**
 * @tc.name  : FindBestFaceOrHeadForBody_001
 * @tc.number: FindBestFaceOrHeadForBody_001
 * @tc.desc  : Testing FindBestFaceOrHeadForBody function.
 */
HWTEST_F(McCameraTrackingControllerTest, FindBestFaceOrHeadForBody_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest FindBestFaceOrHeadForBody_001 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect bodyRect;
    bodyRect.topLeftX = 0.3f;
    bodyRect.topLeftY = 0.3f;
    bodyRect.width = 0.5f;
    bodyRect.height = 0.6f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> bodyObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::HUMAN_BODY, timestamp, bodyRect, 100, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;
    
    CameraStandard::Rect faceRect;
    faceRect.topLeftX = 0.35f;
    faceRect.topLeftY = 0.35f;
    faceRect.width = 0.2f;
    faceRect.height = 0.2f;

    sptr<CameraStandard::MetadataObject> faceObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE, timestamp, faceRect, 200, 0);

    CameraStandard::Rect headRect;
    headRect.topLeftX = 0.3f;
    headRect.topLeftY = 0.3f;
    headRect.width = 0.5f;
    headRect.height = 0.6f;

    sptr<CameraStandard::MetadataObject> headObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::HUMAN_HEAD, timestamp, headRect, 300, 0);

    detectedObjects.push_back(faceObject);
    detectedObjects.push_back(headObject);

    sptr<CameraStandard::MetadataObject> result =
        mcCameraTrackingController.FindBestFaceOrHeadForBody(*bodyObject, detectedObjects);

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->GetObjectId(), 300);
    EXPECT_EQ(result->GetType(), CameraStandard::MetadataObjectType::HUMAN_HEAD);

    DTEST_LOG << "McCameraTrackingControllerTest FindBestFaceOrHeadForBody_001 end" << std::endl;
}

/**
 * @tc.name  : FindBestMatchForBody_001
 * @tc.number: FindBestMatchForBody_001
 * @tc.desc  : Testing FindBestMatchForBody function.
 */
HWTEST_F(McCameraTrackingControllerTest, FindBestMatchForBody_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest FindBestMatchForBody_001 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect bodyRect;
    bodyRect.topLeftX = 0.3f;
    bodyRect.topLeftY = 0.3f;
    bodyRect.width = 0.5f;
    bodyRect.height = 0.6f;

    std::vector<sptr<CameraStandard::MetadataObject>> objectsForBody;

    CameraStandard::Rect highIouRect;
    highIouRect.topLeftX = 0.3f;
    highIouRect.topLeftY = 0.3f;
    highIouRect.width = 0.5f;
    highIouRect.height = 0.6f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> highIouObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE,
            timestamp,
            highIouRect,
            200,
            0);

    CameraStandard::Rect lowIouRect;
    lowIouRect.topLeftX = 0.1f;
    lowIouRect.topLeftY = 0.1f;
    lowIouRect.width = 0.1f;
    lowIouRect.height = 0.1f;

    sptr<CameraStandard::MetadataObject> lowIouObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE,
            timestamp,
            lowIouRect,
            300,
            0);

    objectsForBody.push_back(highIouObject);
    objectsForBody.push_back(lowIouObject);

    sptr<CameraStandard::MetadataObject> result =
        mcCameraTrackingController.FindBestMatchForBody(bodyRect, objectsForBody);

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->GetObjectId(), 200);

    DTEST_LOG << "McCameraTrackingControllerTest FindBestMatchForBody_001 end" << std::endl;
}

/**
 * @tc.name  : FindBestMatchForBody_002
 * @tc.number: FindBestMatchForBody_002
 * @tc.desc  : Test FindBestMatchForBody with multiple matches.
 */
HWTEST_F(McCameraTrackingControllerTest, FindBestMatchForBody_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest FindBestMatchForBody_002 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect bodyRect;
    bodyRect.topLeftX = 0.3f;
    bodyRect.topLeftY = 0.3f;
    bodyRect.width = 0.5f;
    bodyRect.height = 0.6f;

    std::vector<sptr<CameraStandard::MetadataObject>> objectsForBody;
    CameraStandard::Rect rect1;
    rect1.topLeftX = 0.32f;
    rect1.topLeftY = 0.32f;
    rect1.width = 0.25f;
    rect1.height = 0.3f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> obj1 =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE, timestamp, rect1, 200, 0);

    CameraStandard::Rect rect2;
    rect2.topLeftX = 0.35f;
    rect2.topLeftY = 0.35f;
    rect2.width = 0.2f;
    rect2.height = 0.25f;

    sptr<CameraStandard::MetadataObject> obj2 =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE, timestamp, rect2, 300, 0);

    objectsForBody.push_back(obj1);
    objectsForBody.push_back(obj2);

    sptr<CameraStandard::MetadataObject> result =
        mcCameraTrackingController.FindBestMatchForBody(bodyRect, objectsForBody);

    EXPECT_EQ(result, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTest FindBestMatchForBody_002 end" << std::endl;
}

/**
 * @tc.name  : GetTrackingTargetFallback_001
 * @tc.number: GetTrackingTargetFallback_001
 * @tc.desc  : Testing GetTrackingTargetFallback function with lastTrackingFrame.
 */
HWTEST_F(McCameraTrackingControllerTest, GetTrackingTargetFallback_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingTargetFallback_001 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;
    
    CameraStandard::Rect objectRect;
    objectRect.topLeftX = 0.3f;
    objectRect.topLeftY = 0.3f;
    objectRect.width = 0.4f;
    objectRect.height = 0.4f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> mockObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE, timestamp, objectRect, 100, 0);

    detectedObjects.push_back(mockObject);

    sptr<CameraStandard::MetadataObject> targetObject;

    mcCameraTrackingController.lastTrackingFrame_ = std::make_shared<TrackingFrameParams>();
    mcCameraTrackingController.lastTrackingFrame_->targetId = 100;

    int32_t ret = mcCameraTrackingController.GetTrackingTargetFallback(trackingRegion, detectedObjects, targetObject);

    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingTargetFallback_001 end" << std::endl;
}

/**
 * @tc.name  : CalculateIOU_001
 * @tc.number: CalculateIOU_001
 * @tc.desc  : Testing CalculateIOU function with overlapping rectangles.
 */
HWTEST_F(McCameraTrackingControllerTest, CalculateIOU_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest CalculateIOU_001 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect rect1;
    rect1.topLeftX = 0.1f;
    rect1.topLeftY = 0.1f;
    rect1.width = 0.5f;
    rect1.height = 0.5f;

    CameraStandard::Rect rect2;
    rect2.topLeftX = 0.3f;
    rect2.topLeftY = 0.3f;
    rect2.width = 0.6f;
    rect2.height = 0.6f;

    float iou = mcCameraTrackingController.CalculateIOU(rect1, rect2);

    // SUT treats width/height as right/bottom: rect1 area=0.16, rect2 area=0.09,
    // intersection area=0.04, minArea=0.09, IOU=0.04/0.09
    EXPECT_FLOAT_EQ(iou, 0.04f / 0.09f);

    DTEST_LOG << "McCameraTrackingControllerTest CalculateIOU_001 end" << std::endl;
}

/**
 * @tc.name  : CalculateIOU_002
 * @tc.number: CalculateIOU_002
 * @tc.desc  : Testing CalculateIOU function with non-overlapping rectangles.
 */
HWTEST_F(McCameraTrackingControllerTest, CalculateIOU_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest CalculateIOU_002 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect rect1;
    rect1.topLeftX = 0.1f;
    rect1.topLeftY = 0.1f;
    rect1.width = 0.2f;
    rect1.height = 0.2f;

    CameraStandard::Rect rect2;
    rect2.topLeftX = 0.5f;
    rect2.topLeftY = 0.5f;
    rect2.width = 0.2f;
    rect2.height = 0.2f;

    float iou = mcCameraTrackingController.CalculateIOU(rect1, rect2);

    EXPECT_EQ(iou, 0.0f);

    DTEST_LOG << "McCameraTrackingControllerTest CalculateIOU_002 end" << std::endl;
}

/**
 * @tc.name  : CalculateIOU_003
 * @tc.number: CalculateIOU_003
 * @tc.desc  : Testing CalculateIOU function with invalid rectangles.
 */
HWTEST_F(McCameraTrackingControllerTest, CalculateIOU_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest CalculateIOU_003 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect rect1;
    rect1.topLeftX = 0.1f;
    rect1.topLeftY = 0.1f;
    rect1.width = 0.0f;
    rect1.height = 0.2f;

    CameraStandard::Rect rect2;
    rect2.topLeftX = 0.3f;
    rect2.topLeftY = 0.3f;
    rect2.width = 0.2f;
    rect2.height = 0.2f;

    float iou = mcCameraTrackingController.CalculateIOU(rect1, rect2);

    EXPECT_EQ(iou, 0.0f);

    DTEST_LOG << "McCameraTrackingControllerTest CalculateIOU_003 end" << std::endl;
}

/**
 * @tc.name  : CalculateIOU_004
 * @tc.number: CalculateIOU_004
 * @tc.desc  : Test CalculateIOU with same rectangle.
 */
HWTEST_F(McCameraTrackingControllerTest, CalculateIOU_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest CalculateIOU_004 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect rect;
    rect.topLeftX = 0.1f;
    rect.topLeftY = 0.1f;
    rect.width = 0.5f;
    rect.height = 0.5f;

    float iou = mcCameraTrackingController.CalculateIOU(rect, rect);

    EXPECT_FLOAT_EQ(iou, 1.0f);

    DTEST_LOG << "McCameraTrackingControllerTest CalculateIOU_004 end" << std::endl;
}

/**
 * @tc.name  : CinematicVideoModeTrackingTargetFilter_001
 * @tc.number: CinematicVideoModeTrackingTargetFilter_001
 * @tc.desc  : Testing CinematicVideoModeTrackingTargetFilter function in non-cinematic mode.
 */
HWTEST_F(McCameraTrackingControllerTest, CinematicVideoModeTrackingTargetFilter_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest CinematicVideoModeTrackingTargetFilter_001 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->sessionMode = 0;

    CameraStandard::FocusTrackingMetaInfo info;
    std::shared_ptr<TrackingFrameParams> trackingParams = std::make_shared<TrackingFrameParams>();

    int32_t ret = mcCameraTrackingController.CinematicVideoModeTrackingTargetFilter(info, trackingParams);

    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest CinematicVideoModeTrackingTargetFilter_001 end" << std::endl;
}

/**
 * @tc.name  : CinematicVideoModeTrackingTargetFilter_002
 * @tc.number: CinematicVideoModeTrackingTargetFilter_002
 * @tc.desc  : Testing CinematicVideoModeTrackingTargetFilter function with locked tracking mode.
 */
HWTEST_F(McCameraTrackingControllerTest, CinematicVideoModeTrackingTargetFilter_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest CinematicVideoModeTrackingTargetFilter_002 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->sessionMode =
        static_cast<int32_t>(CameraStandard::SceneMode::CINEMATIC_VIDEO);
    mcCameraTrackingController.currentCameraInfo_->pauseFrameCount = 0;

    mcCameraTrackingController.lastTrackingFrame_ = std::make_shared<TrackingFrameParams>();
    mcCameraTrackingController.lastTrackingFrame_->targetId = 100;
    mcCameraTrackingController.lastTrackingFrame_->objectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_BODY);

    std::shared_ptr<TrackingFrameParams> trackingParams = std::make_shared<TrackingFrameParams>();
    trackingParams->targetId = 200;
    trackingParams->objectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE);

    CameraStandard::Rect trackingRect;
    trackingRect.topLeftX = 0.3f;
    trackingRect.topLeftY = 0.3f;
    trackingRect.width = 0.4f;
    trackingRect.height = 0.4f;
    mcCameraTrackingController.trackingRect_ = trackingRect;

    CameraStandard::Rect lastTrackingRect;
    lastTrackingRect.topLeftX = 0.32f;
    lastTrackingRect.topLeftY = 0.32f;
    lastTrackingRect.width = 0.38f;
    lastTrackingRect.height = 0.38f;
    mcCameraTrackingController.lastTrackingRect_ = lastTrackingRect;

    CameraStandard::FocusTrackingMetaInfo info;

    int32_t ret = mcCameraTrackingController.CinematicVideoModeTrackingTargetFilter(info, trackingParams);

    // isSalientDetectionLocked_ defaults to false, so the first condition
    // (!isSalientDetectionLocked_) is true and SUT returns ERR_OK immediately
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest CinematicVideoModeTrackingTargetFilter_002 end" << std::endl;
}

/**
 * @tc.name  : CinematicVideoModeTrackingTargetFilter_003
 * @tc.number: CinematicVideoModeTrackingTargetFilter_003
 * @tc.desc  : Test CinematicVideoModeTrackingTargetFilter with locked tracking mode.
 */
HWTEST_F(McCameraTrackingControllerTest, CinematicVideoModeTrackingTargetFilter_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest CinematicVideoModeTrackingTargetFilter_003 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->sessionMode =
        static_cast<int32_t>(CameraStandard::SceneMode::CINEMATIC_VIDEO);
    mcCameraTrackingController.currentCameraInfo_->pauseFrameCount = 0;

    mcCameraTrackingController.lastTrackingFrame_ = std::make_shared<TrackingFrameParams>();
    mcCameraTrackingController.lastTrackingFrame_->targetId = 100;

    CameraStandard::FocusTrackingMetaInfo info;

    std::shared_ptr<TrackingFrameParams> trackingParams = std::make_shared<TrackingFrameParams>();
    trackingParams->targetId = 100;

    int32_t ret = mcCameraTrackingController.CinematicVideoModeTrackingTargetFilter(info, trackingParams);
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest CinematicVideoModeTrackingTargetFilter_003 end" << std::endl;
}

/**
 * @tc.name  : IsAllowedConversion_001
 * @tc.number: IsAllowedConversion_001
 * @tc.desc  : Testing IsAllowedConversion function with high IOU.
 */
HWTEST_F(McCameraTrackingControllerTest, IsAllowedConversion_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest IsAllowedConversion_001 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect lastTrackingRect;
    lastTrackingRect.topLeftX = 0.3f;
    lastTrackingRect.topLeftY = 0.3f;
    lastTrackingRect.width = 0.4f;
    lastTrackingRect.height = 0.4f;

    CameraStandard::Rect trackingRect;
    trackingRect.topLeftX = 0.32f;
    trackingRect.topLeftY = 0.32f;
    trackingRect.width = 0.38f;
    trackingRect.height = 0.38f;

    bool result = mcCameraTrackingController.IsAllowedConversion(lastTrackingRect, trackingRect);

    EXPECT_TRUE(result);

    DTEST_LOG << "McCameraTrackingControllerTest IsAllowedConversion_001 end" << std::endl;
}

/**
 * @tc.name  : IsAllowedConversion_002
 * @tc.number: IsAllowedConversion_002
 * @tc.desc  : Testing IsAllowedConversion function with low IOU.
 */
HWTEST_F(McCameraTrackingControllerTest, IsAllowedConversion_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest IsAllowedConversion_002 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect lastTrackingRect;
    lastTrackingRect.topLeftX = 0.1f;
    lastTrackingRect.topLeftY = 0.1f;
    lastTrackingRect.width = 0.2f;
    lastTrackingRect.height = 0.2f;

    CameraStandard::Rect trackingRect;
    trackingRect.topLeftX = 0.7f;
    trackingRect.topLeftY = 0.7f;
    trackingRect.width = 0.2f;
    trackingRect.height = 0.2f;

    bool result = mcCameraTrackingController.IsAllowedConversion(lastTrackingRect, trackingRect);

    EXPECT_FALSE(result);

    DTEST_LOG << "McCameraTrackingControllerTest IsAllowedConversion_002 end" << std::endl;
}

/**
 * @tc.name  : UpdateROI_001
 * @tc.number: UpdateROI_001
 * @tc.desc  : Testing UpdateROI function.
 */
HWTEST_F(McCameraTrackingControllerTest, UpdateROI_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest UpdateROI_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    std::shared_ptr<TrackingFrameParams> trackingFrameParams = std::make_shared<TrackingFrameParams>();
    std::shared_ptr<TrackingFrameParams> trackingFrameParamsNull = nullptr;
    CameraStandard::Rect rect;
    mcCameraTrackingController.currentCameraInfo_->cameraType = CameraType::FRONT;
    mcCameraTrackingController.sensorRotation_ = MobileRotation::UP;

    // Test with nullptr - should return without crash
    mcCameraTrackingController.UpdateROI(trackingFrameParamsNull, rect);

    // Test with empty rect - should set roi to zeros
    mcCameraTrackingController.UpdateROI(trackingFrameParams, rect);
    EXPECT_EQ(trackingFrameParams->roi.x, 0.0f);
    EXPECT_EQ(trackingFrameParams->roi.y, 0.0f);
    EXPECT_EQ(trackingFrameParams->roi.width, 0.0f);
    EXPECT_EQ(trackingFrameParams->roi.height, 0.0f);

    // Test with valid rect - should calculate roi
    rect.width = 4;
    rect.topLeftX = 2;
    rect.height = 4;
    rect.topLeftY = 2;
    mcCameraTrackingController.UpdateROI(trackingFrameParams, rect);
    EXPECT_EQ(trackingFrameParams->roi.width, 2.0f);
    EXPECT_EQ(trackingFrameParams->roi.height, 2.0f);

    // Test with different rotation
    mcCameraTrackingController.sensorRotation_ = MobileRotation::LEFT;
    mcCameraTrackingController.UpdateROI(trackingFrameParams, rect);
    DTEST_LOG << "McCameraTrackingControllerTest UpdateROI_001 end" << std::endl;
}

/**
 * @tc.name  : UpdateROI_002
 * @tc.number: UpdateROI_002
 * @tc.desc  : Testing UpdateROI function.
 */
HWTEST_F(McCameraTrackingControllerTest, UpdateROI_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest UpdateROI_002 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    std::shared_ptr<TrackingFrameParams> trackingFrameParams = std::make_shared<TrackingFrameParams>();
    CameraStandard::Rect rect;
    rect.width = 4;
    rect.topLeftX = 2;
    rect.height = 4;
    rect.topLeftY = 2;
    mcCameraTrackingController.currentCameraInfo_->cameraType = CameraType::BACK;
    mcCameraTrackingController.sensorRotation_ = MobileRotation::UP;

    mcCameraTrackingController.UpdateROI(trackingFrameParams, rect);
    EXPECT_EQ(trackingFrameParams->roi.width, 2.0f);
    EXPECT_EQ(trackingFrameParams->roi.height, 2.0f);

    mcCameraTrackingController.sensorRotation_ = MobileRotation::LEFT;
    mcCameraTrackingController.UpdateROI(trackingFrameParams, rect);
    EXPECT_EQ(trackingFrameParams->roi.width, 2.0f);
    EXPECT_EQ(trackingFrameParams->roi.height, 2.0f);
    DTEST_LOG << "McCameraTrackingControllerTest UpdateROI_002 end" << std::endl;
}

/**
 * @tc.name  : UpdateROI_003
 * @tc.number: UpdateROI_003
 * @tc.desc  : Testing UpdateROI function.
 */
HWTEST_F(McCameraTrackingControllerTest, UpdateROI_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest UpdateROI_003 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    std::shared_ptr<TrackingFrameParams> trackingFrameParams = std::make_shared<TrackingFrameParams>();
    CameraStandard::Rect rect;
    rect.width = 4;
    rect.topLeftX = 2;
    rect.height = 4;
    rect.topLeftY = 2;
    mcCameraTrackingController.currentCameraInfo_->cameraType = CameraType::FRONT;
    mcCameraTrackingController.sensorRotation_ = MobileRotation::RIGHT;

    mcCameraTrackingController.UpdateROI(trackingFrameParams, rect);
    EXPECT_EQ(trackingFrameParams->roi.width, 2.0f);
    EXPECT_EQ(trackingFrameParams->roi.height, 2.0f);

    mcCameraTrackingController.sensorRotation_ = MobileRotation::DOWN;
    mcCameraTrackingController.UpdateROI(trackingFrameParams, rect);
    EXPECT_EQ(trackingFrameParams->roi.width, 2.0f);
    EXPECT_EQ(trackingFrameParams->roi.height, 2.0f);
    DTEST_LOG << "McCameraTrackingControllerTest UpdateROI_003 end" << std::endl;
}

/**
 * @tc.name  : UpdateROI_004
 * @tc.number: UpdateROI_004
 * @tc.desc  : Testing UpdateROI function.
 */
HWTEST_F(McCameraTrackingControllerTest, UpdateROI_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest UpdateROI_004 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    std::shared_ptr<TrackingFrameParams> trackingFrameParams = std::make_shared<TrackingFrameParams>();
    CameraStandard::Rect rect;
    rect.width = 4;
    rect.topLeftX = 2;
    rect.height = 4;
    rect.topLeftY = 2;
    mcCameraTrackingController.currentCameraInfo_->cameraType = CameraType::BACK;
    mcCameraTrackingController.sensorRotation_ = MobileRotation::RIGHT;

    mcCameraTrackingController.UpdateROI(trackingFrameParams, rect);
    EXPECT_EQ(trackingFrameParams->roi.width, 2.0f);
    EXPECT_EQ(trackingFrameParams->roi.height, 2.0f);

    mcCameraTrackingController.sensorRotation_ = MobileRotation::DOWN;
    mcCameraTrackingController.UpdateROI(trackingFrameParams, rect);
    EXPECT_EQ(trackingFrameParams->roi.width, 2.0f);
    EXPECT_EQ(trackingFrameParams->roi.height, 2.0f);
    DTEST_LOG << "McCameraTrackingControllerTest UpdateROI_004 end" << std::endl;
}

/**
 * @tc.name  : UpdateMotionsWithTrackingData_001
 * @tc.number: UpdateMotionsWithTrackingData_001
 * @tc.desc  : Testing UpdateMotionsWithTrackingData function.
 */
HWTEST_F(McCameraTrackingControllerTest, UpdateMotionsWithTrackingData_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest UpdateMotionsWithTrackingData_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    std::shared_ptr<TrackingFrameParams> trackingFrameParams = nullptr;
    int32_t trackingObjectId = 100;

    int32_t ret = mcCameraTrackingController.UpdateMotionsWithTrackingData(trackingFrameParams, trackingObjectId);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "McCameraTrackingControllerTest UpdateMotionsWithTrackingData_001 end" << std::endl;
}

/**
 * @tc.name  : UpdateMotionsWithTrackingData_002
 * @tc.number: UpdateMotionsWithTrackingData_002
 * @tc.desc  : Testing UpdateMotionsWithTrackingData function.
 */
HWTEST_F(McCameraTrackingControllerTest, UpdateMotionsWithTrackingData_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest UpdateMotionsWithTrackingData_002 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    std::shared_ptr<TrackingFrameParams> trackingFrameParams = std::make_shared<TrackingFrameParams>();
    int32_t trackingObjectId = 100;
    int32_t mechId = 100;

    int32_t ret = mcCameraTrackingController.UpdateMotionsWithTrackingData(trackingFrameParams, trackingObjectId);
    EXPECT_EQ(ret, ERR_OK);

    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    std::shared_ptr<MotionManager> motionMgrNull = nullptr;
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgrNull;
    ret = mcCameraTrackingController.UpdateMotionsWithTrackingData(trackingFrameParams, trackingObjectId);
    EXPECT_EQ(ret, ERR_OK);

    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;
    ret = mcCameraTrackingController.UpdateMotionsWithTrackingData(trackingFrameParams, trackingObjectId);
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest UpdateMotionsWithTrackingData_002 end" << std::endl;
}

/**
 * @tc.name  : SearchTargetStop_001
 * @tc.number: SearchTargetStop_001
 * @tc.desc  : Testing SearchTargetStop function.
 */
HWTEST_F(McCameraTrackingControllerTest, SearchTargetStop_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest SearchTargetStop_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    int32_t mechId = 100;
    int32_t mechId1 = 101;

    std::shared_ptr<MotionManager> motionMgr =
            std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;
    MechBodyControllerService::GetInstance().motionManagers_[mechId1] = nullptr;

    // Set searchingTarget to true before calling SearchTargetStop
    mcCameraTrackingController.currentCameraInfo_->searchingTarget = true;
    mcCameraTrackingController.SearchTargetStop();

    // Verify that searchingTarget is set to false after calling SearchTargetStop
    EXPECT_EQ(mcCameraTrackingController.currentCameraInfo_->searchingTarget, false);

    DTEST_LOG << "McCameraTrackingControllerTest SearchTargetStop_001 end" << std::endl;
}

/**
 * @tc.name  : ExecSearchTask_001
 * @tc.number: ExecSearchTask_001
 * @tc.desc  : Testing ExecSearchTask function.
 */
HWTEST_F(McCameraTrackingControllerTest, ExecSearchTask_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest ExecSearchTask_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    std::string cmdId = "mockNapiCmdId";
    int32_t mechId = 100;
    int32_t mechId1 = 101;
    uint32_t tokenId = 1;
    std::shared_ptr<SearchParams> searchParams = std::make_shared<SearchParams>();
    bool startFromNeg = true;
    int32_t searchTimes = 39;

    std::shared_ptr<MotionManager> motionMgr =
            std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;
    MechBodyControllerService::GetInstance().motionManagers_[mechId1] = nullptr;

    mcCameraTrackingController.ExecSearchTask(cmdId, tokenId, startFromNeg, searchTimes);

    // Verify that searchingTarget is set to true when searchTimes < SEARCH_TARGET_ROTATE_TIMES
    EXPECT_EQ(mcCameraTrackingController.currentCameraInfo_->searchingTarget, true);
    EXPECT_EQ(mcCameraTrackingController.currentCameraInfo_->tokenId, tokenId);
    EXPECT_EQ(mcCameraTrackingController.currentCameraInfo_->searchTargetNapiCmdId, cmdId);

    DTEST_LOG << "McCameraTrackingControllerTest ExecSearchTask_001 end" << std::endl;
}

/**
 * @tc.name  : RegisterTrackingEventCallback_001
 * @tc.number: RegisterTrackingEventCallback_001
 * @tc.desc  : Testing RegisterTrackingEventCallback function.
 */
HWTEST_F(McCameraTrackingControllerTest, RegisterTrackingEventCallback_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest RegisterTrackingEventCallback_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    uint32_t tokenId = 100;
    sptr <IRemoteObject> callback;

    int32_t ret = mcCameraTrackingController.RegisterTrackingEventCallback(tokenId, callback);
    EXPECT_EQ(ret, INVALID_PARAMETERS_ERR);
    DTEST_LOG << "McCameraTrackingControllerTest RegisterTrackingEventCallback_001 end" << std::endl;
}

/**
 * @tc.name  : GetCurrentCameraInfo_001
 * @tc.number: GetCurrentCameraInfo_001
 * @tc.desc  : Testing GetCurrentCameraInfo function.
 */
HWTEST_F(McCameraTrackingControllerTest, GetCurrentCameraInfo_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest GetCurrentCameraInfo_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();

    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->targetId = 42;
    mcCameraTrackingController.currentCameraInfo_->sessionMode = 1;

    std::shared_ptr<CameraInfo> cameraInfo = mcCameraTrackingController.GetCurrentCameraInfo();
    ASSERT_NE(cameraInfo, nullptr);
    EXPECT_EQ(cameraInfo->targetId, 42);
    EXPECT_EQ(cameraInfo->sessionMode, 1);

    DTEST_LOG << "McCameraTrackingControllerTest GetCurrentCameraInfo_001 end" << std::endl;
}

/**
 * @tc.name  : SensorCallback_001
 * @tc.number: SensorCallback_001
 * @tc.desc  : Testing SensorCallback function.
 */
HWTEST_F(McCameraTrackingControllerTest, SensorCallback_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest SensorCallback_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    SensorEvent sensorEvent;
    SensorEvent* event = &sensorEvent;
    SensorEvent* eventNull = nullptr;

    // Test with nullptr - should return without crash
    MobileRotation rotationBeforeNull = mcCameraTrackingController.sensorRotation_;
    mcCameraTrackingController.SensorCallback(eventNull);
    MobileRotation rotationAfterNull = mcCameraTrackingController.sensorRotation_;
    EXPECT_EQ(rotationBeforeNull, rotationAfterNull);

    // Test with valid event - should process without crash
    GravityData gravityData;
    gravityData.x = 1;
    gravityData.y = 1;
    gravityData.z = 1;
    sensorEvent.sensorTypeId = SENSOR_TYPE_ID_GRAVITY;
    sensorEvent.data = reinterpret_cast<uint8_t*>(&gravityData);
    mcCameraTrackingController.sensorRotation_ = MobileRotation::INVALID;

    mcCameraTrackingController.SensorCallback(event);
    MobileRotation rotationAfter = mcCameraTrackingController.sensorRotation_;
    EXPECT_EQ(rotationAfter, MobileRotation::UP);

    DTEST_LOG << "McCameraTrackingControllerTest SensorCallback_001 end" << std::endl;
}

/**
 * @tc.name  : CalculateSensorRotation_001
 * @tc.number: CalculateSensorRotation_001
 * @tc.desc  : Testing CalculateSensorRotation function.
 */
HWTEST_F(McCameraTrackingControllerTest, CalculateSensorRotation_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest CalculateSensorRotation_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    GravityData gravityData;
    gravityData.x = 1;
    gravityData.y = 1;
    gravityData.z = 1;
    GravityData* data = &gravityData;

    MobileRotation rotation1 = mcCameraTrackingController.CalculateSensorRotation(data);
    EXPECT_EQ(rotation1, MobileRotation::UP);

    gravityData.z = 5;
    GravityData* dataInvaild = &gravityData;
    MobileRotation rotation2 = mcCameraTrackingController.CalculateSensorRotation(dataInvaild);
    EXPECT_EQ(rotation2, MobileRotation::INVALID);
    DTEST_LOG << "McCameraTrackingControllerTest CalculateSensorRotation_001 end" << std::endl;
}

/**
 * @tc.name  : ConvertObjectType_001
 * @tc.number: ConvertObjectType_001
 * @tc.desc  : Testing ConvertObjectType function.
 */
HWTEST_F(McCameraTrackingControllerTest, ConvertObjectType_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest ConvertObjectType_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    uint8_t mechObjectType = 0;
    CameraStandard::MetadataObjectType cameraObjectType = CameraStandard::MetadataObjectType::INVALID;
    mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER));

    cameraObjectType = CameraStandard::MetadataObjectType::FACE;
    mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE));

    cameraObjectType = CameraStandard::MetadataObjectType::HUMAN_BODY;
    mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_BODY));

    cameraObjectType = CameraStandard::MetadataObjectType::CAT_FACE;
    mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER));

    cameraObjectType = CameraStandard::MetadataObjectType::CAT_BODY;
    mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER));

    cameraObjectType = CameraStandard::MetadataObjectType::DOG_FACE;
    mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER));

    cameraObjectType = CameraStandard::MetadataObjectType::DOG_BODY;
    mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER));

    cameraObjectType = CameraStandard::MetadataObjectType::SALIENT_DETECTION;
    mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER));

    cameraObjectType = CameraStandard::MetadataObjectType::BAR_CODE_DETECTION;
    mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER));

    cameraObjectType = CameraStandard::MetadataObjectType::BASE_FACE_DETECTION;
    mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE));

    cameraObjectType = CameraStandard::MetadataObjectType::HUMAN_HEAD;
    mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_HEAD));

    DTEST_LOG << "McCameraTrackingControllerTest ConvertObjectType_001 end" << std::endl;
}

/**
 * @tc.name  : SetTrackingEnabled_001
 * @tc.number: SetTrackingEnabled_001
 * @tc.desc  : Testing SetTrackingEnabled function.
 */
HWTEST_F(McCameraTrackingControllerTest, SetTrackingEnabled_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest SetTrackingEnabled_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    uint32_t tokenId = 100;
    bool isEnabled = true;
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);

    std::shared_ptr<MotionManager> motionMgrNull = nullptr;
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgrNull;
    EXPECT_EQ(mcCameraTrackingController.SetTrackingEnabled(tokenId, isEnabled), ERR_OK);
    
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;
    EXPECT_EQ(mcCameraTrackingController.SetTrackingEnabled(tokenId, isEnabled), ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest SetTrackingEnabled_001 end" << std::endl;
}

/**
 * @tc.name  : SetTrackingLayout_001
 * @tc.number: SetTrackingLayout_001
 * @tc.desc  : Testing SetTrackingLayout function.
 */
HWTEST_F(McCameraTrackingControllerTest, SetTrackingLayout_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest SetTrackingLayout_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    CameraTrackingLayout cameraTrackingLayout;
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);

    std::shared_ptr<MotionManager> motionMgrNull = nullptr;
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgrNull;
    EXPECT_EQ(mcCameraTrackingController.SetTrackingLayout(cameraTrackingLayout), INVALID_TRACKING_LAYOUT);

    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;
    EXPECT_EQ(mcCameraTrackingController.SetTrackingLayout(cameraTrackingLayout), INVALID_TRACKING_LAYOUT);

    mcCameraTrackingController.sensorRotation_ = MobileRotation::DOWN;
    EXPECT_EQ(mcCameraTrackingController.SetTrackingLayout(cameraTrackingLayout), INVALID_TRACKING_LAYOUT);

    mcCameraTrackingController.sensorRotation_ = MobileRotation::LEFT;
    EXPECT_EQ(mcCameraTrackingController.SetTrackingLayout(cameraTrackingLayout), INVALID_TRACKING_LAYOUT);

    mcCameraTrackingController.sensorRotation_ = MobileRotation::RIGHT;
    EXPECT_EQ(mcCameraTrackingController.SetTrackingLayout(cameraTrackingLayout), INVALID_TRACKING_LAYOUT);

    DTEST_LOG << "McCameraTrackingControllerTest SetTrackingLayout_001 end" << std::endl;
}

/**
 * @tc.name  : SetTrackingLayout_002
 * @tc.number: SetTrackingLayout_002
 * @tc.desc  : Testing SetTrackingLayout function.
 */
HWTEST_F(McCameraTrackingControllerTest, SetTrackingLayout_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest SetTrackingLayout_002 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    uint32_t tokenId = 1;
    CameraTrackingLayout cameraTrackingLayout;
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;
    std::shared_ptr<AppSetting> setting = std::make_shared<AppSetting>();
    std::shared_ptr<AppSetting> settingNull = nullptr;
    mcCameraTrackingController.appSettings[tokenId] = setting;
    EXPECT_EQ(mcCameraTrackingController.SetTrackingLayout(tokenId, cameraTrackingLayout), ERR_OK);

    mcCameraTrackingController.appSettings[tokenId] = settingNull;
    EXPECT_EQ(mcCameraTrackingController.SetTrackingLayout(tokenId, cameraTrackingLayout), ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest SetTrackingLayout_002 end" << std::endl;
}

/**
 * @tc.name  : GetTrackingEnabled_001
 * @tc.number: GetTrackingEnabled_001
 * @tc.desc  : Testing GetTrackingEnabled function.
 */
HWTEST_F(McCameraTrackingControllerTest, GetTrackingEnabled_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingEnabled_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    uint32_t tokenId = 100;
    bool isEnabled = true;
    std::shared_ptr<AppSetting> setting = std::make_shared<AppSetting>();
    mcCameraTrackingController.appSettings[tokenId] = setting;
    EXPECT_EQ(mcCameraTrackingController.GetTrackingEnabled(tokenId, isEnabled), ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingEnabled_001 end" << std::endl;
}

/**
 * @tc.name  : UnRegisterSensorListener_001
 * @tc.number: UnRegisterSensorListener_001
 * @tc.desc  : Testing UnRegisterSensorListener function.
 */
HWTEST_F(McCameraTrackingControllerTest, UnRegisterSensorListener_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest UnRegisterSensorListener_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();

    // Test with nullptr callback - should return without crash
    mcCameraTrackingController.user.callback = nullptr;
    mcCameraTrackingController.UnRegisterSensorListener();
    EXPECT_EQ(mcCameraTrackingController.user.callback, nullptr);

    // Test with valid callback - should unregister and set to nullptr
    mcCameraTrackingController.user.callback = McCameraTrackingController::SensorCallback;
    mcCameraTrackingController.UnRegisterSensorListener();
    EXPECT_EQ(mcCameraTrackingController.user.callback, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTest UnRegisterSensorListener_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustROI_FrontCamera_UP_001
 * @tc.number: AdjustROI_FrontCamera_UP_001
 * @tc.desc  : Testing AdjustROI function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustROI_FrontCamera_UP_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustROI_FrontCamera_UP_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi;
    roi.width = 0.2f;
    roi.height = 0.2f;
    CameraStandard::Rect rect;
    rect.topLeftX = 0.5f;
    rect.topLeftY = 0.5f;
    CameraType cameraType = CameraType::FRONT;
    MobileRotation sensorRotation = MobileRotation::UP;

    controller.AdjustROI(roi, rect, cameraType, sensorRotation);
    EXPECT_FLOAT_EQ(roi.x, 1.0f - 0.5f - 0.1f);
    EXPECT_FLOAT_EQ(roi.y, 1.0f - 0.5f - 0.1f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustROI_FrontCamera_UP_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustROI_FrontCamera_LEFT_001
 * @tc.number: AdjustROI_FrontCamera_LEFT_001
 * @tc.desc  : Testing AdjustROI function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustROI_FrontCamera_LEFT_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustROI_FrontCamera_LEFT_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi;
    roi.width = 0.2f;
    roi.height = 0.2f;
    CameraStandard::Rect rect;
    rect.topLeftX = 0.5f;
    rect.topLeftY = 0.5f;
    CameraType cameraType = CameraType::FRONT;
    MobileRotation sensorRotation = MobileRotation::LEFT;

    controller.AdjustROI(roi, rect, cameraType, sensorRotation);
    EXPECT_FLOAT_EQ(roi.x, 1.0f - 0.5f - 0.1f);
    EXPECT_FLOAT_EQ(roi.y, 1.0f - 0.5f - 0.1f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustROI_FrontCamera_LEFT_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustROI_FrontCamera_RIGHT_001
 * @tc.number: AdjustROI_FrontCamera_RIGHT_001
 * @tc.desc  : Testing AdjustROI function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustROI_FrontCamera_RIGHT_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustROI_FrontCamera_RIGHT_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi;
    roi.width = 0.2f;
    roi.height = 0.2f;
    CameraStandard::Rect rect;
    rect.topLeftX = 0.5f;
    rect.topLeftY = 0.5f;
    CameraType cameraType = CameraType::FRONT;
    MobileRotation sensorRotation = MobileRotation::RIGHT;

    controller.AdjustROI(roi, rect, cameraType, sensorRotation);
    EXPECT_FLOAT_EQ(roi.x, 0.5f + 0.1f);
    EXPECT_FLOAT_EQ(roi.y, 0.5f + 0.1f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustROI_FrontCamera_RIGHT_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustROI_BackCamera_UP_001
 * @tc.number: AdjustROI_BackCamera_UP_001
 * @tc.desc  : Testing AdjustROI function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustROI_BackCamera_UP_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustROI_BackCamera_UP_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi;
    roi.width = 0.2f;
    roi.height = 0.2f;
    CameraStandard::Rect rect;
    rect.topLeftX = 0.5f;
    rect.topLeftY = 0.5f;
    CameraType cameraType = CameraType::BACK;
    MobileRotation sensorRotation = MobileRotation::UP;

    controller.AdjustROI(roi, rect, cameraType, sensorRotation);
    EXPECT_FLOAT_EQ(roi.x, 0.5f + 0.1f);
    EXPECT_FLOAT_EQ(roi.y, 0.5f + 0.1f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustROI_BackCamera_UP_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustROI_BackCamera_RIGHT_001
 * @tc.number: AdjustROI_BackCamera_RIGHT_001
 * @tc.desc  : Testing AdjustROI function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustROI_BackCamera_RIGHT_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustROI_BackCamera_RIGHT_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi;
    roi.width = 0.2f;
    roi.height = 0.2f;
    CameraStandard::Rect rect;
    rect.topLeftX = 0.5f;
    rect.topLeftY = 0.5f;
    CameraType cameraType = CameraType::BACK;
    MobileRotation sensorRotation = MobileRotation::RIGHT;

    controller.AdjustROI(roi, rect, cameraType, sensorRotation);
    EXPECT_FLOAT_EQ(roi.x, 1.0f - 0.5f - 0.1f);
    EXPECT_FLOAT_EQ(roi.y, 1.0f - 0.5f - 0.1f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustROI_BackCamera_RIGHT_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustYOffset_Zero_001
 * @tc.number: AdjustYOffset_Zero_001
 * @tc.desc  : Testing AdjustYOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustYOffset_Zero_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Zero_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.0f, 0.0f, 0.0f, 0.0f};
    CameraType cameraType = CameraType::FRONT;
    bool isFace = true;
    bool isBody = false;
    float x = 0.0f;
    float y = 0.0f;

    controller.AdjustYOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.y, 0.3f);
    EXPECT_FLOAT_EQ(roi.x, 0.1f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Zero_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustYOffset_Front_Left_001
 * @tc.number: AdjustYOffset_Front_Left_001
 * @tc.desc  : Testing AdjustYOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustYOffset_Front_Left_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Front_Left_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::FRONT;
    bool isFace = true;
    bool isBody = false;
    float x = 0.0f;
    float y = 0.5f;

    controller.AdjustYOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.y, 0.7f);
    EXPECT_FLOAT_EQ(roi.x, 0.6f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Front_Left_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustYOffset_Front_Right_001
 * @tc.number: AdjustYOffset_Front_Right_001
 * @tc.desc  : Testing AdjustYOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustYOffset_Front_Right_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Front_Right_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::FRONT;
    bool isFace = true;
    bool isBody = false;
    float x = 0.0f;
    float y = -0.5f;

    controller.AdjustYOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.y, 0.3f);
    EXPECT_FLOAT_EQ(roi.x, 0.6f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Front_Right_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustYOffset_Back_Left_001
 * @tc.number: AdjustYOffset_Back_Left_001
 * @tc.desc  : Testing AdjustYOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustYOffset_Back_Left_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Back_Left_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::BACK;
    bool isFace = true;
    bool isBody = false;
    float x = 0.0f;
    float y = -0.5f;

    controller.AdjustYOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.y, 0.7f);
    EXPECT_FLOAT_EQ(roi.x, 0.6f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Back_Left_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustYOffset_Back_Right_001
 * @tc.number: AdjustYOffset_Back_Right_001
 * @tc.desc  : Testing AdjustYOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustYOffset_Back_Right_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Back_Right_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::BACK;
    bool isFace = true;
    bool isBody = false;
    float x = 0.0f;
    float y = 0.5f;

    controller.AdjustYOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.y, 0.3f);
    EXPECT_FLOAT_EQ(roi.x, 0.6f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Back_Right_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustYOffset_Up_001
 * @tc.number: AdjustYOffset_Up_001
 * @tc.desc  : Testing AdjustYOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustYOffset_Up_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Up_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.0f, 0.0f, 1.0f, 1.0f};
    CameraType cameraType = CameraType::BACK;
    bool isFace = true;
    bool isBody = false;
    float x = -0.5f;
    float y = 0.0f;

    controller.AdjustYOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.y, 0.3f);
    EXPECT_FLOAT_EQ(roi.x, 0.2f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Up_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustYOffset_Down_001
 * @tc.number: AdjustYOffset_Down_001
 * @tc.desc  : Testing AdjustYOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustYOffset_Down_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Down_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.0f, 0.0f, 0.0f, 0.0f};
    CameraType cameraType = CameraType::BACK;
    bool isFace = true;
    bool isBody = false;
    float x = 0.5f;
    float y = 0.0f;

    controller.AdjustYOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.y, 0.3f);
    EXPECT_FLOAT_EQ(roi.x, -0.2f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Down_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_Zero_001
 * @tc.number: AdjustXOffset_Zero_001
 * @tc.desc  : Testing AdjustXOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustXOffset_Zero_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_Zero_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.0f, 0.0f, 0.0f, 0.0f};
    CameraType cameraType = CameraType::FRONT;
    bool isFace = true;
    bool isBody = false;
    float x = 0.0f;
    float y = 0.0f;

    controller.AdjustXOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.y, -0.1f);
    EXPECT_FLOAT_EQ(roi.x, 0.3f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_Zero_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_Front_Left_001
 * @tc.number: AdjustXOffset_Front_Left_001
 * @tc.desc  : Testing AdjustXOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustXOffset_Front_Left_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_Front_Left_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::FRONT;
    bool isFace = true;
    bool isBody = false;
    float x = 0.5f;
    float y = 0.0f;

    controller.AdjustXOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.y, 0.7f);
    EXPECT_FLOAT_EQ(roi.x, 0.5f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_Front_Left_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_Front_Right_001
 * @tc.number: AdjustXOffset_Front_Right_001
 * @tc.desc  : Testing AdjustXOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustXOffset_Front_Right_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_Front_Right_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::FRONT;
    bool isFace = true;
    bool isBody = false;
    float x = -0.5f;
    float y = 0.0f;

    controller.AdjustXOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.y, 0.3f);
    EXPECT_FLOAT_EQ(roi.x, 0.5f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_Front_Right_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_Back_Left_001
 * @tc.number: AdjustXOffset_Back_Left_001
 * @tc.desc  : Testing AdjustXOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustXOffset_Back_Left_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_Back_Left_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::BACK;
    bool isFace = true;
    bool isBody = false;
    float x = -0.5f;
    float y = 0.0f;

    controller.AdjustXOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.y, 0.3f);
    EXPECT_FLOAT_EQ(roi.x, 0.5f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_Back_Left_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_Back_Right_001
 * @tc.number: AdjustXOffset_Back_Right_001
 * @tc.desc  : Testing AdjustXOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustXOffset_Back_Right_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_Back_Right_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::BACK;
    bool isFace = true;
    bool isBody = false;
    float x = 0.5f;
    float y = 0.0f;

    controller.AdjustXOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.y, 0.7f);
    EXPECT_FLOAT_EQ(roi.x, 0.5f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_Back_Right_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_Up_001
 * @tc.number: AdjustXOffset_Up_001
 * @tc.desc  : Testing AdjustXOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustXOffset_Up_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_Up_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.0f, 0.0f, 1.0f, 1.0f};
    CameraType cameraType = CameraType::BACK;
    bool isFace = true;
    bool isBody = false;
    float x = 0.0f;
    float y = -0.5f;

    controller.AdjustXOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.y, 0.1f);
    EXPECT_FLOAT_EQ(roi.x, 0.3f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_Up_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_Down_001
 * @tc.number: AdjustXOffset_Down_001
 * @tc.desc  : Testing AdjustXOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustXOffset_Down_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_Down_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.0f, 0.0f, 0.0f, 0.0f};
    CameraType cameraType = CameraType::BACK;
    bool isFace = true;
    bool isBody = false;
    float x = 0.0f;
    float y = 0.5f;

    controller.AdjustXOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.y, 0.1f);
    EXPECT_FLOAT_EQ(roi.x, 0.3f);

    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_Down_001 end" << std::endl;
}

/**
 * @tc.name  : SetStickOffset_001
 * @tc.number: SetStickOffset_001
 * @tc.desc  : Testing SetStickOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, SetStickOffset_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest SetStickOffset_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    int16_t x = 0;
    int16_t y = 0;

    int32_t result = controller.SetStickOffset(x, y);
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest SetStickOffset_001 end" << std::endl;
}

/**
 * @tc.name  : SetStickOffset_002
 * @tc.number: SetStickOffset_002
 * @tc.desc  : Testing SetStickOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, SetStickOffset_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest SetStickOffset_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    int16_t x = -1;
    int16_t y = -1;

    int32_t result = controller.SetStickOffset(x, y);
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest SetStickOffset_002 end" << std::endl;
}

/**
 * @tc.name  : SetStickOffset_003
 * @tc.number: SetStickOffset_003
 * @tc.desc  : Testing SetStickOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, SetStickOffset_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest SetStickOffset_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    int16_t x = 1;
    int16_t y = -1;

    int32_t result = controller.SetStickOffset(x, y);
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest SetStickOffset_003 end" << std::endl;
}

/**
 * @tc.name  : SetStickOffset_004
 * @tc.number: SetStickOffset_004
 * @tc.desc  : Testing SetStickOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, SetStickOffset_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest SetStickOffset_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    int16_t x = -1;
    int16_t y = 1;

    int32_t result = controller.SetStickOffset(x, y);
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest SetStickOffset_004 end" << std::endl;
}

/**
 * @tc.name  : SetStickOffset_005
 * @tc.number: SetStickOffset_005
 * @tc.desc  : Testing SetStickOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, SetStickOffset_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest SetStickOffset_005 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    int16_t x = 1;
    int16_t y = 1;

    int32_t result = controller.SetStickOffset(x, y);
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest SetStickOffset_005 end" << std::endl;
}

/**
 * @tc.name  : GetTokenIdOfCurrentCameraInfo_001
 * @tc.number: GetTokenIdOfCurrentCameraInfo_001
 * @tc.desc  : Testing GetTokenIdOfCurrentCameraInfo when currentCameraInfo_ is nullptr.
 */
HWTEST_F(McCameraTrackingControllerTest, GetTokenIdOfCurrentCameraInfo_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest GetTokenIdOfCurrentCameraInfo_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = nullptr;

    uint32_t tokenId = controller.GetTokenIdOfCurrentCameraInfo();
    EXPECT_EQ(tokenId, 0);

    DTEST_LOG << "McCameraTrackingControllerTest GetTokenIdOfCurrentCameraInfo_001 end" << std::endl;
}

/**
 * @tc.name  : GetTokenIdOfCurrentCameraInfo_002
 * @tc.number: GetTokenIdOfCurrentCameraInfo_002
 * @tc.desc  : Testing GetTokenIdOfCurrentCameraInfo when currentCameraInfo_ is valid.
 */
HWTEST_F(McCameraTrackingControllerTest, GetTokenIdOfCurrentCameraInfo_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest GetTokenIdOfCurrentCameraInfo_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 12345;

    uint32_t tokenId = controller.GetTokenIdOfCurrentCameraInfo();
    EXPECT_EQ(tokenId, 12345);

    DTEST_LOG << "McCameraTrackingControllerTest GetTokenIdOfCurrentCameraInfo_002 end" << std::endl;
}

/**
 * @tc.name  : UpdateCurrentCameraInfoByCaptureSessionInfo_001
 * @tc.number: UpdateCurrentCameraInfoByCaptureSessionInfo_001
 * @tc.desc  : Testing UpdateCurrentCameraInfoByCaptureSessionInfo when currentCameraInfo_ is nullptr.
 */
HWTEST_F(McCameraTrackingControllerTest, UpdateCurrentCameraInfoByCaptureSessionInfo_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest UpdateCurrentCameraInfoByCaptureSessionInfo_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = nullptr;

    CameraStandard::CaptureSessionInfo sessionInfo;
    sessionInfo.position = 1;
    sessionInfo.callerTokenId = 12345;
    sessionInfo.sessionStatus = true;
    sessionInfo.zoomInfo.zoomValue = 1.5f;
    sessionInfo.zoomInfo.equivalentFocus = 50.0f;
    sessionInfo.zoomInfo.videoStabilizationMode = 1;
    sessionInfo.sessionMode = static_cast<int32_t>(CameraStandard::SceneMode::VIDEO);

    controller.UpdateCurrentCameraInfoByCaptureSessionInfo(sessionInfo);
    EXPECT_NE(controller.currentCameraInfo_, nullptr);
    EXPECT_EQ(controller.currentCameraInfo_->tokenId, 12345);
    EXPECT_EQ(controller.currentCameraInfo_->cameraType, CameraType::BACK);
    EXPECT_EQ(controller.currentCameraInfo_->isRecording, true);
    EXPECT_FLOAT_EQ(controller.currentCameraInfo_->zoomFactor, 1.5f);
    EXPECT_FLOAT_EQ(controller.currentCameraInfo_->equivalentFocus, 50.0f);
    EXPECT_EQ(controller.currentCameraInfo_->videoStabilizationMode, 1);
    EXPECT_EQ(controller.currentCameraInfo_->isCameraOn, true);

    DTEST_LOG << "McCameraTrackingControllerTest UpdateCurrentCameraInfoByCaptureSessionInfo_001 end" << std::endl;
}

/**
 * @tc.name  : UpdateCurrentCameraInfoByCaptureSessionInfo_002
 * @tc.number: UpdateCurrentCameraInfoByCaptureSessionInfo_002
 * @tc.desc  : Testing UpdateCurrentCameraInfoByCaptureSessionInfo with camera type switch.
 */
HWTEST_F(McCameraTrackingControllerTest, UpdateCurrentCameraInfoByCaptureSessionInfo_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest UpdateCurrentCameraInfoByCaptureSessionInfo_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->cameraType = CameraType::FRONT;
    controller.currentCameraInfo_->tokenId = 1001;

    CameraStandard::CaptureSessionInfo sessionInfo;
    sessionInfo.position = 1;
    sessionInfo.callerTokenId = 12346;
    sessionInfo.sessionStatus = true;
    sessionInfo.zoomInfo.zoomValue = 2.0f;
    sessionInfo.zoomInfo.equivalentFocus = 60.0f;
    sessionInfo.zoomInfo.videoStabilizationMode = 2;
    sessionInfo.sessionMode = static_cast<int32_t>(CameraStandard::SceneMode::VIDEO);

    controller.UpdateCurrentCameraInfoByCaptureSessionInfo(sessionInfo);
    EXPECT_EQ(controller.currentCameraInfo_->tokenId, 12346);
    EXPECT_EQ(controller.currentCameraInfo_->cameraType, CameraType::BACK);
    EXPECT_EQ(controller.currentCameraInfo_->isRecording, true);
    EXPECT_FLOAT_EQ(controller.currentCameraInfo_->zoomFactor, 2.0f);
    EXPECT_FLOAT_EQ(controller.currentCameraInfo_->equivalentFocus, 60.0f);
    EXPECT_EQ(controller.currentCameraInfo_->videoStabilizationMode, 2);
    EXPECT_EQ(controller.currentCameraInfo_->isCameraOn, true);

    DTEST_LOG << "McCameraTrackingControllerTest UpdateCurrentCameraInfoByCaptureSessionInfo_002 end" << std::endl;
}

/**
 * @tc.name  : UpdateCurrentCameraInfoByCaptureSessionInfo_003
 * @tc.number: UpdateCurrentCameraInfoByCaptureSessionInfo_003
 * @tc.desc  : Testing UpdateCurrentCameraInfoByCaptureSessionInfo with VIDEO output type.
 */
HWTEST_F(McCameraTrackingControllerTest, UpdateCurrentCameraInfoByCaptureSessionInfo_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest UpdateCurrentCameraInfoByCaptureSessionInfo_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->cameraType = CameraType::BACK;
    controller.currentCameraInfo_->tokenId = 1002;

    CameraStandard::CaptureSessionInfo sessionInfo;
    sessionInfo.position = 1;
    sessionInfo.callerTokenId = 1002;
    sessionInfo.sessionStatus = true;
    sessionInfo.zoomInfo.zoomValue = 1.5f;
    sessionInfo.zoomInfo.equivalentFocus = 50.0f;
    sessionInfo.zoomInfo.videoStabilizationMode = 1;
    sessionInfo.sessionMode = static_cast<int32_t>(CameraStandard::SceneMode::VIDEO);

    CameraStandard::OutputInfo outputInfo;
    outputInfo.type = CameraStandard::OutputType::VIDEO;
    outputInfo.width = 1920;
    outputInfo.height = 1080;
    sessionInfo.outputInfos.push_back(outputInfo);

    controller.UpdateCurrentCameraInfoByCaptureSessionInfo(sessionInfo);
    EXPECT_EQ(controller.currentCameraInfo_->width, 1920);
    EXPECT_EQ(controller.currentCameraInfo_->height, 1080);
    EXPECT_EQ(controller.currentCameraInfo_->tokenId, 1002);
    EXPECT_EQ(controller.currentCameraInfo_->cameraType, CameraType::BACK);
    EXPECT_EQ(controller.currentCameraInfo_->isRecording, true);
    EXPECT_FLOAT_EQ(controller.currentCameraInfo_->zoomFactor, 1.5f);
    EXPECT_FLOAT_EQ(controller.currentCameraInfo_->equivalentFocus, 50.0f);
    EXPECT_EQ(controller.currentCameraInfo_->videoStabilizationMode, 1);
    EXPECT_EQ(controller.currentCameraInfo_->isCameraOn, true);

    DTEST_LOG << "McCameraTrackingControllerTest UpdateCurrentCameraInfoByCaptureSessionInfo_003 end" << std::endl;
}

/**
 * @tc.name  : UpdateCurrentCameraInfoByCaptureSessionInfo_004
 * @tc.number: UpdateCurrentCameraInfoByCaptureSessionInfo_004
 * @tc.desc  : Testing UpdateCurrentCameraInfoByCaptureSessionInfo with PREVIEW output type.
 */
HWTEST_F(McCameraTrackingControllerTest, UpdateCurrentCameraInfoByCaptureSessionInfo_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest UpdateCurrentCameraInfoByCaptureSessionInfo_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->cameraType = CameraType::BACK;
    controller.currentCameraInfo_->tokenId = 1003;

    CameraStandard::CaptureSessionInfo sessionInfo;
    sessionInfo.position = 1;
    sessionInfo.callerTokenId = 1003;
    sessionInfo.sessionStatus = true;
    sessionInfo.zoomInfo.zoomValue = 1.5f;
    sessionInfo.zoomInfo.equivalentFocus = 50.0f;
    sessionInfo.zoomInfo.videoStabilizationMode = 1;
    sessionInfo.sessionMode = static_cast<int32_t>(CameraStandard::SceneMode::VIDEO);

    CameraStandard::OutputInfo outputInfo;
    outputInfo.type = CameraStandard::OutputType::PREVIEW;
    outputInfo.width = 1280;
    outputInfo.height = 720;
    sessionInfo.outputInfos.push_back(outputInfo);

    controller.UpdateCurrentCameraInfoByCaptureSessionInfo(sessionInfo);
    EXPECT_EQ(controller.currentCameraInfo_->width, 1280);
    EXPECT_EQ(controller.currentCameraInfo_->height, 720);
    EXPECT_EQ(controller.currentCameraInfo_->tokenId, 1003);
    EXPECT_EQ(controller.currentCameraInfo_->cameraType, CameraType::BACK);
    EXPECT_EQ(controller.currentCameraInfo_->isRecording, true);
    EXPECT_FLOAT_EQ(controller.currentCameraInfo_->zoomFactor, 1.5f);
    EXPECT_FLOAT_EQ(controller.currentCameraInfo_->equivalentFocus, 50.0f);
    EXPECT_EQ(controller.currentCameraInfo_->videoStabilizationMode, 1);
    EXPECT_EQ(controller.currentCameraInfo_->isCameraOn, true);

    DTEST_LOG << "McCameraTrackingControllerTest UpdateCurrentCameraInfoByCaptureSessionInfo_004 end" << std::endl;
}

/**
 * @tc.name  : GetTrackingTargetFallback_002
 * @tc.number: GetTrackingTargetFallback_002
 * @tc.desc  : Testing GetTrackingTargetFallback with SALIENT_DETECTION and isLockFocusTracked true.
 */
HWTEST_F(McCameraTrackingControllerTest, GetTrackingTargetFallback_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingTargetFallback_002 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;

    CameraStandard::Rect objectRect;
    objectRect.topLeftX = 0.3f;
    objectRect.topLeftY = 0.3f;
    objectRect.width = 0.4f;
    objectRect.height = 0.4f;

    CameraStandard::MetaObjectParms parms;
    parms.type = CameraStandard::MetadataObjectType::SALIENT_DETECTION;
    parms.timestamp = 0;
    parms.box = objectRect;
    parms.objectId = 100;
    parms.confidence = 0;
    parms.isLockFocusTracked = true;

    sptr<CameraStandard::MetadataObject> mockObject =
        new CameraStandard::MetadataObject(parms);

    detectedObjects.push_back(mockObject);

    sptr<CameraStandard::MetadataObject> targetObject;

    mcCameraTrackingController.lastTrackingFrame_ = std::make_shared<TrackingFrameParams>();
    mcCameraTrackingController.lastTrackingFrame_->targetId = 100;

    int32_t ret = mcCameraTrackingController.GetTrackingTargetFallback(trackingRegion, detectedObjects, targetObject);

    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(mcCameraTrackingController.isSalientDetectionLocked_);
    EXPECT_NE(targetObject, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingTargetFallback_002 end" << std::endl;
}

/**
 * @tc.name  : GetTrackingTargetFallback_003
 * @tc.number: GetTrackingTargetFallback_003
 * @tc.desc  : Testing GetTrackingTargetFallback without SALIENT_DETECTION with isLockFocusTracked true.
 */
HWTEST_F(McCameraTrackingControllerTest, GetTrackingTargetFallback_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingTargetFallback_003 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;

    CameraStandard::Rect objectRect;
    objectRect.topLeftX = 0.3f;
    objectRect.topLeftY = 0.3f;
    objectRect.width = 0.4f;
    objectRect.height = 0.4f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> mockObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE, timestamp, objectRect, 100, 0);

    detectedObjects.push_back(mockObject);

    sptr<CameraStandard::MetadataObject> targetObject;

    mcCameraTrackingController.lastTrackingFrame_ = std::make_shared<TrackingFrameParams>();
    mcCameraTrackingController.lastTrackingFrame_->targetId = 100;

    int32_t ret = mcCameraTrackingController.GetTrackingTargetFallback(trackingRegion, detectedObjects, targetObject);

    EXPECT_EQ(ret, ERR_OK);
    EXPECT_FALSE(mcCameraTrackingController.isSalientDetectionLocked_);
    EXPECT_NE(targetObject, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingTargetFallback_003 end" << std::endl;
}

/**
 * @tc.name  : GetTrackingTargetFallback_004
 * @tc.number: GetTrackingTargetFallback_004
 * @tc.desc  : Testing GetTrackingTargetFallback with SALIENT_DETECTION but isLockFocusTracked false.
 */
HWTEST_F(McCameraTrackingControllerTest, GetTrackingTargetFallback_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingTargetFallback_004 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;

    CameraStandard::Rect objectRect;
    objectRect.topLeftX = 0.3f;
    objectRect.topLeftY = 0.3f;
    objectRect.width = 0.4f;
    objectRect.height = 0.4f;

    CameraStandard::MetaObjectParms parms;
    parms.type = CameraStandard::MetadataObjectType::SALIENT_DETECTION;
    parms.timestamp = 0;
    parms.box = objectRect;
    parms.objectId = 100;
    parms.confidence = 0;
    parms.isLockFocusTracked = false;

    sptr<CameraStandard::MetadataObject> mockObject =
        new CameraStandard::MetadataObject(parms);

    detectedObjects.push_back(mockObject);

    sptr<CameraStandard::MetadataObject> targetObject;

    mcCameraTrackingController.lastTrackingFrame_ = std::make_shared<TrackingFrameParams>();
    mcCameraTrackingController.lastTrackingFrame_->targetId = 100;

    int32_t ret = mcCameraTrackingController.GetTrackingTargetFallback(trackingRegion, detectedObjects, targetObject);

    EXPECT_EQ(ret, ERR_OK);
    EXPECT_FALSE(mcCameraTrackingController.isSalientDetectionLocked_);
    EXPECT_NE(targetObject, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTest GetTrackingTargetFallback_004 end" << std::endl;
}

/**
 * @tc.name  : CinematicVideoModeTrackingTargetFilter_004
 * @tc.number: CinematicVideoModeTrackingTargetFilter_004
 * @tc.desc  : Testing CinematicVideoModeTrackingTargetFilter with isSalientDetectionLocked false.
 */
HWTEST_F(McCameraTrackingControllerTest, CinematicVideoModeTrackingTargetFilter_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest CinematicVideoModeTrackingTargetFilter_004 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    mcCameraTrackingController.isSalientDetectionLocked_ = false;
    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->sessionMode = 0;

    CameraStandard::FocusTrackingMetaInfo info;
    std::shared_ptr<TrackingFrameParams> trackingParams = std::make_shared<TrackingFrameParams>();

    int32_t ret = mcCameraTrackingController.CinematicVideoModeTrackingTargetFilter(info, trackingParams);

    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest CinematicVideoModeTrackingTargetFilter_004 end" << std::endl;
}

/**
 * @tc.name  : CinematicVideoModeTrackingTargetFilter_005
 * @tc.number: CinematicVideoModeTrackingTargetFilter_005
 * @tc.desc  : Testing CinematicVideoModeTrackingTargetFilter with isSalientDetectionLocked
 *            true and CINEMATIC_VIDEO mode.
 */
HWTEST_F(McCameraTrackingControllerTest, CinematicVideoModeTrackingTargetFilter_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest CinematicVideoModeTrackingTargetFilter_005 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    mcCameraTrackingController.isSalientDetectionLocked_ = true;
    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->sessionMode = 0;

    CameraStandard::FocusTrackingMetaInfo info;
    std::shared_ptr<TrackingFrameParams> trackingParams = std::make_shared<TrackingFrameParams>();

    int32_t ret = mcCameraTrackingController.CinematicVideoModeTrackingTargetFilter(info, trackingParams);

    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest CinematicVideoModeTrackingTargetFilter_005 end" << std::endl;
}

/**
 * @tc.name  : CinematicVideoModeTrackingTargetFilter_006
 * @tc.number: CinematicVideoModeTrackingTargetFilter_006
 * @tc.desc  : Testing CinematicVideoModeTrackingTargetFilter with isSalientDetectionLocked
 *            true and CINEMATIC_VIDEO mode with locked tracking.
 */
HWTEST_F(McCameraTrackingControllerTest, CinematicVideoModeTrackingTargetFilter_006, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest CinematicVideoModeTrackingTargetFilter_006 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();

    mcCameraTrackingController.isSalientDetectionLocked_ = true;
    mcCameraTrackingController.currentCameraInfo_ = std::make_shared<CameraInfo>();
    mcCameraTrackingController.currentCameraInfo_->sessionMode =
        static_cast<int32_t>(CameraStandard::SceneMode::CINEMATIC_VIDEO);
    mcCameraTrackingController.currentCameraInfo_->pauseFrameCount = 0;

    mcCameraTrackingController.lastTrackingFrame_ = std::make_shared<TrackingFrameParams>();
    mcCameraTrackingController.lastTrackingFrame_->targetId = 100;
    mcCameraTrackingController.lastTrackingFrame_->objectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_BODY);

    std::shared_ptr<TrackingFrameParams> trackingParams = std::make_shared<TrackingFrameParams>();
    trackingParams->targetId = 200;
    trackingParams->objectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE);

    CameraStandard::Rect trackingRect;
    trackingRect.topLeftX = 0.3f;
    trackingRect.topLeftY = 0.3f;
    trackingRect.width = 0.4f;
    trackingRect.height = 0.4f;
    mcCameraTrackingController.trackingRect_ = trackingRect;

    CameraStandard::Rect lastTrackingRect;
    lastTrackingRect.topLeftX = 0.32f;
    lastTrackingRect.topLeftY = 0.32f;
    lastTrackingRect.width = 0.38f;
    lastTrackingRect.height = 0.38f;
    mcCameraTrackingController.lastTrackingRect_ = lastTrackingRect;

    CameraStandard::FocusTrackingMetaInfo info;

    int32_t ret = mcCameraTrackingController.CinematicVideoModeTrackingTargetFilter(info, trackingParams);

    // isSalientDetectionLocked_=true, lastTrackingFrame is BODY, trackingParams is FACE,
    // targetId differs (100!=200), IOU(lastTrackingRect_, trackingRect_)=1.0 > IOU_THRESHOLD
    // → isAllowedConversion=true → returns ERR_OK
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTest CinematicVideoModeTrackingTargetFilter_006 end" << std::endl;
}
 
/**
 * @tc.name  : SensorCallback_003
 * @tc.number: SensorCallback_003
 * @tc.desc  : Testing SensorCallback with rotation change from UP to DOWN.
 */
HWTEST_F(McCameraTrackingControllerTest, SensorCallback_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest SensorCallback_003 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();

    // Set initial rotation
    mcCameraTrackingController.sensorRotation_ = MobileRotation::UP;

    // Create sensor event that triggers rotation change to DOWN
    SensorEvent sensorEvent;
    GravityData gravityData;
    gravityData.x = 0;
    gravityData.y = -1;
    gravityData.z = 0;
    sensorEvent.sensorTypeId = SENSOR_TYPE_ID_GRAVITY;
    sensorEvent.data = reinterpret_cast<uint8_t*>(&gravityData);

    // Trigger sensor callback (rotation changes from UP to DOWN)
    mcCameraTrackingController.SensorCallback(&sensorEvent);
    EXPECT_EQ(mcCameraTrackingController.sensorRotation_, MobileRotation::DOWN);

    DTEST_LOG << "McCameraTrackingControllerTest SensorCallback_003 end" << std::endl;
}

/**
 * @tc.name  : SensorCallback_005
 * @tc.number: SensorCallback_005
 * @tc.desc  : Testing SensorCallback with all rotation transitions.
 */
HWTEST_F(McCameraTrackingControllerTest, SensorCallback_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest SensorCallback_005 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();

    // Test all rotation transitions
    // Formula: degree = 90 - atan2(y, -x) / PI * 180
    std::vector<std::pair<GravityData, MobileRotation>> testCases = {
        {{0, 1, 0}, MobileRotation::UP},      // atan2(1, 0) = 90, degree = 0
        {{-1, 0, 0}, MobileRotation::RIGHT},  // atan2(0, 1) = 0, degree = 90
        {{0, -1, 0}, MobileRotation::DOWN},   // atan2(-1, 0) = -90, degree = 180
        {{1, 0, 0}, MobileRotation::LEFT}     // atan2(0, -1) = 180, degree = -90 -> 270
    };

    for (const auto& testCase : testCases) {
        mcCameraTrackingController.sensorRotation_ = MobileRotation::UP;
        SensorEvent sensorEvent;
        sensorEvent.sensorTypeId = SENSOR_TYPE_ID_GRAVITY;
        sensorEvent.data = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&testCase.first));
        mcCameraTrackingController.SensorCallback(&sensorEvent);
        EXPECT_EQ(mcCameraTrackingController.sensorRotation_, testCase.second);
    }

    DTEST_LOG << "McCameraTrackingControllerTest SensorCallback_005 end" << std::endl;
}
 
/**
 * @tc.name  : IsVertical_001
 * @tc.number: IsVertical_001
 * @tc.desc  : Testing IsVertical function with UP rotation.
 */
HWTEST_F(McCameraTrackingControllerTest, IsVertical_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest IsVertical_001 begin" << std::endl;
 
    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
 
    mcCameraTrackingController.sensorRotation_ = MobileRotation::UP;
    bool result = mcCameraTrackingController.IsVertical(MobileRotation::UP);
    EXPECT_TRUE(result);
 
    DTEST_LOG << "McCameraTrackingControllerTest IsVertical_001 end" << std::endl;
}
 
/**
 * @tc.name  : IsVertical_002
 * @tc.number: IsVertical_002
 * @tc.desc  : Testing IsVertical function with DOWN rotation.
 */
HWTEST_F(McCameraTrackingControllerTest, IsVertical_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest IsVertical_002 begin" << std::endl;
 
    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
 
    mcCameraTrackingController.sensorRotation_ = MobileRotation::DOWN;
    bool result = mcCameraTrackingController.IsVertical(MobileRotation::DOWN);
    EXPECT_TRUE(result);
 
    DTEST_LOG << "McCameraTrackingControllerTest IsVertical_002 end" << std::endl;
}
 
/**
 * @tc.name  : IsVertical_003
 * @tc.number: IsVertical_003
 * @tc.desc  : Testing IsVertical function with LEFT rotation (horizontal).
 */
HWTEST_F(McCameraTrackingControllerTest, IsVertical_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest IsVertical_003 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();

    mcCameraTrackingController.sensorRotation_ = MobileRotation::LEFT;
    bool result = mcCameraTrackingController.IsVertical(MobileRotation::LEFT);
    EXPECT_FALSE(result);

    DTEST_LOG << "McCameraTrackingControllerTest IsVertical_003 end" << std::endl;
}
 
/**
 * @tc.name  : IsVertical_004
 * @tc.number: IsVertical_004
 * @tc.desc  : Testing IsVertical function with RIGHT rotation (horizontal).
 */
HWTEST_F(McCameraTrackingControllerTest, IsVertical_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest IsVertical_004 begin" << std::endl;
 
    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
 
    mcCameraTrackingController.sensorRotation_ = MobileRotation::RIGHT;
    bool result = mcCameraTrackingController.IsVertical(MobileRotation::RIGHT);
    EXPECT_FALSE(result);
 
    DTEST_LOG << "McCameraTrackingControllerTest IsVertical_004 end" << std::endl;
}
 
/**
 * @tc.name  : Init_001
 * @tc.number: Init_001
 * @tc.desc  : Testing Init function - should register listeners and update screen info.
 */
HWTEST_F(McCameraTrackingControllerTest, Init_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest Init_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();

    // Set initial rotation
    mcCameraTrackingController.sensorRotation_ = MobileRotation::UP;

    // 预置守卫变量，阻止Init()中的跨DSO系统调用，避免CFI check失败
    auto guardSnapshot = PreSetInitGuards();

    // Call Init - should register listeners and update screen info
    mcCameraTrackingController.Init();

    // Then: Init()内部调用SubscribeEventUtils::Init()→InitCameraDisabled()，
    // 测试环境下GetDisablePolicy和IsCameraMuted均返回false，故isCameraDisabled_应为false
    EXPECT_EQ(mcCameraTrackingController.isCameraDisabled_.load(), false);

    // Clean up: 先清空守卫变量，避免UnInit()中的UnRegister*跨DSO调用
    ClearInitGuards();
    mcCameraTrackingController.UnInit();
    RestoreInitGuards(guardSnapshot);

    // Then: UnInit后sensorRotation_被重置为INVALID
    EXPECT_EQ(mcCameraTrackingController.sensorRotation_, MobileRotation::INVALID);

    DTEST_LOG << "McCameraTrackingControllerTest Init_001 end" << std::endl;
}
 
/**
 * @tc.name  : Init_002
 * @tc.number: Init_002
 * @tc.desc  : Testing Init function with motion manager - should update screen info to all managers.
 */
HWTEST_F(McCameraTrackingControllerTest, Init_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest Init_002 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();

    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;

    // Set initial rotation
    mcCameraTrackingController.sensorRotation_ = MobileRotation::RIGHT;

    // 预置守卫变量，阻止Init()中的跨DSO系统调用，避免CFI check失败
    auto guardSnapshot = PreSetInitGuards();

    // Call Init - should update screen info to all motion managers
    mcCameraTrackingController.Init();

    // Then: Init后motionManagers_中应包含预置的mechId
    EXPECT_EQ(MechBodyControllerService::GetInstance().motionManagers_.count(mechId), 1u);
    EXPECT_EQ(mcCameraTrackingController.isCameraDisabled_.load(), false);

    // Clean up: 先清空守卫变量，避免UnInit()中的UnRegister*跨DSO调用
    ClearInitGuards();
    mcCameraTrackingController.UnInit();
    RestoreInitGuards(guardSnapshot);

    // Then: UnInit后sensorRotation_被重置为INVALID
    EXPECT_EQ(mcCameraTrackingController.sensorRotation_, MobileRotation::INVALID);
    MechBodyControllerService::GetInstance().motionManagers_.clear();

    DTEST_LOG << "McCameraTrackingControllerTest Init_002 end" << std::endl;
}
 
/**
 * @tc.name  : Init_003
 * @tc.number: Init_003
 * @tc.desc  : Testing Init function with different initial rotations.
 */
HWTEST_F(McCameraTrackingControllerTest, Init_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest Init_003 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();

    // Test with different initial rotations
    std::vector<MobileRotation> rotations = {
        MobileRotation::UP,
        MobileRotation::DOWN,
        MobileRotation::LEFT,
        MobileRotation::RIGHT
    };

    // 预置守卫变量，阻止Init()中的跨DSO系统调用，避免CFI check失败
    auto guardSnapshot = PreSetInitGuards();

    for (const auto& rotation : rotations) {
        mcCameraTrackingController.sensorRotation_ = rotation;
        // 每次Init前确保守卫变量非空（UnInit可能已置空）
        PreSetInitGuards();
        mcCameraTrackingController.Init();
        // Then: 每次Init后isCameraDisabled_应为false（测试环境）
        EXPECT_EQ(mcCameraTrackingController.isCameraDisabled_.load(), false);
        // UnInit前清空守卫变量，避免UnRegister*跨DSO调用
        ClearInitGuards();
        mcCameraTrackingController.UnInit();
        // Then: 每次UnInit后sensorRotation_被重置为INVALID
        EXPECT_EQ(mcCameraTrackingController.sensorRotation_, MobileRotation::INVALID);
    }
    RestoreInitGuards(guardSnapshot);

    DTEST_LOG << "McCameraTrackingControllerTest Init_003 end" << std::endl;
}
 
/**
 * @tc.name  : UnInit_001
 * @tc.number: UnInit_001
 * @tc.desc  : Testing UnInit function - should unregister listeners.
 */
HWTEST_F(McCameraTrackingControllerTest, UnInit_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest UnInit_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();

    // First initialize
    // 预置守卫变量，阻止Init()中的跨DSO系统调用，避免CFI check失败
    auto guardSnapshot = PreSetInitGuards();
    mcCameraTrackingController.Init();

    // 预置非默认状态，验证UnInit后会重置
    mcCameraTrackingController.horizontal_ = 1.5f;
    mcCameraTrackingController.vertical_ = 2.5f;

    // Then uninitialize - 先清空守卫变量，避免UnInit()中的UnRegister*跨DSO调用
    ClearInitGuards();
    mcCameraTrackingController.UnInit();
    RestoreInitGuards(guardSnapshot);

    // Then: 验证UnInit()的副作用——成员变量被重置为初始值
    EXPECT_EQ(mcCameraTrackingController.horizontal_, 0.0f);
    EXPECT_EQ(mcCameraTrackingController.vertical_, 0.0f);

    DTEST_LOG << "McCameraTrackingControllerTest UnInit_001 end" << std::endl;
}

/**
 * @tc.name  : HandleCameraDisabledChangeCallback_True_001
 * @tc.number: HandleCameraDisabledChangeCallback_True_001
 * @tc.desc  : Testing HandleCameraDisabledChangeCallback(true) updates disabled flags and cancels notification.
 */
HWTEST_F(McCameraTrackingControllerTest, HandleCameraDisabledChangeCallback_True_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest HandleCameraDisabledChangeCallback_True_001 begin" << std::endl;

    // Given: 重置控制器和通知的相机禁用标志
    McCameraTrackingController &controller = McCameraTrackingController::GetInstance();
    controller.isCameraDisabled_.store(false);
    NotificationUtils::isCameraDisabled_.store(false);

    // When: 调用 HandleCameraDisabledChangeCallback(true)，应更新两个禁用标志并取消通知
    controller.HandleCameraDisabledChangeCallback(true);

    // Then: 控制器isCameraDisabled_和NotificationUtils::isCameraDisabled_均更新为true
    EXPECT_EQ(controller.isCameraDisabled_.load(), true);
    EXPECT_EQ(NotificationUtils::isCameraDisabled_.load(), true);

    // Cleanup: 恢复状态
    controller.isCameraDisabled_.store(false);
    NotificationUtils::isCameraDisabled_.store(false);

    DTEST_LOG << "McCameraTrackingControllerTest HandleCameraDisabledChangeCallback_True_001 end" << std::endl;
}

/**
 * @tc.name  : HandleCameraDisabledChangeCallback_False_001
 * @tc.number: HandleCameraDisabledChangeCallback_False_001
 * @tc.desc  : Testing HandleCameraDisabledChangeCallback(false) only updates flags without canceling notification.
 */
HWTEST_F(McCameraTrackingControllerTest, HandleCameraDisabledChangeCallback_False_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest HandleCameraDisabledChangeCallback_False_001 begin" << std::endl;

    // Given: 先设置禁用状态为 true，模拟相机被禁用
    McCameraTrackingController &controller = McCameraTrackingController::GetInstance();
    controller.isCameraDisabled_.store(true);
    NotificationUtils::isCameraDisabled_.store(true);

    // When: 调用 HandleCameraDisabledChangeCallback(false)，仅更新状态标志，不取消通知
    controller.HandleCameraDisabledChangeCallback(false);

    // Then: 两个禁用标志均更新为 false
    EXPECT_EQ(controller.isCameraDisabled_.load(), false);
    EXPECT_EQ(NotificationUtils::isCameraDisabled_.load(), false);

    DTEST_LOG << "McCameraTrackingControllerTest HandleCameraDisabledChangeCallback_False_001 end" << std::endl;
}
}
}
