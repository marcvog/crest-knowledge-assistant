/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_CHAN_SET_DTO_HPP
#define CREST_CHAN_SET_DTO_HPP

#include <CrestApi/CrestException.h>

#include <optional>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class ChannelSetDto {
 private:
  std::vector<std::pair<std::string, std::string>> m_channels;

 public:
  ChannelSetDto &operator=(const ChannelSetDto &) = default;
  ChannelSetDto &operator=(ChannelSetDto &&) = default;

  ChannelSetDto() {}
  ChannelSetDto(ChannelSetDto const &copy) : m_channels(copy.getChannels()) {}
  void add(const std::string &id, const std::string &name);
  const std::vector<std::pair<std::string, std::string>> &getChannels() const {
    return m_channels;
  }
  size_t size() const { return m_channels.size(); }
  json toJson() const;
  static ChannelSetDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_CHAN_SET_DTO_HPP
