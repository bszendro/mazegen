#include <iostream>
#include <fstream>

#include "gen_wilson.h"
#include "hexmaze.h"
#include "square_maze.h"
#include "svg_painter.h"

using namespace std;

int main()
{
    const auto cell_size = 100;
    // A4-sized image is 2480 x 3508 pixels at 300 ppi
    const auto rows = 3508 / cell_size;
    const auto cols = 2480 / cell_size;

    cout << "rows = " << rows << "\n";
    cout << "cols = " << cols << "\n";

    // SquareMaze m(rows, cols);
    HexMaze m(32, cols);

    {
        // CreateMazeWilson<SquareMaze> maze_gen(0);
        CreateMazeWilson<HexMaze> maze_gen(0);
        maze_gen.createMaze(m);

        std::ofstream ofs;
        ofs.open("test.svg", std::ofstream::out);

        SvgPainter painter(ofs);
        // m.Draw(&painter, 40, 40, 50, 50);
        m.Draw(&painter, 20, 50, 10);
    }

    return 0;
}
