# Mechbody Controller

## Introduction

The Mechanical Device Controller provides developers with a set of APIs for discovering and controlling compatible mechanical devices, enabling seamless connectivity and precise control of target devices.

The Mechbody Controller includes the following common features:

- Accessory information query
- Perception and feedback of accessory status changes
- Controlling accessory movements

## System Architecture

**Figure 1** OpenHarmony Mechbody Controller Architecture
![dcb31eb2_8158620.png](https://raw.gitcode.com/user-images/assets/6945035/1b102d63-9451-4188-820a-e65249fd655d/dcb31eb2_8158620.png 'dcb31eb2_8158620.png')
- **Mechanic Kit**: A set of APIs provided by the Mechanical Device Control Service, offering applications the capability to discover and control mechanical devices through standardized interfaces.
- **Mechanical Device Control Service**: The core service of the mechanical device controller. Currently, it supports managing mechanical devices, controlling their movement by setting position trajectories and motion speeds, and directing mechanical devices to rotate and follow individuals based on camera face recognition results. Motion management capabilities for mechanical devices and the ability to control their movement based on sound direction are under planning.
- **Universal Interconnect APP**: An optional system application for OpenHarmony devices. It is responsible for discovering and filtering mechanical devices, enabling device developers to quickly identify accessory devices that comply with the OpenHarmony mechanical control protocol, and notifying the mechanical device control service to establish connections.
- **System Application**: A device system application developed by device developers. System applications can perform advanced control of mechanical devices, such as trajectory control and speed control.
- **Third-Party Application**: A third-party device application developed by OpenHarmony ecosystem developers. Third-party applications can query connected mechanical control devices and control the activation and deactivation of smart tracking features, among other functions.
- **Bluetooth Communication Service and Camera Service**: Key capabilities relied upon by the mechanical device control service. It depends on the Bluetooth communication service for Bluetooth interactions with mechanical devices and on the camera service to obtain camera parameters and target detection results from the camera.
- **Camera Driver**: A module in the system that implements face detection. If device developers need to support the face smart tracking capability of mechanical devices, they must implement face recognition functionality in the camera driver and report face detection results through the OpenHarmony Camera HDI standard interface.

## Directory Structure

The repository directory structure is as follows:

```shell
distributedhardware_mechbody_controller
├── etc             # Initialization configuration for mechbody controller service
├── interface       # Mechbody controller interfaces and NAPI conversion
├── sa_profile      # SA configuration for mechbody controller
├── service         # Implementation of mechbody controller interfaces
├── test            # Test code for mechbody controller
```

## Constraints

- Mechanical devices only supports equipment from specific manufacturers.

- The development must support BLE (Bluetooth Low Energy).

- The development device must have camera-based face detection capability and support reporting face metadata.

- For the universal connectivity app development, the download path will be provided after release.

- The southbound protocol will be provided later, as it is currently in the standardization process.

## Usage Guide

Three major categories of functional interfaces are provided. Developers can use one or more categories based on their requirements:

- Application developers can use the accessory query interface to query corresponding accessory information.

- When registering for accessory status change notifications, device status changes can be monitored through callbacks.

- The accessory control interface is provided, allowing users to easily develop and control accessories using the interface.

## Example Tutorials

We provide the [Rotation Control Development Guide](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/mechanicManager/rotation-control-guide.md). System applications can perform advanced control of mechanical devices, such as trajectory control and speed control. Additionally, the [Smart Tracking Development Guide]((https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/mechanicManager/camera-tracking-guide.md)) (supports third-party applications) is available. Third-party applications can query connected mechanical control devices and control the activation and deactivation of smart tracking features. These resources offer richer development references, helping developers understand functional logic and improve development efficiency.

## API Reference

The [System API Reference](https://gitcode.com/openharmony/docs/blob/ca6a74112dca41d78b4bb2ca2612aca7d2bce450/zh-cn/application-dev/reference/apis-mechanic-kit/js-apis-mechanicManager-sys.md) and [Non-System API Reference](https://gitcode.com/openharmony/docs/blob/ca6a74112dca41d78b4bb2ca2612aca7d2bce450/zh-cn/application-dev/reference/apis-mechanic-kit/js-apis-mechanicManager.md) provide interface documentation to help developers quickly find detailed descriptions and invocation methods for specific interfaces.

## Related Repositories

distributedhardware_mechbody_controller
