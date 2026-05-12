/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "mcregistermechcamerakeyeventcmd_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <memory>
#include <securec.h>

#include "mc_register_mech_camera_key_event_cmd.h"
#include "mc_data_buffer.h"

constexpr uint8_t FUZZ_TEST_ZERO = 0;
constexpr uint8_t FUZZ_TEST_ONE = 1;
constexpr uint8_t FUZZ_TEST_TWO = 2;
constexpr uint8_t FUZZ_TEST_THREE = 3;
constexpr uint8_t FUZZ_TEST_FOUR = 4;
constexpr uint8_t FUZZ_TEST_FIVE = 5;
constexpr uint8_t FUZZ_TEST_SIX = 6;
constexpr uint8_t FUZZ_TEST_SEVEN = 7;
constexpr uint8_t FUZZ_TEST_EIGHT = 8;
constexpr uint8_t FUZZ_TEST_NINE = 9;

namespace OHOS {
namespace MechBodyController {
static constexpr uint32_t OPT_SIZE = 80;
FuzzedDataProvider *FDP;

void RegisterMechCameraKeyEventCmdMarshalFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    RegisterMechCameraKeyEventCmd cmd;
    auto buffer = cmd.Marshal();
    if (buffer != nullptr) {
        if (provider.ConsumeBool()) {
            uint8_t cmdSet = cmd.GetCmdSet();
            (void)cmdSet;
        }
        if (provider.ConsumeBool()) {
            uint8_t cmdId = cmd.GetCmdId();
            (void)cmdId;
        }
        if (provider.ConsumeBool()) {
            uint16_t reqSize = cmd.GetReqSize();
            (void)reqSize;
        }
        if (provider.ConsumeBool()) {
            uint16_t rspSize = cmd.GetRspSize();
            (void)rspSize;
        }
        if (provider.ConsumeBool()) {
            uint32_t timeoutMs = cmd.GetTimeoutMs();
            (void)timeoutMs;
        }
        if (provider.ConsumeBool()) {
            int32_t retryTimes = cmd.GetRetryTimes();
            (void)retryTimes;
        }
        if (provider.ConsumeBool()) {
            bool needResponse = cmd.NeedResponse();
            (void)needResponse;
        }
    }
}

void RegisterMechCameraKeyEventCmdUnmarshalFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechCameraKeyEventCmd cmd;

    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 20);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            dataBuffer->SetRange(0, dataSize);
            auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!bytes.empty() && dataBuffer->Data() != nullptr) {
                (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
            }
        }
    }

    bool result = cmd.Unmarshal(dataBuffer);
    CameraKeyEvent event = cmd.GetEvent();
    int16_t stickX = cmd.GetStickX();
    int16_t stickY = cmd.GetStickY();
    
    (void)result;
    (void)event;
    (void)stickX;
    (void)stickY;
}

void RegisterMechCameraKeyEventCmdTriggerResponseFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    RegisterMechCameraKeyEventCmd cmd;
    
    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 20);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            dataBuffer->SetRange(0, dataSize);
            auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!bytes.empty() && dataBuffer->Data() != nullptr) {
                (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
            }
        }
    }
    
    cmd.TriggerResponse(dataBuffer);
}

void RegisterMechCameraKeyEventCmdGetEventFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechCameraKeyEventCmd cmd;

    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 20);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            dataBuffer->SetRange(0, dataSize);
            auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!bytes.empty() && dataBuffer->Data() != nullptr) {
                (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
            }
        }
    }

    cmd.Unmarshal(dataBuffer);
    CameraKeyEvent event = cmd.GetEvent();
    (void)event;
}

void RegisterMechCameraKeyEventCmdGetStickXFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechCameraKeyEventCmd cmd;

    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 20);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            dataBuffer->SetRange(0, dataSize);
            auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!bytes.empty() && dataBuffer->Data() != nullptr) {
                (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
            }
        }
    }

    cmd.Unmarshal(dataBuffer);
    int16_t stickX = cmd.GetStickX();
    (void)stickX;
}

void RegisterMechCameraKeyEventCmdGetStickYFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechCameraKeyEventCmd cmd;

    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 20);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            dataBuffer->SetRange(0, dataSize);
            auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!bytes.empty() && dataBuffer->Data() != nullptr) {
                (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
            }
        }
    }

    cmd.Unmarshal(dataBuffer);
    int16_t stickY = cmd.GetStickY();
    (void)stickY;
}

void RegisterMechCameraKeyEventCmdWithCallbackFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    
    RegisterMechCameraKeyEventCmd cmd;
    
    bool shouldSetCallback = provider.ConsumeBool();
    if (shouldSetCallback) {
        cmd.SetResponseCallback([]() {
            // Callback function
        });
    }
    
    bool shouldSetNull = provider.ConsumeBool();
    std::shared_ptr<MechDataBuffer> dataBuffer;
    if (!shouldSetNull) {
        size_t dataSize = provider.ConsumeIntegralInRange<size_t>(0, 20);
        dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
        if (dataBuffer != nullptr && dataSize > 0) {
            dataBuffer->SetRange(0, dataSize);
            auto bytes = provider.ConsumeBytes<uint8_t>(dataSize);
            if (!bytes.empty() && dataBuffer->Data() != nullptr) {
                (void)memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), bytes.data(), bytes.size());
            }
        }
    }
    
    cmd.TriggerResponse(dataBuffer);
}

void RegisterMechCameraKeyEventCmdWithValidDataFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechCameraKeyEventCmd cmd;

    size_t dataSize = 10; // RPT_SIZE + BIT_OFFSET_2 = 8 + 2
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);

        int16_t stickX = provider.ConsumeIntegral<int16_t>();
        int16_t stickY = provider.ConsumeIntegral<int16_t>();
        uint16_t controlByte1 = provider.ConsumeIntegral<uint16_t>();
        uint16_t controlByte2 = provider.ConsumeIntegral<uint16_t>();

        size_t offset = 0;
        if (dataBuffer->Data() != nullptr) {
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &stickX, sizeof(stickX));
            offset += sizeof(stickX);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &stickY, sizeof(stickY));
            offset += sizeof(stickY);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &controlByte1, sizeof(controlByte1));
            offset += sizeof(controlByte1);
            (void)memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &controlByte2, sizeof(controlByte2));
        }
    }

    bool result = cmd.Unmarshal(dataBuffer);
    CameraKeyEvent event = cmd.GetEvent();
    int16_t stickX = cmd.GetStickX();
    int16_t stickY = cmd.GetStickY();
    
    (void)result;
    (void)event;
    (void)stickX;
    (void)stickY;
}

void RegisterMechCameraKeyEventCmdWithAllEventsFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    RegisterMechCameraKeyEventCmd cmd;

    size_t dataSize = 10;
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);

        int16_t stickX = provider.ConsumeIntegral<int16_t>();
        int16_t stickY = provider.ConsumeIntegral<int16_t>();

        size_t offset = 0;
        if (dataBuffer->Data() != nullptr) {
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &stickX, sizeof(stickX));
            offset += sizeof(stickX);
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &stickY, sizeof(stickY));
            offset += sizeof(stickY);

            // Test different key events by setting different control byte patterns
            uint8_t eventPattern = provider.ConsumeIntegral<uint8_t>();
            uint16_t controlByte1 = static_cast<uint16_t>(eventPattern << 13);
            uint16_t controlByte2 = 0;

            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &controlByte1, sizeof(controlByte1));
            offset += sizeof(controlByte1);
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &controlByte2, sizeof(controlByte2));
        }
    }

    bool result = cmd.Unmarshal(dataBuffer);
    CameraKeyEvent event = cmd.GetEvent();
    int16_t stickX = cmd.GetStickX();
    int16_t stickY = cmd.GetStickY();
    
    (void)result;
    (void)event;
    (void)stickX;
    (void)stickY;
}

} // namespace MechBodyController
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    if (size < OHOS::MechBodyController::OPT_SIZE) {
        return 0;
    }
    FuzzedDataProvider fdp(data + size - OHOS::MechBodyController::OPT_SIZE, OHOS::MechBodyController::OPT_SIZE);
    // randomly select a test API
    uint8_t selector = fdp.ConsumeIntegral<uint8_t>() % FUZZ_TEST_NINE;
    switch (selector) {
        case FUZZ_TEST_ZERO:
            OHOS::MechBodyController::RegisterMechCameraKeyEventCmdMarshalFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_ONE:
            OHOS::MechBodyController::RegisterMechCameraKeyEventCmdUnmarshalFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_TWO:
            OHOS::MechBodyController::RegisterMechCameraKeyEventCmdTriggerResponseFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_THREE:
            OHOS::MechBodyController::RegisterMechCameraKeyEventCmdGetEventFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_FOUR:
            OHOS::MechBodyController::RegisterMechCameraKeyEventCmdGetStickXFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_FIVE:
            OHOS::MechBodyController::RegisterMechCameraKeyEventCmdGetStickYFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_SIX:
            OHOS::MechBodyController::RegisterMechCameraKeyEventCmdWithCallbackFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_SEVEN:
            OHOS::MechBodyController::RegisterMechCameraKeyEventCmdWithValidDataFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        case FUZZ_TEST_EIGHT:
            OHOS::MechBodyController::RegisterMechCameraKeyEventCmdWithAllEventsFuzzTest(
                data, size - OHOS::MechBodyController::OPT_SIZE);
            break;
        default:
            break;
    }
    return 0;
}