/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_GEN_MAP_HPP
#define CREST_GEN_MAP_HPP

#include <CrestApi/CrestException.h>

#include <optional>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class GenericMap {
 private:
  std::map<std::string, std::string> additionalProperties;

 public:
  const std::map<std::string, std::string> &getProperties() const {
    return additionalProperties;
  }

  json toJson() const;

  static GenericMap fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_GEN_MAP_HPP
