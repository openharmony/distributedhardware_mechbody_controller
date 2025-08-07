import { logger } from '../utils/Logger';
import { router } from '@kit.ArkUI';

const TAG = '[Index_Page] ';


@Entry
@Component
struct Index {
  @State message: string = '方向控制 : ';
  @State deviceId: string = '';
  @State deviceIdArr: Array<string> = [''];
  @State deviceName: string = '';

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

      Button("北向接口")
        .type(ButtonType.Capsule)
        .align(Alignment.Start)
        .onClick(()=>{
          router.pushUrl({url:'pages/ApiTestPage'},()=>{})
        })
        .margin({
          top:50,
          bottom: 15
        })

      Button("模拟智慧生活配对连接")
        .type(ButtonType.Capsule)
        .align(Alignment.Start)
        .onClick(()=>{
          router.pushUrl({url:'pages/DeviceConnectPage'},()=>{})
        })
        .margin({
          top:50,
          bottom: 15
        })
    }
    .height('100%')
    .width('100%')
  }
}