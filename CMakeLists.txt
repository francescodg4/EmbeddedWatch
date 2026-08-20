cmake_minimum_required(VERSION 3.16)

project(EmbeddedWatch VERSION 0.0.0)

include(FetchContent)

add_library(embeddedwatch
  src/ClockCounter.c
  src/EWatch.c
  src/EWatchAlarm.c
  src/EWatchClock.c
  src/EWatchControl.c
  src/EWatchStopwatch.c
  src/EWatchTimeset.c
)

target_include_directories(embeddedwatch PUBLIC include)

option(BUILD_TESTING "Build the test suite" ON)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
