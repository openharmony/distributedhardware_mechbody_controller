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

struct MechKitStartReportInfo {
    // Connection method: -1-Unknown, 0-Bluetooth connection, 1-NFC connection;
	// currently unknown
    ConnectType connectType = ConnectType::UNKNOWN;
    std::string deviceName;
    MechType deviceType = MechType::PORTABLE_GIMBAL;
    std::string deviceModel;
    MechKitStartResCode mechkitStartResultCode = MechKitStartResCode::SUCCESS;
};

struct MechkitControlInfo {
    uint32_t upWard;
    uint32_t downWard;
    uint32_t turnLeft;
    uint32_t turnRight;
    uint32_t fastRotation;
    uint32_t landScapePortrait;
    uint8_t cameraMode;
    uint64_t trackingDuration;
    uint8_t cameraType;
    float_t zoomRatio;
    std::string packageName;
    uint8_t obstacleNumber;
    uint8_t cliffNumber;
    uint32_t successNumber;
    uint32_t failNumber;
    uint8_t deviceType;
    uint32_t stickNum;
    std::vector<std::string> appName;
    std::vector<uint64_t> existenceTime;
    uint32_t connectDelay;
    uint32_t connectCode;
    std::vector<uint16_t> cmdWord;
    std::vector<uint16_t> errorCode;
};

struct AppForegroundInfo {
    std::string bundleName;
    int64_t startTime;
    uint64_t duration = 0;
    int32_t state;
    
    bool operator<(const AppForegroundInfo& other) const
    {
        return bundleName < other.bundleName;
    }
};

class HisyseventUtils {
public:
    static void DotReportMechKitStartEvent(const MechKitStartReportInfo &mechKitStartReportInfo);
    static void DotReportFocustrackingStartEvent(const std::string &appName);
    static void DoReportMechkitControlStatisticEvent(MechkitControlInfo controlInfo);
};
} // namespace MechBodyController
} // namespace OHOS
#endif // OHOS_HISYSEVENT_UTILS_H