cmake_minimum_required(VERSION 3.8)
project(draw)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O -Wall -fno-builtin -L/usr/lib64 -lXm -lXt -lSM -lICE -lXp -lXext -lX11 ")
set(CMAKE_C_STANDARD 99)


set(SOURCE_FILES draw.c)
add_executable(Draw ${SOURCE_FILES})

target_link_libraries(Draw Xm Xt SM ICE Xp Xext X11)
