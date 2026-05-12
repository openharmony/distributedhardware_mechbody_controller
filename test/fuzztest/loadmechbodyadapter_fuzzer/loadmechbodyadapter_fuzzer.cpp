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
#include <cfloat>
#include "load_mechbody_adapter.h"

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {

enum class TestFunctionId {
    FUZZ_INIT_TRACKING_CORE = 0,
    FUZZ_RESET_TRACKING_CORE = 1,
    FUZZ_CLEAR = 2,
    FUZZ_RUN_TRACKING_CORE = 3,
    FUZZ_INIT_AND_CLEAR = 4,
    FUZZ_INIT_RESET_AND_CLEAR = 5,
    FUZZ_MULTIPLE_INIT = 6,
    FUZZ_RUN_WITH_CALLBACK = 7,
    FUZZ_RUN_WITH_NULL_CALLBACK = 8,
    FUZZ_BOUNDARY_VALUES = 9,
    FUZZ_SEQUENCE_OPERATIONS = 10,
    FUZZ_INIT_RUN_RESET_CLEAR = 11,
    FUZZ_RUN_WITH_EXTREME_VALUES = 12,
    FUZZ_CLEAR_MULTIPLE_TIMES = 13,
    FUZZ_RESET_MULTIPLE_TIMES = 14,
    FUZZ_RUN_WITH_VARYING_CALLBACKS = 15,
    TEST_FUNCTION_ID_MAX = 15,
};

void FuzzInitTrackingCore(FuzzedDataProvider &provider)
{
    if (provider.ConsumeBool()) {
        MechbodyAdapterUtils::InitTrackingCore();
    }
}

void FuzzResetTrackingCore(FuzzedDataProvider &provider)
{
    if (provider.ConsumeBool()) {
        MechbodyAdapterUtils::ResetTrackingCore();
    }
}

void FuzzClear(FuzzedDataProvider &provider)
{
    if (provider.ConsumeBool()) {
        MechbodyAdapterUtils::Clear();
    }
}

void FuzzRunTrackingCore(FuzzedDataProvider &provider)
{
    float x = provider.ConsumeFloatingPoint<float>();
    float y = provider.ConsumeFloatingPoint<float>();
    float width = provider.ConsumeFloatingPoint<float>();
    float height = provider.ConsumeFloatingPoint<float>();

    PushXYFn push = [](float px, float py) {
        (void)px;
        (void)py;
    };

    MechbodyAdapterUtils::RunTrackingCore(x, y, width, height, push);
}

void FuzzInitAndClear(FuzzedDataProvider &provider)
{
    if (provider.ConsumeBool()) {
        MechbodyAdapterUtils::InitTrackingCore();
    }
    if (provider.ConsumeBool()) {
        MechbodyAdapterUtils::Clear();
    }
}

void FuzzInitResetAndClear(FuzzedDataProvider &provider)
{
    if (provider.ConsumeBool()) {
        MechbodyAdapterUtils::InitTrackingCore();
    }
    if (provider.ConsumeBool()) {
        MechbodyAdapterUtils::ResetTrackingCore();
    }
    if (provider.ConsumeBool()) {
        MechbodyAdapterUtils::Clear();
    }
}

void FuzzMultipleInit(FuzzedDataProvider &provider)
{
    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 3);
    for (int32_t i = 0; i < iterations; i++) {
        MechbodyAdapterUtils::InitTrackingCore();
    }
    MechbodyAdapterUtils::Clear();
}

void FuzzRunWithCallback(FuzzedDataProvider &provider)
{
    MechbodyAdapterUtils::InitTrackingCore();

    float x = provider.ConsumeFloatingPoint<float>();
    float y = provider.ConsumeFloatingPoint<float>();
    float width = provider.ConsumeFloatingPoint<float>();
    float height = provider.ConsumeFloatingPoint<float>();

    PushXYFn push = [](float px, float py) {
        (void)px;
        (void)py;
    };

    MechbodyAdapterUtils::RunTrackingCore(x, y, width, height, push);
    MechbodyAdapterUtils::Clear();
}

void FuzzRunWithNullCallback(FuzzedDataProvider &provider)
{
    MechbodyAdapterUtils::InitTrackingCore();

    float x = provider.ConsumeFloatingPoint<float>();
    float y = provider.ConsumeFloatingPoint<float>();
    float width = provider.ConsumeFloatingPoint<float>();
    float height = provider.ConsumeFloatingPoint<float>();

    PushXYFn push = nullptr;

    MechbodyAdapterUtils::RunTrackingCore(x, y, width, height, push);
    MechbodyAdapterUtils::Clear();
}

void FuzzBoundaryValues(FuzzedDataProvider &provider)
{
    MechbodyAdapterUtils::InitTrackingCore();

    float x = provider.ConsumeBool() ? FLT_MIN : FLT_MAX;
    float y = provider.ConsumeBool() ? -FLT_MAX : FLT_MAX;
    float width = provider.ConsumeFloatingPoint<float>();
    float height = provider.ConsumeFloatingPoint<float>();

    PushXYFn push = [](float px, float py) {
        (void)px;
        (void)py;
    };

    MechbodyAdapterUtils::RunTrackingCore(x, y, width, height, push);
    MechbodyAdapterUtils::ResetTrackingCore();
    MechbodyAdapterUtils::Clear();
}

void FuzzSequenceOperations(FuzzedDataProvider &provider)
{
    MechbodyAdapterUtils::InitTrackingCore();

    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 3);

    for (int32_t i = 0; i < iterations; i++) {
        float x = provider.ConsumeFloatingPoint<float>();
        float y = provider.ConsumeFloatingPoint<float>();
        float width = provider.ConsumeFloatingPoint<float>();
        float height = provider.ConsumeFloatingPoint<float>();

        PushXYFn push = [](float px, float py) {
            (void)px;
            (void)py;
        };

        MechbodyAdapterUtils::RunTrackingCore(x, y, width, height, push);
    }

    MechbodyAdapterUtils::ResetTrackingCore();
    MechbodyAdapterUtils::Clear();
}

void FuzzInitRunResetClear(FuzzedDataProvider &provider)
{
    MechbodyAdapterUtils::InitTrackingCore();

    float x = provider.ConsumeFloatingPoint<float>();
    float y = provider.ConsumeFloatingPoint<float>();
    float width = provider.ConsumeFloatingPoint<float>();
    float height = provider.ConsumeFloatingPoint<float>();

    PushXYFn push = [](float px, float py) {
        (void)px;
        (void)py;
    };

    MechbodyAdapterUtils::RunTrackingCore(x, y, width, height, push);
    MechbodyAdapterUtils::ResetTrackingCore();
    MechbodyAdapterUtils::Clear();
}

void RunTrackingCoreWithValues(const float testValues[], size_t testValueCount, size_t i, size_t j, PushXYFn push)
{
    for (size_t k = 0; k < testValueCount; k++) {
        for (size_t m = 0; m < testValueCount; m++) {
            MechbodyAdapterUtils::RunTrackingCore(testValues[i], testValues[j], testValues[k], testValues[m], push);
        }
    }
}

void FuzzRunWithExtremeValues(FuzzedDataProvider &provider)
{
    if (!provider.ConsumeBool()) {
        return;
    }

    MechbodyAdapterUtils::InitTrackingCore();

    PushXYFn push = [](float px, float py) {
        (void)px;
        (void)py;
    };

    float testValues[] = {FLT_MIN, FLT_MAX, -FLT_MAX, 0.0f, 1.0f, -1.0f, 100.0f, -100.0f};
    size_t testValueCount = sizeof(testValues) / sizeof(testValues[0]);

    size_t startI = provider.ConsumeIntegralInRange<size_t>(0, testValueCount - 1);
    size_t startJ = provider.ConsumeIntegralInRange<size_t>(0, testValueCount - 1);
    size_t iterations = provider.ConsumeIntegralInRange<size_t>(1, testValueCount);

    for (size_t i = startI; i < startI + iterations && i < testValueCount; i++) {
        for (size_t j = startJ; j < startJ + iterations && j < testValueCount; j++) {
            RunTrackingCoreWithValues(testValues, testValueCount, i, j, push);
        }
    }

    if (provider.ConsumeBool()) {
        MechbodyAdapterUtils::Clear();
    }
}

void FuzzClearMultipleTimes(FuzzedDataProvider &provider)
{
    MechbodyAdapterUtils::InitTrackingCore();

    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 3);

    for (int32_t i = 0; i < iterations; i++) {
        MechbodyAdapterUtils::Clear();
    }
}

void FuzzResetMultipleTimes(FuzzedDataProvider &provider)
{
    MechbodyAdapterUtils::InitTrackingCore();

    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 3);

    for (int32_t i = 0; i < iterations; i++) {
        MechbodyAdapterUtils::ResetTrackingCore();
    }

    MechbodyAdapterUtils::Clear();
}

void FuzzRunWithVaryingCallbacks(FuzzedDataProvider &provider)
{
    MechbodyAdapterUtils::InitTrackingCore();

    int32_t iterations = provider.ConsumeIntegralInRange<int32_t>(1, 3);

    for (int32_t i = 0; i < iterations; i++) {
        float x = provider.ConsumeFloatingPoint<float>();
        float y = provider.ConsumeFloatingPoint<float>();
        float width = provider.ConsumeFloatingPoint<float>();
        float height = provider.ConsumeFloatingPoint<float>();

        PushXYFn push = [](float px, float py) {
            (void)px;
            (void)py;
        };

        MechbodyAdapterUtils::RunTrackingCore(x, y, width, height, push);
    }

    MechbodyAdapterUtils::ResetTrackingCore();
    MechbodyAdapterUtils::Clear();
}

void RunFuzzTest(FuzzedDataProvider &provider, int32_t testFunctionId)
{
    switch (static_cast<TestFunctionId>(testFunctionId)) {
        case TestFunctionId::FUZZ_RUN_WITH_NULL_CALLBACK:
            FuzzRunWithNullCallback(provider);
            break;
        case TestFunctionId::FUZZ_BOUNDARY_VALUES:
            FuzzBoundaryValues(provider);
            break;
        case TestFunctionId::FUZZ_SEQUENCE_OPERATIONS:
            FuzzSequenceOperations(provider);
            break;
        case TestFunctionId::FUZZ_INIT_RUN_RESET_CLEAR:
            FuzzInitRunResetClear(provider);
            break;
        case TestFunctionId::FUZZ_RUN_WITH_EXTREME_VALUES:
            FuzzRunWithExtremeValues(provider);
            break;
        case TestFunctionId::FUZZ_CLEAR_MULTIPLE_TIMES:
            FuzzClearMultipleTimes(provider);
            break;
        case TestFunctionId::FUZZ_RESET_MULTIPLE_TIMES:
            FuzzResetMultipleTimes(provider);
            break;
        case TestFunctionId::FUZZ_RUN_WITH_VARYING_CALLBACKS:
            FuzzRunWithVaryingCallbacks(provider);
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
        case TestFunctionId::FUZZ_INIT_TRACKING_CORE:
            FuzzInitTrackingCore(provider);
            break;
        case TestFunctionId::FUZZ_RESET_TRACKING_CORE:
            FuzzResetTrackingCore(provider);
            break;
        case TestFunctionId::FUZZ_CLEAR:
            FuzzClear(provider);
            break;
        case TestFunctionId::FUZZ_RUN_TRACKING_CORE:
            FuzzRunTrackingCore(provider);
            break;
        case TestFunctionId::FUZZ_INIT_AND_CLEAR:
            FuzzInitAndClear(provider);
            break;
        case TestFunctionId::FUZZ_INIT_RESET_AND_CLEAR:
            FuzzInitResetAndClear(provider);
            break;
        case TestFunctionId::FUZZ_MULTIPLE_INIT:
            FuzzMultipleInit(provider);
            break;
        case TestFunctionId::FUZZ_RUN_WITH_CALLBACK:
            FuzzRunWithCallback(provider);
            break;
        default:
            RunFuzzTest(provider, testFunctionId);
            break;
    }
    return 0;
}