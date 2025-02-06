#include "src/square_maze.h"

#include <gtest/gtest.h>

// All nodes are open initially
TEST(SquareMazeTest, AllNodesOpen) {
    SquareMaze m(3, 3);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_EQ(m.getNode({i, j}), ENode::Open);
        }
    }
}

// It is not possible to get across the border of the maze
TEST(SquareMazeTest, BorderEdges) {
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
TEST(SquareMazeTest, GetOpenNode) {
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
TEST(SquareMazeTest, GetSetNode) {
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

//----------------------------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------------------------

struct ConnectionTestParam
{
    SquareMaze::NodeIndex node;
    SquareMaze::EdgeIndex edge_out;
    SquareMaze::EdgeIndex edge_in;
};

class SquareMazeConnectionTest : public ::testing::TestWithParam<ConnectionTestParam>
{
public:
    SquareMazeConnectionTest() : m(2, 2) {}

    SquareMaze m;
};

// Check that any edge matches its corresponding edge of the adjacent node
TEST_P(SquareMazeConnectionTest, EdgesAreConnected) {
    const auto& p = GetParam();
    m.setEdge(p.node, p.edge_out, EEdge::Visited);
    EXPECT_EQ(m.getEdge(SquareMaze::nextNode(p.node, p.edge_out), p.edge_in), EEdge::Visited);
    m.setEdge(p.node, p.edge_out, EEdge::Open);
}

INSTANTIATE_TEST_SUITE_P(AllCases, SquareMazeConnectionTest, ::testing::Values(
    ConnectionTestParam{{0, 0}, 1, 3},
    ConnectionTestParam{{0, 0}, 2, 4},
    ConnectionTestParam{{0, 0}, 3, 1},
    ConnectionTestParam{{0, 0}, 4, 2}));

//----------------------------------------------------------------------------------------------------

struct NextNodeTestParam
{
    SquareMaze::NodeIndex node;
    SquareMaze::EdgeIndex edge;
    SquareMaze::NodeIndex result;
};

class SquareMazeNextNodeTest : public ::testing::TestWithParam<NextNodeTestParam> {};

// nextNode returns the correct node index along the given edge
TEST_P(SquareMazeNextNodeTest, ReturnsRightIndexes) {
    const auto& p = GetParam();
    EXPECT_EQ(SquareMaze::nextNode(p.node, p.edge), p.result);
}

INSTANTIATE_TEST_SUITE_P(ValidCases, SquareMazeNextNodeTest, ::testing::Values(
    NextNodeTestParam{{0, 0}, 1, {1, 0}},
    NextNodeTestParam{{0, 0}, 2, {0, 1}},
    NextNodeTestParam{{0, 0}, 3, {-1, 0}},
    NextNodeTestParam{{0, 0}, 4, {0, -1}}));

INSTANTIATE_TEST_SUITE_P(InvalidCases, SquareMazeNextNodeTest, ::testing::Values(
    NextNodeTestParam{{0, 0}, 0, SquareMaze::invalidNode()},
    NextNodeTestParam{{0, 0}, 5, SquareMaze::invalidNode()}));

//----------------------------------------------------------------------------------------------------

// TODO: Test drawing
