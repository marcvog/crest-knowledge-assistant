/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/ChannelSetDto.h>

namespace Crest {

void ChannelSetDto::add(const std::string &id, const std::string &name) {
  m_channels.push_back(std::pair<std::string, std::string>(id, name));
}

ChannelSetDto ChannelSetDto::fromJson(const json &j) {
  ChannelSetDto chDto;
  // Check if the input JSON is an array
  if (!j.is_array()) {
    throw std::invalid_argument("Expected an array of JSON objects.");
  }

  for (const auto &item : j) {
    // Ensure each element is an object with at least one key-value pair
    if (!item.is_object() || item.empty()) {
      continue;  // Skip if not an object or if it's empty
    }
    // Get the first key-value pair in the object
    auto it = item.begin();
    chDto.add(it.key(), it.value());
  }

  return chDto;
}

json ChannelSetDto::toJson() const {
  json chJson = json::array();
  for (auto &ch : m_channels) {
    json obj = {};
    obj[ch.first] = ch.second;
    chJson.push_back(obj);
  }
  return chJson;
}

}  // namespace Crest
