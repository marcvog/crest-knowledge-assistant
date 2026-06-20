/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/TagDto.h>

namespace Crest {

TagDto::TagDto(const std::string &name, const std::string &timeType,
               const std::string &description)
    : name(name),
      timeType(timeType),
      objectType(""),
      description(description),
      synchronization("ALL"),
      status("UNLOCKED"),
      lastValidatedTime(0),
      endOfValidity(0) {}
// Default Ctor
TagDto::TagDto()
    : name(""),
      timeType(""),
      objectType(""),
      description(""),
      synchronization("ALL"),
      status("UNLOCKED"),
      lastValidatedTime(0),
      endOfValidity(0) {}

json TagDto::toJson() const {
  json js = {};
  js["name"] = name;
  js["timeType"] = timeType;
  js["payloadSpec"] = objectType;
  js["synchronization"] = synchronization;
  js["status"] = status;
  if (insertionTime.has_value())
    js["insertionTime"] = insertionTime.value();
  js["description"] = description;
  js["lastValidatedTime"] = lastValidatedTime;
  js["endOfValidity"] = endOfValidity;
  if (modificationTime.has_value())
    js["modificationTime"] = modificationTime.value();
  return js;
}

TagDto TagDto::fromJson(const json &j) {
  TagDto tag;
  auto it = j.find("name");
  if (it != j.end()) {
    tag.name = j["name"];
  } else {
    throw Crest::CrestException(
        "ERROR in TagDto.fromJson: JSON contains no tag name.");
  }

  tag.timeType = j.value("timeType", "");
  tag.objectType = j.value("payloadSpec", "");
  tag.synchronization = j.value("synchronization", "ALL");
  tag.status = j.value("status", "UNLOCKED");
  tag.description = j.value("description", "");
  tag.lastValidatedTime = j.value<uint64_t>("lastValidatedTime", 0);
  tag.endOfValidity = j.value<uint64_t>("endOfValidity", 0);
  if (j.contains(std::string{"insertionTime"}))
    tag.insertionTime = j.value("insertionTime", "");
  if (j.contains(std::string{"modificationTime"}))
    tag.modificationTime = j.value("modificationTime", "");
  return tag;
}

}  // namespace Crest
