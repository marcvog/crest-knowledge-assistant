# Copyright (C) 2002-2025 CERN for the benefit of the ATLAS collaboration

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)


# Enable testing
enable_testing()
include(GoogleTest)

# Helper function for setting up tests for the project.
function(crestapi_add_test TESTNAME)
    add_executable(${TESTNAME} "${TESTNAME}.cxx")
    target_link_libraries(${TESTNAME}
        PRIVATE
	        GTest::GTest
            nlohmann_json::nlohmann_json
            GTest::gtest_main
            GTest::gmock
            CrestApiLib
    )
    add_test(
        NAME ${TESTNAME}
        COMMAND ${TESTNAME}
    )
    gtest_discover_tests(${TESTNAME})
endfunction()


# Declare the tests.
crestapi_add_test(CrestApi_test)
crestapi_add_test(TagDto_test)
crestapi_add_test(IovDto_test)
crestapi_add_test(GlobalTagDto_test)
crestapi_add_test(GlobalTagMapDto_test)
crestapi_add_test(TagMetaDto_test)
crestapi_add_test(StoreDto_test)
crestapi_add_test(RespPage_test)
crestapi_add_test(PayloadSpecDto_test)
crestapi_add_test(ChannelSetDto_test)
crestapi_add_test(TagInfoDto_test)
crestapi_add_test(PayloadTagInfoDto_test)
crestapi_add_test(PayloadDto_test)
crestapi_add_test(RunLumiDto_test)
crestapi_add_test(StringUtils_test)
## crestapi_add_test(json_parse)
## crestapi_add_test(test-json)
## crestapi_add_test(CrestFileResolver_test)  
