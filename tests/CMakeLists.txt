include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.13.0
)

FetchContent_MakeAvailable(Catch2)

add_executable(
    embeddedwatch_test
    test_ewatch.cpp
    test_ewatch_alarm.cpp
    test_ewatch_clock.cpp
    test_ewatch_stopwatch.cpp
    test_ewatch_timeset.cpp
    utility.cpp
)

target_link_libraries(
    embeddedwatch_test
    PRIVATE embeddedwatch Catch2::Catch2WithMain
)

add_test(NAME embeddedwatch_test COMMAND embeddedwatch_test)
