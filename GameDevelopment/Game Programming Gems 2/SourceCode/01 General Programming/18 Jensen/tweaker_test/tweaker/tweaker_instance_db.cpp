/* Copyright (C) Lasse Staff Jensen, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Lasse Staff Jensen, 2001"
 */
//Let's wait til next version of MSVC :)
#pragma warning( disable: 4503 )

#include "tweaker_instance_db.h"

namespace eqx {

/*! Add a new tweaker to database with the given \a i_cInstanceName name and
    \a i_nInstanceID ID, and put it in the given \a i_cCategory category.
*/
Tweaker_c* 
TweakerInstanceDB_c::AddTweaker( const std::string& i_cInstanceName, unsigned int i_nInstanceID, 
                                 const std::string& i_cCategory /*= "root"*/ ) {
  Tweaker_c* pcTweaker = new Tweaker_c( i_cInstanceName );

  TweakerInstance_s sInstance;
  sInstance.m_pcTweaker = pcTweaker;
  sInstance.m_nInstanceID = i_nInstanceID;
  
  InstanceMMap_t& cInstance = GetInstancesMap()[ i_cCategory ];
#ifdef _DEBUG
   for( iInstanceMMap_t iInstance = cInstance.begin(); iInstance != cInstance.end(); ++iInstance ) {
//     assert( iInstance->second.m_nInstanceID != i_nInstanceID ); // make sure it's unique!
   }
#endif
  cInstance.insert( std::make_pair( pcTweaker->GetName(), sInstance ) );
  
  return pcTweaker;         
} // AddTweaker

/*! Delete all the instances with the given \a i_nInstanceID instance name from the DB.
      \note We don't remove the category map even if it ends up empty. This should help to improve the 
      performance in case you're removing and creating tweakers all the time (in copy constructor, maybe).
  */
void 
TweakerInstanceDB_c::DeleteInstances( unsigned int i_nInstanceID ) {
  //search all categories
  for( iCategoryMap_t iCategory = GetInstancesMap().begin(); iCategory!=GetInstancesMap().end(); ++iCategory ) {
    for( iInstanceMMap_t iInstance = iCategory->second.begin(); iInstance!=iCategory->second.end(); /*nop*/ ) {
      if( iInstance->second.m_nInstanceID == i_nInstanceID ) {
        iInstanceMMap_t iTemp = iInstance;
        delete iInstance->second.m_pcTweaker;
        iInstance = iCategory->second.erase( iTemp );
      } else {
        ++iInstance;
      }
    }
  }
} // DeleteInstances

/*! Get the unique \a i_nInstanceID tweaker with the name \a i_cID from the given \a i_cInstances
      map of instances.
  */
Tweaker_c*
TweakerInstanceDB_c::GetTweaker( const InstanceMMap_t& i_cInstances, const std::string& i_cID, 
                                 const unsigned int i_nInstaceID ) {
  Range_t cRange = i_cInstances.equal_range( i_cID );
  while( cRange.first != cRange.second ) {
    if( i_nInstaceID == (*cRange.first).second.m_nInstanceID ) {
      return (*cRange.first).second.m_pcTweaker;
    }
    ++cRange.first;
  }
  return NULL;  // No instances found
} // GetTweaker


TweakerInstanceDB_c::CategoryMap_t& 
TweakerInstanceDB_c::GetInstancesMap() {
  static CategoryMap_t cInstances;
  return cInstances;
} // GetInstancesMap

}; // eqx 