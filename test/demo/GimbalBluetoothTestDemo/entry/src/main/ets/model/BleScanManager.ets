import ble from '@ohos.bluetooth.ble';
import { BusinessError } from '@ohos.base';
import List from '@ohos.util.List';
import { HashMap } from '@kit.ArkTS';
import connection from '@ohos.bluetooth.connection';
import { logger } from '../utils/Logger';
import prompt from '@system.prompt';

const TAG: string = '[BleScanManager]';

// 参考Core Assigned Numbers
const BLE_ADV_TYPE_FLAG = 0x01;
const BLE_ADV_TYPE_16_BIT_SERVICE_UUIDS_INCOMPLETE = 0x02;
const BLE_ADV_TYPE_16_BIT_SERVICE_UUIDS_COMPLETE = 0x03;
const BLE_ADV_TYPE_32_BIT_SERVICE_UUIDS_INCOMPLETE = 0x04;
const BLE_ADV_TYPE_32_BIT_SERVICE_UUIDS_COMPLETE = 0x05;
const BLE_ADV_TYPE_128_BIT_SERVICE_UUIDS_INCOMPLETE = 0x06;
const BLE_ADV_TYPE_128_BIT_SERVICE_UUIDS_COMPLETE = 0x07;
const BLE_ADV_TYPE_LOCAL_NAME_SHORT = 0x08;
const BLE_ADV_TYPE_LOCAL_NAME_COMPLETE = 0x09;
const BLE_ADV_TYPE_TX_POWER_LEVEL = 0x0A;
const BLE_ADV_TYPE_16_BIT_SERVICE_SOLICITATION_UUIDS = 0x14;
const BLE_ADV_TYPE_128_BIT_SERVICE_SOLICITATION_UUIDS = 0x15;
const BLE_ADV_TYPE_32_BIT_SERVICE_SOLICITATION_UUIDS = 0x1F;
const BLE_ADV_TYPE_16_BIT_SERVICE_DATA = 0x16;
const BLE_ADV_TYPE_32_BIT_SERVICE_DATA = 0x20;
const BLE_ADV_TYPE_128_BIT_SERVICE_DATA = 0x21;
const BLE_ADV_MANUFACTURER_SPECIFIC_TYPE = 0xFF;

const BLUETOOTH_UUID_16_BIT_LENGTH = 2;
const BLUETOOTH_UUID_32_BIT_LENGTH = 4;
const BLUETOOTH_UUID_128_BIT_LENGTH = 16;
const SERVICE_UUIDS = 1;
const SERVICE_SOLICITATION_UUIDS = 2;

export class BleScanManager {
  advertiseFlags: number = -1;
  txPowerLevel: number = -1;
  localName: string|undefined = undefined;
  serviceUuids: List<string>|undefined = undefined;
  serviceSolicitationUuids: List<string>|undefined = undefined;
  manufactureSpecificDatas: HashMap<number, Uint8Array>|undefined = undefined;
  serviceData: HashMap<string, Uint8Array>|undefined = undefined;

  // 1 订阅扫描结果
  public onScanResult() {
    logger.info(TAG+ 'onScanResult in.');
    ble.on('BLEDeviceFind', (data: Array<ble.ScanResult>) => {
      if (data.length > 0) {
        logger.info(TAG+ 'BLE scan result: datadata[0].deviceId = ' + data[0].deviceId);
        logger.info(TAG+ 'BLE scan result: data = ' + JSON.stringify(data));
        // OM7P-70006Z , djiServerTest, OM7P-210096-6
        if (data[0].deviceName == '0'){
          logger.info(TAG+ 'onScanResult: data[0].deviceName = ' + data[0].deviceName);
          AppStorage.setOrCreate('scanResultDeviceId', data[0].deviceId);
          AppStorage.setOrCreate('scanResultDeviceName', data[0].deviceName);
          prompt.showToast({
            message: 'scanResult:' + data[0].deviceId,
            duration: 4000
          });
          this.parseScanResult(data[0].data);
        }
      }
    });
    logger.info(TAG+ 'onScanResult end.');
  }

  public parseScanResult(data: ArrayBuffer) {
    let advData = new Uint8Array(data);
    if (advData.byteLength == 0) {
      logger.warn(TAG+ 'nothing');
      return;
    }

    let msg = 'logAdvData: ';
    for (let i = 0; i < advData.byteLength; i++) {
      msg += advData[i] + ' ';
    }
    logger.info(TAG+ msg);

    let curPos = 0;
    while (curPos < advData.byteLength) {
      let length = advData[curPos++]; // 获取当前广播字段类型的长度，curPos指向下一个位置
      if (length == 0) {
        break;
      }
      // 获取当前广播字段内容长度
      let advDataLength = length - 1;
      // 获取当前广播字段类型，curPos指向下一个位置，从该位置解析实际内容，参考Core Specification Supplement, PartA
      let advDataType = advData[curPos++];
      switch (advDataType) {
        case BLE_ADV_TYPE_FLAG:
          this.advertiseFlags = advData[curPos];
          break;
        case BLE_ADV_TYPE_LOCAL_NAME_SHORT:
        case BLE_ADV_TYPE_LOCAL_NAME_COMPLETE:
          this.localName = advData.slice(curPos, curPos + advDataLength).toString(); // todo ascii
          break;
        case BLE_ADV_TYPE_TX_POWER_LEVEL:
          this.txPowerLevel = advData[curPos];
          break;
        case BLE_ADV_TYPE_16_BIT_SERVICE_UUIDS_INCOMPLETE:
        case BLE_ADV_TYPE_16_BIT_SERVICE_UUIDS_COMPLETE:
          this.parseUuid(BLUETOOTH_UUID_16_BIT_LENGTH, curPos, advDataLength, advData, SERVICE_UUIDS);
          break;
        case BLE_ADV_TYPE_32_BIT_SERVICE_UUIDS_INCOMPLETE:
        case BLE_ADV_TYPE_32_BIT_SERVICE_UUIDS_COMPLETE:
          this.parseUuid(BLUETOOTH_UUID_32_BIT_LENGTH, curPos, advDataLength, advData, SERVICE_UUIDS);
          break;
        case BLE_ADV_TYPE_128_BIT_SERVICE_UUIDS_INCOMPLETE:
        case BLE_ADV_TYPE_128_BIT_SERVICE_UUIDS_COMPLETE:
          this.parseUuid(BLUETOOTH_UUID_128_BIT_LENGTH, curPos, advDataLength, advData, SERVICE_UUIDS);
          break;
        case BLE_ADV_TYPE_16_BIT_SERVICE_SOLICITATION_UUIDS:
          this.parseUuid(BLUETOOTH_UUID_16_BIT_LENGTH, curPos, advDataLength, advData, SERVICE_SOLICITATION_UUIDS);
          break;
        case BLE_ADV_TYPE_32_BIT_SERVICE_SOLICITATION_UUIDS:
          this.parseUuid(BLUETOOTH_UUID_32_BIT_LENGTH, curPos, advDataLength, advData, SERVICE_SOLICITATION_UUIDS);
          break;
        case BLE_ADV_TYPE_128_BIT_SERVICE_SOLICITATION_UUIDS:
          this.parseUuid(BLUETOOTH_UUID_128_BIT_LENGTH, curPos, advDataLength, advData, SERVICE_SOLICITATION_UUIDS);
          break;
        case BLE_ADV_TYPE_16_BIT_SERVICE_DATA:
          this.parseServiceData(BLUETOOTH_UUID_16_BIT_LENGTH, curPos, advDataLength, advData);
          break;
        case BLE_ADV_TYPE_32_BIT_SERVICE_DATA:
          this.parseServiceData(BLUETOOTH_UUID_32_BIT_LENGTH, curPos, advDataLength, advData);
          break;
        case BLE_ADV_TYPE_128_BIT_SERVICE_DATA:
          this.parseServiceData(BLUETOOTH_UUID_128_BIT_LENGTH, curPos, advDataLength, advData);
          break;
        case BLE_ADV_MANUFACTURER_SPECIFIC_TYPE:
          this.parseManufactureData(curPos, advDataLength, advData);
          break;
        default:
          break;
      }
      curPos += advDataLength; // curPos指向下一个字段类型
    }
  }

  parseUuid(uuidLength: number, curPos: number, advDataLength: number, advData: Uint8Array, uuidType: number) {
    while (advDataLength > 0) {
      let tmpData: Uint8Array = advData.slice(curPos, uuidLength);
      if (uuidType == SERVICE_UUIDS) {
        this.serviceUuids?.add(tmpData.toString());
      } else if (uuidType == SERVICE_SOLICITATION_UUIDS) {
        this.serviceSolicitationUuids?.add(tmpData.toString());
      }
      advDataLength -= uuidLength;
      curPos += uuidLength;
    }
  }

  parseServiceData(uuidLength: number, curPos: number, advDataLength: number, advData: Uint8Array) {
    let tmpUuid: Uint8Array = advData.slice(curPos, uuidLength);
    let tmpValue: Uint8Array = advData.slice(curPos + uuidLength, advDataLength - uuidLength);
    this.serviceData?.set(tmpUuid.toString(), tmpValue);
  }

  parseManufactureData(curPos: number, advDataLength: number, advData: Uint8Array) {
    let manufactureId = advData[curPos + 1] << 8 + advData[curPos];
    let tmp = advData.slice(curPos + 2, advDataLength - 2);
    this.manufactureSpecificDatas?.set(manufactureId, tmp);
  }

  // 2 开启扫描
  public startScan() {
    // 2.1 构造扫描过滤器，需要能够匹配预期的广播包内容
    let manufactureId = 4567;
    let manufactureData: Uint8Array = new Uint8Array([1, 2, 3, 4]);
    let manufactureDataMask: Uint8Array = new Uint8Array([0xFF, 0xFF, 0xFF, 0xFF]);
    let scanFilter: ble.ScanFilter = { // 根据业务实际情况定义过滤器
      // manufactureId: manufactureId,
      // manufactureData: manufactureData.buffer,
      // manufactureDataMask: manufactureDataMask.buffer,
      // serviceUuid:'0000FFE0-0000-1000-8000-00805F9B34FB',
      // deviceId: 'C6:62:0A:2F:11:CA',
      // name:'OM7P-70006Z'
    };

    // 2.2 构造扫描参数
    let scanOptions: ble.ScanOptions = {
      interval: 500,
      dutyMode: ble.ScanDuty.SCAN_MODE_LOW_POWER,
      matchMode: ble.MatchMode.MATCH_MODE_AGGRESSIVE,
      phyType: ble.PhyType.PHY_LE_1M
    }
    try {
      this.onScanResult(); // 订阅扫描结果
      ble.startBLEScan([scanFilter], scanOptions);
      logger.info(TAG+ 'startBleScan success');
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }

  // 3 关闭扫描
  public stopScan() {
    try {
      ble.off('BLEDeviceFind', (data: Array<ble.ScanResult>) => { // 取消订阅扫描结果
        logger.info(TAG+ 'off success');
      });
      ble.stopBLEScan();
      logger.info(TAG+ 'stopBleScan success');
    } catch (err) {
      logger.error(TAG+ 'errCode: ' + (err as BusinessError).code + ', errMessage: ' + (err as BusinessError).message);
    }
  }
}

let bleScanManager = new BleScanManager();
export default bleScanManager as BleScanManager;