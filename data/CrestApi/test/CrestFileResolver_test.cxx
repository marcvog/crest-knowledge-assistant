
/**
 * @file CrestApi/test/CrestApi_test.cxx
 * @brief Some tests for server methods.
 */

#include "CrestApi/GlobalTagMapDto.h"
#include "CrestApi/GlobalTagMapSetDto.h"
#include "CrestApi/IovSetDto.h"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_CRESTAPI

#include <fstream>
#include <iostream>
#include <string>

#include "../CrestApi/CrestApi.h"
#include "../CrestApi/StringUtils.h"
#include "../acts/CrestFileResolver.h"

using namespace Crest;

const std::string crest_server = "http://atlaf-alma9-02.cern.ch:8080/api-v6.0";
const std::string global_tag = "TEST-GEOMETRY-01";

int main() {
  std::string geoglobal_tag = "TEST-GEOMETRY-01";
  std::cout << "Global tag for geometry: " << geoglobal_tag << std::endl;

  try {
    // Create an instance of CrestFileResolver
    CrestFileResolver crestFileResolver(crest_server, "/tmp/crest_test/",
                                        "/Material/TrackingGeo");
    // Resolve the file path
    std::string file_path = crestFileResolver.resolveFilePath(
        geoglobal_tag, "/Material/TrackingGeo", "test_file.json");
    // Print the parsed inner JSON
    std::cout << "Dumped JSON from Geometry Global Tag for tracking material: "
              << file_path << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error parsing JSON: " << e.what() << std::endl;
  }

  return 0;
}
