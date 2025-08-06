cmake_minimum_required(VERSION 3.16)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(run-test
  TestEWatch.cpp
  TestEWatchAlarm.cpp
  TestEWatchClock.cpp
  TestEWatchStopwatch.cpp
  TestEWatchTimeset.cpp
  utility.cpp
)

target_link_libraries(run-test PRIVATE Catch2::Catch2WithMain)

target_link_libraries(run-test PRIVATE embeddedwatch)
