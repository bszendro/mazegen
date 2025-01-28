#include "svg_painter.h"

SvgPainter::SvgPainter(std::ostream& os): os_(os) {}

void SvgPainter::BeginDraw(int width, int height) {
    os_ << "<!DOCTYPE svg>\n";
    os_ << "<svg height=\"" << height << "\" width=\"" << width << "\" xmlns=\"http://www.w3.org/2000/svg\" style=\"background-color:white\">\n";
}

void SvgPainter::EndDraw() {
    os_ << "</svg>\n";
}

void SvgPainter::DrawLine(const Point2D& p1, const Point2D& p2, const std::string& style) {
    os_ << "<line x1=\"" << p1.x
        << "\" y1=\"" << p1.y
        << "\" x2=\"" << p2.x
        << "\" y2=\"" << p2.y
        << "\" style=\"" << style << "\" />\n";
}

void SvgPainter::DrawPoly(const std::vector<Point2D>& vertices, const std::string& style) {
    os_ << "<polygon points=\"";
    for (const auto v : vertices) {
        os_ << v.x << ',' << v.y << ' ';
    }
    os_ << "\" style=\"" << style << "\" />\n";
}
