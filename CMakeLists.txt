cmake_minimum_required(VERSION 3.28)

project(EmbeddedWatch VERSION 0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(TEST_ENABLE "Enable test build" OFF)

add_subdirectory(ewatch)
add_subdirectory(viewer)

if(TEST_ENABLE)
    enable_testing()
    add_subdirectory(tests)
endif()
