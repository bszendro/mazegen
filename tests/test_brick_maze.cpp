#include "src/brick_maze.h"

#include <gtest/gtest.h>

//----------------------------------------------------------------------------------------------------

using ConnectionTestParam = std::tuple<BrickMaze::NodeIndex, BrickMaze::EdgeIndex, BrickMaze::EdgeIndex>;

class BrickMazeConnectionTest : public ::testing::TestWithParam<ConnectionTestParam>
{
public:
    BrickMazeConnectionTest() : m(2, 2) {}

    BrickMaze m;
};

// Check that any edge matches its corresponding edge of the adjacent node
TEST_P(BrickMazeConnectionTest, EdgesAreConnected) {
    const auto& p = GetParam();
    const auto node = std::get<0>(p);
    const auto edge_out = std::get<1>(p);
    const auto edge_in = std::get<2>(p);
    m.setEdge(node, edge_out, EEdge::Visited);
    EXPECT_EQ(m.getEdge(BrickMaze::nextNode(node, edge_out), edge_in), EEdge::Visited);
    m.setEdge(node, edge_out, EEdge::Open);
}

INSTANTIATE_TEST_SUITE_P(AllCases, BrickMazeConnectionTest, ::testing::Values(
    ConnectionTestParam{{0, 0}, 1, 4},
    ConnectionTestParam{{0, 0}, 2, 5},
    ConnectionTestParam{{0, 0}, 3, 6},
    ConnectionTestParam{{0, 0}, 4, 1},
    ConnectionTestParam{{0, 0}, 5, 2},
    ConnectionTestParam{{0, 0}, 6, 3},

    ConnectionTestParam{{0, 1}, 1, 4},
    ConnectionTestParam{{0, 1}, 2, 5},
    ConnectionTestParam{{0, 1}, 3, 6},
    ConnectionTestParam{{0, 1}, 4, 1},
    ConnectionTestParam{{0, 1}, 5, 2},
    ConnectionTestParam{{0, 1}, 6, 3},

    ConnectionTestParam{{1, 0}, 1, 4},
    ConnectionTestParam{{1, 0}, 2, 5},
    ConnectionTestParam{{1, 0}, 3, 6},
    ConnectionTestParam{{1, 0}, 4, 1},
    ConnectionTestParam{{1, 0}, 5, 2},
    ConnectionTestParam{{1, 0}, 6, 3},

    ConnectionTestParam{{1, 1}, 1, 4},
    ConnectionTestParam{{1, 1}, 2, 5},
    ConnectionTestParam{{1, 1}, 3, 6},
    ConnectionTestParam{{1, 1}, 4, 1},
    ConnectionTestParam{{1, 1}, 5, 2},
    ConnectionTestParam{{1, 1}, 6, 3}));

//----------------------------------------------------------------------------------------------------

using NextNodeTestParam = std::tuple<BrickMaze::NodeIndex, BrickMaze::EdgeIndex, BrickMaze::NodeIndex>;

class BrickMazeNextNodeTest : public ::testing::TestWithParam<NextNodeTestParam> {};

// nextNode returns the correct node index along the given edge
TEST_P(BrickMazeNextNodeTest, ReturnsRightIndexes) {
    const auto& p = GetParam();
    EXPECT_EQ(BrickMaze::nextNode(std::get<0>(p), std::get<1>(p)), std::get<2>(p));
}

INSTANTIATE_TEST_SUITE_P(ValidCases, BrickMazeNextNodeTest, ::testing::Values(
    NextNodeTestParam{{0, 0}, 1, {1, -1}},
    NextNodeTestParam{{0, 0}, 2, {1, 0}},
    NextNodeTestParam{{0, 0}, 3, {0, 1}},
    NextNodeTestParam{{0, 0}, 4, {-1, 0}},
    NextNodeTestParam{{0, 0}, 5, {-1, -1}},
    NextNodeTestParam{{0, 0}, 6, {0, -1}},
    // Check both the next row and the next column to see if i and j are mixed up somewhere
    NextNodeTestParam{{0, 1}, 1, {1, 0}},
    NextNodeTestParam{{0, 1}, 2, {1, 1}},
    NextNodeTestParam{{0, 1}, 3, {0, 2}},
    NextNodeTestParam{{0, 1}, 4, {-1, 1}},
    NextNodeTestParam{{0, 1}, 5, {-1, 0}},
    NextNodeTestParam{{0, 1}, 6, {0, 0}},

    NextNodeTestParam{{1, 0}, 1, {2, 0}},
    NextNodeTestParam{{1, 0}, 2, {2, 1}},
    NextNodeTestParam{{1, 0}, 3, {1, 1}},
    NextNodeTestParam{{1, 0}, 4, {0, 1}},
    NextNodeTestParam{{1, 0}, 5, {0, 0}},
    NextNodeTestParam{{1, 0}, 6, {1, -1}},

    NextNodeTestParam{{1, 1}, 1, {2, 1}},
    NextNodeTestParam{{1, 1}, 2, {2, 2}},
    NextNodeTestParam{{1, 1}, 3, {1, 2}},
    NextNodeTestParam{{1, 1}, 4, {0, 2}},
    NextNodeTestParam{{1, 1}, 5, {0, 1}},
    NextNodeTestParam{{1, 1}, 6, {1, 0}}));

INSTANTIATE_TEST_SUITE_P(InvalidCases, BrickMazeNextNodeTest, ::testing::Values(
    NextNodeTestParam{{0, 0}, 0, BrickMaze::invalidNode()},
    NextNodeTestParam{{0, 0}, 7, BrickMaze::invalidNode()}));

// TODO: add the rest like with the other types
