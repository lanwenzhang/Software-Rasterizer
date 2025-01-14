cmake_minimum_required(VERSION 3.12)

project(softwareRasterizer)

set(CMAKE_CXX_STANDARD 17)

file(GLOB copyResources "./assets")
file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(application)
add_subdirectory(gpu)

add_executable(softwareRasterizer "main.cpp" "math/matrix.h")

target_link_libraries(softwareRasterizer applicationLib gpuLib)