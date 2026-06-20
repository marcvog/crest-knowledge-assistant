/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/TagInfoDto.h>

namespace Crest {

json TagInfoDto::toJson() const {
  json tagInfo = {};
  tagInfo["node_description"] = m_node_description;
  tagInfo["payload_spec"] = m_payload_spec.toJson();
  tagInfo["channel_list"] = m_channel_list.toJson();
  return tagInfo;
}

TagInfoDto TagInfoDto::fromJson(const json &j) {
  std::string node_description = j.value("node_description", "");
  json jsonSpec = j.value("payload_spec", json::array());
  if (jsonSpec.is_string())
    jsonSpec = json::parse(to_string(jsonSpec));
  PayloadSpecDto payload_spec = PayloadSpecDto::fromJson(jsonSpec);
  json jsonCh = j.value("channel_list", json::array());
  if (jsonCh.is_string())
    jsonCh = json::parse(to_string(jsonCh));
  ChannelSetDto channel_list = ChannelSetDto::fromJson(jsonCh);
  TagInfoDto dto(node_description, payload_spec, channel_list);
  return dto;
}

}  // namespace Crest
