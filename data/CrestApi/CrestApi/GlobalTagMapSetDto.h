/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_GLOB_TAG_MAP_SET_DTO_HPP
#define CREST_GLOB_TAG_MAP_SET_DTO_HPP

#include <CrestApi/CrestBaseResponse.h>
#include <CrestApi/CrestException.h>
#include <CrestApi/GlobalTagMapDto.h>

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class GlobalTagMapSetDto : public CrestBaseResponse {
 private:
  inline static const std::string s_dtoType = "GlobalTagMapSetDto";
  std::vector<GlobalTagMapDto> resources;

 public:
  const std::string &getFormat() const { return s_dtoType; }
  std::size_t size() const { return resources.size(); }
  const std::vector<GlobalTagMapDto> &getResources() const { return resources; }

  json toJson() const;
  static GlobalTagMapSetDto fromJson(const json &j);

  static GlobalTagMapSetDto fromFsJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_GLOB_TAG_MAP_SET_DTO_HPP
