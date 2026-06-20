/*
   Copyright (C) 2019-2025 CERN for the benefit of the ATLAS collaboration
 */

/*!
   \file
   \brief Header file for CREST C++ Client Library

   This file contains the CrestApi class.
   (See CrestApiExt.h for CrestApi class extention with additional methods.)
   Here is the method description.
 */

#ifndef CRESTAPI_CRESAPI_H
#define CRESTAPI_CRESAPI_H

#include <CrestApi/CrestApiBase.h>
#include <CrestApi/CrestRequest.h>
#include <curl/curl.h>

#include <cstdint>
#include <iosfwd>
#include <iostream>
#include <list>
#include <map>
#include <string>

#include "nlohmann/json.hpp"

// DTOs:
#include <CrestApi/GlobalTagMapSetDto.h>
#include <CrestApi/GlobalTagSetDto.h>
#include <CrestApi/IovSetDto.h>
#include <CrestApi/PayloadDto.h>
#include <CrestApi/PayloadSetDto.h>
#include <CrestApi/PayloadTagInfoSetDto.h>
#include <CrestApi/RunLumiSetDto.h>
#include <CrestApi/StoreSetDto.h>
#include <CrestApi/TagMetaSetDto.h>
#include <CrestApi/TagSetDto.h>

namespace Crest {

class CrestApi : public CrestApiBase {
 private:
  std::string m_PATH = DEFAULT_CREST_API_VERSION;

  // ... other constants
  std::string m_base_url = "http://localhost:8080";

  Crest::CrestRequest m_request = Crest::CrestRequest();

  /**
   * This is an auxiliary method to read the CREST Server properties.
   * @return JSON with CREST server properties.
   *
   * <pre>
   *
   * Example:
   *  {
   *   "build":{
   *     "artifact":"crestdb",
   *     "name":"crestdb",
   *     "time":"2023-12-02T15:21:57.045Z",
   *     "version":"4.2.1",
   *     "group":"hep.crest"
   *   }
   *  }
   * </pre>
   */
  nlohmann::json getMgmtInfo();

  /**
   * This is an auxiliary method to extract a major version number from
   * full version string.
   * @return  major version number.
   */
  int getMajorVersion(std::string &str);

  /**
   * This method checks the hash of payload from IOV (hash) with
   * the hash calculated for the payload in the std::string (str)
   * (It is an auxiliary method to check the payload.)
   * @param hash - the hash from an IOV.
   * @param str - the payload in std::string.
   * @param location - the location (the method) where this method was called.
   */
  void checkHash(
      const std::string &hash, const std::string &str,
      const std::source_location &location = std::source_location::current());

 public:
  /**
   * CrestApi constructor.
   * @param host - host name of the CREST Server.
   * @param port - port of the CREST Server.
   * @param protocol - protocol of the CREST Server (http or https).
   * @param check_version - the parameter to switch CREST version checking, if
   * this parameter is true,
   * @param apipath - the parameter to provide the default api path, can be used
   * to force a give path. the CREST version test will be executed.
   */
  CrestApi(const std::string &host, const std::string &port,
           const std::string &protocol = "http", bool checkVersion = false,
           std::string_view apipath = DEFAULT_CREST_API_VERSION);

  /**
   * CrestApi constructor.
   * @param url - URL address of the CREST Server (with port).
   * @param apipath - the parameter to provide the default api path, can be used
   * to force a give path.
   * @param check_version - the parameter to switch CREST version checking, if
   * this parameter is true, the CREST version test will be executed. <br>
   *
   * Example:
   * <br>
   * <pre>
   *    std::string url = "http://mvg-test-pc-03.cern.ch:8090";
   *    CrestApi myCrestApi = CrestApi(url);
   * </pre>
   */
  CrestApi(std::string_view url,
           std::string_view apipath = DEFAULT_CREST_API_VERSION,
           bool checkVersion = false);

  ~CrestApi();

  inline static const bool s_CREST_CLIENT_CHECK = false;

  /**
   * This method is a CREST version test. It checks if the major CREST server
   * is equal to the major CrestApi vesrion.
   * If the versions are different an error is thrown.
   * \exception std::runtime_error - error, if the versions are different.
   */
  void checkCrestVersion();

  // Overrides

  // GlobaTag methods

  /**
   * This method creates a global tag on CREST server.
   * @param globalTag - global tag as GlobalTagDto.
   *
   * <pre>
   *
   * Example:
   *
   * // The global tag DTO initialization with constructor:
   *
   *  GlobalTagDto dto(tagname, "test", "1", "M");
   *  dto.setValidity(0);
   *  dto.setType("t");
   *
   * // Or the initialization with JSON:
   *
   *  nlohmann::json js =
   *      {
   *          {"name", tagname},
   *          {"validity", 0},
   *          {"description", "test"},
   *          {"release", "1"},
   *          {"insertionTime", "2018-12-18T11:32:58.081+0000"},
   *          {"snapshotTime", "2018-12-18T11:32:57.952+0000"},
   *          {"scenario", "test"},
   *          {"workflow", "M"},
   *          {"type", "t"},
   *      };
   *
   *  GlobalTagDto dto = GlobalTagDto();
   *  dto = dto.fromJson(js);
   *
   *  // The method call:
   *
   *  myCrestApi.createGlobalTag(dto);
   * </pre>
   */
  void createGlobalTag(GlobalTagDto &globalTag) override;

  /**
   * This method updates the global tag on the CREST server.
   * @param globalTag - global tag as GlobalTagDto
   */
  void updateGlobalTag(GlobalTagDto &globalTag) override;

  /**
   * This method finds a global tag by name on the CREST server. Only one global
   * tag should be returned. (This method is an analogue of the find_global_tag
   * method in Python)
   * @param name - global tag name,
   * @return global tag as GlobalTagDto.
   */
  GlobalTagDto findGlobalTag(const std::string &name) override;

  /**
   * This method finds the global tags on the CREST server.
   * @param name - global tag name pattern, "%" can be used for any symbols,
   * @param size - page size,
   * @param page - page number,
   * @param sort - sorting order (name:ASC or name:DESC),
   * @return global tag list as GlobalTagSetDto.
   */
  GlobalTagSetDto listGlobalTags(const std::string &name, int size, int page,
                                 const std::string &sort) override;

  /**
   * This method removes a global tag on the CREST server.
   * (This method is an analogue of the remove_global_tag method in Python)
   * @param name  - global tag name
   */
  void removeGlobalTag(const std::string &name) override;

  // Tag methods

  /**
   * This method creates a tag on the CREST server.
   * @param tag - tag as TagDto.
   *
   * <pre>
   *
   *  Example:
   *
   * // The tag DTO initialization with constructor:
   *
   *  TagDto dto(tagname, "time", "test");
   *  dto.setSynchronization("none");
   *  dto.setObjectType("JSON");
   *  dto.setLastValidatedTime(0.0);
   *  dto.setEndOfValidity(0.0);
   *
   * // Or the initialization with JSON:
   *
   *  nlohmann::json js =
   *  {
   *    {"name", tagname},
   *    {"timeType", "time"},
   *    {"description", "test"},
   *    {"synchronization", "none"},
   *    {"insertionTime", "2018-12-18T11:32:58.081+0000"},
   *    {"modificationTime", "2018-12-18T11:32:57.952+0000"},
   *    {"payloadSpec", "JSON"},
   *    {"lastValidatedTime", 0.},
   *    {"endOfValidity", 0.},
   *  };
   *
   *  TagDto dto = TagDto();
   *  dto = dto.fromJson(js);
   *
   *  // The method call:
   *
   *  myCrestApi.createTag(dto);
   *
   * </pre>
   */
  void createTag(TagDto &tag) override;

  /**
   * This method updates the tag on the CREST server.
   * @param tag - tag as TagDto
   */
  void updateTag(TagDto &tag) override;

  /**
   * This method finds a tag by the name on the CREST server.
   * (This method is an analogue of the find_tag method in Python)
   * @param name - tag name
   * @return tag as TagDto object.
   */
  TagDto findTag(const std::string &name) override;

  /**
   * This method returns the tag list as TagSetDto from the CREST server.
   * (This method is an analogue of the list_tags method in Python)
   * @param name - tag name pattern, "%" can be used for any symbols,
   * @param size - page size,
   * @param page - page number,
   * @param sort - sorting order (name:ASC or name:DESC).
   * @return tag list as TagSetDto object.
   */
  TagSetDto listTags(const std::string &name, int size, int page,
                     const std::string &sort) override;

  /**
   * This method removes a tag from the CREST server.
   * (This method is an analogue of the remove_tag method in Python)
   * @param tagName - tag name
   */
  void removeTag(const std::string &name) override;

  /**
   * This method gets the number of IOVs for the given tag.
   * (This method is an analogue of the get_size method in Python)
   * @param tagname - tag name.
   * @return IOV number.
   */
  int getSize(const std::string &tagname) override;

  // TagMeta methods

  /**
   * This method creates a tag meta info on the CREST server.
   * @param tag - tag meta info as TagMetaDto.
   *
   * <pre>
   * Example:
   *
   *  PayloadSpecDto spec;
   *  spec.add("ModuleId", "UInt32");
   *  spec.add("ErrorCode", "UInt32");
   *  spec.add("json", "Blob64k");
   *
   *  std::string description =
   * "<timeStamp>time</timeStamp><addrHeader><address_header service_type=\"71\"
   * clid=\"1238547719\"/></addrHeader><typeName>CondAttrListCollection</typeName>";
   *  TagInfoDto taginfo(description);
   *  taginfo.setPayloadSpec(spec);
   *
   *  ChannelSetDto chanset;
   *  chanset.add("583008256", "");
   *  chanset.add("583204864", "");
   *
   *  taginfo.setChannel(chanset);
   *
   *  std::string tag_description =
   * "{\"dbname\":\"OFLP200\",\"nodeFullpath\":\"/TRIGGER/L1Calo/V1/Calibration/JfexModuleSettings\",\"schemaName\":\"COOLOFL_TRIGGER\"}";
   *  TagMetaDto metainfo(tagname, tag_description, taginfo.toJson().dump());
   *
   *  myCrestClient.createTagMeta(dto);
   * </pre
   */
  void createTagMeta(TagMetaDto &tag) override;

  /**
   * This method updates a tag meta info on the CREST server.
   * @param tag - tag meta info as TagMetaDto.
   */
  void updateTagMeta(TagMetaDto &tag) override;

  /**
   * This method reads a tag meta info by the tag name from the CREST server.
   * @param name - tag name
   * @return tag meta info as a TagMetaDto.
   */
  TagMetaDto findTagMeta(const std::string &name) override;

  // GlobalTagMap methods

  /**
   * This method creates a global tag map on the CREST server.
   * @param globalTagMap - the global tag map as GlobalTagMapDto.
   *
   * <pre>
   * Example:
   *
   * // The global tag map DTO initialization with constructor:
   *
   *  GlobalTagMapDto globalTagMap(tagname, globaltag, "test", "/MYFOLDER");
   *
   * // Or the initialization with JSON:
   *
   *  nlohmann::json js =
   *  {
   *    {"globalTagName", globaltag},
   *    {"record", "test"},
   *    {"label", "/MYFOLDER"},
   *    {"tagName", tagname}
   *  };
   *
   *  GlobalTagMapDto globalTagMap = GlobalTagMapDto();
   *  globalTagMap = globalTagMap.fromJson(js);
   *
   *  myCrestApi.createGlobalTagMap(globalTagMap);
   * </pre>
   */
  void createGlobalTagMap(GlobalTagMapDto &globalTagMap) override;

  /**
   * This method searches for tag mappings using the global tag name or tag name
   * on the CREST server.
   * (This method is an analogue of the find_global_tag_map method in Python)
   * @param name - name of a global tag or a tag
   * @param xCrestMapMode - search mode (Trace or BackTrace). If it is set as
   * "Trace" the global tag name will be used for searching, otherwise - the tag
   * name.
   * @return global tag map list as GlobalTagMapSetDto.
   */
  GlobalTagMapSetDto findGlobalTagMap(
      const std::string &name, const std::string &xCrestMapMode) override;

  /**
   * This method removes a global tag map on the CREST server.
   * @param name - the global tag name,
   * @param label - label,
   * @param tagname - tag name.
   */
  void removeGlobalTagMap(const std::string &name, const std::string &record,
                          const std::string &label,
                          const std::string &tagname) override;

  // Iovs

  /**
   * This method selects IOVs for a given tagname on the CREST server.
   * The result is an IOV list.
   * (This method is an analogue of the select_iovs method in Python)
   * @param name - tag name,
   * @param since - since time (the beginning of the time interval),
   * @param until - until time (end of the time interval),
   * @param snapshot - snapshot,
   * @param size - page size,
   * @param page - page number,
   * @param sort - sorting order (id.since:ASC or id.since:DESC)
   * @param header_param - header parameter.
   * @return an IOV list as IovSetDto.
   */
  IovSetDto selectIovs(const std::string &name, uint64_t since, uint64_t until,
                       long snapshot, int size, int page,
                       const std::string &sort,
                       const std::string &header_param = "") override;

  /**
   * This method selects IOVs for a given tagname. The result is an IOV list.
   * (This method is an analogue of the select_iovs method in Python)
   * @param name - tag name,
   * @param since - since time (the beginning of the time interval),
   * @param until - until time (end of the time interval),
   * @param snapshot - snapshot,
   * @param size - page size,
   * @param page - page number,
   * @param sort - sorting order (id.since:ASC or id.since:DESC),
   * @param timeformat - time format for since/until
   * (SEC,MS,RUN,RUN_LUMI,ISO,NUMBER),
   * @param method - the method parameter is used to determine which query is
   * executed (IOVS, GROUPS, MONITOR),
   * @param group_size - group size. The group size represent the pagination
   * type provided for GROUPS query method.
   * @param header_param - header parameter. Optional.
   * @return an IOV list as IovSetDto.
   */
  IovSetDto selectIovs(const std::string &name, const std::string &since,
                       const std::string until, long snapshot, int size,
                       int page, const std::string &sort,
                       const std::string timeformat,
                       const std::string &method = "IOVS",
                       int group_size = 1000,
                       const std::string &header_param = "") override;

  // Same as before, but with a default value for header_param
  IovSetDto selectIovsRanges(
      const std::string &name, uint64_t since, uint64_t until, long snapshot,
      int size, int page, const std::string &sort,
      const std::string &header_param = "X-Crest-Query: RANGES") override;

  /**
   * This method returns IOV groups for a given tagname on CREST server.
   * The result is an IOV list.
   * (This method is an analogue of the select_groups method in Python)
   * @param name - tag name,
   * @param snapshot - snapshot,
   * @param size - page size,
   * @param page - page number,
   * @param sort - sorting order (id.since:ASC or id.since:DESC)
   * @param group_size - group size,
   * @return an IOV groups as IovSetDto.
   */
  IovSetDto selectGroups(const std::string &name, long snapshot, int size,
                         int page, const std::string &sort,
                         int group_size = 1000) override;

  /**
   * This method store a list of iovs in an existing tag on the CREST server.
   * (This method is an analogue of the store_iovs method in Python)
   * @param IovSetDto - the iov list,
   * @return an IOV groups as IovSetDto.
   */
  IovSetDto storeIovs(const IovSetDto &storeset) override;

  // Payload methods

  /**
   * This method stores several payloads in batch mode on the CREST server.
   * (This method is an analogue of the store_data method in Python)
   * @param tag - tag name.
   * @param storeSetJson - iov and payload data as a JSON object.
   * @param payloadFormat - payload format [FILE | JSON].
   * @param objectType - object type.
   * @param compressionType - compression type.
   * @param version - version.
   * @param endTime - end time, if endtime = 0, the server does not use this
   * parameter in the internal check.
   *
   * <pre>
   * Example:
   *
   *  StoreSetDto storeSetDto;
   *
   *  StoreDto dto1(1000,"","Sample data");
   *  dto1.setStreamerInfo("Info123");
   *  storeSetDto.push_back(dto1);
   *
   *  StoreDto dto2(2000,"","Another sample for data");
   *  dto1.setStreamerInfo("Info456");
   *  storeSetDto.push_back(dto2);
   *
   *  myCrestApi.storeData(tagname, storeSetDto, "JSON", "test", "test", "1",
   * endtime);
   *
   * </pre>
   */
  void storeData(const std::string &tag, const StoreSetDto &storeSetJson,
                 const std::string &payloadFormat = "JSON",
                 const std::string &objectType = "none",
                 const std::string &compressionType = "none",
                 const std::string &version = "1.0",
                 uint64_t endTime = -1) override;

  // Payload retrieval methods

  /**
   * This method finds a payload resource associated to the hash on the CREST
   * server. The payload returns as a string. (This method is an analogue of the
   * get_payload method in Python)
   * @param hash - hash.
   * @return payload as a std::string
   */
  std::string getPayload(const std::string &hash) override;

  /**
   *  This method finds a payload meta info for the hash on the CREST server.
   * (This method is an analogue of the get_payload_meta method in Python)
   * @param hash - hash.
   * @return payload meta info as PayloadDto.
   */
  PayloadDto getPayloadMeta(const std::string &hash) override;

  /**
   *  This method finds a streamer info for the hash.
   * @param hash - hash.
   * @return streamer info.
   */
  std::string getStreamerInfo(const std::string &hash) override;

  /**
   * This method returns the full CREST Server version.
   * @return CREST server version.
   */
  std::string getCrestVersion() override;

  /**
   * This method retrieves monitoring information on payload as a list of
   * payload tag information.
   * @param tagname - tag name pattern, "%" can be used for any symbols,
   * @param size - page size,
   * @param page - page number,
   * @param sort - sorting order (name:ASC or name:DESC),
   * @return JSON payload tag info list as a PayloadTagInfoSetDto.<br>
   */
  PayloadTagInfoSetDto listPayloadTagInfo(const std::string &tagname, int size,
                                          int page,
                                          const std::string &sort) override;

  /**
   * This method retrieves run lumi information.
   * @param since - since time (the beginning of the time interval),
   * @param until - until time (end of the time interval),
   * @param format - time format for since/until ([iso, number, run-lumi]),
   * @param mode - the mode parameter is used to determine the meaning of
   * since/until [daterange, runrange].
   * @param size - page size,
   * @param page - page number,
   * @param sort - sorting order (runNumber:ASC or runNumber:DESC),
   * @return JSON run lumi info list as a RunLumiSetDto.<br>
   */
  RunLumiSetDto listRunInfo(const std::string &since, const std::string &until,
                            const std::string format, const std::string mode,
                            int size, int page, const std::string &sort);
};

}  // namespace Crest

#endif  // CRESTAPI_CRESTCLIENT_H
