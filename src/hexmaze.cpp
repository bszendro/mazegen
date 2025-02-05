#include "hexmaze.h"
#include "svg_painter.h"

using namespace std;

using EStyle = IPainter::EStyle;

static constexpr char NODE_OPEN = 0;
static constexpr char NODE_VISITED = 1;
static constexpr char NODE_ONPATH = 2;

static constexpr char EDGE_OPEN = 0; // Edge that was not used during traversal (a wall that is still there)
static constexpr char EDGE_INVALID = 'X'; // Edge that cannot be visited
static constexpr char EDGE_VISITED = 1;
static constexpr char EDGE_ONPATH = 2;

HexMaze::HexMaze(int rows, int cols)
    : rows_(rows)
    , cols_(cols)
    , nodes_(rows, cols)
    , edges_(rows+1, 3*(cols+2))
{
    for (int j = 0; j < edges_.cols() - 2; j += 6) {
        edges_[0][j] = EDGE_INVALID;
        edges_[0][j+1] = EDGE_INVALID;
        edges_[0][j+2] = EDGE_INVALID;
        edges_[rows_][j] = EDGE_INVALID;
        edges_[rows_][j+1] = EDGE_INVALID;
        edges_[rows_][j+2] = EDGE_INVALID;
    }
    for (int j = 4; j < edges_.cols(); j += 6) {
        edges_[0][j] = EDGE_INVALID;
        edges_[rows_][j] = EDGE_INVALID;
    }
    for (int i = 0; i < rows_ + 1; i++) {
        edges_[i][2] = EDGE_INVALID;
        edges_[i][3] = EDGE_INVALID;
        edges_[i][3*(cols+2)-4] = EDGE_INVALID;
        edges_[i][3*(cols+2)-3] = EDGE_INVALID;
    }
}

static bool isEdgeVisible(char edge) {
    return edge == EDGE_OPEN || edge == EDGE_INVALID;
}

static Point2D nodeCenter(HexMaze::NodeIndex node, int rad, int padding_x, int padding_y) {
    const auto i = node.i;
    const auto j = node.j;
    const auto h = static_cast<int>(sqrt(3.0)*rad);
    const auto x_0 = rad + rad*3/2*j + padding_x;
    const auto y_0 = (j % 2) == 0
        ? h/2 + h*i + padding_y
        : h + h*i + padding_y;
    return {x_0, y_0};
}

void HexMaze::Draw(IPainter* painter, int rad, int padding_x, int padding_y) const {
    const auto h = static_cast<int>(sqrt(3.0)*rad);
    const auto width = rad/2 + rad*3/2*cols_ + padding_x*2;
    const auto height = h/2 + h*rows_ + padding_y*2;
    painter->BeginDraw(width, height);

    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            const auto x_0 = rad + rad*3/2*j + padding_x;
            const auto y_0 = (j % 2) == 0
                ? h/2 + h*i + padding_y
                : h + h*i + padding_y;

            const Point2D p1{ x_0 - rad, y_0 };
            const Point2D p2{ x_0 - rad/2, y_0 + h/2 };
            const Point2D p3{ x_0 + rad/2, y_0 + h/2 };
            const Point2D p4{ x_0 + rad, y_0 };
            const Point2D p5{ x_0 + rad/2, y_0 - h/2 };
            const Point2D p6{ x_0 - rad/2, y_0 - h/2 };

            EStyle style;
            switch (nodes_[i][j]) {
                case NODE_OPEN: style = EStyle::OpenCell; break;
                case NODE_VISITED: style = EStyle::VisitedCell; break;
                case NODE_ONPATH: style = EStyle::OnPathCell; break;
                default:
                    assert(0);
            }
            painter->DrawPoly({p5, p4, p3, p2, p1, p6}, style);

            const char e1 = edges_[i+1][3*j + 3];
            const char e2 = edges_[i+1][3*j + 4];
            const char e3 = edges_[i+1][3*j + 5];
            const char e4 = (j % 2) == 0
                ? edges_[i][3*j + 6]
                : edges_[i+1][3*j + 6];
            const char e5 = edges_[i][3*j + 4];
            const char e6 = (j % 2) == 0
                ? edges_[i][3*j + 2]
                : edges_[i+1][3*j + 2];

            const auto center = Point2D{ x_0, y_0 };
            if (isEdgeVisible(e1)) {
                painter->DrawLine(p1, p2, EStyle::Wall);
                // const auto next = nodeCenter(nextNode({i, j}, 1), rad, margin);
                // painter->DrawLine(center, next, "stroke:#ff0000;stroke-width:2");
            }
            if (isEdgeVisible(e2)) {
                painter->DrawLine(p2, p3, EStyle::Wall);
                // const auto next = nodeCenter(nextNode({i, j}, 2), rad, margin);
                // painter->DrawLine(center, next, "stroke:#df0000;stroke-width:2");
            }
            if (isEdgeVisible(e3)) {
                painter->DrawLine(p3, p4, EStyle::Wall);
                // const auto next = nodeCenter(nextNode({i, j}, 3), rad, margin);
                // painter->DrawLine(center, next, "stroke:#bf0000;stroke-width:2");
            }
            if (isEdgeVisible(e4)) {
                painter->DrawLine(p4, p5, EStyle::Wall);
                // const auto next = nodeCenter(nextNode({i, j}, 4), rad, margin);
                // painter->DrawLine(center, next, "stroke:#9f0000;stroke-width:2");
            }
            if (isEdgeVisible(e5)) {
                painter->DrawLine(p5, p6, EStyle::Wall);
                // const auto next = nodeCenter(nextNode({i, j}, 5), rad, margin);
                // painter->DrawLine(center, next, "stroke:#7f0000;stroke-width:2");
            }
            if (isEdgeVisible(e6)) {
                painter->DrawLine(p6, p1, EStyle::Wall);
                // const auto next = nodeCenter(nextNode({i, j}, 6), rad, margin);
                // painter->DrawLine(center, next, "stroke:#00ff00;stroke-width:2");
            }

            if (e1 == EDGE_ONPATH) painter->DrawLine(center, nodeCenter(nextNode({i, j}, 1), rad, padding_x, padding_y), EStyle::Edge);
            if (e2 == EDGE_ONPATH) painter->DrawLine(center, nodeCenter(nextNode({i, j}, 2), rad, padding_x, padding_y), EStyle::Edge);
            if (e3 == EDGE_ONPATH) painter->DrawLine(center, nodeCenter(nextNode({i, j}, 3), rad, padding_x, padding_y), EStyle::Edge);
            if (e4 == EDGE_ONPATH) painter->DrawLine(center, nodeCenter(nextNode({i, j}, 4), rad, padding_x, padding_y), EStyle::Edge);
            if (e5 == EDGE_ONPATH) painter->DrawLine(center, nodeCenter(nextNode({i, j}, 5), rad, padding_x, padding_y), EStyle::Edge);
            if (e6 == EDGE_ONPATH) painter->DrawLine(center, nodeCenter(nextNode({i, j}, 6), rad, padding_x, padding_y), EStyle::Edge);
        }
    }

    painter->EndDraw();
}

static ENode toNode(char c) {
    switch (c) {
        case NODE_OPEN: return ENode::Open;
        case NODE_VISITED: return ENode::Visited;
        case NODE_ONPATH: return ENode::OnPath;
        default:
            assert(0);
    }
}

static char fromNode(ENode node) {
    switch (node) {
        case ENode::Open: return NODE_OPEN;
        case ENode::Visited: return NODE_VISITED;
        case ENode::OnPath: return NODE_ONPATH;
        default:
            assert(0);
    }
}

ENode HexMaze::getNode(NodeIndex node) const {
    return toNode(nodes_[node.i][node.j]);
}

void HexMaze::setNode(NodeIndex node, ENode val) {
    nodes_[node.i][node.j] = fromNode(val);

    if (on_change_hook_) {
        on_change_hook_();
    }
}

void HexMaze::getOpenEdges(NodeIndex node, vector<EdgeIndex>& edges) const {
    const auto i = node.i;
    const auto j = node.j;
    const char e1 = edges_[i+1][3*j + 3];
    const char e2 = edges_[i+1][3*j + 4];
    const char e3 = edges_[i+1][3*j + 5];
    const char e4 = (j % 2) == 0
        ? edges_[i][3*j + 6]
        : edges_[i+1][3*j + 6];
    const char e5 = edges_[i][3*j + 4];
    const char e6 = (j % 2) == 0
        ? edges_[i][3*j + 2]
        : edges_[i+1][3*j + 2];

    edges.clear();
    if (e1 == EDGE_OPEN || e1 == EDGE_ONPATH) edges.push_back(1);
    if (e2 == EDGE_OPEN || e2 == EDGE_ONPATH) edges.push_back(2);
    if (e3 == EDGE_OPEN || e3 == EDGE_ONPATH) edges.push_back(3);
    if (e4 == EDGE_OPEN || e4 == EDGE_ONPATH) edges.push_back(4);
    if (e5 == EDGE_OPEN || e5 == EDGE_ONPATH) edges.push_back(5);
    if (e6 == EDGE_OPEN || e6 == EDGE_ONPATH) edges.push_back(6);
}

static char fromEdge(EEdge edge) {
    switch (edge) {
        case EEdge::Open: return EDGE_OPEN;
        case EEdge::Visited: return EDGE_VISITED;
        case EEdge::OnPath: return EDGE_ONPATH;
        default:
            assert(0);
    }
}

void HexMaze::setEdge(NodeIndex node, EdgeIndex edge, EEdge val) {
    const auto i = node.i;
    const auto j = node.j;
    const auto c = fromEdge(val);
    switch (edge) {
        case 1: edges_[i+1][3*j + 3] = c; break;
        case 2: edges_[i+1][3*j + 4] = c; break;
        case 3: edges_[i+1][3*j + 5] = c; break;
        case 4:
            if ((j % 2) == 0)
                edges_[i][3*j + 6] = c;
            else
                edges_[i+1][3*j + 6] = c;
            break;
        case 5: edges_[i][3*j + 4] = c; break;
        case 6:
            if ((j % 2) == 0)
                edges_[i][3*j + 2] = c;
            else
                edges_[i+1][3*j + 2] = c;
            break;
        default:
            assert(0);
    }

    if (on_change_hook_) {
        on_change_hook_();
    }
}

HexMaze::NodeIndex HexMaze::getOpenNode() const {
    for (int i = 0; i < nodes_.rows(); i++) {
        for (int j = 0; j < nodes_.cols(); j++) {
            if (nodes_[i][j] == NODE_OPEN) {
                return {i, j};
            }
        }
    }
    return invalidNode();
}

HexMaze::NodeIndex HexMaze::nextNode(NodeIndex node, EdgeIndex edge) {
    const auto i = node.i;
    const auto j = node.j;
    switch (edge) {
        case 1:
            if ((j % 2) == 0)
                return {i, j-1};
            else
                return {i+1, j-1};
        case 2: return {i+1, j};
        case 3:
            if ((j % 2) == 0)
                return {i, j+1};
            else
                return {i+1, j+1};
        case 4:
            if ((j % 2) == 0)
                return {i-1, j+1};
            else
                return {i, j+1};
        case 5: return {i-1, j};
        case 6:
            if ((j % 2) == 0)
                return {i-1, j-1};
            else
                return {i, j-1};
    }
    return invalidNode();
}

HexMaze::NodeIndex HexMaze::invalidNode() {
    return {-1, -1};
}

HexMaze::ComputedParams HexMaze::getParamsForSize(int area_width, int area_height, int cell_size) {
    const auto rad = cell_size / 2;
    // area_width = rad/2 + rad*3/2*cols
    const auto cols = (area_width - rad/2) / (rad*3/2);
    const auto h = static_cast<int>(sqrt(3.0)*rad);
    // area_height = h/2 + h*rows
    const auto rows = (area_height - h/2) / h;
    return {rows, cols, rad};
}

void HexMaze::setOnChangeHook(OnChangeHook&& on_change_hook) {
    on_change_hook_ = std::move(on_change_hook);
}
