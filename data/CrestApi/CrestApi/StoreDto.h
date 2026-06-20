/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_STORE_DTO_HPP
#define CREST_STORE_DTO_HPP

#include <CrestApi/CrestException.h>

#include <optional>
#include <string>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class StoreDto {
 private:
  uint64_t since{};
  std::string hash;
  std::string data;
  std::optional<std::string> insertionTime;
  std::optional<std::string> m_streamer_info;

 public:
  StoreDto(uint64_t since, const std::string &data)
      : since(since), hash(""), data(data) {}

  // Ctor
  StoreDto(uint64_t since, const std::string &hash, const std::string &data)
      : since(since), hash(hash), data(data) {}

  // Default Ctor
  StoreDto() : since(0), hash(""), data("") {}

  void setStreamerInfo(const char *str) { m_streamer_info = str; }

  std::string getStreamerInfo() {
    std::string value = m_streamer_info.value_or("default_value");
    return value;
  }

  uint64_t getSince() const { return since; }
  const std::string &getHash() const { return hash; }
  const std::string &getData() const { return data; }

  json toJson() const;

  static StoreDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_STORE_DTO_HPP
