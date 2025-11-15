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
import bleAdvertisingManager from '../model/BleAdvertisingManager';
import gattServerManager from '../model/GattServerManager';
import { logger } from '../utils/Logger';
import { util } from '@kit.ArkTS';
import prompt from '@system.prompt';

const TAG = '[GattServerPage_Page] ';

@Entry
@Component
struct GattServerPage {
  @State recvMessage: string = '';
  @StorageLink('charReqValue') @Watch('onCharReqValueChange') charReqValue: Uint8Array|null = null;
  @State bufferValuesStr: string = '';
  @State notifyData:string = '';
  // @State notifyDataNumber:number = 0;
  controller: TextInputController = new TextInputController();
  arr1: Uint8Array =
    new Uint8Array([0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x22, 0x0c,
      0x4d, 0x65, 0x63, 0x68, 0x61, 0x6e, 0x69, 0x63, 0x44, 0x61, 0x74, 0x61,
      0x0e, 0x00,
      0x03, 0x00,
      0x02, 0x42,
      0x9a, 0x99, 0x07, 0x43,
      0x00, 0x80, 0x96, 0x43,
      0x9a, 0x99, 0xe1, 0xc1,
    ]);


  onCharReqValueChange(){
    logger.info(TAG + 'onCharReqValueChange in. this.recvMessage:' + this.recvMessage);
    // prompt.showToast({
    //   message: `charReqValue: ${this.charReqValue}`,
    //   duration: 4000
    // });
    // if (this.charReqValue) {
    //   this.bufferValuesStr = Array.from(this.charReqValue).map(b => b.toString(16).padStart(2, '0')).join(' ');
    // }
    logger.info(TAG + 'onCharReqValueChange end. this.bufferValuesStr:' + this.bufferValuesStr);
  }


  bufferToStr( buffer: ArrayBuffer ){
    logger.info(TAG + 'bufferToStr in');
    // logger.info(TAG + 'bufferToStr: buffer.LEN:' + buffer.byteLength);
    // 假设已经有了一个 ArrayBuffer
    // const arrayBuffer: ArrayBuffer = new ArrayBuffer(4);
    // new Uint8Array(buffer).set([1, 2, 3, 4]); // 设置一些数据

    // 将 ArrayBuffer 转换为 Uint8Array
    let uint8Array = new Uint8Array(buffer);

    let textDecoderOptions: util.TextDecoderOptions = {
      fatal: false,
      ignoreBOM : true
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

  strToArrayBuffer(str: string) {
    let buf = new ArrayBuffer(str.length * 2);
    let bufView = new Uint16Array(buf);
    for (let i = 0, strLen = str.length; i < strLen; i++) {
      bufView[i] = str.charCodeAt(i);
    }
    return bufView;
  }

  build() {
    Column() {
      Text('BLE服务(云台)')
        .id('GattServerPageHelloWorld')
        .fontSize('20fp')
        .fontWeight(FontWeight.Bold)
        .margin({bottom:100})

      Row() {
        Button('注册服务').onClick(() => {
          gattServerManager.registerServer();
        })
          .margin({right:10})

        Button('删除服务').onClick(() => {
          gattServerManager.unRegisterServer();
        })
      }
      .margin({bottom:30})

      Row() {
        Button('订阅readChar').onClick(() => {
          gattServerManager.onCharacteristicRead();
        })
          .margin({right:10})

        Button('订阅writeChar').onClick(() => {
          gattServerManager.onCharacteristicWrite();
        })

        Text(this.recvMessage)
          .fontSize('20fp')
          .fontWeight(FontWeight.Bold)
          .margin({top:10})
      }
      .margin({bottom:30})

      Row() {
        Button('开启广播').onClick(() => {
          bleAdvertisingManager.startAdvertising();
        })
          .margin({right:10})

        Button('关闭广播').onClick(() => {
          bleAdvertisingManager.stopAdvertising();
        })
      }
      .margin({bottom:30})

      Row() {
        TextInput({ text: this.notifyData, placeholder: 'input your data...', controller: this.controller })
          .placeholderColor(Color.Grey)
          .placeholderFont({ size: 14, weight: 400 })
          .caretColor(Color.Blue)
          .width('95%')
          .height(40)
          .margin(20)
          .fontSize(14)
          .fontColor(Color.Black)
            // .inputFilter('[a-z]', (e) => {
            //   logger.info(JSON.stringify(e));
            // })
          .onChange((value: string) => {
            this.notifyData = value;
            logger.info(TAG + JSON.stringify(this.notifyData));
          })
      }
      .margin({bottom:30})

      Row() {
        Button('notify client').onClick(() => {
          logger.info(TAG + 'click Notify Data: ' + this.arr1);
          // 1.根据notifyData的长度创建一个合适的ArrayBuffer
          let arrayBufferC = new ArrayBuffer(this.arr1.length);
          let notifyCharValue = new Uint8Array(arrayBufferC);

          // 2.遍历notifyData并将每个字符转换为ASCII值
          for (let i = 0; i < this.arr1.length; i++) {
            notifyCharValue[i] = this.arr1[i];
          }
          let bufferValuesStr = Array.from(notifyCharValue).map(b => b.toString(16).padStart(2, '0')).join(' ');
          logger.info(TAG + 'notifyCharacteristicChanged. notifyDataStr:' + bufferValuesStr);
          gattServerManager.notifyCharacteristicChanged(this.arr1);
        })
          .margin({right:10})
      }
      .margin({bottom:30})

    }
    .height('100%')
    .width('100%')
  }
}