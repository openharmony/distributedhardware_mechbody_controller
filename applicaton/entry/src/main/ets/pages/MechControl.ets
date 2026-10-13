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

import mechanicManager from '@ohos.distributedHardware.mechanicManager';
import { BusinessError } from '@ohos.base';
import curves from '@ohos.curves';
import accessibility from '@ohos.accessibility';
import common from '@ohos.app.ability.common';

import { logger } from '../utils/Logger';
import { BUNDLE_NAME, FontSizeScale } from '../common/Constants';
import { UEPointUtils } from '../utils/UEPointUtils';

const TAG = '[MechControl_Page] : ';
let localStorage = LocalStorage.getShared();

@Entry(localStorage)
@Component
struct MechControl {
  @StorageLink('isTrackingEnabled') @Watch('onSwitchChange') isTrackingEnabled: boolean = true;
  @StorageLink('currentLayout') @Watch('onLayoutChange') currentLayout: mechanicManager.CameraTrackingLayout =
    mechanicManager.CameraTrackingLayout.DEFAULT;
  @StorageProp('controlCenterIsDarkMode') isShowAsDarkMode: boolean = false;

  @State currentTrackingLayoutText: ResourceStr = $r('app.string.mech_center');
  @State currentTrackingLayoutSymbol: Resource = $r('sys.symbol.line_1_left_rectangle_line_1_right_viewfinder');
  @State visibilityState: Visibility = Visibility.Hidden;
  @State isOneButton: boolean = !this.isTrackingEnabled;

  private context = this.getUIContext().getHostContext() as common.UIAbilityContext;

  aboutToAppear() {
    logger.info(TAG, 'aboutToAppear in');
    try {
      this.isTrackingEnabled = mechanicManager.getCameraTrackingEnabled();
    } catch (error) {
      let code: number = (error as BusinessError).code;
      let message: string = (error as BusinessError).message;
      logger.error(TAG, `getCameraTrackingEnabled failed. error.code: ${code}, message: ${message}`);
    }

    try {
      this.currentLayout = mechanicManager.getCameraTrackingLayout();
    } catch (error) {
      let code: number = (error as BusinessError).code;
      let message: string = (error as BusinessError).message;
      logger.error(TAG, `getCameraTrackingLayout failed. error.code: ${code}, message: ${message}`);
    }
    logger.info(TAG, `aboutToAppear end. isTrackingEnabled: ${this.isTrackingEnabled}, currentLayout: ${this.currentLayout}`);
  }

  aboutToDisappear(): void {
    logger.info(TAG, 'aboutToDisappear in');
  }

  onPageShow(): void {
    // Trigger the entry animation
    this.visibilityState = Visibility.Visible;
  }

  pageTransition() {
    PageTransitionEnter({ type: RouteType.None, duration: 0 })
    PageTransitionExit({ type: RouteType.None, duration: 0 })
  }

  onSwitchChange() {
    logger.info(TAG, 'onSwitchChange in');
    animateTo({
      duration: 100,
      curve: curves.springMotion(0.55, 0.825, 0.15)
    }, () => {
      this.isOneButton = !this.isTrackingEnabled;
    })
  }

  onLayoutChange() {
    logger.info(TAG, 'onLayoutChange in');
    switch (this.currentLayout) {
      case mechanicManager.CameraTrackingLayout.DEFAULT:
      case mechanicManager.CameraTrackingLayout.MIDDLE: {
        this.currentTrackingLayoutSymbol = $r('sys.symbol.line_1_left_rectangle_line_1_right_viewfinder');
        this.currentTrackingLayoutText = $r('app.string.mech_center');
        break;
      }
      case mechanicManager.CameraTrackingLayout.LEFT: {
        this.currentTrackingLayoutSymbol = $r('sys.symbol.rectangle_line_2_right_viewfinder');
        this.currentTrackingLayoutText = $r('app.string.mech_left');
        break;
      }
      case mechanicManager.CameraTrackingLayout.RIGHT: {
        this.currentTrackingLayoutSymbol = $r('sys.symbol.line_2_left_rectangle_viewfinder');
        this.currentTrackingLayoutText = $r('app.string.mech_right');
        break;
      }
    }
  }

  toggleEffectChange(){
    logger.info(TAG, 'toggleEffectChange in');
    const originalState = this.isTrackingEnabled;
    try {
      if (!this.isTrackingEnabled) {
        logger.info(TAG, 'enable camera tracking');
        mechanicManager.setCameraTrackingEnabled(true);
        UEPointUtils.getInstance().reportFocusTrackingSwitchStateEvent(true);
        this.isTrackingEnabled = true;
      } else {
        logger.info(TAG, 'disable camera tracking');
        mechanicManager.setCameraTrackingEnabled(false);
        UEPointUtils.getInstance().reportFocusTrackingSwitchStateEvent(false);
        this.isTrackingEnabled = false;
      }
    } catch (error) {
      let code: number = (error as BusinessError).code;
      let message: string = (error as BusinessError).message;
      logger.error(TAG, `setUserOperation switch failed. error.code: ${code}, message: ${message}`);
      this.isTrackingEnabled = originalState;
    }
  }

  getSwitchStateInfo(isTrackingEnabled: boolean): string {
    if (isTrackingEnabled) {
      logger.info(TAG, `SwitchState is on. isTrackingEnabled: ${isTrackingEnabled}`);
      return this.context.resourceManager.getStringSync($r('app.string.audio_tracking_switch_on').id);
    } else {
      logger.info(TAG, `SwitchState is off. isTrackingEnabled: ${isTrackingEnabled}`);
      return this.context.resourceManager.getStringSync($r('app.string.audio_tracking_switch_off').id);
    }
  }

  @Builder
  IntelligentTracking() {
    Column() {
      Button() {
        SymbolGlyph($r('sys.symbol.dot_viewfinder'))
          .fontSize('20vp')
          .fontColor([(this.isTrackingEnabled || this.isShowAsDarkMode)
            ? $r('sys.color.icon_on_primary') : $r('sys.color.icon_primary')])
      }
      .buttonStyle(ButtonStyleMode.TEXTUAL)
      .backgroundColor(this.isTrackingEnabled
        ? $r('sys.color.ohos_id_color_emphasize') : $r('sys.color.ohos_id_color_component_normal'))
      .width('40vp')
      .height('40vp')
      .accessibilityText($r('app.string.mech_intelligent_tracking'))
      .type(ButtonType.Circle)
      .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.9 })
      .onClick(() => {
        this.toggleEffectChange();
        try {
          let eventInfo: accessibility.EventInfo = ({
            type: 'announceForAccessibility',
            bundleName: BUNDLE_NAME,
            triggerAction: 'click',
            textAnnouncedForAccessibility: this.getSwitchStateInfo(this.isTrackingEnabled)
          });
          accessibility.sendAccessibilityEvent(eventInfo).then(() => {
            logger.info(TAG, `Succeeded in send event, eventInfo is ${JSON.stringify(eventInfo)}`);
          }).catch((err: Error) => {
            logger.error(TAG, 'Failed to send accessibility event');
          });
        } catch (error) {
          let code: number = (error as BusinessError).code;
          let message: string = (error as BusinessError).message;
          logger.error(TAG, `sendAccessibilityEvent failed,error code: ${code},message: ${message}.`);
        }
        logger.info(TAG, `onClick end, isTrackingEnabled = ${this.isTrackingEnabled}`);
      })

      Text($r('app.string.mech_intelligent_tracking'))
        .fontSize($r('sys.float.ohos_id_text_size_body3'))
        .lineHeight(16)
        .fontWeight(FontWeight.Medium)
        .fontColor(this.isShowAsDarkMode ? $r('sys.color.font_on_primary') : $r('sys.color.font_primary'))
        .margin({ top: $r('sys.float.padding_level2') })
        .textAlign(TextAlign.Center)
        .maxFontScale(FontSizeScale.MD)
        .maxLines(2)
        .textOverflow({ overflow: TextOverflow.Ellipsis })

      Text(this.isTrackingEnabled ? $r('app.string.mech_enable') : $r('app.string.pause'))
        .fontSize($r('sys.float.Caption_M'))
        .lineHeight(13)
        .fontWeight(FontWeight.Regular)
        .fontColor(this.isShowAsDarkMode ? $r('sys.color.font_on_secondary') : $r('sys.color.font_secondary'))
        .margin({ top: $r('sys.float.padding_level1') })
        .textAlign(TextAlign.Center)
        .maxFontScale(FontSizeScale.MD)
        .maxLines(2)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
    }
    .layoutWeight(1)
  }

  @Builder
  TrackingLayout() {
    Column() {
      Button() {
        SymbolGlyph(this.currentTrackingLayoutSymbol)
          .fontSize('20vp')
          .fontColor([$r('sys.color.icon_on_primary')])
      }
      .buttonStyle(ButtonStyleMode.TEXTUAL)
      .backgroundColor($r('sys.color.ohos_id_color_emphasize'))
      .width('40vp')
      .height('40vp')
      .accessibilityText($r('app.string.mech_tracking_layout'))
      .type(ButtonType.Circle)
      .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.9 })
      .onClick(() => {
        logger.info(TAG, 'onClick TrackingLayout in');
        if (this.visibilityState === Visibility.Hidden) {
          logger.warn(TAG, 'repeated onClick!');
          return;
        }
        // Trigger the exit animation
        this.visibilityState = Visibility.Hidden;
        setTimeout(() => {
          this.getUIContext().getRouter().pushUrl({ url: 'pages/TrackingLayout' }, () => {
            logger.info(TAG, 'setTimeout router pushUrl pages/TrackingLayout');
          })
        }, 150)
      })

      Text($r('app.string.mech_tracking_layout'))
        .fontSize($r('sys.float.ohos_id_text_size_body3'))
        .lineHeight(16)
        .fontWeight(FontWeight.Medium)
        .fontColor(this.isShowAsDarkMode ? $r('sys.color.font_on_primary') : $r('sys.color.font_primary'))
        .margin({ top: $r('sys.float.padding_level2') })
        .textAlign(TextAlign.Center)
        .maxFontScale(FontSizeScale.MD)
        .maxLines(2)
        .textOverflow({ overflow: TextOverflow.Ellipsis })

      Text(this.currentTrackingLayoutText)
        .fontSize($r('sys.float.Caption_M'))
        .lineHeight(13)
        .fontWeight(FontWeight.Regular)
        .fontColor(this.isShowAsDarkMode ? $r('sys.color.font_on_secondary') : $r('sys.color.font_secondary'))
        .margin({ top: $r('sys.float.padding_level1') })
        .textAlign(TextAlign.Center)
        .maxFontScale(FontSizeScale.MD)
        .maxLines(2)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
    }
    .layoutWeight(1)
  }

  build() {
    Row() {
      this.IntelligentTracking();

      if (!this.isOneButton) {
        this.TrackingLayout();
      }
    }
    .width('100%')
    .height('100%')
    .alignItems(VerticalAlign.Top)
    .padding({
      top: $r('sys.float.padding_level12'),
      left: `calc((100% - 40vp * 2) / 6)`,
      right: `calc((100% - 40vp * 2) / 6)`
    })
    .expandSafeArea([SafeAreaType.SYSTEM], [SafeAreaEdge.BOTTOM])
    .transition(TransitionEffect.OPACITY.animation({ duration: 150, curve: Curve.Sharp }))
    .visibility(this.visibilityState)
  }
}