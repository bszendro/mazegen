#include "hexmaze.h"
#include "painter.h"

using namespace std;

using EStyle = IPainter::EStyle;

static constexpr char NODE_OPEN = 0;
static constexpr char NODE_VISITED = 1;
static constexpr char NODE_ONPATH = 2;
static constexpr char NODE_INVALID = 3;

static constexpr char EDGE_OPEN = 0; // Edge that was not used during traversal (a wall that is still there)
static constexpr char EDGE_VISITED = 1;
static constexpr char EDGE_ONPATH = 2;
static constexpr char EDGE_INVALID = 3; // Edge that cannot be visited

#define E1(i, j)    (edges_[i+1][3*j + 3])                                      // direction SW
#define E2(i, j)    (edges_[i+1][3*j + 4])                                      // direction S
#define E3(i, j)    (edges_[i+1][3*j + 5])                                      // direction SE
#define E4(i, j)    ((j % 2) == 0 ? edges_[i][3*j + 6] : edges_[i+1][3*j + 6])  // direction NE
#define E5(i, j)    (edges_[i][3*j + 4])                                        // direction N
#define E6(i, j)    ((j % 2) == 0 ? edges_[i][3*j + 2] : edges_[i+1][3*j + 2])  // direction NW

struct PointParams
{
    const Point2D& center;
    int rad;
    int h;
};

inline static Point2D P1(const PointParams& p) { return { p.center.x - p.rad, p.center.y }; }
inline static Point2D P2(const PointParams& p) { return { p.center.x - p.rad/2, p.center.y + p.h/2 }; }
inline static Point2D P3(const PointParams& p) { return { p.center.x + p.rad/2, p.center.y + p.h/2 }; }
inline static Point2D P4(const PointParams& p) { return { p.center.x + p.rad, p.center.y }; }
inline static Point2D P5(const PointParams& p) { return { p.center.x + p.rad/2, p.center.y - p.h/2 }; }
inline static Point2D P6(const PointParams& p) { return { p.center.x - p.rad/2, p.center.y - p.h/2 }; }

HexMaze::HexMaze(int rows, int cols)
    : rows_(rows)
    , cols_(cols)
    , nodes_(rows, cols)
    , edges_(rows+1, 3*(cols+2))
{
    invalidateRegionEdges({0, 0}, {rows-1, cols-1});
}

inline static bool isEdgeVisible(char edge) {
    return edge == EDGE_OPEN || edge == EDGE_INVALID;
}

inline static IPainter::EStyle edgeStyle(char edge) {
    switch (edge) {
        case EDGE_OPEN: return EStyle::Wall;
        case EDGE_INVALID: return EStyle::WallBlocked;
        default:
            assert(0);
    }
}

inline static IPainter::EStyle nodeStyle(char node) {
    switch (node) {
        case NODE_OPEN: return EStyle::OpenCell;
        case NODE_VISITED: return EStyle::VisitedCell;
        case NODE_ONPATH: return EStyle::OnPathCell;
        case NODE_INVALID: return EStyle::OpenCell;
        default:
            assert(0);
    }
}

inline static Point2D nodeCenter(HexMaze::NodeIndex node, int rad, int h, int padding_x, int padding_y) {
    const auto i = node.i;
    const auto j = node.j;
    const auto x_0 = rad + rad*3/2*j + padding_x;
    const auto y_0 = (j % 2) == 0
        ? h/2 + h*i + padding_y
        : h + h*i + padding_y;
    return {x_0, y_0};
}

void HexMaze::Draw(IPainter& painter, const DrawParams& p) const {
    const auto rad = p.cell_width / 2;
    const auto h = static_cast<int>(sqrt(3.0)*rad);
    const auto padding_x = p.stroke_width / 2;
    const auto padding_y = p.stroke_width / 2;
    const auto width = rad/2 + rad*3/2*cols_ + padding_x*2;
    const auto height = h/2 + h*rows_ + padding_y*2;
    painter.BeginDraw(width, height);

    // Cells
    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            const auto c = nodeCenter({i, j}, rad, h, padding_x, padding_y);
            const auto p = PointParams{c, rad, h};
            const auto p1{P1(p)};
            const auto p2{P2(p)};
            const auto p3{P3(p)};
            const auto p4{P4(p)};
            const auto p5{P5(p)};
            const auto p6{P6(p)};

            painter.DrawPoly({p5, p4, p3, p2, p1, p6}, nodeStyle(nodes_[i][j]));
        }
    }

    // Left side walls (skip top row)
    for (int i = 1; i < rows_; i++) {
        const auto j = 0;
        const auto c = nodeCenter({i, j}, rad, h, padding_x, padding_y);
        const auto p = PointParams{c, rad, h};
        const auto p1{P1(p)};
        const auto p6{P6(p)};

        const char e6 = E6(i, j);
        if (isEdgeVisible(e6)) {
            painter.DrawLine(p6, p1, edgeStyle(e6));
        }
    }

    // Right side walls (skip top row)
    for (int i = 1; i < rows_; i++) {
        const auto j = cols_ - 1;
        const auto c = nodeCenter({i, j}, rad, h, padding_x, padding_y);
        const auto p = PointParams{c, rad, h};
        const auto p4{P4(p)};
        const auto p5{P5(p)};

        const char e4 = E4(i, j);
        if (isEdgeVisible(e4)) {
            painter.DrawLine(p4, p5, edgeStyle(e4));
        }
    }

    // Top side walls (even columns)
    for (int j = 0; j < cols_; j+=2) {
        const auto i = 0;
        const auto c = nodeCenter({i, j}, rad, h, padding_x, padding_y);
        const auto p = PointParams{c, rad, h};
        const auto p1{P1(p)};
        const auto p4{P4(p)};
        const auto p5{P5(p)};
        const auto p6{P6(p)};

        const char e4 = E4(i, j);
        const char e5 = E5(i, j);
        const char e6 = E6(i, j);
        if (isEdgeVisible(e4)) {
            painter.DrawLine(p4, p5, edgeStyle(e4));
        }
        if (isEdgeVisible(e5)) {
            painter.DrawLine(p5, p6, edgeStyle(e5));
        }
        if (isEdgeVisible(e6)) {
            painter.DrawLine(p6, p1, edgeStyle(e6));
        }
    }

    // Top side walls (odd columns)
    for (int j = 1; j < cols_; j+=2) {
        const auto i = 0;
        const auto c = nodeCenter({i, j}, rad, h, padding_x, padding_y);
        const auto p = PointParams{c, rad, h};
        const auto p5{P5(p)};
        const auto p6{P6(p)};

        const char e5 = E5(i, j);
        if (isEdgeVisible(e5)) {
            painter.DrawLine(p5, p6, edgeStyle(e5));
        }
    }

    // Other walls for valid nodes
    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            if (nodes_[i][j] == NODE_INVALID) {
                continue;
            }

            const auto c = nodeCenter({i, j}, rad, h, padding_x, padding_y);
            const auto p = PointParams{c, rad, h};
            const auto p1{P1(p)};
            const auto p2{P2(p)};
            const auto p3{P3(p)};
            const auto p4{P4(p)};

            const char e1 = E1(i, j);
            const char e2 = E2(i, j);
            const char e3 = E3(i, j);
            if (isEdgeVisible(e1)) {
                painter.DrawLine(p1, p2, edgeStyle(e1));
            }
            if (isEdgeVisible(e2)) {
                painter.DrawLine(p2, p3, edgeStyle(e2));
            }
            if (isEdgeVisible(e3)) {
                painter.DrawLine(p3, p4, edgeStyle(e3));
            }
        }
    }

    // Other walls for invalid nodes
    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            if (nodes_[i][j] != NODE_INVALID) {
                continue;
            }

            // Check if there is a valid neighbour
            auto next = nextNode({i, j}, 1);
            const auto b1 = !nodeExists(next) || nodes_[next.i][next.j] != NODE_INVALID;
            next = nextNode({i, j}, 2);
            const auto b2 = !nodeExists(next) || nodes_[next.i][next.j] != NODE_INVALID;
            next = nextNode({i, j}, 3);
            const auto b3 = !nodeExists(next) || nodes_[next.i][next.j] != NODE_INVALID;
            if (!b1 && !b2 && !b3) {
                continue;
            }

            const auto c = nodeCenter({i, j}, rad, h, padding_x, padding_y);
            const auto p = PointParams{c, rad, h};
            const auto p1{P1(p)};
            const auto p2{P2(p)};
            const auto p3{P3(p)};
            const auto p4{P4(p)};

            const char e1 = E1(i, j);
            const char e2 = E2(i, j);
            const char e3 = E3(i, j);
            if (b1 && isEdgeVisible(e1)) {
                painter.DrawLine(p1, p2, edgeStyle(e1));
            }
            if (b2 && isEdgeVisible(e2)) {
                painter.DrawLine(p2, p3, edgeStyle(e2));
            }
            if (b3 && isEdgeVisible(e3)) {
                painter.DrawLine(p3, p4, edgeStyle(e3));
            }
        }
    }

    painter.EndDraw();
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
    const char e1 = E1(i, j);
    const char e2 = E2(i, j);
    const char e3 = E3(i, j);
    const char e4 = E4(i, j);
    const char e5 = E5(i, j);
    const char e6 = E6(i, j);

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
        case 1: E1(i, j) = c; break;
        case 2: E2(i, j) = c; break;
        case 3: E3(i, j) = c; break;
        case 4: E4(i, j) = c; break;
        case 5: E5(i, j) = c; break;
        case 6: E6(i, j) = c; break;
        default:
            assert(0);
    }

    if (on_change_hook_) {
        on_change_hook_();
    }
}

EEdge HexMaze::getEdge(NodeIndex node, EdgeIndex edge) const {
    const auto i = node.i;
    const auto j = node.j;
    int intval = 0;
    switch (edge) {
        case 1: intval = E1(i, j); break;
        case 2: intval = E2(i, j); break;
        case 3: intval = E3(i, j); break;
        case 4: intval = E4(i, j); break;
        case 5: intval = E5(i, j); break;
        case 6: intval = E6(i, j); break;
        default:
            assert(0);
    }
    return static_cast<EEdge>(intval);
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

bool HexMaze::nodeExists(NodeIndex node) const {
    return 0 <= node.i && node.i < rows_ &&
           0 <= node.j && node.j < cols_;
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

void HexMaze::invalidateRegionEdges(NodeIndex topLeft, NodeIndex bottomRight) {
    assert(topLeft.i <= bottomRight.i);
    assert(topLeft.j <= bottomRight.j);

    for (int i = topLeft.i; i <= bottomRight.i; i++) {
        {
            // Left vertical line
            const auto j = topLeft.j;
            E1(i, j) = EDGE_INVALID;
            E6(i, j) = EDGE_INVALID;
        }
        {
            // Right vertical line
            const auto j = bottomRight.j;
            E3(i, j) = EDGE_INVALID;
            E4(i, j) = EDGE_INVALID;
        }
    }

    for (int j = topLeft.j; j <= bottomRight.j; j++) {
        {
            // Top horizontal line
            const auto i = topLeft.i;
            if ((j % 2) == 0) {
                E4(i, j) = EDGE_INVALID;
                E5(i, j) = EDGE_INVALID;
                E6(i, j) = EDGE_INVALID;
            } else {
                E5(i, j) = EDGE_INVALID;
            }
        }
        {
            // Bottom horizontal line
            const auto i = bottomRight.i;
            if ((j % 2) == 0) {
                E2(i, j) = EDGE_INVALID;
            } else {
                E1(i, j) = EDGE_INVALID;
                E2(i, j) = EDGE_INVALID;
                E3(i, j) = EDGE_INVALID;
            }
        }
    }
}

void HexMaze::invalidateRegion(NodeIndex topLeft, NodeIndex bottomRight) {
    invalidateRegionEdges(topLeft, bottomRight);

    for (int i = topLeft.i; i <= bottomRight.i; i++) {
        for (int j = topLeft.j; j <= bottomRight.j; j++) {
            nodes_[i][j] = NODE_INVALID;
        }
    }
}

void HexMaze::setOnChangeHook(OnChangeHook&& on_change_hook) {
    on_change_hook_ = std::move(on_change_hook);
}

std::tuple<int, int> HexMaze::ComputeGridSize(int area_width, int area_height,
                                              int cell_width, int stroke_width) {
    const auto rad = cell_width / 2;
    // area_width = rad/2 + rad*3/2*cols + stroke_width
    const auto cols = (area_width - rad/2 - stroke_width) / (rad*3/2);
    const auto h = static_cast<int>(sqrt(3.0)*rad);
    // area_height = h/2 + h*rows + stroke_width
    const auto rows = (area_height - h/2 - stroke_width) / h;
    return {rows, cols};
}

void HexMaze::AddExits() {
    setEdge({0, 0}, 6, EEdge::Visited);
    setEdge({rows_ - 1, cols_ - 1}, 3, EEdge::Visited);
}

ECreateMazeResult HexMaze::CreateMaze(unsigned random_seed) {
    CreateMazeWilson<HexMaze> maze_gen(random_seed);
    return maze_gen.createMaze(*this);
}
