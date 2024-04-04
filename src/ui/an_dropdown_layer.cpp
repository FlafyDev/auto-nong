#include "an_dropdown_layer.hpp"
#include <Geode/Geode.hpp>
#include <fleym.nongd/include/jukebox.hpp>

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
    titleLabel->setID("title-label");
    m_mainLayer->addChild(titleLabel);

    auto songs = CCArray::create();

    for (auto song : m_songCandidates) {
      songs->addObject(ANSongCell::create(songId, song.get(), this, m_cellSize));
    }

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

    handleTouchPriority(this);
    return true;
}

