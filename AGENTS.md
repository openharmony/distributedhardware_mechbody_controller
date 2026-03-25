# OpenHarmony Mechbody Controller

## 基本信息

**项目名称**: OpenHarmony Mechbody Controller  
**仓库地址**: https://gitcode.com/OpenHarmony/distributedhardware_mechbody_controller  
**许可证**: Apache License 2.0  
**版本**: 1.0  
**系统要求**: OpenHarmony标准系统  

**功能描述**:
机械体设备控制器是OpenHarmony操作系统中用于控制机械体设备的系统服务模块。机械体设备是具备自主运动能力的智能机械，包括：
- 云台（Gimbal）- 便携式云台设备
- 机械臂（Robotic Arm）- 多轴机械臂控制
- 自动升降架（Lifting Rack）- XYZ三轴升降控制
- 机械车（Robotic Vehicle）- 差速驱动移动平台

**核心功能**:
1. **设备信息查询** - 查询已连接的机械体设备信息（设备ID、类型、名称、MAC地址等）
2. **设备状态感知** - 感知和反馈设备状态变化（连接/断开、姿态变化、轴状态变化）
3. **运动控制** - 控制设备运动（位置控制、速度控制、轨迹控制）
4. **智能跟踪** - 基于摄像头识别的目标自动跟踪
5. **设备管理** - 设备连接/断开管理、配对状态管理

## 架构设计

```
┌─────────────────────────────────────────────────────────────┐
│                     应用层 (Applications)                    │
│  ┌──────────────────┐         ┌──────────────────┐          │
│  │   系统应用        │         │   三方应用        │          │
│  └────────┬─────────┘         └────────┬─────────┘          │
└───────────┼────────────────────────────┼────────────────────┘
            │                            │
┌───────────┼────────────────────────────┼────────────────────┐
│           │     Mechanic Kit (API)     │                    │
│  ┌────────┴────────────────────────────┴────────┐           │
│  │     IMechBodyController (IPC Interface)      │           │
│  │  - ANI API (ANI胶水层)                       │           │
│  │  - NAPI API (NAPI胶水层)                     │           │
│  └──────────────────────────────────────────────┘           │
└─────────────────────────────────────────────────────────────┘
            │
┌───────────┼─────────────────────────────────────────────────┐
│           │  Mechanical Device Control Service (SA)         │
│  ┌────────┴─────────────────────────────────────────────┐   │
│  │  MechBodyControllerService                           │   │
│  │  ├─ McControllerManager (控制管理)                    │   │
│  │  ├─ McCameraTrackingController (相机跟踪控制)         │   │
│  │  ├─ McMotionManager (运动管理)                        │   │
│  │  └─ McConnectManager (连接管理)                       │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
            │                    │
┌───────────┼────────────────────┼────────────────────────────┐
│           │                    │                            │
│  ┌────────┴────┐    ┌─────────┴────────┐    ┌───────────┐   │
│  │ 蓝牙通信服务 │    │     相机服务      │    │ 传感器服务 │   │
│  └─────────────┘    └──────────────────┘    └───────────┘   │
└─────────────────────────────────────────────────────────────┘
            │
┌───────────┼─────────────────────────────────────────────────┐
│  ┌────────┴──────────────────────────────────────────────┐  │
│  │              机械体设备 (BLE设备)                      │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## 目录结构

```
distributedhardware_mechbody_controller/
├── etc/                          # 服务初始化配置
│   └── init/
│       └── mechbody_controller.cfg
├── interface/                    # 接口层
│   ├── ets/mech_manager/         # ANI胶水层
│   │   ├── idl/                  # 接口定义文件
│   │   ├── include/              # 头文件
│   │   └── src/                  # 实现文件
│   └── napi/mech_manager/        # NAPI胶水层
│       ├── include/
│       └── src/
├── sa_profile/                   # SA (System Ability) 配置
│   └── 5005.json
├── services/                     # 服务层实现
│   ├── include/                  # 头文件
│   │   ├── controller/           # 控制器
│   │   ├── connect/              # 连接管理
│   │   ├── motion/               # 运动管理
│   │   ├── transport/            # 传输层
│   │   ├── utils/                # 工具类
│   │   └── dotReport/            # 事件上报
│   └── src/                      # 实现文件
│       ├── controller/
│       ├── connect/
│       ├── motion/
│       ├── transport/
│       ├── utils/
│       └── dotReport/
├── test/                         # 测试代码
│   ├── unittest/                 # 单元测试
│   ├── fuzztest/                 # 模糊测试
│   └── demo/                     # 示例应用
├── figures/                      # 文档图片
├── bundle.json                   # 构建配置
├── mechbody.gni                  # GN构建配置
├── README.md                     # 英文文档
└── README_zh.md                  # 中文文档
```

## 核心功能

### 设备管理
- 设备连接/断开监听和通知
- 设备信息查询（设备ID、类型、名称、MAC地址等）
- 设备配对状态管理
- 设备GATT连接状态管理
- 设备HID连接状态管理

### 运动控制
- 位置控制（按角度旋转）- RotateByDegree
- 速度控制（按速度旋转）- RotateBySpeed
- 轨迹控制（旋转到指定欧拉角）- RotateToEulerAngles
- 停止运动 - StopMoving
- 运动状态查询（当前角度、速度限制、角度限制、轴状态等）

### 智能跟踪
- 跟踪启用/禁用控制
- 跟踪布局设置
- 目标搜索功能
- 跟踪事件回调通知
- 相机元数据处理和人脸检测

### 事件通知
- 设备连接/断开事件
- 旋转轴状态变化事件
- 命令执行结果事件
- 相机按键事件
- 轮数据事件

## 子模块文档

### 核心服务模块
- [services.md](reference/services.md) - 服务层核心实现，包括主服务、控制器、连接管理、运动管理等

### 接口层模块
- [interface_ets.md](reference/interface_ets.md) - ANI胶水层
- [interface_napi.md](reference/interface_napi.md) - NAPI胶水层

### 传输层模块
- [transport.md](reference/transport.md) - BLE通信协议实现和命令管理

## 构建

### 构建系统
- **构建工具**: GN (Generate Ninja) + Ninja
- **构建配置文件**: `bundle.json`, `mechbody.gni`
- **构建产物**:
  - `mechbody_service` - 服务组件
  - `mechanic_manager_group_ani` - ANI库
  - `mechanicmanager_napi` - NAPI库

### 构建命令

**完整构建**:
```bash
# 在OpenHarmony源码根目录执行
./build.sh --product-name <产品名> --build-target mechbody_controller
```

**单独构建服务**:
```bash
./build.sh --product-name <产品名> --build-target //foundation/distributedhardware/mechbody_controller/services:mechbody_service
```

**构建ETS API**:
```bash
./build.sh --product-name <产品名> --build-target //foundation/distributedhardware/mechbody_controller/interface/ets/mech_manager:mechanic_manager_group_ani
```

**构建NAPI**:
```bash
./build.sh --product-name <产品名> --build-target //foundation/distributedhardware/mechbody_controller/interface/napi/mech_manager:mechanicmanager_napi
```

### 构建产物位置
- 服务: `out/<产品名>/packages/system/lib/libmechbody_service.z.so`
- ETS API: `out/<产品名>/packages/system/lib/libmechanic_manager_ani.z.so`
- NAPI: `out/<产品名>/packages/system/lib/libmechanicmanager_napi.z.so`

### 部署方式
- 作为系统服务集成到OpenHarmony镜像中
- SA ID: 5005
- 服务名称: `mechbody_controller_service`

## 外部依赖

### 系统能力
- `SystemCapability.Mechanic.Core`

### 系统要求
- 必须支持BLE（蓝牙低功耗）协议
- 相机驱动必须具备人脸检测能力并支持上报人脸Metadata数据
- OpenHarmony标准系统

## 关键规则

### 设备协议约束
1. **设备要求**:
   - 机械体设备仅支持特定厂商的设备
   - 设备必须符合OpenHarmony MechanicKit南向协议标准（待发布）

2. **系统要求**:
   - 必须支持BLE（蓝牙低功耗）协议
   - 相机驱动必须具备人脸检测能力并支持上报人脸Metadata数据

### 权限要求
- 系统API需要系统签名
- 蓝牙权限: `ohos.permission.USE_BLUETOOTH`
- 相机权限: `ohos.permission.CAMERA`

### 线程安全
- 多线程访问共享数据需要使用互斥锁
- 回调函数需要在事件处理器线程中执行
- IPC调用需要考虑跨线程安全性

### 错误处理
- 所有公共接口返回 `int32_t` 错误码
- 错误码定义在 `mechbody_controller_ipc_interface_interface_code.h`
- 关键错误需要通过HiSysEvent上报

## 核心接口

### NAPI接口层

#### MechManager
**文件位置**: `interface/napi/mech_manager/js_mech_manager.h`

**职责**: NAPI主入口类，提供JavaScript可调用的接口

**公共接口**:
- `On` - 注册事件回调
- `Off` - 取消注册事件回调
- `GetAttachedDevices` - 获取已连接设备列表
- `SetUserOperation` - 设置用户操作
- `SetCameraTrackingEnabled` - 设置相机跟踪启用状态
- `GetCameraTrackingEnabled` - 获取相机跟踪启用状态
- `SetCameraTrackingLayout` - 设置相机跟踪布局
- `GetCameraTrackingLayout` - 获取相机跟踪布局
- `Rotate` - 按角度旋转
- `RotateToEulerAngles` - 旋转到指定欧拉角
- `GetMaxRotationTime` - 获取最大旋转时间
- `GetMaxRotationSpeed` - 获取最大旋转速度旋转
- `RotateBySpeed` - 按速度旋转
- `StopMoving` - 停止运动
- `GetCurrentAngles` - 获取当前角度
- `GetRotationLimits` - 获取旋转角度限制
- `GetRotationAxesStatus` - 获取旋转轴状态
- `SearchTarget` - 搜索目标

### 回调接口层

#### IJsMechManager
**文件位置**: `interface/napi/mech_manager/js_mech_menager_interface.h`

**职责**: JavaScript管理器回调接口

**公共接口**:
- `AttachStateChangeCallback` - 设备连接状态变化回调
- `TrackingEventCallback` - 跟踪事件回调
- `RotationAxesStatusChangeCallback` - 旋转轴状态变化回调
- `RotatePromiseFulfillment` - 旋转Promise完成回调
- `SearchTargetCallback` - 搜索目标回调

**设计模式**: 单例模式

## 接口调用流程

### 设备管理流程
1. 应用调用 `GetAttachedDevices()` 获取已连接设备列表
2. 应用调用 `On('attachStateChange', callback)` 注册设备连接状态变化回调
3. 设备连接/断开时，服务端通过 `AttachStateChangeCallback` 通知应用

### 运动控制流程
1. 应用调用 `Rotate()` 或 `RotateBySpeed()` 发送运动控制命令
2. 命令通过IPC发送到服务端
3. 服务端通过BLE发送到设备
4. 设备执行命令并返回结果
5. 服务端通过 `RotatePromiseFulfillment` 通知应用结果

### 跟踪控制流程
1. 应用调用 `SetCameraTrackingEnabled(true)` 启用跟踪
2. 应用调用 `SetCameraTrackingLayout()` 设置跟踪布局
3. 应用调用 `On('trackingEvent', callback)` 注册跟踪事件回调
4. 相机检测到目标时，服务端通过 `TrackingEventCallback` 通知应用

### 目标搜索流程
1. 应用调用 `SearchTarget()` 发送目标搜索请求
2. 服务端执行搜索操作
3. 搜索完成后，服务端通过 `SearchTargetCallback` 通知应用结果
