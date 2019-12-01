cmake_minimum_required (VERSION 2.8)

project (demo)

add_executable(main main.cpp SerialPort.cpp)

SET(CMAKE_CXX_FLAGS "-std=c++0x -pthread")
