/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/TagMetaSetDto.h>

namespace Crest {

json TagMetaSetDto::toJson() const {
  json baseJson = CrestBaseResponse::toJson();
  json jsonResources = json::array();
  for (const auto &resource : resources) {
    jsonResources.push_back(((TagMetaDto)resource).toJson());
  }
  baseJson["resources"] = jsonResources;
  return baseJson;
}

TagMetaSetDto TagMetaSetDto::fromJson(const json &j) {
  TagMetaSetDto tagSet;
  tagSet.loadFromJson(j);
  json jsonResources = j.value("resources", json::array());
  for (auto it = jsonResources.begin(); it != jsonResources.end(); ++it) {
    tagSet.resources.push_back(TagMetaDto::fromJson(*it));
  }

  return tagSet;
}

}  // namespace Crest
