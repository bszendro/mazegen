#pragma once

#include "painter.h"

#include <ostream>
#include <string>

struct PainterParams
{
    int stroke_width;
};

class SvgPainter : public IPainter
{
public:
    explicit SvgPainter(std::ostream& os, const PainterParams& params);

    SvgPainter(const SvgPainter&) = delete;
    SvgPainter& operator=(const SvgPainter&) = delete;

    void BeginDraw(int width, int height) override;
    void EndDraw() override;
    void DrawLine(const Point2D& p1, const Point2D& p2, EStyle style) override;
    void DrawPoly(const std::vector<Point2D>& vertices, EStyle style) override;

private:
    std::string toSvgStyle(IPainter::EStyle style) const;

    std::ostream& os_;
    std::string wallStyle_;
    std::string wallBlockedStyle_;
};
