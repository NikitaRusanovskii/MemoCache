cmake_minimum_required(VERSION 3.16)
set(PROJECT memocache)
project(${PROJECT} VERSION 0.0.1)

message("MemoCache library building... Version: ${PROJECT_VERSION}")

set(INCLUDES "${CMAKE_SOURCE_DIR}/include")
add_library(${PROJECT} INTERFACE)
target_include_directories(${PROJECT} INTERFACE ${INCLUDES})

message("MemoCache completed.")