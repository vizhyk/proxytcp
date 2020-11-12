cmake_minimum_required(VERSION 3.17)
project(proxytcp)

set(CMAKE_CXX_STANDARD 17)



include_directories(.)

add_subdirectory(googletest)
include_directories(googletest/googletest/include)


add_executable(proxytcp
        src/main.cpp
        src/Application/Application.cpp
        src/ExecutionModes/ExecutionMode.cpp
        src/ExecutionModes/ForwardingMode.cpp
        src/ExecutionModes/TrackingMode.cpp
        src/ExecutionModes/BanMode.cpp
        src/CommandLineOptions/CommandLineOptions.cpp
        src/ConnectionInfo/ConnectionInfo.cpp
        src/ThreadManager/ThreadPool.cpp
        src/ExecutionModes/SOCKS5Mode.cpp
        src/TestMain.cpp
        src/Connection/Connection.cpp
        src/ConnectionManager/ConnectionManager.cpp
        src/ConversationManager/ConversationManager.cpp
        src/DataBuffer/DataBuffer.cpp
        src/NetworkTrafficParser/SOCKSParser.cpp)

target_link_libraries(proxytcp pthread)
#target_link_libraries(proxytcp gtest gtest_main)
target_link_libraries(proxytcp gtest)
