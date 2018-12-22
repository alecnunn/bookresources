/* Copyright (C) Lasse Staff Jensen, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Lasse Staff Jensen, 2001"
 */
#ifndef EQX_DATATYPES_DEFINED
#define EQX_DATATYPES_DEFINED

/*!
  \file datatypes.h

  \brief  Containes type info and all our basic datatypes. Also defines macros for
          user defined types.

  \author Lasse.Staff.Jensen@funcom.com
*/


#include <assert.h>

namespace eqx {

/*! These classes define an interface for type information.
*/
class TypeID_c{
public:
  virtual const char* GetTypeName() const { return "Unknown"; }
};

template <class T>
class Identifier_c {
  public:
    static const TypeID_c* const GetType(); // (No implementation) 
};

//Note: These macros can't be used with multiple words, like f.ex.: unsigned int

//! Macro for declaring a datatype to support (to be placed in the header file)
#define DECLARE_DATA_TYPE( a ) \
class a##ID_c : public ::eqx::TypeID_c { \
public: \
  virtual const char* GetTypeName() const { return #a; } \
  static ::eqx::TypeID_c* const GetIdentification(); \
};\
template <> \
class ::eqx::Identifier_c<a> { \
public: \
  static const ::eqx::TypeID_c* const \
  GetType() { return a##ID_c::GetIdentification(); } \
}; 

//! Macro for defining the data type (to be placed in the cpp file)
#define DEFINE_DATA_TYPE( a ) \
::eqx::TypeID_c* const a##ID_c::GetIdentification() { \
    static a##ID_c cInstance; \
    return &cInstance; \
} \

//! Macro for defining dummy < and > operators in case you don't need range checking
#define DUMMY_OPERATORS( a ) \
  inline bool operator<( const a##& lhs, const a##& rhs ) { assert( 0 ); return false; } \
  inline bool operator>( const a##& lhs, const a##& rhs ) { assert( 0 ); return false; } 

//-----------------------------------------------------------------------------

typedef unsigned char Tweakable_uchar_t;
typedef signed char Tweakable_schar_t;
typedef unsigned long Tweakable_ulong_t;
typedef unsigned int Tweakable_uint_t;
typedef unsigned short Tweakable_ushort_t;
typedef long double Tweakable_ldouble_t;
typedef unsigned __int64 Tweakable_uint64_t;

DECLARE_DATA_TYPE( bool );
DECLARE_DATA_TYPE( void );
DECLARE_DATA_TYPE( char );
DECLARE_DATA_TYPE( short );
DECLARE_DATA_TYPE( int );
DECLARE_DATA_TYPE( long );
DECLARE_DATA_TYPE( float );
DECLARE_DATA_TYPE( double );
DECLARE_DATA_TYPE( __int64 );
DECLARE_DATA_TYPE( Tweakable_uchar_t );
DECLARE_DATA_TYPE( Tweakable_schar_t );
DECLARE_DATA_TYPE( Tweakable_ulong_t );
DECLARE_DATA_TYPE( Tweakable_uint_t );
DECLARE_DATA_TYPE( Tweakable_ushort_t );
DECLARE_DATA_TYPE( Tweakable_ldouble_t );
DECLARE_DATA_TYPE( Tweakable_uint64_t );

}; // eqx 

#endif // EQX_DATATYPES_DEFINED