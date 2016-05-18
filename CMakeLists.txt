cmake_minimum_required(VERSION 3.7)
project(eduproject)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        eduproject/file1.cpp
        eduproject/file2.cpp
        eduproject/main.cpp
        eduproject/file1.hpp eduproject/file2.hpp)

add_executable(eduproject ${SOURCE_FILES})