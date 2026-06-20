/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_TAG_META_DTO_HPP
#define CREST_TAG_META_DTO_HPP

#include <CrestApi/CrestException.h>
#include <CrestApi/TagInfoDto.h>

#include <optional>
#include <string>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class TagMetaDto {
 private:
  std::string tagName;
  std::string description;
  std::optional<std::string> insertionTime;
  std::string tagInfo;

 public:
  // Ctor
  TagMetaDto(const std::string &tagName, const std::string &description,
             const std::string &info)
      : tagName(tagName), description(description), tagInfo(info) {}

  // Default Ctor
  TagMetaDto() : tagName(""), description(""), tagInfo("") {}

  const std::string &getTagName() const { return tagName; }
  const std::string &getDescription() const { return description; }
  const std::string &getTagInfo() const { return tagInfo; }
  std::optional<std::string> getInsertionTime() const { return insertionTime; }

  // Setters
  void setTagName(const std::string &tagName) { this->tagName = tagName; }
  void setDescription(const std::string &description) {
    this->description = description;
  }
  void setTagInfo(const std::string &tagInfo) { this->tagInfo = tagInfo; }

  json toJson() const;
  static TagMetaDto fromJson(const json &j);

  TagInfoDto getTagInfoDto() const;
};

}  // namespace Crest

#endif  // CREST_TAG_META_DTO_HPP
