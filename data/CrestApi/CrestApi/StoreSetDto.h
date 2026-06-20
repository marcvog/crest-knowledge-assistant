/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_STORE_SET_DTO_HPP
#define CREST_STORE_SET_DTO_HPP

#include <CrestApi/CrestBaseResponse.h>
#include <CrestApi/CrestException.h>
#include <CrestApi/StoreDto.h>

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class StoreSetDto : public CrestBaseResponse {
 private:
  inline static const std::string s_dtoType = "StoreSetDto";
  std::vector<StoreDto> resources;

 public:
  StoreSetDto(const std::vector<StoreDto> &resources) : resources(resources) {}
  StoreSetDto() : resources() {}
  const std::string &getFormat() const { return s_dtoType; }
  std::size_t size() const { return resources.size(); }
  const std::vector<StoreDto> &getResources() const { return resources; }
  void push_back(StoreDto dto);
  void clear();
  json toJson() const;

  static StoreSetDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_STORE_SET_DTO_HPP
