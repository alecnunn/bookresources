/* Copyright (C) Lasse Staff Jensen, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Lasse Staff Jensen, 2001"
 */
#ifndef TWEAKER_INSTANCE_DB
#define TWEAKER_INSTANCE_DB

/*!
  \file tweaker_instance_db.h

  \brief Declaration of the eqx::TweakerInstanceDB_c class, 
         that defines functionality to store and retrive all the tweaker instances.

  \author Lasse.Staff.Jensen@funcom.com
*/

#pragma warning( disable : 4786 )
#include <map>
#include <assert.h>
#include "tweaker.h"


#define TWEAKER_CREATE_ID( a ) eqx::TweakerInstanceDB_c::GetUniqueID( reinterpret_cast< void* >( a ) )

namespace eqx {

class TweakerInstanceDB_c {
public:
  struct TweakerInstance_s {
    Tweaker_c* m_pcTweaker;
    unsigned int  m_nInstanceID;
  }; // TweakerInstance_s
  

  // - Typedefs -
  typedef std::multimap<std::string, TweakerInstance_s> 
    InstanceMMap_t;
  typedef InstanceMMap_t::iterator 
    iInstanceMMap_t;
  typedef InstanceMMap_t::const_iterator
    iConstInstanceMMap_t;
  typedef std::pair<iConstInstanceMMap_t, iConstInstanceMMap_t>
    ConstRange_t;
  typedef std::pair<iInstanceMMap_t, iInstanceMMap_t>
    Range_t;
  
  typedef std::map<std::string, InstanceMMap_t>
    CategoryMap_t;
  typedef CategoryMap_t::iterator 
    iCategoryMap_t;
  typedef CategoryMap_t::const_iterator
    iConstCategoryMap_t;
  
  static unsigned int GetUniqueID( const void* i_pInstance ) {
    return reinterpret_cast<unsigned int>( i_pInstance );
  } // GetUniqueID

  
  static Tweaker_c* AddTweaker( const std::string& i_cInstanceName, unsigned int i_nInstanceID, 
    const std::string& i_cCategory = "root" );
  
  static void DeleteInstances( unsigned int i_nInstanceID );

  static Tweaker_c* GetTweaker( const InstanceMMap_t& i_cInstances, const std::string& i_cID, 
    const unsigned int i_nInstaceID );
  
  /*! Get all the tweaker instances with the name \a i_cID, from the given \a i_cInstances
      map of instances.
  */
  static ConstRange_t
  GetInstances( const InstanceMMap_t& i_cInstances, const std::string& i_cID ) {
    return i_cInstances.equal_range( i_cID );
  } // GetInstances

  /*! Get all the instances stored under the given \a i_cCategory category.
  */
  static iCategoryMap_t
  GetCategory( const std::string& i_cCategory ) { 
    return GetInstancesMap().find( i_cCategory ); 
  } // GetInstances

  /*! \return 
      true  - if the given \a i_cCategory excist <br>
      false - otherwise
  */
  static bool
  CategoryExists( const std::string& i_cCategory ) {
    return GetInstancesMap().find( i_cCategory ) != GetInstancesMap().end();
  }

private:
  
  /*! This function forces the compiler to create the cInstance before used from any other
      static functions.
  */  
  static CategoryMap_t& GetInstancesMap();

}; // TweakerInstanceDB


}; // eqx


#endif

   
