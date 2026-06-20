/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_IOV_SET_DTO_HPP
#define CREST_IOV_SET_DTO_HPP

#include <CrestApi/CrestBaseResponse.h>
#include <CrestApi/CrestException.h>
#include <CrestApi/IovDto.h>

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class IovSetDto : public CrestBaseResponse {
 private:
  inline static const std::string s_dtoType = "IovSetDto";
  std::vector<IovDto> resources;

 public:
  IovSetDto(const std::vector<IovDto> &resources) : resources(resources) {}
  IovSetDto() : resources() {}
  const std::string &getFormat() const { return s_dtoType; }
  std::size_t size() const { return resources.size(); }
  std::vector<uint64_t> getListSince();
  void push_back(const IovDto &dto);
  json toJson() const;

  const std::vector<IovDto> &getResources() const { return resources; }

  static IovSetDto fromJson(const json &j);

  static IovSetDto fromFsJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_IOV_SET_DTO_HPP
