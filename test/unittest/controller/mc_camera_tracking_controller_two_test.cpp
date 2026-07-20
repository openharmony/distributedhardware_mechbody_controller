/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "mc_camera_tracking_controller_two_test.h"
#include "bluetooth_errorcode.h"
#include "c_header/ohos_bt_gatt.h"
#include "../mechbody_controller_log.h"
#include "../test_log.h"
#include "mechbody_controller_service.h"
#include "mc_connect_manager.h"
#include "camera_metadata_info.h"
#include <chrono>
#include <thread>
#include <atomic>

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS {
namespace MechBodyController {

static AttachmentStateMap g_mockMechState = AttachmentStateMap::DETACHED;

AttachmentStateMap MechConnectManager::GetMechState(int32_t mechId)
{
    return g_mockMechState;
}

void McCameraTrackingControllerTwoTest::SetUpTestCase()
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest::SetUpTestCase" << std::endl;
}

void McCameraTrackingControllerTwoTest::TearDownTestCase()
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest::TearDownTestCase" << std::endl;
}

void McCameraTrackingControllerTwoTest::TearDown()
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest::TearDown" << std::endl;
}

void McCameraTrackingControllerTwoTest::SetUp()
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest::SetUp" << std::endl;
}
/**
 * @tc.name  : OnMetadataInfo_002
 * @tc.number: OnMetadataInfo_002
 * @tc.desc  : Testing OnMetadataInfo with failed FOV retrieval.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, OnMetadataInfo_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest OnMetadataInfo_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::shared_ptr<OHOS::Camera::CameraMetadata> metadata = nullptr;

    int32_t ret = controller.OnMetadataInfo(metadata);
    EXPECT_EQ(ret, METADATA_INFO_IS_EMPTY);

    DTEST_LOG << "McCameraTrackingControllerTwoTest OnMetadataInfo_002 end" << std::endl;
}

/**
 * @tc.name  : OnCaptureSessionConfiged_001
 * @tc.number: OnCaptureSessionConfiged_001
 * @tc.desc  : Testing OnCaptureSessionConfiged with valid tokenId.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, OnCaptureSessionConfiged_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest OnCaptureSessionConfiged_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 1001;

    CameraStandard::CaptureSessionInfo sessionInfo;
    sessionInfo.position = 1;

    int32_t ret = controller.OnCaptureSessionConfiged(sessionInfo);
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest OnCaptureSessionConfiged_001 end" << std::endl;
}

/**
 * @tc.name  : OnCaptureSessionConfiged_002
 * @tc.number: OnCaptureSessionConfiged_002
 * @tc.desc  : Testing OnCaptureSessionConfiged with app settings exists.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, OnCaptureSessionConfiged_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest OnCaptureSessionConfiged_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 1001;

    std::shared_ptr<AppSetting> setting = std::make_shared<AppSetting>();
    setting->isTrackingEnabled = false;
    controller.appSettings[1001] = setting;

    CameraStandard::CaptureSessionInfo sessionInfo;
    sessionInfo.position = 1;

    int32_t ret = controller.OnCaptureSessionConfiged(sessionInfo);
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest OnCaptureSessionConfiged_002 end" << std::endl;
}

/**
 * @tc.name  : OnZoomInfoChange_001
 * @tc.number: OnZoomInfoChange_001
 * @tc.desc  : Testing OnZoomInfoChange when currentCameraInfo_ is nullptr.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, OnZoomInfoChange_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest OnZoomInfoChange_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = nullptr;

    CameraStandard::ZoomInfo zoomInfo;
    zoomInfo.focusMode = 0;
    int32_t sessionId = 0;

    int32_t result = controller.OnZoomInfoChange(sessionId, zoomInfo);
    EXPECT_EQ(result, CAMERA_INFO_IS_EMPTY);

    DTEST_LOG << "McCameraTrackingControllerTwoTest OnZoomInfoChange_001 end" << std::endl;
}

/**
 * @tc.name  : OnZoomInfoChange_002
 * @tc.number: OnZoomInfoChange_002
 * @tc.desc  : Testing OnZoomInfoChange with valid focusMode.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, OnZoomInfoChange_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest OnZoomInfoChange_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 1001;
    controller.currentCameraInfo_->fovH = 90;
    controller.currentCameraInfo_->fovV = 60;

    CameraStandard::ZoomInfo zoomInfo;
    zoomInfo.focusMode = 1;
    int32_t sessionId = 0;

    int32_t result = controller.OnZoomInfoChange(sessionId, zoomInfo);
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest OnZoomInfoChange_002 end" << std::endl;
}

/**
 * @tc.name  : ComputeFov_001
 * @tc.number: ComputeFov_001
 * @tc.desc  : Testing ComputeFov when currentCameraInfo_ is nullptr.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, ComputeFov_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest ComputeFov_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = nullptr;

    int32_t result = controller.ComputeFov();
    EXPECT_EQ(result, CAMERA_INFO_IS_EMPTY);

    DTEST_LOG << "McCameraTrackingControllerTwoTest ComputeFov_001 end" << std::endl;
}

/**
 * @tc.name  : ComputeFov_002
 * @tc.number: ComputeFov_002
 * @tc.desc  : Testing ComputeFov with UP sensor rotation and fovFromMetadata_ = false.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, ComputeFov_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest ComputeFov_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 1001;
    controller.currentCameraInfo_->fovH = 0;
    controller.currentCameraInfo_->fovV = 0;
    controller.fovFromMetadata_ = false;

    controller.sensorRotation_ = MobileRotation::UP;

    int32_t result = controller.ComputeFov();
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest ComputeFov_002 end" << std::endl;
}

/**
 * @tc.name  : ComputeFov_003
 * @tc.number: ComputeFov_003
 * @tc.desc  : Testing ComputeFov with RIGHT sensor rotation and fovFromMetadata_ = false.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, ComputeFov_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest ComputeFov_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 1001;
    controller.currentCameraInfo_->fovH = 0;
    controller.currentCameraInfo_->fovV = 0;
    controller.fovFromMetadata_ = false;

    controller.sensorRotation_ = MobileRotation::RIGHT;

    int32_t result = controller.ComputeFov();
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest ComputeFov_003 end" << std::endl;
}

/**
 * @tc.name  : ComputeFov_005
 * @tc.number: ComputeFov_005
 * @tc.desc  : Testing ComputeFov with fovFromMetadata_ = true (FOV from metadata, should not recompute).
 */
HWTEST_F(McCameraTrackingControllerTwoTest, ComputeFov_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest ComputeFov_005 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 1001;
    controller.currentCameraInfo_->fovH = 90;
    controller.currentCameraInfo_->fovV = 60;
    controller.fovFromMetadata_ = true;

    controller.sensorRotation_ = MobileRotation::UP;

    int32_t result = controller.ComputeFov();
    EXPECT_EQ(result, ERR_OK);
    // When fovFromMetadata_ is true, FOV values should remain unchanged
    EXPECT_EQ(controller.currentCameraInfo_->fovH, 90);
    EXPECT_EQ(controller.currentCameraInfo_->fovV, 60);

    DTEST_LOG << "McCameraTrackingControllerTwoTest ComputeFov_005 end" << std::endl;
}

/**
 * @tc.name  : SensorCutForVideoMode_001
 * @tc.number: SensorCutForVideoMode_001
 * @tc.desc  : Testing SensorCutForVideoMode with sensorRatio > viewRatio.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SensorCutForVideoMode_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SensorCutForVideoMode_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->sensorWidth = 4000.0f;
    controller.currentCameraInfo_->sensorHeight = 2250.0f;
    controller.currentCameraInfo_->width = 1920;
    controller.currentCameraInfo_->height = 1080;
    controller.currentCameraInfo_->sessionMode = 1;

    float sensorWidthCut = 0.0f;
    float sensorHighCut = 0.0f;

    controller.SensorCutForVideoMode(sensorWidthCut, sensorHighCut);
    EXPECT_FLOAT_EQ(sensorHighCut, 2250.0f);
    EXPECT_FLOAT_EQ(sensorWidthCut, 4000.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SensorCutForVideoMode_001 end" << std::endl;
}

/**
 * @tc.name  : SensorCutForVideoMode_002
 * @tc.number: SensorCutForVideoMode_002
 * @tc.desc  : Testing SensorCutForVideoMode with sensorRatio <= viewRatio.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SensorCutForVideoMode_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SensorCutForVideoMode_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->sensorWidth = 1920.0f;
    controller.currentCameraInfo_->sensorHeight = 1080.0f;
    controller.currentCameraInfo_->width = 3840;
    controller.currentCameraInfo_->height = 2160;
    controller.currentCameraInfo_->sessionMode = 1;

    float sensorWidthCut = 0.0f;
    float sensorHighCut = 0.0f;

    controller.SensorCutForVideoMode(sensorWidthCut, sensorHighCut);
    EXPECT_FLOAT_EQ(sensorWidthCut, 1920.0f);
    EXPECT_FLOAT_EQ(sensorHighCut, 1080.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SensorCutForVideoMode_002 end" << std::endl;
}

/**
 * @tc.name  : SensorCutForVideoMode_003
 * @tc.number: SensorCutForVideoMode_003
 * @tc.desc  : Testing SensorCutForVideoMode with VIDEO mode and AUTO stabilization.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SensorCutForVideoMode_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SensorCutForVideoMode_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->sensorWidth = 4000.0f;
    controller.currentCameraInfo_->sensorHeight = 2250.0f;
    controller.currentCameraInfo_->width = 1920;
    controller.currentCameraInfo_->height = 1080;
    controller.currentCameraInfo_->sessionMode = static_cast<int32_t>(CameraStandard::SceneMode::VIDEO);
    controller.currentCameraInfo_->videoStabilizationMode =
        static_cast<int32_t>(CameraVideoStabilizationMode::OHOS_CAMERA_VIDEO_STABILIZATION_AUTO);

    float sensorWidthCut = 0.0f;
    float sensorHighCut = 0.0f;

    controller.SensorCutForVideoMode(sensorWidthCut, sensorHighCut);
    EXPECT_FLOAT_EQ(sensorHighCut, 1800.0f);
    EXPECT_FLOAT_EQ(sensorWidthCut, 3200.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SensorCutForVideoMode_003 end" << std::endl;
}

/**
 * @tc.name  : SensorCutForVideoMode_004
 * @tc.number: SensorCutForVideoMode_004
 * @tc.desc  : Testing SensorCutForVideoMode with VIDEO mode and HIGH stabilization.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SensorCutForVideoMode_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SensorCutForVideoMode_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->sensorWidth = 4000.0f;
    controller.currentCameraInfo_->sensorHeight = 2250.0f;
    controller.currentCameraInfo_->width = 1920;
    controller.currentCameraInfo_->height = 1080;
    controller.currentCameraInfo_->sessionMode = static_cast<int32_t>(CameraStandard::SceneMode::VIDEO);
    controller.currentCameraInfo_->videoStabilizationMode =
        static_cast<int32_t>(CameraVideoStabilizationMode::OHOS_CAMERA_VIDEO_STABILIZATION_HIGH);

    float sensorWidthCut = 0.0f;
    float sensorHighCut = 0.0f;

    controller.SensorCutForVideoMode(sensorWidthCut, sensorHighCut);
    EXPECT_FLOAT_EQ(sensorHighCut, 1350.0f);
    EXPECT_FLOAT_EQ(sensorWidthCut, 2400.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SensorCutForVideoMode_004 end" << std::endl;
}

/**
 * @tc.name  : OnSessionStatusChange_001
 * @tc.number: OnSessionStatusChange_001
 * @tc.desc  : Testing OnSessionStatusChange when currentCameraInfo_ is nullptr.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, OnSessionStatusChange_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest OnSessionStatusChange_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = nullptr;
    int32_t sessionId = 0;

    EXPECT_EQ(controller.OnSessionStatusChange(sessionId, false), CAMERA_INFO_IS_EMPTY);

    DTEST_LOG << "McCameraTrackingControllerTwoTest OnSessionStatusChange_001 end" << std::endl;
}

/**
 * @tc.name  : IsCurrentFocus_001
 * @tc.number: IsCurrentFocus_001
 * @tc.desc  : Testing IsCurrentFocus with focus mode in whitelist.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, IsCurrentFocus_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest IsCurrentFocus_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->focusMode = OHOS_CAMERA_FOCUS_MODE_CONTINUOUS_AUTO;

    bool result = controller.IsCurrentFocus();
    EXPECT_FALSE(result);

    DTEST_LOG << "McCameraTrackingControllerTwoTest IsCurrentFocus_001 end" << std::endl;
}

/**
 * @tc.name  : IsCurrentFocus_002
 * @tc.number: IsCurrentFocus_002
 * @tc.desc  : Testing IsCurrentFocus with focus mode not in whitelist.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, IsCurrentFocus_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest IsCurrentFocus_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->focusMode = OHOS_CAMERA_FOCUS_MODE_LOCKED;

    bool result = controller.IsCurrentFocus();
    EXPECT_TRUE(result);

    DTEST_LOG << "McCameraTrackingControllerTwoTest IsCurrentFocus_002 end" << std::endl;
}

/**
 * @tc.name  : GetTrackingEnabled_002
 * @tc.number: GetTrackingEnabled_002
 * @tc.desc  : Testing GetTrackingEnabled with existing app settings.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, GetTrackingEnabled_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest GetTrackingEnabled_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    bool isEnabled = false;

    int32_t result = controller.GetTrackingEnabled(11, isEnabled);
    EXPECT_EQ(isEnabled, true);
    EXPECT_EQ(result, ERR_OK);
    std::shared_ptr<AppSetting> setting = std::make_shared<AppSetting>();
    setting->isTrackingEnabled = false;
    controller.appSettings[11] = setting;
    result = controller.GetTrackingEnabled(11, isEnabled);
    EXPECT_EQ(isEnabled, false);
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest GetTrackingEnabled_002 end" << std::endl;
}

/**
 * @tc.name  : GetTrackingLayout_001
 * @tc.number: GetTrackingLayout_001
 * @tc.desc  : Testing GetTrackingLayout when horizontal exceeds max.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, GetTrackingLayout_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest GetTrackingLayout_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.horizontal_ = 1.0f;

    CameraTrackingLayout cameraTrackingLayout = CameraTrackingLayout::RIGHT;
    int32_t result = controller.GetTrackingLayout(cameraTrackingLayout);
    EXPECT_EQ(controller.currentCameraInfo_->currentCameraTrackingLayout, CameraTrackingLayout::RIGHT);
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest GetTrackingLayout_001 end" << std::endl;
}

/**
 * @tc.name  : GetTrackingLayout_002
 * @tc.number: GetTrackingLayout_002
 * @tc.desc  : Testing GetTrackingLayout when horizontal below min.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, GetTrackingLayout_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest GetTrackingLayout_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.horizontal_ = -0.2f;
    CameraTrackingLayout cameraTrackingLayout = CameraTrackingLayout::LEFT;
    int32_t result = controller.GetTrackingLayout(cameraTrackingLayout);
    EXPECT_EQ(controller.currentCameraInfo_->currentCameraTrackingLayout, CameraTrackingLayout::LEFT);
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest GetTrackingLayout_002 end" << std::endl;
}

/**
 * @tc.name  : GetTrackingLayout_003
 * @tc.number: GetTrackingLayout_003
 * @tc.desc  : Testing GetTrackingLayout when horizontal is in middle range.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, GetTrackingLayout_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest GetTrackingLayout_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.horizontal_ = 0.0f;
    CameraTrackingLayout cameraTrackingLayout = CameraTrackingLayout::MIDDLE;
    int32_t result = controller.GetTrackingLayout(cameraTrackingLayout);
    EXPECT_EQ(controller.currentCameraInfo_->currentCameraTrackingLayout, CameraTrackingLayout::MIDDLE);
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest GetTrackingLayout_003 end" << std::endl;
}

/**
 * @tc.name  : setTrackingLimit_001
 * @tc.number: setTrackingLimit_001
 * @tc.desc  : Testing setTrackingLimit with value > 1.0.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, setTrackingLimit_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest setTrackingLimit_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
        ROI roi = {
        .x = 1.5f,
        .y = 1.5f,
    };
    controller.setTrackingLimit(roi);
    EXPECT_EQ(roi.x, 1.0f);
    EXPECT_EQ(roi.y, 1.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest setTrackingLimit_001 end" << std::endl;
}

/**
 * @tc.name  : setTrackingLimit_002
 * @tc.number: setTrackingLimit_002
 * @tc.desc  : Testing setTrackingLimit with value < 0.0.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, setTrackingLimit_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest setTrackingLimit_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {
        .x = -0.5f,
        .y = -0.5f,
    };
    controller.setTrackingLimit(roi);
    EXPECT_EQ(roi.x, 0.0f);
    EXPECT_EQ(roi.y, 0.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest setTrackingLimit_002 end" << std::endl;
}

/**
 * @tc.name  : setTrackingLimit_003
 * @tc.number: setTrackingLimit_003
 * @tc.desc  : Testing setTrackingLimit with valid value.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, setTrackingLimit_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest setTrackingLimit_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {
        .x = 0.5f,
        .y = 0.5f,
    };
    controller.setTrackingLimit(roi);
    EXPECT_EQ(roi.x, 0.5f);
    EXPECT_EQ(roi.y, 0.5f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest setTrackingLimit_003 end" << std::endl;
}

/**
 * @tc.name  : OnConnectChange_001
 * @tc.number: OnConnectChange_001
 * @tc.desc  : Testing OnConnectChange when currentCameraInfo_ is nullptr.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, OnConnectChange_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest OnConnectChange_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = nullptr;

    controller.OnConnectChange();
    EXPECT_EQ(controller.currentCameraInfo_, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTwoTest OnConnectChange_001 end" << std::endl;
}

/**
 * @tc.name  : OnConnectChange_002
 * @tc.number: OnConnectChange_002
 * @tc.desc  : Testing OnConnectChange when currentCameraInfo_ exists.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, OnConnectChange_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest OnConnectChange_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();

    controller.OnConnectChange();
    EXPECT_EQ(controller.currentCameraInfo_->currentCameraTrackingLayout, CameraTrackingLayout::DEFAULT);

    DTEST_LOG << "McCameraTrackingControllerTwoTest OnConnectChange_002 end" << std::endl;
}

/**
 * @tc.name  : OnCaptureSessionConfiged_003
 * @tc.number: OnCaptureSessionConfiged_003
 * @tc.desc  : Testing OnCaptureSessionConfiged with empty appSettings.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, OnCaptureSessionConfiged_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest OnCaptureSessionConfiged_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    uint32_t tokenId = 9999;
    controller.appSettings.clear();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->currentTrackingEnable = false;

    CameraStandard::CaptureSessionInfo sessionInfo;
    sessionInfo.callerTokenId = tokenId;
    sessionInfo.position = 1;

    int32_t ret = controller.OnCaptureSessionConfiged(sessionInfo);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(controller.currentCameraInfo_->currentTrackingEnable);

    DTEST_LOG << "McCameraTrackingControllerTwoTest OnCaptureSessionConfiged_003 end" << std::endl;
}

/**
 * @tc.name  : OnCaptureSessionConfiged_004
 * @tc.number: OnCaptureSessionConfiged_004
 * @tc.desc  : Testing OnCaptureSessionConfiged with tracking enabled.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, OnCaptureSessionConfiged_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest OnCaptureSessionConfiged_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    uint32_t tokenId = 9998;
    std::shared_ptr<AppSetting> setting = std::make_shared<AppSetting>();
    setting->isTrackingEnabled = true;
    controller.appSettings[tokenId] = setting;
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = tokenId;

    CameraStandard::CaptureSessionInfo sessionInfo;
    sessionInfo.callerTokenId = tokenId;
    sessionInfo.position = 1;

    int32_t ret = controller.OnCaptureSessionConfiged(sessionInfo);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(controller.currentCameraInfo_->currentTrackingEnable);

    DTEST_LOG << "McCameraTrackingControllerTwoTest OnCaptureSessionConfiged_004 end" << std::endl;
}

/**
 * @tc.name  : OnSessionStatusChange_003
 * @tc.number: OnSessionStatusChange_003
 * @tc.desc  : Testing OnSessionStatusChange with true status.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, OnSessionStatusChange_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest OnSessionStatusChange_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();

    int32_t ret = controller.OnSessionStatusChange(1, true);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(controller.currentCameraInfo_->isCameraOn);

    DTEST_LOG << "McCameraTrackingControllerTwoTest OnSessionStatusChange_003 end" << std::endl;
}

/**
 * @tc.name  : UpdateMotionManagers_002
 * @tc.number: UpdateMotionManagers_002
 * @tc.desc  : Testing UpdateMotionManagers with null eventHandler.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, UpdateMotionManagers_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateMotionManagers_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    MechBodyControllerService::GetInstance().motionManagers_.clear();
    std::shared_ptr<AppExecFwk::EventHandler> originalEventHandler = controller.eventHandler_;
    controller.eventHandler_ = nullptr;

    int32_t ret = controller.UpdateMotionManagers();
    EXPECT_EQ(ret, ERR_OK);

    controller.eventHandler_ = originalEventHandler;

    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateMotionManagers_002 end" << std::endl;
}

/**
 * @tc.name  : UpdateMotionManagers_003
 * @tc.number: UpdateMotionManagers_003
 * @tc.desc  : Testing UpdateMotionManagers with valid eventHandler.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, UpdateMotionManagers_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateMotionManagers_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->fovHBasic = 80;
    controller.currentCameraInfo_->fovVBasic = 60;
    controller.currentCameraInfo_->zoomFactor = 1.0f;
    controller.currentCameraInfo_->isRecording = false;
    controller.currentCameraInfo_->cameraType = CameraType::BACK;
    controller.sensorRotation_ = MobileRotation::UP;

    int32_t ret = controller.UpdateMotionManagers();
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateMotionManagers_003 end" << std::endl;
}

// /**
//  * @tc.name  : UpdateMotionManagers_004
//  * @tc.number: UpdateMotionManagers_004
//  * @tc.desc  : Testing UpdateMotionManagers with null MotionManager.
//  */
HWTEST_F(McCameraTrackingControllerTwoTest, UpdateMotionManagers_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateMotionManagers_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->fovHBasic = 80;
    controller.currentCameraInfo_->fovVBasic = 60;
    controller.currentCameraInfo_->zoomFactor = 1.0f;
    controller.currentCameraInfo_->isRecording = false;
    controller.currentCameraInfo_->cameraType = CameraType::BACK;
    controller.sensorRotation_ = MobileRotation::UP;
    controller.eventHandler_ = nullptr;

    auto& motionManagers = MechBodyControllerService::GetInstance().motionManagers_;
    {
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
        motionManagers[999] = nullptr;
    }

    int32_t ret = controller.UpdateMotionManagers();
    EXPECT_EQ(ret, ERR_OK);

    {
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
        motionManagers.erase(999);
    }

    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateMotionManagers_004 end" << std::endl;
}

/**
 * @tc.name  : ProcessTargetByType_004
 * @tc.number: ProcessTargetByType_004
 * @tc.desc  : Testing ProcessTargetByType with default type.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, ProcessTargetByType_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest ProcessTargetByType_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();

    CameraStandard::Rect targetRect;
    targetRect.topLeftX = 0.3f;
    targetRect.topLeftY = 0.3f;
    targetRect.width = 0.2f;
    targetRect.height = 0.2f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> mockObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::SALIENT_DETECTION,
            timestamp, targetRect, 100, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;
    sptr<CameraStandard::MetadataObject> targetObject;
    int32_t ret = controller.ProcessTargetByType(mockObject, detectedObjects, targetObject);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_NE(targetObject, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTwoTest ProcessTargetByType_004 end" << std::endl;
}

/**
 * @tc.name  : FindBestMatchForBody_003
 * @tc.number: FindBestMatchForBody_003
 * @tc.desc  : Testing FindBestMatchForBody with multiple matches.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, FindBestMatchForBody_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest FindBestMatchForBody_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect targetRect1;
    targetRect1.topLeftX = 0.3f;
    targetRect1.topLeftY = 0.3f;
    targetRect1.width = 0.2f;
    targetRect1.height = 0.2f;

    CameraStandard::Rect targetRect2;
    targetRect2.topLeftX = 0.35f;
    targetRect2.topLeftY = 0.35f;
    targetRect2.width = 0.2f;
    targetRect2.height = 0.2f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> mockObject1 =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE,
            timestamp, targetRect1, 100, 0);

    sptr<CameraStandard::MetadataObject> mockObject2 =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE,
            timestamp, targetRect2, 100, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;
    detectedObjects.push_back(mockObject1);
    detectedObjects.push_back(mockObject2);

    sptr<CameraStandard::MetadataObject> result = controller.FindBestMatchForBody(targetRect1, detectedObjects);
    EXPECT_EQ(result, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTwoTest FindBestMatchForBody_003 end" << std::endl;
}

/**
 * @tc.name  : CinematicVideoModeTrackingTargetFilter_004
 * @tc.number: CinematicVideoModeTrackingTargetFilter_004
 * @tc.desc  : Testing CinematicVideoModeTrackingTargetFilter with LOCKED mode and zero region.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, CinematicVideoModeTrackingTargetFilter_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest CinematicVideoModeTrackingTargetFilter_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->sessionMode =
        static_cast<int32_t>(CameraStandard::SceneMode::CINEMATIC_VIDEO);
    controller.currentCameraInfo_->pauseFrameCount = 0;

    controller.lastTrackingFrame_ = std::make_shared<TrackingFrameParams>();
    controller.lastTrackingFrame_->targetId = 100;

    CameraStandard::FocusTrackingMetaInfo info;
    info.SetTrackingMode(CameraStandard::FocusTrackingMode::FOCUS_TRACKING_MODE_LOCKED);
    CameraStandard::Rect trackingRegion = {0.0f, 0.0f, 0.0f, 0.0f};
    info.SetTrackingRegion(trackingRegion);

    std::shared_ptr<TrackingFrameParams> trackingParams = std::make_shared<TrackingFrameParams>();
    trackingParams->targetId = 100;

    int32_t ret = controller.CinematicVideoModeTrackingTargetFilter(info, trackingParams);
    EXPECT_EQ(ret, INVALID_TRACKING_TARGET);

    DTEST_LOG << "McCameraTrackingControllerTwoTest CinematicVideoModeTrackingTargetFilter_004 end" << std::endl;
}

/**
 * @tc.name  : CinematicVideoModeTrackingTargetFilter_005
 * @tc.number: CinematicVideoModeTrackingTargetFilter_005
 * @tc.desc  : Testing CinematicVideoModeTrackingTargetFilter with type conversion allowed.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, CinematicVideoModeTrackingTargetFilter_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest CinematicVideoModeTrackingTargetFilter_005 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->sessionMode =
        static_cast<int32_t>(CameraStandard::SceneMode::CINEMATIC_VIDEO);
    controller.currentCameraInfo_->pauseFrameCount = 0;

    controller.lastTrackingFrame_ = std::make_shared<TrackingFrameParams>();
    controller.lastTrackingFrame_->targetId = 100;
    controller.lastTrackingFrame_->objectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_BODY);

    CameraStandard::Rect trackingRect;
    trackingRect.topLeftX = 0.3f;
    trackingRect.topLeftY = 0.3f;
    trackingRect.width = 0.4f;
    trackingRect.height = 0.4f;
    controller.trackingRect_ = trackingRect;

    CameraStandard::Rect lastTrackingRect;
    lastTrackingRect.topLeftX = 0.32f;
    lastTrackingRect.topLeftY = 0.32f;
    lastTrackingRect.width = 0.38f;
    lastTrackingRect.height = 0.38f;
    controller.lastTrackingRect_ = lastTrackingRect;

    CameraStandard::FocusTrackingMetaInfo info;

    std::shared_ptr<TrackingFrameParams> trackingParams = std::make_shared<TrackingFrameParams>();
    trackingParams->targetId = 200;
    trackingParams->objectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE);

    int32_t ret = controller.CinematicVideoModeTrackingTargetFilter(info, trackingParams);
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest CinematicVideoModeTrackingTargetFilter_005 end" << std::endl;
}

/**
 * @tc.name  : CinematicVideoModeTrackingTargetFilter_006
 * @tc.number: CinematicVideoModeTrackingTargetFilter_006
 * @tc.desc  : Testing CinematicVideoModeTrackingTargetFilter with type conversion not allowed.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, CinematicVideoModeTrackingTargetFilter_006, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest CinematicVideoModeTrackingTargetFilter_006 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->sessionMode =
        static_cast<int32_t>(CameraStandard::SceneMode::CINEMATIC_VIDEO);
    controller.currentCameraInfo_->pauseFrameCount = 0;

    controller.lastTrackingFrame_ = std::make_shared<TrackingFrameParams>();
    controller.lastTrackingFrame_->targetId = 100;
    controller.lastTrackingFrame_->objectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_BODY);

    CameraStandard::Rect trackingRect;
    trackingRect.topLeftX = 0.3f;
    trackingRect.topLeftY = 0.3f;
    trackingRect.width = 0.4f;
    trackingRect.height = 0.4f;
    controller.trackingRect_ = trackingRect;

    CameraStandard::Rect lastTrackingRect;
    lastTrackingRect.topLeftX = 0.32f;
    lastTrackingRect.topLeftY = 0.32f;
    lastTrackingRect.width = 0.38f;
    lastTrackingRect.height = 0.38f;
    controller.lastTrackingRect_ = lastTrackingRect;

    CameraStandard::FocusTrackingMetaInfo info;

    std::shared_ptr<TrackingFrameParams> trackingParams = std::make_shared<TrackingFrameParams>();
    trackingParams->targetId = 200;
    trackingParams->objectType = static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER);

    int32_t ret = controller.CinematicVideoModeTrackingTargetFilter(info, trackingParams);
    EXPECT_EQ(ret, INVALID_TRACKING_TARGET);

    DTEST_LOG << "McCameraTrackingControllerTwoTest CinematicVideoModeTrackingTargetFilter_006 end" << std::endl;
}

/**
 * @tc.name  : UpdateROI_005
 * @tc.number: UpdateROI_005
 * @tc.desc  : Testing UpdateROI with null trackingFrameParams.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, UpdateROI_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateROI_005 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();

    std::shared_ptr<TrackingFrameParams> trackingFrameParams = nullptr;
    CameraStandard::Rect rect;

    controller.UpdateROI(trackingFrameParams, rect);
    EXPECT_EQ(trackingFrameParams, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateROI_005 end" << std::endl;
}

/**
 * @tc.name  : UpdateROI_006
 * @tc.number: UpdateROI_006
 * @tc.desc  : Testing UpdateROI with small rect.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, UpdateROI_006, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateROI_006 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();

    std::shared_ptr<TrackingFrameParams> trackingFrameParams = std::make_shared<TrackingFrameParams>();
    trackingFrameParams->roi.width = 0.01f;
    trackingFrameParams->roi.height = 0.01f;
    CameraStandard::Rect rect;

    controller.UpdateROI(trackingFrameParams, rect);
    EXPECT_FLOAT_EQ(trackingFrameParams->roi.x, 0.0f);
    EXPECT_FLOAT_EQ(trackingFrameParams->roi.y, 0.0f);
    EXPECT_FLOAT_EQ(trackingFrameParams->roi.width, 0.0f);
    EXPECT_FLOAT_EQ(trackingFrameParams->roi.height, 0.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateROI_006 end" << std::endl;
}

/**
 * @tc.name  : UpdateMotionsWithTrackingData_003
 * @tc.number: UpdateMotionsWithTrackingData_003
 * @tc.desc  : Testing UpdateMotionsWithTrackingData with null params.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, UpdateMotionsWithTrackingData_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateMotionsWithTrackingData_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    std::shared_ptr<TrackingFrameParams> params = nullptr;
    int32_t trackingObjectId = 100;

    int32_t ret = controller.UpdateMotionsWithTrackingData(params, trackingObjectId);
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateMotionsWithTrackingData_003 end" << std::endl;
}

/**
 * @tc.name  : UpdateMotionsWithTrackingData_004
 * @tc.number: UpdateMotionsWithTrackingData_004
 * @tc.desc  : Testing UpdateMotionsWithTrackingData with tracking disabled.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, UpdateMotionsWithTrackingData_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateMotionsWithTrackingData_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->currentTrackingEnable = false;

    std::shared_ptr<TrackingFrameParams> params = std::make_shared<TrackingFrameParams>();
    int32_t trackingObjectId = 100;

    int32_t ret = controller.UpdateMotionsWithTrackingData(params, trackingObjectId);
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateMotionsWithTrackingData_004 end" << std::endl;
}

/**
 * @tc.name  : SetTrackingEnabled_002
 * @tc.number: SetTrackingEnabled_002
 * @tc.desc  : Testing SetTrackingEnabled with non-existent tokenId.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SetTrackingEnabled_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SetTrackingEnabled_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    uint32_t tokenId = 8888;
    controller.appSettings.erase(tokenId);

    int32_t ret = controller.SetTrackingEnabled(tokenId, true);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_NE(controller.appSettings.find(tokenId), controller.appSettings.end());
    EXPECT_TRUE(controller.appSettings[tokenId]->isTrackingEnabled);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SetTrackingEnabled_002 end" << std::endl;
}

/**
 * @tc.name  : OnTrackingEvent_001
 * @tc.number: OnTrackingEvent_001
 * @tc.desc  : Testing OnTrackingEvent with CAMERA_TRACKING_USER_ENABLED and no app setting.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, OnTrackingEvent_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest OnTrackingEvent_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->tokenId = 7777;
    controller.appSettings.erase(7777);

    int32_t mechId = 100;
    int32_t ret = controller.OnTrackingEvent(mechId, TrackingEvent::CAMERA_TRACKING_USER_ENABLED);
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest OnTrackingEvent_001 end" << std::endl;
}

/**
 * @tc.name  : IsCurrentTrackingEnabled_002
 * @tc.number: IsCurrentTrackingEnabled_002
 * @tc.desc  : Testing IsCurrentTrackingEnabled with empty motionManagers.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, IsCurrentTrackingEnabled_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest IsCurrentTrackingEnabled_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->currentTrackingEnable = false;
    MechBodyControllerService::GetInstance().motionManagers_.clear();

    bool result = controller.IsCurrentTrackingEnabled();
    EXPECT_FALSE(result);

    DTEST_LOG << "McCameraTrackingControllerTwoTest IsCurrentTrackingEnabled_002 end" << std::endl;
}

/**
 * @tc.name  : IsCurrentTrackingEnabled_003
 * @tc.number: IsCurrentTrackingEnabled_003
 * @tc.desc  : Testing IsCurrentTrackingEnabled with null motionManager.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, IsCurrentTrackingEnabled_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest IsCurrentTrackingEnabled_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr = nullptr;
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;

    bool result = controller.IsCurrentTrackingEnabled();
    EXPECT_FALSE(result);

    DTEST_LOG << "McCameraTrackingControllerTwoTest IsCurrentTrackingEnabled_003 end" << std::endl;
}

/**
 * @tc.name  : IsCurrentTrackingEnabled_004
 * @tc.number: IsCurrentTrackingEnabled_004
 * @tc.desc  : Testing IsCurrentTrackingEnabled with deviceIsEnable true.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, IsCurrentTrackingEnabled_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest IsCurrentTrackingEnabled_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->currentTrackingEnable = true;
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;

    bool result = controller.IsCurrentTrackingEnabled();
    EXPECT_TRUE(result);

    DTEST_LOG << "McCameraTrackingControllerTwoTest IsCurrentTrackingEnabled_004 end" << std::endl;
}

/**
 * @tc.name  : UpdateActionControl_001
 * @tc.number: UpdateActionControl_001
 * @tc.desc  : Testing UpdateActionControl with null eventHandler.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, UpdateActionControl_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateActionControl_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.eventHandler_ = nullptr;

    int32_t ret = controller.UpdateActionControl();
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateActionControl_001 end" << std::endl;
}

/**
 * @tc.name  : UpdateActionControl_002
 * @tc.number: UpdateActionControl_002
 * @tc.desc  : Testing UpdateActionControl with tracking conditions met.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, UpdateActionControl_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateActionControl_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->currentTrackingEnable = true;
    controller.currentCameraInfo_->trackingTargetNum = 1;

    int32_t ret = controller.UpdateActionControl();
    EXPECT_EQ(ret, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest UpdateActionControl_002 end" << std::endl;
}

/**
 * @tc.name  : SetStickOffset_006
 * @tc.number: SetStickOffset_006
 * @tc.desc  : Testing SetStickOffset with stickX > STICK_BASE.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SetStickOffset_006, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SetStickOffset_006 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    int16_t stickX = 400;
    int16_t stickY = 0;

    controller.SetStickOffset(stickX, stickY);
    EXPECT_FLOAT_EQ(controller.targetHorizontal_, 0.02f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SetStickOffset_006 end" << std::endl;
}

/**
 * @tc.name  : SetStickOffset_007
 * @tc.number: SetStickOffset_007
 * @tc.desc  : Testing SetStickOffset with stickX < -STICK_BASE.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SetStickOffset_007, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SetStickOffset_007 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    int16_t stickX = -400;
    int16_t stickY = 0;

    controller.targetHorizontal_ = 0.0f;
    controller.targetVertical_ = 0.0f;
    controller.SetStickOffset(stickX, stickY);
    EXPECT_FLOAT_EQ(controller.targetHorizontal_, -0.02f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SetStickOffset_007 end" << std::endl;
}

/**
 * @tc.name  : SetStickOffset_008
 * @tc.number: SetStickOffset_008
 * @tc.desc  : Testing SetStickOffset with stickY > STICK_BASE.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SetStickOffset_008, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SetStickOffset_008 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    int16_t stickX = 0;
    int16_t stickY = 400;

    controller.SetStickOffset(stickX, stickY);
    EXPECT_FLOAT_EQ(controller.targetVertical_, 0.02f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SetStickOffset_008 end" << std::endl;
}

/**
 * @tc.name  : SetStickOffset_009
 * @tc.number: SetStickOffset_009
 * @tc.desc  : Testing SetStickOffset with stickY < -STICK_BASE.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SetStickOffset_009, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SetStickOffset_009 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    int16_t stickX = 0;
    int16_t stickY = -400;

    controller.targetHorizontal_ = 0.0f;
    controller.targetVertical_ = 0.0f;
    controller.SetStickOffset(stickX, stickY);
    EXPECT_FLOAT_EQ(controller.targetVertical_, -0.02f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SetStickOffset_009 end" << std::endl;
}

/**
 * @tc.name  : SetTrackingLayout_003
 * @tc.number: SetTrackingLayout_003
 * @tc.desc  : Testing SetTrackingLayout with RIGHT layout.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SetTrackingLayout_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SetTrackingLayout_003 begin" << std::endl;

    g_mockMechState = AttachmentStateMap::ATTACHED;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.sensorRotation_ = MobileRotation::UP;
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    CameraTrackingLayout cameraTrackingLayout = CameraTrackingLayout::RIGHT;

    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;

    int32_t result = controller.SetTrackingLayout(cameraTrackingLayout);
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SetTrackingLayout_003 end" << std::endl;
}

/**
 * @tc.name  : SetTrackingLayout_004
 * @tc.number: SetTrackingLayout_004
 * @tc.desc  : Testing SetTrackingLayout with LEFT layout.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SetTrackingLayout_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SetTrackingLayout_004 begin" << std::endl;

    g_mockMechState = AttachmentStateMap::ATTACHED;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.sensorRotation_ = MobileRotation::UP;
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    CameraTrackingLayout cameraTrackingLayout = CameraTrackingLayout::LEFT;

    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;

    int32_t result = controller.SetTrackingLayout(cameraTrackingLayout);
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SetTrackingLayout_004 end" << std::endl;
}

/**
 * @tc.name  : SetTrackingLayout_005
 * @tc.number: SetTrackingLayout_005
 * @tc.desc  : Testing SetTrackingLayout with DEFAULT layout.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SetTrackingLayout_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SetTrackingLayout_005 begin" << std::endl;

    g_mockMechState = AttachmentStateMap::ATTACHED;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.sensorRotation_ = MobileRotation::UP;
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    CameraTrackingLayout cameraTrackingLayout = CameraTrackingLayout::DEFAULT;

    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;

    int32_t result = controller.SetTrackingLayout(cameraTrackingLayout);
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SetTrackingLayout_005 end" << std::endl;
}

/**
 * @tc.name  : SearchTarget_001
 * @tc.number: SearchTarget_001
 * @tc.desc  : Testing SearchTarget with null targetInfo.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SearchTarget_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SearchTarget_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::shared_ptr<TargetInfo> targetInfo = nullptr;
    std::shared_ptr<SearchParams> searchParams = std::make_shared<SearchParams>();
    std::string napiCmdId = "test_cmd_001";
    uint32_t tokenId = 1001;

    int32_t result = controller.SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SearchTarget_001 end" << std::endl;
}

/**
 * @tc.name  : SearchTarget_002
 * @tc.number: SearchTarget_002
 * @tc.desc  : Testing SearchTarget with null searchParams.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SearchTarget_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SearchTarget_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::shared_ptr<TargetInfo> targetInfo = std::make_shared<TargetInfo>();
    std::shared_ptr<SearchParams> searchParams = nullptr;
    std::string napiCmdId = "test_cmd_002";
    uint32_t tokenId = 1002;

    int32_t result = controller.SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);
    EXPECT_EQ(result, INVALID_PARAMETERS_ERR);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SearchTarget_002 end" << std::endl;
}

/**
 * @tc.name  : SearchTarget_003
 * @tc.number: SearchTarget_003
 * @tc.desc  : Testing SearchTarget with empty motionManagers.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SearchTarget_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SearchTarget_003 begin" << std::endl;

    g_mockMechState = AttachmentStateMap::DETACHED;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::shared_ptr<TargetInfo> targetInfo = std::make_shared<TargetInfo>();
    std::shared_ptr<SearchParams> searchParams = std::make_shared<SearchParams>();
    std::string napiCmdId = "test_cmd_003";
    uint32_t tokenId = 1003;
    MechBodyControllerService::GetInstance().motionManagers_.clear();

    int32_t result = controller.SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);
    EXPECT_EQ(result, NO_DEVICE_CONNECTED);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SearchTarget_003 end" << std::endl;
}

/**
 * @tc.name  : SearchTarget_004
 * @tc.number: SearchTarget_004
 * @tc.desc  : Testing SearchTarget with existing tracking target.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SearchTarget_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SearchTarget_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::shared_ptr<TargetInfo> targetInfo = std::make_shared<TargetInfo>();
    std::shared_ptr<SearchParams> searchParams = std::make_shared<SearchParams>();
    std::string napiCmdId = "test_cmd_004";
    uint32_t tokenId = 1004;
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->trackingTargetNum = 1;

    // Add motionManager to pass motionManagers.empty() check
    int32_t mechId = 100;
    std::shared_ptr<MotionManager> motionMgr =
        std::make_shared<MotionManager>(std::make_shared<TransportSendAdapter>(), mechId);
    MechBodyControllerService::GetInstance().motionManagers_[mechId] = motionMgr;

    int32_t result = controller.SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);
    EXPECT_EQ(result, ERR_OK);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SearchTarget_004 end" << std::endl;
}

/**
 * @tc.name  : SearchTargetRotateFinish_002
 * @tc.number: SearchTargetRotateFinish_002
 * @tc.desc  : Testing SearchTargetRotateFinish with non-matching cmdId.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SearchTargetRotateFinish_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SearchTargetRotateFinish_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->searchTargetNapiCmdId = "cmd123";
    std::string cmdId = "cmd456";

    controller.SearchTargetRotateFinish(cmdId);
    EXPECT_EQ(controller.currentCameraInfo_->searchTargetNapiCmdId, "cmd123");

    DTEST_LOG << "McCameraTrackingControllerTwoTest SearchTargetRotateFinish_002 end" << std::endl;
}

/**
 * @tc.name  : SearchTargetStop_002
 * @tc.number: SearchTargetStop_002
 * @tc.desc  : Testing SearchTargetStop with searchingTarget false.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SearchTargetStop_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SearchTargetStop_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->searchingTarget = false;

    controller.SearchTargetStop();
    EXPECT_FALSE(controller.currentCameraInfo_->searchingTarget);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SearchTargetStop_002 end" << std::endl;
}

/**
 * @tc.name  : SearchTargetStop_003
 * @tc.number: SearchTargetStop_003
 * @tc.desc  : Testing SearchTargetStop with non-existent tokenId.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SearchTargetStop_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SearchTargetStop_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();
    controller.currentCameraInfo_->searchingTarget = true;
    controller.currentCameraInfo_->tokenId = 6666;
    controller.appSettings.erase(6666);

    controller.SearchTargetStop();
    EXPECT_FALSE(controller.currentCameraInfo_->searchingTarget);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SearchTargetStop_003 end" << std::endl;
}

/**
 * @tc.name  : CalculateSensorRotation_002
 * @tc.number: CalculateSensorRotation_002
 * @tc.desc  : Testing CalculateSensorRotation with null gravityData.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, CalculateSensorRotation_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateSensorRotation_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    GravityData* gravityData = nullptr;

    MobileRotation result = controller.CalculateSensorRotation(gravityData);
    EXPECT_EQ(result, MobileRotation::UP);

    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateSensorRotation_002 end" << std::endl;
}

/**
 * @tc.name  : CalculateSensorRotation_003
 * @tc.number: CalculateSensorRotation_003
 * @tc.desc  : Testing CalculateSensorRotation returning INVALID.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, CalculateSensorRotation_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateSensorRotation_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    GravityData gravityData;
    gravityData.x = 0.0f;
    gravityData.y = 0.0f;
    gravityData.z = 10.0f;

    MobileRotation result = controller.CalculateSensorRotation(&gravityData);
    EXPECT_EQ(result, MobileRotation::INVALID);

    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateSensorRotation_003 end" << std::endl;
}

/**
 * @tc.name  : CalculateSensorRotation_004
 * @tc.number: CalculateSensorRotation_004
 * @tc.desc  : Testing CalculateSensorRotation returning UP.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, CalculateSensorRotation_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateSensorRotation_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    GravityData gravityData;
    gravityData.x = 0.0f;
    gravityData.y = 1.0f;
    gravityData.z = 0.0f;

    MobileRotation result = controller.CalculateSensorRotation(&gravityData);
    EXPECT_EQ(result, MobileRotation::UP);

    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateSensorRotation_004 end" << std::endl;
}

/**
 * @tc.name  : CalculateSensorRotation_005
 * @tc.number: CalculateSensorRotation_005
 * @tc.desc  : Testing CalculateSensorRotation returning RIGHT.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, CalculateSensorRotation_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateSensorRotation_005 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    GravityData gravityData;
    gravityData.x = -1.0f;
    gravityData.y = 0.0f;
    gravityData.z = 0.0f;

    MobileRotation result = controller.CalculateSensorRotation(&gravityData);
    EXPECT_EQ(result, MobileRotation::RIGHT);

    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateSensorRotation_005 end" << std::endl;
}

/**
 * @tc.name  : CalculateSensorRotation_006
 * @tc.number: CalculateSensorRotation_006
 * @tc.desc  : Testing CalculateSensorRotation returning DOWN.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, CalculateSensorRotation_006, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateSensorRotation_006 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    GravityData gravityData;
    gravityData.x = 0.0f;
    gravityData.y = -1.0f;
    gravityData.z = 0.0f;

    MobileRotation result = controller.CalculateSensorRotation(&gravityData);
    EXPECT_EQ(result, MobileRotation::DOWN);

    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateSensorRotation_006 end" << std::endl;
}

/**
 * @tc.name  : CalculateSensorRotation_007
 * @tc.number: CalculateSensorRotation_007
 * @tc.desc  : Testing CalculateSensorRotation returning LEFT.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, CalculateSensorRotation_007, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateSensorRotation_007 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    GravityData gravityData;
    gravityData.x = 1.0f;
    gravityData.y = 0.0f;
    gravityData.z = 0.0f;

    MobileRotation result = controller.CalculateSensorRotation(&gravityData);
    EXPECT_EQ(result, MobileRotation::LEFT);

    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateSensorRotation_007 end" << std::endl;
}

/**
 * @tc.name  : ConvertObjectType_002
 * @tc.number: ConvertObjectType_002
 * @tc.desc  : Testing ConvertObjectType with FACE type.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, ConvertObjectType_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest ConvertObjectType_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    CameraStandard::MetadataObjectType cameraObjectType = CameraStandard::MetadataObjectType::FACE;
    uint8_t mechObjectType = 0;

    controller.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE));

    DTEST_LOG << "McCameraTrackingControllerTwoTest ConvertObjectType_002 end" << std::endl;
}

/**
 * @tc.name  : ConvertObjectType_003
 * @tc.number: ConvertObjectType_003
 * @tc.desc  : Testing ConvertObjectType with BASE_FACE_DETECTION type.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, ConvertObjectType_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest ConvertObjectType_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    CameraStandard::MetadataObjectType cameraObjectType = CameraStandard::MetadataObjectType::BASE_FACE_DETECTION;
    uint8_t mechObjectType = 0;

    controller.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_FACE));

    DTEST_LOG << "McCameraTrackingControllerTwoTest ConvertObjectType_003 end" << std::endl;
}

/**
 * @tc.name  : ConvertObjectType_004
 * @tc.number: ConvertObjectType_004
 * @tc.desc  : Testing ConvertObjectType with HUMAN_BODY type.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, ConvertObjectType_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest ConvertObjectType_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    CameraStandard::MetadataObjectType cameraObjectType = CameraStandard::MetadataObjectType::HUMAN_BODY;
    uint8_t mechObjectType = 0;

    controller.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_BODY));

    DTEST_LOG << "McCameraTrackingControllerTwoTest ConvertObjectType_004 end" << std::endl;
}

/**
 * @tc.name  : ConvertObjectType_005
 * @tc.number: ConvertObjectType_005
 * @tc.desc  : Testing ConvertObjectType with HUMAN_HEAD type.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, ConvertObjectType_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest ConvertObjectType_005 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    CameraStandard::MetadataObjectType cameraObjectType = CameraStandard::MetadataObjectType::HUMAN_HEAD;
    uint8_t mechObjectType = 0;

    controller.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_HEAD));

    DTEST_LOG << "McCameraTrackingControllerTwoTest ConvertObjectType_005 end" << std::endl;
}

/**
 * @tc.name  : ConvertObjectType_006
 * @tc.number: ConvertObjectType_006
 * @tc.desc  : Testing ConvertObjectType with default type.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, ConvertObjectType_006, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest ConvertObjectType_006 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    CameraStandard::MetadataObjectType cameraObjectType = CameraStandard::MetadataObjectType::SALIENT_DETECTION;
    uint8_t mechObjectType = 0;

    controller.ConvertObjectType(cameraObjectType, mechObjectType);
    EXPECT_EQ(mechObjectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER));

    DTEST_LOG << "McCameraTrackingControllerTwoTest ConvertObjectType_006 end" << std::endl;
}

/**
 * @tc.name  : RegisterSensorListener_001
 * @tc.number: RegisterSensorListener_001
 * @tc.desc  : Testing RegisterSensorListener when callback exists.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, RegisterSensorListener_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest RegisterSensorListener_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.user.callback = reinterpret_cast<void(*)(SensorEvent*)>(0x1234);

    controller.RegisterSensorListener();
    EXPECT_EQ(controller.user.callback, reinterpret_cast<void(*)(SensorEvent*)>(0x1234));
    controller.user.callback = nullptr;

    DTEST_LOG << "McCameraTrackingControllerTwoTest RegisterSensorListener_001 end" << std::endl;
}

/**
 * @tc.name  : UnRegisterSensorListener_002
 * @tc.number: UnRegisterSensorListener_002
 * @tc.desc  : Testing UnRegisterSensorListener when callback is null.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, UnRegisterSensorListener_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest UnRegisterSensorListener_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.user.callback = nullptr;

    controller.UnRegisterSensorListener();
    EXPECT_EQ(controller.user.callback, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTwoTest UnRegisterSensorListener_002 end" << std::endl;
}

/**
 * @tc.name  : AdjustROI_FrontCamera_RIGHT_DOWN_001
 * @tc.number: AdjustROI_FrontCamera_RIGHT_DOWN_001
 * @tc.desc  : Testing AdjustROI with FRONT camera and RIGHT/DOWN rotation.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustROI_FrontCamera_RIGHT_DOWN_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustROI_FrontCamera_RIGHT_DOWN_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi;
    CameraStandard::Rect rect;
    CameraType cameraType = CameraType::FRONT;
    MobileRotation sensorRotation = MobileRotation::RIGHT;

    controller.AdjustROI(roi, rect, cameraType, sensorRotation);
    EXPECT_FLOAT_EQ(roi.x, rect.topLeftX + roi.width / 2.0f);
    EXPECT_FLOAT_EQ(roi.y, rect.topLeftY + roi.height / 2.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustROI_FrontCamera_RIGHT_DOWN_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustROI_BackCamera_UP_LEFT_001
 * @tc.number: AdjustROI_BackCamera_UP_LEFT_001
 * @tc.desc  : Testing AdjustROI with BACK camera and UP/LEFT rotation.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustROI_BackCamera_UP_LEFT_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustROI_BackCamera_UP_LEFT_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi;
    CameraStandard::Rect rect;
    CameraType cameraType = CameraType::BACK;
    MobileRotation sensorRotation = MobileRotation::UP;

    controller.AdjustROI(roi, rect, cameraType, sensorRotation);
    EXPECT_FLOAT_EQ(roi.x, rect.topLeftX + roi.width / 2.0f);
    EXPECT_FLOAT_EQ(roi.y, rect.topLeftY + roi.height / 2.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustROI_BackCamera_UP_LEFT_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustROI_BackCamera_RIGHT_DOWN_001
 * @tc.number: AdjustROI_BackCamera_RIGHT_DOWN_001
 * @tc.desc  : Testing AdjustROI with BACK camera and RIGHT/DOWN rotation.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustROI_BackCamera_RIGHT_DOWN_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustROI_BackCamera_RIGHT_DOWN_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi;
    CameraStandard::Rect rect;
    CameraType cameraType = CameraType::BACK;
    MobileRotation sensorRotation = MobileRotation::RIGHT;

    controller.AdjustROI(roi, rect, cameraType, sensorRotation);
    EXPECT_FLOAT_EQ(roi.x, 1.0f - rect.topLeftX - roi.width / 2.0f);
    EXPECT_FLOAT_EQ(roi.y, 1.0f - rect.topLeftY - roi.height / 2.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustROI_BackCamera_RIGHT_DOWN_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustOffset_001
 * @tc.number: AdjustOffset_001
 * @tc.desc  : Testing AdjustOffset with targetVertical > vertical.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustOffset_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustOffset_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::shared_ptr<TrackingFrameParams> trackingParams = std::make_shared<TrackingFrameParams>();
    trackingParams->roi.x = 0.5f;
    trackingParams->roi.y = 0.5f;
    controller.targetVertical_ = 0.5f;
    controller.vertical_ = 0.0f;
    controller.targetHorizontal_ = 0.0f;
    controller.horizontal_ = 0.0f;
    controller.isStick_ = true;
    controller.sensorRotation_ = MobileRotation::UP;
    controller.lastStickTime_ = std::chrono::steady_clock::now();

    controller.AdjustOffset(trackingParams, CameraType::FRONT);
    EXPECT_EQ(trackingParams->objectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER));

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustOffset_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustOffset_002
 * @tc.number: AdjustOffset_002
 * @tc.desc  : Testing AdjustOffset with targetVertical < vertical.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustOffset_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustOffset_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::shared_ptr<TrackingFrameParams> trackingParams = std::make_shared<TrackingFrameParams>();
    trackingParams->roi.x = 0.5f;
    trackingParams->roi.y = 0.5f;
    controller.targetVertical_ = 0.0f;
    controller.vertical_ = 0.5f;
    controller.targetHorizontal_ = 0.0f;
    controller.horizontal_ = 0.0f;
    controller.isStick_ = true;
    controller.sensorRotation_ = MobileRotation::UP;
    controller.lastStickTime_ = std::chrono::steady_clock::now();

    controller.AdjustOffset(trackingParams, CameraType::FRONT);
    EXPECT_EQ(trackingParams->objectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER));

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustOffset_002 end" << std::endl;
}

/**
 * @tc.name  : AdjustOffset_003
 * @tc.number: AdjustOffset_003
 * @tc.desc  : Testing AdjustOffset with targetHorizontal > horizontal.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustOffset_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustOffset_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::shared_ptr<TrackingFrameParams> trackingParams = std::make_shared<TrackingFrameParams>();
    trackingParams->roi.x = 0.5f;
    trackingParams->roi.y = 0.5f;
    controller.targetVertical_ = 0.0f;
    controller.vertical_ = 0.0f;
    controller.targetHorizontal_ = 0.5f;
    controller.horizontal_ = 0.0f;
    controller.isStick_ = true;
    controller.sensorRotation_ = MobileRotation::UP;
    controller.lastStickTime_ = std::chrono::steady_clock::now();

    controller.AdjustOffset(trackingParams, CameraType::FRONT);
    EXPECT_EQ(trackingParams->objectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER));

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustOffset_003 end" << std::endl;
}

/**
 * @tc.name  : AdjustOffset_004
 * @tc.number: AdjustOffset_004
 * @tc.desc  : Testing AdjustOffset with targetHorizontal < horizontal.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustOffset_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustOffset_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::shared_ptr<TrackingFrameParams> trackingParams = std::make_shared<TrackingFrameParams>();
    trackingParams->roi.x = 0.5f;
    trackingParams->roi.y = 0.5f;
    controller.targetVertical_ = 0.0f;
    controller.vertical_ = 0.0f;
    controller.targetHorizontal_ = 0.0f;
    controller.horizontal_ = 0.5f;
    controller.isStick_ = true;
    controller.sensorRotation_ = MobileRotation::UP;
    controller.lastStickTime_ = std::chrono::steady_clock::now();

    controller.AdjustOffset(trackingParams, CameraType::FRONT);
    EXPECT_EQ(trackingParams->objectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER));

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustOffset_004 end" << std::endl;
}

/**
 * @tc.name  : AdjustOffset_005
 * @tc.number: AdjustOffset_005
 * @tc.desc  : Testing AdjustOffset with isStick false and UP/DOWN rotation.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustOffset_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustOffset_005 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::shared_ptr<TrackingFrameParams> trackingParams = std::make_shared<TrackingFrameParams>();
    trackingParams->roi.x = 0.5f;
    trackingParams->roi.y = 0.5f;
    controller.targetVertical_ = 0.0f;
    controller.vertical_ = 0.0f;
    controller.targetHorizontal_ = 0.0f;
    controller.horizontal_ = 0.0f;
    controller.isStick_ = false;
    controller.sensorRotation_ = MobileRotation::UP;
    controller.lastStickTime_ = std::chrono::steady_clock::now();

    controller.AdjustOffset(trackingParams, CameraType::FRONT);
    EXPECT_EQ(trackingParams->objectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER));

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustOffset_005 end" << std::endl;
}

/**
 * @tc.name  : AdjustOffset_006
 * @tc.number: AdjustOffset_006
 * @tc.desc  : Testing AdjustOffset with isStick false and LEFT/RIGHT rotation.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustOffset_006, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustOffset_006 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    std::shared_ptr<TrackingFrameParams> trackingParams = std::make_shared<TrackingFrameParams>();
    trackingParams->roi.x = 0.5f;
    trackingParams->roi.y = 0.5f;
    controller.targetVertical_ = 0.0f;
    controller.vertical_ = 0.0f;
    controller.targetHorizontal_ = 0.0f;
    controller.horizontal_ = 0.0f;
    controller.isStick_ = false;
    controller.sensorRotation_ = MobileRotation::LEFT;
    controller.lastStickTime_ = std::chrono::steady_clock::now();

    controller.AdjustOffset(trackingParams, CameraType::FRONT);
    EXPECT_EQ(trackingParams->objectType, static_cast<uint8_t>(TrackingObjectType::MSG_OBJ_OTHER));

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustOffset_006 end" << std::endl;
}

/**
 * @tc.name  : AddYOffset_001
 * @tc.number: AddYOffset_001
 * @tc.desc  : Testing AddYOffset with FRONT camera.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AddYOffset_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AddYOffset_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::FRONT;
    float offsetX = 0.1f;
    float offsetY = 0.1f;
    bool isFace = false;
    bool isBody = false;

    controller.AddYOffset(roi, cameraType, offsetX, offsetY, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.x, 0.4f);
    EXPECT_FLOAT_EQ(roi.y, 0.4f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AddYOffset_001 end" << std::endl;
}

/**
 * @tc.name  : AddYOffset_002
 * @tc.number: AddYOffset_002
 * @tc.desc  : Testing AddYOffset with isFace true.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AddYOffset_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AddYOffset_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::BACK;
    float offsetX = 0.1f;
    float offsetY = 0.1f;
    bool isFace = true;
    bool isBody = false;

    controller.AddYOffset(roi, cameraType, offsetX, offsetY, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.x, 0.5f);
    EXPECT_FLOAT_EQ(roi.y, 0.6f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AddYOffset_002 end" << std::endl;
}

/**
 * @tc.name  : AddYOffset_003
 * @tc.number: AddYOffset_003
 * @tc.desc  : Testing AddYOffset with isBody true.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AddYOffset_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AddYOffset_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::BACK;
    float offsetX = 0.1f;
    float offsetY = 0.1f;
    bool isFace = false;
    bool isBody = true;

    controller.AddYOffset(roi, cameraType, offsetX, offsetY, isFace, isBody);
    EXPECT_FLOAT_EQ(roi.x, 0.3f);
    EXPECT_FLOAT_EQ(roi.y, 0.6f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AddYOffset_003 end" << std::endl;
}

/**
 * @tc.name  : AddXOffset_001
 * @tc.number: AddXOffset_001
 * @tc.desc  : Testing AddXOffset with isFace and FRONT camera.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AddXOffset_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AddXOffset_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::FRONT;
    float offsetX = 0.1f;
    float offsetY = 0.1f;
    bool isFace = true;
    bool isBody = false;

    controller.AddXOffset(roi, cameraType, offsetX, offsetY, isFace, isBody);
    EXPECT_GE(roi.x, 0.0f);
    EXPECT_LE(roi.x, 1.0f);
    EXPECT_GE(roi.y, 0.0f);
    EXPECT_LE(roi.y, 1.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AddXOffset_001 end" << std::endl;
}

/**
 * @tc.name  : AddXOffset_002
 * @tc.number: AddXOffset_002
 * @tc.desc  : Testing AddXOffset with isFace and BACK camera.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AddXOffset_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AddXOffset_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::BACK;
    float offsetX = 0.1f;
    float offsetY = 0.1f;
    bool isFace = true;
    bool isBody = false;

    controller.AddXOffset(roi, cameraType, offsetX, offsetY, isFace, isBody);
    EXPECT_GE(roi.x, 0.0f);
    EXPECT_LE(roi.x, 1.0f);
    EXPECT_GE(roi.y, 0.0f);
    EXPECT_LE(roi.y, 1.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AddXOffset_002 end" << std::endl;
}

/**
 * @tc.name  : AddXOffset_003
 * @tc.number: AddXOffset_003
 * @tc.desc  : Testing AddXOffset with isBody and FRONT camera.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AddXOffset_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AddXOffset_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::FRONT;
    float offsetX = 0.1f;
    float offsetY = 0.1f;
    bool isFace = false;
    bool isBody = true;

    controller.AddXOffset(roi, cameraType, offsetX, offsetY, isFace, isBody);
    EXPECT_GE(roi.x, 0.0f);
    EXPECT_LE(roi.x, 1.0f);
    EXPECT_GE(roi.y, 0.0f);
    EXPECT_LE(roi.y, 1.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AddXOffset_003 end" << std::endl;
}

/**
 * @tc.name  : AddXOffset_004
 * @tc.number: AddXOffset_004
 * @tc.desc  : Testing AddXOffset with isBody and BACK camera.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AddXOffset_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AddXOffset_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::BACK;
    float offsetX = 0.1f;
    float offsetY = 0.1f;
    bool isFace = false;
    bool isBody = true;

    controller.AddXOffset(roi, cameraType, offsetX, offsetY, isFace, isBody);
    EXPECT_GE(roi.x, 0.0f);
    EXPECT_LE(roi.x, 1.0f);
    EXPECT_GE(roi.y, 0.0f);
    EXPECT_LE(roi.y, 1.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AddXOffset_004 end" << std::endl;
}

/**
 * @tc.name  : AdjustYOffset_Back_001
 * @tc.number: AdjustYOffset_Back_001
 * @tc.desc  : Testing AdjustYOffset with BACK camera.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustYOffset_Back_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustYOffset_Back_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::BACK;
    bool isFace = false;
    bool isBody = false;
    float x = 0.0f;
    float y = 0.0f;

    controller.AdjustYOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_GE(roi.x, 0.0f);
    EXPECT_LE(roi.x, 1.0f);
    EXPECT_GE(roi.y, 0.0f);
    EXPECT_LE(roi.y, 1.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustYOffset_Back_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustYOffset_VirtualCenter_001
 * @tc.number: AdjustYOffset_VirtualCenter_001
 * @tc.desc  : Testing AdjustYOffset with roi.y > virtualCenterY.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustYOffset_VirtualCenter_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustYOffset_VirtualCenter_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.8f, 0.8f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::FRONT;
    bool isFace = false;
    bool isBody = false;
    float x = 0.0f;
    float y = 0.1f;

    controller.AdjustYOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_GE(roi.x, 0.0f);
    EXPECT_LE(roi.x, 1.0f);
    EXPECT_GE(roi.y, 0.0f);
    EXPECT_LE(roi.y, 1.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustYOffset_VirtualCenter_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustYOffset_VirtualCenter_002
 * @tc.number: AdjustYOffset_VirtualCenter_002
 * @tc.desc  : Testing AdjustYOffset with roi.y < virtualCenterY.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustYOffset_VirtualCenter_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustYOffset_VirtualCenter_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.2f, 0.2f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::FRONT;
    bool isFace = false;
    bool isBody = false;
    float x = 0.0f;
    float y = -0.1f;

    controller.AdjustYOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_GE(roi.x, 0.0f);
    EXPECT_LE(roi.x, 1.0f);
    EXPECT_GE(roi.y, 0.0f);
    EXPECT_LE(roi.y, 1.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustYOffset_VirtualCenter_002 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_Front_001
 * @tc.number: AdjustXOffset_Front_001
 * @tc.desc  : Testing AdjustXOffset with isFace and FRONT camera.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustXOffset_Front_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustXOffset_Front_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::FRONT;
    bool isFace = true;
    bool isBody = false;
    float x = 0.0f;
    float y = 0.0f;

    controller.AdjustXOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_GE(roi.x, 0.0f);
    EXPECT_LE(roi.x, 1.0f);
    EXPECT_GE(roi.y, 0.0f);
    EXPECT_LE(roi.y, 1.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustXOffset_Front_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_Back_001
 * @tc.number: AdjustXOffset_Back_001
 * @tc.desc  : Testing AdjustXOffset with isFace and BACK camera.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustXOffset_Back_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustXOffset_Back_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::BACK;
    bool isFace = true;
    bool isBody = false;
    float x = 0.0f;
    float y = 0.0f;

    controller.AdjustXOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_GE(roi.x, 0.0f);
    EXPECT_LE(roi.x, 1.0f);
    EXPECT_GE(roi.y, 0.0f);
    EXPECT_LE(roi.y, 1.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustXOffset_Back_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_BodyFront_001
 * @tc.number: AdjustXOffset_BodyFront_001
 * @tc.desc  : Testing AdjustXOffset with isBody and FRONT camera.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustXOffset_BodyFront_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustXOffset_BodyFront_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::FRONT;
    bool isFace = false;
    bool isBody = true;
    float x = 0.0f;
    float y = 0.0f;

    controller.AdjustXOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_GE(roi.x, 0.0f);
    EXPECT_LE(roi.x, 1.0f);
    EXPECT_GE(roi.y, 0.0f);
    EXPECT_LE(roi.y, 1.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustXOffset_BodyFront_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_BodyBack_001
 * @tc.number: AdjustXOffset_BodyBack_001
 * @tc.desc  : Testing AdjustXOffset with isBody and BACK camera.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustXOffset_BodyBack_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustXOffset_BodyBack_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.5f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::BACK;
    bool isFace = false;
    bool isBody = true;
    float x = 0.0f;
    float y = 0.0f;

    controller.AdjustXOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_GE(roi.x, 0.0f);
    EXPECT_LE(roi.x, 1.0f);
    EXPECT_GE(roi.y, 0.0f);
    EXPECT_LE(roi.y, 1.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustXOffset_BodyBack_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_VirtualCenter_001
 * @tc.number: AdjustXOffset_VirtualCenter_001
 * @tc.desc  : Testing AdjustXOffset with roi.x > virtualCenterX.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustXOffset_VirtualCenter_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustXOffset_VirtualCenter_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.8f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::FRONT;
    bool isFace = false;
    bool isBody = false;
    float x = 0.1f;
    float y = 0.0f;

    controller.AdjustXOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_GE(roi.x, 0.0f);
    EXPECT_LE(roi.x, 1.0f);
    EXPECT_GE(roi.y, 0.0f);
    EXPECT_LE(roi.y, 1.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustXOffset_VirtualCenter_001 end" << std::endl;
}

/**
 * @tc.name  : AdjustXOffset_VirtualCenter_002
 * @tc.number: AdjustXOffset_VirtualCenter_002
 * @tc.desc  : Testing AdjustXOffset with roi.x < virtualCenterX.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, AdjustXOffset_VirtualCenter_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustXOffset_VirtualCenter_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    ROI roi = {0.2f, 0.5f, 0.2f, 0.2f};
    CameraType cameraType = CameraType::FRONT;
    bool isFace = false;
    bool isBody = false;
    float x = -0.1f;
    float y = 0.0f;

    controller.AdjustXOffset(roi, cameraType, x, y, isFace, isBody);
    EXPECT_GE(roi.x, 0.0f);
    EXPECT_LE(roi.x, 1.0f);
    EXPECT_GE(roi.y, 0.0f);
    EXPECT_LE(roi.y, 1.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest AdjustXOffset_VirtualCenter_002 end" << std::endl;
}

/**
 * @tc.name  : MechControllerIpcDeathListener_OnRemoteDied_001
 * @tc.number: MechControllerIpcDeathListener_OnRemoteDied_001
 * @tc.desc  : Testing OnRemoteDied with DEVICE_ATTACH_CALLBACK type.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, MechControllerIpcDeathListener_OnRemoteDied_001, TestSize.Level1)
{
    MechControllerIpcDeathListener listener;
    listener.tokenId_ = 1001;
    listener.objectType_ = RemoteObjectType::DEVICE_ATTACH_CALLBACK;

    // 预置：确保callback map中有对应tokenId的条目
    MechBodyControllerService::GetInstance().deviceAttachCallback_[listener.tokenId_] = nullptr;

    wptr<IRemoteObject> object;
    listener.OnRemoteDied(object);

    // 验证：确认tokenId_对应的条目已被移除
    EXPECT_EQ(MechBodyControllerService::GetInstance().deviceAttachCallback_.find(listener.tokenId_),
              MechBodyControllerService::GetInstance().deviceAttachCallback_.end());
}

/**
 * @tc.name  : MechControllerIpcDeathListener_OnRemoteDied_002
 * @tc.number: MechControllerIpcDeathListener_OnRemoteDied_002
 * @tc.desc  : Testing OnRemoteDied with TRACKING_EVENT_CALLBACK type.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, MechControllerIpcDeathListener_OnRemoteDied_002, TestSize.Level1)
{
    MechControllerIpcDeathListener listener;
    listener.tokenId_ = 1002;
    listener.objectType_ = RemoteObjectType::TRACKING_EVENT_CALLBACK;

    // 预置：确保callback map中有对应tokenId的条目
    McCameraTrackingController::GetInstance().trackingEventCallback_[listener.tokenId_] = nullptr;

    wptr<IRemoteObject> object;
    listener.OnRemoteDied(object);

    // 验证：确认tokenId_对应的条目已被移除
    EXPECT_EQ(McCameraTrackingController::GetInstance().trackingEventCallback_.find(listener.tokenId_),
              McCameraTrackingController::GetInstance().trackingEventCallback_.end());
}

/**
 * @tc.name  : MechControllerIpcDeathListener_OnRemoteDied_003
 * @tc.number: MechControllerIpcDeathListener_OnRemoteDied_003
 * @tc.desc  : Testing OnRemoteDied with ROTATION_AXES_STATUS_CALLBACK type.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, MechControllerIpcDeathListener_OnRemoteDied_003, TestSize.Level1)
{
    MechControllerIpcDeathListener listener;
    listener.tokenId_ = 1003;
    listener.objectType_ = RemoteObjectType::ROTATION_AXES_STATUS_CALLBACK;

    // 预置：确保callback map中有对应tokenId的条目
    MechBodyControllerService::GetInstance().rotationAxesStatusChangeCallback_[listener.tokenId_] = nullptr;

    wptr<IRemoteObject> object;
    listener.OnRemoteDied(object);

    // 验证：确认tokenId_对应的条目已被移除
    EXPECT_EQ(MechBodyControllerService::GetInstance().rotationAxesStatusChangeCallback_.find(listener.tokenId_),
              MechBodyControllerService::GetInstance().rotationAxesStatusChangeCallback_.end());
}

/**
 * @tc.name  : MechControllerIpcDeathListener_OnRemoteDied_004
 * @tc.number: MechControllerIpcDeathListener_OnRemoteDied_004
 * @tc.desc  : Testing OnRemoteDied with COMMAND_CHANNEL type.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, MechControllerIpcDeathListener_OnRemoteDied_004, TestSize.Level1)
{
    MechControllerIpcDeathListener listener;
    listener.tokenId_ = 1004;
    listener.objectType_ = RemoteObjectType::COMMAND_CHANNEL;

    // 预置：确保cmdChannels map中有对应tokenId的条目
    MechBodyControllerService::GetInstance().cmdChannels_[listener.tokenId_] = nullptr;

    wptr<IRemoteObject> object;
    listener.OnRemoteDied(object);

    // 验证：确认tokenId_对应的条目已被移除
    EXPECT_EQ(MechBodyControllerService::GetInstance().cmdChannels_.find(listener.tokenId_),
              MechBodyControllerService::GetInstance().cmdChannels_.end());
}

/**
 * @tc.name  : OnMetadataInfo_003
 * @tc.number: OnMetadataInfo_003
 * @tc.desc  : Testing OnMetadataInfo with valid FOV metadata (ret == CAM_META_SUCCESS && item.count == 2),
 *             should set fovFromMetadata_ true and return ERR_OK.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, OnMetadataInfo_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest OnMetadataInfo_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();

    constexpr size_t itemCapacity = 10;
    constexpr size_t dataCapacity = 64;
    auto metadata = std::make_shared<OHOS::Camera::CameraMetadata>(itemCapacity, dataCapacity);
    ASSERT_NE(metadata, nullptr);
    ASSERT_NE(metadata->get(), nullptr);

    float fovData[2] = {90.0f, 60.0f};
    metadata->addEntry(OHOS_STATUS_FOV_INFOS, fovData, 2);

    int32_t ret = controller.OnMetadataInfo(metadata);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(controller.fovFromMetadata_);
    EXPECT_EQ(controller.currentCameraInfo_->fovH, static_cast<uint8_t>(fovData[0]));
    EXPECT_EQ(controller.currentCameraInfo_->fovV, static_cast<uint8_t>(fovData[1]));

    DTEST_LOG << "McCameraTrackingControllerTwoTest OnMetadataInfo_003 end" << std::endl;
}

/**
 * @tc.name  : OnMetadataInfo_004
 * @tc.number: OnMetadataInfo_004
 * @tc.desc  : Testing OnMetadataInfo with metadata that does not contain OHOS_STATUS_FOV_INFOS tag
 *             (ret != CAM_META_SUCCESS), should set fovFromMetadata_ false and return GET_FOV_INFO_TAG_FAILED.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, OnMetadataInfo_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest OnMetadataInfo_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();

    constexpr size_t itemCapacity = 10;
    constexpr size_t dataCapacity = 64;
    auto metadata = std::make_shared<OHOS::Camera::CameraMetadata>(itemCapacity, dataCapacity);
    ASSERT_NE(metadata, nullptr);
    ASSERT_NE(metadata->get(), nullptr);

    int32_t ret = controller.OnMetadataInfo(metadata);
    EXPECT_EQ(ret, GET_FOV_INFO_TAG_FAILED);
    EXPECT_FALSE(controller.fovFromMetadata_);

    DTEST_LOG << "McCameraTrackingControllerTwoTest OnMetadataInfo_004 end" << std::endl;
}

/**
 * @tc.name  : OnMetadataInfo_005
 * @tc.number: OnMetadataInfo_005
 * @tc.desc  : Testing OnMetadataInfo with FOV metadata that has wrong item count
 *             (ret == CAM_META_SUCCESS && item.count != 2), should set fovFromMetadata_ false
 *             and return GET_FOV_INFO_TAG_FAILED.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, OnMetadataInfo_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest OnMetadataInfo_005 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();
    controller.currentCameraInfo_ = std::make_shared<CameraInfo>();

    constexpr size_t itemCapacity = 10;
    constexpr size_t dataCapacity = 64;
    auto metadata = std::make_shared<OHOS::Camera::CameraMetadata>(itemCapacity, dataCapacity);
    ASSERT_NE(metadata, nullptr);
    ASSERT_NE(metadata->get(), nullptr);

    float fovData[3] = {90.0f, 60.0f, 30.0f};
    metadata->addEntry(OHOS_STATUS_FOV_INFOS, fovData, 3);

    int32_t ret = controller.OnMetadataInfo(metadata);
    EXPECT_EQ(ret, GET_FOV_INFO_TAG_FAILED);
    EXPECT_FALSE(controller.fovFromMetadata_);

    DTEST_LOG << "McCameraTrackingControllerTwoTest OnMetadataInfo_005 end" << std::endl;
}

/**
 * @tc.name  : GetTrackingTargetFallback_005
 * @tc.number: GetTrackingTargetFallback_005
 * @tc.desc  : Testing GetTrackingTargetFallback with matching boundingBox in third loop (condition true).
 */
HWTEST_F(McCameraTrackingControllerTwoTest, GetTrackingTargetFallback_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest GetTrackingTargetFallback_005 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    trackingRegion.topLeftX = 0.5;
    trackingRegion.topLeftY = 0.5;
    trackingRegion.width = 0.2;
    trackingRegion.height = 0.2;

    CameraStandard::Rect objectRect;
    objectRect.topLeftX = 0.5;
    objectRect.topLeftY = 0.5;
    objectRect.width = 0.3;
    objectRect.height = 0.3;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> mockObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE,
            timestamp, objectRect, 200, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects = {mockObject};

    sptr<CameraStandard::MetadataObject> targetObject;

    controller.lastTrackingFrame_ = nullptr;

    int32_t ret = controller.GetTrackingTargetFallback(trackingRegion, detectedObjects, targetObject);

    EXPECT_EQ(ret, ERR_OK);
    EXPECT_NE(targetObject, nullptr);
    EXPECT_EQ(targetObject->GetObjectId(), 200);
    EXPECT_FALSE(controller.isSalientDetectionLocked_);

    DTEST_LOG << "McCameraTrackingControllerTwoTest GetTrackingTargetFallback_005 end" << std::endl;
}

/**
 * @tc.name  : GetTrackingTargetFallback_006
 * @tc.number: GetTrackingTargetFallback_006
 * @tc.desc  : Testing GetTrackingTargetFallback with no matching boundingBox in third loop (condition false).
 */
HWTEST_F(McCameraTrackingControllerTwoTest, GetTrackingTargetFallback_006, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest GetTrackingTargetFallback_006 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    trackingRegion.topLeftX = 0.1;
    trackingRegion.topLeftY = 0.1;
    trackingRegion.width = 0.2;
    trackingRegion.height = 0.2;

    CameraStandard::Rect objectRect;
    objectRect.topLeftX = 0.9;
    objectRect.topLeftY = 0.9;
    objectRect.width = 0.3;
    objectRect.height = 0.3;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> mockObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE,
            timestamp, objectRect, 300, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects = {mockObject};

    sptr<CameraStandard::MetadataObject> targetObject;

    controller.lastTrackingFrame_ = nullptr;

    int32_t ret = controller.GetTrackingTargetFallback(trackingRegion, detectedObjects, targetObject);

    EXPECT_EQ(ret, ERR_OK);
    EXPECT_NE(targetObject, nullptr);
    EXPECT_EQ(targetObject->GetObjectId(), 300);
    EXPECT_FALSE(controller.isSalientDetectionLocked_);

    DTEST_LOG << "McCameraTrackingControllerTwoTest GetTrackingTargetFallback_006 end" << std::endl;
}

/**
 * @tc.name  : SelectBestObjectByRegion_001
 * @tc.number: SelectBestObjectByRegion_001
 * @tc.desc  : Testing SelectBestObjectByRegion with empty detectedObjects, returns nullptr.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SelectBestObjectByRegion_001, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_001 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    trackingRegion.topLeftX = 0.1f;
    trackingRegion.topLeftY = 0.1f;
    trackingRegion.width = 0.5f;
    trackingRegion.height = 0.5f;

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects;

    sptr<CameraStandard::MetadataObject> result = controller.SelectBestObjectByRegion(trackingRegion, detectedObjects);

    EXPECT_EQ(result, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_001 end" << std::endl;
}

/**
 * @tc.name  : SelectBestObjectByRegion_002
 * @tc.number: SelectBestObjectByRegion_002
 * @tc.desc  : Testing SelectBestObjectByRegion with single FACE object and IOU > threshold.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SelectBestObjectByRegion_002, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_002 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    trackingRegion.topLeftX = 0.1f;
    trackingRegion.topLeftY = 0.1f;
    trackingRegion.width = 0.5f;
    trackingRegion.height = 0.5f;

    CameraStandard::Rect objectRect;
    objectRect.topLeftX = 0.2f;
    objectRect.topLeftY = 0.2f;
    objectRect.width = 0.6f;
    objectRect.height = 0.6f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> mockObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE,
            timestamp, objectRect, 100, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects = {mockObject};

    sptr<CameraStandard::MetadataObject> result = controller.SelectBestObjectByRegion(trackingRegion, detectedObjects);

    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetObjectId(), 100);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_002 end" << std::endl;
}

/**
 * @tc.name  : SelectBestObjectByRegion_003
 * @tc.number: SelectBestObjectByRegion_003
 * @tc.desc  : Testing SelectBestObjectByRegion with single BASE_FACE_DETECTION object and IOU > threshold.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SelectBestObjectByRegion_003, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_003 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    trackingRegion.topLeftX = 0.1f;
    trackingRegion.topLeftY = 0.1f;
    trackingRegion.width = 0.5f;
    trackingRegion.height = 0.5f;

    CameraStandard::Rect objectRect;
    objectRect.topLeftX = 0.2f;
    objectRect.topLeftY = 0.2f;
    objectRect.width = 0.6f;
    objectRect.height = 0.6f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> mockObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::BASE_FACE_DETECTION,
            timestamp, objectRect, 200, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects = {mockObject};

    sptr<CameraStandard::MetadataObject> result = controller.SelectBestObjectByRegion(trackingRegion, detectedObjects);

    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetObjectId(), 200);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_003 end" << std::endl;
}

/**
 * @tc.name  : SelectBestObjectByRegion_004
 * @tc.number: SelectBestObjectByRegion_004
 * @tc.desc  : Testing SelectBestObjectByRegion with single HUMAN_HEAD object and IOU > threshold.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SelectBestObjectByRegion_004, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_004 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    trackingRegion.topLeftX = 0.1f;
    trackingRegion.topLeftY = 0.1f;
    trackingRegion.width = 0.5f;
    trackingRegion.height = 0.5f;

    CameraStandard::Rect objectRect;
    objectRect.topLeftX = 0.2f;
    objectRect.topLeftY = 0.2f;
    objectRect.width = 0.6f;
    objectRect.height = 0.6f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> mockObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::HUMAN_HEAD,
            timestamp, objectRect, 300, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects = {mockObject};

    sptr<CameraStandard::MetadataObject> result = controller.SelectBestObjectByRegion(trackingRegion, detectedObjects);

    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetObjectId(), 300);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_004 end" << std::endl;
}

/**
 * @tc.name  : SelectBestObjectByRegion_005
 * @tc.number: SelectBestObjectByRegion_005
 * @tc.desc  : Testing SelectBestObjectByRegion with unsupported object type (HUMAN_BODY), returns nullptr.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SelectBestObjectByRegion_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_005 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    trackingRegion.topLeftX = 0.1f;
    trackingRegion.topLeftY = 0.1f;
    trackingRegion.width = 0.5f;
    trackingRegion.height = 0.5f;

    CameraStandard::Rect objectRect;
    objectRect.topLeftX = 0.2f;
    objectRect.topLeftY = 0.2f;
    objectRect.width = 0.6f;
    objectRect.height = 0.6f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> mockObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::HUMAN_BODY,
            timestamp, objectRect, 400, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects = {mockObject};

    sptr<CameraStandard::MetadataObject> result = controller.SelectBestObjectByRegion(trackingRegion, detectedObjects);

    EXPECT_EQ(result, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_005 end" << std::endl;
}

/**
 * @tc.name  : SelectBestObjectByRegion_006
 * @tc.number: SelectBestObjectByRegion_006
 * @tc.desc  : Testing SelectBestObjectByRegion with multiple objects, second has higher IOU.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SelectBestObjectByRegion_006, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_006 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    trackingRegion.topLeftX = 0.1f;
    trackingRegion.topLeftY = 0.1f;
    trackingRegion.width = 0.5f;
    trackingRegion.height = 0.5f;

    CameraStandard::Rect rect1;
    rect1.topLeftX = 0.3f;
    rect1.topLeftY = 0.3f;
    rect1.width = 0.6f;
    rect1.height = 0.6f;

    CameraStandard::Rect rect2;
    rect2.topLeftX = 0.15f;
    rect2.topLeftY = 0.15f;
    rect2.width = 0.55f;
    rect2.height = 0.55f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> obj1 =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE,
            timestamp, rect1, 501, 0);
    sptr<CameraStandard::MetadataObject> obj2 =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE,
            timestamp, rect2, 502, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects = {obj1, obj2};

    sptr<CameraStandard::MetadataObject> result = controller.SelectBestObjectByRegion(trackingRegion, detectedObjects);

    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetObjectId(), 502);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_006 end" << std::endl;
}

/**
 * @tc.name  : SelectBestObjectByRegion_007
 * @tc.number: SelectBestObjectByRegion_007
 * @tc.desc  : Testing SelectBestObjectByRegion with multiple objects, second has lower IOU.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SelectBestObjectByRegion_007, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_007 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    trackingRegion.topLeftX = 0.1f;
    trackingRegion.topLeftY = 0.1f;
    trackingRegion.width = 0.5f;
    trackingRegion.height = 0.5f;

    CameraStandard::Rect rect1;
    rect1.topLeftX = 0.15f;
    rect1.topLeftY = 0.15f;
    rect1.width = 0.55f;
    rect1.height = 0.55f;

    CameraStandard::Rect rect2;
    rect2.topLeftX = 0.3f;
    rect2.topLeftY = 0.3f;
    rect2.width = 0.6f;
    rect2.height = 0.6f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> obj1 =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE,
            timestamp, rect1, 601, 0);
    sptr<CameraStandard::MetadataObject> obj2 =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE,
            timestamp, rect2, 602, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects = {obj1, obj2};

    sptr<CameraStandard::MetadataObject> result = controller.SelectBestObjectByRegion(trackingRegion, detectedObjects);

    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetObjectId(), 601);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_007 end" << std::endl;
}

/**
 * @tc.name  : SelectBestObjectByRegion_008
 * @tc.number: SelectBestObjectByRegion_008
 * @tc.desc  : Testing SelectBestObjectByRegion with object IOU below threshold, returns nullptr.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, SelectBestObjectByRegion_008, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_008 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect trackingRegion;
    trackingRegion.topLeftX = 0.0f;
    trackingRegion.topLeftY = 0.0f;
    trackingRegion.width = 0.5f;
    trackingRegion.height = 0.5f;

    CameraStandard::Rect objectRect;
    objectRect.topLeftX = 0.48f;
    objectRect.topLeftY = 0.48f;
    objectRect.width = 0.98f;
    objectRect.height = 0.98f;

    int64_t timestamp = 0;
    sptr<CameraStandard::MetadataObject> mockObject =
        new CameraStandard::MetadataObject(CameraStandard::MetadataObjectType::FACE,
            timestamp, objectRect, 700, 0);

    std::vector<sptr<CameraStandard::MetadataObject>> detectedObjects = {mockObject};

    sptr<CameraStandard::MetadataObject> result = controller.SelectBestObjectByRegion(trackingRegion, detectedObjects);

    EXPECT_EQ(result, nullptr);

    DTEST_LOG << "McCameraTrackingControllerTwoTest SelectBestObjectByRegion_008 end" << std::endl;
}

/**
 * @tc.name  : CalculateIOU_005
 * @tc.number: CalculateIOU_005
 * @tc.desc  : Testing CalculateIOU with rect1.height <= 0, returns IOU_DEFAULT.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, CalculateIOU_005, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateIOU_005 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect rect1;
    rect1.topLeftX = 0.1f;
    rect1.topLeftY = 0.1f;
    rect1.width = 0.5f;
    rect1.height = 0.0f;

    CameraStandard::Rect rect2;
    rect2.topLeftX = 0.2f;
    rect2.topLeftY = 0.2f;
    rect2.width = 0.6f;
    rect2.height = 0.6f;

    float iou = controller.CalculateIOU(rect1, rect2);

    EXPECT_EQ(iou, 0.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateIOU_005 end" << std::endl;
}

/**
 * @tc.name  : CalculateIOU_006
 * @tc.number: CalculateIOU_006
 * @tc.desc  : Testing CalculateIOU with rect2.width <= 0, returns IOU_DEFAULT.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, CalculateIOU_006, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateIOU_006 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect rect1;
    rect1.topLeftX = 0.1f;
    rect1.topLeftY = 0.1f;
    rect1.width = 0.5f;
    rect1.height = 0.5f;

    CameraStandard::Rect rect2;
    rect2.topLeftX = 0.2f;
    rect2.topLeftY = 0.2f;
    rect2.width = 0.0f;
    rect2.height = 0.6f;

    float iou = controller.CalculateIOU(rect1, rect2);

    EXPECT_EQ(iou, 0.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateIOU_006 end" << std::endl;
}

/**
 * @tc.name  : CalculateIOU_007
 * @tc.number: CalculateIOU_007
 * @tc.desc  : Testing CalculateIOU with rect2.height <= 0, returns IOU_DEFAULT.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, CalculateIOU_007, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateIOU_007 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect rect1;
    rect1.topLeftX = 0.1f;
    rect1.topLeftY = 0.1f;
    rect1.width = 0.5f;
    rect1.height = 0.5f;

    CameraStandard::Rect rect2;
    rect2.topLeftX = 0.2f;
    rect2.topLeftY = 0.2f;
    rect2.width = 0.6f;
    rect2.height = 0.0f;

    float iou = controller.CalculateIOU(rect1, rect2);

    EXPECT_EQ(iou, 0.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateIOU_007 end" << std::endl;
}

/**
 * @tc.name  : CalculateIOU_008
 * @tc.number: CalculateIOU_008
 * @tc.desc  : Testing CalculateIOU with inverted rect1 (right <= left), returns IOU_DEFAULT.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, CalculateIOU_008, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateIOU_008 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect rect1;
    rect1.topLeftX = 0.5f;
    rect1.topLeftY = 0.1f;
    rect1.width = 0.3f;
    rect1.height = 0.5f;

    CameraStandard::Rect rect2;
    rect2.topLeftX = 0.1f;
    rect2.topLeftY = 0.1f;
    rect2.width = 0.6f;
    rect2.height = 0.6f;

    float iou = controller.CalculateIOU(rect1, rect2);

    EXPECT_EQ(iou, 0.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateIOU_008 end" << std::endl;
}

/**
 * @tc.name  : CalculateIOU_009
 * @tc.number: CalculateIOU_009
 * @tc.desc  : Testing CalculateIOU with inverted rect2 (bottom <= top), returns IOU_DEFAULT.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, CalculateIOU_009, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateIOU_009 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect rect1;
    rect1.topLeftX = 0.1f;
    rect1.topLeftY = 0.1f;
    rect1.width = 0.6f;
    rect1.height = 0.6f;

    CameraStandard::Rect rect2;
    rect2.topLeftX = 0.2f;
    rect2.topLeftY = 0.5f;
    rect2.width = 0.6f;
    rect2.height = 0.3f;

    float iou = controller.CalculateIOU(rect1, rect2);

    EXPECT_EQ(iou, 0.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateIOU_009 end" << std::endl;
}

/**
 * @tc.name  : CalculateIOU_010
 * @tc.number: CalculateIOU_010
 * @tc.desc  : Testing CalculateIOU with tiny area where minArea <= TRACKING_LOST_CHECK, returns IOU_DEFAULT.
 */
HWTEST_F(McCameraTrackingControllerTwoTest, CalculateIOU_010, TestSize.Level1)
{
    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateIOU_010 begin" << std::endl;

    McCameraTrackingController& controller = McCameraTrackingController::GetInstance();

    CameraStandard::Rect rect1;
    rect1.topLeftX = 0.0f;
    rect1.topLeftY = 0.0f;
    rect1.width = 0.01f;
    rect1.height = 0.01f;

    CameraStandard::Rect rect2;
    rect2.topLeftX = 0.0f;
    rect2.topLeftY = 0.0f;
    rect2.width = 0.01f;
    rect2.height = 0.01f;

    float iou = controller.CalculateIOU(rect1, rect2);

    EXPECT_EQ(iou, 0.0f);

    DTEST_LOG << "McCameraTrackingControllerTwoTest CalculateIOU_010 end" << std::endl;
}
}
}