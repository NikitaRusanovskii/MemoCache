cmake_minimum_required(VERSION 3.16)
set(PROJECT memocache)
project(${PROJECT} VERSION 0.0.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(MEMOCACHE_BUILD_TESTS "build tests" OFF)

message("MemoCache library building... Version: ${PROJECT_VERSION}")

set(INCLUDES "${CMAKE_SOURCE_DIR}/include")

add_library(${PROJECT} INTERFACE)
target_include_directories(${PROJECT} INTERFACE ${INCLUDES})
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("--- Flags to optimization: -O3")
    target_compile_options(${PROJECT} INTERFACE -O3)
endif()

if (MEMOCACHE_BUILD_TESTS)
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        "v1.17.0"
)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT}_tests tests/main.cpp)
target_link_libraries(${PROJECT}_tests PRIVATE ${PROJECT} GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${PROJECT}_tests)

endif()

message("MemoCache completed.")