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
        Application/Application.hpp
        ExecutionModes/ExecutionMode.cpp
        ExecutionModes/ExecutionMode.hpp
        ExecutionModes/ForwardingMode.cpp
        ExecutionModes/ForwardingMode.hpp
        ExecutionModes/TrackingMode.cpp
        ExecutionModes/TrackingMode.hpp
        ExecutionModes/BanMode.cpp
        ExecutionModes/BanMode.hpp
        Utilities/Offsets.hpp
        Utilities/Ports.hpp
        Utilities/Status.hpp
        CommandLineOptions/CommandLineOptions.cpp
        CommandLineOptions/CommandLineOptions.hpp
        ConnectionInfo/ConnectionInfo.cpp
        ConnectionInfo/ConnectionInfo.hpp ThreadManager/ThreadManager.cpp ThreadManager/ThreadManager.hpp Utilities/NonCopyable.hpp)

target_link_libraries(proxytcp pthread)