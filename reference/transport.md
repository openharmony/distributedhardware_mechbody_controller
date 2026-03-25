# Transport 传输层模块

## 概述

Transport模块负责机械体设备与系统之间的BLE通信协议实现。该模块定义了设备通信协议、命令管理、数据转换和事件订阅等功能。

## 架构设计

```
Transport/
├── McSendAdapter (发送适配器)
├── McProtocolConvertor (协议转换器)
├── McDataBuffer (数据缓冲区)
├── McEventListener (事件监听器)
├── McSubscriptionCenter (订阅中心)
└── command/
    └── 0x01/ (协议版本0x01命令)
        ├── mc_action_gimbal_feature_control_cmd.h
        ├── mc_get_mech_camera_tracking_layout_cmd.h
        ├── mc_get_mech_limit_info_cmd.h
        ├── mc_get_mech_real_name_cmd.h
        ├── mc_register_mech_camera_key_event_cmd.h
        ├── mc_register_mech_control_result_cmd.h
        ├── mc_register_mech_position_info_cmd.h
        ├── mc_register_mech_state_info_cmd.h
        ├── mc_register_mech_tracking_enable_cmd.h
        ├── mc_register_mech_wheel_data_cmd.h
        ├── mc_set_mech_camera_info_cmd.h
        ├── mc_set_mech_camera_tracking_enable_cmd.h
        ├── mc_set_mech_camera_tracking_frame_cmd.h
        ├── mc_set_mech_camera_tracking_layout_cmd.h
        ├── mc_set_mech_config_cmd.h
        ├── mc_set_mech_hid_preemptive_cmd.h
        ├── mc_set_mech_rotation_by_speed_cmd.h
        ├── mc_set_mech_rotation_cmd.h
        ├── mc_set_mech_rotation_trace_cmd.h
        └── mc_set_mech_stop_cmd.h
```

## 主要功能模块

### 1. McSendAdapter
**文件位置**: `services/include/transport/mc_send_adapter.h`

**职责**:
- 发送适配器接口
- 提供数据发送接口
- 管理发送状态

**核心接口**:
- `SendData` - 发送数据到设备
- `RegisterEventListener` - 注册事件监听器
- `UnRegisterEventListener` - 取消注册事件监听器

### 2. McProtocolConvertor
**文件位置**: `services/include/transport/mc_protocol_convertor.h`

**职责**:
- 协议转换器
- 将应用层数据转换为BLE协议格式
- 将BLE协议数据转换为应用层数据

**核心接口**:
- `EncodeCommand` - 编码命令
- `DecodeResponse` - 解码响应
- `DecodeNotify` - 解码通知

### 3. McDataBuffer
**文件位置**: `services/include/transport/mc_data_buffer.h`

**职责**:
- 数据缓冲区管理
- 管理发送和接收数据缓冲

**核心接口**:
- `WriteData` - 写入数据
- `ReadData` - 读取数据
- `ClearData` - 清空数据

### 4. McEventListener
**文件位置**: `services/include/transport/mc_event_listener.h`

**职责**:
- 事件监听器接口
- 处理来自设备的各种事件通知

**核心接口**:
- `MechAttitudeNotify` - 设备姿态通知
- `MechParamNotify` - 设备参数通知
- `MechGenericEventNotify` - 通用事件通知
- `MechExecutionResultNotify` - 执行结果通知
- `MechWheelZoomNotify` - 轮数据通知
- `MechButtonEventNotify` - 按键事件通知
- `MechTrackingStatusNotify` - 跟踪状态通知

### 5. McSubscriptionCenter
**文件位置**: `services/include/transport/mc_subscription_center.h`

**职责**:
- 订阅中心
- 管理事件订阅
- 分发事件到订阅者

**核心接口**:
- `Subscribe` - 订阅事件
- `UnSubscribe` - 取消订阅
- `Notify` - 通知事件

## BLE通信协议

### 协议结构

```
| Header | CmdType | SeqId | DataLength | Data | Checksum |
```

### 协议类型

**通知类型 (0x02)**:
- 0x0240 - 按钮事件通知
- 0x0241 - 参数通知
- 0x0242 - 姿态通知
- 0x0243 - 执行结果通知
- 0x0244 - 轮数据通知
- 0x0245 - 跟踪启用通知

**操作类型**:
- REQ (请求) - 客户端发送请求
- RSP (响应) - 设备返回响应
- RPT (上报) - 设备主动上报

### 命令列表 (协议版本0x01)

**控制命令**:
- `mc_set_mech_rotation_cmd` - 设置旋转命令
- `mc_set_mech_rotation_by_speed_cmd` - 设置按速度旋转命令
- `mc_set_mech_rotation_trace_cmd` - 设置旋转轨迹命令
- `mc_set_mech_stop_cmd` - 设置停止命令
- `mc_action_gimbal_feature_control_cmd` - 云台功能控制命令
- `mc_set_mech_hid_ preemptive_cmd` - 设置HID抢占命令

**信息查询命令**:
- `mc_get_mech_limit_info_cmd` - 获取限制信息命令
- `mc_get_mech_real_name_cmd` - 获取真实名称命令
- `mc_get_mech_camera_tracking_layout_cmd` - 获取相机跟踪布局命令

**配置命令**:
- `mc_set_mech_config_cmd` - 设置配置命令
- `mc_set_mech_camera_info_cmd` - 设置相机信息命令

**跟踪命令**:
- `mc_set_mech_camera_tracking_enable_cmd` - 设置相机跟踪启用命令
- `mc_set_mech_camera_tracking_frame_cmd` - 设置相机跟踪帧命令
- `mc_set_mech_camera_tracking_layout_cmd` - 设置相机跟踪布局命令

**注册命令**:
- `mc_register_mech_position_info_cmd` - 注册位置信息命令
- `mc_register_mech_state_info_cmd` - 注册状态信息命令
- `mc_register_mech_control_result_cmd` - 注册控制结果命令
- `mc_register_mech_camera_key_event_cmd` - 注册相机按键事件命令
- `mc_register_mech_tracking_enable_cmd` - 注册跟踪启用命令
- `mc_register_mech_wheel_data_cmd` - 注册轮数据命令

## 外部依赖

### 系统服务依赖
- `bluetooth` - 蓝牙服务
- `hilog` - 日志服务

### 内部依赖
- `mechbody_controller_types.h` - 类型定义
- `mechbody_controller_log.h` - 日志工具

## 协议特性

1. **版本管理**: 支持多版本协议，当前版本为0x01
2. **命令工厂**: 使用工厂模式创建命令对象
3. **序列号管理**: 每个命令分配唯一序列号
4. **校验机制**: 支持数据校验
5. **事件订阅**: 支持事件订阅和分发
6. **异步通信**: 支持异步命令和响应

## 错误处理

- 协议解析错误
- 校验和错误
- 命令超时
- 设备无响应
