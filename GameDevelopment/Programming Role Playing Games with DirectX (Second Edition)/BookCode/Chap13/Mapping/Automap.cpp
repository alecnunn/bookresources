#include "Core_Global.h"
#include "Automap.h"

cAutomap::cAutomap()
{
  m_Graphics = NULL;

  // Set section data and vertex buffer
  m_NumSections = 0;
  m_Visible = NULL;
  m_MapVB = NULL;

  // Point the camera down
  m_Camera.Rotate(1.57f, 0.0f, 0.0f);

  // Set a default window for displaying map
  SetWindow(0,0,100,100);

  m_Scale = 1.0f;  // Set a default scale
}

cAutomap::~cAutomap()
{
  Free();
}

BOOL cAutomap::Create(cGraphics *Graphics,                    \
                      char *Filename, long Color)
{
  cMesh           Mesh;
  sMesh          *MeshPtr;
  ID3DXMesh      *IMeshPtr;
  sGenericVertex *GenVert;
  sVertex         Vertex, *VBPtr;
  long            i, j, Num;
  long            VertexSize, NumFaces;
  unsigned short *IndexPtr;
  char           *VertexPtr;
  float           Radius;
  sVertex         PointerVerts[3] = { 
        {   0.0f, 0.0f,  10.0f, D3DCOLOR_RGBA(128,64,0,255) },
        {   5.0f, 0.0f, -10.0f, D3DCOLOR_RGBA(128,64,0,255) },
        {  -5.0f, 0.0f, -10.0f, D3DCOLOR_RGBA(128,64,0,255) }
      };

  // Free a prior auto map
  Free();

  // Error checking
  if((m_Graphics = Graphics) == NULL)
    return FALSE;

  // Attempt to load the mesh
  if(Mesh.Load(Graphics, Filename) == FALSE)
    return FALSE;

  // Get mesh pointer {
  if((MeshPtr = Mesh.GetParentMesh()) == NULL) {
    Mesh.Free();
    return FALSE;
  }

  // Get size of vertices from source mesh
  VertexSize = D3DXGetFVFVertexSize(MeshPtr->m_Mesh->GetFVF());

  // Get a bounding radius to scale by
  Mesh.GetBounds(NULL,NULL,NULL,NULL,NULL,NULL,&Radius);
  m_Scale = 128.0f / Radius;

  // Get # of sections in map mesh
  if(!(m_NumSections = Mesh.GetNumMeshes())) {
    Mesh.Free();
    return FALSE;
  }

  // Allocate the visibility buffer and clear it
  m_Visible = new char[m_NumSections];
  ZeroMemory(m_Visible, m_NumSections);

  // Allocate the vertex buffers
  m_MapVB = new cVertexBuffer[m_NumSections]();

  // Go through each mesh in the cMesh object and
  // construct a matching vertex buffer. Make sure
  // to start with last section in map to compensate
  // for linked list ordering of meshes in cMesh.
  Num = m_NumSections - 1;
  while(MeshPtr != NULL) {
    // Get a pointer to the mesh
    IMeshPtr = MeshPtr->m_Mesh;

    // Lock index and vertex buffers
    IMeshPtr->LockIndexBuffer(D3DLOCK_READONLY,               \
                            (void**)&IndexPtr);
    IMeshPtr->LockVertexBuffer(D3DLOCK_READONLY,              \
                              (void**)&VertexPtr);

    // Create the vertex buffer
    NumFaces = IMeshPtr->GetNumFaces();
    m_MapVB[Num].Create(Graphics, NumFaces*3,                 \
                         AUTOMAPFVF, sizeof(sVertex));

    // Lock the vertex buffer
    m_MapVB[Num].Lock(0,0);
    VBPtr = (sVertex*)m_MapVB[Num].GetPtr();

    // Pull out vertices and construct vertex list
    for(i=0;i<NumFaces;i++) {
      for(j=0;j<3;j++) {
        // Get pointer to vertex
        GenVert=(sGenericVertex*)                             \
                &VertexPtr[VertexSize * (*IndexPtr++)];

        // Create new vertices
        Vertex.x = GenVert->x * m_Scale;
        Vertex.y = 0.0f;
        Vertex.z = GenVert->z * m_Scale;
        Vertex.Diffuse = Color;

        memcpy(VBPtr++, &Vertex, sizeof(sVertex));
      }
    }

    // Unlock the vertex buffer
    m_MapVB[Num].Unlock();

    // Unlock buffers
    IMeshPtr->UnlockVertexBuffer();
    IMeshPtr->UnlockIndexBuffer();

    // Go to next mesh
    Num--;
    MeshPtr = MeshPtr->m_Next;
  }

  // Create a character pointer vertex buffer
  m_PointerVB.Create(Graphics,3,AUTOMAPFVF, sizeof(sVertex));
  m_PointerVB.Set(0,3,&PointerVerts);

  Mesh.Free();  // Free loaded mesh

  return TRUE;
}

BOOL cAutomap::Free()
{
  long i;
  
  // Free map vertex buffers
  if(m_MapVB != NULL) {
    for(i=0;i<m_NumSections;i++)
      m_MapVB[i].Free();
    delete [] m_MapVB;
    m_MapVB = NULL;
  }
  
  m_NumSections = 0;    // Reset # of sections

  delete [] m_Visible;  // Release visibility array
  m_Visible = NULL;

  m_PointerVB.Free();   // Free pointer vertex buffer

  return TRUE;
}

BOOL cAutomap::Load(char *Filename)
{
  FILE *fp;
  long Num;
  BOOL ReturnVal = FALSE;

  // Open the file
  if((fp=fopen(Filename, "rb"))==NULL)
    return FALSE;

  // Get the number of sections in file
  fread(&Num, 1, sizeof(long), fp);

  // Make sure they match and load in visible flags
  if(m_NumSections == Num && m_Visible != NULL) {
    fread(m_Visible, 1, Num, fp);
    ReturnVal = TRUE;
  }

  fclose(fp);

  return ReturnVal;
}

BOOL cAutomap::Save(char *Filename)
{
  FILE *fp;

  // Error checking
  if(m_NumSections && m_Visible == NULL)
    return FALSE;

  // Create the file
  if((fp=fopen(Filename, "wb"))==NULL)
    return FALSE;

  // Write out number of sections
  fwrite(&m_NumSections, 1, sizeof(long), fp);

  // Write out visible flags
  fwrite(m_Visible, 1, m_NumSections, fp);

  fclose(fp);  // Close file

  return TRUE;  // Return success
}

long cAutomap::GetNumSections()
{
  return m_NumSections;
}

BOOL cAutomap::EnableSection(long Section, BOOL Enable)
{
  if(Section >= m_NumSections || m_Visible == NULL)
    return FALSE;
  
  m_Visible[Section] = (Enable==TRUE) ? 1 : 0;

  return TRUE;
}

BOOL cAutomap::SetWindow(long XPos, long YPos,                \
                         long Width, long Height)
{
  m_Viewport.X = XPos;
  m_Viewport.Y = YPos;
  m_Viewport.Width  = Width;
  m_Viewport.Height = Height;
  m_Viewport.MinZ = 0.0f;
  m_Viewport.MaxZ = 1.0f;

  return TRUE;
}

BOOL cAutomap::Render(cCamera *OldCamera,                     \
                float MXPos, float MYPos, float MZPos,        \
                float NumPositions,                           \
                float *XPos, float *ZPos, float *Angle)
{
  cWorldPosition Pos;
  D3DVIEWPORT9 OldViewport;
  long i;

  // Error checking
  if(m_Graphics == NULL || !m_NumSections || 
     m_MapVB == NULL    || m_Visible == NULL)
    return FALSE;

  // Move camera
  m_Camera.Move(MXPos*m_Scale, MYPos, MZPos*m_Scale);
  m_Graphics->SetCamera(&m_Camera);

  // Get old viewport and set new viewport
  m_Graphics->GetDeviceCOM()->GetViewport(&OldViewport);
  m_Graphics->GetDeviceCOM()->SetViewport(&m_Viewport);

  // Set rendering states and texture
  m_Graphics->EnableZBuffer(FALSE);
  m_Graphics->SetTexture(0, NULL);

  // Render vertex buffers
  m_Graphics->SetWorldPosition(&Pos);
  for(i=0;i<m_NumSections;i++) {
    if(m_Visible[i])
      m_MapVB[i].Render(0, m_MapVB[i].GetNumVertices()/3,     \
                        D3DPT_TRIANGLELIST);
  }

  // Display alpha blending to render pointer
  m_Graphics->EnableAlphaBlending(FALSE);

  // Draw the character positions
  if(NumPositions) {
    for(i=0;i<NumPositions;i++) {
      Pos.Move(XPos[i] * m_Scale, 0.0f, ZPos[i] * m_Scale);
      Pos.Rotate(0.0f, Angle[i], 0.0f);
      m_Graphics->SetWorldPosition(&Pos);
      m_PointerVB.Render(0, 1, D3DPT_TRIANGLELIST);
    }
  }

  // Restore old camera if passed
  if(OldCamera != NULL)
    m_Graphics->SetCamera(OldCamera);

  // Restore old viewport
  m_Graphics->GetDeviceCOM()->SetViewport(&OldViewport);

  return TRUE;
}
