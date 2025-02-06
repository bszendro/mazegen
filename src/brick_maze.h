#pragma once

#include "matrix.h"
#include "node_index_2d.h"
#include "maze_grid.h"

struct IPainter;
struct DrawParams;

// Brick wall grid for maze generation
class BrickMaze : public IMazeGrid
{
public:
    using NodeIndex = NodeIndex2D;
    using EdgeIndex = int;
    using EdgeList = std::vector<EdgeIndex>;

    BrickMaze(int rows, int cols);

    //--------------------------------------------------
    // Interface for CreateMazeWilson
    ENode getNode(NodeIndex node) const;
    void setNode(NodeIndex node, ENode val);

    void getOpenEdges(NodeIndex node, EdgeList& edges) const;
    void setEdge(NodeIndex node, EdgeIndex edge, EEdge val);
    EEdge getEdge(NodeIndex node, EdgeIndex edge) const;

    NodeIndex getOpenNode() const;
    static NodeIndex nextNode(NodeIndex node, EdgeIndex edge);
    static NodeIndex invalidNode();
    //--------------------------------------------------

    // Computes the grid size (rows, cols) for the given parameters:
    // area_width, area_height: Size (in pixels) of the available space to draw the grid
    // cell_width, cell_height: Size (in pixels) of a single cell
    // stroke_width: Thickness of a wall (in pixels)
    static std::tuple<int, int> ComputeGridSize(int area_width, int area_height,
                                                int cell_width, int cell_height,
                                                int stroke_width);

    //--------------------------------------------------
    // IMazeGrid
    void AddExits() override;
    ECreateMazeResult CreateMaze(unsigned random_seed) override;
    void Draw(IPainter& painter, const DrawParams& p) const override;
    //--------------------------------------------------

    void invalidateRegion(NodeIndex topLeft, NodeIndex bottomRight);

    int rows() const { return rows_; }
    int cols() const { return cols_; }

private:
    void invalidateRegionEdges(NodeIndex topLeft, NodeIndex bottomRight);
    bool nodeExists(NodeIndex node) const;

    int rows_;
    int cols_;
    Matrix<int> nodes_;
    Matrix<int> edges_;
};
