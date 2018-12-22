#ifndef _BARRIER_H_
#define _BARRIER_H_

#include "Frustum.h"

enum BarrierTypes {
  Barrier_Sphere = 0,
  Barrier_Box,
  Barrier_Cylinder,
  Barrier_Triangle
};

typedef struct sBarrier {
  long Type;         // Sphere, Box, etc

  long ID;           // Barrier ID

  BOOL Enabled;      // Flag if enabled

  float XPos, YPos, ZPos; // Coordinates
  float XRot, YRot, ZRot; // Rotation

  float x1, y1, z1;  // Coord 1
  float x2, y2, z2;  // Coord 2
  float x3, z3;      // Coord 3

  float Radius;      // Radius of bounds

  cObject Object;
  
  sBarrier *Prev, *Next;

  sBarrier()  { Prev = Next = NULL; }
  ~sBarrier() { delete Next; Next = NULL; Object.Free(); }
} sBarrier;

class cBarrier
{
  private:
    cGraphics *m_Graphics;
    long       m_NumBarriers;
    sBarrier  *m_BarrierParent;

    long      GetNextLong(FILE *fp);
    float     GetNextFloat(FILE *fp);
    sBarrier *AddBarrier(long Type, long ID, BOOL Enabled,    \
                         float XPos, float YPos, float ZPos,  \
                         float XRot, float YRot, float ZRot);

  public:
    cBarrier();
    ~cBarrier();

    // Functions to load and save a barrier list
    BOOL Load(char *Filename);
    BOOL Save(char *Filename);

    // Functions to set a mesh and animation for a barrier
    BOOL SetMesh(long ID, cGraphics *Graphics, cMesh *Mesh);
    BOOL SetAnim(long ID, cAnimation *Anim,                   \
                 char *Name, long Time);

    // Render barriers out using specified frustum
    BOOL Render(unsigned long Time, cFrustum *Frustum);

    BOOL AddSphere(long ID, BOOL Enabled,                     \
                   float XPos,  float YPos,  float ZPos,      \
                   float XRot,  float YRot,  float ZRot,      \
                   float CXPos, float CYPos, float CZPos,     \
                   float Radius);

    BOOL AddBox(long ID, BOOL Enabled,                        \
                float XPos,  float YPos,  float ZPos,         \
                float XRot,  float YRot,  float ZRot,         \
                float XMin, float YMin, float ZMin,           \
                float XMax, float YMax, float ZMax);

    BOOL AddCylinder(long ID, BOOL Enabled,                   \
                     float XPos,  float YPos,  float ZPos,    \
                     float XRot,  float YRot,  float ZRot,    \
                     float CXPos, float CYPos, float CZPos,   \
                     float Radius, float Height);

    BOOL AddTriangle(long ID, BOOL Enabled,                   \
                     float XPos,  float YPos,  float ZPos,    \
                     float XRot,  float YRot,  float ZRot,    \
                     float x1, float z1,                      \
                     float x2, float z2,                      \
                     float x3, float z3,                      \
                     float CYPos, float Height);

    BOOL Remove(long ID);
    BOOL Free();

    long GetBarrier(float XPos, float YPos, float ZPos);

    BOOL GetEnableState(long ID);
    BOOL Enable(long ID, BOOL Enable);

    long GetNumBarriers();
    sBarrier *GetParentBarrier();
};

#endif
