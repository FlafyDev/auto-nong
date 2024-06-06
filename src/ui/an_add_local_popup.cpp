#include "an_add_local_popup.hpp"
#include "../managers/auto_nong_manager.hpp"

bool ANAddLocalPopup::setup(ANDropdownLayer *parent,
                            std::optional<std::shared_ptr<ANSong>> indexIndex) {
  m_parentPopup = parent;
  m_indexIndex = indexIndex;

  this->setTitle(indexIndex.has_value() ? "Update Song" : "Add Song");

  m_infoButton =
      CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
                                    this, menu_selector(ANAddLocalPopup::showInfo));
  m_infoMenu = CCMenu::create();
  m_infoMenu->setID("info-menu");
  m_infoButton->setID("info-button");
  m_infoMenu->addChild(this->m_infoButton);
  m_infoMenu->setContentSize(m_infoButton->getScaledContentSize());
  m_infoMenu->setAnchorPoint({1.0f, 1.0f});
  m_infoMenu->setPosition(m_mainLayer->getContentSize().width - 10.f,
                          m_mainLayer->getContentSize().height - 10.f);
  m_infoMenu->setLayout(ColumnLayout::create());

  m_addSongButton =
      CCMenuItemSpriteExtra::create(ButtonSprite::create(indexIndex.has_value() ? "Update" : "Add"),
                                    this, menu_selector(ANAddLocalPopup::addSong));
  m_addSongMenu = CCMenu::create();
  m_addSongMenu->setID("add-song-menu");
  m_addSongButton->setID("add-song-button");
  m_addSongMenu->addChild(this->m_addSongButton);
  m_addSongMenu->setPosition(m_mainLayer->getContentSize().width / 2, 10.f);
  m_addSongMenu->setAnchorPoint({0.5f, 0.0f});
  m_addSongMenu->setContentSize(m_addSongButton->getContentSize());
  m_addSongMenu->setLayout(ColumnLayout::create());

  auto layout = ColumnLayout::create();
  layout->setAutoScale(false);
  layout->setAxisReverse(true);
  layout->setAxisAlignment(AxisAlignment::End);

  m_mainLayer->addChild(this->m_infoMenu);
  m_mainLayer->addChild(this->m_addSongMenu);

  auto inputParent = CCNode::create();
  inputParent->setID("input-parent");
  auto songInput = TextInput::create(300.f, "Song name*", "bigFont.fnt");
  songInput->setID("song-name-input");
  songInput->setCommonFilter(CommonFilter::Any);
  songInput->getInputNode()->setLabelPlaceholderColor(ccColor3B{108, 153, 216});
  songInput->getInputNode()->setMaxLabelScale(0.7f);
  songInput->getInputNode()->setLabelPlaceholderScale(0.7f);
  songInput->setString(indexIndex.has_value() ? indexIndex.value()->m_name : "");
  m_songNameInput = songInput;

  auto artistInput = TextInput::create(300.f, "Artist name*", "bigFont.fnt");
  artistInput->setID("artist-name-input");
  artistInput->setCommonFilter(CommonFilter::Any);
  artistInput->getInputNode()->setLabelPlaceholderColor(ccColor3B{108, 153, 216});
  artistInput->getInputNode()->setMaxLabelScale(0.7f);
  artistInput->getInputNode()->setLabelPlaceholderScale(0.7f);
  artistInput->setString(indexIndex.has_value() ? indexIndex.value()->m_artist : "");
  m_artistNameInput = artistInput;

  auto startOffsetInput = TextInput::create(300.f, "Start offset (ms)", "bigFont.fnt");
  startOffsetInput->setID("start-offset-input");
  startOffsetInput->setCommonFilter(CommonFilter::Int);
  startOffsetInput->getInputNode()->setLabelPlaceholderColor(ccColor3B{108, 153, 216});
  startOffsetInput->getInputNode()->setMaxLabelScale(0.7f);
  startOffsetInput->getInputNode()->setLabelPlaceholderScale(0.7f);
  startOffsetInput->setString(
      indexIndex.has_value() ? std::to_string(indexIndex.value()->m_startOffsetMS) : "");
  m_startOffsetInput = startOffsetInput;

  auto youtubeLinkInput = TextInput::create(300.f, "Youtube Link", "bigFont.fnt");
  youtubeLinkInput->setID("youtube-link-input");
  youtubeLinkInput->setCommonFilter(CommonFilter::Any);
  youtubeLinkInput->getInputNode()->setLabelPlaceholderColor(ccColor3B{108, 153, 216});
  youtubeLinkInput->getInputNode()->setMaxLabelScale(0.7f);
  youtubeLinkInput->getInputNode()->setLabelPlaceholderScale(0.7f);
  youtubeLinkInput->setString(indexIndex.has_value() && indexIndex.value()->getSource() == "youtube"
                                  ? static_cast<ANYTSong *>(indexIndex.value().get())->m_ytId
                                  : "");
  m_youtubeLinkInput = youtubeLinkInput;

  uint32_t inputs = 3;
  uint32_t gaps = 2;
  float inputHeight = songInput->getContentSize().height;

  inputParent->addChild(songInput);
  inputParent->addChild(artistInput);
  inputParent->addChild(youtubeLinkInput);
  inputParent->addChild(startOffsetInput);

  auto layout2 = ColumnLayout::create();
  layout2->setAxisReverse(true);
  inputParent->setContentSize({250.f, inputs * inputHeight + gaps * layout->getGap()});
  inputParent->setAnchorPoint({0.5f, 1.0f});
  inputParent->setPosition(m_mainLayer->getContentSize().width / 2,
                           m_mainLayer->getContentSize().height - 40.f);
  inputParent->setLayout(layout2);
  m_mainLayer->addChild(inputParent);

  return true;
}

void ANAddLocalPopup::showInfo(CCObject *) {
  FLAlertLayer::create("Information",
                       "This will add a new song to Auto Nong locally.\nAfter adding the song "
                       "you'll have a button to edit or publish the song for other people to use.",
                       "OK")
      ->show();
  return;
}

void ANAddLocalPopup::addSong(CCObject *) {
  const auto songName = m_songNameInput->getString();
  const auto artistName = m_artistNameInput->getString();
  const auto startOffsetString = m_startOffsetInput->getString();
  const auto youtubeLink = m_youtubeLinkInput->getString();

  if (songName.empty() || artistName.empty() || youtubeLink.empty()) {
    FLAlertLayer::create("Error", "Please fill all the required fields.", "OK")->show();
    return;
  }

  const auto ytId = youtubeLinkToID(youtubeLink);

  if (!ytId.has_value()) {
    FLAlertLayer::create("Error", "Invalid Youtube link.", "OK")->show();
    return;
  }

  if (m_indexIndex.has_value()) {
    AutoNongManager::get()->removeSongFromLocalIndex(m_indexIndex.value().get());
  }

  auto song = ANYTSong(songName, artistName, "local", {m_parentPopup->m_songID},
                       startOffsetString.empty() ? 0 : std::stoi(startOffsetString), ytId.value());
  AutoNongManager::get()->addSongToLocalIndex(&song);
  m_parentPopup->updateCells();

  this->m_closeBtn->activate();
  // auto songName = m_songNameInput->getString();
  // auto artistName = m_artistNameInput->getString();
  // auto startOffset = m_startOffsetInput->getString();
  //
  // auto song = ANSong::create();
  // song->m_songName = songName;
  // song->m_artistName = artistName;
  // song->m_startOffset = startOffset.empty() ? 0 : std::stoi(startOffset);
  //
  // m_parentPopup->m_songCandidates.push_back(song);
  // m_parentPopup->updateCellsButtonsState();
  // this->removeFromParent();
}
