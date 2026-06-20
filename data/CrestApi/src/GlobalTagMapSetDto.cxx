/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/GlobalTagMapSetDto.h>

namespace Crest {

json GlobalTagMapSetDto::toJson() const {
  json baseJson = CrestBaseResponse::toJson();
  json jsonResources = json::array();
  for (const auto &resource : resources) {
    jsonResources.push_back(((GlobalTagMapDto)resource).toJson());
  }
  baseJson["resources"] = jsonResources;
  return baseJson;
}

GlobalTagMapSetDto GlobalTagMapSetDto::fromJson(const json &j) {
  GlobalTagMapSetDto tagMapSet;
  tagMapSet.loadFromJson(j);
  json jsonResources = j.value("resources", json::array());
  for (auto it = jsonResources.begin(); it != jsonResources.end(); ++it) {
    tagMapSet.resources.push_back(GlobalTagMapDto::fromJson(*it));
  }

  return tagMapSet;
}

GlobalTagMapSetDto GlobalTagMapSetDto::fromFsJson(const json &j) {
  GlobalTagMapSetDto tagMapSet;
  tagMapSet.loadFromJson(j);
  // int n = j.size();

  for (auto it = j.begin(); it != j.end(); ++it) {
    tagMapSet.resources.push_back(GlobalTagMapDto::fromJson(*it));
  }

  return tagMapSet;
}

}  // namespace Crest
