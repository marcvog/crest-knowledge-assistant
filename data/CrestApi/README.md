# CREST Client C++ Library.
This library implements the client API to interact with the CREST Server.
The CREST server API v6 is deployed in :
* https://atlas-crest-dev-mgr.cern.ch/api-v6.0

## Project structure
CrestApi project structure:

| Directory  | Description                                                |
|------------|------------------------------------------------------------|
| CrestApi   | Contains the header files for the library (CrestApi.h).    |
| scripts    | Contains the bash scripts used to create objects in the CREST Server. |
| src        | Contains the library source code (CrestApi.cxx).           |
| test       | Contains the ATLAS test (test.cxx). *(To be written later)*|
| CMakeList.txt | CMake file for the package CrestApi.                     |

## Updating CrestApi Version in TDAQ Common
For detailed instructions on updating the CrestApi version in TDAQ Common, see [TDAQ_INSTRUCTIONS.md](TDAQ_INSTRUCTIONS.md).

## Build Release
This project uses `bump2version` to manage versioning across multiple files. The tool automatically updates version numbers in `.gitlab-ci.yml` and `CMakeLists.txt` and creates a commit with the changes.

### Prerequisites
Install bump2version (Python package):
```shell
pip install bump2version
```

### Version Bumping
The project follows semantic versioning (MAJOR.MINOR.PATCH). Use the following commands to bump the version:

* **Patch release** (bug fixes, no API changes):
```shell
bump2version patch
```

* **Minor release** (new features, backward compatible):
```shell
bump2version minor
```

* **Major release** (breaking changes):
```shell
bump2version major
```

### What Gets Updated
Running `bump2version` will automatically update the version in:
- `.gitlab-ci.yml` (VERSION variable)
- `CMakeLists.txt` (project VERSION)
- `.bumpversion.cfg` (current_version)

### Release Workflow
1. Ensure you're on the correct branch (typically `master` or a release branch)
2. Run the appropriate `bump2version` command
3. The tool will automatically commit the changes with message: "Bump version: X.Y.Z → X.Y.Z+1"
4. Push the commit: `git push origin <branch>`
5. When merging to master, create a tag manually if needed: `git tag v<version>`

**Note:** The current configuration has `tag = False` in `.bumpversion.cfg`, so tags are not created automatically. Tags should be created manually or during the CI/CD merge process.

## Installation
We provide this code as a standalone library.
It requires:
* the `nlhomann` JSON library (this is delivered within this package, be careful to the version used)
* the `CURL` library
* the `BOOST` library
* the `OpenSSL` library (used for hashing)

### Standalone (lxplus)
Clone the project locally and switch to the relevant branch you want to use.
```shell
git clone ssh://git@gitlab.cern.ch:7999/crest-db/CrestApi.git
```
The following instructions assume you are working from root of CrestApi project.

* Create a build directory
```shell
## create a local installation directory
mkdir $HOME/local_lib
mkdir build
```
* Cmake
Be careful to eventually remove your previous local installation if you were testing another library before.

```shell
cd ./build
cmake -DCMAKE_INSTALL_PREFIX=$HOME/local_lib ..
```
* Compile library and examples
```shell
make
make install
```

## Authorization
Currently, the API supports two types of authorization workflows:
* `Client credentials`: the client is authenticated using a client id and secret.
```shell
export CREST_CLIENT_ID=<client id>
export CREST_CLIENT_SECRET=<secret>
export CREST_AUTH_MODE=CLIENT_CREDENTIALS
```
* `Token exchange`: the client is authenticated using an exchange token. To obtain the token excute `auth-get-user-token`.
```shell
auth-get-user-token -c crest-client -o ~/.client-exchange-token.json
export CREST_AUTH_MODE=JWT
```
Make sure to use `https` in `export CREST_API_URL=https://...`

## Testing
The simple examples are in the `doc` and `test` directory. The executables will be created in the `build` directory.

* `crest_example_fs` : this file can be used for the file system backend.

* `crestapi-test`: it contains a set of tests using BOOST.

You can validate your installation for example using:
```shell
./crest_example_server https://atlas-crest-dev-mgr.cern.ch/api-v6.0
```
