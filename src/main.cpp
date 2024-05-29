#include "includes/geode.hpp"
#include "managers/auto_nong_manager.hpp"
#include "types/serializable_vector.hpp"
#include "ui/an_clear_popup_blacklist.hpp"
#include "ui/multi_string_setting.hpp"

$on_mod(Loaded) {
  Mod::get()->addCustomSetting<MultiStringSettingValue>(
      "indexes",
      Mod::get()->getSettingDefinition("indexes")->get<CustomSetting>()->json->get<Vec<String>>(
          "default"));
  Mod::get()->addCustomSetting<ANClearPPBlacklistSettingValue>("_blacklistPPClear", 0);

  AutoNongManager::get()->loadIndexes();
  // Reload the indexes every hour
  // AutoNongManager::get()->schedule(schedule_selector(AutoNongManager::loadIndexesSchedule), 60 *
  // 60,
  //                                  kCCRepeatForever, 0);

  // Reload the indexes on change
  listenForSettingChanges(
      "indexes", +[](MultiStringSettingStruct value) { AutoNongManager::get()->loadIndexes(); });
}
