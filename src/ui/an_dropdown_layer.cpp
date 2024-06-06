#include "an_dropdown_layer.hpp"
#include "../managers/auto_nong_manager.hpp"
#include "an_add_local_popup.hpp"
#include "an_song_cell.hpp"

bool ANDropdownLayer::setup(int songId, CustomSongWidget *parent, int popupNumber, int totalPopups,
                            bool isRobtopSong) {
  m_songID = songId;
  m_parentWidget = parent;
  m_popupNumber = popupNumber;
  m_totalPopups = totalPopups;
  m_isRobtopSong = isRobtopSong;

  CCMenu *menu;

  auto contentSize = m_mainLayer->getContentSize();

  auto manifestLabel = CCLabelBMFont::create(
      fmt::format("{} / {}", popupNumber, totalPopups).c_str(), "chatFont.fnt");
  manifestLabel->setPosition(contentSize.width / 2, 13.f);
  manifestLabel->limitLabelWidth(140.f, 0.6f, 0.1f);
  manifestLabel->setColor(cc3x(0xc2c2c2));
  manifestLabel->setID("manifest-label");
  m_mainLayer->addChild(manifestLabel);

  auto plusSpr = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
  plusSpr->setScale(.8f);
  auto addNongBtn =
      CCMenuItemSpriteExtra::create(plusSpr, this, menu_selector(ANDropdownLayer::onAddNong));
  addNongBtn->setPosition(contentSize.width - 26.f, 28.f);
  addNongBtn->setID("add-nong-btn");

  menu = CCMenu::create();
  menu->setAnchorPoint(ccp(0, 0));
  menu->setPosition(ccp(0, 0));
  menu->setID("button-menu2");
  menu->addChild(addNongBtn);
  menu->setZOrder(2);
  m_mainLayer->addChild(menu);

  auto optionsSpr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
  optionsSpr->setScale(.8f);
  auto openOptionsBtn = CCMenuItemSpriteExtra::create(
      optionsSpr, this, menu_selector(ANDropdownLayer::onOpenOptions));
  openOptionsBtn->setPosition(26.f, 28.f);
  openOptionsBtn->setID("open-options-btn");

  menu = CCMenu::create();
  menu->setAnchorPoint(ccp(0, 0));
  menu->setPosition(ccp(0, 0));
  menu->setID("button-menu3");
  menu->addChild(openOptionsBtn);
  menu->setZOrder(2);
  m_mainLayer->addChild(menu);

  auto titleLabel =
      CCLabelBMFont::create(fmt::format("Replace song for {}", songId).c_str(), "goldFont.fnt");
  titleLabel->setPosition(contentSize.width / 2, contentSize.height - 22.f);
  titleLabel->limitLabelWidth(contentSize.width - 40.f, 1.3f, 0.1f);
  titleLabel->setID("title-label");
  m_mainLayer->addChild(titleLabel);

  updateCells();

  auto listpos = m_listView->getPosition();
  auto leftspr = CCSprite::createWithSpriteFrameName("GJ_commentSide2_001.png");
  leftspr->setPosition(ccp(listpos.x - 162.f, listpos.y));
  leftspr->setScaleY(6.8f);
  leftspr->setZOrder(1);
  m_mainLayer->addChild(leftspr);
  auto rightspr = CCSprite::createWithSpriteFrameName("GJ_commentSide2_001.png");
  rightspr->setPosition(ccp(listpos.x + 162.f, listpos.y));
  rightspr->setScaleY(6.8f);
  rightspr->setFlipX(true);
  rightspr->setZOrder(1);
  m_mainLayer->addChild(rightspr);
  auto bottomspr = CCSprite::createWithSpriteFrameName("GJ_commentTop2_001.png");
  bottomspr->setPosition(ccp(listpos.x, listpos.y - 95.f));
  bottomspr->setFlipY(true);
  bottomspr->setScaleX(0.934f);
  bottomspr->setZOrder(1);
  m_mainLayer->addChild(bottomspr);
  auto topspr = CCSprite::createWithSpriteFrameName("GJ_commentTop2_001.png");
  topspr->setPosition(ccp(listpos.x, listpos.y + 95.f));
  topspr->setScaleX(0.934f);
  topspr->setZOrder(1);
  m_mainLayer->addChild(topspr);

  handleTouchPriority(this);
  return true;
}

void ANDropdownLayer::updateCells() {
  if (m_listView != nullptr) {
    m_listView->removeMeAndCleanup();
  }

  m_songCandidates = AutoNongManager::get()->getNongsFromSongID(m_songID);

  auto contentSize = m_mainLayer->getContentSize();

  auto songs = CCArray::create();
  m_songCells.clear();

  std::sort(m_songCandidates.begin(), m_songCandidates.end(),
            [](const std::shared_ptr<ANSong> &a, const std::shared_ptr<ANSong> &b) {
              if (a->m_index == "local" && b->m_index != "local") {
                return true;
              } else if (a->m_index != "local" && b->m_index == "local") {
                return false;
              } else if (a->m_index != b->m_index) {
                return a->m_index < b->m_index;
              } else {
                return a->m_name < b->m_name;
              }
            });

  for (auto song : m_songCandidates) {
    auto cell = ANSongCell::create(m_songID, m_isRobtopSong ? -m_songID - 1 : m_songID, song, this,
                                   m_cellSize, m_isRobtopSong);
    songs->addObject(cell);
    m_songCells.push_back(cell);
  }

  m_listView = ListView::create(songs, m_cellSize.height, m_cellSize.width, 200.f);
  m_listView->setPosition(contentSize.width / 2, contentSize.height / 2 - 15.f);
  m_listView->ignoreAnchorPointForPosition(false);
  m_mainLayer->addChild(m_listView);
}

void ANDropdownLayer::onAddNong(CCObject *) {
  ANAddLocalPopup::create(this, std::nullopt)->show();
  // CCApplication::get()->openURL(
  //     fmt::format("https://github.com/FlafyDev/auto-nong-indexes/issues/"
  //                 "new?template=add-nong-song.yml&title=Add+Nong+Song&song-id={}",
  //                 m_songID)
  //         .c_str());
}

void ANDropdownLayer::onOpenOptions(CCObject *) { geode::openSettingsPopup(Mod::get()); }

void ANDropdownLayer::onClose(CCObject *target) {
  for (auto cell : m_songCells) {
    cell->m_parentPopup = nullptr;
  }
  Popup<int, CustomSongWidget *, int, int, bool>::onClose(target);
}

void ANDropdownLayer::updateCellsButtonsState() {
  for (auto cell : m_songCells) {
    cell->setButtonsState();
  }
}
