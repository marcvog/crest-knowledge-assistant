////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2026 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "CrestApi/PayloadTagInfoSetDto.h"

using namespace Crest;

TEST(CrestDtoTest, PayloadTagInfoDtoTest) {
  std::string tag_name = "AF-TileOfl02CalibLasNln-COM-00";
  int niovs = 2;
  double totvolume = 3043.0;
  double avgvolume = 1521.0;

  // Empty PayloadTagInfoDto:
  PayloadTagInfoDto pTagInfoA;
  EXPECT_EQ(pTagInfoA.getTagName(), "");
  EXPECT_EQ(pTagInfoA.getNiovs(), 0);
  EXPECT_EQ(pTagInfoA.getTotVolume(), 0.0);
  EXPECT_EQ(pTagInfoA.getAvgVolume(), 0.0);

  // set parameters:
  pTagInfoA.setTagName(tag_name);
  pTagInfoA.setNiovs(niovs);
  pTagInfoA.setTotVolume(totvolume);
  pTagInfoA.setAvgVolume(avgvolume);

  std::cout << "PayloadTagInfoDto A = " << std::endl
            << pTagInfoA.toJson().dump(4) << std::endl;

  // Parameter check:
  EXPECT_EQ(pTagInfoA.getTagName(), tag_name);
  EXPECT_EQ(pTagInfoA.getNiovs(), niovs);
  EXPECT_EQ(pTagInfoA.getTotVolume(), totvolume);
  EXPECT_EQ(pTagInfoA.getAvgVolume(), avgvolume);

  // PayloadTagInfoDto B (from JSON):
  nlohmann::json jsB = {{"tagname", tag_name},
                        {"niovs", niovs},
                        {"totvolume", totvolume},
                        {"avgvolume", avgvolume}};

  PayloadTagInfoDto pTagInfoB;
  pTagInfoB = pTagInfoB.fromJson(jsB);

  // JSON check:
  nlohmann::json patchA =
      nlohmann::json::diff(pTagInfoA.toJson(), pTagInfoB.toJson());
  std::cout << "patch = " << patchA << std::endl;
  EXPECT_TRUE(patchA.empty())
      << "The PayloadTagInfoDtos in test are not identical.";

  // PayloadTagInfoDto C (setting parameters with constructor):
  PayloadTagInfoDto pTagInfoC(tag_name, niovs, totvolume, avgvolume);
  std::cout << "PayloadTagInfoDto C = " << std::endl
            << pTagInfoC.toJson().dump(4) << std::endl;

  // Parameter check:
  EXPECT_EQ(pTagInfoA.getTagName(), pTagInfoC.getTagName());
  EXPECT_EQ(pTagInfoA.getNiovs(), pTagInfoC.getNiovs());
  EXPECT_EQ(pTagInfoA.getTotVolume(), pTagInfoC.getTotVolume());
  EXPECT_EQ(pTagInfoA.getAvgVolume(), pTagInfoC.getAvgVolume());
}

TEST(CrestDtoTest, PayloadTagInfoSetDtoTest) {
  // Parameters for PayloadTagInfoDtos:
  std::string tag_name[] = {"AF-TileOfl02CalibLasNln-COM-AA",
                            "AF-TileOfl02CalibLasNln-COM-BB"};
  int niovs[] = {2, 3};
  double totvolume[] = {3043.0, 5670.8};
  double avgvolume[] = {1521.0, 2560, 4};

  nlohmann::json res = nlohmann::json::array();

  for (int i = 0; i <= 1; ++i) {
    // PayloadTagInfoDto JSON:
    nlohmann::json js = {{"tagname", tag_name[i]},
                         {"niovs", niovs[i]},
                         {"totvolume", totvolume[i]},
                         {"avgvolume", avgvolume[i]}};
    res.push_back(js);
  }

  nlohmann::json pTagInfoJsA = {{"resources", res}, {"size", 2}};

  PayloadTagInfoSetDto pTagInfoA;
  pTagInfoA = pTagInfoA.fromJson(pTagInfoJsA);

  std::cout << "PayloadTagInfoSetDto = " << std::endl
            << pTagInfoA.toJson().dump(4) << std::endl;

  EXPECT_EQ(pTagInfoA.size(), 2);
  EXPECT_EQ(pTagInfoA.getFormat(), "PayloadTagInfoSetDto");

  // Parameter check:
  std::vector<PayloadTagInfoDto> dataVector = pTagInfoA.getResources();
  EXPECT_EQ(pTagInfoA.size(), dataVector.size());
  for (int i = 0; i <= 1; ++i) {
    EXPECT_EQ(tag_name[i], dataVector[i].getTagName());
    EXPECT_EQ(niovs[i], dataVector[i].getNiovs());
    EXPECT_EQ(totvolume[i], dataVector[i].getTotVolume());
    EXPECT_EQ(avgvolume[i], dataVector[i].getAvgVolume());
  }
}
