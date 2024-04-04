#include "managers/auto_nong_manager.hpp"
#include <Geode/Geode.hpp>
#include <fleym.nongd/include/jukebox.hpp>
#include <matjson.hpp>

using namespace geode::prelude;

#include "types/serializable_vector.hpp"
#include <Geode/Loader.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/CustomSongWidget.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/SongInfoObject.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/loader/SettingEvent.hpp>
#include <Geode/modify/CustomSongWidget.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/IDManager.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/utils/cocos.hpp>

#include "Geode/loader/Log.hpp"
#include "ui/an_clear_popup_blacklist.hpp"
#include "ui/an_multi_string_setting.hpp"


$on_mod(Loaded) {
  Mod::get()->addCustomSetting<MultiStringSettingValue>("indexes",
                                                        Mod::get()->getSettingDefinition("indexes")->get<CustomSetting>()->json->get<std::vector<std::string>>("default")
                                                        );
  Mod::get()->addCustomSetting<ANClearPPBlacklistSettingValue>("_blacklistPPClear", 0);

  AutoNongManager::get()->loadIndexes();
  // Reload the indexes every hour
  AutoNongManager::get()->schedule(schedule_selector(AutoNongManager::loadIndexesSchedule), 60 * 60, kCCRepeatForever, 0);
  // Reload the indexes on change
  listenForSettingChanges("indexes", +[](MultiStringSettingStruct value) {
    AutoNongManager::get()->loadIndexes();
  });
}
