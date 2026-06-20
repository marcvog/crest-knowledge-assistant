/*!
   \file
   \brief Main file

   This file contains the examples for the CREST C++ Client Library.
   Main part of the examples is commented. Please uncomment the code you need.
   Check and correct (if it is necessary) the CREST Server parameters in
   CrestApi instances.
 */

#include <CrestApi/CrestApiExt.h>
#include <CrestApi/CrestLogger.h>

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
std::string apipath = "";
int createFile(std::string filePath);

void print_path() {
  std::cout << SURL << std::endl;
}

bool createDirTree(const std::string &full_path) {
  size_t pos = 0;
  bool ret_val = true;

  while (ret_val == true && pos != std::string::npos) {
    pos = full_path.find('/', pos + 1);
    ret_val = std::filesystem::create_directory(full_path.substr(0, pos));
  }

  return ret_val;
}

//===================================================
// Global Tag Method Tests
//===================================================

void testCreateGlobalTag(const std::string &tagname) {
  std::cout << std::endl << "test: createGlobalTag" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  /*
  // OLD VERSION,
  // global tag innitialized with JSON:

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

  GlobalTagDto dto = GlobalTagDto();
  dto = dto.fromJson(js);
  */

  // global tag innitialization with the constructor:

  GlobalTagDto dto(tagname, "test", "1", "M");
  dto.setValidity(0);
  dto.setType("T");
  dto.setScenario("test");

  try {
    std::cout << std::endl
              << "test: createGlobalTag (input) = " << std::endl
              << dto.toJson().dump(4) << std::endl;
    myCrestApi.createGlobalTag(dto);
    std::cout << std::endl << "test: createGlobalTag (success) " << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: createGlobalTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
  // Use default ctor for GlobalTagDto
  GlobalTagDto dto2("test_gtag_02", "test", "1", "any");

  try {
    std::cout << std::endl
              << "test: createGlobalTag 2 (input) = " << std::endl
              << dto2.toJson().dump(4) << std::endl;
    myCrestApi.createGlobalTag(dto2);
    std::cout << std::endl << "test: createGlobalTag 2 (success) " << std::endl;
    myCrestApi.removeGlobalTag("test_gtag_02");
    std::cout << std::endl << "test: removeGlobalTag 2 (success) " << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: createGlobalTag 2 (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testFindGlobalTag(const std::string &tagname) {
  std::cout << std::endl << "test: findGlobalTag" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    GlobalTagDto dto = myCrestApi.findGlobalTag(tagname);
    std::cout << std::endl
              << "test: findGlobalTag (result) = " << std::endl
              << dto.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: findGlobalTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testListGlobalTagsParams(const std::string &name, int size, int page) {
  std::cout << std::endl
            << "test: listGlobalTagsParams with additional parameters"
            << std::endl;
  CrestApiExt myCrestApi = CrestApiExt(SURL);

  try {
    GlobalTagSetDto list_1 = myCrestApi.listGlobalTagsParams(name, size, page);
    GlobalTagSetDto list_2 = myCrestApi.listGlobalTagsParams(
        _name = name, _size = size, _page = page);
    GlobalTagSetDto list_3 =
        myCrestApi.listGlobalTagsParams(_name = "", _size = size, _page = page);

    std::cout << std::endl
              << "test: listGlobalTagsParams (result A) =" << std::endl;
    std::cout << list_1.toJson().dump(4) << std::endl;

    std::cout << std::endl
              << "test: listGlobalTagsParams (result B) =" << std::endl;
    std::cout << list_2.toJson().dump(4) << std::endl;

    std::cout << std::endl
              << "test: listGlobalTagsParams (result C) =" << std::endl;
    std::cout << list_3.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout
        << std::endl
        << "test: listGlobalTagsParams with additional parameters (failed)"
        << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testRemoveGlobalTag(const std::string &tagname) {
  std::cout << std::endl << "test: removeGlobalTag" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    myCrestApi.removeGlobalTag(tagname);
    std::cout << std::endl << "test: removeGlobalTag (success)" << std::endl;
  } catch (const CrestException &e) {
    std::cout << std::endl << "test: removeGlobalTag (failed)" << std::endl;
  }
}

//===================================================
// Tag Method Tests
//===================================================
void testCreateTag(const std::string &tagname) {
  std::cout << std::endl << "test: createTag" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  /*
  // OLD VERSION,
  // the tag innitialized with JSON:

  nlohmann::json js =
      {
          {"name", tagname},
          {"timeType", "time"},
          {"description", "test"},
          {"synchronization", "ALL"},
          {"status", "UNLOCKED"},
          {"insertionTime", "2018-12-18T11:32:58.081+0000"},
          {"modificationTime", "2018-12-18T11:32:57.952+0000"},
          {"payloadSpec", "JSON"},
          {"lastValidatedTime", 0.},
          {"endOfValidity", 0.},
      };

  TagDto dto = TagDto();
  dto = dto.fromJson(js);
  */

  // the tag innitialization with the constructor:

  TagDto dto(tagname, "time", "test");
  dto.setSynchronization("ALL");
  dto.setObjectType("JSON");
  dto.setStatus("UNLOCKED");
  dto.setLastValidatedTime(0.0);
  dto.setEndOfValidity(0.0);

  try {
    std::cout << std::endl
              << "test: createTag (input) = " << std::endl
              << dto.toJson().dump(4) << std::endl;
    myCrestApi.createTag(dto);
    std::cout << std::endl << "test: createTag (success) " << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: createTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
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

void testListTagsParams(const std::string &name, int size, int page) {
  std::cout << std::endl
            << "test: listTagsParams with additional parameters" << std::endl;
  CrestApiExt myCrestApi = CrestApiExt(SURL);

  try {
    TagSetDto list_1 = myCrestApi.listTagsParams(name, size, page);
    TagSetDto list_2 =
        myCrestApi.listTagsParams(_name = name, _size = size, _page = page);
    TagSetDto list_3 =
        myCrestApi.listTagsParams(_name = "", _size = size, _page = page);

    std::cout << std::endl << "test: listTagsParams (result A) =" << std::endl;
    std::cout << list_1.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl
              << "test: listTagsParams with additional parameters (failed)"
              << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testRemoveTag(const std::string &tagname) {
  std::cout << std::endl << "test: removeTag" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    myCrestApi.removeTag(tagname);
    std::cout << std::endl << "test: removeTag (success)" << std::endl;
  } catch (const CrestException &e) {
    std::cout << std::endl << "test: removeTag (failed)" << std::endl;
  }
}
// Test global tag map methods
void testCreateGlobalTagMap(const std::string &globaltagname,
                            const std::string &tagname) {
  std::cout << std::endl << "test: createGlobalTagMap" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  /*
  // OLD VERSION,
  // the global tag map innitialized with JSON:

  nlohmann::json js =
      {
          {"tagName", tagname},
          {"globalTagName", globaltagname},
          {"record", "None"},
          {"label", "/MYFOLDER"},
      };

  GlobalTagMapDto dto = GlobalTagMapDto();
  dto = dto.fromJson(js);
  */

  // the global tag map innitialized with constructor:

  GlobalTagMapDto dto(tagname, globaltagname, "None", "/MYFOLDER");

  try {
    std::cout << std::endl
              << "test: createGlobalTagMap (input) = " << std::endl
              << dto.toJson().dump(4) << std::endl;
    myCrestApi.createGlobalTagMap(dto);
    std::cout << std::endl
              << "test: createGlobalTagMap (success) " << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: createGlobalTagMap (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testListTagsInGlobalTag(const std::string &globaltagname) {
  std::cout << std::endl
            << "test: listTagsInGlobalTag " << globaltagname.c_str()
            << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    GlobalTagMapSetDto list =
        myCrestApi.findGlobalTagMap(globaltagname, "Trace");

    std::cout << std::endl
              << "test: listTagsInGlobalTag (Trace) =" << std::endl;
    std::cout << list.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: listTagsInGlobalTag (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testRemoveMap(const std::string &globaltagname,
                   const std::string &tagname) {
  std::cout << std::endl << "test: removeMap" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    myCrestApi.removeGlobalTagMap(globaltagname, "None", "/MYFOLDER", tagname);
    std::cout << std::endl << "test: removeMap (success)" << std::endl;
  } catch (const CrestException &e) {
    std::cout << std::endl << "test: removeMap (failed)" << std::endl;
  }
}

// Test for the storePayloads method. The payloads are calculated each time.

void testStorePayloads(const std::string &tagname) {
  std::cout << std::endl << "test: testStorePayloads" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  uint64_t endtime = 0;

  /*
  // OLD variant, StoreSetDto defined via JSON

  // Your JSON string
  std::string jsonString = R"(
      {
        "size": 2,
        "datatype": "data",
        "format": "StoreSetDto",
        "resources": [
          {
            "since": 1000,
            "data": "Sample data",
            "streamerInfo": "Info123"
          },
          {
            "since": 2000,
            "data": "Another sample for data",
            "streamerInfo": "Info456"
          }
        ]
      }
  )";

  nlohmann::json js = json::parse(jsonString);
  StoreSetDto storeSetDto = StoreSetDto::fromJson(js);
  */

  // New version:

  StoreSetDto storeSetDto;

  StoreDto dto1(1000, "", "Sample data");
  dto1.setStreamerInfo("Info123");
  storeSetDto.push_back(dto1);

  StoreDto dto2(2000, "", "Another sample for data");
  dto1.setStreamerInfo("Info456");
  storeSetDto.push_back(dto2);

  try {
    std::cout << "Storing " << storeSetDto.toJson() << std::endl;
    myCrestApi.storeData(tagname, storeSetDto, "JSON", "test", "test", "1",
                         endtime);
    std::cout << std::endl << "test: testStorePayloads (success) " << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: testStorePayloads (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testStorePayloadsFile(const std::string &tagname) {
  std::cout << std::endl << "test: testStorePayloadsFile" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  uint64_t endtime = 0;

  // Your JSON string
  std::string jsonString = R"(
      {
        "size": 2,
        "datatype": "data",
        "format": "StoreSetDto",
        "resources": [
          {
            "since": 5000,
            "data": "file:///tmp/pyld_1.txt",
            "streamerInfo": "Info123"
          },
          {
            "since": 6000,
            "data": "file:///tmp/pyld_2.txt",
            "streamerInfo": "Info456"
          }
        ]
      }
  )";

  nlohmann::json js = json::parse(jsonString);
  StoreSetDto storeSetDto = StoreSetDto::fromJson(js);

  try {
    std::cout << "Storing " << storeSetDto.toJson() << std::endl;
    myCrestApi.storeData(tagname, storeSetDto, "FILE", "test", "test", "1",
                         endtime);
    std::cout << std::endl
              << "test: testStorePayloadsFile (success) " << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl
              << "test: testStorePayloadsFile (failed)" << std::endl;
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

void testGetPayload(const std::string &tagname) {
  std::cout << std::endl << "test: listIovs" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    IovSetDto dto =
        myCrestApi.selectIovs(tagname, 0, -1, 0, 1000, 0, "id.since:ASC");
    std::cout << std::endl
              << "test: listIovs (result) = " << std::endl
              << dto.toJson().dump(4) << std::endl;
    std::vector<IovDto> iovs = dto.getResources();
    for (auto iov : iovs) {
      std::cout << "iov.since = " << iov.getSince() << std::endl;
      std::cout << "iov.payloadHash = " << iov.getPayloadHash() << std::endl;
      std::string payload = myCrestApi.getPayload(iov.getPayloadHash());
      std::cout << "payload = " << payload.substr(0, 100) << std::endl;
      PayloadDto payloadDto = myCrestApi.getPayloadMeta(iov.getPayloadHash());
      std::cout << "payloadDto = " << payloadDto.toJson().dump(4) << std::endl;
      std::cout << "      size = " << payloadDto.size() << std::endl;
      std::string streamer = myCrestApi.getStreamerInfo(iov.getPayloadHash());
      std::cout << "streamerInfo = " << streamer << std::endl;
    }
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: listIovs (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

int createFile(std::string filePath) {
  // Seed the random number generator with the current time
  std::srand(static_cast<unsigned>(std::time(0)));

  // Open the file for writing
  std::ofstream outFile(filePath);

  // Check if the file is successfully opened
  if (!outFile.is_open()) {
    std::cerr << "Error opening file for writing." << std::endl;
    return 1;  // Return an error code
  }

  // Generate and write some random content to the file
  for (int i = 0; i < 1000; ++i) {
    int randomValue =
        std::rand() % 100;  // Generate a random number between 0 and 99
    outFile << "Random value " << i + 1 << ": " << randomValue << std::endl;
  }

  // Close the file
  outFile.close();

  std::cout << "File created successfully at: " << filePath << std::endl;
  return 0;
}

void testGetSize(const std::string &tagname) {
  std::cout << std::endl << "test: getSize" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    int info = myCrestApi.getSize(tagname);
    std::cout << std::endl << "test: getSize (result) = " << info << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: getSize (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
  try {
    int info = myCrestApi.getSize("%");
    std::cout << std::endl
              << "test: getSize should not arrive here " << info << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl
              << "test: getSize (ok): exception expected ... " << std::endl;
    std::cout << e.what() << std::endl;
  }
}

//===================================================
// Tag Meta Info Method Tests
//===================================================

void testCreateTagMeta(const std::string &tagname) {
  std::cout << std::endl << "test: createTagMeta" << std::endl;

  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  /*
  {"chansize":6,\"colsize\":3,"description":"{\"dbname\":\"OFLP200\",\"nodeFullpath\":\"/TRIGGER/L1Calo/V1/Calibration/JfexModuleSettings\",\"schemaName\":\"COOLOFL_TRIGGER\"}","tagInfo":"{\"channel_list\"
  :
  [{\"583008256\":\"\"},{\"583073792\":\"\"},{\"583139328\":\"\"},{\"583204864\":\"\"},{\"583270400\":\"\"},{\"583335936\":\"\"}],\"node_description\":\"<timeStamp>time</timeStamp><addrHeader><address_header
  service_type=\\"71\\"
  clid=\\"1238547719\\"/></addrHeader><typeName>CondAttrListCollection</typeName>\",\"payload_spec\":\"ModuleId:UInt32,ErrorCode:UInt32,json:Blob64k\"}","tagName":"JfexModuleSettings-RUN3-MCDEFAULT-TEST"}
  */

  PayloadSpecDto spec;
  spec.add("ModuleId", "UInt32");
  spec.add("ErrorCode", "UInt32");
  spec.add("json", "Blob64k");

  std::string description =
      "<timeStamp>time</timeStamp><addrHeader><address_header "
      "service_type=\"71\" "
      "clid=\"1238547719\"/></addrHeader><typeName>CondAttrListCollection</"
      "typeName>";
  TagInfoDto taginfo(description);
  taginfo.setPayloadSpec(spec);

  ChannelSetDto chanset;
  chanset.add("583008256", "");
  chanset.add("583204864", "");

  taginfo.setChannel(chanset);

  std::string tag_description =
      "{\"dbname\":\"OFLP200\",\"nodeFullpath\":\"/TRIGGER/L1Calo/V1/"
      "Calibration/JfexModuleSettings\",\"schemaName\":\"COOLOFL_TRIGGER\"}";
  TagMetaDto metainfo(tagname, tag_description, taginfo.toJson().dump());

  nlohmann::json metainfo_js = metainfo.toJson();

  std::cout << "tag meta info = " << std::endl
            << metainfo_js.dump(4) << std::endl;

  try {
    myCrestApi.createTagMeta(metainfo);
    std::cout << std::endl << "test: createTagMeta (success)" << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: createTagMeta (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testCreateTagMetaOldFormat(const std::string &tagname) {
  std::cout << std::endl << "test: createTagMetaOldFormat" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  /*
  {"chansize":6,\"colsize\":3,"description":"{\"dbname\":\"OFLP200\",\"nodeFullpath\":\"/TRIGGER/L1Calo/V1/Calibration/JfexModuleSettings\",\"schemaName\":\"COOLOFL_TRIGGER\"}","tagInfo":"{\"channel_list\"
  :
  [{\"583008256\":\"\"},{\"583073792\":\"\"},{\"583139328\":\"\"},{\"583204864\":\"\"},{\"583270400\":\"\"},{\"583335936\":\"\"}],\"node_description\":\"<timeStamp>time</timeStamp><addrHeader><address_header
  service_type=\\"71\\"
  clid=\\"1238547719\\"/></addrHeader><typeName>CondAttrListCollection</typeName>\",\"payload_spec\":\"ModuleId:UInt32,ErrorCode:UInt32,json:Blob64k\"}","tagName":"JfexModuleSettings-RUN3-MCDEFAULT-TEST"}
  */
  try {

    std::string description =
        "<timeStamp>time</timeStamp><addrHeader><address_header "
        "service_type=\"71\" "
        "clid=\"1238547719\"/></addrHeader><typeName>CondAttrListCollection</"
        "typeName>";
    TagInfoDto taginfo(description);
    std::cout << "tag info description = " << taginfo.toJson().dump(4)
              << std::endl;

    // Set payload spec in old format
    PayloadSpecDto spec =
        PayloadSpecDto::fromJson("id:UInt64,hv:UInt32,data:Blob64k");
    taginfo.setPayloadSpec(spec);
    std::cout << "tag info payload_spec = " << taginfo.toJson().dump(4)
              << std::endl;

    // Set channels in old format
    json jsonInput = R"([
       {"583008256":""},{"583073792":""}
    ])"_json;
    ChannelSetDto chanset = ChannelSetDto::fromJson(jsonInput);
    taginfo.setChannel(chanset);
    std::cout << "tag info channels = " << taginfo.toJson().dump(4)
              << std::endl;

    std::string tag_description =
        "{\"dbname\":\"OFLP200\",\"nodeFullpath\":\"/TRIGGER/L1Calo/V1/"
        "Calibration/JfexModuleSettings\",\"schemaName\":\"COOLOFL_TRIGGER\"}";
    TagMetaDto metainfo(tagname, tag_description, taginfo.toJson().dump());

    myCrestApi.createTagMeta(metainfo);

    nlohmann::json metainfo_js = metainfo.toJson();

    std::cout << "test: createTagMetaOldFormat (success) = " << std::endl
              << metainfo_js.dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl
              << "test: createTagMetaOldFormat (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testFindTagMeta(const std::string &tagname) {
  std::cout << std::endl << "test: findTagMeta " << std::endl;

  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    TagMetaDto dto = myCrestApi.findTagMeta(tagname);
    std::cout << std::endl
              << "test: findTagMeta (result) =" << std::endl
              << dto.toJson().dump(4) << std::endl;
    TagInfoDto taginfo = dto.getTagInfoDto();
    std::cout << std::endl
              << "tag info = " << std::endl
              << taginfo.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: findTagMeta (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testFindExistingTagMeta(const std::string &tagname) {
  std::cout << std::endl << "test: findExistingTagMeta " << std::endl;

  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    TagMetaDto dto = myCrestApi.findTagMeta(tagname);
    std::cout << std::endl
              << "test: findExistingTagMeta (result) =" << std::endl
              << dto.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: findExistingTagMeta (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testListPayloadTagInfo(const std::string &tagname, int size, int page,
                            const std::string &sort) {
  std::cout << std::endl << "test: listPayloadTagInfo" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    PayloadTagInfoSetDto dto =
        myCrestApi.listPayloadTagInfo(tagname, size, page, sort);
    std::cout << std::endl
              << "test: listPayloadTagInfo (result) = " << std::endl
              << dto.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: listPayloadTagInfo (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testGetVersion() {
  std::cout << std::endl << "test: updateTag" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    std::string vers1 = myCrestApi.getClientVersion();
    std::cout << std::endl << "test: getClientVersion = " << vers1 << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: getClientVersion (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try {
    std::string vers2 = myCrestApi.getCrestVersion();
    std::cout << std::endl << "test: getCrestVersion = " << vers2 << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: getCrestVersion (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testTagDto() {
  std::cout << "test: testTagDto" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  TagDto dto("tag_name", "timeType", "decr");
  std::cout << "dto1 = " << std::endl << dto.toJson().dump(4) << std::endl;
  dto.setName("tag no. 2");
  std::cout << "dto1 = " << std::endl << dto.toJson().dump(4) << std::endl;
}

// Update method tests:

void testUpdateTagMeta(const std::string &tagname) {
  std::cout << std::endl << "test: updateTagMeta" << std::endl;

  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {
    TagMetaDto tag_meta = myCrestApi.findTagMeta(tagname);
    std::string old_description = tag_meta.getDescription();
    std::cout << "old tag meta description = " << old_description << std::endl;

    std::string new_description = "(new)test";
    tag_meta.setDescription(new_description);
    std::cout << "new tag meta description = " << new_description << std::endl;

    myCrestApi.updateTagMeta(tag_meta);

    TagMetaDto tag_meta_new = myCrestApi.findTagMeta(tagname);
    std::cout << "tag meta info(" << tagname << ") = " << std::endl
              << tag_meta_new.toJson().dump(4);

    std::cout << std::endl << "test: updateTagMeta (success)" << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: updateTagMeta (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testUpdateTag(const std::string &tagname) {
  std::cout << std::endl << "test: updateTag" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {

    TagDto dto = myCrestApi.findTag(tagname);

    std::cout << "old tag (" << tagname << ") = " << std::endl
              << dto.toJson().dump(4) << std::endl;

    std::string desc = dto.getDescription();
    desc = desc + "_updated";

    TagDto new_dto = dto;
    new_dto.setDescription(desc);

    myCrestApi.updateTag(new_dto);
    TagDto result = myCrestApi.findTag(tagname);

    std::cout << "new tag (" << tagname << ") = " << std::endl
              << result.toJson().dump(4) << std::endl;

    std::cout << std::endl << "test: updateTag (success) " << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: updateTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testUpdateGlobalTag(const std::string &tagname) {
  std::cout << std::endl << "test: updateGlobalTag" << std::endl;
  CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  try {

    GlobalTagDto dto = myCrestApi.findGlobalTag(tagname);

    std::cout << "old global tag (" << tagname << ") = " << std::endl
              << dto.toJson().dump(4) << std::endl;

    std::string desc = dto.getDescription();
    desc = desc + "_updated";

    GlobalTagDto new_dto = dto;
    new_dto.setDescription(desc);

    myCrestApi.updateGlobalTag(new_dto);
    GlobalTagDto result = myCrestApi.findGlobalTag(tagname);

    std::cout << "new global tag (" << tagname << ") = " << std::endl
              << result.toJson().dump(4) << std::endl;

    std::cout << std::endl << "test: updateGlobalTag (success) " << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: updateGlobalTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    Logger::setLogLevel(LogLevel::DEBUG);
    SURL = argv[1];
    print_path();

    createFile("/tmp/pyld_1.txt");
    createFile("/tmp/pyld_2.txt");

    //==============================================================
    // CLASSIC TESTS from crest_examples.cxx

    // TO RUN TEST UNCOMMENT IT!
    // (and check parameters!)

    // std::string tagname = "test_tag_01"; // write your tag name
    std::string tagname = "test_MvG01";
    std::string globaltagname = "MvG_TEST_01";

    // Global Tag Tests:
    testCreateGlobalTag(globaltagname);
    testFindGlobalTag(globaltagname);

    // Test: tag crestion, tag reading and removing.
    testCreateTag(tagname);
    testFindTag(tagname);
    testCreateTagMetaOldFormat(tagname);
    testFindTagMeta(tagname);

    // testCreateTagMetaOldFormat(tagname);

    testCreateGlobalTagMap(globaltagname, tagname);
    testListTagsInGlobalTag(globaltagname);

    testStorePayloads(tagname);
    testStorePayloadsFile(tagname);
    // Now list the iovs
    testListIovs(tagname);
    testGetPayload(tagname);
    testRemoveMap(globaltagname, tagname);
    testRemoveGlobalTag(globaltagname);
    testGetSize(tagname);
    testRemoveTag(tagname);

    // testFindExistingTagMeta("SCTDAQConfigChipSlim-HEAD");

    // testGetVersion();

    // Update Method tests:

    // update tag:
    /*
    std::string tag02 = "test_MvG02";
    testCreateTag(tag02);
    testUpdateTag(tag02);
    testCreateTagMeta(tag02);
    testUpdateTagMeta(tag02);
    testRemoveTag(tag02);
    */

    // update global tag:
    /*
    std::string globaltag02 = "MvG_TEST_02";
    testCreateGlobalTag(globaltag02);
    testUpdateGlobalTag(globaltag02);
    testRemoveGlobalTag(globaltag02);
    */
  } else {
    std::cout << "CREST Server path not found" << std::endl;
    std::cout << "Please, run this program with a server path:" << std::endl;
    std::cout
        << "crest_example_server https://atlas-crest-dev-mgr.cern.ch/api-v6.0"
        << std::endl;
  }
  std::cout << "Test ended" << std::endl;
  return 0;
}
