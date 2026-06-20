/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/RespPage.h>

namespace Crest {

json RespPage::toJson() const {
  return {{"size", size},
          {"totalElements", totalElements},
          {"totalPages", totalPages},
          {"number", number}};
}

RespPage RespPage::fromJson(const json &j) {
  RespPage respPage;
  respPage.size = j.value("size", 0);
  respPage.totalElements = j.value("totalElements", 0);
  respPage.totalPages = j.value("totalPages", 0);
  respPage.number = j.value("number", 0);
  return respPage;
}

}  // namespace Crest
