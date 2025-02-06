#include "src/gen_wilson.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::AtMost;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::SetArgReferee;

// CreateMazeWilson is tested with MockMaze. Which edge will be selected during traversal is determined
// by the return value of getOpenEdges that can change over the course of a test. Thus random selection
// of the edges is ignored in these tests. (A separate test later verifies that edges are selected randomly.)
class MockMaze {
public:
    using NodeIndex = int;
    using EdgeIndex = int;

    MOCK_METHOD(ENode, getNode, (NodeIndex node), (const));
    MOCK_METHOD(void, setNode, (NodeIndex node, ENode val));

    MOCK_METHOD(void, getOpenEdges, (NodeIndex node, std::vector<EdgeIndex>& edges), (const));
    MOCK_METHOD(void, setEdge, (NodeIndex node, EdgeIndex edge, EEdge val));

    MOCK_METHOD(NodeIndex, getOpenNode, (), (const));
    MOCK_METHOD(NodeIndex, nextNode, (NodeIndex node, EdgeIndex edge), (const));
    static NodeIndex invalidNode() { return -1; }
};

using MazeGen = CreateMazeWilson<MockMaze>;

// No open node at the first step is an error
TEST(GenWilsonTest, NoFirstOpenNode) {
    MockMaze m;
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(MockMaze::invalidNode()));

    MazeGen maze_gen(0);
    EXPECT_EQ(maze_gen.createMaze(m), ECreateMazeResult::ErrNoFirstOpenNode);
}

// First open node will be marked as visited
TEST(GenWilsonTest, FirstOpenNodeAdded) {
    MockMaze m;

    const auto n0 = 0;

    InSequence s;
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(n0));
    EXPECT_CALL(m, setNode(n0, ENode::Visited));
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(MockMaze::invalidNode()));

    MazeGen maze_gen(0);
    EXPECT_EQ(maze_gen.createMaze(m), ECreateMazeResult::Ok);
}

// One step from start node leads to a visited node
TEST(GenWilsonTest, OneStepFromStart) {
    MockMaze m;

    const auto n0 = 0, n1 = 1;
    const auto e10 = 10;

    InSequence s;
    // First node is added automatically
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(n0));
    EXPECT_CALL(m, setNode(n0, ENode::Visited));

    // Second open node will start a path
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(n1));
    EXPECT_CALL(m, setNode(n1, ENode::OnPath));
    EXPECT_CALL(m, getOpenEdges(n1, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e10})));
    EXPECT_CALL(m, nextNode(n1, e10)).WillOnce(Return(n0));
    EXPECT_CALL(m, setEdge(n1, e10, EEdge::OnPath));
    // Found a visited node, the path is finished
    EXPECT_CALL(m, getNode(n0)).WillOnce(Return(ENode::Visited));

    // Mark all nodes and edges of the path as visited
    EXPECT_CALL(m, setNode(n1, ENode::Visited));
    EXPECT_CALL(m, setEdge(n1, e10, EEdge::Visited));
    // The last node of the path may be set to visited again but is not required
    EXPECT_CALL(m, setNode(n0, ENode::Visited)).Times(AtMost(1));

    // There are no more open nodes
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(MockMaze::invalidNode()));

    MazeGen maze_gen(0);
    EXPECT_EQ(maze_gen.createMaze(m), ECreateMazeResult::Ok);
}

// Two steps from start node lead to a visited node
TEST(GenWilsonTest, TwoStepsFromStart) {
    MockMaze m;

    const auto n0 = 0, n1 = 1, n2 = 2;
    const auto e12 = 12, e20 = 20;

    InSequence s;
    // First node is added automatically
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(n0));
    EXPECT_CALL(m, setNode(n0, ENode::Visited));

    // Second open node will start a path
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(n1));
    EXPECT_CALL(m, setNode(n1, ENode::OnPath));
    EXPECT_CALL(m, getOpenEdges(n1, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e12})));
    EXPECT_CALL(m, nextNode(n1, e12)).WillOnce(Return(n2));
    EXPECT_CALL(m, setEdge(n1, e12, EEdge::OnPath));

    EXPECT_CALL(m, getNode(n2)).WillOnce(Return(ENode::Open));
    EXPECT_CALL(m, setNode(n2, ENode::OnPath));
    EXPECT_CALL(m, getOpenEdges(n2, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e20})));
    EXPECT_CALL(m, nextNode(n2, e20)).WillOnce(Return(n0));
    EXPECT_CALL(m, setEdge(n2, e20, EEdge::OnPath));

    // Found a visited node, the path is finished
    EXPECT_CALL(m, getNode(n0)).WillOnce(Return(ENode::Visited));

    // Mark all nodes and edges of the path as visited
    EXPECT_CALL(m, setNode(n1, ENode::Visited));
    EXPECT_CALL(m, setEdge(n1, e12, EEdge::Visited));
    EXPECT_CALL(m, setNode(n2, ENode::Visited));
    EXPECT_CALL(m, setEdge(n2, e20, EEdge::Visited));
    // The last node of the path may be set to visited again but is not required
    EXPECT_CALL(m, setNode(n0, ENode::Visited)).Times(AtMost(1));

    // There are no more open nodes
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(MockMaze::invalidNode()));

    MazeGen maze_gen(0);
    EXPECT_EQ(maze_gen.createMaze(m), ECreateMazeResult::Ok);
}

// When a loop ends with the start node
TEST(GenWilsonTest, LoopOnStart) {
    MockMaze m;

    const auto n0 = 0, n1 = 1, n2 = 2;
    const auto e12 = 12, e21 = 21, e10 = 10;

    InSequence s;
    // First node is added automatically
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(n0));
    EXPECT_CALL(m, setNode(n0, ENode::Visited));

    // Second open node will start a path
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(n1));
    EXPECT_CALL(m, setNode(n1, ENode::OnPath));
    EXPECT_CALL(m, getOpenEdges(n1, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e12})));
    EXPECT_CALL(m, nextNode(n1, e12)).WillOnce(Return(n2));
    EXPECT_CALL(m, setEdge(n1, e12, EEdge::OnPath));

    EXPECT_CALL(m, getNode(n2)).WillOnce(Return(ENode::Open));
    EXPECT_CALL(m, setNode(n2, ENode::OnPath));
    EXPECT_CALL(m, getOpenEdges(n2, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e21})));
    // e21 leads back to n1
    EXPECT_CALL(m, nextNode(n2, e21)).WillOnce(Return(n1));
    EXPECT_CALL(m, setEdge(n2, e21, EEdge::OnPath));

    // which is already on the path, found a loop
    EXPECT_CALL(m, getNode(n1)).WillOnce(Return(ENode::OnPath));
    // Erase the loop
    EXPECT_CALL(m, setEdge(n2, e21, EEdge::Open));
    EXPECT_CALL(m, setNode(n2, ENode::Open));
    EXPECT_CALL(m, setEdge(n1, e12, EEdge::Open));
    // now go to n0
    EXPECT_CALL(m, getOpenEdges(n1, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e10})));
    EXPECT_CALL(m, nextNode(n1, e10)).WillOnce(Return(n0));
    EXPECT_CALL(m, setEdge(n1, e10, EEdge::OnPath));

    // which is already visited so the path is finished
    EXPECT_CALL(m, getNode(n0)).WillOnce(Return(ENode::Visited));

    // Mark all nodes and edges of the path as visited
    EXPECT_CALL(m, setNode(n1, ENode::Visited));
    EXPECT_CALL(m, setEdge(n1, e10, EEdge::Visited));
    // The last node of the path may be set to visited again but is not required
    EXPECT_CALL(m, setNode(n0, ENode::Visited)).Times(AtMost(1));

    // There are no more open nodes
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(MockMaze::invalidNode()));

    MazeGen maze_gen(0);
    EXPECT_EQ(maze_gen.createMaze(m), ECreateMazeResult::Ok);
}

// When a loop ends with a node different from the start node
TEST(GenWilsonTest, LoopOnOtherNode) {
    MockMaze m;

    const auto n0 = 0, n1 = 1, n2 = 2, n3 = 3, n4 = 4;
    const auto e12 = 12, e23 = 23, e34 = 34, e42 = 42, e20 = 20;

    InSequence s;
    // First node is added automatically
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(n0));
    EXPECT_CALL(m, setNode(n0, ENode::Visited));

    // Second open node will start a path
    // n1 -> n2
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(n1));
    EXPECT_CALL(m, setNode(n1, ENode::OnPath));
    EXPECT_CALL(m, getOpenEdges(n1, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e12})));
    EXPECT_CALL(m, nextNode(n1, e12)).WillOnce(Return(n2));
    EXPECT_CALL(m, setEdge(n1, e12, EEdge::OnPath));
    // n2 -> n3
    EXPECT_CALL(m, getNode(n2)).WillOnce(Return(ENode::Open));
    EXPECT_CALL(m, setNode(n2, ENode::OnPath));
    EXPECT_CALL(m, getOpenEdges(n2, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e23})));
    EXPECT_CALL(m, nextNode(n2, e23)).WillOnce(Return(n3));
    EXPECT_CALL(m, setEdge(n2, e23, EEdge::OnPath));
    // n3 -> n4
    EXPECT_CALL(m, getNode(n3)).WillOnce(Return(ENode::Open));
    EXPECT_CALL(m, setNode(n3, ENode::OnPath));
    EXPECT_CALL(m, getOpenEdges(n3, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e34})));
    EXPECT_CALL(m, nextNode(n3, e34)).WillOnce(Return(n4));
    EXPECT_CALL(m, setEdge(n3, e34, EEdge::OnPath));
    // n4 -> n2
    EXPECT_CALL(m, getNode(n4)).WillOnce(Return(ENode::Open));
    EXPECT_CALL(m, setNode(n4, ENode::OnPath));
    EXPECT_CALL(m, getOpenEdges(n4, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e42})));
    EXPECT_CALL(m, nextNode(n4, e42)).WillOnce(Return(n2));
    EXPECT_CALL(m, setEdge(n4, e42, EEdge::OnPath));

    // n2 is already on the path, found a loop
    EXPECT_CALL(m, getNode(n2)).WillOnce(Return(ENode::OnPath));
    // Erase the loop
    EXPECT_CALL(m, setEdge(n4, e42, EEdge::Open));
    EXPECT_CALL(m, setNode(n4, ENode::Open));
    EXPECT_CALL(m, setEdge(n3, e34, EEdge::Open));
    EXPECT_CALL(m, setNode(n3, ENode::Open));
    EXPECT_CALL(m, setEdge(n2, e23, EEdge::Open));

    // Now go to n0
    EXPECT_CALL(m, getOpenEdges(n2, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e20})));
    EXPECT_CALL(m, nextNode(n2, e20)).WillOnce(Return(n0));
    EXPECT_CALL(m, setEdge(n2, e20, EEdge::OnPath));

    // which is already visited so the path is finished
    EXPECT_CALL(m, getNode(n0)).WillOnce(Return(ENode::Visited));

    // Mark all nodes and edges of the path as visited
    EXPECT_CALL(m, setNode(n1, ENode::Visited));
    EXPECT_CALL(m, setEdge(n1, e12, EEdge::Visited));
    EXPECT_CALL(m, setNode(n2, ENode::Visited));
    EXPECT_CALL(m, setEdge(n2, e20, EEdge::Visited));
    // The last node of the path may be set to visited again but is not required
    EXPECT_CALL(m, setNode(n0, ENode::Visited)).Times(AtMost(1));

    // There are still open nodes (n3 and n4) but pretend there are none to stop the algorithm
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(MockMaze::invalidNode()));

    MazeGen maze_gen(0);
    EXPECT_EQ(maze_gen.createMaze(m), ECreateMazeResult::Ok);
}

// Self-loop on the start node
TEST(GenWilsonTest, SelfLoopOnStartNode) {
    MockMaze m;

    const auto n0 = 0, n1 = 1;
    const auto e11 = 11, e10 = 10;

    InSequence s;
    // First node is added automatically
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(n0));
    EXPECT_CALL(m, setNode(n0, ENode::Visited));

    // Second open node will start a path
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(n1));
    EXPECT_CALL(m, setNode(n1, ENode::OnPath));
    EXPECT_CALL(m, getOpenEdges(n1, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e11})));
    EXPECT_CALL(m, nextNode(n1, e11)).WillOnce(Return(n1));
    EXPECT_CALL(m, setEdge(n1, e11, EEdge::OnPath));

    EXPECT_CALL(m, getNode(n1)).WillOnce(Return(ENode::OnPath));
    // Erase the loop
    EXPECT_CALL(m, setEdge(n1, e11, EEdge::Open));
    // Now go to n0
    EXPECT_CALL(m, getOpenEdges(n1, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e10})));
    EXPECT_CALL(m, nextNode(n1, e10)).WillOnce(Return(n0));
    EXPECT_CALL(m, setEdge(n1, e10, EEdge::OnPath));

    // which is already visited so the path is finished
    EXPECT_CALL(m, getNode(n0)).WillOnce(Return(ENode::Visited));

    // Mark all nodes and edges of the path as visited
    EXPECT_CALL(m, setNode(n1, ENode::Visited));
    EXPECT_CALL(m, setEdge(n1, e10, EEdge::Visited));
    // The last node of the path may be set to visited again but is not required
    EXPECT_CALL(m, setNode(n0, ENode::Visited)).Times(AtMost(1));

    // There are no more open nodes
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(MockMaze::invalidNode()));

    MazeGen maze_gen(0);
    EXPECT_EQ(maze_gen.createMaze(m), ECreateMazeResult::Ok);
}

// Self-loop on a node different from the start node
TEST(GenWilsonTest, SelfLoopOnOtherNode) {
    MockMaze m;

    const auto n0 = 0, n1 = 1, n2 = 2;
    const auto e12 = 12, e22 = 22, e20 = 20;

    InSequence s;
    // First node is added automatically
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(n0));
    EXPECT_CALL(m, setNode(n0, ENode::Visited));

    // Second open node will start a path
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(n1));
    EXPECT_CALL(m, setNode(n1, ENode::OnPath));
    EXPECT_CALL(m, getOpenEdges(n1, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e12})));
    EXPECT_CALL(m, nextNode(n1, e12)).WillOnce(Return(n2));
    EXPECT_CALL(m, setEdge(n1, e12, EEdge::OnPath));

    EXPECT_CALL(m, getNode(n2)).WillOnce(Return(ENode::Open));
    EXPECT_CALL(m, setNode(n2, ENode::OnPath));
    EXPECT_CALL(m, getOpenEdges(n2, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e22})));
    EXPECT_CALL(m, nextNode(n2, e22)).WillOnce(Return(n2));
    EXPECT_CALL(m, setEdge(n2, e22, EEdge::OnPath));

    EXPECT_CALL(m, getNode(n2)).WillOnce(Return(ENode::OnPath));
    // Erase the loop
    EXPECT_CALL(m, setEdge(n2, e22, EEdge::Open));
    // Find another edge
    EXPECT_CALL(m, getOpenEdges(n2, _)).WillOnce(SetArgReferee<1>(std::vector<int>({e20})));
    EXPECT_CALL(m, nextNode(n2, e20)).WillOnce(Return(n0));
    EXPECT_CALL(m, setEdge(n2, e20, EEdge::OnPath));

    // The path is finished
    EXPECT_CALL(m, getNode(n0)).WillOnce(Return(ENode::Visited));

    // Mark all nodes and edges of the path as visited
    EXPECT_CALL(m, setNode(n1, ENode::Visited));
    EXPECT_CALL(m, setEdge(n1, e12, EEdge::Visited));
    EXPECT_CALL(m, setNode(n2, ENode::Visited));
    EXPECT_CALL(m, setEdge(n2, e20, EEdge::Visited));
    // The last node of the path may be set to visited again but is not required
    EXPECT_CALL(m, setNode(n0, ENode::Visited)).Times(AtMost(1));

    // There are no more open nodes
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(MockMaze::invalidNode()));

    MazeGen maze_gen(0);
    EXPECT_EQ(maze_gen.createMaze(m), ECreateMazeResult::Ok);
}

//----------------------------------------------------------------------------------------------------

using GenWilsonRandomTestParam = std::tuple<int, int>;

class GenWilsonRandomTest : public ::testing::TestWithParam<GenWilsonRandomTestParam> {};

// When `random_seed` is used to seed the generator, `edge` will be selected
TEST_P(GenWilsonRandomTest, RandomEdgeSelection) {
    MockMaze m;

    const auto n0 = 0, n1 = 1;
    const auto& params = GetParam();
    const auto random_seed = std::get<0>(params);
    const auto edge = std::get<1>(params);

    InSequence s;
    // First node is added automatically
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(n0));
    EXPECT_CALL(m, setNode(n0, ENode::Visited));

    // Second open node will start a path
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(n1));
    EXPECT_CALL(m, setNode(n1, ENode::OnPath));
    EXPECT_CALL(m, getOpenEdges(n1, _)).WillOnce(SetArgReferee<1>(std::vector<int>({1, 2, 3, 4, 5, 6})));

    EXPECT_CALL(m, nextNode(n1, edge)).WillOnce(Return(n0));
    EXPECT_CALL(m, setEdge(n1, edge, EEdge::OnPath));

    // The path is finished
    EXPECT_CALL(m, getNode(n0)).WillOnce(Return(ENode::Visited));

    // Mark all nodes and edges of the path as visited
    EXPECT_CALL(m, setNode(n1, ENode::Visited));
    EXPECT_CALL(m, setEdge(n1, edge, EEdge::Visited));
    // The last node of the path may be set to visited again but is not required
    EXPECT_CALL(m, setNode(n0, ENode::Visited)).Times(AtMost(1));

    // There are no more open nodes
    EXPECT_CALL(m, getOpenNode()).WillOnce(Return(MockMaze::invalidNode()));

    MazeGen maze_gen(random_seed);
    EXPECT_EQ(maze_gen.createMaze(m), ECreateMazeResult::Ok);
}

// Edges are selected randomly (based on the seed of the generator)
INSTANTIATE_TEST_SUITE_P(, GenWilsonRandomTest, ::testing::Values(
    GenWilsonRandomTestParam{0, 5},
    GenWilsonRandomTestParam{1, 6},
    GenWilsonRandomTestParam{2, 1},
    GenWilsonRandomTestParam{3, 3}));
