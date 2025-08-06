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

#include "bluetooth_state_test.h"

#include "bluetooth_state_listener.h"
#include "bluetooth_state_adapter.h"
#include "../test_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace MechBodyController {
namespace {
const std::string TAG = "BluetoothState";
}
void BluetoothStateTest::SetUpTestCase()
{
}

void BluetoothStateTest::TearDownTestCase()
{
}

void BluetoothStateTest::SetUp()
{
}

void BluetoothStateTest::TearDown()
{
}


/**
 * @tc.name: testBluetoothStateInit001
 * @tc.desc: test get bluetooth state from bluetooth service
 * @tc.type: FUNC
 * @tc.require: I5Y2VH
 */
HWTEST_F(BluetoothStateTest, testBluetoothStateInit001, TestSize.Level1)
{
    DTEST_LOG << "BluetoothStateTest testBluetoothStateInit001 start" << std::endl;
    std::shared_ptr<BluetoothStateListener> bluetoothStateListener = BluetoothStateListener::GetInstance();
    EXPECT_NO_FATAL_FAILURE(bluetoothStateListener->InitBluetoothState());
    DTEST_LOG << "BluetoothStateTest testBluetoothStateInit001 end" << std::endl;
}

/**
 * @tc.name: testOnStateChanged001
 * @tc.desc: test OnStateChanged from bluetooth service
 * @tc.type: FUNC
 * @tc.require: I5Y2VH
 */
HWTEST_F(BluetoothStateTest, testOnStateChanged001, TestSize.Level1)
{
    DTEST_LOG << "BluetoothStateTest testOnStateChanged001 start" << std::endl;
    std::shared_ptr<BluetoothStateListener> bluetoothStateListener = BluetoothStateListener::GetInstance();
    int transport = static_cast<int>(Bluetooth::BTTransport::ADAPTER_BREDR);
    int status = 0;
    EXPECT_NO_FATAL_FAILURE(bluetoothStateListener->OnStateChanged(transport, status));

    transport = static_cast<int>(Bluetooth::BTTransport::ADAPTER_BLE);
    EXPECT_NO_FATAL_FAILURE(bluetoothStateListener->OnStateChanged(transport, status));

    transport = -1;
    EXPECT_NO_FATAL_FAILURE(bluetoothStateListener->OnStateChanged(transport, status));
    DTEST_LOG << "BluetoothStateTest testOnStateChanged001 end" << std::endl;
}

/**
 * @tc.name: testBluetoothStateChange001
 * @tc.desc: test get bluetooth state from bluetooth service
 * @tc.type: FUNC
 * @tc.require: I5Y2VH
 */
    HWTEST_F(BluetoothStateTest, testBluetoothStateChange001, TestSize.Level1)
{
    DTEST_LOG << "BluetoothStateTest testBluetoothStateChange001 start" << std::endl;
    BluetoothStateAdapter::GetInstance().UpdateBTState(true);
    BluetoothStateAdapter::GetInstance().UpdateBLEState(true);
    EXPECT_TRUE(BluetoothStateAdapter::GetInstance().IsBluetoothActive());

    BluetoothStateAdapter::GetInstance().UpdateBTState(true);
    BluetoothStateAdapter::GetInstance().UpdateBLEState(false);
    EXPECT_TRUE(BluetoothStateAdapter::GetInstance().IsBluetoothActive());

    BluetoothStateAdapter::GetInstance().UpdateBTState(false);
    BluetoothStateAdapter::GetInstance().UpdateBLEState(true);
    EXPECT_TRUE(BluetoothStateAdapter::GetInstance().IsBluetoothActive());

    BluetoothStateAdapter::GetInstance().UpdateBTState(false);
    BluetoothStateAdapter::GetInstance().UpdateBLEState(false);
    EXPECT_FALSE(BluetoothStateAdapter::GetInstance().IsBluetoothActive());
    DTEST_LOG << "luetoothStateTest testBluetoothStateChange001 end" << std::endl;
}

} // DistributedSchedule
} // namespace OHOS