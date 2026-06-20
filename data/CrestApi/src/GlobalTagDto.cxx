/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/GlobalTagDto.h>

namespace Crest {

GlobalTagDto::GlobalTagDto()
    : name(""),
      validity(0),
      description(""),
      release(""),
      scenario(""),
      workflow(""),
      type("T"),
      snapshotTimeMilli(0),
      insertionTimeMilli(0) {}

GlobalTagDto::GlobalTagDto(const std::string &name,
                           const std::string &description,
                           const std::string &release,
                           const std::string &workflow)
    : name(name),
      validity(0),
      description(description),
      release(release),
      scenario("test"),
      workflow(workflow),
      type("T"),
      snapshotTimeMilli(0),
      insertionTimeMilli(0) {}

json GlobalTagDto::toJson() const {
  json js = {};
  js["name"] = name;
  js["validity"] = validity;
  js["description"] = description;
  js["release"] = release;
  if (insertionTime.has_value())
    js["insertionTime"] = insertionTime.value();
  ;
  if (snapshotTime.has_value())
    js["snapshotTime"] = snapshotTime.value();
  js["scenario"] = scenario;
  js["workflow"] = workflow;
  js["type"] = type;
  js["snapshotTimeMilli"] = snapshotTimeMilli;
  js["insertionTimeMilli"] = insertionTimeMilli;
  return js;
}

GlobalTagDto GlobalTagDto::fromJson(const json &j) {
  GlobalTagDto globalTag;
  globalTag.name = j.value("name", "");
  globalTag.validity = j.value<uint64_t>("validity", 0);
  globalTag.description = j.value("description", "");
  globalTag.release = j.value("release", "");
  if (j.contains(std::string{"insertionTime"}))
    globalTag.insertionTime = j.value("insertionTime", "");
  if (j.contains(std::string{"snapshotTime"}))
    globalTag.snapshotTime = j.value("snapshotTime", "");
  globalTag.scenario = j.value("scenario", "");
  globalTag.workflow = j.value("workflow", "");
  globalTag.type = j.value("type", "");
  globalTag.snapshotTimeMilli = j.value("snapshotTimeMilli", 0);
  globalTag.insertionTimeMilli = j.value("insertionTimeMilli", 0);
  return globalTag;
}

}  // namespace Crest
