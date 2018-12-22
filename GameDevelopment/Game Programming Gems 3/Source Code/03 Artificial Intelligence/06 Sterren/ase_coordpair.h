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


#ifndef _ASE_COORDPAIR_H_
#define _ASE_COORDPAIR_H_

#if _MSC_VER > 1000
#pragma once
#endif



#include <vector>

struct ASE_CoordPair 
  { 
    int x; 
    int y; 
    ASE_CoordPair(int anX, int anY)
      : x(anX), y(anY)
      { }
    ASE_CoordPair()
      { }
    bool operator==(const ASE_CoordPair& anotherPair) const;
  };
typedef std::vector<ASE_CoordPair> CoordPairList;



#endif // _ASE_COORDPAIR_H_