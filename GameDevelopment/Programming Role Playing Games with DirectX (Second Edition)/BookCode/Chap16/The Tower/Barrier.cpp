#include "Core_Global.h"
#include "Frustum.h"
#include "Barrier.h"

cBarrier::cBarrier()
{
  m_NumBarriers = 0;
  m_BarrierParent = NULL;
}

cBarrier::~cBarrier()
{
  Free();
}

long cBarrier::GetNextLong(FILE *fp)
{
  char Buf[1024];
  long Pos = 0;
  int c;

  // Read until EOF or EOL
  while(1) {
    if((c = fgetc(fp)) == EOF)
      break;
    if(c == 0x0a || (c == ' ' && Pos))
      break;
    if((c >= '0' && c <= '9') || c == '.' || c == '-')
      Buf[Pos++] = c;
  }
  if(!Pos)
    return -1;
  Buf[Pos] = 0;

  return atol(Buf);
}

float cBarrier::GetNextFloat(FILE *fp)
{
  char Buf[1024];
  long Pos = 0;
  int c;

  // Read until EOF or EOL
  while(1) {
    if((c = fgetc(fp)) == EOF)
      break;
    if(c == 0x0a || (c == ' ' && Pos))
      break;
    if((c >= '0' && c <= '9') || c == '.' || c == '-')
      Buf[Pos++] = c;
  }
  Buf[Pos] = 0;

  return (float)atof(Buf);
}

BOOL cBarrier::SetMesh(long ID,                               \
                       cGraphics *Graphics, cMesh *Mesh)
{
  sBarrier *BarrierPtr;
  long Count = 0;

  // Loop through all Barriers looking for ID
  if((BarrierPtr = m_BarrierParent) != NULL) {
    while(BarrierPtr != NULL) {

      // If matched ID then set mesh
      if(BarrierPtr->ID == ID) {
        BarrierPtr->Object.Create(Graphics, Mesh);
        Count++;
      }

      // Go to next flag
      BarrierPtr = BarrierPtr->Next;
    }
  }

  // Return TRUE if any meshes set
  return (Count) ? TRUE : FALSE;
}

BOOL cBarrier::SetAnim(long ID, cAnimation *Anim,             \
                       char *Name, long Time)
{
  sBarrier *BarrierPtr;
  long Count = 0;

  // Loop through all Barriers looking for ID
  if((BarrierPtr = m_BarrierParent) != NULL) {
    while(BarrierPtr != NULL) {

      // If matched ID then set animation
      if(BarrierPtr->ID == ID) {
        BarrierPtr->Object.SetAnimation(Anim, Name, Time);
        Count++;
      }

      // Go to next flag
      BarrierPtr = BarrierPtr->Next;
    }
  }

  // Return TRUE if any animations set
  return (Count) ? TRUE : FALSE;
}

BOOL cBarrier::Render(unsigned long Time, cFrustum *Frustum)
{
  sBarrier *BarrierPtr;
  float Radius;

  // Error checking
  if(Frustum == NULL)
    return FALSE;

  // Loop through all Barriers looking for ID
  if((BarrierPtr = m_BarrierParent) != NULL) {
    while(BarrierPtr != NULL) {

      // Get radius and perform frustum check
      BarrierPtr->Object.GetBounds(NULL,NULL,NULL,NULL,       \
                                   NULL,NULL,&Radius);
      if(Frustum->CheckSphere(BarrierPtr->XPos,               \
                              BarrierPtr->YPos,               \
                              BarrierPtr->ZPos, Radius)) {

        // Position object
        BarrierPtr->Object.Move(BarrierPtr->XPos,             \
                                BarrierPtr->YPos,             \
                                BarrierPtr->ZPos);
        BarrierPtr->Object.Rotate(BarrierPtr->XRot,           \
                                  BarrierPtr->YRot,           \
                                  BarrierPtr->ZRot);

        // Update animation
        BarrierPtr->Object.UpdateAnimation(Time, TRUE);

        // Render object
        BarrierPtr->Object.Render();
      }

      // Go to next flag
      BarrierPtr = BarrierPtr->Next;
    }
  }

  return TRUE;
}

BOOL cBarrier::Load(char *Filename)
{
  FILE *fp;
  long Type, ID;
  BOOL Enabled;
  float x1, y1, z1, x2, y2, z2, x3, z3, Radius;
  float XPos, YPos, ZPos, XRot, YRot, ZRot;
  
  // Remove all current Barriers
  Free();

  // Open file
  if((fp=fopen(Filename, "rb"))==NULL)
    return FALSE;

  // Start looping, reading in until EOF reached
  while(1) {
    // Get ID of Barrier
    if((ID = GetNextLong(fp)) == -1)
      break;

    // Get type
    Type = GetNextLong(fp);

    // Get enabled status
    Enabled = (GetNextLong(fp)) ? TRUE : FALSE;

    // Get coordinates and rotation
    XPos = GetNextFloat(fp);
    YPos = GetNextFloat(fp);
    ZPos = GetNextFloat(fp);
    XRot = GetNextFloat(fp);
    YRot = GetNextFloat(fp);
    ZRot = GetNextFloat(fp);

    // Read in rest depending on type
    switch(Type) {
      case Barrier_Sphere:
        x1 = GetNextFloat(fp);
        y1 = GetNextFloat(fp);
        z1 = GetNextFloat(fp);
        Radius = GetNextFloat(fp);
        AddSphere(ID, Enabled, XPos, YPos, ZPos,              \
                  XRot, YRot, ZRot, x1, y1, z1, Radius);
        break;
      
      case Barrier_Box:
        x1 = GetNextFloat(fp);
        y1 = GetNextFloat(fp);
        z1 = GetNextFloat(fp);
        x2 = GetNextFloat(fp);
        y2 = GetNextFloat(fp);
        z2 = GetNextFloat(fp);
        AddBox(ID, Enabled, XPos, YPos, ZPos,                 \
               XRot, YRot, ZRot, x1, y1, z1, x2, y2, z2);
        break;
      
      case Barrier_Cylinder:
        x1 = GetNextFloat(fp);
        y1 = GetNextFloat(fp);
        z1 = GetNextFloat(fp);
        Radius = GetNextFloat(fp);
        y2 = GetNextFloat(fp);
        AddCylinder(ID, Enabled, XPos, YPos, ZPos,            \
                    XRot, YRot, ZRot, x1, y1, z1, Radius, y2);
        break;
      
      case Barrier_Triangle:
        x1 = GetNextFloat(fp);
        z1 = GetNextFloat(fp);
        x2 = GetNextFloat(fp);
        z2 = GetNextFloat(fp);
        x3 = GetNextFloat(fp);
        z3 = GetNextFloat(fp);
        y1 = GetNextFloat(fp);
        y2 = GetNextFloat(fp);
        AddTriangle(ID, Enabled, XPos, YPos, ZPos,            \
                    XRot, YRot, ZRot,                         \
                    x1, z1, x2, z2, x3, z3, y1, y2);
        break;

      default: fclose(fp);    // Some error occurred
               Free();
               return FALSE;
    }
  }

  fclose(fp);

  return (m_NumBarriers) ? TRUE : FALSE;
}

BOOL cBarrier::Save(char *Filename)
{
  FILE *fp;
  sBarrier *BarrierPtr;

  if(!m_NumBarriers)
    return FALSE;
  if((BarrierPtr = m_BarrierParent) == NULL)
    return FALSE;
  
  // Open file
  if((fp=fopen(Filename, "wb"))==NULL)
    return FALSE;

  // Write out all Barriers
  while(BarrierPtr != NULL) {
    // Write out ID and Type of Barrier
    fprintf(fp, "%lu ", BarrierPtr->ID);
    fprintf(fp, "%lu ", BarrierPtr->Type);

    // Write out Enabled flag
    fprintf(fp, "%lu ", (BarrierPtr->Enabled) ? 1 : 0);

    // Write out coordinates and rotation
    fprintf(fp, "%lf %lf %lf %lf %lf %lf ",                   \
      BarrierPtr->XPos, BarrierPtr->YPos, BarrierPtr->ZPos,   \
      BarrierPtr->XRot, BarrierPtr->YRot, BarrierPtr->ZRot);

    // Write out remaining data depending on type
    switch(BarrierPtr->Type) {
      case Barrier_Sphere:
        fprintf(fp, "%lf %lf %lf %lf\r\n",                    \
                BarrierPtr->x1, BarrierPtr->y1,               \
                BarrierPtr->z1, BarrierPtr->Radius);
        break;
      
      case Barrier_Box:
        fprintf(fp, "%lf %lf %lf %lf %lf %lf\r\n",            \
           BarrierPtr->x1, BarrierPtr->y1, BarrierPtr->z1,    \
           BarrierPtr->x2, BarrierPtr->y2, BarrierPtr->z2);
        break;
      
      case Barrier_Cylinder:
        fprintf(fp, "%lf %lf %lf %lf %lf\r\n",                \
            BarrierPtr->x1, BarrierPtr->y1, BarrierPtr->z1,   \
            BarrierPtr->Radius, BarrierPtr->y2);
        break;
      
      case Barrier_Triangle:
        fprintf(fp, "%lf %lf %lf %lf %lf %lf %lf %lf\r\n",    \
                BarrierPtr->x1, BarrierPtr->z1,               \
                BarrierPtr->x2, BarrierPtr->z2,               \
                BarrierPtr->x3, BarrierPtr->z3,               \
                BarrierPtr->y1, BarrierPtr->y2);
        break;
    }
  }

  fclose(fp);

  return TRUE;
}

sBarrier *cBarrier::AddBarrier(long Type, long ID,            \
                        BOOL Enabled,                         \
                        float XPos, float YPos, float ZPos,   \
                        float XRot, float YRot, float ZRot)
{
  // Allocate a new Barrier structure and link in
  sBarrier *Barrier = new sBarrier();
  Barrier->Prev = NULL;
  if((Barrier->Next = m_BarrierParent) != NULL)
    m_BarrierParent->Prev = Barrier;
  m_BarrierParent = Barrier;

  // Set Barrier Type, ID, and Enabled flag
  Barrier->Type = Type;
  Barrier->ID = ID;
  Barrier->Enabled = Enabled;
  Barrier->XPos = XPos;
  Barrier->YPos = YPos;
  Barrier->ZPos = ZPos;
  Barrier->XRot = XRot;
  Barrier->YRot = YRot;
  Barrier->ZRot = ZRot;

  // Increase Barrier count
  m_NumBarriers++;

  return Barrier;
}

BOOL cBarrier::AddSphere(long ID, BOOL Enabled,               \
               float XPos,  float YPos,  float ZPos,          \
               float XRot,  float YRot,  float ZRot,          \
               float CXPos, float CYPos, float CZPos,         \
               float Radius)
{
  // Create a new Barrier structure and link in 
  sBarrier *Barrier = AddBarrier(Barrier_Sphere, ID, Enabled, \
                                 XPos, YPos, ZPos,            \
                                 XRot, YRot, ZRot);

  // Setup Barrier data
  Barrier->x1 = CXPos;
  Barrier->y1 = CYPos;
  Barrier->z1 = CZPos;
  Barrier->Radius = Radius * Radius;

  return TRUE;
}

BOOL cBarrier::AddBox(long ID, BOOL Enabled,                  \
                      float XPos,  float YPos,  float ZPos,   \
                      float XRot,  float YRot,  float ZRot,   \
                      float XMin, float YMin, float ZMin,     \
                      float XMax, float YMax, float ZMax)
{
  // Create a new Barrier structure and link in 
  sBarrier *Barrier = AddBarrier(Barrier_Box, ID, Enabled,    \
                                 XPos, YPos, ZPos,            \
                                 XRot, YRot, ZRot);

  // Setup Barrier data (fix for min/max values)
  Barrier->x1 = min(XMin, XMax);
  Barrier->y1 = min(YMin, YMax);
  Barrier->z1 = min(ZMin, ZMax);
  Barrier->x2 = max(XMin, XMax);
  Barrier->y2 = max(YMin, YMax);
  Barrier->z2 = max(ZMin, ZMax);

  return TRUE;
}

BOOL cBarrier::AddCylinder(long ID, BOOL Enabled,             \
                       float XPos,  float YPos,  float ZPos,  \
                       float XRot,  float YRot,  float ZRot,  \
                       float CXPos, float CYPos, float CZPos, \
                       float Radius, float Height)
{
  // Create a new Barrier structure and link in 
  sBarrier *Barrier = AddBarrier(Barrier_Cylinder,ID,Enabled, \
                                 XPos, YPos, ZPos,            \
                                 XRot, YRot, ZRot);
            
  // Setup Barrier data
  Barrier->x1     = XPos;
  Barrier->y1     = YPos;
  Barrier->z1     = ZPos;
  Barrier->Radius = Radius * Radius;
  Barrier->y2     = Height;

  return TRUE;
}

BOOL cBarrier::AddTriangle(long ID, BOOL Enabled,             \
                     float XPos,  float YPos,  float ZPos,    \
                     float XRot,  float YRot,  float ZRot,    \
                     float x1, float z1,                      \
                     float x2, float z2,                      \
                     float x3, float z3,                      \
                     float CYPos, float Height)
{
  // Create a new Barrier structure and link in 
  sBarrier *Barrier = AddBarrier(Barrier_Triangle,ID,Enabled, \
                                 XPos, YPos, ZPos,            \
                                 XRot, YRot, ZRot);

  // Setup Barrier data
  Barrier->x1 = x1;
  Barrier->z1 = z1;
  Barrier->x2 = x2;
  Barrier->z2 = z2;
  Barrier->x3 = x3;
  Barrier->z3 = z3;
  Barrier->y1 = YPos;
  Barrier->y2 = Height;

  return TRUE;
}

BOOL cBarrier::Remove(long ID)
{
  sBarrier *BarrierPtr, *NextBarrier;
  long Count = 0;

  // Scan through list of Barriers
  if((BarrierPtr = m_BarrierParent) != NULL) {
    while(BarrierPtr != NULL) {

      // Remember which is next
      NextBarrier = BarrierPtr->Next;

      // Matched?
      if(BarrierPtr->ID == ID) {

        // Remove from list 
        if(BarrierPtr->Prev != NULL)
          BarrierPtr->Prev->Next = BarrierPtr->Next;
        else
          m_BarrierParent = BarrierPtr->Next;
        if(BarrierPtr->Next != NULL)
          BarrierPtr->Next->Prev = BarrierPtr->Prev;

        // Release memory
        BarrierPtr->Next = NULL;
        delete BarrierPtr;

        // Decrease number of Barriers and increase # removed
        m_NumBarriers--;
        Count++;
      }

      // Go to next Barrier
      BarrierPtr = NextBarrier;
    }
  }

  // Return TRUE if any found and removed
  return (Count) ? TRUE : FALSE;
}

BOOL cBarrier::Free()
{
  delete m_BarrierParent;
  m_BarrierParent = NULL;
  m_NumBarriers = 0;

  return TRUE;
}

long cBarrier::GetBarrier(float XPos, float YPos, float ZPos)
{
  float XDiff, YDiff, ZDiff, Dist;
  D3DXVECTOR2 vecNorm;
  sBarrier *Barrier;

  if((Barrier = m_BarrierParent) != NULL) {
    while(Barrier != NULL) {

      // Only bother if enabled
      if(Barrier->Enabled == TRUE) {

        // Check based on type
        switch(Barrier->Type) {
          case Barrier_Sphere:
            // Check distance from sphere (using radius)
            XDiff = (float)fabs(Barrier->x1 - XPos);
            YDiff = (float)fabs(Barrier->y1 - YPos);
            ZDiff = (float)fabs(Barrier->z1 - ZPos);
            Dist = XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff;
            if(Dist <= Barrier->Radius)
              return Barrier->ID;
            break;
    
          case Barrier_Box:
            // Check if inside box
            if(XPos >= Barrier->x1 && XPos <= Barrier->x2) {
              if(YPos >= Barrier->y1 && YPos <= Barrier->y2) {
                if(ZPos >= Barrier->z1 && ZPos <= Barrier->z2)
                  return Barrier->ID;
              }
            }
            break;

          case Barrier_Cylinder:
            // First make sure within height bounds
            if(YPos >= Barrier->y1 &&                         \
               YPos <= Barrier->y1 + Barrier->y2) {
              // Check distance from cylinder
              XDiff = (float)fabs(Barrier->x1 - XPos);
              YDiff = (float)fabs(Barrier->y1 - YPos);
              ZDiff = (float)fabs(Barrier->z1 - ZPos);
              Dist = XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff;
              if(Dist <= Barrier->Radius)
                return Barrier->ID;
            }
            break;

          case Barrier_Triangle:
            // First make sure within height bounds
            if(YPos >= Barrier->y1 &&                         \
               YPos <= Barrier->y1 + Barrier->y2) {
            
              // Check if point in front of all lines

              // x1,z1 to x2,z2
              D3DXVec2Normalize(&vecNorm,                     \
                     &D3DXVECTOR2(Barrier->z2 - Barrier->z1,  \
                                  Barrier->x1 - Barrier->x2));
              if(D3DXVec2Dot(&D3DXVECTOR2(XPos-Barrier->x1,   \
                                          ZPos-Barrier->z1),  \
                                          &vecNorm) < 0)
                break;

              // x2,z2 to x3,z3
              D3DXVec2Normalize(&vecNorm,                       \
                     &D3DXVECTOR2(Barrier->z3 - Barrier->z2,    \
                                  Barrier->x2 - Barrier->x3));
              if(D3DXVec2Dot(&D3DXVECTOR2(XPos-Barrier->x2,     \
                                          ZPos-Barrier->z2),    \
                                          &vecNorm) < 0)
                break;

              // x3,z3 to x1,z1
              D3DXVec2Normalize(&vecNorm,                       \
                     &D3DXVECTOR2(Barrier->z1 - Barrier->z3,    \
                                  Barrier->x3 - Barrier->x1));
              if(D3DXVec2Dot(&D3DXVECTOR2(XPos-Barrier->x3,     \
                                          ZPos-Barrier->z3),    \
                                          &vecNorm) < 0)
                break;

              return Barrier->ID;
            }
            break;
        }
      }

      // Go to next Barrier
      Barrier = Barrier->Next;
    }
  }

  return 0; // 0 means no Barrier found
}

BOOL cBarrier::GetEnableState(long ID)
{
  sBarrier *BarrierPtr;

  // Loop through all Barriers looking for ID
  if((BarrierPtr = m_BarrierParent) != NULL) {
    while(BarrierPtr != NULL) {

      // If matched ID then set return state
      if(BarrierPtr->ID == ID)
        return BarrierPtr->Enabled;

      // Go to next flag
      BarrierPtr = BarrierPtr->Next;
    }
  }

  // Return FALSE for none found
  return FALSE;
}

BOOL cBarrier::Enable(long ID, BOOL Enable)
{
  sBarrier *BarrierPtr;
  long Count = 0;

  // Loop through all Barriers looking for ID
  if((BarrierPtr = m_BarrierParent) != NULL) {
    while(BarrierPtr != NULL) {

      // If matched ID then set flag and increase count
      if(BarrierPtr->ID == ID) {
        BarrierPtr->Enabled = Enable;
        Count++;
      }

      // Go to next flag
      BarrierPtr = BarrierPtr->Next;
    }
  }

  // Return TRUE if any Barriers changed
  return (Count) ? TRUE : FALSE;
}

long cBarrier::GetNumBarriers()
{
  return m_NumBarriers;
}

sBarrier *cBarrier::GetParentBarrier()
{
  return m_BarrierParent;
}
