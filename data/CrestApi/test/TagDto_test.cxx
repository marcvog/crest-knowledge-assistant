////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2025 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "CrestApi/CrestApi.h"
#include "CrestApi/TagSetDto.h"

using namespace Crest;

const std::string tagname = "test_ctest_tag_01";

TEST(CrestDtoTest, TagDtoTest) {
  std::string description = "test";
  std::string timeType = "time";
  std::string objectType = "JSON";
  std::string synchronization = "ALL";
  std::string status = "UNLOCKED";
  uint64_t validatedTime = 0;
  uint64_t endTime = 0;
  std::string insertionTime = "2018-12-06T11:18:35.641+0000";
  std::string modificationTime = "2018-12-06T11:20:35.641+0000";

  // Tag A creation using the constractor:
  TagDto dtoA(tagname, timeType, description);
  dtoA.setSynchronization(synchronization);
  dtoA.setObjectType(objectType);
  dtoA.setStatus(status);
  dtoA.setLastValidatedTime(validatedTime);
  dtoA.setEndOfValidity(endTime);

  // Parameters check for the tag A:
  EXPECT_EQ(tagname, dtoA.getName());
  EXPECT_EQ(timeType, dtoA.getTimeType());
  EXPECT_EQ(objectType, dtoA.getObjectType());
  EXPECT_EQ(status, dtoA.getStatus());
  EXPECT_EQ(synchronization, dtoA.getSynchronization());
  EXPECT_EQ(validatedTime, dtoA.getLastValidatedTime());
  EXPECT_EQ(endTime, dtoA.getEndOfValidity());

  // optional parameters:
  EXPECT_FALSE(dtoA.getInsertionTime().has_value());
  EXPECT_FALSE(dtoA.getModificationTime().has_value());

  dtoA.setInsertionTime(insertionTime);
  dtoA.setModificationTime(modificationTime);

  EXPECT_TRUE(dtoA.getInsertionTime().has_value());
  EXPECT_TRUE(dtoA.getModificationTime().has_value());
  EXPECT_EQ(insertionTime, dtoA.getInsertionTime());
  EXPECT_EQ(modificationTime, dtoA.getModificationTime());

  nlohmann::json jsA = dtoA.toJson();
  std::cout << "Tag A = " << std::endl << jsA.dump(4) << std::endl;

  // The tag B creation using a JSON:
  nlohmann::json jsB = {{"description", description},
                        {"endOfValidity", endTime},
                        {"insertionTime", insertionTime},
                        {"lastValidatedTime", validatedTime},
                        {"modificationTime", modificationTime},
                        {"name", tagname},
                        {"payloadSpec", objectType},
                        {"synchronization", synchronization},
                        {"timeType", timeType},
                        {"status", status}};

  TagDto dtoB = TagDto();
  dtoB = dtoB.fromJson(jsB);

  std::cout << "Tag B = " << std::endl << jsB.dump(4) << std::endl;

  // JSON check for both tags:
  nlohmann::json patch = nlohmann::json::diff(jsA, jsB);
  std::cout << "patch = " << patch << std::endl;
  EXPECT_TRUE(patch.empty()) << "The tags in test are not identical.";

  // The parameter check for both tags:
  EXPECT_EQ(dtoA.getName(), dtoB.getName());
  EXPECT_EQ(dtoA.getTimeType(), dtoB.getTimeType());
  EXPECT_EQ(dtoA.getObjectType(), dtoB.getObjectType());
  EXPECT_EQ(dtoA.getStatus(), dtoB.getStatus());
  EXPECT_EQ(dtoA.getSynchronization(), dtoB.getSynchronization());
  EXPECT_EQ(dtoA.getLastValidatedTime(), dtoB.getLastValidatedTime());
  EXPECT_EQ(dtoA.getEndOfValidity(), dtoB.getEndOfValidity());
  EXPECT_EQ(dtoA.getInsertionTime(), dtoB.getInsertionTime());
  EXPECT_EQ(dtoA.getModificationTime(), dtoB.getModificationTime());
}

TEST(CrestDtoTest, TagSetDtoTest) {
  std::string description = "test";
  std::string timeType = "time";
  std::string objectType = "JSON";
  std::string synchronization = "ALL";
  std::string status = "UNLOCKED";
  uint64_t validatedTime = 0;
  uint64_t endTime = 0;
  std::string insertionTime = "2018-12-06T11:18:35.641+0000";
  std::string modificationTime = "2018-12-06T11:20:35.641+0000";
  std::string format = "TagSetDto";

  // The tag A creation using a JSON:
  nlohmann::json jsA = {{"description", description},
                        {"endOfValidity", endTime},
                        {"insertionTime", insertionTime},
                        {"lastValidatedTime", validatedTime},
                        {"modificationTime", modificationTime},
                        {"name", tagname},
                        {"payloadSpec", objectType},
                        {"synchronization", synchronization},
                        {"timeType", timeType},
                        {"status", status}};

  TagDto dtoA = TagDto();
  dtoA = dtoA.fromJson(jsA);

  // nlohmann::json res = nlohmann::json::array();
  std::vector<TagDto> resA;
  resA.push_back(dtoA);
  TagSetDto tagSet(resA);
  std::cout << "tagSet = " << std::endl << tagSet.toJson().dump(4) << std::endl;

  // reading tag B from tag list (TagSetDto)
  std::vector<TagDto> resB = tagSet.getResources();
  TagDto dtoB = resB[0];

  // check for the tag number:
  EXPECT_EQ(tagSet.size(), 1);

  nlohmann::json patch = nlohmann::json::diff(jsA, dtoB.toJson());
  std::cout << "patch = " << patch << std::endl;
  EXPECT_TRUE(patch.empty()) << "The tags in test are not identical.";

  std::string tagSetFormat = tagSet.getFormat();
  std::cout << "Format = " << tagSetFormat << std::endl;
  EXPECT_EQ(format, tagSetFormat);

  // create a new TagSetDto with a JSON
  nlohmann::json resourcesJs = nlohmann::json::array();
  resourcesJs.push_back(jsA);
  nlohmann::json tagSetJs = {
      {"format", format}, {"resources", resourcesJs}, {"size", 1}};

  TagSetDto tagSetB = TagSetDto();
  tagSetB = tagSetB.fromJson(tagSetJs);

  // JSON check:
  nlohmann::json patchB =
      nlohmann::json::diff(tagSet.toJson(), tagSetB.toJson());
  std::cout << "patch = " << patchB << std::endl;
  EXPECT_TRUE(patchB.empty()) << "The TagSetDtos in test are not identical.";

  // append a tag:
  TagDto dtoC = dtoA;
  dtoC.setName(tagname + "B");
  tagSet.add(dtoC);
  EXPECT_EQ(tagSet.size(), 2);
}
