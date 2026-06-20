////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2026 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "CrestApi/PayloadSetDto.h"

using namespace Crest;

TEST(CrestDtoTest, PayloadDtoTest) {
  // Parameters:
  std::string hash =
      "8a259907966b2bed592ec61367c57c893674d57b2f6e83310b6480dcd4d6f925";
  std::string version = "default";
  std::string objectType = "JSON";
  std::string objectName = "none";
  std::string compressionType = "none";
  std::string checkSum = "SHA-256";
  std::size_t size = 3357;
  std::string insertionTime = "2025-01-17T14:05:07+0000";

  // Empty PayloadDto:
  PayloadDto pdtoA;
  std::cout << "Empty PayloadDto = " << std::endl
            << pdtoA.toJson().dump(4) << std::endl;

  EXPECT_EQ(pdtoA.size(), 0);
  EXPECT_EQ(pdtoA.getHash(), "");
  EXPECT_EQ(pdtoA.getVersion(), "");
  EXPECT_EQ(pdtoA.getObjectType(), "");
  EXPECT_EQ(pdtoA.getObjectName(), "");
  EXPECT_EQ(pdtoA.getCompressionType(), "");
  EXPECT_EQ(pdtoA.getCheckSum(), "");

  // Set parameters:
  pdtoA.setHash(hash);
  pdtoA.size(size);
  pdtoA.setVersion(version);
  pdtoA.setObjectType(objectType);
  pdtoA.setObjectName(objectName);
  pdtoA.setCompressionType(compressionType);
  pdtoA.setCheckSum(checkSum);

  std::cout << "PayloadDto A = " << std::endl
            << pdtoA.toJson().dump(4) << std::endl;

  // PayloadDto B from JSON:
  nlohmann::json jsB = {{"hash", hash},
                        {"size", size},
                        {"version", version},
                        {"objectType", objectType},
                        {"objectName", objectName},
                        {"compressionType", compressionType},
                        {"checkSum", checkSum}};

  PayloadDto pdtoB;
  pdtoB = pdtoB.fromJson(jsB);

  std::cout << "PayloadDto B = " << std::endl
            << pdtoB.toJson().dump(4) << std::endl;

  // Check the parameters for both PayloadDtos:
  EXPECT_EQ(pdtoA.size(), pdtoB.size());
  EXPECT_EQ(pdtoA.getHash(), pdtoB.getHash());
  EXPECT_EQ(pdtoA.getVersion(), pdtoB.getVersion());
  EXPECT_EQ(pdtoA.getObjectType(), pdtoB.getObjectType());
  EXPECT_EQ(pdtoA.getObjectName(), pdtoB.getObjectName());
  EXPECT_EQ(pdtoA.getCompressionType(), pdtoB.getCompressionType());
  EXPECT_EQ(pdtoA.getCheckSum(), pdtoB.getCheckSum());

  // JSON check:
  nlohmann::json patchA = nlohmann::json::diff(pdtoA.toJson(), pdtoB.toJson());
  std::cout << "patch = " << patchA << std::endl;
  EXPECT_TRUE(patchA.empty()) << "The PayloadDtos in test are not identical.";
}

TEST(CrestDtoTest, PayloadSetDtoTest) {
  // Parameters:
  std::string hash[] = {
      "8a259907966b2bed592ec61367c57c893674d57b2f6e83310b6480dcd4d6f925",
      "43e286c242b25db446d89826c0fd7d52afe67ff36c84f68ed1b7b7e78b57cf7c"};
  std::string version[] = {"default", "first"};
  std::string objectType[] = {"JSON", "JSON"};
  std::string objectName[] = {"none", "aaa"};
  std::string compressionType[] = {"none", "ZIP"};
  std::string checkSum[] = {"SHA-256", "SHA-256"};
  std::size_t size[] = {3357, 4567};
  std::string insertionTime[] = {"2025-01-17T14:05:07+0000",
                                 "2025-02-17T14:05:07+0000"};

  nlohmann::json res = nlohmann::json::array();

  for (int i = 0; i <= 1; ++i) {
    // PayloadDto JSON:
    nlohmann::json js = {{"hash", hash[i]},
                         {"size", size[i]},
                         {"version", version[i]},
                         {"objectType", objectType[i]},
                         {"objectName", objectName[i]},
                         {"compressionType", compressionType[i]},
                         {"checkSum", checkSum[i]},
                         {"insertionTime", insertionTime[i]}};
    res.push_back(js);
  }
  nlohmann::json pdtoAjs = {{"resources", res}, {"size", 2}};

  std::cout << "PayloadSetDto JSON = " << std::endl
            << pdtoAjs.dump(4) << std::endl;

  PayloadSetDto psetA;
  psetA = psetA.fromJson(pdtoAjs);

  std::cout << "PayloadSetDto = " << std::endl
            << psetA.toJson().dump(4) << std::endl;

  EXPECT_EQ(psetA.size(), 2);
  EXPECT_EQ(psetA.getFormat(), "PayloadSetDto");

  // Parameter check:
  std::vector<PayloadDto> dataVector = psetA.getResources();
  for (int i = 0; i <= 1; ++i) {
    EXPECT_EQ(dataVector[i].size(), size[i]);
    EXPECT_EQ(dataVector[i].getHash(), hash[i]);
    EXPECT_EQ(dataVector[i].getVersion(), version[i]);
    EXPECT_EQ(dataVector[i].getObjectType(), objectType[i]);
    EXPECT_EQ(dataVector[i].getObjectName(), objectName[i]);
    EXPECT_EQ(dataVector[i].getCompressionType(), compressionType[i]);
    EXPECT_EQ(dataVector[i].getCheckSum(), checkSum[i]);
  }
}
