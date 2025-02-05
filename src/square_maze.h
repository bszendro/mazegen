#pragma once

#include <vector>

#include "matrix.h"
#include "gen_wilson.h"

struct IPainter;

class SquareMaze
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
    using EdgeList = std::vector<EdgeIndex>;

    SquareMaze(int rows, int cols);

    void Draw(IPainter* painter, int block_width, int block_height, int padding_x, int padding_y) const;

    ENode getNode(NodeIndex node) const;
    void setNode(NodeIndex node, ENode val);

    void getOpenEdges(NodeIndex node, EdgeList& edges) const;
    void setEdge(NodeIndex node, EdgeIndex edge, EEdge val);
    EEdge getEdge(NodeIndex node, EdgeIndex edge) const;

    NodeIndex getOpenNode() const;
    static NodeIndex nextNode(NodeIndex node, EdgeIndex edge);
    static NodeIndex invalidNode();

    struct ComputedParams
    {
        // Number of rows that fit into the given area
        int rows;
        // Number of columns that fit into the given area
        int cols;
    };

    static ComputedParams getParamsForSize(int area_width, int area_height,
                                           int block_width, int block_height);

private:
    static constexpr auto NODE_OPEN = static_cast<int>(ENode::Open);
    static constexpr auto NODE_VISITED = static_cast<int>(ENode::Visited);

    static constexpr auto EDGE_OPEN = static_cast<int>(EEdge::Open);
    static constexpr auto EDGE_ONPATH = static_cast<int>(EEdge::OnPath);
    static constexpr auto EDGE_INVALID = static_cast<int>(EEdge::Invalid);

    static bool isEdgeVisible(int edge);

    int rows_;
    int cols_;
    Matrix<int> nodes_;
    Matrix<int> edges_;
};
