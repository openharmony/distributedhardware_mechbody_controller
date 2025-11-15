import ble from '@ohos.bluetooth.ble';
import constant from '@ohos.bluetooth.constant';
import { BusinessError } from '@ohos.base';
import { logger } from '../utils/Logger';
import prompt from '@system.prompt';

const TAG: string = '[GattServerManager]';

// 判断云台是否支持能力开放
// 检查服务和特征是否存在，服务和特征配置值如下
/**
 * UUID of ble service
 */
let SERVICE = "0000FFE0-0000-1000-8000-00805F9B34FB"
/**
 * uuis of write characteristic
 */
let CHAR_WRITE = "0000FFE5-0000-1000-8000-00805F9B34FB"
/**
 * uuid of notify characteristic
 */
let CHAR_NOTIFY = "0000FFE4-0000-1000-8000-00805F9B34FB"
/**
 * uuid of read descriptor
 */
let DESCRIPTOR_READ = "00002902-0000-1000-8000-00805f9b34fb"


export class GattServerManager {
  gattServer: ble.GattServer|undefined = undefined;
  connectState: ble.ProfileConnectionState = constant.ProfileConnectionState.STATE_DISCONNECTED;
  myServiceUuid: string = '15F1E600-A277-43FC-A484-DD39EF8A9100';
  myCharacteristicUuid_write: string = '15F1E602-A277-43FC-A484-DD39EF8A9100';  //write : 15F1E602-A277-43FC-A484-DD39EF8A9100 15f1e602-a277-43fc-a484-dd39ef8a9100
  myCharacteristicUuid_notify: string = '15F1E603-A277-43FC-A484-DD39EF8A9100';
  // myServiceUuid: string = '0000FFE0-0000-1000-8000-00805F9B34FB';
  // myCharacteristicUuid_write: string = '0000FFE5-0000-1000-8000-00805F9B34FB';
  // myCharacteristicUuid_notify: string = '0000FFE4-0000-1000-8000-00805F9B34FB';
  myDescriptorUuid_read: string = '00002902-0000-1000-8000-00805F9B34FB';
  // myFirstDescriptorUuid: string = '00002902-0000-1000-8000-00805F9B34FB'; // 2902一般用于notification或者indication
  // mySecondDescriptorUuid: string = '00002903-0000-1000-8000-00805F9B34FB';
  clientDeviceId:string|undefined = undefined;

  // 构造BLEDescripto
  private initDescriptor(des: string, value: ArrayBuffer): ble.BLEDescriptor {
    let descriptor: ble.BLEDescriptor = {
      serviceUuid: this.myServiceUuid,
      characteristicUuid: this.myCharacteristicUuid_notify,
      descriptorUuid: des,
      descriptorValue: value
    };
    return descriptor;
  }

  // 构造BLECharacteristic
  private initCharacteristic(): ble.BLECharacteristic {
    let descriptors: Array<ble.BLEDescriptor> = [];
    let descBuffer = new ArrayBuffer(2);
    // let descValue = new Uint8Array(descBuffer);
    // descValue[0] = 31;
    // descValue[1] = 32;
    // descriptors[0] = this.initDescriptor(this.myDescriptorUuid_read, new ArrayBuffer(2));
    // descriptors[0] = this.initDescriptor(this.myFirstDescriptorUuid, new ArrayBuffer(2));
    // descriptors[1] = this.initDescriptor(this.mySecondDescriptorUuid, descBuffer);
    let charBuffer = new ArrayBuffer(4);
    let charValue = new Uint8Array(charBuffer);
    charValue[0] = 21;
    charValue[1] = 22;
    let characteristic: ble.BLECharacteristic = {
      serviceUuid: this.myServiceUuid,
      characteristicUuid: this.myCharacteristicUuid_write,
      characteristicValue: charBuffer,
      descriptors: descriptors
    };
    return characteristic;
  }

  // 1. 订阅连接状态变化事件
  public onGattServerStateChange() {
    if (!this.gattServer) {
      logger.error(TAG+ 'no gattServer');
      return;
    }
    try {
      this.gattServer.on('connectionStateChange', (stateInfo: ble.BLEConnectionChangeState) => {
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
        logger.info(TAG+ 'onGattServerStateChange: device=' + stateInfo.deviceId + ', state=' + state);
        prompt.showToast({
          message: 'connectState:' + state,
          duration: 4000
        });
      });
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 2. server端注册服务时调用
  public registerServer() {
    let characteristics: Array<ble.BLECharacteristic> = [];
    let characteristic_write = this.initCharacteristic(); // 第一个characteristic: CHAR_WRITE

    let charBuffer = new ArrayBuffer(4);
    let charValue = new Uint8Array(charBuffer);
    charValue[0] = 21;
    charValue[1] = 22;

    let descriptors: Array<ble.BLEDescriptor> = [];
    let descBuffer = new ArrayBuffer(2);
    let descValue = new Uint8Array(descBuffer);
    descValue[0] = 31;
    descValue[1] = 32;
    descriptors[0] = this.initDescriptor(this.myDescriptorUuid_read, new ArrayBuffer(2));

    let characteristic_notify: ble.BLECharacteristic = {
      serviceUuid: this.myServiceUuid,
      characteristicUuid: this.myCharacteristicUuid_notify,
      characteristicValue: charBuffer,
      descriptors: descriptors
    };  // 第二个characteristic: CHAR_NOTIFY

    characteristics.push(characteristic_write, characteristic_notify);

    let gattService: ble.GattService = {
      serviceUuid: this.myServiceUuid,
      isPrimary: true,
      characteristics: characteristics
    };

    logger.info(TAG+ 'registerServer ' + JSON.stringify(characteristics));
    try {
      this.gattServer = ble.createGattServer(); // 2.1 构造gattServer，后续的交互都需要使用该实例
      this.onGattServerStateChange(); // 2.2 订阅连接状态
      this.gattServer.addService(gattService);
      prompt.showToast({
        message: 'registerServer & add service success!',
        duration: 4000
      });
      logger.info(TAG + 'gattServer addService end');
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 3. 订阅来自gattClient的读取特征值请求时调用
  public onCharacteristicRead() {
    if (!this.gattServer) {
      logger.error(TAG+ 'no gattServer');
      return;
    }

    logger.info(TAG+ 'onCharacteristicRead');
    try {
      this.gattServer.on('characteristicRead', (charReq: ble.CharacteristicReadRequest) => {
        let deviceId: string = charReq.deviceId;
        let transId: number = charReq.transId;
        let offset: number = charReq.offset;
        logger.info(TAG+ 'receive characteristicRead');
        let rspBuffer = new ArrayBuffer(2);
        let rspValue = new Uint8Array(rspBuffer);
        rspValue[0] = 21;
        rspValue[1] = 22;

        prompt.showToast({
          message: `deviceId:  ${deviceId}`,
          duration: 4000
        });
        logger.info(TAG+ `receive characteristicRead：deviceId:  ${deviceId}`);
        this.clientDeviceId = deviceId;

        let serverResponse: ble.ServerResponse = {
          deviceId: deviceId,
          transId: transId,
          status: 0, // 0表示成功
          offset: offset,
          value: rspBuffer
        };

        try {
          this.gattServer?.sendResponse(serverResponse);
        } catch (err) {
          logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
        }
      });

      prompt.showToast({
        message: 'onCharacteristicRead: on success!',
        duration: 4000
      });
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 4. 订阅来自gattClient的写入特征值请求时调用
  public onCharacteristicWrite() {
    if (!this.gattServer) {
      logger.error(TAG+ 'no gattServer');
      return;
    }

    logger.info(TAG+ 'onCharacteristicWrite');
    try {
      this.gattServer.on('characteristicWrite', (charReq: ble.CharacteristicWriteRequest) => {
        let deviceId: string = charReq.deviceId;
        let transId: number = charReq.transId;
        let offset: number = charReq.offset;
        let rspBuffer = charReq.value;
        logger.info(TAG+ 'receive characteristicWrite: needRsp=' + charReq.needRsp);
        if (!charReq.needRsp) {
          return;
        }

        let charValue = new Uint8Array(charReq.value);
        AppStorage.setOrCreate('charReqValue', charValue);
        logger.info(TAG+ 'receive characteristicWrite: charValue[0]=' + charValue[0]);
        logger.info(TAG+ 'receive characteristicWrite: charValue[1]=' + charValue[1]);
        let bufferValuesStr = Array.from(charValue).map(b => b.toString(16).padStart(2, '0')).join(' ');
        logger.info(TAG + 'onCharReqValueChange received. bufferValuesStr:' + bufferValuesStr);
        this.clientDeviceId = deviceId;
        prompt.showToast({
          message: `bufferValuesHexStr:  ${bufferValuesStr}`,
          duration: 4000
        });

        // let rspBuffer = new ArrayBuffer(0);
        let serverResponse: ble.ServerResponse = {
          deviceId: deviceId,
          transId: transId,
          status: 0, // 0表示成功
          offset: offset,
          value: rspBuffer
        };

        try {
          this.gattServer?.sendResponse(serverResponse);
          logger.info(TAG+ `onCharacteristicWrite: receive characteristicWrite success. deviceId:  ${deviceId}`);
        } catch (err) {
          logger.error(TAG+ 'gattServer.sendResponse. errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
        }

      });

      prompt.showToast({
        message: 'onCharacteristicWrite: on success!',
        duration: 4000
      });
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 5. 订阅来自gattClient的读取描述符请求时调用
  public onDescriptorRead() {
    if (!this.gattServer) {
      logger.error(TAG+ 'no gattServer');
      return;
    }

    logger.info(TAG+ 'onDescriptorRead');
    try {
      this.gattServer.on('descriptorRead', (desReq: ble.DescriptorReadRequest) => {
        let deviceId: string = desReq.deviceId;
        let transId: number = desReq.transId;
        let offset: number = desReq.offset;
        logger.info(TAG+ 'receive descriptorRead');
        let rspBuffer = new ArrayBuffer(2);
        let rspValue = new Uint8Array(rspBuffer);
        rspValue[0] = 31;
        rspValue[1] = 32;
        let serverResponse: ble.ServerResponse = {
          deviceId: deviceId,
          transId: transId,
          status: 0, // 0表示成功
          offset: offset,
          value: rspBuffer
        };

        try {
          this.gattServer?.sendResponse(serverResponse);
        } catch (err) {
          logger.error(TAG+ 'gattServer.sendResponse failed. errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
        }
      });
    } catch (err) {
      logger.error(TAG+ 'onDescriptorRead failed. errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 6. 订阅来自gattClient的写入描述符请求时调用
  public onDescriptorWrite() {
    if (!this.gattServer) {
      logger.error(TAG+ 'no gattServer');
      return;
    }

    logger.info(TAG+ 'onDescriptorWrite');
    try {
      this.gattServer.on('descriptorWrite', (desReq: ble.DescriptorWriteRequest) => {
        let deviceId: string = desReq.deviceId;
        let transId: number = desReq.transId;
        let offset: number = desReq.offset;
        logger.info(TAG+ 'receive descriptorWrite: needRsp=' + desReq.needRsp);
        if (!desReq.needRsp) {
          return;
        }
        let rspBuffer = new ArrayBuffer(0);

        let serverResponse: ble.ServerResponse = {
          deviceId: deviceId,
          transId: transId,
          status: 0, // 0表示成功
          offset: offset,
          value: rspBuffer
        };

        try {
          this.gattServer?.sendResponse(serverResponse);
        } catch (err) {
          logger.error(TAG+ 'gattServer.sendResponse failed. errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
        }
      });
    } catch (err) {
      logger.error(TAG+ 'onDescriptorWrite failed. errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 7. server端删除服务，不再使用时调用
  public unRegisterServer() {
    if (!this.gattServer) {
      logger.error(TAG+ 'no gattServer');
      return;
    }

    logger.info(TAG+ 'unRegisterServer ' + this.myServiceUuid);
    try {
      this.gattServer.removeService(this.myServiceUuid); // 7.1 删除服务
      this.gattServer.off('connectionStateChange', (stateInfo: ble.BLEConnectionChangeState) => { // 7.2 取消订阅连接状态
      });
      this.gattServer.close() // 7.3 如果不再使用此gattServer，则需要close
      prompt.showToast({
        message: 'unRegisterServer & remove service success!',
        duration: 4000
      });
    } catch (err) {
      logger.error(TAG+ 'unRegisterServer failed. errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  notifyCharacteristicChanged(notifyData:Uint8Array){
    if (!this.gattServer) {
      logger.error(TAG+ 'no gattServer');
      return;
    }
    // 1.根据notifyData的长度创建一个合适的ArrayBuffer
    let arrayBufferC = new ArrayBuffer(notifyData.length);
    let notifyCharValue = new Uint8Array(arrayBufferC);

    // 2.遍历notifyData并将每个字符转换为ASCII值
    for (let i = 0; i < notifyData.length; i++) {
      notifyCharValue[i] = notifyData[i];
    }
    logger.info(TAG + ' Notify Data: ' + notifyCharValue);
    logger.info(TAG + ' notifyData.length: ' + notifyData.length);
    logger.info(TAG+ new Uint8Array(arrayBufferC));

    let bufferValuesStr = Array.from(notifyCharValue).map(b => b.toString(16).padStart(2, '0')).join(' ');
    logger.info(TAG + 'notifyCharacteristicChanged. notifyDataStr:' + bufferValuesStr);

    // notify：实际应该根据厂商用CHAR_NOTIFY uuid: "0000FFE4-0000-1000-8000-00805F9B34FB"
    let notifyCharacter: ble.NotifyCharacteristic = {
      serviceUuid: this.myServiceUuid,
      characteristicUuid: this.myCharacteristicUuid_notify,
      characteristicValue: arrayBufferC,
      confirm: true
    };
    if (!this.clientDeviceId) {
      logger.error(TAG+ 'no clientDeviceId');
      return;
    }
    logger.info(TAG+ `notifyCharacteristicChanged：this.clientDeviceId: ${this.clientDeviceId}`);

    try {
      this.gattServer.notifyCharacteristicChanged(this.clientDeviceId, notifyCharacter, (err: BusinessError) => {
        if (err) {
          logger.info('notifyCharacteristicChanged callback failed');
        } else {
          logger.info('notifyCharacteristicChanged callback successful');
        }
      });
    } catch (err) {
      logger.error('errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }

    // try {
    //   this.gattServer.notifyCharacteristicChanged(this.clientDeviceId, notifyCharacter).then(() => {
    //     logger.info('notifyCharacteristicChanged promise successful');
    //   });
    // } catch (err) {
    //   logger.error('errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    // }
  }

}

let gattServerManager = new GattServerManager();
export default gattServerManager as GattServerManager;