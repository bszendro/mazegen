#pragma once

#include <vector>

#include "matrix.h"
#include "gen_wilson.h"

struct IPainter;

class HexMaze
{
public:
    struct NodeIndex
    {
        int i;
        int j;

        bool operator==(const NodeIndex& rhs) const {
            return i == rhs.i && j == rhs.j;
        }

        bool operator!=(const NodeIndex& rhs) {
            return i != rhs.i || j != rhs.j;
        }
    };

    using EdgeIndex = int;
    using EdgeList = std::vector<HexMaze::EdgeIndex>;

    HexMaze(int rows, int cols);

    void Draw(IPainter* painter, double rad, int margin_x, int margin_y) const;

    ENode getNode(NodeIndex node) const;
    void setNode(NodeIndex node, ENode val);

    void getOpenEdges(NodeIndex node, std::vector<EdgeIndex>& edges) const;
    void setEdge(NodeIndex node, EdgeIndex edge, EEdge val);

    NodeIndex getOpenNode() const;
    static NodeIndex nextNode(NodeIndex node, EdgeIndex edge);
    static NodeIndex invalidNode();

    void markRegion(NodeIndex top_left, NodeIndex bottom_right);
    using OnChangeHook = std::function<void ()>;
    void setOnChangeHook(OnChangeHook&& on_change_hook);

private:
    int rows_;
    int cols_;
    Matrix<char> nodes_;
    Matrix<char> edges_; // Each entry represents an edge in the dual graph (a wall in the maze)

    NodeIndex marked_top_left_;
    NodeIndex marked_bottom_right_;

    OnChangeHook on_change_hook_;
};
