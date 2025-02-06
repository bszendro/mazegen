#pragma once

#include "painter.h"
#include "gen_wilson.h"

struct DrawParams
{
    // Width of a cell
    int cell_width;
    // Height of a cell
    int cell_height;
    // Wall width
    int stroke_width;
};

class IMazeGrid
{
public:
    virtual ~IMazeGrid() = default;

    virtual void AddExits() = 0;
    virtual ECreateMazeResult CreateMaze(unsigned random_seed) = 0;
    virtual void Draw(IPainter& painter, const DrawParams& p) const = 0;
};
