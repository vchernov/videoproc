cmake_minimum_required(VERSION 3.2)
project(videoproc)

option(VIDEOPROC_BUILD_EXAMPLES "Build example programs" OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-0.10)
include_directories(${GSTREAMER_INCLUDE_DIRS})

add_library(videoproc STATIC
    videoproc/Pipeline.h
    videoproc/Pipeline.cpp
    videoproc/MainLoop.h
    videoproc/MainLoop.cpp
    videoproc/VideoReader.h
    videoproc/VideoReader.cpp
    videoproc/VideoFileReader.h
    videoproc/VideoFileReader.cpp
    videoproc/ImageWriter.h
    videoproc/ImageWriter.cpp
    videoproc/TeePipeline.cpp
    videoproc/TeePipeline.h
    videoproc/ElementProperties.h
)

install(DIRECTORY videoproc DESTINATION include FILES_MATCHING PATTERN "*.h")
install(TARGETS videoproc DESTINATION lib)

if (VIDEOPROC_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
