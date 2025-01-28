#include <iostream>
#include <fstream>

#include "gen_wilson.h"
#include "hexmaze.h"
#include "svg_painter.h"

using namespace std;

int main()
{
    HexMaze m(300, 200);

    {
        CreateMazeWilson<HexMaze> maze_gen(0);
        maze_gen.createMaze(m);

        std::ofstream ofs;
        ofs.open("test.svg", std::ofstream::out);

        SvgPainter painter(ofs, 3000, 4000);
        m.Draw(&painter, 10, 50, 50);
    }

    return 0;
}
