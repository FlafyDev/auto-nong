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

using namespace geode::prelude;

class ANDropdownLayer : public Popup<int, std::vector<std::shared_ptr<ANSong>>, CustomSongWidget*, int, int> {
protected:
    int m_songID;
    std::vector<std::shared_ptr<ANSong>> m_songCandidates;
    int m_popupNumber;
    int m_totalPopups;
    ListView* m_listView = nullptr;
    CCSize m_cellSize = {
            320.f,
            60.f
        };

    bool setup(int songId, std::vector<std::shared_ptr<ANSong>> songCandidates, CustomSongWidget* parent, int popupNumber, int totalPopups) override;
public:
    Ref<CustomSongWidget> m_parentWidget;

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

