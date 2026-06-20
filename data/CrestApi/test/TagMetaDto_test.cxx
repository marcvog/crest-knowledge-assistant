////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2025 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

#include "CrestApi/TagMetaDto.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

using namespace Crest;

TEST(CrestDtoTest, TagMetaDtoTest) {
  std::string tagName = "test_ctest_tag_01";
  std::string description = "description 01";
  std::string tagInfo = "tag meta info 01";

  // empty tag meta:
  TagMetaDto tagMetaA = TagMetaDto();
  nlohmann::json jsA = tagMetaA.toJson();
  std::cout << " Empty Tag Meta = " << std::endl << jsA.dump(4) << std::endl;

  EXPECT_EQ("", tagMetaA.getTagName());
  EXPECT_EQ("", tagMetaA.getDescription());
  EXPECT_EQ("", tagMetaA.getTagInfo());

  // new parameters:
  tagMetaA.setTagName(tagName);
  tagMetaA.setDescription(description);
  tagMetaA.setTagInfo(tagInfo);

  // tag meta A:
  jsA = tagMetaA.toJson();
  std::cout << "Tag Meta A = " << std::endl << jsA.dump(4) << std::endl;

  //
  nlohmann::json jsB = {
      {"tagName", tagName}, {"description", description}, {"tagInfo", tagInfo}};

  // JSON check for tag meta info:
  nlohmann::json patch = nlohmann::json::diff(jsA, jsB);
  std::cout << "patch = " << patch << std::endl;
  EXPECT_TRUE(patch.empty()) << "JSON are not identical.";

  // tag meta B:
  TagMetaDto tagMetaB = TagMetaDto();
  tagMetaB = tagMetaB.fromJson(jsB);

  jsB = tagMetaA.toJson();
  std::cout << "Tag Meta B = " << std::endl << jsB.dump(4) << std::endl;

  // JSON check for tag meta info A & B:
  patch = nlohmann::json::diff(jsA, jsB);
  std::cout << "patch = " << patch << std::endl;
  EXPECT_TRUE(patch.empty()) << "JSON are not identical.";

  // parameter check for tag meta info A & B:
  EXPECT_EQ(tagMetaA.getTagName(), tagMetaB.getTagName());
  EXPECT_EQ(tagMetaA.getDescription(), tagMetaB.getDescription());
  EXPECT_EQ(tagMetaA.getTagInfo(), tagMetaB.getTagInfo());
}
