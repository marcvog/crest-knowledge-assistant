/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_TAG_DTO_HPP
#define CREST_TAG_DTO_HPP

#include <CrestApi/CrestException.h>

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

using json = nlohmann::json;

namespace Crest {

/**
 * @brief The TagDto class
 * It contains all fields of the TagDto class from the CREST API.
 * When using this to create a new Tag you don't need to set insertion and
 * modification times.
 *
 * @param name The name of the tag.
 * @param timeType The time type of the tag [time, run-lumi,...]
 * @param objectType The type of the object that the tag is associated with [for
 * crest_jsondata it is either crest-json-single-iov, or crest-json-multi-iov]
 * @param description The description of the tag.
 * @param synchronization The synchronization of the tag: UPDX for example
 * (optional, should be taken from tag name).
 * @param lastValidatedTime The last validated time of the tag, in milliseconds
 * (optional, not used in Athena).
 * @param endOfValidity The end of validity of the tag, in the same units as the
 * "since" of the IOVs.
 */
// Create an enum like map for Status and Synchronization
enum class StatusEnum { LOCKED, UNLOCKED };

inline std::string statusToString(StatusEnum status) {
  int istatus = static_cast<int>(status);
  switch (istatus) {
    case static_cast<int>(StatusEnum::LOCKED):
      return "LOCKED";
    case static_cast<int>(StatusEnum::UNLOCKED):
      return "UNLOCKED";
    default:
      throw std::invalid_argument("Invalid status");
  }
}

inline StatusEnum toStatusEnum(const std::string &status) {
  if (status == "LOCKED")
    return StatusEnum::LOCKED;
  if (status == "UNLOCKED")
    return StatusEnum::UNLOCKED;
  throw std::invalid_argument("Invalid status");
}

enum class SyncroEnum { ALL, SV, UPD };

inline std::string synchroToString(SyncroEnum synchro) {
  int isynchro = static_cast<int>(synchro);
  switch (isynchro) {
    case static_cast<int>(SyncroEnum::ALL):
      return "ALL";
    case static_cast<int>(SyncroEnum::SV):
      return "SV";
    case static_cast<int>(SyncroEnum::UPD):
      return "UPD";
    default:
      throw std::invalid_argument("Invalid status");
  }
}

inline SyncroEnum toSynchroEnum(const std::string &synchro) {
  if (synchro == "ALL")
    return SyncroEnum::ALL;
  if (synchro == "SV")
    return SyncroEnum::SV;
  if (synchro == "UPD")
    return SyncroEnum::UPD;
  throw std::invalid_argument("Invalid status");
}

class TagDto {
 private:
  std::string name;
  std::string timeType;
  std::string objectType;
  std::string description;
  std::string synchronization;
  std::string status;
  uint64_t lastValidatedTime;
  uint64_t endOfValidity;
  std::optional<std::string> insertionTime;
  std::optional<std::string> modificationTime;

 public:
  // Ctor
  TagDto(const std::string &name, const std::string &timeType,
         const std::string &description);
  // Default Ctor
  TagDto();
  const std::string &getName() const { return name; }
  const std::string &getTimeType() const { return timeType; }
  const std::string &getObjectType() const { return objectType; }
  const std::string &getDescription() const { return description; }
  const std::string &getSynchronization() const { return synchronization; }
  const std::string &getStatus() const { return status; }

  uint64_t getLastValidatedTime() const { return lastValidatedTime; }
  uint64_t getEndOfValidity() const { return endOfValidity; }
  std::optional<std::string> getInsertionTime() const { return insertionTime; }
  std::optional<std::string> getModificationTime() const {
    return modificationTime;
  }
  // Define setters
  void setName(const std::string &name) { this->name = name; }
  void setTimeType(const std::string &timeType) { this->timeType = timeType; }
  void setObjectType(const std::string &objectType) {
    this->objectType = objectType;
  }
  void setDescription(const std::string &description) {
    this->description = description;
  }
  void setSynchronization(const std::string &synchronization) {
    SyncroEnum is = toSynchroEnum(synchronization);
    this->synchronization = synchroToString(is);
  }
  void setStatus(const std::string &status) {
    StatusEnum is = toStatusEnum(status);
    this->status = statusToString(is);
  }
  void setLastValidatedTime(uint64_t lastValidatedTime) {
    this->lastValidatedTime = lastValidatedTime;
  }
  void setEndOfValidity(uint64_t endOfValidity) {
    this->endOfValidity = endOfValidity;
  }
  void setInsertionTime(const std::string &insertionTime) {
    this->insertionTime = insertionTime;
  }
  void setModificationTime(const std::string &modificationTime) {
    this->modificationTime = modificationTime;
  }

  json toJson() const;
  static TagDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_TAG_DTO_HPP
