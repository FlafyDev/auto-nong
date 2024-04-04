#include "an_dropdown_layer.hpp"
/**
 * Include the Geode headers.
 */
#include <Geode/Geode.hpp>
#include <fleym.nongd/include/jukebox.hpp>

/**
 * Brings cocos2d and all Geode namespaces to the current scope.
 */
using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/CustomSongWidget.hpp>
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
#include <Geode/ui/Popup.hpp>
#include "an_song_cell.hpp"

#include <sstream>
#include "Geode/loader/Log.hpp"

namespace fs = std::filesystem;

bool ANDropdownLayer::setup(int songId, std::vector<std::shared_ptr<ANSong>> songCandidates, CustomSongWidget* parent, int popupNumber, int totalPopups) {
    m_songID = songId;
    m_songCandidates = songCandidates;
    m_parentWidget = parent;
    m_popupNumber = popupNumber;
    m_totalPopups = totalPopups;

    auto contentSize = m_mainLayer->getContentSize();

    auto manifestLabel = CCLabelBMFont::create(std::format("{} / {}", popupNumber, totalPopups).c_str(), "chatFont.fnt");
    manifestLabel->setPosition(contentSize.width / 2, 13.f);
    manifestLabel->limitLabelWidth(140.f, 0.6f, 0.1f);
    manifestLabel->setColor(cc3x(0xc2c2c2));
    manifestLabel->setID("manifest-label");
    m_mainLayer->addChild(manifestLabel);

    auto titleLabel = CCLabelBMFont::create(std::format("Replace song for {}", songId).c_str(), "goldFont.fnt");
    titleLabel->setPosition(contentSize.width / 2, contentSize.height - 22.f);
    titleLabel->limitLabelWidth(contentSize.width - 40.f, 1.5f, 0.1f);
    // titleLabel->setColor(cc3x(0xFFFFFF));
    titleLabel->setID("title-label");
    m_mainLayer->addChild(titleLabel);

    auto songs = CCArray::create();
    // auto activeSong = this->getActiveSong();
    // NongData songData = m_data[m_currentSongID];


    for (auto song : m_songCandidates) {
      songs->addObject(ANSongCell::create(songId, song.get(), this, m_cellSize));
    }
    // if (m_listView) {
    //     m_listView->removeFromParent();
    // }

    auto list = ListView::create(songs, m_cellSize.height, m_cellSize.width, 200.f);
    m_mainLayer->addChild(list);
    list->setPosition(contentSize.width / 2, contentSize.height / 2 - 15.f);
    list->ignoreAnchorPointForPosition(false);

    auto listpos = list->getPosition();
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

    // for (auto const& id : m_songIDS) {
    //     auto result = NongManager::get()->getNongs(id);
    //     if (!result.has_value()) {
    //         NongManager::get()->createDefault(id);
    //         NongManager::get()->writeJson();
    //         result = NongManager::get()->getNongs(id);
    //     }
    //     auto value = result.value();
    //     m_data[id] = value;
    // }
    // if (ids.size() == 1) {
    //     m_currentListType = NongListType::Single;
    //     m_currentSongID = ids[0];
    // } else {
    //     m_currentListType = NongListType::Multiple;
    // }
    // auto contentSize = m_mainLayer->getContentSize();
    //
    // int manifest = NongManager::get()->getCurrentManifestVersion();
    // int count = NongManager::get()->getStoredIDCount();
    // std::stringstream ss;
    // ss << "Manifest v" << manifest << ", storing " << count << " unique song IDs.";
    //
    // auto manifestLabel = CCLabelBMFont::create(ss.str().c_str(), "chatFont.fnt");
    // manifestLabel->setPosition(contentSize.width / 2, 12.f);
    // manifestLabel->limitLabelWidth(140.f, 0.9f, 0.1f);
    // manifestLabel->setColor(cc3x(0xc2c2c2));
    // manifestLabel->setID("manifest-label");
    // m_mainLayer->addChild(manifestLabel);

    // auto bigMenu = CCMenu::create();
    // bigMenu->setID("big-menu");
    // bigMenu->ignoreAnchorPointForPosition(false);
    // auto discordSpr = CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png");
    // auto discordBtn = CCMenuItemSpriteExtra::create(
    //     discordSpr,
    //     this,
    //     menu_selector(NongDropdownLayer::onDiscord)
    // );
    // discordBtn->setID("discord-button");
    // CCPoint position = discordBtn->getScaledContentSize() / 2;
    // position += { 5.f, 5.f };
    // bigMenu->addChildAtPosition(discordBtn, Anchor::BottomLeft, position);
    // this->addChild(bigMenu);

    // auto spr = CCSprite::createWithSpriteFrameName("GJ_downloadBtn_001.png");
    // spr->setScale(0.7f);
    // auto menu = CCMenu::create();
    // menu->setID("bottom-right-menu");
    // auto backMenu = CCMenu::create();
    // backMenu->setID("back-menu");
    // auto downloadBtn = CCMenuItemSpriteExtra::create(
    //     spr,
    //     this,
    //     menu_selector(NongDropdownLayer::fetchSongFileHub)
    // );
    // m_downloadBtn = downloadBtn;
    // spr = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
    // spr->setScale(0.7f);
    // auto addBtn = CCMenuItemSpriteExtra::create(
    //     spr,
    //     this,
    //     menu_selector(NongDropdownLayer::openAddPopup)
    // );
    // m_addBtn = addBtn;
    // spr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    // spr->setScale(0.7f);
    // auto removeBtn = CCMenuItemSpriteExtra::create(
    //     spr,
    //     this,
    //     menu_selector(NongDropdownLayer::deleteAllNongs)
    // );
    // m_deleteBtn = removeBtn;
    // spr = CCSprite::createWithSpriteFrameName("backArrowPlain_01_001.png");
    // auto backBtn = CCMenuItemSpriteExtra::create(
    //     spr,
    //     this,
    //     menu_selector(NongDropdownLayer::onBack)
    // );
    // m_backBtn = backBtn;
    // backBtn->setPosition(ccp(-370.f, 100.f));
    //
    // if (m_currentListType == NongListType::Multiple) {
    //     m_addBtn->setVisible(false);
    //     m_deleteBtn->setVisible(false);
    //     m_downloadBtn->setVisible(false);
    //     m_backBtn->setVisible(false);
    // }
    // if (m_data.size() == 1) {
    //     m_backBtn->setVisible(false);
    // }
    // menu->addChild(addBtn);
    // menu->addChild(downloadBtn);
    // menu->addChild(removeBtn);
    // auto layout = ColumnLayout::create();
    // layout->setAxisAlignment(AxisAlignment::Start);
    // menu->setContentSize({addBtn->getScaledContentSize().width, 200.f});
    // menu->setAnchorPoint(ccp(1.0f, 0.0f));
    // menu->setLayout(layout);
    // menu->setPosition(contentSize.width - 7.f, 7.f);
    // menu->setZOrder(2);
    // m_mainLayer->addChild(menu);
    //
    // backMenu->addChild(backBtn);
    // backMenu->setLayout(ColumnLayout::create());
    // backMenu->setPosition(12.f, contentSize.height / 2);
    // backMenu->setAnchorPoint({ 0.0f, 0.5f });
    // backMenu->setContentSize({backMenu->getContentSize().width, 240.f});
    // backMenu->updateLayout();
    // m_mainLayer->addChild(backMenu);
    //
    // menu = CCMenu::create();
    // menu->setID("settings-menu");
    // // auto sprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    // // sprite->setScale(0.8f);
    // // auto settingsButton = CCMenuItemSpriteExtra::create(
    // //     sprite,
    // //     this,
    // //     menu_selector(NongDropdownLayer::onSettings)
    // // );
    // // settingsButton->setID("settings-button");
    // // menu->addChild(settingsButton);
    // // menu->setPosition(contentSize.width - 30.f, contentSize.height - 10.f);
    // // menu->setAnchorPoint({0.5f, 1.0f});
    // // menu->setContentSize(settingsButton->getScaledContentSize());
    // auto settingsLayout = ColumnLayout::create();
    // settingsLayout->setAxisAlignment(AxisAlignment::End);
    // settingsLayout->setAxisReverse(true);
    // menu->setLayout(settingsLayout);
    // m_mainLayer->addChild(menu);

    // this->createList();
    // auto listpos = m_listView->getPosition();
    // auto leftspr = CCSprite::createWithSpriteFrameName("GJ_commentSide2_001.png");
    // leftspr->setPosition(ccp(listpos.x - 162.f, listpos.y));
    // leftspr->setScaleY(6.8f);
    // leftspr->setZOrder(1);
    // m_mainLayer->addChild(leftspr);
    // auto rightspr = CCSprite::createWithSpriteFrameName("GJ_commentSide2_001.png");
    // rightspr->setPosition(ccp(listpos.x + 162.f, listpos.y));
    // rightspr->setScaleY(6.8f);
    // rightspr->setFlipX(true);
    // rightspr->setZOrder(1);
    // m_mainLayer->addChild(rightspr);
    // auto bottomspr = CCSprite::createWithSpriteFrameName("GJ_commentTop2_001.png");
    // bottomspr->setPosition(ccp(listpos.x, listpos.y - 95.f));
    // bottomspr->setFlipY(true);
    // bottomspr->setScaleX(0.934f);
    // bottomspr->setZOrder(1);
    // m_mainLayer->addChild(bottomspr);
    // auto topspr = CCSprite::createWithSpriteFrameName("GJ_commentTop2_001.png");
    // topspr->setPosition(ccp(listpos.x, listpos.y + 95.f));
    // topspr->setScaleX(0.934f);
    // topspr->setZOrder(1);
    // m_mainLayer->addChild(topspr);
    // auto title = CCSprite::create("JB_ListLogo.png"_spr);
    // title->setPosition(ccp(contentSize.width / 2, contentSize.height - 10.f));
    // title->setScale(0.75f);
    // m_mainLayer->addChild(title);
    handleTouchPriority(this);
    return true;
}

