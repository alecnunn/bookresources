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
 *     purpose: class to represent threat and terrain covered by threat
*/
#include "stdafx.h"
#include "ase_threat.h"

#include <algorithm> // though MSVC6 doesn't feature min/max there
using namespace std;

#include <cmath>
#include <cassert>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


ASE_Threat::ASE_Threat(unsigned int aLocationIdx)
  : m_Position(aLocationIdx)
  {
  }


void ASE_Threat::SetPositionsUnderFire(const PositionsUnderFire& thePositionsUnderFire)
  {
    m_PositionsUnderFire.clear();
    m_PositionsUnderFire.resize(thePositionsUnderFire.size());
    copy(thePositionsUnderFire.begin(), thePositionsUnderFire.end(), m_PositionsUnderFire.begin());
    sort(m_PositionsUnderFire.begin(), m_PositionsUnderFire.end());
  }


bool ASE_Threat::OccupiesPosition(unsigned int aLocationIdx) const
  {
    return ( m_Position == aLocationIdx );
  }


bool ASE_Threat::IsAbleToFireAtLocation(unsigned int aLocationIdx) const
  {
    return binary_search(m_PositionsUnderFire.begin(), m_PositionsUnderFire.end(), aLocationIdx);
  }
