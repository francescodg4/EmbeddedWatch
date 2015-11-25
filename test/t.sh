#!/bin/bash

declare -A test
# ["st"]="make EWatchStopwatch; ./testEWatchStopwatch" 
test=( 
    ["a"]="make testEWatchAlarm && ./testAlarm"
    ["st"]="make testEWatchStopwatch && ./testEWatchStopwatch"
    ["c"]="make testEWatchClock && ./testEWatchClock"
    ["cc"]="make testEWatch && ./testEWatch"
    ["t"]="make testEWatchTimeset && ./testEWatchTimeset"
)

eval ${test["$1"]}

# if [ "$1" == "alarm" ]; then
#     make testAlarm
#     ./testAlarm
# elif [ "$1" == "st" ]; then
#     make testEWatchStopwatch
#     ./testEWatchStopwatch
# elif [ "$1" == "c" ]; then
#     make testEWatchClock
#     ./testEWatchClock
# elif [ "$1" == "cc" ]; then
#     make testEWatch
#     ./testEWatch
# elif [ "$1" == "all" ]; then
#     ./testAlarm
#     ./testStopwatch
#     ./testClock
#     ./testEWatch
# fi
