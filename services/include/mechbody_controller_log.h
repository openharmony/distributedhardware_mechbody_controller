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

#ifndef SERVICES_MECHBODY_CONTROLLER_LOG_H_
#define SERVICES_MECHBODY_CONTROLLER_LOG_H_

#include "errors.h"
#include "hilog/log.h"
#include <cinttypes>
#include <cstdint>

namespace OHOS {
namespace MechBodyController {
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD004170

#undef LOG_TAG
#define LOG_TAG "MechBodyController"

#ifdef HILOGD
#undef HILOGD
#endif

#ifdef HILOGF
#undef HILOGF
#endif

#ifdef HILOGE
#undef HILOGE
#endif

#ifdef HILOGW
#undef HILOGW
#endif

#ifdef HILOGI
#undef HILOGI
#endif

#define HILOGF(fmt, ...) HILOG_FATAL(LOG_CORE, "%{public}s::%{public}s " fmt, TAG.c_str(), __FUNCTION__, ##__VA_ARGS__)
#define HILOGE(fmt, ...) HILOG_ERROR(LOG_CORE, "%{public}s::%{public}s " fmt, TAG.c_str(), __FUNCTION__, ##__VA_ARGS__)
#define HILOGW(fmt, ...) HILOG_WARN(LOG_CORE, "%{public}s::%{public}s " fmt, TAG.c_str(), __FUNCTION__, ##__VA_ARGS__)
#define HILOGI(fmt, ...) HILOG_INFO(LOG_CORE, "%{public}s::%{public}s " fmt, TAG.c_str(), __FUNCTION__, ##__VA_ARGS__)
#define HILOGD(fmt, ...) HILOG_DEBUG(LOG_CORE, "%{public}s::%{public}s " fmt, TAG.c_str(), __FUNCTION__, ##__VA_ARGS__)


#define CHECK_POINTER_RETURN(object, log)                       \
    do {                                                        \
        if ((object) == nullptr) {                              \
            HILOGE("%{public}s nullptr", (log));                \
            return;                                             \
        }                                                       \
    } while (0)

#define CHECK_POINTER_RETURN_VALUE(object, value, log)          \
    do {                                                        \
        if ((object) == nullptr) {                              \
            HILOGE("%{public}s nullptr", (log));                \
            return (value);                                     \
        }                                                       \
    } while (0)

#define CHECK_ERR_RETURN_VALUE(errNo, value, log)                               \
    do {                                                                        \
        auto result = (errNo);                                                  \
        if (result != ERR_OK) {                                                 \
            HILOGE("%{public}s err, code %{public}d", (log), result);           \
            return (value);                                                     \
        }                                                                       \
    } while (0)

#define CHECK_ERR_RETURN(errNo, log)                                            \
    do {                                                                        \
        auto result = (errNo);                                                  \
        if (result != ERR_OK) {                                                 \
            HILOGE("%{public}s err, code %{public}d", (log), result);           \
            return;                                                             \
        }                                                                       \
    } while (0)


enum {
    /**
     * Module type: Mechbody service core
     */
    MC_MODULE_TYPE_SERVICE = 0,
};

// offset of EB error, only be used in this file.
constexpr ErrCode MC_SERVICE_ERR_OFFSET = ErrCodeOffset(SUBSYS_DISTRIBUTEDHARDWARE, MC_MODULE_TYPE_SERVICE);

enum {
    /**
     * Result(96468992) for invalid parameters.
     */
    INVALID_PARAMETERS_ERR = MC_SERVICE_ERR_OFFSET,

    /**
     * Result(96468993) for get sa manager failed.
     */
    MECH_GET_SAMGR_EXCEPTION,

    /**
     * Result(96468994) for start mechbody service failed.
     */
    START_MECH_BODY_SERVICE_FAILED,

    /**
     * Result(96468995) for napi send data to service by ipc failed.
     */
    NAPI_SEND_DATA_FAIL,

    /**
     * Result(96468996) for napi receive data to service by ipc failed.
     */
    NAPI_RECV_DATA_FAIL,

    /**
     * Result(96468997) for The IPC token does not match.
     */
    IPC_TOKEN_DOES_NOT_MATCH,

    /**
     * Result(96468998) for The No matching service implementation is found.
     */
    NO_MATCHING_SERVICE_IMPL,

    /**
     * Result(96468999) for invalid pid.
     */
    INVALID_PID,

    /**
     * Result(96469000) for invalid mech id.
     */
    INVALID_MECH_ID,

    /**
     * Result(96469001) for invalid remote object.
     */
    INVALID_REMOTE_OBJECT,

    /**
     * Result(96469002) for invalid tracking layout.
     */
    INVALID_TRACKING_LAYOUT,

    /**
     * Result(96469003) for invalid rotate parameters.
     */
    INVALID_ROTATE_PARAM,

    /**
     * Result(96469004) for register callback failed.
     */
    REGISTER_CALLBACK_FAILED,

    /**
     * Result(96469005) for unregister callback failed.
     */
    UNREGISTER_CALLBACK_FAILED,

    /**
     * Result(96469006) for send callback info failed.
     */
    SEND_CALLBACK_INFO_FAILED,

    /**
     * Result(96469007) for query device list failed.
     */
    QUERY_DEVICE_LIST_FAILED,

    /**
     * Result(96469008) for unRegist failed.
     */
    UNREGISTER_FAILED_ERR,

    /**
     * Result(96469009) for notify failed.
     */
    NOTIFICATION_FAILED_ERR,

    /**
     * Result(96469010) for device not placed on mech.
     */
    DEVICE_NOT_PLACED_ON_MECH,

    /**
     * Result(96469011) for device not placed on mech.
     */
    DETECTED_OBJECTS_EMPTY,

    /**
     * Result(96469012) for camera info is empty.
     */
    CAMERA_INFO_IS_EMPTY,

    /**
     * Result(96469013) for detected object is empty.
     */
    DETECTED_OBJECT_IS_EMPTY,

    /**
     * Result(96469012) for can not found mach info.
     */
    MECH_INFO_NOT_FOUND,

    /**
     * Result(96469013) for device has connected.
     */
    MECH_HAS_CONNECTED,

    /**
     * Result(96469014) for device connect failed.
     */
    MECH_CONNECT_FAILED,

    /**
     * Result(96469015) for device disconnect failed.
     */
    MECH_DISCONNECT_FAILED,

    /**
     * Result(96469016) for Set racking enabl failed.
     */
    SET_TRACKING_ENABLE_FAILED,

    /**
     * Result(96469017) for device not connected.
     */
    NO_DEVICE_CONNECTED,

    /**
     * Result(96469018) for get device limited info failed.
     */
    GET_LIMIT_INFO_FAILED,

    /**
     * Result(96469019) for get current position failed.
     */
    GET_CURRENT_POSITION_FAILED,
};

enum MechNapiErrorCode {
    /**
     * Result(202) for permission denied.
     */
    PERMISSION_DENIED = 202,

    /**
     * Result(401) for parameter check failed.
     */
    PARAMETER_CHECK_FAILED = 401,

    /**
     * Result(33300001) for the system ability work abnormally.
     */
    SYSTEM_WORK_ABNORMALLY = 33300001,

    /**
     * Result(33300002) for device not connected.
     */
    DEVICE_NOT_CONNECTED = 33300002,

    /**
     * Result(33300003) for device not supported.
     */
    DEVICE_NOT_SUPPORTED = 33300003,
};
} // namespace MechBodyController
} // namespace OHOS
#endif // SERVICES_MECHBODY_CONTROLLER_LOG_H_
