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

    // Initialize Bluetooth state
    bluetoothStateListener->InitBluetoothState();

    // Verify that BluetoothStateAdapter state is properly initialized
    // The state should be consistent with the actual Bluetooth state
    bool isBluetoothActive = BluetoothStateAdapter::GetInstance().IsBluetoothActive();
    // 根据系统蓝牙状态验证适配器状态是否正确同步
    // 如果系统蓝牙开启，isBluetoothActive应该为true
    EXPECT_TRUE(isBluetoothActive);

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

    // Test ADAPTER_BREDR transport with STATE_TURN_ON status
    int transport = static_cast<int>(Bluetooth::BTTransport::ADAPTER_BREDR);
    int status = static_cast<int>(Bluetooth::BTStateID::STATE_TURN_ON);
    bluetoothStateListener->OnStateChanged(transport, status);
    EXPECT_TRUE(BluetoothStateAdapter::GetInstance().IsBluetoothActive());

    // Test ADAPTER_BLE transport with STATE_TURN_ON status
    transport = static_cast<int>(Bluetooth::BTTransport::ADAPTER_BLE);
    status = static_cast<int>(Bluetooth::BTStateID::STATE_TURN_ON);
    bluetoothStateListener->OnStateChanged(transport, status);
    EXPECT_TRUE(BluetoothStateAdapter::GetInstance().IsBluetoothActive());

    // Test invalid transport (-1) - should not affect state
    bool stateBefore = BluetoothStateAdapter::GetInstance().IsBluetoothActive();
    transport = -1;
    status = 0;
    bluetoothStateListener->OnStateChanged(transport, status);
    bool stateAfter = BluetoothStateAdapter::GetInstance().IsBluetoothActive();
    EXPECT_EQ(stateBefore, stateAfter);

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

/**
 * @tc.name: testBluetoothStateListenerGetInstance001
 * @tc.desc: test GetInstance returns same instance
 * @tc.type: FUNC
 * @tc.require: I5Y2VH
 */
HWTEST_F(BluetoothStateTest, testBluetoothStateListenerGetInstance001, TestSize.Level1)
{
    DTEST_LOG << "BluetoothStateTest testBluetoothStateListenerGetInstance001 start" << std::endl;
    auto& instance1 = BluetoothStateListener::GetInstance();
    auto& instance2 = BluetoothStateListener::GetInstance();
    EXPECT_EQ(instance1, instance2);
    DTEST_LOG << "BluetoothStateTest testBluetoothStateListenerGetInstance001 end" << std::endl;
}

/**
 * @tc.name: testOnStateChangedBREDRTurnOn
 * @tc.desc: test OnStateChanged with BREDR transport and STATE_TURN_ON status
 * @tc.type: FUNC
 * @tc.require: I5Y2VH
 */
HWTEST_F(BluetoothStateTest, testOnStateChangedBREDRTurnOn, TestSize.Level1)
{
    DTEST_LOG << "BluetoothStateTest testOnStateChangedBREDRTurnOn start" << std::endl;
    std::shared_ptr<BluetoothStateListener> bluetoothStateListener = BluetoothStateListener::GetInstance();
    int transport = static_cast<int>(Bluetooth::BTTransport::ADAPTER_BREDR);
    int status = static_cast<int>(Bluetooth::BTStateID::STATE_TURN_ON);
    EXPECT_NO_FATAL_FAILURE(bluetoothStateListener->OnStateChanged(transport, status));
    EXPECT_TRUE(BluetoothStateAdapter::GetInstance().IsBluetoothActive());
    DTEST_LOG << "BluetoothStateTest testOnStateChangedBREDRTurnOn end" << std::endl;
}

/**
 * @tc.name: testOnStateChangedBREDRTurnOff
 * @tc.desc: test OnStateChanged with BREDR transport and STATE_TURN_OFF status
 * @tc.type: FUNC
 * @tc.require: I5Y2VH
 */
HWTEST_F(BluetoothStateTest, testOnStateChangedBREDRTurnOff, TestSize.Level1)
{
    DTEST_LOG << "BluetoothStateTest testOnStateChangedBREDRTurnOff start" << std::endl;
    std::shared_ptr<BluetoothStateListener> bluetoothStateListener = BluetoothStateListener::GetInstance();
    BluetoothStateAdapter::GetInstance().UpdateBTState(true);
    BluetoothStateAdapter::GetInstance().UpdateBLEState(false);
    int transport = static_cast<int>(Bluetooth::BTTransport::ADAPTER_BREDR);
    int status = static_cast<int>(Bluetooth::BTStateID::STATE_TURN_OFF);
    EXPECT_NO_FATAL_FAILURE(bluetoothStateListener->OnStateChanged(transport, status));
    EXPECT_FALSE(BluetoothStateAdapter::GetInstance().IsBluetoothActive());
    DTEST_LOG << "BluetoothStateTest testOnStateChangedBREDRTurnOff end" << std::endl;
}

/**
 * @tc.name: testOnStateChangedBLETurnOn
 * @tc.desc: test OnStateChanged with BLE transport and STATE_TURN_ON status
 * @tc.type: FUNC
 * @tc.require: I5Y2VH
 */
HWTEST_F(BluetoothStateTest, testOnStateChangedBLETurnOn, TestSize.Level1)
{
    DTEST_LOG << "BluetoothStateTest testOnStateChangedBLETurnOn start" << std::endl;
    std::shared_ptr<BluetoothStateListener> bluetoothStateListener = BluetoothStateListener::GetInstance();
    int transport = static_cast<int>(Bluetooth::BTTransport::ADAPTER_BLE);
    int status = static_cast<int>(Bluetooth::BTStateID::STATE_TURN_ON);
    EXPECT_NO_FATAL_FAILURE(bluetoothStateListener->OnStateChanged(transport, status));
    EXPECT_TRUE(BluetoothStateAdapter::GetInstance().IsBluetoothActive());
    DTEST_LOG << "BluetoothStateTest testOnStateChangedBLETurnOn end" << std::endl;
}

/**
 * @tc.name: testOnStateChangedBLETurnOff
 * @tc.desc: test OnStateChanged with BLE transport and STATE_TURN_OFF status
 * @tc.type: FUNC
 * @tc.require: I5Y2VH
 */
HWTEST_F(BluetoothStateTest, testOnStateChangedBLETurnOff, TestSize.Level1)
{
    DTEST_LOG << "BluetoothStateTest testOnStateChangedBLETurnOff start" << std::endl;
    std::shared_ptr<BluetoothStateListener> bluetoothStateListener = BluetoothStateListener::GetInstance();
    BluetoothStateAdapter::GetInstance().UpdateBTState(false);
    BluetoothStateAdapter::GetInstance().UpdateBLEState(true);
    int transport = static_cast<int>(Bluetooth::BTTransport::ADAPTER_BLE);
    int status = static_cast<int>(Bluetooth::BTStateID::STATE_TURN_OFF);
    EXPECT_NO_FATAL_FAILURE(bluetoothStateListener->OnStateChanged(transport, status));
    EXPECT_FALSE(BluetoothStateAdapter::GetInstance().IsBluetoothActive());
    DTEST_LOG << "BluetoothStateTest testOnStateChangedBLETurnOff end" << std::endl;
}

/**
 * @tc.name: testOnStateChangedInvalidTransport
 * @tc.desc: test OnStateChanged with invalid transport type
 * @tc.type: FUNC
 * @tc.require: I5Y2VH
 */
HWTEST_F(BluetoothStateTest, testOnStateChangedInvalidTransport, TestSize.Level1)
{
    DTEST_LOG << "BluetoothStateTest testOnStateChangedInvalidTransport start" << std::endl;
    std::shared_ptr<BluetoothStateListener> bluetoothStateListener = BluetoothStateListener::GetInstance();
    BluetoothStateAdapter::GetInstance().UpdateBTState(false);
    BluetoothStateAdapter::GetInstance().UpdateBLEState(false);
    int transport = -1;
    int status = static_cast<int>(Bluetooth::BTStateID::STATE_TURN_ON);
    EXPECT_NO_FATAL_FAILURE(bluetoothStateListener->OnStateChanged(transport, status));
    EXPECT_FALSE(BluetoothStateAdapter::GetInstance().IsBluetoothActive());
    DTEST_LOG << "BluetoothStateTest testOnStateChangedInvalidTransport end" << std::endl;
}

} // DistributedSchedule
} // namespace OHOS