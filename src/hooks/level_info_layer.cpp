#include <Geode/Geode.hpp>
#include <fleym.nongd/include/jukebox.hpp>
#include <matjson.hpp>

using namespace geode::prelude;

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

#include "../managers/auto_nong_manager.hpp"

struct ANLevelInfoLayer : geode::Modify<ANLevelInfoLayer, LevelInfoLayer> {
  bool init(GJGameLevel *level, bool p1) {
    if (!LevelInfoLayer::init(level, p1)) {
      return false;
    }
    AutoNongManager::get()->setCurrentLevelID(level->m_levelID);
    return true;
  }
};

