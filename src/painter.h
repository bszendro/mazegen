#pragma once

#include <vector>

struct Point2D
{
    int x;
    int y;
};

struct IPainter
{
    enum class EStyle
    {
        OpenCell,       // Cell not in the maze
        VisitedCell,    // Cell added to the maze
        OnPathCell,     // Cell on the current random path
        Wall,           // Wall that is removable
        WallBlocked,    // Wall that is not removable
    };

    virtual ~IPainter() = default;
    virtual void BeginDraw(int width, int height) = 0;
    virtual void EndDraw() = 0;
    virtual void DrawLine(const Point2D& p1, const Point2D& p2, EStyle style) = 0;
    virtual void DrawPoly(const std::vector<Point2D>& vertices, EStyle style) = 0;
};
