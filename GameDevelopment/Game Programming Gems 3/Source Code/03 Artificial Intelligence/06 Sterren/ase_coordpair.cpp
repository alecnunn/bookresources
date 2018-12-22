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
 *     purpose: simple struct to handle coordinates
*/


#include "stdafx.h"
#include "ase_coordpair.h"

using namespace std;

#include <cmath>
#include <cassert>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



bool ASE_CoordPair::operator==(const ASE_CoordPair& anotherPair) const
  {
    return (   ( x == anotherPair.x )
            && ( y == anotherPair.y )
           );
  }
