#include "Core_Global.h"
#include "SkyBox.h"

cSkyBox::cSkyBox()
{
  m_Graphics = NULL;
}

cSkyBox::~cSkyBox()
{
  Free();
}

BOOL cSkyBox::Create(cGraphics *Graphics)
{
  sSkyBoxVertex Verts[24] = {
    { -10.0f,  10.0f, -10.0f, 0.0f, 0.0f },  // Top
    {  10.0f,  10.0f, -10.0f, 1.0f, 0.0f },
    { -10.0f,  10.0f,  10.0f, 0.0f, 1.0f },
    {  10.0f,  10.0f,  10.0f, 1.0f, 1.0f },

    { -10.0f, -10.0f,  10.0f, 0.0f, 0.0f },  // Bottom
    {  10.0f, -10.0f,  10.0f, 1.0f, 0.0f },
    { -10.0f, -10.0f, -10.0f, 0.0f, 1.0f },
    {  10.0f, -10.0f, -10.0f, 1.0f, 1.0f },

    { -10.0f,  10.0f, -10.0f, 0.0f, 0.0f },  // Left
    { -10.0f,  10.0f,  10.0f, 1.0f, 0.0f },
    { -10.0f, -10.0f, -10.0f, 0.0f, 1.0f },
    { -10.0f, -10.0f,  10.0f, 1.0f, 1.0f },

    {  10.0f,  10.0f,  10.0f, 0.0f, 0.0f },  // Right
    {  10.0f,  10.0f, -10.0f, 1.0f, 0.0f },
    {  10.0f, -10.0f,  10.0f, 0.0f, 1.0f },
    {  10.0f, -10.0f, -10.0f, 1.0f, 1.0f },

    { -10.0f,  10.0f,  10.0f, 0.0f, 0.0f },  // Front
    {  10.0f,  10.0f,  10.0f, 1.0f, 0.0f },
    { -10.0f, -10.0f,  10.0f, 0.0f, 1.0f },
    {  10.0f, -10.0f,  10.0f, 1.0f, 1.0f },
    
    {  10.0f,  10.0f, -10.0f, 0.0f, 0.0f },  // Back
    { -10.0f,  10.0f, -10.0f, 1.0f, 0.0f },
    {  10.0f, -10.0f, -10.0f, 0.0f, 1.0f },
    { -10.0f, -10.0f, -10.0f, 1.0f, 1.0f },

  };

  Free(); // Free a prior sky box
  
  // Error checking
  if((m_Graphics = Graphics) == NULL)
    return FALSE;

  // Create the vertex buffer (and copy over sky box vertices)
  if(m_VB.Create(m_Graphics, 24, SkyBoxFVF, sizeof(sSkyBoxVertex)) == TRUE)
    m_VB.Set(0,24,(void*)&Verts);

  // Rotate the sky box into default orientation
  Rotate(0.0f, 0.0f, 0.0f);

  return TRUE;  // Return success!
}

BOOL cSkyBox::Free()
{
  m_Graphics = NULL;      // Clear parent cGraphics object
  for(short i=0;i<6;i++)  // Release textures
    m_Textures[i].Free();
  m_VB.Free();            // Release vertex buffer

  return TRUE;  // Return success!
}

BOOL cSkyBox::LoadTexture(short Side, char *Filename, D3DCOLOR Transparent, D3DFORMAT Format)
{
  // Error checking
  if(m_Graphics == NULL || Side < 0 || Side > 5)
    return FALSE;

  m_Textures[Side].Free(); // Free prior texture

  return m_Textures[Side].Load(m_Graphics, Filename, Transparent, Format);
}

BOOL cSkyBox::Rotate(float XRot, float YRot, float ZRot)
{
  return m_Pos.Rotate(XRot, YRot, ZRot);
}

BOOL cSkyBox::RotateRel(float XRot, float YRot, float ZRot)
{
  return m_Pos.RotateRel(XRot, YRot, ZRot);
}

BOOL cSkyBox::Render(cCamera *Camera, BOOL Alpha)
{
  D3DXMATRIX matWorld;
  short i;

  // Error checking
  if(m_Graphics == NULL || Camera == NULL)
    return FALSE;

  // Position sky box around viewer
  m_Pos.Move(Camera->GetXPos(), Camera->GetYPos(), Camera->GetZPos());
  m_Graphics->SetWorldPosition(&m_Pos);

  // Enable alpha testing and alpha blending
  m_Graphics->EnableAlphaTesting(TRUE);
  if(Alpha == TRUE)
    m_Graphics->EnableAlphaBlending(TRUE, D3DBLEND_SRCCOLOR, D3DBLEND_DESTCOLOR);

  // Draw each layer
  for(i=0;i<6;i++) {
    if(m_Textures[i].IsLoaded() == TRUE) {
      m_Graphics->SetTexture(0, &m_Textures[i]);
      m_VB.Render(i*4,2,D3DPT_TRIANGLESTRIP);
    }
  }

  // Disable alpha testing and alpha blending
  m_Graphics->EnableAlphaTesting(FALSE);
  if(Alpha == TRUE)
    m_Graphics->EnableAlphaBlending(FALSE);

  return TRUE;
}
