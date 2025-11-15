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
import ble from '@ohos.bluetooth.ble';
import constant from '@ohos.bluetooth.constant';
import { BusinessError } from '@ohos.base';
import { logger } from '../utils/Logger';
import prompt from '@system.prompt';

const TAG: string = '[GattClientManager]';

export class GattClientManager {
  device: string | undefined = undefined;
  gattClient: ble.GattClientDevice | undefined = undefined;
  connectState: ble.ProfileConnectionState = constant.ProfileConnectionState.STATE_DISCONNECTED;
  // myServiceUuid: string = '00001810-0000-1000-8000-00805F9B34FB';
  // myCharacteristicUuid: string = '00001820-0000-1000-8000-00805F9B34FB';
  // myFirstDescriptorUuid: string = '00002902-0000-1000-8000-00805F9B34FB'; // 2902一般用于notification或者indication
  // mySecondDescriptorUuid: string = '00002903-0000-1000-8000-00805F9B34FB';
  found: boolean = false;

  // old
  // myServiceUuid: string = '0000FFE0-0000-1000-8000-00805F9B34FB';
  // myCharacteristicUuid: string = '0000FFE5-0000-1000-8000-00805F9B34FB';
  //
  // myCharacteristicUuid_write: string = '0000FFF5-0000-1000-8000-00805F9B34FB';
  // myCharacteristicUuid_notify: string = '0000FFE4-0000-1000-8000-00805F9B34FB';
  //
  // myDescriptorUuid_read: string = '00002902-0000-1000-8000-00805F9B34FB';

  // new 2025/08/14
  myServiceUuid: string = '15F1E600-A277-43FC-A484-DD39EF8A9100';
  myCharacteristicUuid: string = '15F1E602-A277-43FC-A484-DD39EF8A9100';  //write : 15F1E602-A277-43FC-A484-DD39EF8A9100 15f1e602-a277-43fc-a484-dd39ef8a9100
  myDescriptorUuid_read: string = '00002902-0000-1000-8000-00805F9B34FB';

  // 构造BLEDescriptor
  private initDescriptor(des: string, value: ArrayBuffer): ble.BLEDescriptor {
    let descriptor: ble.BLEDescriptor = {
      serviceUuid: this.myServiceUuid,
      characteristicUuid: this.myCharacteristicUuid,
      descriptorUuid: des,
      descriptorValue: value
    };
    return descriptor;
  }

  // 构造BLECharacteristic
  private initCharacteristic(): ble.BLECharacteristic {
    let descriptors: Array<ble.BLEDescriptor> = [];
    let descBuffer = new ArrayBuffer(2);
    let descValue = new Uint8Array(descBuffer);
    descValue[0] = 11;
    descValue[1] = 12;
    descriptors[0] = this.initDescriptor(this.myDescriptorUuid_read, new ArrayBuffer(2));
    // descriptors[0] = this.initDescriptor(this.myFirstDescriptorUuid, new ArrayBuffer(2));
    // descriptors[1] = this.initDescriptor(this.mySecondDescriptorUuid, descBuffer);
    let charBuffer = new ArrayBuffer(4);
    let charValue = new Uint8Array(charBuffer);
    charValue[0] = 1;
    charValue[1] = 2;
    let characteristic: ble.BLECharacteristic = {
      serviceUuid: this.myServiceUuid,
      characteristicUuid: this.myCharacteristicUuid,
      characteristicValue: charBuffer,
      descriptors: descriptors
    };
    return characteristic;
  }

  private logCharacteristic(char: ble.BLECharacteristic) {
    let message = 'logCharacteristic uuid:' + char.characteristicUuid + '\n';
    let value = new Uint8Array(char.characteristicValue);
    message += 'logCharacteristic value: ';
    for (let i = 0; i < char.characteristicValue.byteLength; i++) {
      message += value[i] + ' ';
    }
    logger.info(TAG+ message);
  }

  private logDescriptor(des: ble.BLEDescriptor) {
    let message = 'logDescriptor uuid:' + des.descriptorUuid + '\n';
    let value = new Uint8Array(des.descriptorValue);
    message += 'logDescriptor value: ';
    for (let i = 0; i < des.descriptorValue.byteLength; i++) {
      message += value[i] + ' ';
    }
    logger.info(TAG+ message);
  }

  private checkService(services: Array<ble.GattService>): boolean {

    // for (let i = 0; i < services.length; i++) {
    //   logger.info(TAG+ 'checkService : services[i].serviceUuid : ' + services[i].serviceUuid);
    //   // 1.查到云台 myServiceUuid: string = '0000FFE0-0000-1000-8000-00805F9B34FB'
    //   if (services[i].serviceUuid == this.myServiceUuid) {
    //     logger.info(TAG+ 'checkService : 查到云台 ServiceUuid:' + services[i].serviceUuid);
    //     for (let j = 0; j < services[i].characteristics.length; j++) {
    //       logger.info(TAG+ 'checkService : services[i].serviceUuid.characteristicUuid : ' + services[i].characteristics[j].characteristicUuid);
    //       // 2.查到云台 myCharacteristicUuid: string = '0000FFE5-0000-1000-8000-00805F9B34FB'
    //       if (services[i].characteristics[j].characteristicUuid != this.myCharacteristicUuid) {
    //         logger.info(TAG+ 'checkService : 查到云台 characteristicUuid:' + services[i].characteristics[j].characteristicUuid);
    //       }
    //       for (let k = 0; k < services[i].characteristics[j].descriptors.length; k++) {
    //         logger.info(TAG+ 'checkService : services[i].serviceUuid.descriptorUuid : ' + services[i].characteristics[j].descriptors[k].descriptorUuid);
    //         // 3.查到云台 myDescriptorUuid_read: string = '00002902-0000-1000-8000-00805F9B34FB'
    //         if (services[i].characteristics[j].descriptors[k].descriptorUuid == this.myDescriptorUuid_read) {
    //           logger.info(TAG+ 'checkService : 查到云台 descriptorUuid:' + services[i].characteristics[j].descriptors[k].descriptorUuid);
    //           logger.info(TAG+ 'find expected service from server success');
    //           prompt.showToast({
    //             message: 'find expected service from server success!',
    //             duration: 4000
    //           });
    //           return true;
    //         }
    //       }
    //     }
    //   }
    // }

    for (let i = 0; i < services.length; i++) {
      logger.info(TAG+ 'aaaaaaaaaaaaaaaaaaaaaaaaaa checkService : services[i].serviceUuid : ' + services[i].serviceUuid);
      // 1.查到云台 myServiceUuid: string = '15f1e600-a277-43fc-a484-dd39ef8a9100'
      if (services[i].serviceUuid == this.myServiceUuid) {
        logger.info(TAG+ 'checkService : 查到云台 ServiceUuid:' + services[i].serviceUuid);
        logger.info(TAG+ 'find expected service from server success');
        prompt.showToast({
          message: 'find expected service from server success!',
          duration: 4000
        });
        return true;
      }
    }
    logger.error(TAG+ 'no expected service from server');
    return false;
  }

  // 1. 订阅连接状态变化事件
  public onGattClientStateChange() {
    logger.info(TAG+ 'onGattClientStateChange in');
    if (!this.gattClient) {
      logger.error(TAG+ 'no gattClient');
      return;
    }
    try {
      logger.info(TAG+ 'gattClient.on(BLEConnectionStateChange) will start');
      this.gattClient.on('BLEConnectionStateChange', (stateInfo: ble.BLEConnectionChangeState) => {
        let state = '';
        switch (stateInfo.state) {
          case 0:
            state = 'DISCONNECTED';
            break;
          case 1:
            state = 'CONNECTING';
            break;
          case 2:
            state = 'CONNECTED';
            break;
          case 3:
            state = 'DISCONNECTING';
            break;
          default:
            state = 'undefined';
            break;
        }
        logger.info(TAG+ 'onGattClientStateChange: device=' + stateInfo.deviceId + ', state=' + state);
        prompt.showToast({
          message: 'connectState:' + state,
          duration: 4000
        });
        if (stateInfo.deviceId == this.device) {
          this.connectState = stateInfo.state;
        }
      });
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 2. client端主动连接时调用 (客户端(手机)发起连接远端蓝牙低功耗设备（云台）)
  public startConnect(peerDevice: string) { // 对端设备一般通过ble scan获取到
    // if (this.connectState != constant.ProfileConnectionState.STATE_DISCONNECTED) {
    //   logger.error(TAG+ 'startConnect failed');
    //   return;
    // }
    logger.info(TAG+ 'startConnect ' + peerDevice);
    this.device = peerDevice;
    // 2.1 使用device构造gattClient，后续的交互都需要使用该实例
    if (!this.gattClient) {
      this.gattClient = ble.createGattClientDevice(peerDevice);
    }
    try {
      this.onGattClientStateChange(); // 2.2 订阅连接状态
      logger.info(TAG + 'connect start. peerDevice = ' + peerDevice);
      this.gattClient.connect(); // 2.3 发起连接
      logger.info(TAG + 'connect end. peerDevice = ' + peerDevice);
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 3. client端连接成功后，需要进行服务发现
  public discoverServices() {
    if (!this.gattClient) {
      logger.info(TAG+ 'no gattClient');
      return;
    }
    logger.info(TAG+ 'discoverServices start');

    try {
      this.gattClient.setBLEMtuSize(512);
      logger.info(TAG+ 'setBLEMtuSize end');
    } catch (err) {
      logger.error(TAG+ 'setBLEMtuSize errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }

    try {
      this.gattClient.getServices().then((result: Array<ble.GattService>) => {
        logger.info(TAG+ 'getServices success: ' + JSON.stringify(result));
        this.found = this.checkService(result); // 要确保server端的服务内容有业务所需要的服务
      });
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 4. 在确保拿到了server端的服务结果后，读取server端特定服务的特征值时调用
  public readCharacteristicValue() {
    if (!this.gattClient || this.connectState != constant.ProfileConnectionState.STATE_CONNECTED) {
      logger.error(TAG+ 'no gattClient or not connected');
      return;
    }
    if (!this.found) { // 要确保server端有对应的characteristic
      logger.error(TAG+ 'no characteristic from server');
      return;
    }

    let characteristic = this.initCharacteristic();
    logger.info(TAG+ 'readCharacteristicValue');
    try {
      this.gattClient.readCharacteristicValue(characteristic).then((outData: ble.BLECharacteristic) => {
        this.logCharacteristic(outData);
      })
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 5. 在确保拿到了server端的服务结果后，写入server端特定服务的特征值时调用
  public writeCharacteristicValue(characteristic: ble.BLECharacteristic) {
    if (!this.gattClient || this.connectState != constant.ProfileConnectionState.STATE_CONNECTED) {
      logger.error(TAG+ 'no gattClient or not connected');
      return;
    }
    if (!this.found) { // 要确保server端有对应的characteristic
      logger.error(TAG+ 'no characteristic from server');
      return;
    }

    // let characteristic = this.initCharacteristic();
    logger.info(TAG+ 'writeCharacteristicValue');
    try {
      this.gattClient.writeCharacteristicValue(characteristic, ble.GattWriteType.WRITE, (err) => {
        if (err) {
          logger.error(TAG+ 'writeCharacteristicValue callback in : errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
          return;
        }
        logger.info(TAG+ 'writeCharacteristicValue success');
      });
    } catch (err) {
      logger.error(TAG+ 'writeCharacteristicValue: errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 6. 在确保拿到了server端的服务结果后，读取server端特定服务的描述符时调用
  public readDescriptorValue() {
    if (!this.gattClient || this.connectState != constant.ProfileConnectionState.STATE_CONNECTED) {
      logger.error(TAG+ 'no gattClient or not connected');
      return;
    }
    if (!this.found) { // 要确保server端有对应的descriptor
      logger.error(TAG+ 'no descriptor from server');
      return;
    }

    let descBuffer = new ArrayBuffer(0);
    // let descriptor = this.initDescriptor(this.mySecondDescriptorUuid, descBuffer);
    logger.info(TAG+ 'readDescriptorValue');
    try {
      // this.gattClient.readDescriptorValue(descriptor).then((outData: ble.BLEDescriptor) => {
      //   this.logDescriptor(outData);
      // });
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 7. 在确保拿到了server端的服务结果后，写入server端特定服务的描述符时调用
  public writeDescriptorValue() {
    if (!this.gattClient || this.connectState != constant.ProfileConnectionState.STATE_CONNECTED) {
      logger.error(TAG+ 'no gattClient or not connected');
      return;
    }
    if (!this.found) { // 要确保server端有对应的descriptor
      logger.error(TAG+ 'no descriptor from server');
      return;
    }

    let descBuffer = new ArrayBuffer(2);
    let descValue = new Uint8Array(descBuffer);
    descValue[0] = 11;
    descValue[1] = 12;
    let descriptor = this.initDescriptor(this.myDescriptorUuid_read, descBuffer);
    // let descriptor = this.initDescriptor(this.mySecondDescriptorUuid, descBuffer);
    logger.info(TAG+ 'writeDescriptorValue');
    try {
      this.gattClient.writeDescriptorValue(descriptor, (err) => {
        if (err) {
          logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
          return;
        }
        logger.info(TAG+ 'writeDescriptorValue success');
      });
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 8.client端主动断开时调用
  public stopConnect() {
    // if (!this.gattClient || this.connectState != constant.ProfileConnectionState.STATE_CONNECTED) {
    //   logger.error(TAG+ 'no gattClient or not connected');
    //   return;
    // }

    logger.info(TAG+ 'stopConnect ' + this.device);
    try {
      this.gattClient?.disconnect(); // 8.1 断开连接
      this.gattClient?.off('BLEConnectionStateChange', (stateInfo: ble.BLEConnectionChangeState) => {
      });
      // this.gattClient.close() // 8.2 如果不再使用此gattClient，则需要close
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }
}

let gattClientManager = new GattClientManager();
export default gattClientManager as GattClientManager;