////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2026 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

#include "CrestApi/CrestApi.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "test_utils.h"

const std::string tagname = "test_ctest_tag_01";
const std::string global_tag = "TEST_GLOBAL_TAG_01";

////////////////////////////////////////////////////////////////////////////////
/// @brief  Test suite for the CREST clients
////////////////////////////////////////////////////////////////////////////////
class crestApiTest : public MultiClientTestFixture {};

TEST_P(crestApiTest, ClientVersionTest) {
  ASSERT_NO_THROW(std::string version = myCrestApi->getClientVersion();
                  std::cout << "CrestApi: client version = " << version
                            << std::endl;);
}

/*
TEST_P(crestApiTest, ServerVersionTest) {
  // Filesystem plugin doesn't support getCrestVersion
  if (storage == "CrestFilesystem") {
    GTEST_SKIP() << "CrestApiFs does not support getCrestVersion()";
  }

  ASSERT_NO_THROW(
    std::string version =  myCrestApi->getCrestVersion();
    std::cout << "CrestApi: server version = " << version << std::endl;
  );

}
*/

TEST_P(crestApiTest, createTagTest) {
  std::cout << "\n====== CrestApi Test for the server methods =====\n";
  std::cout << "CREST server:  " << storage << "\n\n";

  // Clean the directory for tests:
  if (storage == "CrestFilesystem") {
    try {
      // Remove entire test directory and recreate
      std::filesystem::remove_all(connectionString);
      std::filesystem::create_directories(connectionString);
    } catch (const std::exception &e) {
      // Ignore cleanup errors but log them
      std::cerr << "Warning: filesystem cleanup failed: " << e.what()
                << std::endl;
    }
  }

  std::cout << std::endl << "1) Tag test" << std::endl;

  nlohmann::json js = {{"description", "none"},
                       {"endOfValidity", 0},
                       {"insertionTime", "2018-12-06T11:18:35.641+0000"},
                       {"lastValidatedTime", 0},
                       {"modificationTime", "2018-12-06T11:18:35.641+0000"},
                       {"name", tagname},
                       {"payloadSpec", "Json2Cool"},
                       {"synchronization", "ALL"},
                       {"timeType", "time"},
                       {"status", "UNLOCKED"}};

  std::cout << "tag (" << tagname << ") =" << std::endl
            << js.dump(4) << std::endl;

  TagDto dto = TagDto();
  dto = dto.fromJson(js);

  ASSERT_NO_THROW(myCrestApi->createTag(dto);)
      << "Error: Cannot create the tag " << tagname << " on CREST server."
      << std::endl;

  std::cout << "Tag " << tagname << " created on the CREST server. "
            << std::endl;

  TagDto dto2;

  std::cout << "Reading the file from the CREST server..." << std::endl;

  ASSERT_NO_THROW(dto2 = myCrestApi->findTag(tagname);)
      << "Error: Cannot get the tag " << tagname << " from the CREST server."
      << std::endl;

  std::cout << std::endl
            << "tag (" << tagname << ") = " << std::endl
            << dto2.toJson().dump(4) << std::endl;

  // create the JSON patch:
  nlohmann::json js2 = dto2.toJson();
  js.erase("insertionTime");
  js2.erase("insertionTime");
  js.erase("modificationTime");
  js2.erase("modificationTime");
  nlohmann::json patch = nlohmann::json::diff(js, js2);

  // Check if the tags ars identical:
  bool res = patch.empty();
  EXPECT_TRUE(res) << "Error: The sent and received from the CREST server tags "
                      "are not identical.";
}

TEST_P(crestApiTest, iovPayloadTest) {
  std::cout << std::endl << "3) IOV and Payload test" << std::endl;

  uint64_t endtime = 0;

  // Verify if the tag has no IOVs:
  IovSetDto retrievedIovs;
  ASSERT_NO_THROW(retrievedIovs = myCrestApi->selectIovs(
                      tagname, 0, -1, 0, 1000, 0, "id.since:ASC");)
      << "Error: Cannot get IOVs for the tag " << tagname << std::endl;

  int iovNumber = retrievedIovs.size();
  ASSERT_EQ(iovNumber, 0) << "Error: This tag cannot contain the IOVs, "
                          << "IOV number = " << iovNumber;

  // Create StoreSetDto with test data (similar to testStorePayloads)
  StoreSetDto storeSetDto;

  // Create first IOV entry
  uint64_t since1 = 1000;
  std::string payload1 = "Sample test data for IOV 1000";
  const char *streamer1 = "TestInfo123";
  StoreDto dto1(since1, "", payload1);
  dto1.setStreamerInfo(streamer1);
  storeSetDto.push_back(dto1);

  // Create second IOV entry
  uint64_t since2 = 2000;
  std::string payload2 = "Another sample test data for IOV 2000";
  const char *streamer2 = "TestInfo456";
  StoreDto dto2(since2, "", payload2);
  dto2.setStreamerInfo(streamer2);
  storeSetDto.push_back(dto2);

  std::cout << "Storing IOVs and payloads: " << storeSetDto.toJson().dump(4)
            << std::endl;

  // Store the data using the same method as in the example
  ASSERT_NO_THROW(myCrestApi->storeData(tagname, storeSetDto, "JSON", "test",
                                        "test", "1", endtime);)
      << "Error: Cannot store IOVs and payloads for tag " << tagname
      << std::endl;

  std::cout << "IOVs and payloads stored successfully for tag: " << tagname
            << std::endl;

  // Verify by selecting IOVs (similar to testListIovs)
  ASSERT_NO_THROW(retrievedIovs = myCrestApi->selectIovs(
                      tagname, 0, -1, 0, 1000, 0, "id.since:ASC");)
      << "Error: Cannot get IOVs for the tag " << tagname << std::endl;

  iovNumber = retrievedIovs.size();
  ASSERT_EQ(iovNumber, 2) << "Error: Wrong IOV number received, "
                          << "IOV number = " << iovNumber;

  std::cout << "IOV and Payload test passed successfully." << std::endl;
  std::cout << "Retrieved " << iovNumber << " IOV(s)" << std::endl;
  std::cout << "Retrieved IOVs: " << retrievedIovs.toJson().dump(4)
            << std::endl;

  std::vector<IovDto> iovList;
  ASSERT_NO_THROW(iovList = retrievedIovs.getResources();)
      << "Error: cannot get the IOV list from IovSetDto" << std::endl;

  for (IovDto iov : iovList) {
    uint64_t current_since = iov.getSince();
    std::string current_hash = iov.getPayloadHash();

    if (current_since == since1) {

      // payload test:
      std::string current_payload = "";
      ASSERT_NO_THROW(current_payload = myCrestApi->getPayload(current_hash);)
          << "Error: Cannot get payload for hash " << current_hash << std::endl;
      EXPECT_EQ(current_payload, payload1) << "Wrong payload.";

      // streamer info test:
      std::string current_streamer = "";
      ASSERT_NO_THROW(current_streamer =
                          myCrestApi->getStreamerInfo(current_hash);)
          << "Error: Cannot get streamer info for hash " << current_hash
          << std::endl;
      EXPECT_STREQ(current_streamer.c_str(), streamer1)
          << "Wrong streamer info.";
    }
  }
}

TEST_P(crestApiTest, createGlobalTagtest) {
  std::cout << std::endl << "4) Global Tag test" << std::endl;

  nlohmann::json js = {
      {"name", global_tag},
      {"validity", 0.0},
      {"description", "test"},
      {"release", "1"},
      {"insertionTime", "2018-12-18T11:32:58.081+0000"},
      {"snapshotTime", "2018-12-18T11:32:58+0000"},
      {"scenario", "test"},
      {"workflow", "M"},
      {"type", "t"},
  };

  std::cout << "global tag (" << global_tag << ") =" << std::endl
            << js.dump(4) << std::endl;

  GlobalTagDto dto = GlobalTagDto();
  dto = dto.fromJson(js);

  ASSERT_NO_THROW(myCrestApi->createGlobalTag(dto);)
      << "Error: Cannot create the global tag " << global_tag
      << " on CREST server." << std::endl;
  std::cout << "Global tag " << global_tag << " created on the CREST server. "
            << std::endl;

  GlobalTagDto dto2;

  std::cout << "Reading the global tag from the CREST server..." << std::endl;

  ASSERT_NO_THROW(dto2 = myCrestApi->findGlobalTag(global_tag);)
      << "Error: Cannot get the global tag " << global_tag
      << " from the CREST server." << std::endl;
  std::cout << std::endl
            << "global tag (" << global_tag << ") = " << std::endl
            << dto2.toJson().dump(4) << std::endl;

  // create the patch
  nlohmann::json js2 = dto2.toJson();
  js.erase("insertionTime");
  js2.erase("insertionTime");
  js.erase("modificationTime");
  js2.erase("modificationTime");
  js2.erase("insertionTimeMilli");
  js2.erase("snapshotTimeMilli");
  nlohmann::json patch = nlohmann::json::diff(js, js2);
  std::cout << "global tag patch = " << std::endl << patch.dump(4) << std::endl;

  bool res = patch.empty();

  ASSERT_TRUE(res) << "Error: The sent and received from the CREST server tags "
                      "are not identical.";
}

TEST_P(crestApiTest, globalTagMapTest) {
  std::cout << std::endl << "5) Global Tag Map test" << std::endl;
  bool res = true;

  // Create global tag map using constructor (similar to testCreateGlobalTagMap)
  GlobalTagMapDto dto(tagname, global_tag, "TestRecord", "/TESTFOLDER");

  std::cout << "Creating global tag map: " << std::endl
            << dto.toJson().dump(4) << std::endl;
  ASSERT_NO_THROW(myCrestApi->createGlobalTagMap(dto);)
      << "Error: Cannot create global tag map." << std::endl;
  std::cout << "Global tag map created successfully: " << global_tag << " -> "
            << tagname << std::endl;

  // Verify by finding the global tag map (similar to testListTagsInGlobalTag)
  GlobalTagMapSetDto gtmSet;
  ASSERT_NO_THROW(gtmSet = myCrestApi->findGlobalTagMap(global_tag, "Trace");)
      << "Error: Cannot retrieve global tag map for " << global_tag
      << std::endl;
  std::cout << "Retrieved global tag maps: " << std::endl
            << gtmSet.toJson().dump(4) << std::endl;

  // Check if our tag is in the global tag map
  for (const auto &gtm : gtmSet.getResources()) {
    if (gtm.getTagName() == tagname && gtm.getGlobalTagName() == global_tag) {
      res = true;
      break;
    }
  }
  if (res) {
    std::cout << "Global Tag Map test passed successfully." << std::endl;
    SUCCEED();
  } else {
    std::cout << "Global Tag Map test failed - mapping not found." << std::endl;
    FAIL();
  }
}

TEST_P(crestApiTest, removeGlobalTagTest) {
  std::cout << std::endl << "6) Global tag removing test" << std::endl;

  // CrestApiFs doesn't support the removeGlobalTag method
  if (storage == "CrestFilesystem") {
    GTEST_SKIP() << "CrestApiFs does not support the removeGlobalTag method";
  }

  // First, remove any global tag maps associated with this global tag
  std::cout << "Removing global tag maps for " << global_tag << "..."
            << std::endl;

  int mapSize = 0;
  // Get all mappings for this global tag
  GlobalTagMapSetDto gtmSet;
  ASSERT_NO_THROW(gtmSet = myCrestApi->findGlobalTagMap(global_tag, "");
                  mapSize = gtmSet.size();)
      << "Error: Could not retrieve global tag maps for " << global_tag << "."
      << std::endl;

  if (mapSize != 0) {
    // Remove each mapping
    for (const auto &gtm : gtmSet.getResources()) {
      EXPECT_NO_THROW(myCrestApi->removeGlobalTagMap(
          global_tag, gtm.getRecord(), gtm.getLabel(), gtm.getTagName());)
          << "Warning: Could not remove mapping " << global_tag << " -> "
          << gtm.getTagName() << std::endl;
    }
  }

  // The global tag removing:
  ASSERT_NO_THROW(myCrestApi->removeGlobalTag(global_tag);)
      << "Cannot remove global tag " << global_tag << "." << std::endl;

  std::cout << "The global tag " << global_tag
            << " was removed from CREST server." << std::endl;
}

TEST_P(crestApiTest, removeTagTest) {
  std::cout << std::endl << "7) Tag removing test" << std::endl;

  // CrestApiFs doesn't support the removeTag method
  if (storage == "CrestFilesystem") {
    GTEST_SKIP() << "CrestApiFs does not support the removeTag method";
  }

  std::cout << "The tag " << tagname << " will be deleted." << std::endl;
  ASSERT_NO_THROW(myCrestApi->removeTag(tagname);)
      << "Cannot renove the tag " << tagname << std::endl;
  std::cout << "The tag " << tagname << " was removed." << std::endl;

  ASSERT_THROW(myCrestApi->findTag(tagname);, CrestException)
      << "Error: The tag " << tagname << " was not deleted." << std::endl;
}

TEST_P(crestApiTest, emptyListTest) {
  std::cout << std::endl << "8) Empty list test" << std::endl;

  std::string tag_name = "000_unreal_tag_name_000";
  int expected_N_items = 0;
  int iovNumber = 0;
  int mapNumber = 0;
  int tagNumber = 0;
  int globalTagNumber = 0;

  // empty IOV list test:
  /*
  IovSetDto retrievedIovs;
  ASSERT_NO_THROW(retrievedIovs = myCrestApi->selectIovs(
                      tag_name, 0, -1, 0, 1000, 0, "id.since:ASC");)
      << "Cannot get an empty IOV list for " << tag_name << std::endl;
  iovNumber = retrievedIovs.size();
  std::cout << std::endl
            << "Empty IOV list = " << std::endl
            << retrievedIovs.toJson().dump(4) << std::endl;
  */

  // empty global tag map test:
  GlobalTagMapSetDto gtmSet;
  ASSERT_NO_THROW(gtmSet = myCrestApi->findGlobalTagMap(tag_name, "Trace");)
      << "Cannot get an empty global tag map for " << tag_name << std::endl;
  std::cout << std::endl
            << "Empty global tag map = " << std::endl
            << gtmSet.toJson().dump(4) << std::endl;

  // empty tag list test:
  std::string ftag_name = tag_name + "%";
  TagSetDto tags;
  ASSERT_NO_THROW(tags = myCrestApi->listTags(ftag_name, 1000, 0, "name:ASC");)
      << "Cannot get an empty tag list for " << tag_name + "%" << std::endl;
  std::cout << std::endl
            << "Empty tag list = " << std::endl
            << tags.toJson().dump(4) << std::endl;

  // empty global tag list test:
  GlobalTagSetDto gtags;
  ASSERT_NO_THROW(
      gtags = myCrestApi->listGlobalTags(ftag_name, 1000, 0, "name:ASC");)
      << "Cannot get an empty global tag list for " << tag_name + "%"
      << std::endl;
  std::cout << std::endl
            << "Empty global tag list = " << std::endl
            << gtags.toJson().dump(4) << std::endl;

  EXPECT_EQ(expected_N_items, iovNumber);
  EXPECT_EQ(expected_N_items, mapNumber);
  EXPECT_EQ(expected_N_items, tagNumber);
  EXPECT_EQ(expected_N_items, globalTagNumber);
}

TEST_P(crestApiTest, missingTagTest) {
  std::cout << std::endl << "9) Missing tag test" << std::endl;
  std::string tag_name = "000_unreal_tag_name_000";
  std::cout << "Reaging the missing tag and the global tag with name "
            << tag_name << std::endl;

  EXPECT_THROW(myCrestApi->findTag(tag_name), std::exception);
  EXPECT_THROW(myCrestApi->findGlobalTag(tag_name), std::exception);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  Instantiate tests for the server and the file storage
////////////////////////////////////////////////////////////////////////////////
INSTANTIATE_TEST_SUITE_P(CrestServer, crestApiTest,
                         ::testing::Values(getCrestServerConfig()));
INSTANTIATE_TEST_SUITE_P(CrestFilesystem, crestApiTest,
                         ::testing::Values(getCrestFilesystemConfig()));
