
/**
 * @file CrestApi/test/CrestApi_test.cxx
 * @brief Some tests for server methods.
 */
#ifndef CRESTAPI_CRESTRESOLVER_H
#define CRESTAPI_CRESTRESOLVER_H

#include <fstream>
#include <iostream>
#include <string>

#include "CrestApi/CrestApi.h"
#include "CrestApi/GlobalTagMapDto.h"
#include "CrestApi/GlobalTagMapSetDto.h"
#include "CrestApi/IovSetDto.h"
#include "CrestApi/StringUtils.h"

using namespace Crest;

/**
 * Class to resolve a geometry tracking file.
 * These files are linked to a geometry global tag, and contain only one IOV
 * (since=0).
 */
class CrestFileResolver {
 private:
  std::string m_crest_server;
  CrestApi m_crestApi;
  // This should be somehow linked to the type of geometry tag (ITK, ...)
  std::string mat_label = "ITK";
  // Base directoy where the file will be dumped.
  std::string m_file_path;

 public:
  CrestFileResolver(const std::string &crest_server)
      : m_crest_server(crest_server),
        m_crestApi(crest_server),
        m_file_path("/tmp/crest_test/") {
    // Check CrestApi version.
    if (m_crestApi.getCrestVersion() != "v6.0") {
      std::cerr << "CrestApi version mismatch!" << std::endl;
    }
  }

  CrestFileResolver(const std::string &crest_server,
                    const std::string &file_path, const std::string &label)
      : m_crest_server(crest_server),
        m_crestApi(crest_server),
        mat_label(label),
        m_file_path(file_path) {
    // Check CrestApi version
    if (m_crestApi.getCrestVersion() != "v6.0") {
      std::cerr << "CrestApi version mismatch!" << std::endl;
    }
  }
  // Destructor
  ~CrestFileResolver() = default;

  // Resolve the file path for a given geometry global tag.
  std::string resolveFilePath(const std::string &geometry_gtag,
                              const std::string &label,
                              const std::string &file_name);
  // Dump the payload to a file.
  void dumpToFile(const std::string &filePath, const std::string &content);
};

#endif  // CRESTAPI_CRESTRESOLVER_H