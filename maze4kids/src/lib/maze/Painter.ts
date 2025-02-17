
export interface Point2D {
    x: number;
    y: number;
}

export enum PaintStyle {
    /** Cell not in the maze */
    OpenCell,
    /** Cell added to the maze */
    VisitedCell,
    /** Cell on the current random path */
    OnPathCell,
    /** Wall that is removable */
    Wall,
    /** Wall that is not removable */
    WallBlocked,
}

export interface Painter {
    BeginDraw(width: number, height: number): void;
    EndDraw(): void;
    DrawLine(p1: Point2D, p2: Point2D, style: PaintStyle): void;
    DrawPoly(vertices: Point2D[], style: PaintStyle): void;
}

export interface DrawParams {
    /** Width of a cell */
    cellWidth: number;
    /** Height of a cell */
    cellHeight: number;
    /** Wall width */
    strokeWidth: number;
};
