#ifndef _MAP_H_
#define _MAP_H_

#define MAX_OBJECTS  1024
typedef struct {
  long XPos, YPos;
  char Tile;
} sObject;

class cMap
{
  private:
    long    m_Width, m_Height;  // Width and height of map
    long    m_NumLayers;        // # of layers
    char   *m_Map;              // array for tile information
    cTiles *m_Tiles;            // cTile class to use for tiles

    long    m_NumObjectsToDraw;
    sObject m_Objects[MAX_OBJECTS];

  public:
    cMap();   // Constructor
    ~cMap();  // Destructor

    // Function to create and free a map class
    BOOL Create(long NumLayers, long Width, long Height);
    BOOL Free();

    // Function to set a map's layer data 
    BOOL SetMapData(long Layer, char *Data);

    // Function to clear and add an object to list
    void ClearObjectList();
    BOOL AddObject(long XPos, long YPos, char Tile);

    char *GetPtr(long Layer); // Get pointer to map array
    long GetWidth();   // Get width of map
    long GetHeight();  // Get height of map

    // Assign cTile class object to use for drawing map tiles
    BOOL UseTiles(cTiles *Tiles);

    // Render map using specified top-left map coordinates, as
    // well as # of columns and rows to draw, plus layer used
    // to draw objects.
    BOOL Render(long XPos, long YPos,                         \
                long NumRows, long NumColumns,                \
                long ObjectLayer);
};

#endif
