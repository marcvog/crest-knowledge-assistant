/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_PL_SET_DTO_HPP
#define CREST_PL_SET_DTO_HPP

#include <CrestApi/CrestBaseResponse.h>
#include <CrestApi/CrestException.h>
#include <CrestApi/PayloadDto.h>

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class PayloadSetDto : public CrestBaseResponse {
 private:
  inline static const std::string s_dtoType = "PayloadSetDto";
  std::vector<PayloadDto> resources;

 public:
  const std::string &getFormat() const { return s_dtoType; }
  std::size_t size() const { return resources.size(); }
  const std::vector<PayloadDto> &getResources() const { return resources; }

  // Function to serialize the object to JSON

  json toJson() const;

  // Function to deserialize the object from JSON

  static PayloadSetDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_PL_SET_DTO_HPP
