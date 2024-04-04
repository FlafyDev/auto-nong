#include <Geode/Geode.hpp>
#include <fleym.nongd/include/jukebox.hpp>
#include <matjson.hpp>

using namespace geode::prelude;

#include "../ui/an_dropdown_layer.hpp"
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
#include "../managers/auto_nong_manager.hpp"
#include <ranges>

struct ANCustomSongWidget : geode::Modify<ANCustomSongWidget, CustomSongWidget> {
  bool m_showNong;
  CCMenu *m_nongMenu;

  std::set<int> getSongIds() {
    auto kv = std::views::keys(m_songs);
    std::set<int> songIds{kv.begin(), kv.end()};
    songIds.insert(m_songInfoObject->m_songID);
    return songIds;
  }

  void updateSongInfo() {
    CustomSongWidget::updateSongInfo();
    if (!m_fields->m_showNong) {
      m_fields->m_showNong =
          GameManager::get()->m_levelEditorLayer == nullptr && AutoNongManager::get()->anySongExists(getSongIds());

      if (m_fields->m_showNong) {
        this->scheduleOnce(schedule_selector(ANCustomSongWidget::showPopup), 0.2);
        auto spr = CCSprite::create("logo.png"_spr);
        spr->setScale(0.225f);

        auto btn =
            CCMenuItemSpriteExtra::create(spr, this, menu_selector(ANCustomSongWidget::onNongBtn));

        m_fields->m_nongMenu = CCMenu::create();
        m_fields->m_nongMenu->addChild(btn);
        this->addChild(m_fields->m_nongMenu);
      }
    }
    if (m_fields->m_nongMenu != nullptr) {
      // Different position if info button is visible...
      if (m_songs.size() > 1 || m_sfx.size() > 1) {
        m_fields->m_nongMenu->setPosition(ccp(116.f, 30.f));
      } else {
        m_fields->m_nongMenu->setPosition(ccp(135.f, 28.f));
      }
    }
  }

  void showPopup(float) {
    const int levelID = AutoNongManager::get()->getCurrentLevelID();
    if (levelID == 0)
      return;
    if (!Mod::get()->getSettingValue<bool>("show-level-popup"))
      return;

    std::vector<int> blacklistedLevels =
        Mod::get()->getSavedValue<std::vector<int>>("show-level-popup");
    if (std::find(blacklistedLevels.begin(), blacklistedLevels.end(), levelID) !=
        blacklistedLevels.end())
      return;
    blacklistedLevels.push_back(levelID);
    Mod::get()->setSavedValue<std::vector<int>>("show-level-popup", blacklistedLevels);

    auto popup = createQuickPopup(
        "This level has a NONG song", "A NONG song has been found for this level.\nDownload it?",
        "No", "Yes",
        [this](auto, bool btn2) {
          if (!btn2) {
            return;
          }
          onNongBtn(nullptr);
        },
        false);
    popup->m_scene = this->getParent();
    popup->show();
  }

  void onNongBtn(CCObject *) {
    for (int songId : getSongIds()) {
      auto anSongs = AutoNongManager::get()->getNongsFromSongID(songId);
      if (!anSongs.empty()) {
        auto layer = ANDropdownLayer::create(songId, AutoNongManager::get()->getNongsFromSongID(songId), this, 1, 1);
        layer->m_noElasticity = true;
        layer->setZOrder(106);
        layer->show();
      }
    }
  }
};

