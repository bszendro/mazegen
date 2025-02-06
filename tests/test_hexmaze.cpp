#include "src/hexmaze.h"

#include <gtest/gtest.h>

// All nodes are open initially
TEST(HexMazeTest, AllNodesOpen) {
    HexMaze m(3, 3);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_EQ(m.getNode({i, j}), ENode::Open);
        }
    }
}

// It is not possible to get across the border of the maze
TEST(HexMazeTest, BorderEdgesTest) {
    HexMaze m(3, 3);

    HexMaze::EdgeList edges;
    m.getOpenEdges({0, 0}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({2, 3}));
    m.getOpenEdges({0, 1}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({1, 2, 3, 4, 6}));
    m.getOpenEdges({0, 2}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({1, 2}));
    m.getOpenEdges({1, 0}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({2, 3, 4, 5}));
    m.getOpenEdges({1, 1}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({1, 2, 3, 4, 5, 6}));
    m.getOpenEdges({1, 2}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({1, 2, 5, 6}));
    m.getOpenEdges({2, 0}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({3, 4, 5}));
    m.getOpenEdges({2, 1}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({4, 5, 6}));
    m.getOpenEdges({2, 2}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({1, 5, 6}));
}

// getOpenNode returns an open node as long as there is one
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

// getNode/setNode are using the same indexes under the hood
TEST(HexMazeTest, GetSetNodeTest) {
    HexMaze m(2, 2);

    const auto node = HexMaze::NodeIndex{0, 0};
    EXPECT_EQ(m.getNode(node), ENode::Open);
    m.setNode(node, ENode::Visited);
    EXPECT_EQ(m.getNode(node), ENode::Visited);
    m.setNode(node, ENode::OnPath);
    EXPECT_EQ(m.getNode(node), ENode::OnPath);
    m.setNode(node, ENode::Open);
    EXPECT_EQ(m.getNode(node), ENode::Open);
}

//----------------------------------------------------------------------------------------------------

using SetEdgeTestParam = HexMaze::NodeIndex;

class HexMazeSetEdgeTest : public ::testing::TestWithParam<SetEdgeTestParam> {};

// getOpenEdges/setEdge are using the same indexes under the hood
TEST_P(HexMazeSetEdgeTest, UsesRightIndexes) {
    HexMaze m(4, 4);

    const auto& node = GetParam();
    HexMaze::EdgeList edges;
    m.getOpenEdges(node, edges);
    EXPECT_EQ(edges, HexMaze::EdgeList({1, 2, 3, 4, 5, 6}));
    for (int edge = 1; edge <= 6; edge++) {
        // A visited edge is not open anymore
        m.setEdge(node, edge, EEdge::Visited);
        m.getOpenEdges(node, edges);
        EXPECT_EQ(edges.size(), 6 - edge);
        EXPECT_TRUE(std::find(edges.begin(), edges.end(), edge) == edges.end());
    }
}

INSTANTIATE_TEST_SUITE_P(ValidCases, HexMazeSetEdgeTest, ::testing::Values(
    SetEdgeTestParam{1, 1},
    SetEdgeTestParam{1, 2},
    SetEdgeTestParam{2, 1},
    SetEdgeTestParam{2, 2}));

//----------------------------------------------------------------------------------------------------

struct ConnectionTestParam
{
    HexMaze::NodeIndex node;
    HexMaze::EdgeIndex edge_out;
    HexMaze::EdgeIndex edge_in;
};

class HexMazeConnectionTest : public ::testing::TestWithParam<ConnectionTestParam>
{
public:
    HexMazeConnectionTest() : m(2, 2) {}

    HexMaze m;
};

// Check that any edge matches its corresponding edge of the adjacent node
TEST_P(HexMazeConnectionTest, EdgesAreConnected) {
    const auto& p = GetParam();
    m.setEdge(p.node, p.edge_out, EEdge::Visited);
    EXPECT_EQ(m.getEdge(HexMaze::nextNode(p.node, p.edge_out), p.edge_in), EEdge::Visited);
    m.setEdge(p.node, p.edge_out, EEdge::Open);
}

INSTANTIATE_TEST_SUITE_P(AllCases, HexMazeConnectionTest, ::testing::Values(
    ConnectionTestParam{{0, 0}, 1, 4},
    ConnectionTestParam{{0, 0}, 2, 5},
    ConnectionTestParam{{0, 0}, 3, 6},
    ConnectionTestParam{{0, 0}, 4, 1},
    ConnectionTestParam{{0, 0}, 5, 2},
    ConnectionTestParam{{0, 0}, 6, 3},
    // Check both the next row and the next column to see if i and j are
    // mixed up in the edge index functions
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

struct NextNodeTestParam
{
    HexMaze::NodeIndex node;
    HexMaze::EdgeIndex edge;
    HexMaze::NodeIndex result;
};

class HexMazeNextNodeTest : public ::testing::TestWithParam<NextNodeTestParam> {};

// nextNode returns the correct node index along the given edge
TEST_P(HexMazeNextNodeTest, ReturnsRightIndexes) {
    const auto& p = GetParam();
    EXPECT_EQ(HexMaze::nextNode(p.node, p.edge), p.result);
}

INSTANTIATE_TEST_SUITE_P(ValidCases, HexMazeNextNodeTest, ::testing::Values(
    NextNodeTestParam{{0, 0}, 1, {0, -1}},
    NextNodeTestParam{{0, 0}, 2, {1, 0}},
    NextNodeTestParam{{0, 0}, 3, {0, 1}},
    NextNodeTestParam{{0, 0}, 4, {-1, 1}},
    NextNodeTestParam{{0, 0}, 5, {-1, 0}},
    NextNodeTestParam{{0, 0}, 6, {-1, -1}},
    // Check both the next row and the next column to see if i and j are
    // mixed up in nextNode
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

INSTANTIATE_TEST_SUITE_P(InvalidCases, HexMazeNextNodeTest, ::testing::Values(
    NextNodeTestParam{{0, 0}, 0, HexMaze::invalidNode()},
    NextNodeTestParam{{0, 0}, 7, HexMaze::invalidNode()}));

//----------------------------------------------------------------------------------------------------

// It is not possible to enter into invalid regions
TEST(HexMazeTest, InvalidRegionTest) {
    HexMaze m(3, 3);

    HexMaze::EdgeList edges;
    m.getOpenEdges({1, 1}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({1, 2, 3, 4, 5, 6}));
    m.invalidateRegion({2, 0}, {2, 0});
    m.getOpenEdges({1, 1}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({2, 3, 4, 5, 6}));
    m.invalidateRegion({2, 1}, {2, 1});
    m.getOpenEdges({1, 1}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({3, 4, 5, 6}));
    m.invalidateRegion({2, 2}, {2, 2});
    m.getOpenEdges({1, 1}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({4, 5, 6}));
    m.invalidateRegion({1, 2}, {1, 2});
    m.getOpenEdges({1, 1}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({5, 6}));
    m.invalidateRegion({0, 1}, {0, 1});
    m.getOpenEdges({1, 1}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({6}));
    m.invalidateRegion({1, 0}, {1, 0});
    m.getOpenEdges({1, 1}, edges); EXPECT_EQ(edges, HexMaze::EdgeList({}));
}

// TODO: Test drawing
