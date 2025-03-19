cmake_minimum_required(VERSION 3.12)

project(softwareRasterizer)

set(CMAKE_CXX_STANDARD 17)

file(GLOB copyResources "./assets" "./thirdParty/bin/assimp/assimp-vc143-mtd.dll")
file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/include)

link_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/lib/assimp)

add_subdirectory(application)
add_subdirectory(gpu)

add_executable(softwareRasterizer "main.cpp")

target_link_libraries(softwareRasterizer applicationLib gpuLib assimp-vc143-mtd.lib)