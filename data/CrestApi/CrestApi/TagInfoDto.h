/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_TAG_INFO_DTO_HPP
#define CREST_TAG_INFO_DTO_HPP

#include <CrestApi/ChannelSetDto.h>
#include <CrestApi/CrestException.h>
#include <CrestApi/PayloadSpecDto.h>

#include <string>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class TagInfoDto {
 private:
  std::string m_node_description;
  PayloadSpecDto m_payload_spec;
  ChannelSetDto m_channel_list;

 public:
  TagInfoDto(const std::string &description)
      : m_node_description(description) {}

  TagInfoDto(const std::string &description, PayloadSpecDto &payload_spec,
             ChannelSetDto &channel_list)
      : m_node_description(description),
        m_payload_spec(payload_spec),
        m_channel_list(channel_list) {}

  void setPayloadSpec(PayloadSpecDto &spec) { m_payload_spec = spec; }

  void setChannel(ChannelSetDto &ch) { m_channel_list = ch; }

  const PayloadSpecDto &getPayloadSpec() { return m_payload_spec; }

  const ChannelSetDto &getChannels() { return m_channel_list; }

  const std::string &getFolderDescription() { return m_node_description; }
  size_t channelSize() const { return m_channel_list.size(); }
  size_t columnSize() const { return m_payload_spec.size(); }
  json toJson() const;
  static TagInfoDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_TAG_INFO_DTO_HPP
