cmake_minimum_required(VERSION 3.17)
project(proxytcp)

set(CMAKE_CXX_STANDARD 17)



include_directories(.)

add_executable(proxytcp
        main.cpp
#        Proxy.cpp
#        Proxy.hpp
#        Status.hpp
#        Offsets.hpp
#        Ports.hpp
        Application/Application.cpp
        ExecutionModes/ExecutionMode.cpp
        ExecutionModes/ForwardingMode.cpp
        ExecutionModes/TrackingMode.cpp
        ExecutionModes/BanMode.cpp
        CommandLineOptions/CommandLineOptions.cpp
        ConnectionInfo/ConnectionInfo.cpp
        ThreadManager/ThreadPool.cpp
        )

target_link_libraries(proxytcp pthread)