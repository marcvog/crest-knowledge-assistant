////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2025 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

#include "CrestApi/GlobalTagMapDto.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "CrestApi/CrestApi.h"
#include "CrestApi/GlobalTagMapSetDto.h"

using namespace Crest;

TEST(CrestDtoTest, GlobalTagMapDtoTest) {
  std::string tagName = "test_ctest_tag_01";
  std::string globalTagName = "TEST_GLOBAL_TAG_01";
  std::string record = "record-01";
  std::string label = "label-01";

  // empty global tag map:
  GlobalTagMapDto mapA = GlobalTagMapDto();
  EXPECT_EQ("", mapA.getTagName());
  EXPECT_EQ("", mapA.getGlobalTagName());
  EXPECT_EQ("none", mapA.getRecord());
  EXPECT_EQ("none", mapA.getLabel());

  std::cout << "The empty global tag map = " << std::endl
            << mapA.toJson().dump(4) << std::endl;

  // global tag map A:
  mapA.setLabel(label);
  EXPECT_EQ(label, mapA.getLabel());

  std::cout << "The global tag map A = " << std::endl
            << mapA.toJson().dump(4) << std::endl;

  // global tag map B:
  GlobalTagMapDto mapB = GlobalTagMapDto(tagName, globalTagName, record);
  std::cout << "The global tag map B = " << std::endl
            << mapB.toJson().dump(4) << std::endl;

  // parameter check for global tag map B:
  EXPECT_EQ(tagName, mapB.getTagName());
  EXPECT_EQ(globalTagName, mapB.getGlobalTagName());
  EXPECT_EQ(record, mapB.getRecord());
  EXPECT_EQ("none", mapB.getLabel());

  // global tag map C:
  GlobalTagMapDto mapC = GlobalTagMapDto(tagName, globalTagName, record, label);
  std::cout << "The global tag map C = " << std::endl
            << mapC.toJson().dump(4) << std::endl;

  nlohmann::json jsC = mapC.toJson();

  // global tag map D:
  nlohmann::json jsD = {{"tagName", tagName},
                        {"globalTagName", globalTagName},
                        {"record", record},
                        {"label", label}};

  GlobalTagMapDto mapD = GlobalTagMapDto();
  mapD = mapD.fromJson(jsD);

  // parameter chech for global tag map C & D:
  EXPECT_EQ(mapC.getTagName(), mapD.getTagName());
  EXPECT_EQ(mapC.getGlobalTagName(), mapD.getGlobalTagName());
  EXPECT_EQ(mapC.getRecord(), mapD.getRecord());
  EXPECT_EQ(mapC.getLabel(), mapD.getLabel());

  // JSON check for global tag map C & D:
  nlohmann::json patch = nlohmann::json::diff(jsC, jsD);
  std::cout << "patch = " << std::endl << patch.dump(4) << std::endl;
  EXPECT_TRUE(patch.empty())
      << "The global tag maps in test are not identical.";
}

TEST(CrestDtoTest, GlobalTagSetDtoTest) {
  std::string tagName = "test_ctest_tag_01";
  std::string globalTagName = "TEST_GLOBAL_TAG_01";
  std::string record = "record-01";
  std::string label = "label-01";

  GlobalTagMapSetDto dto;
  nlohmann::json maps = json::array();

  // global tag map:
  GlobalTagMapDto mapA = GlobalTagMapDto(tagName, globalTagName, record, label);
  std::cout << "The global tag map = " << std::endl
            << mapA.toJson().dump(4) << std::endl;

  // nlohmann::json = jsA; // JSON with resources
  maps.push_back(mapA.toJson());
  dto = GlobalTagMapSetDto::fromFsJson(maps);

  EXPECT_EQ(dto.size(), 1);

  // check for a single map:
  nlohmann::json jsA = mapA.toJson();
  std::vector<GlobalTagMapDto> res = dto.getResources();
  nlohmann::json jsB = res[0].toJson();

  // JSON check for both tags:
  nlohmann::json patch1 = nlohmann::json::diff(jsA, jsB);
  EXPECT_TRUE(patch1.empty()) << "The maps in test are not identical.";

  // getFormat check:
  std::string format = dto.getFormat();
  std::cout << "format = " << format << std::endl;
  EXPECT_EQ("GlobalTagMapSetDto", dto.getFormat());

  // toJson check:
  nlohmann::json js = dto.toJson();
  nlohmann::json jsC;
  if (js.contains("resources")) {
    jsC = js["resources"][0];
  }
  // JSON check for both tags:
  nlohmann::json patch2 = nlohmann::json::diff(jsA, jsC);
  EXPECT_TRUE(patch2.empty()) << "The maps in test are not identical.";
}
