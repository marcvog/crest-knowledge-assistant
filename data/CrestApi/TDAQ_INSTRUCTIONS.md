# Instructions from Reiner Hauser

Here's some instructions on how to update the package that builds the CrestApi externals (as well as installs the python packages, plus CORAL and COOL).

### Links
* [TDAQCExternals Repo](https://gitlab.cern.ch/atlas-tdaq-software/TDAQCExternal)
* [TDAQ Common CI Dashboard](https://atlas-tdaq-cdash.web.cern.ch/index.php?project=tdaq-common)
* [TDAQ Common CMake Repo](https://gitlab.cern.ch/atlas-tdaq-software/tdaq-common-cmake)

### Prerequisites

* A configured Kerberos token (i.e. `kinit <user>@CERN.CH`)
* A user-level git config with http `emptyAuth` set to `true`

### Environment Setup (LXPLUS Assumed)

1. Similar to offline's `setupATLAS/asetup` you should define an alias to setup the TDAQ environment:

    ```bash
    alias cm_setup='source /cvmfs/[atlas.cern.ch/repo/sw/tdaq/tools/cmake_tdaq/bin/cm_setup.sh'](http://atlas.cern.ch/repo/sw/tdaq/tools/cmake_tdaq/bin/cm_setup.sh')
    ```

    You run this to setup your environment for working against an available release, in most cases you will use `tdaq-common-nightly`

    ```bash
    cm_setup tdaq-common-nightly
    ```

2. Create a work area and copy our standard top-level `CMakeLists.txt` file there:

    ```bash
    mkdir work
    cd work
    cp /cvmfs/atlas.cern.ch/repo/sw/tdaq/tools/cmake_tdaq/cmake/templates/CMakeLists.txt .
    ```

3. Now checkout the TDAQCExternal package. It is responsible for building and installing all external software for `tdaq-common`

    ```bash
    getpkg TDAQCExternal
    ```

    This command is basically a `git clone .../TDAQCRelease.git` replacement that saves you typing the whole URL.

4. The TDAQCExternal repo will be checked out by default with an HTTPS URL. In order to push later and run the final `tagpkg` command we need to ensure the following git config parameter is set:

    ```bash
    git config --global http.emptyAuth true
    ```

5. Work on a new branch:

    ```bash
    # Inside TDAQCExternal
    git checkout -b <branch> origin/master
    ```

6. Now you can build it:

    ```bash
    # Back in the working directory
    cmake -B build
    cd build
    make -j $(nproc)
    make install
    ```


### Updating the CrestApi Version and Creating TDAQ MR

1. To change the CrestApi version, modify the `CMakeLists.txt` file and change the version in `GIT_TAG` for the `ExternalProject_add()` call for `CrestApi`.
2. To change the version of a Python-only package (e.g. pycrest), edit `constraints.txt` and update the version numbers there. There is no need to modify the CMakeLists.txt file


    Make sure it compiles and installs, obviously. After installation you can source `../installed/setup.sh` to pick up your latest version.


3. Now you can commit and push your changes:

    ```bash
    git add -p
    git commit -v
    git tag ... [ optional ]
    git push
    ```

4. Submit an MR to TDAQCExternal to merge your branch into `master` (assign to Reiner). **Once MR has been accepted proceed to next step.** With maintainer rights, you can also make changes directly on master and push those.

5.  **Once the MR above has been accepted and merged, you can proceed.** If you have 'developer' rights (or above) on the `tdaq-common` project itself this allows you to update the TDAQCExternal package version in the release. You can do this as follows (use `master` if you did not create a tag and your latest changes to TDAQCExternal are on `master`):

    ```bash
    # From the work directory, above TDAQCExternal
    tagpkg TDAQCExternal tdaq-common-nightly <tag>    # if you tagged your version
    tagpkg TDAQCExternal tdaq-common-nightly master   # if you mean the latest commit on the master branch
    ```


Both are ultimately resolved into a commit hash, so it's a matter of preference what you use. (Just make sure you have pushed your changes to gitlab if you use a branch name).

This command will automatically trigger a merge request on and full rebuild of `tdaq-common`, and if the build succeeds the MR will be accepted and merged.

You can track the status of the build on the [TDAQ Common CI Dashboard](https://atlas-tdaq-cdash.web.cern.ch/index.php?project=tdaq-common), and the resulting MR will show up in the [TDAQ Common CMake repo](https://gitlab.cern.ch/atlas-tdaq-software/tdaq-common-cmake/-/merge_requests).

The changes will then show up in the next nightly (typically the next day).
