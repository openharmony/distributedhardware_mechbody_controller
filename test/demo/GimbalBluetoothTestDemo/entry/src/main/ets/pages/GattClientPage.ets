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
import { BusinessError } from '@kit.BasicServicesKit';
import { ble, connection } from '@kit.ConnectivityKit';
import { logger } from '../utils/Logger';
// import { GimbalController } from '../controller/GimbalController';
import bleScanManager from '../model/BleScanManager';
import gattClientManager from '../model/GattClientManager';
import prompt from '@system.prompt';
import { util } from '@kit.ArkTS';

const TAG = '[GattClientPage_Page] ';
// 创建descriptors
let descriptors: Array<ble.BLEDescriptor> = [];
let arrayBuffer = new ArrayBuffer(8);
// let descV = new Uint8Array(arrayBuffer);
// descV[0] = 11;
let descriptor: ble.BLEDescriptor = {
  serviceUuid: '0000FFE0-0000-1000-8000-00805F9B34FB',
  characteristicUuid: '0000FFE4-0000-1000-8000-00805F9B34FB',
  descriptorUuid: '00002902-0000-1000-8000-00805F9B34FB',
  descriptorValue: arrayBuffer
};
descriptors[0] = descriptor;

let intervalId = 0;

// 创建characteristics
let characteristics: Array<ble.BLECharacteristic> = [];
let arrayBufferC = new ArrayBuffer(8);
let cccV = new Uint8Array(arrayBufferC);
cccV[0] = 1;
let characteristic: ble.BLECharacteristic = {
  serviceUuid: '0000FFE0-0000-1000-8000-00805F9B34FB',
  characteristicUuid: '0000FFE4-0000-1000-8000-00805F9B34FB',
  characteristicValue: arrayBufferC,
  descriptors: descriptors
};
characteristics[0] = characteristic;

// 创建gattService
let gattService: ble.GattService = {
  serviceUuid: '0000FFE0-0000-1000-8000-00805F9B34FB',
  isPrimary: true,
  characteristics: characteristics,
  includeServices: []
};

// 云台位置数据
interface GimbalLocationData {
  yawAngle: number,
  rollAngle: number,
  pitchAngle: number,
  ctrlByte: number,
  timeForAction: number,
  returnCode?: number
}

// 定义 PathControlPoint 接口
// interface PathControlPoint {
//   readonly axis: number;
//   readonly duration: number;
//   readonly rad: number;
//   readonly radSpeed: number;
// }


@Entry
@Component
struct GattClientPage {
  @State message: string = '方向控制 : ';
  @State deviceId: string = '';
  @State deviceIdArr: Array<string> = [''];
  @State deviceName: string = '';
  @State gimbalLocationData: GimbalLocationData | null = null;
  @State characteristic: ble.BLECharacteristic | null = null;
  @StorageLink('scanResultDeviceId') scanResultDeviceId: string = '暂未发现设备';
  @StorageLink('scanResultDeviceName') scanResultDeviceName: string = '暂未发现设备';
  @State gimbalLocationDataBuffer: ArrayBuffer | null = null;
  @State data: Array<string> =
    ['aa 3a 04 02 00 00 00 00 ba 04 80 bf 0e 15 b0 01 4f 0a 9b 30 06 00 02 01 05 00 05 00 00 00 3f 00 00 00 3f 00 00 00 3f 00 00 00 3f 28 00 40 00 00 96 00 00 00 00 00 49 95 ab c2'];
  arr: Uint8Array =
    new Uint8Array([0xaa, 0x3a, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0xba, 0x04, 0x80, 0xbf, 0x0e, 0x15, 0xb0, 0x01,
      0x4f, 0x0a, 0x9b, 0x30, 0x06, 0x00, 0x02, 0x01, 0x05, 0x00, 0x05, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x3f,
      0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x3f, 0x28, 0x00, 0x40, 0x00, 0x00, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x49, 0x95, 0xab, 0xc2]);
  arr1: Uint8Array =
    new Uint8Array([0xAA, 0x3A, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x5B, 0x03, 0x89, 0x2D, 0x0E, 0x15, 0x00, 0xD1,
      0x7C, 0x17, 0x9B, 0x30, 0x06, 0x00, 0x02, 0x01, 0x02, 0x00, 0x05, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F,
      0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x28, 0x00, 0x40, 0x00, 0x00, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x9B, 0xCF, 0x40, 0x6D]);
  arr2: Uint8Array =
    new Uint8Array([0xaa, 0x38, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x3b, 0x00, 0xf8, 0x4c, 0x0e, 0x15,
      0x68, 0xee, 0x73, 0x39, 0x3d, 0x31, 0x06, 0x00,
      0x02, 0x01, 0x00, 0x00, 0x05,
      0x9a, 0x99, 0x19, 0x3f,
      0x9a, 0x99, 0x19, 0x3f,
      0x00,0x00,0x00,0x3f,0x00,0x00,0x00,0x3f,
      0x28,0x00,0x40,0x00,0x00,0x96,0x00,0x00,0x00,
      0xc2,0xcd,0x7e,0x25,
    ]);
  arr3: Uint8Array =
    new Uint8Array([
      0xaa, 0x40, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00,
      0x02, 0x00, 0xe8, 0x1e, 0x0e, 0x23, 0x04, 0x2c,
      0x3b, 0x00, 0x00, 0x03, 0x01, 0x9a, 0x99, 0x99,
      0x3e, 0xc3, 0xf5, 0x48, 0xc0, 0x00, 0x00, 0x00,
      0x00, 0x01, 0x7d, 0x77, 0x77, 0x3f, 0xc3, 0xf5,
      0x68, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf5,
      0x10, 0xd1, 0x3f, 0xc3, 0xf5, 0x48, 0xc0, 0x00,
      0x00, 0x00, 0x00, 0x00, 0xe1, 0x8a, 0xca, 0x63,
    ]);

  arr4: Uint8Array =
    new Uint8Array([0x22, 0x00, 0x04]);

  // private GimbalController = new GimbalController({
  //   yawAngle: 0,
  //   rollAngle: 0,
  //   pitchAngle: 0,
  //   ctrlByte: 0,
  //   timeForAction: 0,
  //   returnCode: 0,
  // });

  // requestPermission() { //开启授权
  //   let permissions: Array<Permissions> = ["ohos.permission.ACCESS_BLUETOOTH"];
  //   let context: Context = getContext(this)
  //   let atManager: abilityAccessCtrl.AtManager =
  //     abilityAccessCtrl.createAtManager(); // requestPermissionsFromUser会判断权限的授权状态来决定是否唤起弹窗
  //   atManager.requestPermissionsFromUser(context, permissions).then((data) => {
  //     let grantStatus: Array<number> = data.authResults;
  //     let length: number = grantStatus.length;
  //     for (let i = 0; i < length; i++) {
  //       if (grantStatus[i] === 0) { // 用户授权，可以继续访问目标操作
  //       } else { // 用户拒绝授权，提示用户必须授权才能访问当前页面的功能，并引导用户到系统设置中打开相应的权限
  //         return;
  //       }
  //     } // 授权成功
  //   })
  // }

  aboutToAppear(): void {
    logger.info(TAG + 'aboutToAppear in');
    this.getPairedDevices();
    this.getDeviceName(this.deviceId);
  }

  // 服务端添加服务
  addService() {
    logger.info(TAG + 'addService in');
    try {
      logger.info(TAG + 'gattServer addService start');
      let gattServer: ble.GattServer = ble.createGattServer();
      gattServer.addService(gattService);
      logger.info(TAG + 'gattServer addService end');
    } catch (err) {
      logger.error(TAG + 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  onReceiveEvent = (data: Array<string>): void => { // data为蓝牙设备地址集合。
    logger.info(TAG + 'bluetooth device find = ' + JSON.stringify(data));
    logger.info(TAG + 'data.toString() = ' + data.toString());
    try {
      this.deviceIdArr = data;
      this.deviceId = data.toString();
      this.getDeviceName(this.deviceId);
    } catch (err) {
      logger.error(TAG + 'data assign errCode: ' + (err as BusinessError).code + ', errMessage: ' +
      (err as BusinessError).message);
    }
    logger.info(TAG + 'this.deviceIdArr = ' + JSON.stringify(this.deviceIdArr));
  }

  //
  getRemoteDeviceId() {
    logger.info(TAG + 'getRemoteDeviceId in');
    try {
      connection.on('bluetoothDeviceFind', this.onReceiveEvent);
      // connection.startBluetoothDiscovery();
    } catch (err) {
      logger.error(TAG + 'connection.on(bluetoothDeviceFind) errCode: ' + (err as BusinessError).code +
        ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 客户端(手机)发起连接远端蓝牙低功耗设备（云台）  通路传数据
  // connect() {
  //   logger.info(TAG + 'connect in. deviceId = ' + this.deviceId);
  //   try {
  //     let device: ble.GattClientDevice = ble.createGattClientDevice(this.deviceId);
  //     device.connect();
  //   } catch (err) {
  //     logger.error(TAG + 'connect errCode: ' + (err as BusinessError).code + ', errMessage: ' +
  //     (err as BusinessError).message);
  //   }
  // }

  getDeviceName(deviceId: string): string {
    logger.info(TAG + 'getDeviceName in. deviceId = ' + deviceId);
    try {
      let remoteDeviceName: string = connection.getRemoteDeviceName(deviceId);
      this.deviceName = remoteDeviceName;
      logger.info(TAG + 'getDeviceName in. deviceName = ' + this.deviceName);
    } catch (err) {
      logger.error('get paired DeviceName errCode: ' + (err as BusinessError).code + ', errMessage: ' +
      (err as BusinessError).message);
    }
    return this.deviceName;
  }

  // 获取已配对设备信息
  getPairedDevices() {
    logger.info(TAG + 'getPairedDevices in.');
    try {
      let devices: Array<string> = connection.getPairedDevices();
      logger.info(TAG + 'getPairedDevices in. devices:' + JSON.stringify(devices));
      this.deviceIdArr = devices;
      this.deviceId = devices[0];
      this.deviceName = this.getDeviceName(devices[0]);
    } catch (err) {
      logger.error('errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  writeCharacteristicValueCallBack = (code: BusinessError): void => {
    if (code != null) {
      return;
    }
    logger.info(TAG + 'bluetooth writeCharacteristicValue success');
  }

  // 客户端(手机)发送消息到远端蓝牙低功耗设备（云台）
  // sendMessage() {
  //   logger.info(TAG + 'sendMessage in. this.deviceId = ' + this.deviceId);
  //   try {
  //     let device: ble.GattClientDevice = ble.createGattClientDevice(this.deviceId);
  //     device.writeCharacteristicValue(this.characteristic, ble.GattWriteType.WRITE,
  //       this.writeCharacteristicValueCallBack);
  //   } catch (err) {
  //     logger.error(TAG + 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
  //   }
  // }

  // strToArrayBuffer(str: string): ArrayBuffer {
  //   // let buf = new ArrayBuffer(str.length * 2);
  //   let bufView = new Uint8Array(str.length);
  //   for (let i = 0, strLen = str.length; i < strLen; i++) {
  //     bufView[i] = str.charCodeAt(i);
  //   }
  //   return bufView.buffer;
  // }

  // string2ArrayBuffer(str:string) {
  //   let array = new Uint8Array(str.length);
  //   for (let i = 0; i < str.length; i++) {
  //     array[i] = str.charCodeAt(i);
  //   }
  //   return array.buffer;
  // }

  // gimbalLocationDataToRealBuffer(objData: GimbalLocationData) {
  //   logger.info(`gimbalLocationDataToRealBuffer: convert to Uint8Array`);
  //   let buf1 = this.string2ArrayBuffer(JSON.stringify(objData));
  //   logger.info(`convert to Uint8Array: ${JSON.stringify(buf1)}, buf1 len: ${buf1.byteLength}`);
  //   return buf1;
  // }

  bufferToStr(buffer: ArrayBuffer) {
    logger.info(TAG + 'bufferToStr in');

    // 假设已经有了一个 ArrayBuffer
    // const arrayBuffer: ArrayBuffer = new ArrayBuffer(4);
    // new Uint8Array(buffer).set([1, 2, 3, 4]); // 设置一些数据

    // 将 ArrayBuffer 转换为 Uint8Array
    let uint8Array = new Uint8Array(buffer);

    let textDecoderOptions: util.TextDecoderOptions = {
      fatal: false,
      ignoreBOM: true
    }
    let decodeToStringOptions: util.DecodeToStringOptions = {
      stream: false
    }
    let textDecoder = util.TextDecoder.create('utf-8', textDecoderOptions);
    // let uint8 = new Uint8Array([0xEF, 0xBB, 0xBF, 0x61, 0x62, 0x63]);
    // let retStr = textDecoder.decodeToString(uint8, decodeToStringOptions);
    // let uint8 = new Uint8Array([0xEF, 0xBB, 0xBF, 0x61, 0x62, 0x63]);
    let retStr = textDecoder.decodeToString(uint8Array, decodeToStringOptions);
    console.info("retStr = " + retStr);
    return retStr;
  }

  build() {
    Column() {
      Text('BLE客户端')
        .fontSize('20fp')
        .fontWeight(FontWeight.Bold)
        .margin({ bottom: 30 })

      // title：设备连接
      Flex({
        direction: FlexDirection.Row,
        justifyContent: FlexAlign.Start,
        alignItems: ItemAlign.Center
      }) {
        Text('设备连接 :')
          .fontSize('20fp')
          .fontWeight(FontWeight.Bold)
          .margin({ bottom: 25 })
      }

      Row() {
        Button('开始扫描').onClick(() => {
          bleScanManager.startScan();
          prompt.showToast({
            message: 'scanResult:' + this.scanResultDeviceId,
            duration: 4000
          });
        })
          .margin({ right: 5 })
        Button('停止扫描').onClick(() => {
          bleScanManager.stopScan();
        })
      }.justifyContent(FlexAlign.SpaceBetween)

      Row() {
        TextInput({ placeholder: this.scanResultDeviceId })
          .onChange((value: string) => {
            this.scanResultDeviceId = value;
          })
          .height('40vp')
          .width(180)
          .margin({ right: 8 })
        Button('开始连接').onClick(() => {
          logger.info(TAG + 'click startConnect btn. this.scanResult: ' + this.scanResultDeviceId)
          gattClientManager.startConnect(this.scanResultDeviceId);
        })
      }
      .margin({ top: 8 })

      // 可复制deviceId
      Text('服务端deviceId：' + this.scanResultDeviceId)
        .fontSize('15fp')
        .copyOption(CopyOptions.InApp)
        .fontWeight(FontWeight.Normal)
        .alignRules({
          center: { anchor: '__container__', align: VerticalAlign.Center },
          middle: { anchor: '__container__', align: HorizontalAlign.Center }
        })
        .margin({ top: 20, bottom: 10 })
        .borderRadius($r('sys.float.ohos_id_corner_radius_menu'))


      Text('服务端deviceName：' + this.scanResultDeviceName)
        .fontSize('15fp')
        .fontWeight(FontWeight.Normal)
        .alignRules({
          center: { anchor: '__container__', align: VerticalAlign.Center },
          middle: { anchor: '__container__', align: HorizontalAlign.Center }
        })
        .margin({ top: 10, bottom: 10 })
        .borderRadius($r('sys.float.ohos_id_corner_radius_menu'))

      Row() {
        Button('服务发现').onClick(() => {
          gattClientManager.discoverServices();
        })
          .margin({ right: 5 })
        Button('断开连接').onClick(() => {
          gattClientManager.stopConnect();
        })
      }
      .margin({ bottom: 10 })

      // title：方位控制
      Flex({
        direction: FlexDirection.Row,
        justifyContent: FlexAlign.Start,
        alignItems: ItemAlign.Center
      }) {
        Text(this.message)
          .fontSize('20fp')
          .fontWeight(FontWeight.Bold)
          .margin({ top: 20, bottom: 25 })
      }


      // 只发送固定蓝牙空口包数据
      // Button("发送空口数据(固定值)")
      //   .type(ButtonType.ROUNDED_RECTANGLE)
      //   .onClick(() => {
      //     // this.characteristic = this.setCharacteristic(this.arr2);
      //     this.characteristic = this.setCharacteristic(this.arr2);
      //
      //     // 解析字符串并转换为数字数组,将数字转换为十六进制字符串打印
      //     const bufferValues = Array.from(this.arr2).map(b => b.toString(16).padStart(2, '0')).join(' ');
      //
      //     logger.info(`${TAG} 发送空口数据(固定值): bufferValues toHexStr: ${bufferValues}`);
      //     logger.info(TAG + 'send center data: this.arr2 = ' + JSON.stringify(this.arr2));
      //     // this.characteristic = this.setCharacteristic();
      //     logger.info(TAG + 'send center data: this.characteristic = ' + JSON.stringify(this.characteristic));
      //     intervalId = setInterval(()=>{
      //       gattClientManager.writeCharacteristicValue(this.characteristic as ble.BLECharacteristic);
      //     }, 30)
      //   })
      //   .margin({ top: 10, bottom: 10 })

      // this.locationControl();

      // 云台智能跟随
      // Row() {
      //   // 发送跟踪控制命令
      //   Button("开始跟随")
      //     .type(ButtonType.ROUNDED_RECTANGLE)
      //     .onClick(() => {
      //       // 1.获取跟踪控制命令数据
      //       let buf = this.GimbalController.getTKControlBuffer();
      //       // 2.set到特征值
      //       this.characteristic = this.setCharacteristic(buf);
      //       logger.info(TAG + 'onClick: send data: this.characteristic = ' + JSON.stringify(this.characteristic));
      //       // 4. 发送数据
      //       setInterval(()=>{
      //         if (this.characteristic?.serviceUuid &&
      //           this.characteristic?.characteristicUuid ) {
      //           // 执行写操作
      //           gattClientManager.writeCharacteristicValue(this.characteristic as ble.BLECharacteristic);
      //         }
      //       }, 30)
      //     })
      //     .margin({ top: 10, bottom: 10, right: 5})
      //
      //   // 停止跟随
      //   Button("停止跟随")
      //     .type(ButtonType.ROUNDED_RECTANGLE)
      //     .onClick(() => {
      //       clearInterval(intervalId);
      //     })
      //     .margin({ top: 10, bottom: 10 })
      // }
      // .margin({ bottom: 10 })


      //轨迹控制云台转动
      Row() {
        // 开始执行轨迹：点头
        // Button("开始执行轨迹")
        //   .type(ButtonType.ROUNDED_RECTANGLE)
        //   .onClick(() => {
        //     // 1.获取轨迹控制命令数据
        //     let buf = this.GimbalController.getGimbalPathMoveBuffer();
        //     // 2.set到特征值
        //     this.characteristic = this.setCharacteristic(buf);
        //     logger.info(TAG + 'onClick: send data: this.characteristic = ' + JSON.stringify(this.characteristic));
        //     // 4. 发送数据
        //     gattClientManager.writeCharacteristicValue(this.characteristic as ble.BLECharacteristic);
        //   })
        //   .margin({ top: 10, bottom: 10, right: 5 })

        // 停止轨迹执行
        // Button("停止轨迹执行")
        //   .type(ButtonType.ROUNDED_RECTANGLE)
        //   .onClick(() => {
        //
        //   })
        //   .margin({ top: 10, bottom: 10 })
      }
      .margin({ bottom: 10 })

      Button("点头（固定值）")
        .type(ButtonType.Capsule)
        .onClick(() => {
          this.characteristic = this.setCharacteristic(this.arr3);
          logger.info(TAG + 'onClick: send data: this.characteristic = ' + JSON.stringify(this.characteristic));
          gattClientManager.writeCharacteristicValue(this.characteristic as ble.BLECharacteristic);
        })
        .margin({ top: 10, bottom: 10, right: 5 })
    }
    .height('100%')
    .width('100%')
  }

  // 构造BLEDescriptor
  private setDescriptor(des: string, value: ArrayBuffer): ble.BLEDescriptor {
    let descriptor: ble.BLEDescriptor = {
      serviceUuid: gattClientManager.myServiceUuid,
      characteristicUuid: gattClientManager.myCharacteristicUuid,
      descriptorUuid: des,
      descriptorValue: value
    };
    return descriptor;
  }

  // 构造BLECharacteristic
  private setCharacteristic(data: Uint8Array): ble.BLECharacteristic {
    let descriptors: Array<ble.BLEDescriptor> = [];
    let descBuffer = new ArrayBuffer(2);
    let descValue = new Uint8Array(descBuffer);
    descValue[0] = 11;
    descValue[1] = 12;
    descriptors[0] = this.setDescriptor(gattClientManager.myDescriptorUuid_read, new ArrayBuffer(2));
    // descriptors[0] = this.initDescriptor(this.myFirstDescriptorUuid, new ArrayBuffer(2));
    // descriptors[1] = this.initDescriptor(this.mySecondDescriptorUuid, descBuffer);

    // data -> str -> Uint8Array
    // let str = JSON.stringify(data);
    // let array = new Uint8Array(str.length);
    // for (let i = 0; i < str.length; i++) {
    //   array[i] = str.charCodeAt(i);
    //   logger.info(TAG+ 'setCharacteristic: str[i]=' + str[i]);
    //   logger.info(TAG+ 'setCharacteristic: array[i]=' + array[i]);
    // }

    // for (let i = 0; i < this.arr.length; i++) {
    //   logger.info(TAG+ 'setCharacteristic: this.arr=' + this.arr);
    // }

    // 示例传的buffer
    // let charBuffer = new ArrayBuffer(2);
    // let charValue = new Uint8Array(charBuffer);
    // charValue[0] = 1;
    // charValue[1] = 2;

    let characteristic: ble.BLECharacteristic = {
      serviceUuid: gattClientManager.myServiceUuid,
      characteristicUuid: gattClientManager.myCharacteristicUuid,
      // characteristicValue: array.buffer,
      // characteristicValue: this.arr.buffer,
      characteristicValue: data.buffer,
      descriptors: descriptors
    };
    return characteristic;
  }


}