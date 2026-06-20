////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2025 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "CrestApi/CrestApi.h"
#include "CrestApi/StoreSetDto.h"

using namespace Crest;

TEST(CrestDtoTest, StoreDtoTest) {
  uint64_t since = 1000;
  std::string hash = "abcde";
  std::string data = "my test data";
  std::string insertionTime = "2018-12-06T11:18:35.641+0000";
  const char* streamer_info = "streamer-01";

  // empty StoreDto:
  StoreDto sdtoA;  // = StoreDto();
  std::cout << "empty StoreDto = " << sdtoA.toJson().dump(4) << std::endl;

  EXPECT_EQ(sdtoA.getSince(), 0);
  EXPECT_EQ(sdtoA.getHash(), "");
  EXPECT_EQ(sdtoA.getData(), "");
  // EXPECT_EQ(sdtoA.getStreamerInfo(), "empty");
  std::cout << "streamer info for the empty StoreDto = "
            << sdtoA.getStreamerInfo() << std::endl;

  // StoreDto B:
  StoreDto sdtoB(since, hash, data);
  sdtoB.setStreamerInfo(streamer_info);
  EXPECT_EQ(sdtoB.getSince(), since);
  EXPECT_EQ(sdtoB.getHash(), hash);
  EXPECT_EQ(sdtoB.getData(), data);
  EXPECT_EQ(sdtoB.getStreamerInfo(), streamer_info);  //

  // StoreDto C:
  nlohmann::json jsC = {{"since", since},
                        {"data", data},
                        {"hash", hash},
                        {"streamerInfo", streamer_info}};

  StoreDto sdtoC;
  sdtoC = sdtoC.fromJson(jsC);

  // Parameter check for StoreDto B & C:
  EXPECT_EQ(sdtoB.getSince(), sdtoC.getSince());
  EXPECT_EQ(sdtoB.getHash(), sdtoC.getHash());
  EXPECT_EQ(sdtoB.getData(), sdtoC.getData());
  EXPECT_EQ(sdtoB.getStreamerInfo(), sdtoC.getStreamerInfo());

  // JSON check for StoreDto B & C:
  nlohmann::json patch = nlohmann::json::diff(sdtoB.toJson(), sdtoC.toJson());
  std::cout << "JSON patch = " << patch << std::endl;
  EXPECT_TRUE(patch.empty()) << "The StoreDtos in test are not identical.";
}

TEST(CrestDtoTest, StoreSetDtoTest) {
  uint64_t since = 1000;
  std::string hash = "abcde";
  std::string data = "my test data";
  std::string insertionTime = "2018-12-06T11:18:35.641+0000";
  const char* streamer_info = "streamer-01";
  std::string format = "StoreSetDto";

  // StoreDto A:
  StoreDto sdtoA(since, hash, data);
  sdtoA.setStreamerInfo(streamer_info);

  // empty StoreSetDto:
  StoreSetDto ssDtoA;
  EXPECT_EQ(ssDtoA.size(), 0);

  // add a StoreDto:
  ssDtoA.push_back(sdtoA);
  EXPECT_EQ(ssDtoA.size(), 1);

  std::cout << "StoreSetDto A = " << ssDtoA.toJson().dump(4) << std::endl;

  // StoreDto B:
  nlohmann::json jsB = {{"since", since},
                        {"data", data},
                        {"hash", hash},
                        {"streamerInfo", streamer_info}};

  nlohmann::json resourcesJs = nlohmann::json::array();
  resourcesJs.push_back(jsB);
  nlohmann::json ssDtoJsB = {
      {"format", format}, {"size", 1}, {"resources", resourcesJs}};
  StoreSetDto ssDtoB;
  ssDtoB = ssDtoB.fromJson(ssDtoJsB);
  std::cout << "StoreSetDto B = " << ssDtoB.toJson().dump(4) << std::endl;

  // JSON check:
  nlohmann::json patch = nlohmann::json::diff(ssDtoA.toJson(), ssDtoB.toJson());
  std::cout << "patch = " << patch << std::endl;
  EXPECT_TRUE(patch.empty()) << "The StoreSetDtos in test are not identical.";

  // StoreSetDto cleaning:
  ssDtoA.clear();
  EXPECT_EQ(ssDtoA.size(), 0) << "StoreSetDto was not correctly cleared.";
}
