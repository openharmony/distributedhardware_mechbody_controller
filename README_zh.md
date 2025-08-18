# 机械体设备控制器

## 简介

机械体设备控制器提供查询机械体设备连接状态和控制机械体设备运动的接口，方便连接和控制机械体设备。

机械体设备控制器包含以下常用功能：

- 机械体设备信息查询。
- 机械体设备状态查询。
- 机械体设备运动控制。

## 系统架构

**图 1**  OpenHarmony机械体设备控制器架构图<a name="fig4460722185514"></a></b>

![](figures/mechanic-architecture-zh.png)

- **Mechanic Kit**: 具身硬件开发套件，是机械体设备控制服务对外提供的接口集合。它为应用提供查询机械体设备、控制机械体设备等接口能力。
- **机械体设备控制服务**: 机械体设备控制器的核心服务，当前支持对机械体设备进行管理，通过设置位置轨迹和设置运动速度的方式控制机械体设备运动，根据摄像头人像识别的结果控制机械体设备跟随人物转动。机械体设备的动作管理能力，和基于声音方位控制机械体设备运动的能力正在规划中。
- **通用互联APP**: OpenHarmony设备的可选系统应用。负责发现和过滤机械体设备，方便设备开发者快速发现满足OpenHarmony机械体控制协议的配件设备，并通知机械体设备控制服务完成连接。
- **系统应用**: 设备系统应用，由设备开发者开发系统应用可以对机械体设备进行高阶控制，例如轨迹控制和速度控制。
- **三方应用**: 设备三方应用，由OpenHarmony应用生态开发者开发。三方应用可以查询已连接的机械体控制设备，并控制启动和关闭智能跟踪功能等。
- **蓝牙通信服务和相机服务**：机械体设备控制服务依赖的关键能力。依赖蓝牙通信服务与机械体设备进行通信；依赖相机服务获取相机参数和目标检测结果。
- **相机驱动**: 系统中实现人脸检测的模块，如果设备开发者需要支持机械体设备的人脸智能跟踪能力，必须在相机驱动中实现人像识别功能，并通过OpenHarmony相机HDI标准接口上报人脸检测结果。

## 目录

仓目录结构如下：

```shell
distributedhardware_mechbody_controller
├── etc             # 机械体设备控制器服务初始化配置
├── interface       # 机械体设备控制器接口及NAPI转换
├── sa_profile      # 机械体设备控制器SA配置
├── service         # 机械体设备控制器接口实现
├── test            # 机械体设备控制器测试代码
```

## 约束

- OpenHarmony开发设备必须支持 BLE（Bluetooth Low Energy）协议。
- OpenHarmony开发设备的相机驱动具备摄像头人脸检测能力，支持上报人脸Metadata数据，否则人脸跟踪功能不可用。
- 被控的机械体设备必须符合OpenHarmony MechanicKit南向协议标准，如厂商已对外宣称该设备支持OpenHarmony MechanicKit。
- 通用互联APP正在开发，发布后将补充下载路径。目前，可以使用test目录下的测试Demo进行机械体设备发现，或自行开发设备发现功能。
- 机械体设备南向协议正在标准化过程中，当前待提供。

## 使用说明

机械体设备控制器模块提供了以下三类功能接口，开发者可以根据使用诉求，综合使用这些接口：

- 应用开发者可以使用机械体设备查询接口，查询对应的信息。
- 当注册机械体设备状态变化感知时，可以通过回调查看设备对应的状态变化。
- 通过提供控制机械体设备的接口，可以让应用直接控制与主设备相连的机械体设备进行转动。

## 示例教程
机械体设备控制器模块提供了[设备转动控制开发指南](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/mechanicManager/rotation-control-guide.md)，系统应用可以对机械体设备进行高阶控制，如轨迹控制和速度控制等。同时提供了[智能拍摄跟踪开发指南](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/mechanicManager/camera-tracking-guide.md)(支持三方应用)，三方应用可以查询已连接的机械体设备，控制启动和关闭智能跟踪功能等。

## API参考

[系统API参考](https://gitcode.com/openharmony/docs/blob/ca6a74112dca41d78b4bb2ca2612aca7d2bce450/zh-cn/application-dev/reference/apis-mechanic-kit/js-apis-mechanicManager-sys.md)和[开放API参考](https://gitcode.com/openharmony/docs/blob/ca6a74112dca41d78b4bb2ca2612aca7d2bce450/zh-cn/application-dev/reference/apis-mechanic-kit/js-apis-mechanicManager.md)分别提供了接口的说明文档，可以帮助开发者快速查找到指定接口的详细描述和调用方法。

## 相关仓

distributedhardware_mechbody_controller
