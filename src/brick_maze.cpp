#include "brick_maze.h"
#include "svg_painter.h"

using namespace std;

using EStyle = IPainter::EStyle;

static constexpr auto NODE_OPEN = static_cast<int>(ENode::Open);
static constexpr auto NODE_VISITED = static_cast<int>(ENode::Visited);
static constexpr auto NODE_ONPATH = static_cast<int>(ENode::OnPath);
static constexpr auto NODE_INVALID = -1;

static constexpr auto EDGE_OPEN = static_cast<int>(EEdge::Open);
// static constexpr auto EDGE_VISITED = static_cast<int>(EEdge::Visited);
static constexpr auto EDGE_ONPATH = static_cast<int>(EEdge::OnPath);
static constexpr auto EDGE_INVALID = -1;

#define E1(i, j)    (edges_[i+1][3*j + 3])                                      // direction SW
#define E2(i, j)    (edges_[i+1][3*j + 4])                                      // direction SE
#define E3(i, j)    (edges_[i+1][3*j + 5])                                      // direction E
#define E4(i, j)    ((i % 2) == 0 ? edges_[i][3*j + 3] : edges_[i][3*j + 6])    // direction NE
#define E5(i, j)    ((i % 2) == 0 ? edges_[i][3*j + 1] : edges_[i][3*j + 4])    // direction NW
#define E6(i, j)    (edges_[i+1][3*j + 2])                                      // direction W

struct PointParams
{
    const Point2D& center;
    int cell_width;
    int cell_height;
};

inline static Point2D P1(const PointParams& p) { return { p.center.x - p.cell_width/2, p.center.y + p.cell_height/2 }; }
inline static Point2D P2(const PointParams& p) { return { p.center.x, p.center.y + p.cell_height/2 }; }
inline static Point2D P3(const PointParams& p) { return { p.center.x + p.cell_width/2, p.center.y + p.cell_height/2 }; }
inline static Point2D P4(const PointParams& p) { return { p.center.x + p.cell_width/2, p.center.y - p.cell_height/2 }; }
inline static Point2D P5(const PointParams& p) { return { p.center.x, p.center.y - p.cell_height/2 }; }
inline static Point2D P6(const PointParams& p) { return { p.center.x - p.cell_width/2, p.center.y - p.cell_height/2 }; }

BrickMaze::BrickMaze(int rows, int cols)
    : rows_(rows)
    , cols_(cols)
    , nodes_(rows, cols)
    , edges_(rows+1, 3*(cols+2))
{
    invalidateRegionEdges({0, 0}, {rows-1, cols-1});
}

inline static bool isEdgeVisible(int edge) {
    return edge == EDGE_OPEN || edge == EDGE_INVALID;
}

inline static IPainter::EStyle edgeStyle(int edge) {
    switch (edge) {
        case EDGE_OPEN: return EStyle::Wall;
        case EDGE_INVALID: return EStyle::WallBlocked;
        default:
            assert(0);
    }
}

inline static Point2D nodeCenter(BrickMaze::NodeIndex node, int cell_width, int cell_height, int padding_x, int padding_y) {
    const auto i = node.i;
    const auto j = node.j;
    const auto x_0 = (i % 2) == 0
        ? cell_width/2 + cell_width*j + padding_x
        : cell_width + cell_width*j + padding_x;
    const auto y_0 = cell_height/2 + cell_height*i + padding_y;
    return {x_0, y_0};
}

void BrickMaze::Draw(IPainter& painter, const DrawParams& p) const {
    const auto cell_width = p.cell_width;
    const auto cell_height = p.cell_height;
    const auto padding_x = p.stroke_width / 2;
    const auto padding_y = p.stroke_width / 2;
    const auto width = cell_width/2 + cell_width*cols_ + 2*padding_x;
    const auto height = cell_height*rows_ + 2*padding_y;
    painter.BeginDraw(width, height);

    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            const auto c = nodeCenter({i, j}, cell_width, cell_height, padding_x, padding_y);
            const auto p = PointParams{c, cell_width, cell_height};
            const auto p1{P1(p)};
            const auto p3{P3(p)};
            const auto p4{P4(p)};
            const auto p6{P6(p)};

            EStyle style;
            switch (nodes_[i][j]) {
                case NODE_OPEN: style = EStyle::OpenCell; break;
                case NODE_VISITED: style = EStyle::VisitedCell; break;
                case NODE_ONPATH: style = EStyle::OnPathCell; break;
                case NODE_INVALID: style = EStyle::OpenCell; break;
                default:
                    assert(0);
            }
            painter.DrawPoly({p6, p4, p3, p1}, style);
        }
    }

    // Left side walls
    for (int i = 0; i < rows_; i++) {
        const auto j = 0;
        const auto c = nodeCenter({i, j}, cell_width, cell_height, padding_x, padding_y);
        const auto p = PointParams{c, cell_width, cell_height};
        if ((i % 2) == 0) {
            const auto p1{P1(p)};
            const auto p5{P5(p)};
            const auto p6{P6(p)};

            const auto e5 = E5(i, j);
            const auto e6 = E6(i, j);
            if (i > 0 && isEdgeVisible(e5)) { // skip top row
                painter.DrawLine(p5, p6, edgeStyle(e5));
            }
            if (isEdgeVisible(e6)) {
                painter.DrawLine(p6, p1, edgeStyle(e6));
            }
        } else {
            const auto p1{P1(p)};
            const auto p6{P6(p)};

            const auto e6 = E6(i, j);
            if (isEdgeVisible(e6)) {
                painter.DrawLine(p6, p1, edgeStyle(e6));
            }
        }
    }

    // Right side walls
    for (int i = 0; i < rows_; i++) {
        const auto j = cols_ - 1;
        const auto c = nodeCenter({i, j}, cell_width, cell_height, padding_x, padding_y);
        const auto p = PointParams{c, cell_width, cell_height};
        if ((i % 2) != 0) {
            const auto p4{P4(p)};
            const auto p5{P5(p)};

            const auto e4 = E4(i, j);
            if (isEdgeVisible(e4)) {
                painter.DrawLine(p4, p5, edgeStyle(e4));
            }
        }
    }

    // Top side walls
    for (int j = 0; j < cols_; j++) {
        const int i = 0;
        const auto c = nodeCenter({i, j}, cell_width, cell_height, padding_x, padding_y);
        const auto p = PointParams{c, cell_width, cell_height};

        const auto p4{P4(p)};
        const auto p5{P5(p)};
        const auto p6{P6(p)};

        const auto e4 = E4(i, j);
        const auto e5 = E5(i, j);
        if (isEdgeVisible(e4)) {
            painter.DrawLine(p4, p5, edgeStyle(e4));
        }
        if (isEdgeVisible(e5)) {
            painter.DrawLine(p5, p6, edgeStyle(e4));
        }
    }

    // Other walls for valid nodes
    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            if (nodes_[i][j] == NODE_INVALID) {
                continue;
            }

            const auto c = nodeCenter({i, j}, cell_width, cell_height, padding_x, padding_y);
            const auto p = PointParams{c, cell_width, cell_height};
            const auto p1{P1(p)};
            const auto p2{P2(p)};
            const auto p3{P3(p)};
            const auto p4{P4(p)};

            const auto e1 = E1(i, j);
            const auto e2 = E2(i, j);
            const auto e3 = E3(i, j);

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

            const auto c = nodeCenter({i, j}, cell_width, cell_height, padding_x, padding_y);
            const auto p = PointParams{c, cell_width, cell_height};
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

ENode BrickMaze::getNode(NodeIndex node) const {
    return static_cast<ENode>(nodes_[node.i][node.j]);
}

void BrickMaze::setNode(NodeIndex node, ENode val) {
    nodes_[node.i][node.j] = static_cast<int>(val);
}

void BrickMaze::getOpenEdges(NodeIndex node, EdgeList& edges) const {
    const auto i = node.i;
    const auto j = node.j;
    const auto e1 = E1(i, j);
    const auto e2 = E2(i, j);
    const auto e3 = E3(i, j);
    const auto e4 = E4(i, j);
    const auto e5 = E5(i, j);
    const auto e6 = E6(i, j);

    edges.clear();
    if (e1 == EDGE_OPEN || e1 == EDGE_ONPATH) edges.push_back(1);
    if (e2 == EDGE_OPEN || e2 == EDGE_ONPATH) edges.push_back(2);
    if (e3 == EDGE_OPEN || e3 == EDGE_ONPATH) edges.push_back(3);
    if (e4 == EDGE_OPEN || e4 == EDGE_ONPATH) edges.push_back(4);
    if (e5 == EDGE_OPEN || e5 == EDGE_ONPATH) edges.push_back(5);
    if (e6 == EDGE_OPEN || e6 == EDGE_ONPATH) edges.push_back(6);
}

void BrickMaze::setEdge(NodeIndex node, EdgeIndex edge, EEdge val) {
    const auto i = node.i;
    const auto j = node.j;
    const auto intval = static_cast<int>(val);
    switch (edge) {
        case 1: E1(i, j) = intval; break;
        case 2: E2(i, j) = intval; break;
        case 3: E3(i, j) = intval; break;
        case 4: E4(i, j) = intval; break;
        case 5: E5(i, j) = intval; break;
        case 6: E6(i, j) = intval; break;
        default:
            assert(0);
    }
}

EEdge BrickMaze::getEdge(NodeIndex node, EdgeIndex edge) const {
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

BrickMaze::NodeIndex BrickMaze::getOpenNode() const {
    for (int i = 0; i < nodes_.rows(); i++) {
        for (int j = 0; j < nodes_.cols(); j++) {
            if (nodes_[i][j] == EDGE_OPEN) {
                return {i, j};
            }
        }
    }
    return invalidNode();
}

bool BrickMaze::nodeExists(NodeIndex node) const {
    return 0 <= node.i && node.i < rows_ &&
           0 <= node.j && node.j < cols_;
}

BrickMaze::NodeIndex BrickMaze::nextNode(NodeIndex node, EdgeIndex edge) {
    const auto i = node.i;
    const auto j = node.j;
    switch (edge) {
        case 1:
            if ((i % 2) == 0)
                return {i+1, j-1};
            else
                return {i+1, j};
        case 2:
            if ((i % 2) == 0)
                return {i+1, j};
            else
                return {i+1, j+1};
        case 3: return {i, j+1};
        case 4:
            if ((i % 2) == 0)
                return {i-1, j};
            else
                return {i-1, j+1};
        case 5:
            if ((i % 2) == 0)
                return {i-1, j-1};
            else
                return {i-1, j};
        case 6: return {i, j-1};
    }
    return invalidNode();
}

BrickMaze::NodeIndex BrickMaze::invalidNode() {
    return {-1, -1};
}

void BrickMaze::invalidateRegionEdges(NodeIndex topLeft, NodeIndex bottomRight) {
    assert(topLeft.i <= bottomRight.i);
    assert(topLeft.j <= bottomRight.j);

    for (int i = topLeft.i; i <= bottomRight.i; i++) {
        {
            // Left vertical line
            const auto j = topLeft.j;
            if ((i % 2) == 0) {
                E5(i, j) = EDGE_INVALID;
                E6(i, j) = EDGE_INVALID;
                E1(i, j) = EDGE_INVALID;
            } else {
                E6(i, j) = EDGE_INVALID;
            }
        }
        {
            // Right vertical line
            const auto j = bottomRight.j;
            if ((i % 2) == 0) {
                E3(i, j) = EDGE_INVALID;
            } else {
                E2(i, j) = EDGE_INVALID;
                E3(i, j) = EDGE_INVALID;
                E4(i, j) = EDGE_INVALID;
            }
        }
    }

    for (int j = topLeft.j; j <= bottomRight.j; j++) {
        {
            // Top horizontal line
            const auto i = topLeft.i;
            E4(i, j) = EDGE_INVALID;
            E5(i, j) = EDGE_INVALID;
        }
        {
            // Bottom horizontal line
            const auto i = bottomRight.i;
            E1(i, j) = EDGE_INVALID;
            E2(i, j) = EDGE_INVALID;
        }
    }
}

void BrickMaze::invalidateRegion(NodeIndex topLeft, NodeIndex bottomRight) {
    invalidateRegionEdges(topLeft, bottomRight);

    for (int i = topLeft.i; i <= bottomRight.i; i++) {
        for (int j = topLeft.j; j <= bottomRight.j; j++) {
            nodes_[i][j] = NODE_INVALID;
        }
    }
}

std::tuple<int, int> BrickMaze::ComputeGridSize(int area_width, int area_height,
                                                int cell_width, int cell_height,
                                                int stroke_width) {
    // area_width = cell_width/2 + cell_width*cols + stroke_width
    const auto cols = (area_width - cell_width/2 - stroke_width) / cell_width;
    // area_height = cell_height*rows + stroke_width
    const auto rows = (area_height - stroke_width) / cell_height;
    return {rows, cols};
}

void BrickMaze::AddExits() {
    setEdge({0, 0}, 5, EEdge::Visited);
    setEdge({rows_ - 1, cols_ - 1}, 2, EEdge::Visited);
}

ECreateMazeResult BrickMaze::CreateMaze(unsigned random_seed) {
    CreateMazeWilson<BrickMaze> maze_gen(random_seed);
    return maze_gen.createMaze(*this);
}
