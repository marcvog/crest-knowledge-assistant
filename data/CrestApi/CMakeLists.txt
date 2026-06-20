
# Copyright (C) 2002-2025 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
cmake_minimum_required(VERSION 3.12)
project(CrestApi VERSION 6.2.6 LANGUAGES CXX)

# Export compile commands for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find the necessary externals.
find_package(CURL REQUIRED)
find_package(Boost REQUIRED COMPONENTS unit_test_framework)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)

# Set the C++ standard to use.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL
    "Whether to enable compiler-specific C++ extensions")

# Set the default library type to build.
option(BUILD_SHARED_LIBS
    "Flag for building shared/static libraries" TRUE)

# Use the standard GNU installation directories.
include(GNUInstallDirs)
set(CMAKE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/CrestApi")

# Create version header file (version.h)
set(CRESTAPI_VERSION_FILE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CrestApi")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CrestApi/version.h.in"
    "${CRESTAPI_VERSION_FILE_DIR}/version.h")

# Turn on all warnings with GCC and Clang.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

# Declare the header and source file names.
set(HEADERS
    "CrestApi/CrestApi.h"
    "CrestApi/CrestApiBase.h"
    "CrestApi/CrestApiFs.h"
    "CrestApi/CrestLogger.h"
    "CrestApi/CrestRequest.h"
    "CrestApi/CrestAuth.h"
    "CrestApi/JwtUtils.h"
    "CrestApi/CrestApiExt.h"
    "CrestApi/CrestException.h"
    "CrestApi/CrestCondException.h"
    "CrestApi/CrestDetail.h"
    "CrestApi/StringUtils.h"
    "CrestApi/GenericMap.h"
    "CrestApi/ChannelSetDto.h"
    "CrestApi/CrestBaseResponse.h"
    "CrestApi/GlobalTagDto.h"
    "CrestApi/GlobalTagMapDto.h"
    "CrestApi/GlobalTagMapSetDto.h"
    "CrestApi/GlobalTagSetDto.h"
    "CrestApi/IovDto.h"
    "CrestApi/IovSetDto.h"
    "CrestApi/PayloadDto.h"
    "CrestApi/PayloadSetDto.h"
    "CrestApi/PayloadSpecDto.h"
    "CrestApi/PayloadTagInfoDto.h"
    "CrestApi/PayloadTagInfoSetDto.h"
    "CrestApi/RespPage.h"
    "CrestApi/StoreDto.h"
    "CrestApi/StoreSetDto.h"
    "CrestApi/TagDto.h"
    "CrestApi/TagInfoDto.h"
    "CrestApi/TagMetaDto.h"
    "CrestApi/TagMetaSetDto.h"
    "CrestApi/TagSetDto.h"
    "CrestApi/RunLumiDto.h"
    "CrestApi/RunLumiSetDto.h"
    "${CRESTAPI_VERSION_FILE_DIR}/version.h")

set(SOURCES
    "src/CrestApi.cxx"
    "src/CrestRequest.cxx"
    "src/CrestAuth.cxx"
    "src/JwtUtils.cxx"
    "src/CrestLogger.cxx"
    "src/StringUtils.cxx"
    "src/CrestApiFs.cxx"
    "src/CrestApiBase.cxx"
    "src/ChannelSetDto.cxx"
    "src/GenericMap.cxx"
    "src/GlobalTagMapSetDto.cxx"
    "src/IovSetDto.cxx"
    "src/PayloadSpecDto.cxx"
    "src/RespPage.cxx"
    "src/TagMetaDto.cxx"
    "src/CrestApiBase.cxx"
    "src/CrestBaseResponse.cxx"
    "src/GlobalTagDto.cxx"
    "src/GlobalTagSetDto.cxx"
    "src/PayloadDto.cxx"
    "src/PayloadTagInfoDto.cxx"
    "src/StoreDto.cxx"
    "src/TagDto.cxx"
    "src/TagMetaSetDto.cxx"
    "src/GlobalTagMapDto.cxx"
    "src/IovDto.cxx"
    "src/PayloadSetDto.cxx"
    "src/PayloadTagInfoSetDto.cxx"
    "src/StoreSetDto.cxx"
    "src/TagInfoDto.cxx"
    "src/TagSetDto.cxx"
    "src/RunLumiDto.cxx"
    "src/RunLumiSetDto.cxx"
)

# Set up the build of the main library of the project.
add_library(CrestApiLib ${SOURCES} ${HEADERS})
target_link_libraries(CrestApiLib
    PUBLIC
        CURL::libcurl
        Boost::boost
        nlohmann_json::nlohmann_json
    PRIVATE
        OpenSSL::SSL
)
target_include_directories(CrestApiLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CRESTAPI_VERSION_FILE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Remove the unwanted header from the list
list(REMOVE_ITEM HEADERS "${CRESTAPI_VERSION_FILE_DIR}/version.h")

set_target_properties(CrestApiLib PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
    VERSION "${PROJECT_VERSION}"
)

# Install the library.
install(TARGETS CrestApiLib
    EXPORT CrestApiTargets
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/CrestApi"
)

# Set up the tests.
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()

# Set up the examples.
option(CRESTAPI_BUILD_EXAMPLES
    "Flag for building the examples" TRUE)
if(CRESTAPI_BUILD_EXAMPLES)
    add_subdirectory(examples)
##    add_subdirectory(acts)
    add_subdirectory(tools)
endif()

# Export the project.
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CrestApiConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CrestApiConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CrestApiConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_CMAKEDIR}"
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CrestApiConfigVersion.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CrestApiConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_CMAKEDIR}"
)
install(EXPORT CrestApiTargets
    NAMESPACE CrestApi::
    DESTINATION "${CMAKE_INSTALL_CMAKEDIR}"
)
