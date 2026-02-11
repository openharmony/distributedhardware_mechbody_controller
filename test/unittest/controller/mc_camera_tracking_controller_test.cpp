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
 * @tc.desc  : Testing OnFocusTracking function.
 */
HWTEST_F(McCameraTrackingControllerTest, OnFocusTracking_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest OnFocusTracking_002 begin" << std::endl;

    McCameraTrackingController &mcCameraTrackingController = McCameraTrackingController::GetInstance();
    CameraStandard::FocusTrackingMetaInfo info;
    mcCameraTrackingController.currentCameraInfo_->currentTrackingEnable = true;

    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.OnFocusTracking(info));
    DTEST_LOG << "McCameraTrackingControllerTest OnFocusTracking_002 end" << std::endl;
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

    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.BuildTrackingParams(info));
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

    EXPECT_EQ(result, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTest BuildTrackingParams_005 end" << std::endl;
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

    EXPECT_NE(result, nullptr);

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

    EXPECT_NE(result, nullptr);

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

    EXPECT_NE(result, nullptr);

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
    EXPECT_NE(targetObject, nullptr);

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

    EXPECT_GT(iou, 0.0f);
    EXPECT_LT(iou, 1.0f);

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

    EXPECT_TRUE(ret == ERR_OK || ret == INVALID_TRACKING_TARGET);

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

    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.CinematicVideoModeTrackingTargetFilter(info, trackingParams));

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

    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.UpdateROI(trackingFrameParamsNull, rect));
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.UpdateROI(trackingFrameParams, rect));

    rect.width = 4;
    rect.topLeftX = 2;
    rect.height = 4;
    rect.topLeftY = 2;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.UpdateROI(trackingFrameParams, rect));
    mcCameraTrackingController.sensorRotation_ = MobileRotation::LEFT;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.UpdateROI(trackingFrameParams, rect));
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

    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.UpdateROI(trackingFrameParams, rect));
    mcCameraTrackingController.sensorRotation_ = MobileRotation::LEFT;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.UpdateROI(trackingFrameParams, rect));
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

    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.UpdateROI(trackingFrameParams, rect));
    mcCameraTrackingController.sensorRotation_ = MobileRotation::DOWN;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.UpdateROI(trackingFrameParams, rect));
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

    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.UpdateROI(trackingFrameParams, rect));
    mcCameraTrackingController.sensorRotation_ = MobileRotation::DOWN;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.UpdateROI(trackingFrameParams, rect));
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
 * @tc.name  : SearchTargetRotateFinish_001
 * @tc.number: SearchTargetRotateFinish_001
 * @tc.desc  : Testing SearchTargetRotateFinish function.
 */
HWTEST_F(McCameraTrackingControllerTest, SearchTargetRotateFinish_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest SearchTargetRotateFinish_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    std::string cmdId = "mockNapiCmdId";

    EXPECT_NO_FATAL_FAILURE(
            mcCameraTrackingController.SearchTargetRotateFinish(cmdId));

    cmdId = "searchTargetTaskName";

    EXPECT_NO_FATAL_FAILURE(
            mcCameraTrackingController.SearchTargetRotateFinish(cmdId));

    DTEST_LOG << "McCameraTrackingControllerTest SearchTargetRotateFinish_001 end" << std::endl;
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

    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.SearchTargetStop());

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

    EXPECT_NO_FATAL_FAILURE(
        mcCameraTrackingController.ExecSearchTask(cmdId, tokenId, startFromNeg, searchTimes));

    DTEST_LOG << "McCameraTrackingControllerTest ExecSearchTask_001 end" << std::endl;
}

/**
 * @tc.name  : ExecSearchTaskWithLimit_001
 * @tc.number: ExecSearchTaskWithLimit_001
 * @tc.desc  : Testing ExecSearchTaskWithLimit function.
 */
HWTEST_F(McCameraTrackingControllerTest, ExecSearchTaskWithLimit_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest ExecSearchTaskWithLimit_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    std::string cmdId = "mockNapiCmdId";
    int32_t mechId = 100;
    int32_t mechId1 = 101;
    uint32_t tokenId = 1;
    std::shared_ptr<SearchParams> searchParams = std::make_shared<SearchParams>();
    bool startFromNeg = true;
    RotateDegreeLimit limit;

    std::shared_ptr<MotionManager> motionMgr =
            std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;
    MechBodyControllerService::GetInstance().motionManagers_[mechId1] = nullptr;

    EXPECT_NO_FATAL_FAILURE(
            mcCameraTrackingController.ExecSearchTaskWithLimit(cmdId, tokenId, startFromNeg, limit));

    DTEST_LOG << "McCameraTrackingControllerTest ExecSearchTaskWithLimit_001 end" << std::endl;
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

    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.GetCurrentCameraInfo());
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

    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.SensorCallback(event));
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.SensorCallback(eventNull));
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

    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.CalculateSensorRotation(data));

    gravityData.z = 5;
    GravityData* dataInvaild = &gravityData;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.CalculateSensorRotation(dataInvaild));
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
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType));

    cameraObjectType = CameraStandard::MetadataObjectType::FACE;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType));

    cameraObjectType = CameraStandard::MetadataObjectType::HUMAN_BODY;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType));

    cameraObjectType = CameraStandard::MetadataObjectType::CAT_FACE;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType));

    cameraObjectType = CameraStandard::MetadataObjectType::CAT_BODY;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType));

    cameraObjectType = CameraStandard::MetadataObjectType::DOG_FACE;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType));

    cameraObjectType = CameraStandard::MetadataObjectType::DOG_BODY;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType));

    cameraObjectType = CameraStandard::MetadataObjectType::SALIENT_DETECTION;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType));

    cameraObjectType = CameraStandard::MetadataObjectType::BAR_CODE_DETECTION;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType));

    cameraObjectType = CameraStandard::MetadataObjectType::BASE_FACE_DETECTION;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.ConvertObjectType(cameraObjectType, mechObjectType));
    DTEST_LOG << "McCameraTrackingControllerTest ConvertObjectType_001 end" << std::endl;
}

/**
 * @tc.name  : OnMechConnect_001
 * @tc.number: OnMechConnect_001
 * @tc.desc  : Testing OnMechConnect function.
 */
HWTEST_F(McCameraTrackingControllerTest, OnMechConnect_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest OnMechConnect_001 begin" << std::endl;

    auto listener = std::make_shared<MechConnectListenerImpl>();
    MechInfo mechInfo;
    EXPECT_NO_FATAL_FAILURE(listener->OnMechConnect(mechInfo));

    DTEST_LOG << "McCameraTrackingControllerTest OnMechConnect_001 end" << std::endl;
}

/**
 * @tc.name  : OnMechDisconnect_001
 * @tc.number: OnMechDisconnect_001
 * @tc.desc  : Testing OnMechDisconnect function.
 */
HWTEST_F(McCameraTrackingControllerTest, OnMechDisconnect_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest OnMechDisconnect_001 begin" << std::endl;

    auto listener = std::make_shared<MechConnectListenerImpl>();
    MechInfo mechInfo;
    EXPECT_NO_FATAL_FAILURE(listener->OnMechDisconnect(mechInfo));

    DTEST_LOG << "McCameraTrackingControllerTest OnMechDisconnect_001 end" << std::endl;
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
    mcCameraTrackingController.user.callback = nullptr;
    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.UnRegisterSensorListener());

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
    CameraStandard::Rect rect;
    CameraType cameraType = CameraType::FRONT;
    MobileRotation sensorRotation = MobileRotation::UP;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustROI(roi, rect, cameraType, sensorRotation));

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
    CameraStandard::Rect rect;
    CameraType cameraType = CameraType::FRONT;
    MobileRotation sensorRotation = MobileRotation::LEFT;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustROI(roi, rect, cameraType, sensorRotation));

    DTEST_LOG << "McCameraTrackingControllerTest AdjustROI_FrontCamera_LEFT_001 end" << std::endl;
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
    float x = 0.0f;
    float y = 0.0f;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustYOffset(roi, cameraType, x, y, isFace));

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
    float x = 0.0f;
    float y = 0.5f;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustYOffset(roi, cameraType, x, y, isFace));

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
    float x = 0.0f;
    float y = -0.5f;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustYOffset(roi, cameraType, x, y, isFace));

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
    float x = 0.0f;
    float y = -0.5f;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustYOffset(roi, cameraType, x, y, isFace));

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
    float x = 0.0f;
    float y = 0.5f;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustYOffset(roi, cameraType, x, y, isFace));

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
    float x = -0.5f;
    float y = 0.0f;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustYOffset(roi, cameraType, x, y, isFace));

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
    float x = 0.5f;
    float y = 0.0f;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustYOffset(roi, cameraType, x, y, isFace));

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
    float x = 0.0f;
    float y = 0.0f;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustXOffset(roi, cameraType, x, y, isFace));

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
    float x = 0.5f;
    float y = 0.0f;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustXOffset(roi, cameraType, x, y, isFace));
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
    float x = -0.5f;
    float y = 0.0f;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustXOffset(roi, cameraType, x, y, isFace));

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
    float x = -0.5f;
    float y = 0.0f;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustXOffset(roi, cameraType, x, y, isFace));

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
    float x = 0.5f;
    float y = 0.0f;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustXOffset(roi, cameraType, x, y, isFace));

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

    float x = 0.0f;
    float y = -0.5f;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustXOffset(roi, cameraType, x, y, isFace));

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
    float x = 0.0f;
    float y = 0.5f;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustXOffset(roi, cameraType, x, y, isFace));

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

    EXPECT_NO_FATAL_FAILURE(controller.SetStickOffset(x, y));

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

    EXPECT_NO_FATAL_FAILURE(controller.SetStickOffset(x, y));

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

    EXPECT_NO_FATAL_FAILURE(controller.SetStickOffset(x, y));

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

    EXPECT_NO_FATAL_FAILURE(controller.SetStickOffset(x, y));

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

    EXPECT_NO_FATAL_FAILURE(controller.SetStickOffset(x, y));

    DTEST_LOG << "McCameraTrackingControllerTest SetStickOffset_005 end" << std::endl;
}
}
}