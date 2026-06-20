////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2025 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "CrestApi/CrestApi.h"
#include "CrestApi/IovSetDto.h"

using namespace Crest;

// const std::string tagname = "test_ctest_tag_01";

TEST(CrestDtoTest, IovDtoTest) {
  std::string tagname = "test_ctest_tag_01";
  uint64_t since = 123;
  std::string hash = "abcd";

  // empty IOV:
  IovDto iovA = IovDto();
  EXPECT_EQ("", iovA.getTagName());
  EXPECT_EQ(0, iovA.getSince());
  EXPECT_EQ("", iovA.getPayloadHash());

  std::cout << "Empty IOV = " << std::endl
            << iovA.toJson().dump(4) << std::endl;

  // setting parameters for IOV A:
  iovA.setSince(since);
  iovA.setTagName(tagname);
  iovA.setPayloadHash(hash);

  std::cout << "IOV A = " << std::endl << iovA.toJson().dump(4) << std::endl;

  // parameter check for IOV A:
  EXPECT_EQ(tagname, iovA.getTagName());
  EXPECT_EQ(since, iovA.getSince());
  EXPECT_EQ(hash, iovA.getPayloadHash());

  // IOV B with same paraneters:
  IovDto iovB = IovDto(tagname, since, hash);

  std::cout << "IOV B = " << std::endl << iovB.toJson().dump(4) << std::endl;

  // parameter check for IOV B:
  EXPECT_EQ(tagname, iovB.getTagName());
  EXPECT_EQ(since, iovB.getSince());
  EXPECT_EQ(hash, iovB.getPayloadHash());

  nlohmann::json jsC = {
      {"tagName", tagname}, {"since", since}, {"payloadHash", hash}};

  IovDto iovC = IovDto();
  iovC = iovC.fromJson(jsC);

  std::cout << "IOV C = " << std::endl << iovC.toJson().dump(4) << std::endl;

  // parameter check for IOV C:
  EXPECT_EQ(tagname, iovC.getTagName());
  EXPECT_EQ(since, iovC.getSince());
  EXPECT_EQ(hash, iovC.getPayloadHash());

  // check for JSONs:
  nlohmann::json jsA = iovA.toJson();
  nlohmann::json jsD = iovC.toJson();
  jsA.erase("insertionTime");
  jsC.erase("insertionTime");

  // patch calculation:
  nlohmann::json patch = nlohmann::json::diff(jsA, jsC);
  bool res = patch.empty();
  EXPECT_TRUE(res) << "The IOV JSONs are not identical.";
}

TEST(CrestDtoTest, IovSetDtoTest) {
  std::string tagname = "test_ctest_tag_01";
  uint64_t since = 123;
  std::string hash = "abcd";
  uint64_t sinceC = 345;
  std::string hashC = "fghij";
  std::string format = "IovSetDto";
  std::string insertionTime = "2018-12-06T11:18:35.641+0000";

  // IOV A:
  IovDto iovA = IovDto();
  iovA.setSince(since);
  iovA.setTagName(tagname);
  iovA.setPayloadHash(hash);
  iovA.setInsertionTime(insertionTime);

  std::cout << "IOV A = " << std::endl << iovA.toJson().dump(4) << std::endl;

  // IovSetDto A:
  IovSetDto iovSetA = IovSetDto();
  iovSetA.push_back(iovA);
  std::cout << "iovSetA = " << iovSetA.toJson().dump(4) << std::endl;

  // size check:
  EXPECT_EQ(iovSetA.size(), 1);

  // format check:
  std::cout << "IovSetDto format = " << iovSetA.getFormat() << std::endl;
  EXPECT_EQ(iovSetA.getFormat(), format);

  // check for an IOV:
  std::vector<IovDto> res = iovSetA.getResources();
  IovDto iovB = res[0];
  nlohmann::json patch1 = nlohmann::json::diff(iovA.toJson(), iovB.toJson());
  EXPECT_TRUE(patch1.empty()) << "The IOVs are not identical.";

  // IovSetDto B (from JSON):
  nlohmann::json resourcesJs = nlohmann::json::array();

  nlohmann::json iovJs = {{"tagName", tagname},
                          {"since", since},
                          {"payloadHash", hash},
                          {"insertionTime", insertionTime}};

  resourcesJs.push_back(iovJs);
  nlohmann::json iovSetJs = {
      {"format", format}, {"resources", resourcesJs}, {"size", 1}};

  IovSetDto iovSetB = IovSetDto();
  iovSetB = iovSetB.fromJson(iovSetJs);

  // JSON check for both IovSetDto:
  nlohmann::json patch2 =
      nlohmann::json::diff(iovSetA.toJson(), iovSetB.toJson());
  std::cout << "IovSetDto patch = " << std::endl << patch2 << std::endl;
  EXPECT_TRUE(patch2.empty()) << "The iovSetDtos are not identical.";

  // iov C:
  IovDto iovC = iovA;
  iovC.setSince(sinceC);
  iovC.setPayloadHash(hashC);
  iovSetA.push_back(iovC);

  // size check:
  EXPECT_EQ(iovSetA.size(), 2);
}
