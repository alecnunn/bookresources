// Terrain Tile class

#pragma once

#include "System/Mesh.h"
#include "System/Display.h"


/// Terrain Tile class.
/// Manages and renders a square terrain tile mesh

class TerrainTile
{
public:

    TerrainTile(System::Display *display, Heightfield &heightfield, int x, int y, int x1, int y1, int x2, int y2)
    {
        this->x = x;
        this->y = y;

        // calculate dimensions

        const int width = (x2 - x1) + 1;
        const int height = (y2 - y1) + 1;

        assert(width==height);

		const int vertex_count = width * height;
		const int index_count = (width-1) * (height-1) * 2 * 3;

        // initialize vertex buffer

        std::vector<System::Vertex> vertex(vertex_count);

        int i = 0;

		for (int y=y1; y<=y2; y++)
		{
			for (int x=x1; x<=x2; x++)
			{
                // get real coords

		        float rx, ry;
		        heightfield.vectorAtIndex(x,y,rx,ry);

		        // get height at index (x,y)

		        const float height = heightfield.heightAtIndex(x,y);
        		
		        // get normal at (x,y)

		        Vector normal;
                heightfield.normalAtIndex(x,y,normal);
		        normal.normalize();

		        // fill vertex data

                vertex[i].position = Vector(rx, ry, height);
                vertex[i].normal = normal;
                vertex[i].u = rx * 0.1f;
                vertex[i].v = ry * 0.1f;

                i++;
	        }
		}

		// initialize index buffer

        std::vector<int> index(index_count);

		int k = 0;

		for (int y=0; y<height-1; y++)
		{
			int i = width * y;

			for (int x=0; x<width-1; x++)
			{
				index[k] = i;
				index[k+1] = i + 1 + width;
				index[k+2] = i + width;

				index[k+3] = i;
				index[k+4] = i + 1;
				index[k+5] = i + 1 + width;

				k += 6;

				i ++;
			}
		}

        // create the mesh object

        mesh = display->createMesh(vertex, index);
    }

    ~TerrainTile()
    {
        mesh->release();
    }

    void render(System::Display *display)
    {
        assert(mesh);

        display->selectMesh(mesh);
        display->renderMesh();
        display->selectMesh(0);
    }

    int getX() const
    {
        return x;
    }

    int getY() const
    {
        return y;
    }

private:

    int x;
    int y;
    System::Mesh *mesh;
};
