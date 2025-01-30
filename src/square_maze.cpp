#include "square_maze.h"
#include "svg_painter.h"

using namespace std;

using EStyle = IPainter::EStyle;

SquareMaze::SquareMaze(int rows, int cols)
    : rows_(rows)
    , cols_(cols)
    , nodes_(rows, cols)
    , edges_(rows+1, 2*(cols+2))
{
    // Top and bottom
    for (int i = 2; i < edges_.cols() - 2; i += 2) {
        edges_[0][i] = EDGE_INVALID;
        edges_[rows][i] = EDGE_INVALID;
    }
    // Left and right
    for (int j = 1; j < edges_.rows(); j++) {
        edges_[j][1] = EDGE_INVALID;
        edges_[j][2*(cols+2)-3] = EDGE_INVALID;
    }
}

bool SquareMaze::isEdgeVisible(int edge) {
    return edge == EDGE_OPEN || edge == EDGE_INVALID;
}

void SquareMaze::Draw(IPainter* painter, int block_width, int block_height, int margin_x, int margin_y) const {
    const auto width = block_width*cols_ + 2*margin_x;
    const auto height = block_height*rows_ + 2*margin_y;
    painter->BeginDraw(width, height);

    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            const auto x_0 = block_width*j + margin_x;
            const auto y_0 = block_height*i + margin_y;

            const Point2D p1{ x_0, y_0 + block_height };
            const Point2D p2{ x_0 + block_width, y_0 + block_height };
            const Point2D p3{ x_0 + block_width, y_0 };
            const Point2D p4{ x_0, y_0 };

            switch (nodes_[i][j]) {
                case NODE_OPEN:
                    painter->DrawPoly({p4, p3, p2, p1}, EStyle::OpenCell);
                    break;
                case NODE_VISITED:
                    painter->DrawPoly({p4, p3, p2, p1}, EStyle::VisitedCell);
                    break;
                default:
                    assert(0);
            }

            const auto e1 = edges_[i+1][2*j + 2];
            const auto e2 = edges_[i+1][2*j + 3];
            const auto e3 = edges_[i][2*j + 2];
            const auto e4 = edges_[i+1][2*j + 1];

            if (isEdgeVisible(e1)) {
                painter->DrawLine(p1, p2, EStyle::Wall);
            }
            if (isEdgeVisible(e2)) {
                painter->DrawLine(p2, p3, EStyle::Wall);
            }
            if (isEdgeVisible(e3)) {
                painter->DrawLine(p3, p4, EStyle::Wall);
            }
            if (isEdgeVisible(e4)) {
                painter->DrawLine(p4, p1, EStyle::Wall);
            }
        }
    }

    painter->EndDraw();
}

ENode SquareMaze::getNode(NodeIndex node) const {
    return static_cast<ENode>(nodes_[node.i][node.j]);
}

void SquareMaze::setNode(NodeIndex node, ENode val) {
    nodes_[node.i][node.j] = static_cast<int>(val);
}

void SquareMaze::getOpenEdges(NodeIndex node, EdgeList& edges) const {
    const auto i = node.i;
    const auto j = node.j;
    const auto e1 = edges_[i+1][2*j + 2];
    const auto e2 = edges_[i+1][2*j + 3];
    const auto e3 = edges_[i][2*j + 2];
    const auto e4 = edges_[i+1][2*j + 1];

    edges.clear();
    if (e1 == EDGE_OPEN || e1 == EDGE_ONPATH) edges.push_back(1);
    if (e2 == EDGE_OPEN || e2 == EDGE_ONPATH) edges.push_back(2);
    if (e3 == EDGE_OPEN || e3 == EDGE_ONPATH) edges.push_back(3);
    if (e4 == EDGE_OPEN || e4 == EDGE_ONPATH) edges.push_back(4);
}

void SquareMaze::setEdge(NodeIndex node, EdgeIndex edge, EEdge val) {
    const auto i = node.i;
    const auto j = node.j;
    const auto intval = static_cast<int>(val);
    switch (edge) {
        case 1: edges_[i+1][2*j + 2] = intval; break;
        case 2: edges_[i+1][2*j + 3] = intval; break;
        case 3: edges_[i][2*j + 2] = intval; break;
        case 4: edges_[i+1][2*j + 1] = intval; break;
        default:
            assert(0);
    }
}

EEdge SquareMaze::getEdge(NodeIndex node, EdgeIndex edge) const {
    const auto i = node.i;
    const auto j = node.j;
    int intval = 0;
    switch (edge) {
        case 1: intval = edges_[i+1][2*j + 2]; break;
        case 2: intval = edges_[i+1][2*j + 3]; break;
        case 3: intval = edges_[i][2*j + 2]; break;
        case 4: intval = edges_[i+1][2*j + 1]; break;
        default:
            assert(0);
    }
    return static_cast<EEdge>(intval);
}

SquareMaze::NodeIndex SquareMaze::getOpenNode() const {
    for (int i = 0; i < nodes_.rows(); i++) {
        for (int j = 0; j < nodes_.cols(); j++) {
            if (nodes_[i][j] == EDGE_OPEN) {
                return {i, j};
            }
        }
    }
    return invalidNode();
}

SquareMaze::NodeIndex SquareMaze::nextNode(NodeIndex node, EdgeIndex edge) {
    const auto i = node.i;
    const auto j = node.j;
    switch (edge) {
        case 1: return {i+1, j};
        case 2: return {i, j+1};
        case 3: return {i-1, j};
        case 4: return {i, j-1};
    }
    return invalidNode();

}

SquareMaze::NodeIndex SquareMaze::invalidNode() {
    return {-1, -1};
}
