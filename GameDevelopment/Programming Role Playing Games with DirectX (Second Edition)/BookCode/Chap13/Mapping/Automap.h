#ifndef _AUTOMAP_H_
#define _AUTOMAP_H_

class cAutomap
{
  private:
    typedef struct {
      float x, y, z;  // 3D coordinates
    } sGenericVertex;

    typedef struct {
      float x, y, z;     // Coordinates
      D3DCOLOR Diffuse;  // Map color
    } sVertex;
    #define AUTOMAPFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

    cGraphics     *m_Graphics;     // Parent cGraphics object

    long           m_NumSections;  // # sections in map
    char          *m_Visible;      // Visibility of sections
    cVertexBuffer *m_MapVB;        // Map vertex buffer

    cVertexBuffer  m_PointerVB;    // Pointer vertex buffer

    D3DVIEWPORT9   m_Viewport;     // Area to draw map
    cCamera        m_Camera;       // Camera used to render map

    float          m_Scale;        // Scale used to draw map

  public:
    cAutomap();   // Constructor
    ~cAutomap();  // Destructor
 
    // Functions to create and free a map
    BOOL Create(cGraphics *Graphics, char *Filename,          \
                long Color = D3DCOLOR_RGBA(64,64,64,255));
    BOOL Free();

    // Functions to load/save enabled map sections
    BOOL Load(char *Filename);
    BOOL Save(char *Filename);

    // Return number of map sections
    long GetNumSections();

    // Enable/disable a map section's visibility flag
    BOOL EnableSection(long Section, BOOL Enable);

    // Define an area to draw the map
    BOOL SetWindow(long XPos, long YPos,                      \
                   long Width, long Height);

    // Render the map to display
    BOOL Render(cCamera *OldCamera,                           \
                float MXPos, float MYPos, float MZPos,        \
                float NumPositions,                           \
                float *XPos, float *ZPos, float *Angle);
};

#endif
