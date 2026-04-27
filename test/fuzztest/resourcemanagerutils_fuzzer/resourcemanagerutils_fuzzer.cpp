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
#include "resource_manager_utils.h"

using namespace OHOS;

namespace {

enum class TestFunctionId {
    FUZZ_GET_SYSTEM_STRING_BY_NAME = 0,
    FUZZ_GET_PIXEL_MAP_BY_NAME = 1,
    FUZZ_GET_STRING_EMPTY_NAME = 2,
    FUZZ_GET_PIXEL_MAP_EMPTY_NAME = 3,
    FUZZ_GET_STRING_SPECIAL_CHARS = 4,
    FUZZ_GET_PIXEL_MAP_SPECIAL_CHARS = 5,
    FUZZ_GET_STRING_LONG_NAME = 6,
    FUZZ_GET_PIXEL_MAP_LONG_NAME = 7,
    FUZZ_GET_STRING_MULTIPLE = 8,
    FUZZ_GET_PIXEL_MAP_MULTIPLE = 9,
    FUZZ_ALL_FUNCTIONS = 10,
};

void FuzzGetSystemStringByName(FuzzedDataProvider &provider)
{
    std::string name = provider.ConsumeRandomLengthString();
    ResourceManagerUtils::GetSystemStringByName(name);
}

void FuzzGetPixelMapByName(FuzzedDataProvider &provider)
{
    std::string name = provider.ConsumeRandomLengthString();
    ResourceManagerUtils::GetPixelMapByName(name);
}

void FuzzGetStringEmptyName(FuzzedDataProvider &provider)
{
    (void)provider;
    std::string emptyName = "";
    ResourceManagerUtils::GetSystemStringByName(emptyName);
}

void FuzzGetPixelMapEmptyName(FuzzedDataProvider &provider)
{
    (void)provider;
    std::string emptyName = "";
    ResourceManagerUtils::GetPixelMapByName(emptyName);
}

void FuzzGetStringSpecialChars(FuzzedDataProvider &provider)
{
    std::string specialChars = "!@#$%^&*()_+-=[]{}|;':\",./<>?";
    ResourceManagerUtils::GetSystemStringByName(specialChars);
}

void FuzzGetPixelMapSpecialChars(FuzzedDataProvider &provider)
{
    std::string specialChars = "!@#$%^&*()_+-=[]{}|;':\",./<>?";
    ResourceManagerUtils::GetPixelMapByName(specialChars);
}

void FuzzGetStringLongName(FuzzedDataProvider &provider)
{
    size_t length = provider.ConsumeIntegralInRange<size_t>(100, 1000);
    std::string longName(length, 'a');
    ResourceManagerUtils::GetSystemStringByName(longName);
}

void FuzzGetPixelMapLongName(FuzzedDataProvider &provider)
{
    size_t length = provider.ConsumeIntegralInRange<size_t>(100, 1000);
    std::string longName(length, 'a');
    ResourceManagerUtils::GetPixelMapByName(longName);
}

void FuzzGetStringMultiple(FuzzedDataProvider &provider)
{
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 5);

    for (int32_t i = 0; i < iterations; i++) {
        std::string name = provider.ConsumeRandomLengthString();
        ResourceManagerUtils::GetSystemStringByName(name);
    }
}

void FuzzGetPixelMapMultiple(FuzzedDataProvider &provider)
{
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 5);

    for (int32_t i = 0; i < iterations; i++) {
        std::string name = provider.ConsumeRandomLengthString();
        ResourceManagerUtils::GetPixelMapByName(name);
    }
}

void FuzzAllFunctions(FuzzedDataProvider &provider)
{
    std::string name = provider.ConsumeRandomLengthString();

    ResourceManagerUtils::GetSystemStringByName(name);
    ResourceManagerUtils::GetPixelMapByName(name);

    std::string emptyName = "";
    ResourceManagerUtils::GetSystemStringByName(emptyName);
    ResourceManagerUtils::GetPixelMapByName(emptyName);
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);

    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0, 10);

    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_GET_SYSTEM_STRING_BY_NAME:
            FuzzGetSystemStringByName(provider);
            break;
        case TestFunctionId::FUZZ_GET_PIXEL_MAP_BY_NAME:
            FuzzGetPixelMapByName(provider);
            break;
        case TestFunctionId::FUZZ_GET_STRING_EMPTY_NAME:
            FuzzGetStringEmptyName(provider);
            break;
        case TestFunctionId::FUZZ_GET_PIXEL_MAP_EMPTY_NAME:
            FuzzGetPixelMapEmptyName(provider);
            break;
        case TestFunctionId::FUZZ_GET_STRING_SPECIAL_CHARS:
            FuzzGetStringSpecialChars(provider);
            break;
        case TestFunctionId::FUZZ_GET_PIXEL_MAP_SPECIAL_CHARS:
            FuzzGetPixelMapSpecialChars(provider);
            break;
        case TestFunctionId::FUZZ_GET_STRING_LONG_NAME:
            FuzzGetStringLongName(provider);
            break;
        case TestFunctionId::FUZZ_GET_PIXEL_MAP_LONG_NAME:
            FuzzGetPixelMapLongName(provider);
            break;
        case TestFunctionId::FUZZ_GET_STRING_MULTIPLE:
            FuzzGetStringMultiple(provider);
            break;
        case TestFunctionId::FUZZ_GET_PIXEL_MAP_MULTIPLE:
            FuzzGetPixelMapMultiple(provider);
            break;
        case TestFunctionId::FUZZ_ALL_FUNCTIONS:
            FuzzAllFunctions(provider);
            break;
        default:
            break;
    }

    return 0;
}