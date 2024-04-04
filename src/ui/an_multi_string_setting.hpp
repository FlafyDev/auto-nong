#pragma once 

#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/CCTextInputNode.hpp>
#include <Geode/binding/Slider.hpp>
#include <Geode/binding/SliderThumb.hpp>
#include <Geode/loader/Setting.hpp>
#include <Geode/loader/SettingNode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/ui/ColorPickPopup.hpp>
#include <Geode/ui/InputNode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/string.hpp>
#include <Geode/loader/Mod.hpp>
#include <matjson.hpp>
#include "list_cell.hpp"
#include <vector>

using namespace geode::prelude;

struct MultiStringSettingStruct {
  std::vector<std::string> m_strings;
};

class MultiStringSettingValue;

class MultiStringSettingValue : public SettingValue {
protected:
    std::vector<std::string> m_strings;
public:
    MultiStringSettingValue(std::string const& key, std::string const& modID, std::vector<std::string> strings)
      : SettingValue(key, modID), m_strings(strings) {}

    bool load(matjson::Value const& json) override {
      m_strings.clear();
      auto array = json.as_array();
      for (auto const& elem : array) {
        m_strings.push_back(elem.as_string());
      }
      return true;
    }

    bool save(matjson::Value& json) const override {
      auto array = matjson::Array();
      for (auto const& string : m_strings) {
        array.push_back(string);
      }
      json = array;
      return true;
    }

    SettingNode* createNode(float width) override;

    void setStrings(std::vector<std::string> strings) {
      this->m_strings = strings;
      this->valueChanged();
    }
    
    std::vector<std::string> getStrings() const {
      return this->m_strings;
    }
};

template<>
struct SettingValueSetter<MultiStringSettingStruct> {
    static MultiStringSettingStruct get(SettingValue* setting) {
      return MultiStringSettingStruct { static_cast<MultiStringSettingValue*>(setting)->getStrings() };
    };
    static void set(MultiStringSettingValue* setting, MultiStringSettingStruct const& value) {
      setting->setStrings(value.m_strings);
    };
};


class MultiStringSettingNode : public SettingNode {
protected:
    MultiStringSettingValue* m_value;
    CCMenuItemSpriteExtra* m_resetBtn;
    CCLabelBMFont* m_label;
    std::vector<std::string> m_localValue;
    std::string m_name;
    std::string m_description;

    bool init(MultiStringSettingValue* value, float width);
public:
    void updateVisuals();
    void onView(CCObject*);
    void onReset(CCObject*);
    void onDesc(CCObject*);
    void commit() override;
    bool hasUncommittedChanges() override;
    bool hasNonDefaultValue() override;
    void resetToDefault() override;
    static MultiStringSettingNode* create(MultiStringSettingValue* value, float width);
};

class MultiStringSettingCell : public JBListCell {
protected:
    std::string m_string;
    bool m_last;
    std::function<void (const std::string &)> m_callback;
    std::function<void ()> m_actionCallback;

    bool init(std::string string, CCSize const& size, bool last, std::function<void (const std::string &)> callback, std::function<void ()> actionCallback);
public:
    void onAction(CCObject*);
    static MultiStringSettingCell* create(std::string string, CCSize const& size, bool last, std::function<void (const std::string &)> callback, std::function<void ()> actionCallback);
};
