/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/TagSetDto.h>

namespace Crest {

json TagSetDto::toJson() const {
  json baseJson = CrestBaseResponse::toJson();
  json jsonResources = json::array();
  for (const auto &resource : resources) {
    jsonResources.push_back(((TagDto)resource).toJson());
  }
  baseJson["resources"] = jsonResources;
  return baseJson;
}

TagSetDto TagSetDto::fromJson(const json &j) {
  TagSetDto tagSet;
  tagSet.loadFromJson(j);
  json jsonResources = j.value("resources", json::array());
  for (auto it = jsonResources.begin(); it != jsonResources.end(); ++it) {
    tagSet.resources.push_back(TagDto::fromJson(*it));
  }

  return tagSet;
}

}  // namespace Crest
