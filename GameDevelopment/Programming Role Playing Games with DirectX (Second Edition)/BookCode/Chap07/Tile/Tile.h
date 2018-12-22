#ifndef _TILE_H_
#define _TILE_H_

class cTiles
{
  private:
    cGraphics *m_Graphics;     // Parent cGraphics

    long       m_NumTextures;  // # of textures
    cTexture  *m_Textures;     // cTexture array
    long      *m_Widths;       // Tile widths array
    long      *m_Heights;      // Tile heights array
    long      *m_Columns;      // # columns in texture

  public:
    cTiles();
    ~cTiles();

    // Functions to create and free the tile interface
    BOOL Create(cGraphics *Graphics, long NumTextures);
    BOOL Free();

    // Functions to load and free a single texture
    BOOL Load(long TextureNum, char *Filename,                \
              short TileWidth = 0, short TileHeight = 0,      \
              D3DCOLOR Transparent = 0,                       \
              D3DFORMAT Format = D3DFMT_A1R5G5B5);
    BOOL Free(long TextureNum);

    // Functions to retrieve tile dimensions and 
    // # of tiles in a texture.
    long GetWidth(long TextureNum);
    long GetHeight(long TextureNum);
    long GetNum(long TextureNum);

    // Enable or disable transparent blitting
    BOOL SetTransparent(BOOL Enabled = TRUE);

    // Draw a single tile to location
    BOOL Draw(long TextureNum, long TileNum,                  \
              long ScreenX, long ScreenY,                     \
              D3DCOLOR Color = 0xFFFFFFFF,                    \
              float XScale = 1.0f, float YScale = 1.0f);
};

#endif
