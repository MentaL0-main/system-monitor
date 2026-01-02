cmake_minimum_required(VERSION 3.10)

project(system-monitor CXX)

find_package(SDL3 REQUIRED)

add_executable(system-monitor src/main.cpp src/monitor.cpp)

target_link_libraries(system-monitor PRIVATE SDL3::SDL3)

target_compile_options(system-monitor PRIVATE "-g")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
