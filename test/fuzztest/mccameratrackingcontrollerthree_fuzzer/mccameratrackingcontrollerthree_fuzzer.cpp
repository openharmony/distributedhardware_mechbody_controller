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

#include <fuzzer/FuzzedDataProvider.h>
#include "mccameratrackingcontrollerthree_fuzzer.h"
#include "mc_camera_tracking_controller.h"
#include "mechbody_controller_service.h"
#include "mechbody_controller_types.h"
#include "mechbody_controller_enums.h"
#include "mc_motion_manager.h"
#include "mc_connect_manager.h"
#include "mock_transport_send_adapter.h"
#include <memory>
#include <iremote_object.h>

using namespace OHOS;
using namespace OHOS::MechBodyController;

namespace {
constexpr size_t MAX_CMD_ID_LENGTH = 64;
constexpr int32_t MIN_LAYOUT_TYPE = 0;
constexpr int32_t MAX_LAYOUT_TYPE = 2;
constexpr int32_t TEST_MECH_ID = 1;

McCameraTrackingController* g_cameraTrackingController = nullptr;
std::shared_ptr<MotionManager> g_motionManager = nullptr;
std::shared_ptr<MockTransportSendAdapter> g_mockAdapter = nullptr;

void InitCameraTrackingController()
{
    if (g_cameraTrackingController == nullptr) {
        g_cameraTrackingController = &McCameraTrackingController::GetInstance();
    }
    
    // 初始化 MockTransportSendAdapter 和 MotionManager
    if (g_motionManager == nullptr) {
        g_mockAdapter = std::make_shared<MockTransportSendAdapter>();
        g_motionManager = std::make_shared<MotionManager>(g_mockAdapter, TEST_MECH_ID, false, 0x00000000);
        
        // 添加设备信息到 MechConnectManager
        MechInfo testMech;
        testMech.mechId = TEST_MECH_ID;
        testMech.mechType = MechType::PORTABLE_GIMBAL;
        testMech.mac = "AA:BB:CC:DD:EE:FF";
        testMech.state = AttachmentState::ATTACHED;
        testMech.deviceIdentifier = 0x00000000;
        MechConnectManager::GetInstance().AddMechInfo(testMech);
        MechConnectManager::GetInstance().NotifyMechState(TEST_MECH_ID, true);
        
        // 将 MotionManager 添加到 MechBodyControllerService
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
        MechBodyControllerService::GetInstance().motionManagers_[TEST_MECH_ID] = g_motionManager;
    }
}

void FuzzSearchTarget(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();

    std::string napiCmdId = provider.ConsumeRandomLengthString(MAX_CMD_ID_LENGTH);
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();

    // 测试不同的场景以覆盖更多代码路径
    // 测试 nullptr 参数
    auto targetInfoForNull = std::shared_ptr<TargetInfo>(nullptr);
    auto searchParams = std::make_shared<SearchParams>();
    g_cameraTrackingController->SearchTarget(napiCmdId, tokenId, targetInfoForNull, searchParams);
    // 测试 motionManagers 为空的情况
    {
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
        MechBodyControllerService::GetInstance().motionManagers_.clear();
    }
    auto targetInfo = std::make_shared<TargetInfo>();
    g_cameraTrackingController->SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);
    // 恢复 motionManager
    {
        std::lock_guard<std::mutex> lock(MechBodyControllerService::GetInstance().motionManagersMutex);
        MechBodyControllerService::GetInstance().motionManagers_[TEST_MECH_ID] = g_motionManager;
    }
    // 测试已经有 tracking target 的情况
    auto cameraInfo = g_cameraTrackingController->GetCurrentCameraInfo();
    if (cameraInfo != nullptr) {
        cameraInfo->trackingTargetNum = 1;
    }
    g_cameraTrackingController->SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);
    // 恢复
    if (cameraInfo != nullptr) {
        cameraInfo->trackingTargetNum = 0;
    }
    // 测试正常情况
    int32_t dataRangeMaxValue = 2;
    searchParams->direction =
        static_cast<SearchDirection>(provider.ConsumeIntegralInRange<int32_t>(0, dataRangeMaxValue));
    g_cameraTrackingController->SearchTarget(napiCmdId, tokenId, targetInfo, searchParams);
}

void FuzzSearchTargetRotateFinish(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    std::string cmdId = provider.ConsumeRandomLengthString(MAX_CMD_ID_LENGTH);
    g_cameraTrackingController->SearchTargetRotateFinish(cmdId);
}

void FuzzRegisterTrackingEventCallback(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    sptr<IRemoteObject> callback = nullptr;
    g_cameraTrackingController->RegisterTrackingEventCallback(tokenId, callback);
}

void FuzzUnRegisterTrackingEventCallback(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    g_cameraTrackingController->UnRegisterTrackingEventCallback(tokenId);
}

void FuzzSetTrackingLayoutWithToken(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    uint32_t tokenId = provider.ConsumeIntegral<uint32_t>();
    CameraTrackingLayout layout = static_cast<CameraTrackingLayout>(
        provider.ConsumeIntegralInRange<int32_t>(MIN_LAYOUT_TYPE, MAX_LAYOUT_TYPE));
    g_cameraTrackingController->SetTrackingLayout(tokenId, layout);
}

void FuzzUpdateCurrentCameraInfo(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    InitCameraTrackingController();
    CameraStandard::CaptureSessionInfo captureSessionInfo;
    captureSessionInfo.sessionId = provider.ConsumeIntegral<int32_t>();
    g_cameraTrackingController->UpdateCurrentCameraInfoByCaptureSessionInfo(captureSessionInfo);
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzSearchTarget(data, size);
    FuzzSearchTargetRotateFinish(data, size);
    FuzzRegisterTrackingEventCallback(data, size);
    FuzzUnRegisterTrackingEventCallback(data, size);
    FuzzSetTrackingLayoutWithToken(data, size);
    FuzzUpdateCurrentCameraInfo(data, size);
    return 0;
}