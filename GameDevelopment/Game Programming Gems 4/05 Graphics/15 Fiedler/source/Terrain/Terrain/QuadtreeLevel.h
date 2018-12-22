// Quadtree Level class

#pragma once


/// Stores information about a level in the quadtree.

class QuadtreeLevel
{
public:

    int width;                  ///< width of this quadtree level in nodes
    int height;                 ///< height of this quadtree level in nodes
    int delta;                  ///< delta in integer heightfield samples for each node
    int size;                   ///< size of this quadtree level in nodes squared
    int stride;                 ///< number of nodes between successive child nodes at this quadtree level
};
