// File: CGeometry.cpp:


#include "include/CGeometry.h"


// snap vertex coordinates to grid
void CGeometry::Snap(ZFXVector *pVc, float fScale) {
   float fFractal=0.0f, fInt=0.0f;

   if (fScale > 2.0f) 
      *pVc *= 10.0f;
   else 
      *pVc *= 100.0f;

   // x coordinate
   fFractal = modff(pVc->x, &fInt);
   if ( pVc->x < 0.0f) {
      if (fFractal < -0.5f) pVc->x = floorf(pVc->x);
      else pVc->x = ceilf(pVc->x);
      }
   else { 
      if (fFractal > 0.5f) pVc->x = ceilf(pVc->x);
      else pVc->x = floorf(pVc->x);
      }

   // y coordinate
   fFractal = modff(pVc->y, &fInt);
   if ( pVc->y < 0.0f) {
      if (fFractal < -0.5f) pVc->y = floorf(pVc->y);
      else pVc->y = ceilf(pVc->y);
      }
   else { 
      if (fFractal > 0.5f) pVc->y = ceilf(pVc->y);
      else pVc->y = floorf(pVc->y);
      }

   // z coordinate
   fFractal = modff(pVc->z, &fInt);
   if ( pVc->z < 0.0f) {
      if (fFractal < -0.5f) pVc->z = floorf(pVc->z);
      else pVc->z = ceilf(pVc->z);
      }
   else { 
      if (fFractal > 0.5f) pVc->z = ceilf(pVc->z);
      else pVc->z = floorf(pVc->z);
      }

   if (fScale > 2.0f) 
      *pVc /= 10.0f;
   else 
      *pVc /= 100.0f;
   } // Snap
/*----------------------------------------------------------------*/


/**
 * 
 */
void CGeometry::CreateLightInv(CPolymesh *pMesh, CPolygon *pPoly, ZFXVector vcP,
                            float fW, float fH, float fD) {
   CPolygon  Poly[8];
   WORD   i[3] = {  2,  1,  0 };


   CreateLight(NULL, Poly, vcP, fW, fH, fD);

   // set new indices in changed clock order
   for (int n=0; n<8; n++) {
      // change indices
      Poly[n].SetIndices(i, 3);

      if (pPoly) pPoly[n].CopyOf(&Poly[n]);

      // add this side to the mesh
      if (pMesh) pMesh->AddPolygon(Poly[n]);
      }
   } // CreateLightInv
/*----------------------------------------------------------------*/


/**
 * 
 */
void CGeometry::CreateLight(CPolymesh *pMesh, CPolygon *pPoly, ZFXVector vcP,
                            float fW, float fH, float fD) {
   VERTEX vB[6]; // base vertices
   VERTEX v[24]; // to texture we need all sides as real tris

   CPolygon  Poly[8];
   WORD   i[3] = {  0,  1,  2 };
   
   // build base vertices
   vB[0].x = vcP.x - (fW / 2.0f);
   vB[0].y = vcP.y;
   vB[0].z = vcP.z - (fD / 2.0f);
   vB[1].x = vcP.x + (fW / 2.0f);
   vB[1].y = vcP.y;
   vB[1].z = vcP.z - (fD / 2.0f);
   vB[2].x = vcP.x + (fW / 2.0f);
   vB[2].y = vcP.y;
   vB[2].z = vcP.z + (fD / 2.0f);
   vB[3].x = vcP.x - (fW / 2.0f);
   vB[3].y = vcP.y;
   vB[3].z = vcP.z + (fD / 2.0f);
   vB[4].x = vcP.x;
   vB[4].y = vcP.y + (fH / 2.0f);
   vB[4].z = vcP.z;
   vB[4].tu = 0.5f; vB[4].tv = 0.0f;
   vB[5].x = vcP.x;
   vB[5].y = vcP.y - (fH / 2.0f);
   vB[5].z = vcP.z;
   vB[5].tu = 0.5f; vB[5].tv = 1.0f;

   // top pyramid
   v[0] = vB[1];
   v[1] = vB[0];
   v[2] = vB[4];
   v[0].tu = 1.0f;  v[0].tv = 1.0f;
   v[1].tu = 0.0f;  v[1].tv = 1.0f;

   v[3] = vB[1];
   v[4] = vB[4];
   v[5] = vB[2];
   v[3].tu = 0.0f;  v[3].tv = 1.0f;
   v[5].tu = 1.0f;  v[5].tv = 1.0f;

   v[6] = vB[2];
   v[7] = vB[4];
   v[8] = vB[3];
   v[6].tu = 0.0f;  v[6].tv = 1.0f;
   v[8].tu = 1.0f;  v[8].tv = 1.0f;

   v[9]  = vB[3];
   v[10] = vB[4];
   v[11] = vB[0];
   v[9].tu  = 0.0f;  v[9].tv  = 1.0f;
   v[11].tu = 1.0f;  v[11].tv = 1.0f;

   // lower pyramid
   v[12] = vB[1];
   v[13] = vB[5];
   v[14] = vB[0];
   v[12].tu = 1.0f;  v[12].tv = 0.0f;
   v[14].tu = 0.0f;  v[14].tv = 0.0f;

   v[15] = vB[2];
   v[16] = vB[5];
   v[17] = vB[1];
   v[15].tu = 1.0f;  v[15].tv = 0.0f;
   v[17].tu = 0.0f;  v[17].tv = 0.0f;

   v[18] = vB[3];
   v[19] = vB[5];
   v[20] = vB[2];
   v[18].tu = 1.0f;  v[18].tv = 0.0f;
   v[20].tu = 0.0f;  v[20].tv = 0.0f;

   v[21] = vB[0];
   v[22] = vB[5];
   v[23] = vB[3];
   v[21].tu = 1.0f;  v[21].tv = 0.0f;
   v[23].tu = 0.0f;  v[23].tv = 0.0f;


   // build the eight faces
   for (int n=0; n<8; n++) {
      // construct polygon
      Poly[n].Reset();
      Poly[n].SetVertices(&v[n*3], 3);
      Poly[n].SetIndices(i, 3);

      if (pPoly) pPoly[n].CopyOf(&Poly[n]);

      // add this side to the mesh
      if (pMesh) pMesh->AddPolygon(Poly[n]);
      }
   } // CreateLight
/*----------------------------------------------------------------*/


/**
 * Creates a cube with its sides facing outwards. Returns the cube
 * as polymesh. If pPoly is an array it will also be filled with
 * the cubes faces.
 */
void CGeometry::CreateCube(CPolymesh *pMesh, CPolygon *pPoly, ZFXVector vcP,
                           float fW, float fH, float fD) {
   CPolygon  Poly[6];
   WORD   i[36] = {  3, 0, 1,  3, 1, 2,   // top
                     2, 3, 0,  2, 0, 1,   // right
                     2, 1, 0,  3, 2, 0,   // left
                     0, 1, 2,  2, 3, 0,   // back
                     2, 3, 0,  1, 2, 0,   // front
                     2, 3, 0,  0, 1, 2 }; // bottom
   
   // create polylist for inverted cube
   CreateCubeInv(NULL, &Poly[0], vcP, fW, fH, fD);

   // set new indices in changed clock order
   for (int n=0; n<6; n++) {
      // change indices
      Poly[n].SetIndices(&i[n*6], 6);

      if (pPoly) pPoly[n].CopyOf(&Poly[n]);

      // add this side to the mesh
      if (pMesh) pMesh->AddPolygon(Poly[n]);
      }
   } // CreateCube
/*----------------------------------------------------------------*/


/**
 * Creates a cube with its sides facing inwards. Returns the cube
 * as polymesh. If pPoly is an array it will also be filled with
 * the cubes faces.
 */
void CGeometry::CreateCubeInv(CPolymesh *pMesh, CPolygon *pPoly, ZFXVector vcP,
                              float fW, float fH, float fD) {
   CPolygon Poly[6];
   VERTEX   v[24];
   WORD     i[36] = {  1, 0, 3,  2, 1, 3,   // top
                       0, 3, 2,  1, 0, 2,   // right
                       0, 1, 2,  0, 2, 3,   // left
                       2, 1, 0,  0, 3, 2,   // back
                       0, 3, 2,  0, 2, 1,   // front
                       0, 3, 2,  2, 1, 0 }; // bottom
   
   // top rectangle
   v[0].x = vcP.x - (fW / 2.0f);
   v[0].y = vcP.y + (fH / 2.0f);
   v[0].z = vcP.z - (fD / 2.0f);
   v[0].tu = 0.0f;  v[0].tv = 0.0f;

   v[1].x = vcP.x - (fW / 2.0f);
   v[1].y = vcP.y + (fH / 2.0f);
   v[1].z = vcP.z + (fD / 2.0f);
   v[1].tu = 0.0f;  v[1].tv = 1.0f;

   v[2].x = vcP.x + (fW / 2.0f);
   v[2].y = vcP.y + (fH / 2.0f);
   v[2].z = vcP.z + (fD / 2.0f);
   v[2].tu = 1.0f;  v[2].tv = 1.0f;

   v[3].x = vcP.x + (fW / 2.0f);
   v[3].y = vcP.y + (fH / 2.0f);
   v[3].z = vcP.z - (fD / 2.0f);
   v[3].tu = 1.0f;  v[3].tv = 0.0f;

   // right side rectanlge
   v[4]    = v[3];
   v[4].tu = 1.0f;  
   v[4].tv = 0.0f;
   v[5]    = v[2];
   v[5].tu = 0.0f;  
   v[5].tv = 0.0f;

   v[6].x  = vcP.x + (fW / 2.0f);
   v[6].y  = vcP.y - (fH / 2.0f);
   v[6].z  = vcP.z + (fD / 2.0f);
   v[6].tu = 0.0f;  v[6].tv = 1.0f;

   v[7].x  = vcP.x + (fW / 2.0f);
   v[7].y  = vcP.y - (fH / 2.0f);
   v[7].z  = vcP.z - (fD / 2.0f);
   v[7].tu = 1.0f;  v[7].tv = 1.0f;

   // left side rectangle
   v[8]    = v[0];
   v[8].tu = 0.0f;  
   v[8].tv = 0.0f;
   v[9]    = v[1];
   v[9].tu = 1.0f;  
   v[9].tv = 0.0f;

   v[10].x = vcP.x - (fW / 2.0f);
   v[10].y = vcP.y - (fH / 2.0f);
   v[10].z = vcP.z + (fD / 2.0f);
   v[10].tu = 1.0f;  v[10].tv = 1.0f;

   v[11].x = vcP.x - (fW / 2.0f);
   v[11].y = vcP.y - (fH / 2.0f);
   v[11].z = vcP.z - (fD / 2.0f);
   v[11].tu = 0.0f;  v[11].tv = 1.0f;

   // back side rectanlge
   v[12]    = v[2];
   v[12].tu = 1.0f; 
   v[12].tv = 0.0f;
   v[13]    = v[1];
   v[13].tu = 0.0f; 
   v[13].tv = 0.0f;
   v[14]    = v[10];
   v[14].tu = 0.0f; 
   v[14].tv = 1.0f;
   v[15]    = v[6];
   v[15].tu = 1.0f; 
   v[15].tv = 1.0f;

   // front side rectangle
   v[16]    = v[0];
   v[16].tu = 1.0f; 
   v[16].tv = 0.0f;
   v[17]    = v[3];
   v[17].tu = 0.0f;  
   v[17].tv = 0.0f;
   v[18]    = v[7];
   v[18].tu = 0.0f; 
   v[18].tv = 1.0f;
   v[19]    = v[11];
   v[19].tu = 1.0f; 
   v[19].tv = 1.0f;

   // bottom side rectangle
   v[20]    = v[7];
   v[20].tu = 1.0f; 
   v[20].tv = 1.0f;
   v[21]    = v[6];
   v[21].tu = 1.0f; 
   v[21].tv = 0.0f;
   v[22]    = v[10];
   v[22].tu = 0.0f;  
   v[22].tv = 0.0f;
   v[23]    = v[11];
   v[23].tu = 0.0f; 
   v[23].tv = 1.0f;

   // build the six faces
   for (int n=0; n<6; n++) {
      // construct polygon
      Poly[n].Reset();
      Poly[n].SetVertices(&v[n*4], 4);
      Poly[n].SetIndices(&i[n*6], 6);

      if (pPoly) pPoly[n].CopyOf(&Poly[n]);

      // add this side to the mesh
      if (pMesh) pMesh->AddPolygon(Poly[n]);
      }
   } // CreateInvCube
/*----------------------------------------------------------------*/

