/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/PayloadSpecDto.h>
#include <CrestApi/StringUtils.h>

#include <sstream>

namespace Crest {

void PayloadSpecDto::add(const std::string &name, const std::string &type) {
  m_row.push_back(std::pair<std::string, std::string>(name, type));
}

PayloadSpecDto PayloadSpecDto::fromJson(const json &j) {
  PayloadSpecDto chDto;
  // old format
  if (j.is_string()) {
    std::string input = j.get<std::string>();
    std::istringstream stream(input);
    std::string pair;

    // Split by comma for each key-value pair
    while (std::getline(stream, pair, ',')) {
      size_t colonPos = pair.find(':');
      if (colonPos != std::string::npos) {
        std::string key = pair.substr(0, colonPos);
        std::string value = pair.substr(colonPos + 1);

        // Trim whitespace (if necessary)
        key = Crest::StringUtils::trim(key);
        value = Crest::StringUtils::trim(value);

        chDto.add(key, value);
      }
    }
  }
  // New format: JSON array of objects
  else if (j.is_array()) {
    for (const auto &item : j) {
      if (item.is_object()) {
        for (const auto &el : item.items()) {
          chDto.add(el.key(), el.value().get<std::string>());
          break;  // Only add the first key-value pair per object
        }
      }
    }
  }
  return chDto;
}

json PayloadSpecDto::toJson() const {
  json chJson = json::array();
  for (auto &ch : m_row) {
    json obj = {};
    obj[ch.first] = ch.second;
    chJson.push_back(obj);
  }
  return chJson;
}

}  // namespace Crest
