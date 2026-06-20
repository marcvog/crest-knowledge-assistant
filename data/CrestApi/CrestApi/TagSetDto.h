/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_TAG_SET_DTO_HPP
#define CREST_TAG_SET_DTO_HPP

#include <CrestApi/CrestBaseResponse.h>
#include <CrestApi/CrestDetail.h>
#include <CrestApi/CrestException.h>
#include <CrestApi/TagDto.h>

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class TagSetDto : public CrestBaseResponse {
 private:
  inline static const std::string s_dtoType = "TagSetDto";
  std::vector<TagDto> resources;

 public:
  TagSetDto(const std::vector<TagDto> &resources) : resources(resources) {}
  TagSetDto() : resources() {}
  const std::string &getFormat() const { return s_dtoType; }

  size_t size() const { return resources.size(); }

  void add(const TagDto &tag) { resources.push_back(tag); }

  const std::vector<TagDto> &getResources() const { return resources; }

  json toJson() const;
  static TagSetDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_TAG_SET_DTO_HPP
