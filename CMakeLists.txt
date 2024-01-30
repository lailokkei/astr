cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(astr)
add_executable(astr main.cpp math.cpp)
target_link_libraries(astr SDL2)

