cmake_minimum_required(VERSION 3.30)
project(OpenMPExample CXX)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenMP REQUIRED)

add_executable(OpenMPExample main.cpp)

target_link_libraries(OpenMPExample PRIVATE OpenMP::OpenMP_CXX)  # Используем OpenMP_CXX для C++