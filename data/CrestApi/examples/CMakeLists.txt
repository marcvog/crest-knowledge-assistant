# Copyright (C) 2002-2025 CERN for the benefit of the ATLAS collaboration

# Helper function for setting up executables for the project.
function(crestapi_add_executable name)
    add_executable("crest_${name}" "crest_${name}.cxx")
    target_link_libraries("crest_${name}"
        PRIVATE
            nlohmann_json::nlohmann_json
            CrestApiLib
    )
endfunction()

# Declare the executables.
#crestapi_add_executable(condcontainer_example)
crestapi_add_executable(example_fs)
crestapi_add_executable(example_server)
crestapi_add_executable(example_server_getdata)
crestapi_add_executable(functions_example)

function(crestcontainer_add_executable name)
    add_executable("crest_${name}" "crest_${name}.cxx")
    target_link_libraries("crest_${name}"
        PRIVATE
            nlohmann_json::nlohmann_json
            CrestApiLib
            CrestContainerLib
    )
endfunction()
crestcontainer_add_executable(condcontainer_example)

