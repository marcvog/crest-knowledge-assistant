# Copyright (C) 2002-2025 CERN for the benefit of the ATLAS collaboration
# Set the name of the package.
cmake_minimum_required(VERSION 3.12)
project(CrestResolver VERSION 6.1 LANGUAGES CXX)

# Find the necessary externals.
find_package(CrestApi REQUIRED)


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
set(CMAKE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/CrestResolver")

# Turn on all warnings with GCC and Clang.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

# Declare the header and source file names.
set(HEADERS
    "./CrestFileResolver.h"
)

set(SOURCES
    "./CrestFileResolver.cxx"
)

# Set up the build of the main library of the project.
add_library(CrestResolverLib ${SOURCES} ${HEADERS})
target_link_libraries(CrestResolverLib
    PUBLIC
        CURL::libcurl
        Boost::boost
        nlohmann_json::nlohmann_json
        CrestApi::CrestApiLib
    PRIVATE
        OpenSSL::SSL
)
target_include_directories(CrestResolverLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

