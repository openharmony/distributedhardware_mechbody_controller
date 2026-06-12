/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "resource_manager_utils.h"

#include "mechbody_controller_log.h"
#include "locale_config.h"

namespace OHOS {
namespace MechBodyController {
namespace {
    const std::string TAG = "ResourceManagerUtils";
}
using namespace Global::Resource;

const char *HAP_PATH = "/system/app/DHardware_UI/DHardware_UI.hap";

std::mutex ResourceManagerUtils::resourceManagerMutex_;

void ResourceManagerUtils::InitResourceManager()
{
    if (!resConfig_) {
        resConfig_ = Global::Resource::CreateResConfig();
    }
    if (!resourceManager_) {
        HILOGI("Init");
        resourceManager_ = Global::Resource::CreateResourceManager(false);
        if (resourceManager_ == nullptr) {
            HILOGE("resourceManager new failed");
            return;
        }
        if (!resourceManager_->AddResource(HAP_PATH)) {
            HILOGE("AddResource failed");
            return;
        }
    }
    RefreshResConfig();
}

void ResourceManagerUtils::RefreshResConfig()
{
    std::string language = Global::I18n::LocaleConfig::GetSystemLanguage();
    if (language.empty()) {
        HILOGE("Get system language failed, skip RefreshResConfig");
        return;
    }
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(language, status);
    if (status != U_ZERO_ERROR || locale == nullptr) {
        HILOGE("forLanguageTag failed, errCode:%{public}d", status);
        return;
    }
    if (!resConfig_) {
        HILOGE("resConfig_ is nullptr");
        return;
    }
    resConfig_->SetLocaleInfo(locale.getLanguage(), locale.getScript(), locale.getCountry());
    if (!resourceManager_) {
        HILOGE("resourceManager_ is nullptr");
        return;
    }
    resourceManager_->UpdateResConfig(*resConfig_);
    HILOGI("Refresh success");
}

std::string ResourceManagerUtils::GetSystemStringByName(std::string name)
{
    std::lock_guard<std::mutex> lock(resourceManagerMutex_);
    InitResourceManager();

    std::string result;
    if (!resourceManager_) {
        HILOGE("resourceManager_ is nullptr");
        return result;
    }

    Global::Resource::RState rstate = resourceManager_->GetStringByName(name.c_str(), result);
    HILOGI("name: %{public}s, rstate: %{public}d", name.c_str(), static_cast<int32_t>(rstate));
    return result;
}

std::shared_ptr<Media::PixelMap> ResourceManagerUtils::GetPixelMapByName(std::string name)
{
    std::lock_guard<std::mutex> lock(resourceManagerMutex_);
    InitResourceManager();

    if (!resourceManager_) {
        HILOGE("resourceManager_ is nullptr");
        return nullptr;
    }

    size_t len = 0;
    std::unique_ptr<uint8_t[]> outValue;
    Global::Resource::RState rstate = resourceManager_->GetMediaDataByName(
        name.c_str(), len, outValue);
    HILOGI("rstate: %{public}d", static_cast<int32_t>(rstate));

    Media::SourceOptions opts;
    uint32_t errorCode = 0;
    std::unique_ptr<Media::ImageSource> imageSource =
        Media::ImageSource::CreateImageSource(outValue.get(), len, opts, errorCode);
    HILOGI("CreateImageSource errorCode: %{public}d", static_cast<int32_t>(errorCode));
    if (imageSource == nullptr) {
        HILOGE("imageSource is nullptr");
        return nullptr;
    }
    Media::DecodeOptions decodeOpts;
    std::unique_ptr<Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HILOGI("CreatePixelMap errorCode: %{public}d", static_cast<int32_t>(errorCode));

    std::shared_ptr<Media::PixelMap> iconPixelMap = std::move(pixelMap);
    return iconPixelMap;
}
}
}