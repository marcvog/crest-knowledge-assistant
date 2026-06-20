/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/StoreSetDto.h>

namespace Crest {

json StoreSetDto::toJson() const {
  json baseJson = CrestBaseResponse::toJson();
  json jsonResources = json::array();
  for (const auto &resource : resources) {
    jsonResources.push_back(((StoreDto)resource).toJson());
  }
  baseJson["resources"] = jsonResources;
  return baseJson;
}

void StoreSetDto::push_back(StoreDto dto) {
  resources.push_back(dto);
}

void StoreSetDto::clear() {
  resources.clear();
}

StoreSetDto StoreSetDto::fromJson(const json &j) {
  StoreSetDto storeSetDto;
  json jsonResources = j.value("resources", json::array());
  for (auto it = jsonResources.begin(); it != jsonResources.end(); ++it) {
    storeSetDto.push_back(StoreDto::fromJson(*it));
  }

  return storeSetDto;
}

}  // namespace Crest
