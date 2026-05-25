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

#include "resource_manager_utils_test.h"

#include "../test_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {

void ResourceManagerUtilsTest::SetUpTestCase()
{
    DTEST_LOG << "ResourceManagerUtilsTest::SetUpTestCase" << std::endl;
}

void ResourceManagerUtilsTest::TearDownTestCase()
{
    DTEST_LOG << "ResourceManagerUtilsTest::TearDownTestCase" << std::endl;
}

void ResourceManagerUtilsTest::SetUp()
{
    DTEST_LOG << "ResourceManagerUtilsTest::SetUp" << std::endl;
}

void ResourceManagerUtilsTest::TearDown()
{
    DTEST_LOG << "ResourceManagerUtilsTest::TearDown" << std::endl;
}

/**
 * @tc.name: GetSystemStringByName_001
 * @tc.desc: 测试通过名称获取系统字符串
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerUtilsTest, GetSystemStringByName_001, TestSize.Level3)
{
    DTEST_LOG << "ResourceManagerUtilsTest GetSystemStringByName_001 begin" << std::endl;

    // When: 通过不存在的名称获取系统字符串
    std::string result = ResourceManagerUtils::GetSystemStringByName("non_existent_resource_name_12345");

    // Then: 返回空字符串（资源不存在）
    EXPECT_TRUE(result.empty());

    DTEST_LOG << "ResourceManagerUtilsTest GetSystemStringByName_001 end" << std::endl;
}

/**
 * @tc.name: GetSystemStringByName_002
 * @tc.desc: 测试通过空名称获取系统字符串（边界测试）
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerUtilsTest, GetSystemStringByName_002, TestSize.Level3)
{
    DTEST_LOG << "ResourceManagerUtilsTest GetSystemStringByName_002 begin" << std::endl;

    // When: 通过空名称获取系统字符串
    std::string result = ResourceManagerUtils::GetSystemStringByName("");

    // Then: 返回空字符串（空名称无效）
    EXPECT_TRUE(result.empty());

    DTEST_LOG << "ResourceManagerUtilsTest GetSystemStringByName_002 end" << std::endl;
}

/**
 * @tc.name: GetPixelMapByName_001
 * @tc.desc: 测试通过名称获取 PixelMap
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerUtilsTest, GetPixelMapByName_001, TestSize.Level3)
{
    DTEST_LOG << "ResourceManagerUtilsTest GetPixelMapByName_001 begin" << std::endl;

    // When: 通过存在的系统图标名称获取 PixelMap
    std::shared_ptr<Media::PixelMap> pixelMap = ResourceManagerUtils::GetPixelMapByName("ic_public_voice_filled");

    // Then: 返回非空的 PixelMap 对象
    ASSERT_NE(pixelMap, nullptr) << "PixelMap should not be null for existing system icon";

    // 验证 PixelMap 的基本属性（业务逻辑验证）
    EXPECT_GT(pixelMap->GetWidth(), 0) << "PixelMap width should be positive";
    EXPECT_GT(pixelMap->GetHeight(), 0) << "PixelMap height should be positive";

    DTEST_LOG << "ResourceManagerUtilsTest GetPixelMapByName_001 end" << std::endl;
}

/**
 * @tc.name: GetPixelMapByName_002
 * @tc.desc: 测试通过空名称获取 PixelMap（边界测试）
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerUtilsTest, GetPixelMapByName_002, TestSize.Level3)
{
    DTEST_LOG << "ResourceManagerUtilsTest GetPixelMapByName_002 begin" << std::endl;

    // When: 通过空名称获取 PixelMap
    std::shared_ptr<Media::PixelMap> pixelMap = ResourceManagerUtils::GetPixelMapByName("");

    // Then: 返回空指针（空名称无效）
    EXPECT_EQ(pixelMap, nullptr);

    DTEST_LOG << "ResourceManagerUtilsTest GetPixelMapByName_002 end" << std::endl;
}

/**
 * @tc.name: GetPixelMapByName_003
 * @tc.desc: 测试通过不存在的名称获取 PixelMap（边界测试）
 * @tc.type: FUNC
 */
HWTEST_F(ResourceManagerUtilsTest, GetPixelMapByName_003, TestSize.Level3)
{
    DTEST_LOG << "ResourceManagerUtilsTest GetPixelMapByName_003 begin" << std::endl;

    // When: 通过不存在的名称获取 PixelMap
    std::shared_ptr<Media::PixelMap> pixelMap = ResourceManagerUtils::GetPixelMapByName("nonexistent_icon_12345");

    // Then: 返回空指针（资源不存在）
    EXPECT_EQ(pixelMap, nullptr);

    DTEST_LOG << "ResourceManagerUtilsTest GetPixelMapByName_003 end" << std::endl;
}

} // namespace OHOS