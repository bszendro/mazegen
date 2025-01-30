#include "gen_wilson.h"
#include "hexmaze.h"
#include "square_maze.h"
#include "svg_painter.h"

#include <stdio.h>

#include <iostream>
#include <fstream>

using namespace std;

static void save_image(const string& name, const HexMaze& m) {
    ofstream ofs;
    ofs.open(name, std::ofstream::out);

    SvgPainter painter(ofs);
    // m.Draw(&painter, 40, 40, 50, 50);
    m.Draw(&painter, 20, 50, 10);
}

int main()
{
    const auto cell_size = 100;
    // A4-sized image is 2480 x 3508 pixels at 300 ppi
    const auto rows = 3508 / cell_size;
    const auto cols = 2480 / cell_size;

    cout << "rows = " << rows << "\n";
    cout << "cols = " << cols << "\n";

    // int counter = 1;

    // SquareMaze m(rows, cols);
    HexMaze m(32, cols);
    m.markRegion({8, 6}, {12, 10});
    // m.setOnChangeHook([&counter, &m]{
    //     char buffer[64];
    //     snprintf(buffer, sizeof(buffer), "test_%06d.svg", counter);
    //     save_image(buffer, m);
    //     counter++;
    //     if (counter == 1000) exit(0);
    // });

    // save_image("test_000000.svg", m);

    {
        // CreateMazeWilson<SquareMaze> maze_gen(0);
        CreateMazeWilson<HexMaze> maze_gen(0);
        maze_gen.createMaze(m);

        save_image("test_000000.svg", m);
    }

    return 0;
}
