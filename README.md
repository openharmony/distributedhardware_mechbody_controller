# Mechbody Controller

## Introduction

The Mechbody Controller provides developers with a set of simple and easy-to-understand interfaces, enabling convenient system integration and connection with corresponding accessories.

The Mechbody Controller includes the following common features:

- Accessory information query
- Perception and feedback of accessory status changes
- Controlling accessory movements

## System Architecture

**Figure 1** OpenHarmony Mechbody Controller Architecture
![dd8fbe80_8158620.png](https://raw.gitcode.com/user-images/assets/6945035/cfb47fd2-eaec-4367-91e5-fb738ecc28fc/dd8fbe80_8158620.png 'dd8fbe80_8158620.png')

- **Mechanic Kit**: Provides applications with query and control interfaces. Commands such as queries and controls are sent to the accessory side via Bluetooth connection to direct accessories to perform corresponding actions.

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

Requires specified accessories and alignment with the transmission protocol.

## Usage Guide

Three major categories of functional interfaces are provided. Developers can use one or multiple categories based on requirements:

- Application developers can use accessory query interfaces to retrieve corresponding accessory information

- When registering for accessory status change notifications, device status changes can be monitored via callbacks

- Accessory control interfaces are provided, enabling developers to conveniently control accessories through interfaces

## Related Repositories

distributedhardware_mechbody_controller
