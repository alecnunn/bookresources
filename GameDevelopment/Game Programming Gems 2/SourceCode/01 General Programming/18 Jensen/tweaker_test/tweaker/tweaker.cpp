/* Copyright (C) Lasse Staff Jensen, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Lasse Staff Jensen, 2001"
 */

// disable "warning C4786: 'argument' : identifier was truncated to '255' 
// characters in the debug information"
#pragma warning( disable : 4786 )

// - Includes -


#include "tweaker.h"

#include <assert.h>
//#include <strstream>


namespace eqx {


//-----------------------------------------------------------------------------	
// - Destructor -
//-----------------------------------------------------------------------------	

Tweaker_c::~Tweaker_c() {
   for( iTweakableMap_t iBegin = m_cTweakable_map.begin(); iBegin != m_cTweakable_map.end(); ++iBegin ) {
    delete (*iBegin).second;
  }
  m_cTweakable_map.erase( m_cTweakable_map.begin(), m_cTweakable_map.end() );
} // ~Tweaker_c


}; // eqx