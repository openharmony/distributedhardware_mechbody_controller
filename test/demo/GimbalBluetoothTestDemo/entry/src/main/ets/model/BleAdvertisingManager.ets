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
import { BusinessError } from '@ohos.base';
import { logger } from '../utils/Logger';
import prompt from '@system.prompt';

const TAG: string = '[BleAdvertisingManager]';

export class BleAdvertisingManager {
  private advHandle: number = 0xFF; // default invalid value

  // 1 订阅广播状态
  public onAdvertisingStateChange() {
    try {
      ble.on('advertisingStateChange', (data: ble.AdvertisingStateChangeInfo) => {
        logger.info(TAG+ 'bluetooth advertising state = ' + JSON.stringify(data));
        AppStorage.setOrCreate('advertiserState', data.state);
      });
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 2 首次启动广播
  public async startAdvertising() {
    // 2.1 设置广播发送的参数
    let setting: ble.AdvertiseSetting = {
      interval: 1600,
      txPower: 0,
      connectable: true
    };
    // 2.2 构造广播数据
    let manufactureValueBuffer = new Uint8Array(1);
    // manufactureValueBuffer[0] = 1;
    // manufactureValueBuffer[1] = 2;
    // manufactureValueBuffer[2] = 3;
    // manufactureValueBuffer[3] = 4;
    let serviceValueBuffer = new Uint8Array(4);
    serviceValueBuffer[0] = 5;
    serviceValueBuffer[1] = 6;
    serviceValueBuffer[2] = 7;
    serviceValueBuffer[3] = 8;
    let manufactureDataUnit: ble.ManufactureData = {
      manufactureId: 4567,
      manufactureValue: manufactureValueBuffer.buffer
    };
    let serviceDataUnit: ble.ServiceData = {
      serviceUuid: "00001888-0000-1000-8000-00805f9b34fb",
      serviceValue: serviceValueBuffer.buffer
    };
    let advData: ble.AdvertiseData = {
      serviceUuids: ["15F1E600-A277-43FC-A484-DD39EF8A9100"],
      // serviceUuids: ["0000FFE0-0000-1000-8000-00805F9B34FB"],
      // serviceUuids: ["0000FD58-0000-1000-8000-00805f9b34fb", "7659feb1-eb64-5875-ab7c-63902b1e6418"],
      manufactureData: [manufactureDataUnit],
      // manufactureData: [],
      // serviceData: [serviceDataUnit],
      serviceData: [],
      includeDeviceName: true // 表示是否携带设备名，可选参数。注意带上设备名时广播包长度不能超出31个字节。
    };
    let advResponse: ble.AdvertiseData = {
      // serviceUuids: ["00001888-0000-1000-8000-00805f9b34fb"],
      serviceUuids: [],
      // manufactureData: [manufactureDataUnit],
      manufactureData: [],
      // serviceData: [serviceDataUnit]
      serviceData:[],
      // includeDeviceName: true
    };
    // 2.3 构造广播启动完整参数AdvertisingParams
    let advertisingParams: ble.AdvertisingParams = {
      advertisingSettings: setting,
      advertisingData: advData,
      advertisingResponse: advResponse,
      duration: 0 // 可选参数，若大于0，则广播发送一段时间后，则会临时停止，可重新启动发送
    }

    // 2.4 首次启动广播，且获取所启动广播的标识ID
    logger.info(TAG+ 'startAdvertising');
    try {
      this.onAdvertisingStateChange();
      this.advHandle = await ble.startAdvertising(advertisingParams);  // ?
      prompt.showToast({
        message: 'start Advertising success!',
        duration: 4000
      });
    } catch (err) {
      logger.error(TAG+ 'startAdvertising errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 4 临时停止广播，该广播资源仍然存在
  public async disableAdvertising() {
    // 4.1 构造临时停止广播参数
    let advertisingDisableParams: ble.AdvertisingDisableParams = {
      advertisingId: this.advHandle // 使用首次启动广播时获取到的广播标识ID
    }
    // 4.2 临时停止
    logger.info(TAG+ 'disableAdvertising');
    try {
      await ble.disableAdvertising(advertisingDisableParams);
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 5 再次启动广播
  public async enableAdvertising(enableDuration: number) {
    // 5.1 构造临时启动广播参数
    let advertisingEnableParams: ble.AdvertisingEnableParams = {
      advertisingId: this.advHandle, // 使用首次启动广播时获取到的广播标识ID
      duration: enableDuration
    }
    // 5.2 再次启动
    logger.info(TAG+ 'enableAdvertising');
    try {
      await ble.enableAdvertising(advertisingEnableParams);
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 6 完全关闭广播，释放广播资源
  public async stopAdvertising() {
    logger.info(TAG+ 'stopAdvertising');
    try {
      await ble.stopAdvertising(this.advHandle);
      ble.off('advertisingStateChange', (data: ble.AdvertisingStateChangeInfo) => {
        logger.info(TAG+ 'bluetooth advertising state = ' + JSON.stringify(data));
      });
      prompt.showToast({
        message: 'stop Advertising success!',
        duration: 4000
      });
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }
}

let bleAdvertisingManager = new BleAdvertisingManager();
export default bleAdvertisingManager as BleAdvertisingManager;