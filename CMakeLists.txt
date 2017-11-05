cmake_minimum_required(VERSION 3.7)
project(sslclient)

set(CMAKE_CXX_STANDARD 14)
set(PROJECTNAME_ARCHITECTURE "x64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")

set(SOURCE_FILES
        sslclient/client.cpp
        sslclient/sync_ssl_client.hpp)

add_executable(sslclient ${SOURCE_FILES})

find_package(Boost 1.64.0 COMPONENTS system thread filesystem REQUIRED)
find_package(OpenSSL REQUIRED)

if (Boost_FOUND AND ${OPENSSL_FOUND})
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(sslclient ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
endif ()

target_link_libraries(sslclient ws2_32)

