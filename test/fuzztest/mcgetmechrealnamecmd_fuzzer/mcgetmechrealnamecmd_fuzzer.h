/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef MC_GET_MECH_REAL_NAME_CMD_FUZZER_H
#define MC_GET_MECH_REAL_NAME_CMD_FUZZER_H

#define FUZZ_PROJECT_NAME "mcgetmechrealnamecmd_fuzzer"

#include <cstddef>
#include <cstdint>

namespace OHOS {
namespace MechBodyController {

void GetMechRealNameCmdMarshalFuzzTest(const uint8_t *data, size_t size);
void GetMechRealNameCmdTriggerResponseFuzzTest(const uint8_t *data, size_t size);
void GetMechRealNameCmdGetParamsFuzzTest(const uint8_t *data, size_t size);
void GetMechRealNameCmdGetResultFuzzTest(const uint8_t *data, size_t size);
void GetMechRealNameCmdWithCallbackFuzzTest(const uint8_t *data, size_t size);
void GetMechRealNameCmdWithValidResponseFuzzTest(const uint8_t *data, size_t size);
void GetMechRealNameCmdWithEmptyNameFuzzTest(const uint8_t *data, size_t size);

} // namespace MechBodyController
} // namespace OHOS

#endif // MC_GET_MECH_REAL_NAME_CMD_FUZZER_H