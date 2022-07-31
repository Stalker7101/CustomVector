cmake_minimum_required(VERSION 3.10.0)
project(CustomVector VERSION 1.0 LANGUAGES CXX)

set(SOURCES ${CMAKE_SOURCE_DIR}/src)
set(HEADERS ${CMAKE_SOURCE_DIR}/hdr)

file(GLOB CPPS "${SOURCES}/*.cpp")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

add_executable(${PROJECT_NAME} ${CPPS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${HEADERS}
)
