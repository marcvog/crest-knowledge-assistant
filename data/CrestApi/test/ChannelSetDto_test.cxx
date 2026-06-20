////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2026 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

#include "CrestApi/ChannelSetDto.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

using namespace Crest;

TEST(CrestDtoTest, ChannelSetDtoTest) {
  std::string chan[] = {"200", "300", "400"};
  std::string name[] = {"a1", "a2", "a3"};

  // Empty ChannelSetDto:
  ChannelSetDto chansetA;
  EXPECT_EQ(chansetA.size(), 0);

  for (int i = 0; i <= 2; ++i) {
    chansetA.add(chan[i], name[i]);
  }

  std::cout << "ChannelSetDto A = " << std::endl
            << chansetA.toJson().dump(4) << std::endl;

  EXPECT_EQ(chansetA.size(), 3);

  // ChannelSetDto B:
  nlohmann::json jsB = nlohmann::json::array();

  for (int i = 0; i <= 2; ++i) {
    nlohmann::json j;
    j[chan[i]] = name[i];
    jsB.push_back(j);
  }

  std::cout << "JSON B = " << std::endl << jsB.dump(4);

  ChannelSetDto chansetB;
  chansetB = chansetB.fromJson(jsB);

  std::cout << "ChannelSetDto B = " << std::endl
            << chansetB.toJson().dump(4) << std::endl;

  // JSON check for both ChannelSetDtos:
  nlohmann::json patch =
      nlohmann::json::diff(chansetA.toJson(), chansetB.toJson());
  std::cout << "patch = " << patch << std::endl;
  EXPECT_TRUE(patch.empty()) << "The ChannelSetDtos in test are not identical.";

  // ChannelSetDto C:
  ChannelSetDto chansetC = chansetA;
  std::cout << "ChannelSetDto C = " << std::endl
            << chansetC.toJson().dump(4) << std::endl;

  // checking the parameters:
  std::vector<std::pair<std::string, std::string>> dataVector =
      chansetC.getChannels();
  for (int i = 0; i <= 2; ++i) {
    std::pair<std::string, std::string> p = dataVector[i];
    EXPECT_EQ(p.first, chan[i]);
    EXPECT_EQ(p.second, name[i]);
  }
}
