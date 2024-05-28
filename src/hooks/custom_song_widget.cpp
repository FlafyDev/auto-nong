#include "../includes/geode.hpp"
#include "../managers/auto_nong_manager.hpp"
#include "../types/serializable_vector.hpp"
#include "../ui/an_dropdown_layer.hpp"

struct ANCustomSongWidget : geode::Modify<ANCustomSongWidget, CustomSongWidget> {
  struct Fields {
    bool m_showNong;
    CCMenu *m_nongMenu;
  };

  std::set<int> getSongIds() {
    std::set<int> songIds;
    for (const auto &pair : m_songs) {
      songIds.insert(pair.first);
    }
    songIds.insert(m_songInfoObject->m_songID);
    return songIds;
  }

  void updateSongInfo() {
    CustomSongWidget::updateSongInfo();
    if (!m_fields->m_showNong) {
      m_fields->m_showNong = !m_isRobtopSong && GameManager::get()->m_levelEditorLayer == nullptr &&
                             AutoNongManager::get()->anySongExists(getSongIds());

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
      if (m_songs.size() > 1 || m_sfx.size() > 0) {
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
        auto layer = ANDropdownLayer::create(
            songId, AutoNongManager::get()->getNongsFromSongID(songId), this, 1, 1);
        layer->m_noElasticity = true;
        layer->setZOrder(106);
        layer->show();
      }
    }
  }
};
