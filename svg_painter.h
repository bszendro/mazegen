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
    virtual void DrawLine(const Point2D& p1, const Point2D& p2, const std::string& style) = 0;
    virtual void DrawPoly(const std::vector<Point2D>& vertices, const std::string& style) = 0;
};

class SvgPainter : public IPainter
{
public:
    explicit SvgPainter(std::ostream& os, int width, int height);
    ~SvgPainter();

    SvgPainter(const SvgPainter&) = delete;
    SvgPainter& operator=(const SvgPainter&) = delete;

    void DrawLine(const Point2D& p1, const Point2D& p2, const std::string& style) override;
    void DrawPoly(const std::vector<Point2D>& vertices, const std::string& style) override;

private:
    std::ostream& os_;
};
