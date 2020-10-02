cmake_minimum_required(VERSION 3.17)
project(proxytcp)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(proxytcp
        main.cpp
        Proxy.cpp
        Proxy.hpp
        Status.hpp Constants.hpp)
