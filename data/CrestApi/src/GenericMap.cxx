/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/GenericMap.h>

namespace Crest {

json GenericMap::toJson() const {
  json additionalPropertiesJson;
  for (const auto &entry : additionalProperties) {
    additionalPropertiesJson[entry.first] = entry.second;
  }

  return {additionalPropertiesJson};
}

GenericMap GenericMap::fromJson(const json &j) {
  GenericMap map;
  if (j.contains("additionalProperties")) {
    map.additionalProperties =
        j.at("additionalProperties").get<std::map<std::string, std::string>>();
  } else {
    // iterate over the json object
    for (auto it = j.begin(); it != j.end(); ++it) {
      map.additionalProperties[it.key()] = it.value();
    }
  }
  return map;
}

}  // namespace Crest
