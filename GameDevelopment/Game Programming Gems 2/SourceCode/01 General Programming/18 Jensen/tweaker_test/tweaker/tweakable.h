/* Copyright (C) Lasse Staff Jensen, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Lasse Staff Jensen, 2001"
 */
#ifndef EQX_TWEAKABLE_DEFINED
#define EQX_TWEAKABLE_DEFINED

/*!
  \file tweakable.h

  \brief Declaration of the following classes: <br>
          \li eqx::TweakableBase_c
          \li eqx::Tweakable_c
          \li eqx::TweakableType_c 
          \li eqx::TweakableRange_c
          \li eqx::TweakableTypeRange_c 
          \li eqx::TweakableCallback_c
          \li eqx::TweakableCallbackRange_c
          \li eqx::TweakableCallbackType_c 
          \li eqx::TweakableCallbackTypeRange_c 

  Note: The point of having these many classes is to reduce memory.

  \author Lasse.Staff.Jensen@funcom.com
*/


#include "datatypes.h"

#include <assert.h>
#include <string>

namespace eqx {

enum TweakError_e { 
    e_OK, 
    e_UNKNOWN_KEY, 
    e_NOT_STORED,
    e_MIN_EXCEEDED, 
    e_MAX_EXCEEDED 
}; // TweakError_e


class Tweaker_c;

//=============================================================================
//!        -== TweakableBase_c class definition ==-
//----------------------------------------------------------------------------- 
class TweakableBase_c {
public:
  typedef bool ( Function_t )( Tweaker_c* const i_pcTweaker, const std::string& i_cID, void* i_pcData );
  
  //! ctor
  TweakableBase_c( void* const i_pData ) : m_pData( i_pData ) {;}
  //! dtor 
  virtual ~TweakableBase_c() { /*NOP*/;} 

   /*! This function are used to get the correct type information.
    \return \a TypeID_c* - Pointer to the correct type (compare pointer address)
  */
  template <class T>
  static const TypeID_c* const GetTypeID( const T& i_cValue ) {
    return Identifier_c<T>::GetType();
  }

  // Instead of RTTI we will use virtual members for the three different type of
  // data we have => see errata for more info!
  virtual TweakError_e ConstrainValue( const void* const i_pValue ) const { return e_OK; }
  virtual bool TestType( const TypeID_c* const i_pcType ) const { return true; } 
  virtual void ChangeValueAndDoCallback( const void* const i_pValue,
    Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback ) {;}
  virtual TweakError_e GetMinMax( void* o_pMin, void* o_pMax ) const { return e_NOT_STORED; }  
  virtual const TypeID_c* const GetStoredType() const { return NULL; }

protected:
  friend class Tweaker_c;

  //! Pointer to the variable to be tweaked
  void* const m_pData;

}; // TweakableBase_c

//=============================================================================
//!        -== Tweakable_c class definition ==-
//----------------------------------------------------------------------------- 
template<class T>
class Tweakable_c : public TweakableBase_c {
public:
  
  //! Ctor
  Tweakable_c( T* i_pxData ) : 
    TweakableBase_c( reinterpret_cast<void*>( i_pxData ) ) { ; }

  virtual void ChangeValueAndDoCallback( const void* const i_pValue,
    Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback ); 

private:
  friend class Tweaker_c;

}; // TweakableType_c


//=============================================================================
//!        -== TweakableType_c class definition ==-
//----------------------------------------------------------------------------- 
template<class T>
class TweakableType_c : public TweakableBase_c {
public:
  
  //! Constructor
  TweakableType_c( T* i_pxData, const TypeID_c* i_pcType ) : 
    TweakableBase_c( reinterpret_cast<void*>( i_pxData ) ),
    m_pcType( i_pcType ) { /*NOP*/; }

  virtual bool TestType( const TypeID_c* const i_pcType ) const; 
  virtual void ChangeValueAndDoCallback( const void* const i_pValue,
    Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback );

  inline virtual const TypeID_c* const GetStoredType() const { return m_pcType; }

protected:
  friend class Tweaker_c;
    
  //! Type of data
  const TypeID_c* const m_pcType;    
}; // TweakableType_c


//=============================================================================
//!        -== TweakableRange_c class definition ==-
//----------------------------------------------------------------------------- 
template<class T>
class TweakableRange_c : public TweakableBase_c {
public:
  
  //! Constructors
  TweakableRange_c( T* i_pxData, const T& i_xMin, const T& i_xMax ) : 
    TweakableBase_c( reinterpret_cast<void*> ( i_pxData ) ),
    m_xMin( i_xMin ),
    m_xMax( i_xMax ) { ; }

  virtual TweakError_e ConstrainValue( const void* const i_pValue ) const;
  virtual void ChangeValueAndDoCallback( const void* const i_pValue, 
    Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback );

  //- Access members -
  const T& GetMin() const { return m_xMin; }
  const T& GetMax() const { return m_xMax; }
  virtual TweakError_e GetMinMax( void* o_pMin, void* o_pMax ) const;

protected:
  friend class Tweaker_c;
    
  //! Minimum allowed value
  T m_xMin;
  //! Maximum allowed value
  T m_xMax;     
}; // TweakableTypeRange_c


//=============================================================================
//!        -== TweakableTypeRange_c class definition ==-
//----------------------------------------------------------------------------- 
template<class T>
class TweakableTypeRange_c : public TweakableBase_c {
public:
  
  //! Constructor
  TweakableTypeRange_c( T* i_pxData, const T& i_xMin, const T& i_xMax,
    const TypeID_c* i_pcType ) : 
    TweakableBase_c( reinterpret_cast<void*>( i_pxData ) ),
    m_xMin( i_xMin ),
    m_xMax( i_xMax ),
    m_pcType( i_pcType ) { /*NOP*/; }

  virtual bool TestType( const TypeID_c* const i_pcType ) const;
  virtual TweakError_e ConstrainValue( const void* const i_pValue ) const;
  virtual void ChangeValueAndDoCallback( const void* const i_pValue,
    Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback );

  //- Access members -
  const T& GetMin() const { return m_xMin; }
  const T& GetMax() const { return m_xMax; }    
  virtual TweakError_e GetMinMax( void* o_pMin, void* o_pMax ) const;

  inline virtual const TypeID_c* const GetStoredType() const { return m_pcType; }

protected:
  friend class Tweaker_c;

  //! Type of data
  const TypeID_c* const m_pcType;    

  //! Minimum allowed value
  T m_xMin;
  //! Maximum allowed value
  T m_xMax;     

}; // TweakableTypeRange_c


//=============================================================================
//!        -== TweakableCallback_c class definition ==-
//----------------------------------------------------------------------------- 
template<class T>
class TweakableCallback_c : public TweakableBase_c {
public:
  
  //! Constructors
  TweakableCallback_c( T* i_pxData, TweakableBase_c::Function_t* i_pCallback, void* i_pCallbackData ) : 
    TweakableBase_c( reinterpret_cast<void*>( i_pxData ) ),
    m_pfCallback( i_pCallback ),
    m_pCallbackData( i_pCallbackData ) { ; }

  virtual void ChangeValueAndDoCallback( const void* const i_pValue,
    Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback );

protected:
  friend class Tweaker_c;
    
  //! Callback function
  TweakableBase_c::Function_t*  m_pfCallback;

  //! Pointer to user data (or casted this pointer f.ex.)
  void* m_pCallbackData;
}; // TweakableType_c


//=============================================================================
//!        -== TweakableCallbackRange_c class definition ==-
//----------------------------------------------------------------------------- 
template<class T>
class TweakableCallbackRange_c : public TweakableBase_c {
public:
    
  //! Constructors
  TweakableCallbackRange_c( T* i_pxData, const T& i_xMin, const T& i_xMax, 
    TweakableBase_c::Function_t* i_pCallback, void* i_pCallbackData ) : 
    TweakableBase_c( reinterpret_cast<void*> ( i_pxData ) ),
    m_xMin( i_xMin ),
    m_xMax( i_xMax ),
    m_pfCallback( i_pCallback ),
    m_pCallbackData( i_pCallbackData ) { ; }

  virtual TweakError_e ConstrainValue( const void* const i_pValue ) const;
  virtual void ChangeValueAndDoCallback( const void* const i_pValue,
    Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback );
 
  //- Access members -
  const T& GetMin() const { return m_xMin; }
  const T& GetMax() const { return m_xMax; }
  virtual TweakError_e GetMinMax( void* o_pMin, void* o_pMax ) const;

protected:
  friend class Tweaker_c;
  
  //! Minimum allowed value
  T m_xMin;
  //! Maximum allowed value
  T m_xMax;
    
  //! Callback function
  TweakableBase_c::Function_t*  m_pfCallback;

  //! Pointer to user data (or casted this pointer f.ex.)
  void* m_pCallbackData;

}; // TweakableCallbackRange_c

//=============================================================================
//!        -== TweakableCallbackType_c class definition ==-
//----------------------------------------------------------------------------- 
template<class T>
class TweakableCallbackType_c : public TweakableBase_c {
public:
    
  //! Constructors
  TweakableCallbackType_c( T* i_pxData, const TypeID_c* i_pcType, 
    TweakableBase_c::Function_t* i_pCallback, void* i_pCallbackData ) : 
    TweakableBase_c( reinterpret_cast<void*> ( i_pxData ) ),
    m_pcType( i_pcType ),    
    m_pfCallback( i_pCallback ),
    m_pCallbackData( i_pCallbackData ) { ; }

  virtual bool TestType( const TypeID_c* const i_pcType ) const;
  virtual void ChangeValueAndDoCallback( const void* const i_pValue,
    Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback );
 
  inline virtual const TypeID_c* const GetStoredType() const { return m_pcType; }


protected:
  friend class Tweaker_c;
  // - Datafield -
  
  //! Type of data
  const TypeID_c* const m_pcType;    
 
  //! Callback function
  TweakableBase_c::Function_t*  m_pfCallback;

  //! Pointer to user data (or casted this pointer f.ex.)
  void* m_pCallbackData;

}; // TweakableCallbackType_c

//=============================================================================
//!        -== TweakableCallbackTypeRange_c class definition ==-
//----------------------------------------------------------------------------- 
template<class T>
class TweakableCallbackTypeRange_c : public TweakableBase_c {
public:
    
  //! Constructors
  TweakableCallbackTypeRange_c( T* i_pxData, const T& i_xMin, const T& i_xMax, 
    const TypeID_c* i_pcType, TweakableBase_c::Function_t* i_pCallback, void* i_pCallbackData ) : 
    TweakableBase_c( reinterpret_cast<void*> ( i_pxData ) ),
    m_xMin( i_xMin ),
    m_xMax( i_xMax ),
    m_pcType( i_pcType ),
    m_pfCallback( i_pCallback ),
    m_pCallbackData( i_pCallbackData ) { ; }
 
  virtual bool TestType( const TypeID_c* const i_pcType ) const;
  virtual TweakError_e ConstrainValue( const void* const i_pValue ) const;
  virtual void ChangeValueAndDoCallback( const void* const i_pValue,
    Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback );

  //- Access members -
  const T& GetMin() const { return m_xMin; }
  const T& GetMax() const { return m_xMax; }
  virtual TweakError_e GetMinMax( void* o_pMin, void* o_pMax ) const;

  inline virtual const TypeID_c* const GetStoredType() const { return m_pcType; }

protected:
  friend class Tweaker_c;
  
  //! Type of data
  const TypeID_c* const m_pcType;    

  //! Minimum allowed value
  T m_xMin;
  //! Maximum allowed value
  T m_xMax;
    
  //! Callback function
  TweakableBase_c::Function_t*  m_pfCallback;

  //! Pointer to user data (or casted this pointer f.ex.)
  void* m_pCallbackData;

}; // TweakableCallbackTypeRange_c

//-----------------------------------------------------------------------------

}; // eqx 

#include "tweakable.inl"

#endif // EQX_TWEAKABLE_DEFINED