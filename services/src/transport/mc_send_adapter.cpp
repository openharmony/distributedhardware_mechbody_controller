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

#include "mc_send_adapter.h"

#include <map>
#include <string>
#include <securec.h>

#include "mechbody_controller_log.h"
#include "mc_command_factory.h"
#include "mc_connect_manager.h"
#include "mc_data_buffer.h"
#include "mc_protocol_convertor.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "TransportSendAdapter";
    const std::string RESPONSE_TIMEOUT_TASK = "transport_send_task";
    const std::string RESPONSE_TIMEOUT_RESET_TASK = "transport_send_task_reset";
    constexpr int32_t RESPONSE_TIMEOUT = 10000;
    constexpr int32_t RESPONSE_TIMEOUT_RESET = 2000;
    constexpr int32_t CMD_RETRY_INTERVAL = 10; //ms
    constexpr int32_t CMD_DROP_INTERVAL = 70000; //us
}

TransportSendAdapter::TransportSendAdapter()
{
    HILOGI("called");
    if (sendEventHandler_ != nullptr && recvEventHandler_ != nullptr) {
        HILOGI("already inited, end.");
        return;
    }
    sendEventThread_ = std::thread(&TransportSendAdapter::StartSendEvent, this);
    std::unique_lock<std::mutex> sendLock(sendEventMutex_);
    sendEventCon_.wait(sendLock, [this] {
        return sendEventHandler_ != nullptr;
    });

    recvEventThread_ = std::thread(&TransportSendAdapter::StartRecvEvent, this);
    std::unique_lock<std::mutex> recvLock(recvEventMutex_);
    recvEventCon_.wait(recvLock, [this] {
        return recvEventHandler_ != nullptr;
    });

    HILOGI("end");
}

void TransportSendAdapter::StartSendEvent()
{
    HILOGI("called");
    prctl(PR_SET_NAME, TAG.c_str());
    auto runner = AppExecFwk::EventRunner::Create(false);
    {
        std::lock_guard<std::mutex> lock(sendEventMutex_);
        sendEventHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    }
    sendEventCon_.notify_one();
    CHECK_POINTER_RETURN(runner, "runner");
    runner->Run();
    HILOGI("end");
}

void TransportSendAdapter::StartRecvEvent()
{
    HILOGI("called");
    prctl(PR_SET_NAME, TAG.c_str());
    auto runner = AppExecFwk::EventRunner::Create(false);
    {
        std::lock_guard<std::mutex> lock(recvEventMutex_);
        recvEventHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    }
    recvEventCon_.notify_one();
    CHECK_POINTER_RETURN(runner, "runner");
    runner->Run();
    HILOGI("end");
}

TransportSendAdapter::~TransportSendAdapter()
{
    HILOGI("called");
    if (sendEventHandler_ != nullptr) {
        sendEventHandler_->GetEventRunner()->Stop();
        HILOGI("prepare to clear thread resources");
        if (sendEventThread_.joinable()) {
            HILOGI("start clearing thread resources");
            sendEventThread_.join();
        }
        sendEventHandler_ = nullptr;
    } else {
        HILOGE("sendEventHandler is nullptr");
    }
    std::lock_guard<std::mutex> lock(recvEventMutex_);
    if (recvEventHandler_ != nullptr) {
        recvEventHandler_->GetEventRunner()->Stop();
        HILOGI("prepare to clear thread resources");
        if (recvEventThread_.joinable()) {
            HILOGI("start clearing thread resources");
            recvEventThread_.join();
        }
        recvEventHandler_ = nullptr;
    } else {
        HILOGE("recvEventHandler is nullptr");
    }
    UnRegisterBluetoothListener();
    HILOGI("end");
}

int32_t TransportSendAdapter::RegisterBluetoothListener()
{
    HILOGI("called");
    if (receviceListener_ == nullptr) {
        HILOGI("RegisterReceiveListener");
        receviceListener_ = std::make_shared<BleReceviceListenerImpl>(shared_from_this());
    }
    int32_t ret = BleSendManager::GetInstance().RegisterTransportSendAdapter(receviceListener_);
    if (ret != ERR_OK) {
        HILOGE("get RegisterBluetoothEventListener failed, ret: %{public}d", ret);
    }
    HILOGI("end.");
    return ERR_OK;
}

int32_t TransportSendAdapter::UnRegisterBluetoothListener()
{
    HILOGI("UnRegisterBluetoothListenercalled");
    if (receviceListener_ == nullptr) {
        HILOGI("UnRegisterReceiveListener ok.");
        return ERR_OK;
    }
    int32_t ret = BleSendManager::GetInstance().UnRegisterTransportSendAdapter(receviceListener_);
    if (ret != ERR_OK) {
        HILOGE("get RegisterBluetoothEventListener failed, ret: %{public}d", ret);
    }
    HILOGI("end.");
    return ERR_OK;
}

int32_t TransportSendAdapter::SendCommand(const std::shared_ptr<CommandBase> &cmd, int32_t delayMs)
{
    if (cmd == nullptr) {
        HILOGE("cmd is nullptr");
        return INVALID_PARAMETERS_ERR;
    }

    auto sendTask = [this, cmd]() {
        HILOGI("sendTask cmdType: 0x%{public}x", cmd->GetCmdType());
        if (MechConnectManager::GetInstance().GetLocalDeviceBleStatus() == false) {
            HILOGE("ble status error");
            return;
        }
        bool timeResult = DiscardExpiredData(cmd);
        if (timeResult) {
            return;
        }

        std::shared_ptr<MechDataBuffer> mechDataBuffer = cmd->Marshal();
        if (mechDataBuffer == nullptr) {
            HILOGE("mechDataBuffer is nullptr");
            return;
        }
        uint16_t seqNo = CreateResponseSeqNo();

        ProtocolConverter protocolConverter;
        OptType optType = cmd->NeedResponse() ? OptType::GET : OptType::PUT;
        auto buffer = protocolConverter.Convert(optType, seqNo, mechDataBuffer);
        if (buffer == nullptr) {
            HILOGE("buffer is nullptr");
            return;
        }
        int32_t result = BleSendManager::GetInstance().SendData(buffer->Data(), buffer->Size());
        HILOGI("sendTask cmdType: 0x%{public}x; dataLen= %{public}zu, result = %{public}d",
            cmd->GetCmdType(), buffer->Size(), result);
        std::this_thread::sleep_for(std::chrono::milliseconds(CMD_RETRY_INTERVAL));
        int32_t retry = cmd->GetRetryTimes();
        while (result != Bluetooth::BT_SUCCESS && retry > 0) {
            HILOGI("data send failed, retry.");
            retry--;
            result = BleSendManager::GetInstance().SendData(buffer->Data(), buffer->Size());
            HILOGI("sendTask cmdType: 0x%{public}x; dataLen= %{public}zu, result = %{public}d",
                cmd->GetCmdType(), buffer->Size(), result);
            std::this_thread::sleep_for(std::chrono::milliseconds(CMD_RETRY_INTERVAL));
        }

        if (cmd->NeedResponse()) {
            PushResponseTask(cmd, seqNo);
        }
    };

    CHECK_POINTER_RETURN_VALUE(sendEventHandler_, INVALID_PARAMETERS_ERR, "sendEventHandler_");
    sendEventHandler_->PostTask(sendTask, delayMs);

    HILOGI("end");
    return ERR_OK;
}

bool TransportSendAdapter::DiscardExpiredData(const std::shared_ptr<CommandBase> &cmd)
{
    HILOGI("called");
    int64_t cmdTime = cmd->GetTimestamp();
    if (cmdTime > 0) {
        std::chrono::microseconds nowUs =
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now().time_since_epoch());
        int64_t currentUs = nowUs.count();
        int64_t timeDiffUs = currentUs - cmdTime;
        HILOGI(
            "sendTask cmd time: %" PRId64 " us, current time: %" PRId64 " us, time diff: %" PRId64 " us",
            cmdTime,
            currentUs,
            timeDiffUs
        );
        // 70ms
        if (timeDiffUs > CMD_DROP_INTERVAL) {
            HILOGE("SendCommand expire 0222");
            return true;
        }
        return false;
    }
    return false;
}


int32_t BleReceviceListenerImpl::OnReceive(const uint8_t *data, uint32_t dataLen)
{
    HILOGD("called");
    ProtocolConverter protocolConverter;
    if (!protocolConverter.Validate(data, dataLen)) {
        HILOGE("protocolConverter Validate");
        return INVALID_PARAMETERS_ERR;
    }
    std::shared_ptr<MechDataBuffer> mechDataBuffer = std::make_shared<MechDataBuffer>(dataLen);
    if (mechDataBuffer == nullptr) {
        HILOGE("mechDataBuffer is nullptr");
        return INVALID_PARAMETERS_ERR;
    }
    mechDataBuffer->SetRange(0, dataLen);
    if (memcpy_s(mechDataBuffer->Data(), mechDataBuffer->Size(), data, dataLen) != 0) {
        HILOGE("memcpy failed");
        return INVALID_PARAMETERS_ERR;
    }
    uint16_t seqNo = 0;
    bool isAck = false;
    std::shared_ptr<MechDataBuffer> newMechDataBuffer = protocolConverter.GetData(mechDataBuffer, seqNo, isAck);
    return sendAdapter_->OnReceive(isAck, seqNo, newMechDataBuffer);
}

int32_t TransportSendAdapter::OnReceive(bool isAck, uint16_t seqNo, std::shared_ptr<MechDataBuffer> dataBuffer)
{
    CHECK_POINTER_RETURN_VALUE(dataBuffer, INVALID_PARAMETERS_ERR, "dataBuffer");
    if (!isAck) {
        CommandFactory commandFactory;
        std::shared_ptr<CommandBase> cmd = commandFactory.CreateFromData(dataBuffer);
        if (cmd == nullptr) {
            HILOGE("cmd is nullptr");
            return ERR_OK;
        }
        auto recvTask = [this, cmd]() {
            SubscriptionCenter::GetInstance().Notify(cmd);
        };
        std::lock_guard<std::mutex> lock(recvEventMutex_);
        if (recvEventHandler_ == nullptr) {
            HILOGE("eventHandler is nullptr");
            return ERR_OK;
        }
        recvEventHandler_->PostTask(recvTask);
        return ERR_OK;
    }

    auto responseTask = [this, seqNo, dataBuffer]() {
        ExeResponseTask(seqNo, dataBuffer);
    };
    std::lock_guard<std::mutex> lock(recvEventMutex_);
    if (recvEventHandler_ == nullptr) {
        HILOGE("eventHandler is nullptr");
        return ERR_OK;
    }
    recvEventHandler_->PostTask(responseTask);
    const std::string taskName = RESPONSE_TIMEOUT_TASK + std::to_string(seqNo);
    recvEventHandler_->RemoveTask(taskName);
    return ERR_OK;
}

BleReceviceListenerImpl::BleReceviceListenerImpl(std::shared_ptr<TransportSendAdapter> sendAdapter)
{
    sendAdapter_ = sendAdapter;
}

int32_t TransportSendAdapter::PushResponseTask(const std::shared_ptr<CommandBase> &cmd, uint16_t seqNo)
{
    if (cmd == nullptr) {
        HILOGE("cmd is nullptr");
        return INVALID_PARAMETERS_ERR;
    }
    {
        std::unique_lock<std::shared_mutex> responseReadLock(responseMutex_);
        pendingRequests_.insert(std::make_pair(seqNo, cmd));
    }
    HILOGI("cmdType : 0x%{public}x, seqNo: %{public}d", cmd->GetCmdType(), seqNo);
    std::lock_guard<std::mutex> lock(recvEventMutex_);
    if (recvEventHandler_ == nullptr) {
        HILOGE("eventHandler is nullptr");
        return INVALID_PARAMETERS_ERR;
    }
    auto responseTimeoutTask = [this, seqNo]() {
        ExeRespTimeoutTask(seqNo);
    };
    auto responseTimeoutResetTask = [this, seqNo]() {
        ExeRespTimeoutResetTask(seqNo);
    };
    
    const std::string taskName = RESPONSE_TIMEOUT_TASK + std::to_string(seqNo);
    const std::string taskNameReset = RESPONSE_TIMEOUT_RESET_TASK + std::to_string(seqNo);
    recvEventHandler_->PostTask(responseTimeoutResetTask, taskNameReset, RESPONSE_TIMEOUT_RESET);
    recvEventHandler_->PostTask(responseTimeoutTask, taskName, RESPONSE_TIMEOUT);
    HILOGI("end");
    return ERR_OK;
}

uint16_t TransportSendAdapter::CreateResponseSeqNo()
{
    HILOGI("called");
    std::unique_lock<std::shared_mutex> responseReadLock(responseMutex_);
    if (lastSeqNo_ >= UINT16_MAX) {
        HILOGE("the seqNo is full, reorder the seqNo.");
        lastSeqNo_ = 0;
        return 0;
    }
    uint16_t seqNo = ++lastSeqNo_;
    HILOGI("end, new seqNo is %{public}d", seqNo);
    return seqNo;
}

const std::shared_ptr<CommandBase> TransportSendAdapter::GetCmdBySeqNo(uint16_t seqNo)
{
    HILOGI("called");
    std::unique_lock<std::shared_mutex> responseReadLock(responseMutex_);
    if (pendingRequests_.count(seqNo) != 0) {
        return pendingRequests_[seqNo];
    }
    HILOGI("end");
    return nullptr;
}

int32_t TransportSendAdapter::ExeResponseTask(uint16_t seqNo, const std::shared_ptr<MechDataBuffer>& mechDataBuffer)
{
    CHECK_POINTER_RETURN_VALUE(mechDataBuffer, INVALID_PARAMETERS_ERR, "mechDataBuffer");
    HILOGI("called, seqNo: %{public}d", seqNo);
    {
        std::unique_lock<std::shared_mutex> responseWriteLock(responseMutex_);
        auto it = pendingRequests_.find(seqNo);
        if (it != pendingRequests_.end() && it->second != nullptr) {
            it->second->TriggerResponse(mechDataBuffer);
            pendingRequests_.erase(it);
        }
    }
    HILOGI("end");
    return ERR_OK;
}

int32_t TransportSendAdapter::ExeRespTimeoutTask(uint16_t seqNo)
{
    HILOGI("called, seqNo: %{public}d", seqNo);
    {
        std::unique_lock<std::shared_mutex> responseWriteLock(responseMutex_);
        auto it = pendingRequests_.find(seqNo);
        if (it != pendingRequests_.end() && it->second != nullptr) {
            it->second->TriggerTimeout();
            pendingRequests_.erase(it);
        }
    }
    HILOGI("end");
    return ERR_OK;
}

int32_t TransportSendAdapter::ExeRespTimeoutResetTask(uint16_t seqNo)
{
    HILOGI("called, seqNo: %{public}d", seqNo);
    {
        std::unique_lock<std::shared_mutex> responseWriteLock(responseMutex_);
        auto it = pendingRequests_.find(seqNo);
        if (it != pendingRequests_.end() && it->second != nullptr) {
            it->second->TriggerTimeoutReset();
        }
    }
    HILOGI("end");
    return ERR_OK;
}

int32_t TransportSendAdapter::RemoveRespTimeoutTask(uint16_t seqNo)
{
    HILOGI("called");
    {
        std::unique_lock<std::shared_mutex> responseWriteLock(responseMutex_);
        auto it = pendingRequests_.find(seqNo);
        if (it != pendingRequests_.end()) {
            pendingRequests_.erase(it);
        }
    }
    HILOGI("end");
    return ERR_OK;
}
}  // namespace MechBodyController
}  // namespace OHOS
