/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/GlobalTagSetDto.h>

namespace Crest {

json GlobalTagSetDto::toJson() const {
  json baseJson = CrestBaseResponse::toJson();
  json jsonResources = json::array();
  for (const auto &resource : resources) {
    jsonResources.push_back(((GlobalTagDto)resource).toJson());
  }
  baseJson["resources"] = jsonResources;
  return baseJson;
}

GlobalTagSetDto GlobalTagSetDto::fromJson(const json &j) {
  GlobalTagSetDto globalTagSet;
  globalTagSet.loadFromJson(j);
  json jsonResources = j.value("resources", json::array());
  for (auto it = jsonResources.begin(); it != jsonResources.end(); ++it) {
    globalTagSet.resources.push_back(GlobalTagDto::fromJson(*it));
  }

  return globalTagSet;
}

}  // namespace Crest
