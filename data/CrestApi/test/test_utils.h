////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2026 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

// #ifndef CHAI_TEST_UTILS_H
// #define CHAI_TEST_UTILS_H

#include <CrestApi/CrestApi.h>
#include <CrestApi/CrestApiFs.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>

using namespace Crest;

////////////////////////////////////////////////////////////////////////////////
/// @brief  CREST client configuration for parametrized tests
////////////////////////////////////////////////////////////////////////////////
using CrestTestConfig = std::pair<std::string, std::string>;

////////////////////////////////////////////////////////////////////////////////
/// @brief  Custom printer for CrestTestConfig (eliminates GetParam() clutter)
////////////////////////////////////////////////////////////////////////////////
inline void PrintTo(const CrestTestConfig& config, std::ostream* os) {
  *os << config.first;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Get CREST client parameters for parametrized testing
/// @return Vector of CREST ctient configurations to test
///
/// Returns configs for all available clients. Uses environment variables:
/// - CREST_SERVER: Local server URL (default
/// https://atlas-crest-dev-mgr.cern.ch/api-v6.0 )
/// - CREST_FS_DIR: Filesystem directory (default: /tmp/crest_test)
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// @brief  Get CrestServer client configuration
////////////////////////////////////////////////////////////////////////////////
inline CrestTestConfig getCrestServerConfig() {
  const char* crestServer = std::getenv("CREST_SERVER");
  if (!crestServer) {
    crestServer = "https://atlas-crest-dev-mgr.cern.ch/api-v6.0";
  }

  return {"CrestServer", std::string(crestServer)};
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Get CrestFilesystem client configuration
////////////////////////////////////////////////////////////////////////////////
inline CrestTestConfig getCrestFilesystemConfig() {
  const char* fsDir = std::getenv("CREST_FS_DIR");
  const char* userEnv = std::getenv("USER");
  std::string user = userEnv ? userEnv : "crestclient";
  std::string folder;
  if (!fsDir) {
    folder = "/tmp/" + user + "/crest_test";
    std::cout << "CREST_FS_DIR not set" << std::endl;
  } else
    folder = fsDir;
  if (!std::filesystem::exists(folder)) {
    std::filesystem::create_directories(folder);
    std::cout << "The directory " << folder << " was created for tests."
              << std::endl;
  }
  return {"CrestFilesystem", folder};
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Get all CREST client configurations (for legacy compatibility)
////////////////////////////////////////////////////////////////////////////////
inline std::vector<CrestTestConfig> getTestClients() {
  return {getCrestServerConfig(), getCrestFilesystemConfig()};
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Parametrized fixture for CrestApi tests
///
/// Provides:
/// - Create CREST client (for server or filesystem)
/// - Unique test name generation
/// - Automatic cleanup of created resources
///
/// Used with TEST_P and INSTANTIATE_TEST_SUITE_P for multi-client testing.
////////////////////////////////////////////////////////////////////////////////
class MultiClientTestFixture
    : public ::testing::TestWithParam<CrestTestConfig> {
 protected:
  std::string storage;
  std::string connectionString;  // MvG
  std::unique_ptr<Crest::CrestApiBase> myCrestApi;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief  Setup - create CREST client
  ////////////////////////////////////////////////////////////////////////////////
  void SetUp() override {
    const auto& config = GetParam();
    storage = config.first;
    connectionString = config.second;

    std::string connectionString = config.second;

    const std::string prefix1 = "http://";
    const std::string prefix2 = "https://";

    if (connectionString.starts_with(prefix1) ||
        connectionString.starts_with(prefix2)) {
      myCrestApi = std::unique_ptr<Crest::CrestApi>(
          new Crest::CrestApi(connectionString));
    } else {
      myCrestApi = std::unique_ptr<Crest::CrestApiFs>(
          new Crest::CrestApiFs(true, connectionString));
    }
    //
  }

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief  TearDown - cleanup created resources
  ////////////////////////////////////////////////////////////////////////////////
  void TearDown() override {
    if (!myCrestApi) {
      return;
    }
  }
};
