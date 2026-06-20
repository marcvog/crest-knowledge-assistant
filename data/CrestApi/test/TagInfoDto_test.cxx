////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2026 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

#include "CrestApi/TagInfoDto.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

using namespace Crest;

TEST(CrestDtoTest, TagInfoDtoTest) {
  // PayloadSpec:
  std::string name[] = {"ModuleId", "ErrorCode", "json"};
  std::string type[] = {"UInt32", "UInt32", "Blob64"};

  PayloadSpecDto spec;

  for (int i = 0; i <= 2; ++i) {
    spec.add(name[i], type[i]);
  }

  // Channel list:
  std::string chan[] = {"200", "300", "400"};
  std::string ch_name[] = {"a1", "a2", "a3"};

  // Empty ChannelSetDto:
  ChannelSetDto chanset;

  for (int i = 0; i <= 2; ++i) {
    chanset.add(chan[i], ch_name[i]);
  }

  std::string description =
      "<timeStamp>time</timeStamp><addrHeader><address_header "
      "service_type=\"71\" "
      "clid=\"1238547719\"/></addrHeader><typeName>CondAttrListCollection</"
      "typeName>";

  // Empty Tag Info:
  TagInfoDto taginfoA(description);
  EXPECT_EQ(taginfoA.getFolderDescription(), description);
  EXPECT_EQ(taginfoA.getChannels().size(), 0);
  EXPECT_EQ(taginfoA.getPayloadSpec().size(), 0);

  taginfoA.setPayloadSpec(spec);
  taginfoA.setChannel(chanset);

  std::cout << "Tag Info A = " << std::endl
            << taginfoA.toJson().dump(4) << std::endl;

  // Parameter check:
  PayloadSpecDto specB = taginfoA.getPayloadSpec();
  EXPECT_EQ(specB.size(), 3);

  std::vector<std::pair<std::string, std::string>> payloadVector =
      specB.getColumns();
  for (int i = 0; i <= 2; ++i) {
    EXPECT_EQ(payloadVector[i].first, name[i]);
    EXPECT_EQ(payloadVector[i].second, type[i]);
  }

  ChannelSetDto chansetB = taginfoA.getChannels();
  std::vector<std::pair<std::string, std::string>> channelVector =
      chansetB.getChannels();
  for (int i = 0; i <= 2; ++i) {
    EXPECT_EQ(channelVector[i].first, chan[i]);
    EXPECT_EQ(channelVector[i].second, ch_name[i]);
  }

  // JSON test:

  // PayloadSpec JSON:
  nlohmann::json spec_js = nlohmann::json::array();
  for (int i = 0; i <= 2; ++i) {
    nlohmann::json j;
    j[name[i]] = type[i];
    spec_js.push_back(j);
  }

  // ChannelSetDto JSON:
  nlohmann::json chan_js = nlohmann::json::array();

  for (int i = 0; i <= 2; ++i) {
    nlohmann::json j;
    j[chan[i]] = ch_name[i];
    chan_js.push_back(j);
  }

  nlohmann::json jsC = {{"channel_list", chan_js},
                        {"node_description", description},
                        {"payload_spec", spec_js}};

  std::cout << "Tag Info JSON: " << std::endl << jsC.dump(4) << std::endl;

  TagInfoDto taginfoC("test");
  taginfoC = taginfoC.fromJson(jsC);

  nlohmann::json patch =
      nlohmann::json::diff(taginfoA.toJson(), taginfoC.toJson());
  std::cout << "patch = " << patch << std::endl;
  EXPECT_TRUE(patch.empty()) << "The TagInfoDtos in test are not identical.";

  // parameter check for both DTOs:
  EXPECT_EQ(taginfoA.getFolderDescription(), taginfoC.getFolderDescription());
  EXPECT_EQ(taginfoA.getChannels().size(), taginfoC.getChannels().size());
  EXPECT_EQ(taginfoA.getPayloadSpec().size(), taginfoC.getPayloadSpec().size());

  PayloadSpecDto specC = taginfoC.getPayloadSpec();
  std::vector<std::pair<std::string, std::string>> payloadVectorC =
      specC.getColumns();
  for (int i = 0; i <= 2; ++i) {
    EXPECT_EQ(payloadVector[i].first, payloadVectorC[i].first);
    EXPECT_EQ(payloadVector[i].second, payloadVectorC[i].second);
  }

  ChannelSetDto chansetC = taginfoC.getChannels();
  std::vector<std::pair<std::string, std::string>> channelVectorC =
      chansetC.getChannels();
  for (int i = 0; i <= 2; ++i) {
    EXPECT_EQ(channelVector[i].first, channelVectorC[i].first);
    EXPECT_EQ(channelVector[i].second, channelVectorC[i].second);
  }
}
