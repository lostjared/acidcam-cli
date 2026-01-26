cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(acidcam-cli LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build configuration: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckIncludeFileCXX)
include(CheckLibraryExists)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    list(APPEND COMPILER_FLAGS "-Wall" "-pedantic" "-Ofast")
else()
    message(STATUS "Debug Mode")
    list(APPEND COMPILER_FLAGS "-Wall" "-pedantic" "-g")
endif()

set(SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/acidcam/acidcam-cli-main.cpp
    ${CMAKE_SOURCE_DIR}/acidcam/acidcam-cli.cpp
    ${CMAKE_SOURCE_DIR}/acidcam/ffmpeg_write.cpp
)

message(STATUS "Checking for required headers...")

check_include_file_cxx("unistd.h" HAVE_UNISTD_H)
if(NOT HAVE_UNISTD_H)
    message(FATAL_ERROR "Required header <unistd.h> not found")
endif()

check_include_file_cxx("dlfcn.h" HAVE_DLFCN_H)
if(NOT HAVE_DLFCN_H)
    message(FATAL_ERROR "Required header <dlfcn.h> not found")
endif()

check_include_file_cxx("dirent.h" HAVE_DIRENT_H)
if(NOT HAVE_DIRENT_H)
    message(FATAL_ERROR "Required header <dirent.h> not found")
endif()

check_include_file_cxx("sys/stat.h" HAVE_SYS_STAT_H)
if(NOT HAVE_SYS_STAT_H)
    message(FATAL_ERROR "Required header <sys/stat.h> not found")
endif()

message(STATUS "All required system headers found")

find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found. Please install OpenCV development packages.")
endif()

if(OpenCV_VERSION VERSION_LESS "3.0")
    message(FATAL_ERROR "OpenCV version ${OpenCV_VERSION} found, but version 3.0 or greater is required")
endif()
message(STATUS "Found OpenCV: ${OpenCV_VERSION}")

find_package(Threads REQUIRED)
if(NOT Threads_FOUND)
    message(FATAL_ERROR "Threading library not found")
endif()
message(STATUS "Found threading library")

find_library(DL_LIBRARY dl)
if(NOT DL_LIBRARY)
    check_library_exists(c dlopen "" HAVE_DLOPEN_IN_LIBC)
    if(HAVE_DLOPEN_IN_LIBC)
        set(DL_LIBRARY "")
        message(STATUS "dlopen found in libc")
    else()
        message(FATAL_ERROR "libdl not found. Please install libc development packages.")
    endif()
else()
    message(STATUS "Found libdl: ${DL_LIBRARY}")
endif()

find_library(ACIDCAM_LIBRARY acidcam)
if(NOT ACIDCAM_LIBRARY)
    message(FATAL_ERROR "libacidcam not found. Please install libacidcam.")
endif()
message(STATUS "Found libacidcam: ${ACIDCAM_LIBRARY}")

find_path(ACIDCAM_INCLUDE_DIR acidcam/ac.h)
if(NOT ACIDCAM_INCLUDE_DIR)
    message(FATAL_ERROR "acidcam/ac.h header not found. Please install libacidcam development files.")
endif()
message(STATUS "Found acidcam headers: ${ACIDCAM_INCLUDE_DIR}")

add_executable(acidcam-cli ${SOURCE_FILES})
target_include_directories(acidcam-cli PRIVATE
    ${ACIDCAM_INCLUDE_DIR}/acidcam
    ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(acidcam-cli PRIVATE
    ${ACIDCAM_LIBRARY}
    ${OpenCV_LIBS}
    Threads::Threads
    ${DL_LIBRARY}
)
target_compile_options(acidcam-cli PRIVATE ${COMPILER_FLAGS})


set_target_properties(acidcam-cli PROPERTIES
    BUILD_RPATH "${CMAKE_BINARY_DIR}/lib"
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

install(TARGETS acidcam-cli DESTINATION bin)
install(DIRECTORY scripts/
    DESTINATION bin
    PATTERN "scripts/*"
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
message(STATUS "")
message(STATUS "=== Build Summary ===")
message(STATUS "Build type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "Install prefix:  ${CMAKE_INSTALL_PREFIX}")
message(STATUS "OpenCV version:  ${OpenCV_VERSION}")
message(STATUS "libacidcam:      ${ACIDCAM_LIBRARY}")
message(STATUS "=====================")
