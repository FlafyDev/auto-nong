#pragma once

#include <Geode/Geode.hpp>
#include <fleym.nongd/include/jukebox.hpp>

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
#include "../types/an_song.hpp"

#include <sstream>
#include "Geode/loader/Log.hpp"

using namespace geode::prelude;

class ANDropdownLayer : public Popup<int, std::vector<std::shared_ptr<ANSong>>, CustomSongWidget*, int, int> {
protected:
    // std::map<int, NongData> m_data;
    // std::vector<int> m_songIDS;
    // int m_currentSongID = -1;
    // int m_defaultSongID;
    int m_songID;
    std::vector<std::shared_ptr<ANSong>> m_songCandidates;
    int m_popupNumber;
    int m_totalPopups;
    ListView* m_listView = nullptr;
    CCSize m_cellSize = {
            320.f,
            60.f
        };
    // NongListType m_currentListType = NongListType::Single;

    // CCMenuItemSpriteExtra* m_downloadBtn = nullptr;
    // CCMenuItemSpriteExtra* m_addBtn = nullptr;
    // CCMenuItemSpriteExtra* m_deleteBtn = nullptr;
    // CCMenuItemSpriteExtra* m_backBtn = nullptr;

    // bool m_fetching = false;

    bool setup(int songId, std::vector<std::shared_ptr<ANSong>> songCandidates, CustomSongWidget* parent, int popupNumber, int totalPopups) override;
    // void createList();
    // SongInfo getActiveSong();
    // CCSize getCellSize() const;
    // void deleteAllNongs(CCObject*);
    // void fetchSongFileHub(CCObject*);
    // void onSettings(CCObject*);
    // void openAddPopup(CCObject*);
public:
    Ref<CustomSongWidget> m_parentWidget;
    // void onSelectSong(int songID);
    // void onBack(CCObject*);
    // void onDiscord(CCObject*);
    // int getSongID();
    // void setActiveSong(SongInfo const& song);
    // void deleteSong(SongInfo const& song);
    // void addSong(SongInfo const& song);
    // void updateParentWidget(SongInfo const& song);
    // void refreshList();

    static ANDropdownLayer* create(int songId, std::vector<std::shared_ptr<ANSong>> songCandidates, CustomSongWidget* parent, int popupNumber, int totalPopups) {
        auto ret = new ANDropdownLayer;
        if (ret && ret->initAnchored(420.f, 280.f, songId, songCandidates, parent, popupNumber, totalPopups, "GJ_square02.png")) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

