/*!
   \file
   \brief Main file

   This file contains the examples for the CREST C++ Client Library, for
   StringUtils functions. Main part of the examples is commented. Please
   uncomment the code you need. Check and correct (if it is necessary) the CREST
   Server parameters in CrestApi instances.
 */

#include <CrestApi/StringUtils.h>  // new

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>

// #include "CxxUtils/checker_macros.h"
// ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

using namespace Crest;

using namespace std;

void testSplit() {
  std::cout << "test: split" << std::endl;
  std::string a = "Hello, big world!";
  std::cout << "full string = " << a << std::endl;
  std::cout << "splitted string: " << std::endl;
  std::string b = " ";
  std::vector<std::string> v = Crest::StringUtils::split(a, b);
  for (auto i : v)
    std::cout << i << endl;
  std::cout << "test: split ended" << std::endl;
}

void testExtractName() {
  std::cout << "test: extractName" << std::endl;
  std::string full_name =
      "virtual Crest::TagDto Crest::CrestApi::findTag(const string&)";
  std::string name = Crest::StringUtils::extractName(full_name);
  std::cout << "full string = " << full_name << std::endl;
  std::cout << "name = " << name << std::endl;
  std::cout << "test: extractName (ended)" << std::endl;
}

void testGetJson() {
  std::cout << "test: getJson" << std::endl;
  try {
    std::string str = "blablabla";
    nlohmann::json response = Crest::StringUtils::getJson(str);
    std::cout << std::endl
              << "test: getJson (result) = " << std::endl
              << response.dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: getJson (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try {
    std::string tagname = "test";
    nlohmann::json js = {
        {"name", tagname},
        {"validity", 0},
        {"description", "test"},
        {"release", "1"},
        {"insertionTime", "2018-12-18T11:32:58.081+0000"},
        {"snapshotTime", "2018-12-18T11:32:57.952+0000"},
        {"scenario", "test"},
        {"workflow", "M"},
        {"type", "t"},
    };

    std::string str = js.dump();

    nlohmann::json response = Crest::StringUtils::getJson(str);
    std::cout << std::endl
              << "test: getJson (result) = " << std::endl
              << response.dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: getJson (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }

  std::cout << "test: getJson (ended)" << std::endl;
}

void testToJson() {
  std::cout << "test: toJson" << std::endl;
  try {
    std::string str = "blablabla";
    nlohmann::json response = Crest::StringUtils::toJson(str);
    std::cout << std::endl
              << "test: toJson (result) = " << std::endl
              << response.dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: toJson (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try {
    std::string tagname = "test";
    nlohmann::json js = {
        {"name", tagname},
        {"validity", 0},
        {"description", "test"},
        {"release", "1"},
        {"insertionTime", "2018-12-18T11:32:58.081+0000"},
        {"snapshotTime", "2018-12-18T11:32:57.952+0000"},
        {"scenario", "test"},
        {"workflow", "M"},
        {"type", "t"},
    };

    std::string str = js.dump();

    nlohmann::json response = Crest::StringUtils::toJson(str);
    std::cout << std::endl
              << "test: getJson (result) = " << std::endl
              << response.dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: toJson (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }

  std::cout << "test: toJson (ended)" << std::endl;
}

int main(int argc, char *argv[]) {

  testSplit();
  testExtractName();

  testGetJson();
  testToJson();

  return 0;
}
