/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/GlobalTagMapDto.h>

namespace Crest {

json GlobalTagMapDto::toJson() const {
  return {{"tagName", tagName},
          {"globalTagName", globalTagName},
          {"record", record},
          {"label", label}};
}

GlobalTagMapDto GlobalTagMapDto::fromJson(const json &j) {
  GlobalTagMapDto tagmap;
  tagmap.tagName = j.value("tagName", "");
  tagmap.globalTagName = j.value("globalTagName", "");
  tagmap.record = j.value("record", "none");
  tagmap.label = j.value("label", "none");
  return tagmap;
}

}  // namespace Crest
