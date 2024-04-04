#include <Geode/Geode.hpp>
#include <fleym.nongd/include/jukebox.hpp>
#include <matjson.hpp>

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/CustomSongWidget.hpp>
#include <Geode/loader/SettingEvent.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/CustomSongWidget.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/binding/SongInfoObject.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/Loader.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/IDManager.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/ui/Popup.hpp>
#include "ui/an_dropdown_layer.hpp"
#include "types/an_song.hpp"

#include "Geode/loader/Log.hpp"
#include "ui/an_multi_string_setting.hpp"
#include "ui/an_clear_popup_blacklist.hpp"
#include "types/serializable_vector.hpp"
#include "utils/utils.hpp"
#include <ranges>


std::map<int, std::vector<std::shared_ptr<ANSong>>> songIDToSongs;

std::vector<std::shared_ptr<ANSong>> findSongs(int songID) {
    if (songIDToSongs.find(songID) == songIDToSongs.end()) {
        return std::vector<std::shared_ptr<ANSong>>{};
    }
    return songIDToSongs[songID];
}

bool anySongExists(std::set<int> songIDs) {
  for (int songID : songIDs) {
    if (songIDToSongs.find(songID) != songIDToSongs.end()) return true;
  }

  return false;
}


int current_level_id = 0;
class $modify(MyCustomSongWidget, CustomSongWidget) {
    bool showNong;
    CCMenu* nongMenu;

    std::set<int> getSongIds() {
      auto kv = std::views::keys(m_songs);
      std::set<int> songIds{ kv.begin(), kv.end() };
      songIds.insert(m_songInfoObject->m_songID);
      return songIds;
    }

    void updateSongInfo() {
      CustomSongWidget::updateSongInfo();
      if (!m_fields->showNong) {
        m_fields->showNong = GameManager::get()->m_levelEditorLayer == nullptr && anySongExists(getSongIds());

        if (m_fields->showNong) {
          this->scheduleOnce(schedule_selector(MyCustomSongWidget::showPopup), 0.2);
          auto spr = CCSprite::create("logo.png"_spr);
          spr->setScale(0.225f);

          auto btn = CCMenuItemSpriteExtra::create(
              spr, this, menu_selector(MyCustomSongWidget::onNongBtn)
          );

          m_fields->nongMenu = CCMenu::create();
          m_fields->nongMenu->addChild(btn);
          this->addChild(m_fields->nongMenu);
        }
      }
      if (m_fields->nongMenu != nullptr) {
        // Different position if info button is visible...
        if (m_songs.size() > 1 || m_sfx.size() > 1) {
          m_fields->nongMenu->setPosition(ccp(116.f, 30.f));
        } else {
          m_fields->nongMenu->setPosition(ccp(135.f, 28.f));
        }
      }
    }

    void showPopup(float) {
      if (current_level_id == 0) return;
      if (!Mod::get()->getSettingValue<bool>("show-level-popup")) return;

      std::vector<int> blacklistedLevels = Mod::get()->getSavedValue<std::vector<int>>("show-level-popup");
      if(std::find(blacklistedLevels.begin(), blacklistedLevels.end(), current_level_id) != blacklistedLevels.end()) return;
      blacklistedLevels.push_back(current_level_id);
      Mod::get()->setSavedValue<std::vector<int>>("show-level-popup", blacklistedLevels);

      auto popup = createQuickPopup("This level has a NONG song", 
        "A NONG song has been found for this level.\nDownload it?", 
        "No", 
        "Yes",
        [this](auto, bool btn2) {
          if (!btn2) {
            return;
          }
          onNongBtn(nullptr);
        },
        false
      );
      popup->m_scene = this->getParent();
      popup->show();
    }


    void onNongBtn(CCObject*) {
      log::info("Nong button clicked {}", current_level_id);
      for (int songId : getSongIds()) {
        log::info("song: {}", songId);
        auto anSongs = findSongs(songId);
        if (!anSongs.empty()) {
          auto layer = ANDropdownLayer::create(songId, findSongs(songId), this, 1, 1);
          layer->m_noElasticity = true;
          layer->setZOrder(106);
          layer->show();
        }
      }
    }
};

class $modify(ANLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool p1) {
        if (!LevelInfoLayer::init(level, p1)) {
            return false;
        }
        current_level_id = level->m_levelID;
        return true;
    }
};



void loadANIndexes() {
  std::vector<std::string> indexes = Mod::get()->getSettingValue<MultiStringSettingStruct>("indexes").m_strings;

  songIDToSongs = {};

  for (std::string index : indexes) {
    if (index.size() < 5) continue;
    geode::utils::web::AsyncWebRequest()
    .fetch(index)
    .bytes()
    .then([index](const geode::ByteVector& r) {
      std::string jsonString = decompressGz(static_cast<std::vector<uint8_t>>(r));

      matjson::Value jsonObj = matjson::parse(jsonString);

      for (const auto& songData : jsonObj.as_array()) {
        std::string name = songData["name"].as_string();
        std::string artist = songData.contains("artist") ? songData["artist"].as_string() : "";
        std::string source = songData["source"].as_string();

        std::shared_ptr<ANSong> song;
        if (source == "youtube") {
            std::string yt_id = songData["yt-id"].as_string();
            song = std::make_shared<ANYTSong>(name, artist, index, yt_id);
        } else if (source == "host") {
            std::string url = songData["url"].as_string();
            song = std::make_shared<ANHostSong>(name, artist, index, url);
        } else {
            std::cout << "Unsupported source: " << source << std::endl;
            continue;
        }

        if (songData.contains("songs")) {
            for (matjson::Value songID : songData["songs"].as_array()) {
                songIDToSongs[songID.as_int()].push_back(song);
            }
        }
      }
    });
  }
}

// Every hour reload the indexes
class $modify(ANGameManager, GameManager) {
  void loadANIndexesLoop(float) {
    loadANIndexes();
  }
};

$on_mod(Loaded) {
  Mod::get()->addCustomSetting<MultiStringSettingValue>("indexes", MultiStringSettingValue::defaultIndexes);
  Mod::get()->addCustomSetting<ANClearPPBlacklistSettingValue>("_blacklistPPClear", 0);

  loadANIndexes();
  GameManager::get()->schedule(schedule_selector(ANGameManager::loadANIndexesLoop), 60*60, kCCRepeatForever, 0);
  listenForSettingChanges("indexes", +[](MultiStringSettingStruct value) {
    loadANIndexes();
  });
}
