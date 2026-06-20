/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/PayloadTagInfoSetDto.h>

namespace Crest {

json PayloadTagInfoSetDto::toJson() const {
  json baseJson = CrestBaseResponse::toJson();
  json jsonResources = json::array();
  for (const auto &resource : resources) {
    jsonResources.push_back(((PayloadTagInfoDto)resource).toJson());
  }
  baseJson["resources"] = jsonResources;
  return baseJson;
}

PayloadTagInfoSetDto PayloadTagInfoSetDto::fromJson(const json &j) {
  PayloadTagInfoSetDto pSet;
  pSet.loadFromJson(j);
  json jsonResources = j.value("resources", json::array());
  for (auto it = jsonResources.begin(); it != jsonResources.end(); ++it) {
    pSet.resources.push_back(PayloadTagInfoDto::fromJson(*it));
  }

  return pSet;
}

}  // namespace Crest
