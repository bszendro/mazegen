import { NodeStatus, EdgeStatus, type NodeIndex, type EdgeIndex, type GridSize } from './MazeGrid';
import { type DrawParams, PaintStyle, type Painter, type Point2D } from './Painter';

const NODE_OPEN = NodeStatus.Open;
const NODE_VISITED = NodeStatus.Visited;
const NODE_ONPATH = NodeStatus.OnPath;
const NODE_INVALID = -1;

const EDGE_OPEN = EdgeStatus.Open;
const EDGE_ONPATH = EdgeStatus.OnPath;
const EDGE_INVALID = -1;

const E1 = (edges: number[][], i: number, j: number) => edges[i + 1][2 * j + 2]; // direction S
const E2 = (edges: number[][], i: number, j: number) => edges[i + 1][2 * j + 3]; // direction E
const E3 = (edges: number[][], i: number, j: number) => edges[i][2 * j + 2];     // direction N
const E4 = (edges: number[][], i: number, j: number) => edges[i + 1][2 * j + 1]; // direction W

interface PointParams {
    center: Point2D;
    cellWidth: number;
    cellHeight: number;
}

const P1 = (p: PointParams): Point2D => ({ x: p.center.x, y: p.center.y + p.cellHeight });
const P2 = (p: PointParams): Point2D => ({ x: p.center.x + p.cellWidth, y: p.center.y + p.cellHeight });
const P3 = (p: PointParams): Point2D => ({ x: p.center.x + p.cellWidth, y: p.center.y });
const P4 = (p: PointParams): Point2D => ({ x: p.center.x, y: p.center.y });

export class SquareMaze {
    private rows: number;
    private cols: number;
    private nodes: number[][];
    private edges: number[][];

    constructor(rows: number, cols: number) {
        this.rows = rows;
        this.cols = cols;
        this.nodes = Array.from({ length: rows }, () => Array(cols).fill(NODE_OPEN));
        this.edges = Array.from({ length: rows + 1 }, () => Array(2 * (cols + 2)).fill(EDGE_OPEN));
        this.invalidateRegionEdges({ i: 0, j: 0 }, { i: rows - 1, j: cols - 1 });
    }

    private static isEdgeVisible(edge: number): boolean {
        return edge === EDGE_OPEN || edge === EDGE_INVALID;
    }

    private static edgeStyle(edge: number): PaintStyle {
        switch (edge) {
            case EDGE_OPEN: return PaintStyle.Wall;
            case EDGE_INVALID: return PaintStyle.WallBlocked;
            default:
                throw new Error("Invalid edge");
        }
    }

    private static nodeStyle(node: number): PaintStyle {
        switch (node) {
            case NODE_OPEN: return PaintStyle.OpenCell;
            case NODE_VISITED: return PaintStyle.VisitedCell;
            case NODE_ONPATH: return PaintStyle.OnPathCell;
            case NODE_INVALID: return PaintStyle.OpenCell;
            default:
                throw new Error("Invalid node");
        }
    }

    private static nodeCenter(node: NodeIndex, cellWidth: number, cellHeight: number, padding_x: number, padding_y: number): Point2D {
        const x_0 = cellWidth * node.j + padding_x;
        const y_0 = cellHeight * node.i + padding_y;
        return { x: x_0, y: y_0 };
    }

    public Draw(painter: Painter, p: DrawParams): void {
        const cellWidth = p.cellWidth;
        const cellHeight = p.cellHeight;
        const padding_x = p.strokeWidth / 2;
        const padding_y = p.strokeWidth / 2;
        const width = cellWidth * this.cols + 2 * padding_x;
        const height = cellHeight * this.rows + 2 * padding_y;
        painter.BeginDraw(width, height);

        // Cells
        for (let i = 0; i < this.rows; i++) {
            for (let j = 0; j < this.cols; j++) {
                const c = SquareMaze.nodeCenter({ i, j }, cellWidth, cellHeight, padding_x, padding_y);
                const p = { center: c, cellWidth, cellHeight };
                const p1 = P1(p);
                const p2 = P2(p);
                const p3 = P3(p);
                const p4 = P4(p);

                painter.DrawPoly([p4, p3, p2, p1], SquareMaze.nodeStyle(this.nodes[i][j]));
            }
        }

        // Left side walls
        for (let i = 0; i < this.rows; i++) {
            const j = 0;
            const c = SquareMaze.nodeCenter({ i, j }, cellWidth, cellHeight, padding_x, padding_y);
            const p = { center: c, cellWidth, cellHeight };
            const p1 = P1(p);
            const p4 = P4(p);

            const e4 = E4(this.edges, i, j);
            if (SquareMaze.isEdgeVisible(e4)) {
                painter.DrawLine(p4, p1, SquareMaze.edgeStyle(e4));
            }
        }

        // Top side walls
        for (let j = 0; j < this.cols; j++) {
            const i = 0;
            const c = SquareMaze.nodeCenter({ i, j }, cellWidth, cellHeight, padding_x, padding_y);
            const p = { center: c, cellWidth, cellHeight };
            const p3 = P3(p);
            const p4 = P4(p);

            const e3 = E3(this.edges, i, j);
            if (SquareMaze.isEdgeVisible(e3)) {
                painter.DrawLine(p3, p4, SquareMaze.edgeStyle(e3));
            }
        }

        // Other walls for valid nodes
        for (let i = 0; i < this.rows; i++) {
            for (let j = 0; j < this.cols; j++) {
                if (this.nodes[i][j] === NODE_INVALID) {
                    continue;
                }

                const c = SquareMaze.nodeCenter({ i, j }, cellWidth, cellHeight, padding_x, padding_y);
                const p = { center: c, cellWidth, cellHeight };
                const p1 = P1(p);
                const p2 = P2(p);
                const p3 = P3(p);

                const e1 = E1(this.edges, i, j);
                const e2 = E2(this.edges, i, j);
                if (SquareMaze.isEdgeVisible(e1)) {
                    painter.DrawLine(p1, p2, SquareMaze.edgeStyle(e1));
                }
                if (SquareMaze.isEdgeVisible(e2)) {
                    painter.DrawLine(p2, p3, SquareMaze.edgeStyle(e2));
                }
            }
        }

        // Other walls for invalid nodes
        for (let i = 0; i < this.rows; i++) {
            for (let j = 0; j < this.cols; j++) {
                if (this.nodes[i][j] !== NODE_INVALID) {
                    continue;
                }

                // Check if there is a valid neighbour
                let next = this.nextNode({ i, j }, 1);
                const b1 = !this.nodeExists(next) || this.nodes[next.i][next.j] !== NODE_INVALID;
                next = this.nextNode({ i, j }, 2);
                const b2 = !this.nodeExists(next) || this.nodes[next.i][next.j] !== NODE_INVALID;
                if (!b1 && !b2) {
                    continue;
                }

                const c = SquareMaze.nodeCenter({ i, j }, cellWidth, cellHeight, padding_x, padding_y);
                const p = { center: c, cellWidth, cellHeight };
                const p1 = P1(p);
                const p2 = P2(p);
                const p3 = P3(p);

                const e1 = E1(this.edges, i, j);
                const e2 = E2(this.edges, i, j);
                if (b1 && SquareMaze.isEdgeVisible(e1)) {
                    painter.DrawLine(p1, p2, SquareMaze.edgeStyle(e1));
                }
                if (b2 && SquareMaze.isEdgeVisible(e2)) {
                    painter.DrawLine(p2, p3, SquareMaze.edgeStyle(e2));
                }
            }
        }

        painter.EndDraw();
    }

    public getNode(node: NodeIndex): NodeStatus {
        return this.nodes[node.i][node.j];
    }

    public setNode(node: NodeIndex, status: NodeStatus): void {
        this.nodes[node.i][node.j] = status;
    }

    public getOpenEdges(node: NodeIndex): EdgeIndex[] {
        const i = node.i;
        const j = node.j;
        const e1 = E1(this.edges, i, j);
        const e2 = E2(this.edges, i, j);
        const e3 = E3(this.edges, i, j);
        const e4 = E4(this.edges, i, j);

        let edges: EdgeIndex[] = [];
        if (e1 === EDGE_OPEN || e1 === EDGE_ONPATH) edges.push(1);
        if (e2 === EDGE_OPEN || e2 === EDGE_ONPATH) edges.push(2);
        if (e3 === EDGE_OPEN || e3 === EDGE_ONPATH) edges.push(3);
        if (e4 === EDGE_OPEN || e4 === EDGE_ONPATH) edges.push(4);
        return edges;
    }

    public setEdge(node: NodeIndex, edge: number, val: EdgeStatus): void {
        const i = node.i;
        const j = node.j;
        const intval = val;
        switch (edge) {
            case 1: this.edges[i + 1][2 * j + 2] = intval; break;
            case 2: this.edges[i + 1][2 * j + 3] = intval; break;
            case 3: this.edges[i][2 * j + 2] = intval; break;
            case 4: this.edges[i + 1][2 * j + 1] = intval; break;
            default:
                throw new Error("Invalid edge");
        }
    }

    public getEdge(node: NodeIndex, edge: number): EdgeStatus {
        const i = node.i;
        const j = node.j;
        let intval = 0;
        switch (edge) {
            case 1: intval = this.edges[i + 1][2 * j + 2]; break;
            case 2: intval = this.edges[i + 1][2 * j + 3]; break;
            case 3: intval = this.edges[i][2 * j + 2]; break;
            case 4: intval = this.edges[i + 1][2 * j + 1]; break;
            default:
                throw new Error("Invalid edge");
        }
        return intval;
    }

    public getOpenNode(): NodeIndex | null {
        for (let i = 0; i < this.nodes.length; i++) {
            for (let j = 0; j < this.nodes[i].length; j++) {
                if (this.nodes[i][j] === EDGE_OPEN) {
                    return { i, j };
                }
            }
        }
        return null;
    }

    public nodeExists(node: NodeIndex): boolean {
        return 0 <= node.i && node.i < this.rows &&
            0 <= node.j && node.j < this.cols;
    }

    public nextNode(node: NodeIndex, edge: number): NodeIndex {
        const i = node.i;
        const j = node.j;
        switch (edge) {
            case 1: return { i: i + 1, j };
            case 2: return { i, j: j + 1 };
            case 3: return { i: i - 1, j };
            case 4: return { i, j: j - 1 };
            default:
                throw new Error("Invalid edge");
        }
    }

    public invalidateRegionEdges(topLeft: NodeIndex, bottomRight: NodeIndex): void {
        if (topLeft.i > bottomRight.i || topLeft.j > bottomRight.j) {
            throw new Error("Invalid region");
        }

        for (let i = topLeft.i; i <= bottomRight.i; i++) {
            {
                // Left vertical line
                const j = topLeft.j;
                this.edges[i + 1][2 * j + 1] = EDGE_INVALID;
            }
            {
                // Right vertical line
                const j = bottomRight.j;
                this.edges[i + 1][2 * j + 3] = EDGE_INVALID;
            }
        }

        for (let j = topLeft.j; j <= bottomRight.j; j++) {
            {
                // Top horizontal line
                const i = topLeft.i;
                this.edges[i][2 * j + 2] = EDGE_INVALID;
            }
            {
                // Bottom horizontal line
                const i = bottomRight.i;
                this.edges[i + 1][2 * j + 2] = EDGE_INVALID;
            }
        }
    }

    public invalidateRegion(topLeft: NodeIndex, bottomRight: NodeIndex): void {
        this.invalidateRegionEdges(topLeft, bottomRight);

        for (let i = topLeft.i; i <= bottomRight.i; i++) {
            for (let j = topLeft.j; j <= bottomRight.j; j++) {
                this.nodes[i][j] = NODE_INVALID;
            }
        }
    }

    public static ComputeGridSize(areaWidth: number, areaHeight: number, cellWidth: number, cellHeight: number, strokeWidth: number): GridSize {
        const cols = Math.floor((areaWidth - strokeWidth) / cellWidth);
        const rows = Math.floor((areaHeight - strokeWidth) / cellHeight);
        return { rows, cols };
    }

    public AddExits(): void {
        this.setEdge({ i: 0, j: 0 }, 3, EdgeStatus.Visited);
        this.setEdge({ i: this.rows - 1, j: this.cols - 1 }, 1, EdgeStatus.Visited);
    }
}
