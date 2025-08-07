import router from '@ohos.router';
import mechanicManager from '@ohos.distributedHardware.mechanicManager';
import { logger } from '../utils/Logger';

@Entry
@Component
struct DeviceList {
  @State deviceInfo: string = '点击查询获取设备列表信息';

  @Styles
  fancy() {
    .height('100px')
    .width('400px')
    .margin({
      top: 5
    })
  }

  build() {
    Column() {
      Text(this.deviceInfo)
        .width('100%')
        .height('70%')
        .padding(10)
        .textAlign(TextAlign.Center)
        .fontSize(20);

      Column(){
        Button() {
          Text('查询')
            .fontSize(20)
            .fontColor(Color.White)
        }.onClick(() => {
          try {
            logger.info('查询设备列表');
            let mechInfos: mechanicManager.MechInfo[] = mechanicManager.getAttachedMechDevices();
            this.deviceInfo = JSON.stringify(mechInfos);
            logger.info('设备列表: ' + JSON.stringify(mechInfos));
          } catch (e) {
            this.deviceInfo = '查询失败, code: ' + e.code + ' message: ' + e.message;
            logger.error('查询失败, code: ' + e.code + ' message: ' + e.message);
          }
        }).fancy();

        Button() {
          Text('返回')
            .fontSize(20)
            .fontColor(Color.White)
        }.onClick(() => {
          router.back();
        }).fancy();
      }

    }
    .height('100%')
    .width('100%')
  }
}