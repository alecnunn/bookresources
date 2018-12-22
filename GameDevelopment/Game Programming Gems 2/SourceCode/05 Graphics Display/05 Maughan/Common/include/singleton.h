#ifndef __SINGLETON_H
#define __SINGLETON_H

/* Copyright (C) Scott Bilas, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Bilas, 2000"
 */
#include <cassert>

template <typename T> class Singleton
{
    static T* ms_Singleton;

public:
    Singleton( void )
    {
        assert( !ms_Singleton );
        int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
        ms_Singleton = (T*)((int)this + offset);
    }
   virtual ~Singleton( void )
        {  assert( ms_Singleton );  ms_Singleton = 0;  }
    static T& GetSingleton( void )
        {  assert( ms_Singleton );  return ( *ms_Singleton );  }
    static T* GetSingletonPtr( void )
        {  return ( ms_Singleton );  }
};

template <typename T> T* Singleton <T>::ms_Singleton = 0;


// Sample usage:
//
/*

class TextureMgr : public Singleton <TextureMgr>
{
public:
    Texture* GetTexture( const char* name );
    // ...
};

void SomeFunction( void )
{
    Texture* stone1 = TextureMgr::GetSingleton().GetTexture( "stone1" );
    // ...
}
*/

#endif __SINGLETON_H