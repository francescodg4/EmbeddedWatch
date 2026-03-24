add_executable(embeddedwatch_viewer src/main.cpp)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)

target_link_libraries(embeddedwatch_viewer PRIVATE raylib embeddedwatch)
