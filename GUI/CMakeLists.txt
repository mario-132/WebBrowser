cmake_minimum_required(VERSION 3.5)

project(GUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(GUI main.cpp x11window.cpp x11window.h freetypeeasy.cpp freetypeeasy.h)
target_include_directories(GUI PUBLIC /usr/include/freetype2 /usr/include/libpng16)
target_link_libraries(GUI X11 freetype)
