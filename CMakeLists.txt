cmake_minimum_required(VERSION 3.10)
project(proxytcp)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_subdirectory(googletest)
include_directories(googletest/googletest/include)


add_executable(proxytcp
        src/ByteStream/ByteStream.cpp
        Tests/ByteStream/ByteStreamTests.cpp

        )


target_link_libraries(proxytcp gtest gtest_main)
