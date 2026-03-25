# Services 核心服务模块

## 概述

Services模块是机械体设备控制器的核心服务层实现，负责设备连接管理、运动控制、智能跟踪等功能。该模块作为SystemAbility运行，提供IPC接口供应用层调用。

## 架构设计

```
Services/
├── MechBodyControllerService (主服务)
│   ├── McControllerManager (控制管理器)
│   │   └── McCameraTrackingController (相机跟踪控制器)
│   ├── McMotionManager (运动管理器)
│   └── McConnectManager (连接管理器)
├── BleSendManager (BLE发送管理器)
└── TransportSendAdapter (传输适配器)
```

## 主要功能模块

### 1. MechBodyControllerService
**文件位置**: `services/include/mechbody_controller_service.h`, `services/src/mechbody_controller_service.cpp`

**职责**: SystemAbility主服务类，实现IPC接口，管理设备连接/断开事件、运动管理器生命周期、回调注册

**核心接口**: 设备连接/断开处理、设备列表查询、运动控制（RotateByDegree/RotateBySpeed/RotateToEulerAngles/StopMoving）、跟踪控制（SetTrackingEnabled/GetTrackingEnabled/SetTrackingLayout/GetTrackingLayout）、目标搜索（SearchTarget/SearchTargetEnd）、回调注册

**设计模式**: 单例模式

### 2. McControllerManager
**文件位置**: `services/include/controller/mc_controller_manager.h`, `services/src/controller/mc_controller_manager.cpp`

**职责**: 协调各子模块工作，跟踪功能管理，用户设置管理

**核心接口**: 跟踪启用/禁用管理、跟踪事件回调管理、跟踪布局设置、目标搜索管理

**设计模式**: 单例模式

### 3. McCameraTrackingController
**文件位置**: `services/include/controller/mc_camera_tracking_controller.h`, `services/src/controller/mc_camera_tracking_controller.cpp`

**职责**: 处理相机元数据和人脸检测结果，计算跟踪控制参数，目标检测和匹配，ROI计算和调整

**核心功能**: 相机会话配置监听、焦点跟踪信息处理、目标检测和匹配（支持头/脸/身体）、IOU目标匹配、目标类型转换、传感器姿态计算、布局偏移调整、目标搜索执行

### 4. McMotionManager
**文件位置**: `services/include/motion/mc_motion_manager.h`, `services/src/motion/mc_motion_manager.cpp`

**职责**: 执行运动控制命令，管理运动状态，设备状态查询和更新

**核心接口**: 运动控制（Rotate/RotateBySpeed/StopRotate）、状态查询（GetSpeedControlTimeLimit/GetRotateSpeedLimit/GetCurrentPosition/GetRotationLimit/GetRotationAxesStatus）、跟踪控制（SetMechCameraTrackingFrame/GetMechCameraTrackingEnabled/SetMechCameraTrackingEnabled/SetMechCameraTrackingLayout/GetMechCameraTrackingLayout）、设备信息（SetMechCameraInfo/GetMechBaseInfo/GetMechCapabilityInfo）、预设动作（PerformPresetAction）

**事件处理**: 设备姿态通知、设备参数通知、通用事件通知、执行结果通知、轮数据通知、按键事件通知、跟踪状态通知

### 5. McConnectManager
**文件位置**: `services/include/connect/mc_connect_manager.h`, `services/src/connect/mc_connect_manager.cpp`

**职责**: 处理设备连接/断开，蓝牙状态监听，设备信息管理

**核心接口**: 初始化/去初始化、设备变化监听器管理、设备连接/断开通知、设备信息查询、蓝牙状态管理、设备信息管理、GATT/HID状态管理

**设计模式**: 单例模式

### 6. BleSendManager
**文件位置**: `services/include/ble_send_manager.h`, `services/src/ble_send_manager.cpp`

**职责**: BLE数据发送管理，GATT连接管理，HID连接管理

**核心接口**: SendData、ConnectGatt/DisconnectGatt、ConnectHid/DisconnectHid

### 7. BluetoothStateAdapter
**文件位置**: `services/include/connect/bluetooth_state_adapter.h`, `services/src/connect/bluetooth_state_adapter.cpp`

**职责**: 蓝牙状态适配，蓝牙状态变化监听

### 8. BluetoothStateListener
**文件位置**: `services/include/connect/bluetooth_state_listener.h`, `services/src/connect/bluetooth_state_listener.cpp`

**职责**: 蓝牙状态监听器，蓝牙状态变化通知

### 9. McControllerIpcDeathListener
**文件位置**: `services/include/controller/mc_controller_ipc_death_listener.h`, `services/src/controller/mc_controller_ipc_death_listener.cpp`

**职责**: IPC死亡监听器，处理客户端进程死亡

## 外部依赖

### 系统服务依赖
- `eventhandler` - 事件处理器
- `bluetooth` - 蓝牙服务
- `camera_framework` - 相机框架
- `sensor` - 传感器服务
- `hilog` - 日志服务
- `hisysevent` - 系统事件
- `ipc` - IPC通信
- `safwk` - SA框架
- `input` - 输入服务

### 内部依赖
- `mechbody_controller_types.h` - 类型定义
- `mechbody_controller_ipc_interface_code.h` - IPC接口代码
- `mechbody_controller_log.h` - 日志工具
- `mechbody_controller_utils.h` - 工具函数

## 关键数据结构

### DeviceStatus
设备状态信息，包含：设备启用状态、设备状态信息、布局参数、持续时间、轴限制标志、欧拉角、旋转角度限制、旋转速度限制、设备基础信息、设备能力信息、旋转轴状态、跟踪状态

### MechNapiCommandCallbackInfo
NAPI命令回调信息，包含：tokenId、napiCmdId、willLimitChange

## 线程模型

- **主线程**: 处理IPC调用
- **事件处理线程**: 处理设备事件和回调
- **跟踪检查线程**: 检查跟踪状态和超时

## 错误处理

- 所有公共接口返回 `int32_t` 错误码
- 错误码定义在 `mechbody_controller_ipc_interface_code.h`
- 关键错误通过HiSysEvent上报
