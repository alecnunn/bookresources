/* Copyright (C) William van der Sterren, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2002"
 */

/***************************************************************************  
 *                                                                            
 *     purpose: class computing radial line-of-sight scan mask/template
*/


#include "stdafx.h"
#include "ase_piescan.h"
#include "ase_losscan.h"

using namespace std;

#include <cassert>
#include <limits>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


ASE_PieScan::ASE_PieScan()
  : m_MaxRange(-1)
  {
  }


void ASE_PieScan::PrepareScan(float aRange)
  {
    assert( aRange > 1 );

    static const float PI     = static_cast<float>(4.0f * atan(1));

    m_MaxRange = aRange;
    m_Rays.clear();
    m_Rays.reserve(static_cast<int>(2 * PI * aRange + 1));

    /*! generate all positions (cx,cy) on a circle with r=aRange surrounding (0,0)
        and the steps towards it

        currently used 0.5 degree steps
    */
    for ( float r = 0; r < 2 * PI; r += (PI/360) )
      {
        float fx;
        float fy;
        fx = static_cast<float>(sin(r)) * aRange;
        fy = static_cast<float>(cos(r)) * aRange;

        PrepareScanToPosition(fx, fy);
      }
    assert( !m_Rays.empty() );
  }
 

void ASE_PieScan::PrepareScanToPosition(float anX, float anY)
  {
    ASE_Ray       ray;
    CoordPairList positions;

    const float kStepSize = 0.25f;

    float dx;
    float dy;
    dx = anX;
    dy = anY;

    float l;
    l  = m_MaxRange/ kStepSize;
    dx /= l;
    dy /= l;
  
    int px;
    int py;
    px = 0;
    py = 0;

    float fx;
    float fy;
    fx = 0;
    fy = 0;  

    for ( float dist = 0; dist < m_MaxRange; dist += kStepSize )
      {
        fx += dx;
        fy += dy;

        int x;
        int y;
        x = static_cast<int>(floor(fx + 0.5));
        y = static_cast<int>(floor(fy + 0.5));
        if (   ( x != px ) 
            || ( y != py ) 
           )
          {
            px = x;
            py = y;

            // it is a valid line-of-fire location,
            ASE_CoordPair position(x, y);
            positions.push_back(position);
          }
      }

    if ( !IsIdenticalToLastRay(positions) )
      {
        unsigned int sector;
        sector = ASE_LineOfSightScan::GetSectorForLine(px, py);

        ray.SetRay(sector, positions);

        m_Rays.push_back(ray);
      }
  }


unsigned int ASE_PieScan::GetNumberOfRays() const
  {
    assert( m_MaxRange > 0 );
    return m_Rays.size();
  }


void         ASE_PieScan::GetRayForLocation(unsigned int        aRayNumber, 
                                            const ASE_CoordPair aPosition, 
                                            CoordPairList&      theRayTrace
                                           ) const
  {
    assert( m_MaxRange > 0 );
    assert( aRayNumber < m_Rays.size() );

    m_Rays[aRayNumber].GetRayForLocation(aPosition, theRayTrace);
  }


bool ASE_PieScan::IsRayInSector(unsigned int aRayNumber, unsigned int aSector) const
  {
    assert( m_MaxRange > 0 );
    assert( aRayNumber < m_Rays.size() );

    return ( m_Rays[aRayNumber].IsPartOfSector(aSector) ); 
  }


bool ASE_PieScan::IsIdenticalToLastRay(const CoordPairList& aNewRay) const
  {
    return (   ( !m_Rays.empty() )
            && ( m_Rays.back().HasSameRay(aNewRay) )
           );
  }
