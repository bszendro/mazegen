import { assert } from 'chai';
import { HexMaze } from '../src/lib/maze/HexMaze';
import { EdgeStatus, NodeStatus } from '../src/lib/maze/MazeGrid';

describe('HexMaze', () => {
    it('should initialize all nodes to be open', () => {
        const m = new HexMaze(3, 3);

        for (let i = 0; i < 3; i++) {
            for (let j = 0; j < 3; j++) {
                assert.strictEqual(m.getNode({i, j}), NodeStatus.Open);
            }
        }
    });

    it('should not allow to get across the border of the maze', () => {
        const m = new HexMaze(3, 3);

        assert.deepStrictEqual(m.getOpenEdges({i: 0, j: 0}), [2, 3]);
        assert.deepStrictEqual(m.getOpenEdges({i: 0, j: 1}), [1, 2, 3, 4, 6]);
        assert.deepStrictEqual(m.getOpenEdges({i: 0, j: 2}), [1, 2]);
        assert.deepStrictEqual(m.getOpenEdges({i: 1, j: 0}), [2, 3, 4, 5]);
        assert.deepStrictEqual(m.getOpenEdges({i: 1, j: 1}), [1, 2, 3, 4, 5, 6]);
        assert.deepStrictEqual(m.getOpenEdges({i: 1, j: 2}), [1, 2, 5, 6]);
        assert.deepStrictEqual(m.getOpenEdges({i: 2, j: 0}), [3, 4, 5]);
        assert.deepStrictEqual(m.getOpenEdges({i: 2, j: 1}), [4, 5, 6]);
        assert.deepStrictEqual(m.getOpenEdges({i: 2, j: 2}), [1, 5, 6]);
    });

    it('should return an open node as long as there is one', () => {
        const m = new HexMaze(2, 1);

        const node1 = m.getOpenNode();
        assert.notStrictEqual(node1, null);
        m.setNode(node1!, NodeStatus.Visited);
        const node2 = m.getOpenNode();
        assert.notStrictEqual(node2, null);
        assert.notStrictEqual(node1, node2);
        m.setNode(node2!, NodeStatus.Visited);
        assert.strictEqual(m.getOpenNode(), null);
    });

    it('should use the same indexes under the hood for getNode/setNode', () => {
        const m = new HexMaze(2, 2);
        const node = {i: 0, j: 0};

        assert.strictEqual(m.getNode(node), NodeStatus.Open);
        m.setNode(node, NodeStatus.Visited);
        assert.strictEqual(m.getNode(node), NodeStatus.Visited);
        m.setNode(node, NodeStatus.OnPath);
        assert.strictEqual(m.getNode(node), NodeStatus.OnPath);
        m.setNode(node, NodeStatus.Open);
        assert.strictEqual(m.getNode(node), NodeStatus.Open);
    });

    it('should use the same indexes under the hood for getOpenEdges/setEdge', () => {
        [
            {i: 1, j: 1},
            {i: 1, j: 2},
            {i: 2, j: 1},
            {i: 2, j: 2},
        ].forEach(node => {
            const m = new HexMaze(4, 4);

            assert.deepStrictEqual(m.getOpenEdges(node), [1, 2, 3, 4, 5, 6]);
            for (let edge = 1; edge <= 6; edge++) {
                // A visited edge is not open anymore
                m.setEdge(node, edge, EdgeStatus.Visited);
                const remainingEdges = Array.from({ length: 6 - edge }, (value, index) => edge + index + 1);
                assert.deepStrictEqual(m.getOpenEdges(node), remainingEdges);
            }
        });
    });

    it('should have each edge to match its corresponding edge of the adjacent node', () => {
        const m = new HexMaze(2, 2);

        [
            {node: {i: 0, j: 0}, edgeOut: 1, edgeIn: 4},
            {node: {i: 0, j: 0}, edgeOut: 2, edgeIn: 5},
            {node: {i: 0, j: 0}, edgeOut: 3, edgeIn: 6},
            {node: {i: 0, j: 0}, edgeOut: 4, edgeIn: 1},
            {node: {i: 0, j: 0}, edgeOut: 5, edgeIn: 2},
            {node: {i: 0, j: 0}, edgeOut: 6, edgeIn: 3},
            // Check both the next row and the next column to see if i and j are
            // mixed up in the edge index functions
            {node: {i: 0, j: 1}, edgeOut: 1, edgeIn: 4},
            {node: {i: 0, j: 1}, edgeOut: 2, edgeIn: 5},
            {node: {i: 0, j: 1}, edgeOut: 3, edgeIn: 6},
            {node: {i: 0, j: 1}, edgeOut: 4, edgeIn: 1},
            {node: {i: 0, j: 1}, edgeOut: 5, edgeIn: 2},
            {node: {i: 0, j: 1}, edgeOut: 6, edgeIn: 3},

            {node: {i: 1, j: 0}, edgeOut: 1, edgeIn: 4},
            {node: {i: 1, j: 0}, edgeOut: 2, edgeIn: 5},
            {node: {i: 1, j: 0}, edgeOut: 3, edgeIn: 6},
            {node: {i: 1, j: 0}, edgeOut: 4, edgeIn: 1},
            {node: {i: 1, j: 0}, edgeOut: 5, edgeIn: 2},
            {node: {i: 1, j: 0}, edgeOut: 6, edgeIn: 3},

            {node: {i: 1, j: 1}, edgeOut: 1, edgeIn: 4},
            {node: {i: 1, j: 1}, edgeOut: 2, edgeIn: 5},
            {node: {i: 1, j: 1}, edgeOut: 3, edgeIn: 6},
            {node: {i: 1, j: 1}, edgeOut: 4, edgeIn: 1},
            {node: {i: 1, j: 1}, edgeOut: 5, edgeIn: 2},
            {node: {i: 1, j: 1}, edgeOut: 6, edgeIn: 3},
        ].forEach(({node, edgeOut, edgeIn}) => {
            m.setEdge(node, edgeOut, EdgeStatus.Visited);
            assert.strictEqual(m.getEdge(m.nextNode(node, edgeOut), edgeIn), EdgeStatus.Visited);
            m.setEdge(node, edgeOut, EdgeStatus.Open);
        });
    });

    it('should return the correct next node along an edge', () => {
        const m = new HexMaze(1, 1);

        [
            {node: {i: 0, j: 0}, edge: 1, result: {i: 0, j: -1}},
            {node: {i: 0, j: 0}, edge: 2, result: {i: 1, j: 0}},
            {node: {i: 0, j: 0}, edge: 3, result: {i: 0, j: 1}},
            {node: {i: 0, j: 0}, edge: 4, result: {i: -1, j: 1}},
            {node: {i: 0, j: 0}, edge: 5, result: {i: -1, j: 0}},
            {node: {i: 0, j: 0}, edge: 6, result: {i: -1, j: -1}},
            // Check both the next row and the next column to see if i and j are
            // mixed up in nextNode
            {node: {i: 0, j: 1}, edge: 1, result: {i: 1, j: 0}},
            {node: {i: 0, j: 1}, edge: 2, result: {i: 1, j: 1}},
            {node: {i: 0, j: 1}, edge: 3, result: {i: 1, j: 2}},
            {node: {i: 0, j: 1}, edge: 4, result: {i: 0, j: 2}},
            {node: {i: 0, j: 1}, edge: 5, result: {i: -1, j: 1}},
            {node: {i: 0, j: 1}, edge: 6, result: {i: 0, j: 0}},

            {node: {i: 1, j: 0}, edge: 1, result: {i: 1, j: -1}},
            {node: {i: 1, j: 0}, edge: 2, result: {i: 2, j: 0}},
            {node: {i: 1, j: 0}, edge: 3, result: {i: 1, j: 1}},
            {node: {i: 1, j: 0}, edge: 4, result: {i: 0, j: 1}},
            {node: {i: 1, j: 0}, edge: 5, result: {i: 0, j: 0}},
            {node: {i: 1, j: 0}, edge: 6, result: {i: 0, j: -1}},

            {node: {i: 1, j: 1}, edge: 1, result: {i: 2, j: 0}},
            {node: {i: 1, j: 1}, edge: 2, result: {i: 2, j: 1}},
            {node: {i: 1, j: 1}, edge: 3, result: {i: 2, j: 2}},
            {node: {i: 1, j: 1}, edge: 4, result: {i: 1, j: 2}},
            {node: {i: 1, j: 1}, edge: 5, result: {i: 0, j: 1}},
            {node: {i: 1, j: 1}, edge: 6, result: {i: 1, j: 0}},
        ].forEach(({node, edge, result}) => {
            assert.deepStrictEqual(m.nextNode(node, edge), result);
        });

        // When the edge is invalid, an error should be thrown
        [
            {node: {i: 0, j: 0}, edge: 0},
            {node: {i: 0, j: 0}, edge: 7},
        ].forEach(({node, edge}) => {
            assert.throws(() => m.nextNode(node, edge), Error, "Invalid edge");
        });
    });

    it('should not allow to enter into invalid regions', () => {
        const m = new HexMaze(3, 3);
        const node = {i: 1, j: 1};

        assert.deepStrictEqual(m.getOpenEdges(node), [1, 2, 3, 4, 5, 6]);
        m.invalidateRegion({i: 2, j: 0}, {i: 2, j: 0});
        assert.deepStrictEqual(m.getOpenEdges(node), [2, 3, 4, 5, 6]);
        m.invalidateRegion({i: 2, j: 1}, {i: 2, j: 1});
        assert.deepStrictEqual(m.getOpenEdges(node), [3, 4, 5, 6]);
        m.invalidateRegion({i: 2, j: 2}, {i: 2, j: 2});
        assert.deepStrictEqual(m.getOpenEdges(node), [4, 5, 6]);
        m.invalidateRegion({i: 1, j: 2}, {i: 1, j: 2});
        assert.deepStrictEqual(m.getOpenEdges(node), [5, 6]);
        m.invalidateRegion({i: 0, j: 1}, {i: 0, j: 1});
        assert.deepStrictEqual(m.getOpenEdges(node), [6]);
        m.invalidateRegion({i: 1, j: 0}, {i: 1, j: 0});
        assert.deepStrictEqual(m.getOpenEdges(node), []);
    });

    // TODO: test drawing
});
