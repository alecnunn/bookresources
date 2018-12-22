
/* Original version Copyright (C) Scott Bilas, 2000.
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Bilas, 2000"
 */


/** 
 * small add-on made by
 * Sebastien Schertenleib, 2003
 *
 */
#ifndef _VHDSINGLETON_H__
#define _VHDSINGLETON_H__

#include <assert.h>

// End SJS additions
/** Template class for creating single-instance global classes.
*/
template <typename T> class vhdSingleton
{
protected:

    static T* ms_Singleton;

public:
    vhdSingleton( void )
    {
        assert( !ms_Singleton );
		ms_Singleton = static_cast <T*> (this);
    }
    ~vhdSingleton( void )
        {  assert( ms_Singleton );  ms_Singleton = 0;  }
    static T& getSingleton( void )
        {  assert( ms_Singleton );  return ( *ms_Singleton );  }
    static T* getSingletonPtr( void )
        {  return ( ms_Singleton );  }
	static void setSingletonPtr(T* ptrT)
	{
		ms_Singleton=ptrT;
	}
};


#endif
