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
#include "ble_send_manager_test.h"
#include "bluetooth_errorcode.h"
#include "c_header/ohos_bt_gatt.h"
#include "mechbody_controller_log.h"
#include "test_log.h"
#include <chrono>
#include <thread>
#include <atomic>
#include "uuid.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS {
namespace MechBodyController {
namespace {
    constexpr int32_t BLE_REVEVICE_LISTENER_SIZE = 1;
    constexpr size_t NOTIFY_DATA_MAX_SIZE = 1024 * 1024;
    BleSendManager& bleSendManager_ = BleSendManager::GetInstance();
    using namespace OHOS::Bluetooth;
    constexpr int32_t ERROR_NO_LISTENERS = -10001;
    constexpr int32_t ERROR_INVALID_PARAMETER = -10002;
    constexpr int32_t MECHBODY_GATT_INVALID_PARAM = -10003;
    static const int32_t ERROR_SIZE_TOO_LARGE = -10004;
    constexpr int32_t MECHBODY_GATT_CONNECT_FAILED = -10005;
    constexpr int32_t BUF_MIN_LEN = 8;
    constexpr int32_t BUF_LESS_THAN_MIN_LEN = 5;
    constexpr int32_t BUF_GREATER_THAN_MAX_LEN = 252;
    constexpr int32_t ERROR_INVALID_PARAMETERS = 2901003;
    UUID MECHBODY_CHARACTERISTIC_NOTIFY_UUID = UUID::FromString("15f1e603-a277-43fc-a484-dd39ef8a9100"); // notify uuid
}

class MyBleReceiveListener : public BleReceviceListener {
public:
    MyBleReceiveListener() {}

    int32_t OnReceive(const uint8_t *data, uint32_t dataLen) override
    {
        return ERR_OK;
    }
    virtual ~MyBleReceiveListener() {}
};

void BleSendManagerTest::SetUpTestCase()
{
    DTEST_LOG << "BleSendManagerTest::SetUpTestCase" << std::endl;
}

void BleSendManagerTest::TearDownTestCase()
{
    DTEST_LOG << "BleSendManagerTest::TearDownTestCase" << std::endl;
}

void BleSendManagerTest::TearDown()
{
    DTEST_LOG << "BleSendManagerTest::TearDown" << std::endl;
}

void BleSendManagerTest::SetUp()
{
    DTEST_LOG << "BleSendManagerTest::SetUp" << std::endl;
}

/**
 * @tc.name  : InitScanPara_001
 * @tc.number: InitScanPara_001
 * @tc.desc  : InitScanPara is correctly initializes the scan parameters.
 */
HWTEST_F(BleSendManagerTest, InitScanPara_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest InitScanPara_001 begin" << std::endl;
    bleSendManager_.InitScanPara();

    EXPECT_EQ(bleSendManager_.scanParam_.settings.GetScanMode(), BleScanMode::OHOS_BLE_SCAN_MODE_LOW_POWER);

    EXPECT_EQ(bleSendManager_.scanParam_.filters.size(), BLE_REVEVICE_LISTENER_SIZE);

    EXPECT_NE(bleSendManager_.bleCentralManager_, nullptr);
    DTEST_LOG << "BleSendManagerTest InitScanPara_001 end" << std::endl;
}

/**
 * @tc.name  : StartScan_001
 * @tc.number: StartScan_001
 * @tc.desc  : StartScan when bleCentralManager_ is null.
 */
HWTEST_F(BleSendManagerTest, StartScan_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest StartScan_001 begin" << std::endl;
    bleSendManager_.bleCentralManager_ = nullptr;
    EXPECT_NO_FATAL_FAILURE(bleSendManager_.StartScan());
    DTEST_LOG << "BleSendManagerTest StartScan_001 end" << std::endl;
}

/**
 * @tc.name  : StartScan_002
 * @tc.number: StartScan_002
 * @tc.desc  : StartScan when bleCentralManager_ is not null.
 */
HWTEST_F(BleSendManagerTest, StartScan_002, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest StartScan_001 begin" << std::endl;
    std::shared_ptr<BleCentralManagerCallbackImpl> bleCentralManagerCallback =
        std::make_shared<BleCentralManagerCallbackImpl>();
    bleSendManager_.bleCentralManager_ = std::make_shared<Bluetooth::BleCentralManager>(bleCentralManagerCallback);
    EXPECT_NO_FATAL_FAILURE(bleSendManager_.StartScan());
    DTEST_LOG << "BleSendManagerTest StartScan_002 end" << std::endl;
}

/**
 * @tc.name  : StopScan_001
 * @tc.number: StopScan_001
 * @tc.desc  : StopScan when bleCentralManager_ is null.
 */
HWTEST_F(BleSendManagerTest, StopScan_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest StopScan_001 begin" << std::endl;
    bleSendManager_.bleCentralManager_ = nullptr;
    EXPECT_NO_FATAL_FAILURE(bleSendManager_.StopScan());
    DTEST_LOG << "BleSendManagerTest StopScan_001 end" << std::endl;
}

/**
 * @tc.name  : StopScan_002
 * @tc.number: StopScan_002
 * @tc.desc  : StopScan when bleCentralManager_ is not null.
 */
HWTEST_F(BleSendManagerTest, StopScan_002, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest StopScan_002 begin" << std::endl;
    std::shared_ptr<BleCentralManagerCallbackImpl> bleCentralManagerCallback =
        std::make_shared<BleCentralManagerCallbackImpl>();
    bleSendManager_.bleCentralManager_ = std::make_shared<Bluetooth::BleCentralManager>(bleCentralManagerCallback);
    EXPECT_NO_FATAL_FAILURE(bleSendManager_.StopScan());
    DTEST_LOG << "BleSendManagerTest StopScan_002 end" << std::endl;
}

/**
 * @tc.name  : OnReceive_001
 * @tc.number: OnReceive_001
 * @tc.desc  : OnReceive should return ERROR_INVALID_PARAMETER when data is nullptr.
 */
HWTEST_F(BleSendManagerTest, OnReceive_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest OnReceive_001 begin" << std::endl;
    uint8_t* data = nullptr;
    size_t size = 10;
    EXPECT_EQ(bleSendManager_.OnReceive(data, size), ERROR_INVALID_PARAMETER);
    DTEST_LOG << "BleSendManagerTest OnReceive_001 end" << std::endl;
}

/**
 * @tc.name  : OnReceive_002
 * @tc.number: OnReceive_002
 * @tc.desc  : OnReceive should return ERROR_INVALID_PARAMETER  when size is zero.
 */
HWTEST_F(BleSendManagerTest, OnReceive_002, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest OnReceive_002 begin" << std::endl;
    uint8_t data[10] = {0};
    size_t size = 0;
    EXPECT_EQ(bleSendManager_.OnReceive(data, size), ERROR_INVALID_PARAMETER);
    DTEST_LOG << "BleSendManagerTest OnReceive_002 end" << std::endl;
}

/**
 * @tc.name  : OnReceive_003
 * @tc.number: OnReceive_003
 * @tc.desc  : OnReceive should return ERROR_SIZE_TOO_LARGE  when size exceeds NOTIFY_DATA_MAX_SIZE.
 */
HWTEST_F(BleSendManagerTest, OnReceive_003, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest OnReceive_003 begin" << std::endl;
    uint8_t data[NOTIFY_DATA_MAX_SIZE + 1] = {0};
    size_t size = NOTIFY_DATA_MAX_SIZE + 1;
    EXPECT_EQ(bleSendManager_.OnReceive(data, size), ERROR_SIZE_TOO_LARGE);
    DTEST_LOG << "BleSendManagerTest OnReceive_003 end" << std::endl;
}


/**
 * @tc.name  : OnReceive_004
 * @tc.number: OnReceive_004
 * @tc.desc  : Test when bleReceviceListeners_ is empty, OnReceive should return ERROR_NO_LISTENERS
 */
HWTEST_F(BleSendManagerTest, OnReceive_004, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest OnReceive_004 begin" << std::endl;
    uint8_t data[10] = {0};
    size_t size = 10;
    EXPECT_EQ(bleSendManager_.OnReceive(data, size), ERROR_NO_LISTENERS);
    DTEST_LOG << "BleSendManagerTest OnReceive_004 end" << std::endl;
}

/**
 * @tc.name  : OnReceive_005
 * @tc.number: OnReceive_005
 * @tc.desc  : Test when bleReceviceListeners_ contains nullptr, OnReceive should return ERROR_NO_LISTENERS
 */
HWTEST_F(BleSendManagerTest, OnReceive_005, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest OnReceive_005 begin" << std::endl;
    uint8_t data[10] = {0};
    size_t size = 10;
    bleSendManager_.bleReceviceListeners_.push_back(nullptr);
    EXPECT_EQ(bleSendManager_.OnReceive(data, size), ERROR_NO_LISTENERS);
    DTEST_LOG << "BleSendManagerTest OnReceive_005 end" << std::endl;
}

/**
 * @tc.name  : CheckGattcIsReady_001
 * @tc.number: CheckGattcIsReady_001
 * @tc.desc  : CheckGattcIsReady when isGattReady_ is true.
 */
HWTEST_F(BleSendManagerTest, CheckGattcIsReady_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest CheckGattcIsReady_001 begin" << std::endl;
    bleSendManager_.isGattReady_ = true;
    EXPECT_TRUE(bleSendManager_.CheckGattcIsReady());
    DTEST_LOG << "BleSendManagerTest CheckGattcIsReady_001 end" << std::endl;
}

/**
 * @tc.name  : CheckGattcIsReady_002
 * @tc.number: CheckGattcIsReady_002
 * @tc.desc  : CheckGattcIsReady when isGattReady_ is false.
 */
HWTEST_F(BleSendManagerTest, CheckGattcIsReady_002, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest CheckGattcIsReady_002 begin" << std::endl;
    bleSendManager_.isGattReady_ = false;
    EXPECT_FALSE(bleSendManager_.CheckGattcIsReady());
    DTEST_LOG << "BleSendManagerTest CheckGattcIsReady_002 end" << std::endl;
}

/**
 * @tc.name  : RegisterTransportSendAdapter_001
 * @tc.number: RegisterTransportSendAdapter_001
 * @tc.desc  : RegisterTransportSendAdapter registering a listener that already exists in the list.
 */
HWTEST_F(BleSendManagerTest, RegisterTransportSendAdapter_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest RegisterTransportSendAdapter_001 begin" << std::endl;
    auto listener = std::make_shared<MyBleReceiveListener>();
    bleSendManager_.bleReceviceListeners_.push_back(listener);
    int32_t result = bleSendManager_.RegisterTransportSendAdapter(listener);
    EXPECT_EQ(result, ERR_OK);
    DTEST_LOG << "BleSendManagerTest RegisterTransportSendAdapter_001 end" << std::endl;
}

/**
 * @tc.name  : RegisterTransportSendAdapter_002
 * @tc.number: RegisterTransportSendAdapter_002
 * @tc.desc  : RegisterTransportSendAdapter registering a listener that does not exist in the list.
 */
HWTEST_F(BleSendManagerTest, RegisterTransportSendAdapter_002, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest RegisterTransportSendAdapter_002 begin" << std::endl;
    auto listener = std::make_shared<MyBleReceiveListener>();
    int32_t result = bleSendManager_.RegisterTransportSendAdapter(listener);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_NE(std::find(bleSendManager_.bleReceviceListeners_.begin(),
        bleSendManager_.bleReceviceListeners_.end(), listener), bleSendManager_.bleReceviceListeners_.end());
    DTEST_LOG << "BleSendManagerTest RegisterTransportSendAdapter_002 end" << std::endl;
}

/**
 * @tc.name  : RegisterTransportSendAdapter_003
 * @tc.number: RegisterTransportSendAdapter_003
 * @tc.desc  : RegisterTransportSendAdapter listener is null.
 */
HWTEST_F(BleSendManagerTest, RegisterTransportSendAdapter_003, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest RegisterTransportSendAdapter_002 begin" << std::endl;
    int32_t result = bleSendManager_.RegisterTransportSendAdapter(nullptr);
    EXPECT_EQ(result, ERROR_NO_LISTENERS);
    DTEST_LOG << "BleSendManagerTest RegisterTransportSendAdapter_002 end" << std::endl;
}

/**
 * @tc.name  : UnRegisterTransportSendAdapter_001
 * @tc.number: UnRegisterTransportSendAdapter_001
 * @tc.desc  : UnRegisterTransportSendAdapter when the listeners list is empty.
 */
HWTEST_F(BleSendManagerTest, UnRegisterTransportSendAdapter_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest UnRegisterTransportSendAdapter_001 begin" << std::endl;
    auto listener = std::make_shared<MyBleReceiveListener>();
    EXPECT_EQ(bleSendManager_.UnRegisterTransportSendAdapter(listener), ERROR_NO_LISTENERS);
    DTEST_LOG << "BleSendManagerTest UnRegisterTransportSendAdapter_001 end" << std::endl;
}

/**
 * @tc.name  : UnRegisterTransportSendAdapter_002
 * @tc.number: UnRegisterTransportSendAdapter_002
 * @tc.desc  : UnRegisterTransportSendAdapter when the listener is not found in the list.
 */
HWTEST_F(BleSendManagerTest, UnRegisterTransportSendAdapter_002, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest UnRegisterTransportSendAdapter_002 begin" << std::endl;
    auto listener1 = std::make_shared<MyBleReceiveListener>();
    auto listener2 = std::make_shared<MyBleReceiveListener>();
    bleSendManager_.bleReceviceListeners_.push_back(listener1);
    EXPECT_EQ(bleSendManager_.UnRegisterTransportSendAdapter(listener2), ERROR_NO_LISTENERS);
    DTEST_LOG << "BleSendManagerTest UnRegisterTransportSendAdapter_002 end" << std::endl;
}

/**
 * @tc.name  : UnRegisterTransportSendAdapter_003
 * @tc.number: UnRegisterTransportSendAdapter_003
 * @tc.desc  : UnRegisterTransportSendAdapter when the listener is found.
 */
HWTEST_F(BleSendManagerTest, UnRegisterTransportSendAdapter_003, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest UnRegisterTransportSendAdapter_003 begin" << std::endl;
    auto listener = std::make_shared<MyBleReceiveListener>();
    bleSendManager_.bleReceviceListeners_.clear();
    bleSendManager_.bleReceviceListeners_.push_back(listener);
    EXPECT_EQ(bleSendManager_.UnRegisterTransportSendAdapter(listener), ERR_OK);
    EXPECT_EQ(bleSendManager_.bleReceviceListeners_.size(), 0);
    DTEST_LOG << "BleSendManagerTest UnRegisterTransportSendAdapter_003 end" << std::endl;
}

/**
 * @tc.name  : UnRegisterTransportSendAdapter_004
 * @tc.number: UnRegisterTransportSendAdapter_004
 * @tc.desc  : UnRegisterTransportSendAdapter when bleReceviceListeners_ is null.
 */
HWTEST_F(BleSendManagerTest, UnRegisterTransportSendAdapter_004, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest UnRegisterTransportSendAdapter_004 begin" << std::endl;
    auto listener = std::make_shared<MyBleReceiveListener>();
    bleSendManager_.bleReceviceListeners_.clear();
    EXPECT_EQ(bleSendManager_.UnRegisterTransportSendAdapter(listener), ERROR_NO_LISTENERS);
    DTEST_LOG << "BleSendManagerTest UnRegisterTransportSendAdapter_004 end" << std::endl;
}

/**
 * @tc.name  : OnGattReady_001
 * @tc.number: OnGattReady_001
 * @tc.desc  : OnGattReady when it is called.
 */
HWTEST_F(BleSendManagerTest, OnGattReady_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest OnGattReady_001 begin" << std::endl;
    MechInfo mechInfo;
    bleSendManager_.OnGattReady(mechInfo);
    DTEST_LOG << "BleSendManagerTest OnGattReady_001 end" << std::endl;
}

/**
 * @tc.name  : MechbodyGattcDisconnect_001
 * @tc.number: MechbodyGattcDisconnect_001
 * @tc.desc  : Test MechbodyGattcDisconnect function.
 */
HWTEST_F(BleSendManagerTest, MechbodyGattcDisconnect_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest MechbodyGattcDisconnect_001 begin" << std::endl;
    std::string mac = "mac";
    bleSendManager_.address_ = mac;
    BluetoothRemoteDevice device(bleSendManager_.address_, 1);
    bleSendManager_.gattClient_ = nullptr;
    MechInfo mechInfo;
    int32_t result = bleSendManager_.MechbodyGattcDisconnect(mechInfo);
    EXPECT_EQ(result, MECHBODY_GATT_INVALID_PARAM);

    bleSendManager_.gattClient_ = std::make_shared<OHOS::Bluetooth::GattClient>(device);
    result = bleSendManager_.MechbodyGattcDisconnect(mechInfo);
    EXPECT_EQ(result, MECHBODY_GATT_INVALID_PARAM);
    DTEST_LOG << "BleSendManagerTest MechbodyGattcDisconnect_001 end" << std::endl;
}

/**
 * @tc.name  : MechbodyGattcWriteCharacteristic_001
 * @tc.number: MechbodyGattcWriteCharacteristic_001
 * @tc.desc  : MechbodyGattcWriteCharacteristic when data is nullptr.
 */
HWTEST_F(BleSendManagerTest, MechbodyGattcWriteCharacteristic_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest MechbodyGattcWriteCharacteristic_001 begin" << std::endl;
    uint8_t* data = nullptr;
    uint32_t dataLen = 0;
    int32_t result = bleSendManager_.MechbodyGattcWriteCharacteristic(data, dataLen);
    EXPECT_EQ(result, MECHBODY_GATT_INVALID_PARAM);
    DTEST_LOG << "BleSendManagerTest MechbodyGattcWriteCharacteristic_001 end" << std::endl;
}

/**
 * @tc.name  : MechbodyGattcWriteCharacteristic_002
 * @tc.number: MechbodyGattcWriteCharacteristic_002
 * @tc.desc  : MechbodyGattcWriteCharacteristic when gattClient_ is nullptr.
 */
HWTEST_F(BleSendManagerTest, MechbodyGattcWriteCharacteristic_002, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest MechbodyGattcWriteCharacteristic_002 begin" << std::endl;
    uint8_t data[BUF_MIN_LEN + 1] = {0};
    uint32_t dataLen = BUF_MIN_LEN + 1;
    bleSendManager_.gattClient_ = nullptr;
    int32_t result = bleSendManager_.MechbodyGattcWriteCharacteristic(data, dataLen);
    EXPECT_EQ(result, MECHBODY_GATT_INVALID_PARAM);
    DTEST_LOG << "BleSendManagerTest MechbodyGattcWriteCharacteristic_002 end" << std::endl;
}

/**
 * @tc.name  : MechbodyGattcWriteCharacteristic_003
 * @tc.number: MechbodyGattcWriteCharacteristic_003
 * @tc.desc  : MechbodyGattcWriteCharacteristic when gattClient_ is not nullptr.
 */
HWTEST_F(BleSendManagerTest, MechbodyGattcWriteCharacteristic_003, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest MechbodyGattcWriteCharacteristic_003 begin" << std::endl;
    uint8_t data[BUF_MIN_LEN + 1] = {0};
    uint32_t dataLen = BUF_MIN_LEN + 1;
    std::string mac = "mac";
    bleSendManager_.address_ = mac;
    BluetoothRemoteDevice device(bleSendManager_.address_, 1);
    bleSendManager_.gattClient_ = std::make_shared<OHOS::Bluetooth::GattClient>(device);
    int32_t result = bleSendManager_.MechbodyGattcWriteCharacteristic(data, dataLen);
    EXPECT_EQ(result, ERROR_INVALID_PARAMETERS);
    DTEST_LOG << "BleSendManagerTest MechbodyGattcWriteCharacteristic_003 end" << std::endl;
}

/**
 * @tc.name  : MechbodyGattcWriteCharacteristic_004
 * @tc.number: MechbodyGattcWriteCharacteristic_004
 * @tc.desc  : MechbodyGattcWriteCharacteristic when gattClient_ is nullptr.
*/
HWTEST_F(BleSendManagerTest, MechbodyGattcWriteCharacteristic_004, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest MechbodyGattcWriteCharacteristic_004 begin" << std::endl;
    uint8_t data[BUF_MIN_LEN + 1] = {0};
    uint32_t dataLen = BUF_MIN_LEN - 1;
    int32_t result = bleSendManager_.MechbodyGattcWriteCharacteristic(data, dataLen);
    EXPECT_EQ(result, MECHBODY_GATT_INVALID_PARAM);
    DTEST_LOG << "BleSendManagerTest MechbodyGattcWriteCharacteristic_004 end" << std::endl;
}

/**
 * @tc.name  : SendData_001
 * @tc.number: SendData_001
 * @tc.desc  : SendData when data is nullptr.
 */
HWTEST_F(BleSendManagerTest, SendData_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest SendData_001 begin" << std::endl;
    uint8_t* data = nullptr;
    uint32_t dataLen = 0;
    int32_t result = bleSendManager_.SendData(data, dataLen);
    EXPECT_EQ(result, MECHBODY_GATT_INVALID_PARAM);
    DTEST_LOG << "BleSendManagerTest SendData_001 end" << std::endl;
}

/**
 * @tc.name  : SendData_002
 * @tc.number: SendData_002
 * @tc.desc  : SendData when dataLen is less than BUF_MIN_LEN.
 */
HWTEST_F(BleSendManagerTest, SendData_002, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest SendData_002 begin" << std::endl;
    uint8_t data[BUF_LESS_THAN_MIN_LEN];
    uint32_t dataLen = BUF_LESS_THAN_MIN_LEN;
    int32_t result = bleSendManager_.SendData(data, dataLen);
    EXPECT_EQ(result, MECHBODY_GATT_INVALID_PARAM);
    DTEST_LOG << "BleSendManagerTest SendData_002 end" << std::endl;
}

/**
 * @tc.name  : SendData_003
 * @tc.number: SendData_003
 * @tc.desc  : SendData when dataLen is less than BUF_MIN_LEN.
 */
HWTEST_F(BleSendManagerTest, SendData_003, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest SendData_003 begin" << std::endl;
    uint8_t data[BUF_GREATER_THAN_MAX_LEN];
    uint32_t dataLen = BUF_GREATER_THAN_MAX_LEN;
    int32_t result = bleSendManager_.SendData(data, dataLen);
    EXPECT_EQ(result, MECHBODY_GATT_INVALID_PARAM);
    DTEST_LOG << "BleSendManagerTest SendData_003 end" << std::endl;
}

/**
 * @tc.name  : OnScanCallback_001`
 * @tc.number: OnScanCallback_001
 * @tc.desc  : SendData when dataLen is less than BUF_MIN_LEN.
 */
HWTEST_F(BleSendManagerTest, OnScanCallback_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest OnScanCallback_001 begin" << std::endl;
    Bluetooth::BleScanResult result;
    BleCentralManagerCallbackImpl callbackImpl;
    EXPECT_NO_FATAL_FAILURE(callbackImpl.OnScanCallback(result));
    DTEST_LOG << "BleSendManagerTest OnScanCallback_001 end" << std::endl;
}

/**
 * @tc.name  : OnConnectionStateChanged_001
 * @tc.number: OnConnectionStateChanged_001
 * @tc.desc  : Test OnConnectionStateChanged function.
 */
HWTEST_F(BleSendManagerTest, OnConnectionStateChanged_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest OnConnectionStateChanged_001 begin" << std::endl;
    bleSendManager_.gattClient_ = nullptr;
    int connectionState = 2;
    int ret = 0;
    MechInfo mechInfo;
    EXPECT_NO_FATAL_FAILURE(bleSendManager_.OnConnectionStateChanged(connectionState, ret, mechInfo));

    connectionState = 3;
    EXPECT_NO_FATAL_FAILURE(bleSendManager_.OnConnectionStateChanged(connectionState, ret, mechInfo));

    connectionState = 1;
    std::string mac = "mac";
    bleSendManager_.address_ = mac;
    BluetoothRemoteDevice device(bleSendManager_.address_, 1);
    bleSendManager_.gattClient_ = std::make_shared<OHOS::Bluetooth::GattClient>(device);
    EXPECT_NO_FATAL_FAILURE(bleSendManager_.OnConnectionStateChanged(connectionState, ret, mechInfo));
    DTEST_LOG << "BleSendManagerTest OnConnectionStateChanged_001 end" << std::endl;
}

/**
 * @tc.name  : OnServicesDiscovered_001
 * @tc.number: OnServicesDiscovered_001
 * @tc.desc  : Test OnServicesDiscovered function.
 */
HWTEST_F(BleSendManagerTest, OnServicesDiscovered_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest OnServicesDiscovered_001 begin" << std::endl;
    std::string mac = "mac";
    bleSendManager_.address_ = mac;
    BluetoothRemoteDevice device(bleSendManager_.address_, 1);
    bleSendManager_.gattClient_ = nullptr;
    auto callback = std::make_shared<BleGattClientCallback>();
    int status = -1;
    EXPECT_NO_FATAL_FAILURE(callback->OnServicesDiscovered(status));

    status = OHOS::Bluetooth::GattStatus::GATT_SUCCESS;
    EXPECT_NO_FATAL_FAILURE(callback->OnServicesDiscovered(status));

    bleSendManager_.gattClient_ = std::make_shared<OHOS::Bluetooth::GattClient>(device);
    EXPECT_NO_FATAL_FAILURE(callback->OnServicesDiscovered(status));
    DTEST_LOG << "BleSendManagerTest OnServicesDiscovered_001 end" << std::endl;
}

/**
 * @tc.name  : OnCharacteristicWriteResult_001
 * @tc.number: OnCharacteristicWriteResult_001
 * @tc.desc  : Test OnCharacteristicWriteResult function.
 */
HWTEST_F(BleSendManagerTest, OnCharacteristicWriteResult_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest OnCharacteristicWriteResult_001 begin" << std::endl;
    auto callback = std::make_shared<BleGattClientCallback>();
    GattCharacteristic characteristic = GattCharacteristic(MECHBODY_CHARACTERISTIC_NOTIFY_UUID, 0, 0, 0);
    int ret = OHOS::Bluetooth::GattStatus::GATT_SUCCESS;
    EXPECT_NO_FATAL_FAILURE(callback->OnCharacteristicWriteResult(characteristic, ret));

    ret = OHOS::Bluetooth::GattStatus::GATT_FAILURE;
    EXPECT_NO_FATAL_FAILURE(callback->OnCharacteristicWriteResult(characteristic, ret));

    ret = OHOS::Bluetooth::GattStatus::WRITE_NOT_PERMITTED;
    EXPECT_NO_FATAL_FAILURE(callback->OnCharacteristicWriteResult(characteristic, ret));
    DTEST_LOG << "BleSendManagerTest OnCharacteristicWriteResult_001 end" << std::endl;
}

/**
 * @tc.name  : MechbodyGattcConnect_001
 * @tc.number: MechbodyGattcConnect_001
 * @tc.desc  : Test MechbodyGattcConnect function.
 */
HWTEST_F(BleSendManagerTest, MechbodyGattcConnect_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest MechbodyGattcConnect_001 begin" << std::endl;
    std::string mac = "mac";
    std::string deviceName = "deviceName";
    int ret = bleSendManager_.MechbodyGattcConnect(mac, deviceName);
    EXPECT_EQ(ret, MECHBODY_GATT_CONNECT_FAILED);
    DTEST_LOG << "BleSendManagerTest MechbodyGattcConnect_001 end" << std::endl;
}

/**
 * @tc.name  : MechbodyGattcConnect_002
 * @tc.number: MechbodyGattcConnect_002
 * @tc.desc  : Test MechbodyGattcConnect function.
*/
HWTEST_F(BleSendManagerTest, MechbodyGattcConnect_002, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest MechbodyGattcConnect_002 begin" << std::endl;
    std::string mac = "123456789";
    std::string deviceName = "deviceName";
    bleSendManager_.bleGattClientCallBack_ = std::make_shared<BleGattClientCallback>();
    int ret = bleSendManager_.MechbodyGattcConnect(mac, deviceName);
    EXPECT_EQ(ret, MECHBODY_GATT_CONNECT_FAILED);

    MechInfo mechInfo;
    ret = bleSendManager_.MechbodyGattcConnect(mac, deviceName);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "BleSendManagerTest MechbodyGattcConnect_002 end" << std::endl;
}

/**
 * @tc.name  : UnInit_001
 * @tc.number: UnInit_001
 * @tc.desc  : Test UnInit function.
 */
HWTEST_F(BleSendManagerTest, UnInit_001, TestSize.Level1)
{
    DTEST_LOG << "BleSendManagerTest UnInit_001 begin" << std::endl;
    std::string mac = "mac";
    bleSendManager_.address_ = mac;
    BluetoothRemoteDevice device(bleSendManager_.address_, 1);
    auto callback = std::make_shared<BleGattClientCallback>();
    bleSendManager_.gattClient_ = std::make_shared<OHOS::Bluetooth::GattClient>(device);
    bleSendManager_.bleGattClientCallBack_ = callback;

    EXPECT_NO_FATAL_FAILURE(bleSendManager_.UnInit());
    DTEST_LOG << "BleSendManagerTest UnInit_001 end" << std::endl;
}
}
}