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

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>
#include <securec.h>
#include <cstdint>
#include <map>

#include "mcsendadapter_fuzzer.h"
#include "mc_send_adapter.h"
#include "mc_data_buffer.h"
#include "mc_protocol_convertor.h"
#include "mc_command_base.h"
#include "mechbody_controller_log.h"

using namespace OHOS::MechBodyController;

namespace {
    const std::string TAG = "McSendAdapterFuzzTest";
    const uint32_t MAX_DATA_SIZE = 4096;

    static void FuzzSendCommandBasic(FuzzedDataProvider& provider)
    {
        if (provider.remaining_bytes() < 1) {
            return;
        }
        bool useNullCmd = provider.ConsumeBool();
        int32_t delayMs = provider.ConsumeIntegral<int32_t>();

        if (useNullCmd) {
            std::shared_ptr<CommandBase> nullCmd = nullptr;
            TransportSendAdapter adapter;
            adapter.SendCommand(nullCmd, delayMs);
            return;
        }

        auto cmd = std::make_shared<MockCommand>();
        cmd->SetCmdSet(provider.ConsumeIntegral<uint8_t>());
        cmd->SetCmdId(provider.ConsumeIntegral<uint8_t>());
        cmd->SetNeedResponse(false);
        cmd->SetRetryTimes(provider.ConsumeIntegral<int32_t>());
        cmd->SetTimestamp(provider.ConsumeIntegral<int64_t>());

        TransportSendAdapter adapter;
        adapter.SendCommand(cmd, delayMs);
    }

    static void FuzzSendCommandWithResponse(FuzzedDataProvider& provider)
    {
        if (provider.remaining_bytes() < 1) {
            return;
        }
        auto cmd = std::make_shared<MockCommand>();
        int32_t rangeMax = 5;
        cmd->SetCmdSet(provider.ConsumeIntegral<uint8_t>());
        cmd->SetCmdId(provider.ConsumeIntegral<uint8_t>());
        cmd->SetNeedResponse(true);
        cmd->SetRetryTimes(provider.ConsumeIntegralInRange<int32_t>(0, rangeMax));
        cmd->SetTimestamp(provider.ConsumeIntegral<int64_t>());

        int32_t delayMs = provider.ConsumeIntegral<int32_t>();
        TransportSendAdapter adapter;
        adapter.SendCommand(cmd, delayMs);
    }

    static void FuzzDiscardExpiredData(FuzzedDataProvider& provider)
    {
        if (provider.remaining_bytes() < 1) {
            return;
        }
        auto cmd = std::make_shared<MockCommand>();
        cmd->SetTimestamp(provider.ConsumeIntegral<int64_t>());

        TransportSendAdapter adapter;
        adapter.DiscardExpiredData(cmd);
    }

    static void FuzzOnReceive(FuzzedDataProvider& provider)
    {
        if (provider.remaining_bytes() < 1) {
            return;
        }
        bool isAck = provider.ConsumeBool();
        uint16_t seqNo = provider.ConsumeIntegral<uint16_t>();
        bool useNullBuffer = provider.ConsumeBool();

        std::shared_ptr<MechDataBuffer> dataBuffer;
        if (!useNullBuffer) {
            uint32_t bufferSize = provider.ConsumeIntegralInRange<uint32_t>(1, MAX_DATA_SIZE);
            dataBuffer = std::make_shared<MechDataBuffer>(bufferSize);
            dataBuffer->SetRange(0, bufferSize);
        }

        TransportSendAdapter adapter;
        adapter.OnReceive(isAck, seqNo, dataBuffer);
    }

    static void FuzzBleReceiveListenerOnReceive(FuzzedDataProvider& provider)
    {
        if (provider.remaining_bytes() < 1) {
            return;
        }
        bool useNullData = provider.ConsumeBool();
        uint32_t dataLen = provider.ConsumeIntegralInRange<uint32_t>(1, MAX_DATA_SIZE);

        std::shared_ptr<TransportSendAdapter> sendAdapter = std::make_shared<TransportSendAdapter>();
        sendAdapter->RegisterBluetoothListener();
        BleReceviceListenerImpl listener(sendAdapter);

        if (useNullData) {
            listener.OnReceive(nullptr, dataLen);
            return;
        }

        auto data = std::make_unique<uint8_t[]>(dataLen);
        for (uint32_t i = 0; i < dataLen && provider.remaining_bytes() > 0; i++) {
            data[i] = provider.ConsumeIntegral<uint8_t>();
        }
        listener.OnReceive(data.get(), dataLen);
        sendAdapter->UnRegisterBluetoothListener();
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    FuzzSendCommandBasic(provider);
    FuzzSendCommandWithResponse(provider);
    FuzzDiscardExpiredData(provider);
    FuzzOnReceive(provider);
    FuzzBleReceiveListenerOnReceive(provider);

    return 0;
}