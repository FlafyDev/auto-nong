#include "multi_string_setting.hpp"
#include <algorithm>
#include <vector>
#include "../types/serializable_vector.hpp"

using namespace geode::prelude;

SettingNode *MultiStringSettingValue::createNode(float width) {
  return MultiStringSettingNode::create(this, width);
}

class MultiStringSettingPopup
    : public geode::Popup<std::vector<std::string>, std::function<void(std::vector<std::string>)>> {
protected:
  std::vector<std::string> m_localValue;
  std::function<void(std::vector<std::string>)> m_newStringsCallback;
  CCMenu *m_listMenu;
  ListView *m_list;
  bool setup(std::vector<std::string> localValue,
             std::function<void(std::vector<std::string>)> newStringsCallback) override {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    m_localValue = localValue;
    m_newStringsCallback = newStringsCallback;

    m_listMenu = CCMenu::create();
    m_listMenu->setPosition(0, 0);
    this->m_mainLayer->addChild(m_listMenu);

    createList();

    handleTouchPriority(this);
    return true;
  }

  void onActionButton(CCObject* target) {
    auto id = static_cast<CCNode*>(target)->getID();
    auto index = std::stoi(id.substr(4));
    if (index == m_localValue.size() - 1) {
      m_localValue.push_back("");
    } else {
      m_localValue.erase(m_localValue.begin() + index);
    }
    m_newStringsCallback(m_localValue);
    createList();
  }


  void createList() {
    std::optional<float> previousListPosition = {};
    if (m_list != nullptr) {
      auto tableView = m_list->m_tableView;
      float viewHeight = tableView->getContentHeight();
      float entireListHeight = tableView->m_contentLayer->getContentHeight();
      // VV
      if (entireListHeight > viewHeight) {
        float mostPossibleMovedHeightBottom =
            entireListHeight - viewHeight; // The most the list can move vertically from the bottom
        float entireListYPosition = tableView->m_contentLayer->getPositionY();
        // VV
        float mostPossibleMovedHeightTop =
            -mostPossibleMovedHeightBottom - entireListYPosition; // Same but from the top
        previousListPosition = mostPossibleMovedHeightTop;
      }
    }
    m_listMenu->removeAllChildren();
    m_list = nullptr;

    auto cells = CCArray::create();
    auto size = this->m_mainLayer->getContentSize();

    for (int i = 0; i < m_localValue.size(); i++) {
        auto last = i == m_localValue.size() - 1;
        auto size2 = CCSize {size.width - 10.f, 40.f};

        auto menu = CCMenu::create();
        menu->setPosition(0, 0);
        // menu->setContentSize(size2);


        auto inputNode = TextInput::create(103.f, "Index url", "chatFont.fnt");
        inputNode->setScale(1.f);
        inputNode->setPosition(size2.width / 2 - 15.f, size2.height / 2);
        inputNode->setFilter(
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~:/?#[]@!$&'()*+,;=");
        inputNode->setMaxCharCount(300);
        inputNode->setWidth(size2.width - 60.f);
        inputNode->setString(m_localValue[i], false);
        inputNode->setCallback([this, i](std::string const &str) {
          m_localValue[i] = str;
          m_newStringsCallback(m_localValue);
        });
        menu->addChild(inputNode);

        auto spr = last ? CCSprite::create("addIcon.png"_spr)
                          : CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
        spr->setScale(0.75f);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MultiStringSettingPopup::onActionButton));
        btn->setID(fmt::format("btn-{}", i));
        btn->setPosition(size2.width - 15.f, size2.height / 2);
        menu->addChild(btn);
        cells->addObject(menu);
    }

    auto list = ListView::create(cells, 40.f, size.width - 8.f, size.height - 16.f);
    list->setPosition(4.f, 10.f);

    std::optional<float> newListPosition = {};
    if (previousListPosition.has_value()) {
      auto tableView = list->m_tableView;
      float viewHeight = tableView->getContentHeight();
      float entireListHeight = tableView->m_contentLayer->getContentHeight();
      // VV
      float mostPossibleMovedHeightBottom = entireListHeight - viewHeight;
      // VV
      newListPosition = -previousListPosition.value() - mostPossibleMovedHeightBottom;
      newListPosition = std::clamp(newListPosition.value(), -mostPossibleMovedHeightBottom, 0.f);
      if (entireListHeight < viewHeight) {
        newListPosition = viewHeight - entireListHeight;
      }
    }
    if (newListPosition.has_value()) {
      list->m_tableView->m_contentLayer->setPosition(CCPoint{0, newListPosition.value()});
    }
    m_listMenu->addChild(list);
    m_list = list;
  }

public:
  static MultiStringSettingPopup *
  create(std::vector<std::string> localValue,
         std::function<void(std::vector<std::string>)> newStringsCallback) {
    auto ret = new MultiStringSettingPopup();
    if (ret && ret->initAnchored(420.f, 160.f, localValue, newStringsCallback)) {
      ret->autorelease();
      return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
  }
};


bool MultiStringSettingNode::init(MultiStringSettingValue *value, float width) {
  if (!SettingNode::init(value))
    return false;
  this->m_value = value;
  for (std::string &str : value->getStrings()) {
    m_localValue.push_back(str);
  }

  float height = 40.f;
  this->setContentSize({width, height});

  auto menu = CCMenu::create();
  menu->setPosition(0, 0);
  menu->setID("inputs-menu");

  // No way to get the JSON without hardcoding the setting ID...
  auto settingJson = Mod::get()->getSettingDefinition("indexes")->get<CustomSetting>()->json;
  m_defaultValue = settingJson->get<std::vector<std::string>>("default");
  m_name = settingJson->get<std::string>("name");
  m_description = settingJson->get<std::string>("description");

  m_label = CCLabelBMFont::create(m_name.c_str(), "bigFont.fnt");
  m_label->setAnchorPoint({0.f, 0.5f});
  m_label->setPosition(20.f, height / 2);
  m_label->setScale(0.5f);
  menu->addChild(m_label);

  auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
  infoSpr->setScale(.6f);
  auto infoBtn =
      CCMenuItemSpriteExtra::create(infoSpr, this, menu_selector(MultiStringSettingNode::onDesc));
  infoBtn->setPosition(m_label->getScaledContentSize().width + 40.f, height / 2);
  menu->addChild(infoBtn);

  auto resetSpr = CCSprite::createWithSpriteFrameName("geode.loader/reset-gold.png");
  resetSpr->setScale(.5f);
  m_resetBtn =
      CCMenuItemSpriteExtra::create(resetSpr, this, menu_selector(MultiStringSettingNode::onReset));
  m_resetBtn->setPosition(m_label->getScaledContentSize().width + 40.f + 20.f, height / 2);
  menu->addChild(m_resetBtn);

  auto viewSpr = ButtonSprite::create("View");
  viewSpr->setScale(0.72f);
  auto viewBtn =
      CCMenuItemSpriteExtra::create(viewSpr, this, menu_selector(MultiStringSettingNode::onView));
  viewBtn->setPosition(width - 40.f, height - 20.f);
  menu->addChild(viewBtn);

  this->addChild(menu);
  handleTouchPriority(this);

  updateVisuals();

  return true;
}

void MultiStringSettingNode::onView(CCObject *) {
  auto popup =
      MultiStringSettingPopup::create(m_localValue, [this](std::vector<std::string> newStrings) {
        m_localValue = newStrings;
        updateVisuals();
      });
  popup->m_noElasticity = true;
  popup->show();
}

void MultiStringSettingNode::onReset(CCObject *) { resetToDefault(); }

void MultiStringSettingNode::onDesc(CCObject *) {
  FLAlertLayer::create(m_name.c_str(), m_description.c_str(), "OK")->show();
}

void MultiStringSettingNode::updateVisuals() {
  m_resetBtn->setVisible(hasNonDefaultValue());
  m_label->setColor(hasUncommittedChanges() ? cc3x(0x1d0) : cc3x(0xfff));
  this->dispatchChanged();
}

void MultiStringSettingNode::commit() {
  this->m_value->setStrings(m_localValue);
  updateVisuals();
  this->dispatchCommitted();
}

bool MultiStringSettingNode::hasUncommittedChanges() {
  if (m_localValue.size() != m_value->getStrings().size())
    return true;
  for (int i = 0; i < m_localValue.size(); i++) {
    if (m_localValue[i] != m_value->getStrings()[i])
      return true;
  }
  return false;
}

bool MultiStringSettingNode::hasNonDefaultValue() {
  if (m_localValue.size() != m_defaultValue.size()) return true;
  for (int i = 0; i < m_localValue.size(); i++) {
    if (m_localValue[i] != m_defaultValue[i]) return true;
  }
  return false;
}

void MultiStringSettingNode::resetToDefault() {
  m_localValue.clear();
  for (std::string& str : m_defaultValue) {
    m_localValue.push_back(str);
  }
  updateVisuals();
}

MultiStringSettingNode *MultiStringSettingNode::create(MultiStringSettingValue *value,
                                                       float width) {
  auto ret = new MultiStringSettingNode();
  if (ret && ret->init(value, width)) {
    ret->autorelease();
    return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}
