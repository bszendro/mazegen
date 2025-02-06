#pragma once

struct NodeIndex2D
{
    int i;
    int j;

    bool operator==(const NodeIndex2D& rhs) const {
        return i == rhs.i && j == rhs.j;
    }

    bool operator!=(const NodeIndex2D& rhs) {
        return i != rhs.i || j != rhs.j;
    }
};
