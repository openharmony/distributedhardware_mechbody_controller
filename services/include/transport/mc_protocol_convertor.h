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
#ifndef MC_PROTOCOL_CONVERTOR_H
#define MC_PROTOCOL_CONVERTOR_H

#include "mechbody_controller_types.h"
#include "mc_data_buffer.h"

namespace OHOS {
namespace MechBodyController {

class IProtocolConverter {
public:
    virtual ~IProtocolConverter() = default;

    virtual std::shared_ptr<MechDataBuffer> Convert(
        OptType optType, uint16_t seqNo, std::shared_ptr<MechDataBuffer> data) const = 0;

    virtual std::shared_ptr<MechDataBuffer> GetData(
        std::shared_ptr<MechDataBuffer> pclData, uint16_t &seqNo, bool &isAck) const = 0;

    virtual int32_t GetVersion() const = 0;

    virtual bool Validate(const uint8_t* data, size_t length) const = 0;
};

class ProtocolConverter : public IProtocolConverter {
private:
    static constexpr int32_t PROTOCOL_VERSION = 0;
    const std::string BT_PAYLOAD_MECH_SERVICE_NAME = "MechanicData";
public:
    std::shared_ptr<MechDataBuffer> Convert(
        OptType optType, uint16_t seqNo, std::shared_ptr<MechDataBuffer> data) const override;
    std::shared_ptr<MechDataBuffer> Convert(
        OptType optType, uint16_t seqNo, std::shared_ptr<MechDataBuffer> data, std::string serviceName);
    std::shared_ptr<MechDataBuffer> GetData(
        std::shared_ptr<MechDataBuffer> pclData, uint16_t &seqNo, bool &isAck) const override;

    int32_t GetVersion() const override { return PROTOCOL_VERSION; }

    bool Validate(const uint8_t* data, size_t length) const override;
private:
    std::string serviceName_ = BT_PAYLOAD_MECH_SERVICE_NAME;
};
} // namespace MechBodyController
} // namespace OHOS

#endif // MC_PROTOCOL_CONVERTOR_H
