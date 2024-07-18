set(US_CMAKE_MINIMUM_REQUIRED_VERSION 3.15)
cmake_minimum_required(VERSION ${US_CMAKE_MINIMUM_REQUIRED_VERSION})
cmake_policy( VERSION ${US_CMAKE_MINIMUM_REQUIRED_VERSION})

#set(CMAKE_CXX_STANDARD 23) #rhel 40 has older version.
set(CMAKE_CXX_STANDARD 17)

### find our os (wins, centos, ubuntu, etc)
set(VAR_OS "")
IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
        MESSAGE(DEBUG "Linux")
        execute_process (
                COMMAND bash -c "awk -F= '/^ID=/{print $2}' /etc/os-release |tr -d '\n' | tr -d '\"'"
                OUTPUT_VARIABLE outOS
        )
        MESSAGE(DEBUG "Linux os: ${outOS}")
#	IF(${outOS} CONTAINS "Red Hat")
#		set(CMAKE_CXX_STANDARD 17)	
#	ENDIF
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
        MESSAGE(DEBUG "Windows")
        set(VAR_OS "wins")
ELSE()
        MESSAGE(FATAL "No OS detected!")
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Linux")


set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Only Debug and Release configurations are supported
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE INTERNAL "Only Debug and Release are supported" FORCE)

# cmake project name
project(toolkit)

#find_package(OpenSSL REQUIRED)
#find_package(Threads REQUIRED)

#pkg_check_modules(DBUS REQUIRED dbus-1 dbus-glib-1) # This calls pkgconfig with appropriate arguments

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT WIN32)
  # In non-win32 debug build, debug_malloc is on by default
  option(USE_DEBUG_MALLOC "Build oeenclave with memory leak detection capability." ON)
else ()
  # In win32 or non-debug builds, debug_malloc is off by default
  option(USE_DEBUG_MALLOC "Build oeenclave with memory leak detection capability." OFF)
endif ()

# OS and compiler specific options
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    SET(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} /Zi /GS /sdl /guard:cf")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GS /sdl /guard:cf")
    add_compile_options("/D NOMINMAX")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wconversion -pedantic -Wno-unknown-pragmas -Wformat -Wformat-security -D_FORTIFY_SOURCE=2  ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-overflow -fstack-protector-strong -fPIE -fPIC -z noexecstack")  
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-delete-null-pointer-checks")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fwrapv")

endif()

# Version format - Major.Minor.Bugfix.builddate(yymmdd)
set(MAJOR_MINOR_BUGFIX "1.08.0")
string(TIMESTAMP buildDate "%y%m%d")
string(CONCAT _version ${MAJOR_MINOR_BUGFIX} "." ${buildDate})

# set version define macro
#add_definitions(-DPKG_VERSION="${_version}")

# set binary name with system details
string(CONCAT BINARY_OUTPUT flow-tool "_" ${CMAKE_SYSTEM_NAME} "_" ${CMAKE_SYSTEM_PROCESSOR})
string(TOLOWER ${BINARY_OUTPUT} BINARY_OUTPUT_LOWER)
set(BINARY_OUTPUT_FILE ${BINARY_OUTPUT_LOWER})

# copy file from src to dest; Communication with your code
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/PkgConfig.h.in
#               ${CMAKE_CURRENT_BINARY_DIR}/PkgConfig.h)
               
include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ../common/include/
  src/include/
)

add_executable(
  ${BINARY_OUTPUT_FILE}
  src/main.cpp
  src/pluginManager.cpp
)

#target_link_libraries(${BINARY_OUTPUT_FILE} PRIVATE -lcap)

#if(UNIX)
#  target_link_libraries(${BINARY_OUTPUT_FILE} PRIVATE cap -lsqlite3 -lcurl)
#endif()
