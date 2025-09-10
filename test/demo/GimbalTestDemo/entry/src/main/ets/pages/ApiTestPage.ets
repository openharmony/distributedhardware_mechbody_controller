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
import router from '@ohos.router';
import mechanicManager from '@ohos.distributedHardware.mechanicManager';
import { logger } from '../utils/Logger';
import prompt from '@system.prompt';

const TAG = 'ApiTestPage: '

@Entry
@Component
struct ApiTestPage {
  @State yaw: number = 0;
  @State roll: number = 0;
  @State pitch: number = 0;
  @State relativeYaw: number = 0;
  @State relativeRoll: number = 0;
  @State relativePitch: number = 0;
  @State rotateTime: number = 500;
  @State relativeRotateTime: number = 500;
  @State speedRotateTime: number = 500;
  @State yawSpeed: number = 0;
  @State rollSpeed: number = 0;
  @State pitchSpeed: number = 0;
  @State mMechId: number = 0;
  scroller: Scroller = new Scroller();
  controller: TextInputController = new TextInputController();

  private getMechDevices(): mechanicManager.MechInfo[] | undefined {
    let mechDevices: mechanicManager.MechInfo[] | undefined;
    try {
      mechDevices = mechanicManager.getAttachedMechDevices();
      logger.info(`${TAG} mechDevices length: ${mechDevices?.length}`);
    } catch (e) {
      logger.error(`${TAG} getMechDevices fail, error: ${JSON.stringify(e)}`);
    }
    return mechDevices;
  }

  onPageShow(): void {
    logger.info(`${TAG} onPageShow start.`);
    let mechDevices = this.getMechDevices();
    if (mechDevices === undefined) {
      return;
    }
    try {
      if (mechDevices.length > 0) {
        this.mMechId = mechDevices[0]?.mechId;
      }
    } catch (e) {
      logger.error(`${TAG} get MechId fail, error: ${JSON.stringify(e)}`);
    }

    logger.info(`${TAG} onPageShow end: this.mMechId =${this.mMechId}`);
  }

  @Styles
  fancy() {
    .height('100px')
    .width('800px')
    .margin({
      top: 5
    })
  }

  relativeRotate(yaw: number, roll: number, pitch: number) {
    logger.info('relativeRotate in');
    try {
      logger.info('开始旋转');
      let degree: mechanicManager.RotationAngles = {
        yaw: yaw,
        roll: roll,
        pitch: pitch,
      }
      logger.info('旋转参数：degree' + JSON.stringify(degree));
      logger.info('旋转参数：this.relativeRotateTime = ' + this.relativeRotateTime);
      mechanicManager.rotate(this.mMechId, degree, this.relativeRotateTime)
        .then((result) => {
          logger.info('旋转结果：' + JSON.stringify(result));
          prompt.showToast({
            message: `(相对)旋转结果：result= ${JSON.stringify(result)}, degree= ${JSON.stringify(degree)},
                       relativeRotateTime=${this.relativeRotateTime}`,
            duration: 2000
          });
        });
      logger.info('结束旋转');
    } catch (e) {
      logger.error('旋转失败, code: ' + e.code + ' message: ' + e.message);
    }
  }

  rotateBySpeed(yawSpeed: number, rollSpeed: number, pitchSpeed: number) {
    logger.info('relativeRotate in');
    try {
      logger.info('开始旋转');
      let speed: mechanicManager.RotationSpeed = {
        yawSpeed: yawSpeed,
        rollSpeed: rollSpeed,
        pitchSpeed: pitchSpeed
      }
      logger.info('旋转参数：this.speedRotateTime = ' + this.speedRotateTime);
      mechanicManager.rotateBySpeed(this.mMechId, speed, this.speedRotateTime)
        .then((result) => {
          logger.info('旋转结果：' + JSON.stringify(result));
          prompt.showToast({
            message: `(速度)旋转结果：result= ${JSON.stringify(result)}, speed= ${JSON.stringify(speed)},
                       speedRotateTime=${this.speedRotateTime}`,
            duration: 2000
          });
        });
      logger.info('结束旋转');
    } catch (e) {
      logger.error('旋转失败, code: ' + e.code + ' message: ' + e.message);
    }
  }

  build() {
    Column() {
      Scroll(this.scroller) {
        Column() {
          Button() {
            Text('设备连接状态监听注册')
              .fontSize(20)
              .fontColor(Color.White)
          }.onClick(() => {
            router.pushUrl({
              url: "pages/AttachStateChangeCallbackRegister",
            })
          }).fancy();

          Button() {
            Text('设备列表')
              .fontSize(20)
              .fontColor(Color.White)
          }.onClick(() => {
            router.pushUrl({
              url: "pages/DeviceList",
            })
          }).fancy();

          Button('用户使能')
            .onClick(() => {
              try {
                logger.info('用户使能');
                mechanicManager.setUserOperation(mechanicManager.Operation.CONNECT, "123", "123");
                logger.info('用户使能成功');
              } catch (e) {
                logger.error('用户使能失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 5
          })

          Row() {
            Button('启用跟踪')
              .onClick(() => {
                try {
                  logger.info('启用跟踪');
                  mechanicManager.setCameraTrackingEnabled(true);
                  logger.info('启用跟踪成功');
                  prompt.showToast({
                    message: '启用跟踪成功',
                    duration: 1000
                  });
                } catch (e) {
                  logger.error('启用跟踪失败, code: ' + e.code + ' message: ' + e.message);
                }
              }).margin({
              top: 5
            })

            Button('关闭跟踪')
              .onClick(() => {
                try {
                  logger.info('关闭跟踪');
                  mechanicManager.setCameraTrackingEnabled(false);
                  logger.info('关闭跟踪成功');
                  prompt.showToast({
                    message: '关闭跟踪成功',
                    duration: 1000
                  });
                } catch (e) {
                  logger.error('关闭跟踪失败, code: ' + e.code + ' message: ' + e.message);
                }
              }).margin({
              top: 5
            })
          }
          .margin({
            top: 20
          })

          Button('获取跟踪状态')
            .onClick(() => {
              try {
                logger.info('获取跟踪状态');
                let isenabled: boolean = mechanicManager.getCameraTrackingEnabled();
                logger.info('当前跟踪状态: ' + isenabled);
                prompt.showToast({
                  message: '当前跟踪状态: isenabled =' + isenabled,
                  duration: 2000
                });
              } catch (e) {
                logger.error('获取跟踪状态失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 5
          })

          Row() {
            Button('注册跟踪事件监听')
              .onClick(() => {
                try {
                  logger.info('注册跟踪事件监听');
                  mechanicManager.on("trackingEvent", (result: mechanicManager.TrackingEventInfo) => {
                    logger.info('注册跟踪事件监听:result  ' + JSON.stringify(result));
                    prompt.showToast({
                      message: '注册跟踪事件监听成功',
                      duration: 2000
                    });
                  });
                  logger.info('注册跟踪事件监听成功');
                } catch (e) {
                  logger.error('注册跟踪事件监听失败, code: ' + e.code + ' message: ' + e.message);
                }
              }).margin({
              top: 5
            });

            Button('去注册跟踪事件监听')
              .onClick(() => {
                try {
                  logger.info('去注册跟踪事件监听');
                  mechanicManager.off("trackingEvent", (result: mechanicManager.TrackingEventInfo) => {
                    logger.info('result: ' + JSON.stringify(result));
                    prompt.showToast({
                      message: '去注册跟踪事件监听成功',
                      duration: 2000
                    });
                  });
                  logger.info('去注册跟踪事件监听成功');
                } catch (e) {
                  logger.error('去注册跟踪事件监听失败, code: ' + e.code + ' message: ' + e.message);
                }
              }).margin({
              top: 5
            })
          }

          // title：设置布局
          Flex({
            direction: FlexDirection.Row,
            justifyContent: FlexAlign.Start,
            alignItems: ItemAlign.Center
          }) {
            Text('设置布局 ：')
              .fontSize('20fp')
              .fontWeight(FontWeight.Bold)
              .margin({ top: 20, bottom: 25 })
          }

          Button('默认')
            .onClick(() => {
              try {
                logger.info('设置DEFAULT布局');
                mechanicManager.setCameraTrackingLayout(mechanicManager.CameraTrackingLayout.DEFAULT);
                logger.info('设置DEFAULT布局成功');
              } catch (e) {
                logger.error('设置DEFAULT布局失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 20
          })


          Button('居左')
            .onClick(() => {
              try {
                logger.info('设置LEFT布局');
                mechanicManager.setCameraTrackingLayout(mechanicManager.CameraTrackingLayout.LEFT);
                logger.info('设置LEFT布局成功');
              } catch (e) {
                logger.error('设置LEFT布局失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 20
          })

          Button('居中')
            .onClick(() => {
              try {
                logger.info('设置MIDDLE布局');
                mechanicManager.setCameraTrackingLayout(mechanicManager.CameraTrackingLayout.MIDDLE);
                logger.info('设置MIDDLE布局成功');
              } catch (e) {
                logger.error('设置MIDDLE布局失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 20
          })

          Button('居右')
            .onClick(() => {
              try {
                logger.info('设置RIGHT布局');
                mechanicManager.setCameraTrackingLayout(mechanicManager.CameraTrackingLayout.RIGHT);
                logger.info('设置RIGHT布局成功');
              } catch (e) {
                logger.error('设置RIGHT布局失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 20
          })

          Button('查询布局')
            .onClick(() => {
              try {
                logger.info('查询布局');
                let layout = mechanicManager.getCameraTrackingLayout();
                logger.info('查询布局成功，当前布局: ' + layout);
                prompt.showToast({
                  message: '查询布局成功，当前布局: ' + layout,
                  duration: 2000
                });
              } catch (e) {
                logger.error('查询布局, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 20, bottom: 20
          })

          // title：旋转角度&速度控制
          Flex({
            direction: FlexDirection.Row,
            justifyContent: FlexAlign.Start,
            alignItems: ItemAlign.Center
          }) {
            Text('旋转角度控制 ：')
              .fontSize('20fp')
              .fontWeight(FontWeight.Bold)
              .margin({ top: 20, bottom: 25 })
          }

          Flex({
            direction: FlexDirection.Row,
            justifyContent: FlexAlign.Start,
            alignItems: ItemAlign.Center
          }) {
            Text('绝对 ：')
              .fontSize('18fp')
              .fontWeight(FontWeight.Bold)
              .margin({ top: 10, bottom: 10, left: 10 })
          }

          Button('旋转 - 角度（绝对）')
            .onClick(() => {
              try {
                logger.info('开始旋转');
                let degree: mechanicManager.RotationAngles = {
                  yaw: 0.3,
                  roll: 0.0,
                  pitch: 0.0,
                }
                logger.info('旋转参数：this.rotateTime = ' + this.rotateTime);
                mechanicManager.rotateToEulerAngles(this.mMechId, degree, this.rotateTime)
                  .then((result) => {
                    logger.info('旋转结果：' + JSON.stringify(result));
                    prompt.showToast({
                      message: `(绝对)旋转结果：result= ${JSON.stringify(result)}, degree= ${JSON.stringify(degree)},
                       rotateTime=${this.rotateTime}`,
                      duration: 2000
                    });
                  });
                logger.info('结束旋转');
              } catch (e) {
                logger.error('旋转失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 15
          })

          Button('旋转到原点')
            .onClick(() => {
              try {
                logger.info('开始旋转');
                let degree: mechanicManager.EulerAngles = {
                  yaw: 0,
                  roll: 0,
                  pitch: 0,
                }
                mechanicManager.rotateToEulerAngles(this.mMechId, degree, 500)
                  .then((result) => {
                    logger.info('旋转结果：' + JSON.stringify(result));
                  });
                logger.info('结束旋转');
              } catch (e) {
                logger.error('旋转失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 15
          })

          /********相对**********/
          Flex({
            direction: FlexDirection.Row,
            justifyContent: FlexAlign.Start,
            alignItems: ItemAlign.Center
          }) {
            Text('相对 ：')
              .fontSize('18fp')
              .fontWeight(FontWeight.Bold)
              .margin({ top: 10, bottom: 10, left: 10 })
          }

          Button('上')
            .onClick(() => {
              this.pitch = -0.1 * Math.PI;
              this.relativeRotate(0, 0, this.pitch);
            }).margin({
            top: 20
          })

          Flex({
            direction: FlexDirection.Row,
            justifyContent: FlexAlign.SpaceEvenly,
            alignItems: ItemAlign.Center
          }) {
            Button('左')
              .onClick(() => {
                this.yaw = -0.1 * Math.PI;
                this.relativeRotate(this.yaw, 0, 0);
              }).margin({
              top: 20
            })

            Button('右')
              .onClick(() => {
                this.yaw = 0.1 * Math.PI;
                this.relativeRotate(this.yaw, 0, 0);
              }).margin({
              top: 20
            })
          }

          Button('下')
            .onClick(() => {
              this.pitch = 0.1 * Math.PI;
              this.relativeRotate(0, 0, this.pitch);
            }).margin({
            top: 20
          })

          Button('查询最大旋转时间')
            .onClick(() => {
              try {
                logger.info('查询最大旋转时间');
                let maxTime = mechanicManager.getMaxRotationTime(this.mMechId);
                prompt.showToast({
                  message: `查询最大旋转时间:maxTime = ${maxTime}`,
                  duration: 2000
                });
                logger.info('查询最大旋转时间成功，最大时间: ' + maxTime);
              } catch (e) {
                logger.error('查询最大旋转时间失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 30
          })

          // title：旋转速度控制
          Flex({
            direction: FlexDirection.Row,
            justifyContent: FlexAlign.Start,
            alignItems: ItemAlign.Center
          }) {
            Text('旋转速度控制 ：')
              .fontSize('20fp')
              .fontWeight(FontWeight.Bold)
              .margin({ top: 30, bottom: 10 })
          }

          Button('查询旋转速度')
            .onClick(() => {
              try {
                logger.info('查询旋转速度');
                let speedLimit: mechanicManager.RotationSpeed = mechanicManager.getMaxRotationSpeed(this.mMechId);
                logger.info('查询旋转速度成功，速度限位信息: ' + JSON.stringify(speedLimit));
                prompt.showToast({
                  message: `查询旋转速度：speedLimit= ${JSON.stringify(speedLimit)}`,
                  duration: 2000
                });
              } catch (e) {
                logger.error('查询旋转速度失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 10, bottom: 10
          })


          Button('上')
            .onClick(() => {
              this.pitchSpeed = -0.1 * Math.PI;
              this.rotateBySpeed(0, 0, this.pitchSpeed);
            }).margin({
            top: 5
          })

          Flex({
            direction: FlexDirection.Row,
            justifyContent: FlexAlign.SpaceEvenly,
            alignItems: ItemAlign.Center
          }) {
            Button('左')
              .onClick(() => {
                this.yawSpeed = 0.1 * Math.PI;
                this.rotateBySpeed(this.yawSpeed, 0, 0);
              }).margin({
              top: 20
            })

            Button('右')
              .onClick(() => {
                this.yawSpeed = -0.1 * Math.PI;
                this.rotateBySpeed(this.yawSpeed, 0, 0);
              }).margin({
              top: 20
            })
          }

          Button('下')
            .onClick(() => {
              this.pitchSpeed = 0.1 * Math.PI;
              this.rotateBySpeed(0, 0, this.pitchSpeed);
            }).margin({
            top: 20
          })

          Button('停止旋转')
            .onClick(() => {
              try {
                logger.info('停止旋转');
                mechanicManager.stopMoving(this.mMechId)
                  .then((result) => {
                    logger.info('获得停止完成');
                    logger.info('获得停止结果：' + JSON.stringify(result));
                  });
                logger.info('停止成功');
              } catch (e) {
                logger.error('停止失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 30
          })

          Button('查询当前位置')
            .onClick(() => {
              try {
                logger.info('查询当前位置');
                let currentAngles: mechanicManager.EulerAngles = mechanicManager.getCurrentAngles(this.mMechId);
                logger.info('查询当前位置成功，位置: ' + JSON.stringify(currentAngles));
                prompt.showToast({
                  message: `查询当前位置 currentAngles : ${JSON.stringify(currentAngles)}`,
                  duration: 2000
                });
              } catch (e) {
                logger.error('查询当前位置失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 5
          })

          Button('查询旋转限位信息')
            .onClick(() => {
              try {
                logger.info('查询旋转限位信息');
                let rotationLimit: mechanicManager.RotationLimits = mechanicManager.getRotationLimits(this.mMechId);
                logger.info('查询旋转限位信息成功，限位信息: ' + JSON.stringify(rotationLimit));
                prompt.showToast({
                  message: `查询旋转限位信息 rotationLimit : ${JSON.stringify(rotationLimit)}`,
                  duration: 2000
                });
              } catch (e) {
                logger.error('查询旋转限位信息失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 5
          })

          Button('查询旋转轴状态')
            .onClick(() => {
              try {
                logger.info('查询旋转轴状态');
                let axisStatus: mechanicManager.RotationAxesStatus =
                  mechanicManager.getRotationAxesStatus(this.mMechId);
                logger.info('查询旋转轴状态成功，轴状态: ' + JSON.stringify(axisStatus));
                prompt.showToast({
                  message: `查询旋转轴状态 axisStatus : ${JSON.stringify(axisStatus)}`,
                  duration: 2000
                });
              } catch (e) {
                logger.error('查询旋转轴状态失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 5
          })

          Button('注册轴状态监听')
            .onClick(() => {
              try {
                logger.info('注册轴状态监听');
                mechanicManager.on("rotationAxesStatusChange", (result) => {
                  logger.info('result: ' + JSON.stringify(result));
                  prompt.showToast({
                    message: `监听旋转轴状态：result.status = ${JSON.stringify(result.status)}`,
                    duration: 2000
                  });
                });
                logger.info('注册成功');
              } catch (e) {
                logger.error('注册失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 5
          });

          Button('去注册轴状态监听')
            .onClick(() => {
              try {
                logger.info('去注册轴状态监听');
                mechanicManager.off("rotationAxesStatusChange", (result) => {
                  logger.info('result: ' + JSON.stringify(result));
                  prompt.showToast({
                    message: `去注册轴状态监听：result.status = ${JSON.stringify(result.status)}`,
                    duration: 2000
                  });
                });
                logger.info('去注册成功');
              } catch (e) {
                logger.error('去注册失败, code: ' + e.code + ' message: ' + e.message);
              }
            }).margin({
            top: 5
          });

          Button('找人')
          .onClick(() => {
            try {
              logger.info('找人启动');
              let targetInfo : mechanicManager.TargetInfo = {
                targetType: 0
              }
              let searchParam : mechanicManager.SearchParams = {
                direction: 0
              }
              mechanicManager.searchTarget(targetInfo, searchParam, (result) => {
                logger.info('result: ' + JSON.stringify(result));
                prompt.showToast({
                  message: `找人完成：result.status = ${JSON.stringify(result)}`,
                  duration: 2000
                });
              });
              logger.info('找人启动成功');
            } catch (e) {
              logger.error('找人启动失败, code: ' + e.code + ' message: ' + e.message);
            }
          }).margin({
          top: 5
        });

          Button('找人')
          .onClick(() => {
            try {
              logger.info('找人启动');
              let targetInfo : mechanicManager.TargetInfo = {
                targetType: 0
              }
              let searchParam : mechanicManager.SearchParams = {
                direction: 0
              }
              mechanicManager.searchTarget(targetInfo, searchParam, (result) => {
                logger.info('result: ' + JSON.stringify(result));
                prompt.showToast({
                  message: `找人完成：result.status = ${JSON.stringify(result)}`,
                  duration: 2000
                });
              });
              logger.info('找人启动成功');
            } catch (e) {
              logger.error('找人启动失败, code: ' + e.code + ' message: ' + e.message);
            }
          }).margin({
          top: 5
        });

        }
        .width('100%')
      }
      .scrollable(ScrollDirection.Vertical) // 滚动方向纵向
      .scrollBar(BarState.On) // 滚动条常驻显示
      .scrollBarColor(Color.Gray) // 滚动条颜色
      .scrollBarWidth(3) // 滚动条宽度
      .friction(0.6)
      .edgeEffect(EdgeEffect.None)
      .onScrollEdge((side: Edge) => {
        logger.info('To the edge');
      })
      .onScrollStop(() => {
        logger.info('Scroll Stop');
      })
    }.width('100%').height('100%')
  }
}