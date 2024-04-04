#include "../includes/geode.hpp"
#include "../managers/auto_nong_manager.hpp"

struct ANLevelInfoLayer : geode::Modify<ANLevelInfoLayer, LevelInfoLayer> {
  bool init(GJGameLevel *level, bool p1) {
    if (!LevelInfoLayer::init(level, p1)) {
      return false;
    }
    AutoNongManager::get()->setCurrentLevelID(level->m_levelID);
    return true;
  }
};
