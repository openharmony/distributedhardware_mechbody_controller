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

namespace OHOS {
namespace MechBodyController {
static constexpr uint32_t OPT_SIZE = 80;
void RegisterMechCameraKeyEventCmdWithAllEventsFuzzTest(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    RegisterMechCameraKeyEventCmd cmd;
    auto buffer = cmd.Marshal();
    size_t dataSize = 10;
    std::shared_ptr<MechDataBuffer> dataBuffer = std::make_shared<MechDataBuffer>(dataSize);
    if (dataBuffer != nullptr) {
        dataBuffer->SetRange(0, dataSize);
        int16_t stickX = provider.ConsumeIntegral<int16_t>();
        int16_t stickY = provider.ConsumeIntegral<int16_t>();
        size_t offset = 0;
        if (dataBuffer->Data() != nullptr) {
            uint8_t reserved = 0;
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved, sizeof(reserved));
            offset += sizeof(reserved);
            memcpy_s(dataBuffer->Data() + offset, dataSize - offset, &reserved, sizeof(reserved));
            offset += sizeof(reserved);
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
    cmd.TriggerResponse(dataBuffer);
    bool unmarshalResult = cmd.Unmarshal(dataBuffer);
    CameraKeyEvent event = cmd.GetEvent();
    int16_t stickX = cmd.GetStickX();
    int16_t stickY = cmd.GetStickY();
    uint8_t result = cmd.GetResult();

    (void)unmarshalResult;
    (void)event;
    (void)stickX;
    (void)stickY;
    (void)result;
}

} // namespace MechBodyController
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    size_t optSize = std::min(size, static_cast<size_t>(OHOS::MechBodyController::OPT_SIZE));
    FuzzedDataProvider fdp(data + size - optSize, optSize);
    OHOS::MechBodyController::RegisterMechCameraKeyEventCmdWithAllEventsFuzzTest(
        data, size - optSize);
    return 0;
}