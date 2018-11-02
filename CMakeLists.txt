cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(Network)
add_subdirectory(Database)
add_subdirectory(Compiler)
add_subdirectory(Shell)
