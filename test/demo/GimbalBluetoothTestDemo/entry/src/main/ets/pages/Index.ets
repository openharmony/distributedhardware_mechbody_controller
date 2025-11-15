import { BusinessError } from '@kit.BasicServicesKit';
import { ble, connection } from '@kit.ConnectivityKit';
import { logger } from '../utils/Logger';
import { abilityAccessCtrl, Permissions } from '@kit.AbilityKit';
import { router } from '@kit.ArkUI';

const TAG = '[Index_Page] ';
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

@Entry
@Component
struct Index {
  @State message: string = '方向控制 : ';
  @State deviceId: string = '';
  @State deviceIdArr: Array<string> = [''];
  @State deviceName: string = '';
  @State gimbalLocationData: GimbalLocationData | null = null;
  @State characteristic: ble.BLECharacteristic | null = null;


  aboutToAppear(): void {
    logger.info(TAG + 'aboutToAppear in');
  }

  build() {
    Column() {
      Text(this.deviceName)
        .fontSize('20fp')
        .fontWeight(FontWeight.Bold)
        .alignRules({
          center: { anchor: '__container__', align: VerticalAlign.Center },
          middle: { anchor: '__container__', align: HorizontalAlign.Center }
        })
        .margin({ bottom: 10 })
        .borderRadius($r('sys.float.ohos_id_corner_radius_menu'))

      Button("模拟云台侧（BLE服务端）")
        .type(ButtonType.Capsule)
        .align(Alignment.Start)
        .onClick(()=>{
          router.pushUrl({url:'pages/GattServerPage'},()=>{})
        })
        .margin({
          top:100,
          bottom: 15
        })

      Button("手机侧（BLE客户端）")
        .type(ButtonType.Capsule)
        .align(Alignment.Start)
        .onClick(()=>{
          router.pushUrl({url:'pages/GattClientPage'},()=>{})
        })
        .margin({
          top:50,
          bottom: 15
        })

      // Button("手机侧（BLE客户端-弹框显示接收数据）")
      //   .type(ButtonType.Capsule)
      //   .align(Alignment.Start)
      //   .onClick(()=>{
      //     // router.pushUrl({url:'pages/GattClientPage'},()=>{})
      //     router.pushUrl({url:'pages/GattClientPage_Test1'},()=>{})
      //   })
      //   .margin({
      //     top:50,
      //     bottom: 15
      //   })
    }
    .height('100%')
    .width('100%')
    // .backgroundImage($r('app.media.background'))
  }
}