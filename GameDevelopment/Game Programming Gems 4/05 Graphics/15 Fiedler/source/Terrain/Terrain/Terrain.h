// Terrain class

#pragma once

#include "Heightfield.h"
#include "System/Display.h"
#include "System/Texture.h"
#include "System/Material.h"

#include "TerrainTile.h"


/// Terrain class.
/// Builds terrain tiles from heightfield.

class Terrain
{
public:

    /// initialize terrain

    void initialize(System::Display *display, Heightfield &heightfield, int size)
    {
        this->display = display;
        this->heightfield = &heightfield;
        this->size = size;

        this->resolution = heightfield.width() / size;

        // create tiles

        tiles.resize(resolution*resolution);

        for (unsigned int y=0; y<resolution; y++)
        {
            for (unsigned int x=0; x<resolution; x++)
            {
                TerrainTile *tile = new TerrainTile(display, heightfield, x, y, x*size, y*size, (x+1)*size, (y+1)*size);
                setTile(x,y, tile);
            }
        }

        // create material

        material = display->createMaterial();
        material->ambient(Color(0.25f,0.25f,0.25f));
        material->diffuse(Color(1,1,1));
        material->specular(Color(0,0,0));
        material->power(0);

        // create texture

        texture = display->createTexture("texture.png");
    }

    ~Terrain()
    {
        free();
    }

    void free()
    {
        for (unsigned int i=0; i<tiles.size(); i++)
            delete tiles[i];

        tiles.clear();

        if (material)
        {
            material->release();
            material = 0;
        }

        if (texture)
        {
            texture->release();
            texture = 0;
        }
    }

    /// render tiles in specified order.

    void renderTiles(const std::vector<int> &tiles, unsigned int tileHideCount)
    {
        display->selectTexture(texture);
        display->selectMaterial(material);

        display->cullMode(System::Display::Cull::NONE);

        const int maximum = tiles.size() - tileHideCount;

        for (int i=0; i<maximum; i++)
        {
            unsigned int index = tiles[i];

            assert(index>=0);
            assert(index<this->tiles.size());

            this->tiles[index]->render(display);
        }

        display->cullMode(System::Display::Cull::COUNTERCLOCKWISE);

        display->selectMaterial(0);
        display->selectTexture(0);
    }

    /// visualize tiles in array.
    /// shows a top down visualization grid of the tiles that are in the array.

    void visualizeTiles(const std::vector<int> &tiles, int tileHideCount, float x1, float y1, float x2, float y2)
    {
        // save display matrices

        Matrix view = display->view();
        Matrix world = display->world();
        Matrix projection = display->projection();

        // setup orthographic projection

        Matrix matrix;
        matrix.orthographic((float) display->width(), (float) display->height(), 0, 1);

        Matrix identity;
        identity.identity();

        display->view(identity);
        display->world(identity);
        display->projection(matrix);

        // visualize rendered tiles

        System::Material *material = display->createMaterial();
        material->ambient(Color(1,1,1));
        material->diffuse(Color(1,1,1));
        material->specular(Color(0,0,0));
        material->power(0);

        display->selectMaterial(material);

        display->beginQuadStream();

        const float s = (x2 - x1) / resolution;

        const int maximum = tiles.size() - tileHideCount;

        for (int i=0; i<maximum; i++)
        {
            const float x = resolution - (float) this->tiles[tiles[i]]->getX();
            const float y = (float) this->tiles[tiles[i]]->getY();

            Vector vertex[4];

            vertex[0] = Vector(x,y,0) * s;
            vertex[1] = Vector(x+0.8f,y,0) * s;
            vertex[2] = Vector(x+0.8f,y+0.8f,0) * s;
            vertex[3] = Vector(x,y+0.8f,0) * s;

            vertex[0] += Vector(x1,y1,0);
            vertex[1] += Vector(x1,y1,0);
            vertex[2] += Vector(x1,y1,0);
            vertex[3] += Vector(x1,y1,0);

            display->renderQuad(vertex);
        }

        display->endQuadStream();

        display->selectMaterial(0);

        material->release();

        // restore display matrices

        display->view(view);
        display->world(world);
        display->projection(projection);
    }

    /// set tile at (x,y) in tile coordinates

    void setTile(int x, int y, TerrainTile *tile)
    {
        tiles[x+resolution*y] = tile;
    }

    /// get tile at (x,y) in tile coordinates

    TerrainTile* getTile(int x, int y)
    {
        return tiles[x+resolution*y];
    }

	/// get resolution of terrain in tiles.
	/// the terrain is currently always square, for example a 512x512
	/// terrain would have a resolution of 512/16=32 if its tile size is 16.

	unsigned int getResolution() const
    {
        return resolution;
    }

	/// get the size of the terrain tiles.
	/// tiles are always square, eg: 16x16.

	unsigned int getTileSize() const
    {
        return size;
    }

private:

    std::vector<TerrainTile*> tiles;    ///< 2d array of terrain tiles. see getTile method.

    System::Display *display;           ///< display object
    System::Material *material;         ///< material for rendering terrain
    System::Texture *texture;           ///< texture for rendering terrain

    Heightfield *heightfield;           ///< heightfield object the terrain tiles were built from.

    unsigned int resolution;            ///< terrain resolution in tiles (note: terrain is always square)
    unsigned int size;                  ///< tile size (again, tiles are themselves always square also)
};
