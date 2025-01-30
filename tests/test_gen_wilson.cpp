#include "src/gen_wilson.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::SetArgReferee;

// TODO: Do we need this mock? How about a fake instead? What do you want to test?
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

// No open node at the first step is an error
TEST(GenWilsonTest, NoFirstOpenNode) {
    MockMaze m;
    EXPECT_CALL(m, getOpenNode())
        .WillOnce(Return(MockMaze::invalidNode()));

    CreateMazeWilson<MockMaze> maze_gen(0);
    EXPECT_EQ(maze_gen.createMaze(m), CreateMazeWilson<MockMaze>::ECreateResult::ErrNoFirstOpenNode);
}

// First open node will be added to the maze
TEST(GenWilsonTest, FirstOpenNodeAdded) {
    MockMaze m;
    {
        InSequence s;
        EXPECT_CALL(m, getOpenNode())
            .WillOnce(Return(123));
        EXPECT_CALL(m, setNode(123, ENode::Visited));
        EXPECT_CALL(m, getOpenNode())
            .WillOnce(Return(MockMaze::invalidNode()));
    }

    CreateMazeWilson<MockMaze> maze_gen(0);
    EXPECT_EQ(maze_gen.createMaze(m), CreateMazeWilson<MockMaze>::ECreateResult::Ok);
}

// One step from start node leads to a visited node
TEST(GenWilsonTest, OneStepFromStart) {
    MockMaze m;
    {
        InSequence s;

        const auto first_node = 123;
        const auto start_node = 456;
        const auto edge = 42;

        // First node is added automatically
        EXPECT_CALL(m, getOpenNode())
            .WillOnce(Return(first_node));
        EXPECT_CALL(m, setNode(first_node, ENode::Visited));

        // Second open node will start a path
        EXPECT_CALL(m, getOpenNode())
            .WillOnce(Return(start_node));
        EXPECT_CALL(m, setNode(start_node, ENode::OnPath));
        EXPECT_CALL(m, getOpenEdges(start_node, _))
            .WillOnce(SetArgReferee<1>(std::vector<int>({edge})));
        EXPECT_CALL(m, nextNode(start_node, edge))
            .WillOnce(Return(first_node));
        EXPECT_CALL(m, setEdge(start_node, edge, EEdge::OnPath));
        // The path reached an already visited node, it can be added now to the maze
        EXPECT_CALL(m, getNode(first_node))
            .WillOnce(Return(ENode::Visited));

        // Adding the path to the maze
        EXPECT_CALL(m, setNode(start_node, ENode::Visited));
        EXPECT_CALL(m, setEdge(start_node, edge, EEdge::Visited));
        EXPECT_CALL(m, setNode(first_node, ENode::Visited));

        // There are no more open nodes
        EXPECT_CALL(m, getOpenNode())
            .WillOnce(Return(MockMaze::invalidNode()));
    }

    CreateMazeWilson<MockMaze> maze_gen(0);
    EXPECT_EQ(maze_gen.createMaze(m), CreateMazeWilson<MockMaze>::ECreateResult::Ok);
}

// class SingleRowMaze
// {
// public:
//     using NodeIndex = int;
//     using EdgeIndex = int;

//     explicit SingleRowMaze(int length) :

//     ENode getNode(NodeIndex node) const;
//     void setNode(NodeIndex node, ENode val);

//     void getOpenEdges(NodeIndex node, std::vector<EdgeIndex>& edges) const;
//     void setEdge(NodeIndex node, EdgeIndex edge, EEdge val);

//     NodeIndex getOpenNode() const;
//     static NodeIndex nextNode(NodeIndex node, EdgeIndex edge);
//     static NodeIndex invalidNode();

// private:
//     std::vector<int> nodes_;
//     std::vector<int> edges_;
// };

// // One step from start node leads to a visited node
// TEST(GenWilsonTest, OneStepFromStart2) {

//     CreateMazeWilson<OneStepMockMaze> maze_gen(0);
//     EXPECT_EQ(maze_gen.createMaze(m), CreateMazeWilson<MockMaze>::ECreateResult::Ok);

//     EXPECT_EQ();
// }
