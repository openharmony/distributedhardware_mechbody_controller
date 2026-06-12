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

#include "mc_swing_detect_event_test.h"
#include "../test_log.h"
#include "mechbody_controller_service.h"
#include <cmath>

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS {
namespace MechBodyController {

void McSwingDetectEventTest::SetUpTestCase()
{
    DTEST_LOG << "McSwingDetectEventTest::SetUpTestCase" << std::endl;
}

void McSwingDetectEventTest::TearDownTestCase()
{
    DTEST_LOG << "McSwingDetectEventTest::TearDownTestCase" << std::endl;
}

void McSwingDetectEventTest::TearDown()
{
    DTEST_LOG << "McSwingDetectEventTest::TearDown" << std::endl;
}

void McSwingDetectEventTest::SetUp()
{
    DTEST_LOG << "McSwingDetectEventTest::SetUp" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, GetInstance_001, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest GetInstance_001 begin" << std::endl;

    MechSwingController& instance1 = MechSwingController::GetInstance();
    MechSwingController& instance2 = MechSwingController::GetInstance();
    EXPECT_EQ(&instance1, &instance2);

    DTEST_LOG << "McSwingDetectEventTest GetInstance_001 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, ParseEventData_001, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest ParseEventData_001 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    std::string testData = "eventId:1;state:2;fov:70.0;face:[[100,200,300,400]];body:[[50,60,70,80]]";
    
    EventData result = controller.ParseEventData(testData);
    EXPECT_EQ(result.eventId, 1);
    EXPECT_EQ(result.state, 2);
    EXPECT_EQ(result.fov, 70);
    EXPECT_EQ(result.face.size(), 1);
    EXPECT_EQ(result.face[0].size(), 4);
    EXPECT_EQ(result.face[0][0], 100);
    EXPECT_EQ(result.face[0][1], 200);
    EXPECT_EQ(result.face[0][2], 300);
    EXPECT_EQ(result.face[0][3], 400);

    DTEST_LOG << "McSwingDetectEventTest ParseEventData_001 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, ParseEventData_002, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest ParseEventData_002 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    std::string testData = "eventId:10;state:5;fov:80.5";
    
    EventData result = controller.ParseEventData(testData);
    EXPECT_EQ(result.eventId, 10);
    EXPECT_EQ(result.state, 5);
    EXPECT_EQ(result.fov, 80);
    EXPECT_EQ(result.face.size(), 0);
    EXPECT_EQ(result.body.size(), 0);

    DTEST_LOG << "McSwingDetectEventTest ParseEventData_002 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, ParseEventData_003, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest ParseEventData_003 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    std::string testData = "";
    
    EventData result = controller.ParseEventData(testData);
    EXPECT_EQ(result.eventId, 0);
    EXPECT_EQ(result.state, 0);
    EXPECT_EQ(result.fov, 0);
    EXPECT_EQ(result.face.size(), 0);

    DTEST_LOG << "McSwingDetectEventTest ParseEventData_003 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, ParseEventData_004, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest ParseEventData_004 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    std::string testData = "invalid_data_without_format";
    
    EventData result = controller.ParseEventData(testData);
    EXPECT_EQ(result.eventId, 0);
    EXPECT_EQ(result.state, 0);

    DTEST_LOG << "McSwingDetectEventTest ParseEventData_004 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, ParseNestedList_001, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest ParseNestedList_001 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    std::string testData = "[[100,200,300,400],[50,60,70,80]]";
    
    std::vector<std::vector<int32_t>> result = controller.ParseNestedList(testData);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0].size(), 4);
    EXPECT_EQ(result[0][0], 100);
    EXPECT_EQ(result[0][1], 200);
    EXPECT_EQ(result[1].size(), 4);
    EXPECT_EQ(result[1][0], 50);

    DTEST_LOG << "McSwingDetectEventTest ParseNestedList_001 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, ParseNestedList_002, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest ParseNestedList_002 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    std::string testData = "[[1, 2, 3, 4]]";
    
    std::vector<std::vector<int32_t>> result = controller.ParseNestedList(testData);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].size(), 4);
    EXPECT_EQ(result[0][0], 1);
    EXPECT_EQ(result[0][1], 2);
    EXPECT_EQ(result[0][2], 3);
    EXPECT_EQ(result[0][3], 4);

    DTEST_LOG << "McSwingDetectEventTest ParseNestedList_002 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, ParseNestedList_003, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest ParseNestedList_003 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    std::string testData = "no_brackets_here";
    
    std::vector<std::vector<int32_t>> result = controller.ParseNestedList(testData);
    EXPECT_EQ(result.size(), 0);

    DTEST_LOG << "McSwingDetectEventTest ParseNestedList_003 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, CalculateFOV_4to3_001, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest CalculateFOV_4to3_001 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    int32_t fov = 70;
    
    auto [fovh, fovv] = controller.calculateFOV_4to3(fov);
    
    EXPECT_GT(fovh, fov);
    EXPECT_EQ(fovv, static_cast<float>(fov));

    float fovRad = static_cast<float>(fov) * M_PI / 180.0f;
    float expectedFovh = 2.0f * atan(tanf(fovRad * 0.5f) * (4.0f / 3.0f)) * 180.0f / M_PI;
    EXPECT_NEAR(fovh, expectedFovh, 0.1f);

    DTEST_LOG << "McSwingDetectEventTest CalculateFOV_4to3_001 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, CalculateFOV_4to3_002, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest CalculateFOV_4to3_002 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    int32_t fov = 90;
    
    auto [fovh, fovv] = controller.calculateFOV_4to3(fov);
    
    EXPECT_GT(fovh, fov);
    EXPECT_EQ(fovv, static_cast<float>(fov));

    DTEST_LOG << "McSwingDetectEventTest CalculateFOV_4to3_002 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, CalculateFOV_4to3_003, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest CalculateFOV_4to3_003 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    int32_t fov = 0;
    
    auto [fovh, fovv] = controller.calculateFOV_4to3(fov);
    
    EXPECT_NEAR(fovh, 0.0f, 0.1f);
    EXPECT_NEAR(fovv, 0.0f, 0.1f);

    DTEST_LOG << "McSwingDetectEventTest CalculateFOV_4to3_003 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, AdjustROI_001, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest AdjustROI_001 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    ROI roi;
    roi.x = 0.5f;
    roi.y = 0.5f;
    roi.width = 0.2f;
    roi.height = 0.2f;

    controller.AdjustROI(roi, MobileRotation::UP);
    EXPECT_NEAR(roi.x, 0.5f, 0.01f);
    EXPECT_NEAR(roi.y, 0.5f, 0.01f);

    DTEST_LOG << "McSwingDetectEventTest AdjustROI_001 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, AdjustROI_002, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest AdjustROI_002 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    ROI roi;
    roi.x = 0.3f;
    roi.y = 0.7f;
    roi.width = 0.2f;
    roi.height = 0.2f;

    controller.AdjustROI(roi, MobileRotation::DOWN);
    EXPECT_NEAR(roi.x, 0.3f, 0.01f);
    EXPECT_NEAR(roi.y, 0.7f, 0.01f);

    DTEST_LOG << "McSwingDetectEventTest AdjustROI_002 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, AdjustROI_003, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest AdjustROI_003 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    ROI roi;
    roi.x = 0.5f;
    roi.y = 0.5f;
    roi.width = 0.1f;
    roi.height = 0.1f;

    controller.AdjustROI(roi, MobileRotation::LEFT);
    EXPECT_NEAR(roi.x, 0.5f, 0.01f);
    EXPECT_NEAR(roi.y, 0.5f, 0.01f);

    DTEST_LOG << "McSwingDetectEventTest AdjustROI_003 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, AdjustROI_004, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest AdjustROI_004 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    ROI roi;
    roi.x = 0.4f;
    roi.y = 0.6f;
    roi.width = 0.3f;
    roi.height = 0.3f;

    controller.AdjustROI(roi, MobileRotation::RIGHT);
    EXPECT_NEAR(roi.x, 0.4f, 0.01f);
    EXPECT_NEAR(roi.y, 0.6f, 0.01f);

    DTEST_LOG << "McSwingDetectEventTest AdjustROI_004 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, HandleSwingEvent_001, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest HandleSwingEvent_001 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    std::string testData = "eventId:1;state:2;fov:70.0;face:[[100,200,300,400]]";
    
    bool result = controller.HandleSwingEvent(testData);
    EXPECT_TRUE(result);

    DTEST_LOG << "McSwingDetectEventTest HandleSwingEvent_001 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, HandleSwingEvent_002, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest HandleSwingEvent_002 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    std::string testData = "eventId:1;state:2;fov:70.0;face:[[]]";
    
    bool result = controller.HandleSwingEvent(testData);
    EXPECT_TRUE(result);

    DTEST_LOG << "McSwingDetectEventTest HandleSwingEvent_002 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, HandleSwingEvent_003, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest HandleSwingEvent_003 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    std::string testData = "eventId:1;state:2;fov:70.0";
    
    bool result = controller.HandleSwingEvent(testData);
    EXPECT_TRUE(result);

    DTEST_LOG << "McSwingDetectEventTest HandleSwingEvent_003 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, HandleSwingEvent_004, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest HandleSwingEvent_004 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    std::string testData = "";
    
    bool result = controller.HandleSwingEvent(testData);
    EXPECT_FALSE(result);

    DTEST_LOG << "McSwingDetectEventTest HandleSwingEvent_004 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, EventDataToString_001, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest EventDataToString_001 begin" << std::endl;

    EventData data;
    data.eventId = 1;
    data.state = 2;
    data.fov = 70;
    data.face = {{100, 200, 300, 400}};
    data.body = {{50, 60, 70, 80}};
    
    std::string result = data.ToString();
    EXPECT_TRUE(result.find("eventId=1") != std::string::npos);
    EXPECT_TRUE(result.find("state=2") != std::string::npos);
    EXPECT_TRUE(result.find("fov=70") != std::string::npos);

    DTEST_LOG << "McSwingDetectEventTest EventDataToString_001 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, EventDataToString_002, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest EventDataToString_002 begin" << std::endl;

    EventData data;
    data.eventId = 0;
    data.state = 0;
    data.fov = 0;
    data.face = {};
    data.body = {};
    
    std::string result = data.ToString();
    EXPECT_TRUE(result.find("eventId=0") != std::string::npos);
    EXPECT_TRUE(result.find("face=[]") != std::string::npos);
    EXPECT_TRUE(result.find("body=[]") != std::string::npos);

    DTEST_LOG << "McSwingDetectEventTest EventDataToString_002 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, RegisterSwingEvent_001, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest RegisterSwingEvent_001 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    bool result = controller.RegisterSwingEvent();
    EXPECT_TRUE(result);

    DTEST_LOG << "McSwingDetectEventTest RegisterSwingEvent_001 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, UnregisterSwingEvent_001, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest UnregisterSwingEvent_001 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    bool result = controller.UnregisterSwingEvent();
    EXPECT_TRUE(result);

    DTEST_LOG << "McSwingDetectEventTest UnregisterSwingEvent_001 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, RegisterUnregisterSwingEvent_001, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest RegisterUnregisterSwingEvent_001 begin" << std::endl;

    MechSwingController& controller = MechSwingController::GetInstance();
    bool registerResult = controller.RegisterSwingEvent();
    EXPECT_TRUE(registerResult);
    bool unregisterResult = controller.UnregisterSwingEvent();
    EXPECT_TRUE(unregisterResult);

    DTEST_LOG << "McSwingDetectEventTest RegisterUnregisterSwingEvent_001 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, MechRegisterSwingEvent_001, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest MechRegisterSwingEvent_001 begin" << std::endl;

    bool result = MechRegisterSwingEvent();
    EXPECT_TRUE(result);

    DTEST_LOG << "McSwingDetectEventTest MechRegisterSwingEvent_001 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, MechUnregisterSwingEvent_001, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest MechUnregisterSwingEvent_001 begin" << std::endl;

    bool result = MechUnregisterSwingEvent();
    EXPECT_TRUE(result);

    DTEST_LOG << "McSwingDetectEventTest MechUnregisterSwingEvent_001 end" << std::endl;
}

HWTEST_F(McSwingDetectEventTest, SwingEventCallbackOnSwingEvent_001, TestSize.Level1)
{
    DTEST_LOG << "McSwingDetectEventTest SwingEventCallbackOnSwingEvent_001 begin" << std::endl;

    SwingEventCallback callback;
    std::string testData = "eventId:1;state:2;fov:70.0;face:[[100,200,300,400]]";
    callback.onSwingEvent(testData);
    EXPECT_TRUE(callback.IsEventProcessed());

    DTEST_LOG << "McSwingDetectEventTest SwingEventCallbackOnSwingEvent_001 end" << std::endl;
}

} // namespace MechBodyController
} // namespace OHOS