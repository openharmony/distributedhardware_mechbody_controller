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

#ifndef OHOS_LOAD_MECHBODY_ADAPTER_H
#define OHOS_LOAD_MECHBODY_ADAPTER_H

#include <functional>
#include <mutex>
#include <string>

namespace OHOS {
namespace MechBodyController {
using PushXYFn = std::function<void(float x, float y)>;
typedef void (*InitTrackingCore)();
typedef int32_t (*RunTrackingCore)(float, float, float, float, const PushXYFn&);

class MechbodyAdapterUtils {
public:
    static int32_t InitTrackingCore();
    static int32_t RunTrackingCore(float x, float y, float width, float height, const PushXYFn& push);
    static void Clear();

private:
    static std::mutex mechAdapterMutex_;
    static void* mechAdapterHandle_;
    static OHOS::MechBodyController::InitTrackingCore initFunc_;
    static OHOS::MechBodyController::RunTrackingCore runFunc_;
};
} // namespace MechBodyController
} // namespace OHOS
#endif // OHOS_LOAD_MECHBODY_ADAPTER_H