/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/PayloadSetDto.h>

namespace Crest {

// Function to serialize the object to JSON

json PayloadSetDto::toJson() const {
  json baseJson = CrestBaseResponse::toJson();
  json jsonResources = json::array();
  for (const auto &resource : resources) {
    jsonResources.push_back(((PayloadDto)resource).toJson());
  }
  baseJson["resources"] = jsonResources;
  return baseJson;
}

// Function to deserialize the object from JSON

PayloadSetDto PayloadSetDto::fromJson(const json &j) {
  PayloadSetDto payloadSetDto;
  payloadSetDto.loadFromJson(j);
  json jsonResources = j.value("resources", json::array());
  for (auto it = jsonResources.begin(); it != jsonResources.end(); ++it) {
    payloadSetDto.resources.push_back(PayloadDto::fromJson(*it));
  }

  return payloadSetDto;
}

}  // namespace Crest
