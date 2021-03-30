cmake_minimum_required(VERSION 3.10)

project(proxytcp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_definitions(--coverage)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}   --coverage")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g -O0 -Wall  --coverage ")

include_directories(.)
include_directories(src)

ADD_LIBRARY(SourceFiles STATIC
        src/ConversationFlow/SOCKS5Flow/ClientHelloTransmission.cpp
        src/ConversationFlow/SOCKS5Flow/ConnectionRequestTransmission.cpp
        src/ConversationFlow/TLSFlow/TLSReceivingClientHello.cpp
        src/ConversationFlow/TLSFlow/TLSReceivingServerHello.cpp
        src/ConversationFlow/TLSFlow/TLSDefault.cpp
        src/ConversationFlow/ConversationFlow.cpp
        src/Connection/Connection.cpp
        src/ByteStream/ByteStream.cpp
        src/TrafficParsing/SOCKS5/SOCKS5Parser.cpp
        src/TrafficParsing/TLS/TLSParser.cpp
        src/TrafficParsing/TLS/TLSRecordParser.cpp
        src/TrafficParsing/TLS/TLSMessageParser.cpp
        src/PCAPGenerator/PCAPGenerator.cpp
        src/ConversationPipeline/ConversationPipeline.cpp
        src/ConversationPipeline/CapturingConversationPipeline.cpp
        src/ConversationPipeline/PayloadBuffer/PayloadBuffer.cpp
        src/ConversationPipeline/PayloadBuffer/TLSPayloadBuffer.cpp
        src/ConversationManager/SocketConversationManager.cpp
        src/ConversationManager/SocketCapturingConversationManager.cpp
        src/ConnectionManager/ConnectionManager.cpp
        src/ConnectionManager/SocketConnectionManager.cpp
        src/ConnectionManager/SocketCapturingConnectionManager.cpp
        src/Application/Application.cpp
        src/Connection/SocketConnection.cpp
        src/Connection/SocketCapturingConnection.cpp
        src/PCAP/PCAPFile.cpp
        src/PCAP/PCAPCapturingFile.cpp
        src/PCAP/PCAPReplayFile.cpp
        src/CommandLineOptions/CommandLineOptions.cpp
#        src/ConversationFlow/BreakConnection.cpp
#        src/ConversationFlow/BreakConnection.hpp
        )

add_executable(proxytcp main.cpp)
target_link_libraries(proxytcp SourceFiles)

add_subdirectory(googletest)
include_directories(googletest/googletest/include)

add_executable(proxytcp_tests
        Tests/UnitTests/ByteStream/ByteStreamTests.cpp
        Tests/UnitTests/TrafficParsing/SOCKS5Parser.cpp
        Tests/UnitTests/TrafficParsing/TLSParser.cpp
        Tests/UnitTests/SOCKS5/SOCKS5FlowTest.cpp
        Tests/UnitTests/ConnectionManager/ConnectionManagerTest.cpp
        Tests/UnitTests/SOCKS5/SOCKS5FlowTest.cpp
        Tests/UnitTests/ConversationPipeline/ConversationPipelineTest.cpp
        Tests/UnitTests/TrafficParsing/TLSRecordParser.cpp
        Tests/UnitTests/Connection/ConnectionTest.cpp
        )

target_link_libraries(proxytcp_tests SourceFiles)
target_link_libraries(proxytcp_tests gtest gtest_main)

