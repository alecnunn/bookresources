#include "Core_Global.h"
#include "Tile.h"
#include "Map.h"

cMap::cMap()
{
  m_Map   = NULL;
  m_Tiles = NULL;
  m_NumObjectsToDraw = 0;
  m_Width = m_Height = 0;
}

cMap::~cMap()
{
  Free();
}

BOOL cMap::Create(long NumLayers, long Width, long Height)
{
  // Free a prior map
  Free();

  // Save # layers, width, and height
  m_NumLayers = NumLayers;
  m_Width     = Width;
  m_Height    = Height;

  // Allocate map data memory
  if((m_Map = new char[m_NumLayers*m_Width*m_Height]) == NULL)
    return FALSE;

  // Clear it out
  ZeroMemory(m_Map, m_NumLayers*m_Width*m_Height);

  // Reset # objects to draw
  m_NumObjectsToDraw = 0;

  return TRUE;
}

BOOL cMap::Free()
{
  // Free map array
  delete [] m_Map;
  m_Map = NULL;
  m_Width = m_Height = 0;
  m_NumLayers = 0;

  return TRUE;
}

BOOL cMap::SetMapData(long Layer, char *Data)
{
  // Error checking
  if(Layer >= m_NumLayers)
    return FALSE;

  // Copy over data
  memcpy(&m_Map[Layer*m_Width*m_Height],Data,m_Width*m_Height);

  return TRUE;
}

void cMap::ClearObjectList()
{
  m_NumObjectsToDraw = 0;
}

BOOL cMap::AddObject(long XPos, long YPos, char Tile)
{
  if(m_NumObjectsToDraw < MAX_OBJECTS) {
    m_Objects[m_NumObjectsToDraw].XPos = XPos;
    m_Objects[m_NumObjectsToDraw].YPos = XPos;
    m_Objects[m_NumObjectsToDraw].Tile = Tile;
    m_NumObjectsToDraw++; 

    return TRUE;
  }

  return FALSE;
}

char *cMap::GetPtr(long Layer)
{
  if(Layer >= m_NumLayers)
    return NULL;

  return &m_Map[Layer*m_Width*m_Height];
}

long cMap::GetWidth()
{
  return m_Width;
}

long cMap::GetHeight()
{
  return m_Height;
}

BOOL cMap::UseTiles(cTiles *Tiles)
{
  if((m_Tiles = Tiles) == NULL)
    return FALSE;

  return TRUE;
}

BOOL cMap::Render(long XPos,    long YPos, 
                  long NumRows, long NumColumns,
                  long ObjectLayer)
{
  long MapX, MapY; 
  long XOff, YOff;
  long Layer, Row, Column, i;
  char TileNum;
  char *MapPtr;
  long TileX, TileY;

  // Error checking
  if(m_Map == NULL || m_Tiles == NULL)
    return FALSE;

  // Calculate smooth scrolling variables
  MapX = XPos / m_Tiles->GetWidth(0);
  MapY = YPos / m_Tiles->GetHeight(0);
  XOff = XPos % m_Tiles->GetWidth(0);
  YOff = YPos % m_Tiles->GetHeight(0);

  // Loop through each layer
  for(Layer=0;Layer<m_NumLayers;Layer++) {

    // Get a pointer to the map data
    MapPtr = &m_Map[Layer*m_Width*m_Height];

    // Loop for each row and column
    for(Row=0;Row<NumRows+1;Row++) {
      for(Column=0;Column<NumColumns+1;Column++) {

        // Get the tile number to draw (and draw it)
        TileNum = MapPtr[(Row + MapY)*m_Width+Column+MapX];
        TileX = Column * m_Tiles->GetWidth(0)  - XOff;
        TileY = Row    * m_Tiles->GetHeight(0) - YOff;

        m_Tiles->Draw(0, TileNum, (DWORD)(TileX*2.5f), (DWORD)(TileY*2.5f), 0xFFFFFFFF, 2.5f, 2.5f);
      }
    }

    // Draw objects if on object layer
    if(Layer == ObjectLayer) {
      for(i=0;i<m_NumObjectsToDraw;i++)
        m_Tiles->Draw(0, m_Objects[i].Tile,                   \
                         m_Objects[i].XPos - XOff,            \
                         m_Objects[i].YPos - YOff);
    }
  }

  return TRUE;
}
