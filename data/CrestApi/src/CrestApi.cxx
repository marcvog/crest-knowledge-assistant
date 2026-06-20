/*
  Copyright (C) 2020-2026 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/CrestApi.h>
#include <CrestApi/CrestDetail.h>
#include <CrestApi/CrestRequest.h>
#include <CrestApi/StringUtils.h>

#include <boost/uuid/uuid.hpp>             // uuid class
#include <boost/uuid/uuid_generators.hpp>  // generators
#include <boost/uuid/uuid_io.hpp>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace Crest::Detail;
namespace Crest {

/**
 * CrestApi constructor for server mode, using the host, port, and protocol
 * parameters.
 *
 */
CrestApi::CrestApi(const std::string &host, const std::string &port,
                   const std::string &protocol, bool check_version,
                   std::string_view apipath) {
  if (check_version == true) {
    checkCrestVersion();
  }
  m_PATH = apipath;
  m_request.setHost(host);
  m_request.setPort(port);
  m_request.setPrefix(protocol);
  m_request.initAuth(std::getenv("CREST_AUTH_MODE"));
}

/**
 * CrestApi constructor for Internet mode. If CrestApi is created with this
 * method the data will be sent to the CREST Server.
 * @param url - URL address of the CREST Server (with port).
 * @param apipath - the parameter to provide the default api path, can be used
 * to force a give path.
 * @param check_version - the parameter to switch CREST version checking, if
 * this parameter is true, the CREST version test will be executed. <br> <br>
 * Example:
 * <br>
 * <pre>
 *    std::string url = "https://atlas-crest-dev-mgr.cern.ch/api-v6.0";
 *    CrestApi myCrestApi = CrestApi(url);
 * </pre>
 */
CrestApi::CrestApi(std::string_view url, std::string_view apipath,
                   bool check_version) {
  StringUtils::ParsedUrl parsedUrl = StringUtils::parseUrl(url);

  m_request.setHost(parsedUrl.host.data());
  m_request.setPort(std::to_string(*parsedUrl.port));
  m_request.setPrefix(parsedUrl.protocol.data());
  m_request.initAuth(std::getenv("CREST_AUTH_MODE"));

  if (parsedUrl.apipath == "") {
    m_PATH = urlPaths[UrlPathIndex::CREST_ROOT];
  } else {
    m_PATH = parsedUrl.apipath;
  }
  if (apipath != "") {
    m_PATH = apipath;
  }
  m_request.setUrl("");

  if (check_version == true) {
    checkCrestVersion();
  }
}

/**
 * CrestApi destructor.
 */
CrestApi::~CrestApi() {}

// The auxillary method to remove XML/HTML tags from a std::string

nlohmann::json CrestApi::getMgmtInfo() {
  std::string current_path = "";
  current_path += urlPaths[UrlPathIndex::MGMT_INFO];
  std::string retv;
  nlohmann::json js = nullptr;
  nlohmann::json respond = nullptr;
  try {
    retv = m_request.performRequest(current_path, Action::GET, js);
  } catch (const std::exception &e) {
    // Check if actuator is the path and try to get the info from there
    current_path = "";
    current_path += urlPaths[UrlPathIndex::ACTUATOR_INFO];
    retv = m_request.performRequest(current_path, Action::GET, js);
    if (retv.empty()) {
      throw CrestException("ERROR in CrestApi::getMgmtInfo: " +
                           (std::string)e.what());
    }
  }
  respond = StringUtils::getJson(retv);

  return respond;
}

std::string CrestApi::getCrestVersion() {

  std::string version = "";
  nlohmann::json buildInfo;
  nlohmann::json info = getMgmtInfo();
  // The CREST Server response has the following structure:
  // { "build": { "artifact": "crestdb", "name": "crest", "version": "4.0",
  // "group": "hep.crest"} }
  try {
    buildInfo = info["build"];
    version = buildInfo["version"];
  } catch (const std::exception &e) {
    throw CrestException("ERROR in CrestApi::getCrestVersion: " +
                         to_string(info) + " does not contain version info.");
  }
  return version;
}

int CrestApi::getMajorVersion(std::string &str) {
  int v = -1;
  int n = str.find('.');

  if (n < 1) {
    throw CrestException("ERROR in CrestApi::getMajorVersion: string \"" + str +
                         "\" does not contain major version.");
  }

  std::string vers = str.substr(0, n);

  try {
    v = std::stoi(str);
  } catch (const std::exception &e) {
    throw CrestException("ERROR in CrestApi::getMajorVersion: string \"" + str +
                         "\" does not contain major version.");
  }

  return v;
}

void CrestApi::checkCrestVersion() {
  std::string client = getClientVersion();
  std::string server = getCrestVersion();

  int clientVersion = getMajorVersion(client);
  int serverVersion = getMajorVersion(server);

  if (clientVersion != serverVersion) {
    throw CrestException(
        "ERROR in CrestApi::checkCrestVersion: CrestApi version \"" + client +
        "\" does not correspond to the server version \"" + server + "\".");
  }
}

//==============================================================================================================
// GlobalTag methods:
void CrestApi::createGlobalTag(GlobalTagDto &dto) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::GLOBALTAGS];

  std::string retv;
  nlohmann::json js = dto.toJson();
  retv = m_request.performRequest(current_path, Action::POST, js);
}

void CrestApi::updateGlobalTag(GlobalTagDto &dto) {
  std::string name = dto.getName();
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::ADMIN];
  current_path += urlPaths[UrlPathIndex::GLOBALTAGS];
  current_path += '/';
  current_path += name;

  std::string retv;
  nlohmann::json js = dto.toJson();
  retv = m_request.performRequest(current_path, Action::PUT, js);
}

GlobalTagDto CrestApi::findGlobalTag(const std::string &name) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::GLOBALTAGS];
  current_path += '/';
  current_path += name;
  std::string retv;

  nlohmann::json js = nullptr;
  retv = m_request.performRequest(current_path, Action::GET, js);
  nlohmann::json response = StringUtils::getJson(retv);

  GlobalTagSetDto dto = GlobalTagSetDto::fromJson(response);
  std::vector<GlobalTagDto> v = dto.getResources();
  // Only one element is expected:
  return v[0];
}

GlobalTagSetDto CrestApi::listGlobalTags(const std::string &name, int size,
                                         int page, const std::string &sort) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::GLOBALTAGS];
  current_path += '?';
  if (name != "") {
    current_path += "name=";
    current_path += StringUtils::urlEncode(name);
    current_path += '&';
  }
  if (size != -1) {
    current_path += "size=";
    current_path += std::to_string(size);
    current_path += '&';
  }
  if (page != -1) {
    current_path += "page=";
    current_path += std::to_string(page);
    current_path += '&';
  }
  if (sort != "") {
    current_path += "sort=";
    current_path += sort;
    current_path += '&';
  }
  std::string retv;

  nlohmann::json js = nullptr;
  retv = m_request.performRequest(current_path, Action::GET, js);
  nlohmann::json response = StringUtils::getJson(retv);

  GlobalTagSetDto dto = GlobalTagSetDto::fromJson(response);

  return dto;
}

// The method to remove a global tag:
void CrestApi::removeGlobalTag(const std::string &name) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::ADMIN];
  current_path += urlPaths[UrlPathIndex::GLOBALTAGS];
  current_path += '/';
  current_path += name;
  std::string retv;
  nlohmann::json js = nullptr;
  retv = m_request.performRequest(current_path, Action::DELETE, js);
}

//==============================================================================================================
// Tag methods:
void CrestApi::createTag(TagDto &dto) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::TAGS];
  std::string retv;
  nlohmann::json js = dto.toJson();
  retv = m_request.performRequest(current_path, Action::POST, js);
}

void CrestApi::updateTag(TagDto &dto) {
  std::string tagName = dto.getName();
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::TAGS];
  current_path += '/';
  current_path += tagName;
  std::cout << "updateTag: path = " << current_path << std::endl;
  std::string retv;
  nlohmann::json js = dto.toJson();
  retv = m_request.performRequest(current_path, Action::PUT, js);
}

// The method to find a tag:
TagDto CrestApi::findTag(const std::string &name) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::TAGS];
  current_path += '/';
  current_path += name;
  std::string retv;
  nlohmann::json js = nullptr;
  retv = m_request.performRequest(current_path, Action::GET, js);
  nlohmann::json response = StringUtils::getJson(retv);

  TagSetDto dto = TagSetDto::fromJson(response);

  std::vector<TagDto> v = dto.getResources();
  // Only one element is expected:
  return v[0];
}

TagSetDto CrestApi::listTags(const std::string &name, int size, int page,
                             const std::string &sort) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::TAGS];
  current_path += '?';
  if (name != "") {
    current_path += "name=";
    current_path += StringUtils::urlEncode(name);
    current_path += '&';
  }
  if (size != -1) {
    current_path += "size=";
    current_path += std::to_string(size);
    current_path += '&';
  }
  if (page != -1) {
    current_path += "page=";
    current_path += std::to_string(page);
    current_path += '&';
  }
  if (sort != "") {
    current_path += "sort=";
    current_path += sort;
    current_path += '&';
  }
  std::string retv;

  nlohmann::json js = nullptr;
  retv = m_request.performRequest(current_path, Action::GET, js);
  nlohmann::json response = StringUtils::getJson(retv);

  TagSetDto dto = TagSetDto::fromJson(response);

  return dto;
}

// The method to remove a tag:
void CrestApi::removeTag(const std::string &name) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::ADMIN];
  current_path += urlPaths[UrlPathIndex::TAGS];
  current_path += '/';
  current_path += name;
  std::string retv;
  nlohmann::json js = nullptr;
  retv = m_request.performRequest(current_path, Action::DELETE, js);
}

//==============================================================================================================
// TagMeta methods:
void CrestApi::createTagMeta(TagMetaDto &dto) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::TAGS];
  current_path += '/';
  current_path += dto.getTagName();
  current_path += urlPaths[UrlPathIndex::META];
  std::string retv;
  nlohmann::json js = dto.toJson();
  retv = m_request.performRequest(current_path, Action::POST, js);
}

void CrestApi::updateTagMeta(TagMetaDto &dto) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::TAGS];
  current_path += '/';
  current_path += dto.getTagName();
  current_path += urlPaths[UrlPathIndex::META];
  std::string retv;
  nlohmann::json js = dto.toJson();
  retv = m_request.performRequest(current_path, Action::PUT, js);
}

TagMetaDto CrestApi::findTagMeta(const std::string &name) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::TAGS];
  current_path += '/';
  current_path += name;
  current_path += urlPaths[UrlPathIndex::META];
  std::string retv;
  nlohmann::json js = nullptr;
  retv = m_request.performRequest(current_path, Action::GET, js);
  nlohmann::json response = StringUtils::getJson(retv);
  TagMetaSetDto dto = TagMetaSetDto::fromJson(response);
  std::vector<TagMetaDto> v = dto.getResources();
  // Only one element is expected:
  return v[0];
}

int CrestApi::getSize(const std::string &tagname) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::IOVS];
  current_path += urlPaths[UrlPathIndex::SIZE];
  current_path += "?tagname=";
  current_path += tagname;

  // CHECK TAG NAME
  if (tagname.find('*') != std::string::npos ||
      tagname.find('%') != std::string::npos) {
    throw CrestException(
        "ERROR in CrestApi::getSize Invalid character (* or %) in tagname.");
  }

  std::string retv;

  nlohmann::json js = nullptr;
  retv = m_request.performRequest(current_path, Action::GET, js);
  nlohmann::json resp = StringUtils::getJson(retv);
  auto res = resp.find("resources");
  nlohmann::json r;

  if (res != resp.end()) {
    r = resp["resources"][0];
  }

  int result = 0;
  if (r.find("niovs") != r.end()) {
    result = r["niovs"];
  } else {
    if (resp.find("size") != resp.end()) {
      result = resp["size"];
    } else {
      throw CrestException(
          "ERROR in CrestApi::getSize CREST Server JSON "
          "response has no \"size\" key.");
    }
  }
  return result;
}

//==============================================================================================================
// GlobalTagMaps methods:
void CrestApi::createGlobalTagMap(GlobalTagMapDto &dto) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::GLOBALTAGMAPS];
  std::string retv;
  nlohmann::json js = dto.toJson();
  retv = m_request.performRequest(current_path, Action::POST, js);
}

GlobalTagMapSetDto CrestApi::findGlobalTagMap(
    const std::string &name, const std::string &xCrestMapMode) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::GLOBALTAGMAPS];
  current_path += '/';
  current_path += name;
  std::string headers_params = "";

  if (xCrestMapMode != "") {
    headers_params += "X-Crest-MapMode: ";
    headers_params += xCrestMapMode;
  } else {
    headers_params += "X-Crest-MapMode: ";
    headers_params += "Trace";
  }
  std::string retv;
  nlohmann::json js = nullptr;
  retv =
      m_request.performRequest(current_path, Action::GET, js, headers_params);
  nlohmann::json response = StringUtils::getJson(retv);

  GlobalTagMapSetDto dto = GlobalTagMapSetDto::fromJson(response);

  return dto;
}

void CrestApi::removeGlobalTagMap(const std::string &name,
                                  const std::string &record,
                                  const std::string &label,
                                  const std::string &tagname) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::GLOBALTAGMAPS];
  current_path += '/';
  current_path += name;
  current_path += '?';
  if (tagname != "") {
    current_path += "tagname=";
    current_path += tagname;
    current_path += '&';
  }
  if (record != "") {
    current_path += "record=";
    current_path += record;
    current_path += '&';
  }
  if (label != "") {
    current_path += "label=";
    current_path += label;
    current_path += '&';
  }

  std::string retv;
  nlohmann::json js = nullptr;
  retv = m_request.performRequest(current_path, Action::DELETE, js);

  nlohmann::json response = StringUtils::getJson(retv);
  GlobalTagMapSetDto dto = GlobalTagMapSetDto::fromJson(response);
  if (dto.size() != 0) {
    GlobalTagMapDto gtmap = dto.getResources()[0];
    if (name != gtmap.getGlobalTagName() || tagname != gtmap.getTagName() ||
        label != gtmap.getLabel()) {
      throw CrestException(
          "ERROR in CrestApi::removeGlobalTagMap Cannot delete global tag "
          "map.");
    }
  } else {
    throw CrestException(
        "ERROR in CrestApi::removeGlobalTagMap Cannot delete global tag map.");
  }
}

//==============================================================================================================
// Iovs methods:
IovSetDto CrestApi::selectIovs(const std::string &name, uint64_t since,
                               uint64_t until, long snapshot, int size,
                               int page, const std::string &sort,
                               const std::string &header_param) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::IOVS];

  current_path += "?method=";
  current_path += iovMethod[IovMethodIndex::METHOD_IOVS];
  current_path += "&tagname=";

  current_path += name;
  current_path += "&since=";
  current_path += std::to_string(since);
  current_path += "&until=";
  if (until == static_cast<uint64_t>(-1)) {
    current_path += "INF";
  } else {
    current_path += std::to_string(until);
  }
  current_path += "&snapshot=";
  current_path += std::to_string(snapshot);
  //
  current_path += "&size=";
  current_path += std::to_string(size);
  current_path += "&page=";
  current_path += std::to_string(page);
  current_path += "&sort=";
  current_path += sort;
  //

  std::string retv;

  nlohmann::json js = nullptr;
  if (!header_param.empty()) {
    // Call performRequest with the extra argument
    retv =
        m_request.performRequest(current_path, Action::GET, js, header_param);
  } else {
    // Call performRequest without the extra argument
    retv = m_request.performRequest(current_path, Action::GET, js);
  }
  nlohmann::json response = StringUtils::getJson(retv);

  IovSetDto dto = IovSetDto::fromJson(response);

  return dto;
}

IovSetDto CrestApi::selectIovs(const std::string &name,
                               const std::string &since,
                               const std::string until, long snapshot, int size,
                               int page, const std::string &sort,
                               const std::string timeformat,
                               const std::string &method, int group_size,
                               const std::string &header_param) {

  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::IOVS];

  current_path += "?tagname=";

  current_path += name;
  current_path += "&since=";
  current_path += since;
  current_path += "&until=";
  // if (until == static_cast<uint64_t>(-1))
  if (until == "-1") {
    current_path += "INF";
  } else {
    current_path += until;
  }
  current_path += "&snapshot=";
  current_path += std::to_string(snapshot);

  current_path += "&size=";
  current_path += std::to_string(size);
  current_path += "&page=";
  current_path += std::to_string(page);
  current_path += "&sort=";
  current_path += sort;

  current_path += "&timeformat=";
  current_path += timeformat;
  current_path += "&method=";
  current_path += method;
  current_path += "&groupsize=";
  current_path += std::to_string(group_size);

  std::string retv;

  nlohmann::json js = nullptr;
  if (!header_param.empty()) {
    // Call performRequest with the extra argument
    retv =
        m_request.performRequest(current_path, Action::GET, js, header_param);
  } else {
    // Call performRequest without the extra argument
    retv = m_request.performRequest(current_path, Action::GET, js);
  }
  nlohmann::json response = StringUtils::getJson(retv);

  IovSetDto dto = IovSetDto::fromJson(response);

  return dto;
}

IovSetDto CrestApi::selectIovsRanges(const std::string &name, uint64_t since,
                                     uint64_t until, long snapshot, int size,
                                     int page, const std::string &sort,
                                     const std::string &header_param) {
  return selectIovs(name, since, until, snapshot, size, page, sort,
                    header_param);
}

IovSetDto CrestApi::selectGroups(const std::string &name, long snapshot,
                                 int size, int page, const std::string &sort,
                                 int group_size) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::IOVS];

  current_path += "?method=";
  current_path += iovMethod[IovMethodIndex::METHOD_GROUPS];
  current_path += "&tagname=";

  current_path += name;
  current_path += "&snapshot=";
  current_path += std::to_string(snapshot);

  //
  current_path += "&size=";
  current_path += std::to_string(size);
  current_path += "&page=";
  current_path += std::to_string(page);
  current_path += "&sort=";
  current_path += sort;
  current_path += "&groupsize=";
  current_path += std::to_string(group_size);
  //

  std::string retv;

  nlohmann::json js = nullptr;
  retv = m_request.performRequest(current_path, Action::GET, js);
  nlohmann::json response = StringUtils::getJson(retv);

  IovSetDto dto = IovSetDto::fromJson(response);

  return dto;
}

IovSetDto CrestApi::storeIovs(const IovSetDto &iovsetdto) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::IOVS];
  //
  std::string retv;

  nlohmann::json js = nullptr;
  if (iovsetdto.getResources().size() > 0) {
    js = iovsetdto.toJson();
    retv = m_request.performRequest(current_path, Action::POST, js);
    nlohmann::json response = StringUtils::getJson(retv);

    IovSetDto dto = IovSetDto::fromJson(response);
    return dto;
  } else {
    throw CrestException(
        "ERROR in CrestApi::storeIovs: the iovsetdto is empty.");
  }
}

//==============================================================================================================
// Payload methods:
void CrestApi::storeData(const std::string &tag,
                         const StoreSetDto &storeSetJson,
                         const std::string &payloadFormat,
                         const std::string &objectType,
                         const std::string &compressionType,
                         const std::string &version, uint64_t endTime) {

  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::PAYLOADS];
  std::vector<std::string> files;
  std::string retv;
  nlohmann::json js = storeSetJson.toJson();
  if (payloadFormat == "JSON") {
    retv = m_request.uploadPayload(current_path, tag, endTime, js, objectType,
                                   compressionType, version, files);
  } else {
    // Assumes the data content in the JSON is just a file path.
    nlohmann::json resources = nullptr;

    auto subjectIdIter1 = js.find("resources");
    if (subjectIdIter1 != js.end()) {
      resources = js["resources"];
    }
    int partN = resources.size();
    for (int i = 0; i < partN; i++) {
      nlohmann::json element = resources[i];
      std::string file_param;

      auto subjectIdIter1 = element.find("data");
      if (subjectIdIter1 != element.end()) {
        file_param = element["data"];
      }
      //
      int found_dots = file_param.find_first_of(':');
      int word_size = file_param.size();
      std::string data_file = file_param.substr(found_dots + 3, word_size);
      files.push_back(data_file);
    }
    retv = m_request.uploadPayload(current_path, tag, endTime, js, objectType,
                                   compressionType, version, files);
  }
}

std::string CrestApi::getPayload(const std::string &hash) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::PAYLOADS];
  current_path += urlPaths[UrlPathIndex::DATA];
  current_path += "?format=BLOB&hash=";
  current_path += hash;
  std::string retv;
  nlohmann::json js = nullptr;
  retv = m_request.performRequest(current_path, Action::GET, js);
  checkHash(hash, retv);
  return retv;
}

PayloadDto CrestApi::getPayloadMeta(const std::string &hash) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::PAYLOADS];
  current_path += urlPaths[UrlPathIndex::DATA];
  current_path += "?format=META&hash=";
  current_path += hash;
  std::string retv;
  nlohmann::json js = nullptr;
  retv = m_request.performRequest(current_path, Action::GET, js);
  nlohmann::json response = StringUtils::getJson(retv);

  PayloadDto dto = PayloadDto::fromJson(response);
  return dto;
}

PayloadTagInfoSetDto CrestApi::listPayloadTagInfo(const std::string &tagname,
                                                  int size, int page,
                                                  const std::string &sort) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::MONITORING_PAYLOADS];
  current_path += "?tagname=";
  current_path += StringUtils::urlEncode(tagname);

  current_path += "&size=";
  current_path += std::to_string(size);
  current_path += "&page=";
  current_path += std::to_string(page);
  current_path += "&sort=";
  current_path += sort;

  std::string retv;
  nlohmann::json js = nullptr;
  retv = m_request.performRequest(current_path, Action::GET, js);
  nlohmann::json response = StringUtils::getJson(retv);

  PayloadTagInfoSetDto dto = PayloadTagInfoSetDto::fromJson(response);

  return dto;
}

void CrestApi::checkHash(const std::string &hash, const std::string &str,
                         const std::source_location &location) {
  const std::string method_name = StringUtils::getCurrentMethod(location);
  // If hash starts with "triggerdb://" it is a special case for the
  // TriggerDB payloads, which do not have a hash.
  if (hash.starts_with("triggerdb://")) {
    return;  // No hash check needed for TriggerDB payloads.
  }
  std::string calculatedHash = getHash(str);
  if (hash != calculatedHash) {
    throw CrestException("ERROR in " + method_name + ": payload is corrupted.");
  }
  return;
}

std::string CrestApi::getStreamerInfo(const std::string &hash) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::PAYLOADS];
  current_path += urlPaths[UrlPathIndex::DATA];
  current_path += "?format=STREAMER&hash=";
  current_path += hash;
  std::string retv;
  nlohmann::json js = nullptr;
  retv = m_request.performRequest(current_path, Action::GET, js);
  return retv;
}

RunLumiSetDto CrestApi::listRunInfo(const std::string &since,
                                    const std::string &until,
                                    const std::string format,
                                    const std::string mode, int size, int page,
                                    const std::string &sort) {
  std::string current_path = m_PATH;
  current_path += urlPaths[UrlPathIndex::RUNINFO];
  current_path += "?since=";
  current_path += since;
  current_path += "&until=";
  current_path += until;
  current_path += "&format=";
  current_path += format;
  current_path += "&mode=";
  current_path += mode;

  if (size != -1) {
    current_path += "&size=";
    current_path += std::to_string(size);
  }
  if (page != -1) {
    current_path += "&page=";
    current_path += std::to_string(page);
  }
  if (sort != "") {
    current_path += "&sort=";
    current_path += sort;
  }

  std::string retv;

  nlohmann::json js = nullptr;
  retv = m_request.performRequest(current_path, Action::GET, js);
  nlohmann::json response = StringUtils::getJson(retv);

  RunLumiSetDto dto = RunLumiSetDto::fromJson(response);

  return dto;
}
//==============================================================================================================

}  // namespace Crest
