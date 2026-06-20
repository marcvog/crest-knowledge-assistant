////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2025 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

#include "CrestApi/PayloadSpecDto.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

using namespace Crest;

TEST(CrestDtoTest, PayloadSpecDtoTest) {

  std::string name[] = {"ModuleId", "ErrorCode", "json"};
  std::string type[] = {"UInt32", "UInt32", "Blob64"};

  // PayloadSpec A (empty):
  PayloadSpecDto specA;
  EXPECT_EQ(specA.size(), 0);

  // set parameters:
  specA.add(name[0], type[0]);
  specA.add(name[1], type[1]);
  specA.add(name[2], type[2]);

  std::cout << "PayloadSpec A = " << std::endl
            << specA.toJson().dump(4) << std::endl;

  EXPECT_EQ(specA.size(), 3);

  // PayloadSpec B
  nlohmann::json jsB = nlohmann::json::array();

  for (int i = 0; i <= 2; ++i) {
    nlohmann::json j;
    j[name[i]] = type[i];
    jsB.push_back(j);
  }

  PayloadSpecDto specB = PayloadSpecDto();
  specB = specB.fromJson(jsB);
  std::cout << "PayloadSpec B = " << std::endl
            << specB.toJson().dump(4) << std::endl;

  // JSON check for both PayloadSpecs:
  nlohmann::json patch = nlohmann::json::diff(specA.toJson(), specB.toJson());
  std::cout << "patch = " << patch << std::endl;
  EXPECT_TRUE(patch.empty()) << "The PayloadSpecs in test are not identical.";

  // PayloadSpec C:
  PayloadSpecDto specC = specA;
  std::cout << "PayloadSpec C = " << std::endl
            << specC.toJson().dump(4) << std::endl;

  std::vector<std::pair<std::string, std::string>> dataVector =
      specC.getColumns();
  for (int i = 0; i <= 2; ++i) {
    std::pair<std::string, std::string> p = dataVector[i];
    EXPECT_EQ(p.first, name[i]);
    EXPECT_EQ(p.second, type[i]);
  }
}
