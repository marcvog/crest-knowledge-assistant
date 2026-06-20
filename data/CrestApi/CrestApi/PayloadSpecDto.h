/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_PL_SPEC_DTO_HPP
#define CREST_PL_SPEC_DTO_HPP

#include <CrestApi/CrestException.h>

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class PayloadSpecDto {
 private:
  std::vector<std::pair<std::string, std::string>> m_row;

 public:
  PayloadSpecDto &operator=(const PayloadSpecDto &) = default;
  PayloadSpecDto &operator=(PayloadSpecDto &&) = default;
  PayloadSpecDto() {}
  PayloadSpecDto(PayloadSpecDto const &copy) : m_row(copy.getColumns()) {}
  const std::vector<std::pair<std::string, std::string>> &getColumns() const {
    return m_row;
  }
  size_t size() const { return m_row.size(); }
  void add(const std::string &name, const std::string &type);
  json toJson() const;
  static PayloadSpecDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_PL_SPEC_DTO_HPP
