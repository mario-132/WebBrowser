cmake_minimum_required(VERSION 3.5)

project(HTMLRenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(HTMLRenderer main.cpp freetypeeasy.cpp freetypeeasy.h x11window.cpp x11window.h stb_image.h
    webservice.cpp webservice.h renderdom.cpp renderdom.h debugger.cpp debugger.h css.cpp css.h htmlrenderer.cpp htmlrenderer.h)
target_link_libraries(HTMLRenderer gumbo)
target_include_directories(HTMLRenderer PUBLIC /usr/include/freetype2 /usr/include/libpng16 /opt/netsurf)
add_definitions(-DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=8)
target_link_directories(HTMLRenderer PUBLIC /opt/netsurf)
target_link_libraries(HTMLRenderer freetype X11 curl css parserutils wapcaplet)

option(USEMAGICK "Use Magick++ image decoder" ON)
if (USEMAGICK)
    target_include_directories(HTMLRenderer PUBLIC /usr/include/ImageMagick-6 /usr/include/x86_64-linux-gnu/ImageMagick-6)
    target_link_libraries(HTMLRenderer Magick++-6.Q16 MagickWand-6.Q16 MagickCore-6.Q16)
    add_definitions(-DUSEMAGICK)
else()
    add_definitions(-DUSESTBI)
endif()

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror=return-type")
option(USEDEBUGGER "Use gtk based debugger" ON)
if (USEDEBUGGER)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET "gtk+-3.0")
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -rdynamic -Werror=return-type")
    target_link_libraries(HTMLRenderer PkgConfig::GTK3 pthread)
endif()
