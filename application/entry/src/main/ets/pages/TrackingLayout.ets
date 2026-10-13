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

import { logger } from '../utils/Logger';
import { FontSizeScale } from '../common/Constants';
import { UEPointUtils } from '../utils/UEPointUtils';

const TAG = '[TrackingLayout_Page] : ';
let localStorage = LocalStorage.getShared();

@Entry(localStorage)
@Component
struct TrackingLayout {
  @StorageProp('currentLayout') currentLayout: mechanicManager.CameraTrackingLayout = mechanicManager.CameraTrackingLayout.DEFAULT;
  @StorageProp('controlCenterIsDarkMode') isShowAsDarkMode: boolean = false;

  @State currentTrackingLayoutText: ResourceStr = $r('app.string.mech_center');
  @State currentTrackingLayoutSymbol: Resource = $r('sys.symbol.line_1_left_rectangle_line_1_right_viewfinder');
  @State visibilityState: Visibility = Visibility.Hidden;

  pageTransition() {
    PageTransitionEnter({ type: RouteType.None, duration: 0 })
    PageTransitionExit({ type: RouteType.None, duration: 0 })
  }

  aboutToAppear() {
    logger.info(TAG, `aboutToAppear in. currentLayout: ${this.currentLayout}`);
  }

  onPageShow(): void {
    // Trigger the entry animation
    this.visibilityState = Visibility.Visible;
  }

  @Builder
  BackButton() {
    Column() {
      Button() {
        SymbolGlyph($r('sys.symbol.chevron_backward'))
          .fontSize('20vp')
          .fontColor([this.isShowAsDarkMode ? $r('sys.color.icon_on_primary') : $r('sys.color.icon_primary')])
      }
      .width('40vp')
      .height('40vp')
      .type(ButtonType.Circle)
      .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.9 })
      .buttonStyle(ButtonStyleMode.TEXTUAL)
      .accessibilityText($r('app.string.audio_back'))
      .backgroundColor($r('sys.color.ohos_id_color_component_normal'))
      .onClick(() => {
        logger.info(TAG, 'onClick BackButton');
        // Trigger the exit animation
        this.visibilityState = Visibility.Hidden;
        setTimeout(() => {
          this.getUIContext().getRouter().back();
        }, 150);
      })
    }
    .layoutWeight(1)
  }

  @Builder
  TrackingLayoutLeft() {
    Column() {
      Button() {
        SymbolGlyph($r('sys.symbol.rectangle_line_2_right_viewfinder'))
          .fontSize('20vp')
          .fontColor([(this.currentLayout == mechanicManager.CameraTrackingLayout.LEFT || this.isShowAsDarkMode) ? $r('sys.color.icon_on_primary') : $r('sys.color.icon_primary')])
      }
      .width('40vp')
      .height('40vp')
      .type(ButtonType.Circle)
      .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.9 })
      .buttonStyle(ButtonStyleMode.TEXTUAL)
      .accessibilityText($r('app.string.mech_left'))
      .backgroundColor(this.currentLayout == mechanicManager.CameraTrackingLayout.LEFT ?
        $r('sys.color.ohos_id_color_emphasize') : $r('sys.color.ohos_id_color_component_normal'))
      .onClick(() => {
        logger.info(TAG, 'onClick TrackingLayoutLeft in');
        this.setCameraTrackingLayout(mechanicManager.CameraTrackingLayout.LEFT);
      })

      Text($r('app.string.mech_left'))
        .fontSize($r('sys.float.Body_S'))
        .lineHeight(16)
        .fontWeight(FontWeight.Regular)
        .fontColor(this.isShowAsDarkMode ? $r('sys.color.font_on_primary') : $r('sys.color.font_primary'))
        .margin({ top: $r('sys.float.padding_level2') })
        .textAlign(TextAlign.Center)
        .maxFontScale(FontSizeScale.MD)
        .maxLines(2)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
    }
    .layoutWeight(1)
  }

  @Builder
  TrackingLayoutCenter() {
    Column() {
      Button() {
        SymbolGlyph($r('sys.symbol.line_1_left_rectangle_line_1_right_viewfinder'))
          .fontSize('20vp')
          .fontColor([(this.currentLayout == mechanicManager.CameraTrackingLayout.MIDDLE ||
            this.currentLayout == mechanicManager.CameraTrackingLayout.DEFAULT ||
            this.isShowAsDarkMode) ? $r('sys.color.icon_on_primary') : $r('sys.color.icon_primary')])
      }
      .width('40vp')
      .height('40vp')
      .type(ButtonType.Circle)
      .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.9 })
      .buttonStyle(ButtonStyleMode.TEXTUAL)
      .accessibilityText($r('app.string.mech_center'))
      .backgroundColor((this.currentLayout == mechanicManager.CameraTrackingLayout.MIDDLE ||
        this.currentLayout == mechanicManager.CameraTrackingLayout.DEFAULT) ? $r('sys.color.ohos_id_color_emphasize') :
        $r('sys.color.ohos_id_color_component_normal'))
      .onClick(() => {
        logger.info(TAG, 'onClick TrackingLayoutCenter in');
        this.setCameraTrackingLayout(mechanicManager.CameraTrackingLayout.MIDDLE);
      })

      Text($r('app.string.mech_center'))
        .fontSize($r('sys.float.Body_S'))
        .lineHeight(16)
        .fontWeight(FontWeight.Regular)
        .fontColor(this.isShowAsDarkMode ? $r('sys.color.font_on_primary') : $r('sys.color.font_primary'))
        .margin({ top: $r('sys.float.padding_level2') })
        .textAlign(TextAlign.Center)
        .maxFontScale(FontSizeScale.MD)
        .maxLines(2)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
    }
    .layoutWeight(1)
  }

  @Builder
  TrackingLayoutRight() {
    Column() {
      Button() {
        SymbolGlyph($r('sys.symbol.line_2_left_rectangle_viewfinder'))
          .fontSize('20vp')
          .fontColor([(this.currentLayout == mechanicManager.CameraTrackingLayout.RIGHT ||
            this.isShowAsDarkMode) ? $r('sys.color.icon_on_primary') : $r('sys.color.icon_primary')])
      }
      .width('40vp')
      .height('40vp')
      .type(ButtonType.Circle)
      .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.9 })
      .buttonStyle(ButtonStyleMode.TEXTUAL)
      .accessibilityText($r('app.string.mech_right'))
      .backgroundColor(this.currentLayout == mechanicManager.CameraTrackingLayout.RIGHT ?
        $r('sys.color.ohos_id_color_emphasize') : $r('sys.color.ohos_id_color_component_normal'))
      .onClick(() => {
        logger.info(TAG, 'onClick TrackingLayoutRight in');
        this.setCameraTrackingLayout(mechanicManager.CameraTrackingLayout.RIGHT);
      })

      Text($r('app.string.mech_right'))
        .fontSize($r('sys.float.Body_S'))
        .lineHeight(16)
        .fontWeight(FontWeight.Regular)
        .fontColor(this.isShowAsDarkMode ? $r('sys.color.font_on_primary') : $r('sys.color.font_primary'))
        .margin({ top: $r('sys.float.padding_level2') })
        .textAlign(TextAlign.Center)
        .maxFontScale(FontSizeScale.MD)
        .maxLines(2)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
    }
    .layoutWeight(1)
  }

  build() {
    Row() {
      this.BackButton();
      this.TrackingLayoutLeft();
      this.TrackingLayoutCenter();
      this.TrackingLayoutRight();
    }
    .width('100%')
    .height('100%')
    .alignItems(VerticalAlign.Top)
    .padding({
      top: $r('sys.float.padding_level16'),
      left: `calc((100% - 40vp * 4) / 10)`,
      right: `calc((100% - 40vp * 4) / 10)`
    })
    .expandSafeArea([SafeAreaType.SYSTEM], [SafeAreaEdge.BOTTOM])
    .transition(TransitionEffect.OPACITY.animation({ duration: 150, curve: Curve.Sharp }))
    .visibility(this.visibilityState)
  }

  setCameraTrackingLayout(layout: mechanicManager.CameraTrackingLayout) {
    logger.info(TAG, `setCameraTrackingLayout: ${layout}`);
    let originalLayout = this.currentLayout;
    try {
      mechanicManager.setCameraTrackingLayout(layout);
      UEPointUtils.getInstance().reportCameraTrackingLayoutEvent(layout);
      this.currentLayout = layout;
    } catch (error) {
      this.currentLayout = originalLayout;
      let code: number = (error as BusinessError).code;
      let message: string = (error as BusinessError).message;
      logger.error(TAG, `setCameraTrackingLayout failed. Layout: ${layout}, error.code: ${code}, message: ${message}`);
    }
  }
}
