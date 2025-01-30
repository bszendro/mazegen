#include "svg_painter.h"

#include <assert.h>

using namespace std;

SvgPainter::SvgPainter(ostream& os): os_(os) {}

void SvgPainter::BeginDraw(int width, int height) {
    os_ << "<!DOCTYPE svg>\n";
    os_ << "<svg height=\"" << height << "\" width=\"" << width << "\" xmlns=\"http://www.w3.org/2000/svg\" style=\"background-color:white\">\n";
}

void SvgPainter::EndDraw() {
    os_ << "</svg>\n";
}

static string toSvgStyle(IPainter::EStyle style) {
    switch (style) {
        case IPainter::EStyle::OpenCell: return "fill:darkgray";
        case IPainter::EStyle::VisitedCell: return "fill:white";
        case IPainter::EStyle::OnPathCell: return "fill:lightgray";
        case IPainter::EStyle::Wall: return "stroke:black;stroke-width:4;stroke-linecap:round";
        case IPainter::EStyle::Edge: return "stroke:#ff0000;stroke-width:2";

        case IPainter::EStyle::MarkedOpenCell: return "fill:darkred";
        case IPainter::EStyle::MarkedVisitedCell: return "fill:crimson";
        case IPainter::EStyle::MarkedOnPathCell: return "fill:deeppink";
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
