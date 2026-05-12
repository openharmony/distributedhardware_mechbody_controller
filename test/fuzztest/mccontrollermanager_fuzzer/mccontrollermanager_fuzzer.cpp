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
#include "mccontrollermanager_fuzzer.h"
#include "mc_controller_manager.h"
#include "mechbody_controller_types.h"
#include "mechbody_controller_enums.h"

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

enum class TestFunctionId {
    FUZZ_SET_TRACKING_ENABLED = 0,
    FUZZ_GET_TRACKING_ENABLED = 1,
    FUZZ_SET_TRACKING_ENABLED_AND_GET = 2,
    FUZZ_SET_TRACKING_LAYOUT = 3,
    FUZZ_GET_TRACKING_LAYOUT = 4,
    FUZZ_SEARCH_TARGET_STOP = 5,
    FUZZ_SET_TRACKING_ENABLED_WITH_BOUNDARY_VALUES = 6,
    FUZZ_GET_TRACKING_ENABLED_WITH_BOUNDARY_VALUES = 7,
    FUZZ_SET_TRACKING_LAYOUT_WITH_BOUNDARY_VALUES = 8,
    FUZZ_SET_TRACKING_LAYOUT_WITH_ALL_ENUM_VALUES = 9,
    FUZZ_MULTIPLE_OPERATIONS = 10,
    FUZZ_SET_TRACKING_ENABLED_WITH_FALSE_VALUE = 11,
    FUZZ_SET_TRACKING_ENABLED_WITH_TRUE_VALUE = 12,
    FUZZ_SET_TRACKING_LAYOUT_WITH_DEFAULT = 13,
    FUZZ_SET_TRACKING_LAYOUT_WITH_LEFT = 14,
    FUZZ_SET_TRACKING_LAYOUT_WITH_MIDDLE = 15,
    FUZZ_SET_TRACKING_LAYOUT_WITH_RIGHT = 16,
    FUZZ_SEARCH_TARGET_STOP_DUPLICATE = 17,
    TEST_FUNCTION_ID_MAX = 17
};

void FuzzSetTrackingEnabled(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    bool isEnabled = provider.ConsumeBool();
    McControllerManager::GetInstance().SetTrackingEnabled(tokenId, isEnabled);
}

void FuzzGetTrackingEnabled(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    bool isEnabled = false;
    McControllerManager::GetInstance().GetTrackingEnabled(tokenId, isEnabled);
}

void FuzzSetTrackingEnabledAndGet(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    bool isEnabled = provider.ConsumeBool();
    McControllerManager::GetInstance().SetTrackingEnabled(tokenId, isEnabled);
    bool resultEnabled = false;
    McControllerManager::GetInstance().GetTrackingEnabled(tokenId, resultEnabled);
}

void FuzzSetTrackingLayout(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    CameraTrackingLayout layout = static_cast<CameraTrackingLayout>(
        provider.ConsumeIntegralInRange<int32_t>(0, 3));
    McControllerManager::GetInstance().SetTrackingLayout(tokenId, layout);
}

void FuzzGetTrackingLayout(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    CameraTrackingLayout layout = CameraTrackingLayout::DEFAULT;
    McControllerManager::GetInstance().GetTrackingLayout(tokenId, layout);
}

void FuzzSearchTargetStop(FuzzedDataProvider &provider)
{
    if (provider.ConsumeBool()) {
        McControllerManager::GetInstance().SearchTargetStop();
    }
}

void FuzzSetTrackingEnabledWithBoundaryValues(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeBool() ? 0 : UINT32_MAX;
    bool isEnabled = provider.ConsumeBool();
    McControllerManager::GetInstance().SetTrackingEnabled(tokenId, isEnabled);
}

void FuzzGetTrackingEnabledWithBoundaryValues(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeBool() ? 0 : UINT32_MAX;
    bool isEnabled = false;
    McControllerManager::GetInstance().GetTrackingEnabled(tokenId, isEnabled);
}

void FuzzSetTrackingLayoutWithBoundaryValues(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeBool() ? 0 : UINT32_MAX;
    CameraTrackingLayout layout = static_cast<CameraTrackingLayout>(
        provider.ConsumeIntegralInRange<int32_t>(0, 3));
    McControllerManager::GetInstance().SetTrackingLayout(tokenId, layout);
}

void FuzzSetTrackingLayoutWithAllEnumValues(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    
    CameraTrackingLayout layouts[] = {
        CameraTrackingLayout::DEFAULT,
        CameraTrackingLayout::LEFT,
        CameraTrackingLayout::MIDDLE,
        CameraTrackingLayout::RIGHT
    };
    
    CameraTrackingLayout layout = layouts[provider.ConsumeIntegralInRange<size_t>(0, 3)];
    McControllerManager::GetInstance().SetTrackingLayout(tokenId, layout);
}

void FuzzMultipleOperations(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    bool isEnabled = provider.ConsumeBool();
    McControllerManager::GetInstance().SetTrackingEnabled(tokenId, isEnabled);

    CameraTrackingLayout layout = static_cast<CameraTrackingLayout>(
        provider.ConsumeIntegralInRange<int32_t>(0, 3));
    McControllerManager::GetInstance().SetTrackingLayout(tokenId, layout);

    bool resultEnabled = false;
    McControllerManager::GetInstance().GetTrackingEnabled(tokenId, resultEnabled);

    CameraTrackingLayout resultLayout = CameraTrackingLayout::DEFAULT;
    McControllerManager::GetInstance().GetTrackingLayout(tokenId, resultLayout);

    McControllerManager::GetInstance().SearchTargetStop();
}

void FuzzSetTrackingEnabledWithFalseValue(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    bool isEnabled = false;
    McControllerManager::GetInstance().SetTrackingEnabled(tokenId, isEnabled);
}

void FuzzSetTrackingEnabledWithTrueValue(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    bool isEnabled = true;
    McControllerManager::GetInstance().SetTrackingEnabled(tokenId, isEnabled);
}

void FuzzSetTrackingLayoutWithDefault(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    CameraTrackingLayout layout = CameraTrackingLayout::DEFAULT;
    McControllerManager::GetInstance().SetTrackingLayout(tokenId, layout);
}

void FuzzSetTrackingLayoutWithLeft(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    CameraTrackingLayout layout = CameraTrackingLayout::LEFT;
    McControllerManager::GetInstance().SetTrackingLayout(tokenId, layout);
}

void FuzzSetTrackingLayoutWithMiddle(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    CameraTrackingLayout layout = CameraTrackingLayout::MIDDLE;
    McControllerManager::GetInstance().SetTrackingLayout(tokenId, layout);
}

void FuzzSetTrackingLayoutWithRight(FuzzedDataProvider &provider)
{
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    CameraTrackingLayout layout = CameraTrackingLayout::RIGHT;
    McControllerManager::GetInstance().SetTrackingLayout(tokenId, layout);
}

void RunFuzzTest(FuzzedDataProvider &provider, int32_t testFunctionId)
{
    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_SET_TRACKING_ENABLED_WITH_BOUNDARY_VALUES:
            FuzzSetTrackingEnabledWithBoundaryValues(provider);
            break;
        case TestFunctionId::FUZZ_GET_TRACKING_ENABLED_WITH_BOUNDARY_VALUES:
            FuzzGetTrackingEnabledWithBoundaryValues(provider);
            break;
        case TestFunctionId::FUZZ_SET_TRACKING_LAYOUT_WITH_BOUNDARY_VALUES:
            FuzzSetTrackingLayoutWithBoundaryValues(provider);
            break;
        case TestFunctionId::FUZZ_SET_TRACKING_LAYOUT_WITH_ALL_ENUM_VALUES:
            FuzzSetTrackingLayoutWithAllEnumValues(provider);
            break;
        case TestFunctionId::FUZZ_MULTIPLE_OPERATIONS:
            FuzzMultipleOperations(provider);
            break;
        case TestFunctionId::FUZZ_SET_TRACKING_ENABLED_WITH_FALSE_VALUE:
            FuzzSetTrackingEnabledWithFalseValue(provider);
            break;
        case TestFunctionId::FUZZ_SET_TRACKING_ENABLED_WITH_TRUE_VALUE:
            FuzzSetTrackingEnabledWithTrueValue(provider);
            break;
        case TestFunctionId::FUZZ_SET_TRACKING_LAYOUT_WITH_DEFAULT:
            FuzzSetTrackingLayoutWithDefault(provider);
            break;
        case TestFunctionId::FUZZ_SET_TRACKING_LAYOUT_WITH_LEFT:
            FuzzSetTrackingLayoutWithLeft(provider);
            break;
        case TestFunctionId::FUZZ_SET_TRACKING_LAYOUT_WITH_MIDDLE:
            FuzzSetTrackingLayoutWithMiddle(provider);
            break;
        case TestFunctionId::FUZZ_SET_TRACKING_LAYOUT_WITH_RIGHT:
            FuzzSetTrackingLayoutWithRight(provider);
            break;
        case TestFunctionId::FUZZ_SEARCH_TARGET_STOP_DUPLICATE:
            FuzzSearchTargetStop(provider);
            break;
        default:
            break;
    }
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    int32_t testFunctionId = provider.ConsumeIntegralInRange<int32_t>(0,
        static_cast<int32_t>(TestFunctionId::TEST_FUNCTION_ID_MAX));
    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_SET_TRACKING_ENABLED:
            FuzzSetTrackingEnabled(provider);
            break;
        case TestFunctionId::FUZZ_GET_TRACKING_ENABLED:
            FuzzGetTrackingEnabled(provider);
            break;
        case TestFunctionId::FUZZ_SET_TRACKING_ENABLED_AND_GET:
            FuzzSetTrackingEnabledAndGet(provider);
            break;
        case TestFunctionId::FUZZ_SET_TRACKING_LAYOUT:
            FuzzSetTrackingLayout(provider);
            break;
        case TestFunctionId::FUZZ_GET_TRACKING_LAYOUT:
            FuzzGetTrackingLayout(provider);
            break;
        case TestFunctionId::FUZZ_SEARCH_TARGET_STOP:
            FuzzSearchTargetStop(provider);
            break;
        default:
            RunFuzzTest(provider, testFunctionId);
            break;
    }
    return 0;
}