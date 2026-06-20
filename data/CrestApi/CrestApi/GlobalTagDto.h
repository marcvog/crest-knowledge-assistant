/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_GLOBAL_TAG_DTO_HPP
#define CREST_GLOBAL_TAG_DTO_HPP

#include <CrestApi/CrestException.h>

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

using json = nlohmann::json;

enum class TypeEnum { TEST, LOCKED, NONE };

inline std::string typeToString(TypeEnum ts) {
  int is = static_cast<int>(ts);
  switch (is) {
    case static_cast<int>(TypeEnum::LOCKED):
      return "L";
    case static_cast<int>(TypeEnum::TEST):
      return "T";
    case static_cast<int>(TypeEnum::NONE):
      return "N";
    default:
      throw std::invalid_argument("Invalid type");
  }
}

inline TypeEnum toTypeEnum(const std::string &ts) {
  if (ts == "L")
    return TypeEnum::LOCKED;
  if (ts == "T")
    return TypeEnum::TEST;
  if (ts == "N")
    return TypeEnum::NONE;
  throw std::invalid_argument("Invalid type");
}

namespace Crest {

class GlobalTagDto {
 private:
  std::string name;
  int64_t validity;
  std::string description;
  std::string release;
  std::optional<std::string> insertionTime;
  std::optional<std::string> snapshotTime;
  std::string scenario;
  std::string workflow;
  std::string type;
  int64_t snapshotTimeMilli;
  int64_t insertionTimeMilli;

 public:
  // Ctor
  GlobalTagDto(const std::string &name, const std::string &description,
               const std::string &release, const std::string &workflow);

  // Default Ctor
  GlobalTagDto();

  // Setters and Getters
  const std::string &getName() const { return name; }
  int64_t getValidity() const { return validity; }
  const std::string &getDescription() const { return description; }
  const std::string &getRelease() const { return release; }
  std::optional<std::string> getInsertionTime() const { return insertionTime; }
  std::optional<std::string> getSnapshotTime() const { return snapshotTime; }
  const std::string &getScenario() const { return scenario; }
  const std::string &getWorkflow() const { return workflow; }
  const std::string &getType() const { return type; }
  int64_t getSnapshotTimeMilli() const { return snapshotTimeMilli; }
  int64_t getInsertionTimeMilli() const { return insertionTimeMilli; }

  void setName(const std::string &name) { this->name = name; }
  void setRelease(const std::string &release) { this->release = release; }
  void setWorkflow(const std::string &workflow) { this->workflow = workflow; }
  void setValidity(int64_t validity) { this->validity = validity; }
  void setDescription(const std::string &description) {
    this->description = description;
  }
  void setSnapshotTime(const std::string &snapshotTime) {
    this->snapshotTime = snapshotTime;
  }
  void setScenario(const std::string &scenario) { this->scenario = scenario; }
  void setType(const std::string &type) {
    TypeEnum is = toTypeEnum(type);
    this->type = typeToString(is);
  }

  json toJson() const;

  static GlobalTagDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_GLOBAL_TAG_DTO_HPP
