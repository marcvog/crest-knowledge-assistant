# Copyright (C) 2002-2025 CERN for the benefit of the ATLAS collaboration
# Set the name of the package.
cmake_minimum_required(VERSION 3.12)
project(CrestContainer VERSION 6.1 LANGUAGES CXX)

# Find the necessary externals.
#find_package(CrestApi 6.1 REQUIRED)


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
set(CMAKE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/CrestContainer")

# Turn on all warnings with GCC and Clang.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

# Declare the header and source file names.
set(HEADERS
    "./CrestContainer.h"
)

set(SOURCES
    "./CrestContainer.cxx"
)
# Use RPATH for build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)
# Don't use RPATH for install tree when building
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
# Add the automatically determined parts of the RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Set up the build of the main library of the project.
add_library(CrestContainerLib ${SOURCES} ${HEADERS})
target_link_libraries(CrestContainerLib
    PUBLIC
        CURL::libcurl
        Boost::boost
        nlohmann_json::nlohmann_json
	CrestApiLib
    PRIVATE
        OpenSSL::SSL
)
target_include_directories(CrestContainerLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(CrestContainerLib PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
    VERSION "${PROJECT_VERSION}"
)

# Install the library.
install(TARGETS CrestContainerLib
    EXPORT CrestContainerTargets
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/CrestContainer"
)

# Export the project.
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CrestContainerConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CrestContainerConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CrestContainerConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_CMAKEDIR}"
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CrestContainerConfigVersion.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CrestContainerConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_CMAKEDIR}"
)
install(EXPORT CrestContainerTargets
    NAMESPACE CrestApi::
    DESTINATION "${CMAKE_INSTALL_CMAKEDIR}"
)
