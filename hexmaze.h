#pragma once

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

    HexMaze(int rows, int cols);

    void Draw(IPainter* painter, double rad, int x_margin, int y_margin) const;

    ENode getNode(NodeIndex node) const;
    void setNode(NodeIndex node, ENode val);

    void getOpenEdges(NodeIndex node, std::vector<EdgeIndex>& edges) const;
    void setEdge(NodeIndex node, EdgeIndex edge, EEdge val);

    NodeIndex getOpenNode() const;
    NodeIndex nextNode(NodeIndex node, EdgeIndex edge) const;
    static NodeIndex invalidNode();

private:
    int rows_;
    int cols_;
    Matrix nodes_;
    Matrix edges_; // Each entry represents an edge in the dual graph (a wall in the maze)
};
