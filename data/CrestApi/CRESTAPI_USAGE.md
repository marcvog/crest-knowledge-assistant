# CREST API documentation

## General principles
The organization of CREST data can be sketched in few principles. 
1. `TAG` : a tag is a virtual container of metadata (the `IOVs`), and is defined via the following properties
    * `name` : a string identifying the tag in a unique way
    * `description` : a general description of the tag
    * `timeType` :  the time type of the tag [time, run-lumi,...]
    * `payloadSpec` : the type of the object that the tag is associated with (a CrestContainer can generate either `crest-json-single-iov`, or `crest-json-multi-iov`)
    * `synchronization` : the synchronization of the tag, this is still to be defined, but could be `UPDX` for example (optional, can be taken from tag name).
    * `lastValidatedTime`:  The last validated time of the tag, in millisecon (optional, not used in Athena).
    * `endOfValidity` : the end of validity of the tag, in the same units as the "since" of the IOVs (see later).
1. `TAGMETA` : this object has been introduced mainly to provide a correct mapping with the existing `metadata` of `COOL`, in particular related to `channels` list and `folder` specifications. It is in one-to-one relationship with the `TAG`. It is essential to have these metadata in order to increase compatibility with COOL related code in Athena. We will provide more details below on its content. For Athena usage, it is mandatory to create a TAGMETA entry for each TAG that a user create. 

2. `IOV` : the metadata represeting the start of validity for a given payload. It is defined via the following properties
    * `tagName` : the name of the tag.
    * `since` : the since of the IOV.
    * `insertionTime` : the insertion time of the IOV. Provided by the server.
    * `payloadHash` : the unique key for the payload. It is computed as the SHA256 hash of the payload by the server.
    In CREST the concept of IOV is always *open-ended*, in the sense that it is valid until the next since. The only IOV for which we foresee a possible *end time* is the last IOV; when the *end time* is not INFINITY, then it can be provided during the PAYLOAD upload. It will update the *endOfValidity* field in the TAG table. Any search functionality will in any case only use the *since* field in CREST, and eventually the *insertionTime*. The latter is used to *go back* into the history of a given *since*, in the case that it has been overridden by the expert with another PAYLOAD.
3. `PAYLOAD` : it is the binary object containing the conditions data payload. In our default serialization this is a JSON file, created with the help of `CrestContainer`. This allows to transparently migrate existing COOL data in a format that can then be used to generate a generic `CoralAttributeList` container on the Athena side. In terms of server implementation, the CREST server itself and its underlying relational DB tables are completely agnostic as far as the serialization choice of the PAYLOAD is concerned. For obvious reason, that is not at all the case on the client side, so it is important to understand the implication of the chosen serialization, and eventually adopt a strategy which is optimal for the given payload type.

The CREST API can then be seen as a *KEY=VALUE* store for PAYLOAD files. The TAG identifies a *payload type* (a combination of *folder* and *tag* in COOL), and every TAG will contain the *time history* of the PAYLOAD. When looking at the IOV in a tag, we get a unique *KEY* (the `payloadHash`) corresponding to a given time. Using that *KEY* we can download the corresponding PAYLOAD in a reproducible way, satisfying the basic principle of a REST interface. 

## Workflow
An expert having a specific payload to upload over time (a sort of conditions type) should proceed with the following steps in order to upload to CREST.
1. Create a `TAG` if it does not exists.
2. Upload `IOV` and `PAYLOAD` to an existing `TAG`.

Here a set of code snippets represting this workflow.

### Create a tag
```cpp
  CrestApi myCrestApi = CrestApi(SURL,false);
  std::string tagname = "TEST-USER-01";
  nlohmann::json js =
  {
    {"name", tagname},
    {"timeType", "time"},
    {"description", "test"},
    {"synchronization", "ALL"},
    {"insertionTime", "2018-12-18T11:32:58.081+0000"},
    {"modificationTime", "2018-12-18T11:32:57.952+0000"},
    {"payloadSpec", "JSON"}, // For athena reading you need to be careful to this field [crest-json-single-iov | crest-json-multi-iov]
    {"lastValidatedTime", 0.},
    {"endOfValidity", 0.},
  };


  TagDto dto = TagDto();
  dto = dto.fromJson(js);

  try {
    std::cout << std::endl << "test: createTag (input) = " << std::endl
              << dto.toJson().dump(4) << std::endl;
    myCrestApi.createTag(dto);
    std::cout << std::endl << "test: createTag (success) " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }

```
### Create a tag META information 
This step is extremely important for Athena. The META information on the `TAG` are a set of data which are present in `COOL` for the existing systems, and used often by `Athena` code. Here is an example on how to create `TAGMETA` information. A `COOL` expert can recognize below the information that was previously uploaded at `FOLDER` level in `COOL`, like payload specifications and node description. We included as well a tag description which contains (for automatically migrated tag) a sort of bookkeeping information of the original location in `COOL` for a given TAG.
```cpp

  CrestApi myCrestApi = CrestApi(SURL,false);

  // We create a TagInfo object 
  std::string description = "<timeStamp>time</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\"/></addrHeader><typeName>CondAttrListCollection</typeName>";
  TagInfoDto taginfo(description);

  // We create payload specifications (COOL format of "column name" and "column type")
  // To get a full list of Types you can look into the file CrestApi/CrestContainer.h
  PayloadSpecDto spec;
  spec.add("ModuleId","UInt32");
  spec.add("ErrorCode","UInt32");
  spec.add("json","Blob64k");
  // Add the specification to the taginfo object.
  taginfo.setPayloadSpec(spec);

  // Create a channel set.
  ChannelSetDto chanset;
  // Each channel has an "ID" which is numerical and an optional "name". 
  // Both fields are represented as strings in our convention, but please be careful that the first is an INT type.
  chanset.add("583008256","");
  chanset.add("583204864","");

  // Add the channels set to the taginfo object
  taginfo.setChannel(chanset);

  // Create the TagMeta object, using a description, the associated tag name and the previously filled taginfo.
  // The description string is not relevant (even in Athena), but it is useful to have a full bookkeeping of the
  // original folder, schema and DB that was the source of those data (important when migrating from COOL)
  std::string tag_description = "{\"dbname\":\"OFLP200\",\"nodeFullpath\":\"/TRIGGER/L1Calo/V1/Calibration/JfexModuleSettings\",\"schemaName\":\"COOLOFL_TRIGGER\"}";
  TagMetaDto metainfo(tagname,tag_description,taginfo.toJson().dump());

  try
  {
    myCrestApi.createTagMeta(metainfo);
    std::cout << std::endl
              << "test: createTagMeta (success)" << std::endl;
  }
  catch (const std::exception &e)
  {
    std::cout << std::endl
              << "test: createTagMeta (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }

```

### Find a tag
Retrieve a tag from CREST via its full name. Other methods exist to search for a given pattern.

```cpp
  CrestApi myCrestApi = CrestApi(SURL,false);
  std::string tagname = "TEST-USER-01";

  try {
    TagDto dto = myCrestApi.findTag(tagname);
    std::cout << std::endl << "test: findTag (result) = " << std::endl
              << dto.toJson().dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: findTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }

```

### Upload data
We show below 2 examples, the first one is a sort of low level way to upload data, by directly creating the relevant JSON object that is in a good format for CREST upload. Here is an example:

```cpp
  CrestApi myCrestApi = CrestApi(SURL,false);
  std::string tagname = "TEST-USER-01";

  uint64_t endtime = 0;
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

  try {
    std::cout << "Storing " << storeSetDto.toJson()  << std::endl;
    myCrestApi.storeData(tagname, storeSetDto, "JSON", "test", "test", "1", endtime);    
    std::cout << std::endl << "test: testStorePayloads (success) " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: testStorePayloads (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }

```
The best way to upload a payload is in any case to use the helper class CrestContainer in order to create a `StoreSetDto` object. We show below an example, where we create 3 columns of different types, add a record to each column, then store in a channel: here we suppose that 2 channels are present (1 and 2) in our tag meta info. Nevertheless there is at the moment no enforcement of choerence between the channel information in this container and the one in TagMeta.  

```cpp
  CrestApi myCrestApi = CrestApi(SURL,false);
  std::string tagname = "TEST-USER-01";

  uint64_t endtime = 0;

  // Your JSON string
  CrestContainer container;
  container.addColumn("a",TypeId::UInt16);
  container.addColumn("b",TypeId::Bool);
  container.addColumn("c",TypeId::Float);

  // Add data to the container
  container.addRecord("a",1,1);
  container.addRecord("b",1,true);
  container.addRecord("c",1,1.0);
  // Put data in channel "1", this will flush the underlying container, and clean it.
  container.addData("1");

  // Add another "row"
  container.addRecord("a",1,2);
  container.addRecord("b",1,false);
  container.addRecord("c",1,5.9);
  // Put this new row in another channel
  container.addData("2");

  // Add the full set of channels to an IOV with since=9000
  container.addIov(9000);

  StoreSetDto storeSetDto = StoreSetDto::fromJson(container.getStoreSetDto());

  try {
    std::cout << "Storing " << storeSetDto.toJson()  << std::endl;
    myCrestApi.storeData(tagname, storeSetDto, "JSON", "test", "test", "1", endtime);    
    std::cout << std::endl << "test: testStorePayloadsCrestContainer (success) " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: testStorePayloadsCrestContainer (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }

```

## The GLOBAL_TAG, a container of TAGs
When dealing with large detector systems like ATLAS, where experts use a wide range of different PAYLAODs to provide the needed information for reconstruction and simulation workflows, it can be cumbersome to bookkeep which TAG to use in a consistent way for a version of Athena (or other software). 
In order to guarantee a precise follow up of the TAG usage during time, CREST provides the concept of *GLOBAL_TAG*, which is essentially a container of *TAG*s. 

1. `GLOBAL_TAG` : a tag is a virtual container of metadata (the `IOVs`), and is defined via the following properties
    * `name` : a string identifying the tag in a unique way
    * `description` : a general description of the tag
    * `validity` : an int flag (TBD)
    * `release` : a string identifying some correspondance to a release (TBD)
    * `insertionTime` : the insertion time of the global tag, provided by the server.
    * `snapshotTime` (optional) : the moment at which we LOCK the global tag.
    * `scenario` : which scenario is this global tag ment to be used for (TBD: could be BLK, MC, ...).
    * `workflow` : which workflow is the global tag used for (TBD: could be UPDx).
    * `type` :  a char indicating the global tag type (TBD: could be T for test, L for Locked...).
    * `snapshotTimeMilli` : the snapshot time in milliseconds.
    * `insertionTimeMilli` : the insertion time in milliseconds.
2. `GLOBAL_TAG_MAP`: this object represent the association between a GLOBAL_TAG and its child TAGs.

The usage of these metadata is delegated to *conditions data coordination* in ATLAS (or *global tag coordination*). In agreement with *DATA PREPARATION* team, the *condition coordination* will gather information from system experts about which TAG to use for a given GLOBAL_TAG. This means that the general user (system expert) of CREST will rarely have to deal with these metadata, but for sure his help is essentially to identify if the bookkeeping is correct.