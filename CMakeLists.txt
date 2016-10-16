cmake_minimum_required(VERSION 3.2)
project(videoproc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package(PkgConfig)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-0.10)
include_directories(${GSTREAMER_INCLUDE_DIRS})

add_library(videoproc STATIC
        videoproc/VideoHandler.h
        videoproc/VideoHandler.cpp
        videoproc/VideoReader.h
        videoproc/VideoReader.cpp
        videoproc/VideoFileReader.h
        videoproc/VideoFileReader.cpp
        videoproc/ImageWriter.h
        videoproc/ImageWriter.cpp
        videoproc/MainLoop.h
        videoproc/MainLoop.cpp)

install(DIRECTORY videoproc DESTINATION include FILES_MATCHING PATTERN "*.h")
install(TARGETS videoproc DESTINATION lib)
