cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(acidcam-cli LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(LIBACIDCAM_PATH "${CMAKE_INSTALL_PREFIX}/lib/acidcam" CACHE STRING "Linker Path" FORCE)
set(LIBACIDCAM_INCLUDE "$(CMAKE_INSTALL_PREFIX}/include/acidcam" CACHE STRING "Include Path")
message(STATUS "Build configuration is: ${CMAKE_BUILD_TYPE}")
message(STATUS "is the C++ compiler loaded? ${CMAKE_CXX_COMPILER_LOADED}")

if(CMAKE_CXX_COMPILER_LOADED)
	message(STATUS "The C++ Compiler ID is: ${CMAKE_CXX_COMPILER_ID}")
	message(STATUS "The C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQURIED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
list(APPEND COMPILER_FLAGS "-Wall" "-pedantic" "-ansi" "-Ofast")
else()
message(STATUS "Debug Mode")
list(APPEND COMPILER_FLAGS "-Wall" "-pedantic" "-ansi")
endif()

list(APPEND source_files 
${CMAKE_SOURCE_DIR}/acidcam/acidcam-cli-main.cpp
${CMAKE_SOURCE_DIR}/acidcam/acidcam-cli.cpp
${CMAKE_SOURCE_DIR}/acidcam/acidcam-cli.hpp
)

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
#find_package(acidcam REQUIRED)
find_library(acidcam REQUIRED)
find_library(dl REQUIRED)
if(${OpenCV_VERSION} LESS 3.0)
	message(FATAL_ERROR "Requires OpenCV 3 or greater")
endif()

include_directories(${CMAKE_INSTALL_PREFIX}/include/acidcam ${OpenCV_INCLUDE_DIRS})
add_executable(acidcam-cli ${source_files})
target_link_directories(acidcam-cli PUBLIC ${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries(acidcam-cli dl Threads::Threads ${OpenCV_LIBS} acidcam)
target_compile_options(acidcam-cli PRIVATE ${COMPILER_FLAGS})
install(TARGETS acidcam-cli DESTINATION bin)
