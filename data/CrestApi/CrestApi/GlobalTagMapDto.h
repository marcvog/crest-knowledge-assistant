/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_GLOBAL_TAG_MAP_DTO_HPP
#define CREST_GLOBAL_TAG_MAP_DTO_HPP

#include <CrestApi/CrestException.h>

#include <string>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class GlobalTagMapDto {
 private:
  std::string tagName;
  std::string globalTagName;
  std::string record;
  std::string label;

 public:
  // Ctor
  GlobalTagMapDto(const std::string &tagName, const std::string &globalTagName,
                  const std::string &record)
      : tagName(tagName),
        globalTagName(globalTagName),
        record(record),
        label("none") {}
  GlobalTagMapDto(const std::string &tagName, const std::string &globalTagName,
                  const std::string &record, const std::string &label)
      : tagName(tagName),
        globalTagName(globalTagName),
        record(record),
        label(label) {}
  // Default Ctor
  GlobalTagMapDto()
      : tagName(""), globalTagName(""), record("none"), label("none") {}
  void setLabel(const std::string &label) { this->label = label; }
  const std::string &getTagName() const { return tagName; }
  const std::string &getGlobalTagName() const { return globalTagName; }
  const std::string &getRecord() const { return record; }
  const std::string &getLabel() const { return label; }

  json toJson() const;
  static GlobalTagMapDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_GLOBAL_TAG_MAP_DTO_HPP
