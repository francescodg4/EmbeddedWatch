cmake_minimum_required(VERSION 3.16)

project(EmbeddedWatch VERSION 0.0.0)

include(FetchContent)

add_library(embeddedwatch
  ClockCounter.c
  EWatch.c
  EWatchAlarm.c
  EWatchClock.c
  EWatchControl.c
  EWatchStopwatch.c
  EWatchTimeset.c
)

add_subdirectory(test)
