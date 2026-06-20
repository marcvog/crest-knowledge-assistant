////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2026 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

#include "../CrestApi/StringUtils.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <source_location>
#include <string>

#include "gtest/gtest.h"

using namespace Crest;

TEST(crestApiTest, parseUrlTest) {
  std::cout << "1) Test for the parseUrl method:" << std::endl;
  // std::cout << std::endl << "1) String utils test" << std::endl;
  std::string str = "http://crest-04.cern.ch:8080";
  bool res = true;

  Crest::StringUtils::ParsedUrl parsedUrl = Crest::StringUtils::parseUrl(str);
  if (parsedUrl.host == "crest-04.cern.ch" && parsedUrl.port == 8080 &&
      parsedUrl.protocol == "http") {
    std::cout << "The parseUrlTest test A passed successfully." << std::endl;
  } else {
    std::cout << "The parseUrlTest test A failed." << std::endl;
    res = false;
  }
  // Now add the API path
  str = "https://crest-04.cern.ch:8080/api-v5.0";
  parsedUrl = Crest::StringUtils::parseUrl(str);
  if (parsedUrl.host == "crest-04.cern.ch" && parsedUrl.port == 8080 &&
      parsedUrl.protocol == "https") {
    std::cout << "The parseUrlTest test B passed successfully." << std::endl;
  } else {
    std::cout << "The parseUrlTest test B failed." << std::endl;
    res = false;
  }

  EXPECT_TRUE(res);
}

TEST(crestApiTest, urlEncodeTest) {

  std::cout << std::endl << "2) URL encoding test" << std::endl;
  bool res = true;

  // Test 1: Characters that need zero-padding (< 0x10)
  std::string test1 = "\t";  // Tab = 0x09
  std::string encoded1 = Crest::StringUtils::urlEncode(test1);
  if (encoded1 != "%09") {
    std::cout << "Failed: Tab encoding. Expected %09, got " << encoded1
              << std::endl;
    res = false;
  }

  std::string test2 = "\n";  // Newline = 0x0A
  std::string encoded2 = Crest::StringUtils::urlEncode(test2);
  if (encoded2 != "%0A") {
    std::cout << "Failed: Newline encoding. Expected %0A, got " << encoded2
              << std::endl;
    res = false;
  }

  std::string test3 = "\x01";  // SOH = 0x01
  std::string encoded3 = Crest::StringUtils::urlEncode(test3);
  if (encoded3 != "%01") {
    std::cout << "Failed: SOH encoding. Expected %01, got " << encoded3
              << std::endl;
    res = false;
  }

  // Test 2: Regular special characters
  std::string test4 = " ";  // Space = 0x20
  std::string encoded4 = Crest::StringUtils::urlEncode(test4);
  if (encoded4 != "%20") {
    std::cout << "Failed: Space encoding. Expected %20, got " << encoded4
              << std::endl;
    res = false;
  }

  std::string test5 = "%";  // Percent = 0x25
  std::string encoded5 = Crest::StringUtils::urlEncode(test5);
  if (encoded5 != "%25") {
    std::cout << "Failed: Percent encoding. Expected %25, got " << encoded5
              << std::endl;
    res = false;
  }

  // Test 3: Allowed characters (should not be encoded)
  std::string test6 = "abc123-_.~";
  std::string encoded6 = Crest::StringUtils::urlEncode(test6);
  if (encoded6 != "abc123-_.~") {
    std::cout << "Failed: Allowed chars. Expected abc123-_.~, got " << encoded6
              << std::endl;
    res = false;
  }

  // Test 4: High-byte values (test unsigned char handling)
  std::string test7 = "\xFF";  // 0xFF = 255
  std::string encoded7 = Crest::StringUtils::urlEncode(test7);
  if (encoded7 != "%FF") {
    std::cout << "Failed: High byte encoding. Expected %FF, got " << encoded7
              << std::endl;
    res = false;
  }

  std::string test8 = "\x80";  // 0x80 = 128
  std::string encoded8 = Crest::StringUtils::urlEncode(test8);
  if (encoded8 != "%80") {
    std::cout << "Failed: High byte encoding. Expected %80, got " << encoded8
              << std::endl;
    res = false;
  }

  // Test 5: Complex string with multiple edge cases
  std::string test9 = "hello world\t\n";
  std::string encoded9 = Crest::StringUtils::urlEncode(test9);
  if (encoded9 != "hello%20world%09%0A") {
    std::cout << "Failed: Complex string. Expected hello%20world%09%0A, got "
              << encoded9 << std::endl;
    res = false;
  }

  if (res) {
    std::cout << "URL encoding test passed successfully." << std::endl;
  } else {
    std::cout << "URL encoding test failed." << std::endl;
  }

  EXPECT_TRUE(res);
}

TEST(crestApiTest, removeCRTest) {
  std::string str01 = "Hello big world!|";  // end line replaced by |
  std::string str02 = "Hello\r big\r world!\n";

  EXPECT_EQ(Crest::StringUtils::removeCR(str02), str01);
}

TEST(crestApiTest, splitTest) {
  std::vector<std::string> strA = {"Hello", "big", "world!"};
  std::string delim = "|";
  std::string test = strA[0] + delim + strA[1] + delim + strA[2];

  std::vector<std::string> sp = Crest::StringUtils::split(test, delim);
  EXPECT_EQ(strA.size(), sp.size());
}

TEST(crestApiTest, toJsonTest) {
  std::string key[] = {"keyA", "keyB", "keyC"};
  std::string value[] = {"valueA", "valueB", "valueC"};

  nlohmann::json jsA = {
      {key[0], value[0]}, {key[1], value[1]}, {key[2], value[2]}};

  std::string strA = jsA.dump();

  nlohmann::json jsB = Crest::StringUtils::toJson(strA);
  std::cout << "jsB = " << std::endl << jsB.dump(4) << std::endl;

  // check JSONs:
  nlohmann::json patch = nlohmann::json::diff(jsA, jsB);
  std::cout << "patch = " << patch << std::endl;
  EXPECT_TRUE(patch.empty()) << "The JSONs in test are not identical.";
}

TEST(crestApiTest, parseXMLOutputTest) {
  std::string strA =
      "<!DOCTYPE html>\n<html>\n<body>\n<h1>XML Test</h1>\n</body>\n</html>";
  std::string strB = "XML Test";
  std::string result = Crest::StringUtils::parseXMLOutput(strA);
  result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
  EXPECT_EQ(result, strB);
}

// test method to check the function extractName:
std::string method01() {
  std::source_location location = std::source_location::current();
  std::string fname = Crest::StringUtils::extractName(location.function_name());
  return fname;
}

TEST(crestApiTest, extractNameTest) {
  std::string str1 = "method01";
  std::string str2 = method01();
  EXPECT_EQ(str1, str2);
}

TEST(crestApiTest, parsePortTest) {
  std::string port1 = "8080";
  int port = 8080;
  std::string port2 = "abc";

  std::optional<int> p1 = Crest::StringUtils::parsePort(port1);
  if (p1.has_value()) {
    EXPECT_EQ(p1, port);
  } else {
    std::cerr << "Port not found in " << port1 << std::endl;
    FAIL();
  }

  std::optional<int> p2 = Crest::StringUtils::parsePort(port2);
  if (p2.has_value()) {
    std::cerr << "Port cannot exist in " << port2 << std::endl;
    FAIL();
  } else {
    std::cout << "Test passed: port does not exist in " << port2 << std::endl;
    SUCCEED();
  }
}

TEST(crestApiTest, makeUrlTest) {
  std::string url = "http://crest-04.cern.ch:8080/api";
  std::string host = "crest-04.cern.ch";
  std::string prefix = "http";
  std::string port = "8080";
  std::string address = "/api";

  std::string result = Crest::StringUtils::makeUrl(prefix, host, port, address);
  EXPECT_EQ(result, url);
}

// test method to check the function getWarningString:
std::string method02(const std::string &message) {
  std::source_location location = std::source_location::current();
  std::string warn = Crest::StringUtils::getWarningString(message, location);
  return warn;
}

TEST(crestApiTest, getWarningStringTest) {
  std::string message = "Error message.";
  std::string warning =
      "ERROR std::string method02(const string&) | CREST Server response : "
      "Error message.";
  std::string warn = method02(message);
  std::cout << "Warning = " << warn << std::endl;
  EXPECT_EQ(warn, warning);
}

// test method to check the function getJson:
nlohmann::json method03(const std::string &str) {
  std::source_location location = std::source_location::current();
  nlohmann::json js = Crest::StringUtils::getJson(str, location);
  return js;
}

TEST(crestApiTest, getJsonTest) {
  std::string key[] = {"keyA", "keyB", "keyC"};
  std::string value[] = {"valueA", "valueB", "valueC"};

  nlohmann::json jsA = {
      {key[0], value[0]}, {key[1], value[1]}, {key[2], value[2]}};

  std::string strA = jsA.dump();

  nlohmann::json jsB = Crest::StringUtils::getJson(strA);
  std::cout << "jsB = " << std::endl << jsB.dump(4) << std::endl;

  // check JSONs:
  nlohmann::json patch = nlohmann::json::diff(jsA, jsB);
  std::cout << "patch = " << patch << std::endl;
  EXPECT_TRUE(patch.empty()) << "The JSONs in test are not identical.";

  //
  std::string wrong_js_string = "abc";
  std::string expected_warning =
      "ERROR in method03 | CREST Server response : abc";
  try {
    nlohmann::json js = method03(wrong_js_string);
  } catch (const std::exception &e) {
    std::string warning = e.what();
    EXPECT_EQ(warning, expected_warning);
  }
}
