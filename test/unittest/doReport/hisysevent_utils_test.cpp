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

#include "hisysevent_utils_test.h"
#include "mechbody_controller_log.h"

#include "../test_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {

void HisyseventUtilsTest::SetUpTestCase()
{
    DTEST_LOG << "HisyseventUtilsTest::SetUpTestCase" << std::endl;
}

void HisyseventUtilsTest::TearDownTestCase()
{
    DTEST_LOG << "HisyseventUtilsTest::TearDownTestCase" << std::endl;
}

void HisyseventUtilsTest::SetUp()
{
    DTEST_LOG << "HisyseventUtilsTest::SetUp" << std::endl;
}

void HisyseventUtilsTest::TearDown()
{
    DTEST_LOG << "HisyseventUtilsTest::TearDown" << std::endl;
}

/**
 * @tc.name: DotReportMechKitStartEvent_001
 * @tc.desc: 测试上报 MechKit 启动事件 - 成功场景
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DotReportMechKitStartEvent_001, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DotReportMechKitStartEvent_001 begin" << std::endl;

    // Given: 准备 MechKit 启动报告信息
    MechKitStartReportInfo reportInfo;
    reportInfo.connectType = ConnectType::BLUETOOTH;
    reportInfo.deviceType = MechType::PORTABLE_GIMBAL;
    reportInfo.deviceName = "TestDevice";
    reportInfo.deviceModel = "TestModel";
    reportInfo.mechkitStartResultCode = MechKitStartResCode::SUCCESS;

    // When: 调用上报事件函数
    HisyseventUtils::DotReportMechKitStartEvent(reportInfo);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DotReportMechKitStartEvent_001 end" << std::endl;
}

/**
 * @tc.name: DotReportMechKitStartEvent_002
 * @tc.desc: 测试上报 MechKit 启动事件 - 未知连接类型
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DotReportMechKitStartEvent_002, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DotReportMechKitStartEvent_002 begin" << std::endl;

    // Given: 准备 MechKit 启动报告信息，使用未知连接类型
    MechKitStartReportInfo reportInfo;
    reportInfo.connectType = ConnectType::UNKNOWN;
    reportInfo.deviceType = MechType::PORTABLE_GIMBAL;
    reportInfo.deviceName = "UnknownDevice";
    reportInfo.deviceModel = "UnknownModel";
    reportInfo.mechkitStartResultCode = MechKitStartResCode::GATT_CONNECT_FAIL;

    // When: 调用上报事件函数
    HisyseventUtils::DotReportMechKitStartEvent(reportInfo);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DotReportMechKitStartEvent_002 end" << std::endl;
}

/**
 * @tc.name: DotReportMechKitStartEvent_003
 * @tc.desc: 测试上报 MechKit 启动事件 - NFC 连接类型
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DotReportMechKitStartEvent_003, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DotReportMechKitStartEvent_003 begin" << std::endl;

    // Given: 准备 MechKit 启动报告信息，使用 NFC 连接类型
    MechKitStartReportInfo reportInfo;
    reportInfo.connectType = ConnectType::NFC;
    reportInfo.deviceType = MechType::PORTABLE_GIMBAL;
    reportInfo.deviceName = "NFCDevice";
    reportInfo.deviceModel = "NFCModel";
    reportInfo.mechkitStartResultCode = MechKitStartResCode::SUCCESS;

    // When: 调用上报事件函数
    HisyseventUtils::DotReportMechKitStartEvent(reportInfo);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DotReportMechKitStartEvent_003 end" << std::endl;
}

/**
 * @tc.name: DotReportMechKitStartEvent_004
 * @tc.desc: 测试上报 MechKit 启动事件 - 空设备名称和型号
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DotReportMechKitStartEvent_004, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DotReportMechKitStartEvent_004 begin" << std::endl;

    // Given: 准备 MechKit 启动报告信息，使用空字符串
    MechKitStartReportInfo reportInfo;
    reportInfo.connectType = ConnectType::BLUETOOTH;
    reportInfo.deviceType = MechType::PORTABLE_GIMBAL;
    reportInfo.deviceName = "";
    reportInfo.deviceModel = "";
    reportInfo.mechkitStartResultCode = MechKitStartResCode::SUCCESS;

    // When: 调用上报事件函数
    HisyseventUtils::DotReportMechKitStartEvent(reportInfo);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DotReportMechKitStartEvent_004 end" << std::endl;
}

/**
 * @tc.name: DotReportFocustrackingStartEvent_001
 * @tc.desc: 测试上报焦点跟踪启动事件 - 正常应用名称
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DotReportFocustrackingStartEvent_001, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DotReportFocustrackingStartEvent_001 begin" << std::endl;

    // Given: 准备应用名称
    std::string appName = "TestApp";

    // When: 调用上报事件函数
    HisyseventUtils::DotReportFocustrackingStartEvent(appName);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DotReportFocustrackingStartEvent_001 end" << std::endl;
}

/**
 * @tc.name: DotReportFocustrackingStartEvent_002
 * @tc.desc: 测试上报焦点跟踪启动事件 - 空应用名称
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DotReportFocustrackingStartEvent_002, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DotReportFocustrackingStartEvent_002 begin" << std::endl;

    // Given: 使用空字符串作为应用名称
    std::string appName = "";

    // When: 调用上报事件函数
    HisyseventUtils::DotReportFocustrackingStartEvent(appName);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DotReportFocustrackingStartEvent_002 end" << std::endl;
}

/**
 * @tc.name: DotReportFocustrackingStartEvent_003
 * @tc.desc: 测试上报焦点跟踪启动事件 - 系统相机应用
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DotReportFocustrackingStartEvent_003, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DotReportFocustrackingStartEvent_003 begin" << std::endl;

    // Given: 使用系统相机应用名称
    std::string appName = "com.ohos.camera";

    // When: 调用上报事件函数
    HisyseventUtils::DotReportFocustrackingStartEvent(appName);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DotReportFocustrackingStartEvent_003 end" << std::endl;
}

/**
 * @tc.name: DotReportFocustrackingStartEvent_004
 * @tc.desc: 测试上报焦点跟踪启动事件 - 长应用名称
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DotReportFocustrackingStartEvent_004, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DotReportFocustrackingStartEvent_004 begin" << std::endl;

    // Given: 使用较长的应用名称
    std::string appName = "com.example.very.long.application.name.for.testing.purposes";

    // When: 调用上报事件函数
    HisyseventUtils::DotReportFocustrackingStartEvent(appName);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DotReportFocustrackingStartEvent_004 end" << std::endl;
}

/**
 * @tc.name: DoReportMechkitControlStatisticEvent_001
 * @tc.desc: 测试上报机械控制统计事件 - 正常数据
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DoReportMechkitControlStatisticEvent_001, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_001 begin" << std::endl;

    // Given: 准备正常的控制信息
    MechkitControlInfo controlInfo;
    controlInfo.upWard = 10;
    controlInfo.downWard = 5;
    controlInfo.turnLeft = 15;
    controlInfo.turnRight = 20;
    controlInfo.fastRotation = 3;
    controlInfo.landScapePortrait = 1;
    controlInfo.cameraMode = 2;
    controlInfo.trackingDuration = 1000;
    controlInfo.cameraType = 1;
    controlInfo.zoomRatio = 1.5f;
    controlInfo.packageName = "com.example.test";
    controlInfo.stickNum = 2;
    controlInfo.successNumber = 8;
    controlInfo.failNumber = 1;
    controlInfo.obstacleNumber = 3;
    controlInfo.cliffNumber = 0;
    controlInfo.deviceType = 1;
    controlInfo.appName = {"TestApp1", "TestApp2"};
    controlInfo.existenceTime = {5000, 3000};

    // When: 调用上报统计事件函数
    HisyseventUtils::DoReportMechkitControlStatisticEvent(controlInfo);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_001 end" << std::endl;
}

/**
 * @tc.name: DoReportMechkitControlStatisticEvent_002
 * @tc.desc: 测试上报机械控制统计事件 - 零值边界
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DoReportMechkitControlStatisticEvent_002, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_002 begin" << std::endl;

    // Given: 准备全零值的控制信息
    MechkitControlInfo controlInfo;
    controlInfo.upWard = 0;
    controlInfo.downWard = 0;
    controlInfo.turnLeft = 0;
    controlInfo.turnRight = 0;
    controlInfo.fastRotation = 0;
    controlInfo.landScapePortrait = 0;
    controlInfo.cameraMode = 0;
    controlInfo.trackingDuration = 0;
    controlInfo.cameraType = 0;
    controlInfo.zoomRatio = 0.0f;
    controlInfo.packageName = "";
    controlInfo.stickNum = 0;
    controlInfo.successNumber = 0;
    controlInfo.failNumber = 0;
    controlInfo.obstacleNumber = 0;
    controlInfo.cliffNumber = 0;
    controlInfo.deviceType = 0;
    controlInfo.appName = {};
    controlInfo.existenceTime = {};

    // When: 调用上报统计事件函数
    HisyseventUtils::DoReportMechkitControlStatisticEvent(controlInfo);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_002 end" << std::endl;
}

/**
 * @tc.name: DoReportMechkitControlStatisticEvent_003
 * @tc.desc: 测试上报机械控制统计事件 - 极大值
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DoReportMechkitControlStatisticEvent_003, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_003 begin" << std::endl;

    // Given: 准备极大值的控制信息
    MechkitControlInfo controlInfo;
    controlInfo.upWard = UINT32_MAX;
    controlInfo.downWard = UINT32_MAX;
    controlInfo.turnLeft = UINT32_MAX;
    controlInfo.turnRight = UINT32_MAX;
    controlInfo.fastRotation = UINT32_MAX;
    controlInfo.landScapePortrait = UINT32_MAX;
    controlInfo.cameraMode = UINT8_MAX;
    controlInfo.trackingDuration = UINT64_MAX;
    controlInfo.cameraType = UINT8_MAX;
    controlInfo.zoomRatio = FLT_MAX;
    controlInfo.packageName = "com.example.very.long.package.name.for.testing.extreme.values";
    controlInfo.stickNum = UINT8_MAX;
    controlInfo.successNumber = UINT32_MAX;
    controlInfo.failNumber = UINT32_MAX;
    controlInfo.obstacleNumber = UINT8_MAX;
    controlInfo.cliffNumber = UINT8_MAX;
    controlInfo.deviceType = UINT8_MAX;
    controlInfo.appName = {"App1", "App2", "App3", "App4", "App5"};
    controlInfo.existenceTime = {UINT64_MAX, UINT64_MAX - 1000, UINT64_MAX - 2000};

    // When: 调用上报统计事件函数
    HisyseventUtils::DoReportMechkitControlStatisticEvent(controlInfo);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_003 end" << std::endl;
}

/**
 * @tc.name: DoReportMechkitControlStatisticEvent_004
 * @tc.desc: 测试上报机械控制统计事件 - 单个应用和存在时间
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DoReportMechkitControlStatisticEvent_004, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_004 begin" << std::endl;

    // Given: 准备单个应用的控制信息
    MechkitControlInfo controlInfo;
    controlInfo.upWard = 5;
    controlInfo.downWard = 3;
    controlInfo.turnLeft = 7;
    controlInfo.turnRight = 9;
    controlInfo.fastRotation = 2;
    controlInfo.landScapePortrait = 1;
    controlInfo.cameraMode = 1;
    controlInfo.trackingDuration = 500;
    controlInfo.cameraType = 1;
    controlInfo.zoomRatio = 2.0f;
    controlInfo.packageName = "com.example.singleapp";
    controlInfo.stickNum = 1;
    controlInfo.successNumber = 10;
    controlInfo.failNumber = 2;
    controlInfo.obstacleNumber = 1;
    controlInfo.cliffNumber = 0;
    controlInfo.deviceType = 2;
    controlInfo.appName = {"SingleApp"};
    controlInfo.existenceTime = {10000};

    // When: 调用上报统计事件函数
    HisyseventUtils::DoReportMechkitControlStatisticEvent(controlInfo);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_004 end" << std::endl;
}

/**
 * @tc.name: DoReportMechkitControlStatisticEvent_005
 * @tc.desc: 测试上报机械控制统计事件 - 边界值+1
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DoReportMechkitControlStatisticEvent_005, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_005 begin" << std::endl;

    // Given: 准备边界值+1的控制信息
    MechkitControlInfo controlInfo;
    controlInfo.upWard = 1;
    controlInfo.downWard = 1;
    controlInfo.turnLeft = 1;
    controlInfo.turnRight = 1;
    controlInfo.fastRotation = 1;
    controlInfo.landScapePortrait = 1;
    controlInfo.cameraMode = 1;
    controlInfo.trackingDuration = 1;
    controlInfo.cameraType = 1;
    controlInfo.zoomRatio = 0.1f;
    controlInfo.packageName = "a";
    controlInfo.stickNum = 1;
    controlInfo.successNumber = 1;
    controlInfo.failNumber = 1;
    controlInfo.obstacleNumber = 1;
    controlInfo.cliffNumber = 1;
    controlInfo.deviceType = 1;
    controlInfo.appName = {"A"};
    controlInfo.existenceTime = {1};

    // When: 调用上报统计事件函数
    HisyseventUtils::DoReportMechkitControlStatisticEvent(controlInfo);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_005 end" << std::endl;
}

/**
 * @tc.name: DoReportMechkitControlStatisticEvent_006
 * @tc.desc: 测试上报机械控制统计事件 - 负数边界值
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DoReportMechkitControlStatisticEvent_006, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_006 begin" << std::endl;

    // Given: 准备包含负数浮点值的控制信息
    MechkitControlInfo controlInfo;
    controlInfo.upWard = 10;
    controlInfo.downWard = 5;
    controlInfo.turnLeft = 15;
    controlInfo.turnRight = 20;
    controlInfo.fastRotation = 3;
    controlInfo.landScapePortrait = 1;
    controlInfo.cameraMode = 2;
    controlInfo.trackingDuration = 1000;
    controlInfo.cameraType = 1;
    controlInfo.zoomRatio = -1.0f;  // 负数边界值
    controlInfo.packageName = "com.example.negative";
    controlInfo.stickNum = 2;
    controlInfo.successNumber = 8;
    controlInfo.failNumber = 1;
    controlInfo.obstacleNumber = 3;
    controlInfo.cliffNumber = 0;
    controlInfo.deviceType = 1;
    controlInfo.appName = {"TestApp"};
    controlInfo.existenceTime = {5000};

    // When: 调用上报统计事件函数
    HisyseventUtils::DoReportMechkitControlStatisticEvent(controlInfo);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_006 end" << std::endl;
}

/**
 * @tc.name: DoReportMechkitControlStatisticEvent_007
 * @tc.desc: 测试上报机械控制统计事件 - 空包名
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DoReportMechkitControlStatisticEvent_007, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_007 begin" << std::endl;

    // Given: 准备空包名的控制信息
    MechkitControlInfo controlInfo;
    controlInfo.upWard = 10;
    controlInfo.downWard = 5;
    controlInfo.turnLeft = 15;
    controlInfo.turnRight = 20;
    controlInfo.fastRotation = 3;
    controlInfo.landScapePortrait = 1;
    controlInfo.cameraMode = 2;
    controlInfo.trackingDuration = 1000;
    controlInfo.cameraType = 1;
    controlInfo.zoomRatio = 1.5f;
    controlInfo.packageName = "";  // 空包名
    controlInfo.stickNum = 2;
    controlInfo.successNumber = 8;
    controlInfo.failNumber = 1;
    controlInfo.obstacleNumber = 3;
    controlInfo.cliffNumber = 0;
    controlInfo.deviceType = 1;
    controlInfo.appName = {};
    controlInfo.existenceTime = {};

    // When: 调用上报统计事件函数
    HisyseventUtils::DoReportMechkitControlStatisticEvent(controlInfo);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_007 end" << std::endl;
}

/**
 * @tc.name: DoReportMechkitControlStatisticEvent_008
 * @tc.desc: 测试上报机械控制统计事件 - 最大应用数量
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DoReportMechkitControlStatisticEvent_008, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_008 begin" << std::endl;

    // Given: 准备包含多个应用的控制信息
    MechkitControlInfo controlInfo;
    controlInfo.upWard = 10;
    controlInfo.downWard = 5;
    controlInfo.turnLeft = 15;
    controlInfo.turnRight = 20;
    controlInfo.fastRotation = 3;
    controlInfo.landScapePortrait = 1;
    controlInfo.cameraMode = 2;
    controlInfo.trackingDuration = 1000;
    controlInfo.cameraType = 1;
    controlInfo.zoomRatio = 1.5f;
    controlInfo.packageName = "com.example.multiapp";
    controlInfo.stickNum = 2;
    controlInfo.successNumber = 8;
    controlInfo.failNumber = 1;
    controlInfo.obstacleNumber = 3;
    controlInfo.cliffNumber = 0;
    controlInfo.deviceType = 1;
    controlInfo.appName = {"App1", "App2", "App3", "App4", "App5", "App6", "App7", "App8", "App9", "App10"};
    controlInfo.existenceTime = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000};

    // When: 调用上报统计事件函数
    HisyseventUtils::DoReportMechkitControlStatisticEvent(controlInfo);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_008 end" << std::endl;
}

/**
 * @tc.name: DoReportMechkitControlStatisticEvent_009
 * @tc.desc: 测试上报机械控制统计事件 - 高失败率场景
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DoReportMechkitControlStatisticEvent_009, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_009 begin" << std::endl;

    // Given: 准备高失败率的控制信息
    MechkitControlInfo controlInfo;
    controlInfo.upWard = 5;
    controlInfo.downWard = 5;
    controlInfo.turnLeft = 5;
    controlInfo.turnRight = 5;
    controlInfo.fastRotation = 5;
    controlInfo.landScapePortrait = 5;
    controlInfo.cameraMode = 2;
    controlInfo.trackingDuration = 1000;
    controlInfo.cameraType = 1;
    controlInfo.zoomRatio = 1.0f;
    controlInfo.packageName = "com.example.highfail";
    controlInfo.stickNum = 2;
    controlInfo.successNumber = 1;    // 低成功率
    controlInfo.failNumber = 99;     // 高失败率
    controlInfo.obstacleNumber = 50;
    controlInfo.cliffNumber = 30;
    controlInfo.deviceType = 1;
    controlInfo.appName = {"HighFailApp"};
    controlInfo.existenceTime = {5000};

    // When: 调用上报统计事件函数
    HisyseventUtils::DoReportMechkitControlStatisticEvent(controlInfo);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_009 end" << std::endl;
}

/**
 * @tc.name: DoReportMechkitControlStatisticEvent_010
 * @tc.desc: 测试上报机械控制统计事件 - 长时间跟踪场景
 * @tc.type: FUNC
 */
HWTEST_F(HisyseventUtilsTest, DoReportMechkitControlStatisticEvent_010, TestSize.Level3)
{
    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_010 begin" << std::endl;

    // Given: 准备长时间跟踪的控制信息
    MechkitControlInfo controlInfo;
    controlInfo.upWard = 100;
    controlInfo.downWard = 100;
    controlInfo.turnLeft = 200;
    controlInfo.turnRight = 200;
    controlInfo.fastRotation = 50;
    controlInfo.landScapePortrait = 10;
    controlInfo.cameraMode = 2;
    controlInfo.trackingDuration = 86400000;  // 24小时（毫秒）
    controlInfo.cameraType = 1;
    controlInfo.zoomRatio = 10.0f;
    controlInfo.packageName = "com.example.longtracking";
    controlInfo.stickNum = 2;
    controlInfo.successNumber = 500;
    controlInfo.failNumber = 5;
    controlInfo.obstacleNumber = 20;
    controlInfo.cliffNumber = 10;
    controlInfo.deviceType = 1;
    controlInfo.appName = {"LongTrackingApp"};
    controlInfo.existenceTime = {86400000};

    // When: 调用上报统计事件函数
    HisyseventUtils::DoReportMechkitControlStatisticEvent(controlInfo);

    // Then: 函数执行不会崩溃
    EXPECT_TRUE(true);

    DTEST_LOG << "HisyseventUtilsTest DoReportMechkitControlStatisticEvent_010 end" << std::endl;
}
} // namespace MechBodyController
} // namespace OHOS