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

    void Draw(IPainter* painter, int rad, int padding_x, int padding_y) const;

    ENode getNode(NodeIndex node) const;
    void setNode(NodeIndex node, ENode val);

    void getOpenEdges(NodeIndex node, std::vector<EdgeIndex>& edges) const;
    void setEdge(NodeIndex node, EdgeIndex edge, EEdge val);

    NodeIndex getOpenNode() const;
    static NodeIndex nextNode(NodeIndex node, EdgeIndex edge);
    static NodeIndex invalidNode();

    struct ComputedParams
    {
        // Number of rows that fit into the given area
        int rows;
        // Number of columns that fit into the given area
        int cols;
        // Radius to use for drawing
        int rad;
    };

    static ComputedParams getParamsForSize(int area_width, int area_height, int cell_size);

    using OnChangeHook = std::function<void ()>;
    void setOnChangeHook(OnChangeHook&& on_change_hook);

private:
    int rows_;
    int cols_;
    Matrix<char> nodes_;
    Matrix<char> edges_; // Each entry represents an edge in the dual graph (a wall in the maze)

    OnChangeHook on_change_hook_;
};
