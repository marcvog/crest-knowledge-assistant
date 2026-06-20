# ATHENA INSTRUCTIONS
This documentation provides information on how to link against CrestApi from `tdaq-common` for local developments.

## TDAQ
Build the tdaq-common nightly release (branch tdaq-common-99-00-00)

```bash
git clone https://gitlab.cern.ch/atlas-tdaq-software/tdaq-common-cmake.git
pushd tdaq-common-cmake
git submodule update -i TDAQCExternal
cd TDAQCExternal
git checkout master
popd
mkdir build
cd build
source /cvmfs/atlas.cern.ch/repo/sw/tdaq/tools/cmake_tdaq/bin/setup.sh  x86_64-el9-gcc14-opt
cmake ../tdaq-common-cmake
make -j $(nproc)
make install
```
If you simply want to setup an environment from an official TDAQ release you can instead do the following (you may need to change some arguments):
```bash
source /cvmfs/atlas.cern.ch/repo/sw/tdaq/tools/cmake_tdaq/bin/setup.sh x86_64-el9-gcc14-opt
alias cm_setup='source /cvmfs/atlas.cern.ch/repo/sw/tdaq/tools/cmake_tdaq/bin/cm_setup.sh'
cm_setup tdaq-12-03-00
```

### You may need to change the CrestApi tag specified in CMakeLists.txt of TDAQCExternal

## Build your package
### If you are getting CORAL, COOL and CrestApi from tdaq-common, follow these steps:

1) **Add the following lines to the package `CMakeLists.txt` file:**

```cmake
# External dependencies:
find_package(COOL COMPONENTS CoolKernel CoolApplication)
find_package(CORAL COMPONENTS CoralBase RelationalAccess)
find_package(CrestApi COMPONENTS CrestApi)

target_link_libraries(MyPkgLib
  PUBLIC
    ${CORAL_LIBRARIES}
    ${COOL_LIBRARIES}
    CrestApi::CrestApiLib
)

target_include_directories(MyPkgLib
  PUBLIC
    ${CORAL_INCLUDE_DIRS}
    ${COOL_INCLUDE_DIRS}
    ${CrestApi_INCLUDE_DIRS}
)
```

2) **Set the following variables before building:**

```bash
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:/path-to/tdaq-common-cmake/installed/external/x86_64-el9-gcc14-opt
export LD_LIBRARY_PATH=/path-to/tdaq-common-cmake/installed/external/x86_64-el9-gcc14-opt/lib:$LD_LIBRARY_PATH
```

