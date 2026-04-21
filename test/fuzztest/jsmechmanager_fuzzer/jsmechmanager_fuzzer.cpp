/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <memory>
#include <vector>
#include <map>

#define private public
#define protected public
#include "js_mech_manager.h"
#undef private
#undef protected
#include "mechbody_controller_types.h"
#include "mechbody_controller_log.h"
#include "securec.h"

namespace OHOS {
namespace MechBodyController {

// Mock NAPI types
struct MockNapiValue {
    enum ValueType {
        String,
        Function,
        Boolean,
        Number,
        Object,
        Undefined
    };
    ValueType type;
    std::string stringValue;
    bool boolValue;
    double numberValue;
    std::map<std::string, MockNapiValue> objectProperties;
};

struct MockNapiEnv {
    int dummy;
    bool shouldFail;
    MockNapiEnv() : dummy(0), shouldFail(false) {}
};

struct MockCallbackInfo {
    std::vector<MockNapiValue> args;
};

// Global fuzz data provider for mock functions
static FuzzedDataProvider* g_fdp = nullptr;
static MockCallbackInfo* g_mockCallbackInfo = nullptr;

// Mock NAPI functions
extern "C" {
    napi_status napi_get_cb_info(napi_env env, napi_callback_info info,
        size_t* argc, napi_value* args, napi_value* this_arg,
        void** data) {
        if (env == nullptr || argc == nullptr) {
            return napi_invalid_arg;
        }

        MockCallbackInfo* mockInfo = g_mockCallbackInfo;
        if (mockInfo == nullptr) {
            return napi_invalid_arg;
        }

        size_t argsToCopy = std::min(*argc, mockInfo->args.size());
        for (size_t i = 0; i < argsToCopy; i++) {
            args[i] = reinterpret_cast<napi_value>(&mockInfo->args[i]);
        }
        *argc = mockInfo->args.size();

        return napi_ok;
    }

    napi_status napi_typeof(napi_env env, napi_value value,
        napi_valuetype* result) {
        if (env == nullptr || value == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }

        MockNapiValue* mockValue = reinterpret_cast<MockNapiValue*>(value);
        if (mockValue == nullptr) {
            return napi_invalid_arg;
        }

        switch (mockValue->type) {
            case MockNapiValue::String:
                *result = napi_string;
                break;
            case MockNapiValue::Function:
                *result = napi_function;
                break;
            case MockNapiValue::Boolean:
                *result = napi_boolean;
                break;
            case MockNapiValue::Number:
                *result = napi_number;
                break;
            case MockNapiValue::Object:
                *result = napi_object;
                break;
            default:
                *result = napi_undefined;
                break;
        }

        return napi_ok;
    }

    napi_status napi_get_value_string_utf8(napi_env env, napi_value value,
        char* buf, size_t bufsize, size_t* result) {
        if (env == nullptr || value == nullptr) {
            return napi_invalid_arg;
        }

        MockNapiValue* mockValue = reinterpret_cast<MockNapiValue*>(value);
        if (mockValue == nullptr || mockValue->type != MockNapiValue::String) {
            return napi_string_expected;
        }

        size_t len = mockValue->stringValue.size();
        if (result != nullptr) {
            *result = len;
        }

        if (buf != nullptr && bufsize > 0) {
            size_t copyLen = std::min(len, bufsize - 1);
            memcpy(buf, mockValue->stringValue.c_str(), copyLen);
            buf[copyLen] = '\0';
        }

        return napi_ok;
    }

    napi_status napi_create_reference(napi_env env, napi_value value,
        uint32_t initial_refcount, napi_ref* result) {
        if (env == nullptr || value == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }

        *result = reinterpret_cast<napi_ref>(0x12345678);
        return napi_ok;
    }

    napi_status napi_throw_error(napi_env env, const char* code,
        const char* msg) {
        if (env == nullptr) {
            return napi_invalid_arg;
        }
        return napi_ok;
    }

    napi_status napi_throw_type_error(napi_env env, const char* code,
        const char* msg) {
        if (env == nullptr) {
            return napi_invalid_arg;
        }
        return napi_ok;
    }

    napi_status napi_get_value_bool(napi_env env, napi_value value,
        bool* result) {
        if (env == nullptr || value == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }

        MockNapiValue* mockValue = reinterpret_cast<MockNapiValue*>(value);
        if (mockValue == nullptr || mockValue->type != MockNapiValue::Boolean) {
            return napi_boolean_expected;
        }

        *result = mockValue->boolValue;
        return napi_ok;
    }

    napi_status napi_get_value_int32(napi_env env, napi_value value,
        int32_t* result) {
        if (env == nullptr || value == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }

        MockNapiValue* mockValue = reinterpret_cast<MockNapiValue*>(value);
        if (mockValue == nullptr || mockValue->type != MockNapiValue::Number) {
            return napi_number_expected;
        }

        *result = static_cast<int32_t>(mockValue->numberValue);
        return napi_ok;
    }

    napi_status napi_get_value_double(napi_env env, napi_value value,
        double* result) {
        if (env == nullptr || value == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }

        MockNapiValue* mockValue = reinterpret_cast<MockNapiValue*>(value);
        if (mockValue == nullptr || mockValue->type != MockNapiValue::Number) {
            return napi_number_expected;
        }

        *result = mockValue->numberValue;
        return napi_ok;
    }

    napi_status napi_create_array_with_length(napi_env env, size_t length,
        napi_value* result) {
        if (env == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }

        static MockNapiValue arrayValue;
        arrayValue.type = MockNapiValue::Object;
        *result = reinterpret_cast<napi_value>(&arrayValue);
        return napi_ok;
    }

    napi_status napi_create_object(napi_env env, napi_value* result) {
        if (env == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }

        static MockNapiValue objectValue;
        objectValue.type = MockNapiValue::Object;
        *result = reinterpret_cast<napi_value>(&objectValue);
        return napi_ok;
    }

    napi_status napi_create_int32(napi_env env, int32_t value,
        napi_value* result) {
        if (env == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }

        static MockNapiValue intValue;
        intValue.type = MockNapiValue::Number;
        intValue.numberValue = value;
        *result = reinterpret_cast<napi_value>(&intValue);
        return napi_ok;
    }

    napi_status napi_create_string_utf8(napi_env env, const char* str,
        size_t length, napi_value* result) {
        if (env == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }

        static MockNapiValue stringValue;
        stringValue.type = MockNapiValue::String;
        if (str != nullptr) {
            stringValue.stringValue = std::string(str, length);
        }
        *result = reinterpret_cast<napi_value>(&stringValue);
        return napi_ok;
    }

    napi_status napi_set_named_property(napi_env env, napi_value object,
        const char* utf8name, napi_value value) {
        if (env == nullptr || object == nullptr || utf8name == nullptr ||
            value == nullptr) {
            return napi_invalid_arg;
        }

        MockNapiValue* mockObject = reinterpret_cast<MockNapiValue*>(object);
        MockNapiValue* mockValue = reinterpret_cast<MockNapiValue*>(value);

        if (mockObject != nullptr && mockValue != nullptr) {
            mockObject->objectProperties[utf8name] = *mockValue;
        }

        return napi_ok;
    }

    napi_status napi_set_element(napi_env env, napi_value array,
        uint32_t index, napi_value value) {
        if (env == nullptr || array == nullptr || value == nullptr) {
            return napi_invalid_arg;
        }
        return napi_ok;
    }

    napi_status napi_get_boolean(napi_env env, bool value,
        napi_value* result) {
        if (env == nullptr || result == nullptr) {
            return napi_invalid_arg;
        }

        static MockNapiValue boolValue;
        boolValue.type = MockNapiValue::Boolean;
        boolValue.boolValue = value;
        *result = reinterpret_cast<napi_value>(&boolValue);
        return napi_ok;
    }

    napi_status napi_create_promise(napi_env env, napi_deferred* deferred,
        napi_value* promise) {
        if (env == nullptr || deferred == nullptr || promise == nullptr) {
            return napi_invalid_arg;
        }

        static MockNapiValue promiseValue;
        promiseValue.type = MockNapiValue::Object;
        *promise = reinterpret_cast<napi_value>(&promiseValue);
        *deferred = reinterpret_cast<napi_deferred>(0x87654321);
        return napi_ok;
    }
}

// Mock MechClient for testing
class MockMechClient {
public:
    MockMechClient() = default;
    ~MockMechClient() = default;

    int32_t GetAttachedDevices(std::vector<std::shared_ptr<MechInfo>>& mechInfos) {
        if (g_fdp == nullptr) {
            return ERR_OK;
        }

        // Generate random number of devices
        int32_t numDevices = g_fdp->ConsumeIntegral<int32_t>() % 10;

        for (int32_t i = 0; i < numDevices; i++) {
            std::shared_ptr<MechInfo> info = std::make_shared<MechInfo>();
            info->mechId = g_fdp->ConsumeIntegral<int32_t>();
            info->mechName = g_fdp->ConsumeRandomLengthString(100);
            info->mac = g_fdp->ConsumeRandomLengthString(50);
            info->mechType = static_cast<MechType>(g_fdp->ConsumeIntegral<uint32_t>() % 6);
            mechInfos.push_back(info);
        }

        return ERR_OK;
    }

    int32_t SetCameraTrackingEnabled(bool isEnabled) {
        (void)isEnabled;
        if (g_fdp == nullptr) {
            return ERR_OK;
        }

        // Randomly return different error codes
        int32_t result = g_fdp->ConsumeIntegral<int32_t>() % 5;
        switch (result) {
            case 0:
                return ERR_OK;
            case 1:
                return MechNapiErrorCode::DEVICE_NOT_CONNECTED;
            case 2:
                return MechNapiErrorCode::DEVICE_NOT_SUPPORTED;
            default:
                return MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY;
        }
    }

    int32_t GetCameraTrackingEnabled(bool& isEnabled) {
        if (g_fdp == nullptr) {
            isEnabled = true;
            return ERR_OK;
        }

        isEnabled = g_fdp->ConsumeBool();

        // Randomly return different error codes
        int32_t result = g_fdp->ConsumeIntegral<int32_t>() % 3;
        switch (result) {
            case 0:
                return ERR_OK;
            case 1:
                return MechNapiErrorCode::DEVICE_NOT_CONNECTED;
            default:
                return MechNapiErrorCode::SYSTEM_WORK_ABNORMALLY;
        }
    }
};

// Mock MechManager for testing
class TestMechManager : public MechManager {
public:
    static bool MockInitMechClient() {
        return true;
    }

    static bool MockIsSystemApp() {
        if (g_fdp == nullptr) {
            return true;
        }
        return g_fdp->ConsumeBool();
    }

    static void TestOn(const uint8_t *data, size_t size) {
        if ((data == nullptr) || (size == 0)) {
            return;
        }

        FuzzedDataProvider fdp(data, size);
        g_fdp = &fdp;

        MockNapiEnv env;
        MockCallbackInfo info;

        // Generate random number of arguments
        size_t argc = fdp.ConsumeIntegral<size_t>() % 4;

        // Create mock arguments
        for (size_t i = 0; i < argc; i++) {
            MockNapiValue arg;
            int32_t type = fdp.ConsumeIntegral<int32_t>() % 5;

            switch (type) {
                case 0: // String
                    arg.type = MockNapiValue::String;
                    arg.stringValue = fdp.ConsumeRandomLengthString(100);
                    break;
                case 1: // Function
                    arg.type = MockNapiValue::Function;
                    break;
                case 2: // Boolean
                    arg.type = MockNapiValue::Boolean;
                    arg.boolValue = fdp.ConsumeBool();
                    break;
                case 3: // Number
                    arg.type = MockNapiValue::Number;
                    arg.numberValue = fdp.ConsumeFloatingPoint<double>();
                    break;
                default:
                    arg.type = MockNapiValue::Undefined;
                    break;
            }
            info.args.push_back(arg);
        }

        // Set global mock callback info
        g_mockCallbackInfo = &info;

        // Call On function
        MechManager::On(reinterpret_cast<napi_env>(&env),
            reinterpret_cast<napi_callback_info>(0x1234));

        g_mockCallbackInfo = nullptr;
        g_fdp = nullptr;
    }

    static void TestOff(const uint8_t *data, size_t size) {
        if ((data == nullptr) || (size == 0)) {
            return;
        }

        FuzzedDataProvider fdp(data, size);
        g_fdp = &fdp;

        MockNapiEnv env;
        MockCallbackInfo info;

        // Generate random number of arguments
        size_t argc = fdp.ConsumeIntegral<size_t>() % 4;

        // Create mock arguments
        for (size_t i = 0; i < argc; i++) {
            MockNapiValue arg;
            int32_t type = fdp.ConsumeIntegral<int32_t>() % 5;

            switch (type) {
                case 0: // String
                    arg.type = MockNapiValue::String;
                    arg.stringValue = fdp.ConsumeRandomLengthString(100);
                    break;
                case 1: // Function
                    arg.type = MockNapiValue::Function;
                    break;
                case 2: // Boolean
                    arg.type = MockNapiValue::Boolean;
                    arg.boolValue = fdp.ConsumeBool();
                    break;
                case 3: // Number
                    arg.type = MockNapiValue::Number;
                    arg.numberValue = fdp.ConsumeFloatingPoint<double>();
                    break;
                default:
                    arg.type = MockNapiValue::Undefined;
                    break;
            }
            info.args.push_back(arg);
        }

        // Set global mock callback info
        g_mockCallbackInfo = &info;

        // Call Off function
        MechManager::Off(reinterpret_cast<napi_env>(&env),
            reinterpret_cast<napi_callback_info>(0x1234));

        g_mockCallbackInfo = nullptr;
        g_fdp = nullptr;
    }

    static void TestGetAttachedDevices(const uint8_t *data, size_t size) {
        if ((data == nullptr) || (size == 0)) {
            return;
        }

        FuzzedDataProvider fdp(data, size);
        g_fdp = &fdp;

        MockNapiEnv env;
        MockCallbackInfo info;

        // Generate random number of arguments
        size_t argc = fdp.ConsumeIntegral<size_t>() % 4;

        // Create mock arguments
        for (size_t i = 0; i < argc; i++) {
            MockNapiValue arg;
            int32_t type = fdp.ConsumeIntegral<int32_t>() % 5;

            switch (type) {
                case 0: // String
                    arg.type = MockNapiValue::String;
                    arg.stringValue = fdp.ConsumeRandomLengthString(100);
                    break;
                case 1: // Function
                    arg.type = MockNapiValue::Function;
                    break;
                case 2: // Boolean
                    arg.type = MockNapiValue::Boolean;
                    arg.boolValue = fdp.ConsumeBool();
                    break;
                case 3: // Number
                    arg.type = MockNapiValue::Number;
                    arg.numberValue = fdp.ConsumeFloatingPoint<double>();
                    break;
                default:
                    arg.type = MockNapiValue::Undefined;
                    break;
            }
            info.args.push_back(arg);
        }

        // Set global mock callback info
        g_mockCallbackInfo = &info;

        // Call GetAttachedDevices function
        MechManager::GetAttachedDevices(reinterpret_cast<napi_env>(&env),
            reinterpret_cast<napi_callback_info>(0x1234));

        g_mockCallbackInfo = nullptr;
        g_fdp = nullptr;
    }

    static void TestSetCameraTrackingEnabled(const uint8_t *data, size_t size) {
        if ((data == nullptr) || (size == 0)) {
            return;
        }

        FuzzedDataProvider fdp(data, size);
        g_fdp = &fdp;

        MockNapiEnv env;
        MockCallbackInfo info;

        // Generate random number of arguments
        size_t argc = fdp.ConsumeIntegral<size_t>() % 4;

        // Create mock arguments
        for (size_t i = 0; i < argc; i++) {
            MockNapiValue arg;
            int32_t type = fdp.ConsumeIntegral<int32_t>() % 5;

            switch (type) {
                case 0: // String
                    arg.type = MockNapiValue::String;
                    arg.stringValue = fdp.ConsumeRandomLengthString(100);
                    break;
                case 1: // Function
                    arg.type = MockNapiValue::Function;
                    break;
                case 2: // Boolean
                    arg.type = MockNapiValue::Boolean;
                    arg.boolValue = fdp.ConsumeBool();
                    break;
                case 3: // Number
                    arg.type = MockNapiValue::Number;
                    arg.numberValue = fdp.ConsumeFloatingPoint<double>();
                    break;
                default:
                    arg.type = MockNapiValue::Undefined;
                    break;
            }
            info.args.push_back(arg);
        }

        // Set global mock callback info
        g_mockCallbackInfo = &info;

        // Call SetCameraTrackingEnabled function
        MechManager::SetCameraTrackingEnabled(reinterpret_cast<napi_env>(&env),
            reinterpret_cast<napi_callback_info>(0x1234));

        g_mockCallbackInfo = nullptr;
        g_fdp = nullptr;
    }

    static void TestGetCameraTrackingEnabled(const uint8_t *data, size_t size) {
        if ((data == nullptr) || (size == 0)) {
            return;
        }

        FuzzedDataProvider fdp(data, size);
        g_fdp = &fdp;

        MockNapiEnv env;
        MockCallbackInfo info;

        // Generate random number of arguments
        size_t argc = fdp.ConsumeIntegral<size_t>() % 4;

        // Create mock arguments
        for (size_t i = 0; i < argc; i++) {
            MockNapiValue arg;
            int32_t type = fdp.ConsumeIntegral<int32_t>() % 5;

            switch (type) {
                case 0: // String
                    arg.type = MockNapiValue::String;
                    arg.stringValue = fdp.ConsumeRandomLengthString(100);
                    break;
                case 1: // Function
                    arg.type = MockNapiValue::Function;
                    break;
                case 2: // Boolean
                    arg.type = MockNapiValue::Boolean;
                    arg.boolValue = fdp.ConsumeBool();
                    break;
                case 3: // Number
                    arg.type = MockNapiValue::Number;
                    arg.numberValue = fdp.ConsumeFloatingPoint<double>();
                    break;
                default:
                    arg.type = MockNapiValue::Undefined;
                    break;
            }
            info.args.push_back(arg);
        }

        // Set global mock callback info
        g_mockCallbackInfo = &info;

        // Call GetCameraTrackingEnabled function
        MechManager::GetCameraTrackingEnabled(reinterpret_cast<napi_env>(&env),
            reinterpret_cast<napi_callback_info>(0x1234));

        g_mockCallbackInfo = nullptr;
        g_fdp = nullptr;
    }
};

} // namespace MechBodyController
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    // Run all fuzz tests for On, Off, GetAttachedDevices, SetCameraTrackingEnabled, GetCameraTrackingEnabled
    OHOS::MechBodyController::TestMechManager::TestOn(data, size);
    OHOS::MechBodyController::TestMechManager::TestOff(data, size);
    OHOS::MechBodyController::TestMechManager::TestGetAttachedDevices(data, size);
    OHOS::MechBodyController::TestMechManager::TestSetCameraTrackingEnabled(data, size);
    OHOS::MechBodyController::TestMechManager::TestGetCameraTrackingEnabled(data, size);

    return 0;
}