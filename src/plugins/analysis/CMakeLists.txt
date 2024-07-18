set(US_CMAKE_MINIMUM_REQUIRED_VERSION 3.10)
cmake_minimum_required(VERSION ${US_CMAKE_MINIMUM_REQUIRED_VERSION})
cmake_policy( VERSION ${US_CMAKE_MINIMUM_REQUIRED_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Only Debug and Release configurations are supported
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE INTERNAL "Only Debug and Release are supported" FORCE)

# cmake project name
project(analysis_service)

# The library name for the bundle
set(_lib_name analysis)
set(_lib_guid "98761234-ACD5-8992-3793-2222DC7EC5BB")


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# create the config directory to copy  temp files
set(CONFIG_DEST_DIR "${CMAKE_BINARY_DIR}/config")
set(CONFIG_SCHEMA_DIR "${CMAKE_BINARY_DIR}/config/schema")
#set the files path
set(FILE_CONFIG "${CMAKE_SOURCE_DIR}/config/golden_snapshot.json")
set(FILE_CONFIG_SCH "${CMAKE_SOURCE_DIR}/schema/analysis.manifest.schema_v1.json")

file(MAKE_DIRECTORY "${CONFIG_DEST_DIR}")
configure_file("${FILE_CONFIG}" "${CONFIG_DEST_DIR}/golden_snapshot.json" COPYONLY)
configure_file("${FILE_CONFIG_SCH}" "${CONFIG_SCHEMA_DIR}/analysis.manifest.schema_v1.json" COPYONLY)

# find and load external project dependencies
find_package(nlohmann_json CONFIG REQUIRED)
find_package(nlohmann_json_schema_validator REQUIRED)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT WIN32)
  # In non-win32 debug build, debug_malloc is on by default
  option(USE_DEBUG_MALLOC "Build oeenclave with memory leak detection capability." ON)
endif ()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # compiler flags from optimizing out security checks
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wconversion -pedantic -Wno-unknown-pragmas -Wformat -Wformat-security -D_FORTIFY_SOURCE=2 ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-overflow -fstack-protector-strong -fPIE -fPIC -z noexecstack ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-delete-null-pointer-checks")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fwrapv")
endif()

if(NOT BUILD_SHARED_LIBS)
message( STATUS "${CMAKE_PROJECT_NAME} - BUILD_SHARED_LIBS Flag not set. Turning on...")
option(BUILD_SHARED_LIBS "Build shared library" ON)
endif()

# Version format - Major.Minor_Bugfix.builddate(yymmdd)
set(MAJOR_MINOR_BUGFIX "1.0")
set(_desc "analysis plugin")
string(TIMESTAMP buildDate "%y%m%d")
string(CONCAT _lib_version ${MAJOR_MINOR_BUGFIX} "." ${buildDate})
add_definitions(-DMICRO_SERVICE_VERSION="${_lib_version}" -DMICRO_SERVICE_NAME="${_lib_name}")

# generate manifest.json file 
file(WRITE manifest.json "{
    \"bundle.symbolic_name\" : \"${_lib_name}\",
    \"bundle.activator\" : true
}")

# list of source code files
set(_srcs  
  ../../common/helper.cpp
  ../common/statusInfo.cpp  
  ../common/validator.cpp
  src/analysis.cpp
  src/pkgrules.cpp
  src/plugin.cpp
)

include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
    ../common/include/
    ../../common/include/    
    src/include/
)
# Create the library
add_library(kit-${_lib_name} ${_srcs})

# Add the US_BUNDLE_NAME target property
set_property(TARGET kit-${_lib_name} APPEND PROPERTY US_BUNDLE_NAME ${_lib_name})

# Add the required compile definition
set_property(TARGET kit-${_lib_name} APPEND PROPERTY COMPILE_DEFINITIONS US_BUNDLE_NAME=${_lib_name})


#append the os and arch name
string(CONCAT BINARY_OUTPUT ${BINARY_OUTPUT_FILE} "_" ${CMAKE_SYSTEM_NAME} "_" ${CMAKE_SYSTEM_PROCESSOR})
string(TOLOWER ${BINARY_OUTPUT} BINARY_OUTPUT_LOWER)
set_target_properties(${BINARY_OUTPUT_FILE} PROPERTIES OUTPUT_NAME ${BINARY_OUTPUT_LOWER})
string(CONCAT _lib_name_binary ${_lib_name} ${BINARY_OUTPUT_LOWER} )

# Link all libraries 
target_link_libraries(kit-${_lib_name} PRIVATE nlohmann_json::nlohmann_json nlohmann_json_schema_validator)

set_target_properties(kit-${_lib_name} PROPERTIES
  LABELS Kit
  OUTPUT_NAME ${_lib_name_binary}
  DEBUG_POSTFIX ""
)

add_definitions(-DMICRO_SERVICE_GUID="${_lib_guid}")





