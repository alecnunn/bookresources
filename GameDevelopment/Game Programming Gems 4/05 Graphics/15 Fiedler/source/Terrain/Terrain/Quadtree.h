// Quadtree class

#pragma once

#include "Frustum.h"
#include "Horizon.h"
#include "Terrain.h"
#include "QuadtreeNode.h"
#include "QuadtreeLevel.h"

using namespace Mathematics;


/// A quadtree of nodes forming min/max planes that bound heightfield data z=f(x,y).
/// Implements the terrain approximation quadtree as described in the article.

class Quadtree
{
public:

    /// Initialize the quadtree fitting the supplied heightfield data.

    void initialize(System::Display *display, Heightfield &heightfield, int resolution)
    {
        this->display = display;

        // create material

        material = display->createMaterial();
        material->ambient(Color(1,1,1));
        material->diffuse(Color(1,1,1));
        material->specular(Color(0,0,0));
        material->power(0);

        // heightfield must be square!

        assert(heightfield.width()==heightfield.height());

        this->resolution = resolution;
        this->heightfield = &heightfield;

        // calculate number of node levels and total number of nodes in quadtree
        
        const int leafSize = resolution;

        int total = 0;
        int width = (heightfield.width() - 1) / leafSize;
        int height = (heightfield.height() - 1) / leafSize;
        int levelCount = 0;
        do
        {
            total += width * height;

            width >>= 1;
            height >>= 1;

            levelCount++;
        }
		while (width>0 && height>0);

        // allocate nodes

		nodes.resize(total);
        
        // store information about node levels

        levels.resize(levelCount);

        width = (heightfield.width() - 1) / leafSize;
        height = (heightfield.height() - 1) / leafSize;

        for (int l=levelCount-1; l>=0; l--)
        {
            levels[l].width = width;
            levels[l].height = height;
            levels[l].delta = (heightfield.width()-1) / width;
            levels[l].size = width * height;

            width >>= 1;
            height >>= 1;
        }

        // funky stride calculations

        int stride = 1;

        for (int l=levelCount-1; l>0; l--)
        {
            levels[l].stride = stride;

            stride = stride*4 + 1;
        }

        assert(stride==total);

        levels[0].stride = stride;

        rebuild();
    }

    /// quadtree build type enumeration

    enum BuildType
    {
        TOPDOWN,        ///< standard top-down build
        BOTTOMUP        ///< optimized bottom-up build as described in "Making it Dynamic" section
    };

    /// rebuild entire quadtree using specified method.

    void rebuild(BuildType type=TOPDOWN)
    {
        float x1,y1,x2,y2;
        heightfield->vectorAtIndex(0, 0, x1, y1);
        heightfield->vectorAtIndex(heightfield->width()-1, heightfield->height()-1, x2, y2);

        if (type==TOPDOWN)
        {
            buildTopDownRecurse(0, 0, levels[0].stride, 0, 0, 1, 1, x1, y1, x2, y2);
        }
        else
        {
            LeastSquaresSums sums;
            buildBottomUpRecurse(0, 0, levels[0].stride, 0, 0, 1, 1, x1, y1, x2, y2, sums);
        }
    }

    /// render quadtree nodes in wireframe to given error threshold.

    void render(const Frustum &frustum, float threshold)
    {
        display->blendMode(System::Display::Blend::ADD);
        
        display->selectMaterial(material);

        display->beginLineStream();

        float x1,y1,x2,y2;
        heightfield->vectorAtIndex(0, 0, x1, y1);
        heightfield->vectorAtIndex(heightfield->width()-1, heightfield->height()-1, x2, y2);

        renderRecurse(0, 0, levels[0].stride, x1, y1, x2, y2, frustum, threshold);

        display->endLineStream();

        display->selectMaterial(0);

        display->blendMode(System::Display::Blend::NONE);
    }

    /// determine visible terrain tiles using horizon culling operating at specified error threshold.

    void visibleTerrainTiles(Horizon &horizon, const Frustum &frustum, std::vector<int> &tiles, int tileSize, int tileResolution, float threshold)
    {
        int tileLevel = -1;

        for (unsigned int i=0; i<levels.size(); i++)
        {
            if (levels[i].delta==tileSize)
            {
                tileLevel = i;
                break;
            }
        }

        assert(tileLevel>=0);
        assert(tileLevel<(int)levels.size());

        horizon.clear();

        float x1,y1,x2,y2;
        heightfield->vectorAtIndex(0, 0, x1, y1);
        heightfield->vectorAtIndex(heightfield->width()-1, heightfield->height()-1, x2, y2);

        visibleTerrainTilesRecurse(0, 0, levels[0].stride, 0, 0, 1, 1, x1, y1, x2, y2, horizon, frustum, tiles, (unsigned) tileLevel, tileResolution, threshold, false);
    }

    typedef std::vector<QuadtreeNode> Nodes;           ///< typedef for a vector of quadtree nodes for easy access and iteration
    typedef std::vector<QuadtreeLevel> Levels;         ///< typedef for a vector of quadtree levels for easy access and iteration

protected:

    inline void renderWireframeQuad(System::Display *display, const Vector &a, const Vector &b, const Vector &c, const Vector &d, const Vector &normal = Vector(0,0,1))
    {
        display->renderLine(a,b);
        display->renderLine(b,c);
        display->renderLine(c,d);
        display->renderLine(d,a);
    }

    void renderSlab(System::Display *display, const Mathematics::Slab &slab)
    {
        // build slab vertices

	    const float nx_x1 = slab.normal.x * slab.x1;
        const float ny_y1 = slab.normal.y * slab.y1;
        const float nx_x2 = slab.normal.x * slab.x2;
        const float ny_y2 = slab.normal.y * slab.y2;

	    Vector vertex[8];
        vertex[0] = Vector(slab.x1, slab.y1, slab.maximum - nx_x1 - ny_y1);
        vertex[1] = Vector(slab.x2, slab.y1, slab.maximum - nx_x2 - ny_y1);
        vertex[2] = Vector(slab.x2, slab.y2, slab.maximum - nx_x2 - ny_y2);
        vertex[3] = Vector(slab.x1, slab.y2, slab.maximum - nx_x1 - ny_y2);
        vertex[4] = Vector(slab.x1, slab.y1, slab.minimum - nx_x1 - ny_y1);
        vertex[5] = Vector(slab.x2, slab.y1, slab.minimum - nx_x2 - ny_y1);
        vertex[6] = Vector(slab.x2, slab.y2, slab.minimum - nx_x2 - ny_y2);
        vertex[7] = Vector(slab.x1, slab.y2, slab.minimum - nx_x1 - ny_y2);

	    // render slab faces

	    renderWireframeQuad(display, vertex[0], vertex[1], vertex[2], vertex[3]);		// top
	    renderWireframeQuad(display, vertex[4], vertex[5], vertex[6], vertex[7]);		// bottom

	    // connect faces with lines

	    display->renderLine(vertex[0], vertex[4]);
	    display->renderLine(vertex[1], vertex[5]);
	    display->renderLine(vertex[2], vertex[6]);
	    display->renderLine(vertex[3], vertex[7]);
    }

    /// calculate screen space node error.
    /// the error corresponds roughly to the vertical distance between the
    /// minimum and maximum nodes in screen space, hence the scale by (1-direction.z).

    float screenSpaceError(const QuadtreeNode &node, float cx, float cy, const Vector &eye, const Vector &direction, float k)
    {
        // determine node screen space error

        const float dx = cx - eye.x;
        const float dy = cx - eye.y;
        const float dz = eye.z;

        const float inverseDistance = Float::inverse_sqrt(dx*dx+dy*dy+dz*dz);
        
        return node.getError() * inverseDistance * k * (1.0f-Float::abs(direction.z));
    }

    /// recursive worker function for rendering quadtree nodes.

    void renderRecurse( unsigned int index, unsigned int level, unsigned int stride, 
                        float world_x1, float world_y1, float world_x2, float world_y2,
                        const Frustum &frustum, float threshold )
    {
        // determine screen space error

        QuadtreeNode &node = nodes[index];

        const float error = screenSpaceError(node, (world_x1+world_x2)*0.5f, (world_y1+world_y2)*0.5f, frustum.eye, frustum.direction, frustum.k);

        // render if error threshold is acceptable or leaf node

        if (error<=threshold || level==levels.size()-1)
        {
            // render node in wireframe

            Mathematics::Slab slab = node.slab(world_x1, world_y1, world_x2, world_y2);
            renderSlab(display, slab);
        }
        else
        {
            // recurse into children

            level ++;

            stride = (stride - 1) >> 2;

            index ++;

            const float world_ax = world_x1;
            const float world_cx = world_x2;
            const float world_bx = (world_x1+world_x2)*0.5f;

            const float world_ay = world_y1;
            const float world_cy = world_y2;
            const float world_by = (world_y1+world_y2)*0.5f;

            const int nodeA = index;
            const int nodeB = nodeA + stride;
            const int nodeC = nodeB + stride;
            const int nodeD = nodeC + stride;

            renderRecurse(nodeA, level, stride, world_ax, world_ay, world_bx, world_by, frustum, threshold);       // top left
            renderRecurse(nodeB, level, stride, world_bx, world_ay, world_cx, world_by, frustum, threshold);       // top right
            renderRecurse(nodeC, level, stride, world_ax, world_by, world_bx, world_cy, frustum, threshold);       // bottom left
            renderRecurse(nodeD, level, stride, world_bx, world_by, world_cx, world_cy, frustum, threshold);       // bottom right
        }
    }

    /// quickly whip up a vector 4 class, we need x,y,z,w for clip space!

    class Vector4 : public Mathematics::Vector
    {
    public:

        Vector4(float x, float y, float z, float w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        float w;
    };
   
    /// transform 3 space vector by matrix assuming its w=1: returns homogeneous vector x,y,z,w 

    Vector4 transform(const Matrix &matrix, const Vector &vector)
    {
        return Vector4( vector.x * matrix.m11 + vector.y * matrix.m12 + vector.z * matrix.m13 + matrix.m14,
                        vector.x * matrix.m21 + vector.y * matrix.m22 + vector.z * matrix.m23 + matrix.m24,
		                vector.x * matrix.m31 + vector.y * matrix.m32 + vector.z * matrix.m33 + matrix.m34,
		                vector.x * matrix.m41 + vector.y * matrix.m42 + vector.z * matrix.m43 + matrix.m44 );
    }

    enum ClipCode
    {
	    LEFT = 1,
	    RIGHT = 2,
	    TOP = 4,
	    BOTTOM = 8,
	    NEAR = 16,
	    FAR = 32,
    };

    /// calculate clip code for point in Direct3D style clip coordinates.

    int clipCode(const Vector4 &vector)
    {
		// code bits: [f][n][b][t][r][l]

		int code = 0;

		if (vector.z<=0) code = NEAR;
          
        // note: just near plane clipping here, the rest is done in screen space in the horizon methods

		return code;
    }

    bool clipLine(Vector4 &a, Vector4 &b)
	{
		int code_a = clipCode(a);
		int code_b = clipCode(b);

		if ((code_a | code_b)==0)
		{
			// trivially accept

            return true;
		}
		else if (code_a & code_b)
		{
			// trivially reject

            return false;
		}

		const float dx = b.x - a.x;
		const float dy = b.y - a.y;
		const float dz = b.z - a.z;
		const float dw = b.w - a.w;

        // near plane

        if (code_a & NEAR)
        {
			const float t = - a.z / dz;

			a.x = a.x + dx * t;
			a.y = a.y + dy * t;
			a.z = a.z + dz * t;
			a.w = a.w + dw * t;
        }
        else if (code_b & NEAR)
        {
			const float t = - b.z / dz;

			b.x = a.x + dx * t;
			b.y = a.y + dy * t;
			b.z = a.z + dz * t;
			b.w = a.z + dz * t;
        }

        return true;
    }
    
    /// projects and transforms to viewport space

    void projectAndViewportTransform(Vector4 &vector, float width, float height)
    {
        const float oow = 1.0f / vector.w;
       
        vector.x *= oow;
        vector.y *= oow;

        vector.x = (vector.x+1)*width*0.5f;
        vector.y = (vector.y+1)*height*0.5f;
    }

    /// clip and render line.
    /// inserts line into horizon.

    void clipAndRenderLine(Horizon &horizon, Vector4 a, Vector4 b, float width, float height)
    {
        if (clipLine(a,b))
        {
            projectAndViewportTransform(a, width, height);
            projectAndViewportTransform(b, width, height);

            horizon.insert(a.x, a.y, b.x, b.y);
        }
    }

    /// clip and test line.
    /// returns true if the entire line is below the horizon or outside frustum, false if any part is visible.

    bool clipAndTestLine(Horizon &horizon, Vector4 a, Vector4 b, float width, float height)
    {
        if (clipLine(a,b))
        {
            projectAndViewportTransform(a, width, height);
            projectAndViewportTransform(b, width, height);

            return horizon.test(a.x, a.y, b.x, b.y)==Horizon::BELOW;
        }

        return true;
    }

    /// recursive worker function for determining visible terrain tiles with horizon culling.
    ///
    /// this function is considerably more complex than the pseudocode in the paper, the reason
    /// being that this function determines visible nodes, while that pseudocode simply found the
    /// set of culled nodes -- a subtle difference! its much more efficient this way, as there
    /// should be less visible nodes than culled ones.
    ///
    /// @param index the index of the current quadtree node in the nodes array
    /// @param level the current level of the quadtree, 0 is the first level (a single 1x1 node covering the whole quadtree), then 1, 2, 3 ... up to leaf nodes.
    /// @param stride the number of nodes between each successive child node at this level (swizzled memory format, nodes are stored in memory in recurse order)
    /// @param x1 the left integer coordinate of the current quadtree node in heightfield samples
    /// @param y1 the top integer coordinate of the current quadtree node in heightfield samples
    /// @param x2 the right integer coordinate of the current quadtree node in heightfield samples
    /// @param y2 the bottom integer coordinate of the current quadtree node in heightfield samples
    /// @param world_x1 the left coordinate of the current quadtree node in world coordinates
    /// @param world_y1 the top coordinate of the current quadtree node in world coordinates
    /// @param world_x2 the right coordinate of the current quadtree node in world coordinates
    /// @param world_y2 the bottom coordinate of the current quadtree node in world coordinates
    /// @param horizon the horizon buffer
    /// @param frustum the view frustum
    /// @param tiles the array of visible terrain tiles
    /// @param tileLevel the level in the quadtree at which each node maps 1:1 to a terrain tile
    /// @param tileResolution the resolution of the terrain in tiles (see Terrain::getResolution)
    /// @param threshold the error threshold
    /// @param inserted true if already inserted into horizon (dont insert child nodes when parent is inserted already)

    void visibleTerrainTilesRecurse( unsigned int index, unsigned int level, unsigned int stride, 
                                     int x1, int y1, int x2, int y2,
                                     float world_x1, float world_y1, float world_x2, float world_y2,
                                     Horizon &horizon, 
                                     const Frustum &frustum, 
                                     std::vector<int> &tiles, unsigned int tileLevel, int tileResolution,
                                     float threshold, bool inserted )
    {
        QuadtreeNode &node = nodes[index];

        if (level<=tileLevel)
        { 
            // test node maximum against horizon and stop recursion if entirely below

            Quad minimumQuad = node.quad(world_x1, world_y1, world_x2, world_y2, QuadtreeNode::MINIMUM);
            Quad maximumQuad = node.quad(world_x1, world_y1, world_x2, world_y2, QuadtreeNode::MAXIMUM);

            Vector4 a = transform(frustum.matrix, maximumQuad.a);
            Vector4 b = transform(frustum.matrix, maximumQuad.b);
            Vector4 c = transform(frustum.matrix, maximumQuad.c);
            Vector4 d = transform(frustum.matrix, maximumQuad.d);

            Vector4 e = transform(frustum.matrix, minimumQuad.a);
            Vector4 f = transform(frustum.matrix, minimumQuad.b);
            Vector4 g = transform(frustum.matrix, minimumQuad.c);
            Vector4 h = transform(frustum.matrix, minimumQuad.d);
            
            bool below = clipAndTestLine(horizon, a, b, frustum.width, frustum.height);
            below &= clipAndTestLine(horizon, b, c, frustum.width, frustum.height);
            below &= clipAndTestLine(horizon, c, d, frustum.width, frustum.height);
            below &= clipAndTestLine(horizon, d, a, frustum.width, frustum.height);

            // look at figure 4 again, and notice that min/max planes are not vertically 
            // aligned above each other in screen space because the perspective transform
            // skews them horizontally - this is bad! the extra line checks below are
            // therefore required to ensure proper bounding of the node samples.
            //
            // try commenting them out and watch what happens... oooh that was hard to debug! :(

            below &= clipAndTestLine(horizon, a, e, frustum.width, frustum.height);
            below &= clipAndTestLine(horizon, b, f, frustum.width, frustum.height);
            below &= clipAndTestLine(horizon, c, g, frustum.width, frustum.height);
            below &= clipAndTestLine(horizon, d, h, frustum.width, frustum.height);
            
            if (below)
                return;

            // if at tile level then the tile covered by this node must be visible so add it to tile array

            if (level==tileLevel)
            {
                tiles.push_back(x1+y1*tileResolution);

                // stop now if we have already inserted into horizon

                if (inserted)
                    return;
            }
        }
        
        // test if screen space error is acceptable or leaf node

        bool insertPending = false;

        if (!inserted)
        {
            const float error = screenSpaceError(node, (world_x1+world_x2)*0.5f, (world_y1+world_y2)*0.5f, frustum.eye, frustum.direction, frustum.k);

            if (error<=threshold || level==levels.size()-1)
            {
                // set inserted flag to true

                inserted = true;

                // we cant insert just yet, we must do it after we first recurse to child nodes
                // or we will clobber their checks against the horizon, think about it...

                insertPending = true;
            }
        }

        // recurse into child nodes

        if (level<levels.size()-1)
        {
            // calculate data for traversal

            level ++;

            assert(level<(int)levels.size());

            stride = (stride - 1) >> 2;

            index ++;

            const int ax = x1<<1;            // a | .
            const int cx = x2<<1;            // - b -
            const int bx = (ax+cx)>>1;       // . | c

            const int ay = y1<<1;
            const int cy = y2<<1;
            const int by = (ay+cy)>>1;

            const float world_ax = world_x1;
            const float world_cx = world_x2;
            const float world_bx = (world_x1+world_x2)*0.5f;

            const float world_ay = world_y1;
            const float world_cy = world_y2;
            const float world_by = (world_y1+world_y2)*0.5f;

            // a b
            // c d

            const int nodeA = index;
            const int nodeB = nodeA + stride;
            const int nodeC = nodeB + stride;
            const int nodeD = nodeC + stride;

            // recurse into children in bottom to top order in screen space
            //
            // ...or at least attempt to, this seems to be approximate only, hence
            // there are some artifacts in the culling if you look closely.
            //
            // sorry for the hack, i just plain ran out of time - try to come
            // up with an exact way of traversing in the right order, and
            // any artifacts should go away!

            const float dx = world_bx - frustum.base.x;
            const float dy = world_by - frustum.base.y;

            int mask = 0;

            if (dx<0) mask |= 1;
            if (dy<0) mask |= 2;

            switch (mask)
            {
                case 0:
                    visibleTerrainTilesRecurse(nodeA, level, stride, ax, ay, bx, by, world_ax, world_ay, world_bx, world_by, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    visibleTerrainTilesRecurse(nodeB, level, stride, bx, ay, cx, by, world_bx, world_ay, world_cx, world_by, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    visibleTerrainTilesRecurse(nodeC, level, stride, ax, by, bx, cy, world_ax, world_by, world_bx, world_cy, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    visibleTerrainTilesRecurse(nodeD, level, stride, bx, by, cx, cy, world_bx, world_by, world_cx, world_cy, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    break;

                case 1:
                    visibleTerrainTilesRecurse(nodeB, level, stride, bx, ay, cx, by, world_bx, world_ay, world_cx, world_by, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    visibleTerrainTilesRecurse(nodeA, level, stride, ax, ay, bx, by, world_ax, world_ay, world_bx, world_by, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    visibleTerrainTilesRecurse(nodeD, level, stride, bx, by, cx, cy, world_bx, world_by, world_cx, world_cy, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    visibleTerrainTilesRecurse(nodeC, level, stride, ax, by, bx, cy, world_ax, world_by, world_bx, world_cy, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    break;

                case 2:
                    visibleTerrainTilesRecurse(nodeC, level, stride, ax, by, bx, cy, world_ax, world_by, world_bx, world_cy, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    visibleTerrainTilesRecurse(nodeA, level, stride, ax, ay, bx, by, world_ax, world_ay, world_bx, world_by, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    visibleTerrainTilesRecurse(nodeD, level, stride, bx, by, cx, cy, world_bx, world_by, world_cx, world_cy, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    visibleTerrainTilesRecurse(nodeB, level, stride, bx, ay, cx, by, world_bx, world_ay, world_cx, world_by, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    break;

                case 3:
                    visibleTerrainTilesRecurse(nodeD, level, stride, bx, by, cx, cy, world_bx, world_by, world_cx, world_cy, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    visibleTerrainTilesRecurse(nodeC, level, stride, ax, by, bx, cy, world_ax, world_by, world_bx, world_cy, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    visibleTerrainTilesRecurse(nodeB, level, stride, bx, ay, cx, by, world_bx, world_ay, world_cx, world_by, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    visibleTerrainTilesRecurse(nodeA, level, stride, ax, ay, bx, by, world_ax, world_ay, world_bx, world_by, horizon, frustum, tiles, tileLevel, tileResolution, threshold, inserted);
                    break;
            }
        }

        // insert here, on the way back up as to not interfere with node testing

        if (insertPending)
        {
            // insert node minimum quad in horizon

            Quad quad = node.quad(world_x1, world_y1, world_x2, world_y2, QuadtreeNode::MINIMUM);

            Vector4 a = transform(frustum.matrix, quad.a);
            Vector4 b = transform(frustum.matrix, quad.b);
            Vector4 c = transform(frustum.matrix, quad.c);
            Vector4 d = transform(frustum.matrix, quad.d);
            
            clipAndRenderLine(horizon, a, b, frustum.width, frustum.height);
            clipAndRenderLine(horizon, b, c, frustum.width, frustum.height);
            clipAndRenderLine(horizon, c, d, frustum.width, frustum.height);
            clipAndRenderLine(horizon, d, a, frustum.width, frustum.height);
        }
    }

    /// recursively worker function to build the quadtree top-down.
    ///
    /// @param index the index of the current quadtree node in the nodes array
    /// @param level the current level of the quadtree, 0 is the first level (a single 1x1 node covering the whole quadtree), then 1, 2, 3 ... up to leaf nodes.
    /// @param stride the number of nodes between each successive child node at this level (swizzled memory format, nodes are stored in memory in recurse order)
    /// @param x1 the left integer coordinate of the current quadtree node in heightfield samples
    /// @param y1 the top integer coordinate of the current quadtree node in heightfield samples
    /// @param x2 the right integer coordinate of the current quadtree node in heightfield samples
    /// @param y2 the bottom integer coordinate of the current quadtree node in heightfield samples
    /// @param world_x1 the left coordinate of the current quadtree node in world coordinates
    /// @param world_y1 the top coordinate of the current quadtree node in world coordinates
    /// @param world_x2 the right coordinate of the current quadtree node in world coordinates
    /// @param world_y2 the bottom coordinate of the current quadtree node in world coordinates

    void buildTopDownRecurse( unsigned int index, 
                              unsigned int level, 
                              unsigned int stride, 
                              int x1, int y1, int x2, int y2, 
                              float world_x1, float world_y1, float world_x2, float world_y2 )
    {
        assert(x1!=x2);
        assert(y1!=y2);

        QuadtreeNode &node = nodes[index];

        const int heightfield_x1 = x1 * levels[level].delta;
        const int heightfield_y1 = y1 * levels[level].delta;
        const int heightfield_x2 = x2 * levels[level].delta;
        const int heightfield_y2 = y2 * levels[level].delta;

        LeastSquaresSums sums;
        calculateLeastSquaresSums(*heightfield, heightfield_x1, heightfield_y1, heightfield_x2, heightfield_y2, world_x1, world_y1, sums);

        Vector center;
        node.initialize(sums, *heightfield, heightfield_x1, heightfield_y1, heightfield_x2, heightfield_y2, center);

        if (level<levels.size()-1)
        {
            // parent node

            level ++;

            stride = (stride - 1) >> 2;

            index ++;

            const int ax = x1<<1;            // a | .
            const int cx = x2<<1;            // - b -
            const int bx = (ax+cx)>>1;       // . | c

            const int ay = y1<<1;
            const int cy = y2<<1;
            const int by = (ay+cy)>>1;

            const float world_ax = world_x1;
            const float world_cx = world_x2;
            const float world_bx = (world_x1+world_x2)*0.5f;

            const float world_ay = world_y1;
            const float world_cy = world_y2;
            const float world_by = (world_y1+world_y2)*0.5f;

            const int nodeA = index;
            const int nodeB = nodeA + stride;
            const int nodeC = nodeB + stride;
            const int nodeD = nodeC + stride;

            buildTopDownRecurse(nodeA, level, stride, ax, ay, bx, by, world_ax, world_ay, world_bx, world_by);       // top left
            buildTopDownRecurse(nodeB, level, stride, bx, ay, cx, by, world_bx, world_ay, world_cx, world_by);       // top right
            buildTopDownRecurse(nodeC, level, stride, ax, by, bx, cy, world_ax, world_by, world_bx, world_cy);       // bottom left
            buildTopDownRecurse(nodeD, level, stride, bx, by, cx, cy, world_bx, world_by, world_cx, world_cy);       // bottom right
        }
    }

    /// recursively worker function to build the quadtree bottom-up.
    ///
    /// @param index the index of the current quadtree node in the nodes array
    /// @param level the current level of the quadtree, 0 is the first level (a single 1x1 node covering the whole quadtree), then 1, 2, 3 ... up to leaf nodes.
    /// @param stride the number of nodes between each successive child node at this level (swizzled memory format, nodes are stored in memory in recurse order)
    /// @param x1 the left integer coordinate of the current quadtree node in heightfield samples
    /// @param y1 the top integer coordinate of the current quadtree node in heightfield samples
    /// @param x2 the right integer coordinate of the current quadtree node in heightfield samples
    /// @param y2 the bottom integer coordinate of the current quadtree node in heightfield samples
    /// @param world_x1 the left coordinate of the current quadtree node in world coordinates
    /// @param world_y1 the top coordinate of the current quadtree node in world coordinates
    /// @param world_x2 the right coordinate of the current quadtree node in world coordinates
    /// @param world_y2 the bottom coordinate of the current quadtree node in world coordinates

    void buildBottomUpRecurse( unsigned int index, 
                               unsigned int level, 
                               unsigned int stride, 
                               int x1, int y1, int x2, int y2, 
                               float world_x1, float world_y1, float world_x2, float world_y2,
                               LeastSquaresSums &sums )
    {
        assert(x1!=x2);
        assert(y1!=y2);

        QuadtreeNode &node = nodes[index];

        if (level<levels.size()-1)
        {
            // parent node

            level ++;

            stride = (stride - 1) >> 2;

            index ++;

            const int ax = x1<<1;            // a | .
            const int cx = x2<<1;            // - b -
            const int bx = (ax+cx)>>1;       // . | c

            const int ay = y1<<1;
            const int cy = y2<<1;
            const int by = (ay+cy)>>1;

            const float world_ax = world_x1;
            const float world_cx = world_x2;
            const float world_bx = (world_x1+world_x2)*0.5f;

            const float world_ay = world_y1;
            const float world_cy = world_y2;
            const float world_by = (world_y1+world_y2)*0.5f;

            const int nodeA = index;
            const int nodeB = nodeA + stride;
            const int nodeC = nodeB + stride;
            const int nodeD = nodeC + stride;

            LeastSquaresSums sumsA, sumsB, sumsC, sumsD;

            buildBottomUpRecurse(nodeA, level, stride, ax, ay, bx, by, world_ax, world_ay, world_bx, world_by, sumsA);       // top left
            buildBottomUpRecurse(nodeB, level, stride, bx, ay, cx, by, world_bx, world_ay, world_cx, world_by, sumsB);       // top right
            buildBottomUpRecurse(nodeC, level, stride, ax, by, bx, cy, world_ax, world_by, world_bx, world_cy, sumsC);       // bottom left
            buildBottomUpRecurse(nodeD, level, stride, bx, by, cx, cy, world_bx, world_by, world_cx, world_cy, sumsD);       // bottom right
            
            // in theory there are samples that overlap between child nodes that should be handled correctly,
            // however, in practice their effect is not noticeable so there is no real need to do so - phew!

			sums = sumsA;
            sums += sumsB;
            sums += sumsC;
            sums += sumsD;

            Vector center;
            node.initialize(sums, center);

            const float normalX = node.getNormalX();
            const float normalY = node.getNormalY();
            
            float minimumD = node.getCenterD();
            float maximumD = minimumD;

            nodes[nodeA].pushOut(world_ax, world_ay, world_bx, world_by, normalX, normalY, maximumD, minimumD);
            nodes[nodeB].pushOut(world_bx, world_ay, world_cx, world_by, normalX, normalY, maximumD, minimumD);
            nodes[nodeC].pushOut(world_ax, world_by, world_bx, world_cy, normalX, normalY, maximumD, minimumD);
            nodes[nodeD].pushOut(world_bx, world_by, world_cx, world_cy, normalX, normalY, maximumD, minimumD);

            node.calculateError(maximumD, minimumD);
        }
        else
        {
            // leaf node assuming 1x1 resolution at leaves (!)

            assert(x2-x1==1);
            assert(y2-y1==1);

            calculateLeastSquaresSums_1x1(*heightfield, x1, y1, world_x1, world_y1, sums);

            Vector center;
            node.initialize(sums, *heightfield, x1, y1, x2, y2, center);
        }
    }

private:

    Nodes nodes;                          ///< the array of nodes in swizzled memory layout
    Levels levels;                        ///< array of quadtree level objects

    int resolution;                       ///< resolution of leaf nodes in terrain samples (assumes square nodes)

    Heightfield *heightfield;             ///< the heightfield object from which this quadtree was generated

    System::Display *display;             ///< display object
    System::Material *material;           ///< material for rendering nodes
};
