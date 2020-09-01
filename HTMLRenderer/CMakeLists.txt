cmake_minimum_required(VERSION 3.5)

project(HTMLRenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(HTMLRenderer main.cpp stb_image_write.h stb_truetype.h freetypeeasy.cpp freetypeeasy.h x11window.cpp x11window.h old.cpp)
target_link_libraries(HTMLRenderer gumbo)
target_include_directories(HTMLRenderer PUBLIC /usr/include/freetype2 /usr/include/libpng16)
target_link_libraries(HTMLRenderer freetype X11)
