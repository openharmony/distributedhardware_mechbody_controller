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
    MechDataBuffer* buffer = new MechDataBuffer(capacity);
    delete buffer;
    EXPECT_EQ(buffer->Data(), nullptr);
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
} // namespace MechBodyController
} // namespace OHOS
