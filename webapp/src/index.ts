import { CreateMazeWilson } from './CreateMazeWilson';
// import { SquareMaze } from './SquareMaze';
import { SvgPainter } from './SvgPainter';
import { HexMaze } from './HexMaze';

// declare class MersenneTwister {
//     random(): number;
// }

const m = new MersenneTwister(123);
let randomNumber = m.random();


console.log('Hello!', randomNumber);

// const maze = new SquareMaze(40, 40);
const maze = new HexMaze(40, 40);
const generator = new CreateMazeWilson(() => m.random());
generator.createMaze(maze);

const svgElement = document.getElementById('svg');
if (svgElement !== null) {
    const painter = new SvgPainter(svgElement, { strokeWidth: 2 });
    maze.Draw(painter, {cellWidth: 20, cellHeight: 20, strokeWidth: 2});
}
