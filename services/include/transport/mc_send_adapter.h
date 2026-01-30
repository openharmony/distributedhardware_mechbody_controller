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

#ifndef OHOS_MECH_SEND_MANAGER_H
#define OHOS_MECH_SEND_MANAGER_H

#include <condition_variable>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <string>
#include <sys/prctl.h>
#include <thread>

#include "ble_send_manager.h"
#include "event_handler.h"
#include "mc_subscription_center.h"

namespace OHOS {
namespace MechBodyController {
class TransportSendAdapter : public std::enable_shared_from_this<TransportSendAdapter> {
public:
    explicit TransportSendAdapter();
    virtual ~TransportSendAdapter();
    int32_t SendCommand(const std::shared_ptr<CommandBase> &cmd, int32_t delayMs = 0);
    int32_t RegisterBluetoothListener();
    int32_t UnRegisterBluetoothListener();
    int32_t OnReceive(bool isAck, uint16_t seqNo, std::shared_ptr<MechDataBuffer> dataBuffer);

private:
    int32_t PushResponseTask(const std::shared_ptr<CommandBase> &cmd, uint16_t seqNo);
    int32_t ExeResponseTask(uint16_t seqNo, const std::shared_ptr<MechDataBuffer>& mechDataBuffer);
    int32_t ExeRespTimeoutTask(uint16_t seqNo);
    int32_t ExeRespTimeoutResetTask(uint16_t seqNo);
    int32_t RemoveRespTimeoutTask(uint16_t seqNo);
    void StartSendEvent();
    void StartRecvEvent();
    uint16_t CreateResponseSeqNo();
    const std::shared_ptr<CommandBase> GetCmdBySeqNo(uint16_t seqNo);

private:
    std::mutex sendEventMutex_;
    std::thread sendEventThread_;
    std::condition_variable sendEventCon_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> sendEventHandler_;

    std::mutex recvEventMutex_;
    std::thread recvEventThread_;
    std::condition_variable recvEventCon_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> recvEventHandler_;
    std::shared_mutex responseMutex_;
    std::map<uint16_t, std::shared_ptr<CommandBase>> pendingRequests_;
    uint16_t lastSeqNo_ = 0;
    std::shared_ptr<BleReceviceListener> receviceListener_;
};

class BleReceviceListenerImpl : public BleReceviceListener {
public:
    virtual ~BleReceviceListenerImpl() = default;
    BleReceviceListenerImpl(std::shared_ptr<TransportSendAdapter> sendAdapter);
    int32_t OnReceive(const uint8_t *data, uint32_t dataLen) override;

private:
    std::shared_ptr<TransportSendAdapter> sendAdapter_;
};
}  // namespace MechBodyController
}  // namespace OHOS
#endif  // OHOS_MECH_SEND_MANAGER_H
