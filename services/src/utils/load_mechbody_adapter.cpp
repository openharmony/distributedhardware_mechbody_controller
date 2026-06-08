/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
#include "load_mechbody_adapter.h"

#include <dlfcn.h>
#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "MechbodyAdapterUtils";
}

std::mutex MechbodyAdapterUtils::mechAdapterMutex_;
void* MechbodyAdapterUtils::mechAdapterHandle_ = nullptr;
std::mutex MechbodyAdapterUtils::mechGimbalMutex_;
void* MechbodyAdapterUtils::mechBackgroundHandle_ = nullptr;
OHOS::MechBodyController::InitTrackingCore MechbodyAdapterUtils::initFunc_ = nullptr;
OHOS::MechBodyController::RunTrackingCore MechbodyAdapterUtils::runFunc_ = nullptr;
OHOS::MechBodyController::ResetTrackingCore MechbodyAdapterUtils::resetFunc_ = nullptr;
OHOS::MechBodyController::RegisterBackgroundTracking MechbodyAdapterUtils::backgroundRegisterFunc_ = nullptr;
OHOS::MechBodyController::UnRegisterBackgroundTracking MechbodyAdapterUtils::backgroundUnRegisterFunc_ = nullptr;

int32_t MechbodyAdapterUtils::InitTrackingCore()
{
    HILOGI("start load mechbody adapter.");
    std::lock_guard<std::mutex> lock(mechAdapterMutex_);
#if (defined(__aarch64__) || defined(__x86_64__))
    char resolvedPath[100] = "/system/lib64/libmech_adapter.z.so";
    char gimbalPath[100] = "/system/lib64/librobustgimbaltracker_service.z.so";
#else
    char resolvedPath[100] = "/system/lib/libmech_adapter.z.so";
    char gimbalPath[100] = "/system/lib/librobustgimbaltracker_service.z.so";
#endif
    mechAdapterHandle_ = dlopen(resolvedPath, RTLD_LAZY);
    if (mechAdapterHandle_ == nullptr) {
        HILOGE("Failed to load mechbody adapter: %{public}s", dlerror());
        return LOAD_MECH_ADAPTER_SO_ERROR;
    }
    mechBackgroundHandle_ = dlopen(gimbalPath, RTLD_LAZY);
    if (mechBackgroundHandle_ == nullptr) {
        HILOGE("Failed to load mechbody gimbal: %{public}s", dlerror());
        return LOAD_MECH_ADAPTER_SO_ERROR;
    }
    dlerror();
    int32_t ret = LoadFunction();
    if (ret != ERR_OK) {
        HILOGE("Init remote mech adapter proxy failed, dlclose handle.");
        dlclose(mechAdapterHandle_);
        dlclose(mechBackgroundHandle_);
        mechAdapterHandle_ = nullptr;
        mechBackgroundHandle_ = nullptr;
        initFunc_ = nullptr;
        runFunc_ = nullptr;
        resetFunc_ = nullptr;
        backgroundRegisterFunc_ = nullptr;
        backgroundUnRegisterFunc_ = nullptr;
        return ret;
    }
    if (initFunc_ != nullptr) {
        HILOGI("Init exec.");
        initFunc_();
    }
    return ret;
}

int32_t MechbodyAdapterUtils::LoadFunction()
{
    initFunc_ = reinterpret_cast<OHOS::MechBodyController::InitTrackingCore>(dlsym(mechAdapterHandle_, "Init"));
    const char* dlsymError = dlerror();
    if (dlsymError != nullptr) {
        return LOAD_MECH_ADAPTER_FUNCTION_ERROR;
    }
    dlerror();
    runFunc_ = reinterpret_cast<OHOS::MechBodyController::RunTrackingCore>(dlsym(mechAdapterHandle_,
        "RunTrackingCore"));
    dlsymError = dlerror();
    if (dlsymError != nullptr) {
        return LOAD_MECH_ADAPTER_FUNCTION_ERROR;
    }
    dlerror();
    resetFunc_ = reinterpret_cast<OHOS::MechBodyController::ResetTrackingCore>(dlsym(mechAdapterHandle_, "Reset"));
    dlsymError = dlerror();
    if (dlsymError != nullptr) {
        return LOAD_MECH_ADAPTER_FUNCTION_ERROR;
    }
    backgroundRegisterFunc_ = reinterpret_cast<OHOS::MechBodyController::RegisterBackgroundTracking>(
        dlsym(mechBackgroundHandle_, "RegisterBackgroundTracking"));
    dlsymError = dlerror();
    if (dlsymError != nullptr) {
        return LOAD_MECH_ADAPTER_FUNCTION_ERROR;
    }
    backgroundUnRegisterFunc_ = reinterpret_cast<OHOS::MechBodyController::UnRegisterBackgroundTracking>(
        dlsym(mechBackgroundHandle_, "UnRegisterBackgroundTracking"));
    dlsymError = dlerror();
    if (dlsymError != nullptr) {
        return LOAD_MECH_ADAPTER_FUNCTION_ERROR;
    }
    return ERR_OK;
}

int32_t MechbodyAdapterUtils::RunTrackingCore(float x, float y, float width, float height, const PushXYFn& push)
{
    HILOGI("RunTrackingCore called with x:%{public}f, y:%{public}f, width:%{public}f, height:%{public}f",
        x, y, width, height);
    std::lock_guard<std::mutex> lock(mechAdapterMutex_);
    if (mechAdapterHandle_ == nullptr || runFunc_ == nullptr) {
        HILOGE("Failed to get RunTrackingCore function - handle or function is null.");
        return LOAD_MECH_ADAPTER_FUNCTION_ERROR;
    }
    HILOGI("RunTrackingCore exec.");
    return runFunc_(x, y, width, height, push);
}

int32_t MechbodyAdapterUtils::ResetTrackingCore()
{
    HILOGI("ResetTrackingCore called");
    std::lock_guard<std::mutex> lock(mechAdapterMutex_);
    if (mechAdapterHandle_ == nullptr || resetFunc_ == nullptr) {
        HILOGE("Failed to get ResetTrackingCore function - handle or function is null.");
        return LOAD_MECH_ADAPTER_FUNCTION_ERROR;
    }
    HILOGI("ResetTrackingCore exec.");
    resetFunc_();
    return ERR_OK;
}

void MechbodyAdapterUtils::RegisterBackgroundTracking(const GetBackgroundXYFn& GetBackgroundXYFn)
{
    HILOGI("RegisterBackgroundTracking called");
    std::lock_guard<std::mutex> lock(mechGimbalMutex_);

    HILOGI("RegisterBackgroundTracking exec.");
    backgroundRegisterFunc_(GetBackgroundXYFn);
}

void MechbodyAdapterUtils::UnRegisterBackgroundTracking()
{
    HILOGI("UnRegisterBackgroundTracking called");
    std::lock_guard<std::mutex> lock(mechGimbalMutex_);

    HILOGI("UnRegisterBackgroundTracking exec.");
    backgroundUnRegisterFunc_();
}

void MechbodyAdapterUtils::Clear()
{
    HILOGI("clear exec.");
    std::lock_guard<std::mutex> mechLock(mechAdapterMutex_);
    
    if (mechAdapterHandle_ != nullptr) {
        dlclose(mechAdapterHandle_);
        dlclose(mechBackgroundHandle_);
        mechAdapterHandle_ = nullptr;
        mechBackgroundHandle_ = nullptr;
    }
    initFunc_ = nullptr;
    runFunc_ = nullptr;
    resetFunc_ = nullptr;
    backgroundRegisterFunc_ = nullptr;
    backgroundUnRegisterFunc_ = nullptr;
}
} // namespace MechBodyController
} // namespace OHOS