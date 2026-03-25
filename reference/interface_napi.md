# Interface NAPI模块

## 概述

该模块承担ETS接口与C++接口的胶水层，应用调用ETS接口通过Native API与服务层通信。

## 架构设计

```
interface/napi/mech_manager/
├── include/
│   ├── js_mech_manager.h              # JS管理器
│   ├── js_mech_manager_client.h        # JS客户端
│   ├── js_mech_manager_service.h       # JS服务
│   ├── js_mech_manager_stubstub.h      # JS Stub
│   └── js_mech_menager_interface.h     # JS接口定义
└── src/
    ├── js_mech_manager.cpp              # JS管理器实现
    ├── js_mech_manager_client.cpp       # JS客户端实现
    ├── js_mech_manager_service.cpp      # JS服务实现
    └── js_mech_manager_stub.cpp         # JS Stub实现
```

## 主要功能模块

### 1. JsMechManager
**文件位置**: `interface/napi/mech_manager/include/js_mech_manager.h`, `interface/napi/mech_manager/src/js_mech_manager.cpp`

**职责**:
- NAPI API主入口
- 提供三方应用使用的机械体设备控制接口
- 管理JS客户端生命周期

**核心接口**:
- 设备管理接口
- 运动控制接口
- 跟踪控制接口
- 事件回调注册接口

### 2. JsMechManagerClient
**文件位置**: `interface/napi/mech_manager/include/js_mech_manager_client.h`, `interface/napi/mech_manager/src/js_mech_manager_client.cpp`

**职责**:
- JS客户端实现
- 负责与服务层的IPC通信
- 处理IPC回调
- 管理JavaScript回调

**核心接口**:
- 连接服务
- 发送IPC请求
- 处理IPC响应
- 处理IPC回调
- 管理JavaScript回调函数

### 3. JsMechManagerService
**文件位置**: `interface/napi/mech_manager/include/js_mech_manager_service.h`, `interface/napi/mech_manager/src/js_mech_manager_service.cpp`

**职责**:
- JS服务实现
- 提供服务端回调接口

**核心接口**:
- 实现服务端回调接口
- 处理服务端事件

### 4. JsMechManagerStub
**文件位置**: `interface/napi/mech_manager/include/js_mech_manager_stub.h`, `interface/napi/mech_manager/src/js_mech_manager_stub.cpp`

**职责**:
- JS Stub实现
- 处理来自服务端的IPC调用

**核心接口**:
- 实现IPC接口
- 处理服务端回调

### 5. JsMechManagerInterface
**文件位置**: `interface/napi/mech_manager/include/js_mech_menager_interface.h`

**职责**:
- JS接口定义
- 定义NAPI导出的接口

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

- 供三方应用使用
- 不需要系统签名
- 提供基础的设备控制接口
- 部分高级功能可能受限
