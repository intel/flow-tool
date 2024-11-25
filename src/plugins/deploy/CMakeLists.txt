project(deploy_plugin)

message (STATUS "Project name = ${CMAKE_PROJECT_NAME}")
#message( STATUS "${CMAKE_PROJECT_NAME} - setup include paths and linker dependencies ..." )

set(US_CMAKE_MINIMUM_REQUIRED_VERSION 3.12)
cmake_minimum_required(VERSION ${US_CMAKE_MINIMUM_REQUIRED_VERSION})
cmake_policy( VERSION ${US_CMAKE_MINIMUM_REQUIRED_VERSION})

#set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set to ON to include build-time test binary
option(RUN_BUILD_TIME_TESTS "Build test src and execute tests at compile time" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#message( STATUS "${CMAKE_PROJECT_NAME} - find and load external project settings ..." )

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT WIN32)
  # In non-win32 debug build, debug_malloc is on by default
  option(USE_DEBUG_MALLOC "Build oeenclave with memory leak detection capability." ON)
else ()
  # In win32 or non-debug builds, debug_malloc is off by default
  option(USE_DEBUG_MALLOC "Build oeenclave with memory leak detection capability." OFF)
endif ()

#message( STATUS "${CMAKE_PROJECT_NAME} - add clean-all target " )
#add_custom_target(clean-all
#   COMMAND ${CMAKE_BUILD_TOOL} clean
   #COMMAND ${CMAKE_COMMAND} -P clean-all.cmake
#)

if(NOT BUILD_SHARED_LIBS)
message( STATUS "${CMAKE_PROJECT_NAME} - BUILD_SHARED_LIBS Flag not set. Turning on...")
option(BUILD_SHARED_LIBS "Build shared library" ON)
endif()

# The library name for the bundle
if(WIN32)
set(_os "Windows")
else() 
set(_os "Linux")
endif()

set(ARCH "x86-64")
set(_lib_name deploy)
#generated 8/29/2023 -- redo
#set(_lib_guid "77ecbbe3-9b05-4b95-9d36-31428dcc32b9")
string(CONCAT _lib_binary_name ${_lib_name} "_" ${_os} "_" ${ARCH})

#Versioning format - Major.Minor.builddate(yymmdd)
#update major and minor version manually
set(MAJOR_MINOR "1.0")
#For major bug fix information
set(BUG_FIX "0") 

set(_desc "The plugin that packages components and deploy the components")
set(_framework "1.0.0 >")
string(TIMESTAMP buildDate "%y%m%d")
set(_builddate ${buildDate})
string(CONCAT _lib_version ${MAJOR_MINOR} "." ${BUG_FIX} "." ${buildDate})
#configure_file(versionconfig.h.in "${CMAKE_CURRENT_SOURCE_DIR}/src/include/version.h" )
#configure_file(manifest.json.in "${CMAKE_CURRENT_SOURCE_DIR}/manifest.json" )

#add schema
set(CONFIG_DEST_DIR "${CMAKE_BINARY_DIR}/config")
set(CONFIG_SCHEMA_DIR "${CMAKE_BINARY_DIR}/config/schema")
set(FILE_CONFIG "${CMAKE_SOURCE_DIR}/schema/deploy.manifest.schema_v1.json")
file(MAKE_DIRECTORY "${CONFIG_DEST_DIR}")
configure_file("${FILE_CONFIG}" "${CONFIG_SCHEMA_DIR}/deploy.manifest.schema_v1.json" COPYONLY)
#message("_binary_name : ${FILE_CONFIG}")
if(WIN32)
  string(REPLACE "/" "\\\\" CMAKE_LIBRARY_OUTPUT_DIRECTORY_NATIVE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  string(REPLACE "/" "\\\\" CMAKE_RUNTIME_OUTPUT_DIRECTORY_NATIVE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
else()
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_NATIVE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_NATIVE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

# Default behavior is to embed metrics pass -DEMBED_METRICS=O in cmake commmand for dev mode
set(EMBED_METRICS OFF CACHE BOOL "Embed metrics into binary or read from seperate file at runtime")

#add_definitions(-DMICRO_SERVICE_GUID="${_lib_guid}")
#find_package(OpenSSL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(nlohmann_json_schema_validator REQUIRED)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ../../common/include/
    src/include/
    ../common/include/
)

# A list of source code files
set(_srcs
  src/plugin.cpp
  ../../common/applog.cpp
  ../../common/helper.cpp
  ../common/validator.cpp
  ../common/manifestDataStructure.cpp
  ../common/sysinfo.cpp
  src/deploy.cpp
  src/commandreference.cpp
  src/apihandlers.cpp
  ../common/graph.cpp
)

# Create the library
add_library(Plugin-${_lib_name} ${_srcs})

target_link_libraries(Plugin-${_lib_name} PRIVATE nlohmann_json::nlohmann_json nlohmann_json_schema_validator)

set_target_properties(Plugin-${_lib_name} PROPERTIES
  LABELS Plugin
  OUTPUT_NAME ${_lib_binary_name}
  DEBUG_POSTFIX ""
)

# Fixes immediate binding issue
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    SET(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} /Zi /GS /sdl /guard:cf")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GS /sdl /guard:cf")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wconversion -pedantic -Wno-unknown-pragmas -Wformat -Wformat-security -D_FORTIFY_SOURCE=2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-overflow -fstack-protector-strong -fPIE -fPIC -z noexecstack ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-delete-null-pointer-checks")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fwrapv")
endif()

if(RUN_BUILD_TIME_TESTS)
    # Enable CTest
    enable_testing()
    
    add_subdirectory(test/buildtime_tests)
endif()