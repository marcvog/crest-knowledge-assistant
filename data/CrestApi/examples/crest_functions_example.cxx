/*!
   \file
   \brief Main file

   This file contains the examples for the CREST C++ Client Library.
   Main part of the examples is commented. Please uncomment the code you need.
   Check and correct (if it is necessary) the CREST Server parameters in
   CrestApi instances.
 */

#include <CrestApi/CrestApiExt.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>

// #include "CxxUtils/checker_macros.h"
// ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

using namespace Crest;

using namespace std;
std::string SURL = "https://atlas-crest-dev-mgr.cern.ch/api-v6.0";
std::string apipath = "/api-v6.0";
//===================================================
// Crest Functions Method Tests
//===================================================

void testFindTag(const std::string &tagname) {
  std::cout << std::endl << "test: findTag" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    TagDto dto = myCrestApi.findTag(tagname);
    std::cout << std::endl
              << "test: findTag (result) = " << std::endl
              << dto.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: findGlobalTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testFuncGetIovsForTag(const std::string &tagname) {
  std::cout << std::endl << "test: FuncGetIovsForTag" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL);
  using IovHash = std::pair<std::pair<uint64_t, uint64_t>, std::string>;
  std::vector<IovHash> iovHashes;
  try {
    TagDto dto = myCrestApi.findTag(tagname);
    std::cout << std::endl
              << "test: findTag (result) = " << std::endl
              << dto.toJson().dump(4) << std::endl;
    IovSetDto iovSetDto = myCrestApi.selectIovs(tagname, 0, 1331287800000000001,
                                                0, 1000, 0, "id.since:ASC");
    std::vector<IovDto> iovs = iovSetDto.getResources();
    for (size_t i = 0; i < iovs.size() - 1; ++i) {
      std::cout << "Found iov " << iovs[i].toJson() << std::endl;
      const IovDto &current = iovs[i];
      const IovDto &next = iovs[i + 1];
      std::cout << "iov current/next: " << std::endl
                << "  since: " << current.getSince() << std::endl
                << "  till: " << next.getSince() << std::endl;

      // Assuming we take the first key-value pair from each CrestIov for this
      // example
      uint64_t since = current.getSince();
      uint64_t till = next.getSince();
      std::pair<uint64_t, uint64_t> closediov = std::make_pair(since, till);
      std::pair<std::pair<uint64_t, uint64_t>, std::string> iovHash =
          std::make_pair(closediov, current.getPayloadHash());
      std::cout << "iovHash: " << std::endl
                << "  since: " << since << std::endl
                << "  till: " << till << std::endl
                << "  payloadHash: " << current.getPayloadHash() << std::endl;
      iovHashes.push_back(iovHash);
    }
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: findGlobalTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

int main(int argc, char *argv[]) {

  std::cout << "CREST Server path: " << SURL << std::endl;
  //==============================================================
  // CLASSIC TESTS from crest_examples.cxx

  // TO RUN TEST UNCOMMENT IT!
  // (and check parameters!)

  // std::string tagname = "test_tag_01"; // write your tag name
  std::string tagname = "MuonAlignOnlCscIlines-RUN2-04";

  testFindTag(tagname);
  testFuncGetIovsForTag(tagname);
}
