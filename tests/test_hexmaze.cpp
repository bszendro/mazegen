#include "src/hexmaze.h"
#include <gtest/gtest.h>

using SetEdgeTestParam = std::tuple<HexMaze::NodeIndex, HexMaze::EdgeIndex, int, int>;

class SetEdgeTest : public ::testing::TestWithParam<SetEdgeTestParam> {};

TEST_P(SetEdgeTest, ReturnsRightIndexes) {
    HexMaze m(2, 2);

    const auto& p = GetParam();
    const auto i = std::get<2>(p);
    const auto j = std::get<3>(p);
    EXPECT_EQ(m.edges_[i][j], 0);
    m.setEdge(std::get<0>(p), std::get<1>(p), EEdge::Visited);
    EXPECT_NE(m.edges_[i][j], 0);
}

INSTANTIATE_TEST_SUITE_P(ValidCases, SetEdgeTest, ::testing::Values(
    SetEdgeTestParam{{0, 0}, 1, 0, 3}));

TEST(HexMazeTest, GetOpenNodeTest) {
    HexMaze m(2, 1);

    const auto node1 = m.getOpenNode();
    EXPECT_NE(node1, HexMaze::invalidNode());
    m.setNode(node1, ENode::Visited);
    const auto node2 = m.getOpenNode();
    EXPECT_NE(node2, HexMaze::invalidNode());
    EXPECT_NE(node1, node2);
    m.setNode(node2, ENode::Visited);
    EXPECT_EQ(m.getOpenNode(), HexMaze::invalidNode());
}

using NextNodeTestParam = std::tuple<HexMaze::NodeIndex, HexMaze::EdgeIndex, HexMaze::NodeIndex>;

class NextNodeTest : public ::testing::TestWithParam<NextNodeTestParam> {};

TEST_P(NextNodeTest, ReturnsRightIndexes) {
    const auto& p = GetParam();
    EXPECT_EQ(HexMaze::nextNode(std::get<0>(p), std::get<1>(p)), std::get<2>(p));
}

INSTANTIATE_TEST_SUITE_P(ValidCases, NextNodeTest, ::testing::Values(
    NextNodeTestParam{{0, 0}, 1, {0, -1}},
    NextNodeTestParam{{0, 0}, 2, {1, 0}},
    NextNodeTestParam{{0, 0}, 3, {0, 1}},
    NextNodeTestParam{{0, 0}, 4, {-1, 1}},
    NextNodeTestParam{{0, 0}, 5, {-1, 0}},
    NextNodeTestParam{{0, 0}, 6, {-1, -1}},

    NextNodeTestParam{{0, 1}, 1, {1, 0}},
    NextNodeTestParam{{0, 1}, 2, {1, 1}},
    NextNodeTestParam{{0, 1}, 3, {1, 2}},
    NextNodeTestParam{{0, 1}, 4, {0, 2}},
    NextNodeTestParam{{0, 1}, 5, {-1, 1}},
    NextNodeTestParam{{0, 1}, 6, {0, 0}},

    NextNodeTestParam{{1, 0}, 1, {1, -1}},
    NextNodeTestParam{{1, 0}, 2, {2, 0}},
    NextNodeTestParam{{1, 0}, 3, {1, 1}},
    NextNodeTestParam{{1, 0}, 4, {0, 1}},
    NextNodeTestParam{{1, 0}, 5, {0, 0}},
    NextNodeTestParam{{1, 0}, 6, {0, -1}},

    NextNodeTestParam{{1, 1}, 1, {2, 0}},
    NextNodeTestParam{{1, 1}, 2, {2, 1}},
    NextNodeTestParam{{1, 1}, 3, {2, 2}},
    NextNodeTestParam{{1, 1}, 4, {1, 2}},
    NextNodeTestParam{{1, 1}, 5, {0, 1}},
    NextNodeTestParam{{1, 1}, 6, {1, 0}}));

INSTANTIATE_TEST_SUITE_P(InvalidCases, NextNodeTest, ::testing::Values(
    NextNodeTestParam{{0, 0}, 0, HexMaze::invalidNode()},
    NextNodeTestParam{{0, 0}, 7, HexMaze::invalidNode()},
    NextNodeTestParam{{0, 1}, 0, HexMaze::invalidNode()},
    NextNodeTestParam{{0, 1}, 7, HexMaze::invalidNode()},
    NextNodeTestParam{{1, 0}, 0, HexMaze::invalidNode()},
    NextNodeTestParam{{1, 0}, 7, HexMaze::invalidNode()},
    NextNodeTestParam{{1, 1}, 0, HexMaze::invalidNode()},
    NextNodeTestParam{{1, 1}, 7, HexMaze::invalidNode()}));
