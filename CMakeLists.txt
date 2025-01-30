cmake_minimum_required(VERSION 3.26)

project(mazegen)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -pedantic -Werror)

set(SOURCES
    src/hexmaze.cpp
    src/svg_painter.cpp
    )

set(HEADERS
    src/gen_wilson.h
    src/hexmaze.h
    src/matrix.h
    src/svg_painter.h
    )

add_executable(mazegen src/main.cpp ${SOURCES} ${HEADERS})

target_include_directories(mazegen PRIVATE ${PROJECT_SOURCE_DIR}/include)

# ----------- Unit Tests -----------

enable_testing()

find_package(GTest REQUIRED)

set(TEST_SOURCES
    tests/test_gen_wilson.cpp
    tests/test_hexmaze.cpp
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
