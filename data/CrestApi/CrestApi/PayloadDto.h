/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_PL_DTO_HPP
#define CREST_PL_DTO_HPP

#include <CrestApi/CrestException.h>

#include <optional>
#include <string>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class PayloadDto {
 private:
  std::string hash;
  std::string version;
  std::string objectType;
  std::string objectName;
  std::string compressionType;
  std::string checkSum;
  std::size_t psize{};
  std::optional<std::string> insertionTime;

 public:
  // Function to serialize the object to JSON

  json toJson() const;

  // Function to deserialize the object from JSON

  static PayloadDto fromJson(const json &j);

  // Define Getters for the private members
  std::size_t size() const { return psize; }
  const std::string &getHash() const { return hash; }
  const std::string &getVersion() const { return version; }
  const std::string &getObjectType() const { return objectType; }
  const std::string &getObjectName() const { return objectName; }
  const std::string &getCompressionType() const { return compressionType; }
  const std::string &getCheckSum() const { return checkSum; }

  std::optional<std::string> getInsertionTime() const { return insertionTime; }

  // Setters
  void setHash(const std::string &aHash) { hash = aHash; }
  void setVersion(const std::string &aVersion) { version = aVersion; }
  void setObjectType(const std::string &anObjectType) {
    objectType = anObjectType;
  }
  void setObjectName(const std::string &anObjectName) {
    objectName = anObjectName;
  }
  void setCompressionType(const std::string &aCompressionType) {
    compressionType = aCompressionType;
  }
  void setCheckSum(const std::string &aCheckSum) { checkSum = aCheckSum; }
  void size(std::size_t aSize) { psize = aSize; }
};

}  // namespace Crest

#endif  // CREST_PL_DTO_HPP
