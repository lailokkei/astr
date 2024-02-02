cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(astr)

add_executable(astr src/main.cpp src/math.cpp src/entities.cpp)
target_include_directories(astr PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(astr SDL2)
