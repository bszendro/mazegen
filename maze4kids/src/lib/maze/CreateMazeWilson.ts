import { NodeStatus, EdgeStatus, type EdgeIndex, type NodeIndex, type MazeGrid } from './MazeGrid';

export enum CreateMazeResult {
    Ok,
    ErrNoFirstOpenNode,
    ErrNoOpenEdges,
}

export interface RandomGenerator {
    (): number;
}

interface PathItem {
    edge: EdgeIndex;
    target_node: NodeIndex;
}

export class CreateMazeWilson {
    private current_path_: PathItem[] = [];
    private random_engine_: () => number;

    constructor(random_generator: RandomGenerator) {
        this.random_engine_ = random_generator;
    }

    public createMaze(maze: MazeGrid): CreateMazeResult {
        // Add a random node to the graph
        const first_node = maze.getOpenNode();
        if (first_node === null) {
            return CreateMazeResult.ErrNoFirstOpenNode;
        }
        maze.setNode(first_node, NodeStatus.Visited);

        while (true) {
            // While there are still open nodes, pick one to start a random path
            const start_node = maze.getOpenNode();
            if (start_node === null) {
                break;
            }
            maze.setNode(start_node, NodeStatus.OnPath);

            // Pick a random edge and make a step to the next node
            let step = this.getRandomStep(maze, start_node);
            if (step === null) {
                return CreateMazeResult.ErrNoOpenEdges;
            }
            maze.setEdge(start_node, step.edge, EdgeStatus.OnPath);
            this.current_path_.length = 0;
            this.current_path_.push(step);

            while (true) {
                let last_node = this.current_path_[this.current_path_.length - 1].target_node;
                const status = maze.getNode(last_node);
                if (status === NodeStatus.Visited) {
                    // Stop if the node at the end of the path is already in the graph
                    break;
                }
                if (status === NodeStatus.OnPath) {
                    // Found a loop, erase it

                    // Delete last item
                    if (this.current_path_.length === 0) {
                        throw new Error("Path is empty");
                    }
                    const src_node = this.current_path_.length >= 2
                        ? this.current_path_[this.current_path_.length - 2].target_node
                        : start_node;
                    maze.setEdge(src_node, this.current_path_[this.current_path_.length - 1].edge, EdgeStatus.Open);
                    this.current_path_.pop();

                    while (this.current_path_.length > 0 && this.current_path_[this.current_path_.length - 1].target_node !== last_node) {
                        maze.setNode(this.current_path_[this.current_path_.length - 1].target_node, NodeStatus.Open);
                        const src_node = this.current_path_.length >= 2
                            ? this.current_path_[this.current_path_.length - 2].target_node
                            : start_node;
                        maze.setEdge(src_node, this.current_path_[this.current_path_.length - 1].edge, EdgeStatus.Open);
                        this.current_path_.pop();
                    }
                    last_node = this.current_path_.length === 0 ? start_node : this.current_path_[this.current_path_.length - 1].target_node;

                } else {
                    maze.setNode(last_node, NodeStatus.OnPath);
                }

                // Pick a random edge and make a step to the next node
                step = this.getRandomStep(maze, last_node);
                if (step === null) {
                    return CreateMazeResult.ErrNoOpenEdges;
                }
                maze.setEdge(last_node, step.edge, EdgeStatus.OnPath);
                this.current_path_.push(step);
            }

            // Add all nodes of the path to the graph
            maze.setNode(start_node, NodeStatus.Visited);
            let prev_node = start_node;
            for (const step of this.current_path_) {
                maze.setEdge(prev_node, step.edge, EdgeStatus.Visited);
                maze.setNode(step.target_node, NodeStatus.Visited);
                prev_node = step.target_node;
            }
        }

        return CreateMazeResult.Ok;
    }

    private getRandomStep(maze: MazeGrid, node: NodeIndex): PathItem | null {
        const open_edges = maze.getOpenEdges(node);
        if (open_edges.length === 0) {
            return null;
        }

        const dist = Math.floor(this.random_engine_() * open_edges.length);
        const edge = open_edges[dist];
        const target_node = maze.nextNode(node, edge);
        return { edge, target_node };
    }
}
