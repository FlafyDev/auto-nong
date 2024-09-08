#include "includes/geode.hpp"
#include "managers/auto_nong_manager.hpp"
#include "types/serializable_vector.hpp"
#include "ui/an_clear_popup_blacklist.hpp"
#include "ui/multi_string_setting.hpp"

$on_mod(Loaded) {
  Mod::get()->addCustomSetting<MultiStringSettingValue>(
      "indexes2", Mod::get()
                      ->getSettingDefinition("indexes2")
                      ->get<CustomSetting>()
                      ->json->get<std::vector<std::string>>("default"));
  Mod::get()->addCustomSetting<ANClearPPBlacklistSettingValue>("_blacklistPPClear", 0);

  AutoNongManager::get()->loadIndexes();
  // Reload the indexes every hour
  // AutoNongManager::get()->schedule(schedule_selector(AutoNongManager::loadIndexesSchedule), 60 *
  // 60,
  //                                  kCCRepeatForever, 0);

  // Reload the indexes on change
  listenForSettingChanges(
      "indexes2", +[](MultiStringSettingStruct value) { AutoNongManager::get()->loadIndexes(); });
}
