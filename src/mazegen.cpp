#include <iostream>
#include <fstream>

#include "gen_wilson.h"
#include "hexmaze.h"
#include "svg_painter.h"

using namespace std;

int main()
{
    HexMaze m(10, 20);

    {
        CreateMazeWilson<HexMaze> maze_gen(0);
        maze_gen.createMaze(m);

        std::ofstream ofs;
        ofs.open("test.svg", std::ofstream::out);

        SvgPainter painter(ofs);
        m.Draw(&painter, 30, 50, 50);
    }

    return 0;
}
