/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/StoreDto.h>

namespace Crest {

json StoreDto::toJson() const {
  json dto = {};
  dto["hash"] = hash;
  dto["since"] = since;
  dto["data"] = data;
  dto["streamerInfo"] = m_streamer_info.value_or("empty");
  return dto;
}

StoreDto StoreDto::fromJson(const json &j) {
  StoreDto storeDto;
  storeDto.hash = j.value("hash", "");
  storeDto.since = j.value("since", 0.0);
  storeDto.data = j.value("data", "");
  storeDto.m_streamer_info = j.value("streamerInfo", "");
  return storeDto;
}

}  // namespace Crest
