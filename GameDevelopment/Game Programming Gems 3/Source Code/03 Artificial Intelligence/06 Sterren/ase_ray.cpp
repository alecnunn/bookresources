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
 *     purpose: class to represent ray (part of the 'pie' scan template)
*/


#include "stdafx.h"
#include "ase_ray.h"

using namespace std;

#include <cassert>
#include <limits>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// work around problems with MSVC6 not using STL min and max, but macro instead
#undef max

ASE_Ray::ASE_Ray()
  {
    m_Sector = numeric_limits<unsigned int>::max(); // unacceptable value, use to check initialization
  }


void ASE_Ray::SetRay(unsigned int aSector, const CoordPairList& aTrace)
  {
    assert( m_Sector == numeric_limits<unsigned int>::max() );

    m_Sector = aSector;
    m_Trace.clear();
    m_Trace.resize(aTrace.size());
    copy(aTrace.begin(), aTrace.end(), m_Trace.begin());
    assert( m_Trace.size() == aTrace.size() );
  }


void ASE_Ray::GetRayForLocation(const ASE_CoordPair aPosition, CoordPairList& theRayTrace) const
  {
    assert( m_Sector != numeric_limits<unsigned int>::max() );

    CoordPairList::const_iterator coord;
    for ( coord =  m_Trace.begin(); coord != m_Trace.end(); ++coord )
      {
        ASE_CoordPair pair(aPosition.x + coord->x, aPosition.y + coord->y);
        theRayTrace.push_back(pair);
      }
  }


bool ASE_Ray::IsPartOfSector(unsigned int aSector) const
  {
    assert( m_Sector != numeric_limits<unsigned int>::max() );

    return ( aSector == m_Sector );
  }


bool ASE_Ray::HasSameRay(const CoordPairList& anotherTrace) const
  {
    if ( m_Trace.size() != anotherTrace.size() )
      return false;

    CoordPairList::const_iterator me;
    CoordPairList::const_iterator other;
    for ( me =  m_Trace.begin(), other = anotherTrace.begin();
          me != m_Trace.end();
          ++me, ++other
        )
      {
        if ( !((*me) == (*other)) )
          return false;
      }
    return true; 
  }
