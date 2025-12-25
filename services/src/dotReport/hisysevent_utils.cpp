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
#include <chrono>
#include "hisysevent_utils.h"
#include "hisysevent.h"
#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {

namespace {
    const std::string TAG = "HisyseventUtils";
}

const char* MECHKIT_START_EVENT = "MECHKIT_START";
const char* MECHKIT_FOCUSTRACKING_START_EVENT = "MECHKIT_FOCUSTRACKING_START";

const char* PNAMEID_PARAM_KEY = "PNAMEID";
const char* PNAMEID_PARAM_VALUE = "mechbody_controller";

const char* PVERSIONID_PARAM_KEY = "PVERSIONID";
const char* PVERSIONID_PARAM_VALUE = "1.0";

constexpr char MECHKIT_SYSEVENT_REPORT_DOMAIN[] = "HIMECH_UE";

uint64_t GetTimeStampMs()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    uint64_t timeStampMs = static_cast<uint64_t>(millis);
    return timeStampMs;
}

void HisyseventUtils::DotReportMechKitStartEvent(const MechKitStartReportInfo &mechKitStartReportInfo)
{
    uint64_t eventHappenTime = GetTimeStampMs();
    HILOGW("DotReportMechKitStartEvent, startResCode: %{public}d",
        mechKitStartReportInfo.mechkitStartResultCode);
    int32_t res = HiSysEventWrite(MECHKIT_SYSEVENT_REPORT_DOMAIN,
        MECHKIT_START_EVENT,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "EVENT_TIME", eventHappenTime,
        "CONNECT_TYPE", static_cast<int32_t>(mechKitStartReportInfo.connectType),
        "DEVICE_TYPE", static_cast<int32_t>(mechKitStartReportInfo.deviceType),
        "DEVICE_NAME", mechKitStartReportInfo.deviceName,
        "DEVICE_MODEL", mechKitStartReportInfo.deviceModel,
        "START_RES_CODE", static_cast<int32_t>(mechKitStartReportInfo.mechkitStartResultCode),
        PNAMEID_PARAM_KEY, PNAMEID_PARAM_VALUE,
        PVERSIONID_PARAM_KEY, PVERSIONID_PARAM_VALUE);
    if (res != ERR_OK) {
        HILOGE("DotReportMechKitStartEvent error, res:%{public}d", res);
    }
}

void HisyseventUtils::DotReportFocustrackingStartEvent(const std::string &appName)
{
    uint64_t eventHappenTime = GetTimeStampMs();
    HILOGW("DotReportFocustrackingStartEvent, appName: %{public}s",
        appName.c_str());
    int32_t res = HiSysEventWrite(MECHKIT_SYSEVENT_REPORT_DOMAIN,
        MECHKIT_FOCUSTRACKING_START_EVENT,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "EVENT_TIME", eventHappenTime,
        "APP_NAME", appName,
        PNAMEID_PARAM_KEY, PNAMEID_PARAM_VALUE,
        PVERSIONID_PARAM_KEY, PVERSIONID_PARAM_VALUE);
    if (res != ERR_OK) {
        HILOGE("DotReportFocustrackingStartEvent error, res:%{public}d", res);
    }
}

} // namespace MechBodyController
} // namespace OHOS
