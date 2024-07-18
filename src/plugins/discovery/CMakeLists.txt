project(discovery_service_kit)
message (STATUS "Project name = ${CMAKE_PROJECT_NAME}")

# The library name for the bundle
if(WIN32)
set(_os "Windows")
else() 
set(_os "Linux")
endif()

set(MQTT_C 0)
set(UTILITY_C 0)
set(ARCH "x64")
set(_lib_name discovery)
set(_lib_guid "DBDD94F7-E0F7-1369-8C28-0A27DC7EC5AC")
string(CONCAT _lib_binary_name ${_lib_name} "_" ${_os} "_" ${ARCH})

set(US_CMAKE_MINIMUM_REQUIRED_VERSION 3.10)
cmake_minimum_required(VERSION ${US_CMAKE_MINIMUM_REQUIRED_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#Versioning format - Major.Minor.builddate(yymmdd)
#update major and minor version manually
set(MAJOR_MINOR "1.5")
#For major bug fix information
set(BUG_FIX "0") 

set(_desc "The plugin that discovers/collects platform information")
set(_framework "1.0.0 >")
string(TIMESTAMP buildDate "%y%m%d")
set(_builddate ${buildDate})
string(CONCAT _lib_version ${MAJOR_MINOR} "." ${BUG_FIX} "." ${buildDate})
configure_file(versionconfig.h.in "${CMAKE_CURRENT_SOURCE_DIR}/src/include/version.h" )
#configure_file(manifest.json.in "${CMAKE_CURRENT_SOURCE_DIR}/manifest.json" )


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_NATIVE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_NATIVE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


# Default behavior is to embed metrics pass -DEMBED_METRICS=O in cmake commmand for dev mode
set(EMBED_METRICS OFF CACHE BOOL "Embed metrics into binary or read from seperate file at runtime")

find_package(nlohmann_json CONFIG REQUIRED)

set(json_config_file "snapshot_config.json")

set(metrics_file src/metricsinfo.cpp)

if(EMBED_METRICS)
    # read file containing list of metrics for this microservice and create macro header
    file(READ ${CMAKE_CURRENT_SOURCE_DIR}/samples/${json_config_file} _metrics)
    string(REPLACE "\\\"" "\\\\\"" _metrics "${_metrics}") # escape quotes for windows
    string(REPLACE "\\" "\\\\" _metrics "${_metrics}") # escape backslashes for cmake expansion
    string(REPLACE "\"" "\\\"" _metrics "${_metrics}") # escape quotes for windows
    string(REPLACE ";" "\\\\;" _metrics "${_metrics}") # escape semicolons for cmake
    string(REPLACE "%" "%%" _metrics "${_metrics}") # escape percents for cmake
    string(REGEX REPLACE "\t|\r|\n|[ ][ ]+" "" _metrics "${_metrics}") # remove tabs, eol, spaces
    add_definitions(-DMETRICS="${_metrics}")
else()
  add_definitions(-DMETRICS_FILE="${json_metrics_file}")
endif()
  
  
# A list of source code files
set(_srcs
  ../common/statusInfo.cpp    #common for plugins
  ../../common/helper.cpp     #common for all   
  ../../common/sysfswrapper.cpp
  src/plugin.cpp
  src/discovery.cpp
  src/apihandlers.cpp  
  src/Config.cpp
  ${metrics_file}
 
)  

include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
    ../common/include/       #common for plugins
    ../../common/include/    #common for all 
    src/include/
)


# Create the library
add_library(Plugin-${_lib_name} ${_srcs})

target_link_libraries(Plugin-${_lib_name} nlohmann_json::nlohmann_json) 

set_target_properties(Plugin-${_lib_name} PROPERTIES
  LABELS Plugin
  OUTPUT_NAME ${_lib_binary_name}
  DEBUG_POSTFIX ""
)

# Fixes immediate binding issue

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wconversion -pedantic -Wno-unknown-pragmas -Wformat -Wformat-security -D_FORTIFY_SOURCE=2 ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-overflow -fstack-protector-strong -fPIE -fPIC -z noexecstack")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-delete-null-pointer-checks")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fwrapv")
endif()


