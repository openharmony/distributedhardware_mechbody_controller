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

#ifndef BLE_SEND_MANAGER_TEST_H
#define BLE_SEND_MANAGER_TEST_H

#include <gtest/gtest.h>
#include "ble_send_manager.h"

namespace OHOS {
namespace MechBodyController {

class BleSendManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<BleSendManager> bleSendManager_;
};

class BleGattClientCallbackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<BleGattClientCallback> bleGattClientCallback_;
};

class RemoteDeviceObserverTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<RemoteDeviceObserver> remoteDeviceObserver_;
};

class HostObserverTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<HostObserver> hostObserver_;
};

class HidObserverTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<HidObserver> hidObserver_;
};

class BluetoothServiceStatusChangeListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<BluetoothServiceStatusChangeListener> bluetoothServiceStatusChangeListener_;
};

} // namespace MechBodyController
} // namespace OHOS

#endif // BLE_SEND_MANAGER_TEST_H