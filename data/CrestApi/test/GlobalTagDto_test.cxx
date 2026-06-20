////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2025 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "CrestApi/CrestApi.h"
#include "CrestApi/GlobalTagSetDto.h"

using namespace Crest;

const std::string global_tag = "TEST_GLOBAL_TAG_01";

TEST(CrestDtoTest, GlobalTagDtoTest) {
  std::string description = "my test";
  std::string release = "1";
  std::string type = "T";
  std::string scenario = "test";
  std::string workflow = "M";
  int64_t validity = 1;

  std::string insertionTime = "2018-12-18T11:32:58.081+0000";
  std::string snapshotTime = "2020-12-18T11:32:58.081+0000";

  GlobalTagDto dtoA(global_tag, description, release, workflow);
  dtoA.setValidity(0);
  dtoA.setType(type);
  dtoA.setScenario(scenario);
  dtoA.setValidity(validity);

  // optional parameters:
  EXPECT_FALSE(dtoA.getInsertionTime().has_value());
  EXPECT_FALSE(dtoA.getSnapshotTime().has_value());

  // dtoA.setInsertionTime(insertionTime);
  dtoA.setSnapshotTime(snapshotTime);

  nlohmann::json jsA = dtoA.toJson();
  std::cout << "Global Tag A = " << std::endl << jsA.dump(4) << std::endl;

  nlohmann::json jsB = {
      {"name", global_tag},
      {"validity", validity},
      {"description", description},
      {"release", release},
      //{"insertionTime", insertionTime},
      {"snapshotTime", snapshotTime},
      {"scenario", scenario},
      {"workflow", workflow},
      {"type", type},
  };

  GlobalTagDto dtoB = GlobalTagDto();
  dtoB = dtoB.fromJson(jsB);
  std::cout << "Global Tag B = " << std::endl
            << dtoB.toJson().dump(4) << std::endl;

  // Parameters check for the global tag A:
  EXPECT_EQ(global_tag, dtoA.getName());
  EXPECT_EQ(description, dtoA.getDescription());
  EXPECT_EQ(release, dtoA.getRelease());
  EXPECT_EQ(validity, dtoA.getValidity());
  EXPECT_EQ(scenario, dtoA.getScenario());
  EXPECT_EQ(workflow, dtoA.getWorkflow());
  EXPECT_EQ(type, dtoA.getType());

  // JSON check for both global tags:
  nlohmann::json patch = nlohmann::json::diff(jsA, dtoB.toJson());
  std::cout << "JSON patch = " << std::endl << patch.dump(4) << std::endl;
  bool res = patch.empty();
  EXPECT_TRUE(res) << "The tags in test are not identical.";

  // Parameters check for both global tags:
  EXPECT_EQ(dtoA.getName(), dtoB.getName());
  EXPECT_EQ(dtoA.getDescription(), dtoB.getDescription());
  EXPECT_EQ(dtoA.getRelease(), dtoB.getRelease());
  EXPECT_EQ(dtoA.getValidity(), dtoB.getValidity());
  EXPECT_EQ(dtoA.getScenario(), dtoB.getScenario());
  EXPECT_EQ(dtoA.getWorkflow(), dtoA.getWorkflow());
  EXPECT_EQ(dtoA.getType(), dtoB.getType());
}

TEST(CrestDtoTest, GlobalTagSetDtoTest) {
  std::string description = "my test";
  std::string release = "1";
  std::string type = "T";
  std::string scenario = "test";
  std::string workflow = "M";
  int64_t validity = 1;

  std::string insertionTime = "2018-12-18T11:32:58.081+0000";
  std::string snapshotTime = "2020-12-18T11:32:58.081+0000";

  std::string format = "GlobalTagSetDto";

  nlohmann::json jsA = {
      {"name", global_tag},
      {"validity", validity},
      {"description", description},
      {"release", release},
      //{"insertionTime", insertionTime},
      {"snapshotTime", snapshotTime},
      {"scenario", scenario},
      {"workflow", workflow},
      {"type", type},
  };

  GlobalTagDto dtoA = GlobalTagDto();
  dtoA = dtoA.fromJson(jsA);

  // GlobalTagSetDto A:
  GlobalTagSetDto tagSet = GlobalTagSetDto();
  tagSet.add(dtoA);

  // check for the global tag number:
  EXPECT_EQ(tagSet.size(), 1);

  // reading global tag B from tag list (TagSetDto)
  std::vector<GlobalTagDto> resB = tagSet.getResources();
  GlobalTagDto dtoB = resB[0];

  // JSON check for both global tags:
  nlohmann::json patch = nlohmann::json::diff(dtoA.toJson(), dtoB.toJson());
  std::cout << "patch = " << patch << std::endl;
  EXPECT_TRUE(patch.empty()) << "The tags in test are not identical.";

  // create a new TagSetDto with a JSON
  nlohmann::json resourcesJs = nlohmann::json::array();
  resourcesJs.push_back(jsA);
  nlohmann::json tagSetJs = {
      {"format", format}, {"resources", resourcesJs}, {"size", 1}};

  // GlobalTagSetDto 2:
  GlobalTagSetDto tagSet2 = GlobalTagSetDto();
  tagSet2 = tagSet2.fromJson(tagSetJs);

  // JSON check for both global tags:
  nlohmann::json patch2 =
      nlohmann::json::diff(tagSet.toJson(), tagSet2.toJson());
  std::cout << "GlobalTagSetDto patch = " << patch2 << std::endl;
  EXPECT_TRUE(patch2.empty())
      << "The GlobalTagSetDtos in test are not identical.";

  // global tag C:
  GlobalTagDto dtoC = dtoA;
  dtoC.setName(global_tag + "B");
  tagSet.add(dtoC);

  // check for the global tag number:
  EXPECT_EQ(tagSet.size(), 2);

  // check format:
  std::string tagSetFormat = tagSet.getFormat();
  std::cout << "Format = " << tagSetFormat << std::endl;
  EXPECT_EQ(format, tagSetFormat);
}
