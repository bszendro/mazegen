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

    void Draw(IPainter* painter, int block_width, int block_height, int margin_x, int margin_y) const;

    ENode getNode(NodeIndex node) const;
    void setNode(NodeIndex node, ENode val);

    void getOpenEdges(NodeIndex node, EdgeList& edges) const;
    void setEdge(NodeIndex node, EdgeIndex edge, EEdge val);

    NodeIndex getOpenNode() const;
    static NodeIndex nextNode(NodeIndex node, EdgeIndex edge);
    static NodeIndex invalidNode();

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
