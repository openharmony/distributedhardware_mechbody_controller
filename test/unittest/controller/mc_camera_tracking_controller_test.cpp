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
 * @tc.name  : OnFocusTracking_001
 * @tc.number: OnFocusTracking_001
 * @tc.desc  : Testing OnFocusTracking function.
 */
HWTEST_F(McCameraTrackingControllerTest, OnFocusTracking_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest OnFocusTracking_001 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    CameraStandard::FocusTrackingMetaInfo info;
    mcCameraTrackingController.currentCameraInfo_->currentTrackingEnable = false;

    int32_t ret = mcCameraTrackingController.OnFocusTracking(info);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "McCameraTrackingControllerTest OnFocusTracking_001 end" << std::endl;
}

/**
 * @tc.name  : OnFocusTracking_002
 * @tc.number: OnFocusTracking_002
 * @tc.desc  : Testing OnFocusTracking function.
 */
HWTEST_F(McCameraTrackingControllerTest, OnFocusTracking_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest OnFocusTracking_002 begin" << std::endl;

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
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

    McCameraTrackingController& mcCameraTrackingController = McCameraTrackingController::GetInstance();
    CameraStandard::FocusTrackingMetaInfo info;

    EXPECT_NO_FATAL_FAILURE(mcCameraTrackingController.BuildTrackingParams(info));
    DTEST_LOG << "McCameraTrackingControllerTest BuildTrackingParams_001 end" << std::endl;
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
 * @tc.name  : AdjustYOffset_Front_Right_001
 * @tc.number: AdjustYOffset_Front_Right_001
 * @tc.desc  : Testing AdjustYOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustYOffset_Front_Right_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Front_Right_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.0f, 0.0f, 0.0f, 0.0f};
    CameraType cameraType = CameraType::FRONT;
    CameraTrackingLayout trackingLayout = CameraTrackingLayout::RIGHT;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustYOffset(roi, cameraType, trackingLayout));

    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Front_Right_001 end" << std::endl;
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
    ROI roi = {0.0f, 0.0f, 0.0f, 0.0f};
    CameraType cameraType = CameraType::FRONT;
    CameraTrackingLayout trackingLayout = CameraTrackingLayout::LEFT;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustYOffset(roi, cameraType, trackingLayout));

    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Front_Left_001 end" << std::endl;
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
    ROI roi = {0.0f, 0.0f, 1.0f, 1.0f};
    CameraType cameraType = CameraType::BACK;
    CameraTrackingLayout trackingLayout = CameraTrackingLayout::RIGHT;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustYOffset(roi, cameraType, trackingLayout));

    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Back_Right_001 end" << std::endl;
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
    ROI roi = {0.0f, 0.0f, 0.0f, 0.0f};
    CameraType cameraType = CameraType::BACK;
    CameraTrackingLayout trackingLayout = CameraTrackingLayout::LEFT;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustYOffset(roi, cameraType, trackingLayout));

    DTEST_LOG << "McCameraTrackingControllerTest AdjustYOffset_Back_Left_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_001
 * @tc.number: AdjustXOffset_001
 * @tc.desc  : Testing AdjustXOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustXOffset_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.0f, 0.0f, 0.0f, 0.0f};
    CameraType cameraType = CameraType::FRONT;
    CameraTrackingLayout trackingLayout = CameraTrackingLayout::RIGHT;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustXOffset(roi, cameraType, trackingLayout));

    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_002
 * @tc.number: AdjustXOffset_002
 * @tc.desc  : Testing AdjustXOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustXOffset_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.0f, 0.0f, 0.0f, 0.0f};
    CameraType cameraType = CameraType::FRONT;
    CameraTrackingLayout trackingLayout = CameraTrackingLayout::LEFT;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustXOffset(roi, cameraType, trackingLayout));

    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_002 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_003
 * @tc.number: AdjustXOffset_003
 * @tc.desc  : Testing AdjustXOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustXOffset_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.0f, 0.0f, 0.0f, 0.0f};
    CameraType cameraType = CameraType::FRONT;
    CameraTrackingLayout trackingLayout = CameraTrackingLayout::DEFAULT;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustXOffset(roi, cameraType, trackingLayout));

    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_003 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_004
 * @tc.number: AdjustXOffset_004
 * @tc.desc  : Testing AdjustXOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustXOffset_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.0f, 0.0f, 0.0f, 0.0f};
    CameraType cameraType = CameraType::BACK;
    CameraTrackingLayout trackingLayout = CameraTrackingLayout::RIGHT;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustXOffset(roi, cameraType, trackingLayout));

    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_004 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_005
 * @tc.number: AdjustXOffset_005
 * @tc.desc  : Testing AdjustXOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustXOffset_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_005 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.0f, 0.0f, 0.0f, 0.0f};
    CameraType cameraType = CameraType::BACK;
    CameraTrackingLayout trackingLayout = CameraTrackingLayout::LEFT;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustXOffset(roi, cameraType, trackingLayout));

    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_005 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_006
 * @tc.number: AdjustXOffset_006
 * @tc.desc  : Testing AdjustXOffset function.
 */
HWTEST_F(McCameraTrackingControllerTest, AdjustXOffset_006, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_006 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.0f, 0.0f, 0.0f, 0.0f};
    CameraType cameraType = CameraType::BACK;
    CameraTrackingLayout trackingLayout = CameraTrackingLayout::DEFAULT;

    EXPECT_NO_FATAL_FAILURE(controller.AdjustXOffset(roi, cameraType, trackingLayout));

    DTEST_LOG << "McCameraTrackingControllerTest AdjustXOffset_006 end" << std::endl;
}
}
}