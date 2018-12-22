/* Copyright (C) Lasse Staff Jensen, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Lasse Staff Jensen, 2001"
 */

#include "test.h"

DEFINE_DATA_TYPE( Vector3_t );

bool
MyCallback( eqx::Tweaker_c* const i_pcTweaker, const std::string& i_cID, void* i_pThis ) 
{
  int nCurrentValue; 
  
  // OK, in the callback we can check the current value by:
  i_pcTweaker->GetValue( i_cID, &nCurrentValue );
  // If we decide to reject this change we can return false. This can be usefull in
  // case one want to do any advanced constraints for the given tweakable. For ex.
  // in Equinox we use this for range checking of each element of a vector.   
  // Also note that we need to know what value the given ID correspond to, for the GetValue call.

  // We can change the value directly, but then we need to remember to pass in false or else
  // we end up with an infite recursion: 
  i_pcTweaker->TweakValue( i_cID, 15, false );
  
  // In case we need to attach any additional data to the given callback we use the void pointer. F.ex. we
  // can use it to store the this pointer of the given class. This way we can call init functions
  // when the value changes (f.ex. for values defining size of tables etc.)
  a* pcThis = static_cast<a*>( i_pThis );
  pcThis->ResizeTable();

  return true;  // accept changes
}


a::a() : m_panTable( NULL ) 
{
  // Here we create three unique tweakers for each instance of this class
  // and add it to three different ("global") categories:
  m_pacTweaker[0] = eqx::TweakerInstanceDB_c::AddTweaker(  
    "Instance name",
    TWEAKER_CREATE_ID( this ),
    "Graphics");
  m_pacTweaker[1] = eqx::TweakerInstanceDB_c::AddTweaker(  
    "Instance name",
    TWEAKER_CREATE_ID( this ),
    "Sound");
  m_pacTweaker[2] = eqx::TweakerInstanceDB_c::AddTweaker(  
    "Instance name",
    TWEAKER_CREATE_ID( this ),
    "General");
  
  // Now that we have the tweakers, we can add variables to be tweaked:
  m_bIs = false;
  m_vFloat = 25.12345F;
  m_cVec.x = m_cVec.y = m_cVec.z = 0.0F;
  m_pacTweaker[0]->AddTweakable( &m_bIs, "MyBool", false );
  m_pacTweaker[1]->AddTweakable( &m_vFloat, "MyFloat", 20.0F, 100.0F );
  m_pacTweaker[2]->AddTweakable( &m_cVec, "MyVec" );
  m_pacTweaker[2]->AddTweakable( &m_nNoOfElements, "Number of elements", 1, 30, INCLUDE_TYPE_INFO, MyCallback, this );
  
  m_nNoOfElements = 20;
  ResizeTable(); // Create our example table
}

a::~a()
{ 
  // Since the tweaker instance database controlls all the memory allocation, we tell it
  // to delete all the instances it has from this class:
  eqx::TweakerInstanceDB_c::DeleteInstances( TWEAKER_CREATE_ID( this ) );
  
  delete [] m_panTable;
}

void 
a::ResizeTable() 
{
  if( m_panTable != NULL ) { delete [] m_panTable; }
  m_panTable = new int[ m_nNoOfElements ];
}

void 
a::Test()
{ 
  eqx::TweakError_e eError;
  float vMin;
  float vMax;
  eError = m_pacTweaker[1]->GetLimits( "MyFloat", &vMin, &vMax );
  eError = m_pacTweaker[0]->TweakValue( "MyBool", true );
  eError = m_pacTweaker[1]->TweakValue( "MyFloat", 100.0F );
  eError = m_pacTweaker[2]->TweakValue( "Number of elements", 20 );
  Vector3_t cN;
  cN.x = cN.y = cN.z = 60.0F;
  Vector3_t cP;
  eError = m_pacTweaker[2]->GetValue( "MyVec", &cP );
  eError = m_pacTweaker[2]->TweakValue( "MyVec", cN );
  eError = m_pacTweaker[2]->GetValue( "MyVec", &cP );
  // Ok, trigger an error => MyInt don't exist!
  int nV;
  eError = m_pacTweaker[0]->GetValue( "MyInt", &nV );
}