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
#include "mccameratrackingcontrollerfour_fuzzer.h"
#include "mc_camera_tracking_controller.h"
#include "mechbody_controller_service.h"
#include "mechbody_controller_types.h"
#include "mechbody_controller_enums.h"
#include <memory>
#include <iremote_object.h>

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

McCameraTrackingController* g_cameraTrackingController = nullptr;

constexpr uint32_t MIN_GET_COUNT = 1;
constexpr uint32_t MAX_GET_COUNT = 5;

void InitCameraTrackingController()
{
    if (g_cameraTrackingController == nullptr) {
        g_cameraTrackingController = &McCameraTrackingController::GetInstance();
    }
}

void FuzzSearchTargetStop(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    // 测试 searchingTarget 为 true 的情况
    auto cameraInfo = g_cameraTrackingController->GetCurrentCameraInfo();
    if (cameraInfo != nullptr) {
        cameraInfo->searchingTarget = true;
        cameraInfo->tokenId = provider.ConsumeIntegral<uint32_t>();
        cameraInfo->trackingTargetNum = provider.ConsumeIntegral<int32_t>();
    }
    g_cameraTrackingController->SearchTargetStop();

    // 测试 searchingTarget 为 true 且 motionManagers 为空的情况
    if (cameraInfo != nullptr) {
        cameraInfo->searchingTarget = true;
        cameraInfo->tokenId = provider.ConsumeIntegral<uint32_t>();
        cameraInfo->trackingTargetNum = provider.ConsumeIntegral<int32_t>();
    }
    // 清空 motionManagers
    {
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
        MechBodyControllerService::GetInstance().motionManagers_.clear();
    }
    g_cameraTrackingController->SearchTargetStop();
}

void FuzzGetTokenIdOfCurrentCameraInfo(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    uint32_t getCount = provider.ConsumeIntegralInRange<uint32_t>(MIN_GET_COUNT, MAX_GET_COUNT);
    for (uint32_t i = 0; i < getCount; i++) {
        g_cameraTrackingController->GetTokenIdOfCurrentCameraInfo();
    }
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzSearchTargetStop(data, size);
    FuzzGetTokenIdOfCurrentCameraInfo(data, size);
    return 0;
}