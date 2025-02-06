#include "svg_painter.h"

#include <assert.h>

using namespace std;

SvgPainter::SvgPainter(ostream& os, const PainterParams& params): os_(os) {
    char buf[128];
    snprintf(buf, sizeof(buf), "stroke:black;stroke-width:%d;stroke-linecap:round", params.stroke_width);
    wallStyle_ = buf;
    snprintf(buf, sizeof(buf), "stroke:red;stroke-width:%d;stroke-linecap:round", params.stroke_width);
    wallBlockedStyle_ = buf;
}

void SvgPainter::BeginDraw(int width, int height) {
    os_ << "<!DOCTYPE svg>\n";
    os_ << "<svg height=\"" << height << "\" width=\"" << width << "\" xmlns=\"http://www.w3.org/2000/svg\" style=\"background-color:white\">\n";
}

void SvgPainter::EndDraw() {
    os_ << "</svg>\n";
}

string SvgPainter::toSvgStyle(IPainter::EStyle style) const {
    switch (style) {
        case IPainter::EStyle::OpenCell: return "fill:darkgray";
        case IPainter::EStyle::VisitedCell: return "fill:white";
        case IPainter::EStyle::OnPathCell: return "fill:lightgray";
        case IPainter::EStyle::Wall: return wallStyle_;
        case IPainter::EStyle::WallBlocked: return wallBlockedStyle_;
        default:
            assert(0);
    }
}

void SvgPainter::DrawLine(const Point2D& p1, const Point2D& p2, EStyle style) {
    os_ << "<line x1=\"" << p1.x
        << "\" y1=\"" << p1.y
        << "\" x2=\"" << p2.x
        << "\" y2=\"" << p2.y
        << "\" style=\"" << toSvgStyle(style) << "\" />\n";
}

void SvgPainter::DrawPoly(const vector<Point2D>& vertices, EStyle style) {
    os_ << "<polygon points=\"";
    for (const auto v : vertices) {
        os_ << v.x << ',' << v.y << ' ';
    }
    os_ << "\" style=\"" << toSvgStyle(style) << "\" />\n";
}
