#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/CCTextInputNode.hpp>
#include <Geode/binding/Slider.hpp>
#include <Geode/binding/SliderThumb.hpp>
#include <Geode/loader/Setting.hpp>
#include <Geode/loader/SettingNode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/ui/ColorPickPopup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/string.hpp>
#include <Geode/loader/Mod.hpp>
#include <algorithm>
#include <matjson.hpp>

#include <vector>
#include "an_multi_string_setting.hpp"

using namespace geode::prelude;

// bool MultiStringSettingValue::load(matjson::Value const& json) {
//   strings.clear();
//   auto array = json.as_array();
//   for (auto const& elem : array) {
//     strings.push_back(elem.as_string());
//   }
//   return true;
// }
//
// bool MultiStringSettingValue::save(matjson::Value& json) const {
//   auto array = matjson::Array();
//   for (auto const& string : strings) {
//     array.push_back(string);
//   }
//   json = array;
//   return true;
// }
//
// void MultiStringSettingValue::setStrings(std::vector<std::string> strings) {
//   this->strings = strings;
// }
//
// std::vector<std::string> MultiStringSettingValue::getStrings() {
//   return strings;
// }
//
SettingNode* MultiStringSettingValue::createNode(float width) {
  return MultiStringSettingNode::create(this, width);
}

class MultiStringSettingPopup : public geode::Popup<std::vector<std::string>, std::function<void (std::vector<std::string>)>> {
protected:
    std::vector<std::string> m_localValue;
  std::function<void (std::vector<std::string>)> m_newStringsCallback;
    CCMenu* m_listMenu;
    ListView* m_list;
    bool setup(std::vector<std::string> localValue, std::function<void (std::vector<std::string>)> newStringsCallback) override {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        m_localValue = localValue;
        m_newStringsCallback = newStringsCallback;

        // convenience function provided by Popup 
        // for adding/setting a title to the popup
        // this->setTitle("Hi mom!");
        

        // auto label = CCLabelBMFont::create(value.c_str(), "bigFont.fnt");
        // label->setPosition(winSize / 2);
        // this->addChild(label);

        // auto activeSong = this->getActiveSong();
        // NongData songData = m_data[m_currentSongID];

        // float width = 100.f;

        // if (m_listView) {
        //     m_listView->removeFromParent();
        // }

        m_listMenu = CCMenu::create();
        m_listMenu->setPosition(0, 0);
        this->m_mainLayer->addChild(m_listMenu);

        createList();


        // auto listpos = list->getPosition();
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

    // for (int i = 0; i < localValue.size(); i++) {
    // }

        handleTouchPriority(this);
        return true;
    }

  void createList() {
    std::optional<float> previousListPosition = {};
    if (m_list != nullptr) {
      auto tableView = m_list->m_tableView;
      float viewHeight = tableView->getContentHeight();
      float entireListHeight = tableView->m_contentLayer->getContentHeight();
      // VV
      if (entireListHeight > viewHeight) {
        float mostPossibleMovedHeightBottom = entireListHeight - viewHeight; // The most the list can move vertically from the bottom
        float entireListYPosition = tableView->m_contentLayer->getPositionY();
        // VV
        float mostPossibleMovedHeightTop = -mostPossibleMovedHeightBottom - entireListYPosition; // Same but from the top
        previousListPosition = mostPossibleMovedHeightTop;
      }
    }
    m_listMenu->removeAllChildren();
    m_list = nullptr;

    auto cells = CCArray::create();
    auto size = this->m_mainLayer->getContentSize();

    for (int i = 0; i < m_localValue.size(); i++) {
      cells->addObject(MultiStringSettingCell::create(m_localValue[i], {size.width - 10.f, 40.f}, i == m_localValue.size() - 1, [this, i](const std::string &str) {
        m_localValue[i] = str;
        m_newStringsCallback(m_localValue);
      }, [this, i]() {
        if (i == m_localValue.size() - 1) {
          m_localValue.push_back("");
          m_newStringsCallback(m_localValue);
        } else {
          m_localValue.erase(m_localValue.begin() + i);
          m_newStringsCallback(m_localValue);
        }
        createList();
      }));
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
      newListPosition = -previousListPosition.value()-mostPossibleMovedHeightBottom;
      newListPosition = std::clamp(newListPosition.value(), -mostPossibleMovedHeightBottom, 0.f);
      if (entireListHeight < viewHeight) {
        newListPosition = viewHeight-entireListHeight;
      }
      // newListPosition = std::clamp(newListPosition, -mostPossibleMovedHeightBottom, 0.f);
      // newListPosition = -mostPossibleMovedHeightBottom;
    }
    if (newListPosition.has_value()) {
      list->m_tableView->m_contentLayer->setPosition(CCPoint{0, newListPosition.value()});
    }
    m_listMenu->addChild(list);
    m_list = list;
  }

public:
    static MultiStringSettingPopup* create(std::vector<std::string> localValue, std::function<void (std::vector<std::string>)> newStringsCallback) {
        auto ret = new MultiStringSettingPopup();
        if (ret && ret->initAnchored(420.f, 160.f, localValue, newStringsCallback)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

bool MultiStringSettingCell::init(std::string string, CCSize const& size, bool last, std::function<void (const std::string &)> callback, std::function<void ()> actionCallback) {
  // auto size = this->getParent()->getContentSize();
  if (!JBListCell::init(size)) return false;
  m_string = string;
  m_callback = callback;
  m_actionCallback = actionCallback;
  m_last = last;

  // m_parentNode = parentNode;

  this->setContentSize(size);

  auto menu = CCMenu::create();
  menu->setPosition(0, 0);
  this->addChild(menu);

  // auto label = CCLabelBMFont::create(string.c_str(), "bigFont.fnt");
  // label->setAnchorPoint({0.f, 0.5f});
  // label->setPosition(-size.width / 2 + 10.f, 0);
  // label->setScale(0.5f);
  // this->addChild(label);

  auto inputNode = TextInput::create(103.f, "Index url", "chatFont.fnt");
  inputNode->setScale(1.f);
  inputNode->setPosition(size.width/2 - 15.f, size.height/2);
  inputNode->setFilter("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~:/?#[]@!$&'()*+,;=");
  inputNode->setMaxCharCount(300);
  inputNode->setWidth(size.width - 60.f);
  // inputNode->setID(std::format("inp-{}", i));
  inputNode->setString(string, false);
  inputNode->setCallback([this](std::string const& str) {
    m_callback(str);
  });
  menu->addChild(inputNode);

  auto spr = m_last ? CCSprite::create("addIcon.png"_spr) : CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
  spr->setScale(0.75f);
  auto btn = CCMenuItemSpriteExtra::create(
      spr, this, menu_selector(MultiStringSettingCell::onAction)
  );
  btn->setPosition(size.width - 15.f, size.height/2);
  menu->addChild(btn);

  return true;
}

void MultiStringSettingCell::onAction(CCObject* target) {
  m_actionCallback();
}


MultiStringSettingCell* MultiStringSettingCell::create(std::string string, CCSize const& size, bool last, std::function<void (const std::string &)> callback, std::function<void ()> actionCallback) {
    auto ret = new MultiStringSettingCell();
    if (ret && ret->init(string, size, last, callback, actionCallback)) {
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

std::vector<std::string> MultiStringSettingValue::defaultIndexes = {
  "https://cdn.jsdelivr.net/gh/FlafyDev/auto-nong-indexes@dist/official.json.gz",
  "https://cdn.jsdelivr.net/gh/FlafyDev/auto-nong-indexes@dist/sfh-rooot.json.gz"
};

  bool MultiStringSettingNode::init(MultiStringSettingValue* value, float width) {
    if (!SettingNode::init(value))
        return false;
    this->value = value;
    for (std::string& str : value->getStrings()) {
        localValue.push_back(str);
    }

    // float height = 35.f + 40.f * 3;
    float height = 40.f;
    this->setContentSize({ width, height });


    auto menu = CCMenu::create();
    // menu->setPosition(width - 20.f, height - 45.f);
    menu->setPosition(0, 0);
    menu->setID("inputs-menu");

    m_label = CCLabelBMFont::create("Indexes", "bigFont.fnt");
    m_label->setAnchorPoint({0.f, 0.5f});
    m_label->setPosition(20.f, height/2);
    m_label->setScale(0.5f);
    menu->addChild(m_label);

    auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoSpr->setScale(.6f);
    auto infoBtn = CCMenuItemSpriteExtra::create(
        infoSpr, this, menu_selector(MultiStringSettingNode::onDesc)
    );
    infoBtn->setPosition(m_label->getScaledContentSize().width+40.f, height/2);
    menu->addChild(infoBtn);

    auto resetSpr = CCSprite::createWithSpriteFrameName("geode.loader/reset-gold.png");
    resetSpr->setScale(.5f);
    m_resetBtn = CCMenuItemSpriteExtra::create(
        resetSpr, this, menu_selector(MultiStringSettingNode::onReset)
    );
    m_resetBtn->setPosition(m_label->getScaledContentSize().width + 40.f + 20.f, height/2);
    menu->addChild(m_resetBtn);

    auto viewSpr = ButtonSprite::create("View");
    viewSpr->setScale(0.72f);
    auto viewBtn = CCMenuItemSpriteExtra::create(
        viewSpr, this, menu_selector(MultiStringSettingNode::onView)
    );
    viewBtn->setPosition(width - 40.f, height - 20.f);
    menu->addChild(viewBtn);

    this->addChild(menu);
    handleTouchPriority(this);

    updateVisuals();



    // inputNode = TextInput::create(103.f, "...", "chatFont.fnt");
    // inputNode->setScale(1.f);
    // inputNode->setPosition(-width / 2 + 10.f, -40.f);
    // inputNode->setString("https://cdn.jsdelivr.net/gh/FlafyDev/auto-nong-indexes@dist/example.json.gz", false);
    // inputNode->setFilter("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~:/?#[]@!$&'()*+,;=");
    // inputNode->setMaxCharCount(300);
    // inputNode->setWidth(width - 60.f);
    // menu->addChild(inputNode);
    //
    // spr = CCSprite::create("addIcon.png"_spr);
    // btn = CCMenuItemSpriteExtra::create(
    //     spr, this, nullptr
    // );
    // spr->setScale(0.72f);
    // btn->setPosition(-5.f, -40.f);
    // menu->addChild(btn);




      // createList();
      
      // You may change the height to anything, but make sure to call 
      // setContentSize!

      // auto arrowRight = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
      // arrowRight->setScale(0.3f);
      // auto buttonRight = CCMenuItemSpriteExtra::create(arrowRight, this, nullptr);
      // buttonRight->setPosition(-10.f, 0);
      // menu->addChild(buttonRight);
      //
      // auto arrowLeft = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
      // arrowLeft->setScale(0.3f);
      // arrowLeft->setFlipX(true);
      // auto buttonLeft = CCMenuItemSpriteExtra::create(arrowLeft, this, nullptr);
      // buttonLeft->setPosition(-93.f, 0);
      // menu->addChild(buttonLeft);

      return true;
  }

  void MultiStringSettingNode::onView(CCObject*) {
    auto popup = MultiStringSettingPopup::create(localValue, [this](std::vector<std::string> newStrings) {
      localValue = newStrings;
      updateVisuals();
    });
    popup->m_noElasticity = true;
    popup->show();

          // auto layer = MultiStringSettingPopup::create("Hello, world!");
          // layer->m_noElasticity = true;
          // // based robtroll
          // layer->setZOrder(106);
          // layer->show();
    //   FLAlertLayer::create(
    //     "Title",    // title
    //     "Hi mom!",  // content
    //     "OK"        // button
    // )->show();
  }

  void MultiStringSettingNode::onReset(CCObject*) {
    resetToDefault();
  }

  void MultiStringSettingNode::onDesc(CCObject*) {
    FLAlertLayer::create(
      m_label->getString(),
      "AutoNong indexes.\nFrom indexes the mod knows where to fetch the songs from.",
      "OK"
    )->show();
  }

  void MultiStringSettingNode::updateVisuals() {
    m_resetBtn->setVisible(hasNonDefaultValue());
    m_label->setColor(hasUncommittedChanges() ? cc3x(0x1d0) : cc3x(0xfff));
    this->dispatchChanged();
  }

  // void MultiStringSettingNode::createList() {
  //   float height = 40.f;
  //   this->setContentSize({ width, height });
  //
  //
  //   if (menu != nullptr) {
  //     menu->removeFromParent();
  //   }
  //
  //   // menu = CCMenu::create();
  //   // menu->setPosition(width - 20.f, height - 12.5f);
  //   // menu->setID("button-menu");
  //   // this->addChild(menu);
  //
  //   menu = CCMenu::create();
  //   menu->setPosition(width - 20.f, height - 45.f);
  //   menu->setID("inputs-menu");
  //
  //   auto label = CCLabelBMFont::create("Indexes", "bigFont.fnt");
  //   label->setAnchorPoint({0.f, 0.5f});
  //   label->setPosition(-width+20.f+20.f, 32.5f);
  //   label->setScale(0.5f);
  //   menu->addChild(label);
  //
  // 
  //   auto spr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
  //   spr->setScale(.6f);
  //   auto btn = CCMenuItemSpriteExtra::create(
  //       spr, this, nullptr
  //   );
  //   btn->setPosition(-width+20.f+20.f+label->getContentWidth()+40.f, );
  //   menu->addChild(btn);
  //
  //   for (int i = 0; i < localValue.size(); i++) {
  //     auto inputNode = TextInput::create(103.f, "Index url", "chatFont.fnt");
  //     inputNode->setScale(1.f);
  //     inputNode->setPosition(-width / 2 + 10.f, -40.f * i);
  //     inputNode->setString(localValue[i], false);
  //     inputNode->setFilter("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~:/?#[]@!$&'()*+,;=");
  //     inputNode->setMaxCharCount(300);
  //     inputNode->setWidth(width - 60.f);
  //     inputNode->setID(std::format("inp-{}", i));
  //     inputNode->setCallback([this, i](std::string const& str) {
  //       localValue[i] = str;
  //       this->dispatchChanged();
  //     });
  //     menu->addChild(inputNode);
  //
  //     auto spr = i == localValue.size() - 1 ? CCSprite::create("addIcon.png"_spr) : CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
  //     spr->setScale(0.75f);
  //     auto btn = CCMenuItemSpriteExtra::create(
  //         spr, this, menu_selector(MultiStringSettingNode::onBtnString)
  //     );
  //     btn->setID(std::format("btn-{}", i));
  //     btn->setPosition(-5.f, -40.f * i);
  //     menu->addChild(btn);
  //   }
  //
  //
  //
  //   // inputNode = TextInput::create(103.f, "...", "chatFont.fnt");
  //   // inputNode->setScale(1.f);
  //   // inputNode->setPosition(-width / 2 + 10.f, -40.f);
  //   // inputNode->setString("https://cdn.jsdelivr.net/gh/FlafyDev/auto-nong-indexes@dist/example.json.gz", false);
  //   // inputNode->setFilter("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~:/?#[]@!$&'()*+,;=");
  //   // inputNode->setMaxCharCount(300);
  //   // inputNode->setWidth(width - 60.f);
  //   // menu->addChild(inputNode);
  //   //
  //   // spr = CCSprite::create("addIcon.png"_spr);
  //   // btn = CCMenuItemSpriteExtra::create(
  //   //     spr, this, nullptr
  //   // );
  //   // spr->setScale(0.72f);
  //   // btn->setPosition(-5.f, -40.f);
  //   // menu->addChild(btn);
  //
  //   this->addChild(menu);
  //
  // }

  // void MultiStringSettingNode::onBtnString(CCObject* target) {
  //   std::string id = static_cast<CCNode*>(target)->getID();
  //   int i = std::stoi(id.substr(4));
  //   if (i == localValue.size() - 1) {
  //     localValue.push_back("");
  //     createList();
  //   } else {
  //     localValue.erase(localValue.begin() + i);
  //     createList();
  //   }
  //   this->dispatchChanged();
  // }

  // void MultiStringSettingNode::updateStatus() {
  //   if (hasUncommittedChanges())
  //
  // }

  // void MultiStringSettingNode::onInputCallback(CCObject* target) {
  //   std::string id = static_cast<CCNode*>(target)->getID();
  //   int i = std::stoi(id.substr(4));
  //   localValue[i] = static_cast<TextInput*>(target)->getString();
  // }

  // Whenever the user interacts with your controls, you should call 
  // this->dispatchChanged()

  // When the user wants to save this setting value, this function is 
  // called - this is where you should actually set the value of your 
  // setting
  void MultiStringSettingNode::commit() {
      this->value->setStrings(localValue);
      updateVisuals();
      // Set the actual value

      // Let the UI know you have committed the value
      this->dispatchCommitted();
  }

  bool MultiStringSettingNode::hasUncommittedChanges() {
    if (localValue.size() != value->getStrings().size()) return true;
    for (int i = 0; i < localValue.size(); i++) {
      if (localValue[i] != value->getStrings()[i]) return true;
    }
    return false;
  }

  bool MultiStringSettingNode::hasNonDefaultValue() {
    if (localValue.size() != MultiStringSettingValue::defaultIndexes.size()) return true;
    for (int i = 0; i < localValue.size(); i++) {
      if (localValue[i] != MultiStringSettingValue::defaultIndexes[i]) return true;
    }
    return false;
  }

  // Geode calls this to reset the setting's value back to default
  void MultiStringSettingNode::resetToDefault() {
    localValue.clear();
    for (std::string& str : MultiStringSettingValue::defaultIndexes) {
      localValue.push_back(str);
    }
    updateVisuals();
  }

  MultiStringSettingNode* MultiStringSettingNode::create(MultiStringSettingValue* value, float width) {
      auto ret = new MultiStringSettingNode();
      if (ret && ret->init(value, width)) {
          ret->autorelease();
          return ret;
      }
      CC_SAFE_DELETE(ret);
      return nullptr;
  }
// };

// $on_mod(Loaded) {
//     Mod::get()->addCustomSetting<MultiStringSettingValue>("indexes", defaultIndexes);
//     
//     // or, alternatively:
//     // Mod::get()->registerCustomSetting(
//     //     "my-setting",
//     //     std::make_unique<MySettingValue>("my-setting", Mod::get()->getID(), ...)
//     // );
// }

// "name": "Indexes",
// "description": "AutoNong indexes separated by ';'.",
// "type": "string",
// "default": "https://cdn.jsdelivr.net/gh/FlafyDev/auto-nong-indexes@dist/example.json.gz"
