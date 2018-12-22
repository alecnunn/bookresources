// FILE: CGeometry.h 

#ifndef CGEOMETRIE_H
#define CGEOMETRIE_H

#include <windows.h>
#include "ZFX.h"
#include "CPolygon.h"
#include "CPolymesh.h"


// P R O T O T Y P E S /////////////////////////////////////////////

class CGeometry {
   public:
      CGeometry(void)          {  }
      virtual ~CGeometry(void) {  }

      static void Snap(ZFXVector *pVC, float fScale);

      // CREATE GEOMETRIE

      void CreateLight(CPolymesh*, CPolygon*, ZFXVector vcPos,
                       float fW, float fH, float fD);

      
      void CreateLightInv(CPolymesh*, CPolygon*, ZFXVector vcPos,
                          float fW, float fH, float fD);

      // create a cube with six single faces
      void CreateCube(CPolymesh*, CPolygon*, ZFXVector vcPos,
                      float fW, float fH, float fD);

      void CreateCubeInv(CPolymesh*, CPolygon*, ZFXVector vcPos,
                         float fW, float fH, float fD);


      // REWORK GEOMETRIE
      // maybe to come...

   }; // class
/*----------------------------------------------------------------*/

#endif
