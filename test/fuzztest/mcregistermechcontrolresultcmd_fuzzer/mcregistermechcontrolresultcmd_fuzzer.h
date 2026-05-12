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

#ifndef MC_REGISTER_MECH_CONTROL_RESULT_CMD_FUZZER_H
#define MC_REGISTER_MECH_CONTROL_RESULT_CMD_FUZZER_H

#define FUZZ_PROJECT_NAME "mcregistermechcontrolresultcmd_fuzzer"

#include <cstddef>
#include <cstdint>

namespace OHOS {
namespace MechBodyController {

void RegisterMechControlResultCmdMarshalFuzzTest(const uint8_t *data, size_t size);
void RegisterMechControlResultCmdUnmarshalFuzzTest(const uint8_t *data, size_t size);
void RegisterMechControlResultCmdTriggerResponseFuzzTest(const uint8_t *data, size_t size);
void RegisterMechControlResultCmdGetControlResultFuzzTest(const uint8_t *data, size_t size);
void RegisterMechControlResultCmdGetTaskIdFuzzTest(const uint8_t *data, size_t size);
void RegisterMechControlResultCmdGetResultFuzzTest(const uint8_t *data, size_t size);
void RegisterMechControlResultCmdWithCallbackFuzzTest(const uint8_t *data, size_t size);
void RegisterMechControlResultCmdWithValidDataFuzzTest(const uint8_t *data, size_t size);
void RegisterMechControlResultCmdWithAllValuesFuzzTest(const uint8_t *data, size_t size);

} // namespace MechBodyController
} // namespace OHOS

#endif // MC_REGISTER_MECH_CONTROL_RESULT_CMD_FUZZER_H