/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/TagMetaDto.h>

namespace Crest {

json TagMetaDto::toJson() const {
  json tagMeta = {};
  tagMeta["tagName"] = tagName;
  tagMeta["description"] = description;
  tagMeta["tagInfo"] = tagInfo;
  if (insertionTime.has_value())
    tagMeta["insertionTime"] = insertionTime.value();
  return tagMeta;
}

TagMetaDto TagMetaDto::fromJson(const json &j) {
  TagMetaDto tag;
  tag.tagName = j.value("tagName", "");
  tag.tagInfo = j.value("tagInfo", "");
  tag.description = j.value("description", "");
  if (j.contains(std::string{"insertionTime"}))
    tag.insertionTime = j.value("insertionTime", "");
  return tag;
}

TagInfoDto TagMetaDto::getTagInfoDto() const {
  return TagInfoDto::fromJson(json::parse(tagInfo));
}

}  // namespace Crest
