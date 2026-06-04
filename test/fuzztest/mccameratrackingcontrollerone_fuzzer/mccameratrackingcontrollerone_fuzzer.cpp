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
#include "mccameratrackingcontrollerone_fuzzer.h"
#include "mc_camera_tracking_controller.h"
#include "mechbody_controller_service.h"
#include "mechbody_controller_types.h"
#include "mechbody_controller_enums.h"
#include <memory>
#include <iremote_object.h>

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

// Fuzz test constants
constexpr uint32_t MIN_INIT_COUNT = 1;
constexpr uint32_t MAX_INIT_COUNT = 3;
constexpr uint32_t MIN_GET_COUNT = 1;
constexpr uint32_t MAX_GET_COUNT = 5;
constexpr int32_t MIN_LAYOUT_TYPE = 0;
constexpr int32_t MAX_LAYOUT_TYPE = 2;

McCameraTrackingController* g_cameraTrackingController = nullptr;

void InitCameraTrackingController()
{
    if (g_cameraTrackingController == nullptr) {
        g_cameraTrackingController = &McCameraTrackingController::GetInstance();
    }
}

void FuzzInit(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    uint32_t initCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_INIT_COUNT, MAX_INIT_COUNT);
    for (uint32_t i = 0; i < initCount; i++) {
        g_cameraTrackingController->Init();
    }
}

void FuzzUnInit(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    uint32_t uninitCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_INIT_COUNT, MAX_INIT_COUNT);
    for (uint32_t i = 0; i < uninitCount; i++) {
        g_cameraTrackingController->UnInit();
    }
}

void FuzzSetTrackingEnabled(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    bool isEnabled = provider.ConsumeBool();
    g_cameraTrackingController->SetTrackingEnabled(tokenId, isEnabled);
}

void FuzzGetTrackingEnabled(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    bool isEnabled = false;
    g_cameraTrackingController->GetTrackingEnabled(tokenId, isEnabled);
}

void FuzzSetTrackingLayout(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    CameraTrackingLayout layout = static_cast<CameraTrackingLayout>(
        provider.ConsumeIntegralInRange<int32_t>(MIN_LAYOUT_TYPE, MAX_LAYOUT_TYPE));
    g_cameraTrackingController->SetTrackingLayout(layout);
}

void FuzzGetTrackingLayout(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    CameraTrackingLayout layout;
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        if (provider.ConsumeBool()) {
            g_cameraTrackingController->GetTrackingLayout(layout);
        }
    }
}

void FuzzSetStickOffset(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    int16_t stickX = provider.ConsumeIntegral<int16_t>();
    int16_t stickY = provider.ConsumeIntegral<int16_t>();
    g_cameraTrackingController->SetStickOffset(stickX, stickY);
}

void FuzzOnConnectChange(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    uint32_t changeCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < changeCount; i++) {
        if (provider.ConsumeBool()) {
            g_cameraTrackingController->OnConnectChange();
        }
    }
}

void FuzzUserIdChangeCallback(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    uint32_t callbackCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < callbackCount; i++) {
        g_cameraTrackingController->UserIdChangeCallback();
    }
}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzInit(data, size);
    FuzzUnInit(data, size);
    FuzzSetTrackingEnabled(data, size);
    FuzzGetTrackingEnabled(data, size);
    FuzzSetTrackingLayout(data, size);
    FuzzGetTrackingLayout(data, size);
    FuzzSetStickOffset(data, size);
    FuzzOnConnectChange(data, size);
    FuzzUserIdChangeCallback(data, size);
    return 0;
}