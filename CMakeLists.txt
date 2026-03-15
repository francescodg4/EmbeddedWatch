cmake_minimum_required(VERSION 3.28)

project(EmbeddedWatch VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(TEST_ENABLE "Enable test build" OFF)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)

add_library(
    embeddedwatch
    src/ClockCounter.c
    src/EWatch.c
    src/EWatchAlarm.c
    src/EWatchClock.c
    src/EWatchControl.c
    src/EWatchStopwatch.c
    src/EWatchTimeset.c
    src/EWatchControl.c
)

target_include_directories(embeddedwatch PUBLIC include)

add_executable(embeddedwatch_viewer
    viewer/src/main.cpp
)

target_link_libraries(embeddedwatch_viewer PRIVATE raylib embeddedwatch)

if(TEST_ENABLE)
    enable_testing()
    add_subdirectory(tests)
endif()
