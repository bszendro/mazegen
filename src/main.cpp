#include "gen_wilson.h"
#include "brick_maze.h"
#include "hexmaze.h"
#include "square_maze.h"
#include "svg_painter.h"
#include "maze_grid.h"

#include <boost/program_options.hpp>

#include <stdio.h>

#include <chrono>
#include <iostream>
#include <fstream>

namespace po = boost::program_options;

using namespace std;

template< typename Maze >
static bool save_image(const string& name, const Maze& m,
                       const typename Maze::DrawParams& params, int stroke_width) {
    ofstream ofs;
    ofs.open(name, std::ofstream::out);
    if (ofs.fail()) {
        cerr << "Cannot open output file: " << name << "\n";
        return false;
    }

    SvgPainter painter(ofs, {stroke_width});
    m.Draw(&painter, params);
    return true;
}

// Compute the rectangle available (in pixels) for drawing the maze based on the paper size
static bool getAreaSize(const string& paper_size, int& area_width, int& area_height) {
    const auto size = paper_size.substr(0, 2);
    auto paper_width = 0.0;
    auto paper_height = 0.0;
    if (size == "A3") {
        paper_width = 11.7;
        paper_height = 16.5;
    } else if (size == "A4") {
        paper_width = 8.3;
        paper_height = 11.7;
    } else if (size == "A5") {
        paper_width = 5.8;
        paper_height = 8.3;
    } else {
        cerr << "Invalid size\n";
        return false;
    }

    const auto orientation = paper_size.substr(2);
    if (orientation == "landscape" || orientation == "l") {
        std::swap(paper_width, paper_height);
    } else if (!(orientation.empty() || orientation == "portrait" || orientation == "p")) {
        cerr << "Invalid orientation\n";
        return false;
    }

    const auto margin_x = 0.39; // ~1cm
    const auto margin_y = 0.39; // ~1cm
    const auto ppi = 96;

    area_width = static_cast<int>((paper_width - 2.0*margin_x) * ppi);
    area_height = static_cast<int>((paper_height - 2.0*margin_y) * ppi);
    return true;
}

struct CmdLineParams
{
    string output_filename;
    string shape;
    string paper_size;
    int stroke_width;
    int cell_width;
    int cell_height;
    bool no_maze;
    bool no_exits;
};

int main(int argc, char** argv)
{
    po::options_description desc("Allowed options");
    CmdLineParams params;
    desc.add_options()
        ("help", "Produce this help message")
        ("output-file,o", po::value<string>(&params.output_filename)->default_value("output.svg"), "Output filename")
        ("cell-shape,C", po::value<string>(&params.shape)->default_value("hex"), "Cell shape: hexagonal, square, or brick")
        ("paper-size,s", po::value<string>(&params.paper_size)->default_value("A4"), "Paper size")
        ("stroke-width", po::value<int>(&params.stroke_width)->default_value(4), "Stroke width for walls")
        ("cell-width", po::value<int>(&params.cell_width)->default_value(40), "Cell width")
        ("cell-height", po::value<int>(&params.cell_height)->default_value(40), "Cell height")
        ("no-maze", po::bool_switch(&params.no_maze), "Do not generate a maze, just print the grid (for debugging)")
        ("no-exits", po::bool_switch(&params.no_exits), "Do not add exits at the edges")
        ;
    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
    } catch (po::error& err) {
        cerr << err.what() << "\n";
        return 1;
    }
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    auto area_width = 0;
    auto area_height = 0;
    if (!getAreaSize(params.paper_size, area_width, area_height)) {
        return 1;
    }

    // TODO: Add validation for stroke_width, cell_width, cell_height

    // Create grid with the selected cell shape
    unique_ptr<IMazeGrid> maze;
    if (params.shape == "hex" || params.shape == "hexagonal") {
        const auto &[rows, cols] = HexMaze::ComputeGridSize(area_width, area_height,
                                                            params.cell_width, params.stroke_width);
        maze = make_unique<HexMaze>(rows, cols);
    } else if (params.shape == "sqr" || params.shape == "square") {
        const auto &[rows, cols] = SquareMaze::ComputeGridSize(area_width, area_height,
                                                               params.cell_width, params.cell_height,
                                                               params.stroke_width);
        maze = make_unique<SquareMaze>(rows, cols);
    } else if (params.shape == "brick") {
        const auto &[rows, cols] = BrickMaze::ComputeGridSize(area_width, area_height,
                                                              params.cell_width, params.cell_height,
                                                              params.stroke_width);
        maze = make_unique<BrickMaze>(rows, cols);
    } else {
        cerr << "Invalid cell shape\n";
        return 1;
    }

    if (!params.no_exits) {
        maze->AddExits();
    }

    if (!params.no_maze) {
        // Compute the maze
        const auto random_seed = std::chrono::system_clock::now().time_since_epoch().count();
        maze->CreateMaze(random_seed);
    }

    ofstream ofs;
    ofs.open(params.output_filename, std::ofstream::out);
    if (ofs.fail()) {
        cerr << "Cannot open output file: " << params.output_filename << "\n";
        return 1;
    }

    SvgPainter painter(ofs, {params.stroke_width});
    maze->Draw(painter, {params.cell_width, params.cell_height, params.stroke_width});

    // m.setOnChangeHook([&counter, &m]{
    //     char buffer[64];
    //     snprintf(buffer, sizeof(buffer), "test_%06d.svg", counter);
    //     save_image(buffer, m);
    //     counter++;
    //     if (counter == 1000) exit(0);
    // });

    return 0;
}
