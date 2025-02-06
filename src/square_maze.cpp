#include "square_maze.h"
#include "svg_painter.h"

using namespace std;

using EStyle = IPainter::EStyle;

static constexpr auto NODE_OPEN = static_cast<int>(ENode::Open);
static constexpr auto NODE_VISITED = static_cast<int>(ENode::Visited);
static constexpr auto NODE_ONPATH = static_cast<int>(ENode::OnPath);
static constexpr auto NODE_INVALID = -1;

static constexpr auto EDGE_OPEN = static_cast<int>(EEdge::Open);
static constexpr auto EDGE_ONPATH = static_cast<int>(EEdge::OnPath);
static constexpr auto EDGE_INVALID = -1;

#define E1(i, j)    (edges_[i+1][2*j + 2])                                      // direction S
#define E2(i, j)    (edges_[i+1][2*j + 3])                                      // direction E
#define E3(i, j)    (edges_[i][2*j + 2])                                        // direction N
#define E4(i, j)    (edges_[i+1][2*j + 1])                                      // direction W

struct PointParams
{
    const Point2D& center;
    int cell_width;
    int cell_height;
};

inline static Point2D P1(const PointParams& p) { return { p.center.x, p.center.y + p.cell_height }; }
inline static Point2D P2(const PointParams& p) { return { p.center.x + p.cell_width, p.center.y + p.cell_height }; }
inline static Point2D P3(const PointParams& p) { return { p.center.x + p.cell_width, p.center.y }; }
inline static Point2D P4(const PointParams& p) { return { p.center.x, p.center.y }; }

SquareMaze::SquareMaze(int rows, int cols)
    : rows_(rows)
    , cols_(cols)
    , nodes_(rows, cols)
    , edges_(rows+1, 2*(cols+2))
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

inline static Point2D nodeCenter(SquareMaze::NodeIndex node, int cell_width, int cell_height, int padding_x, int padding_y) {
    const auto i = node.i;
    const auto j = node.j;
    const auto x_0 = cell_width*j + padding_x;
    const auto y_0 = cell_height*i + padding_y;
    return {x_0, y_0};
}

void SquareMaze::Draw(IPainter& painter, const DrawParams& p) const {
    const auto cell_width = p.cell_width;
    const auto cell_height = p.cell_height;
    const auto padding_x = p.stroke_width / 2;
    const auto padding_y = p.stroke_width / 2;
    const auto width = cell_width*cols_ + 2*padding_x;
    const auto height = cell_height*rows_ + 2*padding_y;
    painter.BeginDraw(width, height);

    // Cells
    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < cols_; j++) {
            const auto c = nodeCenter({i, j}, cell_width, cell_height, padding_x, padding_y);
            const auto p = PointParams{c, cell_width, cell_height};
            const auto p1{P1(p)};
            const auto p2{P2(p)};
            const auto p3{P3(p)};
            const auto p4{P4(p)};

            painter.DrawPoly({p4, p3, p2, p1}, nodeStyle(nodes_[i][j]));
        }
    }

    // Left side walls
    for (int i = 0; i < rows_; i++) {
        const auto j = 0;
        const auto c = nodeCenter({i, j}, cell_width, cell_height, padding_x, padding_y);
        const auto p = PointParams{c, cell_width, cell_height};
        const auto p1{P1(p)};
        const auto p4{P4(p)};

        const char e4 = E4(i, j);
        if (isEdgeVisible(e4)) {
            painter.DrawLine(p4, p1, edgeStyle(e4));
        }
    }

    // Top side walls
    for (int j = 0; j < cols_; j++) {
        const auto i = 0;
        const auto c = nodeCenter({i, j}, cell_width, cell_height, padding_x, padding_y);
        const auto p = PointParams{c, cell_width, cell_height};
        const auto p3{P3(p)};
        const auto p4{P4(p)};

        const char e3 = E3(i, j);
        if (isEdgeVisible(e3)) {
            painter.DrawLine(p3, p4, edgeStyle(e3));
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

            const char e1 = E1(i, j);
            const char e2 = E2(i, j);
            if (isEdgeVisible(e1)) {
                painter.DrawLine(p1, p2, edgeStyle(e1));
            }
            if (isEdgeVisible(e2)) {
                painter.DrawLine(p2, p3, edgeStyle(e2));
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
            if (!b1 && !b2) {
                continue;
            }

            const auto c = nodeCenter({i, j}, cell_width, cell_height, padding_x, padding_y);
            const auto p = PointParams{c, cell_width, cell_height};
            const auto p1{P1(p)};
            const auto p2{P2(p)};
            const auto p3{P3(p)};

            const char e1 = E1(i, j);
            const char e2 = E2(i, j);
            if (b1 && isEdgeVisible(e1)) {
                painter.DrawLine(p1, p2, edgeStyle(e1));
            }
            if (b2 && isEdgeVisible(e2)) {
                painter.DrawLine(p2, p3, edgeStyle(e2));
            }
        }
    }


    painter.EndDraw();
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
    const auto e1 = E1(i, j);
    const auto e2 = E2(i, j);
    const auto e3 = E3(i, j);
    const auto e4 = E4(i, j);

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
        case 1: E1(i, j) = intval; break;
        case 2: E2(i, j) = intval; break;
        case 3: E3(i, j) = intval; break;
        case 4: E4(i, j) = intval; break;
        default:
            assert(0);
    }
}

EEdge SquareMaze::getEdge(NodeIndex node, EdgeIndex edge) const {
    const auto i = node.i;
    const auto j = node.j;
    int intval = 0;
    switch (edge) {
        case 1: intval = E1(i, j); break;
        case 2: intval = E2(i, j); break;
        case 3: intval = E3(i, j); break;
        case 4: intval = E4(i, j); break;
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

bool SquareMaze::nodeExists(NodeIndex node) const {
    return 0 <= node.i && node.i < rows_ &&
           0 <= node.j && node.j < cols_;
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

void SquareMaze::invalidateRegionEdges(NodeIndex topLeft, NodeIndex bottomRight) {
    assert(topLeft.i <= bottomRight.i);
    assert(topLeft.j <= bottomRight.j);

    for (int i = topLeft.i; i <= bottomRight.i; i++) {
        {
            // Left vertical line
            const auto j = topLeft.j;
            E4(i, j) = EDGE_INVALID;
        }
        {
            // Right vertical line
            const auto j = bottomRight.j;
            E2(i, j) = EDGE_INVALID;
        }
    }

    for (int j = topLeft.j; j <= bottomRight.j; j++) {
        {
            // Top horizontal line
            const auto i = topLeft.i;
            E3(i, j) = EDGE_INVALID;
        }
        {
            // Bottom horizontal line
            const auto i = bottomRight.i;
            E1(i, j) = EDGE_INVALID;
        }
    }
}

void SquareMaze::invalidateRegion(NodeIndex topLeft, NodeIndex bottomRight) {
    invalidateRegionEdges(topLeft, bottomRight);

    for (int i = topLeft.i; i <= bottomRight.i; i++) {
        for (int j = topLeft.j; j <= bottomRight.j; j++) {
            nodes_[i][j] = NODE_INVALID;
        }
    }
}

std::tuple<int, int> SquareMaze::ComputeGridSize(int area_width, int area_height,
                                                        int cell_width, int cell_height,
                                                        int stroke_width) {
    // area_width = cell_width*cols + stroke_width
    const auto cols = (area_width - stroke_width) / cell_width;
    // area_height = cell_height*rows + stroke_width
    const auto rows = (area_height - stroke_width) / cell_height;
    return {rows, cols};
}

void SquareMaze::AddExits() {
    setEdge({0, 0}, 3, EEdge::Visited);
    setEdge({rows_ - 1, cols_ - 1}, 1, EEdge::Visited);
}

ECreateMazeResult SquareMaze::CreateMaze(unsigned random_seed) {
    CreateMazeWilson<SquareMaze> maze_gen(random_seed);
    return maze_gen.createMaze(*this);
}
