/* Copyright (C) Lasse Staff Jensen, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Lasse Staff Jensen, 2001"
 */
#ifndef TWEAKER_TEST_DEFINED
#define TWEAKER_TEST_DEFINED

#include "tweaker/tweaker_instance_db.h"
#include "tweaker/tweakable.h"

// Let's create a user defined datatype that represent a 3D vector.
// Usually this would be a class with operators etc., but for illustration
// let's stick to a simple struct. 
struct Vector3_t {
  float x, y, z;
 
};
// Now for adding support for our newly created type we need to call the macro:
DECLARE_DATA_TYPE( Vector3_t );


// Since we didn't create any operators for our class, we must place a call to
// the dummy operator macro as well:
DUMMY_OPERATORS( Vector3_t );

// The tweaker also supports callback functions, so let's create one:
bool MyCallback( eqx::Tweaker_c* const i_pcTweaker, const std::string& i_cID, void* i_pThis );

// To save memory try declearing a macro to exclude type information in release build like: 
#ifdef _debug
#define INCLUDE_TYPE_INFO true
#else
#define INCLUDE_TYPE_INFO false
#endif

// OK, we have the basics so let's continue by creating a class and add a few variables to be tweaked:
class a {
public:
  a();
  ~a();

  void ResizeTable();
  void Test();

private:
  int*      m_panTable;
  int       m_nNoOfElements;
  float     m_vFloat;
  bool      m_bIs;
  Vector3_t m_cVec;
  eqx::Tweaker_c* m_pacTweaker[3];
  
};


#endif
