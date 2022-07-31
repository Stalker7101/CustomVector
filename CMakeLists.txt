cmake_minimum_required(VERSION 3.0)
project(Vector VERSION 1.0.0 LANGUAGES CXX)

file(GLOB CPPS "*.?pp ")

include_directories(headers)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

add_executable(${PROJECT_NAME} ${CPPS})
