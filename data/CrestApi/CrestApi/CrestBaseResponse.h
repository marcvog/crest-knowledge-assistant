/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_BASE_RESP_HPP
#define CREST_BASE_RESP_HPP

#include <CrestApi/CrestException.h>
#include <CrestApi/GenericMap.h>
#include <CrestApi/RespPage.h>

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace Crest {

class CrestBaseResponse {
 private:
  std::optional<std::string> datatype;
  std::optional<RespPage> page;
  std::optional<GenericMap> filter;

 public:
  virtual ~CrestBaseResponse() = default;

  std::optional<std::string> getDataType() const { return datatype; }
  std::optional<RespPage> getRespPage() const { return page; }
  std::optional<GenericMap> getFilter() const { return filter; }

  void setRespPage(const RespPage &page) { this->page = page; }
  void setDataType(const std::string &dtype) { datatype = dtype; }

  json toJson() const;
  void loadFromJson(const json &j);
  // virtual const char* getFormat() const =0;
  virtual const std::string &getFormat() const = 0;
  virtual std::size_t size() const = 0;
};

}  // namespace Crest

#endif  // CREST_BASE_RESP_HPP
