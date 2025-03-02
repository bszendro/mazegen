import { NodeStatus, EdgeStatus, type NodeIndex, type EdgeIndex, type GridSize } from './MazeGrid';
import { type DrawParams, PaintStyle, type Painter, type Point2D } from './Painter';

const NODE_OPEN = NodeStatus.Open;
const NODE_VISITED = NodeStatus.Visited;
const NODE_ONPATH = NodeStatus.OnPath;
const NODE_INVALID = -1;

const EDGE_OPEN = EdgeStatus.Open;
const EDGE_ONPATH = EdgeStatus.OnPath;
const EDGE_INVALID = -1;

/** Direction SW */
const E1 = (edges: number[][], i: number, j: number) => edges[i + 1][3 * j + 3];
/** Direction S */
const E2 = (edges: number[][], i: number, j: number) => edges[i + 1][3 * j + 4];
/** Direction SE */
const E3 = (edges: number[][], i: number, j: number) => edges[i + 1][3 * j + 5];
/** Direction NE */
const E4 = (edges: number[][], i: number, j: number) => (j % 2 === 0 ? edges[i][3 * j + 6] : edges[i + 1][3 * j + 6]);
/** Direction N */
const E5 = (edges: number[][], i: number, j: number) => edges[i][3 * j + 4];
/** Direction NW */
const E6 = (edges: number[][], i: number, j: number) => (j % 2 === 0 ? edges[i][3 * j + 2] : edges[i + 1][3 * j + 2]);

interface PointParams {
    center: Point2D;
    rad: number;
    h: number;
}

const P1 = (p: PointParams): Point2D => ({ x: p.center.x - p.rad, y: p.center.y });
const P2 = (p: PointParams): Point2D => ({ x: p.center.x - p.rad / 2, y: p.center.y + p.h / 2 });
const P3 = (p: PointParams): Point2D => ({ x: p.center.x + p.rad / 2, y: p.center.y + p.h / 2 });
const P4 = (p: PointParams): Point2D => ({ x: p.center.x + p.rad, y: p.center.y });
const P5 = (p: PointParams): Point2D => ({ x: p.center.x + p.rad / 2, y: p.center.y - p.h / 2 });
const P6 = (p: PointParams): Point2D => ({ x: p.center.x - p.rad / 2, y: p.center.y - p.h / 2 });

export class HexMaze {
    private rows: number;
    private cols: number;
    private nodes: number[][];
    private edges: number[][];

    constructor(rows: number, cols: number) {
        this.rows = rows;
        this.cols = cols;
        this.nodes = Array.from({ length: rows }, () => Array(cols).fill(NODE_OPEN));
        this.edges = Array.from({ length: rows + 1 }, () => Array(3 * (cols + 2)).fill(EDGE_OPEN));
        this.invalidateRegionEdges({ i: 0, j: 0 }, { i: rows - 1, j: cols - 1 });
    }

    private static isEdgeVisible(edge: EdgeIndex): boolean {
        return edge === EDGE_OPEN || edge === EDGE_INVALID;
    }

    private static edgeStyle(edge: EdgeIndex): PaintStyle {
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

    private static nodeCenter(node: NodeIndex, rad: number, h: number, padding_x: number, padding_y: number): Point2D {
        const x_0 = rad + rad * 3 / 2 * node.j + padding_x;
        const y_0 = (node.j % 2 === 0)
            ? h / 2 + h * node.i + padding_y
            : h + h * node.i + padding_y;
        return { x: x_0, y: y_0 };
    }

    public Draw(painter: Painter, p: DrawParams): void {
        const rad = p.cellWidth / 2;
        const h = Math.sqrt(3.0) * rad;
        const padding_x = p.strokeWidth / 2;
        const padding_y = p.strokeWidth / 2;
        const width = rad / 2 + rad * 3 / 2 * this.cols + padding_x * 2;
        const height = h / 2 + h * this.rows + padding_y * 2;
        painter.BeginDraw(width, height);

        if (p.drawCells) {
            // Cells
            for (let i = 0; i < this.rows; i++) {
                for (let j = 0; j < this.cols; j++) {
                    const c = HexMaze.nodeCenter({ i, j }, rad, h, padding_x, padding_y);
                    const p = { center: c, rad, h };
                    const p1 = P1(p);
                    const p2 = P2(p);
                    const p3 = P3(p);
                    const p4 = P4(p);
                    const p5 = P5(p);
                    const p6 = P6(p);

                    painter.DrawPoly([p5, p4, p3, p2, p1, p6], HexMaze.nodeStyle(this.nodes[i][j]));
                }
            }
        }

        // Left side walls (skip top row)
        for (let i = 1; i < this.rows; i++) {
            const j = 0;
            const c = HexMaze.nodeCenter({ i, j }, rad, h, padding_x, padding_y);
            const p = { center: c, rad, h };
            const p1 = P1(p);
            const p6 = P6(p);

            const e6 = E6(this.edges, i, j);
            if (this.nodes[i][j] !== NODE_INVALID && HexMaze.isEdgeVisible(e6)) {
                painter.DrawLine(p6, p1, HexMaze.edgeStyle(e6));
            }
        }

        // Right side walls (skip top row if the number of columns is odd)
        const iStart = (this.cols % 2 === 0) ? 0 : 1;
        for (let i = iStart; i < this.rows; i++) {
            const j = this.cols - 1;
            const c = HexMaze.nodeCenter({ i, j }, rad, h, padding_x, padding_y);
            const p = { center: c, rad, h };
            const p4 = P4(p);
            const p5 = P5(p);

            const e4 = E4(this.edges, i, j);
            if (this.nodes[i][j] !== NODE_INVALID && HexMaze.isEdgeVisible(e4)) {
                painter.DrawLine(p4, p5, HexMaze.edgeStyle(e4));
            }
        }

        // Top side walls (even columns)
        for (let j = 0; j < this.cols; j += 2) {
            const i = 0;
            const c = HexMaze.nodeCenter({ i, j }, rad, h, padding_x, padding_y);
            const p = { center: c, rad, h };
            const p1 = P1(p);
            const p4 = P4(p);
            const p5 = P5(p);
            const p6 = P6(p);

            const e4 = E4(this.edges, i, j);
            const e5 = E5(this.edges, i, j);
            const e6 = E6(this.edges, i, j);
            if (this.nodes[i][j] !== NODE_INVALID && HexMaze.isEdgeVisible(e4)) {
                painter.DrawLine(p4, p5, HexMaze.edgeStyle(e4));
            }
            if (this.nodes[i][j] !== NODE_INVALID && HexMaze.isEdgeVisible(e5)) {
                painter.DrawLine(p5, p6, HexMaze.edgeStyle(e5));
            }
            if (this.nodes[i][j] !== NODE_INVALID && HexMaze.isEdgeVisible(e6)) {
                painter.DrawLine(p6, p1, HexMaze.edgeStyle(e6));
            }
        }

        // Top side walls (odd columns)
        for (let j = 1; j < this.cols; j += 2) {
            const i = 0;
            const c = HexMaze.nodeCenter({ i, j }, rad, h, padding_x, padding_y);
            const p = { center: c, rad, h };
            const p5 = P5(p);
            const p6 = P6(p);

            const e5 = E5(this.edges, i, j);
            if (this.nodes[i][j] !== NODE_INVALID && HexMaze.isEdgeVisible(e5)) {
                painter.DrawLine(p5, p6, HexMaze.edgeStyle(e5));
            }
        }

        // Other walls for valid nodes
        for (let i = 0; i < this.rows; i++) {
            for (let j = 0; j < this.cols; j++) {
                if (this.nodes[i][j] === NODE_INVALID) {
                    continue;
                }

                const c = HexMaze.nodeCenter({ i, j }, rad, h, padding_x, padding_y);
                const p = { center: c, rad, h };
                const p1 = P1(p);
                const p2 = P2(p);
                const p3 = P3(p);
                const p4 = P4(p);

                const e1 = E1(this.edges, i, j);
                const e2 = E2(this.edges, i, j);
                const e3 = E3(this.edges, i, j);
                if (HexMaze.isEdgeVisible(e1)) {
                    painter.DrawLine(p1, p2, HexMaze.edgeStyle(e1));
                }
                if (HexMaze.isEdgeVisible(e2)) {
                    painter.DrawLine(p2, p3, HexMaze.edgeStyle(e2));
                }
                if (HexMaze.isEdgeVisible(e3)) {
                    painter.DrawLine(p3, p4, HexMaze.edgeStyle(e3));
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
                const b1 = this.nodeExists(next) && this.nodes[next.i][next.j] !== NODE_INVALID;
                next = this.nextNode({ i, j }, 2);
                const b2 = this.nodeExists(next) && this.nodes[next.i][next.j] !== NODE_INVALID;
                next = this.nextNode({ i, j }, 3);
                const b3 = this.nodeExists(next) && this.nodes[next.i][next.j] !== NODE_INVALID;
                if (!b1 && !b2 && !b3) {
                    continue;
                }

                const c = HexMaze.nodeCenter({ i, j }, rad, h, padding_x, padding_y);
                const p = { center: c, rad, h };
                const p1 = P1(p);
                const p2 = P2(p);
                const p3 = P3(p);
                const p4 = P4(p);

                const e1 = E1(this.edges, i, j);
                const e2 = E2(this.edges, i, j);
                const e3 = E3(this.edges, i, j);
                if (b1 && HexMaze.isEdgeVisible(e1)) {
                    painter.DrawLine(p1, p2, HexMaze.edgeStyle(e1));
                }
                if (b2 && HexMaze.isEdgeVisible(e2)) {
                    painter.DrawLine(p2, p3, HexMaze.edgeStyle(e2));
                }
                if (b3 && HexMaze.isEdgeVisible(e3)) {
                    painter.DrawLine(p3, p4, HexMaze.edgeStyle(e3));
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
        const e5 = E5(this.edges, i, j);
        const e6 = E6(this.edges, i, j);

        let edges: EdgeIndex[] = [];
        if (e1 === EDGE_OPEN || e1 === EDGE_ONPATH) edges.push(1);
        if (e2 === EDGE_OPEN || e2 === EDGE_ONPATH) edges.push(2);
        if (e3 === EDGE_OPEN || e3 === EDGE_ONPATH) edges.push(3);
        if (e4 === EDGE_OPEN || e4 === EDGE_ONPATH) edges.push(4);
        if (e5 === EDGE_OPEN || e5 === EDGE_ONPATH) edges.push(5);
        if (e6 === EDGE_OPEN || e6 === EDGE_ONPATH) edges.push(6);
        return edges;
    }

    public setEdge(node: NodeIndex, edge: EdgeIndex, status: EdgeStatus): void {
        const i = node.i;
        const j = node.j;
        switch (edge) {
            case 1: this.edges[i + 1][3 * j + 3] = status; break;
            case 2: this.edges[i + 1][3 * j + 4] = status; break;
            case 3: this.edges[i + 1][3 * j + 5] = status; break;
            case 4:
                if (j % 2 === 0) {
                    this.edges[i][3 * j + 6] = status;
                } else {
                    this.edges[i + 1][3 * j + 6] = status;
                }
                break;
            case 5: this.edges[i][3 * j + 4] = status; break;
            case 6:
                if (j % 2 === 0) {
                    this.edges[i][3 * j + 2] = status;
                } else {
                    this.edges[i + 1][3 * j + 2] = status;
                }
                break;
            default:
                throw new Error("Invalid edge");
        }
    }

    public getEdge(node: NodeIndex, edge: EdgeIndex): EdgeStatus {
        const i = node.i;
        const j = node.j;
        let intval = 0;
        switch (edge) {
            case 1: intval = E1(this.edges, i, j); break;
            case 2: intval = E2(this.edges, i, j); break;
            case 3: intval = E3(this.edges, i, j); break;
            case 4: intval = E4(this.edges, i, j); break;
            case 5: intval = E5(this.edges, i, j); break;
            case 6: intval = E6(this.edges, i, j); break;
            default:
                throw new Error("Invalid edge");
        }
        return intval;
    }

    public getOpenNode(): NodeIndex | null {
        for (let i = 0; i < this.nodes.length; i++) {
            for (let j = 0; j < this.nodes[i].length; j++) {
                if (this.nodes[i][j] === NODE_OPEN) {
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

    public nextNode(node: NodeIndex, edge: EdgeIndex): NodeIndex {
        const i = node.i;
        const j = node.j;
        switch (edge) {
            case 1:
                return (j % 2 === 0) ? { i, j: j - 1 } : { i: i + 1, j: j - 1 };
            case 2: return { i: i + 1, j };
            case 3:
                return (j % 2 === 0) ? { i, j: j + 1 } : { i: i + 1, j: j + 1 };
            case 4:
                return (j % 2 === 0) ? { i: i - 1, j: j + 1 } : { i, j: j + 1 };
            case 5: return { i: i - 1, j };
            case 6:
                return (j % 2 === 0) ? { i: i - 1, j: j - 1 } : { i, j: j - 1 };
            default:
                throw new Error("Invalid edge");
        }
    }

    private invalidateRegionEdges(topLeft: NodeIndex, bottomRight: NodeIndex): void {
        if (topLeft.i > bottomRight.i || topLeft.j > bottomRight.j) {
            throw new Error("Invalid region");
        }

        for (let i = topLeft.i; i <= bottomRight.i; i++) {
            {
                // Left vertical line
                const j = topLeft.j;
                this.edges[i + 1][3 * j + 3] = EDGE_INVALID; // E1
                if (j % 2 === 0) {
                    this.edges[i][3 * j + 2] = EDGE_INVALID; // E6
                } else {
                    this.edges[i + 1][3 * j + 2] = EDGE_INVALID; // E6
                }
            }
            {
                // Right vertical line
                const j = bottomRight.j;
                this.edges[i + 1][3 * j + 5] = EDGE_INVALID; // E3
                if (j % 2 === 0) {
                    this.edges[i][3 * j + 6] = EDGE_INVALID; // E4
                } else {
                    this.edges[i + 1][3 * j + 6] = EDGE_INVALID; // E4
                }
            }
        }

        for (let j = topLeft.j; j <= bottomRight.j; j++) {
            {
                // Top horizontal line
                const i = topLeft.i;
                if ((j % 2) === 0) {
                    this.edges[i][3 * j + 6] = EDGE_INVALID; // E4
                    this.edges[i][3 * j + 4] = EDGE_INVALID; // E5
                    this.edges[i][3 * j + 2] = EDGE_INVALID; // E6
                } else {
                    this.edges[i][3 * j + 4] = EDGE_INVALID; // E5
                }
            }
            {
                // Bottom horizontal line
                const i = bottomRight.i;
                if ((j % 2) === 0) {
                    this.edges[i + 1][3 * j + 4] = EDGE_INVALID; // E2
                } else {
                    this.edges[i + 1][3 * j + 3] = EDGE_INVALID; // E1
                    this.edges[i + 1][3 * j + 4] = EDGE_INVALID; // E2
                    this.edges[i + 1][3 * j + 5] = EDGE_INVALID; // E3
                }
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

    public static ComputeGridSize(areaWidth: number, areaHeight: number, cellWidth: number, strokeWidth: number): GridSize {
        const rad = cellWidth / 2;
        // areaWidth = rad/2 + rad*3/2*cols + strokeWidth
        const cols = Math.floor((areaWidth - rad/2 - strokeWidth) / (rad*3/2));
        const h = Math.sqrt(3.0) * rad;
        // areaHeight = h/2 + h*rows + strokeWidth
        const rows = Math.floor((areaHeight - h/2 - strokeWidth) / h);
        return { rows, cols };
    }

    public AddExits(): void {
        this.setEdge({ i: 0, j: 0 }, 6, EdgeStatus.Visited);
        this.setEdge({ i: this.rows - 1, j: this.cols - 1 }, 3, EdgeStatus.Visited);
    }
}
