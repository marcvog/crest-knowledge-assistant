/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_RUN_LUMI_SET_DTO_HPP
#define CREST_RUN_LUMI_SET_DTO_HPP

#include <CrestApi/CrestBaseResponse.h>
#include <CrestApi/CrestDetail.h>
#include <CrestApi/CrestException.h>
#include <CrestApi/RunLumiDto.h>

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class RunLumiSetDto : public CrestBaseResponse {
 private:
  inline static const std::string s_dtoType = "RunLumiSetDto";
  std::vector<RunLumiDto> resources;

 public:
  RunLumiSetDto(const std::vector<RunLumiDto> &resources)
      : resources(resources) {}
  RunLumiSetDto() : resources() {}
  const std::string &getFormat() const { return s_dtoType; }

  size_t size() const { return resources.size(); }

  void add(const RunLumiDto &rl) { resources.push_back(rl); }

  const std::vector<RunLumiDto> &getResources() const { return resources; }

  json toJson() const;
  static RunLumiSetDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_RUN_LUMI_SET_DTO_HPP
