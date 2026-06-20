/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/PayloadTagInfoDto.h>

namespace Crest {

json PayloadTagInfoDto::toJson() const {
  json js = {};
  js["tagname"] = tagname;
  js["niovs"] = niovs;
  js["totvolume"] = totvolume;
  js["avgvolume"] = avgvolume;
  return js;
}

PayloadTagInfoDto PayloadTagInfoDto::fromJson(const json &j) {
  PayloadTagInfoDto dto;
  auto it = j.find("tagname");
  if (it != j.end()) {
    dto.tagname = j["tagname"];
  } else {
    throw Crest::CrestException(
        "ERROR in PayloadTagInfoDto.fromJson: JSON contains no tag name.");
  }
  dto.niovs = j.value("niovs", 0);
  dto.totvolume = j.value("totvolume", 0.0);
  dto.avgvolume = j.value("avgvolume", 0.0);
  return dto;
}

}  // namespace Crest
