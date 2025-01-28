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
    virtual ~IPainter() {}
    virtual void BeginDraw(int width, int height) = 0;
    virtual void EndDraw() = 0;
    virtual void DrawLine(const Point2D& p1, const Point2D& p2, const std::string& style) = 0;
    virtual void DrawPoly(const std::vector<Point2D>& vertices, const std::string& style) = 0;
};

class SvgPainter : public IPainter
{
public:
    explicit SvgPainter(std::ostream& os);

    SvgPainter(const SvgPainter&) = delete;
    SvgPainter& operator=(const SvgPainter&) = delete;

    void BeginDraw(int width, int height) override;
    void EndDraw() override;
    void DrawLine(const Point2D& p1, const Point2D& p2, const std::string& style) override;
    void DrawPoly(const std::vector<Point2D>& vertices, const std::string& style) override;

private:
    std::ostream& os_;
};
