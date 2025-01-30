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

enum class ECreateMazeResult
{
    Ok,
    ErrNoFirstOpenNode, // All nodes of the maze are already visited
    ErrNoOpenEdges, // We reached a node which doesn't have any open edges
};

// TODO: what is this? document
template< typename Maze >
class CreateMazeWilson
{
public:
    explicit CreateMazeWilson(unsigned random_seed);

    ECreateMazeResult createMaze(Maze& maze);

private:
    using NodeIndex = Maze::NodeIndex;
    using EdgeIndex = Maze::EdgeIndex;

    // Represents a single step on a path
    struct PathItem
    {
        // The edge from the node in the previous step to the current node.
        // If there is no previous step, it is the edge from the start node
        EdgeIndex edge;
        // The node reached in this step
        NodeIndex target_node;
    };

    // Find a random step from the given node. Returns false if there are no open edges from `node`
    bool getRandomStep(const Maze& maze, NodeIndex node, PathItem& step);

    // This variable is local to getRandomStep but is a member to avoid reallocating the array on each call
    std::vector<EdgeIndex> open_edges_;
    // The current random walk
    std::vector<PathItem> current_path_;

    std::mt19937 random_engine_;
};

template< typename Maze >
CreateMazeWilson<Maze>::CreateMazeWilson(unsigned random_seed): random_engine_(random_seed) {}

template< typename Maze >
ECreateMazeResult CreateMazeWilson<Maze>::createMaze(Maze& maze) {
    // Add a random node to the graph
    const auto first_node = maze.getOpenNode();
    if (first_node == Maze::invalidNode()) {
        return ECreateMazeResult::ErrNoFirstOpenNode;
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
        PathItem step;
        if (!getRandomStep(maze, start_node, step)) {
            return ECreateMazeResult::ErrNoOpenEdges;
        }
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
            PathItem step;
            if (!getRandomStep(maze, last_node, step)) {
                return ECreateMazeResult::ErrNoOpenEdges;
            }
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

    return ECreateMazeResult::Ok;
}

template< typename Maze >
bool CreateMazeWilson<Maze>::getRandomStep(const Maze& maze, NodeIndex node, PathItem& step) {
    open_edges_.clear();
    maze.getOpenEdges(node, open_edges_);
    if (open_edges_.empty()) {
        return false;
    }

    std::uniform_int_distribution<int> dist(0, open_edges_.size() - 1);
    const auto edge = open_edges_[dist(random_engine_)];
    const auto target_node = maze.nextNode(node, edge);
    step = {edge, target_node};
    return true;
}
