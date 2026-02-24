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

#ifndef OHOS_HISYSEVENT_UTILS_H
#define OHOS_HISYSEVENT_UTILS_H

#include <string>
#include "mechbody_controller_enums.h"

namespace OHOS {
namespace MechBodyController {

uint64_t GetTimeStampMs();

struct MechKitStartReportInfo {
    // Connection method: -1-Unknown, 0-Bluetooth connection, 1-NFC connection;
	// currently unknown
    ConnectType connectType = ConnectType::UNKNOWN;
    std::string deviceName;
    MechType deviceType = MechType::PORTABLE_GIMBAL;
    std::string deviceModel;
    MechKitStartResCode mechkitStartResultCode = MechKitStartResCode::SUCCESS;
};

class HisyseventUtils {
public:
    static void DotReportMechKitStartEvent(const MechKitStartReportInfo &mechKitStartReportInfo);
    static void DotReportFocustrackingStartEvent(const std::string &appName);
};
} // namespace MechBodyController
} // namespace OHOS
#endif // OHOS_HISYSEVENT_UTILS_H