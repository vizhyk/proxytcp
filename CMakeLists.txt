cmake_minimum_required(VERSION 3.10)

project(proxytcp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_definitions(--coverage)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}   --coverage")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g -O0 -Wall  --coverage ")
include_directories(.)
include_directories(src)

add_executable(proxytcp
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
        src/ConversationPipeline/ConversationPipeline.cpp
        src/ConversationPipeline/PayloadBuffer/PayloadBuffer.cpp
        src/ConversationPipeline/PayloadBuffer/TLSPayloadBuffer.cpp
        src/ConversationManager/ConversationManager.cpp
        src/ConnectionManager/ConnectionManager.cpp
        src/Application/Application.cpp
        src/Connection/SocketConnection.cpp
        src/Connection/SocketCapturingConnection.cpp
        main.cpp
        )


add_subdirectory(googletest)
include_directories(googletest/googletest/include)
add_executable(proxytcp_tests
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
        src/ConversationPipeline/ConversationPipeline.cpp
        src/ConversationPipeline/PayloadBuffer/PayloadBuffer.cpp
        src/ConversationPipeline/PayloadBuffer/TLSPayloadBuffer.cpp
        src/ConversationManager/ConversationManager.cpp
        src/ConnectionManager/ConnectionManager.cpp
        Tests/UnitTests/ByteStream/ByteStreamTests.cpp
        Tests/UnitTests/TrafficParsing/SOCKS5Parser.cpp
        Tests/UnitTests/TrafficParsing/TLSParser.cpp
        Tests/UnitTests/SOCKS5/SOCKS5FlowTest.cpp
        Tests/UnitTests/Connection/Connection.cpp
        Tests/UnitTests/ConnectionManager/ConnectionManagerTest.cpp
        Tests/UnitTests/SOCKS5/SOCKS5FlowTest.cpp
        Tests/UnitTests/ConversationPipeline/ConversationPipelineTest.cpp
        Tests/UnitTests/TrafficParsing/TLSRecordParser.cpp
        )
target_link_libraries(proxytcp_tests gtest gtest_main)

