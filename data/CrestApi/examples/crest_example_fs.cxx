/*!
   \file
   \brief Main file

   This file contains the examples for the CREST C++ Client Library for file
   storage. Main part of the examples is commented. Please uncomment the code
   you need. Check and correct (if it is necessary) the CREST Server parameters
   in CrestApiFs instances.
 */

#include <CrestApi/CrestApiFs.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>

// #include "CxxUtils/checker_macros.h"
// ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

using namespace Crest;

using namespace std;

std::string crest_path = "/tmp/crest";

bool createDirTree(const std::string &full_path) {
  size_t pos = 0;
  bool ret_val = true;

  while (ret_val == true && pos != std::string::npos) {
    pos = full_path.find('/', pos + 1);
    ret_val = std::filesystem::create_directory(full_path.substr(0, pos));
  }

  return ret_val;
}

// File System Tests
void testCreateGlobalTag_FS(const std::string &tagname) {
  std::cout << std::endl << "test file system: createGlobalTag" << std::endl;
  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  /*

  nlohmann::json js = {
      {"name", tagname},
      {"validity", 0},
      {"description", "test"},
      {"release", "1"},
      {"insertionTime", "2018-12-18T11:32:58.081+0000"},
      {"snapshotTime", "2018-12-18T11:32:57.952+0000"},
      {"scenario", "test"},
      {"workflow", "M"},
      {"type", "N"},
  };

  GlobalTagDto dto = GlobalTagDto();
  dto = dto.fromJson(js);
  */

  // global tag innitialization with the constructor:

  GlobalTagDto dto(tagname, "test", "1", "M");
  dto.setValidity(0);
  dto.setType("N");
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
}

//===================================================
// Tag Method Tests for file storage
//===================================================

void testCreateTag_FS(const std::string &tagname) {
  std::cout << std::endl << "test: createTag FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  /*
  // OLD VERSION,
  // the tag innitialized with JSON

  nlohmann::json js = {{"description", "none"},
                       {"endOfValidity", 0},
                       {"insertionTime", "2018-12-06T11:18:35.641+0000"},
                       {"lastValidatedTime", 0},
                       {"modificationTime", "2018-12-06T11:18:35.641+0000"},
                       {"name", tagname},
                       {"payloadSpec", "Json2Cool"},
                       {"synchronization", "ALL"},
                       {"status", "UNLOCKED"},
                       {"timeType", "time"}};

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
    myCrestApi.createTag(dto);
    std::cout << std::endl << "test: createTag FS (success)" << std::endl;
  } catch (const CrestException &e) {
    std::cout << std::endl << "test: createTag FS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: createTag FS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testUpdateTag(const std::string &tagname) {
  std::cout << std::endl << "test: updateTag FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  /*
  // OLD VERSION,
  // the tag innitialized with JSON

  nlohmann::json js = {{"description", "none"},
                       {"endOfValidity", 0},
                       {"insertionTime", "2018-12-06T11:18:35.641+0000"},
                       {"lastValidatedTime", 0},
                       {"modificationTime", "2018-12-06T11:18:35.641+0000"},
                       {"name", tagname},
                       {"payloadSpec", "Json2Cool"},
                       {"synchronization", "ALL"},
                       {"status", "UNLOCKED"},
                       {"timeType", "time"}};

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

  dto.setStatus("LOCKED");
  dto.setDescription("normal");
  try {
    // myCrestApi.updateTag(dto);
    std::cout << std::endl << "test: updateTag FS (success)" << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: updateTag FS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testFindTag_FS(const std::string &tagname) {
  std::cout << std::endl << "test: findTag FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  try {
    TagDto dto = myCrestApi.findTag(tagname);
    std::cout << std::endl
              << "test: findTag FS (result) =" << std::endl
              << dto.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: findTag FS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

//===================================================
// Tag Meta Info Method Tests for file storage
//===================================================

void testCreateTagMeta_FS(const std::string &tagname) {
  std::cout << std::endl << "test: createTagMeta FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

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
    std::cout << std::endl << "test: createTagMeta FS (success)" << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: createTagMeta FS (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testFindTagMeta_FS(const std::string &tagname) {
  std::cout << std::endl << "test: getTagMeta FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  try {
    TagMetaDto dto = myCrestApi.findTagMeta(tagname);
    std::cout << std::endl
              << "test: findTagMeta FS (result) =" << std::endl
              << dto.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: findTagMeta FS (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

// Global Tag Map tests:

void testCreateGlobalTagMap_FS(const std::string &globaltag,
                               const std::string &tagname) {
  std::cout << std::endl << "test: createGlobalTagMap FS" << std::endl;
  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  /*
  // OLD VERSION,
  // the global tag map innitialized with JSON:

  nlohmann::json js = {{"globalTagName", globaltag},
                       {"record", "testing2"},
                       {"label", "/MYFOLDER"},
                       {"tagName", tagname}};

  GlobalTagMapDto globalTagMap = GlobalTagMapDto();
  globalTagMap = globalTagMap.fromJson(js);
  */

  // the global tag map innitialized with constructor:

  GlobalTagMapDto globalTagMap(tagname, globaltag, "testing2", "/MYFOLDER");

  try {
    std::cout << std::endl
              << "test: createGlobalTagMap FS (input) = " << std::endl
              << globalTagMap.toJson().dump(4) << std::endl;
    myCrestApi.createGlobalTagMap(globalTagMap);
    std::cout << std::endl
              << "test: createGlobalTagMap FS (success) " << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl
              << "test: createGlobalTagMap FS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

// Test for the storePayloads method. The payloads are calculated each time.

void testStorePayloads_FS(const std::string &tagname) {
  std::cout << std::endl << "test: testStorePayloads FS" << std::endl;
  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  /*
  // OLD VERSION,
  // the tag innitialized with JSON

  nlohmann::json tag_js = {{"description", "none"},
                           {"endOfValidity", 0},
                           {"insertionTime", "2018-12-06T11:18:35.641+0000"},
                           {"lastValidatedTime", 0},
                           {"modificationTime", "2018-12-06T11:18:35.641+0000"},
                           {"name", tagname},
                           {"payloadSpec", "Json2Cool"},
                           {"synchronization", "none"},
                           {"timeType", "time"}};

  TagDto dto = TagDto();
  dto = dto.fromJson(tag_js);
  */

  // the tag innitialization with the constructor:

  TagDto dto(tagname, "time", "test");
  dto.setSynchronization("ALL");
  dto.setObjectType("JSON");
  dto.setStatus("UNLOCKED");
  dto.setLastValidatedTime(0.0);
  dto.setEndOfValidity(0.0);

  try {
    myCrestApi.createTag(dto);
    std::cout << std::endl
              << "test: tag " << tagname << " was created for test"
              << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl
              << "test: cannot create tag " << tagname << std::endl;
    std::cout << e.what() << std::endl;
  }
  //

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
            "data": "Sample data 2",
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

  StoreDto dto2(2000, "", "Sample data 2");
  dto1.setStreamerInfo("Info456");
  storeSetDto.push_back(dto2);

  try {
    std::cout << "Storing " << storeSetDto.toJson().dump(4) << std::endl;
    myCrestApi.storeData(tagname, storeSetDto, "JSON", "test", "test", "1",
                         endtime);
    std::cout << std::endl
              << "test: testStorePayloads FS (success) " << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl
              << "test: testStorePayloads FS (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }

  /*

  // Your JSON string
  std::string jsonString2 = R"(
      {
        "size": 2,
        "datatype": "data",
        "format": "StoreSetDto",
        "resources": [
          {
            "since": 3000,
            "data": "Sample data 3",
            "streamerInfo": "Info123"
          },
          {
            "since": 4000,
            "data": "Sample data 3",
            "streamerInfo": "Info456"
          }
        ]
      }
  )";

  nlohmann::json js2 = json::parse(jsonString2);
  StoreSetDto storeSetDto2 = StoreSetDto::fromJson(js2);
  */

  // New version:

  StoreSetDto storeSetDto2;

  StoreDto dto3(3000, "", "Sample data 3");
  dto3.setStreamerInfo("Info123");
  storeSetDto2.push_back(dto3);

  StoreDto dto4(4000, "", "Sample data 3");
  dto4.setStreamerInfo("Info456");
  storeSetDto2.push_back(dto4);

  try {
    std::cout << "Storing " << storeSetDto2.toJson().dump(4) << std::endl;
    myCrestApi.storeData(tagname, storeSetDto2, "JSON", "test", "test", "1",
                         endtime);
    std::cout << std::endl
              << "test: testStorePayloads FS (success) " << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl
              << "test: testStorePayloads FS (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testGetSize_FS(const std::string &tagname) {
  std::cout << std::endl << "test: getSize" << std::endl;
  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  try {
    int info = myCrestApi.getSize(tagname);
    std::cout << std::endl << "test: getSize (result) = " << info << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: getSize (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

// New:

int createFile(std::string filePath) {
  // Seed the random number generator with the current time
  sleep(1);
  // std::cout << "current time = "  << static_cast<unsigned>(std::time(0)) <<
  // std::endl; std::cout << "current time = "  << std::time(0) << std::endl;
  // std::cout << "current time = "  << std::time(nullptr) << std::endl;

  std::srand(static_cast<unsigned>(std::time(0)));

  // Open the file for writing
  std::ofstream outFile(filePath);

  // Check if the file is successfully opened
  if (!outFile.is_open()) {
    std::cerr << "Error opening file for writing." << std::endl;
    return 1;  // Return an error code
  }

  // Generate and write some random content to the file
  for (int i = 0; i < 10; ++i) {
    int randomValue =
        std::rand() % 100;  // Generate a random number between 0 and 99
    outFile << "Random value " << i + 1 << ": " << randomValue << std::endl;
  }

  // Close the file
  outFile.close();

  std::cout << "File created successfully at: " << filePath << std::endl;
  return 0;
}

void testStorePayloadsFile_FS(const std::string &tagname) {
  std::cout << std::endl << "test: testStorePayloadsFile_FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  createFile("/tmp/pyld_1.txt");
  createFile("/tmp/pyld_2.txt");

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
    std::cout << "Storing " << std::endl
              << storeSetDto.toJson().dump(4) << std::endl;
    myCrestApi.storeData(tagname, storeSetDto, "FILE", "test", "test", "1",
                         endtime);
    std::cout << std::endl
              << "test: testStorePayloadsFile_FS (success) " << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl
              << "test: testStorePayloadsFile_FS (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }

  createFile("/tmp/pyld_3.txt");
  createFile("/tmp/pyld_4.txt");

  // Your JSON string
  std::string jsonString2 = R"(
      {
        "size": 2,
        "datatype": "data",
        "format": "StoreSetDto",
        "resources": [
          {
            "since": 7000,
            "data": "file:///tmp/pyld_3.txt",
            "streamerInfo": "Info123"
          },
          {
            "since": 8000,
            "data": "file:///tmp/pyld_4.txt",
            "streamerInfo": "Info456"
          }
        ]
      }
  )";

  nlohmann::json js2 = json::parse(jsonString2);
  StoreSetDto storeSetDto2 = StoreSetDto::fromJson(js2);

  try {
    std::cout << "Storing " << std::endl
              << storeSetDto2.toJson().dump(4) << std::endl;
    myCrestApi.storeData(tagname, storeSetDto2, "FILE", "test", "test", "1",
                         endtime);
    std::cout << std::endl
              << "test: testStorePayloadsFile_FS (success) " << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl
              << "test: testStorePayloadsFile_FS (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testGetPayload_FS(const std::string &hash) {
  std::cout << std::endl << "test: getPayload FS" << std::endl;
  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  try {
    std::string payload = myCrestApi.getPayload(hash);
    std::cout << std::endl
              << "test: getPayload FS(result) =" << std::endl
              << payload << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: getPayload FS(failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testGetPayload_FS_2(const std::string &tagname) {
  std::cout << std::endl << "test: getPayload 2" << std::endl;
  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

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
    std::cout << std::endl << "test: getPayload 2 (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testGetPayloadMeta_FS(const std::string &hash) {
  std::cout << std::endl << "test: getPayloadMeta FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  try {
    PayloadDto dto = myCrestApi.getPayloadMeta(hash);
    std::cout << std::endl
              << "test: getPayloadMeta FS (result) =" << std::endl
              << dto.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: getPayloadMeta FS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

//
/* test for private method getPage
void testGetPage_FS()
{
  std::cout << std::endl
            << "test: getPage FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  nlohmann::json js = nlohmann::json::array();
  for (int i = 0; i < 35; i++) {
    nlohmann::json elem =
      {
          {"description", "none"},
          {"timeType", i}
      };
    js.push_back(elem);
  }

  std::cout << "getPage(10,0) = " << std::endl
            << myCrestApi.getPage(js,10,0).dump(4) << std::endl;

  std::cout << "getPage(10,100) = " << std::endl
            << myCrestApi.getPage(js,10,100).dump(4) << std::endl;

  std::cout << "getPage(10,3) = " << std::endl
            << myCrestApi.getPage(js,10,3).dump(4) << std::endl;
}
*/

/*
// test for private method getVectorPage
void testGetVectorPage_FS()
{
  std::cout << std::endl
            << "test: getVectorPage FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  std::vector<std::string> res;
  for (int i = 0; i < 35; i++) {
    std::string elem = "data_" + std::to_string(i);
    res.push_back(elem);
  }

  std::cout << "original vector = " << std::endl;
  for (std::string i: res)
    std::cout << i << ' ';
  std::cout << std::endl;

  std::vector<std::string> result1 = myCrestApi.getVectorPage(res,10,0);
  std::cout << "getVectorPage(10,0) = " << std::endl;
  for (std::string i: result1)
    std::cout << i << ' ';
  std::cout << std::endl;

  std::vector<std::string> result2 = myCrestApi.getVectorPage(res,10,100);
  std::cout << "getVectorPage(10,100) = " << std::endl;
  for (std::string i: result2)
    std::cout << i << ' ';
  std::cout << std::endl;

  std::vector<std::string> result3 = myCrestApi.getVectorPage(res,10,3);
  std::cout << "getVectorPage(10,3) = " << std::endl;
  for (std::string i: result3)
    std::cout << i << ' ';
  std::cout << std::endl;

}
*/

void testFindGlobalTagMap_FS(const std::string &name) {
  std::cout << std::endl << "test: findGlobalTagMap FS" << std::endl;
  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  try {
    GlobalTagMapSetDto dto = myCrestApi.findGlobalTagMap(name, "Trace");
    std::cout << std::endl
              << "test: findGlobalTagMap FS (result) = " << std::endl
              << dto.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: findGlobalTagMap FS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testListIovs_FS(const std::string &tagname) {
  std::cout << std::endl << "test: listIovs FS" << std::endl;
  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  try {
    IovSetDto dto =
        myCrestApi.selectIovs(tagname, 0, -1, 0, 1000, 0, "id.since:ASC");
    std::cout << std::endl
              << "page size = 1000, page = 0, id.since:ASC" << std::endl;
    std::cout << std::endl
              << "test: listIovs FS (result) = " << std::endl
              << dto.toJson().dump(4) << std::endl;

    IovSetDto dto2 =
        myCrestApi.selectIovs(tagname, 0, -1, 0, 2, 1, "id.since:ASC");
    std::cout << std::endl
              << "page size = 2, page = 1, id.since:ASC" << std::endl;
    std::cout << std::endl
              << "test: listIovs FS (result) = " << std::endl
              << dto2.toJson().dump(4) << std::endl;

    IovSetDto dto3 =
        myCrestApi.selectIovs(tagname, 0, -1, 0, 1000, 0, "id.since:DESC");
    std::cout << std::endl
              << "page size = 1000, page = 0, id.since:DESC" << std::endl;
    std::cout << std::endl
              << "test: listIovs FS (result) = " << std::endl
              << dto3.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: listIovs FS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

/*
// test for private method nameList
void testNameList_FS() {
  std::cout << std::endl << "test: nameList FS" << std::endl;
  std::string workdir = "/tmp/crest";
  std::string s_FS_TAG_PATH = "/tags";

  CrestApiFs myCrestApi = CrestApiFs(true, workdir);

  std::string folder = workdir + s_FS_TAG_PATH;

  try {
    std::vector<std::string> taglist = myCrestApi.nameList(folder);

    std::cout << std::endl << "ascending:" << std::endl
        << "test: nameList FS (result):" << std::endl;
    for (std::string tag: taglist)
      std::cout << tag << ' ';
    std::cout << std::endl << "test: nameList FS (end)" << std::endl;

    taglist = myCrestApi.nameList(folder,false);

    std::cout << std::endl << "descending:" << std::endl
        << "test: nameList FS (result):" << std::endl;
    for (std::string tag: taglist)
      std::cout << tag << ' ';
    std::cout << std::endl << "test: nameList FS (end)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: nameList FS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}
*/

void testListTags_FS(const std::string &name, int size, int page) {
  std::cout << std::endl << "test: listTags FS" << std::endl;
  std::string workdir = "/tmp/crest";

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  try {
    TagSetDto dto = myCrestApi.listTags(name, size, page, "name:ASC");
    std::cout << std::endl
              << "test: listTags FS (result) = " << std::endl
              << dto.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: listTags FS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void createNTags_FS(const std::string &tagname, int n) {
  std::cout << std::endl << "createNTags FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  for (int i = 0; i < n; i++) {
    std::string tg = tagname + "__" + std::to_string(i);

    /*
    // OLD VERSION,
    // the tag innitialized with JSON

    nlohmann::json js = {{"description", "none"},
                         {"endOfValidity", 0},
                         {"insertionTime", "2018-12-06T11:18:35.641+0000"},
                         {"lastValidatedTime", 0},
                         {"modificationTime", "2018-12-06T11:18:35.641+0000"},
                         {"name", tg},
                         {"payloadSpec", "Json2Cool"},
                         {"synchronization", "ALL"},
                         {"status", "UNLOCKED"},
                         {"timeType", "time"}};

    TagDto dto = TagDto();
    dto = dto.fromJson(js);
    */

    // the tag innitialization with the constructor:

    TagDto dto(tg, "time", "test");
    dto.setSynchronization("ALL");
    dto.setObjectType("JSON");
    dto.setStatus("UNLOCKED");
    dto.setLastValidatedTime(0.0);
    dto.setEndOfValidity(0.0);

    try {
      myCrestApi.createTag(dto);
      std::cout << std::endl << "tag " << tg << " created" << std::endl;
    } catch (const std::exception &e) {
      std::cout << std::endl << "test: createTag FS (failed)" << std::endl;
      std::cout << e.what() << std::endl;
    }
  }  // i
}

/*
// test for private method
void testSortJson(const std::string & tagname, int n){
  std::cout << std::endl << "test: sortJson FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  nlohmann::json j_list = nlohmann::json::array();

  for (int i = 0; i < n; i++){
    int randNum = rand() % 100; // 0-99
    std::string tg = tagname + "__" +  std::to_string(randNum);
    nlohmann::json elem =
      {
        {"name", tg},
        {"validity", 0},
        {"description", "test"}
      };
    j_list.push_back(elem);
  }

  std::cout << std::endl << "direct order:" << std::endl;
  nlohmann::json sorted = myCrestApi.sortJson(j_list, "name", true);

  std::cout << "sorted = " << std::endl
            << sorted.dump(4);

  std::cout << std::endl << "reverse order:" << std::endl;
  sorted = myCrestApi.sortJson(j_list, "name", false);

  std::cout << "sorted = " << std::endl
            << sorted.dump(4);

  std::cout << std::endl
            << "test: sortJson FS (ended)" << std::endl;
}
*/

/*
// test for private method
void testSortIOVJson(const std::string & tagname, int n){
  std::cout << std::endl << "test: sortIOVJson FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  nlohmann::json j_list = nlohmann::json::array();

  for (int i = 0; i < n; i++){
    int randNum = rand() / 100; // test

    nlohmann::json elem =
      {
  {"insertionTime", "2024-02-28 17:08:26"},
  {"payloadHash",
"944aa370272a061513bc68d5f6b2d5f83664b8da8cc0def2fb87cf2cef34fc6f"},
  {"since", randNum},
  {"tagName", "test_MvG03"}
      };

    j_list.push_back(elem);
  }

  std::cout << "IOV list = " << std::endl
            << j_list.dump(4) << std::endl;

  std::cout << std::endl << "direct order:" << std::endl;
  nlohmann::json sorted = myCrestApi.sortIOVJson(j_list, true);

  std::cout << "sorted = " << std::endl
            << sorted.dump(4);

  std::cout << std::endl << "reverse order:" << std::endl;
  sorted = myCrestApi.sortIOVJson(j_list, false);

  std::cout << "sorted = " << std::endl
            << sorted.dump(4);

  std::cout << std::endl
            << "test: sortJson FS (ended)" << std::endl;
}
*/

/*
// test for private method
void testIsMatch(){
  std::cout << std::endl << "test: isMatch FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  std::cout << "string = " << "XYXZZXY" << "  mask = " << "XY%" << "  result = "
      << " result = " << myCrestApi.isMatch("XYXZZXY", "XY%") << std::endl; //
true std::cout << "string = " << "XYXZZXY" << "  mask = " << "%XZZ%"  << "
result = "
      << " result = " << myCrestApi.isMatch("XYXZZXY", "%XZZ%") << std::endl; //
true std::cout << "string = " << "XYXZZXY" << "  mask = " << "%YZZ%"  << "
result = "
      << myCrestApi.isMatch("XYXZZXY", "%YZZ%") << std::endl;      // false

  std::cout << std::endl
            << "test: isMatch FS (ended)" << std::endl;
}
*/

void createNGlobalTags_FS(const std::string &tagname, int n) {
  std::cout << std::endl << "createNGlobalTags FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  for (int i = 0; i < n; i++) {
    std::string tg = tagname + "__" + std::to_string(i);

    /*
    // OLD VERSION,
    // global tag innitialized with JSON:

    nlohmann::json js = {{"name", tg},
                         {"validity", 0},
                         {"description", "test"},
                         {"release", "1"},
                         {"insertionTime", "2018-12-18T11:32:58.081+0000"},
                         {"snapshotTime", "2018-12-18T11:32:57.952+0000"},
                         {"scenario", "test"},
                         {"workflow", "M"},
                         {"type", "t"}};

    GlobalTagDto dto = GlobalTagDto();
    dto = dto.fromJson(js);
    */

    // global tag innitialization with the constructor:

    GlobalTagDto dto(tg, "test", "1", "M");
    dto.setValidity(0);
    dto.setType("N");
    dto.setScenario("test");

    try {
      myCrestApi.createGlobalTag(dto);
      std::cout << std::endl << "global tag " << tg << " created" << std::endl;
    } catch (const std::exception &e) {
      std::cout << std::endl
                << "test: createNGlobalTags FS (failed)" << std::endl;
      std::cout << e.what() << std::endl;
    }
  }  // i
}

void testListGlobalTags_FS(const std::string &name, int size, int page) {
  std::cout << std::endl << "test: listTags FS" << std::endl;
  std::string workdir = "/tmp/crest";

  std::cout << "Parameters:" << std::endl;
  std::cout << "  name = " << name << std::endl;
  std::cout << "  size = " << size << std::endl;
  std::cout << "  page = " << page << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  try {
    GlobalTagSetDto dto =
        myCrestApi.listGlobalTags(name, size, page, "name:ASC");
    std::cout << std::endl
              << "test: listGlobalTags FS (result) = " << std::endl
              << dto.toJson().dump(4) << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: listGlobalTags FS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testGetVersion_FS() {
  std::cout << std::endl << "test: updateTag FS" << std::endl;
  // CrestApi myCrestApi = CrestApi(SURL, apipath, false);

  std::string workdir = "/tmp/crest";
  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

  try {
    std::string vers1 = myCrestApi.getClientVersion();
    std::cout << std::endl << "test: getClientVersion = " << vers1 << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: getClientVersion FS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try {
    std::string vers2 = myCrestApi.getCrestVersion();
    std::cout << std::endl << "test: getCrestVersion = " << vers2 << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: getCrestVersion FS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

// Update method tests:

void testUpdateTagMeta_FS(const std::string &tagname) {
  std::cout << std::endl << "test: updateTagMeta FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

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

    std::cout << std::endl << "test: updateTagMeta FS (success)" << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: updateTagMeta FS (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testUpdateTag_FS(const std::string &tagname) {
  std::cout << std::endl << "test: updateTag FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

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

    std::cout << std::endl << "test: updateTag FS (success) " << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: updateTag FS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testUpdateGlobalTag_FS(const std::string &tagname) {
  std::cout << std::endl << "test: updateGlobalTag FS" << std::endl;

  CrestApiFs myCrestApi = CrestApiFs(true, crest_path);

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

    std::cout << std::endl
              << "test: updateGlobalTag FS (success) " << std::endl;
  } catch (const std::exception &e) {
    std::cout << std::endl << "test: updateGlobalTag FS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

int main() {

  //==============================================================
  // CLASSIC TESTS from crest_examples.cxx

  // TO RUN TEST UNCOMMENT IT!
  // (and check parameters!)

  std::string tagname = "test_MvG03";
  std::string globaltag = "MvG_TEST_01";

  // clean working directory:
  try {
    std::filesystem::remove_all(crest_path);
  } catch (const std::exception &e) {
    // Ignore cleanup errors but log them
    std::cerr << "Warning: filesystem cleanup failed: " << e.what()
              << std::endl;
  }

  //
  // Global Tag Tests:
  testCreateGlobalTag_FS(globaltag);
  //

  //
  // tag method tests for file storage
  testCreateTag_FS(tagname);
  testCreateTagMeta_FS(tagname);
  testFindTagMeta_FS(tagname);
  testFindTag_FS(tagname);
  //

  /*
  testCreateGlobalTagMap_FS(globaltag,tagname);
  testFindGlobalTagMap_FS(globaltag);
  */

  /*
  // tag meta info method tests for file storage
  testCreateTag_FS(tagname);
  testFindTag_FS(tagname);
  testStorePayloads_FS(tagname);
  testGetSize_FS(tagname);
  testGetPayload_FS_2(tagname);
  */

  /*
  testCreateTag_FS(tagname);
  testStorePayloadsFile_FS(tagname);
  testGetPayload_FS("abdc424af89446400ba4b31ace71538b05eab779449f35cfa3e4a00d9bf53c1b");
  testGetPayloadMeta_FS("abdc424af89446400ba4b31ace71538b05eab779449f35cfa3e4a00d9bf53c1b");

  testListIovs_FS(tagname);
  */

  // testNameList_FS(); // private method

  /*
  // test for tag list:
  createNTags_FS("test_data",10);
  testListTags_FS("%",10000,0);
  testListTags_FS("test_MvG%",10000,0);
  testListTags_FS("%3",10000,0);
  testListTags_FS("",10000,0);
  testListTags_FS("",5,0);
  testListTags_FS("%",4,0);
  */

  /*
  // test for global tag list:
  createNGlobalTags_FS("test_data",10);
  testListGlobalTags_FS("%",10000,0);
  testListGlobalTags_FS("test_MvG%",10000,0);
  testListGlobalTags_FS("%3",10000,0);
  testListGlobalTags_FS("",10000,0);
  testListGlobalTags_FS("",5,0);
  testListGlobalTags_FS("%",4,0);
  */

  // testSortJson("test_only", 20);  // private method
  // testSortIOVJson("test_only", 20); // private method
  // testIsMatch(); // private method
  // testGetVectorPage_FS(); // private method

  // testGetVersion_FS();

  // Update Method tests:

  // update tag / update tag meta info:
  /*
  std::string tag02 = "test_MvG02";
  testCreateTag_FS(tag02);
  testUpdateTag_FS(tag02);
  testCreateTagMeta_FS(tag02);
  testUpdateTagMeta_FS(tag02);
  */

  // update global tag:
  /*
  std::string globaltag02 = "MvG_TEST_02";
  testCreateGlobalTag_FS(globaltag02);
  testUpdateGlobalTag_FS(globaltag02);
  */

  std::cout << "Test ended : check output in /tmp/crest " << std::endl;
  return 0;
}
