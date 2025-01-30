#include "src/square_maze.h"

#include <gtest/gtest.h>

// It is not possible to get across the border of the maze
TEST(SquareMazeTest, BorderEdgesTest) {
    SquareMaze m(3, 3);

    SquareMaze::EdgeList edges;
    m.getOpenEdges({0, 0}, edges); EXPECT_EQ(edges, SquareMaze::EdgeList({1, 2}));
    m.getOpenEdges({0, 1}, edges); EXPECT_EQ(edges, SquareMaze::EdgeList({1, 2, 4}));
    m.getOpenEdges({0, 2}, edges); EXPECT_EQ(edges, SquareMaze::EdgeList({1, 4}));
    m.getOpenEdges({1, 0}, edges); EXPECT_EQ(edges, SquareMaze::EdgeList({1, 2, 3}));
    m.getOpenEdges({1, 1}, edges); EXPECT_EQ(edges, SquareMaze::EdgeList({1, 2, 3, 4}));
    m.getOpenEdges({1, 2}, edges); EXPECT_EQ(edges, SquareMaze::EdgeList({1, 3, 4}));
    m.getOpenEdges({2, 0}, edges); EXPECT_EQ(edges, SquareMaze::EdgeList({2, 3}));
    m.getOpenEdges({2, 1}, edges); EXPECT_EQ(edges, SquareMaze::EdgeList({2, 3, 4}));
    m.getOpenEdges({2, 2}, edges); EXPECT_EQ(edges, SquareMaze::EdgeList({3, 4}));
}

// getOpenNode returns an open node as long as there is one
TEST(SquareMazeTest, GetOpenNodeTest) {
    SquareMaze m(2, 1);

    const auto node1 = m.getOpenNode();
    EXPECT_NE(node1, SquareMaze::invalidNode());
    m.setNode(node1, ENode::Visited);
    const auto node2 = m.getOpenNode();
    EXPECT_NE(node2, SquareMaze::invalidNode());
    EXPECT_NE(node1, node2);
    m.setNode(node2, ENode::Visited);
    EXPECT_EQ(m.getOpenNode(), SquareMaze::invalidNode());
}

// getNode/setNode are using the same indexes under the hood
TEST(SquareMazeTest, GetSetNodeTest) {
    SquareMaze m(2, 2);

    const auto node = SquareMaze::NodeIndex{0, 0};
    EXPECT_EQ(m.getNode(node), ENode::Open);
    m.setNode(node, ENode::Visited);
    EXPECT_EQ(m.getNode(node), ENode::Visited);
    m.setNode(node, ENode::OnPath);
    EXPECT_EQ(m.getNode(node), ENode::OnPath);
    m.setNode(node, ENode::Open);
    EXPECT_EQ(m.getNode(node), ENode::Open);
}

using SetEdgeTestParam = SquareMaze::NodeIndex;

class SquareMazeSetEdgeTest : public ::testing::TestWithParam<SetEdgeTestParam> {};

// getOpenEdges/setEdge are using the same indexes under the hood
TEST_P(SquareMazeSetEdgeTest, UsesRightIndexes) {
    SquareMaze m(4, 4);

    const auto& node = GetParam();
    SquareMaze::EdgeList edges;
    m.getOpenEdges(node, edges);
    EXPECT_EQ(edges, SquareMaze::EdgeList({1, 2, 3, 4}));
    for (int edge = 1; edge <= 4; edge++) {
        // A visited edge is not open anymore
        m.setEdge(node, edge, EEdge::Visited);
        m.getOpenEdges(node, edges);
        EXPECT_EQ(edges.size(), 4 - edge);
        EXPECT_TRUE(std::find(edges.begin(), edges.end(), edge) == edges.end());
    }
}

INSTANTIATE_TEST_SUITE_P(ValidCases, SquareMazeSetEdgeTest, ::testing::Values(
    SetEdgeTestParam{1, 1},
    SetEdgeTestParam{1, 2},
    SetEdgeTestParam{2, 1},
    SetEdgeTestParam{2, 2}));

using NextNodeTestParam = std::tuple<SquareMaze::NodeIndex, SquareMaze::EdgeIndex, SquareMaze::NodeIndex>;

class SquareMazeNextNodeTest : public ::testing::TestWithParam<NextNodeTestParam> {};

// nextNode returns the correct node index along the given edge
TEST_P(SquareMazeNextNodeTest, ReturnsRightIndexes) {
    const auto& p = GetParam();
    EXPECT_EQ(SquareMaze::nextNode(std::get<0>(p), std::get<1>(p)), std::get<2>(p));
}

INSTANTIATE_TEST_SUITE_P(ValidCases, SquareMazeNextNodeTest, ::testing::Values(
    NextNodeTestParam{{0, 0}, 1, {1, 0}},
    NextNodeTestParam{{0, 0}, 2, {0, 1}},
    NextNodeTestParam{{0, 0}, 3, {-1, 0}},
    NextNodeTestParam{{0, 0}, 4, {0, -1}}));

INSTANTIATE_TEST_SUITE_P(InvalidCases, SquareMazeNextNodeTest, ::testing::Values(
    NextNodeTestParam{{0, 0}, 0, SquareMaze::invalidNode()},
    NextNodeTestParam{{0, 0}, 5, SquareMaze::invalidNode()}));

// TODO: DrawTest
