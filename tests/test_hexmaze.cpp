#include "src/hexmaze.h"
#include <gtest/gtest.h>

TEST(HexMazeTest, MyTest) {
    HexMaze m(1, 1);

    EXPECT_NE(m.getOpenNode(), HexMaze::invalidNode());
}
