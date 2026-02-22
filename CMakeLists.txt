cmake_minimum_required(VERSION 3.20)
project(alien_shooter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Curses REQUIRED)

add_executable(alien_shooter
    src/main.cpp
    src/game.cpp
    src/player.cpp
    src/alien.cpp
    src/bullet.cpp
    src/pickup.cpp
    src/renderer.cpp
)

target_include_directories(alien_shooter PRIVATE ${CURSES_INCLUDE_DIRS} src)
target_link_libraries(alien_shooter PRIVATE ${CURSES_LIBRARIES})
