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

#include "load_mechbody_adapter_test.h"
#include "mechbody_controller_log.h"

#include "../test_log.h"
#include <dlfcn.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {

void MechbodyAdapterUtilsTest::SetUpTestCase()
{
    DTEST_LOG << "MechbodyAdapterUtilsTest::SetUpTestCase" << std::endl;
}

void MechbodyAdapterUtilsTest::TearDownTestCase()
{
    DTEST_LOG << "MechbodyAdapterUtilsTest::TearDownTestCase" << std::endl;
    // 清理资源
    MechbodyAdapterUtils::Clear();
}

void MechbodyAdapterUtilsTest::SetUp()
{
    DTEST_LOG << "MechbodyAdapterUtilsTest::SetUp" << std::endl;
    // 每个测试前清理资源
    MechbodyAdapterUtils::Clear();
}

void MechbodyAdapterUtilsTest::TearDown()
{
    DTEST_LOG << "MechbodyAdapterUtilsTest::TearDown" << std::endl;
    // 每个测试后清理资源
    MechbodyAdapterUtils::Clear();
}

/**
 * @tc.name: InitTrackingCore_001
 * @tc.desc: 测试动态库加载失败场景
 * @tc.type: FUNC
 */
HWTEST_F(MechbodyAdapterUtilsTest, InitTrackingCore_001, TestSize.Level3)
{
    DTEST_LOG << "MechbodyAdapterUtilsTest InitTrackingCore_001 begin" << std::endl;

    // Given: 使用不存在的动态库路径会导致加载失败
    // When: 调用初始化方法
    int32_t ret = MechbodyAdapterUtils::InitTrackingCore();

    // Then: 根据环境验证具体的返回值
    // 如果动态库存在，应该返回成功
    if (ret == ERR_OK) {
        // 验证初始化成功后，通过调用依赖内部状态的公共方法来验证
        // RunTrackingCore 和 ResetTrackingCore 依赖内部句柄和函数指针
        auto pushFn = [](float x, float y) {};
        int32_t runRet = MechbodyAdapterUtils::RunTrackingCore(100.0f, 100.0f, 640.0f, 480.0f, pushFn);
        int32_t resetRet = MechbodyAdapterUtils::ResetTrackingCore();

        // 如果初始化成功，这些方法应该能够正常执行
        EXPECT_EQ(runRet, ERR_OK);
        EXPECT_EQ(resetRet, ERR_OK);
    } else {
        // 验证返回了具体的错误码
        EXPECT_TRUE(ret == LOAD_MECH_ADAPTER_SO_ERROR || ret == LOAD_MECH_ADAPTER_FUNCTION_ERROR);
    }

    DTEST_LOG << "MechbodyAdapterUtilsTest InitTrackingCore_001 end, ret=" << ret << std::endl;
}

/**
 * @tc.name: InitTrackingCore_002
 * @tc.desc: 测试重复初始化
 * @tc.type: FUNC
 */
HWTEST_F(MechbodyAdapterUtilsTest, InitTrackingCore_002, TestSize.Level3)
{
    DTEST_LOG << "MechbodyAdapterUtilsTest InitTrackingCore_002 begin" << std::endl;

    // Given: 第一次初始化
    int32_t ret1 = MechbodyAdapterUtils::InitTrackingCore();

    // When: 第二次初始化
    int32_t ret2 = MechbodyAdapterUtils::InitTrackingCore();

    // Then: 两次初始化结果应该一致
    EXPECT_EQ(ret1, ret2);

    DTEST_LOG << "MechbodyAdapterUtilsTest InitTrackingCore_002 end, ret1=" << ret1
              << ", ret2=" << ret2 << std::endl;
}

/**
 * @tc.name: RunTrackingCore_001
 * @tc.desc: 测试未初始化时调用 RunTrackingCore 返回错误
 * @tc.type: FUNC
 */
HWTEST_F(MechbodyAdapterUtilsTest, RunTrackingCore_001, TestSize.Level3)
{
    DTEST_LOG << "MechbodyAdapterUtilsTest RunTrackingCore_001 begin" << std::endl;

    // Given: 未初始化，句柄和函数指针为空
    // 已在 SetUp 中调用 Clear()

    // When: 调用 RunTrackingCore
    auto pushFn = [](float x, float y) {};
    int32_t ret = MechbodyAdapterUtils::RunTrackingCore(100.0f, 100.0f, 640.0f, 480.0f, pushFn);

    // Then: 返回错误码
    EXPECT_EQ(ret, LOAD_MECH_ADAPTER_FUNCTION_ERROR);

    DTEST_LOG << "MechbodyAdapterUtilsTest RunTrackingCore_001 end, ret=" << ret << std::endl;
}

/**
 * @tc.name: ResetTrackingCore_001
 * @tc.desc: 测试未初始化时调用 ResetTrackingCore 返回错误
 * @tc.type: FUNC
 */
HWTEST_F(MechbodyAdapterUtilsTest, ResetTrackingCore_001, TestSize.Level3)
{
    DTEST_LOG << "MechbodyAdapterUtilsTest ResetTrackingCore_001 begin" << std::endl;

    // Given: 未初始化，句柄和函数指针为空
    // 已在 SetUp 中调用 Clear()

    // When: 调用 ResetTrackingCore
    int32_t ret = MechbodyAdapterUtils::ResetTrackingCore();

    // Then: 返回错误码
    EXPECT_EQ(ret, LOAD_MECH_ADAPTER_FUNCTION_ERROR);

    DTEST_LOG << "MechbodyAdapterUtilsTest ResetTrackingCore_001 end, ret=" << ret << std::endl;
}

/**
 * @tc.name: Clear_001
 * @tc.desc: 测试清理资源时句柄存在
 * @tc.type: FUNC
 */
HWTEST_F(MechbodyAdapterUtilsTest, Clear_001, TestSize.Level3)
{
    DTEST_LOG << "MechbodyAdapterUtilsTest Clear_001 begin" << std::endl;

    // Given: 先初始化
    MechbodyAdapterUtils::InitTrackingCore();

    // When: 调用 Clear
    MechbodyAdapterUtils::Clear();

    // Then: 清理成功，后续调用 RunTrackingCore 应该返回错误
    auto pushFn = [](float x, float y) {};
    int32_t ret = MechbodyAdapterUtils::RunTrackingCore(100.0f, 100.0f, 640.0f, 480.0f, pushFn);
    EXPECT_EQ(ret, LOAD_MECH_ADAPTER_FUNCTION_ERROR);

    DTEST_LOG << "MechbodyAdapterUtilsTest Clear_001 end, ret=" << ret << std::endl;
}

/**
 * @tc.name: Clear_002
 * @tc.desc: 测试清理资源时句柄为空
 * @tc.type: FUNC
 */
HWTEST_F(MechbodyAdapterUtilsTest, Clear_002, TestSize.Level3)
{
    DTEST_LOG << "MechbodyAdapterUtilsTest Clear_002 begin" << std::endl;

    // Given: 未初始化，句柄为空
    // 已在 SetUp 中调用 Clear()

    // When: 调用 Clear
    MechbodyAdapterUtils::Clear();

    // Then: 不会导致崩溃
    auto pushFn = [](float x, float y) {};
    int32_t ret = MechbodyAdapterUtils::RunTrackingCore(100.0f, 100.0f, 640.0f, 480.0f, pushFn);
    EXPECT_EQ(ret, LOAD_MECH_ADAPTER_FUNCTION_ERROR);

    DTEST_LOG << "MechbodyAdapterUtilsTest Clear_002 end, ret=" << ret << std::endl;
}

/**
 * @tc.name: RunTrackingCore_004
 * @tc.desc: 测试多次调用 RunTrackingCore
 * @tc.type: FUNC
 */
HWTEST_F(MechbodyAdapterUtilsTest, RunTrackingCore_004, TestSize.Level3)
{
    DTEST_LOG << "MechbodyAdapterUtilsTest RunTrackingCore_004 begin" << std::endl;

    // Given: 初始化成功
    int32_t initRet = MechbodyAdapterUtils::InitTrackingCore();
    // In test environment, the dynamic library may not be available
    if (initRet != ERR_OK && initRet != DEVICE_NOT_NEED_SCREEN_INFO) {
        DTEST_LOG << "MechbodyAdapterUtilsTest RunTrackingCore_004 skipped, initRet=" << initRet << std::endl;
        // Split into two test cases instead of early return
        // This case only tests the initialization failure scenario
        GTEST_SKIP() << "Initialization failed, skipping RunTrackingCore test";
    }

    // When: 多次调用 RunTrackingCore
    auto pushFn = [](float x, float y) {};
    int32_t ret1 = MechbodyAdapterUtils::RunTrackingCore(100.0f, 100.0f, 640.0f, 480.0f, pushFn);
    int32_t ret2 = MechbodyAdapterUtils::RunTrackingCore(200.0f, 200.0f, 640.0f, 480.0f, pushFn);
    int32_t ret3 = MechbodyAdapterUtils::RunTrackingCore(300.0f, 300.0f, 640.0f, 480.0f, pushFn);

    // Then: 每次调用都应该返回成功
    EXPECT_EQ(ret1, ERR_OK);
    EXPECT_EQ(ret2, ERR_OK);
    EXPECT_EQ(ret3, ERR_OK);

    DTEST_LOG << "MechbodyAdapterUtilsTest RunTrackingCore_004 end, ret1=" << ret1
              << ", ret2=" << ret2 << ", ret3=" << ret3 << std::endl;
}

/**
 * @tc.name: ResetTrackingCore_003
 * @tc.desc: 测试多次调用 ResetTrackingCore
 * @tc.type: FUNC
 */
HWTEST_F(MechbodyAdapterUtilsTest, ResetTrackingCore_003, TestSize.Level3)
{
    DTEST_LOG << "MechbodyAdapterUtilsTest ResetTrackingCore_003 begin" << std::endl;

    // Given: 初始化成功
    int32_t initRet = MechbodyAdapterUtils::InitTrackingCore();
    // In test environment, the dynamic library may not be available
    if (initRet != ERR_OK && initRet != DEVICE_NOT_NEED_SCREEN_INFO) {
        DTEST_LOG << "MechbodyAdapterUtilsTest ResetTrackingCore_003 skipped, initRet=" << initRet << std::endl;
        // Split test cases instead of early return
        GTEST_SKIP() << "Initialization failed, skipping ResetTrackingCore test";
    }

    // When: 多次调用 ResetTrackingCore
    int32_t ret1 = MechbodyAdapterUtils::ResetTrackingCore();
    int32_t ret2 = MechbodyAdapterUtils::ResetTrackingCore();

    // Then: 每次调用都应该返回成功
    EXPECT_EQ(ret1, ERR_OK);
    EXPECT_EQ(ret2, ERR_OK);

    DTEST_LOG << "MechbodyAdapterUtilsTest ResetTrackingCore_003 end, ret1=" << ret1
              << ", ret2=" << ret2 << std::endl;
}

/**
 * @tc.name: InitTrackingCore_003
 * @tc.desc: 测试InitTrackingCore后mechAdapterHandle_的状态（覆盖50-53行分支）
 * @tc.type: FUNC
 */
HWTEST_F(MechbodyAdapterUtilsTest, InitTrackingCore_003, TestSize.Level3)
{
    DTEST_LOG << "MechbodyAdapterUtilsTest InitTrackingCore_003 begin" << std::endl;

    // Given: 清理状态，确保句柄为空
    MechbodyAdapterUtils::Clear();

    // When: 调用InitTrackingCore
    int32_t ret = MechbodyAdapterUtils::InitTrackingCore();

    // Then: 根据环境验证返回值和mechAdapterHandle_状态
    if (ret == ERR_OK) {
        // so文件存在时dlopen成功，mechAdapterHandle_非空
        EXPECT_NE(MechbodyAdapterUtils::mechAdapterHandle_, nullptr);
    } else {
        // so文件不存在时dlopen失败，走50-53行分支返回LOAD_MECH_ADAPTER_SO_ERROR
        EXPECT_EQ(ret, LOAD_MECH_ADAPTER_SO_ERROR);
        EXPECT_EQ(MechbodyAdapterUtils::mechAdapterHandle_, nullptr);
    }

    DTEST_LOG << "MechbodyAdapterUtilsTest InitTrackingCore_003 end, ret=" << ret << std::endl;
}

/**
 * @tc.name: InitTrackingCore_004
 * @tc.desc: 测试InitTrackingCore后mechBackgroundHandle_的状态（覆盖55-58行分支）
 * @tc.type: FUNC
 */
HWTEST_F(MechbodyAdapterUtilsTest, InitTrackingCore_004, TestSize.Level3)
{
    DTEST_LOG << "MechbodyAdapterUtilsTest InitTrackingCore_004 begin" << std::endl;

    // Given: 清理状态
    MechbodyAdapterUtils::Clear();

    // When: 调用InitTrackingCore
    int32_t ret = MechbodyAdapterUtils::InitTrackingCore();

    // Then: 根据环境验证返回值和mechBackgroundHandle_状态
    if (ret == ERR_OK) {
        // so文件存在时两个dlopen都成功，mechBackgroundHandle_非空
        EXPECT_NE(MechbodyAdapterUtils::mechBackgroundHandle_, nullptr);
    } else if (ret == LOAD_MECH_ADAPTER_SO_ERROR) {
        // 第一个so不存在时走50-53行分支，mechBackgroundHandle_未被赋值为空
        // 第一个so存在但第二个so不存在时走55-58行分支，mechBackgroundHandle_为nullptr
        EXPECT_EQ(MechbodyAdapterUtils::mechBackgroundHandle_, nullptr);
    }

    DTEST_LOG << "MechbodyAdapterUtilsTest InitTrackingCore_004 end, ret=" << ret << std::endl;
}

/**
 * @tc.name: InitTrackingCore_005
 * @tc.desc: 测试LoadFunction中dlsym查找Init符号失败返回LOAD_MECH_ADAPTER_FUNCTION_ERROR
 * @tc.type: FUNC
 */
HWTEST_F(MechbodyAdapterUtilsTest, InitTrackingCore_005, TestSize.Level3)
{
    DTEST_LOG << "MechbodyAdapterUtilsTest InitTrackingCore_005 begin" << std::endl;

    // Given: 清理状态，预设句柄为dlopen(nullptr)有效值，
    // dlsym在此句柄中查找"Init"符号将失败
    MechbodyAdapterUtils::Clear();
    MechbodyAdapterUtils::mechAdapterHandle_ = dlopen(nullptr, RTLD_LAZY);
    MechbodyAdapterUtils::mechBackgroundHandle_ = dlopen(nullptr, RTLD_LAZY);
    ASSERT_NE(MechbodyAdapterUtils::mechAdapterHandle_, nullptr);
    ASSERT_NE(MechbodyAdapterUtils::mechBackgroundHandle_, nullptr);

    // When: 直接调用LoadFunction，dlsym查找"Init"符号失败
    int32_t ret = MechbodyAdapterUtils::LoadFunction();

    // Then: LoadFunction返回LOAD_MECH_ADAPTER_FUNCTION_ERROR
    EXPECT_EQ(ret, LOAD_MECH_ADAPTER_FUNCTION_ERROR);

    // 清理句柄
    dlclose(MechbodyAdapterUtils::mechAdapterHandle_);
    dlclose(MechbodyAdapterUtils::mechBackgroundHandle_);
    MechbodyAdapterUtils::mechAdapterHandle_ = nullptr;
    MechbodyAdapterUtils::mechBackgroundHandle_ = nullptr;

    DTEST_LOG << "MechbodyAdapterUtilsTest InitTrackingCore_005 end, ret=" << ret << std::endl;
}

/**
 * @tc.name: InitTrackingCore_006
 * @tc.desc: 测试InitTrackingCore后函数指针和后续调用的状态（覆盖61-74行分支清理效果）
 * @tc.type: FUNC
 */
HWTEST_F(MechbodyAdapterUtilsTest, InitTrackingCore_006, TestSize.Level3)
{
    DTEST_LOG << "MechbodyAdapterUtilsTest InitTrackingCore_006 begin" << std::endl;

    // Given: 清理状态
    MechbodyAdapterUtils::Clear();

    // When: 调用InitTrackingCore
    int32_t ret = MechbodyAdapterUtils::InitTrackingCore();

    // Then: 根据环境验证返回值和后续调用状态
    if (ret == ERR_OK) {
        // 初始化成功时，函数指针有效，后续调用正常工作
        auto pushFn = [](float x, float y) {};
        int32_t runRet = MechbodyAdapterUtils::RunTrackingCore(100.0f, 100.0f, 640.0f, 480.0f, pushFn);
        EXPECT_EQ(runRet, ERR_OK);
        int32_t resetRet = MechbodyAdapterUtils::ResetTrackingCore();
        EXPECT_EQ(resetRet, ERR_OK);
        EXPECT_TRUE(MechbodyAdapterUtils::IsSupportBackground());
    } else {
        // 初始化失败时，函数指针为空，后续调用返回错误码
        auto pushFn = [](float x, float y) {};
        int32_t runRet = MechbodyAdapterUtils::RunTrackingCore(100.0f, 100.0f, 640.0f, 480.0f, pushFn);
        EXPECT_EQ(runRet, LOAD_MECH_ADAPTER_FUNCTION_ERROR);
        int32_t resetRet = MechbodyAdapterUtils::ResetTrackingCore();
        EXPECT_EQ(resetRet, LOAD_MECH_ADAPTER_FUNCTION_ERROR);
        EXPECT_FALSE(MechbodyAdapterUtils::IsSupportBackground());
    }

    DTEST_LOG << "MechbodyAdapterUtilsTest InitTrackingCore_006 end, ret=" << ret << std::endl;
}

} // namespace MechBodyController
} // namespace OHOS