cmake_minimum_required(VERSION 3.7)
project(eduproject)

set(CMAKE_CXX_STANDARD 14)
set(PROJECTNAME_ARCHITECTURE "x64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")

set(SOURCE_FILES
        eduproject/client.cpp
        eduproject/sync_tcp_client.hpp)

add_executable(eduproject ${SOURCE_FILES})

find_package(Boost 1.64.0 COMPONENTS system thread filesystem REQUIRED)
if (Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(eduproject ${Boost_LIBRARIES})
endif ()

target_link_libraries(eduproject ws2_32)

