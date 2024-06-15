#include "an_song_cell.hpp"
#include "../managers/auto_nong_manager.hpp"
#include "../types/an_song.hpp"
#include "../utils/utils.hpp"
#include "an_add_local_popup.hpp"
#include "fleym.nongd/include/jukebox.hpp"
#include "list_cell.hpp"

bool ANSongCell::init(int songId, int songJukeboxId, std::shared_ptr<ANSong> song,
                      ANDropdownLayer *parentPopup, CCSize const &size, bool isRobtopSong) {
  if (!JBListCell::init(size))
    return false;

  m_songId = songId;
  m_songJukeboxId = songJukeboxId;
  m_anSong = song;
  m_parentPopup = parentPopup;
  m_isRobtopSong = isRobtopSong;
  m_customSongWidget = parentPopup->m_parentWidget;
  m_songPath = AutoNongManager::get()->getSongPath(*m_anSong);

  CCSprite *spr;
  CCMenu *menu;

  auto height = this->getContentSize().height;
  auto contentSize = this->getContentSize();

  // Progress Bar
  m_progressBarBack = CCSprite::createWithSpriteFrameName("d_circle_01_001.png");
  m_progressBarBack->setColor(ccc3(50, 50, 50));
  m_progressBarBack->setPosition(ccp(295.f, height / 2));
  m_progressBarBack->setScale(0.62f);

  spr = CCSprite::createWithSpriteFrameName("d_circle_01_001.png");
  spr->setColor(ccc3(0, 255, 0));
  m_progressBar = CCProgressTimer::create(spr);
  m_progressBar->setType(CCProgressTimerType::kCCProgressTimerTypeRadial);
  m_progressBar->setPercentage(50.f);
  m_progressBar->setID("progress-bar");
  m_progressBar->setPosition(ccp(295.f, height / 2));
  m_progressBar->setScale(0.66f);

  this->addChild(m_progressBar);
  this->addChild(m_progressBarBack);

  // Download button
  spr = CCSprite::createWithSpriteFrameName("GJ_downloadBtn_001.png");
  spr->setScale(0.8f);
  m_downloadButton =
      CCMenuItemSpriteExtra::create(spr, this, menu_selector(ANSongCell::onDownload));
  m_downloadButton->setAnchorPoint(ccp(0.5f, 0.5f));
  m_downloadButton->setID("set-button");
  menu = CCMenu::create();
  menu->addChild(m_downloadButton);
  menu->setAnchorPoint(ccp(0, 0));
  menu->setPosition(ccp(295.f, height / 2));
  menu->setID("button-menu1");
  this->addChild(menu);

  // Set button
  m_setToggle =
      CCMenuItemToggler::createWithStandardSprites(this, menu_selector(ANSongCell::onSetSong), .6f);
  m_setToggle->setPositionX(-10.f);
  menu = CCMenu::create();
  menu->setAnchorPoint(ccp(0, 0));
  menu->setPosition(ccp(272.f, height / 2));
  menu->setID("button-menu2");
  menu->addChild(m_setToggle);
  this->addChild(menu);

  // Delete button
  spr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
  spr->setScale(0.74f);
  m_trashButton = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ANSongCell::onDeleteSong));
  menu = CCMenu::create();
  menu->setAnchorPoint(ccp(0, 0));
  menu->setPosition(ccp(295.f, height / 2));
  menu->setID("button-menu2");
  menu->addChild(m_trashButton);
  this->addChild(menu);

  if (song->m_index == "local") {
    // Edit button
    auto editSpr = CCSprite::createWithSpriteFrameName("GJ_editBtn_001.png");
    editSpr->setScale(.3f);
    auto editBtn =
        CCMenuItemSpriteExtra::create(editSpr, this, menu_selector(ANSongCell::onEditSong));
    editBtn->setID("edit-btn");
    menu = CCMenu::create();
    menu->setAnchorPoint(ccp(0, 0));
    menu->setPosition(ccp(17.f, height * 0.7));
    menu->setID("button-menu3");
    menu->addChild(editBtn);
    menu->setZOrder(2);
    this->addChild(menu);

    // Publish button
    auto publishSpr = CCSprite::createWithSpriteFrameName("GJ_shareBtn_001.png");
    publishSpr->setScale(.3f);
    auto publishBtn =
        CCMenuItemSpriteExtra::create(publishSpr, this, menu_selector(ANSongCell::onPublishSong));
    publishBtn->setID("publish-btn");
    menu = CCMenu::create();
    menu->setAnchorPoint(ccp(0, 0));
    menu->setPosition(ccp(17.f, height * 0.3));
    menu->setID("button-menu3");
    menu->addChild(publishBtn);
    menu->setZOrder(2);
    this->addChild(menu);
  }

  setButtonsState();

  m_songInfoLayer = CCLayer::create();
  m_songNameLabel = CCLabelBMFont::create(m_anSong->m_name.c_str(), "bigFont.fnt");
  m_songNameLabel->limitLabelWidth(220.f, 0.7f, 0.1f);
  m_songNameLabel->setID("song-name");

  m_artistNameLabel = CCLabelBMFont::create(m_anSong->m_artist.c_str(), "goldFont.fnt");
  m_artistNameLabel->limitLabelWidth(120.f, 0.7f, 0.1f);
  m_artistNameLabel->setID("artist-name");

  m_sourceLabel = CCLabelBMFont::create(
      fmt::format("{} : {}", getSubstringAfterSlash(m_anSong->m_index), m_anSong->getSource())
          .c_str(),
      "goldFont.fnt");
  m_sourceLabel->limitLabelWidth(120.f, 0.5f, 0.1f);
  m_sourceLabel->setID("artist-name");

  m_songInfoLayer->addChild(m_sourceLabel);
  m_songInfoLayer->addChild(m_artistNameLabel);
  m_songInfoLayer->addChild(m_songNameLabel);
  m_songInfoLayer->setID("song-info");
  auto layout = ColumnLayout::create();
  layout->setAutoScale(false);
  layout->setAxisAlignment(AxisAlignment::Center);
  layout->setCrossAxisLineAlignment(AxisAlignment::Start);
  m_songInfoLayer->setContentSize(ccp(240.f, height - 20.f));
  m_songInfoLayer->setAnchorPoint(ccp(0.f, 0.f));
  m_songInfoLayer->setPosition(ccp(12.f + (song->m_index == "local" ? 20.f : 0.f), 1.5f));
  m_songInfoLayer->setLayout(layout);

  this->addChild(m_songInfoLayer);
  return true;
}

void ANSongCell::onSetSong(CCObject *target) {
  if (m_setToggle->isToggled()) {
    AutoNongManager::get()->unsetSong(m_songJukeboxId);
  } else {
    AutoNongManager::get()->setSong(*m_anSong, m_songJukeboxId);
  }
  // Cancel the toggling of the button that happens after the callback.
  m_setToggle->toggle(!m_setToggle->isToggled());
}

void ANSongCell::setButtonsState() {
  auto status = AutoNongManager::get()->getSongDownloadStatus(*m_anSong, m_songJukeboxId);

  if (auto *d = status.getStatus<ANSongDownloadStatus::Downloading>()) {
    m_trashButton->setVisible(false);
    m_downloadButton->setVisible(true);
    m_setToggle->setVisible(false);

    m_progressBar->setPercentage(d->progress * 100.f);
    m_progressBar->setVisible(true);
    m_progressBarBack->setVisible(true);
    m_downloadButton->setColor(ccc3(105, 105, 105));
    return;
  }

  m_downloadButton->setColor(ccc3(255, 255, 255));
  m_progressBar->setVisible(false);
  m_progressBarBack->setVisible(false);

  const bool downloaded = status.isStatus<ANSongDownloadStatus::Downloaded>();
  m_trashButton->setVisible(downloaded);
  m_downloadButton->setVisible(!downloaded);
  m_setToggle->setVisible(downloaded);
  m_setToggle->toggle(status.isStatus<ANSongDownloadStatus::Active>());
}

void ANSongCell::onDeleteSong(CCObject *target) {
  AutoNongManager::get()->deleteSong(*m_anSong, m_songJukeboxId);
}

void ANSongCell::onEditSong(CCObject *target) {
  ANAddLocalPopup::create(m_parentPopup, m_anSong)->show();
}

void ANSongCell::onDownload(CCObject *target) {
  AutoNongManager::get()->downloadSong(m_anSong, m_songJukeboxId);
}

void ANSongCell::onPublishSong(CCObject *target) {
  if (auto url = AutoNongManager::get()->generatePublishUrl(*m_anSong, m_songId)) {
    CCApplication::get()->openURL(url->c_str());
  }
}

ANSongCell *ANSongCell::create(int songId, int songJukeboxId, std::shared_ptr<ANSong> song,
                               ANDropdownLayer *parentPopup, CCSize const &size,
                               bool isRobtopSong) {
  auto ret = new ANSongCell();
  if (ret && ret->init(songId, songJukeboxId, song, parentPopup, size, isRobtopSong)) {
    return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}
