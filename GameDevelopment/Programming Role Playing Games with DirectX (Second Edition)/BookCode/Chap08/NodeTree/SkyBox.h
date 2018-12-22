#ifndef _SKYBOX_H_
#define _SKYBOX_H_

enum SkyBoxSides {
  TOP = 0,
  BOTTOM,
  LEFT,
  RIGHT,
  FRONT,
  BACK
};

class cSkyBox
{
  private:
    typedef struct sSkyBoxVertex {
      float x, y, z;
      float u, v;
    } sSkyBoxVertex;
    #define SkyBoxFVF (D3DFVF_XYZ | D3DFVF_TEX1)

    cGraphics     *m_Graphics;     // Parent cGraphics object
    cTexture       m_Textures[6];  // Face textures (0-5)
    cVertexBuffer  m_VB;           // Mesh vertex buffer
    cWorldPosition m_Pos;          // Sky box orientation

  public:
    cSkyBox();   // Constructor
    ~cSkyBox();  // Destructor

    // Create and free a sky box class object
    BOOL Create(cGraphics *Graphics);
    BOOL Free();

    // Set a specific side's texture map. Allow for 
    // transparent and storage format changes.
    BOOL LoadTexture(short Side, char *Filename, 
                     D3DCOLOR Transparent = 0, 
                     D3DFORMAT = D3DFMT_UNKNOWN);

    // Rotate box to an absolute or relative rotation.
    BOOL Rotate(float XRot, float YRot, float ZRot);
    BOOL RotateRel(float XRot, float YRot, float ZRot);

    // Render the sky box (using optional alpha-blending)
    // and using current view transformation from Camera.
    BOOL Render(cCamera *Camera, BOOL Alpha = FALSE);
};

#endif
