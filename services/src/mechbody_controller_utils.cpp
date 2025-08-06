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

#ifndef OHOS_MECHBODY_CONTROLLER_UTILS_H
#define OHOS_MECHBODY_CONTROLLER_UTILS_H

#include "mechbody_controller_utils.h"

#include <cmath>
#include <string>

#include "mechbody_controller_log.h"

namespace OHOS {
namespace MechBodyController {
const std::string TAG = "McUtils";
constexpr size_t INT32_SHORT_ID_LEN = 20;
constexpr size_t INT32_MIN_ID_LEN = 6;
constexpr size_t INT32_PLAINTEXT_LEN = 4;
constexpr float DEGREE_CONSTANT_180_FLOAT = 180.0f;

std::string GetAnonymStr(const std::string &value)
{
    std::string res;
    std::string tmpStr("******");
    size_t strLen = value.length();
    if (strLen < INT32_MIN_ID_LEN) {
        return tmpStr;
    }

    if (strLen <= INT32_SHORT_ID_LEN) {
        res += value[0];
        res += tmpStr;
        res += value[strLen - 1];
    } else {
        res.append(value, 0, INT32_PLAINTEXT_LEN);
        res += tmpStr;
        res.append(value, strLen - INT32_PLAINTEXT_LEN, INT32_PLAINTEXT_LEN);
    }

    return res;
}

std::string GetAnonymInt32(const int32_t value)
{
    std::string tmpStr = std::to_string(value);
    return GetAnonymStr(tmpStr);
}

std::string GetAnonymUint32(const uint32_t value)
{
    std::string tmpStr = std::to_string(value);
    return GetAnonymStr(tmpStr);
}

float RadToDegree(float radius)
{
    return radius * DEGREE_CONSTANT_180_FLOAT / M_PI;
}
} // namespace MechBodyController
} // namespace OHOS
#endif // OHOS_MECHBODY_CONTROLLER_UTILS_H
