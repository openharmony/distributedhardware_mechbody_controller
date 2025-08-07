import { hilog } from '@kit.PerformanceAnalysisKit';
import { BackupExtensionAbility, BundleVersion } from '@kit.CoreFileKit';

const domain = 0x0000;

export default class EntryBackupAbility extends BackupExtensionAbility {
  public async onBackup() {
    hilog.info(domain, 'testTag', 'onBackup ok');
    await Promise.resolve();
  }

  public async onRestore(bundleVersion: BundleVersion) {
    hilog.info(domain, 'testTag', 'onRestore ok %{public}s', JSON.stringify(bundleVersion));
    await Promise.resolve();
  }
}