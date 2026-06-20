/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/IovDto.h>

namespace Crest {

json IovDto::toJson() const {
  json iov = {};
  iov["tagName"] = tagName;
  iov["since"] = since;
  if (insertionTime.has_value())
    iov["insertionTime"] = insertionTime.value();
  iov["payloadHash"] = payloadHash;
  return iov;
}

IovDto IovDto::fromJson(const json &j) {
  IovDto iov;
  iov.tagName = j.value("tagName", "");
  iov.since = j.value<uint64_t>("since", 0);
  iov.insertionTime = "";
  iov.payloadHash = "";

  if (j.contains("insertionTime")) {
    if (!j["insertionTime"].is_null()) {
      iov.insertionTime = j["insertionTime"];
    }
  }

  if (j.contains("payloadHash")) {
    if (!j["payloadHash"].is_null()) {
      iov.payloadHash = j["payloadHash"];
    }
  }

  return iov;
}

}  // namespace Crest
