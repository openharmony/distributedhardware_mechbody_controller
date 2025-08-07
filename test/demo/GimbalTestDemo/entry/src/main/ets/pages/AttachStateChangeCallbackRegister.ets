import router from '@ohos.router';
import mechanicManager from '@ohos.distributedHardware.mechanicManager';
import prompt from '@system.prompt';
import { logger } from '../utils/Logger';
import { BusinessError } from '@kit.BasicServicesKit';

@Entry
@Component
struct AttachStateChangeCallbackRegister {
  @State message: string = 'Hello World';
  callback1 = (result: mechanicManager.AttachStateChangeInfo) => {
    logger.info('callback1 result: ' + JSON.stringify(result));
    prompt.showToast({
      message: `callback1 监听连接状态：result.state = ${JSON.stringify(result.state)}`,
      duration: 2000
    });
  };

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
      Text(this.message)
        .width('100%')
        .height('30%')
        .padding(10)
        .textAlign(TextAlign.Center)
        .fontSize(20);

      Column() {
        Button() {
          Text('注册 - 1')
            .fontSize(20)
            .fontColor(Color.White)
        }.onClick(() => {

          try {
            logger.info('注册 - 1');
            mechanicManager.on("attachStateChange", this.callback1);
            this.message = '注册 - 1: 成功';
            logger.info('注册 - 1: 成功');
          } catch (error) {
            let code: number = (error as BusinessError).code;
            let message: string = (error as BusinessError).message;
            this.message = '注册 - 1: 失败, code: ' + code + ' message: ' + message;
            logger.error('注册 - 1: 失败, code: ' + code + ' message: ' + message);
          }
        }).fancy();

        Column() {
          Button() {
            Text('去注册 - 1')
              .fontSize(20)
              .fontColor(Color.White)
          }.onClick(() => {
            try {
              logger.info('去注册 - 1');
              mechanicManager.off("attachStateChange", this.callback1);
              this.message = '去注册 - 1: 成功';
              logger.info('去注册 - 1: 成功');
            } catch (error) {
              let code: number = (error as BusinessError).code;
              let message: string = (error as BusinessError).message;
              this.message = '去注册 - 1: 失败, code: ' + code + ' message: ' + message;
              logger.error('去注册 - 1: 失败, code: ' + code + ' message: ' + message);
            }
          }).fancy();

          Button() {
            Text('去注册 - all')
              .fontSize(20)
              .fontColor(Color.White)
          }.onClick(() => {
            try {
              logger.info('去注册注册 - all');
              mechanicManager.off("attachStateChange");
              this.message = '去注册 - all: 成功';
              logger.info('去注册 - all: 成功');
            } catch (error) {
              let code: number = (error as BusinessError).code;
              let message: string = (error as BusinessError).message;
              this.message = '去注册 - all: 失败, code: ' + code + ' message: ' + message;
              logger.error('去注册 - all: 失败, code: ' + code + ' message: ' + message);
            }
          }).fancy();
        }.width('45%');
      }.height('800px')

        Column() {
          Button() {
            Text('返回')
              .fontSize(20)
              .fontColor(Color.White)
          }.onClick(() => {
            router.back();
          }).fancy();

        }.width('50%');

    }
  }
}