#pragma once

#include <assert.h>
#include <random>

#include <iostream>

enum class ENode
{
    Open,
    Visited,
    OnPath,
};

enum class EEdge
{
    Open,
    Visited,
    OnPath,
    Invalid,
};

template< typename Maze >
class CreateMazeWilson
{
public:
    enum class ECreateResult
    {
        Ok,
        ErrNoFirstOpenNode,
    };

    explicit CreateMazeWilson(int random_seed);

    ECreateResult createMaze(Maze& maze);

private:
    using NodeIndex = Maze::NodeIndex;
    using EdgeIndex = Maze::EdgeIndex;

    struct PathItem
    {
        EdgeIndex edge;
        NodeIndex target_node;
    };

    PathItem getRandomStep(const Maze& maze, NodeIndex node);

    std::vector<EdgeIndex> open_edges_;
    std::vector<PathItem> current_path_;

    std::mt19937 random_engine_;
};

template< typename Maze >
CreateMazeWilson<Maze>::CreateMazeWilson(int random_seed) : random_engine_(random_seed) {}

template< typename Maze >
CreateMazeWilson<Maze>::ECreateResult CreateMazeWilson<Maze>::createMaze(Maze& maze) {
    // Add a random node to the graph
    const auto first_node = maze.getOpenNode();
    if (first_node == Maze::invalidNode()) {
        return ECreateResult::ErrNoFirstOpenNode;
    }
    maze.setNode(first_node, ENode::Visited);

    for (;;) {
        // While there are still open nodes, pick one to start a random path
        const auto start_node = maze.getOpenNode();
        if (start_node == Maze::invalidNode()) {
            break;
        }
        maze.setNode(start_node, ENode::OnPath);

        // Pick a random edge and make a step to the next node
        const auto step = getRandomStep(maze, start_node);
        maze.setEdge(start_node, step.edge, EEdge::OnPath);
        current_path_.clear();
        current_path_.emplace_back(step);

        for (;;) {
            auto last_node = current_path_.back().target_node;
            const auto status = maze.getNode(last_node);
            if (status == ENode::Visited) {
                // Stop if the node at the end of the path is already in the graph
                break;
            }
            if (status == ENode::OnPath) {
                // Found a loop, erase it

                // Delete last item
                assert(!current_path_.empty());
                maze.setNode(current_path_.back().target_node, ENode::Open);
                const auto src_node = current_path_.size() >= 2
                    ? current_path_[current_path_.size() - 2].target_node
                    : start_node;
                maze.setEdge(src_node, current_path_.back().edge, EEdge::Open);
                current_path_.pop_back();

                while (!current_path_.empty() && current_path_.back().target_node != last_node) {
                    maze.setNode(current_path_.back().target_node, ENode::Open);
                    const auto src_node = current_path_.size() >= 2
                        ? current_path_[current_path_.size() - 2].target_node
                        : start_node;
                    maze.setEdge(src_node, current_path_.back().edge, EEdge::Open);
                    current_path_.pop_back();
                }
                last_node = current_path_.empty() ? start_node : current_path_.back().target_node;

            } else {
                maze.setNode(last_node, ENode::OnPath);
            }

            // Pick a random edge and make a step to the next node
            const auto step = getRandomStep(maze, last_node);
            maze.setEdge(last_node, step.edge, EEdge::OnPath);
            current_path_.emplace_back(step);
        }

        // Add all nodes of the path to the graph
        maze.setNode(start_node, ENode::Visited);
        auto prev_node = start_node;
        for (const auto& step : current_path_) {
            maze.setEdge(prev_node, step.edge, EEdge::Visited);
            maze.setNode(step.target_node, ENode::Visited);
            prev_node = step.target_node;
        }
    }
    return ECreateResult::Ok;
}

template< typename Maze >
CreateMazeWilson<Maze>::PathItem CreateMazeWilson<Maze>::getRandomStep(const Maze& maze, NodeIndex node) {
    open_edges_.clear();
    maze.getOpenEdges(node, open_edges_);
    assert(!open_edges_.empty()); // TODO: this should return an error instead of an assert

    std::uniform_int_distribution<int> dist(0, open_edges_.size() - 1);
    const auto edge = open_edges_[dist(random_engine_)];
    const auto target_node = maze.nextNode(node, edge);
    return {edge, target_node};
}
