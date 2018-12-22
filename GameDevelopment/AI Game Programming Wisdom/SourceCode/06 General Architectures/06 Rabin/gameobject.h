/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */

#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "global.h"

class StateMachine;

class GameObject
{
public:

	GameObject( objectID id );
	~GameObject( void );

	objectID GetID( void )							{ return( m_ID ); }
	StateMachine* GetStateMachine( void )			{ return( m_StateMachine ); }
	void SetStateMachine( StateMachine * mch )		{ m_StateMachine = mch; }

private:

	objectID m_ID;

	StateMachine* m_StateMachine;

};


#endif	// __GAMEOBJECT_H__