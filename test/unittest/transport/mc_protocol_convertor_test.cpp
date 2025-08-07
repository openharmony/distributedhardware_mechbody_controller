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

#include <gtest/gtest.h>
#include "mc_protocol_convertor_test.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {
namespace {
const std::string TAG = "ProtocolConverter";
constexpr size_t MAX_DATA_SIZE = 251;
constexpr size_t MAX_SERVICE_NAME_SIZE = 0xFF;
constexpr size_t BT_HEADER_LENTH = 8;
constexpr size_t BT_SERVICE_HEADER_LENTH = 4;
constexpr size_t BT_COMMAND_HEADER_LENTH = 2;
constexpr int32_t BUF_MIN_LEN = 8;
}

void ProtocolConverterTest::SetUp()
{
    converter_ = std::make_unique<ProtocolConverter>();
}

void ProtocolConverterTest::TearDown()
{
    converter_ = nullptr;
}

void ProtocolConverterTest::SetUpTestCase() {}

void ProtocolConverterTest::TearDownTestCase() {}

/**
* @tc.name  : Convert_ShouldReturnNull_WhenInputDataIsNull
* @tc.number: Convert_Test_001
* @tc.desc  : Test the Convert function when the input data is nullptr.
*/
HWTEST_F(ProtocolConverterTest, Convert_ShouldReturnNull_WhenInputDataIsNull, TestSize.Level1)
{
    std::shared_ptr<MechDataBuffer> data = nullptr;
    auto result = converter_->Convert(OptType::GET, 0, data);
    EXPECT_EQ(result, nullptr);
}

/**
* @tc.name  : Convert_ShouldReturnNull_WhenDataSizeExceedsMax
* @tc.number: Convert_Test_002
* @tc.desc  : Test the Convert function when the input data size exceeds MAX_DATA_SIZE.
*/
HWTEST_F(ProtocolConverterTest, Convert_ShouldReturnNull_WhenDataSizeExceedsMax, TestSize.Level1)
{
    auto data = std::make_shared<MechDataBuffer>(MAX_DATA_SIZE + 1);
    auto result = converter_->Convert(OptType::GET, 0, data);
    EXPECT_EQ(result, nullptr);
}

/**
* @tc.name  : Convert_ShouldReturnNull_WhenServiceNameSizeExceedsMax
* @tc.number: Convert_Test_003
* @tc.desc  : Test the Convert function when the service name size exceeds MAX_SERVICE_NAME_SIZE.
*/
HWTEST_F(ProtocolConverterTest, Convert_ShouldReturnNull_WhenServiceNameSizeExceedsMax, TestSize.Level1)
{
    converter_->Convert(OptType::GET, 0, std::make_shared<MechDataBuffer>(1), "a");
    auto result = converter_->Convert(OptType::GET, 0, std::make_shared<MechDataBuffer>(1),
                                      std::string(MAX_SERVICE_NAME_SIZE + 1, 'a'));
    EXPECT_EQ(result, nullptr);
}

/**
* @tc.name  : Convert_ShouldReturnValidData_WhenInputIsValid
* @tc.number: Convert_Test_004
* @tc.desc  : Test the Convert function when the input data is valid.
*/
HWTEST_F(ProtocolConverterTest, Convert_ShouldReturnValidData_WhenInputIsValid, TestSize.Level1)
{
    auto data = std::make_shared<MechDataBuffer>(10);
    data->AppendFloat(0.0f);
    auto result = converter_->Convert(OptType::GET, 0, data);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->Size(), BT_HEADER_LENTH + BT_SERVICE_HEADER_LENTH + converter_->serviceName_.size() + 0 +
                                  BT_COMMAND_HEADER_LENTH + sizeof(float));
}

/**
* @tc.name  : Convert_ShouldReturnNull_WhenProtocolSizeSizeExceedsMax
* @tc.number: Convert_Test_005
* @tc.desc  : Test the Convert function when the service name size exceeds MAX_SERVICE_NAME_SIZE.
*/
HWTEST_F(ProtocolConverterTest, Convert_ShouldReturnNull_WhenProtocolSizeSizeExceedsMax, TestSize.Level1)
{
    converter_->Convert(OptType::GET, 0, std::make_shared<MechDataBuffer>(1), "a");
    auto result = converter_->Convert(OptType::GET, 0, std::make_shared<MechDataBuffer>(1),
                                      std::string(MAX_SERVICE_NAME_SIZE - 1, 'a'));
    EXPECT_EQ(result, nullptr);
}

/**
* @tc.name  : GetData_ShouldReturnValidData_WhenInputIsValid
* @tc.number: GetData_Test_001
* @tc.desc  : Test the GetData function when the input data is valid.
*/
HWTEST_F(ProtocolConverterTest, GetData_ShouldReturnValidData_WhenInputIsValid, TestSize.Level1)
{
    auto data = std::make_shared<MechDataBuffer>(sizeof(float));
    ASSERT_NE(data, nullptr);
    data->AppendFloat(0.0f);

    auto pclData = converter_->Convert(OptType::GET, 0, data, "aa");
    uint16_t seqNo = 0;
    bool isAck = false;
    auto result = converter_->GetData(pclData, seqNo, isAck);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->Size(), data->Size() + BT_COMMAND_HEADER_LENTH);
    EXPECT_EQ(seqNo, 0);
    EXPECT_EQ(isAck, false);
}

/**
* @tc.name  : GetData_ShouldReturnNull_WhenInputDataIsNull
* @tc.number: GetData_Test_002
* @tc.desc  : Test the GetData function when the input data is nullptr.
*/
HWTEST_F(ProtocolConverterTest, GetData_ShouldReturnNull_WhenInputDataIsNull, TestSize.Level1)
{
    std::shared_ptr<MechDataBuffer> pclData = nullptr;
    uint16_t seqNo = 0;
    bool isAck = false;
    auto result = converter_->GetData(pclData, seqNo, isAck);
    EXPECT_EQ(result, nullptr);
}

/**
* @tc.name  : Validate_ShouldReturnFalse_001
* @tc.number: Validate_Test_001
* @tc.desc  : Test the Validate function when the input data is nullptr.
*/
HWTEST_F(ProtocolConverterTest, Validate_ShouldReturnFalse_001, TestSize.Level1)
{
    uint8_t* data = nullptr;
    size_t length = 0;
    auto result = converter_->Validate(data, length);
    EXPECT_EQ(result, false);
}

/**
* @tc.name  : Validate_ShouldReturnFalse_002
* @tc.number: Validate_Test_002
* @tc.desc  : Test the Validate function when the input data is nullptr.
*/
HWTEST_F(ProtocolConverterTest, Validate_ShouldReturnFalse_002, TestSize.Level1)
{
    uint8_t data[BUF_MIN_LEN + 1] = {0};
    size_t length = 0;
    auto result = converter_->Validate(data, length);
    EXPECT_EQ(result, false);
}

/**
* @tc.name  : Validate_ShouldReturnFalse_003
* @tc.number: Validate_Test_003
* @tc.desc  : Test the Validate function when the input data is nullptr.
*/
HWTEST_F(ProtocolConverterTest, Validate_ShouldReturnFalse_003, TestSize.Level1)
{
    uint8_t data[BUF_MIN_LEN + 1] = {0};
    size_t length = MAX_DATA_SIZE + 1;
    auto result = converter_->Validate(data, length);
    EXPECT_EQ(result, false);
}

/**
* @tc.name  : Validate_ShouldReturnTrue_001
* @tc.number: Validate_Test_004
* @tc.desc  : Test the Validate function when the input data is nullptr.
*/
HWTEST_F(ProtocolConverterTest, Validate_ShouldReturnTrue_001, TestSize.Level1)
{
    uint8_t data[BUF_MIN_LEN + 1] = {0};
    size_t length = MAX_DATA_SIZE - 1;
    auto result = converter_->Validate(data, length);
    EXPECT_EQ(result, true);
}
}  // namespace MechBodyController
}  // namespace OHOS
