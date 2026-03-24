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
