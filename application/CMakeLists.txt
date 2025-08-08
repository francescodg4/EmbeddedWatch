include(FetchContent)

fetchcontent_declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

fetchcontent_makeavailable(raylib)

add_executable(embeddedwatch_app
    src/main.cpp
    src/timer.cpp
)

target_include_directories(embeddedwatch_app PRIVATE include)
target_link_libraries(embeddedwatch_app PRIVATE embeddedwatch raylib)
