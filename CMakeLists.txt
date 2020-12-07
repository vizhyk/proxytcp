cmake_minimum_required(VERSION 3.10)

project(proxytcp)

set(CMAKE_CXX_STANDARD 17)
include_directories(.)

add_executable(proxytcp
        src/ByteStream/ByteStream.cpp
        src/TrafficParsing/SOCKS5/SOCKS5Parser.cpp
        src/TrafficParsing/TLS/TLSParser.cpp
        main.cpp
        )


add_subdirectory(googletest)
include_directories(googletest/googletest/include)
add_executable(proxytcp_tests
        src/Connection/Connection.cpp
        src/Connection/ClientConnection.cpp
        src/Connection/ServerConnection.cpp
        src/ByteStream/ByteStream.cpp
        src/TrafficParsing/SOCKS5/SOCKS5Parser.cpp
        src/TrafficParsing/TLS/TLSParser.cpp
        Tests/ByteStream/ByteStreamTests.cpp
        Tests/TrafficParsing/SOCKS5Parser.cpp
        Tests/TrafficParsing/TLSParser.cpp
        )
target_link_libraries(proxytcp_tests gtest gtest_main)



