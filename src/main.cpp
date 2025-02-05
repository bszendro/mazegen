#include "gen_wilson.h"
#include "hexmaze.h"
#include "square_maze.h"
#include "svg_painter.h"

#include <stdio.h>

#include <iostream>
#include <fstream>

using namespace std;

// static void save_image(const string& name, const HexMaze& m, int rad, int padding_x, int padding_y) {
//     ofstream ofs;
//     ofs.open(name, std::ofstream::out);

//     SvgPainter painter(ofs);
//     m.Draw(&painter, rad, padding_x, padding_y);
// }

static void save_image(const string& name, const SquareMaze& m, int block_width, int block_height,
                       int padding_x, int padding_y) {
    ofstream ofs;
    ofs.open(name, std::ofstream::out);

    SvgPainter painter(ofs);
    m.Draw(&painter, block_width, block_height, padding_x, padding_y);
}

int main()
{
    // A4 Portrait
    const auto paper_width = 8.27;
    const auto paper_height = 11.69;
    const auto margin_x = 0.39; // ~1cm
    const auto margin_y = 0.39; // ~1cm
    const auto ppi = 96;
    const auto stroke_width = 4;

    const auto area_width = static_cast<int>((paper_width - 2.0*margin_x) * ppi) - stroke_width;
    const auto area_height = static_cast<int>((paper_height - 2.0*margin_y) * ppi) - stroke_width;

    // const auto cell_size = 40;
    const auto block_width = 40;
    const auto block_height = 40;

    // const auto p = HexMaze::getParamsForSize(area_width, area_height, cell_size);
    const auto p = SquareMaze::getParamsForSize(area_width, area_height, block_width, block_height);

    cout << "area_width = " << area_width << "\n";
    cout << "area_height = " << area_height << "\n";

    cout << "rows = " << p.rows << "\n";
    cout << "cols = " << p.cols << "\n";
    // cout << "rad = " << p.rad << "\n";

    // int counter = 1;

    SquareMaze m(p.rows, p.cols);
    // HexMaze m(p.rows, p.cols);
    // m.setOnChangeHook([&counter, &m]{
    //     char buffer[64];
    //     snprintf(buffer, sizeof(buffer), "test_%06d.svg", counter);
    //     save_image(buffer, m);
    //     counter++;
    //     if (counter == 1000) exit(0);
    // });

    // save_image("test_000000.svg", m);

    {
        CreateMazeWilson<SquareMaze> maze_gen(0);
        // CreateMazeWilson<HexMaze> maze_gen(0);
        maze_gen.createMaze(m);

        // save_image("test_000000.svg", m, p.rad, stroke_width/2, stroke_width/2);
        save_image("test_000000.svg", m, block_width, block_height, stroke_width/2, stroke_width/2);
    }

    return 0;
}
