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

#ifndef OHOS_RESOURCE_MANAGER_UTILS_H
#define OHOS_RESOURCE_MANAGER_UTILS_H

#include "resource_manager.h"
#include "image_source.h"
#include "pixel_map.h"

#include <mutex>

namespace OHOS {

class ResourceManagerUtils {
public:
    static std::string GetSystemStringByName(std::string name);
    static std::shared_ptr<Media::PixelMap> GetPixelMapByName(std::string name);

private:
    static void InitResourceManager();

private:
    static std::mutex resourceManagerMutex_;
    inline static Global::Resource::ResourceManager *resourceManager_ = nullptr;
};
} // namespace OHOS
#endif // OHOS_RESOURCE_MANAGER_UTILS_H