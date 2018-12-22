/* Copyright (C) Lasse Staff Jensen, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Lasse Staff Jensen, 2001"
 */
#ifndef EQX_TWEAKER_DEFINED
#define EQX_TWEAKER_DEFINED

/*!
  \file tweaker.h

  \brief Declaration of the eqx::Tweaker_c class which defines an interface 
   and functions to tweak a tweakable.

  \author Lasse.Staff.Jensen@funcom.com
*/

// disable "warning C4786: 'argument' : identifier was truncated to '255' 
// characters in the debug information"
#pragma warning( disable : 4786 )

// - Standar lib include -
#include <string>
#include <iterator>
#include <map>

// - Local includes -
#include "tweakable.h"

namespace eqx {

// I'm sorry this class is a mess, but Microsoft VS6.0 won't compile the
// template members without having the implementation directly in the header
// (and no, inlining won't work :-(((( ). Please use the docs!

//=============================================================================
//!       -== Tweaker_c class definision ==-
//----------------------------------------------------------------------------- 

class Tweaker_c {
public:
  
  // - Typedefs -
  typedef std::map<std::string, eqx::TweakableBase_c*> 
    TweakableMap_t;
  typedef TweakableMap_t::iterator
    iTweakableMap_t;
  typedef TweakableMap_t::const_iterator
    iConstTweakableMap_t;
  typedef std::pair<TweakableMap_t::iterator, bool> 
    iTweakableMap_pair_t;
  
  
  
  //! Ctor 
  Tweaker_c( const std::string& i_cName = std::string() ) { m_cName = i_cName; } 
  
  
  /*! Add a tweakable to the tweakable database.
      The given \a i_cTweakableID is an unique string to identify the given \a i_pxValue
      tweakable. <br>
      If \a i_bAddType is set to \c true then we will add type information for this tweakable. <br>
      If \a i_pfCallback is not \c null callback function will also be stored with the given
      \a i_pCallbackData data pointer.
  */
  template<class Value_x>
  void AddTweakable( Value_x* i_pxValue, std::string i_cTweakableID,
    bool i_bAddType = true, TweakableBase_c::Function_t* i_pfCallback = NULL,
    void* i_pCallbackData = NULL ) {
    iTweakableMap_pair_t cInsertResult;
	 
	  // 1. Insert the tweakable
    if( i_bAddType == true ) {
      if( i_pfCallback == NULL ) {
        cInsertResult = m_cTweakable_map.insert(
          std::make_pair( i_cTweakableID, 
          new TweakableType_c<Value_x>( i_pxValue, TweakableBase_c::GetTypeID( *i_pxValue ) ) ) );
      } else {
         cInsertResult = m_cTweakable_map.insert(
          std::make_pair( i_cTweakableID, 
          new TweakableCallbackType_c<Value_x>( i_pxValue, TweakableBase_c::GetTypeID( *i_pxValue ), 
            i_pfCallback, i_pCallbackData ) ) );
      }
    } else {
      if( i_pfCallback == NULL ) {
        cInsertResult = m_cTweakable_map.insert(
		      std::make_pair( i_cTweakableID, new Tweakable_c<Value_x>( i_pxValue ) ) );
      } else {
        cInsertResult = m_cTweakable_map.insert(
		      std::make_pair( i_cTweakableID, new TweakableCallback_c<Value_x>( i_pxValue, i_pfCallback, i_pCallbackData ) ) );
      }
    }

    // 2. Assert if not OK since we need it to work!
    assert( cInsertResult.second == true );
        
  } // AddTweakable


  /*! Add a tweakable to the tweakable database.
      This add method stores \a i_xMin and \a i_xMax values with the tweakable. These
      constraints are enforced by the TweakValue() method. <br>
      If \a i_bAddType is set to \c true then we will add type information for this tweakable. <br>
      If \a i_pfCallback is not \c null callback function will also be stored with the given \a
      i_pCallbackData data pointer.
  */
  template<class Value_x>
  void AddTweakable( Value_x* i_pxValue, const std::string& i_cTweakableID,
   const Value_x& i_xMin, const Value_x& i_xMax, bool i_bAddType = true, 
   TweakableBase_c::Function_t* i_pfCallback = NULL, void* i_pCallbackData = NULL ) {
        
    iTweakableMap_pair_t cInsertResult;
	    
    // 1. Insert the tweakable
    if( i_bAddType == true ) {
      if( i_pfCallback == NULL ) {
        cInsertResult = m_cTweakable_map.insert(
          std::make_pair( i_cTweakableID, 
          new TweakableTypeRange_c<Value_x>( i_pxValue, i_xMin, i_xMax, TweakableBase_c::GetTypeID( *i_pxValue )  ) ) );
      } else {
        cInsertResult = m_cTweakable_map.insert(
          std::make_pair( i_cTweakableID, 
          new TweakableCallbackTypeRange_c<Value_x>( i_pxValue, i_xMin, i_xMax, 
            TweakableBase_c::GetTypeID( *i_pxValue ), i_pfCallback, i_pCallbackData ) ) );
      }
    } else {
      if( i_pfCallback == NULL ) {
        cInsertResult = m_cTweakable_map.insert(
          std::make_pair( i_cTweakableID, 
          new TweakableRange_c<Value_x>( i_pxValue, i_xMin, i_xMax ) ) );
      } else {
        cInsertResult = m_cTweakable_map.insert(
          std::make_pair( i_cTweakableID, 
          new TweakableCallbackRange_c<Value_x>( i_pxValue, i_xMin, i_xMax, i_pfCallback, i_pCallbackData ) ) );
      }
    }
	
    // 2. Assert if not OK since we need it to work!
    assert( cInsertResult.second == true );
        
  } // AddTweakable


  /*! Set the tweakable with the \a i_cID ID to the given \a i_xValue. 
      If \a i_bDoCallback = true then the callback function (if any) will be called.
      \return 
      \a e_UNKNOW_KEY   - Can't find any tweakable with the given \a i_cID <br>
      \a e_MIN_EXCEEDED - Given value less than minimum aloved <br>
      \a e_MAX_EXCEEDED - Given value larger than maximum aloved <br>
      \a e_OK           - Tweakable set equal to given value

      \note All IDs stored in the database are \b case \b sensetive!
  */
  template<class Value_x>
  TweakError_e TweakValue( const std::string& i_cID, const Value_x& i_xValue, bool i_bDoCallback = true ) {
    TweakableBase_c* pcTweakable;
	  iTweakableMap_t iSearchResult = m_cTweakable_map.find( i_cID );
    if( iSearchResult == m_cTweakable_map.end() ) { return e_UNKNOWN_KEY; }
	  pcTweakable = (*iSearchResult).second;

    // 1. Check the limits
    TweakError_e eError = pcTweakable->ConstrainValue( reinterpret_cast<const void* const>( &i_xValue ) );
    if( eError != e_OK ) { return eError; }
    // 2. Check the type
    assert( pcTweakable->TestType( TweakableBase_c::GetTypeID( i_xValue ) ) == true );
    // 3. Change the value 
    pcTweakable->ChangeValueAndDoCallback( reinterpret_cast<const void* const>( &i_xValue ), 
      this, i_cID, i_bDoCallback );

    return e_OK;
  } // TweakValue

  /*! Get the value of the tweakable with the given \a i_cID and store it at 
      the given \a o_pxValue location. 
      \return 
      \a e_UNKNOW_KEY  - Can't find any tweakable with the given \a i_cID <br>
      \a e_OK          - Value stored succesfully in \a o_pxValue 
  */
  template<class Value_x>
  TweakError_e GetValue( const std::string& i_cID, Value_x* o_pxValue ) const {
    TweakableBase_c* pcTweakable;
	  iTweakableMap_t iSearchResult = m_cTweakable_map.find( i_cID );
    if( iSearchResult == m_cTweakable_map.end() ) { return e_UNKNOWN_KEY; }
	  pcTweakable = (*iSearchResult).second;

    assert( pcTweakable->TestType( TweakableBase_c::GetTypeID( *o_pxValue ) ) == true );
  
    *o_pxValue = *(reinterpret_cast<Value_x*>( pcTweakable->m_pData ) );
    return e_OK;   
  } // GetValue

  /*! Get limits of the given tweakable and store them into \a o_pxMin and \a o_pxMax.
      \return
      \a e_UNKNOWN_KEY - Can't find any tweakable with the given \a i_cID <br> 
      \a e_NOT_STORED  - The tweakable doesn't have any limits defined <br>
      \a e_OK          - Value stored succesfully
  */
  template<class Value_x>
  TweakError_e GetLimits( const std::string& i_cID, Value_x* o_pxMin, Value_x* o_pxMax ) const {
    iTweakableMap_t iSearchResult = m_cTweakable_map.find( i_cID );
    if ( iSearchResult == m_cTweakable_map.end() ) { return e_UNKNOWN_KEY; }

    assert( iSearchResult->second->TestType( TweakableBase_c::GetTypeID( *o_pxMin ) ) == true );

    return iSearchResult->second->GetMinMax( o_pxMin, o_pxMax );
  }

  //! \return true if tweakable with the given name exists
  bool DoesTweakableExists( const std::string& i_cID ) const {
    return m_cTweakable_map.find( i_cID ) != m_cTweakable_map.end();
  }

  //! Get the name of this tweaker
  const std::string& GetName() const { return m_cName; };
  
  //! Get the number of tweakables stored in this tweaker
  unsigned int GetNoOfTweakables() const { return m_cTweakable_map.size(); }

  // Some function to ease the GUI implementation -> remove'em if you don't need it!
  const iConstTweakableMap_t begin() const { return m_cTweakable_map.begin(); }
  const iConstTweakableMap_t end() const { return m_cTweakable_map.end(); }
  const iConstTweakableMap_t find( const std::string& i_cName ) const { return m_cTweakable_map.find( i_cName ); }

private:
  //! Destructor - force user to use TweakerInstanceDB_c::DeleteInstances()
  virtual ~Tweaker_c();

  //! All the tweakables this tweaker controls
  TweakableMap_t  m_cTweakable_map;
  std::string     m_cName;

  friend class TweakerInstanceDB_c;
}; // Tweaker_c


}; // eqx 



#endif // EQX_TWEAKER_DEFINED