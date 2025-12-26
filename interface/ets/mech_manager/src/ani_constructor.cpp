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

#include "mechbody_controller_log.h"
#include "ohos.distributedHardware.mechanicManager.ani.hpp"

#if __has_include(<ani.h>)
#include <ani.h>
#elif __has_include(<ani/ani.h>)
#include <ani/ani.h>
#else
#error "ani.h not found. Please ensure the Ani SDK is correctly installed."
#endif

namespace {
    const std::string TAG = "AniConstructor";
}

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    if (!vm) {
        HILOGE("vm is null");
        return ANI_ERROR;
    }
    if (!result) {
        HILOGE("result is null");
        return ANI_ERROR;
    }
    ani_env *env;
    int32_t ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ANI_OK != ret) {
        HILOGE("Failed to get ANI environment, error: %{public}d", ret);
        return ANI_ERROR;
    }
    ret = ohos::distributedHardware::mechanicManager::ANIRegister(env);
    if (ANI_OK != ret) {
        HILOGE("ANIRegister failed, error: %{public}d", ret);
        return ANI_ERROR;
    }
    *result = ANI_VERSION_1;
    return ANI_OK;
}
