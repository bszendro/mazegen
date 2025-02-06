cmake_minimum_required(VERSION 3.30)

project(mazegen)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -pedantic -Werror)

find_package(Boost 1.87.0 REQUIRED COMPONENTS program_options)

set(SOURCES
    src/brick_maze.cpp
    src/hexmaze.cpp
    src/square_maze.cpp
    src/svg_painter.cpp
    )

set(HEADERS
    src/brick_maze.h
    src/gen_wilson.h
    src/hexmaze.h
    src/matrix.h
    src/maze_grid.h
    src/node_index_2d.h
    src/painter.h
    src/square_maze.h
    src/svg_painter.h
    )

add_executable(mazegen src/main.cpp ${SOURCES} ${HEADERS})
target_link_libraries(mazegen Boost::program_options)

target_include_directories(mazegen PRIVATE ${PROJECT_SOURCE_DIR}/include)

# ----------- Unit Tests -----------

enable_testing()

find_package(GTest REQUIRED)

set(TEST_SOURCES
    tests/test_brick_maze.cpp
    tests/test_gen_wilson.cpp
    tests/test_hexmaze.cpp
    tests/test_square_maze.cpp
    )

add_executable(test_mazegen ${TEST_SOURCES} ${SOURCES} ${HEADERS})
target_include_directories(test_mazegen PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(test_mazegen
    PRIVATE
    GTest::GTest
    GTest::gmock_main
)

target_include_directories(test_mazegen PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_test(NAME test_mazegen COMMAND test_mazegen)

# ----------- End Unit Tests -----------
