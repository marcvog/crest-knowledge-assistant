/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/CrestBaseResponse.h>

namespace Crest {

json CrestBaseResponse::toJson() const {
  json result = {};
  if (datatype.has_value())
    result["datatype"] = datatype.value();
  result["format"] = getFormat();
  result["size"] = size();

  // Check if the 'page' optional contains a value
  if (page.has_value()) {
    // Check if the value of 'page' is not null
    if (!page.value().toJson().is_null()) {
      result["page"] = page.value().toJson();
    }
  }

  // Check if the 'filter' optional contains a value
  if (filter.has_value()) {
    // Check if the value of 'filter' is not null
    if (!filter.value().toJson().is_null()) {
      result["filter"] = filter.value().toJson();
    }
  }

  return result;
}

void CrestBaseResponse::loadFromJson(const json &j) {
  if (j.contains("datatype"))
    datatype = j.value("datatype", "");
  // Check for the presence of "page" key and create an optional
  if (j.contains("page")) {
    if (!j["page"].is_null()) {
      page = std::make_optional((RespPage::fromJson(j["page"])));
    }
  }

  // Check for the presence of "filter" key and create an optional
  filter = j.contains("filter")
               ? std::make_optional(GenericMap::fromJson(j["filter"]))
               : std::nullopt;
}

}  // namespace Crest
