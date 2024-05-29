#pragma once

#include "../includes/geode.hpp"
#include "list_cell.hpp"

using namespace geode::prelude;

struct MultiStringSettingStruct {
  Vec<String> m_strings;
};

class MultiStringSettingValue;

class MultiStringSettingValue : public SettingValue {
protected:
  Vec<String> m_strings;

public:
  MultiStringSettingValue(String const &key, String const &modID, Vec<String> strings)
      : SettingValue(key, modID), m_strings(strings) {}

  bool load(matjson::Value const &json) override {
    m_strings.clear();
    auto array = json.as_array();
    for (auto const &elem : array) {
      m_strings.push_back(elem.as_string());
    }
    return true;
  }

  bool save(matjson::Value &json) const override {
    auto array = matjson::Array();
    for (auto const &string : m_strings) {
      array.push_back(string);
    }
    json = array;
    return true;
  }

  SettingNode *createNode(float width) override;

  void setStrings(Vec<String> strings) {
    this->m_strings = strings;
    this->valueChanged();
  }

  Vec<String> getStrings() const { return this->m_strings; }
};

template <> struct SettingValueSetter<MultiStringSettingStruct> {
  static MultiStringSettingStruct get(SettingValue *setting) {
    return MultiStringSettingStruct{static_cast<MultiStringSettingValue *>(setting)->getStrings()};
  };
  static void set(MultiStringSettingValue *setting, MultiStringSettingStruct const &value) {
    setting->setStrings(value.m_strings);
  };
};

class MultiStringSettingNode : public SettingNode {
protected:
  MultiStringSettingValue *m_value;
  CCMenuItemSpriteExtra *m_resetBtn;
  CCLabelBMFont *m_label;
  Vec<String> m_localValue;
  String m_name;
  String m_description;
  Vec<String> m_defaultValue;

  bool init(MultiStringSettingValue *value, float width);

public:
  void updateVisuals();
  void onView(CCObject *);
  void onReset(CCObject *);
  void onDesc(CCObject *);
  void commit() override;
  bool hasUncommittedChanges() override;
  bool hasNonDefaultValue() override;
  void resetToDefault() override;
  static MultiStringSettingNode *create(MultiStringSettingValue *value, float width);
};
