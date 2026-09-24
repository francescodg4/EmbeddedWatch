cmake_minimum_required(VERSION 3.16)

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  raylib
  URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(raylib)

add_executable(ewatch-gui
  main.c
  GuiCommon.c
  ThemeModern.c
  ThemeWinamp.c
  ThemeEmerald.c
)

target_link_libraries(ewatch-gui PRIVATE embeddedwatch raylib)

if(MSVC)
  target_compile_definitions(ewatch-gui PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if(NOT WIN32)
  target_link_libraries(ewatch-gui PRIVATE m)
endif()
