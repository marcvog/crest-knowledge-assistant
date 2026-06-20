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

int createFile(std::string filePath);

void print_path() {
  std::cout << SURL << std::endl;
}

void testFindTag(const std::string &tagname) {
  std::cout << std::endl << "test: findTag" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    TagDto dto = myCrestApi.findTag(tagname);
    std::cout << std::endl
              << "test: findTag (result) = " << std::endl
              << dto.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: findTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testFindTagsMeta(const std::string &name) {
  std::cout << std::endl << "test: testFindTagsMeta " << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    TagMetaDto tagmetadto = myCrestApi.findTagMeta(name);
    std::cout << std::endl
              << "test: listTagsMeta (result) = " << std::endl
              << tagmetadto.toJson().dump(4) << std::endl;
    nlohmann::json meta_info = tagmetadto.toJson();
    if (meta_info.contains("tagInfo")) {
      std::string metainf = meta_info["tagInfo"];
      nlohmann::json js = nlohmann::json::parse(metainf);
    }
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: testFindTagsMeta (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testListIovs(const std::string &tagname) {
  std::cout << std::endl << "test: listIovs" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    IovSetDto dto =
        myCrestApi.selectIovs(tagname, 0, -1, 0, 1000, 0, "id.since:ASC");
    std::cout << std::endl
              << "test: listIovs (result) = " << std::endl
              << dto.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: listIovs (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testGetPayload(const std::string &hash) {
  std::cout << std::endl << "test: testGetPayload" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    std::string payload = myCrestApi.getPayload(hash);
    std::cout << "payload = " << payload << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: testGetPayload (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    SURL = argv[1];
    if (argc > 2) {
      apipath = argv[2];
    }
    print_path();

    std::string tagname = "LARIdentifierLArTTCellMapAtlas-RUN2-HadFcalFix2";
    std::string hash =
        "760dcf41a1366887e0da41b252535ad2af0ff575edc8caf014b0a178d30d0a4d";

    testFindTag(tagname);
    testFindTagsMeta(tagname);
    testListIovs(tagname);
    testGetPayload(hash);
  } else {
    std::cout << "CREST Server path not found" << std::endl;
    std::cout << "Please, run this program with a server path:" << std::endl;
    std::cout << "crest_example_server_getdata "
                 "https://atlas-crest-dev-mgr.cern.ch/api-v6.0"
              << std::endl;
  }
  std::cout << "Test ended" << std::endl;
  return 0;
}
