/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/IovSetDto.h>

namespace Crest {

json IovSetDto::toJson() const {
  json baseJson = CrestBaseResponse::toJson();
  json jsonResources = json::array();
  for (const auto &resource : resources) {
    jsonResources.push_back(((IovDto)resource).toJson());
  }
  baseJson["resources"] = jsonResources;
  return baseJson;
}

std::vector<uint64_t> IovSetDto::getListSince() {
  std::vector<uint64_t> v;
  for (IovDto item_iov : resources) {
    uint64_t since = item_iov.getSince();
    v.emplace_back(since);
  }
  return v;
}

void IovSetDto::push_back(const IovDto &dto) {
  resources.push_back(dto);
}

IovSetDto IovSetDto::fromJson(const json &j) {
  IovSetDto iovSet;
  iovSet.loadFromJson(j);
  json jsonResources = j.value("resources", json::array());
  for (auto it = jsonResources.begin(); it != jsonResources.end(); ++it) {
    iovSet.resources.push_back(IovDto::fromJson(*it));
  }

  return iovSet;
}

IovSetDto IovSetDto::fromFsJson(const json &j) {
  IovSetDto iovSet;
  iovSet.loadFromJson(j);
  // int n = j.size();

  for (auto it = j.begin(); it != j.end(); ++it) {
    iovSet.resources.push_back(IovDto::fromJson(*it));
  }

  iovSet.setDataType("iovs");

  return iovSet;
}

}  // namespace Crest
