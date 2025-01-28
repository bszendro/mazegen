cmake_minimum_required(VERSION 3.26)

project(mazegen)

set(SOURCES
    hexmaze.cpp
    mazegen.cpp
    svg_painter.cpp
    )

set(HEADERS
    gen_wilson.h
    hexmaze.h
    matrix.h
    svg_painter.h
    )

add_executable(mazegen ${SOURCES} ${HEADERS})
target_compile_features(mazegen PUBLIC cxx_std_20)
