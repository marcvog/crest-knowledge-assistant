/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_IOV_DTO_HPP
#define CREST_IOV_DTO_HPP

#include <CrestApi/CrestException.h>

#include <optional>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

/**
 * @brief The TagDto class
 * It contains all fields of the TagDto class from the CREST API.
 * When using this to create a new Tag you don't need to set insertion and
 * modification times.
 *
 * @param tagName The name of the tag.
 * @param since The since of the IOV.
 * @param insertionTime The insertion time of the IOV. Provided by the server.
 * @param payloadHash the unique key for the payload. It is computed as the
 * SHA256 hash of the payload.
 */
class IovDto {
 private:
  std::string tagName;
  uint64_t since;
  std::optional<std::string> insertionTime;
  std::string payloadHash;

 public:
  // Ctor
  IovDto(const std::string &tagName, uint64_t since,
         const std::string &payloadHash)
      : tagName(tagName), since(since), payloadHash(payloadHash) {}

  // Default Ctor
  IovDto() : tagName(""), since(0), payloadHash("") {}

  const std::string &getTagName() const { return tagName; }
  uint64_t getSince() const { return since; }
  std::optional<std::string> getInsertionTime() const { return insertionTime; }
  const std::string &getPayloadHash() const { return payloadHash; }

  // Setters
  void setTagName(const std::string &tagName) { this->tagName = tagName; }
  void setSince(uint64_t since) { this->since = since; }
  void setInsertionTime(const std::string &insertionTime) {
    this->insertionTime = insertionTime;
  }
  void setPayloadHash(const std::string &payloadHash) {
    this->payloadHash = payloadHash;
  }

  json toJson() const;
  static IovDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_IOV_DTO_HPP
