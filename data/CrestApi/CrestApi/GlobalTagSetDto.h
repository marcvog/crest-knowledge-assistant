/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_GLOB_TAG_SET_DTO_HPP
#define CREST_GLOB_TAG_SET_DTO_HPP

#include <CrestApi/CrestBaseResponse.h>
#include <CrestApi/CrestException.h>
#include <CrestApi/GlobalTagDto.h>

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class GlobalTagSetDto : public CrestBaseResponse {
 private:
  inline static const std::string s_dtoType = "GlobalTagSetDto";
  std::vector<GlobalTagDto> resources;

 public:
  const std::string &getFormat() const { return s_dtoType; }

  std::size_t size() const { return resources.size(); }
  const std::vector<GlobalTagDto> &getResources() const { return resources; }

  void add(const GlobalTagDto &tag) { resources.push_back(tag); }

  json toJson() const;

  static GlobalTagSetDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_GLOB_TAG_SET_DTO_HPP
