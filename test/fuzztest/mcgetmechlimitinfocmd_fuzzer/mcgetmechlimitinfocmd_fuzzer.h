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

#ifndef MC_GET_MECH_LIMIT_INFO_CMD_FUZZER_H
#define MC_GET_MECH_LIMIT_INFO_CMD_FUZZER_H

#define FUZZ_PROJECT_NAME "mcgetmechlimitinfocmd_fuzzer"

#include <cstddef>
#include <cstdint>

namespace OHOS {
namespace MechBodyController {

void GetMechLimitInfoCmdMarshalFuzzTest(const uint8_t *data, size_t size);
void GetMechLimitInfoCmdTriggerResponseFuzzTest(const uint8_t *data, size_t size);
void GetMechLimitInfoCmdGetParamsFuzzTest(const uint8_t *data, size_t size);
void GetMechLimitInfoCmdGetResultFuzzTest(const uint8_t *data, size_t size);
void GetMechLimitInfoCmdWithCallbackFuzzTest(const uint8_t *data, size_t size);
void GetMechLimitInfoCmdWithValidResponseFuzzTest(const uint8_t *data, size_t size);

} // namespace MechBodyController
} // namespace OHOS

#endif // MC_GET_MECH_LIMIT_INFO_CMD_FUZZER_H