#pragma once

#include <ostream>
#include <string>
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
        OnPathCell,
        Wall,           // Wall between cells
        Edge,           // Line across cells to show the path
    };

    virtual ~IPainter() {}
    virtual void BeginDraw(int width, int height) = 0;
    virtual void EndDraw() = 0;
    virtual void DrawLine(const Point2D& p1, const Point2D& p2, EStyle style) = 0;
    virtual void DrawPoly(const std::vector<Point2D>& vertices, EStyle style) = 0;
};

class SvgPainter : public IPainter
{
public:
    explicit SvgPainter(std::ostream& os);

    SvgPainter(const SvgPainter&) = delete;
    SvgPainter& operator=(const SvgPainter&) = delete;

    void BeginDraw(int width, int height) override;
    void EndDraw() override;
    void DrawLine(const Point2D& p1, const Point2D& p2, EStyle style) override;
    void DrawPoly(const std::vector<Point2D>& vertices, EStyle style) override;

private:
    std::ostream& os_;
};
