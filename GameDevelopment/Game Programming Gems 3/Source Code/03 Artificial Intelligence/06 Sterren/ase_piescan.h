/* Copyright (C) William van der Sterren, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2002"
 */

/* Copyright (C) James Matthews, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) James Matthews, 2001"
 */

/***************************************************************************  
 *                                                                            
 *     purpose: class computing radial line-of-sight scan mask/template
*/


#ifndef _ASE_PIESCAN_H_
#define _ASE_PIESCAN_H_

#if _MSC_VER > 1000
#pragma once
#endif


#include "ase_ray.h"


class ASE_PieScan 
{ 
  public:
    ASE_PieScan();

    void PrepareScan(float aRange); 

  public: // queries
    unsigned int GetNumberOfRays() const;
    void         GetRayForLocation(unsigned int        aRayNumber, 
                                   const ASE_CoordPair aPosition, 
                                   CoordPairList&      theRayTrace
                                  ) const;

    bool         IsRayInSector(unsigned int aRayNumber, unsigned int aSector) const;

  protected: // scan computations
    void PrepareScanToPosition(float anX, float anY); 

    bool IsIdenticalToLastRay(const CoordPairList& aNewRay) const;
  
  private:
    typedef std::vector<ASE_Ray> Rays;

    float         m_MaxRange;
    Rays          m_Rays;
};



#endif // _ASE_PIESCAN_H_