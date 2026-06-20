/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_PL_TAGINFO_SET_DTO_HPP
#define CREST_PL_TAGINFO_SET_DTO_HPP

#include <CrestApi/CrestBaseResponse.h>
#include <CrestApi/CrestException.h>
#include <CrestApi/PayloadTagInfoDto.h>

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class PayloadTagInfoSetDto : public CrestBaseResponse {
 private:
  inline static const std::string s_dtoType = "PayloadTagInfoSetDto";
  std::vector<PayloadTagInfoDto> resources;

 public:
  const std::string &getFormat() const { return s_dtoType; }
  std::size_t size() const { return resources.size(); }

  const std::vector<PayloadTagInfoDto> &getResources() const {
    return resources;
  }

  json toJson() const;

  static PayloadTagInfoSetDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_PL_TAGINFO_SET_DTO_HPP
