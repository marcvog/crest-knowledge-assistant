/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/RunLumiSetDto.h>

namespace Crest {

json RunLumiSetDto::toJson() const {
  json baseJson = CrestBaseResponse::toJson();
  json jsonResources = json::array();
  for (const auto &resource : resources) {
    jsonResources.push_back(((RunLumiDto)resource).toJson());
  }
  baseJson["resources"] = jsonResources;
  return baseJson;
}

RunLumiSetDto RunLumiSetDto::fromJson(const json &j) {
  RunLumiSetDto rlSet;
  rlSet.loadFromJson(j);
  json jsonResources = j.value("resources", json::array());
  for (auto it = jsonResources.begin(); it != jsonResources.end(); ++it) {
    rlSet.resources.push_back(RunLumiDto::fromJson(*it));
  }

  return rlSet;
}

}  // namespace Crest
