# Interface ETS API模块

## 概述

该模块承担ETS接口与C++接口的胶水层使用IDL定义接口，通过ANI (Ark Native Interface) 与服务层通信。

## 架构设计

```
interface/ets/mech_manager/
├── idl/
│   └── ohos.distributedHardware.mechanicManager.taihe  # IDL接口定义
├── include/
│   ├── ani_mech_manager.h               # ANI管理器
│   ├── ani_mech_manager_client.h        # ANI客户端
│   └── ani_mech_manager_stub.h          # ANI Stub
└── src/
    ├── ani_constructor.cpp              # 构造函数
    ├── ani_mech_manager.cpp             # ANI管理器实现
    ├── ani_mech_manager_client.cpp      # ANI客户端实现
    ├── ani_mech_manager_stub.cpp        # ANI Stub实现
    └── ohos.distributedHardware.mechanicManager.impl.cpp  # IDL实现
```

## 主要功能模块

### 1. AniMechManager
**文件位置**: `interface/ets/mech_manager/include/ani_mech_manager.h`, `interface/ets/mech_manager/src/ani_mech_manager.cpp`

**职责**:
- ETS API主入口
- 提供系统应用专用的机械体设备控制接口
- 管理ANI客户端生命周期

**核心接口**:
- 设备管理接口
- 运动控制接口
- 跟踪控制接口
- 事件回调注册接口

### 2. AniMechManagerClient
**文件位置**: `interface/ets/mech_manager/include/ani_mech_manager_client.h`, `interface/ets/mech_manager/src/ani_mech_manager_client.cpp`

**职责**:
- ANI客户端实现
- 负责与服务层的IPC通信
- 处理IPC回调

**核心接口**:
- 连接服务
- 发送IPC请求
- 处理IPC响应
- 处理IPC回调

### 3. AniMechManagerStub
**文件位置**: `interface/ets/mech_manager/include/ani_mech_manager_stub.h`, `interface/ets/mech_manager/src/ani_mech_manager_stub.cpp`

**职责**:
- ANI Stub实现
- 处理来自服务端的IPC调用

**核心接口**:
- 实现IPC接口
- 处理服务端回调

### 4. IDL实现
**文件位置**: `interface/ets/mech_manager/src/ohos.distributedHardware.mechanicManager.impl.cpp`

**职责**:
- IDL接口的C++实现
- 连接ANI接口和ETS接口

## 外部依赖

### 系统服务依赖
- `napi` - Native API
- `ipc` - IPC通信
- `safwk` - SA框架
- `hilog` - 日志服务

### 内部依赖
- `mechbody_controller_interface.h` - 服务接口定义
- `mechbody_controller_types.h` - 类型定义

## 使用限制

- 仅限系统应用使用
- 需要系统签名
- 提供比NAPI更完整的接口
