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
#include <memory>

#include "mc_data_buffer_test.h"

#include "mechbody_controller_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {
void MechDataBufferTest::SetUpTestCase()
{
}

void MechDataBufferTest::TearDownTestCase()
{
}

void MechDataBufferTest::SetUp()
{
}

void MechDataBufferTest::TearDown()
{
}

/**
* @tc.name  : Constructor_ShouldAllocateMemory_WhenCapacityIsValid
* @tc.number: MechDataBuffer_Test_001
* @tc.desc  : Test constructor to ensure memory is allocated when capacity is valid
*/
HWTEST_F(MechDataBufferTest, Constructor_ShouldAllocateMemory_WhenCapacityIsValid, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    EXPECT_NE(buffer.Data(), nullptr);
    EXPECT_EQ(buffer.Capacity(), capacity);
}

/**
* @tc.name  : Constructor_ShouldNotAllocateMemory_WhenCapacityIsZero
* @tc.number: MechDataBuffer_Test_002
* @tc.desc  : Test constructor to ensure memory is not allocated when capacity is zero
*/
HWTEST_F(MechDataBufferTest, Constructor_ShouldNotAllocateMemory_WhenCapacityIsZero, TestSize.Level1)
{
    size_t capacity = 0;
    MechDataBuffer buffer(capacity);
    EXPECT_EQ(buffer.Data(), nullptr);
    EXPECT_EQ(buffer.Capacity(), 0);
}

/**
* @tc.name  : Constructor_ShouldNotAllocateMemory_WhenCapacityIsGreaterThanMax
* @tc.number: MechDataBuffer_Test_003
* @tc.desc  : Test constructor to ensure memory is not allocated when capacity is greater than MECH_MAX_BUFFER_SIZE
*/
HWTEST_F(MechDataBufferTest, Constructor_ShouldNotAllocateMemory_WhenCapacityIsGreaterThanMax, TestSize.Level1)
{
    size_t capacity = MechDataBuffer::MECH_MAX_BUFFER_SIZE + 1;
    MechDataBuffer buffer(capacity);
    EXPECT_EQ(buffer.Data(), nullptr);
    EXPECT_EQ(buffer.Capacity(), 0);
}

/**
* @tc.name  : Destructor_ShouldFreeMemory_WhenBufferIsNotNull
* @tc.number: MechDataBuffer_Test_004
* @tc.desc  : Test destructor to ensure memory is freed when buffer is not null
*/
HWTEST_F(MechDataBufferTest, Destructor_ShouldFreeMemory_WhenBufferIsNotNull, TestSize.Level1)
{
    size_t capacity = 100;
    std::unique_ptr<MechDataBuffer> buffer = std::make_unique<MechDataBuffer>(capacity);
    EXPECT_NE(buffer->Data(), nullptr);
    EXPECT_EQ(buffer->Capacity(), capacity);
    buffer.reset();
    EXPECT_EQ(buffer.get(), nullptr);
}

/**
* @tc.name  : SetRange_ShouldSetRange_WhenParametersAreValid
* @tc.number: MechDataBuffer_Test_005
* @tc.desc  : Test SetRange to ensure range is set when parameters are valid
*/
HWTEST_F(MechDataBufferTest, SetRange_ShouldSetRange_WhenParametersAreValid, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    size_t offset = 10;
    size_t size = 20;
    EXPECT_EQ(buffer.SetRange(offset, size), ERR_OK);
    EXPECT_EQ(buffer.Offset(), offset);
    EXPECT_EQ(buffer.Size(), size);
}

/**
* @tc.name  : SetRange_ShouldReturnError_WhenParametersAreInvalid
* @tc.number: MechDataBuffer_Test_006
* @tc.desc  : Test SetRange to ensure error is returned when parameters are invalid
*/
HWTEST_F(MechDataBufferTest, SetRange_ShouldReturnError_WhenParametersAreInvalid, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    size_t offset = 110;
    size_t size = 20;
    EXPECT_EQ(buffer.SetRange(offset, size), INVALID_PARAMETERS_ERR);

    size_t offsetInvaild = 90;
    size_t sizeInvaild = 20;
    EXPECT_EQ(buffer.SetRange(offsetInvaild, sizeInvaild), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendUint8_ShouldAppendData_WhenBufferHasEnoughSpace
* @tc.number: MechDataBuffer_Test_007
* @tc.desc  : Test AppendUint8 to ensure data is appended when buffer has enough space
*/
HWTEST_F(MechDataBufferTest, AppendUint8_ShouldAppendData_WhenBufferHasEnoughSpace, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    uint8_t value = 0x55;
    EXPECT_EQ(buffer.AppendUint8(value), ERR_OK);
    EXPECT_EQ(buffer.Size(), sizeof(value));
}

/**
* @tc.name  : AppendUint8_ShouldReturnError_WhenBufferDoesNotHaveEnoughSpace
* @tc.number: MechDataBuffer_Test_008
* @tc.desc  : Test AppendUint8 to ensure error is returned when buffer does not have enough space
*/
HWTEST_F(MechDataBufferTest, AppendUint8_ShouldReturnError_WhenBufferDoesNotHaveEnoughSpace, TestSize.Level1)
{
    size_t capacity = 0;
    MechDataBuffer buffer(capacity);
    uint8_t value = 0x55;
    EXPECT_EQ(buffer.AppendUint8(value), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadUint8_ShouldReadData_WhenParametersAreValid
* @tc.number: MechDataBuffer_Test_009
* @tc.desc  : Test ReadUint8 to ensure data is read when parameters are valid
*/
HWTEST_F(MechDataBufferTest, ReadUint8_ShouldReadData_WhenParametersAreValid, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    uint8_t value = 0x55;
    buffer.AppendUint8(value);
    uint8_t readValue = 0;
    EXPECT_EQ(buffer.ReadUint8(0, readValue), ERR_OK);
    EXPECT_EQ(readValue, value);
}

/**
* @tc.name  : ReadUint8_ShouldReturnError_WhenParametersAreInvalid
* @tc.number: MechDataBuffer_Test_010
* @tc.desc  : Test ReadUint8 to ensure error is returned when parameters are invalid
*/
HWTEST_F(MechDataBufferTest, ReadUint8_ShouldReturnError_WhenParametersAreInvalid, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    uint8_t readValue = 0;
    EXPECT_EQ(buffer.ReadUint8(100, readValue), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadUint32_ShouldReadData_WhenParametersAreValid
* @tc.number: MechDataBuffer_Test_011
* @tc.desc  : Test ReadUint32 to ensure data is read when parameters are valid
*/
HWTEST_F(MechDataBufferTest, ReadUint32_ShouldReadData_WhenParametersAreValid, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    uint32_t value = 0x55001232;
    buffer.AppendUint32(value);
    uint32_t readValue = 0;
    EXPECT_EQ(buffer.ReadUint32(0, readValue), ERR_OK);
    EXPECT_EQ(readValue, value);
}

/**
* @tc.name  : ReadUint32_ShouldReturnError_WhenParametersAreInvalid
* @tc.number: MechDataBuffer_Test_012
* @tc.desc  : Test ReadUint32 to ensure error is returned when parameters are invalid
*/
HWTEST_F(MechDataBufferTest, ReadUint32_ShouldReturnError_WhenParametersAreInvalid, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    uint32_t readValue = 0;
    EXPECT_EQ(buffer.ReadUint32(100, readValue), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadUint64_ShouldReadData_WhenParametersAreValid
* @tc.number: MechDataBuffer_Test_013
* @tc.desc  : Test ReadUint64 to ensure data is read when parameters are valid
*/
HWTEST_F(MechDataBufferTest, ReadUint64_ShouldReadData_WhenParametersAreValid, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    uint64_t value = 0x5500123255001232;
    buffer.AppendUint64(value);
    uint64_t readValue = 0;
    EXPECT_EQ(buffer.ReadUint64(0, readValue), ERR_OK);
    EXPECT_EQ(readValue, value);
}

/**
* @tc.name  : ReadUint64_ShouldReturnError_WhenParametersAreInvalid
* @tc.number: MechDataBuffer_Test_014
* @tc.desc  : Test ReadUint64 to ensure error is returned when parameters are invalid
*/
HWTEST_F(MechDataBufferTest, ReadUint64_ShouldReturnError_WhenParametersAreInvalid, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    uint64_t readValue = 0;
    EXPECT_EQ(buffer.ReadUint64(100, readValue), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendUint8_ShouldReturnError_WhenCapacityOverflow
* @tc.number: MechDataBuffer_Test_015
* @tc.desc  : Test AppendUint8 to ensure error is returned when rangeOffset + rangeLength + sizeof(value) > capacity
*/
HWTEST_F(MechDataBufferTest, AppendUint8_ShouldReturnError_WhenCapacityOverflow, TestSize.Level1)
{
    size_t capacity = 2;
    MechDataBuffer buffer(capacity);
    uint8_t value = 0x55;
    ASSERT_EQ(buffer.AppendUint8(value), ERR_OK);
    ASSERT_EQ(buffer.AppendUint8(value), ERR_OK);
    EXPECT_EQ(buffer.AppendUint8(value), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendUint8_ShouldReturnError_WhenRangeOffsetCausesOverflow
* @tc.number: MechDataBuffer_Test_016
* @tc.desc  : Test AppendUint8 to ensure error is returned when rangeOffset makes remaining space insufficient
*/
HWTEST_F(MechDataBufferTest, AppendUint8_ShouldReturnError_WhenRangeOffsetCausesOverflow, TestSize.Level1)
{
    size_t capacity = 4;
    MechDataBuffer buffer(capacity);
    ASSERT_EQ(buffer.SetRange(4, 0), ERR_OK);
    uint8_t value = 0x55;
    EXPECT_EQ(buffer.AppendUint8(value), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendUint16_ShouldAppendData_WhenBufferHasEnoughSpace
* @tc.number: MechDataBuffer_Test_017
* @tc.desc  : Test AppendUint16 to ensure data is appended when buffer has enough space
*/
HWTEST_F(MechDataBufferTest, AppendUint16_ShouldAppendData_WhenBufferHasEnoughSpace, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    uint16_t value = 0x1234;
    EXPECT_EQ(buffer.AppendUint16(value), ERR_OK);
    EXPECT_EQ(buffer.Size(), sizeof(value));
}

/**
* @tc.name  : AppendUint16_ShouldReturnError_WhenBufferIsNull
* @tc.number: MechDataBuffer_Test_018
* @tc.desc  : Test AppendUint16 to ensure error is returned when buffer data_ is null
*/
HWTEST_F(MechDataBufferTest, AppendUint16_ShouldReturnError_WhenBufferIsNull, TestSize.Level1)
{
    size_t capacity = 0;
    MechDataBuffer buffer(capacity);
    uint16_t value = 0x1234;
    EXPECT_EQ(buffer.AppendUint16(value), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendUint16_ShouldReturnError_WhenCapacityOverflow
* @tc.number: MechDataBuffer_Test_019
* @tc.desc  : Test AppendUint16 to ensure error is returned when remaining space is insufficient
*/
HWTEST_F(MechDataBufferTest, AppendUint16_ShouldReturnError_WhenCapacityOverflow, TestSize.Level1)
{
    size_t capacity = 2;
    MechDataBuffer buffer(capacity);
    uint16_t value = 0x1234;
    ASSERT_EQ(buffer.AppendUint8(0x55), ERR_OK);
    EXPECT_EQ(buffer.AppendUint16(value), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendUint16_ShouldReturnError_WhenRangeOffsetCausesOverflow
* @tc.number: MechDataBuffer_Test_020
* @tc.desc  : Test AppendUint16 to ensure error is returned when rangeOffset makes remaining space insufficient
*/
HWTEST_F(MechDataBufferTest, AppendUint16_ShouldReturnError_WhenRangeOffsetCausesOverflow, TestSize.Level1)
{
    size_t capacity = 4;
    MechDataBuffer buffer(capacity);
    ASSERT_EQ(buffer.SetRange(3, 0), ERR_OK);
    uint16_t value = 0x1234;
    EXPECT_EQ(buffer.AppendUint16(value), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendUint32_ShouldAppendData_WhenBufferHasEnoughSpace
* @tc.number: MechDataBuffer_Test_021
* @tc.desc  : Test AppendUint32 to ensure data is appended when buffer has enough space
*/
HWTEST_F(MechDataBufferTest, AppendUint32_ShouldAppendData_WhenBufferHasEnoughSpace, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    uint32_t value = 0x55001232;
    EXPECT_EQ(buffer.AppendUint32(value), ERR_OK);
    EXPECT_EQ(buffer.Size(), sizeof(value));
}

/**
* @tc.name  : AppendUint32_ShouldReturnError_WhenBufferIsNull
* @tc.number: MechDataBuffer_Test_022
* @tc.desc  : Test AppendUint32 to ensure error is returned when buffer data_ is null
*/
HWTEST_F(MechDataBufferTest, AppendUint32_ShouldReturnError_WhenBufferIsNull, TestSize.Level1)
{
    size_t capacity = 0;
    MechDataBuffer buffer(capacity);
    uint32_t value = 0x55001232;
    EXPECT_EQ(buffer.AppendUint32(value), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendUint32_ShouldReturnError_WhenCapacityOverflow
* @tc.number: MechDataBuffer_Test_023
* @tc.desc  : Test AppendUint32 to ensure error is returned when remaining space is insufficient
*/
HWTEST_F(MechDataBufferTest, AppendUint32_ShouldReturnError_WhenCapacityOverflow, TestSize.Level1)
{
    size_t capacity = 4;
    MechDataBuffer buffer(capacity);
    uint8_t value8 = 0x55;
    ASSERT_EQ(buffer.AppendUint8(value8), ERR_OK);
    EXPECT_EQ(buffer.AppendUint32(0x55001232), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendUint32_ShouldReturnError_WhenRangeOffsetCausesOverflow
* @tc.number: MechDataBuffer_Test_024
* @tc.desc  : Test AppendUint32 to ensure error is returned when rangeOffset makes remaining space insufficient
*/
HWTEST_F(MechDataBufferTest, AppendUint32_ShouldReturnError_WhenRangeOffsetCausesOverflow, TestSize.Level1)
{
    size_t capacity = 8;
    MechDataBuffer buffer(capacity);
    ASSERT_EQ(buffer.SetRange(5, 0), ERR_OK);
    uint32_t value = 0x55001232;
    EXPECT_EQ(buffer.AppendUint32(value), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendUint64_ShouldAppendData_WhenBufferHasEnoughSpace
* @tc.number: MechDataBuffer_Test_025
* @tc.desc  : Test AppendUint64 to ensure data is appended when buffer has enough space
*/
HWTEST_F(MechDataBufferTest, AppendUint64_ShouldAppendData_WhenBufferHasEnoughSpace, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    uint64_t value = 0x5500123255001232;
    EXPECT_EQ(buffer.AppendUint64(value), ERR_OK);
    EXPECT_EQ(buffer.Size(), sizeof(value));
}

/**
* @tc.name  : AppendUint64_ShouldReturnError_WhenBufferIsNull
* @tc.number: MechDataBuffer_Test_026
* @tc.desc  : Test AppendUint64 to ensure error is returned when buffer data_ is null
*/
HWTEST_F(MechDataBufferTest, AppendUint64_ShouldReturnError_WhenBufferIsNull, TestSize.Level1)
{
    size_t capacity = 0;
    MechDataBuffer buffer(capacity);
    uint64_t value = 0x5500123255001232;
    EXPECT_EQ(buffer.AppendUint64(value), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendUint64_ShouldReturnError_WhenCapacityOverflow
* @tc.number: MechDataBuffer_Test_027
* @tc.desc  : Test AppendUint64 to ensure error is returned when remaining space is insufficient
*/
HWTEST_F(MechDataBufferTest, AppendUint64_ShouldReturnError_WhenCapacityOverflow, TestSize.Level1)
{
    size_t capacity = 8;
    MechDataBuffer buffer(capacity);
    uint8_t value8 = 0x55;
    ASSERT_EQ(buffer.AppendUint8(value8), ERR_OK);
    EXPECT_EQ(buffer.AppendUint64(0x5500123255001232), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendUint64_ShouldReturnError_WhenRangeOffsetCausesOverflow
* @tc.number: MechDataBuffer_Test_028
* @tc.desc  : Test AppendUint64 to ensure error is returned when rangeOffset makes remaining space insufficient
*/
HWTEST_F(MechDataBufferTest, AppendUint64_ShouldReturnError_WhenRangeOffsetCausesOverflow, TestSize.Level1)
{
    size_t capacity = 16;
    MechDataBuffer buffer(capacity);
    ASSERT_EQ(buffer.SetRange(9, 0), ERR_OK);
    uint64_t value = 0x5500123255001232;
    EXPECT_EQ(buffer.AppendUint64(value), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendInt16_ShouldAppendData_WhenBufferHasEnoughSpace
* @tc.number: MechDataBuffer_Test_029
* @tc.desc  : Test AppendInt16 to ensure data is appended when buffer has enough space
*/
HWTEST_F(MechDataBufferTest, AppendInt16_ShouldAppendData_WhenBufferHasEnoughSpace, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    int16_t value = -1234;
    EXPECT_EQ(buffer.AppendInt16(value), ERR_OK);
    EXPECT_EQ(buffer.Size(), sizeof(value));
}

/**
* @tc.name  : AppendInt16_ShouldReturnError_WhenBufferIsNull
* @tc.number: MechDataBuffer_Test_030
* @tc.desc  : Test AppendInt16 to ensure error is returned when buffer data_ is null
*/
HWTEST_F(MechDataBufferTest, AppendInt16_ShouldReturnError_WhenBufferIsNull, TestSize.Level1)
{
    size_t capacity = 0;
    MechDataBuffer buffer(capacity);
    int16_t value = -1234;
    EXPECT_EQ(buffer.AppendInt16(value), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendInt16_ShouldReturnError_WhenCapacityOverflow
* @tc.number: MechDataBuffer_Test_031
* @tc.desc  : Test AppendInt16 to ensure error is returned when remaining space is insufficient
*/
HWTEST_F(MechDataBufferTest, AppendInt16_ShouldReturnError_WhenCapacityOverflow, TestSize.Level1)
{
    size_t capacity = 2;
    MechDataBuffer buffer(capacity);
    uint8_t value8 = 0x55;
    ASSERT_EQ(buffer.AppendUint8(value8), ERR_OK);
    EXPECT_EQ(buffer.AppendInt16(-1234), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendInt16_ShouldReturnError_WhenRangeOffsetCausesOverflow
* @tc.number: MechDataBuffer_Test_032
* @tc.desc  : Test AppendInt16 to ensure error is returned when rangeOffset makes remaining space insufficient
*/
HWTEST_F(MechDataBufferTest, AppendInt16_ShouldReturnError_WhenRangeOffsetCausesOverflow, TestSize.Level1)
{
    size_t capacity = 4;
    MechDataBuffer buffer(capacity);
    ASSERT_EQ(buffer.SetRange(3, 0), ERR_OK);
    int16_t value = -1234;
    EXPECT_EQ(buffer.AppendInt16(value), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendFloat_ShouldAppendData_WhenBufferHasEnoughSpace
* @tc.number: MechDataBuffer_Test_033
* @tc.desc  : Test AppendFloat to ensure data is appended when buffer has enough space
*/
HWTEST_F(MechDataBufferTest, AppendFloat_ShouldAppendData_WhenBufferHasEnoughSpace, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    float value = 3.14f;
    EXPECT_EQ(buffer.AppendFloat(value), ERR_OK);
    EXPECT_EQ(buffer.Size(), sizeof(value));
}

/**
* @tc.name  : AppendFloat_ShouldReturnError_WhenBufferIsNull
* @tc.number: MechDataBuffer_Test_034
* @tc.desc  : Test AppendFloat to ensure error is returned when buffer data_ is null
*/
HWTEST_F(MechDataBufferTest, AppendFloat_ShouldReturnError_WhenBufferIsNull, TestSize.Level1)
{
    size_t capacity = 0;
    MechDataBuffer buffer(capacity);
    float value = 3.14f;
    EXPECT_EQ(buffer.AppendFloat(value), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendFloat_ShouldReturnError_WhenCapacityOverflow
* @tc.number: MechDataBuffer_Test_035
* @tc.desc  : Test AppendFloat to ensure error is returned when remaining space is insufficient
*/
HWTEST_F(MechDataBufferTest, AppendFloat_ShouldReturnError_WhenCapacityOverflow, TestSize.Level1)
{
    size_t capacity = 4;
    MechDataBuffer buffer(capacity);
    uint8_t value8 = 0x55;
    ASSERT_EQ(buffer.AppendUint8(value8), ERR_OK);
    EXPECT_EQ(buffer.AppendFloat(3.14f), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendFloat_ShouldReturnError_WhenRangeOffsetCausesOverflow
* @tc.number: MechDataBuffer_Test_036
* @tc.desc  : Test AppendFloat to ensure error is returned when rangeOffset makes remaining space insufficient
*/
HWTEST_F(MechDataBufferTest, AppendFloat_ShouldReturnError_WhenRangeOffsetCausesOverflow, TestSize.Level1)
{
    size_t capacity = 8;
    MechDataBuffer buffer(capacity);
    ASSERT_EQ(buffer.SetRange(5, 0), ERR_OK);
    float value = 3.14f;
    EXPECT_EQ(buffer.AppendFloat(value), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendDataBuffer_ShouldAppendData_WhenBufferHasEnoughSpace
* @tc.number: MechDataBuffer_Test_037
* @tc.desc  : Test AppendDataBuffer to ensure data is appended when buffer has enough space
*/
HWTEST_F(MechDataBufferTest, AppendDataBuffer_ShouldAppendData_WhenBufferHasEnoughSpace, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    auto srcBuffer = std::make_shared<MechDataBuffer>(4);
    ASSERT_EQ(srcBuffer->AppendUint8(0x01), ERR_OK);
    ASSERT_EQ(srcBuffer->AppendUint8(0x02), ERR_OK);
    EXPECT_EQ(buffer.AppendDataBuffer(srcBuffer), ERR_OK);
    EXPECT_EQ(buffer.Size(), srcBuffer->Size());
}

/**
* @tc.name  : AppendDataBuffer_ShouldReturnError_WhenBufferIsNull
* @tc.number: MechDataBuffer_Test_038
* @tc.desc  : Test AppendDataBuffer to ensure error is returned when buffer data_ is null
*/
HWTEST_F(MechDataBufferTest, AppendDataBuffer_ShouldReturnError_WhenBufferIsNull, TestSize.Level1)
{
    size_t capacity = 0;
    MechDataBuffer buffer(capacity);
    auto srcBuffer = std::make_shared<MechDataBuffer>(4);
    EXPECT_EQ(buffer.AppendDataBuffer(srcBuffer), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendDataBuffer_ShouldReturnError_WhenCapacityOverflow
* @tc.number: MechDataBuffer_Test_039
* @tc.desc  : Test AppendDataBuffer to ensure error is returned when remaining space is insufficient
*/
HWTEST_F(MechDataBufferTest, AppendDataBuffer_ShouldReturnError_WhenCapacityOverflow, TestSize.Level1)
{
    size_t capacity = 4;
    MechDataBuffer buffer(capacity);
    ASSERT_EQ(buffer.AppendUint8(0x55), ERR_OK);
    auto srcBuffer = std::make_shared<MechDataBuffer>(4);
    ASSERT_EQ(srcBuffer->AppendUint32(0x55001232), ERR_OK);
    EXPECT_EQ(buffer.AppendDataBuffer(srcBuffer), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : AppendDataBuffer_ShouldReturnError_WhenRangeOffsetCausesOverflow
* @tc.number: MechDataBuffer_Test_040
* @tc.desc  : Test AppendDataBuffer to ensure error is returned when rangeOffset makes remaining space insufficient
*/
HWTEST_F(MechDataBufferTest, AppendDataBuffer_ShouldReturnError_WhenRangeOffsetCausesOverflow, TestSize.Level1)
{
    size_t capacity = 8;
    MechDataBuffer buffer(capacity);
    ASSERT_EQ(buffer.SetRange(5, 0), ERR_OK);
    auto srcBuffer = std::make_shared<MechDataBuffer>(4);
    ASSERT_EQ(srcBuffer->AppendUint32(0x55001232), ERR_OK);
    EXPECT_EQ(buffer.AppendDataBuffer(srcBuffer), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadUint8_ShouldReturnError_WhenBufferIsNull
* @tc.number: MechDataBuffer_Test_041
* @tc.desc  : Test ReadUint8 to ensure error is returned when buffer data_ is null
*/
HWTEST_F(MechDataBufferTest, ReadUint8_ShouldReturnError_WhenBufferIsNull, TestSize.Level1)
{
    size_t capacity = 0;
    MechDataBuffer buffer(capacity);
    uint8_t readValue = 0;
    EXPECT_EQ(buffer.ReadUint8(0, readValue), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadUint16_ShouldReadData_WhenParametersAreValid
* @tc.number: MechDataBuffer_Test_042
* @tc.desc  : Test ReadUint16 to ensure data is read when parameters are valid
*/
HWTEST_F(MechDataBufferTest, ReadUint16_ShouldReadData_WhenParametersAreValid, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    uint16_t value = 0x1234;
    ASSERT_EQ(buffer.AppendUint16(value), ERR_OK);
    uint16_t readValue = 0;
    EXPECT_EQ(buffer.ReadUint16(0, readValue), ERR_OK);
    EXPECT_EQ(readValue, value);
}

/**
* @tc.name  : ReadUint16_ShouldReturnError_WhenBufferIsNull
* @tc.number: MechDataBuffer_Test_043
* @tc.desc  : Test ReadUint16 to ensure error is returned when buffer data_ is null
*/
HWTEST_F(MechDataBufferTest, ReadUint16_ShouldReturnError_WhenBufferIsNull, TestSize.Level1)
{
    size_t capacity = 0;
    MechDataBuffer buffer(capacity);
    uint16_t readValue = 0;
    EXPECT_EQ(buffer.ReadUint16(0, readValue), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadUint16_ShouldReturnError_WhenOffsetExceedsRange
* @tc.number: MechDataBuffer_Test_044
* @tc.desc  : Test ReadUint16 to ensure error is returned when offset exceeds range length
*/
HWTEST_F(MechDataBufferTest, ReadUint16_ShouldReturnError_WhenOffsetExceedsRange, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    ASSERT_EQ(buffer.AppendUint8(0x55), ERR_OK);
    uint16_t readValue = 0;
    EXPECT_EQ(buffer.ReadUint16(0, readValue), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadUint16_ShouldReturnError_WhenRangeOffsetCausesOverflow
* @tc.number: MechDataBuffer_Test_045
* @tc.desc  : Test ReadUint16 to ensure error is returned when rangeOffset makes available data insufficient
*/
HWTEST_F(MechDataBufferTest, ReadUint16_ShouldReturnError_WhenRangeOffsetCausesOverflow, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    ASSERT_EQ(buffer.AppendUint16(0x1234), ERR_OK);
    ASSERT_EQ(buffer.SetRange(1, 1), ERR_OK);
    uint16_t readValue = 0;
    EXPECT_EQ(buffer.ReadUint16(0, readValue), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadUint32_ShouldReturnError_WhenBufferIsNull
* @tc.number: MechDataBuffer_Test_046
* @tc.desc  : Test ReadUint32 to ensure error is returned when buffer data_ is null
*/
HWTEST_F(MechDataBufferTest, ReadUint32_ShouldReturnError_WhenBufferIsNull, TestSize.Level1)
{
    size_t capacity = 0;
    MechDataBuffer buffer(capacity);
    uint32_t readValue = 0;
    EXPECT_EQ(buffer.ReadUint32(0, readValue), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadUint64_ShouldReturnError_WhenBufferIsNull
* @tc.number: MechDataBuffer_Test_047
* @tc.desc  : Test ReadUint64 to ensure error is returned when buffer data_ is null
*/
HWTEST_F(MechDataBufferTest, ReadUint64_ShouldReturnError_WhenBufferIsNull, TestSize.Level1)
{
    size_t capacity = 0;
    MechDataBuffer buffer(capacity);
    uint64_t readValue = 0;
    EXPECT_EQ(buffer.ReadUint64(0, readValue), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadFloat_ShouldReadData_WhenParametersAreValid
* @tc.number: MechDataBuffer_Test_048
* @tc.desc  : Test ReadFloat to ensure data is read when parameters are valid
*/
HWTEST_F(MechDataBufferTest, ReadFloat_ShouldReadData_WhenParametersAreValid, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    float value = 3.14f;
    ASSERT_EQ(buffer.AppendFloat(value), ERR_OK);
    float readValue = 0.0f;
    EXPECT_EQ(buffer.ReadFloat(0, readValue), ERR_OK);
    EXPECT_EQ(readValue, value);
}

/**
* @tc.name  : ReadFloat_ShouldReturnError_WhenBufferIsNull
* @tc.number: MechDataBuffer_Test_049
* @tc.desc  : Test ReadFloat to ensure error is returned when buffer data_ is null
*/
HWTEST_F(MechDataBufferTest, ReadFloat_ShouldReturnError_WhenBufferIsNull, TestSize.Level1)
{
    size_t capacity = 0;
    MechDataBuffer buffer(capacity);
    float readValue = 0.0f;
    EXPECT_EQ(buffer.ReadFloat(0, readValue), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadFloat_ShouldReturnError_WhenOffsetExceedsRange
* @tc.number: MechDataBuffer_Test_050
* @tc.desc  : Test ReadFloat to ensure error is returned when offset exceeds range length
*/
HWTEST_F(MechDataBufferTest, ReadFloat_ShouldReturnError_WhenOffsetExceedsRange, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    ASSERT_EQ(buffer.AppendUint8(0x55), ERR_OK);
    float readValue = 0.0f;
    EXPECT_EQ(buffer.ReadFloat(0, readValue), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadFloat_ShouldReturnError_WhenRangeOffsetCausesOverflow
* @tc.number: MechDataBuffer_Test_051
* @tc.desc  : Test ReadFloat to ensure error is returned when rangeOffset makes available data insufficient
*/
HWTEST_F(MechDataBufferTest, ReadFloat_ShouldReturnError_WhenRangeOffsetCausesOverflow, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    ASSERT_EQ(buffer.AppendFloat(3.14f), ERR_OK);
    ASSERT_EQ(buffer.SetRange(1, 2), ERR_OK);
    float readValue = 0.0f;
    EXPECT_EQ(buffer.ReadFloat(0, readValue), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadInt16_ShouldReadData_WhenParametersAreValid
* @tc.number: MechDataBuffer_Test_052
* @tc.desc  : Test ReadInt16 to ensure data is read when parameters are valid
*/
HWTEST_F(MechDataBufferTest, ReadInt16_ShouldReadData_WhenParametersAreValid, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    int16_t value = -1234;
    ASSERT_EQ(buffer.AppendInt16(value), ERR_OK);
    int16_t readValue = 0;
    EXPECT_EQ(buffer.ReadInt16(0, readValue), ERR_OK);
    EXPECT_EQ(readValue, value);
}

/**
* @tc.name  : ReadInt16_ShouldReturnError_WhenBufferIsNull
* @tc.number: MechDataBuffer_Test_053
* @tc.desc  : Test ReadInt16 to ensure error is returned when buffer data_ is null
*/
HWTEST_F(MechDataBufferTest, ReadInt16_ShouldReturnError_WhenBufferIsNull, TestSize.Level1)
{
    size_t capacity = 0;
    MechDataBuffer buffer(capacity);
    int16_t readValue = 0;
    EXPECT_EQ(buffer.ReadInt16(0, readValue), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadInt16_ShouldReturnError_WhenOffsetExceedsRange
* @tc.number: MechDataBuffer_Test_054
* @tc.desc  : Test ReadInt16 to ensure error is returned when offset exceeds range length
*/
HWTEST_F(MechDataBufferTest, ReadInt16_ShouldReturnError_WhenOffsetExceedsRange, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    ASSERT_EQ(buffer.AppendUint8(0x55), ERR_OK);
    int16_t readValue = 0;
    EXPECT_EQ(buffer.ReadInt16(0, readValue), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadInt16_ShouldReturnError_WhenRangeOffsetCausesOverflow
* @tc.number: MechDataBuffer_Test_055
* @tc.desc  : Test ReadInt16 to ensure error is returned when rangeOffset makes available data insufficient
*/
HWTEST_F(MechDataBufferTest, ReadInt16_ShouldReturnError_WhenRangeOffsetCausesOverflow, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    ASSERT_EQ(buffer.AppendInt16(-1234), ERR_OK);
    ASSERT_EQ(buffer.SetRange(1, 1), ERR_OK);
    int16_t readValue = 0;
    EXPECT_EQ(buffer.ReadInt16(0, readValue), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadString_ShouldReadData_WhenParametersAreValid
* @tc.number: MechDataBuffer_Test_056
* @tc.desc  : Test ReadString to ensure data is read when parameters are valid
*/
HWTEST_F(MechDataBufferTest, ReadString_ShouldReadData_WhenParametersAreValid, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    const char *testStr = "hello";
    size_t strLen = 5;
    for (size_t i = 0; i < strLen; i++) {
        ASSERT_EQ(buffer.AppendUint8(static_cast<uint8_t>(testStr[i])), ERR_OK);
    }
    char readStr[6] = {0};
    EXPECT_EQ(buffer.ReadString(0, readStr, strLen), ERR_OK);
    EXPECT_STREQ(readStr, testStr);
}

/**
* @tc.name  : ReadString_ShouldReturnError_WhenBufferIsNull
* @tc.number: MechDataBuffer_Test_057
* @tc.desc  : Test ReadString to ensure error is returned when buffer data_ is null
*/
HWTEST_F(MechDataBufferTest, ReadString_ShouldReturnError_WhenBufferIsNull, TestSize.Level1)
{
    size_t capacity = 0;
    MechDataBuffer buffer(capacity);
    char readStr[6] = {0};
    EXPECT_EQ(buffer.ReadString(0, readStr, 5), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadString_ShouldReturnError_WhenOffsetExceedsRange
* @tc.number: MechDataBuffer_Test_058
* @tc.desc  : Test ReadString to ensure error is returned when offset plus length exceeds range length
*/
HWTEST_F(MechDataBufferTest, ReadString_ShouldReturnError_WhenOffsetExceedsRange, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    ASSERT_EQ(buffer.AppendUint8(0x41), ERR_OK);
    char readStr[6] = {0};
    EXPECT_EQ(buffer.ReadString(0, readStr, 5), INVALID_PARAMETERS_ERR);
}

/**
* @tc.name  : ReadString_ShouldReturnError_WhenRangeOffsetCausesOverflow
* @tc.number: MechDataBuffer_Test_059
* @tc.desc  : Test ReadString to ensure error is returned when rangeOffset makes available data insufficient
*/
HWTEST_F(MechDataBufferTest, ReadString_ShouldReturnError_WhenRangeOffsetCausesOverflow, TestSize.Level1)
{
    size_t capacity = 100;
    MechDataBuffer buffer(capacity);
    const char *testStr = "hello";
    size_t strLen = 5;
    for (size_t i = 0; i < strLen; i++) {
        ASSERT_EQ(buffer.AppendUint8(static_cast<uint8_t>(testStr[i])), ERR_OK);
    }
    ASSERT_EQ(buffer.SetRange(2, 2), ERR_OK);
    char readStr[6] = {0};
    EXPECT_EQ(buffer.ReadString(0, readStr, strLen), INVALID_PARAMETERS_ERR);
}

} // namespace MechBodyController
} // namespace OHOS
