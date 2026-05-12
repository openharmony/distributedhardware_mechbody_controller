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

#ifndef MC_GET_MECH_CAMERA_TRACKING_LAYOUT_CMD_FUZZER_H
#define MC_GET_MECH_CAMERA_TRACKING_LAYOUT_CMD_FUZZER_H

#define FUZZ_PROJECT_NAME "mcgetmechcameratrackinglayoutcmd_fuzzer"

#include <cstddef>
#include <cstdint>

namespace OHOS {
namespace MechBodyController {

void GetMechCameraTrackingLayoutCmdConstructorFuzzTest(const uint8_t *data, size_t size);
void GetMechCameraTrackingLayoutCmdMarshalFuzzTest(const uint8_t *data, size_t size);
void GetMechCameraTrackingLayoutCmdTriggerResponseFuzzTest(const uint8_t *data, size_t size);
void GetMechCameraTrackingLayoutCmdGetParamsFuzzTest(const uint8_t *data, size_t size);
void GetMechCameraTrackingLayoutCmdGetResultFuzzTest(const uint8_t *data, size_t size);
void GetMechCameraTrackingLayoutCmdWithCallbackFuzzTest(const uint8_t *data, size_t size);
void GetMechCameraTrackingLayoutCmdValidDataFuzzTest(const uint8_t *data, size_t size);
void GetMechCameraTrackingLayoutCmdInvalidTypeFuzzTest(const uint8_t *data, size_t size);
void GetMechCameraTrackingLayoutCmdInvalidLengthFuzzTest(const uint8_t *data, size_t size);
void GetMechCameraTrackingLayoutCmdShortDataFuzzTest(const uint8_t *data, size_t size);
void GetMechCameraTrackingLayoutCmdFullWorkflowFuzzTest(const uint8_t *data, size_t size);

} // namespace MechBodyController
} // namespace OHOS

#endif // MC_GET_MECH_CAMERA_TRACKING_LAYOUT_CMD_FUZZER_H