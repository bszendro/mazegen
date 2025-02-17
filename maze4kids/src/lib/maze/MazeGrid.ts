
export enum NodeStatus {
  Open,
  Visited,
  OnPath,
}

export enum EdgeStatus {
  Open,
  Visited,
  OnPath,
}

export interface NodeIndex {
  i: number;
  j: number;
}

export type EdgeIndex = number;

export interface MazeGrid {
  getOpenNode(): NodeIndex | null;
  getNode(node: NodeIndex): NodeStatus;
  setNode(node: NodeIndex, status: NodeStatus): void;

  getOpenEdges(node: NodeIndex): EdgeIndex[];
  getEdge(node: NodeIndex, edge: EdgeIndex): EdgeStatus;
  setEdge(node: NodeIndex, edge: EdgeIndex, status: EdgeStatus): void;

  nextNode(node: NodeIndex, edge: EdgeIndex): NodeIndex;
}

export interface GridSize {
  rows: number;
  cols: number;
}
