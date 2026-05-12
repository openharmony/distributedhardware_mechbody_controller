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

#ifndef MC_REGISTER_MECH_POSITION_INFO_CMD_FUZZER_H
#define MC_REGISTER_MECH_POSITION_INFO_CMD_FUZZER_H

#define FUZZ_PROJECT_NAME "mcregistermechpositioninfocmd_fuzzer"

#include <cstddef>
#include <cstdint>

namespace OHOS {
namespace MechBodyController {

void RegisterMechPositionInfoCmdMarshalFuzzTest(const uint8_t *data, size_t size);
void RegisterMechPositionInfoCmdUnmarshalFuzzTest(const uint8_t *data, size_t size);
void RegisterMechPositionInfoCmdTriggerResponseFuzzTest(const uint8_t *data, size_t size);
void RegisterMechPositionInfoCmdGetPositionFuzzTest(const uint8_t *data, size_t size);
void RegisterMechPositionInfoCmdGetResultFuzzTest(const uint8_t *data, size_t size);
void RegisterMechPositionInfoCmdWithCallbackFuzzTest(const uint8_t *data, size_t size);
void RegisterMechPositionInfoCmdWithValidDataFuzzTest(const uint8_t *data, size_t size);
void RegisterMechPositionInfoCmdWithAllValuesFuzzTest(const uint8_t *data, size_t size);
void RegisterMechPositionInfoCmdWithValidResponseFuzzTest(const uint8_t *data, size_t size);
void RegisterMechPositionInfoCmdWithNullResponseFuzzTest(const uint8_t *data, size_t size);

} // namespace MechBodyController
} // namespace OHOS

#endif // MC_REGISTER_MECH_POSITION_INFO_CMD_FUZZER_H