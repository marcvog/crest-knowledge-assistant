
/**
 * @file CrestApi/test/CrestApi_test.cxx
 * @brief Some tests for server methods.
 */

#include "CrestFileResolver.h"

using namespace Crest;

std::string CrestFileResolver::resolveFilePath(const std::string &geometry_gtag,
                                               const std::string &label,
                                               const std::string &file_name) {
  // Here you can implement the logic to resolve the file path.
  // Use crestapi to resolve mappings and detect the tag.
  std::cout << "Load mappings for " << geometry_gtag << std::endl;
  GlobalTagMapSetDto mapset =
      m_crestApi.findGlobalTagMap(geometry_gtag, "Trace");
  std::cout << "test: listTagsInGlobalTag (Trace) =" << std::endl;
  std::cout << mapset.toJson().dump(4) << std::endl;

  if (mapset.getResources().size() < 1) {
    std::cerr << "No mapping found for the global tag: " << geometry_gtag
              << std::endl;
    return "";
  }
  // Get the mapping (geometry tag name)
  GlobalTagMapDto map = mapset.getResources()[0];
  std::cout << "Found map " << map.toJson().dump(4) << std::endl;
  std::cout << "Compare label " << this->mat_label << " with " << map.getLabel()
            << std::endl;
  // Check that the label is correct
  if (this->mat_label != map.getLabel()) {
    std::cerr << "Label mismatch: expected " << mat_label << ", got "
              << map.getLabel() << std::endl;
    return "";
  }
  std::string mat_tag = map.getTagName();
  std::cout << "Load iovs for tag " << mat_tag << std::endl;
  // Now get the IOV.
  IovSetDto iovset =
      m_crestApi.selectIovs(mat_tag, 0, 10, 0, 10, 0, "id.since:ASC");
  if (iovset.getResources().size() < 1) {
    std::cerr << "No IOV found for the tag: " << mat_tag << std::endl;
    return "";
  }
  // Get the IOV
  IovDto iov = iovset.getResources()[0];
  std::cout << "Found IOV " << iov.toJson().dump(4) << std::endl;
  // Get the payload using the iov hash
  std::string hash = iov.getPayloadHash();
  std::string payload = m_crestApi.getPayload(hash);
  // Dump the payload to a file
  std::string file_path = m_file_path + file_name;
  // Write the payload to the file
  std::cout << "Dump payload to file " << file_path << std::endl;
  dumpToFile(file_path, payload);
  return file_path;
}

void CrestFileResolver::dumpToFile(const std::string &filePath,
                                   const std::string &content) {
  // Open the file in write mode
  std::ofstream outFile(filePath);

  // Check if the file was successfully opened
  if (!outFile) {
    std::cerr << "Error: Could not open file " << filePath << " for writing."
              << std::endl;
    return;
  }

  // Write the content to the file
  outFile << content;

  // Close the file
  outFile.close();

  std::cout << "Content successfully written to " << filePath << std::endl;
}
