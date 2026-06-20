/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_TAG_META_SET_DTO_HPP
#define CREST_TAG_META_SET_DTO_HPP

#include <CrestApi/CrestBaseResponse.h>
#include <CrestApi/CrestException.h>
#include <CrestApi/TagMetaDto.h>

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class TagMetaSetDto : public CrestBaseResponse {
 private:
  inline static const std::string s_dtoType = "TagMetaSetDto";
  std::vector<TagMetaDto> resources;

 public:
  const std::string &getFormat() const { return s_dtoType; }
  std::size_t size() const { return resources.size(); }

  const std::vector<TagMetaDto> &getResources() const { return resources; }

  json toJson() const;
  static TagMetaSetDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_TAG_META_SET_DTO_HPP
