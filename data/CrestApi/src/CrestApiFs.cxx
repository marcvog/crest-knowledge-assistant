/*
  Copyright (C) 2020-2026 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/CrestApiFs.h>
#include <CrestApi/CrestRequest.h>

#include <algorithm>
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
#include <tuple>

namespace Crest {

/**
 * CrestApiFs constructor for file stotage client.
 * If CrestApiFs is created with this method the data will be stored
 * to the file storage.
 * @param rewriteIfExists - Boolean flag to reinitialize the file system.
 * @param root_folder - root directory of the CREST storage.
 */
CrestApiFs::CrestApiFs(bool rewriteIfExists, const std::string &root_folder)
    : m_root_folder(root_folder), m_isRewrite(rewriteIfExists) {
  if (m_root_folder == "") {
    m_root_folder = std::filesystem::current_path();
    m_root_folder += "/crest";
  }
  if (!std::filesystem::exists(std::filesystem::path(m_root_folder))) {
    std::filesystem::create_directory(std::filesystem::path(m_root_folder));
  }
  m_data_folder = m_root_folder + "/data";
  if (!std::filesystem::exists(std::filesystem::path(m_data_folder))) {
    std::filesystem::create_directory(std::filesystem::path(m_data_folder));
  }
}

/**
 * CrestClient destructor.
 */
CrestApiFs::~CrestApiFs() {}

// UTILITIES Methods

std::string CrestApiFs::getFileString(const std::string &path) {
  std::ifstream ifs(path);
  std::stringstream buf;
  buf << ifs.rdbuf();
  return buf.str();
}

std::string CrestApiFs::buildPath(const std::string &path,
                                  const std::string &name) {
  std::string p = m_root_folder;
  if (!std::filesystem::exists(std::filesystem::path(m_root_folder))) {
    std::filesystem::create_directory(std::filesystem::path(m_root_folder));
  }
  p += path;
  if (!std::filesystem::exists(std::filesystem::path(p))) {
    std::filesystem::create_directory(std::filesystem::path(p));
  }
  p += '/';
  p += name;
  if (!std::filesystem::exists(std::filesystem::path(p))) {
    std::filesystem::create_directory(std::filesystem::path(p));
  }
  return p;
}

void CrestApiFs::getFileList(const std::string &path) {
  std::filesystem::path p(path);
  for (auto i = std::filesystem::directory_iterator(p);
       i != std::filesystem::directory_iterator(); i++) {
    std::string file = i->path().filename().string();
    if (file != "data") {
      std::cout << file << std::endl;
    } else {
      continue;
    }
  }
}

// Global tag methods:

bool CrestApiFs::globalTagExists(const std::string &name) {
  bool tag_exists = false;
  try {
    findGlobalTag(name);
    tag_exists = true;
  } catch (...) {
  }
  return tag_exists;
}

void CrestApiFs::createGlobalTag(GlobalTagDto &globalTag) {
  std::string name = "";
  if (globalTag.getName() != "") {
    name = globalTag.getName();
  } else {
    throw CrestException(
        "ERROR in CrestApiFs::createGlobalTag: global tag name is not set.");
  }

  if (globalTagExists(name)) {
    throw CrestException(
        "ERROR in CrestApiFs::createGlobalTag: | CREST response: Conflict "
        "error message => GlobalTag already exists for name " +
        name);
  }

  std::string workDir = buildPath(s_FS_GLOBALTAG_PATH, name);
  std::string globalTagFile = workDir + s_FS_GLOBALTAG_FILE;

  if (m_isRewrite) {
    if (std::filesystem::exists(std::filesystem::path(globalTagFile))) {
      std::filesystem::remove(std::filesystem::path(globalTagFile));
    }

    std::ofstream outFile;

    outFile.open(globalTagFile.c_str());
    outFile << globalTag.toJson();
    outFile.close();
  }

  return;
}

void CrestApiFs::updateGlobalTag(GlobalTagDto &globalTag) {
  std::string name = "";
  if (globalTag.getName() != "") {
    name = globalTag.getName();
  } else {
    throw CrestException(
        "ERROR in CrestApiFs::createGlobalTag: global tag name is not set.");
  }

  if (!globalTagExists(name)) {
    throw CrestException(
        "ERROR in CrestApiFs::updateGlobalTag: | CREST response: Resource Not "
        "Found error message => Cannot find global tag " +
        name);
  }

  std::string workDir = buildPath(s_FS_GLOBALTAG_PATH, name);
  std::string globalTagFile = workDir + s_FS_GLOBALTAG_FILE;

  if (m_isRewrite) {
    if (std::filesystem::exists(std::filesystem::path(globalTagFile))) {
      std::filesystem::remove(std::filesystem::path(globalTagFile));
    }

    std::ofstream outFile;

    outFile.open(globalTagFile.c_str());
    outFile << globalTag.toJson();
    outFile.close();
  }

  return;
}

GlobalTagDto CrestApiFs::findGlobalTag(const std::string &name) {
  nlohmann::json js = nullptr;
  GlobalTagDto dto;

  std::string workDir = buildPath(s_FS_GLOBALTAG_PATH, name);
  std::string file_path = workDir + s_FS_GLOBALTAG_FILE;

  try {
    std::string tag = getFileString(file_path);
    js = nlohmann::json::parse(tag);
    dto = GlobalTagDto::fromJson(js);
  } catch (...) {
    throw CrestException(
        "ERROR in CrestApiFs::findGlobalTag: cannot get the global tag " +
        name + " form the file storage.");
  }

  return dto;
}

void CrestApiFs::removeGlobalTag(const std::string &) {
  checkFsException("CrestApiFs::removeGlobalTag");
}

GlobalTagSetDto CrestApiFs::listGlobalTags(const std::string &name, int size,
                                           int page, const std::string &sort) {

  std::string folder = m_root_folder + s_FS_GLOBALTAG_PATH;

  GlobalTagSetDto tagSet;

  bool ascending = true;
  if (sort == "name:ASC") {
    ascending = true;
  } else if (sort == "name:DESC") {
    ascending = false;
  } else {
    throw CrestException(
        "ERROR in CrestApiFs::listTags: wrong sort parameter." + sort);
  }

  int totalItems = 0;  // total global tag number
  try {
    std::vector<std::string> taglist = nameList(folder, ascending);
    std::vector<std::string> clearedTaglist;

    for (const std::string &tag : taglist) {
      std::string file_name = folder + "/" + tag + "/" + s_FS_GLOBALTAG_FILE;

      if (std::filesystem::exists(file_name)) {
        if (name != "") {
          if (isMatch(tag, name)) {
            clearedTaglist.push_back(tag);
          }
        } else {
          clearedTaglist.push_back(tag);
        }
      }
    }

    totalItems = clearedTaglist.size();  // total global tag number

    taglist = getVectorPage(clearedTaglist, size, page);
    for (const std::string &tag : taglist) {
      std::string file_name = folder + "/" + tag + "/" + s_FS_GLOBALTAG_FILE;
      GlobalTagDto dto = findGlobalTag(tag);
      tagSet.add(dto);
    }
    tagSet.setDataType("");
  } catch (const std::exception &e) {
    throw CrestException(
        "ERROR in CrestApiFs::listGlobalTags: cannot get the tag list.");
  }

  // add RespPage:
  int tPages = std::ceil((double)totalItems / size);
  RespPage respp(tagSet.size(), totalItems, tPages, page);
  tagSet.setRespPage(respp);

  return tagSet;
}

// Tag methods:

bool CrestApiFs::tagExists(const std::string &name) {
  bool tag_exists = false;
  try {
    findTag(name);
    tag_exists = true;
  } catch (...) {
  }
  return tag_exists;
}

void CrestApiFs::createTag(TagDto &tag) {
  std::string name = tag.getName();
  std::string workDir = buildPath(s_FS_TAG_PATH, name);

  if (tagExists(name)) {
    throw CrestException(
        "ERROR in CrestApiFs::createTag: | CREST response: Conflict error "
        "message => Tag already exists for name " +
        name);
  }

  if (name.compare(m_currentTag) != 0 && m_isRewrite) {
    flush();
  }
  m_currentTag = name;
  std::string tagFile = workDir + s_FS_TAG_FILE;
  std::string iovFile = workDir + s_FS_IOV_FILE;

  if (m_isRewrite) {
    if (std::filesystem::exists(std::filesystem::path(tagFile))) {
      std::filesystem::remove(std::filesystem::path(tagFile));
    }
    if (std::filesystem::exists(std::filesystem::path(iovFile))) {
      std::filesystem::remove(std::filesystem::path(iovFile));
    }
    std::ofstream outFile;
    outFile.open(tagFile.c_str());
    outFile << tag.toJson();
    outFile.close();
  }
  if (m_data.find(name) == m_data.end()) {
    m_data.insert(std::pair<std::string, nlohmann::json>(
        name, nlohmann::json(nlohmann::json::value_t::array)));
  }
}

void CrestApiFs::updateTag(TagDto &tag) {
  std::string name = tag.getName();
  std::string workDir = buildPath(s_FS_TAG_PATH, name);

  if (!tagExists(name)) {
    throw CrestException(
        "ERROR in CrestApiFs::updateTag: | CREST response: Resource Not Found "
        "error message => Tag not found: " +
        name);
  }

  if (name.compare(m_currentTag) != 0 && m_isRewrite) {
    flush();
  }
  m_currentTag = name;
  std::string tagFile = workDir + s_FS_TAG_FILE;
  std::string iovFile = workDir + s_FS_IOV_FILE;

  if (m_isRewrite) {
    if (std::filesystem::exists(std::filesystem::path(tagFile))) {
      std::filesystem::remove(std::filesystem::path(tagFile));
    }
    std::ofstream outFile;
    outFile.open(tagFile.c_str());
    outFile << tag.toJson();
    outFile.close();
  }
  if (m_data.find(name) == m_data.end()) {
    m_data.insert(std::pair<std::string, nlohmann::json>(
        name, nlohmann::json(nlohmann::json::value_t::array)));
  }
}

TagDto CrestApiFs::findTag(const std::string &name) {
  nlohmann::json js = nullptr;
  TagDto dto;

  std::string workDir = buildPath(s_FS_TAG_PATH, name);
  std::string file_path = workDir + s_FS_TAG_FILE;

  try {
    std::string tag = getFileString(file_path);
    js = nlohmann::json::parse(tag);
    dto = TagDto::fromJson(js);
  } catch (...) {
    throw CrestException("ERROR in CrestApiFs::findTag: cannot get the tag " +
                         name + " form the file storage.");
  }

  return dto;
}

void CrestApiFs::removeTag(const std::string &) {
  checkFsException("CrestApiFs::removeTag");
}

TagSetDto CrestApiFs::listTags(const std::string &name, int size, int page,
                               const std::string &sort) {

  std::string folder = m_root_folder + s_FS_TAG_PATH;

  TagSetDto tagSet;

  bool ascending = true;
  if (sort == "name:ASC") {
    ascending = true;
  } else if (sort == "name:DESC") {
    ascending = false;
  } else {
    throw CrestException(
        "ERROR in CrestApiFs::listTags: wrong sort parameter." + sort);
  }

  int totalItems = 0;  // total tag number
  try {
    std::vector<std::string> taglist = nameList(folder, ascending);
    std::vector<std::string> clearedTaglist;

    for (const std::string &tag : taglist) {
      std::string file_name = folder + "/" + tag + "/" + s_FS_TAG_FILE;

      if (std::filesystem::exists(file_name)) {
        if (name != "") {
          if (isMatch(tag, name)) {
            clearedTaglist.push_back(tag);
          }
        } else {
          clearedTaglist.push_back(tag);
        }
      }
    }

    totalItems = clearedTaglist.size();  // total tag number

    taglist = getVectorPage(clearedTaglist, size, page);
    for (const std::string &tag : taglist) {
      std::string file_name = folder + "/" + tag + "/" + s_FS_TAG_FILE;
      TagDto dto = findTag(tag);
      tagSet.add(dto);
    }
    tagSet.setDataType("");
  } catch (const std::exception &e) {
    throw CrestException(
        "ERROR in CrestApiFs::listTags: cannot get the tag list.");
  }

  // add RespPage:
  int tPages = std::ceil((double)totalItems / size);
  RespPage respp(tagSet.size(), totalItems, tPages, page);
  tagSet.setRespPage(respp);

  return tagSet;
}

void CrestApiFs::flush() {

  for (auto &item : m_data) {
    nlohmann::json iov = item.second;
    std::string name = item.first;
    std::string workDir = m_root_folder + s_FS_TAG_PATH + '/' + name;
    std::ofstream outFile;
    std::string tagFile = workDir + s_FS_IOV_FILE;
    outFile.open(tagFile.c_str());
    outFile << iov;
    outFile.close();
  }

  m_data.clear();
}

// TagMeta methods

bool CrestApiFs::tagMetaExists(const std::string &name) {
  bool tag_meta_exists = false;
  try {
    findTagMeta(name);
    tag_meta_exists = true;
  } catch (...) {
  }
  return tag_meta_exists;
}

void CrestApiFs::createTagMeta(TagMetaDto &tag) {
  std::string name = tag.getTagName();

  if (!tagExists(name)) {
    throw CrestException(
        "ERROR in CrestApiFs::createTagMeta: | CREST response: Resource Not "
        "Found "
        "error message => Tag not found: " +
        name);
  }

  if (tagMetaExists(name)) {
    throw CrestException(
        "ERROR in CrestApiFs::createTagMeta | CREST response: Conflict error "
        "message => Tag meta already exists for name " +
        name);
  }

  std::string workDir = buildPath(s_FS_TAG_PATH, name);
  std::string tagMetaFile = workDir + s_FS_TAGMETAINFO_FILE;

  if (m_isRewrite) {
    if (std::filesystem::exists(std::filesystem::path(tagMetaFile))) {
      std::filesystem::remove(std::filesystem::path(tagMetaFile));
    }

    std::ofstream outFile;

    outFile.open(tagMetaFile.c_str());
    outFile << tag.toJson();
    outFile.close();
  }
}

void CrestApiFs::updateTagMeta(TagMetaDto &tag) {
  std::string name = tag.getTagName();

  if (!tagMetaExists(name)) {
    throw CrestException(
        "ERROR in CrestApiFs::updateTagMeta | CREST response: Resource Not "
        "Found error message => Cannot find meta info for tag " +
        name);
  }

  std::string workDir = buildPath(s_FS_TAG_PATH, name);
  std::string tagMetaFile = workDir + s_FS_TAGMETAINFO_FILE;

  if (m_isRewrite) {
    if (std::filesystem::exists(std::filesystem::path(tagMetaFile))) {
      std::filesystem::remove(std::filesystem::path(tagMetaFile));
    }

    std::ofstream outFile;

    outFile.open(tagMetaFile.c_str());
    outFile << tag.toJson();
    outFile.close();
  }
}

TagMetaDto CrestApiFs::findTagMeta(const std::string &name) {
  nlohmann::json js = nullptr;
  TagMetaDto dto;

  std::string workDir = buildPath(s_FS_TAG_PATH, name);
  std::string file_path = workDir + s_FS_TAGMETAINFO_FILE;

  try {
    std::string tag = getFileString(file_path);
    js = nlohmann::json::parse(tag);
    dto = TagMetaDto::fromJson(js);
  } catch (...) {
    throw CrestException(
        "ERROR in CrestApiFs::findTagMeta: cannot get the tag " + name +
        " form the file storage.");
  }

  return dto;
}

// GlobalTagMap methods

void CrestApiFs::createGlobalTagMap(GlobalTagMapDto &globalTagMap) {
  nlohmann::json js = globalTagMap.toJson();

  // global tag name:
  std::string name = "";
  try {
    name = static_cast<std::string>(js["globalTagName"]);
  } catch (...) {
    throw CrestException(
        "ERROR in CrestClient::createGlobalTagMap: cannot get "
        "the global tag name from JSON.");
  }

  // tag name:
  std::string tagname = "";
  try {
    tagname = static_cast<std::string>(js["tagName"]);
  } catch (...) {
    throw CrestException(
        "ERROR in CrestClient::createGlobalTagMap: cannot get "
        "the tag name from JSON.");
  }

  std::string workDir = buildPath(s_FS_GLOBALTAG_PATH, name);
  std::string catalogFile = workDir + s_FS_MAP_FILE;

  if (std::filesystem::exists(std::filesystem::path(catalogFile))) {
    // cathalogue file exists:

    std::string array_lst = getFileString(catalogFile);

    nlohmann::json cathalogue;
    try {
      cathalogue = nlohmann::json::parse(array_lst);
    } catch (...) {
      throw CrestException(
          "ERROR in CrestApiFs::createGlobalTagMap: global "
          "tag map file corrupted.");
    }

    if (std::filesystem::exists(std::filesystem::path(catalogFile))) {
      // the file storage contains the record of the global tag map:
      int m = cathalogue.size();
      for (int i = 0; i < m; i++) {
        const std::string &tn = cathalogue[i]["tagName"];
        if (tn == tagname) {
          cathalogue.erase(i);
        }
      }
    } else {
      // the file storage does not contain the record of the global tag map:
    }
    std::filesystem::remove(std::filesystem::path(catalogFile));
    cathalogue.push_back(js);
    std::ofstream outFile;

    outFile.open(catalogFile.c_str());
    outFile << cathalogue;
    outFile.close();
  } else {
    // cathalogue file does not exist (creation):

    nlohmann::json cathalogue = nlohmann::json::array();
    cathalogue.push_back(js);

    std::ofstream outFile;

    outFile.open(catalogFile.c_str());
    outFile << cathalogue;
    outFile.close();
  }

  return;
}

void CrestApiFs::removeGlobalTagMap(const std::string &, const std::string &,
                                    const std::string &, const std::string &) {
  checkFsException("CrestApiFs::removeGlobalTagMap");
}

void CrestApiFs::checkFsException(const char *method_name) {
  throw CrestException("ERROR in " + std::string(method_name) +
                       " This methods is unsupported for FILESYSTEM mode");
}

GlobalTagMapSetDto CrestApiFs::findGlobalTagMap(
    const std::string &name, const std::string &xCrestMapMode) {

  nlohmann::json js = nlohmann::json::array();

  if (xCrestMapMode != "Trace") {
    throw CrestException(
        "ERROR in CrestApiFs::getGlobalTagMap: not supported "
        "value for the parameter xCrestMapMode = " +
        xCrestMapMode);
  }

  std::string workDir = m_root_folder + s_FS_GLOBALTAG_PATH;
  std::string file_path = workDir;
  file_path += '/';
  file_path += name;
  file_path += s_FS_MAP_FILE;

  try {
    std::string tag = getFileString(file_path);
    js = nlohmann::json::parse(tag);
  } catch (...) {
    // the empty global tag map set earlier
  }

  GlobalTagMapSetDto dto;
  dto = GlobalTagMapSetDto::fromFsJson(js);

  // add RespPage:
  int totalItems = dto.size();
  RespPage respp(totalItems, totalItems, 1,
                 0);  // full global tag map in one response
  dto.setRespPage(respp);

  return dto;
}

// Iovs methods

IovSetDto CrestApiFs::selectIovs(
    const std::string &name, uint64_t since, uint64_t until,
    [[maybe_unused]] long snapshot, int size, int page, const std::string &sort,
    [[maybe_unused]] const std::string &header_param) {
  IovSetDto dto;

  nlohmann::json js = nlohmann::json::array();
  int niovs = 0;

  try {
    nlohmann::json iovList = findAllIovs(name);
    niovs = iovList.size();

    for (int i = 0; i < niovs; i++) {
      if (iovList[i].find("since") != iovList[i].end()) {
        uint64_t currentS = iovList[i]["since"];

        if (until != static_cast<uint64_t>(-1)) {
          if (currentS >= since && currentS <= until) {
            js.push_back(iovList[i]);
          }
        } else {  // until == -1, Infinity
          if (currentS >= since) {
            js.push_back(iovList[i]);
          }
        }  // until == -1
      }
    }  // for
  } catch (...) {
    throw CrestException(
        "ERROR in CrestClient::selectIovs : cannot get the "
        "iov list from the file storage");
  }

  bool ascending = true;
  if (sort == "id.since:ASC")
    ascending = true;
  else if (sort == "id.since:DESC")
    ascending = false;
  else {
    throw CrestException(
        "ERROR in CrestApiFs::selectIovs: wrong sort parameter." + sort);
  }

  nlohmann::json sorted = sortIOVJson(js, ascending);
  nlohmann::json ext = getPage(sorted, size, page);
  dto = IovSetDto::fromFsJson(ext);

  // add RespPage:
  int tPages = std::ceil((double)niovs / size);
  RespPage respp(dto.size(), niovs, tPages, page);
  dto.setRespPage(respp);

  return dto;
}

IovSetDto CrestApiFs::selectIovs(const std::string &name,
                                 const std::string &since,
                                 const std::string until, long snapshot,
                                 int size, int page, const std::string &sort,
                                 const std::string timeformat,
                                 const std::string &method, int group_size,
                                 const std::string &header_param) {
  IovSetDto dto;
  checkFsException("CrestApiFs::selectIovs");
  return dto;
}

IovSetDto CrestApiFs::selectIovsRanges(const std::string &name, uint64_t since,
                                       uint64_t until, long snapshot, int size,
                                       int page, const std::string &sort,
                                       const std::string &header_param) {
  return selectIovs(name, since, until, snapshot, size, page, sort,
                    header_param);
}

nlohmann::json CrestApiFs::findAllIovs(const std::string &tagname) {
  nlohmann::json js = nlohmann::json::array();
  std::string file_path = m_root_folder;
  file_path += '/';
  file_path += s_FS_TAG_PATH;
  file_path += '/';
  file_path += tagname;
  file_path += s_FS_IOV_FILE;

  // If the file doesn't exist return an empty array
  if (!std::filesystem::exists(std::filesystem::path(file_path))) {
    return js;
  }

  try {
    std::string tag = getFileString(file_path);
    js = nlohmann::json::parse(tag);
  } catch (const std::exception &e) {
    throw CrestException(
        "ERROR in CrestApi::findAllIovs : cannot get the "
        "iov information from the file storage ");
  }

  return js;
}

int CrestApiFs::getSize(const std::string &tagname) {
  int res = 0;

  try {
    nlohmann::json iovs = findAllIovs(tagname);
    int length = iovs.size();
    return length;
  } catch (...) {
    return res;
  }

  return res;
}

IovSetDto CrestApiFs::selectGroups(const std::string &name, long snapshot,
                                   int size, int page, const std::string &sort,
                                   int group_size) {

  IovSetDto dto;
  nlohmann::json iovList = nlohmann::json::array();

  try {
    iovList = findAllIovs(name);
  } catch (...) {
    throw CrestException(
        "ERROR in CrestClient::selectGroups : cannot get the "
        "data from the file storage");
  }

  bool ascending = true;
  if (sort == "id.since:ASC")
    ascending = true;
  else if (sort == "id.since:DESC")
    ascending = false;
  else {
    throw CrestException(
        "ERROR in CrestApiFs::selectGroups: wrong sort parameter." + sort);
  }

  nlohmann::json sorted = sortIOVJson(iovList, ascending);
  int nTotal = sorted.size();
  // int nRes = std::ceil((double)nTotal / group_size);
  int nRes = std::floor((double)nTotal / group_size);
  nlohmann::json iovRes = nlohmann::json::array();
  for (int i = 0; i < nRes; i++) {
    nlohmann::json js = sorted[i * group_size];
    if ((js.find("since") != js.end()) && (js.find("tagName") != js.end())) {
      nlohmann::json new_js = {{"tagName", js["tagName"]},
                               {"since", js["since"]},
                               {"payloadHash", ""},
                               {"insertionTime", ""}};
      iovRes.push_back(new_js);
    } else {
      throw CrestException(
          "ERROR in CrestApiFs::selectGroups: cannot parse iovs.json");
    }
  }

  nlohmann::json ext = getPage(iovRes, size, page);
  dto = IovSetDto::fromFsJson(ext);

  // add RespPage:
  int tPages = std::ceil((double)nRes / size);
  RespPage respp(dto.size(), nRes, tPages, page);
  dto.setRespPage(respp);

  return dto;
}

IovSetDto CrestApiFs::storeIovs(const IovSetDto &iovsetdto) {
  checkFsException("CrestApiFs::storeIovs: not supported for FILESYSTEM mode");
  return IovSetDto();
}

// Payload methods
void CrestApiFs::storeData(const std::string &tag,
                           const StoreSetDto &storeSetJson,
                           const std::string &payloadFormat,
                           const std::string &objectType,
                           const std::string &compressionType,
                           const std::string &version,
                           [[maybe_unused]] uint64_t endTime) {
  std::string name = tag;

  std::string workDir = buildPath(s_FS_TAG_PATH, name);
  std::string tagFile = workDir + s_FS_TAG_FILE;
  std::string iovFile = workDir + s_FS_IOV_FILE;

  nlohmann::json js_data = storeSetJson.toJson();

  std::vector<StoreDto> res = storeSetJson.getResources();

  try {
    for (auto &kvp : res) {
      std::string payload = kvp.getData();
      uint64_t since = kvp.getSince();
      std::string streamer = kvp.getStreamerInfo();
      // Register everything on the file system.
      storePayloadDump(tag, since, payload, payloadFormat, objectType,
                       compressionType, version, streamer);
    }
  }  // end of try
  catch (...) {
    throw CrestException(
        "ERROR in CrestApiFs::storeData cannot store the data in a file");
  }  // end of catch
  flush();
}

std::string CrestApiFs::getFirstLetters(const std::string &str) {
  std::string result = str.substr(0, s_FS_PREFIX_LENGTH);
  return result;
}

void CrestApiFs::storePayloadDump(const std::string &tag, uint64_t since,
                                  const std::string &js,
                                  const std::string &payloadFormat,
                                  const std::string &objectType,
                                  const std::string &compressionType,
                                  const std::string &version,
                                  const std::string &streamerInfo) {

  std::ofstream outFile;

  std::string hashCode;
  std::string payloadLocalFile;

  // payload file:
  if (payloadFormat == "JSON") {
    hashCode = getHash(js);
  } else {

    int found_dots = js.find_first_of(':');
    int word_size = js.size();
    payloadLocalFile = js.substr(found_dots + 3, word_size);

    hashCode = getHashForFile(payloadLocalFile);
  }

  std::string workDir = m_data_folder;
  workDir += '/';
  workDir += getFirstLetters(hashCode);
  if (!std::filesystem::exists(std::filesystem::path(workDir))) {
    std::filesystem::create_directory(std::filesystem::path(workDir));
  }
  workDir += '/';
  workDir += hashCode;
  if (!std::filesystem::exists(std::filesystem::path(workDir))) {
    std::filesystem::create_directory(std::filesystem::path(workDir));
  }
  std::string tagFile = workDir + "/payload.json";

  if (payloadFormat == "JSON") {
    outFile.open(tagFile);
    outFile << js;
    outFile.close();
  } else {
    try {
      std::filesystem::copy_file(payloadLocalFile, tagFile);
    } catch (std::filesystem::filesystem_error &e) {
      throw CrestException(
          "ERROR in CrestApiFs::storePayloadDump cannot not "
          "save payload file: " +
          tagFile + e.what());
    }
  }

  // Define the meta info for the payload:
  nlohmann::json jsn = {{"hash", hashCode},
                        {"checkSum", "SHA-256"},
                        {"objectType", objectType},
                        {"version", version},
                        {"size", js.size()},
                        {"compressionType", compressionType},
                        {"insertionTime", StringUtils::getDateAndTime()}};

  // payload meta info file:
  std::string metaFile = workDir + "/meta.json";

  outFile.open(metaFile);
  outFile << jsn.dump();
  outFile.close();

  // streamer info:
  createStreamerInfo(hashCode, streamerInfo);

  // check if data exists

  if (m_data.find(tag) == m_data.end()) {
    try {
      nlohmann::json jsi = findAllIovs(tag);
      m_data.insert(std::pair<std::string, nlohmann::json>(tag, jsi));
    } catch (...) {
      try {  // tag exists, but there are no IOVs
        nlohmann::json tagJS = findTag(tag).toJson();
        nlohmann::json jsFree = nlohmann::json::array({});
        m_data.insert(std::pair<std::string, nlohmann::json>(tag, jsFree));
      } catch (...) {
        throw CrestException(
            "ERROR in CrestApiFs::storePayloadDump cannot get data for tag \"" +
            tag + "\" from file storage.");
      }
    }
  }

  std::map<std::string, nlohmann::json>::iterator it = m_data.find(tag);
  if (it != m_data.end()) {
    std::string link = hashCode;
    nlohmann::json iovs = it->second;
    nlohmann::json obj(nlohmann::json::value_t::object);
    obj["tagName"] = tag;
    obj["since"] = since;
    obj["insertionTime"] = StringUtils::getDateAndTime();
    obj["payloadHash"] = link;
    iovs.push_back(obj);
    m_data[it->first] = iovs;
  }
}

// Payload methods

std::string CrestApiFs::getPayload(const std::string &hash) {
  std::string workDir = m_data_folder;
  workDir += '/';
  workDir += getFirstLetters(hash);
  workDir += '/';
  workDir += hash;
  std::string filePath = workDir + "/payload.json";
  std::string res = "";

  try {
    if (std::filesystem::exists(filePath)) {
      res = getFileString(filePath);
    } else {
      throw CrestException("payload with hash " + hash + " does not exist.");
    }
  } catch (const std::exception &e) {
    std::string message = e.what();
    throw CrestException(
        "ERROR in CrestApiFs::getPayload cannot get the "
        "payload form file storage, " +
        message);
  }

  return res;
}

PayloadDto CrestApiFs::getPayloadMeta(const std::string &hash) {
  nlohmann::json js = nullptr;
  PayloadDto dto;

  std::string workDir = m_data_folder;
  workDir += '/';
  workDir += getFirstLetters(hash);
  workDir += '/';
  workDir += hash;

  std::string filePath = workDir;
  filePath += "/meta.json";

  std::string res = "";

  try {
    if (std::filesystem::exists(filePath)) {
      res = getFileString(filePath);
      js = nlohmann::json::parse(res);
      dto = PayloadDto::fromJson(js);
    } else {
      throw CrestException("payload meta info with hash " + hash +
                           " does not exist.");
    }
  } catch (const std::exception &e) {
    std::string message = e.what();
    throw CrestException(
        "ERROR in CrestClient::getPayloadMeta cannot get the "
        "payload meta info form file storage, " +
        message);
  }

  return dto;
}

std::string CrestApiFs::getStreamerInfo(const std::string &hash) {
  std::string workDir = m_data_folder;
  workDir += '/';
  workDir += getFirstLetters(hash);
  workDir += '/';
  workDir += hash;
  std::string filePath = workDir + "/streamer.json";
  std::string res = "";

  try {
    if (std::filesystem::exists(filePath)) {
      res = getFileString(filePath);
    } else {
      throw CrestException("streamer info with hash " + hash +
                           " does not exist.");
    }
  } catch (const std::exception &e) {
    std::string message = e.what();
    throw CrestException(
        "ERROR in CrestApiFs::getStreamerInfo cannot get the "
        "streamer info form file storage, " +
        message);
  }

  return res;
}

void CrestApiFs::createStreamerInfo(const std::string &hash,
                                    const std::string streamerInfo) {
  std::string workDir = m_data_folder;
  workDir += '/';
  workDir += getFirstLetters(hash);
  workDir += '/';
  workDir += hash;
  std::string filePath = workDir + "/streamer.json";

  if (m_isRewrite) {
    if (std::filesystem::exists(std::filesystem::path(filePath))) {
      std::filesystem::remove(std::filesystem::path(filePath));
    }

    std::ofstream outFile;

    outFile.open(filePath.c_str());
    outFile << streamerInfo;
    outFile.close();
  }
}
//

nlohmann::json CrestApiFs::getPage(nlohmann::json data, int size, int page) {
  nlohmann::json js = nlohmann::json::array();
  int dataSize = data.size();

  if (dataSize == 0)
    return js;  // the data is absent

  // index interval to load the data from JSON array:
  int kmin = size * page;
  int kmax = size * (page + 1);

  // check if the interval is correct:
  if (kmin > dataSize - 1)
    return js;  // out of range

  if (kmax > dataSize - 1) {  // this step is not full
    kmax = dataSize;
  }

  for (int i = kmin; i < kmax; i++) {
    js.push_back(data[i]);
  }
  return js;
}

std::vector<std::string> CrestApiFs::getVectorPage(
    const std::vector<std::string> &data, int size, int page) {
  std::vector<std::string> res;
  int dataSize = data.size();

  if (dataSize == 0)
    return res;  // the data is absent

  // index interval to load the data from JSON array:
  int kmin = size * page;
  int kmax = size * (page + 1);

  // check if the interval is correct:
  if (kmin > dataSize - 1)
    return res;  // out of range

  if (kmax > dataSize - 1) {  // this step is not full
    kmax = dataSize;
  }

  for (int i = kmin; i < kmax; i++) {
    res.push_back(data[i]);
  }
  return res;
}

std::vector<std::string> CrestApiFs::nameList(std::string &folder,
                                              bool ascending) {
  std::vector<std::string> tag_list;
  std::filesystem::path p(folder);

  for (auto i = std::filesystem::directory_iterator(p);
       i != std::filesystem::directory_iterator(); i++) {
    std::string file = i->path().filename().string();
    tag_list.push_back(file);
  }

  std::sort(tag_list.begin(), tag_list.end());

  if (ascending == false) {
    std::reverse(tag_list.begin(), tag_list.end());
  }

  return tag_list;
}

// method to sort JSON array by the key (par) value
nlohmann::json CrestApiFs::sortJson(nlohmann::json js, const std::string &par,
                                    bool order) {
  nlohmann::json respond = nlohmann::json::array();
  std::vector<std::string> parlist;
  std::map<std::string, nlohmann::json> m;

  int size = js.size();
  for (int i = 0; i < size; i++) {
    nlohmann::json elem = js[i];

    auto res = elem.find(par);

    if (res != elem.end()) {
      std::string par_val = elem[par];
      parlist.push_back(par_val);
      m[par_val] = elem;
    }
  }

  std::sort(parlist.begin(), parlist.end());

  if (order == false) {
    std::reverse(parlist.begin(), parlist.end());
  }

  for (std::string item : parlist) {
    respond.push_back(m[item]);
  }

  return respond;
}

// method to sort JSON array with IOVs by the since value
nlohmann::json CrestApiFs::sortIOVJson(nlohmann::json js, bool order) {
  std::string p_since = "since";
  std::string p_hash = "payloadHash";
  std::vector<std::tuple<double, std::string, nlohmann::json> > v;
  nlohmann::json respond = nlohmann::json::array();

  int size = js.size();
  for (int i = 0; i < size; i++) {
    nlohmann::json elem = js[i];
    double since = 0.0;
    std::string hash;

    auto s = elem.find(p_since);

    if (s != elem.end()) {
      since = elem[p_since];
    } else {
      throw CrestException(
          "ERROR in CrestApiFs::sortIOVJson: cannot get since parameter from "
          "IOV list.");
    }

    auto h = elem.find(p_hash);

    if (h != elem.end()) {
      hash = elem[p_hash];
    } else {
      throw CrestException(
          "ERROR in CrestApiFs::sortIOVJson: cannot get payloadHash parameter "
          "from IOV list.");
    }

    v.emplace_back(since, hash, elem);
  }

  sort(v.begin(), v.end());
  if (!order) {
    std::reverse(v.begin(), v.end());
  }

  for (const auto &i : v) {
    std::cout << get<0>(i) << ", " << get<1>(i) << ", " << get<2>(i)
              << std::endl;
  }

  for (const auto &i : v) {
    respond.push_back(get<2>(i));
  }

  return respond;
}

bool CrestApiFs::isMatch(const std::string &word, long unsigned int n,
                         const std::string &pattern, long unsigned int m) {
  if (m == pattern.size()) {
    return n == word.size();
  }

  if (n == word.size()) {
    for (long unsigned int i = m; i < pattern.size(); i++) {
      if (pattern[i] != '%') {
        return false;
      }
    }

    return true;
  }

  if (pattern[m] == '?' || pattern[m] == word[n]) {
    return isMatch(word, n + 1, pattern, m + 1);
  }

  if (pattern[m] == '%') {
    return isMatch(word, n + 1, pattern, m) || isMatch(word, n, pattern, m + 1);
  }

  return false;
}

bool CrestApiFs::isMatch(const std::string &word, const std::string &pattern) {
  return isMatch(word, 0, pattern, 0);
}

std::string CrestApiFs::getCrestVersion() {
  throw CrestException(
      "ERROR in CrestApiFs::getCrestVersion: cannot get the "
      "CREST server version for file storage.");
}

PayloadTagInfoSetDto CrestApiFs::listPayloadTagInfo(const std::string &tagname,
                                                    int size, int page,
                                                    const std::string &sort) {
  checkFsException("CrestApiFs::listPayloadTagInfo");
  return PayloadTagInfoSetDto();
}

}  // namespace Crest
